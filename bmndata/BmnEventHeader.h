
#ifndef BMNEVENTHEADER_H
#define BMNEVENTHEADER_H

#include "BmnEnums.h"
#include "BmnTrigInfo.h"

#include "FairEventHeader.h"
#include "FairRootManager.h"

#include "TTimeStamp.h"

#include <map>
#include <vector>
using namespace std;

class BmnEventHeader : public FairEventHeader {
private:
    /** Event Id **/
    UInt_t fEventId;
    /** Event Time in TTimeStamp **/
    TTimeStamp fEventTimeTS;
    /** Event Type (payload = 0 or pedestal = 1)**/
    BmnEventType fType;
    /** Trigger Type (beam = 6 or target = 1)**/
    BmnTriggerType fTrigType;
    /** Tripped Gems (1 bit for 1 GEM module)**/
    Bool_t fTripWord;

    map <UInt_t, Long64_t> fTimeShift;
    /** T0 information for current event**/
    Double_t fStartSignalTime; //ns
    Double_t fStartSignalWidth; //ns
    BmnTrigInfo* fTrigInfo;

    UInt_t fTrigT0;
    UInt_t fTrigBC1;
    UInt_t fTrigBC2;
    UInt_t fTrigVETO;
    UInt_t fTrigBD;
    UInt_t fTrigSi;
    vector<UInt_t> fModSi;
    vector<UInt_t> fModBD;
    vector<UInt_t> fModBC2;
    vector<UInt_t> fModBC1;
    vector<UInt_t> fModT0;
    vector<UInt_t> fModVETO;

    vector<Double_t> fAmpSi;
    vector<Double_t> fAmpBD;
    vector<Double_t> fAmpBC2;
    vector<Double_t> fAmpBC1;
    vector<Double_t> fAmpT0;
    vector<Double_t> fAmpVETO;

    vector<Double_t> fTimeSi;
    vector<Double_t> fTimeBD;
    vector<Double_t> fTimeBC2;
    vector<Double_t> fTimeBC1;
    vector<Double_t> fTimeVETO;

    TString fHeaderName;

public:
    /** Default constructor */
    BmnEventHeader();

    /** Constructor */
    BmnEventHeader(UInt_t run, UInt_t ev, TTimeStamp time, BmnEventType type, BmnTrigInfo* info, Bool_t trip, UInt_t T0, UInt_t BC1, UInt_t BC2, UInt_t VETO, UInt_t BD, UInt_t Si);

    /** Constructor */
    BmnEventHeader(UInt_t run, UInt_t ev, TTimeStamp time, BmnEventType type, Bool_t trip, BmnTrigInfo* info);

    /** Constructor */
    BmnEventHeader(UInt_t run, UInt_t ev, TTimeStamp time, BmnEventType type, Bool_t trip, BmnTrigInfo* info, map<UInt_t, Long64_t> ts);

    /** Get the type of this event*/
    BmnEventType GetType() {
        return fType;
    }

    /** Get the type of this event*/
    BmnTriggerType GetTrig() {
        return fTrigType;
    }

    /** Get the trip word for this event*/
    Bool_t GetTripWord() {
        return fTripWord;
    }

    /** Get the run ID for this run*/
    UInt_t GetEventId() {
        return fEventId;
    }

    /** Get the time for this event*/
    TTimeStamp GetEventTimeTS() {
        return fEventTimeTS;
    }

    /** Get the spill statistics*/
    BmnTrigInfo* GetTrigInfo() {
        return fTrigInfo;
    }

    /** Get the triggers for this run*/
    UInt_t GetTrigT0() {
        return fTrigT0;
    }

    map <UInt_t, Long64_t> GetTimeShift() {
        return fTimeShift;
    }

    UInt_t GetTrigBC1() {
        return fTrigBC1;
    }

    UInt_t GetTrigBC2() {
        return fTrigBC2;
    }

    UInt_t GetTrigVETO() {
        return fTrigVETO;
    }

    UInt_t GetTrigSi() {
        return fTrigSi;
    }

    UInt_t GetTrigBD() {
        return fTrigBD;
    }

    vector <UInt_t> GetModBD() {
        return fModBD;
    }

    vector <UInt_t> GetModSi() {
        return fModSi;
    }

    vector <Double_t> GetTimeSi() {
        return fTimeSi;
    }

    vector <Double_t> GetTimeBC1() {
        return fTimeBC1;
    }

    vector <UInt_t> GetModBC1() {
        return fModBC1;
    }

    vector <Double_t> GetTimeBC2() {
        return fTimeBC2;
    }

    vector <UInt_t> GetModBC2() {
        return fModBC2;
    }

    vector <UInt_t> GetModT0() {
        return fModT0;
    }

    vector <UInt_t> GetModVETO() {
        return fModVETO;
    }

    vector <Double_t> GetTimeVETO() {
        return fTimeVETO;
    }

    vector <Double_t> GetAmpBD() {
        return fAmpBD;
    }

    vector <Double_t> GetAmpSi() {
        return fAmpSi;
    }

    vector <Double_t> GetAmpBC1() {
        return fAmpBC1;
    }

    vector <Double_t> GetAmpBC2() {
        return fAmpBC2;
    }

    vector <Double_t> GetAmpT0() {
        return fAmpT0;
    }

    vector <Double_t> GetAmpVETO() {
        return fAmpVETO;
    }

    /** Set the event ID for this run
     * @param evid : unique event id
     */
    void SetEventId(UInt_t evid) {
        fEventId = evid;
    }

    /** Set the time for this event in */
    void SetEventTimeTS(TTimeStamp time) {
        fEventTimeTS = time;
    }

    /** Set the type for this event
     * @param type : type (0 or 1)
     */
    void SetType(BmnEventType type) {
        fType = type;
    }

    void SetStartSignalInfo(Double_t time, Double_t width) {
        fStartSignalTime = time;
        fStartSignalWidth = width;
    }

    Double_t GetStartSignalTime() {
        return fStartSignalTime;
    }

    Double_t GetStartSignalWidth() {
        return fStartSignalWidth;
    }

    void SetTrigType(BmnTriggerType type) {
        if (!fTrigInfo)
            fTrigInfo = new BmnTrigInfo();
        fTrigInfo->SetTrigType(type);
    }

    void SetTripWord(Bool_t flag) {
        fTripWord = flag;
    }

    void SetTrigInfo(BmnTrigInfo* info) {
        if (fTrigInfo)
            delete fTrigInfo;
        fTrigInfo = new BmnTrigInfo(info);
    }

    void SetTimeShift(map <UInt_t, Long64_t> ts) {
        fTimeShift = ts;
    }

    void SetModSi(UInt_t ModSi) {
        //  copy(begin(ModBD), end(ModBD), begin(fModBD));
        fModSi.push_back(ModSi);
    }

    void SetTimeSi(Double_t TimeSi) {
        //  copy(begin(ModBD), end(ModBD), begin(fModBD));
        fTimeSi.push_back(TimeSi);
    }

    void SetTimeBD(Double_t TimeBD) {
        //  copy(begin(ModBD), end(ModBD), begin(fModBD));
        fTimeBD.push_back(TimeBD);
    }

    void SetModBD(UInt_t ModBD) {
        //  copy(begin(ModBD), end(ModBD), begin(fModBD));
        fModBD.push_back(ModBD);
    }

    void SetTimeBC1(Double_t TimeBC1) {
        //  copy(begin(ModBD), end(ModBD), begin(fModBD));
        fTimeBC1.push_back(TimeBC1);
    }

    void SetModBC1(UInt_t ModBC1) {
        //  copy(begin(ModBD), end(ModBD), begin(fModBD));
        fModBC1.push_back(ModBC1);
    }

    void SetTimeBC2(Double_t TimeBC2) {
        //  copy(begin(ModBD), end(ModBD), begin(fModBD));
        fTimeBC2.push_back(TimeBC2);
    }

    void SetModBC2(UInt_t ModBC2) {

        fModBC2.push_back(ModBC2);
    }

    void SetModT0(UInt_t ModT0) {

        fModT0.push_back(ModT0);
    }

    void SetModVETO(UInt_t ModVETO) {

        fModVETO.push_back(ModVETO);
    }

    void SetTimeVETO(Double_t TimeVETO) {
        //  copy(begin(ModBD), end(ModBD), begin(fModBD));
        fTimeVETO.push_back(TimeVETO);
    }

    void SetAmpSi(Double_t AmpSi) {

        fAmpSi.push_back(AmpSi);
    }

    void SetAmpBD(Double_t AmpBD) {

        fAmpBD.push_back(AmpBD);
    }

    void SetAmpBC1(Double_t AmpBC1) {

        fAmpBC1.push_back(AmpBC1);
    }

    void SetAmpBC2(Double_t AmpBC2) {

        fAmpBC2.push_back(AmpBC2);
    }

    void SetAmpT0(Double_t AmpT0) {

        fAmpT0.push_back(AmpT0);
    }

    void SetAmpVETO(Double_t AmpVETO) {

        fAmpVETO.push_back(AmpVETO);
    }

    void SetHeaderName(TString header_name) {
        fHeaderName = header_name;
    }

    virtual void Register(Bool_t Persistence = kTRUE) {
        FairRootManager::Instance()->Register(fHeaderName.Data(), "EvtHeader", this, Persistence);
    }

    /**
     * Destructor
     */
    virtual ~BmnEventHeader();

    ClassDef(BmnEventHeader, 6)
};

#endif /* BMNEVENTHEADER_H */
