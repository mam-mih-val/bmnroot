#include "BmnGemStripDigit.h"

BmnGemStripDigit::BmnGemStripDigit() {
    fStation = -1;
    fModule = 0;
    fLowerStrip = 0;
    fUpperStrip = 0;
    fX = 0.0;
    fY = 0.0;
    fZ = 0.0;
}

BmnGemStripDigit::BmnGemStripDigit(BmnGemStripDigit* digit) {
    fStation = digit->fStation;
    fModule = digit->fModule;
    fLowerStrip = digit->fLowerStrip;
    fUpperStrip = digit->fUpperStrip;
    fX = digit->fX;
    fY = digit->fY;
    fZ = digit->fZ;
}

BmnGemStripDigit::BmnGemStripDigit(Int_t iStation, Int_t iModule, Int_t iLowStrip, Int_t iUpStrip, Double_t xcoord, Double_t ycoord, Double_t zcoord) {
    fStation = iStation;
    fModule = iModule;
    fLowerStrip = iLowStrip;
    fUpperStrip = iUpStrip;
    fX = xcoord;
    fY = ycoord;
    fZ = zcoord;
}

BmnGemStripDigit::~BmnGemStripDigit() {

}

ClassImp(BmnGemStripDigit)
