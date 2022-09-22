#ifndef BMNRAWDATADECODER_H
#define BMNRAWDATADECODER_H 1

#include "BmnGemRaw2Digit.h"
#include "BmnMwpcRaw2Digit.h"
#include "BmnDchRaw2Digit.h"
#include "BmnSiliconRaw2Digit.h"
#include "BmnTof1Raw2Digit.h"
#include "BmnTof2Raw2DigitNew.h"
#include "BmnZDCRaw2Digit.h"
#include "BmnScWallRaw2Digit.h"
#include "BmnFHCalRaw2Digit.h"
#include "BmnHodoRaw2Digit.h"
#include "BmnNdetRaw2Digit.h"
#include "BmnECALRaw2Digit.h"
#include "BmnLANDRaw2Digit.h"
#include "BmnTofCalRaw2Digit.h"
#include "BmnTrigRaw2Digit.h"
#include "BmnCscRaw2Digit.h"
#include "BmnMscRaw2Digit.h"
#include "BmnEventHeader.h"
#include "DigiArrays.h"

#include "TString.h"
#include "TFile.h"
#include "TTimeStamp.h"
#include "TTree.h"
#include "TClonesArray.h"

#pragma GCC system_header
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>

#include <map>
#include <deque>
#include <vector>
#include <fstream>

/********************************************************/
// wait limit for input data (ms)
#define WAIT_LIMIT 40000000
using namespace std;
namespace pt = boost::property_tree;

class BmnRawDataDecoder {
public:
    BmnRawDataDecoder(TString file = "", TString outfile = "", ULong_t nEvents = 0, ULong_t period = 7);
    virtual ~BmnRawDataDecoder();

    static BmnStatus ParseRunTLV(UInt_t *buf, UInt_t &len, UInt_t &runId);
    BmnStatus ParseJsonTLV(UInt_t *buf, UInt_t &len);
    BmnStatus ConvertRawToRoot();
    BmnStatus ConvertRawToRootIterate(UInt_t *buf, UInt_t len);
    BmnStatus ConvertRawToRootIterateFile(UInt_t limit = WAIT_LIMIT);
    BmnStatus ClearArrays();
    BmnStatus DecodeDataToDigi();
    BmnStatus DecodeDataToDigiIterate();
    BmnStatus FinishRun();
    BmnStatus InitConverter(TString FileName);
    BmnStatus InitConverter();
    BmnStatus InitDecoder();
    BmnStatus InitMaps();
    void ResetDecoder(TString file);
    BmnStatus DisposeDecoder();
    BmnStatus wait_file(Int_t len, UInt_t limit = WAIT_LIMIT);
    BmnStatus TakeDataWordShort(UChar_t n, UInt_t *d, UInt_t i, Short_t* valI);
    BmnStatus TakeDataWordUShort(UChar_t n, UInt_t *d, UInt_t i, UShort_t* valU);

    DigiArrays GetDigiArraysObject() {
        //        fDigiTree->GetEntry(GetEventId());
        DigiArrays d; // = new DigiArrays();
        d.silicon = silicon;
        d.gem = gem;
        d.csc = csc;
        d.tof400 = tof400;
        d.tof700 = tof700;
        d.zdc = zdc;
        d.scwall = scwall;
        d.fhcal = fhcal;
        d.hodo = hodo;
        d.ndet = ndet;
        d.ecal = ecal;
        d.land = land;
        d.tofcal = tofcal;
        d.dch = dch;
        d.mwpc = mwpc;
        d.header = eventHeader;
        d.trigAr = NULL;
        d.trigSrcAr = NULL;
        if (fTrigMapper) {
//            if (fBmnSetup == kBMNSETUP)
                d.trigAr = fTrigMapper->GetTrigArrays();
//            else
//                d.trigSrcAr = fTrigMapper->GetTrigArrays();
        }
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

    BmnScWallRaw2Digit *GetScWallMapper() {
        return fScWallMapper;
    }

    BmnFHCalRaw2Digit *GetFHCalMapper() {
        return fFHCalMapper;
    }

    BmnHodoRaw2Digit *GetHodoMapper() {
        return fHodoMapper;
    }

    BmnNdetRaw2Digit *GetNdetMapper() {
        return fNdetMapper;
    }

    BmnECALRaw2Digit *GetECALMapper() {
        return fECALMapper;
    }

    BmnLANDRaw2Digit *GetLANDMapper() {
        return fLANDMapper;
    }

    BmnTofCalRaw2Digit *GetTofCalMapper() {
        return fTofCalMapper;
    }

    void SetTrigPlaceMapping(TString map) {
        fTrigPlaceMapFileName = map;
    }

    void SetSiliconMapping(TString map) {
        fSiliconMapFileName = map;
    }

    void SetCSCMapping(TString map) {
        fCscMapFileName = map;
    }

    void SetTrigChannelMapping(TString file) {
        fTrigChannelMapFileName = file;
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

    void SetTof700SlewingReference(Int_t chamber, Int_t refrun, Int_t refchamber);

    void SetZDCMapping(TString map) {
        fZDCMapFileName = map;
    }

    void SetZDCCalibration(TString cal) {
        fZDCCalibrationFileName = cal;
    }

    void SetScWallMapping(TString map) {
        fScWallMapFileName = map;
    }

    void SetScWallCalibration(TString cal) {
        fScWallCalibrationFileName = cal;
    }

    void SetFHCalMapping(TString map) {
        fFHCalMapFileName = map;
    }

    void SetFHCalCalibration(TString cal) {
        fFHCalCalibrationFileName = cal;
    }

    void SetHodoMapping(TString map) {
        fHodoMapFileName = map;
    }

    void SetHodoCalibration(TString cal) {
        fHodoCalibrationFileName = cal;
    }

    void SetNdetMapping(TString map) {
        fNdetMapFileName = map;
    }

    void SetNdetCalibration(TString cal) {
        fNdetCalibrationFileName = cal;
    }

    void SetECALMapping(TString map) {
        fECALMapFileName = map;
    }

    void SetECALCalibration(TString cal) {
        fECALCalibrationFileName = cal;
    }

    void SetMSCMapping(TString map) {
        fMSCMapFileName = map;
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

    void SetTofCalMapping(TString map) {
        fTofCalMapFileName = map;
    }

    void SetTofCalPedestal(TString clock) {
        fTofCalClockFileName = clock;
    }

    void SetTofCalTCal(TString tcal) {
        fTofCalTCalFileName = tcal;
    }

    void SetTofCalDiffSync(TString diff_sync) {
        fTofCalDiffSyncFileName = diff_sync;
    }

    void SetTofCalVScint(TString vscint) {
        fTofCalVScintFileName = vscint;
    }

    TString GetRootFileName() {
        return fRootFileName;
    }

    BmnStatus SetDetectorSetup(std::map<DetectorId, bool> setup) {
        fDetectorSetup = setup;
        return kBMNSUCCESS;
    }

    void SetEvForPedestals(UInt_t v) {
        this->fEvForPedestals = v;
    }

    UInt_t GetEvForPedestals() {
        return fEvForPedestals;
    }

    void SetBmnSetup(BmnSetup v) {
        fBmnSetup = v;
    }

    BmnSetup GetBmnSetup() const {
        return fBmnSetup;
    }

    void SetAdcDecoMode(BmnADCDecoMode v) {
        fAdcDecoMode = v;
    }

    BmnADCDecoMode GetAdcDecoMode() const {
        return fAdcDecoMode;
    }

    void SetVerbose(Int_t v) {
        fVerbose = v;
    }

    Int_t GetVerbose() const {
        return fVerbose;
    }

    UInt_t GetBoundaryRun(UInt_t nSmpl) {
        //format for SILICON data was changed during March 2017 seance (run 1542)
        //format for GEM was changed after March 2017 seance (run 1992)
        //so we have to use this crutch.
        return (nSmpl == 128) ? 1542 : 1992;
    }

    void SetRawRootFile(TString filename) {
        fRootFileName = filename;
    }

    void SetDigiRootFile(TString filename) {
        fDigiFileName = filename;
    }

private:

    std::map<DetectorId, bool> fDetectorSetup;
    pt::ptree conf;
    Bool_t isSpillStart;
    UInt_t fSpillCntr;
    BmnEventType evType = kBMNPAYLOAD;
    bool isRawRootInputFile;

    Int_t fTOF700ReferenceRun;
    Int_t fTOF700ReferenceChamber;
    Int_t GetRunIdFromFile(TString name);
    vector<UInt_t> fSiliconSerials; //list of serial id for Silicon
    UInt_t fNSiliconSerials;
    vector<UInt_t> fGemSerials; //list of serial id for GEM
    UInt_t fNGemSerials;
    vector<UInt_t> fCscSerials; //list of serial id for CSC
    UInt_t fNCscSerials;
    vector<UInt_t> fZDCSerials; //list of serial id for ZDC
    UInt_t fNZDCSerials;
    vector<UInt_t> fScWallSerials; //list of serial id for ScWall
    UInt_t fNScWallSerials;
    vector<UInt_t> fFHCalSerials; //list of serial id for FHCAL
    UInt_t fNFHCalSerials;
    vector<UInt_t> fHodoSerials; //list of serial id for HODO
    UInt_t fNHodoSerials;
    vector<UInt_t> fNdetSerials; //list of serial id for NDET
    UInt_t fNNdetSerials;
    vector<UInt_t> fECALSerials; //list of serial id for ECal
    UInt_t fNECALSerials;

    UInt_t fPedoCounter;

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
    TTree *fRawTreeSpills;
    TTree *fDigiTree;
    TTree *fDigiTreeSpills;
    TString fRootFileName;
    TString fRawFileName;
    TString fDigiFileName;
    TString fDigiRunHdrName;
    TString fDchMapFileName;
    TString fMwpcMapFileName;
    TString fGemMapFileName;
    TString fTof400PlaceMapFileName;
    TString fTof400StripMapFileName;
    TString fTof700MapFileName;
    TString fTof700GeomFileName;
    TString fZDCMapFileName;
    TString fZDCCalibrationFileName;
    TString fScWallMapFileName;
    TString fScWallCalibrationFileName;
    TString fFHCalMapFileName;
    TString fFHCalCalibrationFileName;
    TString fHodoMapFileName;
    TString fHodoCalibrationFileName;
    TString fNdetMapFileName;
    TString fNdetCalibrationFileName;
    TString fECALMapFileName;
    TString fECALCalibrationFileName;
    TString fMSCMapFileName;
    TString fLANDMapFileName;
    TString fLANDClockFileName;
    TString fLANDTCalFileName;
    TString fLANDDiffSyncFileName;
    TString fLANDVScintFileName;
    TString fTofCalMapFileName;
    TString fTofCalClockFileName;
    TString fTofCalTCalFileName;
    TString fTofCalDiffSyncFileName;
    TString fTofCalVScintFileName;
    TString fSiliconMapFileName;
    TString fCscMapFileName;
    TString fTrigPlaceMapFileName;
    TString fTrigChannelMapFileName;

    TFile *fRootFileIn;
    TFile *fRootFileOut;
    TFile *fDigiFileOut;
    FILE *fRawFileIn;

    //DAQ arrays
    TClonesArray *sync;
    TClonesArray *adc32; //gem
    TClonesArray *adc128; //sts
    TClonesArray *adc; //zdc & ecal & scwall & fhcal
    TClonesArray *hrb;
    TClonesArray *tacquila; // LAND.
    TClonesArray *tacquila2; // ToF-Cal
    TClonesArray *tdc;
    TClonesArray *tqdc_tdc;
    TClonesArray *tqdc_adc;
    TClonesArray *msc;
    BmnEventHeader *eventHeaderDAQ;

    //Digi arrays
    TClonesArray *silicon;
    TClonesArray *gem;
    TClonesArray *csc;
    TClonesArray *tof400;
    TClonesArray *tof700;
    TClonesArray *zdc;
    TClonesArray *scwall;
    TClonesArray *fhcal;
    TClonesArray *hodo;
    TClonesArray *ndet;
    TClonesArray *ecal;
    TClonesArray *land;
    TClonesArray *tofcal;
    TClonesArray *dch;
    TClonesArray *mwpc;

    //header array
    BmnEventHeader *eventHeader;
    BmnSpillHeader *spillHeader;

    UInt_t data[10000000];
    ULong_t fMaxEvent;

    UInt_t fDat; //current 32-bits word
    UInt_t syncCounter;
    BmnCscRaw2Digit *fCscMapper;
    BmnGemRaw2Digit *fGemMapper;
    BmnSiliconRaw2Digit *fSiliconMapper;
    BmnDchRaw2Digit *fDchMapper;
    BmnMwpcRaw2Digit *fMwpcMapper;
    BmnTrigRaw2Digit *fTrigMapper;
    BmnTof1Raw2Digit *fTof400Mapper;
    BmnTof2Raw2DigitNew *fTof700Mapper;
    BmnZDCRaw2Digit *fZDCMapper;
    BmnScWallRaw2Digit *fScWallMapper;
    BmnFHCalRaw2Digit *fFHCalMapper;
    BmnHodoRaw2Digit *fHodoMapper;
    BmnNdetRaw2Digit *fNdetMapper;
    BmnECALRaw2Digit *fECALMapper;
    BmnLANDRaw2Digit *fLANDMapper;
    BmnTofCalRaw2Digit *fTofCalMapper;
    BmnMscRaw2Digit *fMSCMapper;
    UInt_t nSpillEvents;
    BmnTrigInfo* trigInfoTemp;
    BmnTrigInfo* trigInfoSum;
    BmnEventType fCurEventType;
    BmnEventType fPrevEventType;
    BmnSetup fBmnSetup;
    BmnADCDecoMode fAdcDecoMode;
    UInt_t fPedEvCntrBySpill;
    UInt_t fPedEvCntr;
    UInt_t fNoiseEvCntr;
    Int_t fEvForPedestals;
    Bool_t fPedEnough;
    Bool_t fNoiseEnough;
//    GemMapValue* fGemMap;
    UInt_t fT0Serial;

    //Map to store pairs <Crate serial> - <crate time - T0 time>
    map<UInt_t, Long64_t> fTimeShifts;
    Double_t fT0Time; //ns
    Double_t fT0Width; //ns
    map<TTimeStamp, Int_t> leaps;
    TTimeStamp utc_valid;
    Int_t tai_utc_dif;
    Int_t fVerbose;

    int refrun_tof700_slewing[60];
    int refchamber_tof700_slewing[60];
    int type_tof700_slewing[60];

    BmnStatus InitUTCShift();
    Int_t GetUTCShift(TTimeStamp t);
    BmnStatus GetT0Info(Double_t& t0time, Double_t &t0width);
    BmnStatus ProcessEvent(UInt_t *data, UInt_t len);
    /**
     * Parse ADC64VE 
     * format Mstream Waveform V2 from https://afi.jinr.ru/MStreamWaveformDigitizer
     * @param d Data array ptr
     * @param len payload length
     * @param serial 
     * @param nSmpl
     * @param arr ADC digits storage
     * @return kBMNSUCCESS
     */
    BmnStatus Process_ADC64VE(UInt_t *data, UInt_t len, UInt_t serial, UInt_t nSmpl, TClonesArray *arr);
    BmnStatus Process_ADC64WR(UInt_t *data, UInt_t len, UInt_t serial, TClonesArray *arr);
    BmnStatus Process_FVME(UInt_t *data, UInt_t len, UInt_t serial, BmnTrigInfo* spillInfo);
    BmnStatus Process_HRB(UInt_t *data, UInt_t len, UInt_t serial);
    BmnStatus Process_Tacquila(UInt_t *data, UInt_t len);
    BmnStatus FillU40VE(UInt_t *d, UInt_t slot, UInt_t &idx, BmnTrigInfo* spillInfo);
    BmnStatus FillBlockTDC(UInt_t *d, UInt_t serial, uint16_t &len, TClonesArray *ar);
    BmnStatus FillBlockADC(UInt_t *d, UInt_t serial, uint8_t channel, uint16_t &len, TClonesArray *ar);
    BmnStatus FillTDC(UInt_t *d, UInt_t serial, UInt_t slot, UInt_t modId, UInt_t &idx);
    BmnStatus FillTQDC(UInt_t *d, UInt_t serial, UInt_t slot, UInt_t modId, UInt_t &idx);
    /**
     * Parse TQDC16VS-E MStream data block
     * https://afi.jinr.ru/DataFormatTQDC16VSE
     * @param d data pointer
     * @param serial device serial
     * @param len payload length
     * @return operation success
     */
    BmnStatus FillTQDC_E(UInt_t *d, UInt_t serial, UInt_t &len);
    /**
     * Parse MSC16VE-E MStream data block
     * https://afi.jinr.ru/DataFormatMSC_ETH
     * @param d data pointer
     * @param serial device serial
     * @param len payload length
     * @return operation success
     */
    BmnStatus FillMSC16VE_E(UInt_t *d, UInt_t serial, UInt_t &len);
    BmnStatus FillTDC72VXS(UInt_t *d, UInt_t serial, UInt_t &len);
    /**
     * Parse UT24VE-TRC MStream data block
     * https://afi.jinr.ru/DataFormatUT24VE-TRC
     * @param d data pointer
     * @param serial device serial
     * @param len payload length
     * @param evType calibration/payload event
     * @return operation success
     */
    BmnStatus FillUT24VE_TRC(UInt_t *d, UInt_t &serial, UInt_t &len);
    BmnStatus FillSYNC(UInt_t *d, UInt_t serial, UInt_t &idx);
    inline void FillWR(UInt_t iSerial, ULong64_t iEvent, Long64_t t_sec, Long64_t t_ns);

    BmnStatus FillMSC(UInt_t *d, UInt_t serial, UInt_t slot, UInt_t &idx);
    BmnStatus FillTimeShiftsMap();
    BmnStatus FillTimeShiftsMapNoDB(UInt_t t0serial);

    BmnStatus CopyDataToPedMap(TClonesArray* adcGem, TClonesArray* adcSil, UInt_t ev);
};

#endif
