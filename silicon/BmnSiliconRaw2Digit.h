#ifndef BMNSILICONRAW2DIGIT_H
#define BMNSILICONRAW2DIGIT_H

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
#include "BmnSiliconDigit.h"

#define ADC128_N_SAMPLES 128 //number of samples in one ADC digit

using namespace std;
using namespace TMath;

struct BmnSiliconMapping {
    Short_t layer; //X = 0 or X' = 1
    UInt_t serial;
    Short_t module;
    Short_t channel;
    Int_t start_strip;
};

class BmnSiliconRaw2Digit {
public:
    BmnSiliconRaw2Digit(Int_t period, Int_t run, TString name);
    BmnSiliconRaw2Digit();
    virtual ~BmnSiliconRaw2Digit();

    BmnStatus FillEvent(TClonesArray *adc, TClonesArray *sts);

private:

    vector<BmnSiliconMapping> fMap;
    UInt_t* fCrates;
    Int_t fPeriod;
    Int_t fRun;
    Int_t fNCrates;
    Int_t fEventId;
    
    BmnStatus ReadMapFile(TString fName);

    ClassDef(BmnSiliconRaw2Digit, 1);
};

#endif /* BMNSILICONRAW2DIGIT_H */

