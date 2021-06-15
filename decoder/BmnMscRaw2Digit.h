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

#include "BmnEventHeader.h"
#include "BmnSpillHeader.h"
#include "BmnEnums.h"
#include "BmnMath.h"
#include "BmnMSCDigit.h"
#include "BmnTrigInfo.h"
#include "DigiRunHeader.h"

using namespace std;
using namespace TMath;

struct MscMap {
    UInt_t serial;
    UShort_t slot;

    UShort_t BT; ///< BeamTrigger index
    UShort_t BTnBusy; ///< (BeamTrigger & not Busy) index
    UShort_t L0;
    UShort_t TriggerProtection;
    UShort_t BC1;
    UShort_t BC2;
    UShort_t BC3;
};

class BmnMscRaw2Digit {
    const UInt_t nCnt = 16;

public:
    BmnMscRaw2Digit(TString mappingFile, TTree *spillTree = nullptr, TTree *digiSpillTree = nullptr);

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

    BmnStatus SumEvent(TClonesArray *msc, BmnEventHeader *hdr, BmnSpillHeader *sh, UInt_t &nPedEvBySpill);

    TTree *GetRawSpillTree() {
        return fRawSpillTree;
    }
    TTree *GetDigSpillTree() {
        return fDigSpillTree;
    }

    void SetRawSpillTree(TTree *tree) {
        fRawSpillTree = tree;
        if (fRawSpillTree)
            fRawSpillTree->GetEntry(iSpill);
    }
    void SetDigSpillTree(TTree *tree) {
        fDigSpillTree = tree;
    }

private:
    vector<MscMap> fMap;
    ifstream fMapFile;
    TString fMapFileName;
    
    TTree *fRawSpillTree = nullptr;
    TTree *fDigSpillTree = nullptr;
    UInt_t iSpill = 0u;

    UInt_t fBT = 0u;
    UInt_t fBTnBusy = 0u;
    UInt_t fAccepted = 0u;
    Double_t fBTAccepted = 0.0;
    UInt_t fBC1 = 0.0;
    UInt_t fBC2 = 0.0;
    UInt_t fBC3 = 0.0;

    ClassDef(BmnMscRaw2Digit, 1);
};

#endif /* BMNMSCRAW2DIGIT_H */

