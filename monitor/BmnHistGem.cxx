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

const UInt_t moduleCount[GEM_STATIONS_COUNT] = {1, 1, 1, 1, 2, 2, 2};
const UInt_t layersCount[GEM_STATIONS_COUNT] = {2, 4, 4, 4, 4, 4, 4};
const UInt_t nStrips[GEM_STATIONS_COUNT] = {256, 825, 825, 825, 825, 825, 1019};
#define MAX_STRIPS 1020

BmnHistGem::BmnHistGem(TString title, Bool_t createNoiseMask) {
    fTitle = title;
    fName = title + "_cl";
    cout << "Constructor " << fTitle << endl;
    TString name;
    for (Int_t stationIndex = 0; stationIndex < GEM_STATIONS_COUNT; stationIndex++) {
        vector<vector<TH1F*> > rowGEM;
        for (Int_t moduleIndex = 0; moduleIndex < moduleCount[stationIndex]; moduleIndex++) {
            vector<TH1F*> colGEM;
            for (Int_t layerIndex = 0; layerIndex < layersCount[stationIndex]; layerIndex++) {
                name = Form(fTitle + "_Station_%d_module_%d_layer_%d", stationIndex, moduleIndex, layerIndex);
                TH1F *h = new TH1F(name, name, nStrips[stationIndex], 0, nStrips[stationIndex]);
                colGEM.push_back(h);
            }
            rowGEM.push_back(colGEM);
        }
        histGemStrip.push_back(rowGEM);
    }
    for (Int_t stationIndex = 0; stationIndex < GEM_STATIONS_COUNT; stationIndex++) {
        vector<vector<Int_t*> > rowGEM;
        for (Int_t moduleIndex = 0; moduleIndex < moduleCount[stationIndex]; moduleIndex++) {
            vector<Int_t*> colGEM;
            for (Int_t layerIndex = 0; layerIndex < layersCount[stationIndex]; layerIndex++) {
                Int_t* m = new Int_t[MAX_STRIPS];
                colGEM.push_back(m);
                for (Int_t im = 0; im < MAX_STRIPS; im++)
                    m[im] = 1;
            }
            rowGEM.push_back(colGEM);
        }
        maskGemStrip.push_back(rowGEM);
    }
}

BmnHistGem::~BmnHistGem() {
    for (auto row : maskGemStrip)
        for (auto col : row)
            for (auto el : col)
                delete[] el;
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
    TString cmdTitle = path + "Reset";
   fServer->SetItemField(path, "_monitoring","2000");
   fServer->SetItemField(path, "_layout","grid3x3");
   TString examples = TString("[") +
           histGemStrip[0][0][0]->GetTitle() + TString(",") +
           histGemStrip[0][0][1]->GetTitle() + TString(",") +
           histGemStrip[6][1][0]->GetTitle() + TString(",") +
           histGemStrip[6][0][0]->GetTitle() + TString("]");
   fServer->SetItemField(path,"_drawitem", examples);
   fServer->RegisterCommand(cmdTitle.Data(), cmd.Data(), "button;");
   fServer->Restrict(cmdTitle.Data(), "visible=admin");
   fServer->Restrict(cmdTitle.Data(), "allow=admin");
    fServer->Restrict(cmdTitle.Data(), "deny=guest");
    
}

void BmnHistGem::SetDir(TFile *outFile, TTree *recoTree){
    frecoTree = recoTree;
    TDirectory *dir = NULL;
    if (outFile != NULL)
        dir = outFile->mkdir(fTitle + "_hists");
    //        dir->cd();
    for (auto row : histGemStrip)
        for (auto col : row)
            for (auto el : col)
                el->SetDirectory(dir); 
}


void BmnHistGem::FillFromDigi(TClonesArray * gemDigits) {
    for (Int_t digIndex = 0; digIndex < gemDigits->GetEntriesFast(); digIndex++) {
        BmnGemStripDigit* gs = (BmnGemStripDigit*) gemDigits->At(digIndex);
        Int_t module = gs->GetModule();
        Int_t station = gs->GetStation();
        Int_t layer = gs->GetStripLayer();
        Int_t gemStrip = gs->GetStripNumber();
        histGemStrip[station][module][layer]->Fill(gemStrip);
    }
}

void BmnHistGem::FillFromDigiMasked(TClonesArray * gemDigits, vector<vector<vector<TH1F*> > >* hist0, Double_t threshold, BmnEventHeader * head) {//vector<vector<vector<Int_t*> > >* mask) {
    for (Int_t digIndex = 0; digIndex < gemDigits->GetEntriesFast(); digIndex++) {
        BmnGemStripDigit* gs = (BmnGemStripDigit*) gemDigits->At(digIndex);
        Int_t module = gs->GetModule();
        Int_t station = gs->GetStation();
        Int_t layer = gs->GetStripLayer();
        Int_t gemStrip = gs->GetStripNumber();
//        cout << "station " << station << " module " << module << " layer "<< layer << " strip " << gemStrip<<endl;
//        cout << histGemStrip[station][module][layer] << endl;
//        cout << histGemStrip[station][module][layer]->GetTitle() << endl;
//        cout << " hist0 getbincontent " << (*hist0)[station][module][layer]->GetBinContent(gemStrip) << endl;
//        cout << " hist getbincontent " << histGemStrip[station][module][layer]->GetBinContent(gemStrip) << endl;
        if ((*hist0)[station][module][layer]->GetBinContent((*hist0)[station][module][layer]->FindBin(gemStrip)) <= threshold * 0.9)
            histGemStrip[station][module][layer]->AddBinContent(histGemStrip[station][module][layer]->FindBin(gemStrip));
//            histGemStrip[station][module][layer]->Fill(gemStrip);
                TString name = Form(fTitle + "_Station_%d_module_%d_layer_%d_Run%d", station, module, layer, head->GetRunId());
        histGemStrip[station][module][layer]->SetTitle(name);
    }
}

void BmnHistGem::UpdateNoiseMask(Double_t threshold) {
    for (Int_t iStation = 0; iStation < GEM_STATIONS_COUNT; iStation++)
        for (Int_t iModule = 0; iModule < moduleCount[iStation]; iModule++)
            for (Int_t iLayer = 0; iLayer < layersCount[iStation]; iLayer++)
                for (Int_t iStrip = 0; iStrip < histGemStrip[iStation][iModule][iLayer]->GetNbinsX() + 1; iStrip++){
                    maskGemStrip[iStation][iModule][iLayer][iStrip] = 0;
//                    cout << " MASK " << maskGemStrip[iStation][iModule][iLayer][iStrip] << endl;
//                            (histGemStrip[iStation][iModule][iLayer]->GetBinContent(iStrip) > threshold) ? 0 : 0;
                }
}

void BmnHistGem::ApplyNoiseMask(vector<vector<vector<TH1F*> > >* hist0, Double_t threshold) {
//    auto m
//    cout << " mask " << (*mask)[0][0][0][199] << endl;
    for (Int_t iStation = 0; iStation < GEM_STATIONS_COUNT; iStation++)
        for (Int_t iModule = 0; iModule < moduleCount[iStation]; iModule++)
            for (Int_t iLayer = 0; iLayer < layersCount[iStation]; iLayer++)
                for (Int_t iStrip = 0; iStrip < histGemStrip[iStation][iModule][iLayer]->GetNbinsX() + 1; iStrip++)
                    if ((*hist0)[iStation][iModule][iLayer]->GetBinContent(iStrip) > threshold)
                    histGemStrip[iStation][iModule][iLayer]->SetBinContent(iStrip, 0);
}

void BmnHistGem::Reset() {
    for (auto row : histGemStrip)
        for (auto col : row)
            for (auto el : col)
                el->Reset();
    for (auto row : maskGemStrip)
        for (auto col : row)
            for (auto el : col)
                for (Int_t im = 0; im < MAX_STRIPS; im++)
                    el[im] = 1;
}


ClassImp(BmnHistGem);

