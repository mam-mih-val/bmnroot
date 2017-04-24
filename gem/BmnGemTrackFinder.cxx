
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
const Float_t kCHI2CUT = 100.0;
const Int_t kNHITSFORTRTACK = 4;
//-----------------------------------------

using namespace std;
using namespace TMath;

BmnGemTrackFinder::BmnGemTrackFinder() :
fPDG(211),
fEventNo(0),
fDistCut(1.0) {
    fKalman = NULL;
    fGemHitArray = NULL;
    fGemTracksArray = NULL;
    fGemSeedsArray = NULL;
    fField = NULL;
    fIsField = kTRUE;
    fGoForward = kTRUE;
    fHitsBranchName = "BmnGemStripHit";
    fSeedsBranchName = "BmnGemSeed";
    fTracksBranchName = "BmnGemTrack";
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

    fPropagator = new BmnTrackPropagator();

    fDetector = new BmnGemStripStationSet_RunSpring2017(BmnGemStripConfiguration::RunSpring2017);

    if (fVerbose) cout << "======================== GEM track finder init finished ===================" << endl;
}

void BmnGemTrackFinder::Exec(Option_t* opt) {

    if (fVerbose) cout << "\n======================== GEM track finder exec started ====================" << endl;
    if (fVerbose) cout << "\n Event number: " << fEventNo++ << endl;

    clock_t tStart = clock();
    fGemTracksArray->Delete();
    //    CheckSplitting(fGemSeedsArray); //comment tmp

    for (Int_t iTr = 0; iTr < fGemSeedsArray->GetEntriesFast(); ++iTr) {
        BmnGemTrack* gemTrack = (BmnGemTrack*) fGemSeedsArray->At(iTr);
        BmnGemTrack track;
        track.SetParamFirst(*(gemTrack->GetParamFirst()));
        track.SetParamLast(*(gemTrack->GetParamLast()));

        if (fGoForward) {
            BmnGemStripHit* startHit = (BmnGemStripHit*) fGemHitArray->At(gemTrack->GetHitIndex(0));
            track.AddHit(gemTrack->GetHitIndex(0), startHit);
            for (Int_t iSt = startHit->GetStation() + 1; iSt < fDetector->GetNStations(); ++iSt)
                NearestHitMerge(iSt, &track, fGoForward);
        } else {
            BmnGemStripHit* lastHit = (BmnGemStripHit*) fGemHitArray->At(gemTrack->GetHitIndex(gemTrack->GetNHits() - 1));
            track.AddHit(gemTrack->GetHitIndex(gemTrack->GetNHits() - 1), lastHit);
            for (Int_t iSt = lastHit->GetStation() - 2; iSt >= 0; iSt--)
                NearestHitMerge(iSt, &track, fGoForward);
        }
        if (track.GetNHits() >= kNHITSFORTRTACK) {
            CalculateLength(&track);
            new((*fGemTracksArray)[fGemTracksArray->GetEntriesFast()]) BmnGemTrack(track);
        }
    }


    //main branch! Comment tmp
    //    
    //    for (Int_t iTr = 0; iTr < fGemSeedsArray->GetEntriesFast(); ++iTr) {
    //        BmnGemTrack* track = (BmnGemTrack*) fGemSeedsArray->At(iTr);
    //
    //        if (track->GetChi2() < 0.0) continue; //split param
    //
    //        BmnGemTrack tr = *track;
    //        const Short_t nHits = tr.GetNHits();
    //        FairTrackParam par = *(tr.GetParamFirst());
    //
    //        vector<BmnFitNode> nodes;
    //        nodes.reserve(nHits);
    //        Double_t chi2 = 0.0;
    //        fKalman = new BmnKalmanFilter_tmp();
    //
    //        vector<Double_t>* F = new vector<Double_t> (25, 0.);
    //        if (F != NULL) {
    //            F->assign(25, 0.);
    //            (*F)[0] = 1.;
    //            (*F)[6] = 1.;
    //            (*F)[12] = 1.;
    //            (*F)[18] = 1.;
    //            (*F)[24] = 1.;
    //        }
    //
    //        Double_t length = 0.0;
    //
    //        for (Int_t iHit = 0; iHit < nHits; ++iHit) {
    //            BmnGemStripHit* hit = (BmnGemStripHit*) GetHit(tr.GetHitIndex(iHit));
    //            Double_t z = hit->GetZ();
    //            BmnFitNode node;
    //
    //            if (fIsField)
    //                fKalman->TGeoTrackPropagate(&par, z, fPDG, F, &length, "field");
    //            node.SetPredictedParam(&par);
    //            if (fIsField)
    //                fKalman->Update(&par, hit, chi2);
    //            node.SetUpdatedParam(&par);
    //            node.SetChiSqFiltered(chi2);
    //            node.SetF(*F);
    //
    //            nodes.push_back(node);
    //        }
    //
    //        tr.SetLength(length);
    //
    //        delete F;
    //        tr.SetFitNodes(nodes);
    //        tr.SetParamFirst(*(nodes[0].GetUpdatedParam()));
    //        tr.SetParamLast(*(nodes[nodes.size() - 1].GetUpdatedParam()));
    ////        if (fKalman->FitSmooth(&tr, fGemHitArray) == kBMNERROR) continue;
    //        tr.SetChi2(chi2);
    //        tr.SetNDF(nHits - 3);
    //
    //        //        if (tr.GetChi2() / tr.GetNDF() > fChiSqCut) {
    //        //            tr.SetFlag(kBMNBAD);
    //        //            for (Int_t iHit = 0; iHit < tr.GetNHits(); ++iHit)
    //        //                GetHit(tr.GetHitIndex(iHit))->SetUsing(kFALSE);
    //        //        } else tr.SetFlag(kBMNGOOD);
    //
    //        new((*fGemTracksArray)[fGemTracksArray->GetEntriesFast()]) BmnGemTrack(tr);
    //        delete fKalman;
    //    }

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

BmnHit * BmnGemTrackFinder::GetHit(Int_t i) {
    BmnHit* hit = (BmnHit*) fGemHitArray->At(i);
    if (!hit) cout << "-W- Wrong attempting to get hit number " << i << " from fGemHitArray, which contains " << fGemHitArray->GetEntriesFast() << " elements" << endl;
    return hit;
}

BmnStatus BmnGemTrackFinder::NearestHitMerge(UInt_t station, BmnGemTrack* track, Bool_t goForward) {

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

    for (Int_t iHit = 0; iHit < fGemHitArray->GetEntriesFast(); ++iHit) {
        BmnGemStripHit* hit = (BmnGemStripHit*) GetHit(iHit);
        if (!hit) continue;
        if (hit->GetStation() != station) continue;

        fKalman = new BmnKalmanFilter_tmp();
        Double_t length = track->GetLength();
        vector<Double_t>* F = new vector<Double_t> (25, 0.);
        if (F != NULL) {
            F->assign(25, 0.);
            (*F)[0] = 1.;
            (*F)[6] = 1.;
            (*F)[12] = 1.;
            (*F)[18] = 1.;
            (*F)[24] = 1.;
        }
        FairTrackParam parPredict = (goForward) ? (*(track->GetParamFirst())) : (*(track->GetParamLast()));
        fKalman->TGeoTrackPropagate(&parPredict, hit->GetZ(), fPDG, F, &length, "field");
        FairTrackParam parUpdate = parPredict;
        Double_t chi;
        fKalman->Update(&parUpdate, hit, chi);
        dist = Dist(parPredict.GetX(), parPredict.GetY(), hit->GetX(), hit->GetY());
        if (dist < minDist && dist < fDistCut) { // Check if hit is inside validation gate and closer to the track.
            minChiSq = chi;
            minDist = dist;
            minHit = hit;
            minLen = length;
            minIdx = iHit;
            minParUp = parUpdate;
            minParPred = parPredict;
        }
        delete F;
        delete fKalman;
    }

    if (minHit != NULL) {
        if (goForward)
            track->SetParamLast(minParUp);
        else
            track->SetParamFirst(minParUp);
        //        printf("Station = %d\n", station);
        //        printf("Xh = %f <----> Xt = %f\n", minHit->GetX(), minParUp.GetX());
        //        printf("Yh = %f <----> Yt = %f\n", minHit->GetY(), minParUp.GetY());
        //        printf("Zh = %f <----> Zt = %f\n", minHit->GetZ(), minParUp.GetZ());
        track->SetChi2(track->GetChi2() + minChiSq);
        track->SetNDF(track->GetNDF() + 1);
        track->AddHit(minIdx, minHit);
        //track->SetLength(minLen);
        track->SortHits();
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