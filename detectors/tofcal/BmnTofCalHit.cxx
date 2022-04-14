
#include "BmnTofCalHit.h"

BmnTofCalHit::BmnTofCalHit() : BmnHit() {
 }

BmnTofCalHit::BmnTofCalHit(Int_t plane, Int_t bar, TVector3 pos, TVector3 dpos, Float_t time, Float_t energy)
: BmnHit(plane,pos,dpos,bar){
}

BmnTofCalHit::~BmnTofCalHit() {
 }

ClassImp(BmnTofCalHit)
