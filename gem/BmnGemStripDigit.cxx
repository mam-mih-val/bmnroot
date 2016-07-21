#include "BmnGemStripDigit.h"

BmnGemStripDigit::BmnGemStripDigit() {
    fStation = -1;
    fModule = 0;
    fStripLayer = 0;
    fStripNumber = 0;
    fStripSignal = 0.0;
    fStripSignalNoise = 0.0;
}

BmnGemStripDigit::BmnGemStripDigit(BmnGemStripDigit* digit) {
    fStation = digit->fStation;
    fModule = digit->fModule;
    fStripLayer = digit->fStripLayer;
    fStripNumber = digit->fStripNumber;
    fStripSignal = digit->fStripSignal;
    fStripSignalNoise = digit->fStripSignalNoise;
}

BmnGemStripDigit::BmnGemStripDigit(Int_t iStation, Int_t iModule, Int_t iStripLayer, Int_t iStripNumber, Double_t iStripSignal) {
    fStation = iStation;
    fModule = iModule;
    fStripLayer = iStripLayer;
    fStripNumber = iStripNumber;
    fStripSignal = iStripSignal;
    fStripSignalNoise = 0.0;
}

BmnGemStripDigit::BmnGemStripDigit(Int_t iStation, Int_t iModule, Int_t iStripLayer, Int_t iStripNumber, Double_t iStripSignal, Double_t iStripSignalNoise) {
    fStation = iStation;
    fModule = iModule;
    fStripLayer = iStripLayer;
    fStripNumber = iStripNumber;
    fStripSignal = iStripSignal;
    fStripSignalNoise = iStripSignalNoise;
}

BmnGemStripDigit::~BmnGemStripDigit() {

}

ClassImp(BmnGemStripDigit)
