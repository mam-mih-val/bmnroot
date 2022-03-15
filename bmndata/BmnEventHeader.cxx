#include "BmnEventHeader.h"

// -----   Default constructor   -------------------------------------------

BmnEventHeader::BmnEventHeader() :
FairEventHeader(),
fHeaderName("BmnEventHeader."),
fEventId(0),
fPeriodId(0),
fEventTimeTS(TTimeStamp()),
fEventType(kBMNPAYLOAD),
fTripWord(kFALSE),
fSpillStart(kFALSE),
fStartSignalTime(0),
fStartSignalWidth(0),
fInputsAR(0),
fInputsBR(0),
fTrigUnion(0) {
    fTrigInfo = new BmnTrigInfo();
}

BmnEventHeader::BmnEventHeader(UInt_t run_id, UInt_t event_id, TTimeStamp event_time, BmnEventType event_type, Bool_t trip, BmnTrigInfo* trig_info) :
FairEventHeader(),
fHeaderName("BmnEventHeader."),
fEventId(event_id),
fEventTimeTS(event_time),
fEventType(event_type),
fTripWord(trip),
fSpillStart(kFALSE),
fStartSignalTime(0),
fStartSignalWidth(0),
fInputsAR(0),
fInputsBR(0),
fTrigInfo(trig_info),
fTrigUnion(0) {
    SetRunId(run_id);
    SetEventTime(event_time.AsDouble());
}

BmnEventHeader::BmnEventHeader(UInt_t run_id, UInt_t event_id, TTimeStamp event_time, BmnEventType event_type, Bool_t trip, BmnTrigInfo* trig_info, map<UInt_t, Long64_t> time_shift) :
FairEventHeader(),
fHeaderName("BmnEventHeader."),
fEventId(event_id),
fEventTimeTS(event_time),
fEventType(event_type),
fTripWord(trip),
fSpillStart(kFALSE),
fStartSignalTime(0),
fStartSignalWidth(0),
fTrigInfo(trig_info),
fTimeShift(time_shift),
fTrigUnion(0) {
    SetRunId(run_id);
    SetEventTime(event_time.AsDouble());
}

// -----   Destructor   ----------------------------------------------------

BmnEventHeader::~BmnEventHeader() {
    delete fTrigInfo;
}

void BmnEventHeader::Clear() {
    fHeaderName = "";
    fEventId = 0;
    fPeriodId = 0;
    fEventTimeTS = TTimeStamp();
    fEventType = (BmnEventType) 0;
    fTripWord = kFALSE;
    fSpillStart = kFALSE;
    fStartSignalTime = 0.0;
    fStartSignalWidth = 0.0;
    if (fTrigInfo) {
        delete fTrigInfo;
        fTrigInfo = nullptr;
//        fTrigInfo = new BmnTrigInfo();
    }
    fTrigUnion = 0;
}


ClassImp(BmnEventHeader)
