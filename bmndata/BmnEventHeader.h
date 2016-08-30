
#ifndef BMNEVENTHEADER_H
#define BMNEVENTHEADER_H

#include "TNamed.h"

class BmnEventHeader : public TNamed {
public:

    /** Default constructor */
    BmnEventHeader();
    
    /** Constructor */
    BmnEventHeader(UInt_t run, UInt_t ev, Double_t s, Double_t ns);

    /** Get the run ID for this run*/
    UInt_t GetRunId() {
        return fRunId;
    }

    /** Get the run ID for this run*/
    UInt_t GetEventId() {
        return fEventId;
    }

    /** Get the time for this event in s*/
    Double_t GetEventTimeS() {
        return fEventTimeS;
    }

    /** Get the time for this event in ns*/
    Double_t GetEventTimeNS() {
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
    void SetEventTimeNS(Double_t time) {
        fEventTimeNS = time;
    }

    /** Set the time for this event in s
     * @param time : time in s
     */
    void SetEventTimeS(Double_t time) {
        fEventTimeS = time;
    }

    /**
     * Destructor
     */
    virtual ~BmnEventHeader();
protected:

    /** Run Id */
    UInt_t fRunId;
    /** Event Id **/
    UInt_t fEventId;
    /** Event Time in s**/
    Double_t fEventTimeS;
    /** Event Time in ns**/
    Double_t fEventTimeNS;

    ClassDef(BmnEventHeader, 3)

};
#endif /* BMNEVENTHEADER_H */

