#include "BmnCscDigit.h"

BmnCscDigit::BmnCscDigit() {
    fStation = -1;
    fModule = 0;
    fStripLayer = 0;
    fStripNumber = 0;
    fStripSignal = 0.0;
    fIsGoodDigit = kTRUE;
}

BmnCscDigit::BmnCscDigit(BmnCscDigit* digit) {
    fStation = digit->fStation;
    fModule = digit->fModule;
    fStripLayer = digit->fStripLayer;
    fStripNumber = digit->fStripNumber;
    fStripSignal = digit->fStripSignal;
}

BmnCscDigit::BmnCscDigit(Short_t iStation, Short_t iModule, Short_t iStripLayer, Int_t iStripNumber, Double_t iStripSignal) {
    fStation = iStation;
    fModule = iModule;
    fStripLayer = iStripLayer;
    fStripNumber = iStripNumber;
    fStripSignal = iStripSignal;
}

BmnCscDigit::~BmnCscDigit() {

}

ClassImp(BmnCscDigit)
