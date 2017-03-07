/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   BmnHistDch.cxx
 * Author: ilnur
 * 
 * Created on November 29, 2016, 2:44 PM
 */

#include "BmnHistDch.h"
#include "BmnHitFinderRun1.h"


//const Float_t angles[kNPLANES] = {
//    135, 135, 45, 45, 90, 90, 180, 180,
//    135, 135, 45, 45, 90, 90, 180, 180
//};
const TString names[kNPLANES] = {
    "VA_1", "VB_1", "UA_1", "UB_1", "YA_1", "YB_1", "XA_1", "XB_1",
    "VA_2", "VB_2", "UA_2", "UB_2", "YA_2", "YB_2", "XA_2", "XB_2"
};

BmnHistDch::BmnHistDch(TString title = "DCH") {
 TGaxis::SetMaxDigits(2);
    fTitle = title;
    fName = title + "_cl";
    for (Int_t i = 0; i < kNPLANES; ++i) {
        h_wires[i] = new TH1F(fTitle + "_" + names[i], names[i], kNREALWIRES, 0, kNREALWIRES);
        h_wires[i]->SetTitleSize(0.06, "XY");
        h_wires[i]->SetLabelSize(0.08, "XY");
        h_wires[i]->GetXaxis()->SetTitle("Wire Number");
        h_wires[i]->GetXaxis()->SetTitleColor(kOrange + 10);
        h_wires[i]->GetXaxis()->SetTitleFont(62);
        h_wires[i]->GetYaxis()->SetTitle("Activation Count");
        h_wires[i]->GetYaxis()->SetTitleColor(kOrange + 10);
        h_wires[i]->GetYaxis()->SetTitleOffset(1.8);
        h_wires[i]->GetYaxis()->SetTitleFont(62);
        h_times[i] = new TH1F(fTitle + "_" + names[i] + "_Time", names[i] + "_Time", 500, -300, 300);
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
    fDchHits = new TClonesArray("BmnDchHit");
    name = fTitle + "_h_DCH1";
    h_DCH1 = new TH2F(name, "DCH #1", 500, -DCH_WDTH, DCH_WDTH, 500, -DCH_WDTH, DCH_WDTH);
    h_DCH1->GetXaxis()->SetTitle("X, cm");
    h_DCH1->GetXaxis()->SetTitleColor(kOrange + 10);
    h_DCH1->GetYaxis()->SetTitle("Y, cm");
    h_DCH1->GetYaxis()->SetTitleColor(kOrange + 10);
    name = fTitle + "_h_DCH2";
    h_DCH2 = new TH2F(name, "DCH #2", 500, -DCH_WDTH, DCH_WDTH, 500, -DCH_WDTH, DCH_WDTH);
    h_DCH2->GetXaxis()->SetTitle("X, cm");
    h_DCH2->GetXaxis()->SetTitleColor(kOrange + 10);
    h_DCH2->GetYaxis()->SetTitle("Y, cm");
    h_DCH2->GetYaxis()->SetTitleColor(kOrange + 10);
    name = fTitle + "CanvasWires";
    canWires = new TCanvas(name, name, PAD_WIDTH * PLANE_ROWS, PAD_HEIGHT * PLANE_COLS);
    canWires->Divide(PLANE_ROWS, PLANE_COLS);
    canWires->SetGrid();
    canWiresPads.resize(PLANE_ROWS * PLANE_COLS);
    name = fTitle + "CanvasTimes";
    canTimes = new TCanvas(name, name, PAD_WIDTH * PLANE_ROWS, PAD_HEIGHT * PLANE_COLS);
    canTimes->Divide(PLANE_ROWS, PLANE_COLS);
    canTimesPads.resize(PLANE_ROWS * PLANE_COLS);
    for (Int_t rowIndex = 0; rowIndex < PLANE_ROWS; rowIndex++) {
        for (Int_t colIndex = 0; colIndex < PLANE_COLS; colIndex++) {
            PadInfo *p = new PadInfo();
            p->current = h_wires[rowIndex * PLANE_COLS + colIndex];
            canWiresPads[rowIndex * PLANE_ROWS + colIndex] = p;
            PadInfo *pt = new PadInfo();
            pt->current = h_times[rowIndex * PLANE_COLS + colIndex];
            canTimesPads[rowIndex * PLANE_COLS + colIndex] = pt;
            canWires->GetPad(rowIndex * PLANE_COLS + colIndex + 1)->SetGrid();
            canTimes->GetPad(rowIndex * PLANE_COLS + colIndex + 1)->SetGrid();
        }
    }
}

BmnHistDch::~BmnHistDch() {
    for (Int_t i = 0; i < kNPLANES; ++i){
        delete h_wires[i];
        delete h_times[i];
    }
    delete fDchHits;
    delete h_DCH1;
    delete h_DCH2;
}

void BmnHistDch::Register(THttpServer *serv) {
    fServer = serv;
    fServer->Register("/", this);
    TString path = "/" + fTitle + "/";
    fServer->Register(path, canWires);
    fServer->Register(path, canTimes);
    for (Int_t i = 0; i < kNPLANES; ++i){
        fServer->Register(path, h_wires[i]);
        fServer->Register(path, h_times[i]);
    }
    fServer->Register(path, h_DCH1);
    fServer->Register(path, h_DCH2);
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

void BmnHistDch::SetDir(TFile *outFile = NULL, TTree *recoTree = NULL) {
    frecoTree = recoTree;
    TDirectory *dir = NULL;
    if (outFile != NULL)
        dir = outFile->mkdir(fTitle + "_hists");
    for (Int_t i = 0; i < kNPLANES; ++i){
        h_wires[i]->SetDirectory(dir);
        h_times[i]->SetDirectory(dir);
    }
    h_DCH1->SetDirectory(dir);
    h_DCH2->SetDirectory(dir);

}

void BmnHistDch::DrawBoth() {
    BmnHist::DrawRef(canWires, &canWiresPads);
    BmnHist::DrawRef(canTimes, &canTimesPads);
}

void BmnHistDch::FillFromDigi(TClonesArray * DchDigits) {
    //    Int_t rid = (head) ? head->GetRunId() : -1;
    fDchHits->Clear();
    ProcessDchDigits(DchDigits, fDchHits);
    for (Int_t iDig = 0; iDig < DchDigits->GetEntriesFast(); ++iDig) {
        BmnDchDigit* dig = (BmnDchDigit*) DchDigits->At(iDig);
        Int_t plane = dig->GetPlane();
        h_wires[plane]->Fill(dig->GetWireNumber());
        h_times[plane]->Fill(dig->GetTime());
    }
    for (Int_t iHit = 0; iHit < fDchHits->GetEntriesFast(); iHit++) {
        BmnDchHit* hit = (BmnDchHit*) fDchHits->At(iHit);
        if (hit->GetDchId() == 1) h_DCH1->Fill(hit->GetX(), hit->GetY());
        if (hit->GetDchId() == 2) h_DCH2->Fill(hit->GetX(), hit->GetY());
    }
}

BmnStatus BmnHistDch::LoadRefRun(TString FileName) {
    printf("Loading ref histos\n");
    refFile = new TFile(refPath + FileName, "read");
    if (refFile->IsOpen() == false) {
        printf("Cannot open file %s !\n", FileName.Data());
        return kBMNERROR;
    }
    Int_t iPad = 0;
    TString refName = Form("ref%06d_", refID);
    TString name;
    for (Int_t rowIndex = 0; rowIndex < PLANE_ROWS; rowIndex++) {
        for (Int_t colIndex = 0; colIndex < PLANE_COLS; colIndex++) {
            iPad = rowIndex * PLANE_ROWS + colIndex;
            delete canWiresPads[iPad]->ref;
            canWiresPads[iPad]->ref = NULL;
            TH1F* tempH = NULL;
            name = fTitle + "_" + names[iPad];
            tempH = (TH1F*) refFile->Get(refName + "DCH_hists/" + refName + name);
            if (tempH == NULL) {
                tempH = (TH1F*) refFile->Get(TString("DCH_hists/") + name);
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
            delete canTimesPads[iPad]->ref;
            canTimesPads[iPad]->ref = NULL;
            tempH = NULL;
            name = fTitle + "_" + names[iPad] + "_Time";
            tempH = (TH1F*) refFile->Get(refName + "DCH_hists/" + refName + name);
            if (tempH == NULL) {
                tempH = (TH1F*) refFile->Get(TString("DCH_hists/") + name);
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

BmnStatus BmnHistDch::SetRefRun(Int_t id) {
    TString FileName = Form("bmn_run%04d_hist.root", id);
    printf("SetRefRun: %s\n", FileName.Data());
    if (refRunName != FileName) {
        refRunName = FileName;
        refID = id;
        LoadRefRun(refRunName);
    }
}

void BmnHistDch::Reset() {
    for (Int_t i = 0; i < kNPLANES; ++i){
        h_wires[i]->Reset();
        h_times[i]->Reset();
    }
    h_DCH1->Reset();
    h_DCH2->Reset();
}


ClassImp(BmnHistDch);
