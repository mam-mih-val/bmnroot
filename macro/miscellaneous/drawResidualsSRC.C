
#include <TCanvas.h>
#include <TChain.h>
#include <TClonesArray.h>
#include <TFitResultPtr.h>
#include <TH2F.h>
#include <TLatex.h>
#include <TString.h>
#include <TStyle.h>

#include <FairHit.h>
#include <Rtypes.h>
#include <TGraph.h>

R__ADD_INCLUDE_PATH($VMCWORKDIR)
#include "macro/run/bmnloadlibs.C"
//
void drawResidualsSRC(TString fileName = "../run/srcdst_3332.root", Int_t nEvents = 50000) {
    bmnloadlibs();  // load libraries

    TChain* out = new TChain("bmndata");
    out->Add(fileName.Data());

    cout << "#recorded entries = " << out->GetEntries() << endl;

    TClonesArray* dchHits = nullptr;
    out->SetBranchAddress("BmnDchHit", &dchHits);
    TClonesArray* gemTracks = nullptr;
    out->SetBranchAddress("BmnGemTrack", &gemTracks);
    TClonesArray* cscHits = nullptr;
    out->SetBranchAddress("BmnCSCHit", &cscHits);
    TClonesArray* tof700Hits = nullptr;
    out->SetBranchAddress("BmnTof700Hit", &tof700Hits);

    const Int_t nDet = 4;
    const Int_t nRes = 2;

    TString namesDet[nDet] = {"CSC", "DCH1", "TOF-700", "DCH2"};
    TString namesRes[nRes] = {"X-residuals", "Y-residuals"};

    gStyle->SetOptStat(0);
    gStyle->SetPalette(53);  //(kGreyScale);

    TH1F* hist[nDet][nRes];

    const Int_t nMod = 60;
    TH1F* hResTof700Mod[nMod][nRes];
    for (Int_t iMod = 0; iMod < nMod; iMod++) {
        for (Int_t jRes = 0; jRes < nRes; jRes++) {
            TString histName = Form("TOF-700  mod-%d  %s", iMod, namesRes[jRes].Data());
            hResTof700Mod[iMod][jRes] = new TH1F(histName, histName, 100, -7, 7);
        }
    }

    for (Int_t iDet = 0; iDet < nDet; iDet++) {
        for (Int_t jRes = 0; jRes < nRes; jRes++) {
            TString histName = namesDet[iDet] + namesRes[jRes];
            TString histNameEv = namesDet[iDet] + namesRes[jRes] + "Ev";
            hist[iDet][jRes] = new TH1F(histName, "", 100, -7, 7);
        }
    }

    for (Int_t iEv = 0; iEv < nEvents; iEv++) {
        out->GetEntry(iEv);
        if (iEv % 1000 == 0) printf("event: %d\n", iEv);
        for (Int_t iHit = 0; iHit < cscHits->GetEntriesFast(); iHit++) {
            BmnHit* hit = (BmnHit*)cscHits->At(iHit);
            hist[0][0]->Fill(hit->GetResX());
            hist[0][1]->Fill(hit->GetResY());
        }
        for (Int_t iHit = 0; iHit < dchHits->GetEntriesFast(); iHit++) {
            BmnHit* hit = (BmnHit*)dchHits->At(iHit);
            if (hit->GetStation() == 0) {
                hist[1][0]->Fill(hit->GetResX());
                hist[1][1]->Fill(hit->GetResY());
            }
            if (hit->GetStation() == 1) {
                hist[3][0]->Fill(hit->GetResX());
                hist[3][1]->Fill(hit->GetResY());
            }
        }
        for (Int_t iHit = 0; iHit < tof700Hits->GetEntriesFast(); iHit++) {
            BmnHit* hit = (BmnHit*)tof700Hits->At(iHit);
            Int_t module = (hit->GetDetectorID() >> 8) & 0xFF;
            hResTof700Mod[module][0]->Fill(hit->GetResX());
            hResTof700Mod[module][1]->Fill(hit->GetResY());
            hist[2][0]->Fill(hit->GetResX());
            hist[2][1]->Fill(hit->GetResY());
        }
    }

    gStyle->SetFitFormat("5.3f");
    gStyle->SetOptFit(0010);
    gStyle->SetStatY(0.98);
    gStyle->SetStatX(0.98);
    gStyle->SetStatW(0.2);
    gStyle->SetStatH(0.5);
    gStyle->SetOptStat("i");
    TCanvas* res = new TCanvas("res", "res", 2000, 1000);
    res->Divide(nDet, nRes, 0, 0);
    for (Int_t jRes = 0; jRes < nRes; jRes++) {
        for (Int_t iDet = 0; iDet < nDet; iDet++) {
            Int_t padId = iDet + nDet * jRes + 1;
            TPad* pad = (TPad*)res->cd(padId);
            pad->SetGrid(0, 0);
            hist[iDet][jRes]->Fit("gaus", "WWQ");
            hist[iDet][jRes]->Draw();
            hist[iDet][jRes]->GetXaxis()->SetLabelSize(0.08);
            TLatex latexDet;
            latexDet.SetTextSize(0.08);
            latexDet.DrawLatex(-4.5, 0.9 * hist[iDet][jRes]->GetMaximum(), namesDet[iDet]);
            TLatex latexRes;
            latexRes.SetTextSize(0.06);
            latexRes.DrawLatex(-4.5, 0.8 * hist[iDet][jRes]->GetMaximum(), namesRes[jRes]);
        }
    }
    res->SaveAs("ResidualsSRC.pdf");

    const Int_t nRow = 6;
    const Int_t nCol = 10;

    TCanvas* ResYTof700Mod = new TCanvas("ResYTof700Mod", "ResYTof700Mod", 2000, 1200);
    ResYTof700Mod->Divide(nCol, nRow, 0, 0);
    for (Int_t iMod = 0; iMod < nMod; iMod++) {
        Int_t padId = iMod % nCol + (iMod / nCol) * nCol + 1;
        TPad* pad = (TPad*)ResYTof700Mod->cd(padId);
        pad->SetGrid(0, 0);
        hResTof700Mod[iMod][1]->Fit("gaus", "WWQ");
        hResTof700Mod[iMod][1]->Draw();
        hResTof700Mod[iMod][1]->GetXaxis()->SetLabelSize(0.08);
        // TLatex latexDet;
        // latexDet.SetTextSize(0.08);
        // latexDet.DrawLatex(-4.5, 0.9 * hResXTof700Mod[iMod][0]->GetMaximum(), namesDet[iDet]);
        // TLatex latexRes;
        // latexRes.SetTextSize(0.06);
        // latexRes.DrawLatex(-4.5, 0.8 * hResXTof700Mod[iMod][0]->GetMaximum(), namesRes[0]);
    }
    ResYTof700Mod->SaveAs("ResidualsY_SRC_tof700_mod.pdf");

    TCanvas* ResXTof700Mod = new TCanvas("ResXTof700Mod", "ResXTof700Mod", 2000, 1200);
    ResXTof700Mod->Divide(nCol, nRow, 0, 0);
    for (Int_t iMod = 0; iMod < nMod; iMod++) {
        Int_t padId = iMod % nCol + (iMod / nCol) * nCol + 1;
        TPad* pad = (TPad*)ResXTof700Mod->cd(padId);
        pad->SetGrid(0, 0);
        hResTof700Mod[iMod][0]->Fit("gaus", "WWQ");
        hResTof700Mod[iMod][0]->Draw();
        hResTof700Mod[iMod][0]->GetXaxis()->SetLabelSize(0.08);
        // TLatex latexDet;
        // latexDet.SetTextSize(0.08);
        // latexDet.DrawLatex(-4.5, 0.9 * hResXTof700Mod[iMod][0]->GetMaximum(), namesDet[iDet]);
        // TLatex latexRes;
        // latexRes.SetTextSize(0.06);
        // latexRes.DrawLatex(-4.5, 0.8 * hResXTof700Mod[iMod][0]->GetMaximum(), namesRes[0]);
    }
    ResXTof700Mod->SaveAs("ResidualsX_SRC_tof700_mod.pdf");
}
