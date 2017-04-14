/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   BmnHistToF700.cxx
 * Author: ilnur
 * 
 * Created on November 29, 2016, 1:24 PM
 */

#include "BmnHistToF700.h"

BmnHistToF700::BmnHistToF700(TString title = "ToF700") {
    fTitle = title;
    fName = title + "_cl";
    fSelectedPlane = -1;
    fSelectedStrip = -1;
    fSelectedSide = -1;
    TString name;
    name = fTitle + "_Leading_Time";
    histLeadingTime = new TH1D(name, name, 500, -800, 800);
    histLeadingTime->GetXaxis()->SetTitle("Time, ns");
    histLeadingTime->GetYaxis()->SetTitle("Activations count");
    name = fTitle + "_Leading_Time_Specific";
    histLeadingTimeSpecific = new TH1D(name, name, 500, -800, 800);
    histLeadingTimeSpecific->GetXaxis()->SetTitle("Time, ns");
    histLeadingTimeSpecific->GetYaxis()->SetTitle("Activations count");
    name = fTitle + "_Amplitude";
    histAmp = new TH1D(name, name, 4096, 0, 5000);
    histAmp->GetXaxis()->SetTitle("Amplitude, ns");
    histAmp->GetYaxis()->SetTitle("Activations count");
    name = fTitle + "_Amplitude_Specific";
    histAmpSpecific = new TH1D(name, name, 4096, 0, 5000);
    histAmpSpecific->GetXaxis()->SetTitle("Amplitude, ns");
    histAmpSpecific->GetYaxis()->SetTitle("Activations count");
    name = fTitle + "_Strip";
    histStrip = new TH1I(name, name, TOF2_MAX_CHAMBERS * TOF2_MAX_STRIPS_IN_CHAMBER, 0, TOF2_MAX_CHAMBERS * TOF2_MAX_STRIPS_IN_CHAMBER);
    histStrip->GetXaxis()->SetTitle("Strip #");
    histStrip->GetYaxis()->SetTitle("Activations count");
//    name = fTitle + "_StripSimult";
//    histStripSimult = new TH1I(name, name, TOF2_MAX_STRIPS_IN_CHAMBER, 0, TOF2_MAX_STRIPS_IN_CHAMBER);
//    name = fTitle + "_State";
//    histState = new TH2F(name, name, TOF2_MAX_STRIPS_IN_CHAMBER, 0, TOF2_MAX_STRIPS_IN_CHAMBER, 2, 0, 2);

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
//    canTimesPads[5]->current = histStripSimult;
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

BmnHistToF700::~BmnHistToF700() {
    delete histL;
    delete histR;
    delete Events;
}

void BmnHistToF700::FillFromDigi(TClonesArray * ToF4Digits) {
    histL->Reset();
    histR->Reset();
    histSimultaneous.Reset();
//    histState->Reset();
    Events->Clear();
    for (Int_t digIndex = 0; digIndex < ToF4Digits->GetEntriesFast(); digIndex++) {
        BmnTof2Digit *td = (BmnTof2Digit *) ToF4Digits->At(digIndex);
        Int_t strip = td->GetStrip();
        histLeadingTime->Fill(td->GetTime());
        histAmp->Fill(td->GetAmplitude());
        histStrip->Fill(strip + td->GetPlane() * TOF2_MAX_STRIPS_IN_CHAMBER);
//        if (td->GetPlane() == fSelectedPlane)
//            histState->Fill(td->GetStrip(), td->GetSide(), td->GetAmplitude());
        //        if (td->GetSide() == 0)
        //            histL->Fill(strip);
        //        else
        //            histR->Fill(strip);
        if (
                ((td->GetPlane() == fSelectedPlane) || (fSelectedPlane < 0)) &&
                ((td->GetStrip() == fSelectedStrip) || (fSelectedStrip < 0))) {
            histAmpSpecific->Fill(td->GetAmplitude());
            histLeadingTimeSpecific->Fill(td->GetTime());
        }

        new ((*Events)[Events->GetEntriesFast()])
                BmnTof2Digit(td->GetPlane(), td->GetStrip(), td->GetTime(), td->GetAmplitude(), td->GetDiff());
    }
    //histSimultaneous = (*histL) * (*histR);
    Int_t s;
//    histStripSimult->ResetStats();
//    for (Int_t binIndex = 1; binIndex < TOF2_MAX_STRIPS_IN_CHAMBER; binIndex++) {
//        s = ((histL->GetBinContent(binIndex) * histR->GetBinContent(binIndex)) != 0) ? 1 : 0;
//        histStripSimult->AddBinContent(s);
//    }
}

void BmnHistToF700::Register(THttpServer *serv) {
    fServer = serv;
    fServer->Register("/", this);
    TString path = "/" + fTitle + "/";
    fServer->Register(path, canTimes);
//    fServer->Register(path, histAmp);
//    fServer->Register(path, histAmpSpecific);
//    fServer->Register(path, histStrip);
//    fServer->Register(path, histStripSimult);
//    fServer->Register(path, histState);
//    fServer->Register(path, histLeadingTime);
//    fServer->Register(path, histLeadingTimeSpecific);

    TString cmd = "/" + fName + "/->SetRefRun(%arg1%)";
    TString cmdTitle = path + "SetRefRun";
    fServer->RegisterCommand(cmdTitle.Data(), cmd.Data(), "button;");
    fServer->Restrict(cmdTitle.Data(), "visible=shift");
    fServer->Restrict(cmdTitle.Data(), "allow=shift");
    fServer->Restrict(cmdTitle.Data(), "deny=guest");
    fServer->SetItemField(path.Data(), "_monitoring", "2000");
    fServer->SetItemField(path.Data(), "_layout","grid3x3");
    cmdTitle = path + "ChangeSlection";
    fServer->RegisterCommand(cmdTitle, TString("/") + fName.Data() + "/->SetSelection(%arg1%,%arg2%)", "button;");
    fServer->Restrict(cmdTitle.Data(), "visible=shift");
    fServer->Restrict(cmdTitle.Data(), "allow=shift");
    fServer->Restrict(cmdTitle.Data(), "deny=guest");
    cmdTitle = path + TString("Reset");
    fServer->RegisterCommand(cmdTitle, TString("/") + fName.Data() + "/->Reset()", "button;");
    fServer->Restrict(cmdTitle.Data(), "visible=shift");
    fServer->Restrict(cmdTitle.Data(), "allow=shift");
    fServer->Restrict(cmdTitle.Data(), "deny=guest");
}

void BmnHistToF700::SetDir(TFile* outFile, TTree* recoTree) {
    frecoTree = recoTree;
    TDirectory *dir = NULL;
    if (outFile != NULL)
        dir = outFile->mkdir(fTitle + "_hists");
    histLeadingTime->SetDirectory(dir);
    histLeadingTimeSpecific->SetDirectory(dir);
    histAmp->SetDirectory(dir);
    histAmpSpecific->SetDirectory(dir);
    histStrip->SetDirectory(dir);
//    histStripSimult->SetDirectory(dir);
//    histState->SetDirectory(dir);
    if (Events != NULL)
        delete Events;
    Events = new TClonesArray("BmnTof2Digit");
    if (frecoTree != NULL)
        frecoTree->Branch(fTitle.Data(), &Events);
}

void BmnHistToF700::SetSelection(Int_t Plane, Int_t Strip) {
    SetPlane(Plane);
    SetStrip(Strip);
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
        frecoTree->Draw(direction, command, "");
    }
}

void BmnHistToF700::Reset() {
    histLeadingTime->Reset();
    histLeadingTimeSpecific->Reset();
    histAmp->Reset();
    histAmpSpecific->Reset();
    histStrip->Reset();
//    histStripSimult->Reset();
//    histState->Reset();
}

void BmnHistToF700::DrawBoth() {
    BmnHist::DrawRef(canTimes, &canTimesPads);
}

BmnStatus BmnHistToF700::SetRefRun(Int_t id) {
    if (refID != id) {
        TString FileName = Form("bmn_run%04d_hist.root", id);
        printf("SetRefRun: %s\n", FileName.Data());
        refRunName = FileName;
        refID = id;
        BmnHist::LoadRefRun(refID, refPath + FileName, fTitle, canTimesPads, Names);
        DrawBoth();
    }
}

ClassImp(BmnHistToF700);

