#include "BmnEventHeader.h"

// -----   Default constructor   -------------------------------------------

BmnEventHeader::BmnEventHeader()
: fRunId(0),
fEventId(0),
fStartSignalTime(0),
fStartSignalWidth(0),
fEventTime(TDatime()),
fType(kBMNPAYLOAD),
fTrigType(kBMNBEAM) {

}

BmnEventHeader::BmnEventHeader(UInt_t run, UInt_t ev, TDatime time, BmnEventType type, BmnTriggerType trig, Bool_t trip)
: fRunId(run),
fEventId(ev),
fType(type),
fEventTime(time),
fStartSignalTime(0),
fStartSignalWidth(0),
fTripWord(trip),
fTrigType(trig) {

}

BmnEventHeader::BmnEventHeader(UInt_t run, UInt_t ev, TDatime time, BmnEventType type, BmnTriggerType trig, Bool_t trip, map<UInt_t, Long64_t> ts)
: fRunId(run),
fEventId(ev),
fType(type),
fEventTime(time),
fTripWord(trip),
fStartSignalTime(0),
fStartSignalWidth(0),
fTrigType(trig),
fTimeShift(ts) {

}
// -----   Destructor   ----------------------------------------------------

BmnEventHeader::~BmnEventHeader() {
}
// -------------------------------------------------------------------------

ClassImp(BmnEventHeader)
