#ifndef BMNGEMRAW2DIGIT_H
#define BMNGEMRAW2DIGIT_H

#include "TString.h"
#include "TTree.h"
#include "TClonesArray.h"
#include "TH1F.h"
#include <iostream>
#include "BmnGemStripDigit.h"
#include "BmnADCDigit.h"
#include "TMath.h"
#include <fstream>
#include <list>
#include <map>
#include "BmnAdcProcessor.h"
#include <vector>
#include <UniDetectorParameter.h>
#include <UniDetector.h>

#include <TSystem.h>
#include <BmnEventHeader.h>
#include "BmnCSCDigit.h"
#include <BmnFunctionSet.h>

#define N_CH_IN_CRATE 2048 //number of channels in one crate (64ch x 32smpl))
#define N_CH_IN_SMALL_GEM 512 //number of channels in small GEM stations (sum of all redout channels)
#define N_CH_IN_MID_GEM 2176 //number of channels in middle GEM stations (sum of all redout channels)
#define N_CH_IN_BIG_GEM 3200 //number of channels in one part of big GEM stations (sum of all redout channels)
#define N_CH_IN_BIG_GEM_0 1023//988 //number of channels in hot zone of one part of big GEM stations (sum of redout channels from X0 and Y0)
#define N_CH_IN_BIG_GEM_1 2176//2100 //number of channels in big zone of one part of big GEM stations (sum of redout channels from X1 and Y1)
#define N_CH_BUF 4096
#define N_MODULES 2
#define N_LAYERS 4
//MK Real time 14122.027469 s, CPU time 8545.100000 s
//SM Real time  8283.128545 s, CPU time 7648.220000 s

using namespace std;
using namespace TMath;

struct BmnGemMap {
    Int_t strip;
    Int_t lay; // strip type: 0 - x, 1 - y
    Int_t mod; //hot zones: 1 - inner zone, 0 - outer zone

    BmnGemMap(Int_t s, Int_t l, Int_t m) : strip(s), lay(l), mod(m) {}

    BmnGemMap() : strip(-1), lay(-1), mod(-1) {}
};

struct GemMapLine {
    Int_t Serial;
    Int_t Ch_lo;
    Int_t Ch_hi;
    Int_t GEM_id;
    Int_t Side;
    Int_t Type;
    Int_t Station;
    Int_t Module;
    Int_t Zone;

    void Print() {
        cout << Serial << " " << Ch_lo << " " << Ch_hi << " " << GEM_id << " " << Side << " " << Type << " " << Station << " " << Module << " " << Zone << endl;
    }

    GemMapLine() :
        Serial(-1),
        Ch_lo(-1),
        Ch_hi(-1),
        GEM_id(-1),
        Side(-1),
        Type(-1),
        Station(-1),
        Module(-1),
        Zone(-1) {}
};

class BmnGemRaw2Digit : public BmnAdcProcessor {
public:
    BmnGemRaw2Digit(Int_t period, Int_t run, vector<UInt_t> vSer, TString mapFileName, BmnSetup bmnSetup = kBMNSETUP, BmnADCDecoMode decoMode = kBMNADCMK);
    BmnGemRaw2Digit();
    ~BmnGemRaw2Digit();

    BmnStatus FillEvent(TClonesArray* adc, TClonesArray* gem);
    BmnStatus FillProfiles(TClonesArray* adc);
    BmnStatus FillNoisyChannels();

private:

    // starting thresholds, number of iterations
    Int_t niterped = 3;
    Float_t thrped = 35;
    Float_t thrpedcsc = 80;
    vector<TH1I*> hNhits;

    BmnGemMap* fSmall;
    BmnGemMap* fMid;
    vector<BmnGemMap*> fBigHot;
    vector<BmnGemMap*> fBig;

    TH1F**** fSigProf;
    Bool_t**** fNoisyChannels;

    TString fMapFileName;



    vector<GemMapLine*> fMap;
    //vector<GemMapValue*> fMap;
    inline void MapStrip(GemMapLine* gemM, UInt_t ch, Int_t iSmpl, Int_t& station, Int_t& mod, Int_t& lay, Int_t& strip);
    void ProcessDigit(BmnADCDigit* adcDig, GemMapLine* gemM, TClonesArray* gem, Bool_t doFill);
    void ProcessAdc(TClonesArray* silicon, Bool_t doFill);
    BmnStatus ReadMap(TString parName, BmnGemMap* m, Int_t lay, Int_t mod);
    BmnStatus ReadLocalMap(TString parName, BmnGemMap* m, Int_t lay, Int_t mod);
    BmnStatus ReadGlobalMap(TString FileName);

    Int_t fEntriesInGlobMap; // number of entries in BD table for Global Mapping

    Int_t fEventId;
    BmnGemStripStationSet* fGemStationSetDer = nullptr;

    ClassDef(BmnGemRaw2Digit, 1);
};

#endif /* BMNGEMRAW2DIGIT_H */

