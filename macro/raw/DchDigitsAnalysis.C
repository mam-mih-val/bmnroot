#include "TH1F.h"
#include "TLine.h"
#include "TH2F.h"
#include "TGraph.h"
#include "TCanvas.h"
#include "TChain.h"
#include "TMath.h"

using namespace TMath;

const UInt_t kNPLANES = 16;
const UInt_t kNWIRES = 240;
const Int_t Number = 4;
const Int_t nb = 100;

void DchDigitsAnalysis(UInt_t runId = 0) {
    gStyle->SetOptStat(0);
    /* Load basic libraries */
    gROOT->LoadMacro("$VMCWORKDIR/macro/run/bmnloadlibs.C");
    bmnloadlibs(); // load bmn libraries

    TChain *eveTree = new TChain("cbmsim");
    TString inName = Form("bmn_run%04d_digi.root", runId);
    eveTree->Add(inName);

    TClonesArray *DchDigits;
    eveTree->SetBranchAddress("BmnDchDigit", &DchDigits);

    Long64_t nEvents = eveTree->GetEntries();
    cout << nEvents << endl;

    TString name;
    TH1F * h_wires[kNPLANES];
    Float_t v_wires[kNPLANES][kNWIRES] = {};

    for (Int_t i = 0; i < kNPLANES; ++i)
        h_wires[i] = new TH1F(Form("Plane#%d", i), Form("Plane#%d", i), kNWIRES, 0, kNWIRES);


    for (Int_t iEv = 0; iEv < nEvents; iEv++) {
        if (iEv % 100 == 0) cout << "EVENT: " << iEv << endl;
        eveTree->GetEntry(iEv);
        for (Int_t iDig = 0; iDig < DchDigits->GetEntriesFast(); ++iDig) {
            BmnDchDigit* dig = (BmnDchDigit*) DchDigits->At(iDig);
            Int_t plane = dig->GetPlane();
            Int_t wire = dig->GetWireNumber();
            if (wire > kNWIRES - 1) {
                wire -= 128; //8 * 16 last preamplifier setup behind hole, so move signal in correct place
            }
            v_wires[plane][wire] += 1;
            h_wires[plane]->Fill(wire);
        }
    }


    for (Int_t i = 0; i < kNPLANES; ++i) {
        Float_t maxSig = 0.0;
        for (Int_t j = 0; j < kNWIRES; ++j) {
            Float_t sig = v_wires[i][j];
            if (sig > maxSig) maxSig = sig;
        }
        for (Int_t j = 0; j < kNWIRES; ++j) {
            v_wires[i][j] /= maxSig;
        }
    }

    Int_t myPalette[nb];
    Double_t R[Number] = {0.00, 0.00, 1.00, 1.00};
    Double_t G[Number] = {0.00, 1.00, 0.65, 0.00};
    Double_t B[Number] = {1.00, 0.00, 0.00, 0.00};
    Double_t Length[Number] = {0.0, 0.33, 0.66, 1.0};

    Int_t FI = TColor::CreateGradientColorTable(Number, Length, R, G, B, nb);
    for (Int_t i = 0; i < nb; ++i) {
        myPalette[i] = FI + i;
    }

    TCanvas* SuperCave = new TCanvas("SuperCave", "SuperCave", 1000, 1500);
    for (Int_t i = 0; i < kNPLANES; ++i) {

        SuperCave->cd();
        TPad *pad1 = new TPad("", "", 0, 0, 1.0, 2.0 / 3.0);
        //pad1->SetMargin(0.16, 0.16, 0.01, 0.0);
        DrawWires(pad1, myPalette, v_wires[i]);
        SuperCave->cd();
        TPad *pad2 = new TPad("", "", 0, 2.0 / 3.0, 1.0, 1.0);
        //pad2->SetMargin(0.16, 0.16, 0.0, 0.2);
        pad2->SetBottomMargin(0.0);
        pad2->Draw();
        pad2->cd();
        h_wires[i]->SetLineWidth(2);
        h_wires[i]->SetFillColor(kBlue - 10);
        h_wires[i]->SetLineColor(kBlue);
        h_wires[i]->GetXaxis()->SetLabelSize(0.05);
        h_wires[i]->GetYaxis()->SetLabelSize(0.05);
        h_wires[i]->Draw("X+");

        //        DrawGemDigitsColz(h_wires_2d[i], h_wires[i], SuperCave);
        if (i == 0) {
            SuperCave->Print(Form("dch_run%04d.pdf(", runId), "");
        } else if (i == (kNPLANES - 1)) {
            SuperCave->Print(Form("dch_run%04d.pdf)", runId), "");
        } else {
            SuperCave->Print(Form("dch_run%04d.pdf", runId), "");
        }
    }
    delete SuperCave;

}

void DrawWires(TPad* pad, Int_t* palette, Float_t * wires) {

    pad->Draw();
    pad->cd();
    const UInt_t kNANGLE = 8;
    Double_t l = pad->GetAbsWNDC();
    Double_t a = l / (1 + Sqrt(2));
    Double_t b = l / (2 + Sqrt(2));
    Double_t xPol[kNANGLE] = {b, 0, 0, b, b + a, l, l, b + a};
    Double_t yPol[kNANGLE] = {0, b, b + a, l, l, b + a, b, 0};
    TGraph* gr = new TGraph(kNANGLE, xPol, yPol);
    gr->GetXaxis()->SetRangeUser(0.0, 1.0);
    gr->GetYaxis()->SetRangeUser(0.0, 1.0);
    gr->SetTitle("");
    gr->SetLineWidth(3);
    gr->GetXaxis()->SetLabelColor(0);
    gr->GetXaxis()->SetTickLength(0);
    gr->GetYaxis()->SetLabelColor(0);
    gr->GetYaxis()->SetTickLength(0);
    gr->Draw("la");
    Float_t w = l / kNWIRES;
    Float_t xi = 0;
    Float_t y0 = 0.0;
    Float_t y1 = 0.0;

    TLine line;
    line.SetLineWidth(3);
    for (Int_t j = 0; j < kNWIRES; ++j) {

        if (xi <= b) {
            y0 = -xi + b;
            y1 = xi + a + b;
        } else if (xi >= a + b) {
            y0 = -xi + 2 * a + 3 * b;
            y1 = xi - a - b;
        } else {
            y0 = 0;
            y1 = l;
        }

        Int_t colorId = Int_t(wires[j] * (nb - 1));
        line.SetLineColor(palette[colorId]);
        line.DrawLine(xi, y0, xi, y1);
        xi += w;
    }
    TEllipse* hole = new TEllipse((a + 2 * b) / 2, (a + 2 * b) / 2, (a + 2 * b) / 30);
    hole->Draw("same");
}
