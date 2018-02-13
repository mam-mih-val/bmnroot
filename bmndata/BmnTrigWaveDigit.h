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
#include "BmnTQDCADCDigit.h"


class BmnTrigWaveDigit: public BmnTrigDigit /*, public BmnADCSRCDigit */ {
public:
    BmnTrigWaveDigit();
    BmnTrigWaveDigit(BmnTrigDigit *trigDigit, BmnTQDCADCDigit *adcDigit);
    BmnTrigWaveDigit(Short_t iMod, Double_t iTime, Double_t iAmp, BmnTQDCADCDigit *adcDigit);
    
    UInt_t GetNSamples() const {
        return fNsmpl;
    }
   
    int GetIntegral() const {
	int spectra = 0;
	for (Int_t i = 0; i < fNsmpl; ++i)
		spectra += fValueI[i];
	return spectra;
    }    
    
    int GetPeak() const {
	int peak = -100000;
	for (Int_t i = 0; i < fNsmpl; ++i)
		if ( fValueI[i] > peak ) peak = fValueI[i];
	return peak;
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

