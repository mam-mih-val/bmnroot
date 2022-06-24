#include "BmnSpillHeader.h"

// -----   Default constructor   -------------------------------------------

BmnSpillHeader::BmnSpillHeader() :
fPeriodId(0)
, fEventId(0)
, fEventTimeTS(TTimeStamp())
, fBeamTrigger(0)
, fBTnBusy(0)
, fL0(0)
, fTrigProtection(0)
, fBC1(0)
, fBC2(0)
, fBC3(0)
, fAccepted(0)
, fCand(0)
, fBP(0)
, fAP(0)
, fRj(0)
, fAll(0)
, fAvail(0)
, fBC1H(0)
, fBC1BP(0)
, fBC1xBC2(0)
, fBC1nBusy(0)
, fIntTrig(0)
, fSRCTrig(0)
, fTrignBusy(0) {
}

BmnSpillHeader::BmnSpillHeader(UInt_t run_id, UInt_t event_id, TTimeStamp event_time) :
fPeriodId(run_id)
, fEventId(event_id)
, fEventTimeTS(event_time)
, fBeamTrigger(0)
, fBTnBusy(0)
, fL0(0)
, fTrigProtection(0)
, fBC1(0)
, fBC2(0)
, fBC3(0)
, fAccepted(0)
, fCand(0)
, fBP(0)
, fAP(0)
, fRj(0)
, fAll(0)
, fAvail(0)
, fBC1H(0)
, fBC1BP(0)
, fBC1xBC2(0)
, fBC1nBusy(0)
, fIntTrig(0)
, fSRCTrig(0)
, fTrignBusy(0) {
}


// -----   Destructor   ----------------------------------------------------

BmnSpillHeader::~BmnSpillHeader() {
}

void BmnSpillHeader::Clear() {
    fEventId = 0;
    fPeriodId = 0;
    fEventTimeTS = TTimeStamp();
    fBeamTrigger = 0;
    fBTnBusy = 0;
    fL0 = 0;
    fTrigProtection = 0;
    fBC1 = 0;
    fBC2 = 0;
    fBC3 = 0;
    fAccepted = 0;
    fCand = 0;
    fBP = 0;
    fAP = 0;
    fRj = 0;
    fAll = 0;
    fAvail = 0;
    fBC1H = 0;
    fBC1BP = 0;
    fBC1xBC2 = 0;
    fBC1nBusy = 0;
    fIntTrig = 0;
    fSRCTrig = 0;
    fTrignBusy = 0;
}


ClassImp(BmnSpillHeader)
