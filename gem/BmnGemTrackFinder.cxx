
// This Class' Header ------------------
#include <TMath.h>
#include "TStyle.h"
#include "BmnGemTrackFinder.h"
#include "TObjArray.h"
#include "BmnGemHit.h"
#include "FairMCPoint.h"
#include "CbmMCTrack.h"
#include "CbmStsPoint.h"
#include "FairRunAna.h"
#include "BmnGemStripHit.h"
#include "BmnMatrixMath.h"

//-----------------------------------------
static Float_t workTime = 0.0;
const Float_t kCHI2CUT = 1000.0;
//-----------------------------------------

using namespace std;
using namespace TMath;

BmnGemTrackFinder::BmnGemTrackFinder() :
fPDG(211),
fEventNo(0),
fChiSqCut(250.) {
    //    fKalman = NULL;
    fGemHitArray = NULL;
    fGemTracksArray = NULL;
    fMCTracksArray = NULL;
    fGemSeedsArray = NULL;
    fGemSeedsArrayLow = NULL;
    fGemSeedsArrayMid1 = NULL;
    fGemSeedsArrayMid2 = NULL;
    fGemSeedsArrayBig = NULL;
    fMCPointsArray = NULL;
    fHitsBranchName = "BmnGemStripHit";
    fSeedsBranchName = "BmnGemSeeds";
    fTracksBranchName = "BmnGemTrack";
}

BmnGemTrackFinder::~BmnGemTrackFinder() {
}

InitStatus BmnGemTrackFinder::Init() {

    cout << "======================== GEM track finder init started ====================" << endl;

    //Get ROOT Manager
    FairRootManager* ioman = FairRootManager::Instance();
    if (NULL == ioman) {
        Fatal("Init", "FairRootManager is not instantiated");
    }

    fGemHitArray = (TClonesArray*) ioman->GetObject(fHitsBranchName); //in
    fGemSeedsArray = (TClonesArray*) ioman->GetObject(fSeedsBranchName); //in
    fGemSeedsArrayLow = (TClonesArray*) ioman->GetObject("BmnGemSeedsLow"); //in
    fGemSeedsArrayMid1 = (TClonesArray*) ioman->GetObject("BmnGemSeedsMid1"); //in
    fGemSeedsArrayMid2 = (TClonesArray*) ioman->GetObject("BmnGemSeedsMid2"); //in
    fGemSeedsArrayBig = (TClonesArray*) ioman->GetObject("BmnGemSeedsBig"); //in
    fGemTracksArray = new TClonesArray(fTracksBranchName, 100); //out
    ioman->Register("BmnGemTracks", "GEM", fGemTracksArray, kTRUE);

    fMCTracksArray = (TClonesArray*) ioman->GetObject("MCTrack");
    fMCPointsArray = (TClonesArray*) ioman->GetObject("StsPoint");

    fPropagator = new BmnTrackPropagator();

    cout << "======================== GEM track finder init finished ===================" << endl;
}

void BmnGemTrackFinder::Exec(Option_t* opt) {

    cout << "\n====================== GEM track finder exec started ======================" << endl;
    cout << "\n Event number: " << fEventNo++ << endl;

    clock_t tStart = clock();
    fGemTracksArray->Clear();

    fField = FairRunAna::Instance()->GetField();

//    CheckSplitting(fGemSeedsArrayBig);
//    for (Int_t i = 0; i < fGemSeedsArrayBig->GetEntriesFast(); ++i) {
//        //        fKalman = new BmnKalmanFilter_tmp();
//        BmnGemTrack* track = (BmnGemTrack*) fGemSeedsArrayBig->At(i);
//        if (track->GetChi2() < 0.0) continue; //split param
//
//        BmnGemTrack tr = *track;
//
//        if (!IsParCorrect(tr.GetParamFirst())) continue;
//        if (!IsParCorrect(tr.GetParamLast())) continue;
//
//        if (tr.GetNHits() < 4) continue;
//
//        TVector3 spirPar = SpiralFit(&tr, fGemHitArray);
//        Float_t R = spirPar.Z();
//        Float_t b = spirPar.Y();
//        Float_t a = spirPar.X();
//        BmnGemStripHit* hit0 = (BmnGemStripHit*) GetHit(tr.GetHitIndex(0));
//        Float_t x0 = hit0->GetX();
//        Float_t y0 = hit0->GetY();
//        Float_t z0 = hit0->GetZ();
//        Float_t Tx = Tan(-a / b); //Tx for r == 0
//        Float_t Ty = tr.GetParamFirst()->GetTy();
//        const Float_t Pxz = 0.0003 * Abs(fField->GetBy(x0, y0, z0)) * R; // Pt
//        //        cout << fField->GetBx(x0, y0, z0) << " " << fField->GetBy(x0, y0, z0) << " " << fField->GetBz(x0, y0, z0) << endl;
//        //        cout << fField->GetBx(0, 0, 0) << " " << fField->GetBy(0, 0, 0) << " " << fField->GetBz(0, 0, 0) << endl;
//
//        if (Abs(Pxz) < 0.00001) continue;
//        const Float_t Pz = Pxz / Sqrt(1 + Sqr(Tx));
//        const Float_t Px = Pz * Tx;
//        const Float_t Py = Pz * Ty;
//        Float_t QP = 1.0 / Sqrt(Px * Px + Py * Py + Pz * Pz);
//
//        FairTrackParam par;
//        par.SetPosition(TVector3(x0, y0, z0));
//        par.SetQp(QP);
//        par.SetTx(Tx);
//        par.SetTy(Ty);
//        tr.SetParamFirst(par);
//        tr.SetChi2(ChiSq(spirPar, &tr, fGemHitArray));
//        if (tr.GetChi2() > kCHI2CUT) tr.SetFlag(kBMNBAD);
//        else tr.SetFlag(kBMNGOOD);
//
//        //        FairTrackParam smoothPar = fKalman->Filtration(&tr, fGemHitArray);
//        //        tr.SetParamFirst(smoothPar);
//
//        new((*fGemTracksArray)[fGemTracksArray->GetEntriesFast()]) BmnGemTrack(tr);
//        //        delete fKalman;
//    }
//
//    CheckSplitting(fGemSeedsArrayMid);
//    for (Int_t i = 0; i < fGemSeedsArrayMid->GetEntriesFast(); ++i) {
//        //        fKalman = new BmnKalmanFilter_tmp();
//        BmnGemTrack* track = (BmnGemTrack*) fGemSeedsArrayMid->At(i);
//        if (track->GetChi2() < 0.0) continue; //split param
//
//        BmnGemTrack tr = *track;
//
//        if (!IsParCorrect(tr.GetParamFirst())) continue;
//        if (!IsParCorrect(tr.GetParamLast())) continue;
//
//        if (tr.GetNHits() < 4) continue;
//
//        TVector3 spirPar = SpiralFit(&tr, fGemHitArray);
//        Float_t R = spirPar.Z();
//        Float_t b = spirPar.Y();
//        Float_t a = spirPar.X();
//        BmnGemStripHit* hit0 = (BmnGemStripHit*) GetHit(tr.GetHitIndex(0));
//        Float_t x0 = hit0->GetX();
//        Float_t y0 = hit0->GetY();
//        Float_t z0 = hit0->GetZ();
//        Float_t Tx = Tan(-a / b);
//        Float_t Ty = tr.GetParamFirst()->GetTy();
//        const Float_t Pxz = 0.0003 * Abs(fField->GetBy(x0, y0, z0)) * R; // Pt
//        if (Abs(Pxz) < 0.00001) continue;
//        const Float_t Pz = Pxz / Sqrt(1 + Sqr(Tx));
//        const Float_t Px = Pz * Tx;
//        const Float_t Py = Pz * Ty;
//        Float_t QP = 1.0 / Sqrt(Px * Px + Py * Py + Pz * Pz);
//
//        FairTrackParam par;
//        par.SetPosition(TVector3(x0, y0, z0));
//        par.SetQp(QP);
//        par.SetTx(Tx);
//        par.SetTy(Ty);
//        tr.SetParamFirst(par);
//        tr.SetChi2(ChiSq(spirPar, &tr, fGemHitArray));
//        if (tr.GetChi2() > kCHI2CUT) tr.SetFlag(kBMNBAD);
//        else tr.SetFlag(kBMNGOOD);
//
//        //        FairTrackParam smoothPar = fKalman->Filtration(&tr, fGemHitArray);
//        //        tr.SetParamFirst(smoothPar);
//
//        new((*fGemTracksArray)[fGemTracksArray->GetEntriesFast()]) BmnGemTrack(tr);
//        //        delete fKalman;
//    }
//
//    CheckSplitting(fGemSeedsArrayLow);
//    for (Int_t i = 0; i < fGemSeedsArrayLow->GetEntriesFast(); ++i) {
//        //        fKalman = new BmnKalmanFilter_tmp();
//        BmnGemTrack* track = (BmnGemTrack*) fGemSeedsArrayLow->At(i);
//        if (track->GetChi2() < 0.0) continue; //split param
//
//        BmnGemTrack tr = *track;
//
//        if (!IsParCorrect(tr.GetParamFirst())) continue;
//        if (!IsParCorrect(tr.GetParamLast())) continue;
//
//        if (tr.GetNHits() < 4) continue;
//
//        TVector3 spirPar = SpiralFit(&tr, fGemHitArray);
//        Float_t R = spirPar.Z();
//        Float_t b = spirPar.Y();
//        Float_t a = spirPar.X();
//        BmnGemStripHit* hit0 = (BmnGemStripHit*) GetHit(tr.GetHitIndex(0));
//        Float_t x0 = hit0->GetX();
//        Float_t y0 = hit0->GetY();
//        Float_t z0 = hit0->GetZ();
//        Float_t Tx = Tan(-a / b);
//        Float_t Ty = tr.GetParamFirst()->GetTy();
//        const Float_t Pxz = 0.0003 * Abs(fField->GetBy(x0, y0, z0)) * R; // Pt
//        //        cout << fField->GetBy(x0, y0, z0) << endl;
//        if (Abs(Pxz) < 0.00001) continue;
//        const Float_t Pz = Pxz / Sqrt(1 + Sqr(Tx));
//        const Float_t Px = Pz * Tx;
//        const Float_t Py = Pz * Ty;
//        Float_t QP = 1.0 / Sqrt(Px * Px + Py * Py + Pz * Pz);
//
//        FairTrackParam par;
//        par.SetPosition(TVector3(x0, y0, z0));
//        par.SetQp(QP);
//        par.SetTx(Tx);
//        par.SetTy(Ty);
//        tr.SetParamFirst(par);
//        tr.SetChi2(ChiSq(spirPar, &tr, fGemHitArray));
//        if (tr.GetChi2() > kCHI2CUT) tr.SetFlag(kBMNBAD);
//        else tr.SetFlag(kBMNGOOD);
//
//        //        FairTrackParam smoothPar = fKalman->Filtration(&tr, fGemHitArray);
//        //        tr.SetParamFirst(smoothPar);
//
//        new((*fGemTracksArray)[fGemTracksArray->GetEntriesFast()]) BmnGemTrack(tr);
//        //        delete fKalman;
//    }

    const Int_t nIter = 4;
    TClonesArray* arr[nIter] = {fGemSeedsArrayBig, fGemSeedsArrayMid1, fGemSeedsArrayMid2, fGemSeedsArrayLow};
    
    for (Int_t iter = 0; iter < nIter; ++iter) {
        CheckSplitting(arr[iter]);
        for (Int_t iTr = 0; iTr < arr[iter]->GetEntriesFast(); ++iTr) {
            BmnGemTrack* track = (BmnGemTrack*) arr[iter]->At(iTr);
            if (track->GetChi2() < 0.0) continue; //split param

            BmnGemTrack tr = *track;

            if (!IsParCorrect(tr.GetParamFirst())) continue;
            if (!IsParCorrect(tr.GetParamLast())) continue;

            if (tr.GetNHits() < 4) continue;

            TVector3 spirPar = SpiralFit(&tr, fGemHitArray);
            Float_t R = spirPar.Z();
            Float_t b = spirPar.Y();
            Float_t a = spirPar.X();
            BmnGemStripHit* hit0 = (BmnGemStripHit*) GetHit(tr.GetHitIndex(0));
            Float_t x0 = hit0->GetX();
            Float_t y0 = hit0->GetY();
            Float_t z0 = hit0->GetZ();
            Float_t Tx = Tan(-a / b);
            Float_t Ty = tr.GetParamFirst()->GetTy();
            const Float_t Pxz = 0.0003 * Abs(fField->GetBy(x0, y0, z0)) * R; // Pt
            if (Abs(Pxz) < 0.00001) continue;
            const Float_t Pz = Pxz / Sqrt(1 + Sqr(Tx));
            const Float_t Px = Pz * Tx;
            const Float_t Py = Pz * Ty;
            Float_t QP = 1.0 / Sqrt(Px * Px + Py * Py + Pz * Pz);

            FairTrackParam par;
            par.SetPosition(TVector3(x0, y0, z0));
            par.SetQp(QP);
            par.SetTx(Tx);
            par.SetTy(Ty);
            tr.SetParamFirst(par);
            tr.SetChi2(ChiSq(spirPar, &tr, fGemHitArray));
            if (tr.GetChi2() > kCHI2CUT) tr.SetFlag(kBMNBAD);
            else tr.SetFlag(kBMNGOOD);
            new((*fGemTracksArray)[fGemTracksArray->GetEntriesFast()]) BmnGemTrack(tr);
        }
    }


    clock_t tFinish = clock();
    cout << "GEM_TRACKING: Number of found tracks: " << fGemTracksArray->GetEntriesFast() << endl;

    workTime += ((Float_t) (tFinish - tStart)) / CLOCKS_PER_SEC;

    cout << "\n====================== GEM track finder exec finished =====================" << endl;

}

void BmnGemTrackFinder::Finish() {
    cout << "GEM tracking is finished" << endl;
}

BmnStatus BmnGemTrackFinder::ConnectNearestSeed(BmnGemTrack* baseSeed, TClonesArray* arr) {

    if (baseSeed->GetChi2() < 0.0) return kBMNERROR;
    FairTrackParam* baseSeedLastPar = baseSeed->GetParamLast();

    Float_t yI = baseSeedLastPar->GetY();
    Float_t zI = baseSeedLastPar->GetZ();
    Float_t xI = baseSeedLastPar->GetX();
    Float_t tyI = baseSeedLastPar->GetTy();
    Float_t txI = baseSeedLastPar->GetTx();

    //needed to get nearest track
    Float_t minDeltaR = 1.2; //best by QA

    BmnGemTrack* minTrackRight = NULL;
    for (Int_t j = 0; j < arr->GetEntriesFast(); ++j) {
        BmnGemTrack* trackJ = (BmnGemTrack*) arr->At(j);
        if (trackJ->GetChi2() < 0.0) continue;
        if (trackJ->IsUsed()) continue;

        FairTrackParam* firstJ = trackJ->GetParamFirst();

        Float_t zJ = firstJ->GetZ();
        Float_t xJ = firstJ->GetX();
        Float_t yJ = firstJ->GetY();
        Float_t tyJ = firstJ->GetTy();
        Float_t txJ = firstJ->GetTx();
        if (zI > zJ) continue;
        const Float_t zMid = zI + (zJ - zI) / 2.0;
        const Float_t yI_zMid = tyI * (zMid - zI) + yI;
        const Float_t yJ_zMid = tyJ * (zMid - zJ) + yJ;
        const Float_t xI_zMid = txI / 2 / zI * (zMid * zMid - zI * zI) + xI;
        const Float_t xJ_zMid = txJ / 2 / zJ * (zMid * zMid - zJ * zJ) + xJ;
        const Float_t r = Sqrt((xI_zMid - xJ_zMid) * (xI_zMid - xJ_zMid) + (yI_zMid - yJ_zMid) * (yI_zMid - yJ_zMid));

        if (r < minDeltaR) {
            minTrackRight = trackJ;
            minDeltaR = r;
        }
    }

    if (minTrackRight != NULL) {

        minTrackRight->SetUsing(kTRUE);
        ConnectNearestSeed(minTrackRight, arr);
        for (Int_t iHit = 0; iHit < minTrackRight->GetNHits(); ++iHit) {
            BmnGemStripHit* hit = (BmnGemStripHit*) GetHit(minTrackRight->GetHitIndex(iHit));
            baseSeed->AddHit(minTrackRight->GetHitIndex(iHit), hit);
            hit->SetUsing(kTRUE);
            //hit->SetType(7);
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
        if (seed->GetChi2() < 0.0) continue;
        if (seed->IsUsed()) continue;
        Short_t minStation = 100;
        for (Int_t iHit = 0; iHit < seed->GetNHits(); ++iHit) {
            BmnGemStripHit* hit = (BmnGemStripHit*) GetHit(seed->GetHitIndex(iHit));
            if (hit->GetStation() < minStation) minStation = hit->GetStation();
        }
        if (minStation <= 2) {
            seed->SetUsing(kTRUE);
            ConnectNearestSeed(seed, arr);
        }
    }
}

//**************Implementation of Lebedev's algorithm for merging*************//

BmnStatus BmnGemTrackFinder::NearestHitMerge(UInt_t station, BmnGemTrack * tr) {
    // First find hit with minimum Z position and build map from Z hit position
    // to track parameter to improve the calculation speed.

    Double_t zMin = 10e10;
    map<Float_t, FairTrackParam> zParamMap;

    for (Int_t hitIdx = 0; hitIdx < fGemHitArray->GetEntriesFast(); ++hitIdx) {
        BmnGemStripHit* hit = (BmnGemStripHit*) GetHit(hitIdx);
        if (hit->IsUsed()) continue;
        if (hit->GetStation() != station) continue;
        if (hit->GetType() == 0) continue; //don't use fakes
        zMin = min(zMin, hit->GetZ());
        zParamMap[hit->GetZ()] = FairTrackParam();
    }

    tr->SetFlag(kBMNGOOD);
    FairTrackParam par;
    if (station != 0)
        par = *(tr->GetParamLast());
    else
        par = *(tr->GetParamFirst());
    // Extrapolate track minimum Z position of hit using magnetic field propagator
    if (fPropagator->TGeoTrackPropagate(&par, zMin, fPDG, NULL, NULL, "field") == kBMNERROR) {
        return kBMNERROR;
    }
    // Extrapolate track parameters to each Z position in the map.
    // This is done to improve calculation speed.
    // In case of planar TOF geometry only 1 track extrapolation is required,
    // since all hits located at the same Z.
    for (map<Float_t, FairTrackParam>::iterator it = zParamMap.begin(); it != zParamMap.end(); it++) {
        (*it).second = par;
        fPropagator->TGeoTrackPropagate(&(*it).second, (*it).first, fPDG, NULL, NULL, "field");
    }

    // Loop over hits
    Float_t minChiSq = 10e10; // minimum chi-square of hit
    BmnHit* minHit = NULL; // Pointer to hit with minimum chi-square
    Float_t minDist = 5;
    Int_t minIdx = 0;
    Float_t dist = 0.0;
    FairTrackParam minPar; // Track parameters for closest hit
    for (Int_t hitIdx = 0; hitIdx < fGemHitArray->GetEntriesFast(); ++hitIdx) {
        BmnGemStripHit* hit = (BmnGemStripHit*) GetHit(hitIdx);
        if (hit->IsUsed()) continue;
        if (hit->GetStation() != station) continue;
        if (hit->GetType() == 0) continue; //don't use fakes
        if (zParamMap.find(hit->GetZ()) == zParamMap.end()) { // This should never happen
            cout << "-E- NearestHitMerge: Z position " << hit->GetZ() << " not found in map. Something is wrong.\n";
        }
        FairTrackParam tpar(zParamMap[hit->GetZ()]);
        Float_t chi = 0.0;

        //        cout << "HIT ID = " << hitIdx << " Hx = " << hit->GetX() << " BEFORE = " << tpar.GetX() << " " << tpar.GetY() << " " << tpar.GetZ() << " " << tpar.GetTx() << " " << tpar.GetTy() << " " << tpar.GetQp() << " chi = " << chi << endl;
        //fUpdate->Update(&tpar, hit, chi); //update by KF
        //        cout << "AFTER = " << tpar.GetX() << " " << tpar.GetY() << " " << tpar.GetZ() << " " << tpar.GetTx() << " " << tpar.GetTy() << " " << tpar.GetQp() << " chi = " << chi << endl;
        dist = Dist(tpar.GetX(), tpar.GetY(), hit->GetX(), hit->GetY());
        if (chi < fChiSqCut && chi < minChiSq && dist < minDist) { // Check if hit is inside validation gate and closer to the track.
            //if (dist < minDist) { // Check if hit is inside validation gate and closer to the track.
            minDist = dist;
            minChiSq = chi;
            minHit = hit;
            minPar = tpar;
            minIdx = hitIdx;
        }
    }

    if (minHit != NULL) { // Check if hit was added
        tr->SetParamLast(minPar);
        tr->SetChi2(tr->GetChi2() + minChiSq);
        minHit->SetUsing(kTRUE);
        tr->AddHit(minIdx, minHit);
        tr->SortHits();
    }
    return kBMNSUCCESS;
}
//****************************************************************************//

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

BmnStatus BmnGemTrackFinder::NearestHitMerge1(UInt_t station, BmnGemTrack* tr) {

    BmnFitNode node;

    FairTrackParam par;

    if (station != 0)
        par = *(tr->GetParamLast());
    else
        par = *(tr->GetParamFirst());

    //    par.Print(); cout << endl;

    if (!IsParCorrect(&par)) return kBMNERROR;

    Float_t minChiSq = 10e10; // minimum chi-square of hit
    Float_t minDist = 5000;
    BmnFitNode minNode;
    BmnHit* minHit = NULL; // Pointer to hit with minimum chi-square
    FairTrackParam minPar; // Track parameters for closest hit
    Int_t minIdx = 0;
    for (Int_t hitIdx = 0; hitIdx < fGemHitArray->GetEntriesFast(); ++hitIdx) {
        BmnGemStripHit* hit = (BmnGemStripHit*) GetHit(hitIdx);
        if (hit->IsUsed()) continue;
        if (hit->GetStation() != station) continue;
        if (hit->GetType() == 0) continue; //don't use fakes
        Float_t chi = 0.0;
        //        if (fKalman->Prediction(&par, hit->GetZ(), node) != kBMNSUCCESS) continue;
        //        if (fKalman->Prediction(&par, hit->GetZ(), node) != kBMNSUCCESS) break;
        //        cout << "HIT INFO: x = " << hit->GetX() << " y = " << hit->GetY() << " z = " << hit->GetZ() << endl;
        //        par.Print();
        //        cout << "BEFORE: HIT ID = " << hitIdx << " Hx = " << hit->GetX() << " " << par.GetX() << " " << par.GetY() << " " << par.GetZ() << " " << par.GetTx() << endl;
        //        if (fKalman->Correction(&par, hit, chi, node) != kBMNSUCCESS) continue;
        //        if (fKalman->Correction(&par, hit, chi, node) != kBMNSUCCESS) break;
        //        par.Print();
        //        cout << "AFTER:                            " << par.GetX() << " " << par.GetY() << " " << par.GetZ() << " " << par.GetTx() << " chi = " << chi << endl;
        //        if (chi < fChiSqCut && chi < minChiSq) { // Check if hit is inside validation gate and closer to the track.
        //        if (chi < minChiSq) { // Check if hit is inside validation gate and closer to the track.
        //            minChiSq = chi;
        //            minHit = hit;
        //            minPar = par;
        //            minIdx = hitIdx;
        //        }
        Float_t dist = Dist(par.GetX(), par.GetY(), hit->GetX(), hit->GetY());
        if (dist < minDist) { // Check if hit is inside validation gate and closer to the track.
            minDist = dist;
            minHit = hit;
            minPar = par;
            minIdx = hitIdx;
            minNode = node;
        }
    }

    if (minHit != NULL) { // Check if hit was added
        tr->SetParamLast(minPar);
        tr->SetChi2(tr->GetChi2() + minChiSq);
        minHit->SetUsing(kTRUE);
        tr->AddHit(minIdx, minHit);
        tr->SortHits();
        //        fKalman->AddFitNode(minNode);
    }

    return kBMNSUCCESS;
}

//Float_t BmnGemTrackFinder::ChiSq(const FairTrackParam* par, const BmnHit* hit) {
//    Float_t dxx = hit->GetDx() * hit->GetDx();
//    Float_t dyy = hit->GetDy() * hit->GetDy();
//    Float_t xmx = hit->GetX() - par->GetX();
//    Float_t ymy = hit->GetY() - par->GetY();
//    Float_t C0 = par->GetCovariance(0, 0);
//    Float_t C1 = par->GetCovariance(0, 1);
//    Float_t C5 = par->GetCovariance(1, 1);
//
//    Float_t norm = dxx * dyy - dxx * C5 - dyy * C0 + C0 * C5 - C1 * C1;
//    if (norm == 0.) norm = 1e-10;
//    return ((xmx * (dyy - C5) + ymy * C1) * xmx + (xmx * C1 + ymy * (dxx - C0)) * ymy) / norm;
//    return 0;
//}
