/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   BmnTrigWaveDigit.cxx
 * Author: ilnur
 * 
 * Created on October 16, 2017, 4:52 PM
 */

#include "BmnTrigWaveDigit.h"

BmnTrigWaveDigit::BmnTrigWaveDigit(){
    fMod = -1;
    fNsmpl = 0;
    fValueI = new Short_t[fNsmpl];
    for (Int_t i = 0; i < fNsmpl; ++i)
        fValueI[i] = 0;
    fTrigTimestamp = 0;
    fAdcTimestamp = 0;
}

BmnTrigWaveDigit::BmnTrigWaveDigit(Short_t iMod, Short_t *iValue, UInt_t nVals, Double_t trigTimestamp, Double_t adcTimestamp){
    fMod = iMod;
    fNsmpl = nVals;
    fValueI = new Short_t[fNsmpl];
    for (Int_t i = 0; i < fNsmpl; ++i)
        fValueI[i] = iValue[i];
    fTrigTimestamp = trigTimestamp;
    fAdcTimestamp = adcTimestamp;
}

BmnTrigWaveDigit::~BmnTrigWaveDigit() {
    delete [] fValueI;
}

ClassImp(BmnTrigWaveDigit)
