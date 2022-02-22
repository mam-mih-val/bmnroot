/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   BmnHistEcal.cxx
 * Author: ilnur
 * 
 * Created on March 17, 2017, 2:48 PM
 */

#include "TGaxis.h"

#include "BmnHistECAL.h"

BmnHistECAL::BmnHistECAL(TString title, TString path) : BmnHist() {
    fTitle = title;
    fName = title + "_cl";
    TGaxis::SetMaxDigits(1);
    TString name;
    name = fTitle + "_h2d_Grid";
    h2d_grid = new TH2D(name, fTitle + " Grid", 32, 0, 32, 32, 0, 32);
    h2d_grid->GetXaxis()->SetTitle("iX");
    h2d_grid->GetXaxis()->SetTitleColor(kOrange + 10);
    h2d_grid->GetYaxis()->SetTitle("iY");
    h2d_grid->GetYaxis()->SetTitleColor(kOrange + 10);
    h2d_grid->GetZaxis()->SetNoExponent(kTRUE);
    name = fTitle + "_h2d_Profile";
    h2d_profile = new TH2D(name, fTitle + " Profile", 1200, 0, 1200, 1200, 0, 1200);
    h2d_profile->GetXaxis()->SetTitle("X");
    h2d_profile->GetXaxis()->SetTitleColor(kOrange + 10);
    h2d_profile->GetYaxis()->SetTitle("Y");
    h2d_profile->GetYaxis()->SetTitleColor(kOrange + 10);
    name = fTitle + "_X_Amplitude";
    hx = new TH1D(name, fTitle + " X Amplitude", 1200, 0, 1200);
    hx->GetYaxis()->SetNoExponent(kTRUE);
    hx->GetYaxis()->SetMoreLogLabels(kFALSE);
    name = fTitle + "_Y_Amplitude";
    hy = new TH1D(name, fTitle + " Y Amplitude", 1200, 0, 1200);
    hy->GetYaxis()->SetNoExponent(kFALSE);
    hx->GetYaxis()->SetMoreLogLabels(kTRUE);
    name = fTitle + "CanvasAmplitudes";
    canAmps = new TCanvas(name, name, PAD_WIDTH * ECAL_ROWS, PAD_HEIGHT * ECAL_COLS);
    canAmps->Divide(ECAL_ROWS, ECAL_COLS);
    canAmpsPads.resize(ECAL_ROWS * ECAL_COLS);
    NamesAmps.resize(ECAL_ROWS * ECAL_COLS);

    PadInfo *p = new PadInfo();
    p->current = hx;
    canAmpsPads[0] = p;
    PadInfo *p1 = new PadInfo();
    p1->current = hy;
    canAmpsPads[1] = p1;

    for (Int_t rowIndex = 0; rowIndex < ECAL_ROWS; rowIndex++) {
        for (Int_t colIndex = 0; colIndex < ECAL_COLS; colIndex++) {
            Int_t iPad = rowIndex * ECAL_COLS + colIndex;
            canAmps->GetPad(iPad + 1)->SetGrid();
            NamesAmps[iPad] = canAmpsPads[iPad]->current->GetName();
            TH1 *h = canAmpsPads[iPad]->current;
            h->SetTitleSize(0.06, "XY");
            h->SetLabelSize(0.08, "XY");
            h->GetXaxis()->SetTitle("Coordinate");
            h->GetXaxis()->SetTitleColor(kOrange + 10);
            h->GetYaxis()->SetTitle("Amplitude");
            h->GetYaxis()->SetTitleColor(kOrange + 10);
            h->GetYaxis()->SetTitleOffset(1.4);
            h->GetYaxis()->SetNoExponent(kFALSE);
        }
    }
}

BmnHistECAL::~BmnHistECAL() {
    delete canAmps;
    if (fDir != NULL)
        return;
    delete h2d_grid;
    delete h2d_profile;
    delete hx;
    delete hy;
    for (auto pad : canAmpsPads)
        delete pad;
}

void BmnHistECAL::Register(THttpServer *serv) {
    fServer = serv;
    fServer->Register("/", this);
    TString path = "/" + fTitle + "/";
    fServer->Register(path, h2d_grid);
    fServer->Register(path, h2d_profile);
    fServer->Register(path, canAmps);
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

void BmnHistECAL::SetDir(TFile *outFile = NULL, TTree *recoTree = NULL) {
    frecoTree = recoTree;
    fDir = NULL;
    if (outFile != NULL)
        fDir = outFile->mkdir(fTitle + "_hists");
    h2d_grid->SetDirectory(fDir);
    h2d_profile->SetDirectory(fDir);
    hx->SetDirectory(fDir);
    hy->SetDirectory(fDir);

}

void BmnHistECAL::DrawBoth() {
    BmnHist::DrawRef(canAmps, &canAmpsPads);
    return;
}

void BmnHistECAL::FillFromDigi(DigiArrays *fDigiArrays) {
    TClonesArray * digits = fDigiArrays->ecal;
    if (!digits)
        return;
    Double_t xAmp = 0;
    Double_t yAmp = 0;
    Double_t Amp = 0;
    for (Int_t iDig = 0; iDig < digits->GetEntriesFast(); iDig++) {
        BmnECALDigit* dig = (BmnECALDigit*) digits->At(iDig);
        Int_t ix = dig->GetIX();
        Int_t iy = dig->GetIY();
        Double_t amp = dig->GetAmp();
        Amp += amp;
        h2d_grid->Fill(ix, iy, dig->GetAmp());
        xAmp += amp * dig->GetX();
        yAmp += amp * dig->GetY();
    }
    if (Amp == 0)
        return;
    xAmp /= Amp;
    yAmp /= Amp;
    h2d_profile->Fill(xAmp, yAmp, Amp);
    hx->Fill(xAmp, Amp);
    hy->Fill(yAmp, Amp);
}

BmnStatus BmnHistECAL::SetRefRun(Int_t id) {
    TString FileName = Form("bmn_run%04d_hist.root", id);
    printf("SetRefRun: %s\n", FileName.Data());
    if (refRunName != FileName) {
        refRunName = FileName;
        refID = id;
        BmnHist::LoadRefRun(refID, refPath + FileName, fTitle, canAmpsPads, NamesAmps);
        DrawBoth();
    }
    return kBMNSUCCESS;
}

void BmnHistECAL::ClearRefRun() {
    for (auto pad : canAmpsPads){
        if (pad->ref) delete pad->ref;
        pad->ref = NULL;
    }
    refID = 0;
}

void BmnHistECAL::Reset() {
    h2d_grid->Reset();
    h2d_profile->Reset();
    hx->Reset();
    hy->Reset();
}

ClassImp(BmnHistECAL);
