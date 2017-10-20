/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   BmnADCSRCDigit.h
 * Author: ilnur
 *
 * Created on October 16, 2017, 4:07 PM
 */

#ifndef BMNADCSRCDIGIT_H
#define BMNADCSRCDIGIT_H

#include "BmnADCDigit.h"


class BmnADCSRCDigit : public BmnADCDigit {
public:
    BmnADCSRCDigit();
    BmnADCSRCDigit(UInt_t iSerial, UInt_t iChannel, UInt_t n, Short_t *iValue, UShort_t TrigTS, UShort_t AdcTS);
    
    UShort_t GetTrigTimestamp() const {
        return fTrigTimestamp;
    }

    UShort_t GetAdcTimestamp() const {
        return fAdcTimestamp;
    }
    virtual ~BmnADCSRCDigit();
protected:
    UShort_t fTrigTimestamp;
    UShort_t fAdcTimestamp;
    

    ClassDef(BmnADCSRCDigit, 1);
};

#endif /* BMNADCSRCDIGIT_H */

