
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
void drawResidualsBMN(TString fileName = "/home/merz/pavel/nfs/RUN7_res/BMNDST_4649.root", Int_t nEvents = 10000) {
    bmnloadlibs();  // load libraries

    TChain* out = new TChain("bmndata");
    out->Add(fileName.Data());

    cout << "#recorded entries = " << out->GetEntries() << endl;

    TClonesArray* dchTracks = nullptr;
    out->SetBranchAddress("BmnDchTrack", &dchTracks);
    TClonesArray* gemTracks = nullptr;
    out->SetBranchAddress("BmnGemTrack", &gemTracks);
    TClonesArray* cscHits = nullptr;
    out->SetBranchAddress("BmnCSCHit", &cscHits);
    TClonesArray* tof700Hits = nullptr;
    out->SetBranchAddress("BmnTof700Hit", &tof700Hits);
    TClonesArray* tof400Hits = nullptr;
    out->SetBranchAddress("BmnTof400Hit", &tof400Hits);

    const Int_t nDet = 3;
    const Int_t nRes = 2;

    TString namesDet[nDet] = {"CSC", "TOF-400", "TOF-700"};
    TString namesRes[nRes] = {"X-residuals", "Y-residuals"};

    gStyle->SetOptStat(0);
    gStyle->SetPalette(53);  //(kGreyScale);

    TH2F* histByEv[nDet][nRes];
    TH1F* hist[nDet][nRes];
    for (Int_t iDet = 0; iDet < nDet; iDet++) {
        for (Int_t jRes = 0; jRes < nRes; jRes++) {
            TString histName = namesDet[iDet] + namesRes[jRes];
            TString histNameEv = namesDet[iDet] + namesRes[jRes] + "Ev";
            hist[iDet][jRes] = new TH1F(histName, "", 100, -5, 5);
            histByEv[iDet][jRes] = new TH2F(histNameEv, "", nEvents, 0, nEvents, 100, -100, 100);
        }
    }

    for (Int_t iEv = 0; iEv < nEvents; iEv++) {
        out->GetEntry(iEv);
        if (iEv % 1000 == 0) printf("event: %d\n", iEv);
        for (Int_t iHit = 0; iHit < tof700Hits->GetEntriesFast(); iHit++) {
            BmnHit* hit = (BmnHit*)tof700Hits->At(iHit);
            hist[2][0]->Fill(hit->GetResX());
            hist[2][1]->Fill(hit->GetResY());
            histByEv[2][0]->Fill(iEv, hit->GetResX());
            histByEv[2][1]->Fill(iEv, hit->GetResY());
        }
        for (Int_t iHit = 0; iHit < tof400Hits->GetEntriesFast(); iHit++) {
            BmnHit* hit = (BmnHit*)tof400Hits->At(iHit);
            hist[1][0]->Fill(hit->GetResX());
            hist[1][1]->Fill(hit->GetResY());
            histByEv[1][0]->Fill(iEv, hit->GetResX());
            histByEv[1][1]->Fill(iEv, hit->GetResY());
        }
        for (Int_t iHit = 0; iHit < cscHits->GetEntriesFast(); iHit++) {
            BmnHit* hit = (BmnHit*)cscHits->At(iHit);
            hist[0][0]->Fill(hit->GetResX());
            hist[0][1]->Fill(hit->GetResY());
            histByEv[0][0]->Fill(iEv, hit->GetResX());
            histByEv[0][1]->Fill(iEv, hit->GetResY());
        }
        // for (Int_t iTrack = 0; iTrack < dchTracks->GetEntriesFast(); iTrack++) {
        //     BmnDchTrack* track = (BmnDchTrack*)dchTracks->At(iTrack);
        //     if (track->GetParamFirst()->GetZ() < 550.0) nDch1++;
        //     if (track->GetParamFirst()->GetZ() > 650.0) nDch2++;
        // }
        // Int_t nMwpc = 0;
        // for (Int_t iTrack = 0; iTrack < MwpcTracks->GetEntriesFast(); iTrack++) {
        //     BmnMwpcTrack* track = (BmnMwpcTrack*)MwpcTracks->At(iTrack);
        //     if (track->GetParamFirst()->GetZ() > -300.0) nMwpc++;
        // }
    }

    TCanvas* res = new TCanvas("res", "res", 1000, 1000);
    res->Divide(nDet, nRes, 0, 0);
    for (Int_t jRes = 0; jRes < nRes; jRes++) {
        for (Int_t iDet = 0; iDet < nDet; iDet++) {
            Int_t padId = iDet + nRes * jRes + 1;
            TPad* pad = (TPad*)res->cd(padId);
            pad->SetGrid(0, 0);
            hist[iDet][jRes]->Fit("gaus", "WW");
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

    TCanvas* resEv = new TCanvas("resEv", "resEv", 1000, 1000);
    resEv->Divide(nDet, nRes, 0, 0);
    for (Int_t jRes = 0; jRes < nRes; jRes++) {
        for (Int_t iDet = 0; iDet < nDet; iDet++) {
            Int_t padId = iDet + nRes * jRes + 1;
            TPad* pad = (TPad*)res->cd(padId);
            pad->SetGrid(0, 0);
            histByEv[iDet][jRes]->Fit("gaus", "WW");
            histByEv[iDet][jRes]->Draw();
            histByEv[iDet][jRes]->GetXaxis()->SetLabelSize(0.08);
            TLatex latexDet;
            latexDet.SetTextSize(0.08);
            latexDet.DrawLatex(-4.5, 0.9 * hist[iDet][jRes]->GetMaximum(), namesDet[iDet]);
            TLatex latexRes;
            latexRes.SetTextSize(0.06);
            latexRes.DrawLatex(-4.5, 0.8 * hist[iDet][jRes]->GetMaximum(), namesRes[jRes]);
        }
    }
    res->SaveAs("ResidualsSRCEv.pdf");
}
