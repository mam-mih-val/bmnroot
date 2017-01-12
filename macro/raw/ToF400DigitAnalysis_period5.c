/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include <TMath.h>

#include "TChain.h"
#include "TH1.h"
#include "TH1S.h"
#include "TH2S.h"
#include "TH2F.h"
#include "TCanvas.h"
#include "TFile.h"
#include "TList.h"
#include "TDirectory.h"
#include "TPad.h"

const UInt_t kNST = 48;
const Double_t kTimeBin = 24. / 1024.;

struct ToF400Detector
{
    Double_t sTimeL[kNST];
    Double_t sTimeR[kNST];
    Double_t sTime[kNST];
    Double_t sAmpL[kNST];
    Double_t sAmpR[kNST];
    Double_t sAmp[kNST];
    Int_t sNHits;
};

struct BC2Detector
{
    Double_t sTime;
    Double_t sAmp;
    Int_t sNHits;
};

void ToF400DigitAnalysis_period5(Int_t Run = 84, Int_t nEvForRead = 0, Int_t Periud = 4)
{
    gROOT->LoadMacro("$VMCWORKDIR/macro/run/bmnloadlibs.C");
    bmnloadlibs(); // load BmnRoot libraries

    TChain *eveTree = new TChain("cbmsim");
    TString inName = Form("bmn_run%04d_digi.root", Run);
    cout << "Open file " << inName << endl << endl;
    eveTree->Add(inName);

    TClonesArray *ToF400Digits;
    eveTree->SetBranchAddress("TOF400", &ToF400Digits);

    TClonesArray *BC2Digits;
    eveTree->SetBranchAddress("T0", &BC2Digits);

    Long64_t nEvents = eveTree->GetEntries();
    if (nEvForRead == 0) nEvForRead = nEvents;
    cout << "Will be readed " << nEvForRead << " events from " << nEvents << endl;

    //------variables----------------------------------------------------------------------


    ToF400Detector Plane1;
    ToF400Detector Plane2;
    BC2Detector BC2;
    CleanTof(&Plane1);
    CleanTof(&Plane2);
    CleanBC2(&BC2);

    TList * ListStat = new TList();
    TList * ListAmpTofPlane1 = new TList();
    TList * ListTimeTofPlane1 = new TList();
    TList * ListLRTofPlane1 = new TList();
    TList * ListAmpTofPlane2 = new TList();
    TList * ListTimeTofPlane2 = new TList();
    TList * ListLRTofPlane2 = new TList();
    TString name;

    TH1S * hAmpTofPlane1[kNST];
    TH1S * hTimeTofPlane1[kNST];
    TH1S * hLevtMinusRightPlane1[kNST];
    for (Int_t i = 0; i < kNST + 1; ++i)
    {
        name = Form("hAmpTofPlane1_%d", i);
        hAmpTofPlane1[i] = new TH1S(name, name, 1024, 0, 50.);
        ListAmpTofPlane1->Add(hAmpTofPlane1[i]);
        name = Form("hTimeTofPlane1_%d", i);
        hTimeTofPlane1[i] = new TH1S(name, name, 1000, 0, 1000.);
        ListTimeTofPlane1->Add(hTimeTofPlane1[i]);
        name = Form("hLevtMinusRightPlane1_%d", i);
        hLevtMinusRightPlane1[i] = new TH1S(name, name, 1024, -24., 24.);
        ListLRTofPlane1->Add(hLevtMinusRightPlane1[i]);
    }

    TH1S * hAmpTofPlane2[kNST];
    TH1S * hTimeTofPlane2[kNST];
    TH1S * hLevtMinusRightPlane2[kNST];
    for (Int_t i = 0; i < kNST + 1; ++i)
    {
        name = Form("hAmpTofPlane2_%d", i);
        hAmpTofPlane2[i] = new TH1S(name, name, 1024, 0, 50.);
        ListAmpTofPlane2->Add(hAmpTofPlane2[i]);
        name = Form("hTimeTofPlane2_%d", i);
        hTimeTofPlane2[i] = new TH1S(name, name, 1000, 0, 1000.);
        ListTimeTofPlane2->Add(hTimeTofPlane2[i]);
        name = Form("hLevtMinusRightPlane2_%d", i);
        hLevtMinusRightPlane2[i] = new TH1S(name, name, 1024, -24., 24.);
        ListLRTofPlane2->Add(hLevtMinusRightPlane2[i]);
    }

    TH1S *hHitStripsPerEvPlane1 = new TH1S("hHitStripsPerEvPlane1", "Hit Strips Per Ev on Plane#1", kNST, 0, kNST);
    ListStat->Add(hHitStripsPerEvPlane1);
    TH1S *hHitStripsPerEvPlane2 = new TH1S("hHitStripsPerEvPlane2", "Hit Strips Per Ev on Plane#2", kNST, 0, kNST);
    ListStat->Add(hHitStripsPerEvPlane2);
    TH2S *hLefRightCorelation_Plane1 = new TH2S("hLefRightCorelation_Plane1", " Lef-Right Corelation Plane#1", kNST, 0, kNST, kNST, 0, kNST);
    ListStat->Add(hLefRightCorelation_Plane1);
    TH2S *hLefRightCorelation_Plane2 = new TH2S("hLefRightCorelation_Plane2", " Lef-Right Corelation Plane#2", kNST, 0, kNST, kNST, 0, kNST);
    ListStat->Add(hLefRightCorelation_Plane2);

    TList * ListBC2 = new TList();
    TH1S *hHitBC2PerEv = new TH1S("hHitBC2PerEv", "Hit BC2 Per Ev", 10, 0, 10);
    ListBC2->Add(hHitBC2PerEv);
    TH1S *hAmpBC2 = new TH1S("hAmpBC2", "Amp BC2", 1024, 0, 50);
    ListBC2->Add(hAmpBC2);
    TH1S *hTimeBC2 = new TH1S("hTimeBC2", "Time BC2", 1000, 0, 1000);
    ListBC2->Add(hTimeBC2);

    TH1S *hdt = new TH1S("hdt", "dt BC2 - ToF400(Plane1)", 1024 + 512, -24, 48);
    hdt->SetLineWidth(3);
    ListStat->Add(hdt);
    TH2S *hdt_vs_AmpBC2 = new TH2S("hdt_vs_AmpBC2", "hdt_vs_AmpBC2", 1024, 0, 50, 1024 + 512, -24, 48);
    ListStat->Add(hdt_vs_AmpBC2);
    TH2S *hdt_vs_AmpToF = new TH2S("hdt_vs_AmpToF", "hdt_vs_AmpToF", 1024, 0, 50, 1024 + 512, -24, 48);
    ListStat->Add(hdt_vs_AmpToF);

    TH2F *h_StripHit = new TH2F("h_StripHit", "h_StripHit", 48, 0, 48, 2, 0, 2);
    TH1I *h_StripHitSum = new TH1I("h_StripHitSum", "h_StripHitSum", 48, 0, 48);
    TH1F *h_StripAmp = new TH1F("h_StripAmp", "h_StripAmp", 48, 0, 48);
    h_StripAmp->GetYaxis()->SetLabelSize(0.07);
    h_StripAmp->SetLineWidth(2);
    h_StripAmp->SetLineColor(46);
    TH1F *h_StripTime = new TH1F("h_StripTime", "h_StripTime", 48, 0, 48);
    h_StripTime->GetYaxis()->SetLabelSize(0.07);
    h_StripTime->GetYaxis()->SetRangeUser(240, 360);
    h_StripTime->SetLineWidth(2);
    h_StripTime->SetLineColor(46);
    TH1F *h_StripAmpL = new TH1F("h_StripAmpL", "h_StripAmpL", 48, 0, 48);
    h_StripAmpL->GetYaxis()->SetLabelSize(0.07);
    h_StripAmpL->SetLineWidth(2);
    h_StripAmpL->SetLineColor(2);
    TH1F *h_StripTimeL = new TH1F("h_StripTimeL", "h_StripTimeL", 48, 0, 48);
    h_StripTimeL->GetYaxis()->SetLabelSize(0.07);
    h_StripTimeL->GetYaxis()->SetRangeUser(240, 360);
    h_StripTimeL->SetLineWidth(2);
    h_StripTimeL->SetLineColor(2);
    TH1F *h_StripAmpR = new TH1F("h_StripAmpR", "h_StripAmpR", 48, 0, 48);
    h_StripAmpR->GetYaxis()->SetLabelSize(0.07);
    h_StripAmpR->SetLineWidth(2);
    h_StripAmpR->SetLineColor(9);
    TH1F *h_StripTimeR = new TH1F("h_StripTimeR", "h_StripTimeR", 48, 0, 48);
    h_StripTimeR->GetYaxis()->SetLabelSize(0.07);
    h_StripTimeR->GetYaxis()->SetRangeUser(240, 470);
    h_StripTimeR->SetLineWidth(2);
    h_StripTimeR->SetLineColor(9);

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

    for (Int_t iEv = 0; iEv < nEvForRead; iEv++)
    {
        if (iEv % 10000 == 0) cout << "EVENT: " << iEv << endl;
        eveTree->GetEntry(iEv);
        CleanTof(&Plane1);
        CleanTof(&Plane2);
        CleanBC2(&BC2);
        h_StripHit->Reset();
        h_StripAmp->Reset();
        h_StripTime->Reset();
        h_StripAmpL->Reset();
        h_StripTimeL->Reset();
        h_StripAmpR->Reset();
        h_StripTimeR->Reset();
        for (Int_t iDig = 0; iDig < ToF400Digits->GetEntriesFast(); ++iDig)
        {
            BmnTof1Digit* digTof = (BmnTof1Digit*) ToF400Digits->At(iDig);
            if (digTof->GetPlane() != 2)
            {
                Int_t strip = digTof->GetStrip();
                //    cout << "Plane = " << digTof->GetPlane() << " Ev# " << iEv << endl;
                if (digTof->GetSide() == 0)
                {
                    //if (digTof->GetTime() > 300. && digTof->GetTime() < 430.) {
                    Plane1.sAmpL[strip] = digTof->GetAmplitude();
                    Plane1.sTimeL[strip] = digTof->GetTime();
                    h_StripHit->SetBinContent(strip + 1, digTof->GetSide() + 1, digTof->GetAmplitude());
                }
                if (digTof->GetSide() == 1)
                    //if (digTof->GetTime() > 300. && digTof->GetTime() < 430.)
                {

                    Plane1.sAmpR[strip] = digTof->GetAmplitude();
                    Plane1.sTimeR[strip] = digTof->GetTime();
                    h_StripHit->SetBinContent(strip + 1, digTof->GetSide() + 1, digTof->GetAmplitude());
                }
/*
                if (Plane1.sTimeR[strip] != 0 && Plane1.sTimeL[strip] != 0 && TMath::Abs(Plane1.sTimeL[strip] - Plane1.sTimeR[strip]) > 1.5)
                {
                    Plane1.sTimeR[strip] = 0.;
                    Plane1.sTimeL[strip] = 0.;
                    Plane1.sAmpR[strip] = 0.;
                    Plane1.sAmpL[strip] = 0.;
                }
*/
            }
            if (digTof->GetPlane() != 7)
            {
                Int_t strip = digTof->GetStrip();
                //    cout << "Plane = " << digTof->GetPlane() << " Ev# " << iEv << endl;
                if (digTof->GetSide() == 0)
                {
                    //if (digTof->GetTime() > 300. && digTof->GetTime() < 430.) {
                    Plane2.sAmpL[strip] = digTof->GetAmplitude();
                    Plane2.sTimeL[strip] = digTof->GetTime();
                    //h_StripHit->SetBinContent(strip + 1, digTof->GetSide() + 1, digTof->GetAmplitude());
                }
                if (digTof->GetSide() == 1)
                {
                    //  if (digTof->GetTime() > 300. && digTof->GetTime() < 430.) {
                    Plane2.sAmpR[strip] = digTof->GetAmplitude();
                    Plane2.sTimeR[strip] = digTof->GetTime();
                    //h_StripHit->SetBinContent(strip + 1, digTof->GetSide() + 1, digTof->GetAmplitude());
                }
                if (Plane2.sTimeR[strip] != 0 && Plane2.sTimeL[strip] != 0 && TMath::Abs(Plane2.sTimeL[strip] - Plane2.sTimeR[strip]) > 0.8)
                {
                    Plane2.sTimeR[strip] = 0;
                    Plane2.sTimeL[strip] = 0;
                    Plane2.sAmpR[strip] = 0.;
                    Plane2.sAmpL[strip] = 0.;
                }
            }
        }

        for (Int_t iDig = 0; iDig < BC2Digits->GetEntriesFast(); ++iDig)
        {
            BmnTrigDigit* digBC2 = (BmnTrigDigit*) BC2Digits->At(iDig);
            if (digBC2->GetMod() == 0)
            {
                BC2.sNHits++;
                //if (digBC2->GetTime() > 9000. && digBC2->GetTime() < 15000.) {
                BC2.sAmp = digBC2->GetAmp() /** kTimeBin*/;
                BC2.sTime = digBC2->GetTime() /** kTimeBin*/;
                hAmpBC2->Fill(BC2.sAmp);
                hTimeBC2->Fill(BC2.sTime);
                //}
            }
        }
        hHitBC2PerEv->Fill(BC2.sNHits);

        for (Int_t i = 0; i < kNST; i++)
            for (Int_t j = 0; j < kNST; j++)
            {
                if (Plane1.sAmpL[i] != 0 && Plane1.sAmpR[j] != 0)
                    hLefRightCorelation_Plane1->Fill(i, j);
                if (Plane2.sAmpL[i] != 0 && Plane2.sAmpR[j] != 0)
                    hLefRightCorelation_Plane2->Fill(i, j);
            }

        for (Int_t i = 0; i < kNST; i++)
        {
            //    if (BC2.sTime != 0 && BC2.sAmp < 30. && BC2.sAmp > 22.)
            if (Plane1.sAmpL[i] != 0 && Plane1.sAmpR[i] != 0)
            {
                Plane1.sAmp[i] = Plane1.sAmpL[i] + Plane1.sAmpR[i];
                Plane1.sTime[i] = (Plane1.sTimeL[i] + Plane1.sTimeR[i]) / 2.;
                Plane1.sNHits++;
                h_StripHitSum->Fill(i);
                hAmpTofPlane1[i]->Fill(Plane1.sAmp[i]);
                hTimeTofPlane1[i]->Fill(Plane1.sTime[i]);
                hLevtMinusRightPlane1[i]->Fill((Plane1.sTimeL[i] - Plane1.sTimeR[i]) / 2.);
                hAmpTofPlane1[kNST]->Fill(Plane1.sAmp[i]);
                hTimeTofPlane1[kNST]->Fill(Plane1.sTime[i]);
                hLevtMinusRightPlane1[kNST]->Fill((Plane1.sTimeL[i] - Plane1.sTimeR[i]) / 2.);
                h_StripAmp->Fill(i + 0.5, Plane1.sAmp[i]);
                h_StripTime->Fill(i + 0.5, Plane1.sTime[i]);
                h_StripAmpL->Fill(i + 0.5, Plane1.sAmpL[i]);
                h_StripTimeL->Fill(i + 0.5, Plane1.sTimeL[i]);
                h_StripAmpR->Fill(i + 0.5, Plane1.sAmpR[i]);
                h_StripTimeR->Fill(i + 0.5, Plane1.sTimeR[i]);

                /*
                                if (BC2.sTime != 0 && BC2.sAmp < 30. && BC2.sAmp > 22.) {
                                    Int_t MaxStr = i;
                                    Double_t dt = Plane1.sTime[MaxStr] - BC2.sTime;
                                    dt = dt - (0.4627 * Plane1.sAmp[MaxStr] - 0.9938);
                                    dt = dt - (-14.4506 * TMath::Exp(-1 * BC2.sAmp / 35.29832) + 6.99931);
                                    dt = dt - (5.854 - 0.378 * BC2.sAmp + 0.005775 * BC2.sAmp * BC2.sAmp);
                                    if (Plane1.sAmp[MaxStr] <= 26.17)
                                        dt = dt - (-41.65 + 5.602 * Plane1.sAmp[MaxStr] - 0.2459 * Plane1.sAmp[MaxStr] * Plane1.sAmp[MaxStr]
                                            + 0.003531 * Plane1.sAmp[MaxStr] * Plane1.sAmp[MaxStr] * Plane1.sAmp[MaxStr]);
                                    else
                                        dt = dt - (-32.95 + 2.812 * Plane1.sAmp[MaxStr] - 0.07872 * Plane1.sAmp[MaxStr] * Plane1.sAmp[MaxStr]
                                            + 0.0007292 * Plane1.sAmp[MaxStr] * Plane1.sAmp[MaxStr] * Plane1.sAmp[MaxStr]);
                                    hdt->Fill(dt);
                                    hdt_vs_AmpBC2->Fill(BC2.sAmp, dt);
                                    hdt_vs_AmpToF->Fill(Plane1.sAmp[MaxStr], dt);
                                    h_StripTime->Fill(i + 0.5, dt);
                                }
                 */

            }
            if (Plane2.sAmpL[i] != 0 && Plane2.sAmpR[i] != 0)
            {
                Plane2.sAmp[i] = Plane2.sAmpL[i] + Plane2.sAmpR[i];
                Plane2.sTime[i] = (Plane2.sTimeL[i] + Plane2.sTimeR[i]) / 2.;
                Plane2.sNHits++;
                h_StripHitSum->Fill(i);
                hAmpTofPlane2[i]->Fill(Plane2.sAmp[i]);
                hTimeTofPlane2[i]->Fill(Plane2.sTime[i]);
                hLevtMinusRightPlane2[i]->Fill((Plane2.sTimeL[i] - Plane2.sTimeR[i]) / 2.);
                hAmpTofPlane2[kNST]->Fill(Plane2.sAmp[i]);
                hTimeTofPlane2[kNST]->Fill(Plane2.sTime[i]);
                hLevtMinusRightPlane2[kNST]->Fill((Plane2.sTimeL[i] - Plane2.sTimeR[i]) / 2.);
                /*
                                h_StripAmp->Fill(i + 0.5, Plane2.sAmp[i]);
                                h_StripTime->Fill(i + 0.5, Plane2.sTime[i]);
                                h_StripAmpL->Fill(i + 0.5, Plane2.sAmpL[i]);
                                h_StripTimeL->Fill(i + 0.5, Plane2.sTimeL[i]);
                                h_StripAmpR->Fill(i + 0.5, Plane2.sAmpR[i]);
                                h_StripTimeR->Fill(i + 0.5, Plane2.sTimeR[i]);
                 */
            }
        }
        hHitStripsPerEvPlane1->Fill(Plane1.sNHits);
        hHitStripsPerEvPlane2->Fill(Plane2.sNHits);



        if (Plane1.sNHits == 1 && BC2.sTime != 0)
        {

            Int_t MaxStr = TMath::LocMax(kNST, Plane1.sAmp);
            Double_t dt = Plane1.sTime[MaxStr] - BC2.sTime + 36.;
            //cout << " dt = " << dt << endl;
            /*
                        dt = dt - (0.4627 * Plane1.sAmp[MaxStr] - 0.9938);
                        dt = dt - (-14.4506 * TMath::Exp(-1 * BC2.sAmp / 35.29832) + 6.99931);
                        dt = dt - (5.854 - 0.378 * BC2.sAmp + 0.005775 * BC2.sAmp * BC2.sAmp);
                                    if (Plane1.sAmp[MaxStr] <= 26.17)
                                        dt = dt - (-41.65 + 5.602 * Plane1.sAmp[MaxStr] - 0.2459 * Plane1.sAmp[MaxStr] * Plane1.sAmp[MaxStr]
                                            + 0.003531 * Plane1.sAmp[MaxStr] * Plane1.sAmp[MaxStr] * Plane1.sAmp[MaxStr]);
                                    else
                                        dt = dt - (-32.95 + 2.812 * Plane1.sAmp[MaxStr] - 0.07872 * Plane1.sAmp[MaxStr] * Plane1.sAmp[MaxStr]
                                            + 0.0007292 * Plane1.sAmp[MaxStr] * Plane1.sAmp[MaxStr] * Plane1.sAmp[MaxStr]);
             */

            hdt->Fill(dt);
            hdt_vs_AmpBC2->Fill(BC2.sAmp, dt);
            hdt_vs_AmpToF->Fill(Plane1.sAmp[MaxStr], dt);

        }

        if (Plane1.sNHits != 0)
        {
            Int_t MaxStr = TMath::LocMax(kNST, Plane1.sTime);
            //h_StripTime->GetYaxis()->SetRangeUser(-15., +15.);
            //h_StripTime->GetYaxis()->SetRangeUser(Plane1.sTime[MaxStr] - 15., Plane1.sTime[MaxStr] + 5.);
            cout << "event # " << iEv;
            Pad_StripHit->cd();
            h_StripHit->Draw("COLZ");
            Pad_StripHitSum->cd();
            h_StripHitSum->Draw();
            Pad_StripAmp->cd();
            h_StripAmp->Draw("");
            //        h_StripAmpL->Draw("SAME");
            //        h_StripAmpR->Draw("SAME");

            Pad_StripTime->cd();
            h_StripTime->Draw("");
            //        h_StripTimeL->Draw("SAME");
            //        h_StripTimeR->Draw("SAME");
            c_1->Update();
            getchar();
        }
    }

    TString outName = Form("bmn_run%04d_TofAn.root", Run);
    TFile *fileout = new TFile(outName.Data(), "RECREATE");

    TDirectory * Dir;
    Dir = fileout->mkdir("ToF");
    Dir->cd();

    TDirectory * DirPlane1;
    DirPlane1 = Dir->mkdir("SRPC200");
    DirPlane1->cd();
    ListTimeTofPlane1->Write();
    ListAmpTofPlane1->Write();
    ListLRTofPlane1->Write();

    TDirectory * DirPlane2;
    DirPlane2 = Dir->mkdir("SRPCmpd");
    DirPlane2->cd();
    ListTimeTofPlane2->Write();
    ListAmpTofPlane2->Write();
    ListLRTofPlane2->Write();

    TDirectory * DirBC2;
    DirBC2 = fileout->mkdir("BC2");
    DirBC2->cd();
    ListBC2->Write();

    fileout->cd();
    ListStat->Write();
    fileout->Close();

}//end of macros

//----Additional functions ----------------------------------------------------------------

void CleanTof(ToF400Detector * Det)
{
    for (Int_t st = 0; st < kNST; st++)
    {

        Det->sAmpL[st] = 0.;
        Det->sAmpR[st] = 0.;
        Det->sAmp[st] = 0.;
        Det->sTimeL[st] = 0.;
        Det->sTimeR[st] = 0.;
        Det->sTime[st] = 0.;
    }
    Det->sNHits = 0;
}

void CleanBC2(BC2Detector * Det)
{
    Det->sTime = 0;
    Det->sAmp = 0;
    Det->sNHits = 0;
}