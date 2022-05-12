#include "BmnMath.h"

#include <cmath>
#include <iostream>
#include "CbmGlobalTrack.h"
#include "CbmHit.h"
#include "CbmStripHit.h"
#include "FairTrackParam.h"
#include "Math/BrentRootFinder.h"
#include "Math/WrappedTF1.h"
#include "TArc.h"
#include "TCanvas.h"
#include "TClonesArray.h"
#include "TF1.h"
#include "TFitResult.h"
#include "TGraph.h"
#include "TH2F.h"
#include "TLine.h"
#include "TMath.h"
#include "FairRunAna.h"
#include "BmnFieldMap.h"

using namespace TMath;

namespace lit {


Float_t ChiSq(const FairTrackParam* par, const BmnHit* hit) {
    Float_t dxx = hit->GetDx() * hit->GetDx();
    Float_t dxy = 0.0;  //hit->GetDxy();
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

Int_t NDF(const BmnTrack* track) {
    Int_t ndf = 0;
    for (Int_t i = 0; i < track->GetNHits(); i++)
        ndf += 2;
    ndf -= 5;
    return ndf;
}

}  // namespace lit


//checking with flag isField is needed for backward compatibility
Bool_t IsParCorrect(const FairTrackParam* par, const Bool_t isField) {
    return IsParCorrect(par);
}

Bool_t IsParCorrect(const FairTrackParam* par) {
    const Float_t maxSlopeX = 5.;
    const Float_t maxSlopeY = 1.;
    const Float_t maxX = 300.0;
    const Float_t maxY = 200.0;
    //const Float_t minSlope = 1e-10;
    const Float_t maxQp = 100.;  // p = 10 MeV
    const Float_t minQp = 0.01;  // p = 100 GeV

    if (abs(par->GetTx()) > maxSlopeX || abs(par->GetTy()) > maxSlopeY) return kFALSE;
    if (abs(par->GetX()) > maxX || abs(par->GetY()) > maxY) return kFALSE;
    if (IsNaN(par->GetX()) || IsNaN(par->GetY()) || IsNaN(par->GetTx()) || IsNaN(par->GetTy())) return kFALSE;

    BmnFieldMap* field = (BmnFieldMap*) FairRunAna::Instance()->GetField();
    Bool_t isField = !(field->IsFieldOff());

    if (isField) {
        if (abs(par->GetQp()) > maxQp) return kFALSE;
        if (abs(par->GetQp()) < minQp) return kFALSE;
        if (IsNaN(par->GetQp())) return kFALSE;
    }

    return kTRUE;
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
    Float_t Xi = 0.0, Yi = 0.0;  // coordinates of current track point
    Float_t a = 0.0, b = 0.0;    // parameters of line: y = a * x + b

    Float_t Si = 0.0;      // sigma
    Float_t Wi = 0.0;      // weight = 1 / sigma^2
    Float_t SumW = 0.0;    // sum of weights
    Float_t SumWX = 0.0;   // sum of (weight * x)
    Float_t SumWY = 0.0;   // sum of (weight * y)
    Float_t SumWXY = 0.0;  // sum of (weight * x * y)
    Float_t SumWX2 = 0.0;  // sum of (weight * x * x)

    const Float_t nHits = track->GetNHits();

    for (Int_t i = 0; i < nHits; ++i) {
        BmnHit* hit = (BmnHit*)arr->At(track->GetHitIndex(i));
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
        BmnHit* hit = (BmnHit*)arr->At(track->GetHitIndex(i));
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

        //        chi2 += Sqr((Yi - a * Xi - b) / Si);
        chi2 += Sq((Yi - a * Xi - b) / (a * Xi + b));
    }

    return TVector3(a, b, chi2);
}

TVector3 LineFitBy3Hits(const BmnHit* h0, const BmnHit* h1, const BmnHit* h2) {
    //Weighted Least Square Method//

    // sigma
    Double_t S0 = h0->GetDy();
    Double_t S1 = h1->GetDy();
    Double_t S2 = h2->GetDy();
    // weight = 1 / sigma^2
    Double_t W0 = 1.0 / S0 / S0;
    Double_t W1 = 1.0 / S1 / S1;
    Double_t W2 = 1.0 / S2 / S2;

    Double_t X0 = h0->GetZ();
    Double_t X1 = h1->GetZ();
    Double_t X2 = h2->GetZ();

    Double_t Y0 = h0->GetY();
    Double_t Y1 = h1->GetY();
    Double_t Y2 = h2->GetY();

    Double_t SumW = W0 + W1 + W2;                                  // sum of weights
    Double_t SumWX = W0 * X0 + W1 * X1 + W2 * X2;                  // sum of (weight * x)
    Double_t SumWY = W0 * Y0 + W1 * Y1 + W2 * Y2;                  // sum of (weight * y)
    Double_t SumWXY = W0 * X0 * Y0 + W1 * X1 * Y1 + W2 * X2 * Y2;  // sum of (weight * x * y)
    Double_t SumWX2 = W0 * X0 * X0 + W1 * X1 * X1 + W2 * X2 * X2;  // sum of (weight * x * x)

    // parameters of line: y = a * x + b
    Double_t a = (SumW * SumWXY - SumWX * SumWY) / (SumW * SumWX2 - SumWX * SumWX);
    Double_t b = (SumWX2 * SumWY - SumWX * SumWXY) / (SumW * SumWX2 - SumWX * SumWX);

    Double_t chi2 = Sq((Y0 - a * X0 - b) / S0) + Sq((Y1 - a * X1 - b) / S1) + Sq((Y2 - a * X2 - b) / S2);

    return TVector3(a, b, chi2);
}

void LineFit(Double_t& par1, Double_t& par2, BmnTrack* track, TClonesArray* arr, Int_t type, Int_t idSkip) {
    //Weighted Least Square Method//
    Float_t Xi = 0.0, Yi = 0.0;  // coordinates of current track point
    Float_t a = 0.0, b = 0.0;    // parameters of line: y = a * x + b

    Float_t Si = 0.0;      // sigma
    Float_t Wi = 0.0;      // weight = 1 / sigma^2
    Float_t SumW = 0.0;    // sum of weights
    Float_t SumWX = 0.0;   // sum of (weight * x)
    Float_t SumWY = 0.0;   // sum of (weight * y)
    Float_t SumWXY = 0.0;  // sum of (weight * x * y)
    Float_t SumWX2 = 0.0;  // sum of (weight * x * x)

    const Float_t nHits = track->GetNHits();
    for (Int_t i = 0; i < nHits; ++i) {
        BmnHit* hit = (BmnHit*)arr->At(track->GetHitIndex(i));

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
        BmnHit* hit = (BmnHit*)arr->At(track->GetHitIndex(i));

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

TVector3 CircleFit(BmnTrack* track, const TClonesArray* arr, Double_t& chi2) {
    //Weighted Least Square Method//
    Double_t Xi = 0.0, Yi = 0.0, Zi = 0.0;  // coordinates of current track point
    Double_t Xc = 0.0, Zc = 0.0, R = 0.0;
    chi2 = 0.0;

    Double_t Wi = 0.0;  // weight = 1 / sigma^2

    Double_t Sx = 0.0;  // sum of weights
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
        BmnHit* hit = (BmnHit*)arr->At(track->GetHitIndex(i));
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

    for (Int_t i = 0; i < nHits; ++i) {
        BmnHit* hit = (BmnHit*)arr->At(track->GetHitIndex(i));
        chi2 += Sq((R - Sqrt(Sq(hit->GetX() - Xc) + Sq(hit->GetZ() - Zc))) / hit->GetDx());
    }

    return TVector3(Zc, Xc, R);
}

TVector3 CircleFit(vector<BmnHit*> hits, Int_t idSkip) {
    //Weighted Least Square Method//
    Double_t Xi = 0.0, Yi = 0.0, Zi = 0.0;  // coordinates of current track point
    Double_t Xc = 0.0, Zc = 0.0, R = 0.0;

    Double_t Wi = 0.0;  // weight = 1 / sigma^2

    Double_t Sx = 0.0;  // sum of weights
    Double_t Sxx = 0.0;
    Double_t Syy = 0.0;
    Double_t Sxy = 0.0;
    Double_t Sy = 0.0;
    Double_t Sz = 0.0;
    Double_t Szx = 0.0;
    Double_t Szy = 0.0;

    //        TH2F* h_Hits = new TH2F("h_Hits", "h_Hits", 400, 0.0, 250.0, 400, -10.0, 10.0);

    const Float_t nHits = hits.size();
    for (Int_t i = 0; i < nHits; ++i) {
        if (i == idSkip) continue;
        BmnHit* hit = hits[i];
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

    return TVector3(Zc, Xc, R);
}

Double_t CalcTx(const BmnHit* h0, const BmnHit* h1, const BmnHit* h2) {
    //function calculates Tx in point h0, so for TX_last use reverse order: CalcTx(h2, h1, h0)
    TVector3 CircParZX = CircleBy3Hit(h0, h1, h2);
    Double_t Xc = CircParZX.Y();  // x-coordinate of fit-circle center
    Double_t Zc = CircParZX.X();  // z-coordinate of fit-circle center

    return (-1.0 * (h0->GetZ() - Zc) / (h0->GetX() - Xc));
}

Float_t Dist(Float_t x1, Float_t y1, Float_t x2, Float_t y2) {
    if (Sq(x1 - x2) + Sq(y1 - y2) <= 0.0) {
        return 0.0;
    } else {
        return Sqrt(Sq(x1 - x2) + Sq(y1 - y2));
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

TVector3 CircleBy3Hit(BmnTrack* track, const TClonesArray* arr) {
    const Float_t nHits = track->GetNHits();
    if (nHits < 3) return TVector3(0.0, 0.0, 0.0);
    BmnHit* hit0 = (BmnHit*)arr->At(track->GetHitIndex(0));
    BmnHit* hit1 = (BmnHit*)arr->At(track->GetHitIndex(1));
    BmnHit* hit2 = (BmnHit*)arr->At(track->GetHitIndex(2));

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

TVector3 Pol2By3Hit(BmnTrack* track, const TClonesArray* arr) {
    const Int_t nHits = track->GetNHits();
    if (nHits < 3) return TVector3(0.0, 0.0, 0.0);
    BmnHit* hit0 = (BmnHit*)arr->At(track->GetHitIndex(0));
    BmnHit* hit1 = (BmnHit*)arr->At(track->GetHitIndex(1));
    BmnHit* hit2 = (BmnHit*)arr->At(track->GetHitIndex(2));

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

void DrawHits(BmnTrack* track, const TClonesArray* arr) {
    const Int_t nHits = track->GetNHits();
    Float_t z[nHits];
    Float_t x[nHits];
    for (Int_t i = 0; i < nHits; ++i) {
        z[i] = ((BmnHit*)arr->At(track->GetHitIndex(i)))->GetZ();
        x[i] = ((BmnHit*)arr->At(track->GetHitIndex(i)))->GetX();
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

TVector3 CircleBy3Hit(const BmnHit* h0, const BmnHit* h1, const BmnHit* h2) {
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
        {0, 0, 0, 0}};   //coef matrix
    Float_t f[4] = {0};  //free coef
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
    }  //i

    A[0][0] = 2 * z_loc[0] * z_loc[0] * h[0] / rh_sigm_seg[0] + z_loc[4] * z_loc[4] * h[4] / rh_sigm_seg[4] + z_loc[6] * z_loc[6] * h[6] / rh_sigm_seg[6] +
              2 * z_loc[1] * z_loc[1] * h[1] / rh_sigm_seg[1] + z_loc[5] * z_loc[5] * h[5] / rh_sigm_seg[5] + z_loc[7] * z_loc[7] * h[7] / rh_sigm_seg[7];  //aX_a

    A[0][1] = 2 * z_loc[0] * h[0] / rh_sigm_seg[0] + z_loc[4] * h[4] / rh_sigm_seg[4] + z_loc[6] * h[6] / rh_sigm_seg[6] +
              2 * z_loc[1] * h[1] / rh_sigm_seg[1] + z_loc[5] * h[5] / rh_sigm_seg[5] + z_loc[7] * h[7] / rh_sigm_seg[7];  //bX_a

    A[0][2] = z_loc[6] * z_loc[6] * h[6] / rh_sigm_seg[6] - z_loc[4] * z_loc[4] * h[4] / rh_sigm_seg[4] +
              z_loc[7] * z_loc[7] * h[7] / rh_sigm_seg[7] - z_loc[5] * z_loc[5] * h[5] / rh_sigm_seg[5];  //aY

    A[0][3] = z_loc[6] * h[6] / rh_sigm_seg[6] - z_loc[4] * h[4] / rh_sigm_seg[4] + z_loc[7] * h[7] / rh_sigm_seg[7] - z_loc[5] * h[5] / rh_sigm_seg[5];  //bY_a

    //dChi2/d_b_x

    A[1][0] = 2 * z_loc[0] * h[0] / rh_sigm_seg[0] + z_loc[4] * h[4] / rh_sigm_seg[4] + z_loc[6] * h[6] / rh_sigm_seg[6] + 2 * z_loc[1] * h[1] / rh_sigm_seg[1] + z_loc[5] * h[5] / rh_sigm_seg[5] + z_loc[7] * h[7] / rh_sigm_seg[7];

    A[1][1] = 2 * h[0] / rh_sigm_seg[0] + 1 * h[4] / rh_sigm_seg[4] + 1 * h[6] / rh_sigm_seg[6] + 2 * h[1] / rh_sigm_seg[1] + 1 * h[5] / rh_sigm_seg[5] + 1 * h[7] / rh_sigm_seg[7];  //bX_a

    A[1][2] = z_loc[6] * h[6] / rh_sigm_seg[6] - z_loc[4] * h[4] / rh_sigm_seg[4] + z_loc[7] * h[7] / rh_sigm_seg[7] - z_loc[5] * h[5] / rh_sigm_seg[5];  //aY_a

    A[1][3] = 1 * h[7] / rh_sigm_seg[7] - 1 * h[5] / rh_sigm_seg[5] + 1 * h[6] / rh_sigm_seg[6] - 1 * h[4] / rh_sigm_seg[4];  //bY_a

    //dChi2/da_y

    A[2][0] = z_loc[6] * z_loc[6] * h[6] / rh_sigm_seg[6] - z_loc[4] * z_loc[4] * h[4] / rh_sigm_seg[4] + z_loc[7] * z_loc[7] * h[7] / rh_sigm_seg[7] - z_loc[5] * z_loc[5] * h[5] / rh_sigm_seg[5];  //aX_a

    A[2][1] = z_loc[6] * h[6] / rh_sigm_seg[6] - z_loc[4] * h[4] / rh_sigm_seg[4] + z_loc[7] * h[7] / rh_sigm_seg[7] - z_loc[5] * h[5] / rh_sigm_seg[5];  //bX_a

    A[2][2] = 2 * z_loc[2] * z_loc[2] * h[2] / rh_sigm_seg[2] + z_loc[4] * z_loc[4] * h[4] / rh_sigm_seg[4] + z_loc[6] * z_loc[6] * h[6] / rh_sigm_seg[6] + 2 * z_loc[3] * z_loc[3] * h[3] / rh_sigm_seg[3] + z_loc[5] * z_loc[5] * h[5] / rh_sigm_seg[5] + z_loc[7] * z_loc[7] * h[7] / rh_sigm_seg[7];  //aY_a

    A[2][3] = 2 * z_loc[2] * h[2] / rh_sigm_seg[2] + z_loc[4] * h[4] / rh_sigm_seg[4] + z_loc[6] * h[6] / rh_sigm_seg[6] + 2 * z_loc[3] * h[3] / rh_sigm_seg[3] + z_loc[5] * h[5] / rh_sigm_seg[5] + z_loc[7] * h[7] / rh_sigm_seg[7];

    ////dChi2/db_y

    A[3][0] = z_loc[6] * h[6] / rh_sigm_seg[6] - z_loc[4] * h[4] / rh_sigm_seg[4] + z_loc[7] * h[7] / rh_sigm_seg[7] - z_loc[5] * h[5] / rh_sigm_seg[5];  //aX_a

    A[3][1] = 1 * h[6] / rh_sigm_seg[6] - 1 * h[4] / rh_sigm_seg[4] + 1 * h[7] / rh_sigm_seg[7] - 1 * h[5] / rh_sigm_seg[5];  //bX_a

    A[3][2] = 2 * z_loc[2] * h[2] / rh_sigm_seg[2] + z_loc[4] * h[4] / rh_sigm_seg[4] + z_loc[6] * h[6] / rh_sigm_seg[6] + 2 * z_loc[3] * h[3] / rh_sigm_seg[3] + z_loc[5] * h[5] / rh_sigm_seg[5] + z_loc[7] * h[7] / rh_sigm_seg[7];  //aY_a

    A[3][3] = 2 * h[2] / rh_sigm_seg[2] + 1 * h[4] / rh_sigm_seg[4] + 1 * h[6] / rh_sigm_seg[6] + 2 * h[3] / rh_sigm_seg[3] + 1 * h[5] / rh_sigm_seg[5] + 1 * h[7] / rh_sigm_seg[7];  //bY_a

    //free coef

    //dChi2/da_x

    f[0] = 2 * z_loc[0] * rh_seg[0] * h[0] / rh_sigm_seg[0] + sqrt_2 * z_loc[6] * rh_seg[6] * h[6] / rh_sigm_seg[6] - sqrt_2 * z_loc[4] * rh_seg[4] * h[4] / rh_sigm_seg[4] +
           2 * z_loc[1] * rh_seg[1] * h[1] / rh_sigm_seg[1] + sqrt_2 * z_loc[7] * rh_seg[7] * h[7] / rh_sigm_seg[7] - sqrt_2 * z_loc[5] * rh_seg[5] * h[5] / rh_sigm_seg[5];  //j = nr of seg
    //dChi2/db_x
    f[1] = 2 * rh_seg[0] * h[0] / rh_sigm_seg[0] + sqrt_2 * rh_seg[6] * h[6] / rh_sigm_seg[6] - sqrt_2 * rh_seg[4] * h[4] / rh_sigm_seg[4] +
           2 * rh_seg[1] * h[1] / rh_sigm_seg[1] + sqrt_2 * rh_seg[7] * h[7] / rh_sigm_seg[7] - sqrt_2 * rh_seg[5] * h[5] / rh_sigm_seg[5];  //j = nr of seg
    //dChi2/da_y
    f[2] = 2 * z_loc[2] * rh_seg[2] * h[2] / rh_sigm_seg[2] + sqrt_2 * z_loc[6] * rh_seg[6] * h[6] / rh_sigm_seg[6] + sqrt_2 * z_loc[4] * rh_seg[4] * h[4] / rh_sigm_seg[4] +
           2 * z_loc[3] * rh_seg[3] * h[3] / rh_sigm_seg[3] + sqrt_2 * z_loc[7] * rh_seg[7] * h[7] / rh_sigm_seg[7] + sqrt_2 * z_loc[5] * rh_seg[5] * h[5] / rh_sigm_seg[5];  //j = nr of seg
    ////dChi2/db_y
    f[3] = 2 * rh_seg[2] * h[2] / rh_sigm_seg[2] + sqrt_2 * rh_seg[6] * h[6] / rh_sigm_seg[6] + sqrt_2 * rh_seg[4] * h[4] / rh_sigm_seg[4] +
           2 * rh_seg[3] * h[3] / rh_sigm_seg[3] + sqrt_2 * rh_seg[7] * h[7] / rh_sigm_seg[7] + sqrt_2 * rh_seg[5] * h[5] / rh_sigm_seg[5];  //j = nr of seg

    //inverse the matrix

    /**** Gaussian algorithm for 4x4 matrix inversion ****/
    Int_t i1, j1, k1, l1;
    Double_t factor;
    Double_t temp[4];
    Double_t b[4][4];
    Double_t A0[4][4];

    for (i1 = 0; i1 < 4; i1++)
        for (j1 = 0; j1 < 4; j1++) A0[i1][j1] = A[i1][j1];

    // Set b to I
    for (i1 = 0; i1 < 4; i1++)
        for (j1 = 0; j1 < 4; j1++)
            if (i1 == j1)
                b[i1][j1] = 1.0;
            else
                b[i1][j1] = 0.0;

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
        for (j1 = 4 - 1; j1 > -1; j1--) {
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
        for (j1 = i1 - 1; j1 > -1; j1--) {
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
        {0, 0, 0, 0}};

    for (i1 = 0; i1 < 4; ++i1)
        for (j1 = 0; j1 < 4; ++j1) {
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

void Pol2Fit(BmnTrack* track, const TClonesArray* arr, Double_t& A, Double_t& B, Double_t& C, Int_t idSkip) {
    const Float_t nHits = track->GetNHits();
    TGraph* gr = new TGraph(nHits - 1);
    Int_t iPoint = 0;
    for (Int_t i = 0; i < nHits; ++i) {
        if (i == idSkip) continue;
        BmnHit* hit = (BmnHit*)arr->At(track->GetHitIndex(i));
        gr->SetPoint(iPoint++, hit->GetZ(), hit->GetX());
    }
    TFitResultPtr ptr = gr->Fit("pol2", "SQ");
    delete gr;
    A = ptr->Parameter(2);
    B = ptr->Parameter(1);
    C = ptr->Parameter(0);
}

TVector3 Pol2Fit(vector<BmnHit*> hits, Int_t idSkip) {
    const Int_t nHits = hits.size();
    TGraph gr(nHits - 1);
    Int_t iPoint = 0;
    for (Int_t i = 0; i < nHits; ++i) {
        if (i == idSkip) continue;
        BmnHit* hit = hits[i];
        gr.SetPoint(iPoint++, hit->GetZ(), hit->GetX());
    }
    TFitResultPtr ptr = gr.Fit("pol2", "SQ");
    Double_t A = ptr->Parameter(2);
    Double_t B = ptr->Parameter(1);
    Double_t C = ptr->Parameter(0);

    return TVector3(A, B, C);
}

TVector2 LineFit(vector<BmnHit*> hits, Int_t idSkip, TString type) {
    //Weighted Least Square Method//
    Float_t Xi = 0.0, Yi = 0.0;  // coordinates of current track point
    Float_t a = 0.0, b = 0.0;    // parameters of line: y = a * x + b

    Float_t Si = 0.0;      // sigma
    Float_t Wi = 0.0;      // weight = 1 / sigma^2
    Float_t SumW = 0.0;    // sum of weights
    Float_t SumWX = 0.0;   // sum of (weight * x)
    Float_t SumWY = 0.0;   // sum of (weight * y)
    Float_t SumWXY = 0.0;  // sum of (weight * x * y)
    Float_t SumWX2 = 0.0;  // sum of (weight * x * x)

    const Int_t nHits = hits.size();

    for (Int_t i = 0; i < nHits; ++i) {
        BmnHit* hit = hits[i];
        if (i == idSkip)
            continue;
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

        if (Si == 0.0) return TVector2(0.0, 0.0);

        Wi = 1.0 / Si / Si;
        SumW += Wi;
        SumWXY += Wi * Xi * Yi;
        SumWX += Wi * Xi;
        SumWX2 += Wi * Xi * Xi;
        SumWY += Wi * Yi;
    }

    a = (SumW * SumWXY - SumWX * SumWY) / (SumW * SumWX2 - SumWX * SumWX);
    b = (SumWX2 * SumWY - SumWX * SumWXY) / (SumW * SumWX2 - SumWX * SumWX);

    return TVector2(a, b);
}

vector<Double_t> dist(vector<Double_t> qp, Double_t mu) {
    vector<Double_t> res;
    for (Int_t iEle = 0; iEle < qp.size(); iEle++)
        res.push_back(Abs(qp[iEle] - mu));

    return res;
}

vector<Double_t> W(vector<Double_t> dist, Double_t sig) {
    vector<Double_t> res;
    const Int_t C = 10;
    for (Int_t iEle = 0; iEle < dist.size(); iEle++) {
        Double_t w = (dist[iEle] > C * sig) ? 0. : Power(1 - Power(dist[iEle] / C / sig, 2), 2);
        res.push_back(w);
    }

    return res;
}

Double_t Sigma(vector<Double_t> dist, vector<Double_t> w) {
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

Double_t Mu(vector<Double_t> qp, vector<Double_t> w) {
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
    if (nEv < 100)
        return;
    if ((iEv % (nEv / 100)) > 100.0 / nEv)
        return;
    Float_t progress = iEv * 1.0 / nEv;
    Int_t barWidth = 70;
    Int_t pos = barWidth * progress;
    cout.flush();
    for (Int_t i = 0; i < barWidth; ++i) {
        if (i <= pos)
            printf(ANSI_COLOR_BLUE_BG " " ANSI_COLOR_RESET);
        else
            printf(ANSI_COLOR_YELLOW_BG " " ANSI_COLOR_RESET);
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
        if (i <= pos)
            printf(ANSI_COLOR_BLUE_BG " " ANSI_COLOR_RESET);
        else
            printf(ANSI_COLOR_YELLOW_BG " " ANSI_COLOR_RESET);
    }

    printf(ANSI_COLOR_RED "[%d%%]\r" ANSI_COLOR_RESET, Int_t(progress * 100.0 + 0.5));
    cout.flush();
}

Double_t GetVZByTwoStraightTracks(BmnTrack* tr0, BmnTrack* tr1, Double_t& dist) {
    //method returns z coordinate of two tracks crossing point in case of absent magnetic field
    //dist is filling by distance between this two tracks
    FairTrackParam* p0 = tr0->GetParamFirst();
    FairTrackParam* p1 = tr1->GetParamFirst();
    Double_t axl = p0->GetTx();
    Double_t ayl = p0->GetTy();
    Double_t axr = p1->GetTx();
    Double_t ayr = p1->GetTy();
    Double_t bxl = p0->GetX() - axl * p0->GetZ();
    Double_t byl = p0->GetY() - ayl * p0->GetZ();
    Double_t bxr = p1->GetX() - axr * p1->GetZ();
    Double_t byr = p1->GetY() - ayr * p1->GetZ();
    Double_t vz = -((bxl - bxr) * (axl - axr) + (byl - byr) * (ayl - ayr)) / ((axl - axr) * (axl - axr) + (ayl - ayr) * (ayl - ayr));
    dist = Sqrt(Sq(axl * vz + bxl - axr * vz - bxr) + Sq(ayl * vz + byl - ayr * vz - byr));
    return vz;
}

Double_t GetVzByVectorStraightTracks(vector<BmnTrack> tr, Double_t& dist) {
    if (tr.size() < 2) {
        dist = 1000;
        return 1000;
    }
    Double_t A = 0;
    Double_t B = 0;
    for (Int_t i = 0; i < tr.size() - 1; ++i) {
        FairTrackParam* pi = tr[i].GetParamFirst();
        Double_t axi = pi->GetTx();
        Double_t ayi = pi->GetTy();
        Double_t bxi = pi->GetX() - axi * pi->GetZ();
        Double_t byi = pi->GetY() - ayi * pi->GetZ();
        for (Int_t j = i + 1; j < tr.size(); ++j) {
            FairTrackParam* pj = tr[j].GetParamFirst();
            Double_t axj = pj->GetTx();
            Double_t ayj = pj->GetTy();
            Double_t bxj = pj->GetX() - axj * pj->GetZ();
            Double_t byj = pj->GetY() - ayj * pj->GetZ();
            A += (Sq(axi - axj) + Sq(ayi - ayj));
            B += ((bxi - bxj) * (axi - axj) + (byi - byj) * (ayi - ayj));
        }
    }
    Double_t vz = -B / A;

    Double_t SumDist = 0;
    Int_t cntr = 0;
    for (Int_t i = 0; i < tr.size() - 1; ++i) {
        FairTrackParam* pi = tr[i].GetParamFirst();
        Double_t axi = pi->GetTx();
        Double_t ayi = pi->GetTy();
        Double_t bxi = pi->GetX() - axi * pi->GetZ();
        Double_t byi = pi->GetY() - ayi * pi->GetZ();
        for (Int_t j = i + 1; j < tr.size(); ++j) {
            FairTrackParam* pj = tr[j].GetParamFirst();
            Double_t axj = pj->GetTx();
            Double_t ayj = pj->GetTy();
            Double_t bxj = pj->GetX() - axj * pj->GetZ();
            Double_t byj = pj->GetY() - ayj * pj->GetZ();
            SumDist += Sqrt(Sq(axi * vz + bxi - axj * vz - bxj) + Sq(ayi * vz + byi - ayj * vz - byj));
            cntr++;
        }
    }
    dist = SumDist / cntr;

    return vz;
}


void UpdateTrackParam(FairTrackParam* initPar, const FairTrackParam* detPar, Double_t& chiSq) {

    //		pardet(5) - det track parameters
    //		covdet(5) - det track parameters covariation matrix
    //		parinit(5) - init track parameters
    //		covinit(5) - init track parameters covariation matrix
    //		parinitdet(5) - init+det track parameters
    //		covinitdet(5) - init+det track parameters covariation matrix
    //		detPar - det track param
    //		initTr - Global track param

    TMatrixDSym covdet(5), icovdet(5);
    covdet *= 0.;
    for (int ir = 0; ir < 5; ir++) {
        covdet(ir, ir) = detPar->GetCovariance(ir, ir);
        if (detPar->GetCovariance(ir, ir) == 0) covdet(ir, ir) = 0.00001;
    }
    covdet(4, 4) = 10000.;

    icovdet = covdet;
    icovdet.Invert();

    TMatrixDSym covinit(5), icovinit(5);
    covinit *= 0.;
    for (int ir = 0; ir < 5; ir++) for (int ic = 0; ic < 5; ic++) covinit(ir, ic) = initPar->GetCovariance(ir, ic);
    icovinit = covinit;
    icovinit.Invert();

    TMatrixDSym covinitdet(5), icovinitdet(5);
    icovinitdet *= 0.;
    icovinitdet += icovinit;
    icovinitdet += icovdet;
    covinitdet = icovinitdet;
    covinitdet.Invert();

    TVectorD suminitdet(5), parinitdet(5);
    TVectorD parinit(5), pardet(5);
    pardet(0) = detPar->GetX();
    pardet(1) = detPar->GetY();
    pardet(2) = detPar->GetTx();
    pardet(3) = detPar->GetTy();
    pardet(4) = 1.;
    parinit(0) = initPar->GetX();
    parinit(1) = initPar->GetY();
    parinit(2) = initPar->GetTx();
    parinit(3) = initPar->GetTy();
    parinit(4) = initPar->GetQp();
    suminitdet *= 0.;
    suminitdet += icovinit * parinit;
    suminitdet += icovdet * pardet;
    parinitdet = covinitdet * suminitdet;

    //Set Updated parameters

    initPar->SetX(parinitdet(0));
    initPar->SetY(parinitdet(1));
    initPar->SetTx(parinitdet(2));
    initPar->SetTy(parinitdet(3));
    initPar->SetQp(parinitdet(4));
    initPar->SetCovMatrix(covinitdet);

    //Calculate chiSq 

    TMatrixD dP2(5, 1);

    dP2(0, 0) = parinitdet(0) - pardet(0);
    dP2(1, 0) = parinitdet(1) - pardet(1);
    dP2(2, 0) = parinitdet(2) - pardet(2);
    dP2(3, 0) = parinitdet(3) - pardet(3);
    dP2(4, 0) = parinitdet(4) - pardet(4);

    TMatrixD dP2_T(1, 5);

    dP2_T.Transpose(dP2);

    //-----------------------

    TMatrixD dP1(5, 1);

    dP1(0, 0) = parinitdet(0) - parinit(0);
    dP1(1, 0) = parinitdet(1) - parinit(1);
    dP1(2, 0) = parinitdet(2) - parinit(2);
    dP1(3, 0) = parinitdet(3) - parinit(3);
    dP1(4, 0) = parinitdet(4) - parinit(4);

    TMatrixD dP1_T(1, 5);

    dP1_T.Transpose(dP1);

    TMatrixD chi(1, 1);
    chi = dP2_T * icovdet * dP2 + dP1_T * icovinit * dP1;
    //chi = dP2_T*icovdet*dP2 + dP1_T*icovinitdet*dP1;  

    chiSq = chi(0, 0);
}
