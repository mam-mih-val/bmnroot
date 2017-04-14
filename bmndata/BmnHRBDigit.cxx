#include "BmnHRBDigit.h"

BmnHRBDigit::BmnHRBDigit() {
}

BmnHRBDigit::BmnHRBDigit(UInt_t iSerial, UInt_t iChannel, UInt_t iSample, UInt_t iTH, UInt_t iTL) {
    fSerial = iSerial;
    fChannel = iChannel;
    fSample = iSample;
    fTimeHigh = iTH;
    fTimeLow = iTL;
}

BmnHRBDigit::~BmnHRBDigit() {
}

ClassImp(BmnHRBDigit)
