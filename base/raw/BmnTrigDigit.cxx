
#include "BmnTrigDigit.h"

BmnTrigDigit::BmnTrigDigit() {
    fMod = -1;
    fTime = -1.0;
    fAmp = -1.0;
    fTimestamp = -1.0;
}

BmnTrigDigit::BmnTrigDigit(Short_t iMod, Double_t iTime, Double_t iAmp, Double_t iTimestamp) {
    fMod = iMod;
    fTime = iTime;
    fAmp = iAmp;
    fTimestamp = iTimestamp;
}

BmnTrigDigit::~BmnTrigDigit() {}

ClassImp(BmnTrigDigit)
