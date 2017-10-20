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

BmnTrigWaveDigit::BmnTrigWaveDigit() : BmnTrigDigit()/*, BmnADCSRCDigit() */{
    fNsmpl = 0;
    fValueI = new Short_t[fNsmpl];
    for (Int_t i = 0; i < fNsmpl; ++i)
        fValueI[i] = 0;
    fTrigTimestamp = 0;
    fAdcTimestamp = 0;
}

BmnTrigWaveDigit::BmnTrigWaveDigit(BmnTrigDigit* trig, BmnADCSRCDigit* adc) :
    BmnTrigDigit(trig->GetMod(), trig->GetTime(), trig->GetAmp())/*,
    BmnADCSRCDigit(
            adc->GetSerial(), adc->GetChannel(), adc->GetNSamples(),
            adc->GetShortValue(), adc->GetTrigTimestamp(), adc->GetAdcTimestamp()
            ) */{
    Short_t *iValue;
    fNsmpl = adc->GetNSamples();
    fValueI = new Short_t[fNsmpl];
    for (Int_t i = 0; i < fNsmpl; ++i)
        fValueI[i] = iValue[i];
    fTrigTimestamp = adc->GetTrigTimestamp();
    fAdcTimestamp = adc->GetAdcTimestamp();
}

BmnTrigWaveDigit::BmnTrigWaveDigit(Short_t iMod, Double_t iTime, Double_t iAmp, BmnADCSRCDigit* adc) :
   BmnTrigDigit(iMod, iTime, iAmp)/*,
BmnADCSRCDigit(
            adc->GetSerial(), adc->GetChannel(), adc->GetNSamples(),
            adc->GetShortValue(), adc->GetTrigTimestamp(), adc->GetAdcTimestamp()
            )*/{
    Short_t *iValue;
    fNsmpl = adc->GetNSamples();
    fValueI = new Short_t[fNsmpl];
    for (Int_t i = 0; i < fNsmpl; ++i)
        fValueI[i] = iValue[i];
    fTrigTimestamp = adc->GetTrigTimestamp();
    fAdcTimestamp = adc->GetAdcTimestamp();
}

BmnTrigWaveDigit::~BmnTrigWaveDigit() {
    delete [] fValueI;
}

ClassImp(BmnTrigWaveDigit)
