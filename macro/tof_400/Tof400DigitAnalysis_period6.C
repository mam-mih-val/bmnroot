/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include <stdio.h>
#include <fstream>
#include <istream>
#include <iostream>
#include <ctime>

#include <TMath.h>
#include "TChain.h"
#include "TH1.h"
#include "TH1F.h"
#include "TH1S.h"
#include "TH2S.h"
#include "TH2F.h"
#include "TCanvas.h"
#include "TFile.h"
#include "TList.h"
#include "TDirectory.h"
#include "TPad.h"
#include "TClonesArray.h"

Int_t CorrPlane7_It1, CorrT0_It1;
Double_t AmpBD, TimeBD, NHitBD;
Double_t CorrPlane7Coeff_It1[5][4] = {
    {12.24, -3.919, 0.2624, -0.005153},
    {-122.9, 14.84, -0.6158, 0.008724},
    {-12.34, 0.482, 0., 0.},
    {-12.28, 0.479, 0., 0.},
    {-13.14, 0.5034, 0., 0.}
};
Double_t CorrT0Coeff_It1[4][4] = {
    {-5.486, 1.087, -0.07936, 0.00208},
    {10.77, -2.635, 0.1984, -0.004631},
    {-2.163, 0.1362, 0., 0.},
    {200, 0, 0, 0}
};

Double_t CorrLR[10][48] = {0.};

Bool_t FlagHit = kFALSE;
Int_t XforHit = 0, YforHit = 0;



class BmnTrigDigit;

void
Tof400DigitAnalysis_period6(TString file = "", Int_t nEvForRead = 0, Int_t Periud = 5) {
    TStopwatch timer;
    timer.Start();

    gROOT->LoadMacro("$VMCWORKDIR/macro/run/bmnloadlibs.C");
    bmnloadlibs(); // load BmnRoot libraries

    ifstream f_call;
    char line[256];
    Int_t Pl, St;
    Double_t Temp;
    TString NameCallFile = "Tof400LRcorr.dat";
    f_call.open(NameCallFile.Data());
    f_call.getline(line, 256);
    f_call.getline(line, 256);
    while (!f_call.eof()) {
        f_call >> Pl >> St >> Temp >> CorrLR[Pl][St];
        if (TMath::Abs(Temp - CorrLR[Pl][St]) > 2.) CorrLR[Pl][St] = -11.9766;
    }

    BmnTOF1Detector * Plane[10];

    TString name;
    for (Int_t i = 0; i < 10; i++) {
        name = Form("Plane%d", i);
        Plane[i] = new BmnTOF1Detector(i, kTRUE);
        //Plane[i]->SetCorrLR(CorrLR[i]);
        Plane[i]->SetCorrLR("Tof400LRcorr.dat");
        Plane[i]->SetCorrSlewing("Tof400SlewingCorr_period6.root");
    }

    TList *fList = new TList();
    TList *fListTof = new TList();
    TH1I *h_dtBDT0 = new TH1I("h_dtBDT0", "h_dtBDT0", 1024, -24, 24);
    fList->Add(h_dtBDT0);
    TH2F *h_dtBDT0_vs_AmpT0 = new TH2F("h_dtBDT0_vs_AmpT0", "h_dtBDT0_vs_AmpT0", 1024, 0, 48, 1024, -24, 24);
    fList->Add(h_dtBDT0_vs_AmpT0);
    TH2F *h_dtBDT0_vs_AmpBD = new TH2F("h_dtBDT0_vs_AmpBD", "h_dtBDT0_vs_AmpBD", 1024, 0, 48, 1024, -24, 24);
    fList->Add(h_dtBDT0_vs_AmpBD);
    TH2F *h_TimeBD_vs_TimeT0 = new TH2F("h_TimeBD_vs_TimeT0", "h_TimeBD_vs_TimeT0", 680, 250, 420, 680, 250, 420);
    fList->Add(h_TimeBD_vs_TimeT0);
    TH1I *h_dtBDT0_Mod = new TH1I("h_dtBDT0_Mod", "h_dtBDT0_Mod", 1024, -24, 24);
    fList->Add(h_dtBDT0_Mod);
    TH1I *h_NHitBD = new TH1I("h_NHitBD", "h_NHitBD", 40, 0, 40);
    fList->Add(h_NHitBD);

    TH1I *h_dtBC2T0 = new TH1I("h_dtBC2T0", "h_dtBC2T0", 1024, -6., 6.);
    fList->Add(h_dtBC2T0);
    TH2F *h_dtBC2T0_vs_AmpT0 = new TH2F("h_dtBC2T0_vs_AmpT0", "h_dtBC2T0_vs_AmpT0", 1024, 0, 48, 1024, -12., 12.);
    fList->Add(h_dtBC2T0_vs_AmpT0);
    TH2F *h_dtBC2T0_vs_AmpBC2 = new TH2F("h_dtBC2T0_vs_AmpBC2", "h_dtBC2T0_vs_AmpBC2", 1024, 0, 48, 1024, -12., 12.);
    fList->Add(h_dtBC2T0_vs_AmpBC2);
    TH2F *h_TimeBC2_vs_TimeT0 = new TH2F("h_TimeBC2_vs_TimeT0", "h_TimeBC2_vs_TimeT0", 680, 250, 420, 680, 250, 420);
    fList->Add(h_TimeBC2_vs_TimeT0);
    TH1I *h_dtBC2T0_Corr = new TH1I("h_dtBC2T0_Corr", "h_dtBC2T0_Corr", 1024, -6., 6.);
    fList->Add(h_dtBC2T0_Corr);

    TH2F *h_HitRPC = new TH2F("h_HitRPC", "h_HitRPC", 96, -96, 0, 5, 0, 5);
    fListTof->Add(h_HitRPC);

    TChain *eveTree = new TChain("cbmsim");
    TString inName = Form("/home/storage/digi/%s", file.Data());
    //inName = file;
    cout << "Open file " << inName << endl << endl;
    eveTree->Add(inName);

    TClonesArray *ToF400Digits;
    eveTree->SetBranchAddress("TOF400", &ToF400Digits);

    TClonesArray *T0Digits;
    eveTree->SetBranchAddress("T0", &T0Digits);

    TClonesArray *VetoDigits;
    eveTree->SetBranchAddress("VETO", &VetoDigits);

    TClonesArray *BDDigits;
    eveTree->SetBranchAddress("BD", &BDDigits);

    TClonesArray *BC2Digits;
    eveTree->SetBranchAddress("BC2", &BC2Digits);

    TClonesArray *EventHeader;
    eveTree->SetBranchAddress("EventHeader", &EventHeader);

    Long64_t nEvents = eveTree->GetEntries();
    if (nEvForRead == 0) nEvForRead = nEvents;
    cout << "Will be readed " << nEvForRead << " events from " << nEvents << endl;

    for (Int_t iEv = 0; iEv < nEvForRead; iEv++) {

        if (iEv % 50000 == 0) cout << "EVENT: " << iEv << endl;
        eveTree->GetEntry(iEv);
        if ((T0Digits->GetEntriesFast()) == 1
                && (BC2Digits->GetEntriesFast()) == 1
                && (VetoDigits->GetEntriesFast()) == 0
                && (BDDigits->GetEntriesFast()) >= 2
                //&& (BDDigits->GetEntriesFast()) == 0
                ) {
            BmnTrigDigit* digT0 = (BmnTrigDigit*) T0Digits->At(0);
            if (digT0->GetAmp() >= 17.3 && digT0->GetAmp() <= 19.2) {
                AmpBD = 0.;
                TimeBD = 0.;
                NHitBD = 0.;

                BmnTrigDigit* digT0 = (BmnTrigDigit*) T0Digits->At(0);
                //--------------------------- RPC --------------------------------------------------
                for (Int_t i = 0; i < 10; i++)
                    Plane[i]->Clear();

                for (Int_t iDig = 0; iDig < ToF400Digits->GetEntriesFast(); ++iDig) {
                    FlagHit = kFALSE;
                    BmnTof1Digit* digTof = (BmnTof1Digit*) ToF400Digits->At(iDig);
                    FlagHit = Plane[digTof->GetPlane()]->SetDigit(digTof);
                    if (FlagHit == kTRUE) {
                        if (digTof->GetPlane() < 5) {
                            XforHit = digTof->GetStrip() * -1. - 48;
                            YforHit = 4 - digTof->GetPlane();
                        } else {
                            XforHit = digTof->GetStrip() * -1.;
                            YforHit = 4 - (digTof->GetPlane() - 5);
                        }
                        h_HitRPC->Fill(XforHit, YforHit);
                    }
                }

                for (Int_t i = 0; i < 10; i++)
                    Plane[i] -> FindHits(digT0);

                //--------------------------- BD --------------------------------------------------
                for (Int_t iDig = 0; iDig < BDDigits->GetEntriesFast(); ++iDig) {
                    BmnTrigDigit* digBD = (BmnTrigDigit*) BDDigits->At(iDig);
                    h_TimeBD_vs_TimeT0 -> Fill(digT0->GetTime(), digBD->GetTime());
                    if (TMath::Abs(digBD->GetTime() - digT0->GetTime() - 13.) < 1.8) {
                        AmpBD = AmpBD + digBD->GetAmp();
                        TimeBD = TimeBD + digBD->GetTime();
                        NHitBD++;
                    }
                    h_dtBDT0_Mod->Fill(CalculateDtT0BD(digBD, digT0));
                }
                h_NHitBD->Fill(NHitBD);
                AmpBD = AmpBD / NHitBD;
                TimeBD = TimeBD / NHitBD;

                Double_t dtBDT0 = CalculateDtT0BD(TimeBD, AmpBD, digT0);
                h_dtBDT0 -> Fill(dtBDT0);
                h_dtBDT0_vs_AmpT0 -> Fill(digT0->GetAmp(), dtBDT0);
                h_dtBDT0_vs_AmpBD -> Fill(AmpBD, dtBDT0);

                //--------------------------- BC2 --------------------------------------------------
                BmnTrigDigit* digBC2 = (BmnTrigDigit*) BC2Digits->At(0);
                if (digBC2->GetAmp() >= 16.2 && digBC2->GetAmp() <= 19.3) {
                    Double_t dtBC2T0 = CalculateDtT0BC2(digBC2, digT0);
                    h_dtBC2T0 -> Fill(digBC2->GetTime() - digT0->GetTime());
                    h_dtBC2T0_Corr->Fill(dtBC2T0);
                    h_dtBC2T0_vs_AmpT0 -> Fill(digT0->GetAmp(), dtBC2T0);
                    h_dtBC2T0_vs_AmpBC2 -> Fill(digBC2->GetAmp(), dtBC2T0);
                    h_TimeBC2_vs_TimeT0->Fill(digBC2->GetTime(), digT0->GetTime());
                }//*/


                //--------------------------- EventHeader --------------------------------------------------
                Int_t iEvDig = EventHeader->GetEntriesFast();
                if (iEvDig != 1) cout << "iEvDig == " << iEvDig << endl;
                BmnEventHeader* digEvent = (BmnEventHeader*) EventHeader->At(0);

            }// end             if (digT0->GetAmp() >= 17.3 && digT0->GetAmp() <= 19.2)
        } // end if ((T0Digits->GetEntriesFast()) == 1 && (VetoDigits->GetEntriesFast()) == 0 && (BDDigits->GetEntriesFast()) >= 2)
    } // end for (Int_t iEv = 0; iEv < nEvForRead; iEv++)

    cout << "End Data Analysis" << endl;

    timer.Stop();
    cout << "Readed " << nEvForRead << " events" << endl;
    cout << "Time          = " << timer.RealTime() << " s" << endl;
    cout << "Time/Event    = " << timer.RealTime() / (Double_t) nEvForRead * 1000. << " ms/Event" << endl;
    cout << "TimeCPU       = " << timer.CpuTime() << " s" << endl;
    cout << "TimeCPU/Event = " << timer.CpuTime() / (Double_t) nEvForRead * 1000. << " ms/Event" << endl;

    ofstream f_time;
    TString NameTimeFile = "Tof400An_Time.dat";
    //TString NameCallFile = file;
    //Point = NameCallFile.First('.');
    //NameCallFile.Replace(Point, 15, "_LRcorr.dat");
    cout << "Write time to " << NameTimeFile.Data() << endl;
    f_time.open(NameTimeFile.Data(), std::ofstream::out | std::ofstream::app);
    f_time << file << "\t" << timer.RealTime() << "\t" << timer.RealTime() / (Double_t) nEvForRead * 1000. << "\t" << timer.CpuTime() << "\t" << timer.CpuTime() / (Double_t) nEvForRead * 1000. << endl;
    f_time.close();

    timer.Reset();
    timer.Start();
    TString outName = file;
    Int_t Point = outName.First('.');
    outName.Replace(Point, 15, "_TofAn6.root");
    outName = Form("/home/storage/analysis/%s", outName.Data());
    cout << "Save data to " << outName.Data() << endl;
    TFile *fileout = new TFile(outName.Data(), "RECREATE");

    TDirectory *Dir;
    Dir = fileout->mkdir("ToF");
    Dir->cd();
    fListTof->Write();

    TDirectory * DirPlane[10];
    TDirectory * Dir1Plane[10];
    TDirectory * Dir2Plane[10];
    for (Int_t i = 0; i < 10; i++) {
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

    timer.Stop();

    cout << "Time for write root = " << timer.RealTime() << " s" << endl;

}//end of macros

//----Additional functions ----------------------------------------------------------------

Double_t CalculateDtT0BD(BmnTrigDigit *bd, BmnTrigDigit * t0) {
    Double_t dt = bd->GetTime() - t0->GetTime();
    dt = dt - (14.55 + 0.1218 * bd->GetAmp()
            - 0.02473 * bd->GetAmp() * bd->GetAmp()
            + 0.0006237 * bd->GetAmp() * bd->GetAmp() * bd->GetAmp());
    dt = dt - (1.947 - 0.5363 * t0->GetAmp()
            + 0.03428 * t0->GetAmp() * t0->GetAmp()
            - 0.0005853 * t0->GetAmp() * t0->GetAmp() * t0->GetAmp());
    /*  dt = dt - (-3.412 + 0.1866 * t0->GetAmp());//*/
    return dt;
}

Double_t CalculateDtT0BD(Double_t timeBD, Double_t ampBD, BmnTrigDigit * t0) {
    Double_t dt = timeBD - t0->GetTime();
    dt = dt - (14.55 + 0.1218 * ampBD
            - 0.02473 * ampBD * ampBD
            + 0.0006237 * ampBD * ampBD * ampBD);
    dt = dt - (1.947 - 0.5363 * t0->GetAmp()
            + 0.03428 * t0->GetAmp() * t0->GetAmp()
            - 0.0005853 * t0->GetAmp() * t0->GetAmp() * t0->GetAmp());
    //dt = dt - (-3.412 + 0.1866 * t0->GetAmp());//*/
    return dt;
}

Double_t CalculateDtT0BC2(BmnTrigDigit *bc2, BmnTrigDigit * t0) {
    Double_t dt = bc2->GetTime() - t0->GetTime();
    dt = dt - (40.47
            - 6.338 * bc2->GetAmp()
            + 0.35 * bc2->GetAmp() * bc2->GetAmp()
            - 0.006571 * bc2->GetAmp() * bc2->GetAmp() * bc2->GetAmp());
    dt = dt - (1.947 - 0.5363 * t0->GetAmp()
            + 0.03428 * t0->GetAmp() * t0->GetAmp()
            - 0.0005853 * t0->GetAmp() * t0->GetAmp() * t0->GetAmp());
    /*    dt = dt - (0.4034 - 0.01599 * t0->GetAmp());
        dt = dt + 0.001108; //zero for C 3.5 Gec/nuclon */
    return dt;
}

/*
Double_t CalculateDt(ToF400Detector * Det, T0Detector * T0, Int_t Str = -1)
{

    if (Str == -1) Str = TMath::LocMax(kNST, Plane7->sAmp);
    Double_t dt = Det->sTime[Str] - T0->sTime;
    //cout << " dt = " << dt << endl;
      if (Det->sAmp[Str] < 20.46) CorrPlane7_It1 = 0;
      else if (Det->sAmp[Str] >= 20.46 && Det->sAmp[Str] < 27.25) CorrPlane7_It1 = 1;
      else if (Det->sAmp[Str] >= 27.25 && Det->sAmp[Str] < 34.81) CorrPlane7_It1 = 2;
      else if (Det->sAmp[Str] >= 34.81 && Det->sAmp[Str] < 37.11) CorrPlane7_It1 = 3;
      else if (Det->sAmp[Str] >= 37.11) CorrPlane7_It1 = 4;
      dt = dt - (CorrPlane7Coeff_It1[CorrPlane7_It1][0] + CorrPlane7Coeff_It1[CorrPlane7_It1][1] * Det->sAmp[Str] +
              CorrPlane7Coeff_It1[CorrPlane7_It1][2] * Det->sAmp[Str] * Det->sAmp[Str] +
              CorrPlane7Coeff_It1[CorrPlane7_It1][3] * Det->sAmp[Str] * Det->sAmp[Str] * Det->sAmp[Str]); //

      if (T0->sAmp >= 6.15 && T0->sAmp < 12.3) CorrT0_It1 = 0; //deutron
      else if (T0->sAmp >= 12.3 && T0->sAmp < 17.14) CorrT0_It1 = 1; //deutron
      else if (T0->sAmp >= 17.9 && T0->sAmp < 19.63) CorrT0_It1 = 2; //C6+
      else CorrT0_It1 = 3; //skip
      dt = dt - (CorrT0Coeff_It1[CorrT0_It1][0] + CorrT0Coeff_It1[CorrT0_It1][1] * T0->sAmp +
              CorrT0Coeff_It1[CorrT0_It1][2] * T0->sAmp * T0->sAmp +
              CorrT0Coeff_It1[CorrT0_It1][3] * T0->sAmp * T0->sAmp * T0->sAmp); //
    return dt;
}//*/