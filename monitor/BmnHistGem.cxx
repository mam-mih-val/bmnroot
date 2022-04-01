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

#include "BmnHistGem.h"
#include "BmnRawDataDecoder.h"

BmnHistGem::BmnHistGem(TString title, TString path, Int_t PeriodID, BmnSetup stp) : BmnHist(PeriodID, stp) {
    sumMods = 0;
    maxLayers = 0;
    refPath = path;
    fTitle = title;
    fName = title + "_cl";
    TString name;
    gemStationSet = BmnAdcProcessor::GetGemStationSet(fPeriodID, fSetup);
    for (Int_t iStation = 0; iStation < gemStationSet->GetNStations(); iStation++) {
        vector<vector<TH1F*> > rowGEM;
        BmnGemStripStation * st = gemStationSet->GetGemStation(iStation);
        sumMods += st->GetNModules();
        for (Int_t iModule = 0; iModule < st->GetNModules(); iModule++) {
            vector<TH1F*> colGEM;
            BmnGemStripModule *mod = st->GetModule(iModule);
            if (maxLayers < mod->GetNStripLayers())
                maxLayers = mod->GetNStripLayers();
            for (Int_t iLayer = 0; iLayer < mod->GetNStripLayers(); iLayer++) {
                BmnGemStripLayer lay = mod->GetStripLayer(iLayer);
                name = Form(fTitle + "_Station_%d_module_%d_layer_%d", iStation, iModule, iLayer);
                TH1F *h = new TH1F(name, name, lay.GetNStrips(), 0, lay.GetNStrips());
                h->SetTitleSize(0.06, "XY");
                h->SetLabelSize(0.08, "XY");
                h->GetXaxis()->SetTitle("Strip Number");
                h->GetXaxis()->SetTitleColor(kOrange + 10);
                h->GetYaxis()->SetTitle("Activation Count");
                h->GetYaxis()->SetTitleColor(kOrange + 10);
                h->GetYaxis()->SetTitleOffset(1.0);
                colGEM.push_back(h);

            }
            rowGEM.push_back(colGEM);

        }
        histGemStrip.push_back(rowGEM);

    }
    // Create canvas
    name = fTitle + "Canvas";
    canGemStrip = new TCanvas(name, name, PAD_WIDTH * maxLayers, PAD_HEIGHT * sumMods);
    canGemStrip->Divide(maxLayers, sumMods);
    Int_t modCtr = 0; // filling GEM Canvas' pads
    canStripPads.resize(sumMods * maxLayers);
    Names.resize(sumMods * maxLayers);
    for (Int_t iStation = 0; iStation < gemStationSet->GetNStations(); iStation++) {
        BmnGemStripStation * st = gemStationSet->GetGemStation(iStation);
        for (Int_t iModule = 0; iModule < st->GetNModules(); iModule++) {
            BmnGemStripModule *mod = st->GetModule(iModule);
            for (Int_t iLayer = 0; iLayer < mod->GetNStripLayers(); iLayer++) {
                PadInfo *p = new PadInfo();
                p->opt = "";
                p->current = histGemStrip[iStation][iModule][iLayer];
                Int_t iPad = modCtr * maxLayers + iLayer;
                canStripPads[iPad] = p;
                canGemStrip->GetPad(iPad + 1)->SetGrid();
                Names[iPad] = canStripPads[iPad]->current->GetName();
            }
            modCtr++;
        }
    }
}

BmnHistGem::~BmnHistGem() {
    delete gemStationSet;
    delete canGemStrip;
    if (fDir)
        return;
    for (auto pad : canStripPads)
        delete pad;
}

void BmnHistGem::Register(THttpServer * serv) {
    fServer = serv;
    fServer->Register("/", this);
    TString path = "/" + fTitle + "/";
    fServer->Register(path, canGemStrip);
    //    for (auto row : histGemStrip)
    //        for (auto col : row)
    //            for (auto el : col)
    //                fServer->Register(path, el);
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

void BmnHistGem::SetDir(TFile *outFile, TTree * recoTree) {
    frecoTree = recoTree;
    fDir = NULL;
    if (outFile != NULL)
        fDir = outFile->mkdir(fTitle + "_hists");
    for (auto row : histGemStrip)
        for (auto col : row)
            for (auto el : col)
                el->SetDirectory(fDir);
}

void BmnHistGem::DrawBoth() {
    BmnHist::DrawRef(canGemStrip, &canStripPads);
}

void BmnHistGem::FillFromDigi(DigiArrays *fDigiArrays) {
    TClonesArray * gemDigits = fDigiArrays->gem;
    if (!gemDigits)
        return;
    for (Int_t digIndex = 0; digIndex < gemDigits->GetEntriesFast(); digIndex++) {
        BmnGemStripDigit* gs = (BmnGemStripDigit*) gemDigits->At(digIndex);
        Int_t module = gs->GetModule();
        Int_t station = gs->GetStation();
        Int_t layer = gs->GetStripLayer();
        Int_t gemStrip = gs->GetStripNumber();
        histGemStrip[station][module][layer]->Fill(gemStrip);
    }
}

BmnStatus BmnHistGem::SetRefRun(Int_t id) {
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

void BmnHistGem::ClearRefRun() {
    for (auto pad : canStripPads) {
        if (pad->ref) delete pad->ref;
        pad->ref = NULL;
    }
    refID = 0;
}

void BmnHistGem::Reset() {
    for (auto row : histGemStrip)
        for (auto col : row)
            for (auto el : col)
                el->Reset();
}


ClassImp(BmnHistGem);

