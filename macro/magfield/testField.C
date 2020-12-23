#include <TH2F.h>
#include <TCanvas.h>
#include <TString.h>
#include <Rtypes.h>
#include <TStyle.h>
#include <TPaletteAxis.h>
#include <TList.h>

using namespace TMath;

R__ADD_INCLUDE_PATH($VMCWORKDIR)
#include "macro/run/bmnloadlibs.C"
        
 void testField(TString in = "") {
    gStyle->SetOptStat(0);

    BmnFieldMap* magField = new BmnNewFieldMap(in);
    magField->SetScale(1.);
    magField->Init();

    const Int_t nSlices = 16;
    const Double_t y[nSlices] = {-35., -30., -25., -20., -15., -10., -5., 0., 5., 10., 15., 20., 25., 30., 35., 40.};
    TH2F * ByXZ[nSlices];

    for (Int_t iSlice = 0; iSlice < nSlices; iSlice++) {
        ByXZ[iSlice] = new TH2F(
                Form("Field_map_XZ (GetBy(X, %G, Z))", y[iSlice]),
                Form("Field_map_XZ (GetBy(X, %G, Z))", y[iSlice]),
                magField->GetNz(), magField->GetZmin(), magField->GetZmax(),
                magField->GetNx() - 1, magField->GetXmin(), magField->GetXmax());

        for (Int_t iX = magField->GetXmin(); iX < magField->GetXmax(); iX += magField->GetXstep()) {
            for (Int_t iZ = magField->GetZmin(); iZ < magField->GetZmax(); iZ += magField->GetZstep()) {
                ByXZ[iSlice]->Fill(Double_t(iZ), Double_t(iX), magField->GetBy(Double_t(iX), y[iSlice], Double_t(iZ)));
            }
        }
    }

    TCanvas* c = new TCanvas("", "", 400, 400);
    c->Divide(Int_t(TMath::Sqrt(nSlices)), Int_t(TMath::Sqrt(nSlices)));
    for (Int_t iSlice = 0; iSlice < nSlices; iSlice++) {
        c->cd(iSlice + 1);
        ByXZ[iSlice]->Draw("colz");
        ByXZ[iSlice]->GetXaxis()->SetTitle("Z [cm]");
        ByXZ[iSlice]->GetXaxis()->SetLabelSize(0.05);
        ByXZ[iSlice]->GetXaxis()->SetTitleSize(0.05);
        ByXZ[iSlice]->GetXaxis()->SetTitleOffset(1.05);
        ByXZ[iSlice]->GetXaxis()->CenterTitle();
        ByXZ[iSlice]->GetYaxis()->SetTitle("X [cm]");
        ByXZ[iSlice]->GetYaxis()->SetLabelSize(0.05);
        ByXZ[iSlice]->GetYaxis()->SetTitleSize(0.05);
        ByXZ[iSlice]->GetYaxis()->CenterTitle();
    }
    c->SaveAs("profiles_" + in + ".pdf");
}
