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
#include <root/TNamed.h>

#include "BmnHistGem.h"

const UInt_t moduleCount[GEM_STATIONS_COUNT] = {1, 1, 1, 1, 2, 2, 2};
const UInt_t layersCount[GEM_STATIONS_COUNT] = {2, 4, 4, 4, 4, 4, 4};
const UInt_t nStrips[GEM_STATIONS_COUNT] = {256, 825, 825, 825, 825, 1100, 1119};
#define MAX_STRIPS 1020

BmnHistGem::BmnHistGem(TString title, TString path, Bool_t createNoiseMask) : BmnHist() {
    refPath = path;
    fTitle = title;
    fName = title + "_cl";
    TString name;
    // Create canvas
    sumMods = accumulate(begin(moduleCount), end(moduleCount), 0);
    maxLayers = *max_element(begin(layersCount), end(layersCount));
    name = fTitle + "Canvas";
    canGemStrip = new TCanvas(name, name, PAD_WIDTH * maxLayers, PAD_HEIGHT * sumMods);
    canGemStrip->Divide(maxLayers, sumMods);
    // Create histograms
    for (Int_t stationIndex = 0; stationIndex < GEM_STATIONS_COUNT; stationIndex++) {
        vector<vector<TH1F*> > rowGEM;
        for (Int_t moduleIndex = 0; moduleIndex < moduleCount[stationIndex]; moduleIndex++) {
            vector<TH1F*> colGEM;
            for (Int_t layerIndex = 0; layerIndex < layersCount[stationIndex]; layerIndex++) {
                name = Form(fTitle + "_Station_%d_module_%d_layer_%d", stationIndex, moduleIndex, layerIndex);
                TH1F *h = new TH1F(name, name, nStrips[stationIndex], 0, nStrips[stationIndex]);
                h->SetTitleSize(0.06, "XY");
                h->SetLabelSize(0.08, "XY");
                h->GetXaxis()->SetTitle("Strip Number");
                h->GetXaxis()->SetTitleColor(kOrange + 10);
                h->GetYaxis()->SetTitle("Activation Count");
                h->GetYaxis()->SetTitleColor(kOrange + 10);
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
    Int_t modCtr = 0; // filling GEM Canvas' pads
    canGemStripPads.resize(sumMods * maxLayers);
    for (Int_t stationIndex = 0; stationIndex < GEM_STATIONS_COUNT; stationIndex++) {
        for (Int_t moduleIndex = 0; moduleIndex < moduleCount[stationIndex]; moduleIndex++) {
            for (Int_t layerIndex = 0; layerIndex < layersCount[stationIndex]; layerIndex++) {
//                PadInfo<TH1F> *p = new PadInfo<TH1F>();
                PadInfo *p = new PadInfo();
                p->current = histGemStrip[stationIndex][moduleIndex][layerIndex];
                canGemStripPads[modCtr * maxLayers + layerIndex] = p;
                canGemStrip->GetPad(modCtr * maxLayers + layerIndex + 1)->SetGrid();
            }
            modCtr++;
        }
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
    fServer->Register(path, canGemStrip);
    for (auto row : histGemStrip)
        for (auto col : row)
            for (auto el : col)
                fServer->Register(path, el);
    fServer->SetItemField(path, "_monitoring", "2000");
    fServer->SetItemField(path, "_layout", "grid3x3");
    TString examples = TString("[") +
            histGemStrip[0][0][0]->GetTitle() + TString(",") +
            histGemStrip[0][0][1]->GetTitle() + TString(",") +
            histGemStrip[6][1][0]->GetTitle() + TString(",") +
            histGemStrip[6][0][0]->GetTitle() + TString("]");
    fServer->SetItemField(path, "_drawitem", examples);
    TString cmd = "/" + fName + "/->Reset()";
    TString cmdTitle = path + "Reset";
    fServer->RegisterCommand(cmdTitle.Data(), cmd.Data(), "button;");
    fServer->Restrict(cmdTitle.Data(), "visible=shift");
    fServer->Restrict(cmdTitle.Data(), "allow=shift");
    fServer->Restrict(cmdTitle.Data(), "deny=guest");
    cmd = "/" + fName + "/->SetRefRun(%arg1%)";
    cmdTitle = path + "SetRefRun";
    fServer->RegisterCommand(cmdTitle.Data(), cmd.Data(), "button;");
    //    fServer->Restrict(cmdTitle.Data(), "deny=guest");

}

void BmnHistGem::SetDir(TFile *outFile, TTree *recoTree) {
    frecoTree = recoTree;
    TDirectory *dir = NULL;
    if (outFile != NULL)
        dir = outFile->mkdir(fTitle + "_hists");
    for (auto row : histGemStrip)
        for (auto col : row)
            for (auto el : col)
                el->SetDirectory(dir);
}

void BmnHistGem::DrawBoth() {
    BmnHist::DrawRef(canGemStrip, &canGemStripPads);
}

void BmnHistGem::FillFromDigi(TClonesArray * gemDigits) {
    for (Int_t digIndex = 0; digIndex < gemDigits->GetEntriesFast(); digIndex++) {
        BmnGemStripDigit* gs = (BmnGemStripDigit*) gemDigits->At(digIndex);
        Int_t module = gs->GetModule();
        Int_t station = gs->GetStation();
        Int_t layer = gs->GetStripLayer();
        Int_t gemStrip = gs->GetStripNumber();
//printf("station %d, module %d, layer %d\n", station, module, layer);
        histGemStrip[station][module][layer]->Fill(gemStrip);
    }
}

void BmnHistGem::FillFromDigiMasked(TClonesArray * gemDigits, vector<vector<vector<TH1F*> > >* hist0, Double_t threshold) {//vector<vector<vector<Int_t*> > >* mask) {
    for (Int_t digIndex = 0; digIndex < gemDigits->GetEntriesFast(); digIndex++) {
        BmnGemStripDigit* gs = (BmnGemStripDigit*) gemDigits->At(digIndex);
        Int_t module = gs->GetModule();
        Int_t station = gs->GetStation();
        Int_t layer = gs->GetStripLayer();
        Int_t gemStrip = gs->GetStripNumber();
        if ((*hist0)[station][module][layer]->GetBinContent((*hist0)[station][module][layer]->FindBin(gemStrip)) <= threshold * 0.7)
            histGemStrip[station][module][layer]->Fill(gemStrip);
        //            histGemStrip[station][module][layer]->AddBinContent(histGemStrip[station][module][layer]->FindBin(gemStrip));
    }
}

void BmnHistGem::UpdateNoiseMask(Double_t threshold) {
    for (Int_t iStation = 0; iStation < GEM_STATIONS_COUNT; iStation++)
        for (Int_t iModule = 0; iModule < moduleCount[iStation]; iModule++)
            for (Int_t iLayer = 0; iLayer < layersCount[iStation]; iLayer++)
                for (Int_t iStrip = 0; iStrip < histGemStrip[iStation][iModule][iLayer]->GetNbinsX() + 1; iStrip++) {
                    maskGemStrip[iStation][iModule][iLayer][iStrip] = 0;
                    //                    cout << " MASK " << maskGemStrip[iStation][iModule][iLayer][iStrip] << endl;
                    //                            (histGemStrip[iStation][iModule][iLayer]->GetBinContent(iStrip) > threshold) ? 0 : 0;
                }
}

void BmnHistGem::ApplyNoiseMask(vector<vector<vector<TH1F*> > >* hist0, Double_t threshold) {
    for (Int_t iStation = 0; iStation < GEM_STATIONS_COUNT; iStation++)
        for (Int_t iModule = 0; iModule < moduleCount[iStation]; iModule++)
            for (Int_t iLayer = 0; iLayer < layersCount[iStation]; iLayer++)
                for (Int_t iStrip = 0; iStrip < histGemStrip[iStation][iModule][iLayer]->GetNbinsX() + 1; iStrip++)
                    if ((*hist0)[iStation][iModule][iLayer]->GetBinContent(iStrip) > threshold)
                        histGemStrip[iStation][iModule][iLayer]->SetBinContent(iStrip, 0);
}

BmnStatus BmnHistGem::LoadRefRun(TString FileName) {
    printf("Loading ref histos\n");
    refFile = new TFile(refPath + FileName, "read");
    if (refFile->IsOpen() == false) {
        printf("Cannot open file %s !\n", FileName.Data());
        return kBMNERROR;
    }
    canGemStripPads.clear();
    canGemStripPads.resize(maxLayers * sumMods);
    Int_t modCtr = 0;
    TString refName = Form("ref%06d_", refID);
    TString name;
    for (Int_t stationIndex = 0; stationIndex < GEM_STATIONS_COUNT; stationIndex++) {
        for (Int_t moduleIndex = 0; moduleIndex < moduleCount[stationIndex]; moduleIndex++) {
            for (Int_t layerIndex = 0; layerIndex < layersCount[stationIndex]; layerIndex++) {
                name = Form(fTitle + "_Station_%d_module_%d_layer_%d", stationIndex, moduleIndex, layerIndex);
//                PadInfo<TH1F> *p = new PadInfo<TH1F>();
                PadInfo *p = new PadInfo();
                p->current = histGemStrip[stationIndex][moduleIndex][layerIndex];
                //                p.ref = (TH1F*) gDirectory->Get(name.Data());
                //                TH1F* tempHist;
                p->ref = (TH1F*) refFile->Get(refName + "GEM_hists/" + refName + name);
                if (p->ref == NULL) {
                    TH1F* tempH = (TH1F*) refFile->Get(TString("GEM_hists/") + name);
                    if (tempH == NULL) {
                        printf("Cannot load %s !\n", name.Data());
                        continue;
//                        return kBMNERROR;
                    }
                    p->ref = (TH1F*) (tempH->Clone(name));
                }
                p->ref->SetLineColor(kRed);
                p->ref->SetDirectory(0);
                canGemStripPads[modCtr * maxLayers + layerIndex] = p;
            }
            modCtr++;
        }
    }
    //    refFile->Close();
    delete refFile;
    refFile = NULL;
    return kBMNSUCCESS;
}

BmnStatus BmnHistGem::SetRefRun(Int_t id) {
    TString FileName = Form("bmn_run%04d_hist.root", id);
    printf("SetRefRun: %s\n", FileName.Data());
    if (refRunName != FileName) {
        refRunName = FileName;
        refID = id;
        LoadRefRun(refRunName);
    }
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

