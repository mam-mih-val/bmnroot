#include "BmnADC32Digit.h"

BmnADC32Digit::BmnADC32Digit() {
}

BmnADC32Digit::BmnADC32Digit(UInt_t iSerial, UInt_t iChannel, UInt_t *iValue) {
    fSerial = iSerial;
    fChannel = iChannel;
    for (Int_t i = 0; i < 32/*2048*/; ++i)
        fValue[i] = iValue[i];
}

BmnADC32Digit::~BmnADC32Digit() {
}

ClassImp(BmnADC32Digit)
