
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
const Float_t kCHI2CUT = 1000000.0;
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
        Float_t chi2 = 0.0;
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

        Float_t length = 0.0;

        for (Int_t iHit = 0; iHit < nHits; ++iHit) {
            BmnGemStripHit* hit = (BmnGemStripHit*) GetHit(tr.GetHitIndex(iHit));
            Float_t z = hit->GetZ();
            BmnFitNode node;

            fKalman->TGeoTrackPropagate(&par, z, fPDG, F, &length, "field");
            node.SetPredictedParam(&par);
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

        if (fKalman->FitSmooth(&tr, fGemHitArray) == kBMNERROR) continue;
        tr.SetChi2(chi2);

        //        if (tr.GetChi2() / tr.GetNDF() > kCHI2CUT) tr.SetFlag(kBMNBAD);
        //        else tr.SetFlag(kBMNGOOD);
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

Bool_t BmnGemTrackFinder::CalculateParamsByCircle(BmnGemTrack* tr) {
    //Needed for start approximation of track parameters

    BmnGemStripHit* firstHit = (BmnGemStripHit*) fGemHitArray->At(tr->GetHitIndex(0));
    if (!firstHit) return kFALSE;

    TVector3 linePar = LineFit(tr, fGemHitArray);
    TVector3 circPar = CircleBy3Hit(tr, fGemHitArray);

    Float_t R = circPar.Z(); // radius of fit-circle
    Float_t Xc = circPar.X(); // x-coordinate of fit-circle center
    Float_t Zc = circPar.Y(); // z-coordinate of fit-circle center
    const Float_t B = linePar.X(); //angle coefficient for helicoid
    const Int_t nHits = tr->GetNHits();

    Float_t Xmean(0.0), Ymean(0.0), Zmean(0.0); // <Xi> , <Yi> , <Zi>
    Float_t ZXmean(0.0); // <(Zi-Zc)/(Xi-Xc)>
    Float_t ZX2mean(0.0); // <(Zi-Zc)/(Xi-Xc)^2>
    Float_t XZXmean(0.0); // <Xi*(Zi-Zc)/(Xi-Xc)>
    Float_t YZXmean(0.0); // <Yi*(Zi-Zc)/(Xi-Xc)>
    Float_t OneXmean(0.0); // <1/(Xi-Xc)>
    Float_t XOneXmean(0.0); // <Xi/(Xi-Xc)>
    Float_t YOneXmean(0.0); // <Yi/(Xi-Xc)>
    Float_t QPmean(0.0);
    Float_t XQPmean(0.0);
    Float_t YQPmean(0.0);
    Float_t ZXQPmean(0.0);
    Float_t OneXQPmean(0.0);

    //Covariance matrix
    Float_t Cov_X_X(0.0), Cov_X_Y(0.0), Cov_X_Tx(0.0), Cov_X_Ty(0.0), Cov_X_Qp(0.0);
    Float_t Cov_Y_Y(0.0), Cov_Y_Tx(0.0), Cov_Y_Ty(0.0), Cov_Y_Qp(0.0);
    Float_t Cov_Tx_Tx(0.0), Cov_Tx_Ty(0.0), Cov_Tx_Qp(0.0);
    Float_t Cov_Ty_Ty(0.0), Cov_Ty_Qp(0.0);
    Float_t Cov_Qp_Qp(0.0);
    Float_t Q = (tr->GetParamFirst()->GetQp() > 0.0) ? +1.0 : -1.0;
    Float_t S = 0.0003 * (fField->GetBy(firstHit->GetX(), firstHit->GetY(), firstHit->GetZ()));
    Float_t QP = Q / S / Sqrt(R * R + B * B);

    for (Int_t i = 0; i < nHits; ++i) {
        BmnGemStripHit* hit = (BmnGemStripHit*) fGemHitArray->At(tr->GetHitIndex(i));
        Float_t Xi = hit->GetX();
        Float_t Yi = hit->GetY();
        Float_t Zi = hit->GetZ();
        QPmean += QP;
        Xmean += Xi;
        Ymean += Yi;
        Zmean += Zi;
        ZXmean += (Zi - Zc) / (Xi - Xc);
        ZXQPmean += (Zi - Zc) / (Xi - Xc) * QP;
        ZX2mean += (Zi - Zc) / (Xi - Xc) / (Xi - Xc);
        XZXmean += (Zi - Zc) / (Xi - Xc) * Xi;
        YZXmean += (Zi - Zc) / (Xi - Xc) * Yi;
        OneXmean += 1.0 / (Xi - Xc);
        OneXQPmean += 1.0 / (Xi - Xc) * QP;
        XOneXmean += Xi / (Xi - Xc);
        YOneXmean += Yi / (Xi - Xc);
        XQPmean += Xi * QP;
        YQPmean += Yi * QP;

        Cov_X_X += Sqr(Xi);
        Cov_X_Y += Xi * Yi;
        Cov_Y_Y += Sqr(Yi);
        Cov_Tx_Tx += Sqr((Zi - Zc) / (Xi - Xc));
        Cov_Ty_Ty += Sqr(1.0 / (Xi - Xc));
        Cov_Qp_Qp += Sqr(QP);
    }
    Xmean /= nHits;
    Ymean /= nHits;
    Zmean /= nHits;
    ZXmean /= nHits;
    ZX2mean /= nHits;
    XZXmean /= nHits;
    YZXmean /= nHits;
    OneXmean /= nHits;
    XOneXmean /= nHits;
    YOneXmean /= nHits;
    XQPmean /= nHits;
    YQPmean /= nHits;
    QPmean /= nHits;
    ZXQPmean /= nHits;
    OneXQPmean /= nHits;

    Cov_X_X = Cov_X_X / nHits - Sqr(Xmean);
    Cov_X_Y = Cov_X_Y / nHits - Xmean * Ymean;
    Cov_X_Tx = Xmean * ZXmean - XZXmean;
    Cov_X_Ty = 0.0; //B * (Xmean * OneXmean - XOneXmean);
    Cov_Y_Y = Cov_Y_Y / nHits - Sqr(Ymean);
    Cov_Y_Tx = Ymean * ZXmean - YZXmean;
    Cov_Y_Ty = 0.0; //B * (Ymean * OneXmean - YOneXmean);
    Cov_Tx_Ty = B * (ZX2mean - ZXmean * OneXmean);
    Cov_Tx_Tx = Cov_Tx_Tx / nHits - Sqr(ZXmean);
    Cov_Ty_Ty = 0.0; //Sqr(B) * (Cov_Ty_Ty / nHits - OneXmean * OneXmean);
    Cov_Qp_Qp = Cov_Qp_Qp / nHits - Sqr(QPmean);
    Cov_X_Qp = XQPmean - Xmean * QPmean;
    Cov_Y_Qp = YQPmean - Ymean * QPmean;
    Cov_Tx_Qp = -1.0 * ZXQPmean + ZXmean * QPmean;
    Cov_Ty_Qp = 0.0; //B * OneXQPmean - OneXmean * QPmean;

    FairTrackParam par;
    const Float_t cov_const = 1e-15;
    par.SetCovariance(0, 0, Cov_X_X);
    par.SetCovariance(0, 1, Cov_X_Y);
    par.SetCovariance(0, 2, Cov_X_Tx);
    par.SetCovariance(0, 3, Cov_X_Ty);
    par.SetCovariance(0, 4, Cov_X_Qp);
    //    par.SetCovariance(0, 4, cov_const);
    par.SetCovariance(1, 1, Cov_Y_Y);
    par.SetCovariance(1, 2, Cov_Y_Tx);
    par.SetCovariance(1, 3, Cov_Y_Ty);
    par.SetCovariance(1, 4, Cov_Y_Qp);
    //    par.SetCovariance(1, 4, cov_const);
    par.SetCovariance(2, 2, Cov_Tx_Tx);
    par.SetCovariance(2, 3, Cov_Tx_Ty);
    par.SetCovariance(2, 4, Cov_Tx_Qp);
    //    par.SetCovariance(2, 4, cov_const);
    par.SetCovariance(3, 3, Cov_Ty_Ty);
    par.SetCovariance(3, 4, Cov_Ty_Qp);
    //    par.SetCovariance(3, 4, cov_const);
    par.SetCovariance(4, 4, Cov_Qp_Qp);
    //    par.SetCovariance(4, 4, cov_const);

    Float_t fX = firstHit->GetX();
    Float_t fY = firstHit->GetY();
    Float_t fZ = firstHit->GetZ();

    //    Float_t h = (fX < Xc) ? -1.0 : 1.0;
    Float_t h = -1.0;

    Float_t Tx_first = h * (fZ - Zc) / (fX - Xc);
    Float_t Ty_first = B;

    //update for firstParam
    const Float_t PxzFirst = 0.0003 * (fField->GetBy(fX, fY, fZ)) * R; // Pt
    if (Abs(PxzFirst) < 0.00001) return kFALSE;
    const Float_t PzFirst = PxzFirst / Sqrt(1 + Sqr(Tx_first));
    const Float_t PxFirst = PzFirst * Tx_first;
    const Float_t PyFirst = PzFirst * Ty_first;
    Float_t QPFirst = Q / Sqrt(PxFirst * PxFirst + PyFirst * PyFirst + PzFirst * PzFirst);
    par.SetPosition(TVector3(fX, fY, fZ));
    par.SetQp(QPFirst);
    par.SetTx(Tx_first);
    par.SetTy(Ty_first);
    if (!IsParCorrect(&par)) return kFALSE;
    tr->SetParamFirst(par);

    return kTRUE;
}