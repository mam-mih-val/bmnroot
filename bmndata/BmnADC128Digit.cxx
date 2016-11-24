#include "BmnADC128Digit.h"

BmnADC128Digit::BmnADC128Digit() {
}

BmnADC128Digit::BmnADC128Digit(UInt_t iSerial, UChar_t iChannel, UShort_t *iValue) {
    fSerial = iSerial;
    fChannel = iChannel;
    for (Int_t i = 0; i < 128/*2048*/; ++i)
        fValue[i] = iValue[i];
}

BmnADC128Digit::~BmnADC128Digit() {
}

ClassImp(BmnADC128Digit)
