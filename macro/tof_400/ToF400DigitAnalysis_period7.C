/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   TestMacro.C
 * Author: mikhailr
 *
 * Created on March 5, 2018, 5:14 PM
 */

#include <cstdlib>
#include <ctime>
#include "TStopwatch.h"

using namespace std;

Double_t AmpBD, TimeBD, NHitBD;

Bool_t FlagHit = kFALSE;
Int_t XforHit = 0, YforHit = 0;
const Int_t NDet = 20;

void ToF400DigitAnalysis_period7(TString file = "/home/storage/digits/bmn_run2310_digi.root", Int_t nEvForRead = 0) {

    gROOT->LoadMacro("$VMCWORKDIR/macro/run/bmnloadlibs.C");
    bmnloadlibs(); // load BmnRoot libraries

    BmnTOF1Detector * Plane[NDet];
    TString name;
    for (Int_t i = 0; i < NDet; i++) {
        name = Form("Plane%d", i);
        Plane[i] = new BmnTOF1Detector(i, 2);
        //Plane[i]->SetCorrLR(CorrLR[i]);
        Plane[i]->SetCorrLR("TOF400_LRcorr_RUN7.dat");
        //Plane[i]->SetCorrSlewing("Tof400SlewingCorr_period6.root");
        Plane[i]->SetGeoFile("geofile_full_RUN7.root");
        //    Plane[i]->SetGeoFile("geometry_run6.root");
    }

    TList *fList = new TList();
    TList *fListTof = new TList();

    TH2I *hCrosing = new TH2I("hCrosing", "hCrosing", 48, 0, 48, 48, 0, 48);
    fList->Add(hCrosing);
    TH1I *hCrosingDx = new TH1I("hCrosingDx", "hCrosingDx", 48, 0., 60.);
    fList->Add(hCrosingDx);
    TH1I *hCrosingDy = new TH1I("hCrosingDy", "hCrosingDy", 120, -30., 30.);
    fList->Add(hCrosingDy);
    TH1I *hCrosingDr = new TH1I("hCrosingDr", "hCrosingDr", 600, 0., 60.);
    fList->Add(hCrosingDr);
    TH1I *hCrosingDt = new TH1I("hCrosingDt", "hCrosingDt", 1024, -12., 12.);
    fList->Add(hCrosingDt);
    TH2I *hCrosingWidth_Width = new TH2I("hCrosingWidth_Width", "hCrosingWidth_Width", 1024, 12, 60, 1024, 12, 60);
    fList->Add(hCrosingWidth_Width);
    TH1I *hCrosingDwidth = new TH1I("hCrosingDwidth", "hCrosingDwidth", 512, -12., 12.);
    fList->Add(hCrosingDwidth);

    TH1I *hCrosingDtSt = new TH1I("hCrosingDtSt", "hCrosingDtSt", 1024, -12., 12.);
    fList->Add(hCrosingDtSt);
    TH2I *hCrosingDtSt_Width1 = new TH2I("hCrosingDtSt_Width1", "hCrosingDtSt_Width1", 1024, 12, 60, 1024, -12, 12);
    fList->Add(hCrosingDtSt_Width1);
    TH2I *hCrosingDtSt_Width2 = new TH2I("hCrosingDtSt_Width2", "hCrosingDtSt_Width2", 1024, 12, 60, 1024, -12, 12);
    fList->Add(hCrosingDtSt_Width2);


    TH2I *h_HitRPC = new TH2I("h_HitRPC", "h_HitRPC", 192, -96, 96, 5, 0, 5);
    fListTof->Add(h_HitRPC);

    TH2I *h_XYRPC = new TH2I("h_XYRPC", "h_XYRPC", 320, -200, 200, 120, -75, 75);
    fListTof->Add(h_XYRPC);

    TH2I *h_Time_vs_Angle = new TH2I("h_Time_vs_Angle", "h_Time_vs_Angle", 320, -200, 200, 1024, -6., 18.);
    fListTof->Add(h_Time_vs_Angle);

    TH1I *hdtPlane7 = new TH1I("hdtPlane7", "hdtPlane7", 1024, -5., 43.);
    fList->Add(hdtPlane7);

    TH2I *hdtPlane7_vs_AmpT0 = new TH2I("hdtPlane7_vs_AmpT0", "hdtPlane7_vs_AmpT0", 512, 0., 50., 512, -5., 43.);
    fList->Add(hdtPlane7_vs_AmpT0);

    TH2I *hdtPlane7_vs_AmpPlane7 = new TH2I("hdtPlane7_vs_AmpPlane7", "hdtPlane7_vs_AmpPlane7", 512, 0., 50., 512, -5., 43.);
    fList->Add(hdtPlane7_vs_AmpPlane7);

    TH1I *hdtPlane12 = new TH1I("hdtPlane12", "hdtPlane12", 1024, -5., 43.);
    fList->Add(hdtPlane12);

    TH2I *hdtPlane12_vs_AmpT0 = new TH2I("hdtPlane12_vs_AmpT0", "hdtPlane12_vs_AmpT0", 512, 0., 50., 512, -5., 43.);
    fList->Add(hdtPlane12_vs_AmpT0);

    TH2I *hdtPlane12_vs_AmpPlane12 = new TH2I("hdtPlane12_vs_AmpPlane12", "hdtPlane12_vs_AmpPlane12", 512, 0., 50., 512, -5., 43.);
    fList->Add(hdtPlane12_vs_AmpPlane12);

    TChain *eveTree = new TChain("cbmsim");
    TString inName = Form("%s", file.Data());
    //inName = file;
    cout << "Open file " << inName << endl << endl;
    eveTree->Add(inName);

    TClonesArray *ToF400Digits;
    eveTree->SetBranchAddress("TOF400", &ToF400Digits);

    TClonesArray *T0Digits;
    eveTree->SetBranchAddress("BC2", &T0Digits);

    TClonesArray *VetoDigits;
    eveTree->SetBranchAddress("VC", &VetoDigits);

    TClonesArray *BDDigits;
    eveTree->SetBranchAddress("BD", &BDDigits);

    TClonesArray *SiDigits;
    eveTree->SetBranchAddress("Si", &SiDigits);

    Long64_t nEvents = eveTree->GetEntries();
    if (nEvForRead == 0 || nEvForRead > nEvents) nEvForRead = nEvents;
    cout << "Will be read " << nEvForRead << " events from " << nEvents << endl;

    for (Int_t iEv = 0; iEv < nEvForRead; iEv++) {

        if (iEv % 1000 == 0) cout << "EVENT: " << iEv << endl;
        eveTree->GetEntry(iEv);

        Int_t CountMod0 = 0, CountMod1 = 0;
        for (Int_t i = 0; i < T0Digits->GetEntriesFast(); i++) {
            BmnTrigDigit* digT0 = (BmnTrigDigit*) T0Digits->At(i);
            if (digT0->GetMod() == 0) CountMod0++;
            if (digT0->GetMod() == 1) CountMod1++;
        }

        if (
                CountMod0 == 1 && CountMod1 <= 0
                //(T0Digits->GetEntriesFast()) >= 0
                //&& (BC2Digits->GetEntriesFast()) >= 0
                //&& (VetoDigits->GetEntriesFast()) == 0
                //&& (BDDigits->GetEntriesFast()) >= 0
                //&& (BDDigits->GetEntriesFast()) == 0
                ) //*/
        {

            BmnTrigDigit* digT0 = NULL;
            for (Int_t i = 0; i < T0Digits->GetEntriesFast(); i++) {
                digT0 = (BmnTrigDigit*) T0Digits->At(i);
                if (digT0->GetMod() == 0) break;
            }
            if (digT0->GetAmp() >= 19.26 && digT0->GetAmp() <= 22.06) {

                //--------------------------- RPC --------------------------------------------------
                for (Int_t i = 0; i < NDet; i++)
                    Plane[i]->Clear();

                //cout << "ToF400Digits->GetEntriesFast()=" << ToF400Digits->GetEntriesFast() << endl;
                for (Int_t iDig = 0; iDig < ToF400Digits->GetEntriesFast(); ++iDig) {
                    FlagHit = kFALSE;
                    BmnTof1Digit* digTof = (BmnTof1Digit*) ToF400Digits->At(iDig);
                    FlagHit = Plane[digTof->GetPlane()]->SetDigit(digTof);
                    //cout << "SetDigit Plane#" << digTof->GetPlane() << " Strip#" <<  digTof->GetStrip() << " = " << FlagHit << endl;
                    if (FlagHit == kTRUE) {
                        if (digTof->GetPlane() < 5) {
                            XforHit = digTof->GetStrip() * -1. - 48;
                            YforHit = 4 - digTof->GetPlane();
                        } else if (digTof->GetPlane() >= 5 && digTof->GetPlane() < 10) {
                            XforHit = digTof->GetStrip() * -1.;
                            YforHit = 4 - (digTof->GetPlane() - 5);
                        } else if (digTof->GetPlane() >= 10 && digTof->GetPlane() < 15) {
                            XforHit = 48 - digTof->GetStrip();
                            YforHit = 4 - (digTof->GetPlane() - 10);
                        } else if (digTof->GetPlane() >= 15 && digTof->GetPlane() < 20) {
                            XforHit = 96 - (digTof->GetStrip() + 48);
                            YforHit = 4 - (digTof->GetPlane() - 15);
                        }
                        //cout << "PlaneR = " << digTof->GetPlane() << "; StripR = " << digTof->GetStrip() << endl;
                        //cout << "Plane = " << YforHit << "; Strip = " << XforHit << endl;
                        //getchar();
                        h_HitRPC->Fill(XforHit, YforHit);
                    }
                }
                //getchar();

                TVector3 XYZ;
                XYZ.SetXYZ(0., 0., 0.);
                Double_t ToF = 0;
                FlagHit = kFALSE;
                for (Int_t i = 0; i < NDet; i++) {
                    Plane[i] -> FindHits(digT0);
                    for (Int_t s = 0; s < 47; s++) {
                        XYZ.SetXYZ(0., 0., 0.);
                        ToF = 0;
                        FlagHit = kFALSE;
                        FlagHit = Plane[i]->GetXYZTime(s, &XYZ, &ToF);
                        if (FlagHit == kTRUE) {
                            h_XYRPC -> Fill(XYZ.x(), XYZ.y());
                            if (i == 7) h_Time_vs_Angle -> Fill(XYZ.x(), ToF);
                        }
                        if (ToF != 0 && i == 7 && s == 3) {
                            //    cout << " Ev = " << iEv << "; Strip = " << s << "; ToF = " <<ToF << "; Amp = " << Plane[i]->GetWidth(s) << endl;
                            hdtPlane7->Fill(ToF);
                            hdtPlane7_vs_AmpT0->Fill(digT0->GetAmp(), ToF);
                            hdtPlane7_vs_AmpPlane7->Fill(Plane[i]->GetWidth(s), ToF);
                        }
                        if (ToF != 0 && i == 12 && s == 45) {
                            //    cout << " Ev = " << iEv << "; Strip = " << s << "; ToF = " <<ToF << "; Amp = " << Plane[i]->GetWidth(s) << endl;
                            hdtPlane12->Fill(ToF);
                            hdtPlane12_vs_AmpT0->Fill(digT0->GetAmp(), ToF);
                            hdtPlane12_vs_AmpPlane12->Fill(Plane[i]->GetWidth(s), ToF);
                        }
                    }
                }
                CrossingDet(Plane[2], Plane[7], hCrosing, hCrosingDx, hCrosingDy, hCrosingDr, hCrosingDt, hCrosingDtSt, hCrosingDtSt_Width1, hCrosingDtSt_Width2);


            }// end             if (digT0->GetAmp() >= 17.3 && digT0->GetAmp() <= 19.2)
        } // end if ((T0Digits->GetEntriesFast()) == 1 && (VetoDigits->GetEntriesFast()) == 0 && (BDDigits->GetEntriesFast()) >= 2)
    } // end for (Int_t iEv = 0; iEv < nEvForRead; iEv++)

    cout << "End Data Analysis" << endl;

    TString outName = file;
    Int_t Point = outName.First('.');
    outName.Replace(Point, 15, "_TofAn7.root");
    outName = Form("%s", outName.Data());
    cout << "Save data to " << outName.Data() << endl;
    TFile *fileout = new TFile(outName.Data(), "RECREATE");

    TDirectory *Dir;
    Dir = fileout->mkdir("ToF");
    Dir->cd();
    fListTof->Write();

    TDirectory * DirPlane[NDet];
    TDirectory * Dir1Plane[NDet];
    TDirectory * Dir2Plane[NDet];
    for (Int_t i = 0; i < NDet; i++) {
        DirPlane[i] = Dir->mkdir(Plane[i]->GetName());
        DirPlane[i] -> cd();
        Plane[i]->GetList(0)->Write();
        Dir1Plane[i] = DirPlane[i]->mkdir("Detector");
        Dir1Plane[i] -> cd();
        Plane[i]->GetList(1)->Write();
        Dir2Plane[i] = DirPlane[i]->mkdir("ToF");
        Dir2Plane[i] -> cd();
        Plane[i]->GetList(2)->Write();
    }

    fileout->cd();
    fList->Write();
    fileout->Close();

}

Double_t CalculateDtT0BD(BmnTrigDigit *bd, BmnTrigDigit * t0) {
    //    Double_t dt = bd->GetTime() - t0->GetTime();
    /*   dt = dt - (14.55 + 0.1218 * bd->GetAmp()
               - 0.02473 * bd->GetAmp() * bd->GetAmp()
               + 0.0006237 * bd->GetAmp() * bd->GetAmp() * bd->GetAmp());
       dt = dt - (1.947 - 0.5363 * t0->GetAmp()
               + 0.03428 * t0->GetAmp() * t0->GetAmp()
               - 0.0005853 * t0->GetAmp() * t0->GetAmp() * t0->GetAmp());
       /*  dt = dt - (-3.412 + 0.1866 * t0->GetAmp());//*/
    //  return dt;
}

Double_t CalculateDtT0BD(Double_t timeBD, Double_t ampBD, BmnTrigDigit * t0) {
    //    Double_t dt = timeBD - t0->GetTime();
    /*   dt = dt - (14.55 + 0.1218 * ampBD
               - 0.02473 * ampBD * ampBD
               + 0.0006237 * ampBD * ampBD * ampBD);
       dt = dt - (1.947 - 0.5363 * t0->GetAmp()
               + 0.03428 * t0->GetAmp() * t0->GetAmp()
               - 0.0005853 * t0->GetAmp() * t0->GetAmp() * t0->GetAmp());
       //dt = dt - (-3.412 + 0.1866 * t0->GetAmp());//*/
    //  return dt;
}

Double_t CalculateDtT0BC2(BmnTrigDigit *bc2, BmnTrigDigit * t0) {
    //   Double_t dt = bc2->GetTime() - t0->GetTime();
    /*   dt = dt - (40.47
               - 6.338 * bc2->GetAmp()
               + 0.35 * bc2->GetAmp() * bc2->GetAmp()
               - 0.006571 * bc2->GetAmp() * bc2->GetAmp() * bc2->GetAmp());
       dt = dt - (1.947 - 0.5363 * t0->GetAmp()
               + 0.03428 * t0->GetAmp() * t0->GetAmp()
               - 0.0005853 * t0->GetAmp() * t0->GetAmp() * t0->GetAmp());
       /*    dt = dt - (0.4034 - 0.01599 * t0->GetAmp());
           dt = dt + 0.001108; //zero for C 3.5 Gec/nuclon */
    //   return dt;
}

void CrossingDet(BmnTOF1Detector *Det1, BmnTOF1Detector *Det2, TH2 *hD1D2, TH1 *hDx, TH1 *hDy, TH1 *hDr, TH1 *hDt, TH1 *hDtSt, TH2 *hDtSt_W1, TH2 *hDtSt_W2) {
    TVector3 XyzD1, XyzD2;
    Double_t ToF1 = 0, ToF2 = 0, dT = 0.;
    for (Int_t iD1 = 0; iD1 < 12; iD1++) {
        XyzD1.SetXYZ(0., 0., 0.);
        ToF1 = 0;
        if (Det1->GetXYZTime(iD1, &XyzD1, &ToF1))
            for (Int_t iD2 = 36; iD2 < 48; iD2++) {
                XyzD2.SetXYZ(0., 0., 0.);
                ToF2 = 0.;
                if (Det2->GetXYZTime(iD2, &XyzD2, &ToF2)) {
                    hD1D2->Fill(iD1, iD2);
                    Double_t dX = XyzD1.x() - XyzD2.x();
                    Double_t dY = XyzD1.y() - XyzD2.y();
                    Double_t dR = TMath::Sqrt(dX * dX + dY * dY);
                    hDx->Fill(dX);
                    hDy->Fill(dY);
                    hDr->Fill(dR);
                    hDt->Fill(ToF1 - ToF2);
                    hCrosingWidth_Width->Fill(Det1->GetWidth(iD1), Det2->GetWidth(iD2));
                    hCrosingDwidth->Fill(Det1->GetWidth(iD1) - Det2->GetWidth(iD2));
                    if (iD1 == 2 && iD2 == 42) {
                        hDtSt->Fill(ToF1 - ToF2);
                        hDtSt_W1->Fill(Det1->GetWidth(iD1), ToF1 - ToF2 );
                        hDtSt_W2->Fill(Det2->GetWidth(iD2), ToF1 - ToF2);
                    }
                    //     cout << 3 << endl;
                }
            }
    }
}