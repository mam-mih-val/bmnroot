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

#include <TString.h>
#include <Rtypes.h>
R__ADD_INCLUDE_PATH($VMCWORKDIR)
#include "macro/run/bmnloadlibs.C"

using namespace std;
using namespace TMath;

const Int_t dim = 2;
// Redefine histo limits (supposing exclusion - less(idx0) or more(idx1)) due to cuts obtained (useCutHistoLimits should be kTRUE)

// C-Cu, T = 4 GeV/n
Double_t Mom1[dim] = {0., 3.};
Double_t Mom2[dim] = {0., 1.};
Double_t Eta1[dim] = {1.5, 2.5};
Double_t Eta2[dim] = {1.5, 2.5};

// more than in sense of exclusion ...
Double_t Dca1[dim] = {.1, 100.};
Double_t Dca2[dim] = {.1, 100.};

Double_t Dca12[dim]  = {0., 1.};
Double_t Path[dim] = {2.5, 20};

Double_t Alpha = 0.5;
Double_t PtPodol = 0.11;

void EvalQA(TString fileName = "tmp.root", Bool_t useCuts = false, Bool_t drawFit = true, TString outNameInvMass = "im.pdf")//, TString dstName = "dst-4649.root")
{
    gStyle->SetOptStat(1);
    bmnloadlibs(); // load BmnRoot libraries
    // -----   Timer   ---------------------------------------------------------
    TStopwatch timer;
    timer.Start();

    TChain* out = new TChain("bmndata");
    out->Add(fileName.Data());
//    TChain* dst = new TChain("bmndata");
//    dst->Add(dstName.Data());
    cout << "#recorded entries = " << out->GetEntries() << endl;
    if (out->GetEntries() == 0) {
        cout << "Something is wrong! Please, check file you passed as input!" << endl;
        exit(-1);
    }
//    cout << "#recorded entries = " << dst->GetEntries() << " in DST "<< endl;
//    if (dst->GetEntries() == 0) {
//        cout << "Something is wrong! Please, check file you passed as input!" << endl;
//        exit(-1);
//    }

    TClonesArray* particlePair = NULL;
    out->SetBranchAddress("ParticlePair", &particlePair);
//    DstEventHeader* dstEventHdr = NULL;
//    dst->SetBranchAddress("DstEventHeader.", &dstEventHdr);

    TCanvas* canvLambda = new TCanvas("c2", "c2", 800, 800);
    canvLambda->Divide(1, 1);

    Double_t rL = 1.1;
    Double_t rR = 1.22;
    TH1F* invMassSpectrum = new TH1F("invMassSpectrum", "invMassSpectrum", 75, rL, rR);
    invMassSpectrum->SetTitle("Invariant mass: #Lambda^{0} #rightarrow p + #pi^{-}");

    TString _par = "X";
    TString _parY = "Y";
    vector<TString> cutStrVec;
    if (useCuts) {
        cutStrVec.push_back(Form("%.1f < Path < %.1f", Path[0], Path[1]));
        cutStrVec.push_back(Form("%.1f < Dca12 < %.1f", Dca12[0], Dca12[1]));
    }

    for (Int_t iEv = 0; iEv < out->GetEntries(); iEv++) {
        out->GetEntry(iEv);
//        dst->GetEntry(iEv);
        if (iEv % 10000 == 0)
            cout << "Ev# = " << iEv << endl;

        for (Int_t iPair = 0; iPair < particlePair->GetEntriesFast(); iPair++) {
            BmnParticlePair* pair = (BmnParticlePair*) particlePair->UncheckedAt(iPair);
            Double_t invMass = pair->GetInvMass();
            Double_t mom1 = pair->GetMomPart1();
            Double_t mom2 = pair->GetMomPart2();
            Double_t eta1 = pair->GetEtaPart1();
            Double_t eta2 = pair->GetEtaPart2();
            Double_t dca1 = pair->GetDCA1();
            Double_t dca2 = pair->GetDCA2();
            Double_t dca12 = pair->GetDCA12();
            Double_t path = pair->GetPath();

            Double_t alpha = pair->GetAlpha();
            Double_t ptPodol = pair->GetPtPodol();

            // Cuts to be applied ...
            if (useCuts) {
                //                if (alpha < Alpha)
                //                    continue;
                //                if (ptPodol > PtPodol)
                //                    continue;
                //
                if (path < Path[0] || path > Path[1])
                    continue;

                //  if (mom1 < Mom1[0] || mom1 > Mom1[1])
                //	continue;

                // if (eta1 < Eta1[0] || eta1 > Eta1[1])
                //	continue;

                // if (eta2 < Eta2[0] || eta2 > Eta2[1])
                //	continue;
                if (dca12 < Dca12[0] || dca12 > Dca12[1])
                    continue;

                //  if (dca1 < Dca1[0] || dca1 > Dca1[1])
                //     continue;

                //  if (dca2 < Dca2[0] || dca2 > Dca2[1])
                //      continue;

                //  if (mom2 < Mom2[0] || mom2 > Mom2[1])
                //      continue;
            }
//            if (dstEventHdr->GetADC1() <1200 || dstEventHdr->GetADC1() > 1600)
//                continue;
                
            invMassSpectrum->Fill(invMass);
        }
    }


    Double_t par[8];
    TF1* bg = new TF1("BG", "pol4", rL, rR);
    invMassSpectrum->Fit(bg, "R");
    bg->GetParameters(&par[0]);
    
    Double_t rL_Resonance = 1.1;
    Double_t rR_Resonance = 1.13;
    const Double_t bgCenter = bg->Eval((rL_Resonance+rR_Resonance)/2.0);
    printf("center %f\n", bgCenter);
    TF1* sig = new TF1(
            "SIG",
            [bgCenter](Double_t *x, Double_t *p){
                return p[0]*Exp(-0.5*pow((x[0]-p[1])/p[2], 2))/*/(p[2] * sqrt(2 * M_PI))*/+bgCenter;
            },
                    rL_Resonance, rR_Resonance, 3);
    Double_t temp[3] = {100, (rR_Resonance + rL_Resonance)/2.0, (rR_Resonance - rL_Resonance)/100.0};
    sig->SetParameters(temp);
    invMassSpectrum->Fit(sig, "R");
    sig->GetParameters(&par[5]);
    
    TVirtualPad* pad = canvLambda->cd();
    pad->SetLeftMargin(0.2);
    TF1 *f = new TF1("f", "pol4(0)+gaus(5)", rL, rR);
    f->SetNpx(500);

    f->SetParameters(par);
    f->SetLineColor(kMagenta + 1);
    f->SetLineWidth(3);
    TString fitOpt = "RS";
    TFitResultPtr fitRes = invMassSpectrum->Fit(f, (drawFit) ? fitOpt.Data() : TString(fitOpt + "0").Data());
    Double_t mean = fitRes->Parameter(6);
    Double_t sigma = fitRes->Parameter(7);
    Double_t T = invMassSpectrum->Integral(invMassSpectrum->FindBin(mean - 3 * sigma), invMassSpectrum->FindBin(mean + 3 * sigma));
    Double_t B = bg->Integral(mean - 3 * sigma, mean + 3 * sigma) / invMassSpectrum->GetBinWidth(1);
    Double_t S = sig->Integral(mean - 3 * sigma, mean + 3 * sigma) / invMassSpectrum->GetBinWidth(1);
    Double_t S_to_B = T / B - 1;
    Double_t Signif = (T - B) / T;
    printf("Tot %f  Bg %f sig %f\n", T, B, S);
    f->GetParameters(par);
    delete sig;
    sig = new TF1("SIG", "gaus", rL_Resonance, rR_Resonance);
    sig->SetParameters(&par[5]);
    bg->SetParameters(&par[0]);
    T = invMassSpectrum->Integral(invMassSpectrum->FindBin(mean - 3 * sigma), invMassSpectrum->FindBin(mean + 3 * sigma));
    B = bg->Integral(mean - 3 * sigma, mean + 3 * sigma) / invMassSpectrum->GetBinWidth(1);
    S = sig->Integral(mean - 3 * sigma, mean + 3 * sigma) / invMassSpectrum->GetBinWidth(1);
    S_to_B = T / B - 1;
    Signif = (T - B) / T;
    printf("Tot %f  Bg %f sig %f after refit\n", T, B, S);
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
    invMassSpectrum->SetMinimum(0);

    // Draw the legend
    Double_t legendX = 0.55;
    Double_t legendY = 0.85;
    TLegend *legend = new TLegend(legendX, legendY - (4 + cutStrVec.size() )* 0.05, legendX + 0.05, legendY);
    legend->SetTextFont(72);
    legend->SetTextSize(0.03);
    legend->AddEntry("", Form("Mass = %.4f", mean), 0);
    legend->AddEntry("", Form("  #sigma    = %.4f", sigma), 0);
    legend->AddEntry("", Form("S/B = %.4f", S_to_B), 0);
    legend->AddEntry("", Form("S/(S + B) = %.4f", Signif), 0);
    legend->AddEntry("", Form("Sig integral = %.4f", S), 0);
    if (cutStrVec.size() > 0)
        legend->AddEntry("", "", 0);
    for (TString &cutS : cutStrVec) {
        legend->AddEntry("", cutS.Data(), 0);
    }
    legend->SetLineColor(0);
    //    if (useCuts){
    legend->Draw();
    sig->Draw("SAME");
    //    }
    // -----   Finish   --------------------------------------------------------
    canvLambda->SaveAs(outNameInvMass.Data());
    delete out;
//    delete dst;
    timer.Stop();
    Double_t rtime = timer.RealTime();
    Double_t ctime = timer.CpuTime();
    cout << endl << endl;
    cout << "Macro finished successfully." << endl;
    cout << "Real time " << rtime << " s, CPU time " << ctime << " s" << endl;
    cout << endl;
}

