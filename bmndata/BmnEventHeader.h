
#ifndef BMNEVENTHEADER_H
#define BMNEVENTHEADER_H

#include <map>
#include "TNamed.h"
#include "TDatime.h"
#include "BmnEnums.h"
#include "BmnTrigInfo.h"

using namespace std;

class BmnEventHeader : public TNamed {
private:

    /** Run Id */
    UInt_t fRunId;
    /** Event Id **/
    UInt_t fEventId;
    /** Event Time**/
    TDatime fEventTime;
    /** Event Type (payload = 0 or pedestal = 1)**/
    BmnEventType fType;
//    /** Trigger Type (beam = 6 or target = 1)**/
//    BmnTriggerType fTrigType;
    /** Tripped Gems (1 bit for 1 GEM module)**/
    Bool_t fTripWord;
    /** Time shifts between T0-crate and others
     * first - crate serial ID
     * second - time shift (ns)
     **/
    map<UInt_t, Long64_t> fTimeShift;
    /** T0 information for current event**/
    Double_t fStartSignalTime; //ns
    Double_t fStartSignalWidth; //ns
    BmnTrigInfo* fTrigInfo;

public:

    /** Default constructor */
    BmnEventHeader();

    /** Constructor */
    BmnEventHeader(UInt_t run, UInt_t ev, TDatime time, BmnEventType type, Bool_t trip, BmnTrigInfo* info);

    /** Constructor */
    BmnEventHeader(UInt_t run, UInt_t ev, TDatime time, BmnEventType type, Bool_t trip, BmnTrigInfo* info, map<UInt_t, Long64_t> ts);

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
        if (!fTrigInfo)
            return kBMNBEAM;
        return fTrigInfo->GetTrigType();
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
    TDatime GetEventTime() {
        return fEventTime;
    }
    
    /** Get the spill statistics*/
    BmnTrigInfo* GetTrigInfo() {
        return fTrigInfo;
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
    void SetEventTime(TDatime time) {
        fEventTime = time;
    }

    /** Set the type for this event
     * @param type : type (0 or 1)
     */
    void SetEventTimeS(BmnEventType type) {
        fType = type;
    }

    void SetStartSignalInfo(Double_t time, Double_t width) {
        fStartSignalTime = time;
        fStartSignalWidth = width;
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
        fTrigInfo = info;
    }

    /**
     * Destructor
     */
    virtual ~BmnEventHeader();

    ClassDef(BmnEventHeader, 4)

};
#endif /* BMNEVENTHEADER_H */

