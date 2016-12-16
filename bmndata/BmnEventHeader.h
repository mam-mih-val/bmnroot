
#ifndef BMNEVENTHEADER_H
#define BMNEVENTHEADER_H

#include "TNamed.h"
#include "TTimeStamp.h"
#include "BmnEnums.h"

class BmnEventHeader : public TNamed {
private:

    /** Run Id */
    UInt_t fRunId;
    /** Event Id **/
    UInt_t fEventId;
    /** Event Time**/
    TTimeStamp fEventTime;
    /** Event Type (payload = 0 or pedestal = 1)**/
    BmnEventType fType;
    /** Trigger Type (beam = 6 or target = 1)**/
    BmnTriggerType fTrigType;

public:

    /** Default constructor */
    BmnEventHeader();

    /** Constructor */
    BmnEventHeader(UInt_t run, UInt_t ev, TTimeStamp time, BmnEventType type, BmnTriggerType trig);

    /** Get the run ID for this run*/
    UInt_t GetRunId() {
        return fRunId;
    }
    
    /** Get the type of this event*/
    BmnEventType GetType() {
        return fType;
    }
    
    /** Get the type of this event*/
    BmnTriggerType GetTrig() {
        return fTrigType;
    }

    /** Get the run ID for this run*/
    UInt_t GetEventId() {
        return fEventId;
    }
    
    /** Get the time for this event*/
    TTimeStamp GetEventTime() {
        return fEventTime;
    }

    /** Set the run ID for this run
     * @param runid : unique run id
     */
    void SetRunId(UInt_t runid) {
        fRunId = runid;
    }

    /** Set the event ID for this run
     * @param runid : unique event id
     */
    void SetEventId(UInt_t evid) {
        fEventId = evid;
    }
   
    /** Set the time for this event in */
    void SetEventTime(TTimeStamp time) {
        fEventTime = time;
    }
    
    /** Set the type for this event
     * @param type : type (0 or 1)
     */
    void SetEventTimeS(BmnEventType type) {
        fType = type;
    }

    /**
     * Destructor
     */
    virtual ~BmnEventHeader();

    ClassDef(BmnEventHeader, 3)

};
#endif /* BMNEVENTHEADER_H */

