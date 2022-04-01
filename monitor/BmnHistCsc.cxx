/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   BmnMonHists.cxx
 * Author: ilnur
 *
 * Created on November 15, 2016, 12:57 PM
 */

#include <algorithm>
#include <numeric>

#include "BmnHistCsc.h"
#include "BmnRawDataDecoder.h"

BmnHistCsc::BmnHistCsc(TString title, TString path, Int_t periodID, BmnSetup setup) : BmnHist(periodID, setup) {
    sumMods = 0;
    maxLayers = 0;
    refPath = path;
    fTitle = title;
    fName = title + "_cl";
    TString name;
    cscStationSet =  BmnAdcProcessor::GetCSCStationSet(periodID, fSetup);
    for (Int_t iStation = 0; iStation < cscStationSet->GetNStations(); iStation++) {
        vector<vector<TH1F*> > rowCSC;
        BmnCSCStation * st = cscStationSet->GetStation(iStation);
        sumMods += st->GetNModules();
        for (Int_t iModule = 0; iModule < st->GetNModules(); iModule++) {
            vector<TH1F*> colCSC;
            BmnCSCModule *mod = st->GetModule(iModule);
            if (maxLayers < mod->GetNStripLayers())
                maxLayers = mod->GetNStripLayers();
            for (Int_t iLayer = 0; iLayer < mod->GetNStripLayers(); iLayer++) {
                BmnCSCLayer lay = mod->GetStripLayer( iLayer);
                name = Form(fTitle + "_Station_%d_module_%d_layer_%d", iStation, iModule, iLayer);
                TH1F *h = new TH1F(name, name, lay.GetNStrips(), 0, lay.GetNStrips());
                h->SetTitleSize(0.06, "XY");
                h->SetLabelSize(0.08, "XY");
                h->GetXaxis()->SetTitle("Strip Number");
                h->GetXaxis()->SetTitleColor(kOrange + 10);
                h->GetYaxis()->SetTitle("Activation Count");
                h->GetYaxis()->SetTitleColor(kOrange + 10);
                h->GetYaxis()->SetTitleOffset(1.0);
                colCSC.push_back(h);

            }
            rowCSC.push_back(colCSC);

        }
        histCscStrip.push_back(rowCSC);

    }
//    sumMods = 2;
//    maxLayers = 2;
    // Create canvas
    name = fTitle + "Canvas";
    canCscStrip = new TCanvas(name, name, PAD_WIDTH * maxLayers, PAD_HEIGHT * sumMods);
    canCscStrip->Divide(maxLayers, sumMods);
    Int_t modCtr = 0; // filling CSC Canvas' pads
    canStripPads.resize(sumMods * maxLayers);
    Names.resize(sumMods * maxLayers);

    for (Int_t iStation = 0; iStation < cscStationSet->GetNStations(); iStation++) {
        BmnCSCStation * st = cscStationSet->GetStation(iStation);
        for (Int_t iModule = 0; iModule < st->GetNModules(); iModule++) {
            BmnCSCModule *mod = st->GetModule(iModule);
            for (Int_t iLayer = 0; iLayer < mod->GetNStripLayers(); iLayer++) {
                PadInfo *p = new PadInfo();
                p->opt = "";
                p->current = histCscStrip[iStation][iModule][iLayer];
                Int_t iPad = modCtr * maxLayers + iLayer;
                canStripPads[iPad] = p;
                canCscStrip->GetPad(iPad + 1)->SetGrid();
                Names[iPad] = canStripPads[iPad]->current->GetName();
            }
            modCtr++;
        }
    }
}

BmnHistCsc::~BmnHistCsc() {
    delete cscStationSet;
    delete canCscStrip;
    if (fDir)
        return;
    for (auto pad : canStripPads)
        delete pad;
}

void BmnHistCsc::Register(THttpServer * serv) {
    fServer = serv;
    fServer->Register("/", this);
    TString path = "/" + fTitle + "/";
    fServer->Register(path, canCscStrip);
    fServer->SetItemField(path, "_monitoring", "2000");
    fServer->SetItemField(path, "_layout", "grid3x3");
    TString cmd = "/" + fName + "/->Reset()";
    TString cmdTitle = path + "Reset";
//    fServer->RegisterCommand(cmdTitle.Data(), cmd.Data(), "button;");
    fServer->Restrict(cmdTitle.Data(), "visible=shift");
    fServer->Restrict(cmdTitle.Data(), "allow=shift");
    fServer->Restrict(cmdTitle.Data(), "deny=guest");
    cmd = "/" + fName + "/->SetRefRun(%arg1%)";
    cmdTitle = path + "SetRefRun";
    fServer->RegisterCommand(cmdTitle.Data(), cmd.Data(), "button;");
    //    fServer->Restrict(cmdTitle.Data(), "deny=guest");

}

void BmnHistCsc::SetDir(TFile *outFile, TTree * recoTree) {
    frecoTree = recoTree;
    fDir = NULL;
    if (outFile != NULL)
        fDir = outFile->mkdir(fTitle + "_hists");
    for (auto row : histCscStrip)
        for (auto col : row)
            for (auto el : col)
                el->SetDirectory(fDir);
}

void BmnHistCsc::DrawBoth() {
    BmnHist::DrawRef(canCscStrip, &canStripPads);
}

void BmnHistCsc::FillFromDigi(DigiArrays *fDigiArrays){
    TClonesArray * cscDigits = fDigiArrays->csc;
    if (!cscDigits)
        return;
    for (Int_t digIndex = 0; digIndex < cscDigits->GetEntriesFast(); digIndex++) {
        BmnCSCDigit* gs = (BmnCSCDigit*) cscDigits->At(digIndex);//!!!
        Int_t module = gs->GetModule();
        Int_t station = gs->GetStation();
        Int_t layer = gs->GetStripLayer();
        Int_t cscStrip = gs->GetStripNumber();
        histCscStrip[station][module][layer]->Fill(cscStrip);
    }
}

BmnStatus BmnHistCsc::SetRefRun(Int_t id) {
    if (refID != id) {
        TString FileName = Form("bmn_run%04d_hist.root", id);
        printf("SetRefRun: %s\n", FileName.Data());
        refRunName = FileName;
        refID = id;
        BmnHist::LoadRefRun(refID, refPath + FileName, fTitle, canStripPads, Names);
        DrawBoth();
    }

    return kBMNSUCCESS;
}

void BmnHistCsc::ClearRefRun() {
    for (auto pad : canStripPads){
        if (pad->ref) delete pad->ref;
        pad->ref = NULL;
    }
    refID = 0;
}

void BmnHistCsc::Reset() {
    for (auto row : histCscStrip)
        for (auto col : row)
            for (auto el : col)
                el->Reset();
}


ClassImp(BmnHistCsc);

