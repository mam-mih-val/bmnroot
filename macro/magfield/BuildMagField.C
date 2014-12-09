#include <fcntl.h>
#include <math.h>
#include <cmath>

#include "TCanvas.h"
#include "TF1.h"
#include "TPaveText.h"
#include "TGraph.h"
#include "TGraph2D.h"
#include "TVector3.h"
#include "TH1F.h"
#include "TGraphDelaunay.h"
#include "TFitResultPtr.h"
#include "TFitResult.h"
#include "TFile.h"

using namespace std;
using namespace TMath;

class BmnFieldMap;

// Approx. is done by Gauss's functions over all field components

// File to write parameters (const, mean, sigma) of the mag.field approx
TFile f("ApproxFieldParams.root", "recreate");
TString nameOutput("field_sp41v1_ascii_Extrap.dat");
const Bool_t isDrawProfile = kFALSE; // kTRUE;
const Double_t Zmax = 600.; // Upper Z-limit of the used extrapolation
const Double_t Zmin = 0.0; // Lower Z-limit of the used extrapolation
const Double_t Zleft = 260.; // Lower Z-limit to find extrapolation functions 
const Double_t Zright = 320.; // Upper Z-limit to find extrapolation functions 
const Double_t Zedge = 198.;  // Zedge corresponds to endpoint of the field map used in Z-direction
const Int_t Npoints = 850; // Number of nodes along Z-axis
const Int_t dimGrid = 15; // dimGrid x dimGrid corresponds to number of nodes to build a grid along X(Y)-axis 
const Int_t dimNhist = 5; // dimNhist corresponds to number of profiles drawn
const Double_t Zleft_new = 285.;    // Zleft redefined to check extrap. quality in the field map
const Double_t Zright_new = Zright; // Zright redefined to check extrap. quality in the field map
const Int_t N = 3;   // N x N corresponds to number of Z-slices, extrap. quality
const Int_t Nx = 15; // Number of bins, X-axis, extrap. quality
const Int_t Ny = 15; // Number of bins, Y-axis, extrap. quality
const Double_t const_restrict = 5000.; // ??? DO WE NEED IT?
const Double_t factor = 10.; // Transition from T to kG
const Int_t fNz = 300; // Defines endpoint along Z-axis: Zmin + step * (fZ-1), Z = 598 cm (at present!)

void BuildMagField() {

    gROOT->LoadMacro("$VMCWORKDIR/macro/run/bmnloadlibs.C");
    bmnloadlibs(); // load libraries

    Double_t fieldZ = 124.5; // field centre z position
    Double_t fieldScale = 1.;
    BmnFieldMap* magField = new BmnFieldMapSym3("field_sp41v1_ascii_noExtrap.dat");
    magField->SetPosition(0., 0., fieldZ);
    magField->SetScale(fieldScale);
    magField->Init();
    //    magField->Print();

    TString MagFieldComp[3] = {"x", "y", "z"};

    for (Int_t i = 0; i < 3; i++)
        PlotFieldProfiles(magField, MagFieldComp[i], isDrawProfile);
    
    CreateNewMap(magField, fieldZ, f);
    f.Close();
}

void PlotFieldProfiles(BmnFieldMap* magField, TString fcomp, Bool_t isDrawProfile) {

    const Double_t Ymax = magField->GetYmax() - 1.; // Define Ymax_new = 53 cm (54 - 1)
    const Double_t Ymin = -Ymax;

    const Double_t Xmax = magField->GetXmax() - 1.; // Define Ymax_new = 227 cm (227 - 1)
    const Double_t Xmin = -Xmax;

    // Create graphs to collect data on Constant, Mean, Sigma and Field Profiles
    TGraph2D* constant = new TGraph2D();
    TGraph2D* mean = new TGraph2D();
    TGraph2D* sigma = new TGraph2D();
    TGraph*** bField = new TGraph**[dimGrid];

    // Define number of slices on X(Y)-dimension of the used grid
    const Int_t nSlicesX = dimGrid - 1;
    const Int_t nSlicesY = dimGrid - 1;

    // Define step along X(Y, Z)-axises of the used grid
    const Double_t dx = (Xmax - Xmin) / nSlicesX;
    const Double_t dy = (Ymax - Ymin) / nSlicesY;
    const Double_t dz = (Zmax - Zmin) / Npoints;

    Int_t counter = 0;
    const Int_t shift = 0;

    TVector3 params(0., 0., 0.); // Vector to store fit parameters

    for (Int_t i = 0; i < dimGrid; i++) {

        bField[i] = new TGraph*[dimGrid];

        Double_t xSlice = Xmin + (i + shift) * dx;

        for (Int_t j = 0; j < dimGrid; j++) {
            Float_t ySlice = Ymin + (j + shift) * dy;
            bField[i][j] = new TGraph();
            CreateGraphs(bField[i][j], xSlice, ySlice, "B" + fcomp + " = f(x, y, z) ", "x = ", "y = ");
            for (Int_t k = 0; k < Npoints; k++) {
                Double_t z = Zmin + k * dz;
                bField[i][j]->SetPoint(k, z, (fcomp == "x") ? magField->GetBx(xSlice, ySlice, z) :
                        (fcomp == "y") ? magField->GetBy(xSlice, ySlice, z) :
                        (fcomp == "z") ? magField->GetBz(xSlice, ySlice, z));
            }

            // if (bField[i][j]->Fit("gaus", "SQ", "", Zleft, Zright)->IsValid() == kFALSE || (int)(bField[i][j]->Fit("gaus", "SQ", "", Zleft, Zright)) == 4) continue;
            Double_t* fit_par = bField[i][j]->Fit("gaus", "SQ", "", Zleft, Zright)->GetParams();
              
            if (Abs(fit_par[0]) < const_restrict)
                params.SetXYZ(fit_par[0], fit_par[1], fit_par[2]);

            constant->SetPoint(counter, xSlice, ySlice, params.X());
            mean->SetPoint(counter, xSlice, ySlice, params.Y());
            sigma->SetPoint(counter, xSlice, ySlice, params.Z());

            counter++;
        }
    }

    if (isDrawProfile) {
        ExtrapQuality(Xmin, Xmax, Ymin, Ymax, magField, fcomp, constant, mean, sigma);

        TF1*** fit_func = new TF1**[dimNhist];

        for (Int_t i = 0; i < dimNhist; i++) {
            fit_func[i] = new TF1*[dimNhist];
            Double_t x = Xmin + (i + shift) * dx;

            for (Int_t j = 0; j < dimNhist; j++) {
                Double_t y = Ymin + (j + shift) * dy;
                fit_func[i][j] = new TF1("func", "[0]*exp(-0.5*((x-[1])/[2])**2)", Zleft, Zmax);
                fit_func[i][j]->SetParameter(0, constant->Interpolate(x, y));
                fit_func[i][j]->SetParameter(1, mean->Interpolate(x, y));
                fit_func[i][j]->SetParameter(2, sigma->Interpolate(x, y));
            }
        }

        DrawProfileExtrap(dimNhist, bField, fit_func, "B" + fcomp + "_extrap.eps", fcomp);
        DrawProfile(dimNhist, bField, "B" + fcomp + "_orig.eps");
        DrawGraph(constant, "B" + fcomp + "_gauss_const.eps");
        DrawGraph(mean, "B" + fcomp + "_gauss_mean.eps");
        DrawGraph(sigma, "B" + fcomp + "_gaus_sigma.eps");
    }

    constant->Write("B" + fcomp + "Const");
    mean->Write("B" + fcomp + "Mean");
    sigma->Write("B" + fcomp + "Sigma");
}

void DrawProfile(Int_t dimGrid, TGraph*** gr, TString name) {

    TCanvas* c = new TCanvas("", "", 1200, 800);
    c->Divide(dimGrid, dimGrid);

    Int_t counter = 1;

    for (Int_t i = 0; i < dimGrid; i++)
        for (Int_t j = 0; j < dimGrid; j++) {

            c->GetPad(counter)->SetGridx();
            c->GetPad(counter)->SetGridy();
            c->cd(counter);
            gr[i][j]->Draw();

            counter++;

            gr[i][j]->GetXaxis()->SetTitleSize(0.05);
            gr[i][j]->GetXaxis()->SetLabelSize(0.05);
            gr[i][j]->GetYaxis()->SetTitleSize(0.05);
            gr[i][j]->GetYaxis()->SetLabelSize(0.05);
        }

    c->SaveAs(name);
    delete c;
}

void DrawProfileExtrap(Int_t dimGrid, TGraph*** gr, TF1*** f, TString name, TString fcomp) {

    TCanvas* c = new TCanvas("", "", 1200, 800);
    c->Divide(dimGrid, dimGrid);
    Int_t counter = 1;
    for (Int_t i = 0; i < dimGrid; i++)
        for (Int_t j = 0; j < dimGrid; j++) {
            c->GetPad(counter)->SetGridx();
            c->GetPad(counter)->SetGridy();
            c->cd(counter);
            gr[i][j]->Draw();

            gr[i][j]->GetXaxis()->SetTitleSize(0.05);
            gr[i][j]->GetXaxis()->SetLabelSize(0.07);
            gr[i][j]->GetYaxis()->SetTitleSize(0.05);
            gr[i][j]->GetYaxis()->SetLabelSize(0.07);
            gr[i][j]->GetXaxis()->SetTitle("Z, cm");
            gr[i][j]->GetYaxis()->SetTitle("B" + fcomp + ", kG");
            counter++;
            // cout << "par0 = " << f[i][j]->GetParameter(0) << endl;
            f[i][j]->Draw("same");
        }
    c->SaveAs(name);
    delete c;
}

void DrawGraph(TGraph2D* gr, TString name) {

    TCanvas* c = new TCanvas("", "", 1200, 800);
    gr->Draw("surf1");
    c->SaveAs(name);
    delete c;
}

void ExtrapQuality(Double_t Xmin, Double_t Xmax, Double_t Ymin, Double_t Ymax, BmnFieldMap* field, TString fcomp, TGraph2D* C, TGraph2D* mu, TGraph2D* sigma) {

    const Int_t Nz = N * N;

    TGraph2D* field_value[Nz];
    TGraph2D* field_extrap_value[Nz];
    TGraph2D* diff[Nz];
    Int_t counter = 0;

    const Double_t dX = (Xmax - Xmin) / Nx;
    const Double_t dY = (Ymax - Ymin) / Ny;
    const Double_t dZ = (Zright_new - Zleft_new) / Nz;

    TCanvas* c1 = new TCanvas("c1", "", 1200, 800);
    c1->Divide(N, N);
    TCanvas* c2 = new TCanvas("c2", "", 1200, 800);
    c2->Divide(N, N);
    TCanvas* c3 = new TCanvas("c3", "", 1200, 800);
    c3->Divide(N, N);

    for (Int_t i = 0; i < Nz; i++) {
        counter = 0;
        diff[i] = new TGraph2D();
        field_value[i] = new TGraph2D();
        field_extrap_value[i] = new TGraph2D();

        Double_t Z = Zleft_new + i * dZ;
        Double_t field_val = 0.;

        for (Int_t j = 0; j < Nx + 1; j++) {
            Double_t X = Xmin + j * dX;

            for (Int_t k = 0; k < Ny + 1; k++) {
                Double_t Y = Ymin + k * dY;
                Double_t extr_val = C->Interpolate(X, Y) * exp(-0.5 * ((Z - mu->Interpolate(X, Y)) / sigma->Interpolate(X, Y))**2);
                field_val = ((fcomp == "x") ? (field->GetBx(X, Y, Z)) : (fcomp == "y") ? (field->GetBy(X, Y, Z)) : (fcomp == "z") ? (field->GetBz(X, Y, Z)));
                field_extrap_value[i]->SetPoint(counter, X, Y, extr_val);
                field_value[i]->SetPoint(counter, X, Y, field_val);
                diff[i]->SetPoint(counter, X, Y, Abs(extr_val - field_val) / Abs(field_val) * 100.);
                counter++;
            }
        }
        SetGraphDrawAttrib(diff[i], Z, "X, cm", "Y, cm", i + 1, c1);
        SetGraphDrawAttrib(field_extrap_value[i], Z, "X, cm", "Y, cm", i + 1, c2);
        SetGraphDrawAttrib(field_value[i], Z, "X, cm", "Y, cm", i + 1, c3);
    }

    c1->SaveAs("B" + fcomp + "_diff_overlap_reg.eps");
    delete c1;
    c3->SaveAs("B" + fcomp + "_fieldMap.eps");
    delete c3;
    c2->SaveAs("B" + fcomp + "_fieldMap_extrap.eps");
    delete c2;
}

void SetGraphDrawAttrib(TGraph2D* gr, Double_t z, TString axisX, TString axisY, Int_t counter, TCanvas* c) {

    TH2D* tmp = gr->GetHistogram();
    Char_t buff[1000];
    sprintf(buff, "Z = %G cm", z);
    tmp->SetTitleSize(222);
    tmp->SetTitle(buff);
    c->cd(counter);

    tmp->GetXaxis()->SetTitle(axisX);
    tmp->GetYaxis()->SetTitle(axisY);
    tmp->GetXaxis()->SetTitleSize(0.03);
    tmp->GetXaxis()->SetLabelSize(0.03);
    tmp->GetYaxis()->SetTitleSize(0.03);
    tmp->GetYaxis()->SetLabelSize(0.03);
    tmp->GetZaxis()->SetTitleSize(0.03);
    tmp->GetZaxis()->SetLabelSize(0.03);
    tmp->Draw("colz");
}

void CreateGraphs(TGraph* gr, Double_t slice1, Double_t slice2, TString title1, TString title2, TString title3) {

    Char_t buff[1000] = " ";
    sprintf(buff, (title2 + "%G" + "  ").Data(), slice1);
    title1 += TString(buff);
    sprintf(buff, (title3 + "%G").Data(), slice2);
    title1 += TString(buff);
    gr->SetTitle(title1);
}

void CreateNewMap(BmnFieldMap* magField, Double_t fieldZ, TFile& f) {

    Int_t fNx = magField->GetNx();
    Int_t fNy = magField->GetNy();

    Double_t shift = 1e-6;

    Double_t Xmin = magField->GetXmin() - shift;
    Double_t dx = magField->GetXstep();
    Double_t Ymin = magField->GetYmin() - shift;
    Double_t dy = magField->GetYstep();
    Double_t Zmin1 = magField->GetZmin() - shift;
    Double_t dz = magField->GetZstep();

    Double_t x, y, z, bx, by, bz;
   
    Int_t index = 0;

    ofstream mapFile(nameOutput);

    TGraph2D* BxConst = (TGraph2D*)f.Get("BxConst");
    TGraph2D* ByConst = (TGraph2D*)f.Get("ByConst");
    TGraph2D* BzConst = (TGraph2D*)f.Get("BzConst");
    TGraph2D* BxMean = (TGraph2D*)f.Get("BxMean");
    TGraph2D* ByMean = (TGraph2D*)f.Get("ByMean");
    TGraph2D* BzMean = (TGraph2D*)f.Get("BzMean");
    TGraph2D* BxSigma = (TGraph2D*)f.Get("BxSigma");
    TGraph2D* BySigma = (TGraph2D*)f.Get("BySigma");
    TGraph2D* BzSigma = (TGraph2D*)f.Get("BzSigma");
    
    magField->SetPosition(0., 0., 0.);

    mapFile.precision(6);
    mapFile << showpoint;
    for (Int_t ix = 0; ix < fNx; ix++) {
        x = Xmin + dx * ix;
        for (Int_t iy = 0; iy < fNy; iy++) {
            y = Ymin + dy * iy;
            for (Int_t iz = 0; iz < fNz; iz++) {
                z = Zmin1 + dz * iz;
                index = ix * fNy * fNz + iy * fNz + iz;
                if (z <= Zedge) {
                    bx = (Abs(magField->GetBx(x, y, z)) / factor > 10e-8) ? (magField->GetBx(x, y, z) / factor) : 0.0;
                    by = (Abs(magField->GetBy(x, y, z)) / factor > 10e-8) ? (magField->GetBy(x, y, z) / factor) : 0.0;
                    bz = (Abs(magField->GetBz(x, y, z)) / factor > 10e-8) ? (magField->GetBz(x, y, z) / factor) : 0.0;
                    mapFile << bx << " " << by << " " << bz << endl;
                } 
                else if (z > Zedge) {
                    z += fieldZ;
                    bx = FieldExtrapolate(x, y, z,  BxConst, BxMean, BxSigma);
                    by = FieldExtrapolate(x, y, z,  ByConst, ByMean, BySigma);
                    bz = FieldExtrapolate(x, y, z,  BzConst, BzMean, BzSigma);
                    mapFile << bx / factor << " " << by / factor << " " << bz / factor << endl;
                }
            } // z-Loop
        } // y-Loop
    } // x-Loop
}

Double_t FieldExtrapolate(Double_t x, Double_t y, Double_t z, TGraph2D* c, TGraph2D* m, TGraph2D* s) {

    Double_t Mean = m->Interpolate(x, y);
    Double_t Sigma = s->Interpolate(x, y);
    
    return c->Interpolate(x, y) * TMath::Exp(-0.5 * (z - Mean) * (z - Mean) / Sigma / Sigma);
}