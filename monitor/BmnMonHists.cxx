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

#include "BmnMonHists.h"

BmnMonHists::BmnMonHists() {
//    memset(histGemStrip, 0, sizeof(histGemStrip));
}

BmnMonHists::BmnMonHists(const BmnMonHists& orig) {
}

BmnMonHists::~BmnMonHists() {
}

void BmnMonHists::ClearGEM() {
        for (auto row = histGemStrip.begin(); row != histGemStrip.end(); row++)
            for (auto col = row->begin(); col != row->end(); col++)
                for (auto el = col->begin(); el != col->end(); el++) {
                    (*el)->Reset();
                }
//    for (Int_t row = 0; row < MAX_STATIONS; row++)
//        for (Int_t col = 0; col < MAX_MODULES; col++)
//            for (Int_t el = 0; el < MAX_LAYERS; el++)
//            {
//                if (histGemStrip[row][col][el] != NULL)
//                    histGemStrip[row][col][el]->Reset();
//            }
}

void BmnMonHists::ClearToF700() {

}

void BmnMonHists::ClearToF400() {
    histToF400LeadingTime->Reset();
    histToF400Amp->Reset();
    histToF400Strip->Reset();
    histToF400StripSimult->Reset();
}

void BmnMonHists::ClearTriggers() {
    histBC1TimeLen->Reset();
    histBC2TimeLen->Reset();
    histSDTimeLen->Reset();
    histVDTimeLen->Reset();
    histFDTimeLen->Reset();
    histBDTimeLen->Reset();
    histTriggers->Reset();
}

void BmnMonHists::Clear() {
    ClearGEM();
    ClearToF400();
    ClearToF700();
    ClearTriggers();
}

ClassImp(BmnMonHists);

