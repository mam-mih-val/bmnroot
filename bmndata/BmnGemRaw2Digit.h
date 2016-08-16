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
#include <UniDbDetectorParameter.h>

#define ADC_N_SAMPLES 32 //number of samples in one ADC digit
#define N_CH_IN_CRATE 2048 //number of channels in one crate (64ch x 32smpl))
#define N_CH_IN_SMALL_GEM 512 //number of channels in small GEM stations (sum of all redout channels)
#define N_CH_IN_MID_GEM 2176 //number of channels in middle GEM stations (sum of all redout channels)
#define N_CH_IN_BIG_GEM 3200 //number of channels in big one part of GEM stations (sum of all redout channels)

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
    Int_t ped;
    Int_t noise;

    BmnGemPed(Int_t p, Int_t n) : ped(p), noise(n) {
    }

    BmnGemPed() : ped(0), noise(0) {
    }
};

class BmnGemRaw2Digit {
public:
    BmnGemRaw2Digit(Int_t period, Int_t run);
    BmnGemRaw2Digit();
    virtual ~BmnGemRaw2Digit();

    BmnStatus FillEvent(TClonesArray *adc, TClonesArray *gem);
    BmnStatus CalcGemPedestals(TClonesArray *adc, TTree *tree);

private:

    BmnGemMap* fSmall;
    BmnGemMap* fMid;
    BmnGemMap* fBigL;
    BmnGemMap* fBigR;

    GemMapStructure* fMap;
    GemPedestalStructure* fPed;
    
    UInt_t* fCrates;
    BmnGemPed** fPedArr;
    
    void ProcessDigit(BmnADC32Digit* adcDig, GemMapStructure* gemM, TClonesArray *gem);
    BmnStatus ReadMap(TString parName, BmnGemMap* m, Int_t size, Int_t lay, Int_t mod);
    Int_t fEntriesInGlobMap; // number of entries in BD table for Globl Mapping
    Int_t fEntriesInPedMap; // number of entries in BD table for Pedestal Mapping
    
    Int_t fNchXsmall;
    Int_t fNchYsmall;
    Int_t fNchX0mid;
    Int_t fNchY0mid;
    Int_t fNchX1mid;
    Int_t fNchY1mid;
    Int_t fNchX0big_l;
    Int_t fNchX0big_r;
    Int_t fNchX1big_l;
    Int_t fNchX1big_r;
    Int_t fNchY0big_l;
    Int_t fNchY0big_r;
    Int_t fNchY1big_l;
    Int_t fNchY1big_r;
    
    Int_t fPeriod;
    Int_t fRun;
    Int_t fNCrates;

    ClassDef(BmnGemRaw2Digit, 1);
};

#endif /* BMNGEMRAW2DIGIT_H */

