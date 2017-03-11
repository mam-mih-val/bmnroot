/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   BmnHist.cxx
 * Author: ilnur
 * 
 * Created on February 2, 2017, 2:10 PM
 */

#include "BmnHist.h"

BmnHist::BmnHist() {
    refFile = NULL;
    frecoTree = NULL;
}

BmnHist::~BmnHist() {
}

//template <class HH>

void BmnHist::DrawRef(TCanvas *canGemStrip, vector<PadInfo*> *canGemStripPads) {
    Double_t maxy;
    Double_t k = 1;
    for (Int_t iPad = 0; iPad < canGemStripPads->size(); iPad++) {
        TVirtualPad *pad = canGemStrip->cd(iPad + 1);
        pad->Clear();
        if (!canGemStripPads->at(iPad)) continue;
        if (canGemStripPads->at(iPad)->current) {
            maxy = canGemStripPads->at(iPad)->current->GetBinContent(canGemStripPads->at(iPad)->current->GetMaximumBin());
            canGemStripPads->at(iPad)->current->Draw();
            if (canGemStripPads->at(iPad)->ref != NULL) {
                k = (canGemStripPads->at(iPad)->ref->Integral() > 0) ?
                        canGemStripPads->at(iPad)->current->Integral() /
                        (Double_t) canGemStripPads->at(iPad)->ref->Integral() : 1;
                if (k == 0) k = 1;
                if (canGemStripPads->at(iPad)->ref->Integral() > 0)
                    canGemStripPads->at(iPad)->ref->DrawNormalized("same hist", canGemStripPads->at(iPad)->current->Integral());
                k = k * canGemStripPads->at(iPad)->ref->GetBinContent(canGemStripPads->at(iPad)->ref->GetMaximumBin());
                if (maxy < k)
                    maxy = k;
                canGemStripPads->at(iPad)->current->GetYaxis()->SetRangeUser(0, maxy * 1.05);
            }
        }
        //        pad->Update();
        pad->Modified();
    }
    canGemStrip->Update();
    canGemStrip->Modified();
}

BmnStatus BmnHist::LoadRefRun(Int_t refID, TString FullName, TString fTitle, vector<PadInfo*> canPads, vector<TString> Names) {
    printf("Loading ref histos\n");
//    TString FileName = Form("bmn_run%04d_hist.root", refID);
    TFile *refFile = new TFile(FullName, "read");
    if (refFile->IsOpen() == false) {
        printf("Cannot open file %s !\n", FullName.Data());
        return kBMNERROR;
    }
    TString refName = Form("ref%06d_", refID);
    TString name;
    for (Int_t iPad = 0; iPad < Names.size(); iPad++) {
        name = Names[iPad];
        if (name.Length() == 0)
            continue;
        delete canPads[iPad]->ref;
        canPads[iPad]->ref = NULL;
        TH1F* tempH = NULL;
        tempH = (TH1F*) refFile->Get(refName + fTitle + "_hists/" + refName + name);
        if (tempH == NULL) {
            tempH = (TH1F*) refFile->Get(fTitle + "_hists/" + name);
        }
        if (tempH == NULL) {
            printf("Cannot load %s !\n", name.Data());
            continue;
            //                return kBMNERROR;
        }
        canPads[iPad]->ref = (TH1F*) (tempH->Clone(name));
        canPads[iPad]->ref->SetLineColor(kRed);
        canPads[iPad]->ref->SetDirectory(0);
        printf("Loaded %s \n", canPads[iPad]->ref->GetName());
    }
    delete refFile;
    refFile = NULL;
    return kBMNSUCCESS;
}


ClassImp(BmnHist);
