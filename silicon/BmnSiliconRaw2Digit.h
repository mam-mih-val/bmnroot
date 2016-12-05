#ifndef BMNSILICONRAW2DIGIT_H
#define BMNSILICONRAW2DIGIT_H

#include "TString.h"
#include "TTree.h"
#include "TClonesArray.h"
#include <iostream>
#include "BmnADC128Digit.h"
#include "BmnEnums.h"
#include "TMath.h"
#include <fstream>
#include <list>
#include <map>
#include <vector>
#include <UniDbDetectorParameter.h>

#define ADC128_N_SAMPLES 128 //number of samples in one ADC digit

using namespace std;
using namespace TMath;

class BmnSiliconRaw2Digit {
public:
    BmnSiliconRaw2Digit(Int_t period, Int_t run);
    BmnSiliconRaw2Digit();
    virtual ~BmnSiliconRaw2Digit();

    BmnStatus FillEvent(TClonesArray *adc, TClonesArray *sts);

private:

    UInt_t* fCrates;
    Int_t fPeriod;
    Int_t fRun;
    Int_t fNCrates;
    Int_t fEventId;

    ClassDef(BmnSiliconRaw2Digit, 1);
};

#endif /* BMNSILICONRAW2DIGIT_H */

