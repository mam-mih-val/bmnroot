#include "BmnTDCDigit.h"

BmnTDCDigit::BmnTDCDigit() {
}

BmnTDCDigit::BmnTDCDigit(UInt_t iSerial, UChar_t iType, UChar_t iSlot, Bool_t iLeading, UChar_t iChannel, UChar_t iHptdcId, UInt_t iValue) {
    fSerial = iSerial;
    fType = iType;
    fSlot = iSlot;
    fLeading = iLeading;
    fHptdcId = iHptdcId;
    fChannel = iChannel;
    fValue = iValue;
}

BmnTDCDigit::~BmnTDCDigit() {
}

ClassImp(BmnTDCDigit)
