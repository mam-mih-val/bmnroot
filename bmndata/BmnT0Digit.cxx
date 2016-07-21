
#include "BmnT0Digit.h"

BmnT0Digit::BmnT0Digit() {
    fDet = -1;
    fMod = -1;
    fTime = -1.0;
    fAmp = -1.0;
}

BmnT0Digit::BmnT0Digit(Short_t iDet, Short_t iMod, Float_t iTime, Float_t iAmp) {
    fDet = iDet;
    fMod = iMod;
    fTime = iTime;
    fAmp = iAmp;
}

BmnT0Digit::~BmnT0Digit() {}

ClassImp(BmnT0Digit)
