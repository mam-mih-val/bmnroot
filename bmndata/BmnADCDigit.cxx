#include "BmnADCDigit.h"

BmnADCDigit::BmnADCDigit() {
    fSerial = 0;;
    fChannel = 0;
    fNsmpl = 0;
    fValue = new UShort_t[fNsmpl];
    for (Int_t i = 0; i < fNsmpl; ++i)
        fValue[i] = 0.0;
}

BmnADCDigit::BmnADCDigit(UInt_t iSerial, UInt_t iChannel, UInt_t n, UShort_t *iValue) {
    fSerial = iSerial;
    fChannel = iChannel;
    fNsmpl = n;
    fValue = new UShort_t[fNsmpl];
    for (Int_t i = 0; i < fNsmpl; ++i)
        fValue[i] = iValue[i];
}

BmnADCDigit::~BmnADCDigit() { 
    delete [] fValue;
}

ClassImp(BmnADCDigit)
