#include "BmnMSCDigit.h"

BmnMSCDigit::BmnMSCDigit() {
    fSerial = 0;
    fSlot = 0;
    fTime = 0;
    for (Int_t i = 0; i < 16; ++i) 
        fValue[i] = 0;
}

BmnMSCDigit::BmnMSCDigit(UInt_t iSerial, UChar_t iSlot, UInt_t *iValue, UInt_t evId, UInt_t iTime) {
    fSerial = iSerial;
    fSlot = iSlot;
    fEventId = evId;
    fTime = iTime;
    for (Int_t i = 0; i < 16; ++i) 
        fValue[i] = iValue[i];
}

BmnMSCDigit::BmnMSCDigit(UInt_t iSerial, UChar_t iSlot, UInt_t evId, UInt_t iTime) {
    fSerial = iSerial;
    fSlot = iSlot;
    fEventId = evId;
    fTime = iTime;
}

BmnMSCDigit::~BmnMSCDigit() {
}

ClassImp(BmnMSCDigit)
