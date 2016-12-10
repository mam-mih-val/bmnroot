#include "TString.h"
#include "TSystem.h"
#include "BmnEnums.h"
#include "BmnTTBDigit.h"
#include "BmnTDCDigit.h"
#include "BmnHRBDigit.h"
#include "BmnADCDigit.h"
#include "BmnADC32Digit.h"
#include "BmnADC128Digit.h"
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
#include "BmnSiliconRaw2Digit.h"
#include "BmnTof1Raw2Digit.h"
#include "BmnTof2Raw2DigitNew.h"
#include "BmnTrigRaw2Digit.h"
#include "BmnEventHeader.h"
#include "BmnRunHeader.h"
#include "BmnEnums.h"
#include <bitset>
#include <stdio.h>
#include <stdlib.h>
#include <cstdlib>
#include <cstdio>
#include <list>
#include <map>
#include <deque>
#include <UniDbDetectorParameter.h>

// wait limit for input data
#define WAIT_LIMIT 45000000
using namespace std;

struct DigiArrays {
    TClonesArray *gem;
    TClonesArray *tof400;
    TClonesArray *tof700;
    TClonesArray *dch;
    TClonesArray *t0;
    TClonesArray *bc1;
    TClonesArray *bc2;
    TClonesArray *veto;
    TClonesArray *fd;
    TClonesArray *bd;
    //header array
    TClonesArray *header;
};

class BmnRawDataDecoder {
public:
    BmnRawDataDecoder(TString file, ULong_t nEvents = 0, ULong_t period = 4);
    BmnRawDataDecoder();
    virtual ~BmnRawDataDecoder();

    BmnStatus ConvertRawToRoot();
    BmnStatus ConvertRawToRootIterate();
    BmnStatus ConvertRawToRootIterateFile();
    BmnStatus ClearArrays();
    BmnStatus DecodeDataToDigi();
    BmnStatus DecodeDataToDigiIterate();
    BmnStatus CalcGemPedestals();
    BmnStatus InitConverter();
    BmnStatus InitConverter(deque<UInt_t> *dq);
    BmnStatus DisposeConverter();
    BmnStatus InitDecoder();
    void ResetDecoder(TString file);
    BmnStatus DisposeDecoder();
    BmnStatus wait_stream(deque<UInt_t> *que, Int_t len);
    BmnStatus wait_file(Int_t len);
    BmnStatus SlewingTOF700();

    void SetQue(deque<UInt_t> *v) {
        fDataQueue = v;
    }

    deque<UInt_t> *GetQue() {
        return fDataQueue;
    }

    struct DigiArrays GetDigiArrays() {
        struct DigiArrays d;
        d.gem = gem;
        d.tof400 = tof400;
        d.tof700 = tof700;
        d.dch = dch;
        d.t0 = t0;
        d.bc1 = bc1;
        d.bc2 = bc2;
        d.veto = veto;
        d.fd = fd;
        d.bd = bd;
        d.header = eventHeader;
        return d;
    }

    TTree* GetDigiTree() {
        return fDigiTree;
    }

    void *GetQueMutex() {
        return fDataMutex;
    }

    void SetQueMutex(void *v) {
        fDataMutex = v;
    }

    void SetRunId(UInt_t v) {
        fRunId = v;
    }

    map<UInt_t, Long64_t> GetTimeShifts() {
        return fTimeShifts;
    }

    UInt_t GetRunId() const {
        return fRunId;
    }

    UInt_t GetNevents() const {
        return fNevents;
    }
    
    UInt_t GetEventId() const {
        return fEventId;
    }

    void SetTrigMapping(TString map) {
        fTrigMapFileName = map;
    }

    void SetTrigINLFile(TString file) {
        fTrigINLFileName = file;
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

    TString GetRootFileName() {
        return fRootFileName;
    }

private:

    vector<UInt_t> fGemSerials; //list of serial id for GEM
    UInt_t fNGemSerials;

    UInt_t fPedoCounter;
    
    UInt_t fRunId;
    UInt_t fPeriodId;
    UInt_t fEventId;
    UInt_t fNevents;
    //for event
    Long64_t fTime_s;
    Long64_t fTime_ns;
    //for run
    Long64_t fTimeStart_s;
    Long64_t fTimeStart_ns;
    Long64_t fTimeFinish_s;
    Long64_t fTimeFinish_ns;

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
    TString fTrigINLFileName;

    ifstream fDchMapFile;
    ifstream fGemMapFile;
    ifstream fTof400MapFile;
    ifstream fTof700MapFile;
    ifstream fTrigMapFile;
    ifstream fTrigINLFile;

    TFile *fRootFileIn;
    TFile *fRootFileOut;
    TFile *fDigiFileOut;
    FILE *fRawFileIn;

    //DAQ arrays
    TClonesArray *sync;
    TClonesArray *adc32; //gem
    TClonesArray *adc128; //sts
    TClonesArray *hrb;
    TClonesArray *tdc;
    TClonesArray *msc;
    TClonesArray *eventHeaderDAQ;
    BmnRunHeader *runHeaderDAQ;
    //    TClonesArray *runHeaderDAQ;
    TClonesArray *pedestalAdc;

    //Digi arrays
    TClonesArray *silicon;
    TClonesArray *gem;
    TClonesArray *tof400;
    TClonesArray *tof700;
    TClonesArray *dch;
    TClonesArray *t0;
    TClonesArray *bc1;
    TClonesArray *bc2;
    TClonesArray *veto;
    TClonesArray *fd;
    TClonesArray *bd;
    //header array
    TClonesArray *eventHeader;
    BmnRunHeader *runHeader;
    //    TClonesArray *runHeader;

    UInt_t data[10000000];
    ULong_t fMaxEvent;

    UInt_t fDat; //current 32-bits word
    UInt_t syncCounter; 
    BmnGemRaw2Digit *fGemMapper;
    BmnSiliconRaw2Digit *fSiliconMapper;
    BmnDchRaw2Digit *fDchMapper;
    BmnTrigRaw2Digit *fTrigMapper;
    BmnTof1Raw2Digit *fTof400Mapper;
    BmnTof2Raw2DigitNew *fTof700Mapper;
    deque<UInt_t> *fDataQueue;
    void *fDataMutex; // actually std::mutex

    //Map to store pairs <Crate serial> - <crate time - T0 time>
    map<UInt_t, Long64_t> fTimeShifts;
    Double_t fT0Time; //ns
    Double_t fT0Width; //ns

    BmnStatus ProcessEvent(UInt_t *data, UInt_t len);
    BmnStatus Process_ADC64VE(UInt_t *data, UInt_t len, UInt_t serial, UInt_t nSmpl, TClonesArray *arr);
    BmnStatus Process_FVME(UInt_t *data, UInt_t len, UInt_t serial, BmnEventType &ped);
    BmnStatus Process_HRB(UInt_t *data, UInt_t len, UInt_t serial);
    BmnStatus FillTDC(UInt_t *d, UInt_t serial, UInt_t slot, UInt_t modId, UInt_t &idx);
    BmnStatus FillSYNC(UInt_t *d, UInt_t serial, UInt_t &idx);

    BmnStatus FillMSC(UInt_t *d, UInt_t serial, UInt_t &idx) {
        return kBMNSUCCESS;
    };
    BmnStatus FillTimeShiftsMap();
    BmnStatus FillTimeShiftsMapNoDB(UInt_t t0serial);

    BmnStatus CopyDataToPedMap(TClonesArray* adc, UInt_t ev);
};
