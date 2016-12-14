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

BmnHistMwpc::BmnHistMwpc(TString title = "MWPC") {
    fTitle = title;
    fName = title + "_cl";
    for (Int_t i = 0; i < MWPC_PLANES; ++i)
        h_wires[i] = new TH1F(fTitle + "_" + Form("Plane_%d", i), Form("Plane_%d", i), MWPC_WIRES, 0, MWPC_WIRES);
//    Int_t FI = TColor::CreateGradientColorTable(Number, Length, R, G, B, nb);
//    for (Int_t i = 0; i < nb; ++i) {
//        myPalette[i] = FI + i;
//    }
}


BmnHistMwpc::~BmnHistMwpc() {
}
void BmnHistMwpc::Register(THttpServer *serv) {
    fServer = serv;
    fServer->Register("/", this);
    TString path = "/" + fTitle + "/";
    for (Int_t i = 0; i < MWPC_PLANES; ++i)
        fServer->Register(path, h_wires[i]);
    TString cmd = "/" + fName + "/->Reset()";
    fServer->SetItemField(path.Data(), "_monitoring", "2000");
    fServer->SetItemField(path.Data(), "_layout", "grid3x3");
    TString cmdTitle = path + "Reset";
    fServer->RegisterCommand(cmdTitle.Data(), cmd.Data(), "button;");
    fServer->Restrict(cmdTitle, "visible=admin");
    fServer->Restrict(cmdTitle, "allow=admin");
}

void BmnHistMwpc::SetDir(TFile *outFile = NULL, TTree *recoTree = NULL) {
    frecoTree = recoTree;
    TDirectory *dir = NULL;
    if (outFile != NULL)
        dir = outFile->mkdir(fTitle + "_hists");
    for (Int_t i = 0; i < MWPC_PLANES; ++i)
        h_wires[i]->SetDirectory(dir);

}

void BmnHistMwpc::FillFromDigi(TClonesArray * DchDigits, BmnEventHeader * head) {
    Int_t rid = (head) ? head->GetRunId() : -1;
    Int_t iEv  = head->GetEventId();
    for (Int_t iDig = 0; iDig < DchDigits->GetEntriesFast(); ++iDig) {
        BmnMwpcDigit* dig = (BmnMwpcDigit*) DchDigits->At(iDig);
        Int_t plane = dig->GetPlane();
        Int_t wire = dig->GetWireNumber();
        //        v_wires[plane][wire] += 1;
        //        if (wire > kNREALWIRES - 1) {
        //            wire -= 128; //8 * 16 last preamplifier setup behind hole, so move signal in correct place
        //        }
        
        h_wires[plane]->SetTitle(Form("%d_runID_%d_eventID_%d", plane, rid, iEv));
        h_wires[plane]->Fill(wire);
    }
}

void BmnHistMwpc::Reset() {
    for (Int_t i = 0; i < MWPC_PLANES; ++i)
        h_wires[i]->Reset();
}



ClassImp(BmnHistMwpc);