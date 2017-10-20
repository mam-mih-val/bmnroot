#ifndef BMNRAWDATADECODER_H
#define BMNRAWDATADECODER_H 1

#include "TString.h"
#include "TSystem.h"
#include "BmnEnums.h"
#include "BmnTTBDigit.h"
#include "BmnTDCDigit.h"
#include "BmnHRBDigit.h"
#include "BmnADCDigit.h"
#include "BmnADCSRCDigit.h"
#include "BmnSyncDigit.h"
#include "TFile.h"
#include "TTimeStamp.h"
#include "TTree.h"
#include "TClonesArray.h"
#include <iostream>
#include <vector>
#include <fstream>
//#include <regex>
#include "BmnGemRaw2Digit.h"
#include "BmnGemStripDigit.h"
#include "BmnMwpcRaw2Digit.h"
#include "BmnDchRaw2Digit.h"
#include "BmnSiliconRaw2Digit.h"
#include "BmnTof1Raw2Digit.h"
#include "BmnTof2Raw2DigitNew.h"
#include "BmnZDCRaw2Digit.h"
#include "BmnECALRaw2Digit.h"
#include "BmnTrigRaw2Digit.h"
#include "BmnEventHeader.h"
#include "BmnRunHeader.h"
#include "BmnEnums.h"
#include "DigiArrays.h"
#include <bitset>
#include <stdio.h>
#include <stdlib.h>
#include <cstdlib>
#include <cstdio>
#include <list>
#include <map>
#include <deque>
#include <UniDbDetectorParameter.h>
#include <UniDbRun.h>
#include "UniDbTangoData.h"

// wait limit for input data (ms)
#define WAIT_LIMIT 40000000
using namespace std;

class BmnRawDataDecoder {
public:
    BmnRawDataDecoder(TString file, ULong_t nEvents = 0, ULong_t period = 4);
    BmnRawDataDecoder();
    virtual ~BmnRawDataDecoder();

    BmnStatus ConvertRawToRoot();
    BmnStatus ConvertRawToRootIterate();
    BmnStatus ConvertRawToRootIterateFile(UInt_t limit = WAIT_LIMIT);
    BmnStatus ClearArrays();
    BmnStatus DecodeDataToDigi();
    BmnStatus DecodeDataToDigiIterate();
    BmnStatus CalcGemPedestals();
    BmnStatus InitConverter(TString FileName);
    BmnStatus InitConverter(deque<UInt_t> *dq);
    BmnStatus InitDecoder();
    BmnStatus InitMaps();
    void ResetDecoder(TString file);
    BmnStatus DisposeDecoder();
    BmnStatus wait_stream(deque<UInt_t> *que, Int_t len, UInt_t limit = WAIT_LIMIT);
    BmnStatus wait_file(Int_t len, UInt_t limit = WAIT_LIMIT);
    BmnStatus SlewingTOF700Init();
    BmnStatus SlewingTOF700();
    BmnStatus PreparationTOF700Init() { return SlewingTOF700Init(); };
    BmnStatus PreparationTOF700();

    void SetQue(deque<UInt_t> *v) {
        fDataQueue = v;
    }

    deque<UInt_t> *GetQue() {
        return fDataQueue;
    }

    DigiArrays GetDigiArraysObject() {
        //        fDigiTree->GetEntry(GetEventId());
        DigiArrays d; // = new DigiArrays();
        d.silicon = silicon;
        d.gem = gem;
        d.tof400 = tof400;
        d.tof700 = tof700;
        d.zdc = zdc;
        d.ecal = ecal;
        d.dch = dch;
        d.mwpc = mwpc;
        d.trigger = trigger;
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

    void SetTOF700ReferenceRun(Int_t n) {
        fTOF700ReferenceRun = n;
    }

    void SetTOF700ReferenceChamber(Int_t n) {
        fTOF700ReferenceChamber = n;
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

    BmnZDCRaw2Digit *GetZDCMapper() {
        return fZDCMapper;
    }

    BmnECALRaw2Digit *GetECALMapper() {
        return fECALMapper;
    }

    void SetTrigMapping(TString map) {
        fTrigMapFileName = map;
    }
    
    void SetSiliconMapping(TString map) {
        fSiliconMapFileName = map;
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

    void SetTof700Geom(TString geom) {
        fTof700GeomFileName = geom;
    }

    void SetZDCMapping(TString map) {
        fZDCMapFileName = map;
    }

    void SetECALMapping(TString map) {
        fECALMapFileName = map;
    }

    void SetZDCCalibration(TString cal) {
        fZDCCalibrationFileName = cal;
    }

    void SetECALCalibration(TString cal) {
        fECALCalibrationFileName = cal;
    }

    TString GetRootFileName() {
        return fRootFileName;
    }

    BmnStatus SetDetectorSetup(Bool_t* setup) {
        for (Int_t i = 0; i < 9; ++i) {
            fDetectorSetup[i] = setup[i];
        }

        return kBMNSUCCESS;
    }

    void SetEvForPedestals(UInt_t v) {
        this->fEvForPedestals = v;
    }

    UInt_t GetEvForPedestals() {
        return fEvForPedestals;
    }

private:

    //9 bits correspond to detectors which we need to decode
    Bool_t fDetectorSetup[9];


    Int_t fTOF700ReferenceRun;
    Int_t fTOF700ReferenceChamber;

    Int_t GetRunIdFromFile(TString name);
    vector<UInt_t> fSiliconSerials; //list of serial id for Silicon
    UInt_t fNSiliconSerials;
    vector<UInt_t> fGemSerials; //list of serial id for GEM
    UInt_t fNGemSerials;
    vector<UInt_t> fZDCSerials; //list of serial id for ZDC
    UInt_t fNZDCSerials;
    vector<UInt_t> fECALSerials; //list of serial id for ECal
    UInt_t fNECALSerials;

    UInt_t fPedoCounter;

    UInt_t fRunId;
    TDatime fRunStartTime;
    TDatime fRunEndTime;
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
    TString fTof700GeomFileName;
    TString fZDCMapFileName;
    TString fZDCCalibrationFileName;
    TString fECALMapFileName;
    TString fECALCalibrationFileName;
    TString fSiliconMapFileName;
    TString fTrigMapFileName;
    TString fTrigINLFileName;

    ifstream fDchMapFile;
    ifstream fMwpcMapFile;
    ifstream fGemMapFile;
    ifstream fTof400MapFile;
    ifstream fTof700MapFile;
    ifstream fZDCMapFile;
    ifstream fZDCCalibraionFile;
    ifstream fECALMapFile;
    ifstream fECALCalibraionFile;
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
    TClonesArray *adc; //zdc & ecal
    TClonesArray *hrb;
    TClonesArray *tdc;
    TClonesArray *tqdc_tdc;
    TClonesArray *tqdc_adc;
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
    TClonesArray *zdc;
    TClonesArray *ecal;
    TClonesArray *dch;
    TClonesArray *mwpc;
    TClonesArray *trigger;
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
    BmnTrigRaw2Digit *fTrigSRCMapper;
    BmnTof1Raw2Digit *fTof400Mapper;
    BmnTof2Raw2DigitNew *fTof700Mapper;
    BmnZDCRaw2Digit *fZDCMapper;
    BmnECALRaw2Digit *fECALMapper;
    BmnEventType fCurEventType;
    BmnEventType fPrevEventType;
    UInt_t fPedEvCntr;
    Int_t fEvForPedestals;
    Bool_t fPedEnough;
    GemMapStructure* fGemMap;
    TriggerMapStructure* fT0Map;
    deque<UInt_t> *fDataQueue;

    //Map to store pairs <Crate serial> - <crate time - T0 time>
    map<UInt_t, Long64_t> fTimeShifts;
    Double_t fT0Time; //ns
    Double_t fT0Width; //ns

    BmnStatus ProcessEvent(UInt_t *data, UInt_t len);
    BmnStatus Process_ADC64VE(UInt_t *data, UInt_t len, UInt_t serial, UInt_t nSmpl, TClonesArray *arr);
    BmnStatus Process_ADC64WR(UInt_t *data, UInt_t len, UInt_t serial, TClonesArray *arr);
    BmnStatus Process_FVME(UInt_t *data, UInt_t len, UInt_t serial, BmnEventType &ped, BmnTriggerType &trig);
    BmnStatus Process_HRB(UInt_t *data, UInt_t len, UInt_t serial);
    BmnStatus FillTDC(UInt_t *d, UInt_t serial, UInt_t slot, UInt_t modId, UInt_t &idx);
    BmnStatus FillTQDC(UInt_t *d, UInt_t serial, UInt_t slot, UInt_t modId, UInt_t &idx);
    BmnStatus FillSYNC(UInt_t *d, UInt_t serial, UInt_t &idx);

    BmnStatus FillMSC(UInt_t *d, UInt_t serial, UInt_t &idx) {
        return kBMNSUCCESS;
    };
    BmnStatus FillTimeShiftsMap();
    BmnStatus FillTimeShiftsMapNoDB(UInt_t t0serial);

    BmnStatus CopyDataToPedMap(TClonesArray* adcGem, TClonesArray* adcSil, UInt_t ev);
};

#endif
