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

#ifndef BMNTQDCADCDIGIT_H
#define BMNTQDCADCDIGIT_H

#include "BmnADCDigit.h"


class BmnTQDCADCDigit : public BmnADCDigit {
public:
    BmnTQDCADCDigit();
    BmnTQDCADCDigit(UInt_t iSerial, UInt_t iChannel, UChar_t iSlot, UInt_t n, Short_t *iValue, UShort_t TrigTS, UShort_t AdcTS);
    
    UShort_t GetTrigTimestamp() const {
        return fTrigTimestamp;
    }

    UShort_t GetAdcTimestamp() const {
        return fAdcTimestamp;
    }

    UChar_t GetSlot() const {
        return fSlot;
    }
    virtual ~BmnTQDCADCDigit();
protected:
    UShort_t fTrigTimestamp;
    UShort_t fAdcTimestamp;
    UChar_t fSlot;

    ClassDef(BmnTQDCADCDigit, 1);
};

#endif /* BMNTQDCADCDIGIT_H */

