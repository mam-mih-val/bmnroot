/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   BmnADCSRCDigit.cxx
 * Author: ilnur
 * 
 * Created on October 16, 2017, 4:07 PM
 */

#include "BmnTQDCADCDigit.h"

BmnTQDCADCDigit::BmnTQDCADCDigit() :  BmnADCDigit() {
    fTrigTimestamp = 0;
    fAdcTimestamp = 0;
}

BmnTQDCADCDigit::BmnTQDCADCDigit(
        UInt_t iSerial, UInt_t iChannel, UChar_t iSlot, UInt_t n, Short_t *iValue, UShort_t TrigTS, UShort_t AdcTS) :
BmnADCDigit(iSerial, iChannel, n, iValue)
{
    fTrigTimestamp = TrigTS;
    fAdcTimestamp = AdcTS;
    fSlot = iSlot;
}

BmnTQDCADCDigit::~BmnTQDCADCDigit() {
}

ClassImp(BmnTQDCADCDigit)