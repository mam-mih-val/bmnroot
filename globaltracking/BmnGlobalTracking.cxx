
/** BmnGlobalTracking.cxx
 * \author Sergey Merts <Sergey.Merts@gmail.com>
 * @since 2014
 * @version 1.0
 **/

#include "BmnGlobalTracking.h"
#include "BmnEventHeader.h"
#include "BmnMwpcGeometry.h"
using namespace TMath;

//some variables for efficiency calculation
static Float_t workTime = 0.0;
//-----------------------------------------

const Float_t thresh = 0.7; // threshold for efficiency calculation (70%)

BmnGlobalTracking::BmnGlobalTracking() :
fDetConf(31), //31 means that all detectors are presented
fMcTracks(NULL),
fGemTracks(NULL),
fGemHits(NULL),
fSilHits(NULL),
fGemVertex(NULL),
fTof1Hits(NULL),
fTof2Hits(NULL),
fDchHits(NULL),
fGlobalTracks(NULL),
fGemMcPoints(NULL),
fTof1McPoints(NULL),
fTof2McPoints(NULL),
fDchMcPoints(NULL),
fEvHead(NULL),
fIsField(kTRUE),
fPDG(2212),
fChiSqCut(100.),
fVertex(NULL),
fEventNo(0) {
}

BmnGlobalTracking::~BmnGlobalTracking() {
}

InitStatus BmnGlobalTracking::Init() {
    if (fVerbose) cout << "BmnGlobalTracking::Init started\n";

    FairRootManager* ioman = FairRootManager::Instance();
    if (!ioman) {
        Fatal("Init", "FairRootManager is not instantiated");
    }

    fDet.DetermineSetup();
    if (fVerbose) cout << fDet.ToString();

    // MWPC
    if (fDet.GetDet(kMWPC)) {
        fMwpcHits = (TClonesArray*) ioman->GetObject("BmnMwpcHit");
        if (!fMwpcHits)
            if (fVerbose)
                cout << "Init. No BmnMwpcHit array!" << endl;
        fMwpcTracks = (TClonesArray*) ioman->GetObject("BmnMwpcTrack");
        if (!fMwpcTracks)
            if (fVerbose)
                cout << "Init. No BmnMwpcTrack array!" << endl;
    }

    // SILICON
    fSilHits = (TClonesArray*) ioman->GetObject("BmnSiliconHit");
    if (!fSilHits)
        if (fVerbose)
            cout << "Init. No BmnSiliconHit array!" << endl;

    // GEM
    if (fDet.GetDet(kGEM)) {
        fGemHits = (TClonesArray*) ioman->GetObject("BmnGemStripHit");
        if (!fGemHits)
            if (fVerbose)
                cout << "Init. No BmnGemStripHit array!" << endl;
        fGemTracks = (TClonesArray*) ioman->GetObject("BmnGemTrack");
        if (!fGemTracks) {
            cout << "BmnGlobalTracking::Init(): branch " << "BmnGemTrack" << " not found! Task will be deactivated" << endl;
            SetActive(kFALSE);
            return kERROR;
        }
    }

    // Vertex
    fGemVertex = (TClonesArray*) ioman->GetObject("BmnVertex");
    if (!fGemVertex)
        if (fVerbose)
            cout << "Init. No BmnVertex array!" << endl;

    // TOF1
    if (fDet.GetDet(kTOF1)) {
        fTof1Hits = (TClonesArray*) ioman->GetObject("BmnTof1Hit");
        if (!fTof1Hits)
            if (fVerbose)
                cout << "Init. No BmnTof1Hit array!" << endl;
    }

    // TOF2
    if (fDet.GetDet(kTOF)) {
        fTof2Hits = (TClonesArray*) ioman->GetObject("BmnTofHit");
        if (!fTof2Hits)
            if (fVerbose)
                cout << "Init. No BmnTof2Hit array!" << endl;
    }

    // DCH
    if (fDet.GetDet(kDCH)) {
        fDchTracks = (TClonesArray*) ioman->GetObject("BmnDchTrack");
        if (!fDchTracks)
            if (fVerbose)
                cout << "Init. No BmnDchTrack array!" << endl;
        fDchHits = (TClonesArray*) ioman->GetObject("BmnDchHit");
        if (!fDchHits)
            if (fVerbose)
                cout << "Init. No BmnDchHit array!" << endl;
    }

    fEvHead = (TClonesArray*) ioman->GetObject("EventHeader");
    if (!fEvHead)
        if (fVerbose) cout << "Init. No EventHeader array!" << endl;

    // Create and register track arrays
    fGlobalTracks = new TClonesArray("BmnGlobalTrack", 100);
    ioman->Register("BmnGlobalTrack", "GLOBAL", fGlobalTracks, kTRUE);

    if (fVerbose) cout << "BmnGlobalTracking::Init finished\n";
    return kSUCCESS;
}

void BmnGlobalTracking::Exec(Option_t* opt) {
    
    if (!IsActive())
        return;

    if (fVerbose) cout << "\n======================== Global tracking exec started =====================\n" << endl;

    //    if (!fEvHead) return;
    //    fEventNo = ((BmnEventHeader*) fEvHead->At(0))->GetEventId();
    //    if (fVerbose) printf("Event number: %d\n", fEventNo);

    clock_t tStart = clock();
    fGlobalTracks->Delete();

    //    CreateDchHitsFromTracks();

    if (!fGemTracks) return;

    if (fGemVertex) {
        if (fGemVertex->GetEntriesFast() > 0)
            fVertex = (CbmVertex*) fGemVertex->At(0);
        else
            fVertex = NULL;
    }

    for (Int_t i = 0; i < fGemTracks->GetEntriesFast(); ++i) {
        BmnGemTrack* gemTrack = (BmnGemTrack*) fGemTracks->At(i);
        new((*fGlobalTracks)[i]) BmnGlobalTrack();
        BmnGlobalTrack* glTr = (BmnGlobalTrack*) fGlobalTracks->At(i);
        glTr->SetParamFirst(*(gemTrack->GetParamFirst()));
        glTr->SetParamLast(*(gemTrack->GetParamLast()));
        glTr->SetGemTrackIndex(i);
        glTr->SetNHits(gemTrack->GetNHits());
        glTr->SetNDF(gemTrack->GetNDF());
        glTr->SetChi2(gemTrack->GetChi2());
        glTr->SetLength(gemTrack->GetLength());

        vector<BmnFitNode> nodes(4); //MWPC, TOF1, TOF2 and DCH
        glTr->SetFitNodes(nodes);

        //MatchingMWPC(glTr);
        MatchingSil(glTr);
        MatchingTOF(glTr, 1, i);
        //MatchingDCH(glTr);
        //Refit(glTr);
    }

    //CalculateLength();

    clock_t tFinish = clock();
    workTime += ((Float_t) (tFinish - tStart)) / CLOCKS_PER_SEC;

    if (fVerbose) cout << "GLOBAL_TRACKING: Number of merged tracks: " << fGlobalTracks->GetEntriesFast() << endl;
    if (fVerbose) cout << "\n======================== Global tracking exec finished ====================\n" << endl;
}

BmnStatus BmnGlobalTracking::MatchingMWPC(BmnGlobalTrack* tr) {

    if (!fMwpcTracks) return kBMNERROR;

    BmnKalmanFilter* kalman = new BmnKalmanFilter();

    Double_t minChiSq = DBL_MAX;
    BmnTrack* minTrack = NULL; // Pointer to the nearest track
    Int_t minIdx = -1;
    FairTrackParam minParUp; // updated track parameters for the closest dch track
    FairTrackParam minParPred; // predicted track parameters for the closest dch track

    for (Int_t trIdx = 0; trIdx < fMwpcTracks->GetEntriesFast(); ++trIdx) {
        BmnTrack* mwpcTr = (BmnTrack*) fMwpcTracks->At(trIdx);
        FairTrackParam parPredict(*(tr->GetParamFirst()));
        kalman->TGeoTrackPropagate(&parPredict, mwpcTr->GetParamLast()->GetZ(), fPDG, NULL, NULL, fIsField);
        FairTrackParam parUpdate = parPredict;
        Double_t chi = 0.0;
        BmnMwpcGeometry geo;
        TVector3 err = TVector3(geo.GetWireStep() / Sqrt(12.0), geo.GetWireStep() / Sqrt(12.0), 1.0 / Sqrt(12.0));
        BmnMwpcHit hit(1, TVector3(mwpcTr->GetParamLast()->GetX(), mwpcTr->GetParamLast()->GetY(), mwpcTr->GetParamLast()->GetZ()), err, 0); //tmp hit for updating track parameters
        kalman->Update(&parUpdate, &hit, chi);
        if (chi < minChiSq) {
            minChiSq = chi;
            minTrack = mwpcTr;
            minParPred = parPredict;
            minParUp = parUpdate;
            minIdx = trIdx;
        }
    }

    if (minTrack != NULL) { // Check if hit was added
        tr->SetParamFirst(minParUp);
        tr->SetChi2(tr->GetChi2() + minChiSq);
        tr->SetMwpcTrackIndex(minIdx);
        tr->SetNHits(tr->GetNHits() + minTrack->GetNHits());
        tr->SetNDF(tr->GetNDF() + minTrack->GetNHits()); //FIXME!
        BmnFitNode *node = &((tr->GetFitNodes()).at(0));
        node->SetUpdatedParam(&minParUp);
        node->SetPredictedParam(&minParPred);
        return kBMNSUCCESS;
    } else {
        return kBMNERROR;
    }

    delete kalman;
}

BmnStatus BmnGlobalTracking::MatchingTOF(BmnGlobalTrack* tr, Int_t num, Int_t trIndex) {

    TClonesArray* tofHits = (num == 1 && fTof1Hits) ? fTof1Hits : (num == 2 && fTof2Hits) ? fTof2Hits : NULL;
    if (!tofHits) return kBMNERROR;

    BmnKalmanFilter* kalman = new BmnKalmanFilter();

    Double_t minChiSq = DBL_MAX;
    Double_t minDist = DBL_MAX;
    BmnHit* minHit = NULL; // Pointer to the nearest hit
    Int_t minIdx = -1;
    Double_t LenPropLast = 0., LenPropFirst = 0.;
    FairTrackParam minParPredLast; // predicted track parameters for closest hit
    for (Int_t hitIdx = 0; hitIdx < tofHits->GetEntriesFast(); ++hitIdx) {
        BmnHit* hit = (BmnHit*) tofHits->At(hitIdx);
        if (hit->IsUsed()) continue; // skip Tof hit which used before
        FairTrackParam parPredict(*(tr->GetParamLast()));
        Double_t len = 0.;
        //printf("hitIdx = %d\n", hitIdx);
        //printf("BEFORE: len = %f.3\t", len);
        //printf("Param->GetX() = %.2f\n", parPredict.GetX());
        BmnStatus resultPropagate = kalman->TGeoTrackPropagate(&parPredict, hit->GetZ(), fPDG, NULL, &len, fIsField);
        if (resultPropagate == kBMNERROR) continue; // skip in case kalman error
        Double_t dist = TMath::Sqrt(TMath::Power(parPredict.GetX() - hit->GetX(), 2) + TMath::Power(parPredict.GetY() - hit->GetY(), 2));
        //printf("AFTER:  len = %.3f\t", len);
        //printf("Param->GetX() = %.2f\t", parPredict.GetX());
        //printf ("Distanc = %.3f\n", dist);
        //getchar();
        if (dist < minDist && dist <= 5.) {
            minDist = dist;
            minHit = hit;
            minParPredLast = parPredict;
            minIdx = hitIdx;
            LenPropLast = len;
        }
    }

    if (minHit != NULL) { // Check if hit was added
        FairTrackParam ParPredFirst(*(tr->GetParamFirst()));
        FairTrackParam ParPredLast(*(tr->GetParamLast()));
        ParPredFirst.SetQp(ParPredLast.GetQp());
        Double_t LenTrack = tr->GetLength();
        Double_t zTarget = -21.7; // z of target by default
        if (fVertex)
            zTarget = fVertex->GetZ();
        BmnStatus resultPropagate = kalman->TGeoTrackPropagate(&ParPredFirst, zTarget, fPDG, NULL, &LenPropFirst, fIsField);
        if (resultPropagate != kBMNERROR) { // skip in case kalman error

            if (num == 1)
                tr->SetTof1HitIndex(minIdx);
            else
                tr->SetTof2HitIndex(minIdx);

            minHit->SetIndex(trIndex);
            //    printf("LenFirst = %.3f;  LenTrack = %.3f;  LenLast = %.3f\n", LenPropFirst, LenTrack, LenPropLast);
            minHit->SetLength(LenPropFirst + LenTrack + LenPropLast); // length from target to Tof hit
            //    printf("Writed length = %.3f\n", minHit->GetLength());
            minHit->SetUsing(kTRUE);
            tr->SetNHits(tr->GetNHits() + 1);

            delete kalman;
            return kBMNSUCCESS;
        } else {
            delete kalman;
            return kBMNERROR;
        }
    } else return kBMNERROR;
}

BmnStatus BmnGlobalTracking::MatchingSil(BmnGlobalTrack* tr) {

    if (!fSilHits) return kBMNERROR;

    Double_t distCut = 1.0;

    BmnKalmanFilter* kalman = new BmnKalmanFilter();

    Double_t minDist = DBL_MAX;
    BmnHit* minHit = NULL; // Pointer to the nearest hit
    Int_t minIdx = -1;
    FairTrackParam minParPred; // predicted track parameters for closest hit
    for (Int_t hitIdx = 0; hitIdx < fSilHits->GetEntriesFast(); ++hitIdx) {
        BmnHit* hit = (BmnHit*) fSilHits->At(hitIdx);
        if (hit->IsUsed()) continue;
        FairTrackParam parPredict(*(tr->GetParamFirst()));
        if (kalman->TGeoTrackPropagate(&parPredict, hit->GetZ(), fPDG, NULL, NULL, fIsField) == kBMNERROR) continue;
        Double_t dist = Sqrt(Power(parPredict.GetX() - hit->GetX(), 2) + Power(parPredict.GetY() - hit->GetY(), 2));
        if (dist < minDist && dist <= distCut) {
            minDist = dist;
            minHit = hit;
            minParPred = parPredict;
            minIdx = hitIdx;
        }
    }

    if (minHit != NULL) {
        FairTrackParam ParPredFirst(*(tr->GetParamFirst()));
        FairTrackParam ParPredLast(*(tr->GetParamLast()));
        ParPredFirst.SetQp(ParPredLast.GetQp());

        FairTrackParam parUpdate = minParPred;
        Double_t chi = 0.0;
        if (kalman->Update(&parUpdate, minHit, chi) == kBMNERROR) {
            delete kalman;
            return kBMNERROR;
        } else {
            tr->SetParamFirst(parUpdate);
            tr->SetChi2(Abs(tr->GetChi2()) + Abs(chi));
            tr->SetSilHitIndex(minIdx);
            tr->SetNHits(tr->GetNHits() + 1);
            minHit->SetUsing(kTRUE);

            delete kalman;
            return kBMNSUCCESS;
        }
    } else
        return kBMNERROR;
}

BmnStatus BmnGlobalTracking::CreateDchHitsFromTracks() {
    for (Int_t i = 0; i < fDchTracks->GetEntriesFast(); ++i) {
        BmnDchTrack* dchTr = (BmnDchTrack*) fDchTracks->At(i);
        Float_t x = dchTr->GetParamFirst()->GetX();
        Float_t y = dchTr->GetParamFirst()->GetY();
        Float_t z = dchTr->GetParamFirst()->GetZ();
        new((*fDchHits)[fDchHits->GetEntriesFast()]) BmnDchHit(1, TVector3(x, y, z), TVector3(0, 0, 0), 0);
    }
}

BmnStatus BmnGlobalTracking::MatchingDCH(BmnGlobalTrack* tr) {

    if (!fDchTracks) return kBMNERROR;

    BmnKalmanFilter* kalman = new BmnKalmanFilter();

    Double_t minChiSq = DBL_MAX;
    BmnTrack* minTrack = NULL; // Pointer to the nearest track
    Int_t minIdx = -1;
    FairTrackParam minParUp; // updated track parameters for the closest dch track
    FairTrackParam minParPred; // predicted track parameters for the closest dch track
    for (Int_t trIdx = 0; trIdx < fDchTracks->GetEntriesFast(); ++trIdx) {
        BmnTrack* dchTr = (BmnTrack*) fDchTracks->At(trIdx);
        FairTrackParam parPredict(*(tr->GetParamLast()));
        kalman->TGeoTrackPropagate(&parPredict, dchTr->GetParamFirst()->GetZ(), fPDG, NULL, NULL, fIsField);
        FairTrackParam parUpdate = parPredict;
        Double_t chi;
        BmnDchHit hit(1, TVector3(dchTr->GetParamFirst()->GetX(), dchTr->GetParamFirst()->GetY(), dchTr->GetParamFirst()->GetZ()), TVector3(0.5 / Sqrt(12.0), 0.5 / Sqrt(12.0), 1.0 / Sqrt(12.0)), 0); //tmp hit for updating track parameters
        kalman->Update(&parUpdate, &hit, chi);
        if (chi < minChiSq) {
            minChiSq = chi;
            minTrack = dchTr;
            minParPred = parPredict;
            minParUp = parUpdate;
            minIdx = trIdx;
        }
    }

    if (minTrack != NULL) { // Check if hit was added
        tr->SetParamLast(minParUp);
        tr->SetChi2(tr->GetChi2() + minChiSq);
        tr->SetDchTrackIndex(minIdx);
        tr->SetNHits(tr->GetNHits() + minTrack->GetNHits());
        tr->SetNDF(tr->GetNDF() + minTrack->GetNHits()); //FIXME!
        vector<BmnFitNode> nodes = tr->GetFitNodes();
        BmnFitNode *node = &((tr->GetFitNodes()).at(3));
        node->SetUpdatedParam(&minParUp);
        node->SetPredictedParam(&minParPred);
        return kBMNSUCCESS;
    } else {
        return kBMNERROR;
    }
    delete kalman;
}

BmnStatus BmnGlobalTracking::RefitToDetector(BmnGlobalTrack* tr, Int_t hitId, TClonesArray* hitArr, FairTrackParam* par, Int_t* nodeIdx, vector<BmnFitNode>* nodes) {

    BmnKalmanFilter* kalman = new BmnKalmanFilter();

    if (tr->GetTof2HitIndex() != -1) {
        BmnHit* hit = (BmnHit*) hitArr->At(hitId);
        Float_t Ze = hit->GetZ();
        Double_t length = 0;
        vector<Double_t> F(25);
        if (kalman->TGeoTrackPropagate(par, Ze, 2212, &F, &length, fIsField) == kBMNERROR) {
            tr->SetFlag(kBMNBAD);
            return kBMNERROR;
        }

        nodes->at(*nodeIdx).SetPredictedParam(par);
        nodes->at(*nodeIdx).SetF(F);
        Double_t chi2Hit = 0.;
        if (kalman->Update(par, hit, chi2Hit) == kBMNERROR) {
            tr->SetFlag(kBMNBAD);
            return kBMNERROR;
        }
        //        tr->SetParamLast(par);
        //        tr->SetParamFirst(par);

        nodes->at(*nodeIdx).SetUpdatedParam(par);
        nodes->at(*nodeIdx).SetChiSqFiltered(chi2Hit);
        tr->SetChi2(tr->GetChi2() + chi2Hit);
        tr->SetLength(tr->GetLength() + length);
        (*nodeIdx)--;
    }

    delete kalman;
    return kBMNSUCCESS;
}

BmnStatus BmnGlobalTracking::Refit(BmnGlobalTrack* tr) {

    vector<BmnFitNode> nodes(tr->GetNHits());
    Int_t nodeIdx = tr->GetNHits() - 1;
    FairTrackParam par = *(tr->GetParamLast());
    //    FairTrackParam par = *(tr->GetParamFirst());

    //TOF2 part
    //    if (fDet.GetDet(kTOF) && tr->GetTof2HitIndex() != -1 && fTof2Hits) {
    //        if (RefitToDetector(tr, tr->GetTof2HitIndex(), fTof2Hits, &par, &nodeIdx, &nodes) == kBMNERROR) return kBMNERROR;
    //    }

    //DCH1 part
    //        if (fDet.GetDet(kDCH) && tr->GetDchHitIndex() != -1 && fDchHits) {
    //            if (RefitToDetector(tr, tr->GetDchHitIndex(), fDchHits, &par, &nodeIdx, &nodes) == kBMNERROR) return kBMNERROR;
    //        }

    //TOF1 part
    if (fDet.GetDet(kTOF1) && tr->GetTof1HitIndex() != -1 && fTof1Hits) {
        if (RefitToDetector(tr, tr->GetTof1HitIndex(), fTof1Hits, &par, &nodeIdx, &nodes) == kBMNERROR) return kBMNERROR;
    }

    //GEM part
    if (fDet.GetDet(kGEM) && tr->GetGemTrackIndex() != -1) {
        BmnGemTrack* gemTr = (BmnGemTrack*) fGemTracks->At(tr->GetGemTrackIndex());
        for (Int_t i = gemTr->GetNHits() - 1; i >= 0; --i) {
            if (RefitToDetector(tr, i, fGemHits, &par, &nodeIdx, &nodes) == kBMNERROR) return kBMNERROR;
        }
    }

    tr->SetParamFirst(par);
    tr->SetFitNodes(nodes);
    return kBMNSUCCESS;
}

void BmnGlobalTracking::Finish() {
    cout << "Work time of the Global matching: " << workTime << endl;
}

void BmnGlobalTracking::CalculateLength() {
    if (fGlobalTracks == NULL) return;

    /* Calculate the length of the global track
     * starting with (0, 0, 0) and adding all
     * distances between hits
     */
    for (Int_t iTr = 0; iTr < fGlobalTracks->GetEntriesFast(); iTr++) {
        BmnGlobalTrack* glTr = (BmnGlobalTrack*) fGlobalTracks->At(iTr);
        vector<Float_t> X, Y, Z;
        X.push_back(0.);
        Y.push_back(0.);
        Z.push_back(0.);

        if (glTr->GetGemTrackIndex() > -1) {
            if (!isRUN1) {
                const BmnGemTrack* gemTr = (BmnGemTrack*) fGemTracks->At(glTr->GetGemTrackIndex());
                for (Int_t iGem = 0; iGem < gemTr->GetNHits(); iGem++) {
                    const BmnHit* hit = (BmnHit*) fGemHits->At(gemTr->GetHitIndex(iGem));
                    if (!hit) continue;
                    X.push_back(hit->GetX());
                    Y.push_back(hit->GetY());
                    Z.push_back(hit->GetZ());
                }
            }
        }
        if (fDet.GetDet(kTOF1)) {
            if (glTr->GetTof1HitIndex() > -1 && fTof1Hits) {
                const BmnHit* hit = (BmnHit*) fTof1Hits->At(glTr->GetTof1HitIndex());
                if (!hit) continue;
                X.push_back(hit->GetX());
                Y.push_back(hit->GetY());
                Z.push_back(hit->GetZ());
            }
        }
        if (fDet.GetDet(kDCH)) {
            //            if (glTr->GetDch1HitIndex() > -1 && fDch1Hits) {
            //                const BmnHit* hit = (BmnHit*) fDch1Hits->At(glTr->GetDch1HitIndex());
            //                if (!hit) continue;
            //                X.push_back(hit->GetX());
            //                Y.push_back(hit->GetY());
            //                Z.push_back(hit->GetZ());
            //            }
        }
        if (fDet.GetDet(kTOF)) {
            if (glTr->GetTof2HitIndex() > -1 && fTof2Hits) {
                const BmnHit* hit = (BmnHit*) fTof2Hits->At(glTr->GetTof2HitIndex());
                if (!hit) continue;
                X.push_back(hit->GetX());
                Y.push_back(hit->GetY());
                Z.push_back(hit->GetZ());
            }
        }
        // Calculate distances between hits
        Float_t length = 0.;
        for (Int_t i = 0; i < X.size() - 1; i++) {
            Float_t dX = X[i] - X[i + 1];
            Float_t dY = Y[i] - Y[i + 1];
            Float_t dZ = Z[i] - Z[i + 1];
            length += Sqrt(dX * dX + dY * dY + dZ * dZ);
        }
        glTr->SetLength(length);
    }
}

Float_t BmnGlobalTracking::Sqr(Float_t x) {
    return x * x;
}

ClassImp(BmnGlobalTracking);
