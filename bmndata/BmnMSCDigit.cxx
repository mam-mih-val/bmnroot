#include "BmnMSCDigit.h"

BmnMSCDigit::BmnMSCDigit() :
fSerial(0),
fSlot(0) {
    for (Int_t i = 0; i < NVals; ++i)
        fValue[i] = 0;
}

BmnMSCDigit::BmnMSCDigit(UInt_t iSerial, UChar_t iSlot, UInt_t *iValue, UInt_t evId, TTimeStamp time) :
fSerial(iSerial),
fSlot(iSlot),
fEventId(evId),
fTS(time) {
    for (Int_t i = 0; i < NVals; ++i)
        fValue[i] = iValue[i];
}

BmnMSCDigit::BmnMSCDigit(UInt_t iSerial, UChar_t iSlot, UInt_t evId, TTimeStamp time) :
fSerial(iSerial),
fSlot(iSlot),
fEventId(evId),
fTS(time) {
}

BmnMSCDigit::~BmnMSCDigit() {
}

ClassImp(BmnMSCDigit)
