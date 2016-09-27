#include "TString.h"
#include "BmnEnums.h"
#include "BmnTTBDigit.h"
#include "BmnTDCDigit.h"
#include "BmnADCDigit.h"
#include "BmnADC32Digit.h"
#include "BmnSyncDigit.h"
#include "TFile.h"
#include "TTree.h"
#include "TClonesArray.h"
#include <iostream>
#include <vector>
#include <fstream>
#include "BmnGemRaw2Digit.h"
#include "BmnGemStripDigit.h"
#include "BmnDchRaw2Digit.h"
#include "BmnTof1Raw2Digit.h"
#include "BmnTrigRaw2Digit.h"
#include "BmnEventHeader.h"
#include <bitset>
#include <stdio.h>
#include <stdlib.h>
#include <cstdlib>
#include <cstdio>

class BmnRawDataDecoder {
public:
    BmnRawDataDecoder(TString file, ULong_t nEvents = 0);
    BmnRawDataDecoder();
    virtual ~BmnRawDataDecoder();

    BmnStatus ConvertRawToRoot();
    BmnStatus DecodeDataToDigi();
    BmnStatus CalcGemPedestals();

    UInt_t GetRunId() const {
        return fRunId;
    }

    UInt_t GetNevents() const {
        return fNevents;
    }

    void SetTrigMapping(TString map) {
        fTrigMapFileName = map;
    }
    
    void SetDchMapping(TString map) {
        fDchMapFileName = map;
    }

    void SetGemMapping(TString map) {
        fGemMapFileName = map;
    }

    void SetTof400Mapping(TString map) {
        fTof400MapFileName = map;
    }

    void SetTof700Mapping(TString map) {
        fTof700MapFileName = map;
    }
    
    void SetPedestalRun(Bool_t ped) {
        fPedestalRun = ped;
    }


private:

    UInt_t fRunId;
    UInt_t fEventId;
    UInt_t fNevents;
    UInt_t fTime_s;
    UInt_t fTime_ns;
    
    Long64_t fLengthRawFile;
    Long64_t fCurentPositionRawFile;

    TTree *fRawTree;
    TTree *fDigiTree;
    TString fRootFileName;
    TString fRawFileName;
    TString fDigiFileName;
    TString fDchMapFileName;
    TString fGemMapFileName;
    TString fTof400MapFileName;
    TString fTof700MapFileName;
    TString fTrigMapFileName;
    
    ifstream fDchMapFile;
    ifstream fGemMapFile;
    ifstream fTof400MapFile;
    ifstream fTof700MapFile;
    ifstream fTrigMapFile;
    
    TFile *fRootFileIn;
    TFile *fRootFileOut;
    TFile *fDigiFileOut;
    FILE *fRawFileIn;

    //DAQ arrays
    TClonesArray *sync;
    TClonesArray *adc;
    TClonesArray *tdc;
    TClonesArray *msc;

    //Digi arrays
    TClonesArray *gem;
    TClonesArray *tof400;
    TClonesArray *tof700;
    TClonesArray *dch;
    TClonesArray *t0;
    TClonesArray *bc1;
    TClonesArray *bc2;
    TClonesArray *veto;
    //header array
    TClonesArray *header;

    UInt_t data[1000000];
    ULong_t fMaxEvent;
    
    Bool_t fPedestalRun;

    BmnStatus ProcessEvent(UInt_t *data, UInt_t len);

    BmnStatus Process_ADC64VE(UInt_t *data, UInt_t len, UInt_t serial);
    BmnStatus Process_FVME(UInt_t *data, UInt_t len, UInt_t serial);

    BmnStatus FillTDC(UInt_t *d, UInt_t serial, UInt_t slot, UInt_t modId, UInt_t &idx);
    BmnStatus FillTRIG(UInt_t *d, UInt_t serial, UInt_t &idx);
    BmnStatus FillMSC(UInt_t *d, UInt_t serial, UInt_t &idx);
};
