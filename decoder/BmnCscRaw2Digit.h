#ifndef BMNCSCRAW2DIGIT_H
#define BMNCSCRAW2DIGIT_H

#include "TString.h"
#include "TTree.h"
#include "TClonesArray.h"
#include "BmnADCDigit.h"
#include "BmnCscDigit.h"
#include "BmnEnums.h"
#include "TMath.h"
#include "BmnMath.h"
#include "BmnAdcProcessor.h"
#include <UniDbDetectorParameter.h>
#include <UniDbDetector.h>
#include "TH1F.h"

#define N_CH_BUF 4096
#define N_CSC_MODULES 2
#define N_CSC_LAYERS 4

using namespace std;
using namespace TMath;

struct BmnCscMapping {
    Short_t layer; //X = 0 or X' = 1
    UInt_t serial;
    Short_t zone;
    Short_t module;
    Int_t channel_low;
    Int_t channel_high;
    Short_t station;
};

class BmnCscRaw2Digit : public BmnAdcProcessor {
public:
    BmnCscRaw2Digit(Int_t period, Int_t run, vector<UInt_t> vSer);
    BmnCscRaw2Digit();
    ~BmnCscRaw2Digit();

    BmnStatus FillEvent(TClonesArray *adc, TClonesArray *csc);
    BmnStatus FillProfiles(TClonesArray *adc);
    BmnStatus FillNoisyChannels();

private:

    TString fMapFileName;
    vector<BmnCscMapping*> fMap;    
    vector<UInt_t> fSerials;
    Int_t fEventId; 
        
    TH1F**** fSigProf;
    Bool_t**** fNoisyChannels;

    BmnCscMapping* FindMapEntry(BmnADCDigit* adcDig);
    void ProcessDigit(BmnADCDigit* adcDig, BmnCscMapping* cscM, TClonesArray *csc, Bool_t doFill);
    BmnStatus ReadMapFile();

    ClassDef(BmnCscRaw2Digit, 2);
};

#endif /* BMNCSCRAW2DIGIT_H */

