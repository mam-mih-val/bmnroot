#ifndef BMNGEMRAW2DIGIT_H
#define BMNGEMRAW2DIGIT_H

#include "BmnEnums.h"
#include "TString.h"
#include "TTree.h"
#include "TClonesArray.h"
#include "BmnADCDigit.h"
#include <iostream>
#include <fstream>
#include <map>
#include <vector>
#include "BmnGemStripDigit.h"
#include "BmnADC32Digit.h"
#include "BmnEnums.h"
#include "BmnADC32Digit.h"
#include "TMath.h"
#include <UniDbDetectorParameter.h>

#define ADC_N_SAMPLES 32 //number of samples in one ADC digit

using namespace std;
using namespace TMath;

//struct BmnGemPedestal {
//    UInt_t serial;
//    UInt_t ch;
//    UInt_t pedestal;
//    UInt_t comMod;
//};

class BmnGemRaw2Digit {
public:
    BmnGemRaw2Digit(Int_t period, Int_t run);
    BmnGemRaw2Digit();

    ~BmnGemRaw2Digit() {
    };

    BmnStatus FillEvent(TClonesArray *adc, TClonesArray *gem);
    BmnStatus CalcGemPedestals(TClonesArray *adc, TTree *tree);

private:

    IIStructure* fX0_big_l;
    IIStructure* fX0_big_r;
    IIStructure* fX1_big_l;
    IIStructure* fX1_big_r;
    IIStructure* fY0_big_l;
    IIStructure* fY0_big_r;
    IIStructure* fY1_big_l;
    IIStructure* fY1_big_r;
    IIStructure* fX0_mid;
    IIStructure* fY0_mid;
    IIStructure* fX1_mid;
    IIStructure* fY1_mid;
    IIStructure* fX_small;
    IIStructure* fY_small;

    GemMapStructure* fMap;
    UInt_t** fPedMap; // pedestals
    UInt_t** fCMMap; // common mode shift
    UInt_t** fNoiseMap; // channel noises
    BmnStatus FillMaps();
    void ProcessDigit(BmnADC32Digit* adcDig, GemMapStructure* gemM, TClonesArray *gem);
    Int_t SearchInMap(IIStructure* m, Int_t size, UInt_t ch);
    UInt_t SearchPed(UInt_t ch, UInt_t ser);
    UInt_t SearchNoise(UInt_t chn, UInt_t ser);
    UInt_t SearchComMod(UInt_t chn, UInt_t ser);
    Int_t fEntriesInGlobMap; // member of entries in BD table
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

    ClassDef(BmnGemRaw2Digit, 1);
};

#endif /* BMNGEMRAW2DIGIT_H */

