#include "BmnADCDigit.h"

BmnADCDigit::BmnADCDigit() :
isEmbedded(kFALSE) {
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

BmnADCDigit::BmnADCDigit(UInt_t iSerial, UShort_t iChannel, UInt_t n, vector<UShort_t> iValue) :
isEmbedded(kFALSE),
fValueI(nullptr) {
    fSerial = iSerial;
    fChannel = iChannel;
    fNsmpl = n;
    fValueU = new UShort_t[fNsmpl];
    for (Int_t i = 0; i < fNsmpl; ++i) {
        fValueU[i] = iValue[i];
    }
}

BmnADCDigit::BmnADCDigit(UInt_t iSerial, UShort_t iChannel, UInt_t n, UShort_t *iValue) :
isEmbedded(kFALSE),
fValueI(nullptr) {
    fSerial = iSerial;
    fChannel = iChannel;
    fNsmpl = n;
    fValueU = new UShort_t[fNsmpl];
    if (iValue)
        for (Int_t i = 0; i < fNsmpl; ++i) {
            fValueU[i] = iValue[i];
        }
}

BmnADCDigit::BmnADCDigit(UInt_t iSerial, UShort_t iChannel, UInt_t n, Short_t *iValue, Bool_t flagEmb) :
fValueU(nullptr) {
    isEmbedded = flagEmb;
    fSerial = iSerial;
    fChannel = iChannel;
    fNsmpl = n;
    fValueI = new Short_t[fNsmpl];
    if (iValue)
        for (Int_t i = 0; i < fNsmpl; ++i) {
            fValueI[i] = iValue[i];
        }
}

BmnADCDigit::~BmnADCDigit() {
    if (fValueI) delete [] fValueI;
    if (fValueU) delete [] fValueU;
}

ClassImp(BmnADCDigit)
