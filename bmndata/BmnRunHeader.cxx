#include "BmnRunHeader.h"

// -----   Default constructor   -------------------------------------------

BmnRunHeader::BmnRunHeader()
: fRunId(0),
fNev(0),
fStartTime(TDatime()),
fFinishTime(TDatime()) {

}

BmnRunHeader::BmnRunHeader(UInt_t run, TDatime st, TDatime ft, UInt_t ne)
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
