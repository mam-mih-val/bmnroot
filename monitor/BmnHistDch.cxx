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
//Double_t R[Number] = {0.00, 0.00, 1.00, 1.00};
//Double_t G[Number] = {0.00, 1.00, 0.65, 0.00};
//Double_t B[Number] = {1.00, 0.00, 0.00, 0.00};
//Double_t Length[Number] = {0.0, 0.33, 0.66, 1.0};

BmnHistDch::BmnHistDch(TString title = "DCH") {
    fTitle = title;
    fName = title + "_cl";
    for (Int_t i = 0; i < kNPLANES; ++i) {
        h_wires[i] = new TH1F(fTitle + "_" + names[i], names[i], kNREALWIRES, 0, kNREALWIRES);
        h_wires[i]->SetTitleSize(0.06, "XY");
        h_wires[i]->SetLabelSize(0.08, "XY");
        h_wires[i]->GetXaxis()->SetTitle("Wire Number");
        h_wires[i]->GetXaxis()->SetTitleColor(kOrange + 5);
        h_wires[i]->GetYaxis()->SetTitle("Activation Count");
        h_wires[i]->GetYaxis()->SetTitleColor(kOrange + 5);
    }
    //    Int_t FI = TColor::CreateGradientColorTable(Number, Length, R, G, B, nb);
    //    for (Int_t i = 0; i < nb; ++i) {
    //        myPalette[i] = FI + i;
    //    }
    TString name;
    fDchHits = new TClonesArray("BmnDchHit");
    //    h_DCH1 = new TH2F("h_DCH1", "DCH #1", 500, 0, 0, 500, 0, 0);
    //    h_DCH2 = new TH2F("h_DCH2", "DCH #2", 500, 0, 0, 500, 0, 0);
    //    h_DCH1 = new TH2F("h_DCH1", "DCH #1", kNWIRES, -kNWIRES/2, kNWIRES/2, kNWIRES, -kNWIRES/2, kNWIRES/2);
    //    h_DCH2 = new TH2F("h_DCH2", "DCH #2", kNWIRES, -kNWIRES/2, kNWIRES/2, kNWIRES, -kNWIRES/2, kNWIRES/2);
    name = fTitle + "_h_DCH1";
    h_DCH1 = new TH2F(name, "DCH #1", 500, -DCH_WDTH, DCH_WDTH, 500, -DCH_WDTH, DCH_WDTH);
    h_DCH1->GetXaxis()->SetTitle("X, cm");
    h_DCH1->GetXaxis()->SetTitleColor(kOrange + 5);
    h_DCH1->GetYaxis()->SetTitle("Y, cm");
    h_DCH1->GetYaxis()->SetTitleColor(kOrange + 5);
    name = fTitle + "_h_DCH2";
    h_DCH2 = new TH2F(name, "DCH #2", 500, -DCH_WDTH, DCH_WDTH, 500, -DCH_WDTH, DCH_WDTH);
    h_DCH2->GetXaxis()->SetTitle("X, cm");
    h_DCH2->GetXaxis()->SetTitleColor(kOrange + 5);
    h_DCH2->GetYaxis()->SetTitle("Y, cm");
    h_DCH2->GetYaxis()->SetTitleColor(kOrange + 5);
}

BmnHistDch::~BmnHistDch() {
    for (Int_t i = 0; i < kNPLANES; ++i) {
        fServer->Unregister(h_wires[i]);
        delete h_wires[i];
    }
    delete fDchHits;
    delete h_DCH1;
    delete h_DCH2;
    fServer->Unregister(this);
}

void BmnHistDch::Register(THttpServer *serv) {
    fServer = serv;
    fServer->Register("/", this);
    TString path = "/" + fTitle + "/";
    for (Int_t i = 0; i < kNPLANES; ++i)
        fServer->Register(path, h_wires[i]);
    fServer->Register(path, h_DCH1);
    fServer->Register(path, h_DCH2);
    TString cmd = "/" + fName + "/->Reset()";
    fServer->SetItemField(path.Data(), "_monitoring", "2000");
    fServer->SetItemField(path.Data(), "_layout", "grid3x3");
    TString cmdTitle = path + "Reset";
    fServer->RegisterCommand(cmdTitle.Data(), cmd.Data(), "button;");
    fServer->Restrict(cmdTitle, "visible=shift");
    fServer->Restrict(cmdTitle, "allow=shift");
}

void BmnHistDch::SetDir(TFile *outFile = NULL, TTree *recoTree = NULL) {
    frecoTree = recoTree;
    TDirectory *dir = NULL;
    if (outFile != NULL)
        dir = outFile->mkdir(fTitle + "_hists");
    for (Int_t i = 0; i < kNPLANES; ++i)
        h_wires[i]->SetDirectory(dir);
    h_DCH1->SetDirectory(dir);
    h_DCH2->SetDirectory(dir);

}

void BmnHistDch::FillFromDigi(TClonesArray * DchDigits) {
    //    Int_t rid = (head) ? head->GetRunId() : -1;
    fDchHits->Clear();
    ProcessDchDigits(DchDigits, fDchHits);
    for (Int_t iDig = 0; iDig < DchDigits->GetEntriesFast(); ++iDig) {
        BmnDchDigit* dig = (BmnDchDigit*) DchDigits->At(iDig);
        Int_t plane = dig->GetPlane();
        Int_t wire = dig->GetWireNumber();
        //        h_wires[plane]->SetTitle(names[plane] + Form("_runID_%d_eventID_%d", rid, iEv));
        h_wires[plane]->Fill(wire);
    }
    for (Int_t iHit = 0; iHit < fDchHits->GetEntriesFast(); iHit++) {
        BmnDchHit* hit = (BmnDchHit*) fDchHits->At(iHit);
        if (hit->GetDchId() == 1) h_DCH1->Fill(hit->GetX(), hit->GetY());
        if (hit->GetDchId() == 2) h_DCH2->Fill(hit->GetX(), hit->GetY());
    }
}

void BmnHistDch::Reset() {
    for (Int_t i = 0; i < kNPLANES; ++i)
        h_wires[i]->Reset();
    h_DCH1->Reset();
    h_DCH2->Reset();
}


ClassImp(BmnHistDch);
