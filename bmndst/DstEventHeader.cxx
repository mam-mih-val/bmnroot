#include "DstEventHeader.h"

// -----   Default constructor   -------------------------------------------
DstEventHeader::DstEventHeader() : FairEventHeader(),
                                   fHeaderName("DstEventHeader."),
                                   fEventId(0),
                                   fEventTimeTS(TTimeStamp()),
                                   fTriggerType(kBMNMINBIAS),
                                   fB(0),
                                   fZ2in(-100.0),
                                   fZ2out(-100.0),
                                   fADCin(-100.0),
                                   fADCout(-100.0),
                                   fZ1(-100.0),
                                   fZ2(-100.0),
                                   fZ3(-100.0),
                                   fZ4(-100.0),
                                   fADC1(-100.0),
                                   fADC2(-100.0),
                                   fADC3(-100.0),
                                   fADC4(-100.0),
                                   fZin(-100) {}

// -----   Constructor with parameters   -----------------------------------
DstEventHeader::DstEventHeader(UInt_t run_id, UInt_t event_id, TTimeStamp event_time, BmnTriggerType trigger_type, Double_t b) : FairEventHeader(),
                                                                                                                                 fHeaderName("DstEventHeader."),
                                                                                                                                 fEventId(event_id),
                                                                                                                                 fEventTimeTS(event_time),
                                                                                                                                 fTriggerType(trigger_type),
                                                                                                                                 fB(b),
                                                                                                                                 fZ2in(-100.0),
                                                                                                                                 fZ2out(-100.0),
                                                                                                                                 fADCin(-100.0),
                                                                                                                                 fADCout(-100.0),
                                                                                                                                 fZ1(-100.0),
                                                                                                                                 fZ2(-100.0),
                                                                                                                                 fZ3(-100.0),
                                                                                                                                 fZ4(-100.0),
                                                                                                                                 fADC1(-100.0),
                                                                                                                                 fADC2(-100.0),
                                                                                                                                 fADC3(-100.0),
                                                                                                                                 fADC4(-100.0),
                                                                                                                                 fZin(-100) {
    SetRunId(run_id);
    SetEventTime(event_time.AsDouble());
}

// -----   Destructor   ----------------------------------------------------
DstEventHeader::~DstEventHeader() {}

ClassImp(DstEventHeader)
