#ifndef BMNCSCRAW2DIGIT_H
#define BMNCSCRAW2DIGIT_H

#include "TString.h"
#include "TTree.h"
#include "TClonesArray.h"
#include "BmnADCDigit.h"
#include "BmnCSCDigit.h"
#include "BmnEnums.h"
#include "TMath.h"
#include "BmnMath.h"
#include "BmnAdcProcessor.h"
#include "TH1F.h"

#define N_CSC_MODULES 2
#define N_CSC_CHANNELS 2048

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

typedef map<Int_t, BmnCscMapping*> InChanMapCSC;

class BmnCscRaw2Digit : public BmnAdcProcessor {
public:
    BmnCscRaw2Digit(Int_t period, Int_t run, vector<UInt_t> vSer, TString MapFileName);
    BmnCscRaw2Digit();
    ~BmnCscRaw2Digit();

    BmnStatus FillEvent(TClonesArray *adc, TClonesArray *csc);
    BmnStatus FillProfiles(TClonesArray *adc);
    BmnStatus FillNoisyChannels();

private:

    TString fMapFileName;
//    vector<BmnCscMapping*> fMap;
    map<UInt_t, InChanMapCSC> fOuterMap; // serial map
    vector<InChanMapCSC> fMapVec; // serial map
    vector<UInt_t> fSerials;
    Int_t channel2layer[N_CSC_MODULES][N_CSC_CHANNELS];
    Int_t channel2strip[N_CSC_MODULES][N_CSC_CHANNELS];
    Int_t fEventId;
    BmnCSCStationSet* fCscStationSetDer = nullptr;

    TH1F**** fSigProf;
    Bool_t**** fNoisyChannels;

    BmnCscMapping* FindMapEntry(BmnADCDigit* adcDig);
    void ProcessDigit(BmnADCDigit* adcDig, BmnCscMapping* cscM, TClonesArray *csc, Bool_t doFill);
    void ProcessAdc(TClonesArray *adc, TClonesArray *csc, Bool_t doFill);
    BmnStatus ReadMapFile();
    BmnStatus ReadMapLocalFile();
    inline Int_t LayerPrediction(Int_t module, Int_t x);
    inline void MapStrip(BmnCscMapping* cscM, UInt_t iCh, Int_t iSmpl, Int_t &station, Int_t &module, Int_t &layer, Int_t &strip);

    ClassDef(BmnCscRaw2Digit, 2);
};

#endif /* BMNCSCRAW2DIGIT_H */

