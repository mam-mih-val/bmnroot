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
    TString name;
    name = fTitle + "_BC1_Time_Length";
    histBC1TimeLen = new TH1D(name, name, 300, 0, 20000);
    name = fTitle + "_BC2_Time_Length";
    histBC2TimeLen = new TH1D(name, name, 300, 0, 2000);
    name = fTitle + "_FD_Time_Length";
    histFDTimeLen = new TH1D(name, name, 300, 0, 2000);
    name = fTitle + "_SD_Time_Length";
    histSDTimeLen = new TH1D(name, name, 300, 0, 2000);
    name = fTitle + "_VETO_Time_Length";
    histVDTimeLen = new TH1D(name, name, 300, 0, 2000);
    name = fTitle + "_BD_Channels";
    histBDChannels = new TH1I(name, name, BD_CHANNELS, 0, BD_CHANNELS);
    name = fTitle + "_BD_Specific_Channel";
    histBDSpecific = new TH1D(name, name, 300, 0, 2000);
    name = fTitle + "_Triggers_Counter";
    histTriggers = new TH1I(name, name, 5, 0, 5);
    TString triggerNames[5] = {"BC1", "SD", "BC2", "VD", "FD"};
    TAxis* xa = histTriggers->GetXaxis();
    for (Int_t i = 0; i < sizeof (triggerNames) / sizeof (*triggerNames); i++) {
        xa->SetBinLabel(i + 1, triggerNames[i]);
    }
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
        TClonesArray * BDdigits,
        BmnEventHeader * head, Int_t iEv) {
    BDEvents->Clear();
    histTriggers->SetTitle(fTitle + Form("_Triggers_Counter_runID_%d_nEvents_%d",
            head->GetRunId(), iEv));
    for (Int_t digIndex = 0; digIndex < BC1digits->GetEntriesFast(); digIndex++) {
        BmnTrigDigit* td0 = (BmnTrigDigit*) BC1digits->At(digIndex);
        histBC1TimeLen->Fill(td0->GetAmp());
        histTriggers->Fill(0);
    }
    for (Int_t digIndex = 0; digIndex < SDdigits->GetEntriesFast(); digIndex++) {
        BmnTrigDigit* td1 = (BmnTrigDigit*) SDdigits->At(digIndex);
        histSDTimeLen->Fill(td1->GetAmp());
        histTriggers->Fill(1);
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
    for (Int_t digIndex = 0; digIndex < FDdigits->GetEntriesFast(); digIndex++) {
        BmnTrigDigit* td3 = (BmnTrigDigit*) FDdigits->At(digIndex);
        histFDTimeLen->Fill(td3->GetAmp());
        histTriggers->Fill(4);
    }
    for (Int_t digIndex = 0; digIndex < BDdigits->GetEntriesFast(); digIndex++) {
        BmnTrigDigit* bd = (BmnTrigDigit*) BDdigits->At(digIndex);
        histBDChannels->Fill(bd->GetMod());
        new ((*BDEvents)[BDEvents->GetEntriesFast()]) BmnTrigDigit(bd->GetMod(), bd->GetTime(), bd->GetAmp());
        if (bd->GetMod() == fSelectedBDChannel)
            histBDSpecific->Fill(bd->GetAmp());
    }
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
    TString path = "/" + fTitle + "/";
    fServer->Register(path, histBC1TimeLen);
    fServer->Register(path, histBC2TimeLen);
    fServer->Register(path, histFDTimeLen);
    fServer->Register(path, histVDTimeLen);
    fServer->Register(path, histSDTimeLen);
    fServer->Register(path, histTriggers);
    fServer->Register(path, histBDChannels);
    fServer->Register(path, histBDSpecific);
    TString examples = TString("[") +
            histBC1TimeLen->GetTitle() + TString(",") +
            histBC2TimeLen->GetTitle() + TString(",") +
            histFDTimeLen->GetTitle() + TString(",") +
            histVDTimeLen->GetTitle() + TString(",") +
            histSDTimeLen->GetTitle() + TString(",") +
            histBDSpecific->GetTitle() + TString("]");
    fServer->SetItemField(path.Data(), "_monitoring", "2000");
    fServer->SetItemField(path.Data(), "_layout", "grid3x2");
    fServer->SetItemField(path, "_drawitem", examples);

    TString cmdTitle = path + "ChangeBDChannel";
    fServer->RegisterCommand(cmdTitle.Data(), "/" + fName + "/->SetSelBDChannel(%arg1%)", "button;");
    fServer->Restrict(cmdTitle.Data(), "visible=admin");
    fServer->Restrict(cmdTitle.Data(), "allow=admin");
    fServer->Restrict(cmdTitle.Data(), "deny=guest");
    cmdTitle = path + "Reset";
    fServer->RegisterCommand(cmdTitle.Data(), "/" + fName + "/->Reset()", "button;");
    fServer->Restrict(cmdTitle.Data(), "visible=admin");
    fServer->Restrict(cmdTitle.Data(), "allow=admin");
    fServer->Restrict(cmdTitle.Data(), "deny=guest");
}

void BmnHistTrigger::SetDir(TFile *outFile = NULL, TTree *recoTree = NULL) {
    frecoTree = recoTree;
    TDirectory *dir = NULL;
    if (outFile != NULL)
        dir = outFile->mkdir(fTitle + "_hists");
    //        dir->cd();
    histBC1TimeLen->SetDirectory(dir);
    histBC2TimeLen->SetDirectory(dir);
    histFDTimeLen->SetDirectory(dir);
    histSDTimeLen->SetDirectory(dir);
    histVDTimeLen->SetDirectory(dir);
    histBDChannels->SetDirectory(dir);
    histBDSpecific->SetDirectory(dir);
    histTriggers->SetDirectory(dir);
    if (BDEvents != NULL)
        delete BDEvents;
    BDEvents = new TClonesArray("BmnTrigDigit");
    if (recoTree != NULL)
        recoTree->Branch(fTitle + "_BD", &BDEvents);
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