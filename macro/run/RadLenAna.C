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
#include <Rtypes.h>
R__ADD_INCLUDE_PATH($VMCWORKDIR)
#include "macro/run/bmnloadlibs.C"

using namespace TMath;

void DrawHisto(TH2F* h, TString xName, TString yName, TCanvas *c);
void DrawHisto(TH1F* h, TString xName, TString yName, TCanvas *c);

void RadLenAna(TString infile1 = "RadLenSim.root") {

    TH2F *hXY = new TH2F("hXY", "Material budget in the BM@N", 3000, -400., 400., 3000, -300., 300.);
    TH2F *hZX_narrow = new TH2F("hZX_narrow", "Material budget in the BM@N, |y| < 10.0 cm", 3000, -200., 800., 1500, -400., 400.);
    TH2F *hZY_narrow = new TH2F("hZY_narrow", "Material budget in the BM@N, |y| < 10.0 cm", 3000, -200., 800., 1500, -300., 300.);
    TH2F *hZX = new TH2F("hZX", "Material budget in the BM@N", 3000, -200., 800., 1500, -400., 400.);
    TH2F *hZY = new TH2F("hZY", "Material budget in the BM@N", 3000, -200., 800., 1500, -300., 300.);
    TH2F *hEtaDist = new TH2F("hEtaDist", "Material budget in the BM@N", 3000, -3., 8., 3000, 0., 1200.);

    const Int_t nBinsZ = 1000;
    const Int_t nBinsEta = 100;
    const Int_t nBinsTheta = 500;
    const Int_t nBinsPhi = 500;

    TH1F *hZ = new TH1F("hZ", "Material budget in the BM@N for |x| < 1.0 cm & |y| < 1.0 cm", nBinsZ, 0., 1000.);
    TH1F *hZsum = new TH1F("hZsum", "Integrated material budget in the BM@N for |x| < 1.0 cm & |y| < 1.0 cm", nBinsZ, 0., 1000.);
    Int_t aZ[nBinsZ];
    for (Int_t i = 0; i < nBinsZ; ++i) aZ[i] = 0;

    TH1F *hEta = new TH1F("hEta", "Material budget in the BM@N", nBinsEta, -2., 3.);
    Int_t aEta[nBinsEta];
    for (Int_t i = 0; i < nBinsEta; ++i) aEta[i] = 0;

    Int_t aThetaPhi[nBinsPhi][nBinsTheta];
    for (Int_t i = 0; i < nBinsPhi; ++i)
        for (Int_t j = 0; j < nBinsTheta; ++j)
            aThetaPhi[i][j] = 0;
    TH2F *hThetaPhi = new TH2F("ThetaPhi", "Material budget in the BM@N", nBinsPhi, -180, 180, nBinsTheta, 0., 180.);

    Int_t aTheta[nBinsEta];
    for (Int_t i = 0; i < nBinsEta; ++i) aTheta[i] = 0;
    TH1F *hTheta = new TH1F("hTheta", "Material budget in the BM@N", nBinsEta, 0., 180.);

    bmnloadlibs(); // load bmn libraries

    TChain *tree = new TChain("cbmsim");
    tree->Add(infile1);

    // Activate branches
    TClonesArray *fRadLenPs = NULL;
    tree->SetBranchAddress("RadLen", &fRadLenPs);
    TClonesArray *fMCTracks = NULL;
    tree->SetBranchAddress("MCTrack", &fMCTracks);

    Int_t events = tree->GetEntries();
    cout << " Processing RadLenSimZR..." << endl;
    cout << " Number of events in file = " << events << endl;

    TVector3 PosIn, PosOut, DistVec;

    //    for (Int_t iEv = 0; iEv < events; iEv++) {
    //        tree->GetEntry(iEv);
    //
    //        Int_t fNpoints = fRadLenPs->GetEntriesFast();
    //
    //        for (Int_t pointIndex = 0; pointIndex < fNpoints; pointIndex++) {
    //            FairRadLenPoint* RadLenP = (FairRadLenPoint*) fRadLenPs->At(pointIndex);
    //            Float_t Len = RadLenP->GetRadLength();
    //            PosIn = RadLenP->GetPosition();
    //            PosOut = RadLenP->GetPositionOut();
    //            DistVec = PosIn - PosOut;
    //            Float_t Dist = DistVec.Mag();
    //            Float_t LenEff = Dist / Len;
    //
    //            Float_t X = PosIn.X();
    //            Float_t Y = PosIn.Y();
    //            Float_t Z = PosIn.Z();
    //            Float_t Theta = PosIn.Theta();
    //            Float_t Eta = -Log(Tan(Theta / 2));
    //
    //            hXY->Fill(X, Y, LenEff);
    //            if (Abs(Y) < 10) hZX_narrow->Fill(Z, X, LenEff);
    //            hZX->Fill(Z, X, LenEff);
    //            if (Abs(X) < 10) hZY_narrow->Fill(Z, Y, LenEff);
    //            hZY->Fill(Z, Y, LenEff);
    //            hEtaDist->Fill(Eta, PosIn.Mag(), LenEff);
    //            if (Abs(X) < 10 && Abs(Y) < 10) hZ->Fill(Z, LenEff);
    //
    //            if (pointIndex % 100000 == 0) cout << pointIndex * 100.0 / fNpoints << "% processed" << endl;
    //        }
    //        cout << iEv + 1 << " out of " << events << " events processed" << endl;
    //    }

    for (Int_t iEv = 0; iEv < events; iEv++) {
        tree->GetEntry(iEv);

        for (Int_t iTr = 0; iTr < fMCTracks->GetEntriesFast(); iTr++) {

            CbmMCTrack* mcTrack = (CbmMCTrack*) fMCTracks->At(iTr);
            Double_t sumEta = 0.0;
            Double_t sumRadLenEff = 0.0;
            Int_t nPnts = 0;
            Float_t Eta = 0.0;
            Float_t Phi = 0.0;
            Float_t Theta = 0.0;

            for (Int_t pointIndex = 0; pointIndex < fRadLenPs->GetEntriesFast(); pointIndex++) {
                FairRadLenPoint* RadLenP = (FairRadLenPoint*) fRadLenPs->At(pointIndex);
                if (RadLenP->GetTrackID() != iTr) continue;
                Float_t Len = RadLenP->GetRadLength();
                PosIn = RadLenP->GetPosition();
                PosOut = RadLenP->GetPositionOut();
                DistVec = PosIn - PosOut;
                Float_t Dist = DistVec.Mag();
                Float_t LenEff = Dist / Len;
                Float_t X = PosIn.X();
                Float_t Y = PosIn.Y();
                Float_t Z = PosIn.Z();
                Theta = PosIn.Theta();
                Eta = -Log(Tan(Theta / 2));
                Theta *= TMath::RadToDeg();
                Phi = PosIn.Phi() * TMath::RadToDeg();

                sumRadLenEff += LenEff;
                nPnts++;

                if (Abs(X) < 1 && Abs(Y) < 1) {
                    Int_t binZ = hZ->GetXaxis()->FindBin(Z);
                    aZ[binZ]++;
                    hZ->Fill(Z, LenEff);
                }
                //if (pointIndex % 10000 == 0) cout << pointIndex * 100.0 / fRadLenPs->GetEntriesFast() << "% processed" << endl;
            }
            if (nPnts > 0) {
                //                if (iTr % 10 == 0) cout << "n points on track " << iTr << ": " << nPnts << endl;
                Int_t binPhi2D = hThetaPhi->GetXaxis()->FindBin(Phi);
                Int_t binTheta2D = hThetaPhi->GetYaxis()->FindBin(Theta);
                Int_t binEta = hEta->GetXaxis()->FindBin(Eta);
                Int_t binTheta = hTheta->GetXaxis()->FindBin(Theta);
                
                aEta[binEta]++;
                aTheta[binTheta]++;
                hEta->Fill(Eta, sumRadLenEff);
                hTheta->Fill(Theta, sumRadLenEff);
                aThetaPhi[binPhi2D][binTheta2D]++;
                hThetaPhi->Fill(Phi, Theta, sumRadLenEff);
            }
        }
        cout << iEv + 1 << " out of " << events << " events processed" << endl;
    }

    hEta->SetBinContent(nBinsEta, 0.0);
    hEta->SetBinError(nBinsEta, 0.0);
    hTheta->SetBinContent(nBinsEta, 0.0);
    hTheta->SetBinError(nBinsEta, 0.0);

    hZ->SetBinContent(nBinsZ, 0.0);
    hZ->SetBinError(nBinsZ, 0.0);
    hZ->SetBinContent(0, 0.0);
    hZ->SetBinError(0, 0.0);
    
    Double_t integrZ = 0.0;
    for (Int_t i = 0; i < nBinsZ; ++i) {
        if (aZ[i] != 0) {
            hZ->SetBinContent(i, hZ->GetBinContent(i) / aZ[i]);
            hZ->SetBinError(i, 0.0);
            integrZ += hZ->GetBinContent(i);
        }
        hZsum->SetBinContent(i, integrZ);
    }

    for (Int_t i = 0; i < nBinsEta; ++i) {
        if (aEta[i] != 0) {
            hEta->SetBinContent(i, hEta->GetBinContent(i) / aEta[i]);
            hEta->SetBinError(i, 0.0);
        }
        if (aTheta[i] != 0) {
            hTheta->SetBinContent(i, hTheta->GetBinContent(i) / aTheta[i]);
            hTheta->SetBinError(i, 0.0);
        }
    }

    for (Int_t i = 0; i < nBinsPhi; ++i) {
        hThetaPhi->SetBinContent(i, nBinsTheta, 0.0);
        hThetaPhi->SetBinError(i, nBinsTheta, 0.0);
        hThetaPhi->SetBinContent(i, 0, 0.0);
        hThetaPhi->SetBinError(i, 0, 0.0);
    }
    for (Int_t i = 0; i < nBinsTheta; ++i) {
        hThetaPhi->SetBinContent(nBinsPhi, i, 0.0);
        hThetaPhi->SetBinError(nBinsPhi, i, 0.0);
        hThetaPhi->SetBinContent(0, i, 0.0);
        hThetaPhi->SetBinError(0, i, 0.0);
    }

    for (Int_t i = 0; i < nBinsPhi; ++i)
        for (Int_t j = 0; j < nBinsTheta; ++j)
            if (aThetaPhi[i][j] != 0) {
                hThetaPhi->SetBinContent(i, j, hThetaPhi->GetBinContent(i, j) / aThetaPhi[i][j]);
                hThetaPhi->SetBinError(i, j, 0.0);
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
    
    
    TCanvas *c7 = new TCanvas(hZ->GetName(), hZ->GetName(), 1000, 500);
    DrawHisto(hZ, "Z, cm", "Radiation length, X/X0", c7);
    TCanvas *c71 = new TCanvas(hZsum->GetName(), hZsum->GetName(), 1000, 500);
    DrawHisto(hZsum, "Z, cm", "Integrated radiation length, X/X0", c71);

    TCanvas *c8 = new TCanvas(hEta->GetName(), hEta->GetName(), 1000, 1000);
    DrawHisto(hEta, "#eta", "Radiation length, X/X0", c8);
    TCanvas *c9 = new TCanvas(hThetaPhi->GetName(), hThetaPhi->GetName(), 1000, 1000);
    DrawHisto(hThetaPhi, "#phi, deg", "#theta, deg", c9);
    TCanvas *c10 = new TCanvas(hTheta->GetName(), hTheta->GetName(), 1000, 1000);
    DrawHisto(hTheta, "#theta, deg", "Radiation length, X/X0", c10);

}

void DrawHisto(TH2F* h, TString xName, TString yName, TCanvas *c) {
    h->GetXaxis()->SetTitle(xName);
    h->GetXaxis()->SetTitleOffset(0.8);
    h->GetXaxis()->CenterTitle();
    h->GetYaxis()->SetTitle(yName);
    h->GetYaxis()->SetTitleOffset(0.8);
    h->GetYaxis()->CenterTitle();
    h->GetZaxis()->SetTitle("Radiation length, X/X0");
    h->GetZaxis()->SetTitleOffset(0.7);
    //    c->SetLogz();
    //    h->SetMaximum(10.);
    h->Draw("colz");
    c->SaveAs(Form("%s.pdf", h->GetName()));
}

void DrawHisto(TH1F* h, TString xName, TString yName, TCanvas *c) {
    h->GetXaxis()->SetTitle(xName);
    h->GetXaxis()->SetTitleOffset(0.8);
    h->GetXaxis()->CenterTitle();
    h->GetYaxis()->SetTitle(yName);
    h->GetYaxis()->SetTitleOffset(0.8);
    h->GetYaxis()->CenterTitle();
    //    h->SetMaximum(10.);
    h->Draw("L");
    c->SaveAs(Form("%s.pdf", h->GetName()));
}
