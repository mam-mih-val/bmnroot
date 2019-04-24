
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
    /** Event Header branch name **/
    TString fHeaderName;

    /** Event Id **/
    UInt_t fEventId;
    /** Event Time in TTimeStamp **/
    TTimeStamp fEventTimeTS;
    /** Event Type (payload = 0 or pedestal = 1) **/
    BmnEventType fType;
    /** Trigger Type (beam = 6 or target = 1) **/
    BmnTriggerType fTrigType;
    /** Tripped Gems (1 bit for 1 GEM module) **/
    Bool_t fTripWord;

    map<UInt_t, Long64_t> fTimeShift;          //!

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

public:
    /** Default constructor */
    BmnEventHeader();

    /** Constructor */
    BmnEventHeader(UInt_t run, UInt_t ev, TTimeStamp time, BmnEventType type, Bool_t trip, BmnTrigInfo* info);

    /** Constructor */
    BmnEventHeader(UInt_t run, UInt_t ev, TTimeStamp time, BmnEventType type, Bool_t trip, BmnTrigInfo* info, map<UInt_t, Long64_t> ts);

    /** Constructor */
    BmnEventHeader(UInt_t run, UInt_t ev, TTimeStamp time, BmnEventType type, Bool_t trip, BmnTrigInfo* info, UInt_t T0, UInt_t BC1, UInt_t BC2, UInt_t VETO, UInt_t BD, UInt_t Si);

    /** Destructor */
    virtual ~BmnEventHeader();

    virtual void Register(Bool_t Persistence = kTRUE)
    {
        FairRootManager::Instance()->Register(fHeaderName.Data(), "EvtHeader", this, Persistence);
    }


    /** Get Event Header branch name */
    TString GetHeaderName() { return fHeaderName; }

    /** Get the run ID for this run */
    UInt_t GetEventId() { return fEventId; }

    /** Get the time for this event */
    TTimeStamp GetEventTimeTS() { return fEventTimeTS; }

    /** Get the type of this event */
    BmnEventType GetType() { return fType; }

    /** Get the type of this event */
    BmnTriggerType GetTrig() { return fTrigType; }

    /** Get the trip word for this event */
    Bool_t GetTripWord() { return fTripWord; }

    /** Get the spill statistics */
    BmnTrigInfo* GetTrigInfo() { return fTrigInfo; }

    /** Get the triggers for this run */
    UInt_t GetTrigT0() { return fTrigT0; }

    map<UInt_t, Long64_t> GetTimeShift() { return fTimeShift; }

    Double_t GetStartSignalTime() { return fStartSignalTime; }
    Double_t GetStartSignalWidth() { return fStartSignalWidth; }

    UInt_t GetTrigBC1() { return fTrigBC1; }
    UInt_t GetTrigBC2() { return fTrigBC2; }
    UInt_t GetTrigVETO() { return fTrigVETO; }
    UInt_t GetTrigSi() { return fTrigSi; }
    UInt_t GetTrigBD() { return fTrigBD; }

    vector <UInt_t> GetModBD() { return fModBD; }
    vector <UInt_t> GetModSi() { return fModSi; }
    vector <UInt_t> GetModBC1() { return fModBC1; }
    vector <UInt_t> GetModBC2() { return fModBC2; }
    vector <UInt_t> GetModT0() { return fModT0; }
    vector <UInt_t> GetModVETO() { return fModVETO; }

    vector <Double_t> GetTimeSi() { return fTimeSi; }
    vector <Double_t> GetTimeBC1() { return fTimeBC1; }
    vector <Double_t> GetTimeBC2() { return fTimeBC2; }
    vector <Double_t> GetTimeVETO() { return fTimeVETO; }

    vector <Double_t> GetAmpBD() { return fAmpBD; }
    vector <Double_t> GetAmpSi() { return fAmpSi; }
    vector <Double_t> GetAmpBC1() { return fAmpBC1; }
    vector <Double_t> GetAmpBC2() { return fAmpBC2; }
    vector <Double_t> GetAmpT0() { return fAmpT0; }
    vector <Double_t> GetAmpVETO() { return fAmpVETO; }


    /** Set Event Header branch name */
    void SetHeaderName(TString header_name) { fHeaderName = header_name; }

    /** Set the event ID for this run
     * @param evid : unique event id
     */
    void SetEventId(UInt_t evid) { fEventId = evid; }

    /** Set the time for this event in */
    void SetEventTimeTS(TTimeStamp time) { fEventTimeTS = time; }

    /** Set the type for this event
     * @param type : type (0 or 1)
     */
    void SetType(BmnEventType type) { fType = type; }

    void SetStartSignalInfo(Double_t time, Double_t width)
    {
        fStartSignalTime = time;
        fStartSignalWidth = width;
    }

    void SetTrigType(BmnTriggerType type)
    {
        if (!fTrigInfo) fTrigInfo = new BmnTrigInfo();
        fTrigInfo->SetTrigType(type);
    }

    void SetTrigInfo(BmnTrigInfo* info)
    {
        if (fTrigInfo) delete fTrigInfo;
        fTrigInfo = new BmnTrigInfo(info);
    }

    void SetTripWord(Bool_t flag) { fTripWord = flag; }

    void SetTimeShift(map <UInt_t, Long64_t> ts) { fTimeShift = ts; }

    void SetModSi(UInt_t ModSi) { fModSi.push_back(ModSi); }
    void SetModBD(UInt_t ModBD) { fModBD.push_back(ModBD); }
    void SetModBC1(UInt_t ModBC1) { fModBC1.push_back(ModBC1); }
    void SetModBC2(UInt_t ModBC2) { fModBC2.push_back(ModBC2); }
    void SetModT0(UInt_t ModT0) { fModT0.push_back(ModT0); }
    void SetModVETO(UInt_t ModVETO) { fModVETO.push_back(ModVETO); }

    void SetTimeSi(Double_t TimeSi) { fTimeSi.push_back(TimeSi); }
    void SetTimeBD(Double_t TimeBD) { fTimeBD.push_back(TimeBD); }
    void SetTimeBC1(Double_t TimeBC1) { fTimeBC1.push_back(TimeBC1); }
    void SetTimeBC2(Double_t TimeBC2) { fTimeBC2.push_back(TimeBC2);}
    void SetTimeVETO(Double_t TimeVETO) { fTimeVETO.push_back(TimeVETO); }

    void SetAmpSi(Double_t AmpSi) { fAmpSi.push_back(AmpSi); }
    void SetAmpBD(Double_t AmpBD) { fAmpBD.push_back(AmpBD); }
    void SetAmpBC1(Double_t AmpBC1) { fAmpBC1.push_back(AmpBC1); }
    void SetAmpBC2(Double_t AmpBC2) { fAmpBC2.push_back(AmpBC2); }
    void SetAmpT0(Double_t AmpT0) { fAmpT0.push_back(AmpT0); }
    void SetAmpVETO(Double_t AmpVETO) { fAmpVETO.push_back(AmpVETO); }

    ClassDef(BmnEventHeader, 6)
};

#endif /* BMNEVENTHEADER_H */
