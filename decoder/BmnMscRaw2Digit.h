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
//#include  "db_structures.h"
#include  "ElogRecord.h"
#include  "UniRun.h"
#include  "UniParser.h"
#include  "UniSearchCondition.h"

using namespace std;
using namespace TMath;

struct MscMap {
    UInt_t serial;
    UShort_t slot;

    UShort_t BT; ///< BeamTrigger index
    UShort_t BTnBusy; ///< (BeamTrigger & !Busy)
    UShort_t L0;
    UShort_t TriggerProtection;
    UShort_t BC1;
    UShort_t BC2;
    UShort_t BC3;
    UShort_t BC1H; ///< BC1 (high threshold)
    UShort_t BC1BP; ///< BC1 (before protection)
    UShort_t BC1xBC2;
    UShort_t BC1nBusy;
    UShort_t IntTrig;
    UShort_t SRCTrig;
    UShort_t TrignBusy; ///< Trigger * !Busy

};

class BmnMscRaw2Digit {

public:
    BmnMscRaw2Digit(Int_t period, Int_t run, TString mappingFile, TTree *spillTree = nullptr, TTree *digiSpillTree = nullptr);

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
    BmnStatus SumEvent7(TClonesArray *msc, BmnEventHeader *hdr, BmnSpillHeader *sh, UInt_t &nPedEvBySpill);
    BmnStatus SumEventTemp(TClonesArray *msc);

    BmnStatus ParseTxtSpillLog(TString LogName, TString SchemeName);

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
    BmnSetup fBmnSetup;
    Bool_t isValidSpillLog;
    UInt_t fPeriodId;
    UInt_t fRunId;
    map<TDatime, vector < Int_t> > spill_map;
    UInt_t iSpillMap = 0u;
    TDatime dtStart;
    TDatime dtEnd;
    Int_t fLogShift;

    Int_t fVerbose = 0;

    BmnTrigInfo fTempTI;


    vector<MscMap> fMap;
    ifstream fMapFile;
    TString fMapFileName;

    map<UInt_t, vector<uint64_t> > fBoardSums;

    TTree *fRawSpillTree = nullptr;
    TTree *fDigSpillTree = nullptr;
    UInt_t iSpill = 0u;

    ULong64_t fBT = 0u;
    ULong64_t fBTnBusy = 0u;
    ULong64_t fAccepted = 0u;
    ULong64_t fProtection = 0u;
    ULong64_t fL0 = 0u;
    Double_t fBTAccepted = 0.0;
    ULong64_t fBC1 = 0u;
    ULong64_t fBC2 = 0u;
    ULong64_t fBC3 = 0u;

    ClassDef(BmnMscRaw2Digit, 1);
};

#endif /* BMNMSCRAW2DIGIT_H */

