#include "BmnEventHeader.h"

// -----   Default constructor   -------------------------------------------
BmnEventHeader::BmnEventHeader() :
 FairEventHeader(),
 fHeaderName("BmnEventHeader."),
 fEventId(0),
 fPeriodId(0),
 fEventTimeTS(TTimeStamp()),
 fEventType(kBMNPAYLOAD),
 fTripWord(false),
 fStartSignalTime(0),
 fStartSignalWidth(0),
 fTrigInfo(new BmnTrigInfo())
{}

BmnEventHeader::BmnEventHeader(UInt_t run_id, UInt_t event_id, TTimeStamp event_time, BmnEventType event_type, Bool_t trip, BmnTrigInfo* trig_info) :
 FairEventHeader(),
 fHeaderName("BmnEventHeader."),
 fEventId(event_id),
 fEventTimeTS(event_time),
 fEventType(event_type),
 fTripWord(trip),
 fStartSignalTime(0),
 fStartSignalWidth(0),
 fTrigInfo(trig_info)
{
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
 fStartSignalTime(0),
 fStartSignalWidth(0),
 fTrigInfo(trig_info),
 fTimeShift(time_shift)
{
    SetRunId(run_id);
    SetEventTime(event_time.AsDouble());
}

// -----   Destructor   ----------------------------------------------------
BmnEventHeader::~BmnEventHeader() {}


ClassImp(BmnEventHeader)
