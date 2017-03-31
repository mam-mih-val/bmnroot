
//
// Base class for processing data from ADC detectors
// It's used for pedestal and noisy strips calculation
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

#define N_EV_FOR_PEDESTALS 1000

using namespace std;
using namespace TMath;

class BmnAdcProcessor {
public:
    BmnAdcProcessor(Int_t period, Int_t run, TString det, Int_t nCh, Int_t nSmpl, vector<UInt_t> vSer);
    BmnAdcProcessor();
    virtual ~BmnAdcProcessor();

    BmnStatus RecalculatePedestals();
    Double_t CalcCMS(Double_t* samples, Int_t size);

    UInt_t**** GetPedData() {
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
    
    Bool_t IsChannelNoisy(Int_t ser, Int_t ch, Int_t smpl) {
        return fNoiseChannels[ser][ch][smpl];
    }
    
    Float_t GetPedestal(Int_t ser, Int_t ch, Int_t smpl) {
        return fPedVal[ser][ch][smpl];
    }
    
    Bool_t*** GetNoiseChannels() {
        return fNoiseChannels;
    }
    
    Float_t*** GetPedestals() {
        return fPedVal;
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
    

private:

    vector<UInt_t> fSerials; //list of serial id for ADC-detector
    BmnStatus FindNoisyStrips();

    Int_t fEntriesInGlobMap; // number of entries in BD table for Global Mapping

    Int_t fPeriod;
    Int_t fRun;
    Int_t fNSerials;
    Int_t fNSamples;
    Int_t fNChannels;
    TString fDetName; //it's used for .txt files name 

    UInt_t**** fPedDat; //data set to calculate pedestals
    Float_t*** fPedVal; //set of calculated pedestals
    Float_t*** fPedRms; // set of calculated pedestal errors
    UInt_t*** fAdcProfiles;
    Bool_t*** fNoiseChannels; //false = good channel, true = noisy channel

    ClassDef(BmnAdcProcessor, 1);
};

#endif /* BMNADCPROCESSOR_H */

