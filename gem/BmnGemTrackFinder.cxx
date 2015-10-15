
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
//-----------------------------------------

using namespace std;
using namespace TMath;

BmnGemTrackFinder::BmnGemTrackFinder() :
fPDG(211),
fEventNo(0),
fChiSqCut(250.) {
    fKalman = NULL;
    fGemHitArray = NULL;
    fGemTracksArray = NULL;
    fMCTracksArray = NULL;
    fGemSeedsArray = NULL;
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

    CheckSplitting();

    for (Int_t i = 0; i < fGemSeedsArray->GetEntriesFast(); ++i) {
        fKalman = new BmnKalmanFilter_tmp();
        BmnGemTrack* track = (BmnGemTrack*) fGemSeedsArray->At(i);
        if (track->GetChi2() < 0.0) continue; //split param

        BmnGemTrack tr = *track;

        if (!IsParCorrect(tr.GetParamFirst())) continue;
        if (!IsParCorrect(tr.GetParamLast())) continue;

        BmnGemStripHit* currHit = NULL;
        for (Int_t iStat = 0; iStat < 12; ++iStat) {
            BmnFitNode node;
            currHit = NULL;
            for (Int_t iHit = 0; iHit < tr.GetNHits(); ++iHit) {
                BmnGemStripHit* hit = (BmnGemStripHit*) GetHit(tr.GetHitIndex(iHit));
                if (hit->GetStation() == iStat) {
                    currHit = hit;
                    break;
                }
            }

            if (currHit != NULL) {
                //calculate fit node params and add it to track
                Float_t chi = 0.0;
                fKalman->Correction(tr.GetParamLast(), currHit, chi, node);
                FairTrackParam* par = tr.GetParamLast();
                node.SetUpdatedParam(par);
                node.SetPredictedParam(par);
                node.SetChiSqFiltered(tr.GetChi2());
                node.SetF_matr(fKalman->Transport(par, currHit->GetZ(), "pol2"));
                fKalman->AddFitNode(node);
            } else {
                //try to find hit on this plane by KF
//                                NearestHitMerge1(iStat, &tr);
//                                NearestHitMerge(iStat, &tr);
            }
        }

        if (tr.GetNHits() < 4) continue;

//        FairTrackParam smoothPar = fKalman->Filtration(&tr, fGemHitArray);
//        tr.SetParamFirst(smoothPar);

        new((*fGemTracksArray)[fGemTracksArray->GetEntriesFast()]) BmnGemTrack(tr);
        delete fKalman;
    }

    clock_t tFinish = clock();
    cout << "GEM_TRACKING: Number of found tracks: " << fGemTracksArray->GetEntriesFast() << endl;

    workTime += ((Float_t) (tFinish - tStart)) / CLOCKS_PER_SEC;

    cout << "\n====================== GEM track finder exec finished =====================" << endl;

}

void BmnGemTrackFinder::Finish() {
    cout << "GEM tracking is finished" << endl;
}

BmnStatus BmnGemTrackFinder::ConnectNearestSeed(BmnGemTrack* baseSeed) {

    if (baseSeed->GetChi2() < 0.0) return kBMNERROR;
    FairTrackParam* baseSeedFirstPar = baseSeed->GetParamFirst();
    FairTrackParam* baseSeedLastPar = baseSeed->GetParamLast();

    //needed to get nearest track
    Float_t minDeltaR = 10;
    Float_t minZ = 10000.0;
    BmnGemTrack* minTrackRight = NULL;
    //    cout << endl << "START!" << endl << endl;
    for (Int_t j = 0; j < fGemSeedsArray->GetEntriesFast(); ++j) {
        BmnGemTrack* trackJ = (BmnGemTrack*) fGemSeedsArray->At(j);
        if (trackJ->GetChi2() < 0.0) continue;
        if (trackJ->IsUsed()) continue;

        FairTrackParam* firstJ = trackJ->GetParamFirst();

        Float_t yI = baseSeedLastPar->GetY();
        Float_t zI = baseSeedLastPar->GetZ();
        Float_t xI = baseSeedLastPar->GetX();
        Float_t tyI = baseSeedLastPar->GetTy();
        Float_t txI = baseSeedLastPar->GetTx();
        Float_t zJ = firstJ->GetZ();
        Float_t xJ = firstJ->GetX();
        Float_t yJ = firstJ->GetY();
        Float_t tyJ = firstJ->GetTy();
        Float_t txJ = firstJ->GetTx();
        if (zI >= zJ) continue;
        const Float_t zMid = zI + (zJ - zI) / 2.0;
        const Float_t yI_zMid = tyI * (zMid - zI) + yI;
        const Float_t yJ_zMid = tyJ * (zMid - zJ) + yJ;
        const Float_t xI_zMid = txI / 2 / zI * (zMid * zMid - zI * zI) + xI;
        const Float_t xJ_zMid = txJ / 2 / zJ * (zMid * zMid - zJ * zJ) + xJ;
        const Float_t r = Sqrt((xI_zMid - xJ_zMid) * (xI_zMid - xJ_zMid) + (yI_zMid - yJ_zMid) * (yI_zMid - yJ_zMid));

        //if (zJ > minZ) continue;
        if (r < minDeltaR) {
            minTrackRight = trackJ;
            minDeltaR = r;
            minZ = zJ;
        }
    }

    if (minTrackRight != NULL) {
        minTrackRight->SetUsing(kTRUE);
        ConnectNearestSeed(minTrackRight);
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

BmnStatus BmnGemTrackFinder::CheckSplitting() {

    for (Int_t i = 0; i < fGemSeedsArray->GetEntriesFast(); ++i) {
        BmnGemTrack* seed = (BmnGemTrack*) fGemSeedsArray->At(i);
        if (seed->GetChi2() < 0.0) continue;
        if (seed->IsUsed()) continue;
        Short_t minStation = 100;
        for (Int_t iHit = 0; iHit < seed->GetNHits(); ++iHit) {
            BmnGemStripHit* hit = (BmnGemStripHit*) GetHit(seed->GetHitIndex(iHit));
            if (hit->GetStation() < minStation) minStation = hit->GetStation();
        }
        if (minStation <= 2) {
            seed->SetUsing(kTRUE);
            ConnectNearestSeed(seed);
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
        if (fKalman->Prediction(&par, hit->GetZ(), node) != kBMNSUCCESS) break;
        //        cout << "HIT INFO: x = " << hit->GetX() << " y = " << hit->GetY() << " z = " << hit->GetZ() << endl;
        //        par.Print();
        //        cout << "BEFORE: HIT ID = " << hitIdx << " Hx = " << hit->GetX() << " " << par.GetX() << " " << par.GetY() << " " << par.GetZ() << " " << par.GetTx() << endl;
        //        if (fKalman->Correction(&par, hit, chi, node) != kBMNSUCCESS) continue;
        if (fKalman->Correction(&par, hit, chi, node) != kBMNSUCCESS) break;
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
        fKalman->AddFitNode(minNode);
    }

    return kBMNSUCCESS;
}

Float_t BmnGemTrackFinder::ChiSq(const FairTrackParam* par, const BmnHit* hit) {
    Float_t dxx = hit->GetDx() * hit->GetDx();
    Float_t dyy = hit->GetDy() * hit->GetDy();
    Float_t xmx = hit->GetX() - par->GetX();
    Float_t ymy = hit->GetY() - par->GetY();
    Float_t C0 = par->GetCovariance(0, 0);
    Float_t C1 = par->GetCovariance(0, 1);
    Float_t C5 = par->GetCovariance(1, 1);

    Float_t norm = dxx * dyy - dxx * C5 - dyy * C0 + C0 * C5 - C1 * C1;
    if (norm == 0.) norm = 1e-10;
    return ((xmx * (dyy - C5) + ymy * C1) * xmx + (xmx * C1 + ymy * (dxx - C0)) * ymy) / norm;
    return 0;
}

Bool_t BmnGemTrackFinder::IsParCorrect(const FairTrackParam* par) {
    const Float_t maxSlope = 5.;
    const Float_t minSlope = 1e-10;
    const Float_t maxQp = 1000.; // p = 10 MeV

    if (abs(par->GetTx()) > maxSlope || abs(par->GetTy()) > maxSlope || abs(par->GetTx()) < minSlope || abs(par->GetTy()) < minSlope || abs(par->GetQp()) > maxQp) return kFALSE;
    if (isnan(par->GetX()) || isnan(par->GetY()) || isnan(par->GetTx()) || isnan(par->GetTy()) || isnan(par->GetQp())) return kFALSE;

    return kTRUE;
}