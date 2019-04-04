#include "BmnEventHeader.h"

// -----   Default constructor   -------------------------------------------

BmnEventHeader::BmnEventHeader()
: fRunId(0),
fEventId(0),
fStartSignalTime(0),
fStartSignalWidth(0),
fEventTime(TTimeStamp()),
fType(kBMNPAYLOAD),
fTrigT0(0),
fTrigBC1(0),
fTrigBC2(0),
fTrigVETO(0),
fTrigSi(0),
fTrigBD(0) {
    fModT0.clear();
    fModBD.clear();
    fModSi.clear();
    fModVETO.clear();
    fModBC1.clear();
    fModBC2.clear();

    fAmpT0.clear();
    fAmpBD.clear();
    fAmpSi.clear();
    fAmpVETO.clear();
    fAmpBC1.clear();
    fAmpBC2.clear();

    fTimeBD.clear();
    fTimeSi.clear();
    fTimeVETO.clear();
    fTimeBC1.clear();
    fTimeBC2.clear();
    fTrigInfo = NULL;
}

BmnEventHeader::BmnEventHeader(UInt_t run, UInt_t ev, TTimeStamp time, BmnEventType type, Bool_t trip, BmnTrigInfo* info)
: fRunId(run),
fEventId(ev),
fType(type),
fEventTime(time),
fStartSignalTime(0),
fStartSignalWidth(0),
fTripWord(trip),
fTrigInfo(info) {

}

BmnEventHeader::BmnEventHeader(UInt_t run, UInt_t ev, TTimeStamp time, BmnEventType type, Bool_t trip, BmnTrigInfo* info, map<UInt_t, Long64_t> ts)
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

BmnEventHeader::BmnEventHeader(UInt_t run, UInt_t ev, TTimeStamp time, BmnEventType type, BmnTrigInfo* info, Bool_t trip, UInt_t T0, UInt_t BC1, UInt_t BC2, UInt_t VETO, UInt_t BD, UInt_t Si)
: fRunId(run),
fEventId(ev),
fType(type),
fEventTime(time),
fTripWord(trip),
fTrigInfo(info),
//fTrigType(trig),
fTrigT0(T0),
fTrigBC1(BC1),
fTrigBC2(BC2),
fTrigVETO(VETO),
fTrigSi(Si),
fTrigBD(BD) {
    fModT0.clear();
    fModBD.clear();
    fModSi.clear();
    fTimeSi.clear();
    fModVETO.clear();
    fModBC1.clear();
    fModBC2.clear();

    fAmpT0.clear();
    fAmpBD.clear();
    fAmpSi.clear();
    fAmpVETO.clear();
    fAmpBC1.clear();
    fAmpBC2.clear();

    fTimeBD.clear();
    fTimeSi.clear();
    fTimeVETO.clear();
    fTimeBC1.clear();
    fTimeBC2.clear();
}
// -----   Destructor   ----------------------------------------------------

BmnEventHeader::~BmnEventHeader() {
}
// -------------------------------------------------------------------------

ClassImp(BmnEventHeader)
