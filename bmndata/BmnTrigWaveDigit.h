/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   BmnTrigWaveDigit.h
 * Author: ilnur
 *
 * Created on October 16, 2017, 4:52 PM
 */

#ifndef BMNTRIGWAVEDIGIT_H
#define BMNTRIGWAVEDIGIT_H

#include "BmnTrigDigit.h"
#include "BmnADCSRCDigit.h"


class BmnTrigWaveDigit: public BmnTrigDigit /*, public BmnADCSRCDigit */ {
public:
    BmnTrigWaveDigit();
    BmnTrigWaveDigit(BmnTrigDigit *trigDigit, BmnADCSRCDigit *adcDigit);
    BmnTrigWaveDigit(Short_t iMod, Double_t iTime, Double_t iAmp, BmnADCSRCDigit *adcDigit);
    
    UInt_t GetNSamples() const {
        return fNsmpl;
    }
    
    Short_t *GetShortValue() const {
        return (Short_t *) fValueI;
    }

    void SetShortValue(Short_t *iValue) const {
        for (Int_t i = 0; i < fNsmpl; ++i)
            fValueI[i] = iValue[i];
    }
    
    virtual ~BmnTrigWaveDigit();
protected:
    UInt_t fNsmpl;
    Short_t* fValueI; //[fNsmpl]
    UShort_t fTrigTimestamp;
    UShort_t fAdcTimestamp;

    ClassDef(BmnTrigWaveDigit, 1);
};

#endif /* BMNTRIGWAVEDIGIT_H */

