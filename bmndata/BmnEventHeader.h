
#ifndef BMNEVENTHEADER_H
#define BMNEVENTHEADER_H

#include "TNamed.h"
#include "BmnEnums.h"

class BmnEventHeader : public TNamed {
private:

    /** Run Id */
    UInt_t fRunId;
    /** Event Id **/
    UInt_t fEventId;
    /** Event Time in s**/
    Long64_t fEventTimeS;
    /** Event Time in ns**/
    Long64_t fEventTimeNS;
    /** Event Type (payload = 0 or pedestal = 1)**/
    BmnEventType fType;

public:

    /** Default constructor */
    BmnEventHeader();

    /** Constructor */
    BmnEventHeader(UInt_t run, UInt_t ev, Long64_t s, Long64_t ns, BmnEventType type);

    /** Get the run ID for this run*/
    UInt_t GetRunId() {
        return fRunId;
    }
    
    /** Get the type of this event*/
    BmnEventType GetType() {
        return fType;
    }

    /** Get the run ID for this run*/
    UInt_t GetEventId() {
        return fEventId;
    }

    /** Get the time for this event in s*/
    Long64_t GetEventTimeS() {
        return fEventTimeS;
    }

    /** Get the time for this event in ns*/
    Long64_t GetEventTimeNS() {
        return fEventTimeNS;
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

    /** Set the time for this event in ns
     * @param time : time in ns
     */
    void SetEventTimeNS(Long64_t time) {
        fEventTimeNS = time;
    }

    /** Set the time for this event in s
     * @param time : time in s
     */
    void SetEventTimeS(Long64_t time) {
        fEventTimeS = time;
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

