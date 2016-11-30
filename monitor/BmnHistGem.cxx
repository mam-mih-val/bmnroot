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

#include <stdexcept>
#include <vector>
#include <root/TH1.h>

#include "BmnHistGem.h"
#include "BmnHistDch.h"

const UInt_t moduleCount[GEM_STATIONS_COUNT] = {1, 1, 1, 1, 1, 1, 2};
const UInt_t nStrips[GEM_STATIONS_COUNT] = {256, 825, 825, 825, 825, 825, 1019};

BmnHistGem::BmnHistGem(TString title = "GEM") {
    fTitle = title;
    fName = title + "_cl";
    TString name;
    for (Int_t stationIndex = 0; stationIndex < GEM_STATIONS_COUNT; stationIndex++) {
        vector<vector<TH1F*> > rowGEM;
        for (Int_t moduleIndex = 0; moduleIndex < moduleCount[stationIndex]; moduleIndex++) {
            vector<TH1F*> colGEM;
            for (Int_t layerIndex = 0; layerIndex < GEM_LAYERS_COUNT; layerIndex++) {
                name = Form("GEM_Station_%d_module_%d_layer_%d", stationIndex, moduleIndex, layerIndex);
                TH1F *h = new TH1F(name, name, 100, 0, nStrips[stationIndex]);
                colGEM.push_back(h);
            }
            rowGEM.push_back(colGEM);
        }
        histGemStrip.push_back(rowGEM);
    }
}

BmnHistGem::BmnHistGem(const BmnHistGem& orig) {
}

BmnHistGem::~BmnHistGem() {
}

void BmnHistGem::Register(THttpServer *serv) {
    fServer = serv;
    fServer->Register("/", this);
    TString path = "/" + fTitle + "/";
        for (auto row : histGemStrip)
            for (auto col : row)
                for (auto el : col)
                fServer->Register(path, el);
    TString cmd = "/" + fName + "/->Reset()";
    fServer->SetItemField(path.Data(), "_monitoring", "2000");
    fServer->RegisterCommand((path + "Reset").Data(), cmd.Data(), "button;");
}

void BmnHistGem::SetDir(TFile *outFile, TTree *recoTree){
    frecoTree = recoTree;
    if (outFile != NULL) {
        TDirectory *dir = outFile->mkdir(fTitle + "_hists");
        //        dir->cd();
        for (auto row : histGemStrip)
            for (auto col : row)
                for (auto el : col)
                    el->SetDirectory(dir);  
    }  
}


void BmnHistGem::FillFromDigi(TClonesArray * gemDigits) {
    for (Int_t digIndex = 0; digIndex < gemDigits->GetEntriesFast(); digIndex++) {
        BmnGemStripDigit* gs = (BmnGemStripDigit*) gemDigits->At(digIndex);
        Int_t module = gs->GetModule();
        Int_t station = gs->GetStation();
        Int_t layer = gs->GetStripLayer();
        Int_t gemStrip = gs->GetStripNumber();
        if (gs->GetStripLayer() != 0) continue;
        histGemStrip[station][module][layer]->Fill(gemStrip);
    }
}

void BmnHistGem::Reset() {
    for (auto row : histGemStrip)
        for (auto col : row)
            for (auto el : col)
                el->Reset();
}


ClassImp(BmnHistGem);

