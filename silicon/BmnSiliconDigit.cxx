#include "BmnSiliconDigit.h"

BmnSiliconDigit::BmnSiliconDigit() {
    fStation = -1;
    fModule = 0;
    fLayer = 0;
    fStrip = 0;
    fSignal = 0.0;
    fIsGoodDigit = kTRUE;
}

BmnSiliconDigit::BmnSiliconDigit(BmnSiliconDigit* digit) {
    fStation = digit->fStation;
    fModule = digit->fModule;
    fLayer = digit->fLayer;
    fStrip = digit->fStrip;
    fSignal = digit->fSignal;
}

BmnSiliconDigit::BmnSiliconDigit(Int_t iStation, Int_t iModule, Int_t iLayer, Int_t iStrip, Double_t iSignal) {
    fStation = iStation;
    fModule = iModule;
    fLayer = iLayer;
    fStrip = iStrip;
    fSignal = iSignal;
}

BmnSiliconDigit::~BmnSiliconDigit() {

}

ClassImp(BmnSiliconDigit)