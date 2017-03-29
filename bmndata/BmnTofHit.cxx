
#include "BmnTofHit.h"

BmnTofHit::BmnTofHit() : BmnHit() {
 }

BmnTofHit::BmnTofHit(Int_t detID, TVector3 pos, TVector3 dpos, Int_t index)
: BmnHit(detID, pos, dpos, index){
}

BmnTofHit::~BmnTofHit() {
 }

ClassImp(BmnTofHit)