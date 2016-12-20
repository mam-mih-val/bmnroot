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
        h_wires[i]->GetXaxis()->SetLabelSize(0.1);
        h_wires[i]->GetYaxis()->SetLabelSize(0.1);
    }
    TString name;
    MwpcHits = new TClonesArray("BmnMwpcHit");
    name = fTitle + "_h_MWPC1";
    h_MWPC1 = new TH2F(name, "MWPC #1", 200, -20, 20, 200, -20, 20);
    name = fTitle + "_h_MWPC2";
    h_MWPC2 = new TH2F(name, "MWPC #2", 200, -20, 20, 200, -20, 20);
    //    Int_t FI = TColor::CreateGradientColorTable(Number, Length, R, G, B, nb);
    //    for (Int_t i = 0; i < nb; ++i) {
    //        myPalette[i] = FI + i;
    //    }
}

BmnHistMwpc::~BmnHistMwpc() {
    for (Int_t i = 0; i < MWPC_PLANES; ++i){
        fServer->Unregister(h_wires[i]);
        delete h_wires[i];
    }
    fServer->Unregister(h_MWPC1);
    fServer->Unregister(h_MWPC2);
    delete MwpcHits;
    delete h_MWPC1;
    delete h_MWPC2;
}

void BmnHistMwpc::Register(THttpServer *serv) {
    fServer = serv;
    fServer->Register("/", this);
    TString path = "/" + fTitle + "/";
    for (Int_t i = 0; i < MWPC_PLANES; ++i)
        fServer->Register(path, h_wires[i]);
    fServer->Register(path, h_MWPC1);
    fServer->Register(path, h_MWPC2);
    TString cmd = "/" + fName + "/->Reset()";
    fServer->SetItemField(path.Data(), "_monitoring", "2000");
    fServer->SetItemField(path.Data(), "_layout", "grid3x3");
    TString cmdTitle = path + "Reset";
    fServer->RegisterCommand(cmdTitle.Data(), cmd.Data(), "button;");
    fServer->Restrict(cmdTitle, "visible=shift");
    fServer->Restrict(cmdTitle, "allow=shift");
}

void BmnHistMwpc::SetDir(TFile *outFile = NULL, TTree *recoTree = NULL) {
    frecoTree = recoTree;
    TDirectory *dir = NULL;
    if (outFile != NULL)
        dir = outFile->mkdir(fTitle + "_hists");
    for (Int_t i = 0; i < MWPC_PLANES; ++i)
        h_wires[i]->SetDirectory(dir);
    h_MWPC1->SetDirectory(dir);
    h_MWPC2->SetDirectory(dir);

}

void BmnHistMwpc::FillFromDigi(TClonesArray * MwpcDigits, BmnEventHeader * head) {
    Int_t rid = (head) ? head->GetRunId() : -1;
    Int_t iEv = head->GetEventId();
    MwpcHits->Clear();
    ProcessMwpcDigits(MwpcDigits, MwpcHits);
    for (Int_t iDig = 0; iDig < MwpcDigits->GetEntriesFast(); ++iDig) {
        BmnMwpcDigit* dig = (BmnMwpcDigit*) MwpcDigits->At(iDig);
        Int_t plane = dig->GetPlane();
        Int_t wire = dig->GetWireNumber();
        //        v_wires[plane][wire] += 1;
        //        if (wire > kNREALWIRES - 1) {
        //            wire -= 128; //8 * 16 last preamplifier setup behind hole, so move signal in correct place
        //        }

        h_wires[plane]->SetTitle(Form("%d_runID_%d_eventID_%d", plane, rid, iEv));
        h_wires[plane]->Fill(wire);
    }
    for (Int_t iHit = 0; iHit < MwpcHits->GetEntriesFast(); ++iHit) {
        BmnMwpcHit* hit = (BmnMwpcHit*) MwpcHits->At(iHit);
        if (hit->GetMwpcId() == 0) h_MWPC1->Fill(hit->GetX(), hit->GetY());
        if (hit->GetMwpcId() == 1) h_MWPC2->Fill(hit->GetX(), hit->GetY());
    }
}

void BmnHistMwpc::Reset() {
    for (Int_t i = 0; i < MWPC_PLANES; ++i)
        h_wires[i]->Reset();
    h_MWPC1->Reset();
    h_MWPC2->Reset();
}



ClassImp(BmnHistMwpc);