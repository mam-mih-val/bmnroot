#ifndef BmnSlewingTOF700_H
#define BmnSlewingTOF700_H 1

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
#include "BmnCscRaw2Digit.h"
#include "BmnEventHeader.h"
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

const UInt_t kRUNNUMBERSYNC1 = 0x236E7552;
const size_t kWORDSIZE1 = sizeof (UInt_t);

/********************************************************/

// wait limit for input data (ms)
using namespace std;

class BmnSlewingTOF700 {
public:
    BmnSlewingTOF700(TString file, ULong_t nEvents = 0, ULong_t period = 4);
    BmnSlewingTOF700();
    virtual ~BmnSlewingTOF700();

    BmnStatus ClearArrays();
    BmnStatus InitDecoder();
    BmnStatus InitMaps();
    void ResetDecoder(TString file);
    BmnStatus DisposeDecoder();
    BmnStatus SlewingTOF700Init();
    BmnStatus SlewingTOF700();

    BmnStatus PreparationTOF700Init() {
        return SlewingTOF700Init();
    };
    BmnStatus PreparationTOF700();

    void SetQue(deque<UInt_t> *v) {
        fDataQueue = v;
    }

    deque<UInt_t> *GetQue() {
        return fDataQueue;
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

    void SetTrigPlaceMapping(TString map) {
        fTrigPlaceMapFileName = map;
    }

    void SetTrigChannelMapping(TString file) {
        fTrigChannelMapFileName = file;
    }

    void SetTof700Mapping(TString map) {
        fTof700MapFileName = map;
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


    void SetBmnSetup(BmnSetup v) {
        this->fBmnSetup = v;
    }

    BmnSetup GetBmnSetup() const {
        return fBmnSetup;
    }


private:

    //9 bits correspond to detectors which we need to decode
    Bool_t fDetectorSetup[11];


    Int_t GetRunIdFromFile(TString name);

    UInt_t fRunId;
    TTimeStamp fRunStartTime;
    TTimeStamp fRunEndTime;
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
    TString fTof700MapFileName;
    TString fTof700GeomFileName;
    TString fTrigPlaceMapFileName;
    TString fTrigChannelMapFileName;

    ifstream fTrigMapFile;
    ifstream fTrigINLFile;

    TFile *fRootFileIn;
    TFile *fRootFileOut;
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
    //    TClonesArray *runHeaderDAQ;
    TClonesArray *pedestalAdc;

    //header array
    TClonesArray *eventHeader;

    UInt_t data[10000000];
    ULong_t fMaxEvent;

    UInt_t fDat; //current 32-bits word
    UInt_t syncCounter;
    BmnTrigRaw2Digit *fTrigMapper;
    BmnTof2Raw2DigitNew *fTof700Mapper;
    BmnEventType fCurEventType;
    BmnEventType fPrevEventType;
    BmnSetup fBmnSetup;
    TriggerMapValue* fT0Map;
    deque<UInt_t> *fDataQueue;

    //Map to store pairs <Crate serial> - <crate time - T0 time>
    map<UInt_t, Long64_t> fTimeShifts;
    Double_t fT0Time; //ns
    Double_t fT0Width; //ns

    BmnStatus GetT0Info(Double_t& t0time, Double_t &t0width);
    BmnStatus FillTimeShiftsMap();
    BmnStatus FillTimeShiftsMapNoDB(UInt_t t0serial);

};

#endif
