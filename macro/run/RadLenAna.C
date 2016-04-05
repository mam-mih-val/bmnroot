/* Macro draws histograms of radiation length scan of the BM@N setup
 * Output result values are "effective radiation lengths" which are calculated 
 * as radiation length (cm) divided by distance (cm).
 */

#include <TVector3.h>
#include <TChain.h>
#include <TClonesArray.h>
#include <TH2F.h>
#include "TROOT.h"
#include "TMath.h"

using namespace TMath;

void RadLenAna(TString infile1 = "RadLenSim.root") {

    TH2F *hXY = new TH2F("hXY", "Material budget in the BM@N", 3000, -400., 400., 3000, -300., 300.);
    TH2F *hZX_narrow = new TH2F("hZX_narrow", "Material budget in the BM@N, |y| < 10.0 cm", 3000, -200., 800., 1500, -400., 400.);
    TH2F *hZY_narrow = new TH2F("hZY_narrow", "Material budget in the BM@N, |y| < 10.0 cm", 3000, -200., 800., 1500, -300., 300.);
    TH2F *hZX = new TH2F("hZX", "Material budget in the BM@N", 3000, -200., 800., 1500, -400., 400.);
    TH2F *hZY = new TH2F("hZY", "Material budget in the BM@N", 3000, -200., 800., 1500, -300., 300.);
    TH2F *hEtaDist = new TH2F("hEtaDist", "Material budget in the BM@N", 3000, -3., 8., 3000, 0., 1200.);

    gROOT->LoadMacro("$VMCWORKDIR/macro/run/bmnloadlibs.C");
    bmnloadlibs(); // load bmn libraries

    TChain *tree = new TChain("cbmsim");
    tree->Add(infile1);

    // Activate branches
    TClonesArray *fRadLenPs;
    tree->SetBranchAddress("RadLen", &fRadLenPs);

    Int_t events = tree->GetEntries();
    cout << " Processing RadLenSimZR..." << endl;
    cout << " Number of events in file = " << events << endl;

    TVector3 PosIn, PosOut, DistVec;

    for (Int_t iEv = 0; iEv < events; iEv++) {
        tree->GetEntry(iEv);

        Int_t fNpoints = fRadLenPs->GetEntriesFast();

        for (Int_t pointIndex = 0; pointIndex < fNpoints; pointIndex++) {
            FairRadLenPoint* RadLenP = (FairRadLenPoint*) fRadLenPs->At(pointIndex);
            Float_t Len = RadLenP->GetRadLength();
            PosIn = RadLenP->GetPosition();
            PosOut = RadLenP->GetPositionOut();
            DistVec = PosIn - PosOut;
            Float_t Dist = DistVec.Mag();
            Float_t LenEff = Dist / Len;

            Float_t X = PosIn.X();
            Float_t Y = PosIn.Y();
            Float_t Z = PosIn.Z();
            Float_t Theta = PosIn.Theta();
            Float_t Eta = -Log(Tan(Theta / 2));

            hXY->Fill(X, Y, LenEff);
            if (Abs(Y) < 10) hZX_narrow->Fill(Z, X, LenEff);
            hZX->Fill(Z, X, LenEff);
            if (Abs(X) < 10) hZY_narrow->Fill(Z, Y, LenEff);
            hZY->Fill(Z, Y, LenEff);
            hEtaDist->Fill(Eta, PosIn.Mag(), LenEff);

            if (pointIndex % 100000 == 0) cout << pointIndex * 100.0 / fNpoints << "% processed" << endl;
        }
        cout << iEv + 1 << " out of " << events << " events processed" << endl;
    }

    gStyle->SetOptStat(0);

    TCanvas *c1 = new TCanvas(hXY->GetName(), hXY->GetName(), 800, 600);
    DrawHisto(hXY, "X, cm", "Y, cm", c1);
    TCanvas *c2 = new TCanvas(hZX->GetName(), hZX->GetName(), 1000, 800);
    DrawHisto(hZX, "Z, cm", "X, cm", c2);
    TCanvas *c3 = new TCanvas(hZX_narrow->GetName(), hZX_narrow->GetName(), 1000, 800);
    DrawHisto(hZX_narrow, "Z, cm", "X, cm", c3);
    TCanvas *c4 = new TCanvas(hZY->GetName(), hZY->GetName(), 1000, 600);
    DrawHisto(hZY, "Z, cm", "Y, cm", c4);
    TCanvas *c5 = new TCanvas(hZY_narrow->GetName(), hZY_narrow->GetName(), 1000, 600);
    DrawHisto(hZY_narrow, "Z, cm", "Y, cm", c5);
    TCanvas *c6 = new TCanvas(hEtaDist->GetName(), hEtaDist->GetName(), 1000, 500);
    DrawHisto(hEtaDist, "#eta", "Distance from IP, cm", c6);

}

void DrawHisto(TH2F* h, TString xName, TString yName, TCanvas *c) {
    h->GetXaxis()->SetTitle(xName);
    h->GetXaxis()->CenterTitle();
    h->GetYaxis()->SetTitle(yName);
    h->GetYaxis()->CenterTitle();
    h->GetZaxis()->SetTitle("Radiation length, X/X0");
    h->SetMaximum(10.);
    h->Draw("colz");
}
