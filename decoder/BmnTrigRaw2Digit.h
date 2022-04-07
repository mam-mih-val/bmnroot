#ifndef BMNTRIGRAWTODIGIT_H
#define BMNTRIGRAWTODIGIT_H

#include "BmnEnums.h"
#include "TString.h"
#include "TClonesArray.h"
#include "TPRegexp.h"
#include "TTree.h"
#include "BmnTDCDigit.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <vector>
#include <regex>
#include "BmnTrigDigit.h"
#include "BmnTrigWaveDigit.h"
#include "TMath.h"
#include "BmnTQDCADCDigit.h"

#define CHANNEL_COUNT_MAX      72u
#define TDC72_CHANNEL_COUNT    72u
#define TQDC16_CHANNEL_COUNT   16u
#define TDC_BIN_COUNT        1024
#define HPTIMEBIN       0.02344
#define INVHPTIMEBIN    42.6666
#define ADC_CLOCK_TQDC16VS  8.0  // nano seconds
#define ADC_CLOCK          12.0
#define ADC_CLOCK_OLD      12.5
#define TDC_CLOCK          24.0
#define TDC_CLOCK_OLD      25.0
const UShort_t kNCHANNELS = 8; // number of channels in one HPTDC

using namespace std;
using namespace TMath;

typedef pair<UInt_t, UChar_t> PlMapKey;

struct BmnTrigChannelData {
    TString name;
    UInt_t module;
    UInt_t serial;
    UShort_t slot;
    UShort_t channel;
    Bool_t isNegative;
    TClonesArray* branchArrayPtr;
};

struct BmnTrigParameters {
    TString name;
    UInt_t BoardSerial;
    UInt_t CrateSerial;
    UShort_t slot;
    UShort_t ChannelCount;
    Double_t INL[CHANNEL_COUNT_MAX][TDC_BIN_COUNT];
    UShort_t ChannelMap[CHANNEL_COUNT_MAX];
    Bool_t NegativeMap[CHANNEL_COUNT_MAX];
    Bool_t IsT0;
    TClonesArray * branchArrayPtr[CHANNEL_COUNT_MAX];
    Double_t t[CHANNEL_COUNT_MAX];
    BmnTrigParameters();
};

class BmnTrigRaw2Digit {
public:
    BmnTrigRaw2Digit(TString mappingFile, TString INLFile, TTree *digiTree = NULL);

    BmnTrigRaw2Digit() {
    }

    ~BmnTrigRaw2Digit() {
        for (TClonesArray *ar : trigArrays)
            delete ar;
        //        for (auto el : fMap)
        //            delete el;
        fMap.clear();
        for (auto &el : fPlacementMap) {
            delete el.second;
        }
        fPlacementMap.clear();
    };

    vector<BmnTrigChannelData>* GetMap() {
        return &fMap;
    }

    BmnStatus FillEvent(TClonesArray *tdc, map<UInt_t, Long64_t> & tsMap);
    BmnStatus FillEvent(TClonesArray *tdc, TClonesArray *adc, map<UInt_t, Long64_t> & tsMap);
    BmnStatus ReadINLFromFile(BmnTrigParameters* par);
    BmnStatus ReadChannelMap(TString mappingFile);
    BmnStatus ReadPlacementMap(TString mappingFile);
    BmnStatus ClearArrays();

    vector<TClonesArray*> *GetTrigArrays() {
        return &trigArrays;
    }

    UInt_t GetT0Serial() {
        for (auto itPl : fPlacementMap)
            if (itPl.second->IsT0)
                return itPl.second->CrateSerial;
        return 0;
    }

    void SetSetup(BmnSetup stp) {
        fSetup = stp;
    }

private:

    TString Serial2FileName(TString boardName, UInt_t serial) {
        UShort_t h = serial >> 16;
        UShort_t l = serial & 0x0000FFFF;
        TString inlFileName = Form("%s-%04X-%04X.ini", boardName.Data(), h, l);
        return inlFileName;
    }

    UInt_t ChanCntByName(TString channelCountStr) {
        //    regex reBoardName("(\\D+)(\\d+)(.*)");
        TPRegexp reBoardName("(\\D+)(\\d+)(.*)");
        //        string channelCountStr = name;
        UInt_t channelCount = CHANNEL_COUNT_MAX;
        if (reBoardName.MatchB(channelCountStr)) {
            //        if (regex_match(name, reBoardName)){
            //            channelCountStr = regex_replace(name, reBoardName, "$2");
            reBoardName.Substitute(channelCountStr, "$2");
            channelCount = strtoul(channelCountStr.Data(), nullptr, 10);
            //            channelCount = strtoul(channelCountStr.c_str(), nullptr, 10);
        }
        return channelCount;
    }
    
    void ProcessWave(Short_t *iValue, const uint16_t &nVals, Bool_t &isNeg);

    map< PlMapKey, BmnTrigParameters*> fPlacementMap;
    vector<BmnTrigChannelData> fMap;
    BmnSetup fSetup;
    ifstream fMapFile;
    ifstream fINLFile;
    TString fMapFileName;
    TString fINLFileName;
    Float_t fINLTable[72][1024];
    //    TDirectory *fDir;
    vector<TClonesArray*> trigArrays;

    ClassDef(BmnTrigRaw2Digit, 1);
};

#endif /* BMNTRIGRAWTODIGIT_H */

