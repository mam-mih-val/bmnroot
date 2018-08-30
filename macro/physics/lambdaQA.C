#include <TClonesArray.h>
#include <TChain.h>
#include <TString.h>
#include <TCanvas.h>
#include <TH2.h>
#include <TH1.h>
#include <TF1.h>
#include <TPad.h>
#include <TVirtualPad.h>
#include <TMath.h>

using namespace std;
using namespace TMath;

const Int_t dim = 2;
// Redefine histo limits (supposing exclusion - less(idx0) or more(idx1)) due to cuts obtained (useCutHistoLimits should be kTRUE)

// C-Cu, T = 4 GeV/n
Double_t Mom1[dim] = {0., 4.};
Double_t Mom2[dim] = {0., 1.};
Double_t Eta1[dim] = {2., 3.};
Double_t Eta2[dim] = {2., 3.};

// more than in sense of exclusion ...
Double_t Dca1[dim] = {0.1, 100.};
Double_t Dca2[dim] = {0.1, 100.};

Double_t Dca12X[dim] = {0., 0.4};
Double_t Dca12Y[dim] = {0., 0.4};
Double_t Path = 50.;

Double_t Alpha = 0.5;
Double_t PtPodol = 0.11;

Double_t ArmenPodolEllipseCut(Double_t alpha) {
    const Double_t m1 = 0.938;
    const Double_t m2 = 0.140;
    const Double_t M = 1.117;
    const Double_t Pcm = 0.101;

    Double_t alpha0 = (m1 * m1 - m2 * m2) / (M * M);
    Double_t rAlpha = 2 * Pcm / M;
    Double_t rPt = Pcm;

    if (Abs(alpha - alpha0) > rAlpha)
        return 1000;

    return Sqrt(1 - (alpha - alpha0) * (alpha - alpha0) / (rAlpha * rAlpha)) * rPt;

}

void lambdaQA(TString fileName, Bool_t useCuts = kTRUE, Bool_t useCutHistoLimits = kFALSE) {
    gROOT->LoadMacro("$VMCWORKDIR/macro/run/bmnloadlibs.C");
    gStyle->SetOptStat(0);
    bmnloadlibs(); // load BmnRoot libraries
    // -----   Timer   ---------------------------------------------------------
    TStopwatch timer;
    timer.Start();

    gROOT->LoadMacro("$VMCWORKDIR/macro/run/geometry.C");

    TChain* out = new TChain("cbmsim");
    out->Add(fileName.Data());
    cout << "#recorded entries = " << out->GetEntries() << endl;
    if (out->GetEntries() == 0) {
        cout << "Something is wrong! Please, check file you passed as input!" << endl;
        exit(-1);
    }

    TClonesArray* particlePair = NULL;
    out->SetBranchAddress("ParticlePair", &particlePair);

    TCanvas* armenPodol = new TCanvas("c3", "c3", 800, 800);
    armenPodol->Divide(1, 1);

    TH2F* armenPodolPlot = new TH2F("ArmenPodol", "ArmenPodol", 50, 0., +1., 50, 0.001, 0.3);
    //armenPodol->cd(1)->SetLogz();
    armenPodolPlot->SetTitle("Armenteros-Podolyanski plot");

    TCanvas* canvLambda = new TCanvas("c2", "c2", 800, 800);
    canvLambda->Divide(1, 1);

    Double_t rL = 1.07;
    Double_t rB = 1.22;
    TH1F* invMassSpectrum = new TH1F("invMassSpectrum", "invMassSpectrum", 75, rL, rB);
    invMassSpectrum->SetTitle("Invariant mass: #Lambda^{0} #rightarrow p + #pi^{-}");

    TCanvas* canv = new TCanvas("c1", "c1", 800, 800);
    canv->Divide(2, 5);

    const Int_t nHists = 10;
    TH2F * hists[nHists];
    TString titles[nHists] = {
        "Inv. mass vs. P_{p}",
        "Inv. mass vs. P_{#pi^{-}}",
        "Inv. mass vs. Y_{p}",
        "Inv. mass vs. Y_{#pi^{-}}",
        "Inv. mass vs. DCA1",
        "Inv. mass vs. DCA2",
        "Inv. mass vs. DCA12X",
        "Inv. mass vs. path",
        "Inv. mass vs. DCA12Y",
        "Inv. mass vs. |DCA12x - DCA12Y|"
    };

    TString axisXtitles[nHists] = {
        "P_{p} [GeV/c]",
        "P_{#pi^{-}} [GeV/c]",
        "Y_{p}",
        "Y_{#pi^{-}}",
        "DCA_{1} [cm]",
        "DCA_{2} [cm]",
        "DCA_{12X} [cm]",
        "Path [cm]",
        "DCA_{12Y} [cm]",
        "|DCA_{12X} - DCA_{12Y} [cm]|"
    };

    TString axisYtitles[nHists];
    for (Int_t iHist = 0; iHist < nHists; iHist++)
        axisYtitles[iHist] = "M_{inv} [GeV/c^{2}]";

    Int_t nBinsX[nHists];
    Int_t nBinsY[nHists];
    Double_t xLow[nHists];
    Double_t xUp[nHists] = {10., 10., 5., 5., 2., 2., 2., 50., 2., 1.};
    Double_t yLow[nHists];
    Double_t yUp[nHists];

    for (Int_t iHist = 0; iHist < nHists; iHist++) {
        nBinsX[iHist] = 30;
        nBinsY[iHist] = 30;
        xLow[iHist] = 0.;
        yLow[iHist] = rL;
        yUp[iHist] = rB;
    }

    if (useCutHistoLimits) {
        xLow[0] = Mom1[0];
        xUp[0] = Mom1[1];

        xLow[1] = Mom2[0];
        xUp[1] = Mom2[1];

        xLow[2] = Eta1[0];
        xUp[2] = Eta2[1];

        xLow[3] = Eta2[0];
        xUp[3] = Eta2[1];

        xLow[4] = Dca1[0];
        xUp[4] = Dca1[1];

        xLow[5] = Dca2[0];
        xLow[5] = Dca2[1];

        xLow[6] = Dca12X[0];
        xUp[6] = Dca12X[1];

        xLow[7] = 0.;
        xUp[7] = Path;
    }


    TString names[nHists];

    for (Int_t iHist = 0; iHist < nHists; iHist++) {
        names[iHist] = titles[iHist];
        hists[iHist] = new TH2F(titles[iHist].Data(), names[iHist].Data(),
                nBinsX[iHist], xLow[iHist], xUp[iHist],
                nBinsY[iHist], yLow[iHist], yUp[iHist]);
        hists[iHist]->GetXaxis()->SetLabelSize(0.06);
        hists[iHist]->GetYaxis()->SetLabelSize(0.06);
        hists[iHist]->GetXaxis()->SetTitleSize(0.06);
        hists[iHist]->GetYaxis()->SetTitleSize(0.07);
        hists[iHist]->GetXaxis()->SetTitle(axisXtitles[iHist].Data());
        hists[iHist]->GetYaxis()->SetTitle(axisYtitles[iHist].Data());
        hists[iHist]->GetYaxis()->CenterTitle(kTRUE);
        hists[iHist]->GetYaxis()->SetTitleOffset(1.3);
    }

    TString _par = "X";
    TString _parY = "Y";
    // Fill histos ...
    for (Int_t iEv = 0; iEv < out->GetEntries(); iEv++) {
        out->GetEntry(iEv);
        if (iEv % 1000000 == 0)
            cout << "Ev# = " << iEv << endl;

        for (Int_t iPair = 0; iPair < particlePair->GetEntriesFast(); iPair++) {
            BmnParticlePair* pair = (BmnParticlePair*) particlePair->UncheckedAt(iPair);
            Double_t invMass = pair->GetInvMass(_par);
            Double_t mom1 = pair->GetMomPart1();
            Double_t mom2 = pair->GetMomPart2();
            Double_t eta1 = pair->GetEtaPart1();
            Double_t eta2 = pair->GetEtaPart2();
            Double_t dca1 = pair->GetDCA1();
            Double_t dca2 = pair->GetDCA2();
            Double_t dca12X = pair->GetDCA12(_par);
            Double_t dca12Y = pair->GetDCA12(_parY);
            Double_t path = pair->GetPath(_par);

            Double_t alpha = pair->GetAlpha(_par);
            Double_t ptPodol = pair->GetPtPodol(_par);

            Double_t V0XZ = pair->GetV0XZ();

            // Cuts to be applied ...
            if (useCuts) {
                //                if (alpha < Alpha)
                //                    continue;
                //                if (ptPodol > PtPodol)
                //                    continue;
                //
                //                if (path < 1.5)
                //                   continue;

                if (mom1 < Mom1[0] || mom1 > Mom1[1])
                    continue;
                
                if (eta1 < Eta1[0] || eta1 > Eta1[1])
                    continue;
                
                if (eta2 < Eta2[0] || eta2 > Eta2[1])
                    continue;

                if (dca12X < Dca12X[0] || dca12X > Dca12X[1])
                    continue;

                //                if (dca12Y < Dca12Y[0] || dca12Y > Dca12Y[1])
                //                    continue;

                if (dca1 < Dca1[0] || dca1 > Dca1[1])
                    continue;

                if (dca2 < Dca2[0] || dca2 > Dca2[1])
                    continue;

                if (mom2 < Mom2[0] || mom2 > Mom2[1])
                    continue;
            }

            hists[0]->Fill(mom1, invMass);
            hists[1]->Fill(mom2, invMass);
            hists[2]->Fill(eta1, invMass);
            hists[3]->Fill(eta2, invMass);
            hists[4]->Fill(dca1, invMass);
            hists[5]->Fill(dca2, invMass);
            hists[6]->Fill(dca12X, invMass);
            hists[7]->Fill(path, invMass);
            hists[8]->Fill(dca12Y, invMass);
            hists[9]->Fill(Abs(dca12X - dca12Y), invMass);

            invMassSpectrum->Fill(invMass);
            armenPodolPlot->Fill(alpha, ptPodol);
        }
    }

    for (Int_t iHist = 0; iHist < nHists; iHist++) {
        TVirtualPad* pad = canv->cd(iHist + 1);
        pad->SetBottomMargin(0.2);
        pad->SetLeftMargin(0.2);
        // pad->SetLogz();
        hists[iHist]->Draw("colz");
    }

    Double_t par[8];
    TF1* bg = new TF1("BG", "pol4", rL, rB);
    TF1* sig = new TF1("SIG", "gaus", 1.11, 1.12);
    TVirtualPad* pad = canvLambda->cd();
    pad->SetLeftMargin(0.2);
    invMassSpectrum->Fit(bg, "R");
    //sig->SetParameter(1, 1.115);
    //    sig->SetParameter(2, 0.002);
    invMassSpectrum->Fit(sig, "R");
    bg->GetParameters(&par[0]);
    sig->GetParameters(&par[5]);
    TF1 *f = new TF1("f", "pol4(0)+gaus(5)", rL, rB);
    f->SetNpx(500);

    f->SetParameters(par);
    f->SetLineColor(kMagenta + 1);
    f->SetLineWidth(3);
    TString fitOpt = "RS";
    //f->SetParameter(6, 1.1152);
    f->SetParameter(7, 0.002);
    TFitResultPtr fitRes = invMassSpectrum->Fit(f, (useCuts) ? fitOpt.Data() : TString(fitOpt + "0").Data());
    Double_t mean = fitRes->Parameter(6);
    Double_t sigma = fitRes->Parameter(7);
    Double_t T = invMassSpectrum->Integral(invMassSpectrum->FindBin(mean - 3 * sigma), invMassSpectrum->FindBin(mean + 3 * sigma));
    Double_t B = bg->Integral(mean - 3 * sigma, mean + 3 * sigma) / invMassSpectrum->GetBinWidth(1);
    Double_t S_to_B = T / B - 1;
    Double_t Signif = (T - B) / TMath::Sqrt(T);

    invMassSpectrum->SetMarkerStyle(20);
    invMassSpectrum->SetMarkerColor(kSpring - 6);
    invMassSpectrum->SetLineColor(kSpring - 6);
    invMassSpectrum->SetLineWidth(1);

    canvLambda->cd(1);
    invMassSpectrum->Draw("PE1X0");
    //invMassSpectrum->GetYaxis()->SetRangeUser(0., 40.);
    invMassSpectrum->GetXaxis()->SetTitle("M_{(p + #pi^{-})}, GeV/c^{2}");
    invMassSpectrum->GetYaxis()->SetTitle("Entries / 2 MeV/c^{2}");
    invMassSpectrum->GetXaxis()->SetLabelSize(0.035);
    invMassSpectrum->GetYaxis()->SetLabelSize(0.035);
    invMassSpectrum->GetXaxis()->SetTitleSize(0.05);
    invMassSpectrum->GetYaxis()->SetTitleSize(0.05);
    invMassSpectrum->GetXaxis()->SetTitleOffset(0.85);
    invMassSpectrum->GetYaxis()->SetTitleOffset(1.5);
    invMassSpectrum->GetYaxis()->CenterTitle();

    // Draw the legend
    TLegend *legend = new TLegend(0.6, 0.65, 0.61, 0.85);
    legend->SetTextFont(72);
    legend->SetTextSize(0.04);
    legend->AddEntry("", Form("Mass = %.4f", mean), 0);
    legend->AddEntry("", Form("Sigma = %.4f", sigma), 0);
    legend->AddEntry("", Form("S/B = %.4f", S_to_B), 0);
    legend->AddEntry("", Form("S/#sqrt{S + B} = %.4f", Signif), 0);
    legend->SetLineColor(0);
    if (useCuts)
        legend->Draw();

    armenPodol->cd(1);
    armenPodolPlot->Draw("colz");

    // -----   Finish   --------------------------------------------------------
    canv->SaveAs("L1.pdf");
    canvLambda->SaveAs("L2.pdf");
    armenPodol->SaveAs("L3.pdf");
    delete out;
    delete canv;
    for (Int_t iHist = 0; iHist < nHists; iHist++)
        delete hists[iHist];
    delete hists;
    timer.Stop();
    Double_t rtime = timer.RealTime();
    Double_t ctime = timer.CpuTime();
    cout << endl << endl;
    cout << "Macro finished successfully." << endl;
    cout << "Real time " << rtime << " s, CPU time " << ctime << " s" << endl;
    cout << endl;
}

