
#include "BmnHit.h"

BmnHit::BmnHit() : FairHit() {

}

BmnHit::BmnHit(Int_t detID, TVector3 pos, TVector3 dpos, Int_t index)
: FairHit(detID, pos, dpos, index) {
    fUsing = kFALSE;
    fAddr = -1;
    fXaddr = -1;
    fYaddr = -1;
    fFlag = kFALSE;
    fIndex = -1;
    fDetId = kREF;
    fStation = 1;
    fModule = 1;
    fType = -1;
    fLength = -1.0;
    fResX = -1000.;
    fResY = -1000.;
}

BmnHit::~BmnHit() {

}

ClassImp(BmnHit)