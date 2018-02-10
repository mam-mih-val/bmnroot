#include "BmnADCDigit.h"

BmnADCDigit::BmnADCDigit() {
    fSerial = 0;
    fChannel = 0;
    fNsmpl = 0;
    fValueU = new UShort_t[fNsmpl];
    fValueI = new Short_t[fNsmpl];
    for (Int_t i = 0; i < fNsmpl; ++i) {
        fValueI[i] = 0;
        fValueU[i] = 0;
    }
}

BmnADCDigit::BmnADCDigit(UInt_t iSerial, UInt_t iChannel, UInt_t n, UShort_t *iValue) {
    fSerial = iSerial;
    fChannel = iChannel;
    fNsmpl = n;
    fValueU = new UShort_t[fNsmpl];
    fValueI = new Short_t[fNsmpl];
    for (Int_t i = 0; i < fNsmpl; ++i) {
        fValueU[i] = iValue[i];
        fValueI[i] = 0;
    }
}

BmnADCDigit::BmnADCDigit(UInt_t iSerial, UInt_t iChannel, UInt_t n, Short_t *iValue) {
    fSerial = iSerial;
    fChannel = iChannel;
    fNsmpl = n;
    fValueI = new Short_t[fNsmpl];
    fValueU = new UShort_t[fNsmpl];
    for (Int_t i = 0; i < fNsmpl; ++i) {
        fValueI[i] = iValue[i];
        fValueU[i] = 0;
    }
}

BmnADCDigit::~BmnADCDigit() { 
    delete [] fValueI;
    delete [] fValueU;
}

ClassImp(BmnADCDigit)
