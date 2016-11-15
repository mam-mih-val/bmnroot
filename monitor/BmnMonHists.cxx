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

#include "BmnMonHists.h"

BmnMonHists::BmnMonHists(){//Int_t gemCount, Int_t layer = 0, Int_t TOF400_STRIP_COUNT) {
    /*TString name;
    name = "ToF400_Leading_Time";
    histToF400LeadingTime = new TH1D(name, name, 500, 0, 1000);
    name = "ToF400_Amplitude";
    histToF400Amp = new TH1D(name, name, 4096, 0, 96);
    name = "ToF400_Strip";
    histToF400Strip = new TH1I(name, name, TOF400_STRIP_COUNT, 0, TOF400_STRIP_COUNT);
    name = "ToF400_StripSimult";
    histToF400StripSimult = new TH1I(name, name, TOF400_STRIP_COUNT, 0, TOF400_STRIP_COUNT);
    name = "ToF400_State";
    histToF400State = new TH2F(name, name, TOF400_STRIP_COUNT, 0, TOF400_STRIP_COUNT, 2, 0, 2);*/
}

BmnMonHists::BmnMonHists(const BmnMonHists& orig) {
}

BmnMonHists::~BmnMonHists() {
}
void BmnMonHists::pushGemVec(Int_t i, TH1F* h){
    if (i < histStrip.size())
        histStrip[i].push_back(h);
    else
        if (i == histStrip.size()){
            vector<TH1F*> v;
            v.push_back(h);
            histStrip.push_back(v);
        }
        else
            throw std::out_of_range("vector");
}

void BmnMonHists::Clear(){
    for (auto row = histStrip.begin(); row != histStrip.end(); row++)
        for (auto el = row->begin(); el != row->end(); el++){
            (*el)->Reset();
    }        
    histToF400LeadingTime->Reset();
    histToF400Amp->Reset();
    histToF400Strip->Reset();
    histToF400StripSimult->Reset();
    
}

ClassImp(BmnMonHists);

