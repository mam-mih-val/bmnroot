#ifndef BMNRUNHEADER_H
#define BMNRUNHEADER_H

#include "TNamed.h"
#include "TTimeStamp.h"
#include "BmnEnums.h"

class BmnRunHeader : public TNamed {
private:

    /** Run Id */
    UInt_t fRunId;
    /** Number of events in run */
    UInt_t fNev;
    /** Start time of run**/
    TTimeStamp fStartTime;
    /** Finish time of run**/
    TTimeStamp fFinishTime;

public:

    /** Default constructor */
    BmnRunHeader();

    /** Constructor */
    BmnRunHeader(UInt_t run, TTimeStamp st, TTimeStamp ft, UInt_t ne);

    /** Get the run ID for this run*/
    UInt_t GetRunId() {
        return fRunId;
    }
    
    UInt_t GetNEvents() {
        return fNev;
    }
        
    TTimeStamp GetStartTime() {
        return fStartTime;
    }
        
    TTimeStamp GetFinishTime() {
        return fFinishTime;
    }

    /** Set the run ID for this run
     * @param runid : unique run id
     */
    void SetRunId(UInt_t runid) {
        fRunId = runid;
    }
    
    void SetNEvents(UInt_t ne) {
        fNev = ne;
    }
   
    void SetStartTime(TTimeStamp time) {
        fStartTime = time;
    }
    
    void SetFinishTime(TTimeStamp time) {
        fFinishTime = time;
    }
    
    /**
     * Destructor
     */
    virtual ~BmnRunHeader();

    ClassDef(BmnRunHeader, 1)

};

#endif /* BMNRUNHEADER_H */

