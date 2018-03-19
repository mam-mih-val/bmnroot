#ifndef BMNTRIGRAWTODIGIT_H
#define BMNTRIGRAWTODIGIT_H

#include "BmnEnums.h"
#include "TString.h"
#include "TClonesArray.h"
#include "TTree.h"
#include "BmnTDCDigit.h"
#include <iostream>
#include <fstream>
#include <map>
#include <vector>
#include "BmnTrigDigit.h"
#include "BmnTrigWaveDigit.h"
#include "TMath.h"

#define HPTIMEBIN 0.02344
#define INVHPTIMEBIN 42.6666
#define KNBDCHANNELS 40
#define ADC_CLOCK_TQDC16VS  8  // nano seconds
#define ADC_CLOCK          12
#define ADC_CLOCK_OLD    12.5
#define TDC_CLOCK          24
#define TDC_CLOCK_OLD      25

using namespace std;
using namespace TMath;

struct BmnTrigMapping {
    TString name;
    UInt_t module;
    UInt_t serial;
    Short_t slot;
    Short_t channel;
    TClonesArray* branchRef;
};

class BmnTrigRaw2Digit {
public:
    BmnTrigRaw2Digit(TString mappingFile, TString INLFile);
    BmnTrigRaw2Digit(TString mappingFile, TString INLFile, TTree *digiTree);
    BmnTrigRaw2Digit(){}

    ~BmnTrigRaw2Digit() {
        for (TClonesArray *ar : trigArrays)
            delete ar;
    };

    vector<BmnTrigMapping> GetMap() const {
        return fMap;
    }

    BmnStatus FillEvent(TClonesArray *tdc);
    BmnStatus FillEvent(TClonesArray *tdc, TClonesArray *adc);
    BmnStatus readINLCorrections(TString INLFile);
    BmnStatus readMap(TString mappingFile);
    BmnStatus ClearArrays();
    
    vector<TClonesArray*> *GetTrigArrays(){
        return &trigArrays;
    }
    
    BmnTrigMapping GetT0Map(){        
        for (BmnTrigMapping tM : fMap){
            if (tM.name == "T0")
                return tM;
        }
        for (BmnTrigMapping tM : fMap){
            if (tM.name == "BC2")
                return tM;
        }
        BmnTrigMapping tMno;
        tMno.serial = 0;
        return tMno;
    }
    
    void SetSetup(BmnSetup stp) {
        fSetup = stp;
    }

private:

    vector<BmnTrigMapping> fMap;
    BmnSetup fSetup;
    ifstream fMapFile;
    ifstream fINLFile;
    TString fMapFileName;
    TString fINLFileName;
    Float_t fINLTable[72][1024];
//    TDirectory *fDir;
    vector<TClonesArray*> trigArrays;
    //    TString trigNames[13] = {
    //        "BC1", "BC2", "BC3", "BC4", "VC",
    //        "X1_Left", "X1_Right", "X2_Left", "X2_Right",
    //        "Y1_Left", "Y1_Right", "Y2_Left", "Y2_Right"};

    ClassDef(BmnTrigRaw2Digit, 1);
};

#endif /* BMNTRIGRAWTODIGIT_H */

