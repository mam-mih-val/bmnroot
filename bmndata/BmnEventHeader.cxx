#include "BmnEventHeader.h"

// -----   Default constructor   -------------------------------------------

BmnEventHeader::BmnEventHeader()
: fRunId(0),
fEventId(0),
fType(kBMNPAYLOAD),
fEventTimeS(-1.),
fEventTimeNS(-1.) {

}

BmnEventHeader::BmnEventHeader(UInt_t run, UInt_t ev, Long64_t s, Long64_t ns, BmnEventType type)
: fRunId(run),
fEventId(ev),
fType(type),
fEventTimeS(s),
fEventTimeNS(ns) {

}
// -----   Destructor   ----------------------------------------------------

BmnEventHeader::~BmnEventHeader() {
}
// -------------------------------------------------------------------------

ClassImp(BmnEventHeader)
