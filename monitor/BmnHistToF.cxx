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
#include "BmnRawDataDecoder.h"

BmnHistToF::BmnHistToF(TString title, TString path) : BmnHist() {
    fTitle = title;
    fName = title + "_cl";
    fSelectedPlane = -1;
    fSelectedStrip = -1;
    fSelectedSide = -1;
    TString name;
    name = fTitle + "_Leading_Time";
    histLeadingTime = new TH1D(name, name, 500, 0, 2500);
    histLeadingTime->GetXaxis()->SetTitle("Time, ns");
    histLeadingTime->GetYaxis()->SetTitle("Activations count");
    name = fTitle + "_Leading_Time_Specific";
    histLeadingTimeSpecific = new TH1D(name, name, 500, 0, 2500);
    histLeadingTimeSpecific->GetXaxis()->SetTitle("Time, ns");
    histLeadingTimeSpecific->GetYaxis()->SetTitle("Activations count");
    name = fTitle + "_Amplitude";
    histAmp = new TH1D(name, name, 4096, 0, 96);
    histAmp->GetXaxis()->SetTitle("Amplitude, ns");
    histAmp->GetYaxis()->SetTitle("Activations count");
    TGaxis *ay = (TGaxis*) histAmp->GetYaxis();
    ay->SetMaxDigits(2);
    name = fTitle + "_Amplitude_Specific";
    histAmpSpecific = new TH1D(name, name, 4096, 0, 96);
    histAmpSpecific->GetXaxis()->SetTitle("Amplitude, ns");
    histAmpSpecific->GetYaxis()->SetTitle("Activations count");
    name = fTitle + "_Strip";
    histStrip = new TH1I(name, name, TOF400_STRIP_COUNT, 0, TOF400_STRIP_COUNT);
    histStrip->GetXaxis()->SetTitle("Strip #");
    histStrip->GetYaxis()->SetTitle("Activations count");
    histAmp->GetYaxis()->SetNoExponent(kFALSE);
    name = fTitle + "_StripSimult";
    histStripSimult = new TH1I(name, name, TOF400_STRIP_COUNT, 0, TOF400_STRIP_COUNT);
    histStripSimult->GetXaxis()->SetTitle("Strip count");
    histStripSimult->GetYaxis()->SetTitle("Activations count");
    name = fTitle + "_State";
    histState = new TH2F(name, name, TOF400_STRIP_COUNT, 0, TOF400_STRIP_COUNT, 2, 0, 2);
    histState->GetXaxis()->SetTitle("Strip #");
    histState->GetYaxis()->SetTitle("Side");
    name = fTitle + "_State2D";
    histStrip2d = new TH2I(name, name, TOF400_STRIP_COUNT, 0, TOF400_STRIP_COUNT, TOF400_PLANE_COUNT, 0, TOF400_PLANE_COUNT);
    histStrip2d->GetXaxis()->SetTitle("Strip #");
    histStrip2d->GetYaxis()->SetTitle("Plane #");

    histSimultaneous.SetDirectory(0);
    histL->SetDirectory(0);
    histR->SetDirectory(0);
    fServer = NULL;
    frecoTree = NULL;
    Events = NULL;
    name = fTitle + "Canvas2d";
    can2d = new TCanvas(name, name, PAD_WIDTH * 2, PAD_HEIGHT * 1);
    can2d->Divide(2, 1);
    can2dPads.resize(2 * 1);
    for (Int_t iPad = 0; iPad < 2 * 1; iPad++) {
        PadInfo* p = new PadInfo();
        can2dPads[iPad] = p;
        p->opt = "colz nostat";
    }
    can2dPads[0]->current = histStrip2d;
    can2dPads[1]->current = histState;
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
            ax->SetTitleOffset(1);
            ax->SetTitleFont(62);
            ax = canTimesPads[iPad]->current->GetXaxis();
            ax->SetTitleColor(kOrange + 10);
            ax->SetTitleOffset(1);
            ax->SetTitleFont(62);
        }
    for (Int_t iPad = 0; iPad < can2dPads.size(); iPad++)
        if (can2dPads[iPad]->current) {
            //            Names.push_back(canTimesPads[iPad]->current->GetName());
            can2dPads[iPad]->current->SetTitleSize(0.06, "XY");
            can2dPads[iPad]->current->SetLabelSize(0.08, "XY");
            TAxis *ax = can2dPads[iPad]->current->GetYaxis();
            ax->SetTitleColor(kOrange + 10);
            ax->SetTitleOffset(1);
            ax->SetTitleFont(62);
            ax = can2dPads[iPad]->current->GetXaxis();
            ax->SetTitleColor(kOrange + 10);
            ax->SetTitleOffset(1);
            ax->SetTitleFont(62);
        }
}

BmnHistToF::~BmnHistToF() {
    delete histL;
    delete histR;
    delete Events;
    delete canTimes;
    delete can2d;
    if (fDir)
        return;
    for (auto pad : canTimesPads)
        delete pad;
    for (auto pad : can2dPads)
        delete pad;
}

void BmnHistToF::FillFromDigi(DigiArrays *fDigiArrays) {
    TClonesArray * digits = fDigiArrays->tof400;
    if (!digits)
        return;
    histL->Reset();
    histR->Reset();
    histSimultaneous.Reset();
    Events->Clear();
    for (Int_t digIndex = 0; digIndex < digits->GetEntriesFast(); digIndex++) {
        BmnTof1Digit *td = (BmnTof1Digit *) digits->At(digIndex);
        Int_t strip = td->GetStrip();
        if ((strip == 0) || (strip == (TOF400_STRIP_COUNT - 1))) // noisy
            continue;
        histLeadingTime->Fill(td->GetTime());
        histAmp->Fill(td->GetAmplitude());
        histStrip->Fill(strip);
        histStrip2d->Fill(strip, td->GetPlane(), 1);
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

        //        new ((*Events)[Events->GetEntriesFast()])
        //                BmnTof1Digit(td->GetPlane(), td->GetStrip(), td->GetSide(), td->GetTime(), td->GetAmplitude());
        //        frecoTree->Fill();
    }
    //histSimultaneous = (*histL) * (*histR);
    Int_t s;
    histStripSimult->ResetStats();
    for (Int_t binIndex = 1; binIndex < TOF400_STRIP_COUNT; binIndex++) {
        s = ((histL->GetBinContent(binIndex) * histR->GetBinContent(binIndex)) != 0) ? 1 : 0;
        if (s) histStripSimult->AddBinContent(s);
    }
    //    if (fEventsBranch != NULL)
    //        fEventsBranch->Fill();
}

void BmnHistToF::Register(THttpServer *serv) {
    fServer = serv;
    fServer->Register("/", this);
    TString path = "/" + fTitle + "/";
    fServer->Register(path, canTimes);
    fServer->Register(path, can2d);

    TString cmd = "/" + fName + "/->SetRefRun(%arg1%)";
    TString cmdTitle = path + "SetRefRun";
    fServer->RegisterCommand(cmdTitle.Data(), cmd.Data(), "button;");
    fServer->Restrict(cmdTitle.Data(), "visible=shift");
    fServer->Restrict(cmdTitle.Data(), "allow=shift");
    fServer->Restrict(cmdTitle.Data(), "deny=guest");
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
    fDir = NULL;
    if (outFile != NULL)
        fDir = outFile->mkdir(fTitle + "_hists");
    histLeadingTime->SetDirectory(fDir);
    histLeadingTimeSpecific->SetDirectory(fDir);
    histAmp->SetDirectory(fDir);
    histAmpSpecific->SetDirectory(fDir);
    histStrip->SetDirectory(fDir);
    histStripSimult->SetDirectory(fDir);
    histState->SetDirectory(fDir);
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
        command = Form("%s.fPlane == %d", fTitle.Data(), fSelectedPlane);
    if (fSelectedStrip >= 0) {
        if (command.Length() > 0)
            command = command + " && ";
        command = command + Form("%s.fStrip == %d", fTitle.Data(), fSelectedStrip);
    }
    if (fSelectedSide >= 0) {
        if (command.Length() > 0)
            command = command + " && ";
        command = command + Form("%s.fSide == %d", fTitle.Data(), fSelectedSide);
    }
    histAmpSpecific->Reset();
    histLeadingTimeSpecific->Reset();
    histAmpSpecific->SetTitle("Amplitude For: " + command);
    histLeadingTimeSpecific->SetTitle("Leading Time For: " + command);
    if (frecoTree != NULL) {
        histAmpSpecific->SetTitle("Amplitude For: " + command);
        TString direction = "fAmplitude>>" + TString(histAmpSpecific->GetName());
        //        TCanvas *c1 = new TCanvas("c1");
        frecoTree->Draw(direction.Data(), command.Data(), "goff");
        histLeadingTimeSpecific->SetTitle("Leading Time For: " + command);
        direction = "fTime>>" + TString(histLeadingTimeSpecific->GetName());
        //        TDirectory *d = gDirectory->GetDirectory();
        //        TDirectory()
        frecoTree->Draw(direction.Data(), command.Data(), "goff");
        //        delete c1;
    }
}

void BmnHistToF::Reset() {
    for (auto &h : canTimesPads) {
        h->current->Reset();
    }
    for (auto &h : can2dPads) {
        h->current->Reset();
    }
}

void BmnHistToF::DrawBoth() {
    BmnHist::DrawRef(canTimes, &canTimesPads);
    BmnHist::DrawRef(can2d, &can2dPads);
}

BmnStatus BmnHistToF::SetRefRun(Int_t id) {
    if (refID != id) {
        TString FileName = Form("bmn_run%04d_hist.root", id);
        printf("SetRefRun: %s\n", FileName.Data());
        refRunName = FileName;
        refID = id;
        BmnHist::LoadRefRun(refID, refPath + FileName, fTitle, canTimesPads, Names);
        DrawBoth();
    }
    return kBMNSUCCESS;
}

void BmnHistToF::ClearRefRun() {
    for (auto &pad : canTimesPads) {
        if (pad->ref) delete pad->ref;
        pad->ref = NULL;
    }
    refID = 0;
}

ClassImp(BmnHistToF);
