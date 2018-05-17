
//
// Base class for processing data from ADC detectors
// It's used for pedestal calculation
//

#ifndef BMNADCPROCESSOR_H
#define BMNADCPROCESSOR_H

#include "TString.h"
#include "TTree.h"
#include "TClonesArray.h"
#include <iostream>
#include "BmnADCDigit.h"
#include "BmnEnums.h"
#include "TMath.h"
#include <fstream>
#include <list>
#include <map>
#include <vector>
#include <UniDbDetectorParameter.h>
#include <UniDbDetector.h>

#define N_EV_FOR_PEDESTALS 500
#define ADC_N_CHANNELS 64 //number of ADC channels
#define ADC128_N_SAMPLES 128 //number of samples in one ADC digit //silicon
#define ADC32_N_SAMPLES 32 //number of samples in one ADC digit //gem

using namespace std;
using namespace TMath;

class BmnAdcProcessor {
public:
    BmnAdcProcessor(Int_t period, Int_t run, TString det, Int_t nCh, Int_t nSmpl, vector<UInt_t> vSer);
    BmnAdcProcessor();
    virtual ~BmnAdcProcessor();

    BmnStatus RecalculatePedestals();
    Double_t CalcCMS(Double_t* samples, Int_t size);

    Double_t**** GetPedData() {
        return fPedDat;
    }

    void SetNSerials(Int_t n) {
        fNSerials = n;
    }

    void SetNSamples(Int_t n) {
        fNSamples = n;
    }

    void SetNChannels(Int_t n) {
        fNChannels = n;
    }

    Double_t GetPedestal(Int_t ser, Int_t ch, Int_t smpl) {
        return fPedVal[ser][ch][smpl];
    }

    Double_t*** GetPedestals() {
        return fPedVal;
    }

    Double_t*** GetPedestalsRMS() {
        return fPedRms;
    }

    Int_t GetPeriod() {
        return fPeriod;
    }

    Int_t GetRun() {
        return fRun;
    }

    vector<UInt_t> GetSerials() {
        return fSerials;
    }

    UInt_t GetBoundaryRun(UInt_t nSmpl) {
        //format for SILICON data was changed during March 2017 seance (run 1542)
        //format for GEM was changed after March 2017 seance (run 1992)
        //so we have to use this crutch.
        return (nSmpl == 128) ? 1542 : 1992;
    }


private:

    vector<UInt_t> fSerials; //list of serial id for ADC-detector

    Int_t fEntriesInGlobMap; // number of entries in BD table for Global Mapping

    Int_t fPeriod;
    Int_t fRun;
    Int_t fNSerials;
    Int_t fNSamples;
    Int_t fNChannels;
    TString fDetName; //it's used for .txt files name 

    Double_t**** fPedDat; //data set to calculate pedestals
    Double_t*** fPedVal; //set of calculated pedestals
    Double_t*** fPedRms; // set of calculated pedestal errors
    UInt_t*** fAdcProfiles;

    ClassDef(BmnAdcProcessor, 1);
};

#endif /* BMNADCPROCESSOR_H */

