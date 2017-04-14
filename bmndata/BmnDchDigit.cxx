

#include "BmnDchDigit.h"


BmnDchDigit::BmnDchDigit() {
    fTime = -1;
    fPlane = -1;
    fWire = -1;
    fRef = -1;
}

BmnDchDigit::BmnDchDigit(UChar_t iPlane, Short_t iWire, Double_t iTime, Int_t refId) {
    fTime = iTime;
    fPlane = iPlane;
    fWire = iWire;
    fRef = refId;
}

BmnDchDigit::~BmnDchDigit() {

}

ClassImp(BmnDchDigit)
