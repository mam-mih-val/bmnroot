#include "BmnMath.h"

#include "FairTrackParam.h"
#include "TClonesArray.h"
#include "CbmHit.h"
#include "BmnHit.h"
#include "CbmStripHit.h"
#include "CbmPixelHit.h"
#include "CbmGlobalTrack.h"
#include "BmnGemStripHit.h"
#include "TMath.h"
#include "TGraph.h"
#include "TF1.h"
#include "TArc.h"
#include "TLine.h"
#include "TCanvas.h"
#include "TH2F.h"
#include "Math/WrappedTF1.h"
#include "Math/BrentRootFinder.h"
#include <iostream>
#include <cmath>
#include "TFitResult.h"

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

    Float_t ChiSq(const FairTrackParam* par, const BmnGemStripHit* hit) {
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

TVector3 SpiralFitByTwoPoints(const BmnGemTrack* tr, const TClonesArray* arr) {

    BmnGemStripHit* hit0 = (BmnGemStripHit*) arr->At(tr->GetHitIndex(0));
    BmnGemStripHit* hit1 = (BmnGemStripHit*) arr->At(tr->GetHitIndex(1));
    Float_t x0 = hit0->GetX();
    Float_t x1 = hit1->GetX();
    Float_t z0 = hit0->GetZ();
    Float_t z1 = hit1->GetZ();
    Float_t r0 = Sqrt(x0 * x0 + z0 * z0);
    Float_t r1 = Sqrt(x1 * x1 + z1 * z1);
    Float_t theta0 = ATan2(x0, z0);
    Float_t theta1 = ATan2(x1, z1);

    Float_t b = (r1 - r0) / (theta1 - theta0);
    Float_t a = r0 - theta0 * b;
    Float_t tmp2 = (a + b * theta0) * (a + b * theta0);
    Float_t k = (tmp2 + 2 * b * b) / Sqrt(Power((tmp2 + b * b), 3));
    return TVector3(a, b, 1 / k);

}

TLorentzVector SpiralParabolicFit(BmnGemTrack* tr, const TClonesArray* arr) {
    BmnGemStripHit* hit0 = (BmnGemStripHit*) arr->At(tr->GetHitIndex(0));
    BmnGemStripHit* hit1 = (BmnGemStripHit*) arr->At(tr->GetHitIndex(1));
    BmnGemStripHit* hit2 = (BmnGemStripHit*) arr->At(tr->GetHitIndex(2));
    Float_t x0 = hit0->GetX();
    Float_t x1 = hit1->GetX();
    Float_t x2 = hit2->GetX();
    Float_t z0 = hit0->GetZ();
    Float_t z1 = hit1->GetZ();
    Float_t z2 = hit2->GetZ();
    Float_t r0 = Sqrt(x0 * x0 + z0 * z0);
    Float_t r1 = Sqrt(x1 * x1 + z1 * z1);
    Float_t r2 = Sqrt(x2 * x2 + z2 * z2);
    Float_t t0 = ATan2(x0, z0);
    Float_t t1 = ATan2(x1, z1);
    Float_t t2 = ATan2(x2, z2);

    Float_t dt10 = t1 - t0;
    Float_t dt20 = t2 - t0;
    Float_t dt21 = t2 - t1;
    Float_t dr21 = r2 - r1;
    Float_t dr10 = r1 - r0;

    Float_t a = (dr21 / dt21 - dr10 / dt10) / dt20;
    Float_t b = dr21 / dt21 - a * (t2 + t1);
    Float_t c = r0 - a * t0 * t0 - b * t0;

    Float_t r = r0; //a * t0 * t0 + b * t0 + c;
    Float_t dr = 2 * a * t0 + b;
    Float_t ddr = 2 * a;

    Float_t k = Abs(r * r + 2 * dr * dr - r * ddr) / Sqrt(Power((r * r + dr * dr), 3));
    return TLorentzVector(a, b, c, 1.0 / k);

}

TVector3 SpiralFit(BmnGemTrack* tr, const TClonesArray* arr) {

    BmnGemStripHit* hit0 = (BmnGemStripHit*) arr->At(tr->GetHitIndex(0));
    Float_t xv = hit0->GetX();
    Float_t zv = hit0->GetZ();
    Float_t Thetav = ATan2(xv, zv) * RadToDeg();
    const Float_t kN = (Abs(Thetav) < 90) ? 3 : tr->GetNHits();

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

    return TVector3(a, b, 1 / k);
}

Bool_t IsParCorrect(const FairTrackParam* par, const Bool_t isField) {
    const Float_t maxSlopeX = 5.;
    const Float_t maxSlopeY = 0.5;
    const Float_t maxX = 150.0;
    const Float_t maxY = 100.0;
    const Float_t minSlope = 1e-10;
    const Float_t maxQp = 1000.; // p = 10 MeV

    if (abs(par->GetTx()) > maxSlopeX || abs(par->GetTy()) > maxSlopeY || abs(par->GetTx()) < minSlope || abs(par->GetTy()) < minSlope) return kFALSE;
    if (abs(par->GetX()) > maxX || abs(par->GetY()) > maxY) return kFALSE;
    if (IsNaN(par->GetX()) || IsNaN(par->GetY()) || IsNaN(par->GetTx()) || IsNaN(par->GetTy())) return kFALSE;
    
    if (isField) {
        if (abs(par->GetQp()) > maxQp) return kFALSE;
        if (IsNaN(par->GetQp())) return kFALSE;
    }

    return kTRUE;
}

Float_t ChiSq(const TVector3* par, const BmnGemTrack* tr, const TClonesArray* arr, const TString type) {
    if (type.Contains("spiral")) {
        Float_t a = par->X();
        Float_t b = par->Y();
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
    } else if (type.Contains("circle")) {
        Float_t Xc = par->X();
        Float_t Zc = par->Y();
        Float_t R = par->Z();
        Float_t sum = 0.0;
        for (Int_t i = 0; i < tr->GetNHits(); ++i) {
            BmnHit* hit = (BmnHit*) arr->At(tr->GetHitIndex(i));
            Float_t x = hit->GetX();
            Float_t z = hit->GetZ();
            Float_t x_fit = Xc + Sqrt(R * R - (z - Zc) * (z - Zc));
            sum += (x_fit - x) * (x_fit - x) / x / x;
        }
        return Sqrt(sum);
    }
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

TVector3 LineFit(BmnTrack* track, const TClonesArray* arr, TString type) {

    //Weighted Least Square Method//
    Float_t Xi = 0.0, Yi = 0.0; // coordinates of current track point
    Float_t a = 0.0, b = 0.0; // parameters of line: y = a * x + b

    Float_t Si = 0.0; // sigma
    Float_t Wi = 0.0; // weight = 1 / sigma^2
    Float_t SumW = 0.0; // sum of weights
    Float_t SumWX = 0.0; // sum of (weight * x)
    Float_t SumWY = 0.0; // sum of (weight * y)
    Float_t SumWXY = 0.0; // sum of (weight * x * y)
    Float_t SumWX2 = 0.0; // sum of (weight * x * x)

    //  TH2F* h_Hits = new TH2F("h_Hits", "h_Hits", 400, 0.0, 250.0, 400, -10.0, 10.0);
    //  TH2F* h_HitsAll = new TH2F("h_HitsAll", "h_HitsAll", 400, 0.0, 250.0, 400, -10.0, 10.0);

    const Float_t nHits = track->GetNHits();

    //    for (Int_t iHit = 0; iHit < arr->GetEntriesFast(); iHit++) {
    //        BmnGemStripHit* hit = (BmnGemStripHit*) arr->At(iHit);
    //        h_HitsAll->Fill(hit->GetZ(), hit->GetX());
    //    }

    for (Int_t i = 0; i < nHits; ++i) {
        BmnHit* hit = (BmnHit*) arr->At(track->GetHitIndex(i));
        if (type.Contains("XY")) {
            Xi = hit->GetX();
            Yi = hit->GetY();
            Si = hit->GetDy();
        } else if (type.Contains("ZX")) {
            Xi = hit->GetZ();
            Yi = hit->GetX();
            Si = hit->GetDx();
            //   h_Hits->Fill(hit->GetZ(), hit->GetX());
        } else if (type.Contains("ZY")) {
            Xi = hit->GetZ();
            Yi = hit->GetY();
            Si = hit->GetDy();
        }

        if (Si == 0.0) return TVector3(0.0, 0.0, 0.0);

        Wi = 1.0 / Si / Si;
        SumW += Wi;
        SumWXY += Wi * Xi * Yi;
        SumWX += Wi * Xi;
        SumWX2 += Wi * Xi * Xi;
        SumWY += Wi * Yi;
    }

    a = (SumW * SumWXY - SumWX * SumWY) / (SumW * SumWX2 - SumWX * SumWX);
    b = (SumWX2 * SumWY - SumWX * SumWXY) / (SumW * SumWX2 - SumWX * SumWX);

    // z1, x1, z2, x2
    //   TLine* line = new TLine();
    // line->SetFillStyle(0);
    //    line->SetLineWidth(2);
    //    line->SetLineColor(kBlue);
    // line->SetNoEdges(1);

    //    TCanvas* c_New = new TCanvas("c", "c", 1000, 500);
    //    c_New->cd();
    //    h_Hits->SetMarkerStyle(20);
    //    h_Hits->SetMarkerSize(1.5);
    //    h_Hits->SetMarkerColor(kRed);
    //    h_Hits->Draw("P");
    //    h_HitsAll->SetMarkerStyle(20);
    //    h_HitsAll->SetMarkerSize(0.75);
    //    h_HitsAll->SetMarkerColor(kBlue);
    //    h_HitsAll->Draw("Psame");
    //    line->Draw("same");
    //    c_New->SaveAs("hits.png");
    //    getchar();
    //    delete h_Hits;
    //    delete c_New;
    //    delete line;

    Float_t chi2 = 0.0;

    for (Int_t i = 0; i < nHits; ++i) {
        BmnHit* hit = (BmnHit*) arr->At(track->GetHitIndex(i));
        if (type.Contains("XY")) {
            Xi = hit->GetX();
            Yi = hit->GetY();
            Si = hit->GetDy();
        } else if (type.Contains("ZX")) {
            Xi = hit->GetZ();
            Yi = hit->GetX();
            Si = hit->GetDx();
        } else if (type.Contains("ZY")) {
            Xi = hit->GetZ();
            Yi = hit->GetY();
            Si = hit->GetDy();
        }

        chi2 += Sqr((Yi - a * Xi - b) / Si);
    }

    return TVector3(a, b, chi2);
}

void LineFit(Double_t& par1, Double_t& par2, BmnGemTrack* track, TClonesArray* arr, Int_t type, Int_t idSkip) {

    //Weighted Least Square Method//
    Float_t Xi = 0.0, Yi = 0.0; // coordinates of current track point
    Float_t a = 0.0, b = 0.0; // parameters of line: y = a * x + b

    Float_t Si = 0.0; // sigma
    Float_t Wi = 0.0; // weight = 1 / sigma^2
    Float_t SumW = 0.0; // sum of weights
    Float_t SumWX = 0.0; // sum of (weight * x)
    Float_t SumWY = 0.0; // sum of (weight * y)
    Float_t SumWXY = 0.0; // sum of (weight * x * y)
    Float_t SumWX2 = 0.0; // sum of (weight * x * x)

    const Float_t nHits = track->GetNHits();
    for (Int_t i = 0; i < nHits; ++i) {
        BmnGemStripHit* hit = (BmnGemStripHit*) arr->At(track->GetHitIndex(i));

        if (i == idSkip)
            continue;

        else if (type == 1) {
            Xi = hit->GetZ();
            Yi = hit->GetX();
            Si = hit->GetDx();
        } else {
            Xi = hit->GetZ();
            Yi = hit->GetY();
            Si = hit->GetDy();
        }

        Wi = 1.0 / Si / Si;
        SumW += Wi;
        SumWXY += Wi * Xi * Yi;
        SumWX += Wi * Xi;
        SumWX2 += Wi * Xi * Xi;
        SumWY += Wi * Yi;
    }

    a = (SumW * SumWXY - SumWX * SumWY) / (SumW * SumWX2 - SumWX * SumWX);
    b = (SumWX2 * SumWY - SumWX * SumWXY) / (SumW * SumWX2 - SumWX * SumWX);

    Float_t chi2 = 0.0;

    for (Int_t i = 0; i < nHits; ++i) {
        BmnGemStripHit* hit = (BmnGemStripHit*) arr->At(track->GetHitIndex(i));

        if (i == idSkip)
            continue;

        if (type == 1) {
            Xi = hit->GetZ();
            Yi = hit->GetX();
            Si = hit->GetDx();
        } else {
            Xi = hit->GetZ();
            Yi = hit->GetY();
            Si = hit->GetDy();
        }

        chi2 += ((Yi - a * Xi - b) / Si) * ((Yi - a * Xi - b) / Si);
    }

    par1 = a;
    par2 = b;
}

TVector3 CircleFit(BmnGemTrack* track, const TClonesArray* arr, Double_t &chi2) {

    //Weighted Least Square Method//
    Double_t Xi = 0.0, Yi = 0.0, Zi = 0.0; // coordinates of current track point
    Double_t Xc = 0.0, Zc = 0.0, R = 0.0;
    chi2 = 0.0;

    Double_t Wi = 0.0; // weight = 1 / sigma^2

    Double_t Sx = 0.0; // sum of weights
    Double_t Sxx = 0.0;
    Double_t Syy = 0.0;
    Double_t Sxy = 0.0;
    Double_t Sy = 0.0;
    Double_t Sz = 0.0;
    Double_t Szx = 0.0;
    Double_t Szy = 0.0;

    //        TH2F* h_Hits = new TH2F("h_Hits", "h_Hits", 400, 0.0, 250.0, 400, -10.0, 10.0);

    const Float_t nHits = track->GetNHits();
    for (Int_t i = 0; i < nHits; ++i) {
        BmnGemStripHit* hit = (BmnGemStripHit*) arr->At(track->GetHitIndex(i));
        //Use Z and X coordinates of hits to fit in ZX plane
        Yi = hit->GetZ();
        Xi = hit->GetX();
        //                h_Hits->Fill(hit->GetZ(), hit->GetX());
        Zi = Xi * Xi + Yi * Yi;
        Wi = 1.0 / hit->GetDx() / hit->GetDx();

        Sx += Wi * Xi;
        Sy += Wi * Yi;
        Sz += Wi * Zi;
        Sxx += Wi * Xi * Xi;
        Sxy += Wi * Xi * Yi;
        Syy += Wi * Yi * Yi;
        Szx += Wi * Zi * Xi;
        Szy += Wi * Zi * Yi;
    }

    Double_t C = ((Sz * Sx - Szx) / (Sxx - Sx * Sx) - (Sz * Sy - Szy) / (Sxy - Sx * Sy)) / ((Sxy - Sx * Sy) / (Sxx - Sx * Sx) - (Syy - Sy * Sy) / (Sxy - Sx * Sy));
    Double_t B = ((Sz * Sx - Szx) - C * (Sxy - Sx * Sy)) / (Sxx - Sx * Sx);
    Double_t D = -Sz - B * Sx - C * Sy;

    Xc = -0.5 * B;
    Zc = -0.5 * C;
    R = Sqrt(0.25 * B * B + 0.25 * C * C - D);

    //        BmnGemStripHit* hitF = (BmnGemStripHit*) arr->At(track->GetHitIndex(0));
    //        BmnGemStripHit* hitL = (BmnGemStripHit*) arr->At(track->GetHitIndex(nHits - 1));

    //    
    //        TArc* arc = new TArc(Zc, Xc, R, ATan2((hitF->GetX() - Xc), (hitF->GetZ() - Zc)) * RadToDeg(), ATan2((hitL->GetX() - Xc), (hitL->GetZ() - Zc)) * RadToDeg());
    //        arc->SetFillStyle(0);
    //        arc->SetLineWidth(2);
    //        arc->SetLineColor(kBlue);
    //        arc->SetNoEdges(1);
    //        
    //        TCanvas* c_New = new TCanvas("c", "c", 1000, 500);
    //        c_New->cd();
    //        h_Hits->SetMarkerStyle(20);
    //        h_Hits->SetMarkerSize(1.5);
    //        h_Hits->SetMarkerColor(kRed);
    //        h_Hits->Draw("P");
    //        arc->Draw("same");
    //        c_New->SaveAs("hits.png");
    //        getchar();
    //        delete h_Hits;
    //        delete c_New;
    //        delete arc;

    for (Int_t i = 0; i < nHits; ++i) {
        BmnGemStripHit* hit = (BmnGemStripHit*) arr->At(track->GetHitIndex(i));
        chi2 += Sqr((R - Sqrt(Sqr(hit->GetX() - Xc) + Sqr(hit->GetZ() - Zc))) / hit->GetDx());
    }

    return TVector3(Zc, Xc, R);
}

Float_t Dist(Float_t x1, Float_t y1, Float_t x2, Float_t y2) {
    if (Sqr(x1 - x2) + Sqr(y1 - y2) <= 0.0) {
        return 0.0;
    } else {
        return Sqrt(Sqr(x1 - x2) + Sqr(y1 - y2));
    }
}

Float_t NewtonSolver(Float_t A0, Float_t A1, Float_t A2, Float_t A22) {

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

    return TVector3(Zc, Xc, R);

}

TVector3 Pol2By3Hit(BmnGemTrack* track, const TClonesArray* arr) {
    const Int_t nHits = track->GetNHits();
    if (nHits < 3) return TVector3(0.0, 0.0, 0.0);
    BmnGemStripHit* hit0 = (BmnGemStripHit*) arr->At(track->GetHitIndex(0));
    BmnGemStripHit* hit1 = (BmnGemStripHit*) arr->At(track->GetHitIndex(1));
    BmnGemStripHit* hit2 = (BmnGemStripHit*) arr->At(track->GetHitIndex(2));

    Float_t x0 = hit0->GetX();
    Float_t z0 = hit0->GetZ();
    Float_t x1 = hit1->GetX();
    Float_t z1 = hit1->GetZ();
    Float_t x2 = hit2->GetX();
    Float_t z2 = hit2->GetZ();

    Float_t z0_2 = z0 * z0;
    Float_t z1_2 = z1 * z1;
    Float_t z2_2 = z2 * z2;

    Float_t B = (x2 * (z1_2 - z0_2) + x1 * (z0_2 - z2_2) + x0 * (z2_2 - z1_2)) / ((z2 - z1) * (z1 - z0) * (z0 - z2));
    Float_t A = ((x2 - x1) - B * (z2 - z1)) / (z2_2 - z1_2);
    Float_t C = x0 - B * z0 - A * z0_2;

    return TVector3(A, B, C);

}

void DrawHits(BmnGemTrack* track, const TClonesArray* arr) {
    const Int_t nHits = track->GetNHits();
    Float_t z[nHits];
    Float_t x[nHits];
    for (Int_t i = 0; i < nHits; ++i) {
        z[i] = ((BmnGemStripHit*) arr->At(track->GetHitIndex(i)))->GetZ();
        x[i] = ((BmnGemStripHit*) arr->At(track->GetHitIndex(i)))->GetX();
    }
    TCanvas* c = new TCanvas("c", "c", 1000, 1000);
    TGraph* gr = new TGraph(nHits, z, x);
    gr->Fit("pol2");
    gr->Draw("AL*");
    c->Update();
    getchar();
    delete gr;
    delete c;
}

TVector3 CircleBy3Hit(BmnGemTrack* track, const BmnGemStripHit* h0, const BmnGemStripHit* h1, const BmnGemStripHit* h2) {
    Float_t x0 = h0->GetX();
    Float_t z0 = h0->GetZ();
    Float_t x1 = h1->GetX();
    Float_t z1 = h1->GetZ();
    Float_t x2 = h2->GetX();
    Float_t z2 = h2->GetZ();

    //    cout << x1 << " " << z1 << " " << x2 << " " << z2 << " " << x3 << " " << z3 << " " << endl;

    Float_t x0_2 = x0 * x0;
    Float_t z0_2 = z0 * z0;
    Float_t x1_2 = x1 * x1;
    Float_t z1_2 = z1 * z1;
    Float_t x2_2 = x2 * x2;
    Float_t z2_2 = z2 * z2;
    Float_t dx10 = x1 - x0;
    Float_t dx21 = x2 - x1;
    Float_t dz10 = z1 - z0;
    Float_t dz21 = z2 - z1;
    Float_t dx10_2 = x1_2 - x0_2;
    Float_t dx21_2 = x2_2 - x1_2;
    Float_t dz10_2 = z1_2 - z0_2;
    Float_t dz21_2 = z2_2 - z1_2;

    Float_t B = ((dx10_2 + dz10_2) / dx10 - (dx21_2 + dz21_2) / dx21) / (dz21 / dx21 - dz10 / dx10);
    Float_t A = -(dx21_2 + dz21_2 + B * dz21) / dx21;
    Float_t C = -x0_2 - z0_2 - A * x0 - B * z0;


    Float_t Xc = -A / 2;
    Float_t Zc = -B / 2;
    Float_t R = Sqrt(A * A + B * B - 4 * C) / 2;

    return TVector3(Zc, Xc, R);

}
// Пусть пока этот крокодил поживет здесь:)

void fit_seg(Double_t* z_loc, Double_t* rh_seg, Double_t* rh_sigm_seg, Double_t* par_ab, Int_t skip_first, Int_t skip_second) {
    Double_t sqrt_2 = sqrt(2.);
    //linear fit
    Float_t A[4][4] = {
        {0, 0, 0, 0},
        {0, 0, 0, 0},
        {0, 0, 0, 0},
        {0, 0, 0, 0}
    }; //coef matrix
    Float_t f[4] = {0}; //free coef 
    //      Float_t sigm_sq[8] = {1,1,1,1,1,1,1,1};
    Int_t h[8] = {0, 0, 0, 0, 0, 0, 0, 0};

    for (Int_t i = 0; i < 4; i++)
        par_ab[i] = 999;

    for (Int_t i = 0; i < 8; i++) {
        h[i] = 1;
        //out1<<"setting h[i]"<<endl;
        if (i == skip_first || i == skip_second || Abs(rh_seg[i] + 999.) < FLT_EPSILON) {
            h[i] = 0;
        }
    }//i

    A[0][0] = 2 * z_loc[0] * z_loc[0] * h[0] / rh_sigm_seg[0] + z_loc[4] * z_loc[4] * h[4] / rh_sigm_seg[4] + z_loc[6] * z_loc[6] * h[6] / rh_sigm_seg[6] +
            2 * z_loc[1] * z_loc[1] * h[1] / rh_sigm_seg[1] + z_loc[5] * z_loc[5] * h[5] / rh_sigm_seg[5] + z_loc[7] * z_loc[7] * h[7] / rh_sigm_seg[7]; //aX_a

    A[0][1] = 2 * z_loc[0] * h[0] / rh_sigm_seg[0] + z_loc[4] * h[4] / rh_sigm_seg[4] + z_loc[6] * h[6] / rh_sigm_seg[6] +
            2 * z_loc[1] * h[1] / rh_sigm_seg[1] + z_loc[5] * h[5] / rh_sigm_seg[5] + z_loc[7] * h[7] / rh_sigm_seg[7]; //bX_a

    A[0][2] = z_loc[6] * z_loc[6] * h[6] / rh_sigm_seg[6] - z_loc[4] * z_loc[4] * h[4] / rh_sigm_seg[4] +
            z_loc[7] * z_loc[7] * h[7] / rh_sigm_seg[7] - z_loc[5] * z_loc[5] * h[5] / rh_sigm_seg[5]; //aY

    A[0][3] = z_loc[6] * h[6] / rh_sigm_seg[6] - z_loc[4] * h[4] / rh_sigm_seg[4] + z_loc[7] * h[7] / rh_sigm_seg[7] - z_loc[5] * h[5] / rh_sigm_seg[5]; //bY_a

    //dChi2/d_b_x

    A[1][0] = 2 * z_loc[0] * h[0] / rh_sigm_seg[0] + z_loc[4] * h[4] / rh_sigm_seg[4] + z_loc[6] * h[6] / rh_sigm_seg[6]
            + 2 * z_loc[1] * h[1] / rh_sigm_seg[1] + z_loc[5] * h[5] / rh_sigm_seg[5] + z_loc[7] * h[7] / rh_sigm_seg[7];

    A[1][1] = 2 * h[0] / rh_sigm_seg[0] + 1 * h[4] / rh_sigm_seg[4] + 1 * h[6] / rh_sigm_seg[6]
            + 2 * h[1] / rh_sigm_seg[1] + 1 * h[5] / rh_sigm_seg[5] + 1 * h[7] / rh_sigm_seg[7]; //bX_a

    A[1][2] = z_loc[6] * h[6] / rh_sigm_seg[6] - z_loc[4] * h[4] / rh_sigm_seg[4] + z_loc[7] * h[7] / rh_sigm_seg[7] - z_loc[5] * h[5] / rh_sigm_seg[5]; //aY_a

    A[1][3] = 1 * h[7] / rh_sigm_seg[7] - 1 * h[5] / rh_sigm_seg[5] + 1 * h[6] / rh_sigm_seg[6] - 1 * h[4] / rh_sigm_seg[4]; //bY_a

    //dChi2/da_y

    A[2][0] = z_loc[6] * z_loc[6] * h[6] / rh_sigm_seg[6] - z_loc[4] * z_loc[4] * h[4] / rh_sigm_seg[4] + z_loc[7] * z_loc[7] * h[7] / rh_sigm_seg[7] - z_loc[5] * z_loc[5] * h[5] / rh_sigm_seg[5]; //aX_a

    A[2][1] = z_loc[6] * h[6] / rh_sigm_seg[6] - z_loc[4] * h[4] / rh_sigm_seg[4] + z_loc[7] * h[7] / rh_sigm_seg[7] - z_loc[5] * h[5] / rh_sigm_seg[5]; //bX_a

    A[2][2] = 2 * z_loc[2] * z_loc[2] * h[2] / rh_sigm_seg[2] + z_loc[4] * z_loc[4] * h[4] / rh_sigm_seg[4] + z_loc[6] * z_loc[6] * h[6] / rh_sigm_seg[6]
            + 2 * z_loc[3] * z_loc[3] * h[3] / rh_sigm_seg[3] + z_loc[5] * z_loc[5] * h[5] / rh_sigm_seg[5] + z_loc[7] * z_loc[7] * h[7] / rh_sigm_seg[7]; //aY_a

    A[2][3] = 2 * z_loc[2] * h[2] / rh_sigm_seg[2] + z_loc[4] * h[4] / rh_sigm_seg[4] + z_loc[6] * h[6] / rh_sigm_seg[6]
            + 2 * z_loc[3] * h[3] / rh_sigm_seg[3] + z_loc[5] * h[5] / rh_sigm_seg[5] + z_loc[7] * h[7] / rh_sigm_seg[7];

    ////dChi2/db_y

    A[3][0] = z_loc[6] * h[6] / rh_sigm_seg[6] - z_loc[4] * h[4] / rh_sigm_seg[4] + z_loc[7] * h[7] / rh_sigm_seg[7] - z_loc[5] * h[5] / rh_sigm_seg[5]; //aX_a

    A[3][1] = 1 * h[6] / rh_sigm_seg[6] - 1 * h[4] / rh_sigm_seg[4] + 1 * h[7] / rh_sigm_seg[7] - 1 * h[5] / rh_sigm_seg[5]; //bX_a

    A[3][2] = 2 * z_loc[2] * h[2] / rh_sigm_seg[2] + z_loc[4] * h[4] / rh_sigm_seg[4] + z_loc[6] * h[6] / rh_sigm_seg[6]
            + 2 * z_loc[3] * h[3] / rh_sigm_seg[3] + z_loc[5] * h[5] / rh_sigm_seg[5] + z_loc[7] * h[7] / rh_sigm_seg[7]; //aY_a

    A[3][3] = 2 * h[2] / rh_sigm_seg[2] + 1 * h[4] / rh_sigm_seg[4] + 1 * h[6] / rh_sigm_seg[6]
            + 2 * h[3] / rh_sigm_seg[3] + 1 * h[5] / rh_sigm_seg[5] + 1 * h[7] / rh_sigm_seg[7]; //bY_a


    //free coef

    //dChi2/da_x

    f[0] = 2 * z_loc[0] * rh_seg[0] * h[0] / rh_sigm_seg[0] + sqrt_2 * z_loc[6] * rh_seg[6] * h[6] / rh_sigm_seg[6] - sqrt_2 * z_loc[4] * rh_seg[4] * h[4] / rh_sigm_seg[4] +
            2 * z_loc[1] * rh_seg[1] * h[1] / rh_sigm_seg[1] + sqrt_2 * z_loc[7] * rh_seg[7] * h[7] / rh_sigm_seg[7] - sqrt_2 * z_loc[5] * rh_seg[5] * h[5] / rh_sigm_seg[5]; //j = nr of seg
    //dChi2/db_x
    f[1] = 2 * rh_seg[0] * h[0] / rh_sigm_seg[0] + sqrt_2 * rh_seg[6] * h[6] / rh_sigm_seg[6] - sqrt_2 * rh_seg[4] * h[4] / rh_sigm_seg[4] +
            2 * rh_seg[1] * h[1] / rh_sigm_seg[1] + sqrt_2 * rh_seg[7] * h[7] / rh_sigm_seg[7] - sqrt_2 * rh_seg[5] * h[5] / rh_sigm_seg[5]; //j = nr of seg
    //dChi2/da_y
    f[2] = 2 * z_loc[2] * rh_seg[2] * h[2] / rh_sigm_seg[2] + sqrt_2 * z_loc[6] * rh_seg[6] * h[6] / rh_sigm_seg[6] + sqrt_2 * z_loc[4] * rh_seg[4] * h[4] / rh_sigm_seg[4] +
            2 * z_loc[3] * rh_seg[3] * h[3] / rh_sigm_seg[3] + sqrt_2 * z_loc[7] * rh_seg[7] * h[7] / rh_sigm_seg[7] + sqrt_2 * z_loc[5] * rh_seg[5] * h[5] / rh_sigm_seg[5]; //j = nr of seg
    ////dChi2/db_y
    f[3] = 2 * rh_seg[2] * h[2] / rh_sigm_seg[2] + sqrt_2 * rh_seg[6] * h[6] / rh_sigm_seg[6] + sqrt_2 * rh_seg[4] * h[4] / rh_sigm_seg[4] +
            2 * rh_seg[3] * h[3] / rh_sigm_seg[3] + sqrt_2 * rh_seg[7] * h[7] / rh_sigm_seg[7] + sqrt_2 * rh_seg[5] * h[5] / rh_sigm_seg[5]; //j = nr of seg

    //inverse the matrix

    /**** Gaussian algorithm for 4x4 matrix inversion ****/
    Int_t i1, j1, k1, l1;
    Double_t factor;
    Double_t temp[4];
    Double_t b[4][4];
    Double_t A0[4][4];

    for (i1 = 0; i1 < 4; i1++) for (j1 = 0; j1 < 4; j1++) A0[i1][j1] = A[i1][j1];

    // Set b to I
    for (i1 = 0; i1 < 4; i1++) for (j1 = 0; j1 < 4; j1++)
            if (i1 == j1) b[i1][j1] = 1.0;
            else b[i1][j1] = 0.0;

    for (i1 = 0; i1 < 4; i1++) {
        for (j1 = i1 + 1; j1 < 4; j1++)
            if (Abs(A[i1][i1]) < Abs(A[j1][i1])) {
                for (l1 = 0; l1 < 4; l1++) temp[l1] = A[i1][l1];
                for (l1 = 0; l1 < 4; l1++) A[i1][l1] = A[j1][l1];
                for (l1 = 0; l1 < 4; l1++) A[j1][l1] = temp[l1];
                for (l1 = 0; l1 < 4; l1++) temp[l1] = b[i1][l1];
                for (l1 = 0; l1 < 4; l1++) b[i1][l1] = b[j1][l1];
                for (l1 = 0; l1 < 4; l1++) b[j1][l1] = temp[l1];
            }
        factor = A[i1][i1];
        for (j1 = 4 - 1; j1>-1; j1--) {
            b[i1][j1] /= factor;
            A[i1][j1] /= factor;
        }
        for (j1 = i1 + 1; j1 < 4; j1++) {
            factor = -A[j1][i1];
            for (k1 = 0; k1 < 4; k1++) {
                A[j1][k1] += A[i1][k1] * factor;
                b[j1][k1] += b[i1][k1] * factor;
            }
        }
    }
    for (i1 = 3; i1 > 0; i1--) {
        for (j1 = i1 - 1; j1>-1; j1--) {
            factor = -A[j1][i1];
            for (k1 = 0; k1 < 4; k1++) {
                A[j1][k1] += A[i1][k1] * factor;
                b[j1][k1] += b[i1][k1] * factor;
            }
        }
    }

    Float_t sum;

    Float_t A1[4][4] = {
        {0, 0, 0, 0},
        {0, 0, 0, 0},
        {0, 0, 0, 0},
        {0, 0, 0, 0}
    };

    for (i1 = 0; i1 < 4; ++i1) for (j1 = 0; j1 < 4; ++j1) {
            sum = 0;

            for (k1 = 0; k1 < 4; ++k1)
                sum += A0[i1][k1] * b[k1][j1];
            A1[i1][j1] = sum;
        }

    for (i1 = 0; i1 < 4; i1++) {
        par_ab[i1] = 0;
        for (j1 = 0; j1 < 4; j1++) {
            par_ab[i1] += b[i1][j1] * f[j1];
        }
    }
}

void Pol2Fit(BmnGemTrack* track, const TClonesArray* arr, Double_t &A, Double_t &B, Double_t &C, Int_t idSkip) {
    const Float_t nHits = track->GetNHits();
    TGraph* gr = new TGraph(nHits - 1);
    Int_t iPoint = 0;
    for (Int_t i = 0; i < nHits; ++i) {
        if (i == idSkip) continue;
        BmnGemStripHit* hit = (BmnGemStripHit*) arr->At(track->GetHitIndex(i));
        gr->SetPoint(iPoint++, hit->GetZ(), hit->GetX());
    }
    TFitResultPtr ptr = gr->Fit("pol2", "SQ");
    delete gr;
    A = ptr->Parameter(2);
    B = ptr->Parameter(1);
    C = ptr->Parameter(0);
}

vector <Double_t> dist(vector <Double_t> qp, Double_t mu) {
    vector <Double_t> res;
    for (Int_t iEle = 0; iEle < qp.size(); iEle++)
        res.push_back(Abs(qp[iEle] - mu));

    return res;
}

vector <Double_t> W(vector <Double_t> dist, Double_t sig) {
    vector <Double_t> res;
    const Int_t C = 10;
    for (Int_t iEle = 0; iEle < dist.size(); iEle++) {
        Double_t w = (dist[iEle] > C * sig) ? 0. : Power(1 - Power(dist[iEle] / C / sig, 2), 2);
        res.push_back(w);
    }

    return res;
}

Double_t Sigma(vector <Double_t> dist, vector <Double_t> w) {
    if (dist.size() != w.size())
        throw;

    Double_t WiDi2Sum = 0.;
    Double_t WiSum = 0.;

    for (Int_t iEle = 0; iEle < dist.size(); iEle++) {
        WiSum += w[iEle];
        WiDi2Sum += w[iEle] * dist[iEle] * dist[iEle];
    }

    return Sqrt(WiDi2Sum / WiSum);
}

Double_t Mu(vector <Double_t> qp, vector <Double_t> w) {
    if (qp.size() != w.size())
        throw;

    Double_t WiQpSum = 0.;
    Double_t WiSum = 0.;

    for (Int_t iEle = 0; iEle < qp.size(); iEle++) {
        WiSum += w[iEle];
        WiQpSum += w[iEle] * qp[iEle];
    }

    return WiQpSum / WiSum;
}

//void DrawBar(Int_t iEv, Int_t nEv) {
//    cout.flush();
//    Float_t progress = iEv * 1.0 / nEv;
//    Int_t barWidth = 70;
//    printf(ANSI_COLOR_BLUE "[");
//
//    Int_t pos = barWidth * progress;
//    for (Int_t i = 0; i < barWidth; ++i) {
//        if (i < pos) printf("=");
//        else if (i == pos) printf(">");
//        else printf(" ");
//    }
//
//    printf("] " ANSI_COLOR_RESET);
//    printf(ANSI_COLOR_RED "%d%%\r" ANSI_COLOR_RESET, Int_t(progress * 100.0 + 0.5));
//    cout.flush();
//}

void DrawBar(UInt_t iEv, UInt_t nEv) {
    cout.flush();
    Float_t progress = iEv * 1.0 / nEv;
    Int_t barWidth = 70;

    Int_t pos = barWidth * progress;
    for (Int_t i = 0; i < barWidth; ++i) {
        if (i <= pos) printf(ANSI_COLOR_BLUE_BG " " ANSI_COLOR_RESET);
        else printf(ANSI_COLOR_YELLOW_BG " " ANSI_COLOR_RESET);
    }

    printf(ANSI_COLOR_RED "[%d%%]\r" ANSI_COLOR_RESET, Int_t(progress * 100.0 + 0.5));
    cout.flush();
}

void DrawBar(Long64_t iEv, Long64_t nEv) {
    cout.flush();
    Float_t progress = iEv * 1.0 / nEv;
    Int_t barWidth = 70;

    Int_t pos = barWidth * progress;
    for (Int_t i = 0; i < barWidth; ++i) {
        if (i <= pos) printf(ANSI_COLOR_BLUE_BG " " ANSI_COLOR_RESET);
        else printf(ANSI_COLOR_YELLOW_BG " " ANSI_COLOR_RESET);
    }

    printf(ANSI_COLOR_RED "[%d%%]\r" ANSI_COLOR_RESET, Int_t(progress * 100.0 + 0.5));
    cout.flush();
}
