#include "TProfile.h"
#include <TMath.h>
#include "TCanvas.h"
#include "BmnGemSeedFinder.h"

static Float_t workTime = 0.0;
//-----------------------------------------
const Float_t thresh = 0.7; // threshold for efficiency calculation (70%)

map<ULong_t, Int_t> addresses; // map for calculating addresses of hits in histogram {x/R, y/R}
const UInt_t kNHITSFORSEED = 7; // we use for seeds only kNHITSFORSEED hits
const UInt_t kMAXSTATIONFORSEED = 12; // we start to search seeds only from stations in range from 0 up to kMAXSTATIONFORSEED

using std::cout;
using namespace TMath;

BmnGemSeedFinder::BmnGemSeedFinder() : fEventNo(0) {

    fUseLorentz = kFALSE;
    fGemHitsArray = NULL;
    fGemSeedsArray = NULL;
    fHitsBranchName = "BmnGemStripHit";
    fSeedsBranchName = "BmnGemTrack";
}

BmnGemSeedFinder::~BmnGemSeedFinder() {
}

InitStatus BmnGemSeedFinder::Init() {

    cout << "======================== Seed finder init started =========================" << endl;

    //Get ROOT Manager
    FairRootManager* ioman = FairRootManager::Instance();
    if (NULL == ioman) {
        Fatal("Init", "FairRootManager is not instantiated");
    }

    fGemHitsArray = (TClonesArray*) ioman->GetObject(fHitsBranchName); //in
    if (fGemHitsArray == NULL) {
        cout << "ERROR: No input hits array" << endl;
        return kERROR;
    }
    fGemSeedsArray = new TClonesArray(fSeedsBranchName, 100); //out
    ioman->Register("BmnGemSeeds", "GEM", fGemSeedsArray, kTRUE);


    fMCTracksArray = (TClonesArray*) ioman->GetObject("MCTrack");
    fMCPointsArray = (TClonesArray*) ioman->GetObject("StsPoint");

    fNBins = 10000;
    fMin = -1.0;
    fMax = -fMin;
    fWidth = (fMax - fMin) / fNBins;

    cout << "======================== Seed finder init finished ========================" << endl;
}

void BmnGemSeedFinder::Exec(Option_t* opt) {

    cout << "\n======================== GEM seed finder exec started =====================\n" << endl;
    cout << "Event number: " << fEventNo++ << endl;

    clock_t tStart = clock();
    fGemSeedsArray->Clear();
    addresses.clear();

    if (fUseLorentz) {
        FillAddrWithLorentz(0.5);
    } else {
        FillAddr();
    }

    DoSeeding();
    for (Int_t iHit = 0; iHit < fGemHitsArray->GetEntriesFast(); ++iHit)
        GetHit(iHit)->SetUsing(kFALSE);
    cout << "\nGEM_SEEDING: Number of found seeds: " << fGemSeedsArray->GetEntriesFast() << endl;

    clock_t tFinish = clock();
    workTime += ((Float_t) (tFinish - tStart)) / CLOCKS_PER_SEC;

    cout << "\n======================== Seed finder exec finished ========================" << endl;

}

BmnStatus BmnGemSeedFinder::DoSeeding() {

    for (Int_t i = 0; i < kMAXSTATIONFORSEED; ++i) {
        for (Int_t j = 13; j < 14; ++j)
            FindSeeds(i, j, kTRUE); // from station #i, in gate = 2 * j + 1, only hits presented in every station 
        for (Int_t j = 13; j < 14; ++j)
            FindSeeds(i, j, kFALSE); // from station #i, in gate = 2 * j + 1
    }
    return kBMNSUCCESS;
}

void BmnGemSeedFinder::Finish() {
    addresses.clear();
}

// ========================================================== //
//                                                            //
//                Search track-candidates in                  //
//            2D histogram for {x/R, y/R} - space.            //
//                 R = Sqrt(x^2 + y^2 + z^2)                  //
//                                                            //
// ========================================================== //

void BmnGemSeedFinder::FindSeeds(Int_t startStation, Int_t gate, Bool_t isIdeal) {

    //search only kNHITSFORSEED first hits of track
    //search in left and right directions

    Int_t trCntr = 0;
    trCntr += SearchTrackCandidates(startStation, gate, isIdeal, kTRUE);
    trCntr += SearchTrackCandidates(startStation, gate, isIdeal, kFALSE);

    //    cout << "GEM_SEEDING: Number of candidates ( Stat = " << startStation << ", gate = " << gate * 2 + 1 << ", isIdeal = " << isIdeal << "): " << trCntr << endl;
}

UInt_t BmnGemSeedFinder::SearchTrackCandidates(Int_t startStation, Int_t gate, Bool_t isIdeal, Bool_t isLeft) {
    UInt_t trCntr = 0;
    for (Int_t iHit = 0; iHit < fGemHitsArray->GetEntriesFast(); ++iHit) {

        BmnGemStripHit* hit = GetHit(iHit);

        if (hit->IsUsed()) continue;
        if (startStation != hit->GetStation()) continue;

        BmnGemTrack trackCand;

        Int_t xAddr = hit->GetXaddr();
        Int_t yAddr = hit->GetYaddr();

        Int_t dist = 0;
        Int_t maxDist = 0;
        Int_t hitCntr = 0;
        Int_t startBin = 0;
        Int_t prevStation = startStation - 1; //number of starting station
        if (isLeft) { //search track-candidate in left direction
            for (Int_t i = xAddr; i > 0; i--) {
                SearchTrackCandInLine(i, yAddr, &trackCand, &hitCntr, &maxDist, &dist, &startBin, &prevStation, gate, isIdeal);
                if ((hitCntr > 1) && Abs(i - startBin) > 4 * maxDist) break; //condition to finish search is dist < 3 * MaxDist
                if (hitCntr >= kNHITSFORSEED) break;
            }
        } else { //search track-candidate in right direction
            for (Int_t i = xAddr; i < fNBins; ++i) {
                SearchTrackCandInLine(i, yAddr, &trackCand, &hitCntr, &maxDist, &dist, &startBin, &prevStation, gate, isIdeal);
                if ((hitCntr > 1) && Abs(i - startBin) > 4 * maxDist) break; //condition to finish search is dist < 3 * MaxDist
                if (hitCntr >= kNHITSFORSEED) break;
            }
        }

        trackCand.SortHits();
        const Int_t minNHitsForFit = 3; //kNHITSFORSEED;
        if (trackCand.GetNHits() < minNHitsForFit) { // don't fit track by circle with less then 4 hits
            for (Int_t i = 0; i < trackCand.GetNHits(); ++i)
                GetHit(trackCand.GetHitIndex(i))->SetUsing(kFALSE);
            continue;
        }
        TVector3 circPar = CircleFit(&trackCand);
        //        TVector3 circPar = CircleBy3Hit(&trackCand);
        TVector3 linePar = LineFit(&trackCand);
        if (circPar.Z() < 0.00001) continue;
        trCntr++;
        trackCand.SortHits();

        if (CalculateTrackParams(&trackCand, circPar, linePar)) {
            new((*fGemSeedsArray)[fGemSeedsArray->GetEntriesFast()]) BmnGemTrack(trackCand);
        }
    }
    return trCntr;
}

void BmnGemSeedFinder::SearchTrackCandInLine(const Int_t i, const Int_t y, BmnGemTrack* tr, Int_t* hitCntr, Int_t* maxDist, Int_t* dist, Int_t* startBin, Int_t* prevStation, Int_t gate, Bool_t isIdeal) {

    BmnGemStripHit* hit = NULL;

    for (Int_t j = y - gate; j <= y + gate; ++j) {
        ULong_t addr = j * fNBins + i;
        if (addresses.find(addr) == addresses.end()) continue;
        hit = GetHit(addresses.find(addr)->second);

        if (hit->IsUsed()) continue;
        if (isIdeal) {
            if (hit->GetStation() != (*prevStation) + 1) {
                continue;
            }
        } else {
            if (hit->GetStation() <= (*prevStation)) {
                continue;
            }
        }
        (*hitCntr)++;
        (*prevStation) = hit->GetStation();
        if ((*hitCntr) != 1) {
            (*dist) = Abs(hit->GetXaddr() - (*startBin));
            if ((*dist) > (*maxDist)) (*maxDist) = (*dist);
        }
        (*startBin) = hit->GetXaddr();
        hit->SetUsing(kTRUE);
        //hit->SetChange(kTRUE);
        tr->AddHit(addresses.find(addr)->second, hit);
    }
}

Bool_t BmnGemSeedFinder::CalculateTrackParams(BmnGemTrack* tr, TVector3 circPar, TVector3 linePar) {
    //Needed for start approximation of track parameters

    Float_t R = circPar.Z(); // radius of fit-circle
    Float_t Xc = circPar.X(); // x-coordinate of fit-circle center
    Float_t Zc = circPar.Y(); // z-coordinate of fit-circle center
    fField = FairRunAna::Instance()->GetField();
    const Int_t nHits = tr->GetNHits();
    BmnGemStripHit* lastHit = GetHit(tr->GetHitIndex(nHits - 1));
    BmnGemStripHit* firstHit = GetHit(tr->GetHitIndex(0));
    if (!firstHit || !lastHit) return kFALSE;

    const Float_t B = linePar.X(); //angle coefficient for helicoid

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
    Float_t Q = 1.0;
    Float_t S = 0.0003 * Abs(fField->GetBy(firstHit->GetX(), firstHit->GetY(), firstHit->GetZ()));
    Float_t QP = Q / S / Sqrt(R * R + B * B);

    for (Int_t i = 0; i < nHits; ++i) {
        BmnGemStripHit* hit = GetHit(tr->GetHitIndex(i));
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

    Float_t lX = lastHit->GetX();
    Float_t lY = lastHit->GetY();
    Float_t lZ = lastHit->GetZ();

    Float_t fX = firstHit->GetX();
    Float_t fY = firstHit->GetY();
    Float_t fZ = firstHit->GetZ();

    //    Float_t h = (fX < Xc) ? 1.0 : -1.0;
    Float_t h = -1.0;

    Float_t Tx_first = h * (fZ - Zc) / (fX - Xc);
    Float_t Tx_last = h * (lZ - Zc) / (lX - Xc);
    Float_t Ty_last = B; // / (lX - Xc);
    Float_t Ty_first = B; // / (fX - Xc);

    par.SetPosition(TVector3(lX, lY, lZ));
    par.SetTx(Tx_last);
    par.SetTy(Ty_last); //par.SetTy(-B / (lX - Xc));
    const Float_t PxzLast = 0.0003 * Abs(fField->GetBy(lX, lY, lZ)) * R; // Pt
    //        const Float_t PxzLast = 0.0003 * fField->GetBy(lX, lY, lZ) * R; // Pt
    if (Abs(PxzLast) < 0.00001) return kFALSE;
    const Float_t PzLast = PxzLast / Sqrt(1 + Sqr(Tx_last));
    const Float_t PxLast = PzLast * Tx_last;
    const Float_t PyLast = PzLast * Ty_last;
    Float_t QPLast = 1.0 / Sqrt(PxLast * PxLast + PyLast * PyLast + PzLast * PzLast);
    par.SetQp(QPLast);
    tr->SetParamLast(par);
    if (!IsParCorrect(&par)) return kFALSE;
    //    par.Print();

    //update for firstParam
    const Float_t PxzFirst = 0.0003 * Abs(fField->GetBy(fX, fY, fZ)) * R; // Pt
    if (Abs(PxzFirst) < 0.00001) return kFALSE;
    const Float_t PzFirst = PxzFirst / Sqrt(1 + Sqr(Tx_first));
    const Float_t PxFirst = PzFirst * Tx_first;
    const Float_t PyFirst = PzFirst * Ty_first;
    Float_t QPFirst = 1.0 / Sqrt(PxFirst * PxFirst + PyFirst * PyFirst + PzFirst * PzFirst);
    par.SetPosition(TVector3(fX, fY, fZ));
    par.SetQp(QPFirst);
    par.SetTx(Tx_first);
    par.SetTy(Ty_first); //par.SetTy(-B / (firstHit->GetX() - Xc));
    tr->SetParamFirst(par);
    if (!IsParCorrect(&par)) return kFALSE;
    //    par.Print();


    return kTRUE;
}

TVector3 BmnGemSeedFinder::CircleBy3Hit(BmnGemTrack* track) {
    const Float_t nHits = track->GetNHits();
    if (nHits != 3) return TVector3(0.0, 0.0, 0.0);

    Float_t x1 = GetHit(track->GetHitIndex(0))->GetX();
    Float_t z1 = GetHit(track->GetHitIndex(0))->GetZ();
    Float_t x2 = GetHit(track->GetHitIndex(1))->GetX();
    Float_t z2 = GetHit(track->GetHitIndex(1))->GetZ();
    Float_t x3 = GetHit(track->GetHitIndex(2))->GetX();
    Float_t z3 = GetHit(track->GetHitIndex(2))->GetZ();

    Float_t x1_2 = x1 * x1;
    Float_t z1_2 = z1 * z1;
    Float_t x2_2 = x2 * x2;
    Float_t z2_2 = z2 * z2;
    Float_t x3_2 = x3 * x3;
    Float_t z3_2 = z3 * z3;

    Float_t B = ((x1 - x3) * (x2_2 + z2_2) + (x2 - x1) * (x3_2 + z3_2) + (x3 - x2) * (x1_2 + z1_2)) / (x1 * (z3 - z2) + x2 * (z1 - z3) + x3 * (z2 - z1));
    Float_t A = ((x2_2 + z2_2) - (x1_2 + z1_2) - B * (z1 - z2)) / (x1 - x2);
    Float_t C = -x1_2 - z1_2 - A * x1 - B * z1;

    Float_t Xc = -A / 2;
    Float_t Zc = -B / 2;
    Float_t R = Sqrt(A * A + B * B - 4 * C) / 2;

    return TVector3(Xc, Zc, R);

}

TVector3 BmnGemSeedFinder::CircleFit(BmnGemTrack* track) {

    Float_t Xi, Yi; //centered coordinates
    Float_t Mx, My; // 1-st momentum
    Float_t Ri; // radius
    Float_t Mr, Mxy, Mxx, Myy, Mxr, Myr, Mrr, Mxr2, Myr2, Cov_xy;
    const Float_t C = 3; // parameter for robust fitting (3*Sigma rule)
    Float_t Wi = 1.0; // weight for robust approach // In first approach it's equal 1.0
    Float_t Di = 0.0; //distance between circle and point
    Float_t Sig = 1.0;
    Float_t thresh = C * Sig; // threshold for weights calculating
    Float_t Sw = 0.0; // sum of weights
    Float_t Swd = 0.0; // sum of (w * d^2)
    Float_t A0, A1, A2, A22;
    Float_t GAM, DET;
    Float_t Xc, Yc, R;
    Float_t XcPrev = 1000000.0, YcPrev = 1000000.0; // coordinates of circle center on previous iteration
    Float_t Rprev = 1000000.0; //radius of circle on previous iteration

    const Float_t nHits = track->GetNHits();

    //=============== first approximation with equal weights ===============//
    Mx = My = Mxx = Myy = Mxy = Mxr = Myr = Mrr = 0.0;

    for (Int_t i = 0; i < nHits; ++i) {
        BmnGemStripHit* hit = GetHit(track->GetHitIndex(i));
        if (!(hit->IsUsed())) continue;
        Mx += Wi * hit->GetX();
        My += Wi * hit->GetZ();
        Sw += Wi;
    }
    Mx /= Sw;
    My /= Sw;

    // computing moments
    for (Int_t i = 0; i < nHits; ++i) {
        BmnGemStripHit* hit = GetHit(track->GetHitIndex(i));
        if (!(hit->IsUsed())) continue;
        Xi = hit->GetX() - Mx;
        Yi = hit->GetZ() - My;
        Ri = Xi * Xi + Yi * Yi;

        Mxy += Wi * Xi * Yi;
        Mxx += Wi * Xi * Xi;
        Myy += Wi * Yi * Yi;
        Mxr += Wi * Xi * Ri;
        Myr += Wi * Yi * Ri;
        Mrr += Wi * Ri * Ri;
    }
    Mxx /= Sw;
    Myy /= Sw;
    Mxy /= Sw;
    Mxr /= Sw;
    Myr /= Sw;
    Mrr /= Sw;

    // computing the coefficients of the characteristic polynomial

    Mr = Mxx + Myy;
    Cov_xy = Mxx * Myy - Mxy * Mxy;
    Mxr2 = Mxr * Mxr;
    Myr2 = Myr * Myr;

    A2 = 4.0 * Cov_xy - 3.0 * Mr * Mr - Mrr;
    A1 = Mrr * Mr + 4.0 * Cov_xy * Mr - Mxr2 - Myr2 - Mr * Mr * Mr;
    A0 = Mxr2 * Myy + Myr2 * Mxx - Mrr * Cov_xy - 2.0 * Mxr * Myr * Mxy + Mr * Mr * Cov_xy;
    A22 = A2 + A2;

    Float_t x = NewtonSolver(A0, A1, A2, A22);

    // computing the circle parameters (coordinates of center and radius)
    GAM = -Mr - x - x;
    DET = x * x - x * Mr + Cov_xy;
    Xc = (Mxr * (Myy - x) - Myr * Mxy) / DET / 2.;
    Yc = (Myr * (Mxx - x) - Mxr * Mxy) / DET / 2.;
    R = sqrt(Xc * Xc + Yc * Yc - GAM);

    Xc += Mx;
    Yc += My;

    //=============== end of first approx. with equal weights ================//

    for (Int_t i = 0; i < nHits; ++i) {
        BmnGemStripHit* hit = GetHit(track->GetHitIndex(i));
        if (!(hit->IsUsed())) continue;
        Xi = hit->GetX();
        Yi = hit->GetZ();
        Di = Dist(Xi, Yi, Xc, Yc) - R;
        Swd += Wi * Sqr(Di);
    }

    Sig = Sqrt(Swd / Sw);
    thresh = C * Sig;
    UInt_t cntr = 0; // tmp counter 

    //================= robust procedure with Tukey weights =================//
    while ((Abs(Xc - XcPrev) / XcPrev > 0.001 || Abs(Yc - YcPrev) / YcPrev > 0.001 || Abs(R - Rprev) / Rprev > 0.001) && (cntr < 10)) {
        ++cntr;
        Mx = My = Mxx = Myy = Mxy = Mxr = Myr = Mrr = 0.0;
        Sw = Swd = 0.0;

        for (Int_t i = 0; i < nHits; ++i) {
            BmnGemStripHit* hit = GetHit(track->GetHitIndex(i));
            if (!(hit->IsUsed())) continue;
            Di = Dist(hit->GetX(), hit->GetZ(), Xc, Yc) - R;
            if (Abs(Di) <= thresh) {
                Wi = Sqr(1 - Sqr(Di / thresh));
            } else {
                Wi = 0.0;
                hit->SetUsing(kFALSE);
                continue;
            }
            Mx += Wi * hit->GetX();
            My += Wi * hit->GetZ();
            Sw += Wi;
        }

        Mx /= Sw;
        My /= Sw;

        // computing moments (note: all moments are normed, i.e. divided by N)

        for (Int_t i = 0; i < nHits; ++i) {
            BmnGemStripHit* hit = GetHit(track->GetHitIndex(i));
            if (!(hit->IsUsed())) continue;
            Xi = hit->GetX() - Mx;
            Yi = hit->GetZ() - My;
            Ri = Xi * Xi + Yi * Yi;

            Mxy += Wi * Xi * Yi;
            Mxx += Wi * Xi * Xi;
            Myy += Wi * Yi * Yi;
            Mxr += Wi * Xi * Ri;
            Myr += Wi * Yi * Ri;
            Mrr += Wi * Ri * Ri;
        }
        Mxx /= Sw;
        Myy /= Sw;
        Mxy /= Sw;
        Mxr /= Sw;
        Myr /= Sw;
        Mrr /= Sw;

        // computing the coefficients of the characteristic polynomial

        Mr = Mxx + Myy;
        Cov_xy = Mxx * Myy - Mxy * Mxy;
        Mxr2 = Mxr * Mxr;
        Myr2 = Myr * Myr;

        A2 = 4.0 * Cov_xy - 3.0 * Mr * Mr - Mrr;
        A1 = Mrr * Mr + 4.0 * Cov_xy * Mr - Mxr2 - Myr2 - Mr * Mr * Mr;
        A0 = Mxr2 * Myy + Myr2 * Mxx - Mrr * Cov_xy - 2.0 * Mxr * Myr * Mxy + Mr * Mr * Cov_xy;
        A22 = A2 + A2;

        Float_t x = NewtonSolver(A0, A1, A2, A22);
        // computing the circle parameters (coordinates of center and radius)
        GAM = -Mr - x - x;
        DET = x * x - x * Mr + Cov_xy;
        Xc = (Mxr * (Myy - x) - Myr * Mxy) / DET / 2.0;
        Yc = (Myr * (Mxx - x) - Mxr * Mxy) / DET / 2.0;
        R = Sqrt(Xc * Xc + Yc * Yc - GAM);

        Xc += Mx;
        Yc += My;

        for (Int_t i = 0; i < nHits; ++i) {
            BmnGemStripHit* hit = GetHit(track->GetHitIndex(i));
            if (!(hit->IsUsed())) continue;
            Di = Dist(hit->GetX(), hit->GetZ(), Xc, Yc) - R;
            Swd += Wi * Sqr(Di);
        }

        Sig = Swd / Sw;
        thresh = C * Sig;
        XcPrev = Xc;
        YcPrev = Yc;
        Rprev = R;
    }

    Float_t chi2 = 0.0;
    Int_t nReal = 0;
    Float_t dX, dY, dR, sum = 0.0;
    for (Int_t i = 0; i < nHits; ++i) {
        BmnGemStripHit* hit = GetHit(track->GetHitIndex(i));
        if (!(hit->IsUsed())) continue;
        chi2 += Sqr(R - Dist(hit->GetX(), hit->GetZ(), Xc, Yc)) / R;
        dX = hit->GetX() - Xc;
        dY = hit->GetZ() - Yc;
        dR = Sqrt(dX * dX + dY * dY) - R;
        sum += dR * dR;
        nReal++;
    }
    Float_t sigma = Sqrt(sum / nReal);

    chi2 *= nReal;
    track->SetChi2(chi2);
    track->SetNDF(nReal - 1);
    //    if (chi2 > ChisquareQuantile(0.95, nReal - 1)) {
    //        for (Int_t i = 0; i < nHits; ++i) {
    //            BmnGemStripHit* hit = GetHit(track->GetHitIndex(i));
    //            hit->SetUsing(kFALSE);
    //        }
    //        return TVector3(0.0, 0.0, 0.0);
    //    }

    //    cout << "sigm = " << sigma << " chi2 = " << chi2 << " nReal = " << nReal << " R = " << R << " arb.error = " << sigma / R * 100.0 << endl;
    if (sigma / R * 100.0 > 1.0) { //test check. RMS <= 2% of Radius
        for (Int_t i = 0; i < nHits; ++i) {
            BmnGemStripHit* hit = GetHit(track->GetHitIndex(i));
            hit->SetUsing(kFALSE);
        }
        return TVector3(0.0, 0.0, 0.0);
    } else {
        return TVector3(Xc, Yc, R);
    }
}

TVector3 BmnGemSeedFinder::LineFit(BmnGemTrack* track) {

    //Least Square Method//
    Float_t Zi = 0.0, Yi = 0.0; // coordinates of current track point
    Float_t a = 0.0, b = 0.0; // parameters of line: y = a * z + b
    Float_t SumZ = 0.0, SumY = 0.0, SumZY = 0.0, SumZ2 = 0.0;
    const Float_t nHits = track->GetNHits();
    for (Int_t i = 0; i < nHits; ++i) {
        BmnGemStripHit* hit = GetHit(track->GetHitIndex(i));
        Zi = hit->GetZ();
        Yi = hit->GetY();
        SumZ += Zi;
        SumY += Yi;
        SumZY += Zi * Yi;
        SumZ2 += Sqr(Zi);
    }

    a = (nHits * SumZY - SumZ * SumY) / (nHits * SumZ2 - Sqr(SumZ));
    b = (SumY - a * SumZ) / nHits;

    return TVector3(a, b, 0.0);

}

Float_t BmnGemSeedFinder::Dist(Float_t x1, Float_t y1, Float_t x2, Float_t y2) {
    if (Sqr(x1 - x2) + Sqr(y1 - y2) <= 0.0) {
        return 0.0;
    } else {
        return Sqrt(Sqr(x1 - x2) + Sqr(y1 - y2));
    }
}

Float_t BmnGemSeedFinder::Sqr(Float_t x) {
    return x * x;
}

BmnGemStripHit* BmnGemSeedFinder::GetHit(Int_t i) {
    BmnGemStripHit* hit = (BmnGemStripHit*) fGemHitsArray->At(i);
    if (!hit) cout << "-W- Wrong attempting to get hit number " << i << " from fGemHitsArray, which contains " << fGemHitsArray->GetEntriesFast() << " elements" << endl;
    return hit;
}

Float_t BmnGemSeedFinder::NewtonSolver(Float_t A0, Float_t A1, Float_t A2, Float_t A22) {

    Double_t Dy = 0.0;
    Double_t xnew = 0.0;
    Double_t ynew = 0.0;
    Double_t yold = 1e+11;
    Double_t xold = 0.0;
    const Double_t eps = 1e-12;
    Int_t iter = 0;
    const Int_t iterMax = 20;
    do {
        ynew = A0 + xnew * (A1 + xnew * (A2 + 4.0 * xnew * xnew));
        if (fabs(ynew) > fabs(yold)) {
            xnew = 0.0;
            break;
        }
        Dy = A1 + xnew * (A22 + 16.0 * xnew * xnew);
        xold = xnew;
        xnew = xold - ynew / Dy;
        iter++;
    } while (Abs((xnew - xold) / xnew) > eps && iter < iterMax);

    if (iter == iterMax - 1) {
        xnew = 0.0;
    }

    return xnew;
}

void BmnGemSeedFinder::FillAddr() {
    //Needed for searching seeds by addresses 
    for (Int_t hitIdx = 0; hitIdx < fGemHitsArray->GetEntriesFast(); ++hitIdx) {
        BmnGemStripHit* hit = GetHit(hitIdx);
        if (hit->IsUsed()) continue; //Don't use used hits
        if (hit->GetStation() > kMAXSTATIONFORSEED + kNHITSFORSEED) continue;
        //if (hit->GetRefIndex() < 0) continue; //FIXME!!! Now only for test! (Excluding fake hits) 
        if (hit->GetType() == 0) continue; //Don't use fakes
        const Float_t R = Sqrt(Sqr(hit->GetX()) + Sqr(hit->GetY()) + Sqr(hit->GetZ()));
        //const Float_t R = hit->GetZ(); //Test for different type of transformation
        const Float_t newX = hit->GetX() / R;
        const Float_t newY = hit->GetY() / R;
        //        xRyR->Fill(newX, newY);
        Int_t xAddr = ceil((newX - fMin) / fWidth);
        Int_t yAddr = ceil((newY - fMin) / fWidth);
        ULong_t addr = yAddr * fNBins + xAddr;
        hit->SetAddr(addr);
        hit->SetXaddr(xAddr);
        hit->SetYaddr(yAddr);
        hit->SetFlag(kFALSE);
        addresses.insert(pair<ULong_t, Int_t > (addr, hitIdx));
    }
}

void BmnGemSeedFinder::FillAddrWithLorentz(Float_t sigma_x) {
    //Needed for searching seeds by addresses 
    Float_t sigma_x2 = 0.01 * 0.01;
    TH2F* lorentz = new TH2F("lorentz", "lorentz", fNBins, fMin, fMax, fNBins / 2, fMin, fMax);
    for (Int_t j = 0; j < fNBins; ++j) {
        for (Int_t iHit = 0; iHit < fGemHitsArray->GetEntriesFast(); ++iHit) {
            BmnGemStripHit* hit0 = GetHit(iHit);
            if (hit0->GetStation() > kMAXSTATIONFORSEED + kNHITSFORSEED) continue;
            //            if (hit0->GetType() == 0) continue;
            if (hit0->GetRefIndex() < 0) continue; //FIXME!!! Now only for test! (Excluding fake hits) 
            if (hit0->GetYaddr() != j) continue;
            const Float_t R = Sqrt(Sqr(hit0->GetX()) + Sqr(hit0->GetY()) + Sqr(hit0->GetZ()));
            const Float_t x = hit0->GetX() / R;
            const Float_t y = hit0->GetY() / R;
            for (Int_t i = 0; i < fNBins; ++i) {
                Float_t pot = sigma_x2 / (sigma_x2 + Sqr(x - lorentz->GetXaxis()->GetBinCenter(i)));
                lorentz->Fill(lorentz->GetXaxis()->GetBinCenter(i), lorentz->GetYaxis()->GetBinCenter(j / 2), pot);
            }
        }
    }

    for (Int_t j = 0; j < fNBins / 2; ++j) {
        for (Int_t i = 0; i < fNBins; ++i) {
            if (lorentz->GetBinContent(i, j) <= 1.5) lorentz->SetBinContent(i, j, 0.0);
        }
    }

    for (Int_t j = 0; j < fNBins / 2; ++j) {
        Float_t max = -1.0;
        Int_t startBin = -10;
        Bool_t inPeak = kFALSE;
        Int_t finishBin = -10;
        for (Int_t i = 0; i < fNBins; ++i) {
            if (!inPeak && lorentz->GetBinContent(i, j) > 0.0) { //start signal
                startBin = i;
                inPeak = kTRUE;
            }
            if (inPeak && lorentz->GetBinContent(i, j) > max) max = lorentz->GetBinContent(i, j);
            if (inPeak && lorentz->GetBinContent(i, j) < 1.0) { //stop signal
                finishBin = i;
                inPeak = kFALSE;
                for (Int_t k = startBin; k < finishBin; ++k) {
                    if (lorentz->GetBinContent(k, j) < max / 2.0) lorentz->SetBinContent(k, j, 0.0);
                }
                max = -1.0;
            }
        }
    }

    for (Int_t j = 0; j < fNBins / 2; ++j) {
        for (Int_t i = 0; i < fNBins; ++i) {
            if (lorentz->GetBinContent(i, j) > 0.0) {
                for (Int_t iHit = 0; iHit < fGemHitsArray->GetEntriesFast(); ++iHit) {
                    BmnGemStripHit* hit0 = GetHit(iHit);
                    if (hit0->GetStation() > kMAXSTATIONFORSEED + kNHITSFORSEED) continue;
                    if (hit0->GetRefIndex() < 0) continue; //FIXME!!! Now only for test! (Excluding fake hits) 
                    if (hit0->GetYaddr() / 2 == j && hit0->GetXaddr() == i) {
                        addresses.insert(pair<ULong_t, Int_t > (hit0->GetAddr(), iHit));
                    }
                }
            }
        }
    }
    delete lorentz;
}

void BmnGemSeedFinder::FillHitsForSeedingArray() {
    for (Int_t hitIdx = 0; hitIdx < fGemHitsArray->GetEntriesFast(); ++hitIdx) {
        BmnGemStripHit* hit = GetHit(hitIdx);
        hit->SetFlag(kFALSE); // by default hits are not filtered 
        if (hit->IsUsed()) continue; //Don't use used hits
        if (hit->GetStation() > kMAXSTATIONFORSEED + kNHITSFORSEED) continue;
        if (hit->GetType() == 0) continue; //Don't use fakes

        const Float_t oneOverR = 1.0 / Sqrt(Sqr(hit->GetX()) + Sqr(hit->GetY()) + Sqr(hit->GetZ()));
        const Float_t newX = hit->GetX() * oneOverR;
        const Float_t newY = hit->GetY() * oneOverR;

        Int_t xAddr = ceil((newX - fMin) / fWidth);
        Int_t yAddr = ceil((newY - fMin) / fWidth);
        ULong_t addr = yAddr * fNBins + xAddr;

        hit->SetAddr(addr);
        hit->SetXaddr(xAddr);
        hit->SetYaddr(yAddr);
        hit->SetIndex(hitIdx);

        //        idxInGlobArray.push_back(hitIdx);
        //        hitsForSeeding.push_back(hit);
    }
}

Bool_t BmnGemSeedFinder::IsParCorrect(const FairTrackParam* par) {
    const Float_t maxSlope = 5.;
    const Float_t minSlope = 1e-10;
    const Float_t maxQp = 1000.; // p = 10 MeV

    if (abs(par->GetTx()) > maxSlope || abs(par->GetTy()) > maxSlope || abs(par->GetTx()) < minSlope || abs(par->GetTy()) < minSlope || abs(par->GetQp()) > maxQp) return kFALSE;
    if (isnan(par->GetX()) || isnan(par->GetY()) || isnan(par->GetTx()) || isnan(par->GetTy()) || isnan(par->GetQp())) return kFALSE;

    return kTRUE;
}