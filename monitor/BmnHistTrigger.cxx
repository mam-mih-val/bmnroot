/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   BmnHistTrigger.cxx
 * Author: ilnur
 * 
 * Created on November 27, 2016, 4:18 PM
 */

#include "BmnHistTrigger.h"

BmnHistTrigger::BmnHistTrigger(TString title = "Triggers") {
    fTitle = title;
    fName = title + "_cl";
    fSelectedBDChannel = -1;
    BDEvents = new TClonesArray("BmnTrigDigit");
}

BmnHistTrigger::BmnHistTrigger(const BmnHistTrigger& orig) {
}

BmnHistTrigger::~BmnHistTrigger() {
    delete BDEvents;
    fServer->Unregister(histBC1TimeLen);
    fServer->Unregister(histBC2TimeLen);
    fServer->Unregister(histFDTimeLen);
    fServer->Unregister(histVDTimeLen);
    fServer->Unregister(histSDTimeLen);
    fServer->Unregister(histTriggers);
    fServer->Unregister(histBDChannels);
    fServer->Unregister(histBDSpecific);
    fServer->Unregister(this);
}

void BmnHistTrigger::FillFromDigi(
        TClonesArray * BC1digits,
        TClonesArray * SDdigits,
        TClonesArray * BC2digits,
        TClonesArray * VDdigits,
        TClonesArray * FDdigits,
        TClonesArray * BDdigits) {
    for (Int_t digIndex = 0; digIndex < BC1digits->GetEntriesFast(); digIndex++) {
        BmnTrigDigit* td1 = (BmnTrigDigit*) BC1digits->At(digIndex);
        histBC1TimeLen->Fill(td1->GetAmp());
        histTriggers->Fill(0);
    }
    for (Int_t digIndex = 0; digIndex < BC2digits->GetEntriesFast(); digIndex++) {
        BmnTrigDigit* td2 = (BmnTrigDigit*) BC2digits->At(digIndex);
        histBC2TimeLen->Fill(td2->GetAmp());
        histTriggers->Fill(2);
    }
    for (Int_t digIndex = 0; digIndex < VDdigits->GetEntriesFast(); digIndex++) {
        BmnTrigDigit* tv = (BmnTrigDigit*) VDdigits->At(digIndex);
        histVDTimeLen->Fill(tv->GetAmp());
        histTriggers->Fill(3);
    }
    //            for (Int_t digIndex = 0; digIndex < trigVDDigits->GetEntriesFast(); digIndex++) {
    //                BmnTrigDigit* bd = (BmnTrigDigit*) trigBDDigits->At(digIndex);
    //                bmh.histBDChannels->Fill(bd->GetMod());
    //                BmnTrigDigit *bd1 = new (*(bmh.BDEvents))[bmh.BDEvents->GetEntriesFast()] BmnTrigDigit();
    //                bd1->SetAmp(bd->GetAmp());
    //                bd1->SetDet(bd->GetDet());
    //                bd1->SetMod(bd->GetMod());
    //                bd1->SetTime(bd->GetTime());
    //            }
}

void BmnHistTrigger::SaveHists(TString imgSavePath) {
    SaveHist(histTriggers, imgSavePath);
}

void BmnHistTrigger::SetBDChannel(Int_t iSelChannel) {
    TString title;
    if (iSelChannel > (histBDSpecific->GetNbinsX() - 1)) {
        printf("Wrong channel!\n");
        return;
    }
    if (iSelChannel == -1)
        title = Form("BD for All Channels");
    else
        title = Form("BD for %d channel", iSelChannel);
    histBDSpecific->SetTitle(title);
    printf("Set channel: %d\n", fSelectedBDChannel);
    fSelectedBDChannel = iSelChannel;

}

void BmnHistTrigger::Register(THttpServer *serv) {
    fServer = serv;
    fServer->Register("/", this);
    TString name;
    name = "BC1_Time_Length";
    histBC1TimeLen = new TH1D(name, name, 300, 0, 20000);
    name = "BC2_Time_Length";
    histBC2TimeLen = new TH1D(name, name, 300, 0, 2000);
    name = "FD_Time_Length";
    histFDTimeLen = new TH1D(name, name, 300, 0, 2000);
    name = "SD_Time_Length";
    histSDTimeLen = new TH1D(name, name, 300, 0, 2000);
    name = "VETO_Time_Length";
    histVDTimeLen = new TH1D(name, name, 300, 0, 2000);
    name = "BD_Channels";
    histBDChannels = new TH1I(name, name, BD_CHANNELS, 0, BD_CHANNELS);
    name = "BD_Specific_Channel";
    histBDSpecific = new TH1D(name, name, 300, 0, 2000);
    name = "Triggers_Counter";
    histTriggers = new TH1I(name, name, 5, 0, 5);
    TString triggerNames[5] = {"BC1", "SD", "BC2", "VD", "FD"};
    TAxis* xa = histTriggers->GetXaxis();
    for (Int_t i = 0; i < sizeof (triggerNames) / sizeof (*triggerNames); i++) {
        xa->SetBinLabel(i + 1, triggerNames[i]);
    }
    TString path = "/" + fTitle + "/";
    fServer->Register(path, histBC1TimeLen);
    fServer->Register(path, histBC2TimeLen);
    fServer->Register(path, histFDTimeLen);
    fServer->Register(path, histVDTimeLen);
    fServer->Register(path, histSDTimeLen);
    fServer->Register(path, histTriggers);
    fServer->Register(path, histBDChannels);
    fServer->Register(path, histBDSpecific);
    fServer->SetItemField(path, "_monitoring", "2000");
    serv->SetItemField(path, "_layout", "grid3x2");
    serv->SetItemField(path, "_drawitem",
            "[BC1_Time_Length,BC2_Time_Length,FD_Time_Length,VETO_Time_Length,SD_Time_Length,BD_Specific_Channel]");

    fServer->RegisterCommand(path + "ChangeBDChannel", "/" + fName + "/->SetSelBDChannel(%arg1%)", "button;");
    fServer->RegisterCommand(path + "Reset", "/" + fName + "/->Reset()", "button;");
}

void BmnHistTrigger::SetDir(TFile *outFile = NULL, TTree *recoTree = NULL) {
    frecoTree = recoTree;
    if (outFile != NULL) {
        TDirectory *dir = outFile->mkdir(fTitle + "_hists");
        dir->cd();
        histBC1TimeLen->SetDirectory(dir);
        histBC2TimeLen->SetDirectory(dir);
        histFDTimeLen->SetDirectory(dir);
        histSDTimeLen->SetDirectory(dir);
        histVDTimeLen->SetDirectory(dir);
        histBDChannels->SetDirectory(dir);
        histBDSpecific->SetDirectory(dir);
        histTriggers->SetDirectory(dir);
    }
    if (BDEvents != NULL)
        delete BDEvents;
    BDEvents = new TClonesArray("BmnTrigDigit");
    if (recoTree != NULL)
        recoTree->Branch(fTitle, &BDEvents);
}

void BmnHistTrigger::Reset() {
    histBC1TimeLen->Reset();
    histBC2TimeLen->Reset();
    histSDTimeLen->Reset();
    histVDTimeLen->Reset();
    histFDTimeLen->Reset();
    histBDChannels->Reset();
    histBDSpecific->Reset();
    histTriggers->Reset();
}

ClassImp(BmnHistTrigger);