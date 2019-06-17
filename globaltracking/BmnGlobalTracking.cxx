
/** BmnGlobalTracking.cxx
 * \author Sergey Merts <Sergey.Merts@gmail.com>
 * @since 2014
 * @version 1.0
 **/

#include <Fit/FitResult.h>
#include <iterator>
#include <map>
#include <vector>

#include "BmnEventHeader.h"
#include "BmnGlobalTracking.h"
#include "BmnMwpcGeometry.h"
#include "BmnSiliconTrack.h"
#include "FitWLSQ.h"
#include "TH1F.h"
using namespace TMath;

static Float_t workTime = 0.0;
//-----------------------------------------

BmnGlobalTracking::BmnGlobalTracking() : fDoAlign(kFALSE),
                                         fInnerTracks(NULL),
                                         fSiliconTracks(NULL),
                                         fGemHits(NULL),
                                         fGemTracks(NULL),
                                         fSilHits(NULL),
                                         fCscHits(NULL),
                                         fGemVertex(NULL),
                                         fTof1Hits(NULL),
                                         fTof2Hits(NULL),
                                         fBC1Digits(NULL),
                                         fBC2Digits(NULL),
                                         fBC3Digits(NULL),
                                         fBC4Digits(NULL),
                                         fDchHits(NULL),
                                         fMCTracks(NULL),
                                         fEvHead(NULL),
                                         fIsField(kTRUE),
                                         fPDG(2212),
                                         fChiSqCut(100.),
                                         fVertex(NULL),
                                         fIsSRC(kFALSE),
                                         fKalman(NULL),
                                         fEventNo(0) {
}

BmnGlobalTracking::BmnGlobalTracking(Bool_t isField, Bool_t doAlign) : fInnerTracks(NULL),
                                                                       fSiliconTracks(NULL),
                                                                       fGemHits(NULL),
                                                                       fCscHits(NULL),
                                                                       fGemTracks(NULL),
                                                                       fGemVertex(NULL),
                                                                       fTof1Hits(NULL),
                                                                       fTof2Hits(NULL),
                                                                       fBC1Digits(NULL),
                                                                       fBC2Digits(NULL),
                                                                       fBC3Digits(NULL),
                                                                       fBC4Digits(NULL),
                                                                       fDchHits(NULL),
                                                                       fMCTracks(NULL),
                                                                       fEvHead(NULL),
                                                                       fPDG(2212),
                                                                       fChiSqCut(100.),
                                                                       fVertex(NULL),
                                                                       fPeriod(7),
                                                                       fIsSRC(kTRUE),
                                                                       fDoAlign(doAlign),
                                                                       fIsField(isField),
                                                                       fEventNo(0) {
    TString gPathConfig = gSystem->Getenv("VMCWORKDIR");
    TString gPathSiConfig = gPathConfig + "/parameters/silicon/XMLConfigs/";
    TString confSi = (fPeriod == 7) ? "SiliconRun" + TString(fIsSRC ? "SRC" : "") + "Spring2018.xml" : "SiliconRunSpring2017.xml";
    fDetectorSI = new BmnSiliconStationSet(gPathSiConfig + confSi);

    fKalman = new BmnKalmanFilter();

    if (fDoAlign) {
        fhXCscGemResid = new TH1F("fhXCscGemResid", "fhXCscGemResid", 300, -100.0, 100.0);
        fhYCscGemResid = new TH1F("fhYCscGemResid", "fhYCscGemResid", 300, -100.0, 100.0);

        fhXSiGemResid = new TH1F("fhXSiGemResid", "fhXSiGemResid", 500, -10.0, 10.0);
        fhYSiGemResid = new TH1F("fhYSiGemResid", "fhYSiGemResid", 500, -10.0, 10.0);
        fhTxSiGemResid = new TH1F("fhTxSiGemResid", "fhTxSiGemResid", 500, -0.1, 0.1);
        fhTySiGemResid = new TH1F("fhTySiGemResid", "fhTySiGemResid", 500, -0.1, 0.1);
        fhXdXSiGemResid = new TH2F("fhXdXSiGemResid", "fhXdXSiGemResid", 200, -10, 10, 200, -10.0, 10.0);
        fhYdYSiGemResid = new TH2F("fhYdYSiGemResid", "fhYdYSiGemResid", 200, -10, 10, 200, -10.0, 10.0);
        fhTxdXSiGemResid = new TH2F("fhTxdXSiGemResid", "fhTxdXSiGemResid", 200, -0.1, 0.1, 200, -10.0, 10.0);
        fhTydYSiGemResid = new TH2F("fhTydYSiGemResid", "fhTydYSiGemResid", 200, -0.1, 0.1, 200, -10.0, 10.0);

        fhXDchGemResid = new TH1F("fhXDchGemResid", "fhXDchGemResid", 500, -50.0, 50.0);
        fhYDchGemResid = new TH1F("fhYDchGemResid", "fhYDchGemResid", 500, -50.0, 50.0);
        fhTxDchGemResid = new TH1F("fhTxDchGemResid", "fhTxDchGemResid", 500, -0.1, 0.1);
        fhTyDchGemResid = new TH1F("fhTyDchGemResid", "fhTyDchGemResid", 500, -0.1, 0.1);
        fhXdXDchGemResid = new TH2F("fhXdXDchGemResid", "fhXdXDchGemResid", 200, -10, 60, 200, -20.0, 20.0);
        fhYdYDchGemResid = new TH2F("fhYdYDchGemResid", "fhYdYDchGemResid", 200, -10, 10, 200, -20.0, 20.0);
        fhTxdXDchGemResid = new TH2F("fhTxdXDchGemResid", "fhTxdXDchGemResid", 200, -0.1, 0.1, 200, -10.0, 10.0);
        fhTydYDchGemResid = new TH2F("fhTydYDchGemResid", "fhTydYDchGemResid", 200, -0.1, 0.1, 200, -10.0, 10.0);

        const Int_t nSt = 6;

        fhdXGemSt = new TH1F *[nSt];
        fhdYGemSt = new TH1F *[nSt];
        fhdTxGemSt = new TH1F *[nSt];
        fhdTyGemSt = new TH1F *[nSt];
        fhdXTxGemSt = new TH2F *[nSt];
        fhdYTyGemSt = new TH2F *[nSt];

        for (Int_t i = 0; i < nSt; ++i) {
            TString str1 = Form("fhdXGemSt_%d", i);
            TString str2 = Form("fhdYGemSt_%d", i);
            TString str3 = Form("fhdTxGemSt_%d", i);
            TString str4 = Form("fhdTyGemSt_%d", i);
            TString str5 = Form("fhdXTxGemSt_%d", i);
            TString str6 = Form("fhdYTyGemSt_%d", i);
            fhdXGemSt[i] = new TH1F(str1, str1, 200, -2.0, 2.0);
            fhdYGemSt[i] = new TH1F(str2, str2, 200, -2.0, 2.0);
            fhdXTxGemSt[i] = new TH2F(str5, str5, 200, -0.05, 0.05, 200, -2.0, 2.0);
            fhdYTyGemSt[i] = new TH2F(str6, str6, 200, -0.05, 0.05, 200, -2.0, 2.0);
        }
    }
}

BmnGlobalTracking::~BmnGlobalTracking() {
}

InitStatus BmnGlobalTracking::Init() {
    if (fVerbose)
        cout << "BmnGlobalTracking::Init started\n";

    FairRootManager *ioman = FairRootManager::Instance();
    if (!ioman)
        Fatal("Init", "FairRootManager is not instantiated");

    fDet.DetermineSetup();
    if (fVerbose)
        cout << fDet.ToString();

    fBC1Digits = (TClonesArray *)ioman->GetObject("TQDC_BC1");
    fBC2Digits = (TClonesArray *)ioman->GetObject("TQDC_BC2");
    fBC3Digits = (TClonesArray *)ioman->GetObject("TQDC_BC3");
    fBC4Digits = (TClonesArray *)ioman->GetObject("TQDC_BC4");

    // SILICON

    fSilHits = (TClonesArray *)ioman->GetObject("BmnSiliconHit");
    fGemHits = (TClonesArray *)ioman->GetObject("BmnGemStripHit");
    fCscHits = (TClonesArray *)ioman->GetObject("BmnCSCHit");

    fInnerTracks = (TClonesArray *)ioman->GetObject("BmnGlobalTrack");
    fGemTracks = (TClonesArray *)ioman->GetObject("BmnGemTrack");

    if (!fInnerTracks) {
        cout << "BmnGlobalTracking::Init(): branch BmnInnerTrack not found! Task will be deactivated" << endl;
        SetActive(kFALSE);
        return kERROR;
    }

    fSiliconTracks = (TClonesArray *)ioman->GetObject("BmnSiliconTrack");
    if (!fSiliconTracks)
        cout << "BmnGlobalTracking::Init(): branch BmnSiliconTrack not found!" << endl;

    fDchTracks = (TClonesArray *)ioman->GetObject("BmnDchTrack");
    if (!fDchTracks)
        cout << "BmnGlobalTracking::Init(): branch BmnDchTrack not found!" << endl;

    fEvHead = (TClonesArray *)ioman->GetObject("EventHeader");
    if (!fEvHead)
        if (fVerbose)
            cout << "Init. No EventHeader array!" << endl;

    ioman->Register("TQDC_BC1", "TQDC", fBC1Digits, kTRUE);
    ioman->Register("TQDC_BC2", "TQDC", fBC2Digits, kTRUE);
    ioman->Register("TQDC_BC3", "TQDC", fBC3Digits, kTRUE);
    ioman->Register("TQDC_BC4", "TQDC", fBC4Digits, kTRUE);

    if (fVerbose)
        cout << "BmnGlobalTracking::Init finished\n";
    return kSUCCESS;
}

void BmnGlobalTracking::Exec(Option_t *opt) {
    if (!IsActive())
        return;

    if (fVerbose)
        cout << "\n======================== Global tracking exec started =====================\n"
             << endl;
    fEventNo++;
    clock_t tStart = clock();

    if (!fInnerTracks)
        return;

    for (Int_t i = 0; i < fInnerTracks->GetEntriesFast(); ++i) {
        BmnGlobalTrack *glTrack = (BmnGlobalTrack *)fInnerTracks->At(i);
        BmnGemTrack *gemTrack = (BmnGemTrack *)fGemTracks->At(glTrack->GetGemTrackIndex());
        if (fIsSRC && fSiliconTracks)
            MatchingSil(glTrack);
        if (fDchTracks)
            MatchingDCH(glTrack);
        if (fCscHits)
            MatchingCSC(glTrack);
        if (fDoAlign) {
            FairTrackParam parGem(*(glTrack->GetParamLast()));
            if (fKalman->TGeoTrackPropagate(&parGem, 412.352, fPDG, NULL, NULL, fIsField) == kBMNERROR)  //FIXME: take zPos from geometry
                continue;
            for (Int_t hitIdx = 0; hitIdx < fCscHits->GetEntriesFast(); ++hitIdx) {
                BmnHit *hit = (BmnHit *)fCscHits->At(hitIdx);
                fhXCscGemResid->Fill(parGem.GetX() - hit->GetX());
                fhYCscGemResid->Fill(parGem.GetY() - hit->GetY());
            }

            for (Int_t hitIdx = 0; hitIdx < gemTrack->GetNHits(); ++hitIdx) {
                BmnGemStripHit *hit = (BmnGemStripHit *)fGemHits->At(gemTrack->GetHitIndex(hitIdx));
                fhdXGemSt[hit->GetStation() - 4]->Fill(hit->GetResX());
                fhdYGemSt[hit->GetStation() - 4]->Fill(hit->GetResY());
                fhdXTxGemSt[hit->GetStation() - 4]->Fill(glTrack->GetParamFirst()->GetTx(), hit->GetResX());
                fhdYTyGemSt[hit->GetStation() - 4]->Fill(glTrack->GetParamFirst()->GetTy(), hit->GetResY());
            }
        }

        //First version of silicon hit match to GEM tracks if (fSilHits)
        // {
        //     // Map to be used for matching GEM-tracks with corresponding silicon hits
        //     map<Double_t, pair<Int_t, Int_t>> silDists;
        //     for (Int_t iStat = fDetectorSI->GetNStations() - 1; iStat >= 0; iStat--)
        //     {
        //         CalcSiliconDist(iStat, glTr, silDists);
        //         MatchingSil(glTr, silDists);
        //         silDists.clear();
        //     }
        // }

        //vector<BmnFitNode> nodes(4); //MWPC, TOF1, TOF2 and DCH
        //glTr->SetFitNodes(nodes);

        //MatchingMWPC(glTr);

        //MatchingTOF(glTr, 1, i);
        //MatchingDCH(glTr);
        //Refit(glTr);
        //MatchGemDCH(glTr);
        //MatchDCHTOF(glTr,1);
        //MatchDCHTOF(glTr,2);
    }

    //CalculateLength();

    clock_t tFinish = clock();
    workTime += ((Float_t)(tFinish - tStart)) / CLOCKS_PER_SEC;

    //if (fVerbose) cout << "GLOBAL_TRACKING: Number of merged tracks: " << fGlobalTracks->GetEntriesFast() << endl;
    if (fVerbose)
        cout << "\n======================== Global tracking exec finished ====================\n"
             << endl;
}

BmnStatus BmnGlobalTracking::MatchingCSC(BmnGlobalTrack *tr) {
    Double_t minChi = DBL_MAX;
    Double_t chiCut = 500.0;
    Int_t matchedIdx = -1;

    for (Int_t hitIdx = 0; hitIdx < fCscHits->GetEntriesFast(); ++hitIdx) {
        BmnHit *hit = (BmnHit *)fCscHits->At(hitIdx);
        FairTrackParam par(*(tr->GetParamLast()));
        if (fKalman->TGeoTrackPropagate(&par, hit->GetZ(), fPDG, NULL, NULL, fIsField) == kBMNERROR)
            continue;
        Double_t chi = 0;
        fKalman->Update(&par, hit, chi);
        if (chi < chiCut && chi < minChi) {
            minChi = chi;
            matchedIdx = hitIdx;
        }
    }

    if (matchedIdx != -1) {
        BmnHit *cscHit = (BmnHit *)fCscHits->At(matchedIdx);

        FairTrackParam par(*(tr->GetParamLast()));
        if (fKalman->TGeoTrackPropagate(&par, cscHit->GetZ(), fPDG, NULL, NULL, fIsField) != kBMNERROR) {
            Double_t chi = 0;
            fKalman->Update(&par, cscHit, chi);

            tr->SetCscHitIndex(matchedIdx);
            tr->SetNHits(tr->GetNHits() + 1);
            tr->SetParamLast(par);
            return kBMNSUCCESS;
        } else
            return kBMNERROR;
    } else
        return kBMNERROR;
}

void BmnGlobalTracking::CalcSiliconDist(Int_t stat, BmnGlobalTrack *glTr, map<Double_t, pair<Int_t, Int_t>> &silDists) {
    const Double_t distCut = 1.;
    for (Int_t hitIdx = 0; hitIdx < fSilHits->GetEntriesFast(); hitIdx++) {
        BmnSiliconHit *hit = (BmnSiliconHit *)fSilHits->UncheckedAt(hitIdx);
        if (stat != hit->GetStation())
            continue;

        FairTrackParam parPredict = *glTr->GetParamFirst();

        if (fKalman->TGeoTrackPropagate(&parPredict, hit->GetZ(), fPDG, NULL, NULL, fIsField) == kBMNERROR)
            continue;

        Double_t dist = Sqrt(Power(parPredict.GetX() - hit->GetX(), 2) + Power(parPredict.GetY() - hit->GetY(), 2));
        if (dist > distCut)
            continue;
        silDists.insert(pair<Double_t, pair<Int_t, Int_t>>(dist, make_pair(glTr->GetGemTrackIndex(), hitIdx)));

        glTr->SetParamFirst(parPredict);
    }
}

Int_t BmnGlobalTracking::FindNearestHit(BmnGlobalTrack *tr, TClonesArray *hits, Float_t distCut) {
    if (!hits || !tr)
        return kBMNERROR;

    Double_t minDist = DBL_MAX;
    Int_t minIdx = -1;

    for (Int_t hitIdx = 0; hitIdx < hits->GetEntriesFast(); ++hitIdx) {
        BmnHit *hit = (BmnHit *)hits->At(hitIdx);
        FairTrackParam parPredict(*(tr->GetParamLast()));
        if (fKalman->TGeoTrackPropagate(&parPredict, hit->GetZ(), fPDG, NULL, NULL, fIsField) == kBMNERROR)
            continue;
        Double_t dist = Sqrt(Sq(parPredict.GetX() - hit->GetX()) + Sq(parPredict.GetY() - hit->GetY()));
        if (dist < minDist && dist < distCut) {
            minDist = dist;
            minIdx = hitIdx;
        }
    }

    return minIdx;
}

BmnStatus BmnGlobalTracking::MatchingMWPC(BmnGlobalTrack *tr) {
}

BmnStatus BmnGlobalTracking::MatchingTOF(BmnGlobalTrack *tr, Int_t num, Int_t trIndex) {
    TClonesArray *tofHits = (num == 1 && fTof1Hits) ? fTof1Hits : (num == 2 && fTof2Hits) ? fTof2Hits : NULL;
    if (!tofHits)
        return kBMNERROR;

    Double_t minChiSq = DBL_MAX;
    Double_t minDist = DBL_MAX;
    BmnHit *minHit = NULL;  // Pointer to the nearest hit
    Int_t minIdx = -1;
    Double_t LenPropLast = 0., LenPropFirst = 0.;
    FairTrackParam minParPredLast;  // predicted track parameters for closest hit
    for (Int_t hitIdx = 0; hitIdx < tofHits->GetEntriesFast(); ++hitIdx) {
        BmnHit *hit = (BmnHit *)tofHits->At(hitIdx);
        if (hit->IsUsed())
            continue;  // skip Tof hit which used before
        FairTrackParam parPredict(*(tr->GetParamLast()));
        Double_t len = 0.;
        //printf("hitIdx = %d\n", hitIdx);
        //printf("BEFORE: len = %f.3\t", len);
        //printf("Param->GetX() = %.2f\n", parPredict.GetX());
        BmnStatus resultPropagate = fKalman->TGeoTrackPropagate(&parPredict, hit->GetZ(), fPDG, NULL, &len, fIsField);
        if (resultPropagate == kBMNERROR)
            continue;  // skip in case kalman error
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

    if (minHit != NULL) {  // Check if hit was added
        FairTrackParam ParPredFirst(*(tr->GetParamFirst()));
        FairTrackParam ParPredLast(*(tr->GetParamLast()));
        ParPredFirst.SetQp(ParPredLast.GetQp());
        Double_t LenTrack = tr->GetLength();
        Double_t zTarget = -21.7;  // z of target by default
        if (fVertex)
            zTarget = fVertex->GetZ();
        BmnStatus resultPropagate = fKalman->TGeoTrackPropagate(&ParPredFirst, zTarget, fPDG, NULL, &LenPropFirst, fIsField);
        if (resultPropagate != kBMNERROR) {  // skip in case kalman error

            if (num == 1)
                tr->SetTof1HitIndex(minIdx);
            else
                tr->SetTof2HitIndex(minIdx);

            minHit->SetIndex(trIndex);
            //    printf("LenFirst = %.3f;  LenTrack = %.3f;  LenLast = %.3f\n", LenPropFirst, LenTrack, LenPropLast);
            minHit->SetLength(LenPropFirst + LenTrack + LenPropLast);  // length from target to Tof hit
            //    printf("Writed length = %.3f\n", minHit->GetLength());
            minHit->SetUsing(kTRUE);
            tr->SetNHits(tr->GetNHits() + 1);
            return kBMNSUCCESS;
        } else
            return kBMNERROR;
    } else
        return kBMNERROR;
}

BmnStatus BmnGlobalTracking::MatchingSil(BmnGlobalTrack *glTr) {
    //we need this function only for SRC
    //In BM@N we use silicon and GEM hits as a whole

    if (glTr->GetNHits() != 6)
        return kBMNERROR;

    //    vector<BmnHit*> silTrackHits;
    //    vector<Int_t> silTrackIdx;
    //    for (Int_t iSt = 0; iSt < fDetectorSI->GetNStations(); ++iSt) {
    //        Double_t minDist = DBL_MAX;
    //        BmnHit* minHit = NULL; // Pointer to the nearest hit
    //        Int_t minIdx = -1;
    //        FairTrackParam minParPred; // predicted track parameters for closest hit
    //        for (Int_t hitIdx = 0; hitIdx < fSilHits->GetEntriesFast(); ++hitIdx) {
    //            BmnHit* hit = (BmnHit*) fSilHits->At(hitIdx);
    //            if (hit->GetStation() != iSt) continue;
    //            if (hit->IsUsed()) continue;
    //            FairTrackParam parPredict(*(glTr->GetParamFirst()));
    //            Double_t len = 0.;
    //            //        if (kalman->TGeoTrackPropagate(&parPredict, hit->GetZ(), fPDG, NULL, &len, fIsField) == kBMNERROR) continue; // skip in case kalman error
    //            if (kalman->TGeoTrackPropagate(&parPredict, hit->GetZ(), fPDG, NULL, &len, kFALSE) == kBMNERROR) continue; // skip in case kalman error
    //
    //            //=== we need it only for alignment
    //            fhXSiGemResid->Fill(parPredict.GetX() - hit->GetX());
    //            fhYSiGemResid->Fill(parPredict.GetY() - hit->GetY());
    //            //===
    //
    //            Double_t dist = Sqrt(Sq(parPredict.GetX() - hit->GetX()) + Sq(parPredict.GetY() - hit->GetY()));
    //            if (dist < minDist && dist <= 5.) {
    //                minDist = dist;
    //                minHit = hit;
    //                minParPred = parPredict;
    //                minIdx = hitIdx;
    //            }
    //        }
    //
    //        if (minHit != NULL) { // Check if hit was added
    //            silTrackHits.push_back(minHit);
    //            silTrackIdx.push_back(minIdx);
    //            //            glTr->SetParamFirst(minParPred);
    //            //            glTr->AddHit(minIdx, minHit);
    //            //            glTr->SortHits();
    //
    //        }
    //
    //    }

    Double_t Z0 = -250;

    FairTrackParam parGem(*(glTr->GetParamFirst()));
    fKalman->TGeoTrackPropagate(&parGem, Z0, fPDG, NULL, NULL, fIsField);
    FairTrackParam parGemZ0 = parGem;

    for (Int_t trIdx = 0; trIdx < fSiliconTracks->GetEntriesFast(); ++trIdx) {
        BmnTrack *siTr = (BmnTrack *)fSiliconTracks->At(trIdx);
        FairTrackParam parSil(*(siTr->GetParamLast()));
        fKalman->TGeoTrackPropagate(&parSil, Z0, fPDG, NULL, NULL, kFALSE);
        FairTrackParam parSilZ0 = parSil;
        //=== we need it only for alignment
        fhXSiGemResid->Fill(parGemZ0.GetX() - parSilZ0.GetX());
        fhYSiGemResid->Fill(parGemZ0.GetY() - parSilZ0.GetY());
        fhTxSiGemResid->Fill(parGemZ0.GetTx() - parSilZ0.GetTx());
        fhTySiGemResid->Fill(parGemZ0.GetTy() - parSilZ0.GetTy());
        fhXdXSiGemResid->Fill(parGemZ0.GetX(), parGemZ0.GetX() - parSilZ0.GetX());
        fhYdYSiGemResid->Fill(parGemZ0.GetY(), parGemZ0.GetY() - parSilZ0.GetY());
        fhTxdXSiGemResid->Fill(parGemZ0.GetTx(), parGemZ0.GetX() - parSilZ0.GetX());
        fhTydYSiGemResid->Fill(parGemZ0.GetTy(), parGemZ0.GetY() - parSilZ0.GetY());
    }
}

BmnStatus BmnGlobalTracking::CreateDchHitsFromTracks() {
    for (Int_t i = 0; i < fDchTracks->GetEntriesFast(); ++i) {
        BmnDchTrack *dchTr = (BmnDchTrack *)fDchTracks->At(i);
        Float_t x = dchTr->GetParamFirst()->GetX();
        Float_t y = dchTr->GetParamFirst()->GetY();
        Float_t z = dchTr->GetParamFirst()->GetZ();
        new ((*fDchHits)[fDchHits->GetEntriesFast()]) BmnDchHit(1, TVector3(x, y, z), TVector3(0, 0, 0), 0);
    }
}

BmnStatus BmnGlobalTracking::MatchingDCH(BmnGlobalTrack *tr) {
    Double_t threshDist = 10;
    Double_t minAverDist = DBL_MAX;
    FairTrackParam minParDchZ0;
    BmnTrack *matchedDch = nullptr;
    Int_t minIdx = -1;

    Double_t Z0 = 600;  //480;
    Double_t Z1 = 600;

    //    //find dch track with minimal chi-square ====>
    //    map<Double_t, BmnTrack*> mapDchChiTrack;
    //    for (Int_t trIdx = 0; trIdx < fDchTracks->GetEntriesFast(); ++trIdx) {
    //        BmnTrack* dchTr = (BmnTrack*) fDchTracks->At(trIdx);
    //        if (dchTr->GetNHits() < 14) continue; // try to work only with the long tracks
    //        mapDchChiTrack.insert(pair<Double_t, BmnTrack*>(dchTr->GetChi2(), dchTr));
    //    }
    //
    //    if (mapDchChiTrack.size() == 0) return kBMNERROR;
    if (tr->GetNHits() != 6)
        return kBMNERROR;
    //
    //    map<Double_t, BmnTrack*>::iterator it = mapDchChiTrack.begin();
    //    BmnTrack* minDchTr = (BmnTrack*) (it->second); //DCH track with minimal chi-square
    //    //<====
    //
    //    FairTrackParam parDch(*(minDchTr->GetParamFirst()));
    //    FairTrackParam parGem(*(tr->GetParamLast()));
    //
    //    Double_t chiTmp = 0.0;
    //
    //    if (tr->GetGemTrackIndex() != -1) {
    //        BmnTrack* gemTr = (BmnTrack*) fGemTracks->UncheckedAt(tr->GetGemTrackIndex());
    //        for (Int_t iHit = 0; iHit < gemTr->GetNHits(); iHit++) {
    //            BmnGemStripHit* hit = (BmnGemStripHit*) fGemHits->UncheckedAt(gemTr->GetHitIndex(iHit));
    //            kalman->TGeoTrackPropagate(&parDch, hit->GetZ(), fPDG, NULL, NULL, fIsField);
    //            kalman->TGeoTrackPropagate(&parGem, hit->GetZ(), fPDG, NULL, NULL, fIsField);
    //            Double_t dX = parGem.GetX() - parDch.GetX();
    //            //            printf("Station %d: dX = %f\n", hit->GetStation(), dX);
    //            fhDchGemResidSt[hit->GetStation() - 4]->Fill(dX);
    //        }
    //    }

    FairTrackParam parGem(*(tr->GetParamLast()));
    fKalman->TGeoTrackPropagate(&parGem, Z0, fPDG, NULL, NULL, fIsField);
    FairTrackParam parGemZ0 = parGem;
    fKalman->TGeoTrackPropagate(&parGem, Z1, fPDG, NULL, NULL, fIsField);
    FairTrackParam parGemZ1 = parGem;

    //Prepare virtual hits on Z=400 and z=600

    //    BmnHit* virtualHitZ0 = new BmnHit(1, TVector3(parDch.GetX(), parDch.GetY(), parDch.GetZ()), TVector3(0.001, 0.001, 0.0), 0);
    //    kalman->TGeoTrackPropagate(&parDch, Z1, fPDG, NULL, NULL, kFALSE);
    //    BmnHit* virtualHitZ1 = new BmnHit(1, TVector3(parDch.GetX(), parDch.GetY(), parDch.GetZ()), TVector3(0.01, 0.01, 0.0), 0);

    //    Double_t chiNew = 0.0;
    //    FairTrackParam parGem(*(tr->GetParamLast()));
    //    printf("BEFORE: P = %f, chi = %f\n", 1.0/parGem.GetQp(), chiNew);
    //    printf("BEFORE:\n");
    //    parGem.Print();
    //    kalman->TGeoTrackPropagate(&parGem, Z0, fPDG, NULL, NULL, fIsField);
    //    printf("Hit_Z0 = (%f, %f, %f)\n", virtualHitZ0->GetX(), virtualHitZ0->GetY(), virtualHitZ0->GetZ());
    //    printf("AFTER PROPAGATE Z0:\n");
    //    parGem.Print();
    //    kalman->Update(&parGem, virtualHitZ0, chiNew);
    //    printf("AFTER UPDATE Z0:\n");
    //    parGem.Print();
    //    printf("AFTER Z0:  P = %f, chi = %f\n", 1.0/parGem.GetQp(), chiNew);
    //    kalman->TGeoTrackPropagate(&parGem, Z1, fPDG, NULL, NULL, fIsField);
    //    kalman->Update(&parGem, virtualHitZ1, chiNew);
    //    printf("AFTER Z1:  P = %f, chi = %f\n", 1.0/parGem.GetQp(), chiNew);
    //    tr->SetParamLast(parGem);
    //    printf("chiNew = %f\n", chiNew);

    for (Int_t trIdx = 0; trIdx < fDchTracks->GetEntriesFast(); ++trIdx) {
        BmnTrack *dchTr = (BmnTrack *)fDchTracks->At(trIdx);
        if (dchTr->GetNHits() < 14)
            continue;  // try to work only with the long tracks
        FairTrackParam parDch(*(dchTr->GetParamFirst()));
        fKalman->TGeoTrackPropagate(&parDch, Z0, fPDG, NULL, NULL, kFALSE);
        FairTrackParam parDchZ0 = parDch;
        fKalman->TGeoTrackPropagate(&parDch, Z1, fPDG, NULL, NULL, kFALSE);
        FairTrackParam parDchZ1 = parDch;
        //=== we need it only for alignment
        fhXDchGemResid->Fill(parGemZ0.GetX() - parDchZ0.GetX());
        fhYDchGemResid->Fill(parGemZ0.GetY() - parDchZ0.GetY());
        fhTxDchGemResid->Fill(parGemZ0.GetTx() - parDchZ0.GetTx());
        fhTyDchGemResid->Fill(parGemZ0.GetTy() - parDchZ0.GetTy());
        fhXdXDchGemResid->Fill(parGemZ0.GetX(), parGemZ0.GetX() - parDchZ0.GetX());
        fhYdYDchGemResid->Fill(parGemZ0.GetY(), parGemZ0.GetY() - parDchZ0.GetY());
        fhTxdXDchGemResid->Fill(parGemZ0.GetTx(), parGemZ0.GetX() - parDchZ0.GetX());
        fhTydYDchGemResid->Fill(parGemZ0.GetTy(), parGemZ0.GetY() - parDchZ0.GetY());
        //        printf("parGem.GetX() - parDch.GetX() = %f,   parGem.GetX() = %f,   parDch.GetX() = %f\n", parGemZ0.GetX() - parDch.GetX(), parGemZ0.GetX(), parDch.GetX());
        //        printf("parGem.GetTy() - parDch.GetTy() = %f,   parGem.GetTy() = %f,   parDch.GetTy() = %f\n", parGem.GetTy() - parDch.GetTy(), parGem.GetTy(), parDch.GetTy());
        //===
        //        Double_t dist0 = Sqrt(Sq(parGemZ0.GetX() - parDchZ0.GetX()) + Sq(parGemZ0.GetY() - parDchZ0.GetY()));
        //        Double_t dist1 = Sqrt(Sq(parGemZ1.GetX() - parDchZ1.GetX()) + Sq(parGemZ1.GetY() - parDchZ1.GetY()));
        //        if (dist0 < threshDist && dist1 < threshDist) {
        //            Double_t averDist = 0.5 * (dist0 + dist1);
        //            if (averDist < minAverDist) {
        //                minIdx = trIdx;
        //                matchedDch = dchTr;
        //                minParDchZ0 = parDchZ0;
        //            }
        //        }
    }

    //    if (minIdx != -1) {
    //        tr->SetDchTrackIndex(minIdx);
    //        matchedDch->SetFlag(7);
    //        //        //=== we need it only for alignment
    //        //        fhXDchGemResid->Fill(parGemZ0.GetX() - minParDchZ0.GetX());
    //        //        fhYDchGemResid->Fill(parGemZ0.GetY() - minParDchZ0.GetY());
    //        //        fhTxDchGemResid->Fill(parGemZ0.GetTx() - minParDchZ0.GetTx());
    //        //        fhTyDchGemResid->Fill(parGemZ0.GetTy() - minParDchZ0.GetTy());
    //        //        fhTxXDchGemResid->Fill(parGemZ0.GetX() - minParDchZ0.GetX(), parGemZ0.GetTx() - minParDchZ0.GetTx());
    //        //        fhTyYDchGemResid->Fill(parGemZ0.GetY() - minParDchZ0.GetY(), parGemZ0.GetTy() - minParDchZ0.GetTy());
    //        //        //        printf("parGem.GetX() - parDch.GetX() = %f,   parGem.GetX() = %f,   parDch.GetX() = %f\n", parGemZ0.GetX() - parDch.GetX(), parGemZ0.GetX(), parDch.GetX());
    //        //        //        printf("parGem.GetTy() - parDch.GetTy() = %f,   parGem.GetTy() = %f,   parDch.GetTy() = %f\n", parGem.GetTy() - parDch.GetTy(), parGem.GetTy(), parDch.GetTy());
    //        //        //===
    //    }
}

BmnStatus BmnGlobalTracking::RefitToDetector(BmnGlobalTrack *tr, Int_t hitId, TClonesArray *hitArr, FairTrackParam *par, Int_t *nodeIdx, vector<BmnFitNode> *nodes) {
    if (tr->GetTof2HitIndex() != -1) {
        BmnHit *hit = (BmnHit *)hitArr->At(hitId);
        Float_t Ze = hit->GetZ();
        Double_t length = 0;
        vector<Double_t> F(25);
        if (fKalman->TGeoTrackPropagate(par, Ze, 2212, &F, &length, fIsField) == kBMNERROR) {
            tr->SetFlag(kBMNBAD);
            return kBMNERROR;
        }

        nodes->at(*nodeIdx).SetPredictedParam(par);
        nodes->at(*nodeIdx).SetF(F);
        Double_t chi2Hit = 0.;
        if (fKalman->Update(par, hit, chi2Hit) == kBMNERROR) {
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
    return kBMNSUCCESS;
}

BmnStatus BmnGlobalTracking::Refit(BmnGlobalTrack *tr) {
    //    vector<BmnFitNode> nodes(tr->GetNHits());
    //    Int_t nodeIdx = tr->GetNHits() - 1;
    //    FairTrackParam par = *(tr->GetParamLast());
    //    //    FairTrackParam par = *(tr->GetParamFirst());
    //
    //    //TOF2 part
    //    //    if (fDet.GetDet(kTOF) && tr->GetTof2HitIndex() != -1 && fTof2Hits) {
    //    //        if (RefitToDetector(tr, tr->GetTof2HitIndex(), fTof2Hits, &par, &nodeIdx, &nodes) == kBMNERROR) return kBMNERROR;
    //    //    }
    //
    //    //DCH1 part
    //    //        if (fDet.GetDet(kDCH) && tr->GetDchHitIndex() != -1 && fDchHits) {
    //    //            if (RefitToDetector(tr, tr->GetDchHitIndex(), fDchHits, &par, &nodeIdx, &nodes) == kBMNERROR) return kBMNERROR;
    //    //        }
    //
    //    //TOF1 part
    //    if (fDet.GetDet(kTOF1) && tr->GetTof1HitIndex() != -1 && fTof1Hits) {
    //        if (RefitToDetector(tr, tr->GetTof1HitIndex(), fTof1Hits, &par, &nodeIdx, &nodes) == kBMNERROR) return kBMNERROR;
    //    }
    //
    //    //GEM part
    //    if (fDet.GetDet(kGEM) && tr->GetGemTrackIndex() != -1) {
    //        BmnGemTrack* gemTr = (BmnGemTrack*) fInnerTracks->At(tr->GetGemTrackIndex());
    //        for (Int_t i = gemTr->GetNHits() - 1; i >= 0; --i) {
    //            if (RefitToDetector(tr, i, fGemHits, &par, &nodeIdx, &nodes) == kBMNERROR) return kBMNERROR;
    //        }
    //    }
    //
    //    tr->SetParamFirst(par);
    //    tr->SetFitNodes(nodes);
    return kBMNSUCCESS;
}

void BmnGlobalTracking::Finish() {
    delete fDetectorSI;
    delete fKalman;

    if (fDoAlign) {
        TFile matchResid("matchResid.root", "RECREATE");

        matchResid.Add(fhXCscGemResid);
        matchResid.Add(fhYCscGemResid);

        matchResid.Add(fhXSiGemResid);
        matchResid.Add(fhYSiGemResid);
        matchResid.Add(fhTxSiGemResid);
        matchResid.Add(fhTySiGemResid);
        matchResid.Add(fhXdXSiGemResid);
        matchResid.Add(fhYdYSiGemResid);
        matchResid.Add(fhTxdXSiGemResid);
        matchResid.Add(fhTydYSiGemResid);
        matchResid.Add(fhXDchGemResid);
        matchResid.Add(fhYDchGemResid);
        matchResid.Add(fhTxDchGemResid);
        matchResid.Add(fhTyDchGemResid);
        matchResid.Add(fhXdXDchGemResid);
        matchResid.Add(fhYdYDchGemResid);
        matchResid.Add(fhTxdXDchGemResid);
        matchResid.Add(fhTydYDchGemResid);

        for (Int_t i = 0; i < 6; ++i) {
            fhdXGemSt[i]->Fit("gaus", "WWSQ");
            fhdYGemSt[i]->Fit("gaus", "WWSQ");
            matchResid.Add(fhdXGemSt[i]);
            matchResid.Add(fhdYGemSt[i]);
            matchResid.Add(fhdXTxGemSt[i]);
            matchResid.Add(fhdYTyGemSt[i]);
        }

        matchResid.Write();
        matchResid.Close();
    }

    cout << "Work time of the Global matching: " << workTime << endl;
}

void BmnGlobalTracking::CalculateLength() {
    //    if (fGlobalTracks == NULL) return;
    //
    //    /* Calculate the length of the global track
    //     * starting with (0, 0, 0) and adding all
    //     * distances between hits
    //     */
    //    for (Int_t iTr = 0; iTr < fGlobalTracks->GetEntriesFast(); iTr++) {
    //        BmnGlobalTrack* glTr = (BmnGlobalTrack*) fGlobalTracks->At(iTr);
    //        vector<Float_t> X, Y, Z;
    //        X.push_back(0.);
    //        Y.push_back(0.);
    //        Z.push_back(0.);
    //
    //        if (fDet.GetDet(kTOF1)) {
    //            if (glTr->GetTof1HitIndex() > -1 && fTof1Hits) {
    //                const BmnHit* hit = (BmnHit*) fTof1Hits->At(glTr->GetTof1HitIndex());
    //                if (!hit) continue;
    //                X.push_back(hit->GetX());
    //                Y.push_back(hit->GetY());
    //                Z.push_back(hit->GetZ());
    //            }
    //        }
    //        if (fDet.GetDet(kDCH)) {
    //            //            if (glTr->GetDch1HitIndex() > -1 && fDch1Hits) {
    //            //                const BmnHit* hit = (BmnHit*) fDch1Hits->At(glTr->GetDch1HitIndex());
    //            //                if (!hit) continue;
    //            //                X.push_back(hit->GetX());
    //            //                Y.push_back(hit->GetY());
    //            //                Z.push_back(hit->GetZ());
    //            //            }
    //        }
    //        if (fDet.GetDet(kTOF)) {
    //            if (glTr->GetTof2HitIndex() > -1 && fTof2Hits) {
    //                const BmnHit* hit = (BmnHit*) fTof2Hits->At(glTr->GetTof2HitIndex());
    //                if (!hit) continue;
    //                X.push_back(hit->GetX());
    //                Y.push_back(hit->GetY());
    //                Z.push_back(hit->GetZ());
    //            }
    //        }
    //        // Calculate distances between hits
    //        Float_t length = 0.;
    //        for (Int_t i = 0; i < X.size() - 1; i++) {
    //            Float_t dX = X[i] - X[i + 1];
    //            Float_t dY = Y[i] - Y[i + 1];
    //            Float_t dZ = Z[i] - Z[i + 1];
    //            length += Sqrt(dX * dX + dY * dY + dZ * dZ);
    //        }
    //        glTr->SetLength(length);
    //    }
}

ClassImp(BmnGlobalTracking);
