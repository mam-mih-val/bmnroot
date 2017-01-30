#include "BmnRunHeader.h"

// -----   Default constructor   -------------------------------------------

BmnRunHeader::BmnRunHeader()
: fRunId(0),
fNev(0),
fStartTime(0),
fFinishTime(0) {

}

BmnRunHeader::BmnRunHeader(UInt_t run, TTimeStamp st, TTimeStamp ft, UInt_t ne)
: fRunId(run),
fNev(ne),
fStartTime(st),
fFinishTime(ft) {

}
// -----   Destructor   ----------------------------------------------------

BmnRunHeader::~BmnRunHeader() {
}
// -------------------------------------------------------------------------

ClassImp(BmnRunHeader)
