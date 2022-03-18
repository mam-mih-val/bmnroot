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
#include "BmnRawDataDecoder.h"

BmnHistMwpc::BmnHistMwpc(TString title, TString path) : BmnHist() {
    fTitle = title;
    fName = title + "_cl";
    TString name;
    for (Int_t iModule = 0; iModule < MWPC_MODS; iModule++) {
        vector<TH1F*> row;
        vector<TH1F*> rowTimes;
        for (Int_t iStation = 0; iStation < MWPC_STATIONS; iStation++) {
            name = Form(fTitle + "Wires_Station_%d_module_%d", iStation, iModule);
            TH1F *h = new TH1F(name, name, MWPC_WIRES, 0, MWPC_WIRES);
            h->SetTitleSize(0.06, "XY");
            h->SetLabelSize(0.08, "XY");
            h->GetXaxis()->SetTitle("Strip Number");
            h->GetXaxis()->SetTitleColor(kOrange + 10);
            h->GetYaxis()->SetTitle("Activation Count");
            h->GetYaxis()->SetTitleColor(kOrange + 10);
            row.push_back(h);
            name = Form(fTitle + "Times_Station_%d_module_%d", iModule, iStation);
            TH1F *t = new TH1F(name, name, 500, 0, 1000);
            t->SetTitleSize(0.06, "XY");
            t->SetLabelSize(0.08, "XY");
            t->GetXaxis()->SetTitle("Strip Number");
            t->GetXaxis()->SetTitleColor(kOrange + 10);
            t->GetYaxis()->SetTitle("Activation Count");
            t->GetYaxis()->SetTitleColor(kOrange + 10);
            t->GetYaxis()->SetTitleOffset(1.6);
            rowTimes.push_back(t);
        }
        h_wires.push_back(row);
        h_times.push_back(rowTimes);
    }
    //for (Int_t iStation = 0; iStation < MWPC_STATIONS; iStation++) {
    //name = Form(fTitle + "_h_MWPC%d", iStation);
    name = fTitle + "_h_MWPC0";
    h_MWPC0 = new TH2F(name, "MWPC #0", 200, -20, 20, 200, -20, 20);
    name = fTitle + "_h_MWPC1";
    h_MWPC1 = new TH2F(name, "MWPC #1", 200, -20, 20, 200, -20, 20);
    name = fTitle + "_h_MWPC2";
    h_MWPC2 = new TH2F(name, "MWPC #2", 200, -20, 20, 200, -20, 20);
    name = fTitle + "_h_MWPC3";
    h_MWPC3 = new TH2F(name, "MWPC #3", 200, -20, 20, 200, -20, 20);
    name = fTitle + "CanvasWires";
    canWires = new TCanvas(name, name, PAD_WIDTH * MWPC_MODS, PAD_HEIGHT * MWPC_STATIONS);
    canWires->Divide(MWPC_MODS, MWPC_STATIONS);
    canWiresPads.resize(MWPC_MODS * MWPC_STATIONS);
    name = fTitle + "CanvasTimes";
    canTimes = new TCanvas(name, name, PAD_WIDTH * MWPC_MODS, PAD_HEIGHT * MWPC_STATIONS);
    canTimes->Divide(MWPC_MODS, MWPC_STATIONS);
    canTimesPads.resize(MWPC_MODS * MWPC_STATIONS);
    NamesWires.resize(MWPC_MODS * MWPC_STATIONS);
    NamesTimes.resize(MWPC_MODS * MWPC_STATIONS);
    for (Int_t iPlane = 0; iPlane < MWPC_MODS; iPlane++) {
        for (Int_t iStation = 0; iStation < MWPC_STATIONS; iStation++) {
            //            Int_t iPad = rowIndex * MWPC_STATIONS + colIndex;
            Int_t iPad = iStation * MWPC_MODS + iPlane;
            canWiresPads[iPad] = new PadInfo();
            canWiresPads[iPad]->current = h_wires[iPlane][iStation];
            canTimesPads[iPad] = new PadInfo();
            canTimesPads[iPad]->current = h_times[iPlane][iStation];
            canTimes->GetPad(iPad + 1)->SetGrid();
            canWires->GetPad(iPad + 1)->SetGrid();
            NamesWires[iPad] = canWiresPads[iPad]->current->GetName();
            NamesTimes[iPad] = canTimesPads[iPad]->current->GetName();
        }
    }
}

BmnHistMwpc::~BmnHistMwpc() {
    delete canWires;
    delete canTimes;
    if (fDir != NULL)
        return;
    delete h_MWPC0;
    delete h_MWPC1;
    delete h_MWPC2;
    delete h_MWPC3;
    for (auto pad : canWiresPads)
        delete pad;
    for (auto pad : canTimesPads)
        delete pad;
    for (auto pad : can2dPads)
        delete pad;
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
    for (auto row : h_wires)
        for (auto el : row)
            el->SetDirectory(fDir);
    for (auto row : h_times)
        for (auto el : row)
            el->SetDirectory(fDir);
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
     for (Int_t iDig = 0; iDig < digits->GetEntriesFast(); ++iDig) {
         BmnMwpcDigit* dig = (BmnMwpcDigit*) digits->At(iDig);
         Int_t station = dig->GetStation();
         Int_t plane = dig->GetPlane();
         h_wires[plane][station]->Fill(dig->GetWireNumber());
         h_times[plane][station]->Fill(dig->GetTime());
     }
//     MwpcHits->Clear();
//     for (Int_t iHit = 0; iHit < MwpcHits->GetEntriesFast(); ++iHit) {
//         BmnMwpcHit* hit = (BmnMwpcHit*) MwpcHits->At(iHit);
//         if (hit->GetMwpcId() == 0) h_MWPC0->Fill(hit->GetX(), hit->GetY());
//         if (hit->GetMwpcId() == 1) h_MWPC1->Fill(hit->GetX(), hit->GetY());
//         if (hit->GetMwpcId() == 2) h_MWPC2->Fill(hit->GetX(), hit->GetY());
//         if (hit->GetMwpcId() == 3) h_MWPC3->Fill(hit->GetX(), hit->GetY());
//     }
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

void BmnHistMwpc::ClearRefRun() {
    for (auto pad : canTimesPads) {
        if (pad->ref) delete pad->ref;
        pad->ref = NULL;
    }
    for (auto pad : canWiresPads) {
        if (pad->ref) delete pad->ref;
        pad->ref = NULL;
    }
}

void BmnHistMwpc::Reset() {
    for (auto row : h_wires)
        for (auto el : row)
            el->Reset();
    for (auto row : h_times)
        for (auto el : row)
            el->Reset();
    h_MWPC0->Reset();
    h_MWPC1->Reset();
    h_MWPC2->Reset();
    h_MWPC3->Reset();
}



ClassImp(BmnHistMwpc);
