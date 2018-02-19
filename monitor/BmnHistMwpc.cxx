/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   BmnHistMwpc.cxx
 * Author: ilnur
 * 
 * Created on December 13, 2016, 2:47 PM
 */

#include "BmnHistMwpc.h"
#include "BmnHitFinderRun1.h"
#include "BmnRawDataDecoder.h"

BmnHistMwpc::BmnHistMwpc(TString title) : BmnHist() {
    fTitle = title;
    fName = title + "_cl";
    for (Int_t i = 0; i < MWPC_PLANES; ++i){
        h_wires[i] = new TH1F(fTitle + "_" + Form("Plane_%d", i), Form("Plane_%d", i), MWPC_WIRES, 0, MWPC_WIRES);
        h_wires[i]->SetTitleSize(0.06, "XY");
        h_wires[i]->SetLabelSize(0.08, "XY");
        h_wires[i]->GetXaxis()->SetTitle("Wire Number");
        h_wires[i]->GetXaxis()->SetTitleColor(kOrange + 10);
        h_wires[i]->GetXaxis()->SetTitleFont(62);
        h_wires[i]->GetYaxis()->SetTitle("Activation Count");
        h_wires[i]->GetYaxis()->SetTitleColor(kOrange + 10);
        h_wires[i]->GetYaxis()->SetTitleOffset(1.8);
        h_wires[i]->GetYaxis()->SetTitleFont(62);
        h_times[i] = new TH1F(fTitle + "_" + Form("Plane_%d", i) + "_Time", TString(Form("Plane_%d", i)) + "_Time", 500, 0, 1000);
        h_times[i]->SetTitleSize(0.06, "XY");
        h_times[i]->SetLabelSize(0.08, "XY");
        h_times[i]->GetXaxis()->SetTitle("Time");
        h_times[i]->GetXaxis()->SetTitleColor(kOrange + 10);
        h_times[i]->GetXaxis()->SetTitleFont(62);
        h_times[i]->GetYaxis()->SetTitle("Activation Count");
        h_times[i]->GetYaxis()->SetTitleColor(kOrange + 10);
        h_times[i]->GetYaxis()->SetTitleOffset(1.8);
        h_times[i]->GetYaxis()->SetTitleFont(62);
    }
    TString name;
    MwpcHits = new TClonesArray("BmnMwpcHit");
    name = fTitle + "_h_MWPC0";
    h_MWPC0 = new TH2F(name, "MWPC #0", 200, -20, 20, 200, -20, 20);
    name = fTitle + "_h_MWPC1";
    h_MWPC1 = new TH2F(name, "MWPC #1", 200, -20, 20, 200, -20, 20);
    name = fTitle + "_h_MWPC2";
    h_MWPC2 = new TH2F(name, "MWPC #2", 200, -20, 20, 200, -20, 20);
    name = fTitle + "_h_MWPC3";
    h_MWPC3 = new TH2F(name, "MWPC #3", 200, -20, 20, 200, -20, 20);
    name = fTitle + "CanvasWires";
    canWires = new TCanvas(name, name, PAD_WIDTH * MWPC_ROWS, PAD_HEIGHT * MWPC_COLS);
    canWires->Divide(MWPC_ROWS, MWPC_COLS);
    canWiresPads.resize(MWPC_ROWS * MWPC_COLS);
    name = fTitle + "CanvasTimes";
    canTimes = new TCanvas(name, name, PAD_WIDTH * MWPC_ROWS, PAD_HEIGHT * MWPC_COLS);
    canTimes->Divide(MWPC_ROWS, MWPC_COLS);
    canTimesPads.resize(MWPC_ROWS * MWPC_COLS);
    NamesWires.resize(MWPC_ROWS * MWPC_COLS);
    NamesTimes.resize(MWPC_ROWS * MWPC_COLS);
    for (Int_t rowIndex = 0; rowIndex < MWPC_ROWS; rowIndex++) {
        for (Int_t colIndex = 0; colIndex < MWPC_COLS; colIndex++) {
            Int_t iPad = rowIndex * MWPC_COLS + colIndex;
            PadInfo *p = new PadInfo();
            p->current = h_wires[iPad];
            canWiresPads[iPad] = p;
            PadInfo *pt = new PadInfo();
            pt->current = h_times[iPad];
            canTimesPads[iPad] = pt;
            canTimes->GetPad(iPad + 1)->SetGrid();
            canWires->GetPad(iPad + 1)->SetGrid();
            NamesWires[iPad] = canWiresPads[iPad]->current->GetName();
            NamesTimes[iPad] = canTimesPads[iPad]->current->GetName();
        }
    }
}

BmnHistMwpc::~BmnHistMwpc() {
    if (fDir != NULL)
        return;
    for (Int_t i = 0; i < MWPC_PLANES; ++i){
        delete h_wires[i];
        delete h_times[i];
    }
    delete MwpcHits;
    delete h_MWPC0;
    delete h_MWPC1;
    delete h_MWPC2;
    delete h_MWPC3;
}

void BmnHistMwpc::Register(THttpServer *serv) {
    fServer = serv;
    fServer->Register("/", this);
    TString path = "/" + fTitle + "/";
    fServer->Register(path, canWires);
    fServer->Register(path, canTimes);
    fServer->Register(path, h_MWPC0);
    fServer->Register(path, h_MWPC1);
    fServer->Register(path, h_MWPC2);
    fServer->Register(path, h_MWPC3);
    TString cmd = "/" + fName + "/->Reset()";
    fServer->SetItemField(path.Data(), "_monitoring", "2000");
    fServer->SetItemField(path.Data(), "_layout", "grid3x3");
    TString cmdTitle = path + "Reset";
    fServer->RegisterCommand(cmdTitle.Data(), cmd.Data(), "button;");
    fServer->Restrict(cmdTitle, "visible=shift");
    fServer->Restrict(cmdTitle, "allow=shift");
    cmd = "/" + fName + "/->SetRefRun(%arg1%)";
    cmdTitle = path + "SetRefRun";
    fServer->RegisterCommand(cmdTitle.Data(), cmd.Data(), "button;");
}

void BmnHistMwpc::SetDir(TFile *outFile = NULL, TTree *recoTree = NULL) {
    frecoTree = recoTree;
    fDir = NULL;
    if (outFile != NULL)
        fDir = outFile->mkdir(fTitle + "_hists");
    for (Int_t i = 0; i < MWPC_PLANES; ++i){
        h_wires[i]->SetDirectory(fDir);
        h_times[i]->SetDirectory(fDir);
    }
    h_MWPC0->SetDirectory(fDir);
    h_MWPC1->SetDirectory(fDir);
    h_MWPC2->SetDirectory(fDir);
    h_MWPC3->SetDirectory(fDir);

}

void BmnHistMwpc::DrawBoth() {
    BmnHist::DrawRef(canWires, &canWiresPads);
    BmnHist::DrawRef(canTimes, &canTimesPads);
}

void BmnHistMwpc::FillFromDigi(DigiArrays *fDigiArrays) {
    TClonesArray * digits = fDigiArrays->mwpc;
    if (!digits)
        return;
    MwpcHits->Clear();
    ProcessMwpcDigits(digits, MwpcHits);
    for (Int_t iDig = 0; iDig < digits->GetEntriesFast(); ++iDig) {
        BmnMwpcDigit* dig = (BmnMwpcDigit*) digits->At(iDig);
        Int_t plane = dig->GetPlane();
        h_wires[plane]->Fill(dig->GetWireNumber());
        h_times[plane]->Fill(dig->GetTime());
    }
    for (Int_t iHit = 0; iHit < MwpcHits->GetEntriesFast(); ++iHit) {
        BmnMwpcHit* hit = (BmnMwpcHit*) MwpcHits->At(iHit);
        if (hit->GetMwpcId() == 0) h_MWPC0->Fill(hit->GetX(), hit->GetY());
        if (hit->GetMwpcId() == 1) h_MWPC1->Fill(hit->GetX(), hit->GetY());
        if (hit->GetMwpcId() == 2) h_MWPC2->Fill(hit->GetX(), hit->GetY());
        if (hit->GetMwpcId() == 3) h_MWPC3->Fill(hit->GetX(), hit->GetY());
    }
}

BmnStatus BmnHistMwpc::SetRefRun(Int_t id) {
    TString FileName = Form("bmn_run%04d_hist.root", id);
    printf("SetRefRun: %s\n", FileName.Data());
    if (refRunName != FileName) {
        refRunName = FileName;
        refID = id;
        BmnHist::LoadRefRun(refID, refPath + FileName, fTitle, canTimesPads, NamesTimes);
        BmnHist::LoadRefRun(refID, refPath + FileName, fTitle, canWiresPads, NamesWires);
        DrawBoth();
    }

    return kBMNSUCCESS;
}

void BmnHistMwpc::Reset() {
    for (Int_t i = 0; i < MWPC_PLANES; ++i){
        h_wires[i]->Reset();
        h_times[i]->Reset();
    }
    h_MWPC0->Reset();
    h_MWPC1->Reset();
    h_MWPC2->Reset();
    h_MWPC3->Reset();
}



ClassImp(BmnHistMwpc);
