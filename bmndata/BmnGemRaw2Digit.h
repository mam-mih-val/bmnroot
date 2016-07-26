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

using namespace std;
using namespace TMath;

struct BmnGemMapping {
    UInt_t serial;
    Short_t gemId;
    Short_t station;
    UInt_t adcChLo;
    UInt_t adcChHi;
    UInt_t gemChLo;
    UInt_t gemChHi;
    Bool_t hotZone;
};
//
//struct BmnGemPedestal {
//    UInt_t serial;
//    UInt_t ch;
//    UInt_t pedestal;
//    UInt_t comMod;
//};

class BmnGemRaw2Digit {

public:
    BmnGemRaw2Digit(TString mappingFile);
    BmnGemRaw2Digit();;
    ~BmnGemRaw2Digit() {};

    vector<BmnGemMapping> GetMap() const {
        return fMap;
    }
    
    BmnStatus FillEvent(TClonesArray *adc, TClonesArray *gem);
    BmnStatus CalcGemPedestals(TClonesArray *adc, TTree *tree);
    
private:
    
    // maps for storing <channel, strip> accordance  
    map<UInt_t, UInt_t> X0_big_l;
    map<UInt_t, UInt_t> X0_big_r;
    map<UInt_t, UInt_t> X1_big_l;
    map<UInt_t, UInt_t> X1_big_r;
    map<UInt_t, UInt_t> Y0_big_l;
    map<UInt_t, UInt_t> Y0_big_r;
    map<UInt_t, UInt_t> Y1_big_l;
    map<UInt_t, UInt_t> Y1_big_r;
    map<UInt_t, UInt_t> X0_mid;
    map<UInt_t, UInt_t> Y0_mid;
    map<UInt_t, UInt_t> X1_mid;
    map<UInt_t, UInt_t> Y1_mid;
    map<UInt_t, UInt_t> X_small;
    map<UInt_t, UInt_t> Y_small;
    
    vector<BmnGemMapping> fMap;
    UInt_t** fPedMap; // pedestals
    UInt_t** fCMMap;  // common mode shift
    UInt_t** fNoiseMap;  // channel noises
    BmnStatus FillMaps();
    void ReadAndPut(TString fName, map<UInt_t, UInt_t>& chMap);
    void ProcessDigit(BmnADC32Digit* adcDig, BmnGemMapping* gemM, TClonesArray *gem);
    BmnStatus SearchInMap(map<UInt_t, UInt_t>* m, Int_t& strip, UInt_t ch);
    UInt_t SearchPed(UInt_t ch, UInt_t ser);
    UInt_t SearchNoise(UInt_t chn, UInt_t ser);
    UInt_t SearchComMod(UInt_t chn, UInt_t ser);
        
    ifstream fMapFile;
    TString fMapFileName;

    ClassDef(BmnGemRaw2Digit, 1);
};

#endif /* BMNGEMRAW2DIGIT_H */

