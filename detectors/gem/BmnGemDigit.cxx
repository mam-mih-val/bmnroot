#include "BmnGemDigit.h"

using namespace std;

ClassImp(BmnGemDigit)

BmnGemDigit::BmnGemDigit() {
//    fLighted = -1;
//    fNumber = -1;
//    fAdc = -1;
}

//......................................................................

BmnGemDigit::BmnGemDigit(Int_t iStrip, Int_t iStat, Float_t Q) {
    fLighted = iStrip;
    fNumber = iStat;
    fAdc = Q;
}

//......................................................................

BmnGemDigit::BmnGemDigit(BmnGemDigit* digit) {
    fLighted = digit->GetLightedStrip();
    fNumber = digit->GetStationNum();
    fAdc = digit->GetQ();
}

//......................................................................

BmnGemDigit::~BmnGemDigit() {

}

//......................................................................
