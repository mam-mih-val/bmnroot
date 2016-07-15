#include "TH1F.h"
#include "TCanvas.h"
#include "TChain.h"

void GemDigitsAnalysis(UInt_t runId = 0) {
    gStyle->SetOptStat(0);
    /* Load basic libraries */
    gROOT->LoadMacro("$VMCWORKDIR/macro/run/bmnloadlibs.C");
    bmnloadlibs(); // load bmn libraries

    TChain *eveTree = new TChain("cbmsim");
    TString inName = Form("bmn_run%04d_digi.root", runId);
    eveTree->Add(inName);

    TClonesArray *GemDigits;
    eveTree->SetBranchAddress("BmnGemStripDigit", &GemDigits);

    Long64_t nEvents = eveTree->GetEntries();
    cout << nEvents << endl;

    const UInt_t kNST = 8;
    TString name;
    TH1F * h_X[kNST];
    TH1F * h_X0[kNST];
    TH1F * h_Amp[kNST];
    TH1F * h_XAmp[kNST];

    for (Int_t i = 0; i < kNST; ++i) {
        name = Form("X_%d", i);
        h_X[i] = new TH1F(name, name, 1019, 1, 1019);
        name = Form("X0_%d", i);
        h_X0[i] = new TH1F(name, name, 500, 1, 500);
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
            Int_t sig = digX->GetStripSignal();

            Int_t mod = digX->GetModule();
            if ((mod == 0 && st != 7 && st != 0) || (mod == 1 && st == 7)) {
                h_X[st]->Fill(str);
            } else if ((mod == 3 && st == 7) || (mod == 2 && st == 6)) {
                h_X0[st]->Fill(str);
            } else if (mod == 1 && st != 7 && st != 6 && st != 0) {
                h_X0[st]->Fill(str);
            }
        }
    }

    for (Int_t i = 0; i < kNST; ++i) {
        h_X[i]->Scale(1.0 / nEvents);
        h_X[i]->SetTitle("");
        h_X0[i]->Scale(1.0 / nEvents);
        h_X0[i]->SetTitle("");
    }
    for (Int_t i = 0; i < kNST; ++i) {
        for (Int_t j = 0; j < h_X[i]->GetNbinsX(); ++j) {
            if (h_X[i]->GetBinContent(j) > 0.2) h_X[i]->SetBinContent(j, 0.0);
            if (h_X0[i]->GetBinContent(j) > 0.2) h_X0[i]->SetBinContent(j, 0.0);
        }
    }

    TCanvas* SuperCave = new TCanvas("SuperCave", "SuperCave", 66 * 100, 41 * 100);
    TString pdfName = Form("gem_run%04d.pdf", runId);
    for (Int_t i = 1; i < kNST; ++i) {
        Bool_t right = kFALSE;
        if (i == (kNST - 2))
            right = kTRUE;
        DrawGemDigits(h_X0[i], h_X[i], SuperCave, right);
        if (i == 1) {
            SuperCave->Print(Form("gem_run%04d.pdf(", runId), "");
        } else if (i == (kNST - 1)) {
            SuperCave->Print(Form("gem_run%04d.pdf)", runId), "");
        } else {
            SuperCave->Print(Form("gem_run%04d.pdf", runId), "");
        }

    }
    delete SuperCave;

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
    //        x1->SetFillStyle(3001);
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
    //    x0->SetFillStyle(3002);
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