#include "BmnGemStripDigit.h"

BmnGemStripDigit::BmnGemStripDigit() : BmnStripDigit() {

}

BmnGemStripDigit::BmnGemStripDigit(BmnGemStripDigit* digit) : BmnStripDigit(digit) {

}

BmnGemStripDigit::BmnGemStripDigit(Int_t iStation, Int_t iModule, Int_t iStripLayer, Int_t iStripNumber, Double_t iStripSignal) :
BmnStripDigit(iStation, iModule, iStripLayer, iStripNumber, iStripSignal) {

}

BmnGemStripDigit::~BmnGemStripDigit() {

}

ClassImp(BmnGemStripDigit)
