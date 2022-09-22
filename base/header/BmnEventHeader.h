#ifndef BMNEVENTHEADER_H
#define BMNEVENTHEADER_H

#include "BmnEnums.h"
#include "BmnTrigInfo.h"

#include "FairEventHeader.h"
#include "FairRootManager.h"

#include "TTimeStamp.h"
#include "BmnTrigUnion.h"

#include <map>
#include <vector>
using namespace std;

class BmnEventHeader : public FairEventHeader {
private:
    /** Event Header branch name **/
    TString fHeaderName;    //!

    /** Event Id **/
    UInt_t fEventId;
    /** Period Id **/
    UInt_t fPeriodId;
    /** Event Time in TTimeStamp **/
    TTimeStamp fEventTimeTS;
    /** Event Type (payload = 0 or pedestal = 1) **/
    BmnEventType fEventType;
    /** Tripped Gems (1 bit for 1 GEM module) **/
    Bool_t fTripWord;
    /** start of new spill **/
    Bool_t fSpillStart;
    /** T0 information for current event**/
    Double_t fStartSignalTime; //ns
    Double_t fStartSignalWidth; //ns
    BmnTrigInfo* fTrigInfo;
    UInt_t fTrigUnion;
    UInt_t fInputsAR;
    UInt_t fInputsBR;

    map<UInt_t, Long64_t> fTimeShift;

public:
    /** Default constructor */
    BmnEventHeader();

    /** Constructor */
    BmnEventHeader(UInt_t run_id, UInt_t event_id, TTimeStamp event_time, BmnEventType event_type, Bool_t trip, BmnTrigInfo* trig_info);

    /** Constructor */
    BmnEventHeader(UInt_t run_id, UInt_t event_id, TTimeStamp event_time, BmnEventType event_type, Bool_t trip, BmnTrigInfo* trig_info, map<UInt_t, Long64_t> time_shift);

    /** Destructor */
    virtual ~BmnEventHeader();

    virtual void Register(Bool_t Persistence = kTRUE)
    {
        FairRootManager::Instance()->Register(fHeaderName.Data(), "EvtHeader", this, Persistence);
    }
    
    void Clear();

    /** Get Event Header branch name */
    TString GetHeaderName() { return fHeaderName; }

    /** Get the run ID for this run */
    UInt_t GetEventId() { return fEventId; }

    /** Get the period ID for this run */
    UInt_t GetPeriodId() { return fPeriodId; }

    /** Get the time for this event */
    TTimeStamp GetEventTimeTS() { return fEventTimeTS; }

    /** Get the type of this event */
    BmnEventType GetEventType() { return fEventType; }

    /** Get the trip word for this event */
    Bool_t GetTripWord() { return fTripWord; }

    /** Get wether new spill is started */
    Bool_t GetSpillStart() { return fSpillStart; }

    Double_t GetStartSignalTime() { return fStartSignalTime; }
    Double_t GetStartSignalWidth() { return fStartSignalWidth; }

    /** Get the spill statistics */
    BmnTrigInfo* GetTrigInfo() { return fTrigInfo; }
    
    /** Get the trigger state */
    BmnTrigUnion GetTrigState() {BmnTrigUnion u; u.storage = fTrigUnion; return u; }
    
    /** Get Input signal states, after reduction */
    UInt_t GetInputSignalsAR() {return fInputsAR; }
    
    /** Get Input signal states, before reduction */
    UInt_t GetInputSignalsBR() {return fInputsBR; }

    map<UInt_t, Long64_t> GetTimeShift() { return fTimeShift; }


    /** Set Event Header branch name */
    void SetHeaderName(TString header_name) { fHeaderName = header_name; }

    /** Set the event ID for this run
     * @param evid : unique event id
     */
    void SetEventId(UInt_t event_id) { fEventId = event_id; }

    /** Set the event ID for this run
     * @param evid : unique event id
     */
    void SetPeriodId(UInt_t period_id) { fPeriodId = period_id; }

    /** Set the time for this event in */
    void SetEventTimeTS(TTimeStamp event_time) { fEventTimeTS = event_time; }

    /** Set the type for this event
     * @param type : type (0 or 1)
     */
    void SetEventType(BmnEventType event_type) { fEventType = event_type; }

    void SetTripWord(Bool_t flag) { fTripWord = flag; }

    void SetSpillStart(Bool_t flag) { fSpillStart = flag; }

    void SetStartSignalInfo(Double_t time, Double_t width)
    {
        fStartSignalTime = time;
        fStartSignalWidth = width;
    }

    void SetTrigInfo(BmnTrigInfo* trig_info)
    {
        if (fTrigInfo) delete fTrigInfo;
        if (trig_info)
            fTrigInfo = new BmnTrigInfo(trig_info);
    }
    
    /** Set the trigger state */
    void SetTrigState(BmnTrigUnion &v) { fTrigUnion = v.storage; }
    
    /** Set the Input signal states, after reduction */
    void SetInputSignalsAR(UInt_t v) { fInputsAR = v; }
    
    /** Set the Input signal states, before reduction */
    void SetInputSignalsBR(UInt_t v) { fInputsBR = v; }

    void SetTimeShift(map <UInt_t, Long64_t> time_shift) { fTimeShift = time_shift; }

    ClassDef(BmnEventHeader, 2)
};

#endif /* BMNEVENTHEADER_H */
