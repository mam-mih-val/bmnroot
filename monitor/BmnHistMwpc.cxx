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

BmnHistMwpc::BmnHistMwpc(TString title = "MWPC") {
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
    name = fTitle + "_h_MWPC1";
    h_MWPC1 = new TH2F(name, "MWPC #1", 200, -20, 20, 200, -20, 20);
    name = fTitle + "_h_MWPC2";
    h_MWPC2 = new TH2F(name, "MWPC #2", 200, -20, 20, 200, -20, 20);
    name = fTitle + "CanvasWires";
    canWires = new TCanvas(name, name, PAD_WIDTH * MWPC_ROWS, PAD_HEIGHT * MWPC_COLS);
    canWires->Divide(MWPC_ROWS, MWPC_COLS);
    canWiresPads.resize(MWPC_ROWS * MWPC_COLS);
    name = fTitle + "CanvasTimes";
    canTimes = new TCanvas(name, name, PAD_WIDTH * MWPC_ROWS, PAD_HEIGHT * MWPC_COLS);
    canTimes->Divide(MWPC_ROWS, MWPC_COLS);
    canTimesPads.resize(MWPC_ROWS * MWPC_COLS);
            printf("mwpc rows %d, cols %d\n", MWPC_ROWS, MWPC_COLS);
    for (Int_t rowIndex = 0; rowIndex < MWPC_ROWS; rowIndex++) {
        for (Int_t colIndex = 0; colIndex < MWPC_COLS; colIndex++) {
            PadInfo *p = new PadInfo();
            p->current = h_wires[rowIndex * MWPC_COLS + colIndex];
            canWiresPads[rowIndex * MWPC_COLS + colIndex] = p;
            PadInfo *pt = new PadInfo();
            pt->current = h_times[rowIndex * MWPC_COLS + colIndex];
            canTimesPads[rowIndex * MWPC_COLS + colIndex] = pt;
            canTimes->GetPad(rowIndex * MWPC_COLS + colIndex + 1)->SetGrid();
            canWires->GetPad(rowIndex * MWPC_COLS + colIndex + 1)->SetGrid();
        }
    }
}

BmnHistMwpc::~BmnHistMwpc() {
    for (Int_t i = 0; i < MWPC_PLANES; ++i){
        delete h_wires[i];
        delete h_times[i];
    }
    delete MwpcHits;
    delete h_MWPC1;
    delete h_MWPC2;
}

void BmnHistMwpc::Register(THttpServer *serv) {
    fServer = serv;
    fServer->Register("/", this);
    TString path = "/" + fTitle + "/";
    fServer->Register(path, canWires);
    fServer->Register(path, canTimes);
//    for (Int_t i = 0; i < MWPC_PLANES; ++i){
//        fServer->Register(path, h_wires[i]);
//        fServer->Register(path, h_times[i]);
//    }
    fServer->Register(path, h_MWPC1);
    fServer->Register(path, h_MWPC2);
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
    TDirectory *dir = NULL;
    if (outFile != NULL)
        dir = outFile->mkdir(fTitle + "_hists");
    for (Int_t i = 0; i < MWPC_PLANES; ++i){
        h_wires[i]->SetDirectory(dir);
        h_times[i]->SetDirectory(dir);
    }
    h_MWPC1->SetDirectory(dir);
    h_MWPC2->SetDirectory(dir);

}

void BmnHistMwpc::DrawBoth() {
    BmnHist::DrawRef(canWires, &canWiresPads);
    BmnHist::DrawRef(canTimes, &canTimesPads);
}

void BmnHistMwpc::FillFromDigi(TClonesArray * MwpcDigits) {
    MwpcHits->Clear();
    ProcessMwpcDigits(MwpcDigits, MwpcHits);
    for (Int_t iDig = 0; iDig < MwpcDigits->GetEntriesFast(); ++iDig) {
        BmnMwpcDigit* dig = (BmnMwpcDigit*) MwpcDigits->At(iDig);
        Int_t plane = dig->GetPlane();
        h_wires[plane]->Fill(dig->GetWireNumber());
        h_times[plane]->Fill(dig->GetTime());
    }
    for (Int_t iHit = 0; iHit < MwpcHits->GetEntriesFast(); ++iHit) {
        BmnMwpcHit* hit = (BmnMwpcHit*) MwpcHits->At(iHit);
        if (hit->GetMwpcId() == 0) h_MWPC1->Fill(hit->GetX(), hit->GetY());
        if (hit->GetMwpcId() == 1) h_MWPC2->Fill(hit->GetX(), hit->GetY());
    }
}

BmnStatus BmnHistMwpc::LoadRefRun(TString FileName) {
    printf("Loading ref histos\n");
    refFile = new TFile(refPath + FileName, "read");
    if (refFile->IsOpen() == false) {
        printf("Cannot open file %s !\n", FileName.Data());
        return kBMNERROR;
    }
    Int_t iPad = 0;
    TString refName = Form("ref%06d_", refID);
    TString name;
    for (Int_t rowIndex = 0; rowIndex < MWPC_ROWS; rowIndex++) {
        for (Int_t colIndex = 0; colIndex < MWPC_COLS; colIndex++) {
            iPad = rowIndex * MWPC_COLS + colIndex;
            if (canWiresPads[iPad]->ref){
                delete canWiresPads[iPad]->ref;
                canWiresPads[iPad]->ref = NULL;
            }
            TH1F* tempH = NULL;
            name = fTitle + "_" + Form("Plane_%d", iPad);
            tempH = (TH1F*) refFile->Get(refName + "MWPC_hists/" + refName + name);
            if (tempH == NULL) {
                tempH = (TH1F*) refFile->Get(TString("MWPC_hists/") + name);
            }
            if (tempH == NULL) {
                printf("Cannot load %s !\n", name.Data());
                continue;
//                return kBMNERROR;
            }
            canWiresPads[iPad]->ref = (TH1F*) (tempH->Clone(name));
            canWiresPads[iPad]->ref->SetLineColor(kRed);
            canWiresPads[iPad]->ref->SetDirectory(0);
            printf("Loaded %s \n", canWiresPads[iPad]->ref->GetName());
            if (canTimesPads[iPad]->ref){
                delete canTimesPads[iPad]->ref;
                canTimesPads[iPad]->ref = NULL;
            }
            tempH = NULL;
            name = fTitle + "_" + Form("Plane_%d", iPad) + "_Time";
            tempH = (TH1F*) refFile->Get(refName + "MWPC_hists/" + refName + name);
            if (tempH == NULL) {
                tempH = (TH1F*) refFile->Get(TString("MWPC_hists/") + name);
            }
            if (tempH == NULL) {
                printf("Cannot load %s !\n", name.Data());
                continue;
//                return kBMNERROR;
            }
            canTimesPads[iPad]->ref = (TH1F*) (tempH->Clone(name));
            canTimesPads[iPad]->ref->SetLineColor(kRed);
            canTimesPads[iPad]->ref->SetDirectory(0);
            printf("Loaded %s \n", canTimesPads[iPad]->ref->GetName());
        }
    }
    delete refFile;
    refFile = NULL;
    return kBMNSUCCESS;
}

BmnStatus BmnHistMwpc::SetRefRun(Int_t id) {
    TString FileName = Form("bmn_run%04d_hist.root", id);
    printf("SetRefRun: %s\n", FileName.Data());
    if (refRunName != FileName) {
        refRunName = FileName;
        refID = id;
        LoadRefRun(refRunName);
    }
}

void BmnHistMwpc::Reset() {
    for (Int_t i = 0; i < MWPC_PLANES; ++i){
        h_wires[i]->Reset();
        h_times[i]->Reset();
    }
    h_MWPC1->Reset();
    h_MWPC2->Reset();
}



ClassImp(BmnHistMwpc);