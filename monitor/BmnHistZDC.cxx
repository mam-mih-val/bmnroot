/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   BmnHistZDC.cxx
 * Author: ilnur
 * 
 * Created on March 17, 2017, 2:48 PM
 */

#include <root/TGaxis.h>

#include "BmnHistZDC.h"

BmnHistZDC::BmnHistZDC(TString title) {
    fTitle = title;
    fName = title + "_cl";
    TGaxis::SetMaxDigits(2);
    TString name;
    name = fTitle + "_h2d_ZDC";
    h2d_ZDC = new TH2F(name, "ZDC grid", 20, 0, 20, 20, 0, 20);
    h2d_ZDC->GetXaxis()->SetTitle("iX");
    h2d_ZDC->GetXaxis()->SetTitleColor(kOrange + 10);
    h2d_ZDC->GetYaxis()->SetTitle("iY");
    h2d_ZDC->GetYaxis()->SetTitleColor(kOrange + 10);
    h2d_ZDC->GetZaxis()->SetNoExponent(kFALSE);
}


BmnHistZDC::~BmnHistZDC() {
    delete h2d_ZDC;
}

void BmnHistZDC::Register(THttpServer *serv) {
    fServer = serv;
    fServer->Register("/", this);
    TString path = "/" + fTitle + "/";
    fServer->Register(path, h2d_ZDC);
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

void BmnHistZDC::SetDir(TFile *outFile = NULL, TTree *recoTree = NULL) {
    frecoTree = recoTree;
    TDirectory *dir = NULL;
    if (outFile != NULL)
        dir = outFile->mkdir(fTitle + "_hists");
    h2d_ZDC->SetDirectory(dir);

}

void BmnHistZDC::DrawBoth() {
//    BmnHist::DrawRef(canWires, &canWiresPads);
    return;
}

void BmnHistZDC::FillFromDigi(TClonesArray * ZDCDigits) {
    for (Int_t iDig = 0; iDig < ZDCDigits->GetEntriesFast(); iDig++) {
        BmnZDCDigit* dig = (BmnZDCDigit*) ZDCDigits->At(iDig);
        Int_t ix = dig->GetIX();
        Int_t iy = dig->GetIY();
        h2d_ZDC->Fill(ix, iy, dig->GetAmp());
    }
}

BmnStatus BmnHistZDC::SetRefRun(Int_t id) {
//    TString FileName = Form("bmn_run%04d_hist.root", id);
//    printf("SetRefRun: %s\n", FileName.Data());
//    if (refRunName != FileName) {
//        refRunName = FileName;
//        refID = id;
//        BmnHist::LoadRefRun(refID, refPath + FileName, fTitle, canTimesPads, NamesTimes);
//        BmnHist::LoadRefRun(refID, refPath + FileName, fTitle, canWiresPads, NamesWires);
//        DrawBoth();
//    }
    return kBMNSUCCESS;
}

void BmnHistZDC::Reset() {
    h2d_ZDC->Reset();
}

ClassImp(BmnHistZDC);