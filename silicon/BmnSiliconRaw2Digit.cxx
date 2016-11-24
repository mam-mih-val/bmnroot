#include "BmnSiliconRaw2Digit.h"

BmnSiliconRaw2Digit::BmnSiliconRaw2Digit() {
}

BmnSiliconRaw2Digit::BmnSiliconRaw2Digit(Int_t period, Int_t run) {

    fPeriod = period;
    fRun = run;
    fEventId = 0;
}

BmnSiliconRaw2Digit::~BmnSiliconRaw2Digit() {
}

BmnStatus BmnSiliconRaw2Digit::FillEvent(TClonesArray *adc, TClonesArray *silicon) {
    fEventId++;
}

ClassImp(BmnSiliconRaw2Digit)
