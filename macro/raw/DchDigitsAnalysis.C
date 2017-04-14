#include "TH1F.h"
#include "TLine.h"
#include "TH2F.h"
#include "TGraph.h"
#include "TCanvas.h"
#include "TChain.h"
#include "TMath.h"

using namespace TMath;

const UInt_t kNPLANES = 16;
const UInt_t kNREALWIRES = 240;
const UInt_t kNWIRES = 256;
const Int_t Number = 4;
const Int_t nb = 100;
const Float_t angles[kNPLANES] = {
    135, 135, 45, 45, 90, 90, 180, 180,
    135, 135, 45, 45, 90, 90, 180, 180
};
const TString names[kNPLANES] = {
    "VA_1", "VB_1", "UA_1", "UB_1", "YA_1", "YB_1", "XA_1", "XB_1",
    "VA_2", "VB_2", "UA_2", "UB_2", "YA_2", "YB_2", "XA_2", "XB_2"
};

void DchDigitsAnalysis(UInt_t runId = 0, TString type = "pdf") {
    gStyle->SetOptStat(0);
    /* Load basic libraries */
    gROOT->LoadMacro("$VMCWORKDIR/macro/run/bmnloadlibs.C");
    bmnloadlibs(); // load bmn libraries

    TChain *eveTree = new TChain("cbmsim");
    TString inName = Form("bmn_run%04d_digi.root", runId);
    eveTree->Add(inName);

    TClonesArray *DchDigits;
    eveTree->SetBranchAddress("DCH", &DchDigits);

    Long64_t nEvents = eveTree->GetEntries();
    cout << nEvents << endl;

    TH1F * h_wires[kNPLANES];
    Float_t v_wires[kNPLANES][kNWIRES] = {};

    for (Int_t i = 0; i < kNPLANES; ++i)
        h_wires[i] = new TH1F(names[i], names[i], kNREALWIRES, 0, kNREALWIRES);


    for (Int_t iEv = 0; iEv < nEvents; iEv++) {
        if (iEv % 100 == 0) cout << "EVENT: " << iEv << endl;
        eveTree->GetEntry(iEv);
        for (Int_t iDig = 0; iDig < DchDigits->GetEntriesFast(); ++iDig) {
            BmnDchDigit* dig = (BmnDchDigit*) DchDigits->At(iDig);
            Int_t plane = dig->GetPlane();
            Int_t wire = dig->GetWireNumber();

            v_wires[plane][wire] += 1;
            if (wire > kNREALWIRES - 1) {
                wire -= 128; //8 * 16 last preamplifier setup behind hole, so move signal in correct place
            }
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

    if (type == "pdf") {
        TCanvas* SuperCave = new TCanvas("SuperCave", "SuperCave", 1000, 1500);
        for (Int_t i = 0; i < kNPLANES; ++i) {

            SuperCave->cd();
            TPad *pad1 = new TPad("", "", 0, 0, 1.0, 2.0 / 3.0);
            //pad1->SetMargin(0.16, 0.16, 0.01, 0.0);
            DrawWires(pad1, myPalette, v_wires[i], angles[i]);
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
    } else if (type == "png") {
        TCanvas* SuperCave = new TCanvas("SuperCave", "SuperCave", 1000, 1500);
        for (Int_t i = 0; i < kNPLANES; ++i) {

            SuperCave->cd();
            TPad *pad1 = new TPad("", "", 0, 0, 1.0, 2.0 / 3.0);
            pad1->SetMargin(0.16, 0.16, 0.01, 0.0);
            DrawWires(pad1, myPalette, v_wires[i], angles[i]);
            SuperCave->cd();
            TPad *pad2 = new TPad("", "", 0, 2.0 / 3.0, 1.0, 1.0);
            pad2->SetMargin(0.16, 0.16, 0.0, 0.2);
//            pad2->SetBottomMargin(0.0);
            pad2->Draw();
            pad2->cd();
            h_wires[i]->SetLineWidth(2);
            h_wires[i]->SetFillColor(kBlue - 10);
            h_wires[i]->SetLineColor(kBlue);
            h_wires[i]->GetXaxis()->SetLabelSize(0.05);
            h_wires[i]->GetYaxis()->SetLabelSize(0.05);
            h_wires[i]->Draw("X+");
            SuperCave->SaveAs(Form("dch_run%04d_plane%d.png", runId, i));
        }
        delete SuperCave;
    }

    TCanvas* chmb1 = new TCanvas("DCH-1", "DCH-1", 1000, 1000);
    chmb1->cd();
    for (Int_t i = 0; i < kNPLANES / 2; ++i) {
        DrawAllWires(chmb1, myPalette, v_wires[i], angles[i], i);
    }
    chmb1->SaveAs("DCH1_wires.png");
    TCanvas* chmb2 = new TCanvas("DCH-2", "DCH-2", 1000, 1000);
    chmb2->cd();
    for (Int_t i = kNPLANES / 2; i < kNPLANES; ++i) {
        DrawAllWires(chmb2, myPalette, v_wires[i], angles[i], i);
    }
    chmb2->SaveAs("DCH2_wires.png");
}

void DrawAllWires(TCanvas* c, Int_t* palette, Float_t * wires, Float_t ang, Short_t planeId) {
    const UInt_t kNANGLE = 8;
    Double_t l = c->GetAbsWNDC();
    Double_t a = l / (1 + Sqrt2());
    Double_t b = l / (2 + Sqrt2());
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
    gr->SetFillStyle(4000);
    //    gr->Draw("la");
    Float_t w = l / kNREALWIRES;
    Float_t x0 = 0.0;
    Float_t x1 = 0.0;
    Float_t y0 = 0.0;
    Float_t y1 = 0.0;

    TLine line;
    line.SetLineWidth(2);
    for (Int_t j = 0; j < kNWIRES; ++j) {

        if (j >= 112 && j < 128) {
            x0 = w * j;
            x1 = x0;
            y0 = 0;
            y1 = l / 2;
        } else if (j > kNREALWIRES - 1) {
            x0 = w * (j - 128);
            x1 = x0;
            y0 = l / 2;
            y1 = l;
        } else {
            x0 = w * j;
            x1 = x0;
            if (x0 <= b) {
                y0 = -x0 + b;
                y1 = x0 + a + b;
            } else if (x0 >= a + b) {
                y0 = -x0 + 2 * a + 3 * b;
                y1 = x0 - a - b;
            } else {
                y0 = 0;
                y1 = l;
            }
        }

        Int_t colorId = Int_t(wires[j] * (nb - 1));
        line.SetLineColor(palette[colorId]);
        line.SetLineColorAlpha(palette[colorId], 0.9 - 0.9 / 7 * planeId); //transparency
        if (colorId == 0) {
            line.SetLineColor(kWhite);
            line.SetLineColorAlpha(kWhite, planeId * 0.1);
        }
        Float_t sinPhi = Sin(ang * DegToRad());
        Float_t cosPhi = Cos(ang * DegToRad());
        Float_t x0_ = (x0 - l / 2) * cosPhi - (y0 - l / 2) * sinPhi + l / 2;
        Float_t y0_ = (x0 - l / 2) * sinPhi + (y0 - l / 2) * cosPhi + l / 2;
        Float_t x1_ = (x1 - l / 2) * cosPhi - (y1 - l / 2) * sinPhi + l / 2;
        Float_t y1_ = (x1 - l / 2) * sinPhi + (y1 - l / 2) * cosPhi + l / 2;
        line.DrawLine(x0_, y0_, x1_, y1_);
        x0 += w;
        x1 = x0;
    }
    TEllipse* hole = new TEllipse((a + 2 * b) / 2, (a + 2 * b) / 2, (a + 2 * b) / 30);
    hole->Draw("same");
}

void DrawWires(TPad* pad, Int_t* palette, Float_t * wires, Float_t ang) {

    pad->Draw();
    pad->cd();
    const UInt_t kNANGLE = 8;
    Double_t l = pad->GetAbsWNDC();
    Double_t a = l / (1 + Sqrt2());
    Double_t b = l / (2 + Sqrt2());
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
    Float_t w = l / kNREALWIRES;
    Float_t x0 = 0.0;
    Float_t x1 = 0.0;
    Float_t y0 = 0.0;
    Float_t y1 = 0.0;

    TLine line;
    line.SetLineWidth(3);
    for (Int_t j = 0; j < kNWIRES; ++j) {

        if (j >= 112 && j < 128) {
            x0 = w * j;
            x1 = x0;
            y0 = 0;
            y1 = l / 2;
        } else if (j > kNREALWIRES - 1) {
            x0 = w * (j - 128);
            x1 = x0;
            y0 = l / 2;
            y1 = l;
        } else {
            x0 = w * j;
            x1 = x0;
            if (x0 <= b) {
                y0 = -x0 + b;
                y1 = x0 + a + b;
            } else if (x0 >= a + b) {
                y0 = -x0 + 2 * a + 3 * b;
                y1 = x0 - a - b;
            } else {
                y0 = 0;
                y1 = l;
            }
        }

        Int_t colorId = Int_t(wires[j] * (nb - 1));
        line.SetLineColor(palette[colorId]);
        if (colorId == 0)
            line.SetLineColor(kWhite);
        Float_t sinPhi = Sin(ang * DegToRad());
        Float_t cosPhi = Cos(ang * DegToRad());
        Float_t x0_ = (x0 - l / 2) * cosPhi - (y0 - l / 2) * sinPhi + l / 2;
        Float_t y0_ = (x0 - l / 2) * sinPhi + (y0 - l / 2) * cosPhi + l / 2;
        Float_t x1_ = (x1 - l / 2) * cosPhi - (y1 - l / 2) * sinPhi + l / 2;
        Float_t y1_ = (x1 - l / 2) * sinPhi + (y1 - l / 2) * cosPhi + l / 2;
        line.DrawLine(x0_, y0_, x1_, y1_);
        x0 += w;
        x1 = x0;
    }
    TEllipse* hole = new TEllipse((a + 2 * b) / 2, (a + 2 * b) / 2, (a + 2 * b) / 30);
    hole->Draw("same");
}
