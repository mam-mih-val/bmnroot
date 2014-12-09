#include "BmnDchHit.h"

BmnDchHit::BmnDchHit() : FairHit() {

}

BmnDchHit::BmnDchHit(Int_t detID, TVector3 pos, TVector3 dpos, Int_t index)
: FairHit(detID, pos, dpos, index) {
    fUsing = kFALSE;
}

BmnDchHit::~BmnDchHit() {

}

ClassImp(BmnDchHit)