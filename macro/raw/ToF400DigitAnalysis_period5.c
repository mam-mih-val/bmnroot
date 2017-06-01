/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include <stdio.h>
#include <fstream>
#include <iostream>

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
//#include "BmnEventHeader.h"

const UInt_t kNST = 48;
const Double_t kTimeBin = 24. / 1024.;
Double_t CatTimeBC2Low, CatTimeBC2High;
Int_t CorrPlane7_It1, CorrT0_It1;
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
Int_t HitPlane2T0 = 0, HitPlane7T0 = 0, EffPlane2 = 0, EffPlane7 = 0;
Bool_t FlagDigit[10][kNST];

struct ToF400Detector {
    Double_t sTimeL[kNST];
    Double_t sTimeR[kNST];
    Double_t sTime[kNST];
    Double_t sAmpL[kNST];
    Double_t sAmpR[kNST];
    Double_t sAmp[kNST];
    Bool_t sStrHit[kNST];
    Int_t sNHits;
};

struct T0Detector {
    Double_t sTime;
    Double_t sAmp;
    Int_t sNHits;
};

void
ToF400DigitAnalysis_period5(TString file = "", Int_t nEvForRead = 0, Int_t Periud = 5) {
    gROOT->LoadMacro("$VMCWORKDIR/macro/run/bmnloadlibs.C");
    bmnloadlibs(); // load BmnRoot libraries

    /*if (Run < 796) {
        CatTimeBC2Low = 250.;
        CatTimeBC2High = 450.;
    } else {
        CatTimeBC2Low = 390.;
        CatTimeBC2High = 590.;
    }//*/

    TChain *eveTree = new TChain("cbmsim");
    TString inName;
    inName = file;
    cout << "Open file " << inName << endl << endl;
    eveTree->Add(inName);

    TClonesArray *ToF400Digits;
    eveTree->SetBranchAddress("TOF400", &ToF400Digits);

    TClonesArray *BC2Digits;
    eveTree->SetBranchAddress("T0", &BC2Digits);

    TClonesArray *EventHeader;
    eveTree->SetBranchAddress("EventHeader", &EventHeader);

    Long64_t nEvents = eveTree->GetEntries();
    if (nEvForRead == 0) nEvForRead = nEvents;
    cout << "Will be readed " << nEvForRead << " events from " << nEvents << endl;

    //------variables----------------------------------------------------------------------
    char hid[50];
    Int_t Point;
    ofstream f_call, f_dt;
    TF1 *f_gaus = new TF1("f_gaus", "gaus", -20, 40);

    ToF400Detector Plane2;
    ToF400Detector Plane7;
    T0Detector T0;
    CleanTof(&Plane2);
    CleanTof(&Plane7);
    CleanBC2(&T0);

    TList * ListStat = new TList();
    TList * ListAmpTofPlane2 = new TList();
    TList * ListTimeTofPlane2 = new TList();
    TList * ListLRTofPlane2 = new TList();
    TList * ListAmpTofPlane7 = new TList();
    TList * ListTimeTofPlane7 = new TList();
    TList * ListLRTofPlane7 = new TList();
    TList * ListLRCorrTofPlane7 = new TList();
    TList * ListLRvsAmpLTofPlane7 = new TList();
    TList * ListLRvsAmpRTofPlane7 = new TList();
    TList * ListAnalysis = new TList();
    TString name, name2;

    TH1I * hAmpTofPlane2[kNST + 1];
    TH1I * hTimeTofPlane2[kNST + 1];
    TH1I * hLevtMinusRightPlane2[kNST + 1];
    for (Int_t i = 0; i < kNST + 1; i++) {
        name = Form("hAmpTofPlane2_%d", i);
        hAmpTofPlane2[i] = new TH1I(name, name, 1024, 0, 50.);
        ListAmpTofPlane2->Add(hAmpTofPlane2[i]);
        name = Form("hTimeTofPlane2_%d", i);
        hTimeTofPlane2[i] = new TH1I(name, name, 1000, 0, 1000.);
        ListTimeTofPlane2->Add(hTimeTofPlane2[i]);
        name = Form("hLevtMinusRightPlane2_%d", i);
        hLevtMinusRightPlane2[i] = new TH1I(name, name, 1024, -12., 12.);
        ListLRTofPlane2->Add(hLevtMinusRightPlane2[i]);
    }
    TH2I * hAmpLRPlane2 = new TH2I("hAmpLRPlane2", "hAmpLRPlane2", 1024, 0., 50., 1024, 0., 50.);
    ListAmpTofPlane2->Add(hAmpLRPlane2);

    TH1I * hAmpTofPlane7[kNST + 1];
    TH1I * hTimeTofPlane7[kNST + 1];
    TH1I * hLeftMinusRightPlane7[kNST + 1];
    TH2S * hLeftRightVsAmpLPlane7[kNST + 1];
    TH2S * hLeftRightVsAmpRPlane7[kNST + 1];
    TH2S * hLeftRightCorelationRPlane7[kNST + 1];
    for (Int_t i = 0; i < kNST + 1; i++) {
        name = Form("hAmpTofPlane7_%d", i);
        hAmpTofPlane7[i] = new TH1I(name, name, 1024, 0, 50.);
        ListAmpTofPlane7->Add(hAmpTofPlane7[i]);
        name = Form("hTimeTofPlane7_%d", i);
        hTimeTofPlane7[i] = new TH1I(name, name, 1000, 0, 1000.);
        ListTimeTofPlane7->Add(hTimeTofPlane7[i]);
        name = Form("hLeftRightCorelationRPlane7_%d", i);
        hLeftRightCorelationRPlane7[i] = new TH2S(name, name, 1024, 0., 50., 1024, 0., 50.);
        ListLRCorrTofPlane7->Add(hLeftRightCorelationRPlane7[i]);
        name = Form("hLeftMinusRightPlane7_%d", i);
        hLeftMinusRightPlane7[i] = new TH1I(name, name, 1024, -12., 12.);
        ListLRTofPlane7->Add(hLeftMinusRightPlane7[i]);
        name = Form("hLeftRightVsAmpLPlane7_%d", i);
        hLeftRightVsAmpLPlane7[i] = new TH2S(name, name, 1024, 0., 50., 1024, -12., 12.);
        ListLRvsAmpLTofPlane7->Add(hLeftRightVsAmpLPlane7[i]);
        name = Form("hLeftRightVsAmpRPlane7_%d", i);
        hLeftRightVsAmpRPlane7[i] = new TH2S(name, name, 1024, 0., 50., 1024, -12., 12.);
        ListLRvsAmpRTofPlane7->Add(hLeftRightVsAmpRPlane7[i]);
    }
    TH2I * hAmpLRPlane7 = new TH2I("hAmpLRPlane7", "hAmpLRPlane7", 1024, 0., 50., 1024, 0., 50.);
    ListAmpTofPlane7->Add(hAmpLRPlane7);

    TH1I *hHitStripsPerEvPlane2 = new TH1I("hHitStripsPerEvPlane2", "Hit Strips Per Ev on Plane#2", kNST, 0, kNST);
    ListStat->Add(hHitStripsPerEvPlane2);
    TH1I *hHitStripsPerEvPlane7 = new TH1I("hHitStripsPerEvPlane7", "Hit Strips Per Ev on Plane#7", kNST, 0, kNST);
    ListStat->Add(hHitStripsPerEvPlane7);
    TH2I *hLefRightCorelation_Plane2 = new TH2I("hLefRightCorelation_Plane2", " Lef-Right Corelation Plane#2", kNST, 0, kNST, kNST, 0, kNST);
    ListStat->Add(hLefRightCorelation_Plane2);
    TH2I *hLefRightCorelation_Plane7 = new TH2I("hLefRightCorelation_Plane7", " Lef-Right Corelation Plane#7", kNST, 0, kNST, kNST, 0, kNST);
    ListStat->Add(hLefRightCorelation_Plane7);
    TH2I *hPlane2Plane7Correlation = new TH2I("hPlane2Plane7Correlation", " Plane2-Plane7 Correlation", kNST, 0, kNST, kNST, 0, kNST);
    ListStat->Add(hPlane2Plane7Correlation);
    TH1S *hNoize = new TH1S("hNoize", "hNoize", 500, 0, 50);
    hNoize->GetXaxis()->SetTitle("MHz");
    ListStat->Add(hNoize);
    Int_t NEvStrip47 = 0;
    TH1I *hNEvStrip47 = new TH1I("hNEvStrip47", "hNEvStrip47", 20, 0, 20);
    ListStat->Add(hNEvStrip47);

    TList * ListT0 = new TList();
    TH1I *hHitT0PerEv = new TH1I("hHitT0PerEv", "Hit T0 Per Ev", 10, 0, 10);
    ListT0->Add(hHitT0PerEv);
    TH1I *hAmpT0 = new TH1I("hAmpT0", "Amp T0", 1024, 0, 50);
    ListT0->Add(hAmpT0);
    TH1I *hTimeT0 = new TH1I("hTimeT0", "Time T0", 1000, 0, 1000);
    ListT0->Add(hTimeT0);

    TH1I * hdt[kNST + 1];

    for (Int_t i = 0; i < kNST + 1; i++) {
        name = Form("hdt_%d", i);
        name2 = Form("dt T0 - ToF400_Plane7_str_%d", i);
        hdt[i] = new TH1I(name, name2, 1024 + 512, -18., 18.);
        hdt[i]->SetLineWidth(3);
        ListAnalysis->Add(hdt[i]);
    }

    TH2I * hdt_vs_AmpT0[kNST + 1];
    for (Int_t i = 0; i < kNST + 1; i++) {
        name = Form("hdt_vs_AmpT0_%d", i);
        name2 = Form("dt T0 - ToF400_Plane7_str_%d vs AMP_T0", i);
        hdt_vs_AmpT0[i] = new TH2I(name, name2, 1024, 0, 50, 1024 + 512, -24, 48);
        ListAnalysis->Add(hdt_vs_AmpT0[i]);
    }

    TH2I * hdt_vs_AmpToF[kNST + 1];
    for (Int_t i = 0; i < kNST + 1; i++) {
        name = Form("hdt_vs_AmpToF_%d", i);
        name2 = Form("dt T0 - ToF400_Plane7_str_%d vs AMP_TOF400", i);
        hdt_vs_AmpToF[i] = new TH2I(name, name2, 1024, 0, 50, 1024 + 512, -24, 48);
        ListAnalysis->Add(hdt_vs_AmpToF[i]);
    }

    TH2F *h_StripHit = new TH2F("h_StripHit", "h_StripHit", kNST, 0, kNST, 2, 0, 2);
    TH1I *h_StripHitSum = new TH1I("h_StripHitSum", "h_StripHitSum", kNST, 0, kNST);
    TH1F *h_StripAmp = new TH1F("h_StripAmp", "h_StripAmp", kNST, 0, kNST);
    h_StripAmp->GetYaxis()->SetLabelSize(0.07);
    h_StripAmp->SetLineWidth(2);
    h_StripAmp->SetLineColor(46);
    TH1F *h_StripTime = new TH1F("h_StripTime", "h_StripTime", kNST, 0, kNST);
    h_StripTime->GetYaxis()->SetLabelSize(0.07);
    h_StripTime->GetYaxis()->SetRangeUser(-1., 1.);
    h_StripTime->SetLineWidth(2);
    h_StripTime->SetLineColor(46);
    TH1F *h_StripAmpL = new TH1F("h_StripAmpL", "h_StripAmpL", kNST, 0, kNST);
    h_StripAmpL->GetYaxis()->SetLabelSize(0.07);
    h_StripAmpL->SetLineWidth(2);
    h_StripAmpL->SetLineColor(2);
    TH1F *h_StripTimeL = new TH1F("h_StripTimeL", "h_StripTimeL", kNST, 0, kNST);
    h_StripTimeL->GetYaxis()->SetLabelSize(0.07);
    h_StripTimeL->GetYaxis()->SetRangeUser(300, 450);
    h_StripTimeL->SetLineWidth(2);
    h_StripTimeL->SetLineColor(2);
    TH1F *h_StripAmpR = new TH1F("h_StripAmpR", "h_StripAmpR", kNST, 0, kNST);
    h_StripAmpR->GetYaxis()->SetLabelSize(0.07);
    h_StripAmpR->SetLineWidth(2);
    h_StripAmpR->SetLineColor(9);
    TH1F *h_StripTimeR = new TH1F("h_StripTimeR", "h_StripTimeR", kNST, 0, kNST);
    h_StripTimeR->GetYaxis()->SetLabelSize(0.07);
    h_StripTimeR->GetYaxis()->SetRangeUser(300, 450);
    h_StripTimeR->SetLineWidth(2);
    h_StripTimeR->SetLineColor(9);
    TH1F *h_StripLeftMinusRight = new TH1F("h_StripLeftMinusRight", "h_StripLeftMinusRight", kNST, 0, kNST);
    h_StripLeftMinusRight->GetYaxis()->SetLabelSize(0.07);
    h_StripLeftMinusRight->GetYaxis()->SetRangeUser(-2.5, 2.5);
    h_StripLeftMinusRight->SetLineWidth(2);
    h_StripLeftMinusRight->SetLineColor(9);
    TH2F *h_StripLeftMinusRight_vs_Time = new TH2F("h_StripLeftMinusRight_vs_Time", "h_StripLeftMinusRight_vs_Time", 512, -5., 5., 2048, 250., 450.);
    h_StripLeftMinusRight_vs_Time->GetYaxis()->SetLabelSize(0.07);
    h_StripLeftMinusRight_vs_Time->GetXaxis()->SetLabelSize(0.07);
    h_StripLeftMinusRight_vs_Time->SetMarkerStyle(8);

    TH2F *h_XYTime = new TH2F("h_XYTime", "h_XYTime", kNST, 0, kNST, 64, -16, 16.);
    h_XYTime->GetYaxis()->SetLabelSize(0.07);
    h_XYTime->GetXaxis()->SetLabelSize(0.07);
    //h_XYTime->SetMarkerStyle(8);

    //--------Data Analysis-----------------------------------------------------------------

    TCanvas *c_1 = new TCanvas("c1", "c1", 20, 20, 1500, 750);
    TPad *Pad_StripHit = new TPad("Pad_StripHit", "Pad_StripHit", 0.01, 0.51, 0.49, 0.99, 21);
    TPad *Pad_StripHitSum = new TPad("Pad_StripHitSum", "Pad_StripHitSum", 0.01, 0.01, 0.49, 0.49, 21);
    TPad *Pad_StripAmp = new TPad("Pad_StripAmp", "Pad_StripAmp", 0.51, 0.51, 0.99, 0.99, 21);
    TPad *Pad_StripTime = new TPad("Pad_StripTime", "Pad_StripTime", 0.51, 0.01, 0.99, 0.49, 21);

    Pad_StripHit->Draw();
    Pad_StripHitSum->Draw();
    Pad_StripAmp->Draw();
    Pad_StripAmp->SetGridy();
    Pad_StripTime->Draw();
    Pad_StripTime->SetGridy();

    TString NameDTFile = file;
    Point = NameDTFile.First('.');
    NameDTFile.Replace(Point, 15, "_dt.dat");
    cout << "Write call to " << NameDTFile.Data() << endl;
    f_dt.open(NameDTFile.Data());

    for (Int_t iEv = 0; iEv < nEvForRead; iEv++) {
        if (iEv % 50000 == 0) cout << "EVENT: " << iEv << endl;
        eveTree->GetEntry(iEv);
        CleanTof(&Plane2);
        CleanTof(&Plane7);
        CleanBC2(&T0);
        h_StripHit->Reset();
        h_StripAmp->Reset();
        h_StripTime->Reset();
        h_StripAmpL->Reset();
        h_StripTimeL->Reset();
        h_StripAmpR->Reset();
        h_StripTimeR->Reset();
        h_StripLeftMinusRight->Reset();
        h_StripLeftMinusRight_vs_Time->Reset();
        h_XYTime->Reset();
        NEvStrip47 = 0;
        for (Int_t iDig = 0; iDig < ToF400Digits->GetEntriesFast(); ++iDig) {
            BmnTof1Digit* digTof = (BmnTof1Digit*) ToF400Digits->At(iDig);
            if (digTof->GetPlane() == 2) {
                Int_t strip = digTof->GetStrip();
                Int_t plane = digTof->GetPlane();
                //    cout << "Plane = " << digTof->GetPlane() << " Ev# " << iEv << endl;
                if (digTof->GetSide() == 0 && Plane2.sStrHit[strip] == kFALSE) {
                    Plane2.sAmpL[strip] = digTof->GetAmplitude();
                    Plane2.sTimeL[strip] = digTof->GetTime();
                    //h_StripHit->SetBinContent(strip + 1, digTof->GetSide() + 1, digTof->GetAmplitude());
                }
                if (digTof->GetSide() == 1 && Plane2.sStrHit[strip] == kFALSE) {
                    Plane2.sAmpR[strip] = digTof->GetAmplitude();
                    Plane2.sTimeR[strip] = digTof->GetTime();
                    //h_StripHit->SetBinContent(strip + 1, digTof->GetSide() + 1, digTof->GetAmplitude());
                }

                if ((Plane2.sTimeR[strip] != 0 && Plane2.sTimeL[strip] != 0
                        && strip != 47
                        && TMath::Abs((Plane2.sTimeL[strip] - Plane2.sTimeR[strip]) * 0.5) <= 2.
                        && TMath::Abs((Plane2.sAmpL[strip] - Plane2.sAmpR[strip]) * 0.5) <= 1.5
                        && Plane2.sStrHit[strip] == kFALSE)) //
                    Plane2.sStrHit[strip] = kTRUE; //*/
            }

            if (digTof->GetPlane() == 7) {
                Int_t strip = digTof->GetStrip();
                Int_t plane = digTof->GetPlane();
                //    cout << "Plane = " << digTof->GetPlane() << " Ev# " << iEv << endl;
                if (digTof->GetSide() == 0 && Plane7.sStrHit[strip] == kFALSE) {
                    //if (digTof->GetTime() > 300. && digTof->GetTime() < 430.) {
                    Plane7.sAmpL[strip] = digTof->GetAmplitude();

                    if (strip == 0) {
                        NEvStrip47++;
                        if (Plane7.sTimeL[strip] != 0)
                            hNoize->Fill(1000. / (digTof->GetTime() - Plane7.sTimeL[strip]));
                    }

                    Plane7.sTimeL[strip] = digTof->GetTime();
                    h_StripHit->SetBinContent(strip + 1, digTof->GetSide() + 1, digTof->GetAmplitude());
                    //if (strip == 22) cout << "L ";
                }
                if (digTof->GetSide() == 1 && Plane7.sStrHit[strip] == kFALSE) {
                    //  if (digTof->GetTime() > 300. && digTof->GetTime() < 430.) {
                    Plane7.sAmpR[strip] = digTof->GetAmplitude();
                    Plane7.sTimeR[strip] = digTof->GetTime();
                    h_StripHit->SetBinContent(strip + 1, digTof->GetSide() + 1, digTof->GetAmplitude());
                    //if (strip == 22) cout << "R ";
                }

                if ((Plane7.sTimeR[strip] != 0 && Plane7.sTimeL[strip] != 0
                        && strip != 47
                        && TMath::Abs((Plane7.sTimeL[strip] - Plane7.sTimeR[strip]) * 0.5) <= 2.
                        && TMath::Abs((Plane7.sAmpL[strip] - Plane7.sAmpR[strip]) * 0.5) <= 1.5
                        && Plane7.sStrHit[strip] == kFALSE)) //
                    Plane7.sStrHit[strip] = kTRUE; //*/
            }
        }
        hNEvStrip47->Fill(NEvStrip47);

        for (Int_t i = 0; i < kNST; i++) {
            if (Plane2.sStrHit[i] == kFALSE || i == 0 || i == 47) {
                Plane2.sAmpL[i] = 0;
                Plane2.sAmpR[i] = 0;
                Plane2.sTimeL[i] = 0;
                Plane2.sTimeR[i] = 0;
                Plane2.sStrHit[i] = kFALSE;
            }
            if (Plane7.sStrHit[i] == kFALSE || i == 0 || i == 47) {
                Plane7.sAmpL[i] = 0;
                Plane7.sAmpR[i] = 0;
                Plane7.sTimeL[i] = 0;
                Plane7.sTimeR[i] = 0;
                Plane7.sStrHit[i] = kFALSE;
            }
        }

        for (Int_t iDig = 0; iDig < BC2Digits->GetEntriesFast(); ++iDig) {
            BmnTrigDigit* digBC2 = (BmnTrigDigit*) BC2Digits->At(iDig);
            //if (digBC2->GetAmp() > 6.15 && digBC2->GetAmp() < 17.14) 
            {
                T0.sNHits++;
                if (T0.sAmp == 0) {
                    T0.sAmp = digBC2->GetAmp();
                    T0.sTime = digBC2->GetTime();
                    hAmpT0->Fill(T0.sAmp);
                    hTimeT0->Fill(T0.sTime);
                }
            }
        }
        hHitT0PerEv->Fill(T0.sNHits);

        for (Int_t i = 0; i < kNST; i++)
            for (Int_t j = 0; j < kNST; j++) {
                if (Plane2.sAmpL[i] != 0 && Plane2.sAmpR[j] != 0)
                    hLefRightCorelation_Plane2->Fill(i, j);
                if (Plane7.sAmpL[i] != 0 && Plane7.sAmpR[j] != 0)
                    hLefRightCorelation_Plane7->Fill(i, j);
            }

        for (Int_t i = 0; i < kNST; i++) {
            if (Plane2.sAmpL[i] != 0 && Plane2.sAmpR[i] != 0) {
                Plane2.sAmp[i] = Plane2.sAmpL[i] + Plane2.sAmpR[i];
                Plane2.sTime[i] = (Plane2.sTimeL[i] + Plane2.sTimeR[i]) * 0.5;
                Plane2.sNHits++;
                h_StripHitSum->Fill(i);
                hAmpTofPlane2[i]->Fill(Plane2.sAmp[i]);
                hTimeTofPlane2[i]->Fill(Plane2.sTime[i]);
                hLevtMinusRightPlane2[i]->Fill((Plane2.sTimeL[i] - Plane2.sTimeR[i]) * 0.5);
                hAmpTofPlane2[kNST]->Fill(Plane2.sAmp[i]);
                hTimeTofPlane2[kNST]->Fill(Plane2.sTime[i]);
                hLevtMinusRightPlane2[kNST]->Fill((Plane2.sTimeL[i] - Plane2.sTimeR[i]) * 0.5);
                hAmpLRPlane2->Fill(Plane2.sAmpL[i], Plane2.sAmpR[i]);

                /*   h_StripAmp->Fill(i + 0.5, Plane2.sAmp[i]);
                   h_StripTime->Fill(i + 0.5, Plane2.sTime[i]);
                   h_StripAmpL->Fill(i + 0.5, Plane2.sAmpL[i]);
                   h_StripTimeL->Fill(i + 0.5, Plane2.sTimeL[i]);
                   h_StripAmpR->Fill(i + 0.5, Plane2.sAmpR[i]);
                   h_StripTimeR->Fill(i + 0.5, Plane2.sTimeR[i]);
                   h_StripLeftMinusRight->Fill(i + 0.5, Plane2.sTimeL[i] - Plane2.sTimeR[i]);
                   h_StripLeftMinusRight_vs_Time->Fill(Plane2.sTimeL[i] - Plane2.sTimeR[i], Plane2.sTime[i], Plane2.sAmp[i]);
                   //*/
            }
            //if (Plane7.sAmpL[i] != 0 && Plane7.sAmpR[i] != 0) {
            if (Plane7.sStrHit[i] != 0) {
                Plane7.sAmp[i] = Plane7.sAmpL[i] + Plane7.sAmpR[i];
                Plane7.sTime[i] = (Plane7.sTimeL[i] + Plane7.sTimeR[i]) / 2.;
                Plane7.sNHits++;
                h_StripHitSum->Fill(i);
                hAmpTofPlane7[i]->Fill(Plane7.sAmp[i]);
                hTimeTofPlane7[i]->Fill(Plane7.sTime[i]);
                hLeftMinusRightPlane7[i]->Fill((Plane7.sTimeL[i] - Plane7.sTimeR[i]) * 0.5);
                hAmpTofPlane7[kNST]->Fill(Plane7.sAmp[i]);
                hTimeTofPlane7[kNST]->Fill(Plane7.sTime[i]);
                hLeftMinusRightPlane7[kNST]->Fill((Plane7.sTimeL[i] - Plane7.sTimeR[i]) * 0.5);
                hAmpLRPlane7->Fill(Plane7.sAmpL[i], Plane7.sAmpR[i]);

                hLeftRightCorelationRPlane7[i]->Fill(Plane7.sAmpL[i], Plane7.sAmpR[i]);
                hLeftRightCorelationRPlane7[kNST]->Fill(Plane7.sAmpL[i], Plane7.sAmpR[i]);

                hLeftRightVsAmpLPlane7[i]->Fill(Plane7.sAmpL[i], (Plane7.sTimeL[i] - Plane7.sTimeR[i]) * 0.5);
                hLeftRightVsAmpLPlane7[kNST]->Fill(Plane7.sAmpL[i], (Plane7.sTimeL[i] - Plane7.sTimeR[i]) * 0.5);

                hLeftRightVsAmpRPlane7[i]->Fill(Plane7.sAmpR[i], (Plane7.sTimeL[i] - Plane7.sTimeR[i]) * 0.5);
                hLeftRightVsAmpRPlane7[kNST]->Fill(Plane7.sAmpR[i], (Plane7.sTimeL[i] - Plane7.sTimeR[i]) * 0.5);

            /*      h_StripAmp->Fill(i + 0.5, Plane7.sAmp[i]);
                  h_StripTime->Fill(i + 0.5, CalculateDt(&Plane7, &T0, i));
                  h_StripAmpL->Fill(i + 0.5, Plane7.sAmpL[i]);
                  h_StripTimeL->Fill(i + 0.5, Plane7.sTimeL[i]);
                  h_StripAmpR->Fill(i + 0.5, Plane7.sAmpR[i]);
                  h_StripTimeR->Fill(i + 0.5, Plane7.sTimeR[i]);
                  h_StripLeftMinusRight->Fill(i + 0.5, (Plane7.sTimeL[i] - Plane7.sTimeR[i]) * 0.5);
                  //h_StripLeftMinusRight_vs_Time->Fill((Plane7.sTimeL[i] - Plane7.sTimeR[i]) * 0.5, Plane7.sTime[i], Plane7.sAmp[i]);
                  h_XYTime->Fill(i, ((Plane7.sTimeL[i] - Plane7.sTimeR[i]) * 0.5 / 0.06));

                  //*/
            }
        }
        hHitStripsPerEvPlane2->Fill(Plane2.sNHits);
        hHitStripsPerEvPlane7->Fill(Plane7.sNHits);

        if (Plane2.sNHits == 1 && Plane2.sAmp[5] != 0 && T0.sNHits == 1) {
            HitPlane2T0++;
            if (Plane7.sNHits != 0) EffPlane7++;
        }
        if (Plane7.sNHits == 1 && Plane7.sAmp[42] != 0 && T0.sNHits == 1) {
            HitPlane7T0++;
            if (Plane2.sNHits != 0) EffPlane2++;
        }
        if (Plane2.sNHits != 0 && Plane7.sNHits != 0 && T0.sNHits == 1) {
            Int_t MaxStrPlane2 = TMath::LocMax(kNST, Plane2.sAmp);
            Int_t MaxStrPlane7 = TMath::LocMax(kNST, Plane7.sAmp);
            hPlane2Plane7Correlation->Fill(MaxStrPlane2, MaxStrPlane7);
        }

        Double_t dt = -100.;
        Int_t MaxStr = -1;
        if (Plane7.sNHits == 1 && T0.sNHits == 1
                && T0.sAmp >= 17.9 && T0.sAmp < 19.63) {
            MaxStr = TMath::LocMax(kNST, Plane7.sAmp);
            //dt = Plane7->sTime[MaxStr] - T0->sTime;
            dt = CalculateDt(&Plane7, &T0, MaxStr);
            hdt[MaxStr]->Fill(dt);
            hdt_vs_AmpT0[MaxStr]->Fill(T0.sAmp, dt);
            hdt_vs_AmpToF[MaxStr]->Fill(Plane7.sAmpL[MaxStr], dt);
            hdt[kNST]->Fill(dt);
            hdt_vs_AmpT0[kNST]->Fill(T0.sAmp, dt);
            hdt_vs_AmpToF[kNST]->Fill(Plane7.sAmpL[MaxStr], dt);
        }
        Int_t iEvDig = EventHeader->GetEntriesFast();
        if (iEvDig != 1) cout << "iEvDig == " << iEvDig << endl;
        BmnEventHeader* digEvent = (BmnEventHeader*) EventHeader->At(0);
        f_dt << (digEvent->GetEventId()) << "\t" << MaxStr << "\t" << dt << endl;

   /*     if (Plane7.sNHits != 0) {
            // if (Plane7.sNHits > 2) {
            cout << "Event# " << digEvent->GetEventId() << endl;
            Int_t MaxStr = TMath::LocMax(kNST, Plane7.sTime);
            //h_StripTime->GetYaxis()->SetRangeUser(-15., +15.);
            //h_StripTime->GetYaxis()->SetRangeUser(Plane7.sTime[MaxStr] - 15., Plane7.sTime[MaxStr] + 5.);
            //    cout << "event # " << iEv;
            Pad_StripHit->cd();
            h_StripHit->Draw("COLZ");
            Pad_StripHitSum->cd();
            //h_StripHitSum->Draw ();
            h_StripLeftMinusRight->Draw();
            Pad_StripAmp->cd();
            //h_StripLeftMinusRight_vs_Time->Draw();
            h_XYTime->Draw("COLZ");
            //h_StripAmp->Draw ("");
            //        h_StripAmpL->Draw("SAME");
            //        h_StripAmpR->Draw("SAME");

            Pad_StripTime->cd();
            h_StripTime->Draw("");
            //        h_StripTimeL->Draw("SAME");
            //        h_StripTimeR->Draw("SAME");
            c_1->Update();
            getchar();
        }//*/
    }

    f_dt.close();

    TString NameCallFile = file;
    Point = NameCallFile.First('.');
    NameCallFile.Replace(Point, 15, "_call.dat");
    cout << "Write call to " << NameCallFile.Data() << endl;
    f_call.open(NameCallFile.Data());
    //TProfile * pr_Hist_dt_FFD_SRPC200_vs_WSRPC[3];
    //TProfile * pr_Hist_dt_FFD_SRPC400_vs_WSRPC[3];
    for (i = 0; i < kNST + 1; i++) {

        hAmpTofPlane7[i]->Fit(f_gaus, "QW", "QW", hAmpTofPlane7[i]->GetMaximumBin() * hAmpTofPlane7[i]->GetXaxis()->GetBinWidth(1) - 0.5, hAmpTofPlane7[i]->GetMaximumBin() * hAmpTofPlane7[i]->GetXaxis()->GetBinWidth(1) + 0.8);
        sprintf(hid, "Plane7 Strip %d\t mean %f\t integral %.0f", i, f_gaus->GetParameter(1), hAmpTofPlane7[i]->GetEntries());
        f_call << hid << endl;
    }
    if (HitPlane7T0 != 0 && HitPlane2T0 != 0) {
        f_call << "Efficiency Plane2 = " << EffPlane2 << "/" << HitPlane7T0 << "=" << EffPlane2 * 100. / HitPlane7T0 << " %" << endl;
        cout << "Efficiency Plane2 = " << EffPlane2 << "/" << HitPlane7T0 << "=" << EffPlane2 * 100. / HitPlane7T0 << " %" << endl;
        f_call << "Efficiency Plane7 = " << EffPlane7 << "/" << HitPlane2T0 << "=" << EffPlane7 * 100. / HitPlane2T0 << " %" << endl;
        cout << "Efficiency Plane7 = " << EffPlane7 << "/" << HitPlane2T0 << "=" << EffPlane7 * 100. / HitPlane2T0 << " %" << endl;
    }
    f_call << "Strip\t Mean\t Sigma" << endl;
    for (i = 0; i < kNST + 1; i++) {
        f_gaus->SetParameter(1, 0.);
        f_gaus->SetParameter(2, 0.1);
        hdt[i]->Fit(f_gaus, "QW", "QW", -1, 1);
        f_call << i << "\t" << f_gaus->GetParameter(1) << "\t" << f_gaus->GetParameter(2) * 1000. << endl;
        cout << "Strip " << i << "\tMean " << f_gaus->GetParameter(1) << "\tSigma" << f_gaus->GetParameter(2) * 1000. << endl;
    }
    f_call.close();

    TString outName = file;
    Point = outName.First('.');
    outName.Replace(Point, 15, "_TofAn.root");
    TFile *fileout = new TFile(outName.Data(), "RECREATE");

    TDirectory * Dir;
    Dir = fileout->mkdir("ToF");
    Dir->cd();

    TDirectory * DirPlane2;
    DirPlane2 = Dir->mkdir("Plane2");
    DirPlane2->cd();
    ListTimeTofPlane2->Write();
    ListAmpTofPlane2->Write();
    ListLRTofPlane2->Write();

    TDirectory * DirPlane7;
    TDirectory * DirLRPlane7;
    DirPlane7 = Dir->mkdir("Plane7");
    DirLRPlane7 = DirPlane7->mkdir("LRCorelation");
    DirPlane7->cd();
    ListTimeTofPlane7->Write();
    ListAmpTofPlane7->Write();
    DirLRPlane7->cd();
    ListLRCorrTofPlane7->Write();
    ListLRTofPlane7->Write();
    ListLRvsAmpLTofPlane7->Write();
    ListLRvsAmpRTofPlane7->Write();

    TDirectory * DirT0;
    DirT0 = fileout->mkdir("T0");
    DirT0->cd();
    ListT0->Write();

    TDirectory * DirAnalysis;
    DirAnalysis = fileout->mkdir("Analysis");
    DirAnalysis->cd();
    ListAnalysis->Write();

    fileout->cd();
    ListStat->Write();
    fileout->Close();

}//end of macros

//----Additional functions ----------------------------------------------------------------

void
CleanTof(ToF400Detector * Det) {
    for (Int_t st = 0; st < kNST; st++) {
        Det->sAmpL[st] = 0.;
        Det->sAmpR[st] = 0.;
        Det->sAmp[st] = 0.;
        Det->sTimeL[st] = 0.;
        Det->sTimeR[st] = 0.;
        Det->sTime[st] = 0.;
        Det->sStrHit[st] = kFALSE;
    }
    Det->sNHits = 0;
}

void
CleanBC2(T0Detector * Det) {
    Det->sTime = 0;
    Det->sAmp = 0;
    Det->sNHits = 0;
}

Double_t CalculateDt(ToF400Detector * Det, T0Detector * T0, Int_t Str = -1) {

    if (Str == -1) Str = TMath::LocMax(kNST, Plane7->sAmp);
    Double_t dt = Det->sTime[Str] - T0->sTime;
    //cout << " dt = " << dt << endl;
  /*  if (Det->sAmp[Str] < 20.46) CorrPlane7_It1 = 0;
    else if (Det->sAmp[Str] >= 20.46 && Det->sAmp[Str] < 27.25) CorrPlane7_It1 = 1;
    else if (Det->sAmp[Str] >= 27.25 && Det->sAmp[Str] < 34.81) CorrPlane7_It1 = 2;
    else if (Det->sAmp[Str] >= 34.81 && Det->sAmp[Str] < 37.11) CorrPlane7_It1 = 3;
    else if (Det->sAmp[Str] >= 37.11) CorrPlane7_It1 = 4;
    dt = dt - (CorrPlane7Coeff_It1[CorrPlane7_It1][0] + CorrPlane7Coeff_It1[CorrPlane7_It1][1] * Det->sAmp[Str] +
            CorrPlane7Coeff_It1[CorrPlane7_It1][2] * Det->sAmp[Str] * Det->sAmp[Str] +
            CorrPlane7Coeff_It1[CorrPlane7_It1][3] * Det->sAmp[Str] * Det->sAmp[Str] * Det->sAmp[Str]); //*/

  /*  if (T0->sAmp >= 6.15 && T0->sAmp < 12.3) CorrT0_It1 = 0; //deutron
    else if (T0->sAmp >= 12.3 && T0->sAmp < 17.14) CorrT0_It1 = 1; //deutron
    else if (T0->sAmp >= 17.9 && T0->sAmp < 19.63) CorrT0_It1 = 2; //C6+
    else CorrT0_It1 = 3; //skip
    dt = dt - (CorrT0Coeff_It1[CorrT0_It1][0] + CorrT0Coeff_It1[CorrT0_It1][1] * T0->sAmp +
            CorrT0Coeff_It1[CorrT0_It1][2] * T0->sAmp * T0->sAmp +
            CorrT0Coeff_It1[CorrT0_It1][3] * T0->sAmp * T0->sAmp * T0->sAmp); //*/
    return dt;
}