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


const Float_t angles[kNPLANES] = {
    135, 135, 45, 45, 90, 90, 180, 180,
    135, 135, 45, 45, 90, 90, 180, 180
};
const TString names[kNPLANES] = {
    "VA_1", "VB_1", "UA_1", "UB_1", "YA_1", "YB_1", "XA_1", "XB_1",
    "VA_2", "VB_2", "UA_2", "UB_2", "YA_2", "YB_2", "XA_2", "XB_2"
};
Double_t R[Number] = {0.00, 0.00, 1.00, 1.00};
Double_t G[Number] = {0.00, 1.00, 0.65, 0.00};
Double_t B[Number] = {1.00, 0.00, 0.00, 0.00};
Double_t Length[Number] = {0.0, 0.33, 0.66, 1.0};

BmnHistDch::BmnHistDch(TString title = "DCH") {
    fTitle = title;
    fName = title + "_cl";
}

BmnHistDch::~BmnHistDch() {
    for (Int_t i = 0; i < kNPLANES; ++i)
        fServer->Unregister(h_wires[i]);
    fServer->Unregister(this);
}

void BmnHistDch::Register(THttpServer *serv) {
    fServer = serv;
    fServer->Register("/", this);
    TString name;
    TString path = "/" + fTitle + "/";
    Int_t FI = TColor::CreateGradientColorTable(Number, Length, R, G, B, nb);
    for (Int_t i = 0; i < nb; ++i) {
        myPalette[i] = FI + i;
    }
    for (Int_t i = 0; i < kNPLANES; ++i) {
        h_wires[i] = new TH1F(names[i], names[i], kNREALWIRES, 0, kNREALWIRES);
        fServer->Register(path, h_wires[i]);

    }
    //    TCanvas* chmb1 = new TCanvas("DCH_1", "DCH_1", 1000, 1000);
    TString cmd = "/" + fName + "/->Reset()";
    fServer->SetItemField(path.Data(), "_monitoring", "2000");
    fServer->RegisterCommand((path + "Reset").Data(), cmd.Data(), "button;");
}

void BmnHistDch::SetDir(TFile *outFile = NULL, TTree *recoTree = NULL) {
    frecoTree = recoTree;
    if (outFile != NULL) {
        TDirectory *dir = outFile->mkdir(fTitle + "_hists");
        dir->cd();
        for (Int_t i = 0; i < kNPLANES; ++i)
            h_wires[i]->SetDirectory(dir);
    }

}

void BmnHistDch::FillFromDigi(TClonesArray * DchDigits) {
    for (Int_t iDig = 0; iDig < DchDigits->GetEntriesFast(); ++iDig) {
        BmnDchDigit* dig = (BmnDchDigit*) DchDigits->At(iDig);
        Int_t plane = dig->GetPlane();
        Int_t wire = dig->GetWireNumber();
        //        v_wires[plane][wire] += 1;
        //        if (wire > kNREALWIRES - 1) {
        //            wire -= 128; //8 * 16 last preamplifier setup behind hole, so move signal in correct place
        //        }
        h_wires[plane]->Fill(wire);
    }
}

void BmnHistDch::Reset() {
    for (Int_t i = 0; i < kNPLANES; ++i)
        h_wires[i]->Reset();
}


ClassImp(BmnHistDch);
