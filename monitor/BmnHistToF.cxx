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
    TString name;
    name = fTitle + "_Leading_Time";
    histLeadingTime = new TH1D(name, name, 500, 0, 1000);
    histLeadingTime->GetXaxis()->SetTitle("Time, ns");
    histLeadingTime->GetYaxis()->SetTitle("Activations count");
    name = fTitle + "_Leading_Time_Specific";
    histLeadingTimeSpecific = new TH1D(name, name, 500, 0, 1000);
    histLeadingTimeSpecific->GetXaxis()->SetTitle("Time, ns");
    histLeadingTimeSpecific->GetYaxis()->SetTitle("Activations count");
    name = fTitle + "_Amplitude";
    histAmp = new TH1D(name, name, 4096, 0, 96);
    histAmp->GetXaxis()->SetTitle("Amplitude, ns");
    histAmp->GetYaxis()->SetTitle("Activations count");
    name = fTitle + "_Amplitude_Specific";
    histAmpSpecific = new TH1D(name, name, 4096, 0, 96);
    histAmpSpecific->GetXaxis()->SetTitle("Amplitude, ns");
    histAmpSpecific->GetYaxis()->SetTitle("Activations count");
    name = fTitle + "_Strip";
    histStrip = new TH1I(name, name, TOF400_STRIP_COUNT, 0, TOF400_STRIP_COUNT);
    histStrip->GetXaxis()->SetTitle("Strip #");
    histStrip->GetYaxis()->SetTitle("Activations count");
    name = fTitle + "_StripSimult";
    histStripSimult = new TH1I(name, name, TOF400_STRIP_COUNT, 0, TOF400_STRIP_COUNT);
    //    histStripSimult->GetXaxis()->SetTitle("Strip count");
    //    histStripSimult->GetYaxis()->SetTitle("Activations count");
    name = fTitle + "_State";
    histState = new TH2F(name, name, TOF400_STRIP_COUNT, 0, TOF400_STRIP_COUNT, 2, 0, 2);
    histStrip->GetXaxis()->SetTitle("Strip #");
    histStrip->GetYaxis()->SetTitle("Side");

    histSimultaneous.SetDirectory(0);
    histL->SetDirectory(0);
    histR->SetDirectory(0);
    fServer = NULL;
    frecoTree = NULL;
    Events = NULL;
    name = fTitle + "CanvasTimes";
    canTimes = new TCanvas(name, name, PAD_WIDTH * TOF_ROWS, PAD_HEIGHT * TOF_COLS);
    canTimes->Divide(TOF_ROWS, TOF_COLS);
    canTimesPads.resize(TOF_ROWS * TOF_COLS);
    for (Int_t iPad = 0; iPad < TOF_ROWS * TOF_COLS; iPad++) {
        PadInfo* p = new PadInfo();
        canTimesPads[iPad] = p;
        canTimes->GetPad(iPad + 1)->SetGrid();
    }
    canTimesPads[0]->current = histLeadingTime;
    canTimesPads[1]->current = histAmp;
    canTimesPads[2]->current = histLeadingTimeSpecific;
    canTimesPads[3]->current = histAmpSpecific;
    canTimesPads[4]->current = histStrip;
    canTimesPads[5]->current = histStripSimult;
    for (Int_t iPad = 0; iPad < canTimesPads.size(); iPad++)
        if (canTimesPads[iPad]->current) {
            Names.push_back(canTimesPads[iPad]->current->GetName());
            canTimesPads[iPad]->current->SetTitleSize(0.06, "XY");
            canTimesPads[iPad]->current->SetLabelSize(0.08, "XY");
            TAxis *ax = canTimesPads[iPad]->current->GetYaxis();
            ax->SetTitleColor(kOrange + 10);
            ax->SetTitleOffset(1.8);
            ax->SetTitleFont(62);
            ax = canTimesPads[iPad]->current->GetXaxis();
            ax->SetTitleColor(kOrange + 10);
        }
}

BmnHistToF::~BmnHistToF() {
    delete histL;
    delete histR;
    delete Events;
}

void BmnHistToF::FillFromDigi(TClonesArray * ToF4Digits) {

    histL->Reset();
    histR->Reset();
    histSimultaneous.Reset();
    Events->Clear();
    for (Int_t digIndex = 0; digIndex < ToF4Digits->GetEntriesFast(); digIndex++) {
        BmnTof1Digit *td = (BmnTof1Digit *) ToF4Digits->At(digIndex);
        Int_t strip = td->GetStrip();
        histLeadingTime->Fill(td->GetTime());
        histAmp->Fill(td->GetAmplitude());
        histStrip->Fill(strip);
        if ((td->GetPlane() == fSelectedPlane)) {
            histState->Reset();
            histState->Fill(td->GetStrip(), td->GetSide(), td->GetAmplitude());
        }
        if (td->GetSide() == 0)
            histL->Fill(strip);
        else
            histR->Fill(strip);
        if (
                ((td->GetPlane() == fSelectedPlane) || (fSelectedPlane < 0)) &&
                ((td->GetStrip() == fSelectedStrip) || (fSelectedStrip < 0)) &&
                ((td->GetSide() == fSelectedSide) || (fSelectedSide < 0))) {
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
    //    if (fEventsBranch != NULL)
    //        fEventsBranch->Fill();
}

void BmnHistToF::Register(THttpServer *serv) {
    fServer = serv;
    fServer->Register("/", this);
    TString path = "/" + fTitle + "/";
    fServer->Register(path, canTimes);
//    fServer->Register(path, histAmp);
//    fServer->Register(path, histAmpSpecific);
//    fServer->Register(path, histStrip);
//    fServer->Register(path, histStripSimult);
    fServer->Register(path, histState);
//    fServer->Register(path, histLeadingTime);
//    fServer->Register(path, histLeadingTimeSpecific);

    TString cmd = "/" + fName + "/->SetRefRun(%arg1%)";
    TString cmdTitle = path + "SetRefRun";
    fServer->RegisterCommand(cmdTitle.Data(), cmd.Data(), "button;");
    fServer->Restrict(cmdTitle.Data(), "visible=shift");
    fServer->Restrict(cmdTitle.Data(), "allow=shift");
    fServer->Restrict(cmdTitle.Data(), "deny=guest");
    fServer->SetItemField(path.Data(), "_monitoring", "2000");
    fServer->SetItemField(path.Data(), "_layout", "grid3x3");
    cmdTitle = path + "ChangeSlection";
    fServer->RegisterCommand(cmdTitle, TString("/") + fName.Data() + "/->SetSelection(%arg1%,%arg2%,%arg3%)", "button;");
    fServer->Restrict(cmdTitle, "visible=shift");
    fServer->Restrict(cmdTitle, "allow=shift");
    fServer->Restrict(cmdTitle.Data(), "deny=guest");
    cmdTitle = path + TString("Reset");
    fServer->RegisterCommand(cmdTitle, TString("/") + fName.Data() + "/->Reset()", "button;");
    fServer->Restrict(cmdTitle.Data(), "visible=shift");
    fServer->Restrict(cmdTitle.Data(), "allow=shift");
    //    fServer->Restrict(cmdTitle.Data(), "deny=guest");
}

void BmnHistToF::SetDir(TFile* outFile, TTree* recoTree) {
    frecoTree = recoTree;
    TDirectory *dir = NULL;
    if (outFile != NULL)
        dir = outFile->mkdir(fTitle + "_hists");
    histLeadingTime->SetDirectory(dir);
    histLeadingTimeSpecific->SetDirectory(dir);
    histAmp->SetDirectory(dir);
    histAmpSpecific->SetDirectory(dir);
    histStrip->SetDirectory(dir);
    histStripSimult->SetDirectory(dir);
    histState->SetDirectory(dir);
    if (Events != NULL)
        delete Events;
    Events = new TClonesArray("BmnTof1Digit");
    if (frecoTree != NULL)
        frecoTree->Branch(fTitle.Data(), &Events);
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
    if (frecoTree != NULL) {
        histAmpSpecific->Reset();
        histAmpSpecific->SetTitle("Amplitude For: " + command);
        TString direction = "fAmplitude>>" + TString(histAmpSpecific->GetName());
        frecoTree->Draw(direction, command, "");
        histLeadingTimeSpecific->Reset();
        histLeadingTimeSpecific->SetTitle("Leading Time For: " + command);
        direction = "fTime>>" + TString(histLeadingTimeSpecific->GetName());
//        TDirectory *d = gDirectory->GetDirectory();
//        TDirectory()
        frecoTree->Draw(direction, command, "");
    }
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

void BmnHistToF::DrawBoth() {
    BmnHist::DrawRef(canTimes, &canTimesPads);
}

BmnStatus BmnHistToF::SetRefRun(Int_t id) {
    TString FileName = Form("bmn_run%04d_hist.root", id);
    printf("SetRefRun: %s\n", FileName.Data());
    if (refRunName != FileName) {
        refRunName = FileName;
        refID = id;
        BmnHist::LoadRefRun(refID, fTitle, canTimesPads, Names);
        DrawBoth();
    }
}

ClassImp(BmnHistToF);
