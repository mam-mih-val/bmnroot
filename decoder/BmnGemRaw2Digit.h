#ifndef BMNGEMRAW2DIGIT_H
#define BMNGEMRAW2DIGIT_H

#include "TString.h"
#include "TTree.h"
#include "TClonesArray.h"
#include "TH1F.h"
#include <iostream>
#include "BmnGemStripDigit.h"
#include "BmnADCDigit.h"
#include "BmnEnums.h"
#include "TMath.h"
#include <fstream>
#include <list>
#include <map>
#include "BmnAdcProcessor.h"
#include <vector>
#include <UniDbDetectorParameter.h>
#include <UniDbDetector.h>

#define N_CH_IN_CRATE 2048 //number of channels in one crate (64ch x 32smpl))
#define N_CH_IN_SMALL_GEM 512 //number of channels in small GEM stations (sum of all redout channels)
#define N_CH_IN_MID_GEM 2176 //number of channels in middle GEM stations (sum of all redout channels)
#define N_CH_IN_BIG_GEM 3200 //number of channels in one part of big GEM stations (sum of all redout channels)
#define N_CH_IN_BIG_GEM_0 1023//988 //number of channels in hot zone of one part of big GEM stations (sum of redout channels from X0 and Y0)
#define N_CH_IN_BIG_GEM_1 2176//2100 //number of channels in big zone of one part of big GEM stations (sum of redout channels from X1 and Y1)
#define N_CH_BUF 4096
#define N_MODULES 2
#define N_LAYERS 4

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

class BmnGemRaw2Digit : public BmnAdcProcessor {
public:
    BmnGemRaw2Digit(Int_t period, Int_t run, vector<UInt_t> vSer, TString mapFileName);
    BmnGemRaw2Digit();
    ~BmnGemRaw2Digit();

    BmnStatus FillEvent(TClonesArray *adc, TClonesArray *gem);
    BmnStatus FillProfiles(TClonesArray *adc);
    BmnStatus FillNoisyChannels();

private:

    BmnGemMap* fSmall;
    BmnGemMap* fMid;
    BmnGemMap* fBigL0;
    BmnGemMap* fBigL1;
    BmnGemMap* fBigR0;
    BmnGemMap* fBigR1;
        
    TH1F**** fSigProf;
    Bool_t**** fNoisyChannels;
    
    TString fMapFileName;

    vector<GemMapStructure> fMap;

    void ProcessDigit(BmnADCDigit* adcDig, GemMapStructure* gemM, TClonesArray *gem, Bool_t doFill);
    BmnStatus ReadMap(TString parName, BmnGemMap* m, Int_t lay, Int_t mod);

    Int_t fEntriesInGlobMap; // number of entries in BD table for Global Mapping

    Int_t fEventId;

    ClassDef(BmnGemRaw2Digit, 1);
};

#endif /* BMNGEMRAW2DIGIT_H */

