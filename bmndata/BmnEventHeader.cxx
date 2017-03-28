#include "BmnEventHeader.h"

// -----   Default constructor   -------------------------------------------

BmnEventHeader::BmnEventHeader()
: fRunId(0),
fEventId(0),
fEventTime(TDatime()),
fType(kBMNPAYLOAD),
fTrigType(kBMNBEAM) {

}

BmnEventHeader::BmnEventHeader(UInt_t run, UInt_t ev, TDatime time, BmnEventType type, BmnTriggerType trig, Bool_t trip)
: fRunId(run),
fEventId(ev),
fType(type),
fEventTime(time),
fTripWord(trip),
fTrigType(trig) {

}
// -----   Destructor   ----------------------------------------------------

BmnEventHeader::~BmnEventHeader() {
}
// -------------------------------------------------------------------------

ClassImp(BmnEventHeader)
