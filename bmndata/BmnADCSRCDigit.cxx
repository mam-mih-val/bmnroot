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

#include "BmnADCSRCDigit.h"

BmnADCSRCDigit::BmnADCSRCDigit() :  BmnADCDigit() {
    fTrigTimestamp = 0;
    fAdcTimestamp = 0;
}

BmnADCSRCDigit::BmnADCSRCDigit(
        UInt_t iSerial, UInt_t iChannel, UInt_t n, Short_t *iValue, UShort_t TrigTS, UShort_t AdcTS) :
BmnADCDigit(iSerial, iChannel, n, iValue)
{
    fTrigTimestamp = TrigTS;
    fAdcTimestamp = AdcTS;
}

BmnADCSRCDigit::~BmnADCSRCDigit() {
}

ClassImp(BmnADCSRCDigit)