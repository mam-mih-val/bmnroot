#include <TClonesArray.h>
#include <TChain.h>
#include <TH1F.h>
#include <TF1.h>
#include <TStyle.h>
#include <TLegend.h>
#include <TList.h>
#include <TLegendEntry.h>

// As input, put here a file obtained with LambdaAnal.C
void fitInvSpectrum(Char_t* fileName = "reco.root") {
    gROOT->LoadMacro("$VMCWORKDIR/macro/run/bmnloadlibs.C");
    gStyle->SetOptStat(0);
    bmnloadlibs(); // load BmnRoot libraries
    // -----   Timer   ---------------------------------------------------------
    TStopwatch timer;
    timer.Start();

    gROOT->LoadMacro("$VMCWORKDIR/macro/run/geometry.C");

    TChain* out = new TChain("cbmsim");
    out->Add(fileName);
    cout << "#recorded entries = " << out->GetEntries() << endl;

    TClonesArray* particlePair = NULL;
    TClonesArray* particlePairCuts = NULL;
    out->SetBranchAddress("ParticlePair", &particlePair);
    out->SetBranchAddress("ParticlePairCuts", &particlePairCuts);

    Double_t rB = 1.16;
    TH1F* invMassSpectrum = new TH1F("invMassSpectrum", "invMassSpectrum", 50, 1.07, rB);

    for (Int_t iEv = 0; iEv < out->GetEntries(); iEv++) {
        out->GetEntry(iEv);

        for (Int_t iPair = 0; iPair < particlePair->GetEntriesFast(); iPair++) {
            BmnParticlePair* pair = (BmnParticlePair*) particlePair->UncheckedAt(iPair);
            Double_t V0VpDist = pair->GetV0VpDist(); 
            Double_t V0ProtPionDist = pair->GetV0Part1Part2(); 
            Double_t VpProt = pair->GetVpPart1();
            Double_t VpPion = pair->GetVpPart2();
            
            if (VpPion < 0.4) 
                continue;
            invMassSpectrum->Fill(pair->GetInvMass());
        }
    }

    Double_t par[6];
    TF1 *bg = new TF1("BG", "pol2", 1.08, rB);
    TF1 *sig = new TF1("SIG", "gaus", 1.11, 1.12);
    invMassSpectrum->Fit(bg, "R");    
    invMassSpectrum->Fit(sig, "R");
    bg->GetParameters(&par[0]);
    sig->GetParameters(&par[3]);
    TF1 *f = new TF1("f", "pol2(0)+gaus(3)", 1.08, rB);
    f->SetNpx(500);
    
    f->SetParameters(par);
    f->SetLineColor(kMagenta+1);
    f->SetLineWidth(3);
    TFitResultPtr fitRes = invMassSpectrum->Fit(f, "wRS");   
    Double_t mean = fitRes->Parameter(4);
    Double_t sigma = fitRes->Parameter(5);
    Double_t T = invMassSpectrum->Integral(invMassSpectrum->FindBin(mean - 3 * sigma), invMassSpectrum->FindBin(mean + 3 * sigma));
    Double_t B = bg->Integral(mean - 3 * sigma, mean + 3 * sigma) / invMassSpectrum->GetBinWidth(1);
    Double_t S_to_B = T / B - 1;
    Double_t Signif =  (T - B) / TMath::Sqrt(T);

    invMassSpectrum->SetMarkerStyle(20);
    invMassSpectrum->SetMarkerColor(kSpring-6);
    invMassSpectrum->SetLineColor(kSpring-6);
    invMassSpectrum->SetLineWidth(1);
    invMassSpectrum->Draw("PE1X0");
    
    // Draw the legend
    TLegend *legend=new TLegend(0.6,0.65,0.88,0.85);
    legend->SetTextFont(72);
    legend->SetTextSize(0.04);
    legend->AddEntry("", Form("Mass = %.3f", mean), 0);
    legend->AddEntry("", Form("Sigma = %.3f", sigma), 0);
    legend->AddEntry("", Form("S/B = %.3f", S_to_B), 0);
    legend->AddEntry("", Form("S/#sqrt{S + B} = %.3f", Signif), 0);
    legend->Draw();

    // -----   Finish   --------------------------------------------------------
    timer.Stop();
    Double_t rtime = timer.RealTime();
    Double_t ctime = timer.CpuTime();
    cout << endl << endl;
    cout << "Macro finished successfully." << endl;
    cout << "Real time " << rtime << " s, CPU time " << ctime << " s" << endl;
    cout << endl;
}