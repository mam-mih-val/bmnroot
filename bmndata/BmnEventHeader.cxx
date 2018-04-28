#include "BmnEventHeader.h"

// -----   Default constructor   -------------------------------------------

BmnEventHeader::BmnEventHeader()
: fRunId(0),
fEventId(0),
fStartSignalTime(0),
fStartSignalWidth(0),
fEventTime(TDatime()),
fType(kBMNPAYLOAD) {
    fTrigInfo = NULL;
}

BmnEventHeader::BmnEventHeader(UInt_t run, UInt_t ev, TDatime time, BmnEventType type, Bool_t trip, BmnTrigInfo* info)
: fRunId(run),
fEventId(ev),
fType(type),
fEventTime(time),
fStartSignalTime(0),
fStartSignalWidth(0),
fTripWord(trip),
fTrigInfo(info) {

}

BmnEventHeader::BmnEventHeader(UInt_t run, UInt_t ev, TDatime time, BmnEventType type, Bool_t trip, BmnTrigInfo* info, map<UInt_t, Long64_t> ts)
: fRunId(run),
fEventId(ev),
fType(type),
fEventTime(time),
fTripWord(trip),
fStartSignalTime(0),
fStartSignalWidth(0),
fTrigInfo(info),
fTimeShift(ts) {

}
// -----   Destructor   ----------------------------------------------------

BmnEventHeader::~BmnEventHeader() {
}
// -------------------------------------------------------------------------

ClassImp(BmnEventHeader)
