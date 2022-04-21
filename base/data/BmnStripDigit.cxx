#include "BmnStripDigit.h"

BmnStripDigit::BmnStripDigit() {
    fStation = -1;
    fModule = 0;
    fStripLayer = 0;
    fStripNumber = 0;
    fStripSignal = 0.0;
    fIsGoodDigit = kTRUE;
}

BmnStripDigit::BmnStripDigit(BmnStripDigit* digit) {
    fStation = digit->fStation;
    fModule = digit->fModule;
    fStripLayer = digit->fStripLayer;
    fStripNumber = digit->fStripNumber;
    fStripSignal = digit->fStripSignal;
    fIsGoodDigit = digit->fIsGoodDigit; //AZ-260322
}

BmnStripDigit::BmnStripDigit(Int_t iStation, Int_t iModule, Int_t iStripLayer, Int_t iStripNumber, Double_t iStripSignal) {
    fStation = iStation;
    fModule = iModule;
    fStripLayer = iStripLayer;
    fStripNumber = iStripNumber;
    fStripSignal = iStripSignal;
    fIsGoodDigit = kTRUE; //AZ-260322
}

BmnStripDigit::~BmnStripDigit() {
}

ClassImp(BmnStripDigit)
