#ifndef BMNRAWDATADECODER_H
#define BMNRAWDATADECODER_H 1

#include "TString.h"
#include "TSystem.h"
#include "BmnEnums.h"
#include "BmnTTBDigit.h"
#include "BmnTDCDigit.h"
#include "BmnHRBDigit.h"
#include "BmnADCDigit.h"
#include "BmnSyncDigit.h"
#include "TFile.h"
#include "TTree.h"
#include "TClonesArray.h"
#include <iostream>
#include <vector>
#include <fstream>
#include "BmnGemRaw2Digit.h"
#include "BmnGemStripDigit.h"
#include "BmnMwpcRaw2Digit.h"
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

// wait limit for input data (ms)
#define WAIT_LIMIT 45000000
using namespace std;

struct digiArrays {
    TClonesArray *gem;
    TClonesArray *tof400;
    TClonesArray *tof700;
    TClonesArray *dch;
    TClonesArray *mwpc;
    TClonesArray *t0;
    TClonesArray *bc1;
    TClonesArray *bc2;
    TClonesArray *veto;
    TClonesArray *fd;
    TClonesArray *bd;
    //header array
    TClonesArray *header;
};

class DigiArrays : public TObject {
public:
    DigiArrays(){
        gem = NULL;
        tof400 = NULL;
        tof700 = NULL;
        dch = NULL;
        mwpc = NULL;
        t0 = NULL;
        bc1 = NULL;
        bc2 = NULL;
        veto = NULL;
        fd = NULL;
        bd = NULL;
        header = NULL;
    };
    ~DigiArrays(){};
    void Clear(){
        delete this->bc1;
        delete this->bc2;
        delete this->bd;
        delete this->dch;
        delete this->fd;
        delete this->gem;
        delete this->header;
        delete this->mwpc;
        delete this->t0;
        delete this->tof400;
        delete this->tof700;
        delete this->veto;
    };
    TClonesArray *gem;//->
    TClonesArray *tof400;//->
    TClonesArray *tof700;//->
    TClonesArray *dch;//->
    TClonesArray *mwpc;//->
    TClonesArray *t0;//->
    TClonesArray *bc1;//->
    TClonesArray *bc2;//->
    TClonesArray *veto;//->
    TClonesArray *fd;//->
    TClonesArray *bd;//->
    TClonesArray *header;//->
private:
    ClassDef(DigiArrays, 1)
};
    ClassImp(DigiArrays)

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
    BmnStatus SlewingTOF700Init();
    BmnStatus SlewingTOF700();

    void SetQue(deque<UInt_t> *v) {
        fDataQueue = v;
    }

    deque<UInt_t> *GetQue() {
        return fDataQueue;
    }

    struct digiArrays GetDigiArrays() {
        struct digiArrays d;
        d.gem = gem;
        d.tof400 = tof400;
        d.tof700 = tof700;
        d.dch = dch;
        d.mwpc = mwpc;
        d.t0 = t0;
        d.bc1 = bc1;
        d.bc2 = bc2;
        d.veto = veto;
        d.fd = fd;
        d.bd = bd;
        d.header = eventHeader;
        return d;
    }
    
    
    DigiArrays GetDigiArraysObject() {
//        fDigiTree->GetEntry(GetEventId());
        DigiArrays d;// = new DigiArrays();
        d.gem = (TClonesArray*)gem;
        d.tof400 = (TClonesArray*)tof400;
        d.tof700 = (TClonesArray*)tof700;
        d.dch = (TClonesArray*)dch;
        d.mwpc = (TClonesArray*)mwpc;
        d.t0 = (TClonesArray*)t0;
        d.bc1 = (TClonesArray*)bc1;
        d.bc2 = (TClonesArray*)bc2;
        d.veto = (TClonesArray*)veto;
        d.fd = (TClonesArray*)fd;
        d.bd = (TClonesArray*)bd;
        d.header = (TClonesArray*)eventHeader;
        return d;
    }
    

    TTree* GetDigiTree() {
        return fDigiTree;
    }

    void SetRunId(UInt_t v) {
        fRunId = v;
    }
    
    void SetPeriodId(UInt_t v) {
        fPeriodId = v;
    }

    map<UInt_t, Long64_t> GetTimeShifts() {
        return fTimeShifts;
    }

    UInt_t GetRunId() const {
        return fRunId;
    }
    
    UInt_t GetPeriodId() const {
        return fPeriodId;
    }

    UInt_t GetNevents() const {
        return fNevents;
    }
    
    UInt_t GetEventId() const {
        return fEventId;
    }

    BmnTof2Raw2DigitNew *GetTof700Mapper() {
        return fTof700Mapper;
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

    void SetMwpcMapping(TString map) {
        fMwpcMapFileName = map;
    }

    void SetGemMapping(TString map) {
        fGemMapFileName = map;
    }

    void SetTof400Mapping(TString PlaceMap, TString StripMap) {
        fTof400PlaceMapFileName = PlaceMap;
        fTof400StripMapFileName = StripMap;
    }

    void SetTof700Mapping(TString map) {
        fTof700MapFileName = map;
    }

    TString GetRootFileName() {
        return fRootFileName;
    }

private:

    Int_t GetRunIdFromFile(TString name);
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
    TString fMwpcMapFileName;
    TString fGemMapFileName;
    TString fTof400PlaceMapFileName;
    TString fTof400StripMapFileName;
    TString fTof700MapFileName;
    TString fTrigMapFileName;
    TString fTrigINLFileName;

    ifstream fDchMapFile;
    ifstream fMwpcMapFile;
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
    TClonesArray *mwpc;
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
    BmnMwpcRaw2Digit *fMwpcMapper;
    BmnTrigRaw2Digit *fTrigMapper;
    BmnTof1Raw2Digit *fTof400Mapper;
    BmnTof2Raw2DigitNew *fTof700Mapper;
    BmnEventType fCurEventType;
    BmnEventType fPrevEventType;
    UInt_t fPedEvCntr;
    GemMapStructure* fGemMap;
    TriggerMapStructure* fT0Map;
    deque<UInt_t> *fDataQueue;

    //Map to store pairs <Crate serial> - <crate time - T0 time>
    map<UInt_t, Long64_t> fTimeShifts;
    Double_t fT0Time; //ns
    Double_t fT0Width; //ns

    BmnStatus ProcessEvent(UInt_t *data, UInt_t len);
    BmnStatus Process_ADC64VE(UInt_t *data, UInt_t len, UInt_t serial, UInt_t nSmpl, TClonesArray *arr);
    BmnStatus Process_FVME(UInt_t *data, UInt_t len, UInt_t serial, BmnEventType &ped, BmnTriggerType &trig);
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

#endif