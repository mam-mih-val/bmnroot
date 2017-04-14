#include "TH1F.h"
#include "TH2F.h"
#include "TCanvas.h"
#include "TChain.h"

void GemDigitsAnalysis(UInt_t runId = 0, TString type = "pdf") {
    gStyle->SetOptStat(0);
    /* Load basic libraries */
    gROOT->LoadMacro("$VMCWORKDIR/macro/run/bmnloadlibs.C");
    bmnloadlibs(); // load bmn libraries

    TChain *eveTree = new TChain("cbmsim");
    TString inName = Form("bmn_run%04d_digi.root", runId);
    eveTree->Add(inName);

    TClonesArray *GemDigits;
    eveTree->SetBranchAddress("GEM", &GemDigits);

    Long64_t nEvents = eveTree->GetEntries();
    cout << nEvents << endl;

    const UInt_t kNST = 8;
    const UInt_t nStrips[kNST] = {256, 825, 825, 825, 825, 825, 1019, 1019};

    TString name;
    TH1F * h_X[kNST];
    TH2F * h_X_2d[kNST];
    TH1F * h_X0[kNST];
    TH2F * h_X0_2d[kNST];
    TH1F * h_Amp[kNST];
    TH1F * h_XAmp[kNST];

    for (Int_t i = 0; i < kNST; ++i) {
        name = Form("X_%d", i);
        h_X[i] = new TH1F(name, name, nStrips[i], 0, nStrips[i]);
        name = Form("X_color_%d", i);
        h_X_2d[i] = new TH2F(name, name, nStrips[i], 0, nStrips[i], 1, 0, 1);
        name = Form("X0_%d", i);
        h_X0[i] = new TH1F(name, name, 500, 1, 500);
        name = Form("X0_color_%d", i);
        h_X0_2d[i] = new TH2F(name, name, 500, 1, 500, 1, 0, 1);
        name = Form("Amp_%d", i);
        h_Amp[i] = new TH1F(name, name, 100, 0, 500);
        name = Form("XAmp_%d", i);
        h_XAmp[i] = new TH1F(name, name, 500, 0, 1000);
    }

    for (Int_t iEv = 0; iEv < nEvents; iEv++) {
        if (iEv % 100 == 0) cout << "EVENT: " << iEv << endl;
        eveTree->GetEntry(iEv);
        for (Int_t iDig = 0; iDig < GemDigits->GetEntriesFast(); ++iDig) {
            BmnGemStripDigit* digX = (BmnGemStripDigit*) GemDigits->At(iDig);
            Int_t lay = digX->GetStripLayer();
            if (lay != 0) continue; //use only X
            Int_t st = digX->GetStation();
            Int_t str = digX->GetStripNumber();
            Int_t mod = digX->GetModule();
            if (mod == 0) {
                h_X[st]->Fill(str);
                h_X_2d[st]->Fill(str, 0);
            } else if (mod == 1 && st == 6) {
                h_X[st + 1]->Fill(str);
                h_X_2d[st + 1]->Fill(str, 0);
            }
        }
    }

    const Float_t kThresh = 0.15 * nEvents; //remove signals above threshold (guess it is noisy channels)

    for (Int_t i = 0; i < kNST; ++i)
        for (Int_t j = 1; j <= h_X[i]->GetNbinsX(); ++j) {
            if (h_X[i]->GetBinContent(j) > kThresh) h_X[i]->SetBinContent(j, 0.0);
            for (Int_t k = 1; k <= h_X_2d[i]->GetNbinsY(); ++k)
                if (h_X_2d[i]->GetBinContent(j, k) > kThresh) h_X_2d[i]->SetBinContent(j, k, 0.0);
        }

    if (type == "pdf") {
        TCanvas* SuperCave = new TCanvas("SuperCave", "SuperCave", 66 * 100, 41 * 100);
        for (Int_t i = 0; i < kNST; ++i) {
            DrawGemDigitsColz(h_X_2d[i], h_X[i], SuperCave);
            if (i == 0) {
                SuperCave->Print(Form("gem_run%04d.pdf(", runId), "");
            } else if (i == (kNST - 1)) {
                SuperCave->Print(Form("gem_run%04d.pdf)", runId), "");
            } else {
                SuperCave->Print(Form("gem_run%04d.pdf", runId), "");
            }
        }
        delete SuperCave;
    } else if (type == "png") {
        TCanvas* SuperCave = new TCanvas("SuperCave", "SuperCave", 66 * 100, 41 * 100);
        for (Int_t i = 0; i < kNST; ++i) {
            DrawGemDigitsColz(h_X_2d[i], h_X[i], SuperCave);
            SuperCave->SaveAs(Form("gem_run%04d_st%d.png", runId, i));
        }
        delete SuperCave;
    }

}

void DrawGemDigits(TH1F* x0, TH1F* x1, TCanvas* cave, Bool_t right = kFALSE) {
    const Float_t xBig = 66.0;
    const Float_t yBig = 41.0;
    const Float_t xSmall = 15.0;
    const Float_t ySmall = 10.0;
    const Float_t bigBottMarg = 0.1;
    const Float_t bigTopMarg = 0.1;
    const Float_t bigLeftMarg = 0.1;
    const Float_t bigRightMarg = 0.1;

    cave->cd();
    TPad *pad1 = new TPad(x1->GetTitle(), x1->GetTitle(), 0, 0, 1, 1.0);
    pad1->SetMargin(bigLeftMarg, bigRightMarg, bigBottMarg, bigTopMarg);
    pad1->Draw();
    pad1->cd();
    x1->SetFillColor(kBlue - 10);
    x1->SetLineColor(kBlue);
    x1->GetXaxis()->SetLabelColor(kBlue);
    x1->GetYaxis()->SetLabelColor(kBlue);
    x1->Draw();
    cave->cd();
    TPad *pad2;
    if (!right) {
        pad2 = new TPad(x0->GetTitle(), x0->GetTitle(), bigLeftMarg, bigBottMarg, xSmall / xBig + bigLeftMarg, ySmall / yBig + bigBottMarg);
        pad2->SetMargin(0, bigRightMarg, 0, bigTopMarg);
    } else {
        pad2 = new TPad(x0->GetTitle(), x0->GetTitle(), 1.0 - bigRightMarg - xSmall / xBig, bigBottMarg, 1.0 - bigRightMarg, ySmall / yBig + bigBottMarg);
        pad2->SetMargin(bigLeftMarg, 0, 0, bigTopMarg);
    }

    pad2->SetFillStyle(4000);
    pad2->SetFrameFillColor(0);
    pad2->SetFrameFillStyle(0);
    pad2->SetFrameLineWidth(2);
    pad2->Draw();
    pad2->cd();
    x0->SetFillColor(kRed - 10);
    x0->SetLineColor(kRed);
    x0->GetXaxis()->SetLabelColor(kRed);
    x0->GetXaxis()->SetLabelSize(0.06);
    x0->GetYaxis()->SetLabelColor(kRed);
    x0->GetYaxis()->SetLabelSize(0.06);
    if (!right) {
        x0->Draw("Y+X+");
    } else {
        x0->Draw("X+");
    }
}

void DrawGemDigitsColz(TH2F* h_2d, TH1F* h, TCanvas* cave) {
    const Float_t xBig = 66.0;
    const Float_t yBig = 41.0;
    const Float_t xSmall = 15.0;
    const Float_t ySmall = 10.0;
    const Float_t bigBottMarg = 0.1;
    const Float_t bigTopMarg = 0.1;
    const Float_t bigLeftMarg = 0.1;
    const Float_t bigRightMarg = 0.1;
    cave->cd();
    TPad *pad1 = new TPad(h_2d->GetTitle(), h_2d->GetTitle(), 0, 0, 1.0, 0.5);
    pad1->SetFrameLineWidth(2);
    pad1->SetTopMargin(0.0);
    pad1->Draw();
    pad1->cd();
    h_2d->GetXaxis()->SetLabelColor(kWhite);
    h_2d->GetYaxis()->SetLabelColor(kWhite);
    h_2d->GetXaxis()->SetTickLength(0);
    h_2d->GetYaxis()->SetTickLength(0);
    h_2d->Draw("col");
    //DrawStrips(h_2d);

    cave->cd();
    TPad *pad2 = new TPad("", "", 0, 0.5, 1.0, 1.0);
    pad2->SetBottomMargin(0.0);
    pad2->SetFrameLineWidth(2);
    pad2->Draw();
    pad2->cd();
    h->SetFillColor(kBlue - 10);
    h->SetLineColor(kBlue);
    h->Draw("X+");


}

void DrawStrips(TH2* h) {
    Int_t nx = h->GetNbinsX();
    Double_t x1 = h->GetXaxis()->GetXmin();
    Double_t y1 = h->GetYaxis()->GetXmin();
    Double_t y2 = h->GetYaxis()->GetXmax();
    Double_t x = x1;

    TLine l;
    l.SetLineColor(16);

    for (Int_t i = 0; i < nx; i++) {
        l.DrawLine(x, y1, x, y2);
        x = x + h->GetXaxis()->GetBinWidth(i);
    }
}