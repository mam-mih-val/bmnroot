#include "BmnCSCDigit.h"

BmnCSCDigit::BmnCSCDigit() : BmnStripDigit() {

}

BmnCSCDigit::BmnCSCDigit(BmnCSCDigit* digit) : BmnStripDigit(digit) {

}

BmnCSCDigit::BmnCSCDigit(Int_t iStation, Int_t iModule, Int_t iStripLayer, Int_t iStripNumber, Double_t iStripSignal) :
BmnStripDigit(iStation, iModule, iStripLayer, iStripNumber, iStripSignal) {

}

BmnCSCDigit::~BmnCSCDigit() {

}

ClassImp(BmnCSCDigit)
