
// This Class' Header ------------------
#include <TMath.h>
#include "TStyle.h"
#include "BmnGemTrackFinder.h"
#include "TObjArray.h"
#include "TVector3.h"
#include "BmnGemHit.h"
#include "FairMCPoint.h"
#include "CbmMCTrack.h"
#include "CbmStsPoint.h"
#include "FairRunAna.h"
#include "BmnGemStripHit.h"
#include "BmnMatrixMath.h"
#include "BmnKalmanFilter_tmp.h"

//-----------------------------------------
static Float_t workTime = 0.0;
//-----------------------------------------

using namespace std;
using namespace TMath;

BmnGemTrackFinder::BmnGemTrackFinder() :
fPDG(2212),
fEventNo(0),
fNHitsCut(3),
fDistCut(1.0) {
    fKalman = NULL;
    fGemHitArray = NULL;
    fGemTracksArray = NULL;
    fGemSeedsArray = NULL;
    fField = NULL;
    fIsField = kTRUE;
    fGoForward = kTRUE;
    fIsTarget = kTRUE;
    fHitsBranchName = "BmnGemStripHit";
    fSeedsBranchName = "BmnGemSeed";
    fTracksBranchName = "BmnGemTrack";
    fKalman = new BmnKalmanFilter_tmp();
}

BmnGemTrackFinder::~BmnGemTrackFinder() {
}

InitStatus BmnGemTrackFinder::Init() {

    if (fVerbose) cout << "======================== GEM track finder init started ====================" << endl;

    //Get ROOT Manager
    FairRootManager* ioman = FairRootManager::Instance();
    if (NULL == ioman) {
        Fatal("Init", "FairRootManager is not instantiated");
    }

    fGemHitArray = (TClonesArray*) ioman->GetObject(fHitsBranchName); //in
    fGemSeedsArray = (TClonesArray*) ioman->GetObject(fSeedsBranchName); //in
    fGemTracksArray = new TClonesArray(fTracksBranchName, 100); //out
    ioman->Register("BmnGemTrack", "GEM", fGemTracksArray, kTRUE);

    fField = FairRunAna::Instance()->GetField();
    // Use a wider corridor by default when doing alignment
    if (Abs(fField->GetBy(0., 0., 0.)) < FLT_EPSILON)
        fDistCut = 2.5;

    fPropagator = new BmnTrackPropagator();

    fDetector = new BmnGemStripStationSet_RunSpring2017(BmnGemStripConfiguration::RunSpring2017);

    if (fVerbose) cout << "======================== GEM track finder init finished ===================" << endl;
}

void BmnGemTrackFinder::Exec(Option_t* opt) {

    if (fVerbose) cout << "\n======================== GEM track finder exec started ====================" << endl;
    if (fVerbose) cout << "\n Event number: " << fEventNo++ << endl;

    clock_t tStart = clock();
    fGemTracksArray->Delete();
    //CheckSplitting(fGemSeedsArray); //comment tmp

    vector<BmnGemTrack> tracks;

    for (Int_t iTr = 0; iTr < fGemSeedsArray->GetEntriesFast(); ++iTr) {
        BmnGemTrack* gemTrack = (BmnGemTrack*) fGemSeedsArray->At(iTr);
        if (gemTrack->GetFlag() == -1) continue;
        BmnGemTrack track;
        track.SetParamFirst(*(gemTrack->GetParamFirst()));
        track.SetParamLast(*(gemTrack->GetParamLast()));
        vector<BmnFitNode> nodes(fDetector->GetNStations());
        track.SetFitNodes(nodes);

        if (fGoForward) {
            BmnGemStripHit* startHit = (BmnGemStripHit*) fGemHitArray->At(gemTrack->GetHitIndex(0));
            track.AddHit(gemTrack->GetHitIndex(0), startHit);
            for (Int_t iSt = startHit->GetStation() + 0; iSt < fDetector->GetNStations(); ++iSt)
                NearestHitMerge(iSt, &track);

            if (startHit->GetStation() != 0)
                for (Int_t iSt = startHit->GetStation() - 1; iSt >= 0; iSt--)
                    NearestHitMerge(iSt, &track);
        } else {
            BmnGemStripHit* lastHit = (BmnGemStripHit*) fGemHitArray->At(gemTrack->GetHitIndex(gemTrack->GetNHits() - 1));
            track.AddHit(gemTrack->GetHitIndex(gemTrack->GetNHits() - 1), lastHit);
            for (Int_t iSt = lastHit->GetStation() - 1; iSt >= 0; iSt--)
                NearestHitMerge(iSt, &track);
        }

        if (track.GetNHits() >= fNHitsCut) {
            CalculateLength(&track);
            tracks.push_back(track);
        }
    }

    if (!fIsTarget) {
        Float_t minChi = FLT_MAX;
        BmnGemTrack* minTrack = NULL;
        for (Int_t i = 0; i < tracks.size(); ++i) {
            if (tracks.at(i).GetChi2() < minChi) {
                minChi = tracks.at(i).GetChi2();
                minTrack = &tracks.at(i);
            }
        }
        if (minTrack != NULL) {
            fKalman->FitSmooth(minTrack, fGemHitArray);

            new((*fGemTracksArray)[fGemTracksArray->GetEntriesFast()]) BmnGemTrack(*minTrack);
        }
    } else {
        for (Int_t iTr = 0; iTr < tracks.size(); ++iTr)
            for (Int_t jTr = iTr + 1; jTr < tracks.size(); ++jTr) {
                Int_t nCopies = 0;
                for (Int_t iHit = 0; iHit < tracks[iTr].GetNHits(); iHit++) {
                    Int_t iHitIdx = tracks[iTr].GetHitIndex(iHit);
                    for (Int_t jHit = 0; jHit < tracks[jTr].GetNHits(); jHit++) {
                        Int_t jHitIdx = tracks[jTr].GetHitIndex(jHit);
                        if (iHitIdx == jHitIdx)
                            nCopies++;
                    }
                }
                //                printf("nCopies = %d\n", nCopies);
                if (nCopies > 0) {
                    if (Abs(tracks[iTr].GetChi2()) > Abs(tracks[jTr].GetChi2())) {
                        tracks[iTr].SetFlag(-1);
                        break;
                    } else
                        tracks[jTr].SetFlag(-1);
                }
            }
        for (Int_t iTr = 0; iTr < tracks.size(); ++iTr)
            if (tracks[iTr].GetFlag() != -1 && IsParCorrect(tracks[iTr].GetParamFirst()) && IsParCorrect(tracks[iTr].GetParamLast())) {
                //                Double_t chi = 0.0;
                //                TVector3 circle = CircleFit(&tracks[iTr], fGemHitArray, chi);
                //                printf("R = %f\n", circle.Z());
                //                Float_t QP = Q / S / R;
                //                Float_t S = 0.0003 * fSum;
                new((*fGemTracksArray)[fGemTracksArray->GetEntriesFast()]) BmnGemTrack(tracks[iTr]);
            }
    }

    clock_t tFinish = clock();
    if (fVerbose) cout << "GEM_TRACKING: Number of found tracks: " << fGemTracksArray->GetEntriesFast() << endl;

    workTime += ((Float_t) (tFinish - tStart)) / CLOCKS_PER_SEC;

    if (fVerbose) cout << "\n======================== GEM track finder exec finished ===================" << endl;

}

void BmnGemTrackFinder::Finish() {
    ofstream outFile;
    outFile.open("QA/timing.txt", ofstream::app);
    outFile << "Track Finder Time: " << workTime << endl;
    cout << "Work time of the GEM track finder: " << workTime << endl;
    delete fDetector;
    delete fKalman;
}

BmnStatus BmnGemTrackFinder::ConnectNearestSeed(BmnGemTrack* baseSeed, TClonesArray* arr) {

    if (baseSeed->GetChi2() < 0.0) return kBMNERROR;
    FairTrackParam* baseSeedLastPar = baseSeed->GetParamLast();

    Float_t yI = baseSeedLastPar->GetY();
    Float_t zI = baseSeedLastPar->GetZ();
    Float_t tyI = baseSeedLastPar->GetTy();
    TVector3 spirParI = SpiralFit(baseSeed, fGemHitArray);
    Float_t aI = spirParI.X();
    Float_t bI = spirParI.Y();

    //needed to get nearest track
    //    Float_t minDeltaR = 1.2; //best by QA
    Float_t minDeltaY = 1.2;
    Float_t minDeltaX = 1.2;

    BmnGemTrack* minTrackRight = NULL;
    for (Int_t j = 0; j < arr->GetEntriesFast(); ++j) {
        BmnGemTrack* trackJ = (BmnGemTrack*) arr->At(j);
        if (trackJ->GetChi2() < 0.0) continue;
        if (trackJ->IsUsed()) continue;

        FairTrackParam* firstJ = trackJ->GetParamFirst();

        Float_t zJ = firstJ->GetZ();
        Float_t yJ = firstJ->GetY();
        if (zI > zJ) continue;
        Float_t tyJ = firstJ->GetTy();

        TVector3 spirParJ = SpiralFit(trackJ, fGemHitArray);
        Float_t aJ = spirParJ.X();
        Float_t bJ = spirParJ.Y();

        const Float_t zMid = zI + (zJ - zI) / 2.0;
        const Float_t yI_zMid = tyI * (zMid - zI) + yI;
        const Float_t yJ_zMid = tyJ * (zMid - zJ) + yJ;

        TF1 fI("SpiralI", "sqrt(x^2 + [2]^2) - [0] - [1] * atan(x / [2])", -100, 100);
        fI.SetParameter(0, aI);
        fI.SetParameter(1, bI);
        fI.SetParameter(2, zMid);
        const Float_t xI_zMid = NumericalRootFinder(fI, -100, 100);
        TF1 fJ("SpiralJ", "sqrt(x^2 + [2]^2) - [0] - [1] * atan(x / [2])", -100, 100);
        fJ.SetParameter(0, aJ);
        fJ.SetParameter(1, bJ);
        fJ.SetParameter(2, zMid);
        const Float_t xJ_zMid = NumericalRootFinder(fJ, -100, 100);

        const Float_t dX = Abs(xI_zMid - xJ_zMid);
        const Float_t dY = Abs(yI_zMid - yJ_zMid);
        if (dX < minDeltaX && dY < minDeltaY) {
            minTrackRight = trackJ;
            minDeltaX = dX;
            minDeltaY = dY;
        }
    }

    if (minTrackRight != NULL) {

        minTrackRight->SetUsing(kTRUE);
        ConnectNearestSeed(minTrackRight, arr);
        for (Int_t iHit = 0; iHit < minTrackRight->GetNHits(); ++iHit) {
            BmnGemStripHit* hit = (BmnGemStripHit*) GetHit(minTrackRight->GetHitIndex(iHit));
            baseSeed->AddHit(minTrackRight->GetHitIndex(iHit), hit);
            hit->SetUsing(kTRUE);
        }
        baseSeed->SortHits();
        baseSeed->SetParamLast(*(minTrackRight->GetParamLast()));
        minTrackRight->SetChi2(-1.0);
        minTrackRight->SetFlag(-1);
        return kBMNSUCCESS;
    } else {
        return kBMNERROR;
    }
}

BmnStatus BmnGemTrackFinder::CheckSplitting(TClonesArray* arr) {

    for (Int_t i = 0; i < arr->GetEntriesFast(); ++i) {
        BmnGemTrack* seed = (BmnGemTrack*) arr->At(i);
        if (seed->GetNHits() > 8) continue;
        if (seed->GetChi2() < 0.0) continue;
        if (seed->IsUsed()) continue;
        Short_t minStation = 100;
        for (Int_t iHit = 0; iHit < seed->GetNHits(); ++iHit) {
            BmnGemStripHit* hit = (BmnGemStripHit*) GetHit(seed->GetHitIndex(iHit));
            if (hit->GetStation() < minStation) minStation = hit->GetStation();
        }
        if (minStation <= 2) {
            seed->SetUsing(kTRUE);
            for (Int_t iHit = 0; iHit < seed->GetNHits(); ++iHit) {
                BmnGemStripHit* hit = (BmnGemStripHit*) GetHit(seed->GetHitIndex(iHit));
                hit->SetUsing(kTRUE);
            }
            ConnectNearestSeed(seed, arr);
        }
    }
}

Float_t BmnGemTrackFinder::Dist(Float_t x1, Float_t y1, Float_t x2, Float_t y2) {
    if (Sqr(x1 - x2) + Sqr(y1 - y2) < 0.0) {
        return 0.0;
    } else {
        return Sqrt(Sqr(x1 - x2) + Sqr(y1 - y2));
    }
}

Float_t BmnGemTrackFinder::Sqr(Float_t x) {
    return x * x;
}

BmnGemStripHit * BmnGemTrackFinder::GetHit(Int_t i) {
    BmnGemStripHit* hit = (BmnGemStripHit*) fGemHitArray->At(i);
    if (!hit) return NULL;
    return hit;
}

BmnStatus BmnGemTrackFinder::NearestHitMerge(UInt_t station, BmnGemTrack* track) {

    // goForward - direction of approximation:
    // goForward = 1 - forward (0 --> nStation)
    // goForward = 0 - backward (nStation --> 0)

    BmnHit* minHit = NULL; // Pointer to hit with minimum chi-square
    Float_t minDist = FLT_MAX;
    Double_t minChiSq = DBL_MAX;
    Double_t minLen = DBL_MAX;
    Int_t minIdx = -1;
    Float_t dist = 0.0;
    FairTrackParam minParUp; // updated track parameters for closest hit
    FairTrackParam minParPred; // predicted track parameters for closest hit

    Float_t stationZ = fDetector->GetGemStation(station)->GetModule(0)->GetZPositionRegistered();
    FairTrackParam parPredict = (fGoForward) ? (*(track->GetParamFirst())) : (*(track->GetParamLast()));
    Double_t length = track->GetLength();
    TString propagationType = (fIsField) ? "field" : "line";
    fKalman->TGeoTrackPropagate(&parPredict, stationZ, fPDG, NULL, &length, propagationType);

    for (Int_t iHit = 0; iHit < fGemHitArray->GetEntriesFast(); ++iHit) {
        BmnGemStripHit* hit = (BmnGemStripHit*) GetHit(iHit);
        if (!hit) continue;
        if (!hit->GetFlag()) continue;
        if (hit->GetStation() != station) continue;
        FairTrackParam parUpdate = parPredict;
        Double_t chi = 0.0;
        fKalman->Update(&parUpdate, hit, chi);

        //        if (Abs(chi) > 10.) continue;
        //        dist = Dist(parUpdate.GetX(), parUpdate.GetY(), hit->GetX(), hit->GetY());
        dist = Dist(parPredict.GetX(), parPredict.GetY(), hit->GetX(), hit->GetY());
        //        if (chi < minChiSq) { // Check if hit is inside validation gate and closer to the track.
        if (dist < minDist && dist < fDistCut) { // Check if hit is inside validation gate and closer to the track.
            //        if (dist < minDist) { // Check if hit is inside validation gate and closer to the track.
            minChiSq = chi;
            minDist = dist;
            minHit = hit;
            minLen = length;
            minIdx = iHit;
            minParUp = parUpdate;
            minParPred = parPredict;
        }
    }

    if (minHit != NULL) {
        if (fGoForward)
            track->SetParamLast(minParUp);
        else
            track->SetParamFirst(minParUp);
        track->SetChi2(Abs(track->GetChi2()) + Abs(minChiSq));
        track->SetNDF(track->GetNDF() + 1);
        track->AddHit(minIdx, minHit);
        //track->SetLength(minLen);
        track->SortHits();
        //        minHit->SetFlag(kFALSE);
        BmnFitNode* node = track->GetFitNode(station);
        node->SetUpdatedParam(&minParUp);
        node->SetPredictedParam(&minParPred);
        return kBMNSUCCESS;
    } else {
        return kBMNERROR;
    }
}

Double_t BmnGemTrackFinder::CalculateLength(BmnGemTrack* tr) {
    if (!tr) return 0.0;

    vector<Float_t> X, Y, Z;
    X.push_back(0.);
    Y.push_back(0.);
    Z.push_back(0.);
    for (Int_t iGem = 0; iGem < tr->GetNHits(); iGem++) {
        const BmnHit* hit = (BmnHit*) fGemHitArray->At(tr->GetHitIndex(iGem));
        if (!hit) continue;
        X.push_back(hit->GetX());
        Y.push_back(hit->GetY());
        Z.push_back(hit->GetZ());
    }
    // Calculate distances between hits
    Float_t length = 0.;
    for (Int_t i = 0; i < X.size() - 1; i++) {
        Float_t dX = X[i] - X[i + 1];
        Float_t dY = Y[i] - Y[i + 1];
        Float_t dZ = Z[i] - Z[i + 1];
        length += Sqrt(dX * dX + dY * dY + dZ * dZ);
    }
    tr->SetLength(length);
    return length;

}