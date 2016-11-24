
#include "BmnTrigDigit.h"

BmnTrigDigit::BmnTrigDigit() {
    fDet = -1;
    fMod = -1;
    fTime = -1.0;
    fAmp = -1.0;
}

BmnTrigDigit::BmnTrigDigit(Short_t iDet, Short_t iMod, Double_t iTime, Double_t iAmp) {
    fDet = iDet;
    fMod = iMod;
    fTime = iTime;
    fAmp = iAmp;
}

BmnTrigDigit::~BmnTrigDigit() {}

ClassImp(BmnTrigDigit)
