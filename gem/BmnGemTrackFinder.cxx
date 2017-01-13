
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
//-----------------------------------------

using namespace std;
using namespace TMath;

BmnGemTrackFinder::BmnGemTrackFinder() :
fPDG(211),
fEventNo(0),
fChiSqCut(25.) {
    fKalman = NULL;
    fGemHitArray = NULL;
    fGemTracksArray = NULL;
    fMCTracksArray = NULL;
    fGemSeedsArray = NULL;
    fMCPointsArray = NULL;
    fField = NULL;
    fIsField = kTRUE;
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

    fField = FairRunAna::Instance()->GetField();

    fPropagator = new BmnTrackPropagator();

    cout << "======================== GEM track finder init finished ===================" << endl;
}

void BmnGemTrackFinder::Exec(Option_t* opt) {

    cout << "\n====================== GEM track finder exec started ======================" << endl;
    cout << "\n Event number: " << fEventNo++ << endl;

    clock_t tStart = clock();
    fGemTracksArray->Clear();
    CheckSplitting(fGemSeedsArray);
    for (Int_t iTr = 0; iTr < fGemSeedsArray->GetEntriesFast(); ++iTr) {
        BmnGemTrack* track = (BmnGemTrack*) fGemSeedsArray->At(iTr);

        if (track->GetChi2() < 0.0) continue; //split param

        BmnGemTrack tr = *track;
        const Short_t nHits = tr.GetNHits();
        FairTrackParam par = *(tr.GetParamFirst());

        vector<BmnFitNode> nodes;
        nodes.reserve(nHits);
        Double_t chi2 = 0.0;
        fKalman = new BmnKalmanFilter_tmp();

        vector<Double_t>* F = new vector<Double_t> (25, 0.);
        if (F != NULL) {
            F->assign(25, 0.);
            (*F)[0] = 1.;
            (*F)[6] = 1.;
            (*F)[12] = 1.;
            (*F)[18] = 1.;
            (*F)[24] = 1.;
        }

        Double_t length = 0.0;

        for (Int_t iHit = 0; iHit < nHits; ++iHit) {
            BmnGemStripHit* hit = (BmnGemStripHit*) GetHit(tr.GetHitIndex(iHit));
            Double_t z = hit->GetZ();
            BmnFitNode node;

            if (fIsField)
                fKalman->TGeoTrackPropagate(&par, z, fPDG, F, &length, "field");
            node.SetPredictedParam(&par);
            if (fIsField)
                fKalman->Update(&par, hit, chi2);
            node.SetUpdatedParam(&par);
            node.SetChiSqFiltered(chi2);
            node.SetF(*F);

            nodes.push_back(node);
        }

        tr.SetLength(length);

        delete F;
        tr.SetFitNodes(nodes);
        tr.SetParamFirst(*(nodes[0].GetUpdatedParam()));
        tr.SetParamLast(*(nodes[nodes.size() - 1].GetUpdatedParam()));
        //if (fKalman->FitSmooth(&tr, fGemHitArray) == kBMNERROR) continue;
        tr.SetChi2(chi2);
        tr.SetNDF(nHits - 3);

//        if (tr.GetChi2() / tr.GetNDF() > fChiSqCut) {
//            tr.SetFlag(kBMNBAD);
//            for (Int_t iHit = 0; iHit < tr.GetNHits(); ++iHit)
//                GetHit(tr.GetHitIndex(iHit))->SetUsing(kFALSE);
//        } else tr.SetFlag(kBMNGOOD);

        new((*fGemTracksArray)[fGemTracksArray->GetEntriesFast()]) BmnGemTrack(tr);
        delete fKalman;
    }

    clock_t tFinish = clock();
    cout << "GEM_TRACKING: Number of found tracks: " << fGemTracksArray->GetEntriesFast() << endl;

    workTime += ((Float_t) (tFinish - tStart)) / CLOCKS_PER_SEC;

    cout << "\n====================== GEM track finder exec finished =====================" << endl;

}

void BmnGemTrackFinder::Finish() {
    ofstream outFile;
    outFile.open("QA/timing.txt", ofstream::app);
    outFile << "Track Finder Time: " << workTime << endl;
    cout << "Work time of the GEM track finder: " << workTime << endl;
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