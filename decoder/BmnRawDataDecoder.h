#ifndef BMNRAWDATADECODER_H
#define BMNRAWDATADECODER_H 1

#include "TString.h"
#include "TSystem.h"
#include "BmnEnums.h"
#include "BmnTTBDigit.h"
#include "BmnTDCDigit.h"
#include "BmnHRBDigit.h"
#include "BmnADCDigit.h"
#include "BmnTacquilaDigit.h"
#include "BmnTQDCADCDigit.h"
#include "BmnLANDDigit.h"
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
#include "BmnLANDRaw2Digit.h"
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

/***************** SET OF DAQ CONSTANTS *****************/
const UInt_t kSYNC1 = 0x2A502A50;
const UInt_t kSYNC2 = 0x4A624A62;
const UInt_t kRUNSTARTSYNC = 0x72617453;
const UInt_t kRUNSTOPSYNC = 0x706F7453;
const UInt_t kRUNNUMBERSYNC = 0x236E7552;
const size_t kWORDSIZE = sizeof (UInt_t);
const Short_t kNBYTESINWORD = 4;

//FVME data types
const UInt_t kMODDATAMAX = 0x7;
const UInt_t kMODHEADER = 0x8;
const UInt_t kMODTRAILER = 0x9;
const UInt_t kEVHEADER = 0xA;
const UInt_t kEVTRAILER = 0xB;
const UInt_t kSPILLHEADER = 0xC;
const UInt_t kSPILLTRAILER = 0xD;
const UInt_t kSTATUS = 0xE;
const UInt_t kPADDING = 0xF;

//module ID
const UInt_t kTDC64V = 0x10; //DCH
const UInt_t kTDC64VHLE = 0x53;
const UInt_t kTDC72VHL = 0x12;
const UInt_t kTDC32VL = 0x11;
const UInt_t kTQDC16 = 0x09;
const UInt_t kTQDC16VS = 0x56;
const UInt_t kTQDC16VS_ETH = 0xD6;
const UInt_t kTRIG = 0xA;
const UInt_t kMSC = 0xF;
const UInt_t kUT24VE = 0x49;
const UInt_t kADC64VE = 0xD4;
const UInt_t kADC64VE_XGE = 0xD9;
const UInt_t kADC64WR = 0xCA;
const UInt_t kHRB = 0xC2;
const UInt_t kFVME = 0xD1;
const UInt_t kLAND = 0xDA;
const UInt_t kU40VE_RC = 0x4C;

//event type trigger
const UInt_t kEVENTTYPESLOT = 12;
const UInt_t kGEMTRIGTYPE = 3;
const UInt_t kTRIGBEAM = 6;
const UInt_t kTRIGMINBIAS = 1;

#define ANSI_COLOR_RED   "\x1b[91m"
#define ANSI_COLOR_BLUE  "\x1b[94m"
#define ANSI_COLOR_RESET "\x1b[0m"
/********************************************************/
// wait limit for input data (ms)
#define WAIT_LIMIT 40000000
using namespace std;

class BmnRawDataDecoder {
public:
    BmnRawDataDecoder(TString file, ULong_t nEvents = 0, ULong_t period = 4);
    BmnRawDataDecoder();
    virtual ~BmnRawDataDecoder();

    BmnStatus ConvertRawToRoot();
    BmnStatus ConvertRawToRootIterate(UInt_t *buf, UInt_t len);
    BmnStatus ConvertRawToRootIterateFile(UInt_t limit = WAIT_LIMIT);
    BmnStatus ClearArrays();
    BmnStatus DecodeDataToDigi();
    BmnStatus DecodeDataToDigiIterate();
    BmnStatus FinishRun();
    BmnStatus InitConverter(TString FileName);
    BmnStatus InitConverter();
    BmnStatus InitConverter(deque<UInt_t> *dq);
    BmnStatus InitDecoder();
    BmnStatus InitMaps();
    void ResetDecoder(TString file);
    BmnStatus DisposeDecoder();
    BmnStatus wait_stream(deque<UInt_t> *que, Int_t len, UInt_t limit = WAIT_LIMIT);
    BmnStatus wait_file(Int_t len, UInt_t limit = WAIT_LIMIT);
    BmnStatus SlewingTOF700Init();
    BmnStatus SlewingTOF700();

    BmnStatus PreparationTOF700Init() {
        return SlewingTOF700Init();
    };
    BmnStatus PreparationTOF700();
    BmnStatus TakeDataWordShort(UChar_t n, UInt_t *d, UInt_t i, Short_t* valI);
    BmnStatus TakeDataWordUShort(UChar_t n, UInt_t *d, UInt_t i, UShort_t* valU);

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
        d.land = land;
        d.dch = dch;
        d.mwpc = mwpc;
        d.header = eventHeader;
        d.trigAr = NULL;
        d.trigSrcAr = NULL;
        if (fTrigMapper)
            if (fBmnSetup == kBMNSETUP)
                d.trigAr = fTrigMapper->GetTrigArrays();
            else
                d.trigSrcAr = fTrigMapper->GetTrigArrays();
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

    BmnLANDRaw2Digit *GetLANDMapper() {
        return fLANDMapper;
    }

    void SetTrigMapping(TString map) {
        fTrigMapFileName = map;
    }

    void SetSiliconMapping(TString map) {
        fSiliconMapFileName = map;
    }

    void SetCSCMapping(TString map) {
        fCSCMapFileName = map;
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

    void SetLANDMapping(TString map) {
        fLANDMapFileName = map;
    }

    void SetLANDPedestal(TString clock) {
        fLANDClockFileName = clock;
    }

    void SetLANDTCal(TString tcal) {
        fLANDTCalFileName = tcal;
    }

    void SetLANDDiffSync(TString diff_sync) {
        fLANDDiffSyncFileName = diff_sync;
    }

    void SetLANDVScint(TString vscint) {
        fLANDVScintFileName = vscint;
    }

    TString GetRootFileName() {
        return fRootFileName;
    }

    BmnStatus SetDetectorSetup(Bool_t* setup) {
        for (Int_t i = 0; i < 11; ++i) {
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

    void SetBmnSetup(BmnSetup v) {
        this->fBmnSetup = v;
    }

    BmnSetup GetBmnSetup() const {
        return fBmnSetup;
    }

    UInt_t GetBoundaryRun(UInt_t nSmpl) {
        //format for SILICON data was changed during March 2017 seance (run 1542)
        //format for GEM was changed after March 2017 seance (run 1992)
        //so we have to use this crutch.
        return (nSmpl == 128) ? 1542 : 1992;
    }

private:

    //9 bits correspond to detectors which we need to decode
    Bool_t fDetectorSetup[11];


    Int_t fTOF700ReferenceRun;
    Int_t fTOF700ReferenceChamber;

    Int_t GetRunIdFromFile(TString name);
    vector<UInt_t> fSiliconSerials; //list of serial id for Silicon
    UInt_t fNSiliconSerials;
    vector<UInt_t> fGemSerials; //list of serial id for GEM
    UInt_t fNGemSerials;
    vector<UInt_t> fCSCSerials; //list of serial id for CSC
    UInt_t fNCSCSerials;
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
    TString fLANDMapFileName;
    TString fLANDClockFileName;
    TString fLANDTCalFileName;
    TString fLANDDiffSyncFileName;
    TString fLANDVScintFileName;
    TString fSiliconMapFileName;
    TString fCSCMapFileName;
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
    TClonesArray *tacquila; // LAND.
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
    TClonesArray *land;
    TClonesArray *dch;
    TClonesArray *mwpc;

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
    BmnZDCRaw2Digit *fZDCMapper;
    BmnECALRaw2Digit *fECALMapper;
    BmnLANDRaw2Digit *fLANDMapper;
    BmnEventType fCurEventType;
    BmnEventType fPrevEventType;
    BmnSetup fBmnSetup;
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

    BmnStatus GetT0Info(Double_t& t0time, Double_t &t0width);
    BmnStatus ProcessEvent(UInt_t *data, UInt_t len);
    BmnStatus Process_ADC64VE(UInt_t *data, UInt_t len, UInt_t serial, UInt_t nSmpl, TClonesArray *arr);
    BmnStatus Process_ADC64WR(UInt_t *data, UInt_t len, UInt_t serial, TClonesArray *arr);
    BmnStatus Process_FVME(UInt_t *data, UInt_t len, UInt_t serial, BmnEventType &ped, BmnTriggerType &trig);
    BmnStatus Process_HRB(UInt_t *data, UInt_t len, UInt_t serial);
    BmnStatus Process_Tacquila(UInt_t *data, UInt_t len);
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
