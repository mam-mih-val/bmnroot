#include "BmnMath.h"

#include "FairTrackParam.h"
#include "CbmHit.h"
#include "BmnHit.h"
#include "CbmStripHit.h"
#include "CbmPixelHit.h"
#include "CbmGlobalTrack.h"
#include "BmnGemStripHit.h"
#include "TMath.h"
#include "TF1.h"
#include "Math/WrappedTF1.h"
#include "Math/BrentRootFinder.h"
#include <iostream>
#include <cmath>

using namespace TMath;

namespace lit {

    Float_t ChiSq(const FairTrackParam* par, const CbmHit* hit) {
        Float_t chisq = 0.; //FIXME
        //   if (hit->GetType() == kLITSTRIPHIT) {
        //      chisq = ChiSq(par, (CbmStripHit*)hit);
        //   } else if (hit->GetType() == kLITPIXELHIT) {
        //      chisq = ChiSq(par, (CbmPixelHit*) hit);
        //   }
        return chisq;
    }

    Float_t ChiSq(//FIXME
            const FairTrackParam* par,
            const CbmStripHit* hit) {
        //   Float_t duu = hit->GetDu() * hit->GetDu();
        //   Float_t phiCos = 0;//hit->GetCosPhi();                                       //FIXME
        //   Float_t phiSin = 1;//hit->GetSinPhi();                                       //FIXME
        //   Float_t phiCosSq = phiCos * phiCos;
        //   Float_t phiSinSq = phiSin * phiSin;
        //   Float_t phi2SinCos = 2 * phiCos * phiSin;
        //   Float_t C0 = par->GetCovariance(0);
        //   Float_t C1 = par->GetCovariance(1);
        //   Float_t C5 = par->GetCovariance(5);
        //
        //   Float_t ru = hit->GetU() - par->GetX() * phiCos - par->GetY() * phiSin;
        //
        //   return (ru * ru) / (duu - phiCosSq*C0 - phi2SinCos*C1 - phiSinSq*C5);
        return 0;
    }

    Float_t ChiSq(const FairTrackParam* par, const BmnGemHit* hit) {
        Float_t dxx = hit->GetDx() * hit->GetDx();
        Float_t dxy = 0.0; //hit->GetDxy();
        Float_t dyy = hit->GetDy() * hit->GetDy();
        Float_t xmx = hit->GetX() - par->GetX();
        Float_t ymy = hit->GetY() - par->GetY();
        Float_t C0 = par->GetCovariance(0, 0);
        Float_t C1 = par->GetCovariance(0, 1);
        Float_t C5 = par->GetCovariance(1, 1);

        Float_t norm = dxx * dyy - dxx * C5 - dyy * C0 + C0 * C5 - dxy * dxy + 2 * dxy * C1 - C1 * C1;
        if (norm == 0.) {
            norm = 1e-10;
        }
        return ((xmx * (dyy - C5) - ymy * (dxy - C1)) * xmx + (-xmx * (dxy - C1) + ymy * (dxx - C0)) * ymy) / norm;
    }

    Int_t NDF(const BmnGemTrack* track) {
        Int_t ndf = 0;
        for (Int_t i = 0; i < track->GetNHits(); i++)
            ndf += 2;
        ndf -= 5;
        return ndf;
    }

}

TVector3 SpiralFit(const BmnGemTrack* tr, const TClonesArray* arr) {

    const Float_t kN = tr->GetNHits();

    Float_t sumRTheta = 0.0;
    Float_t sumTheta = 0.0;
    Float_t sumTheta2 = 0.0;
    Float_t sumR = 0.0;
    for (Int_t i = 0; i < kN; ++i) {
        BmnGemStripHit* hit = (BmnGemStripHit*) arr->At(tr->GetHitIndex(i));
        if (hit == NULL) continue;
        Float_t x = hit->GetX();
        Float_t z = hit->GetZ();
        Float_t r = Sqrt(x * x + z * z);
        Float_t Theta = ATan2(x, z);
        //        cout << Theta << " " << r << endl;
        sumTheta += Theta;
        sumR += r;
        sumTheta2 += Theta * Theta;
        sumRTheta += r * Theta;
    }
    Float_t b = (kN * sumRTheta - sumTheta * sumR) / (kN * sumTheta2 - sumTheta * sumTheta);
    Float_t a = (sumR - b * sumTheta) / kN;

    //calculate curvature at first point
    Float_t z0 = ((BmnGemStripHit*) arr->At(tr->GetHitIndex(0)))->GetZ();
    Float_t x0 = ((BmnGemStripHit*) arr->At(tr->GetHitIndex(0)))->GetX();
    Float_t theta0 = ATan2(x0, z0);
    Float_t tmp2 = (a + b * theta0) * (a + b * theta0);
    Float_t k = (tmp2 + 2 * b * b) / Sqrt(Power((tmp2 + b * b), 3));

    //    cout << "kN = " << kN << endl;
    //    cout << 1 / k << " | " << Abs(b / 2) << endl;
    //    cout << theta0 << " | " << -a / b <<  endl;

    //    Float_t k = Abs(2 / b); //curvature for r == 0
    //    cout << a << " " << b << " " << 1 / k << endl;
    return TVector3(a, b, 1 / k);
}

Bool_t IsParCorrect(const FairTrackParam* par) {
    const Float_t maxSlope = 5.;
    const Float_t minSlope = 1e-10;
    const Float_t maxQp = 1000.; // p = 10 MeV

    if (abs(par->GetTx()) > maxSlope || abs(par->GetTy()) > maxSlope || abs(par->GetTx()) < minSlope || abs(par->GetTy()) < minSlope || abs(par->GetQp()) > maxQp) return kFALSE;
    if (isnan(par->GetX()) || isnan(par->GetY()) || isnan(par->GetTx()) || isnan(par->GetTy()) || isnan(par->GetQp())) return kFALSE;

    return kTRUE;
}

Float_t ChiSq(const TVector3 par, const BmnGemTrack* tr, const TClonesArray* arr) {
    Float_t a = par.X();
    Float_t b = par.Y();
    Float_t sum = 0.0;
    for (Int_t i = 0; i < tr->GetNHits(); ++i) {
        BmnHit* hit = (BmnHit*) arr->At(tr->GetHitIndex(i));
        Float_t x = hit->GetX();
        Float_t z = hit->GetZ();
        Float_t r_hit = Sqrt(x * x + z * z);
        Float_t theta = ATan2(x, z);
        Float_t r_fit = a + b * theta;
        sum += (r_hit - r_fit) * (r_hit - r_fit) / r_hit;
    }
    return sum;
}

Int_t stationNumber(const string& detName, const Float_t z) {
    Int_t station = -1;
    if (detName == "Gem") {
        const Float_t delta = 6.;
        if (Abs(30 - z) < delta) station = 0;
        else if (Abs(45 - z) < delta) station = 1;
        else if (Abs(60 - z) < delta) station = 2;
        else if (Abs(80 - z) < delta) station = 3;
        else if (Abs(100 - z) < delta) station = 4;
        else if (Abs(130 - z) < delta) station = 5;
        else if (Abs(160 - z) < delta) station = 6;
        else if (Abs(190 - z) < delta) station = 7;
        else if (Abs(230 - z) < delta) station = 8;
        else if (Abs(270 - z) < delta) station = 9;
        else if (Abs(315 - z) < delta) station = 10;
        else if (Abs(360 - z) < delta) station = 11;
        else station = -1;
    } else if (detName == "Tof1") {
        station = 12;
    } else if (detName == "Dch1") {
        station = 13;
    } else if (detName == "Dch2") {
        station = 14;
    } else if (detName == "Tof2") {
        station = 15;
    }
    return station;
}

Float_t NumericalRootFinder(TF1 f, Float_t left, Float_t right) {

    // Create the wrapper for function
    ROOT::Math::WrappedTF1 wf1(f);

    // Create the Integrator
    ROOT::Math::BrentRootFinder brf;

    // Set parameters of the method
    brf.SetFunction(wf1, left, right);
    brf.Solve();

    //   cout << brf.Root() << endl;

    return brf.Root();
}


TVector3 LineFit(BmnGemTrack* track, const TClonesArray* arr) {

    //Least Square Method//
    Float_t Zi = 0.0, Yi = 0.0; // coordinates of current track point
    Float_t a = 0.0, b = 0.0; // parameters of line: y = a * z + b
    Float_t SumZ = 0.0, SumY = 0.0, SumZY = 0.0, SumZ2 = 0.0;
    const Float_t nHits = track->GetNHits();
    for (Int_t i = 0; i < nHits; ++i) {
        BmnGemStripHit* hit = (BmnGemStripHit*) arr->At(track->GetHitIndex(i));
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

Float_t Sqr(Float_t x) {
    return x * x;
}

TVector3 CircleBy3Hit(BmnGemTrack* track, const TClonesArray* arr) {
    const Float_t nHits = track->GetNHits();
    if (nHits < 3) return TVector3(0.0, 0.0, 0.0);
    BmnGemStripHit* hit0 = (BmnGemStripHit*) arr->At(track->GetHitIndex(0));
    BmnGemStripHit* hit1 = (BmnGemStripHit*) arr->At(track->GetHitIndex(1));
    BmnGemStripHit* hit2 = (BmnGemStripHit*) arr->At(track->GetHitIndex(2));

    Float_t x1 = hit0->GetX();
    Float_t z1 = hit0->GetZ();
    Float_t x2 = hit1->GetX();
    Float_t z2 = hit1->GetZ();
    Float_t x3 = hit2->GetX();
    Float_t z3 = hit2->GetZ();

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