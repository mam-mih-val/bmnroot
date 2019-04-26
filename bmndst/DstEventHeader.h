#ifndef DSTEVENTHEADER_H
#define DSTEVENTHEADER_H

#include "BmnEnums.h"

#include "FairEventHeader.h"
#include "FairRootManager.h"

#include "TTimeStamp.h"

class DstEventHeader : public FairEventHeader
{
 private:
    /** Event Header branch name **/
    TString fHeaderName;    //!

    /** Event Identifier **/
    UInt_t fEventId;
    /** Event Time in TTimeStamp **/
    TTimeStamp fEventTimeTS;
    /** Trigger Type (beam = 6 or target = 1) **/
    BmnTriggerType fTriggerType;
    /** Impact parameter [fm] **/
    Double_t fB;

 public:
    /** Default constructor */
    DstEventHeader();

    /** Constructor with parameters */
    DstEventHeader(UInt_t run_id, UInt_t event_id, TTimeStamp event_time, BmnTriggerType trigger_type, Double_t b);

    /** Destructor */
    virtual ~DstEventHeader();

    virtual void Register(Bool_t Persistence = kTRUE)
    {
        FairRootManager::Instance()->Register(fHeaderName.Data(), "EvtHeader", this, Persistence);
    }


    /** Get Event Header branch name */
    TString GetHeaderName() { return fHeaderName; }

    /** Get run ID for this run */
    UInt_t GetEventId() { return fEventId; }

    /** Get time for this event */
    TTimeStamp GetEventTimeTS() { return fEventTimeTS; }

    /** Get type of this event */
    BmnTriggerType GetTriggerType() { return fTriggerType; }

    /** Get impact parameter - b, fm */
    Double_t GetB() { return fB; }


    /** Set Event Header branch name */
    void SetHeaderName(TString header_name) { fHeaderName = header_name; }

    /** Set event ID for this run */
    void SetEventId(UInt_t event_id) { fEventId = event_id; }

    /** Set time for this event in */
    void SetEventTimeTS(TTimeStamp event_time) { fEventTimeTS = event_time; }

    /** Set trigger type for this event
     * @param trigger_type : trigger type (beam = 6 or target = 1)
     */
    void SetTriggerType(BmnTriggerType trigger_type) { fTriggerType = trigger_type; }

    /** Set impact parameter - b, fm */
    void SetB(Double_t b) { fB = b; }


    ClassDef(DstEventHeader, 1)
};

#endif /* DstEventHeader_H */
