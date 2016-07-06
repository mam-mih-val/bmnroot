#include "BmnMSCDigit.h"

BmnMSCDigit::BmnMSCDigit() {
}

BmnMSCDigit::BmnMSCDigit(UInt_t iSerial, UChar_t iType, UChar_t iSlot, UShort_t *iValue, UInt_t iTime) {
    fSerial = iSerial;
    fType = iType;
    fSlot = iSlot;
    fTime = iTime;
    for (Int_t i = 0; i < 16; ++i) 
        fValue[i] = iValue[i];
}

BmnMSCDigit::~BmnMSCDigit() {
}

ClassImp(BmnMSCDigit)
