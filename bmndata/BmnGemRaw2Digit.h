#ifndef BMNGEMRAW2DIGIT_H
#define BMNGEMRAW2DIGIT_H

#include "TString.h"
#include "TTree.h"
#include "TClonesArray.h"
#include <iostream>
#include "BmnGemStripDigit.h"
#include "BmnADC32Digit.h"
#include "BmnEnums.h"
#include "TMath.h"
#include <fstream>
#include <list>
#include <map>
#include <vector>
#include <UniDbDetectorParameter.h>
#include <UniDbDetector.h>

#define ADC_N_CHANNELS 64 //number of ADC channels
#define ADC32_N_SAMPLES 32 //number of samples in one ADC digit
#define N_CH_IN_CRATE 2048 //number of channels in one crate (64ch x 32smpl))
#define N_CH_IN_SMALL_GEM 512 //number of channels in small GEM stations (sum of all redout channels)
#define N_CH_IN_MID_GEM 2176 //number of channels in middle GEM stations (sum of all redout channels)
#define N_CH_IN_BIG_GEM 3200 //number of channels in one part of big GEM stations (sum of all redout channels)
#define N_CH_IN_BIG_GEM_0 1023//988 //number of channels in hot zone of one part of big GEM stations (sum of redout channels from X0 and Y0)
#define N_CH_IN_BIG_GEM_1 2176//2100 //number of channels in big zone of one part of big GEM stations (sum of redout channels from X1 and Y1)
#define N_CH_BUF 4096
#define N_MODULES 2
#define N_LAYERS 4
#define N_EV_FOR_PEDESTALS 500

using namespace std;
using namespace TMath;

struct BmnGemMap {
    Int_t strip;
    Int_t lay; // strip type: 0 - x, 1 - y
    Int_t mod; //hot zones: 1 - inner zone, 0 - outer zone

    BmnGemMap(Int_t s, Int_t l, Int_t m) : strip(s), lay(l), mod(m) {
    }

    BmnGemMap() : strip(0), lay(0), mod(0) {
    }
};

struct BmnGemPed {
    Double_t ped;
    Double_t noise;

    BmnGemPed(Double_t p, Double_t n) : ped(p), noise(n) {
    }

    BmnGemPed() : ped(0), noise(0) {
    }
};

struct BmnGemPedestal {
    UInt_t ser;
    UInt_t ch;
    Double_t ped;
    Double_t noise;

    BmnGemPedestal(UInt_t s, UInt_t c, Double_t p, Double_t n) : ser(s), ch(c), ped(p), noise(n) {
    }

    BmnGemPedestal() : ser(0), ch(0), ped(0.0), noise(0.0) {
    }
};

class BmnGemRaw2Digit {
public:
    BmnGemRaw2Digit(Int_t period, Int_t run);
    BmnGemRaw2Digit();
    virtual ~BmnGemRaw2Digit();

    BmnStatus FillEvent(TClonesArray *adc, TClonesArray *gem);
    BmnStatus CalcGemPedestals(TClonesArray *adc, TTree *tree);
    BmnStatus RecalculatePedestals();

    UInt_t**** GetPedData() {
        return fPedDat;
    }

private:

    BmnGemMap* fSmall;
    BmnGemMap* fMid;
    BmnGemMap* fBigL0;
    BmnGemMap* fBigL1;
    BmnGemMap* fBigR0;
    BmnGemMap* fBigR1;

    GemMapStructure* fMap;

    vector<UInt_t> fSerials; //list of serial id for GEM
    BmnGemPed** fPedArr;

    void ProcessDigit(BmnADC32Digit* adcDig, GemMapStructure* gemM, TClonesArray *gem);
    BmnStatus ReadMap(TString parName, TString parNameSize, BmnGemMap* m, Int_t lay, Int_t mod);
    Double_t CalcCMS(Double_t* samples, Int_t size);
    BmnStatus FindNoisyStrips();

    Int_t fEntriesInGlobMap; // number of entries in BD table for Global Mapping

    Int_t fPeriod;
    Int_t fRun;
    Int_t fNSerials;
    Int_t fEventId;

    UInt_t**** fPedDat; //data set to calculate pedestals
    Float_t*** fPedVal; //set of calculated pedestals
    Float_t*** fPedRms; // set of calculated pedestal errors
    UInt_t*** fAdcProfiles;
    Bool_t*** fNoiseChannels; //false = good channel, true = noisy channel

    ClassDef(BmnGemRaw2Digit, 1);
};

#endif /* BMNGEMRAW2DIGIT_H */

