#include "BmnRunHeader.h"

// -----   Default constructor   -------------------------------------------

BmnRunHeader::BmnRunHeader()
: fRunId(0),
fStartTime(0),
fFinishTime(0) {

}

BmnRunHeader::BmnRunHeader(UInt_t run, TTimeStamp st, TTimeStamp ft)
: fRunId(run),
fStartTime(st),
fFinishTime(ft) {

}
// -----   Destructor   ----------------------------------------------------

BmnRunHeader::~BmnRunHeader() {
}
// -------------------------------------------------------------------------

ClassImp(BmnRunHeader)
