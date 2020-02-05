#ifndef BMNMSCRAW2DIGIT_H
#define BMNMSCRAW2DIGIT_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <vector>

#include <TClonesArray.h>
#include <TString.h>
#include <TTree.h>

#include "BmnEnums.h"
#include "BmnMSCDigit.h"
#include "BmnTrigInfo.h"
#include "DigiRunHeader.h"

using namespace std;
using namespace TMath;

struct MscMap {
    UInt_t serial;
    UShort_t slot;

    UShort_t BT; ///< BeamTrigger
    UShort_t BTnBusy; ///< BeamTrigger & not Busy
    UShort_t L0;
    UShort_t TriggerProtection;
};

class BmnMscRaw2Digit {
    const UInt_t nCnt = 16;

public:
    BmnMscRaw2Digit(TString mappingFile, TTree *spillTree = nullptr);

    BmnMscRaw2Digit() {
    }

    ~BmnMscRaw2Digit() {
        fMap.clear();
    };
    //    vector<MscMap>* GetMap() {
    //        return &fMap;
    //    }

    BmnStatus ReadChannelMap(TString mappingFile);

    void FillRunHeader(DigiRunHeader *rh);

    BmnStatus SumEvent(TClonesArray *msc, BmnTrigInfo *ti, UInt_t iEv);

    TTree *GetSpillTree() {
        return fRawTreeSpills;
    }

    void SetSpillTree(TTree *tree) {
        fRawTreeSpills = tree;
        if (fRawTreeSpills)
            fRawTreeSpills->GetEntry(iSpill);
    }

private:
    vector<MscMap> fMap;
    ifstream fMapFile;
    TString fMapFileName;
    
    TTree *fRawTreeSpills = nullptr;
    UInt_t iSpill = 0u;

    UInt_t fBT = 0u;
    UInt_t fBTnBusy = 0u;
    Double_t fBTAccepted = 0.0;

    ClassDef(BmnMscRaw2Digit, 1);
};

#endif /* BMNMSCRAW2DIGIT_H */

