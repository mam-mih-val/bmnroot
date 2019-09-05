#include "DstEventHeader.h"

// -----   Default constructor   -------------------------------------------
DstEventHeader::DstEventHeader() :
 FairEventHeader(),
 fHeaderName("DstEventHeader."),
 fEventId(0),
 fEventTimeTS(TTimeStamp()),
 fTriggerType(kBMNMINBIAS),
 fB(0),
 fZ2in(-1000),
 fZ2out(-1000),
 fADCin(-1000),
 fADCout(-1000)
{}

// -----   Constructor with parameters   -----------------------------------
DstEventHeader::DstEventHeader(UInt_t run_id, UInt_t event_id, TTimeStamp event_time, BmnTriggerType trigger_type, Double_t b) :
 FairEventHeader(),
 fHeaderName("DstEventHeader."),
 fEventId(event_id),
 fEventTimeTS(event_time),
 fTriggerType(trigger_type),
 fB(b),
 fZ2in(-1000),
 fZ2out(-1000),
 fADCin(-1000),
 fADCout(-1000)
{
    SetRunId(run_id);
    SetEventTime(event_time.AsDouble());
}

// -----   Destructor   ----------------------------------------------------
DstEventHeader::~DstEventHeader() {}


ClassImp(DstEventHeader)
