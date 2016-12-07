#include "BmnEventHeader.h"

// -----   Default constructor   -------------------------------------------

BmnEventHeader::BmnEventHeader()
: fRunId(0),
fEventId(0),
fEventTime(0),
fType(kBMNPAYLOAD) {

}

BmnEventHeader::BmnEventHeader(UInt_t run, UInt_t ev, TTimeStamp time, BmnEventType type)
: fRunId(run),
fEventId(ev),
fType(type),
fEventTime(time) {

}
// -----   Destructor   ----------------------------------------------------

BmnEventHeader::~BmnEventHeader() {
}
// -------------------------------------------------------------------------

ClassImp(BmnEventHeader)
