/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   BmnHistToF.cxx
 * Author: ilnur
 * 
 * Created on November 27, 2016, 4:24 PM
 */

#include "BmnHistToF.h"

BmnHistToF::BmnHistToF(TString title = "ToF") {
    fTitle = title;
    fName = title + "_cl";
    fSelectedPlane = -1;
    fSelectedStrip = -1;
    fSelectedSide = -1;
}

BmnHistToF::~BmnHistToF() {
    delete histL;
    delete histR;
    delete Events;
    fServer->Unregister(histAmp);
    fServer->Unregister(histAmpSpecific);
    fServer->Unregister(histStrip);
    fServer->Unregister(histStripSimult);
    fServer->Unregister(histState);
    fServer->Unregister(histLeadingTime);
    fServer->Unregister(histLeadingTimeSpecific);
    fServer->Unregister(this);
}

void BmnHistToF::FillFromDigi(TClonesArray * ToF4Digits) {
    histL->Reset();
    histR->Reset();
    histSimultaneous.Reset();
    histState->Reset();
    for (Int_t digIndex = 0; digIndex < ToF4Digits->GetEntriesFast(); digIndex++) {
        BmnTof1Digit *td = (BmnTof1Digit *) ToF4Digits->At(digIndex);
        Int_t strip = td->GetStrip();
        histLeadingTime->Fill(td->GetTime());
        histAmp->Fill(td->GetAmplitude());
        histStrip->Fill(strip);
        if ((td->GetPlane() == fSelectedPlane))
            histState->Fill(td->GetStrip(), td->GetSide(), td->GetAmplitude());
        if (td->GetSide() == 0)
            histL->Fill(strip);
        else
            histR->Fill(strip);
        if ((td->GetPlane() == fSelectedPlane) || (fSelectedPlane < 0) &&
                (td->GetStrip() == fSelectedStrip) || (fSelectedStrip < 0) &&
                (td->GetSide() == fSelectedSide) || (fSelectedSide < 0)) {
            histAmpSpecific->Fill(td->GetAmplitude());
            histLeadingTimeSpecific->Fill(td->GetTime());
        }

        new ((*Events)[Events->GetEntriesFast()])
                BmnTof1Digit(td->GetPlane(), td->GetStrip(), td->GetSide(), td->GetTime(), td->GetAmplitude());
    }
    //histSimultaneous = (*histL) * (*histR);
    Int_t s;
    histStripSimult->ResetStats();
    for (Int_t binIndex = 1; binIndex < TOF400_STRIP_COUNT; binIndex++) {
        s = ((histL->GetBinContent(binIndex) * histR->GetBinContent(binIndex)) != 0) ? 1 : 0;
        histStripSimult->AddBinContent(s);
    }
    fEventsBranch->Fill();
}

void BmnHistToF::SaveHists() {
    SaveHist(histAmpSpecific, pathToImg);
    histLeadingTimeSpecific->SaveAs(pathToImg + histLeadingTimeSpecific->GetTitle() + ".png");
}

void BmnHistToF::Register(THttpServer *serv) {
    fServer = serv;
    fServer->Register("/", this);
    TString name;
    name = fTitle + "_Leading_Time";
    histLeadingTime = new TH1D(name, name, 500, 0, 1000);
    name = fTitle + "_Leading_Time_Specific";
    histLeadingTimeSpecific = new TH1D(name, name, 500, 0, 1000);
    name = fTitle + "_Amplitude";
    histAmp = new TH1D(name, name, 4096, 0, 96);
    name = fTitle + "_Amplitude_Specific";
    histAmpSpecific = new TH1D(name, name, 4096, 0, 96);
    name = fTitle + "_Strip";
    histStrip = new TH1I(name, name, TOF400_STRIP_COUNT, 0, TOF400_STRIP_COUNT);
    name = fTitle + "_StripSimult";
    histStripSimult = new TH1I(name, name, TOF400_STRIP_COUNT, 0, TOF400_STRIP_COUNT);
    name = fTitle + "_State";
    histState = new TH2F(name, name, TOF400_STRIP_COUNT, 0, TOF400_STRIP_COUNT, 2, 0, 2);

    histSimultaneous.SetDirectory(0);
    histL->SetDirectory(0);
    histR->SetDirectory(0);
    TString path = "/" + fTitle + "/";
    fServer->Register(path, histAmp);
    fServer->Register(path, histAmpSpecific);
    fServer->Register(path, histStrip);
    fServer->Register(path, histStripSimult);
    fServer->Register(path, histState);
    fServer->Register(path, histLeadingTime);
    fServer->Register(path, histLeadingTimeSpecific);

    fServer->SetItemField(path, "_monitoring", "2000");
    fServer->RegisterCommand(path + "ChangeSlection", TString("/") + fName.Data() + "/->SetSelection(%arg1%,%arg2%,%arg3%)", "button;");
    fServer->RegisterCommand(path + TString("Reset"), TString("/") + fName.Data() + "/->Reset()", "button;");
}

void BmnHistToF::SetDir(TFile* outFile, TTree* recoTree) {
    frecoTree = recoTree;
    if (outFile != NULL) {
        TDirectory *dir = outFile->mkdir(fTitle + "_hists");
        dir->cd();
        histLeadingTime->SetDirectory(dir);
        histLeadingTimeSpecific->SetDirectory(dir);
        histAmp->SetDirectory(dir);
        histAmpSpecific->SetDirectory(dir);
        histStrip->SetDirectory(dir);
        histStripSimult->SetDirectory(dir);
        histState->SetDirectory(dir);
    }
    if (Events != NULL)
        delete Events;
    Events = new TClonesArray("BmnTof1Digit");
    if (frecoTree != NULL)
        fEventsBranch = frecoTree->Branch(fTitle.Data(), &Events);
}

void BmnHistToF::SetSelection(Int_t Plane, Int_t Strip, Int_t Side) {
    SetPlane(Plane);
    SetStrip(Strip);
    SetSide(Side);
    TString command;
    if (fSelectedPlane >= 0)
        command = Form("fPlane == %d", fSelectedPlane);
    if (fSelectedStrip >= 0) {
        if (command.Length() > 0)
            command = command + " && ";
        command = command + Form("fStrip == %d", fSelectedStrip);
    }
    if (fSelectedSide >= 0) {
        if (command.Length() > 0)
            command = command + " && ";
        command = command + Form("fSide == %d", fSelectedSide);
    }
    histAmpSpecific->Reset();
    TString direction = "fAmplitude>>" + TString(histAmpSpecific->GetName());
    frecoTree->Draw(direction, command, "");
    histLeadingTimeSpecific->Reset();
    direction = "fTime>>" + TString(histLeadingTimeSpecific->GetName());
    frecoTree->Draw(direction, command, "");

}

void BmnHistToF::Reset() {
    histLeadingTime->Reset();
    histLeadingTimeSpecific->Reset();
    histAmp->Reset();
    histAmpSpecific->Reset();
    histStrip->Reset();
    histStripSimult->Reset();
    histState->Reset();
}

ClassImp(BmnHistToF);
