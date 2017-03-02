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
//    Double_t k = 1;
    for (Int_t iPad = 0; iPad < canGemStripPads->size(); iPad++) {
        TVirtualPad *pad = canGemStrip->cd(iPad + 1);
        pad->Clear();
        if (!canGemStripPads->at(iPad)) continue;
        if (canGemStripPads->at(iPad)->current) {
            maxy = canGemStripPads->at(iPad)->current->GetMaximum();
            canGemStripPads->at(iPad)->current->Draw();
            if (canGemStripPads->at(iPad)->ref != NULL) {
//                k = (canGemStripPads[iPad]->ref->GetEntries() > 0) ?
//                        canGemStripPads[iPad]->current->GetEntries() /
//                        (Double_t) canGemStripPads[iPad]->ref->GetEntries() : 1;
                if (canGemStripPads->at(iPad)->ref->GetEntries() > 0)
                    canGemStripPads->at(iPad)->ref->DrawNormalized("same hist", canGemStripPads->at(iPad)->current->GetEntries());
            }
            canGemStripPads->at(iPad)->current->GetYaxis()->SetRangeUser(0, maxy);
        }
//        pad->Update();
        pad->Modified();
    }
//    canGemStrip->Update();
    canGemStrip->Modified();
}


ClassImp(BmnHist);
