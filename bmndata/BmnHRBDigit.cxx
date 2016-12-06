#include "BmnHRBDigit.h"

BmnHRBDigit::BmnHRBDigit() {
}

BmnHRBDigit::BmnHRBDigit(UInt_t iSerial, UInt_t iChannel, UInt_t iSample) {
    fSerial = iSerial;
    fChannel = iChannel;
    fSample = iSample;
}

BmnHRBDigit::~BmnHRBDigit() {
}

ClassImp(BmnHRBDigit)
