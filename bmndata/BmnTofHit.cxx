
#include "BmnTofHit.h"

BmnTofHit::BmnTofHit() : FairHit() {

}

BmnTofHit::BmnTofHit(Int_t detID, TVector3 pos, TVector3 dpos, Int_t index)
: FairHit(detID, pos, dpos, index) {
    fUsing = kFALSE;
}

BmnTofHit::~BmnTofHit() {

}

ClassImp(BmnTofHit)