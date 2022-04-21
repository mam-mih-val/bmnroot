#ifndef BMNSPILLHEADER_H
#define BMNSPILLHEADER_H

#include "BmnEnums.h"
#include "BmnTrigInfo.h"

#include "TTimeStamp.h"
#include "BmnTrigUnion.h"

using namespace std;

class BmnSpillHeader : public TNamed {
private:

    /** Event Id **/
    UInt_t fEventId;
    /** Period Id **/
    UInt_t fPeriodId;
    /** Event Time in TTimeStamp **/
    TTimeStamp fEventTimeTS;
    
    UInt_t fBeamTrigger;
    UInt_t fBTnBusy;
    UInt_t fL0;
    UInt_t fTrigProtection;
    UInt_t fBC1;
    UInt_t fBC2;
    UInt_t fBC3;
    UInt_t fAccepted;
    UInt_t fCand;
    UInt_t fBP;
    UInt_t fAP;
    UInt_t fRj;
    UInt_t fAll;
    UInt_t fAvail;
    UInt_t fBC1H;///< BC1 (high threshold)
    UInt_t fBC1BP;///< BC1 (before protection)
    UInt_t fBC1xBC2;
    UInt_t fBC1nBusy;
    UInt_t fIntTrig;
    UInt_t fSRCTrig;
    UInt_t fTrignBusy;///< Trigger * !Busy

public:
    /** Default constructor */
    BmnSpillHeader();

    /** Constructor */
    BmnSpillHeader(UInt_t run_id, UInt_t event_id, TTimeStamp event_time);

    /** Destructor */
    virtual ~BmnSpillHeader();
    
    void Clear();

    /** Get the the last event ID for the spill */
    UInt_t GetLastEventId() { return fEventId; }

    /** Get the period ID */
    UInt_t GetPeriodId() { return fPeriodId; }

    /** Get the time for this event */
    TTimeStamp GetEventTimeTS() { return fEventTimeTS; }
    
    void SetCand(UInt_t _v) {
        fCand = _v;
    }

    UInt_t GetCand() {
        return fCand;
    }

    void SetAccepted(UInt_t _v) {
        fAccepted = _v;
    }

    UInt_t GetAccepted() {
        return fAccepted;
    }

    void SetBefo(UInt_t _v) {
        fBP = _v;
    }

    UInt_t GetBefo() {
        return fBP;
    }

    void SetAfter(UInt_t _v) {
        fAP = _v;
    }

    UInt_t GetAfter() {
        return fAP;
    }

    void SetRjct(UInt_t _v) {
        fRj = _v;
    }

    UInt_t GetRjct() {
        return fRj;
    }

    void SetAll(UInt_t _v) {
        fAll = _v;
    }

    UInt_t GetAll() {
        return fAll;
    }

    void SetAvail(UInt_t _v) {
        fAvail = _v;
    }

    UInt_t GetAvail() {
        return fAvail;
    }
    
    

    void SetBT(UInt_t _v) {
        fBeamTrigger = _v;
    }

    UInt_t GetBT() {
        return fBeamTrigger;
    }

    void SetBTnBusy(UInt_t _v) {
        fBTnBusy = _v;
    }

    UInt_t GetBTnBusy() {
        return fBTnBusy;
    }

    void SetL0(UInt_t _v) {
        fL0 = _v;
    }

    UInt_t GetL0() {
        return fL0;
    }

    void SetProt(UInt_t _v) {
        fTrigProtection = _v;
    }

    UInt_t GetProt() {
        return fTrigProtection;
    }

    void SetBC1(UInt_t _v) {
        fBC1 = _v;
    }

    UInt_t GetBC1() {
        return fBC1;
    }

    void SetBC2(UInt_t _v) {
        fBC2 = _v;
    }

    UInt_t GetBC2() {
        return fBC2;
    }

    void SetBC3(UInt_t _v) {
        fBC3 = _v;
    }

    UInt_t GetBC3() {
        return fBC3;
    }
    // SRC trig counters
    void SetBC1H(UInt_t _v) {
        fBC1H = _v;
    }

    UInt_t GetBC1H() {
        return fBC1H;
    }

    void SetBC1BP(UInt_t _v) {
        fBC1BP = _v;
    }

    UInt_t GetBC1BP() {
        return fBC1BP;
    }

    void SetBC1xBC2(UInt_t _v) {
        fBC1xBC2 = _v;
    }

    UInt_t GetBC1xBC2() {
        return fBC1xBC2;
    }

    void SetBC1nBusy(UInt_t _v) {
        fBC1nBusy = _v;
    }

    UInt_t GetBC1nBusy() {
        return fBC1nBusy;
    }

    void SetIntTrig(UInt_t _v) {
        fIntTrig = _v;
    }

    UInt_t GetIntTrig() {
        return fIntTrig;
    }

    void SetSRCTrig(UInt_t _v) {
        fSRCTrig = _v;
    }

    UInt_t GetSRCTrig() {
        return fSRCTrig;
    }

    void SetTrignBusy(UInt_t _v) {
        fTrignBusy = _v;
    }

    UInt_t GetTrignBusy() {
        return fTrignBusy;
    }
    
    /** Set the last event ID for the spill
     * @param evid : unique event id
     */
    void SetLastEventId(UInt_t event_id) { fEventId = event_id; }

    /** Set the period ID
     * @param evid : unique period id
     */
    void SetPeriodId(UInt_t period_id) { fPeriodId = period_id; }

    /** Set the time for this event in */
    void SetEventTimeTS(TTimeStamp event_time) { fEventTimeTS = event_time; }
    
    ClassDef(BmnSpillHeader, 1)
};

#endif /* BMNSPILLHEADER_H */
