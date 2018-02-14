
#include "BmnLANDHit.h"

BmnLANDHit::BmnLANDHit() : BmnHit() {
 }

BmnLANDHit::BmnLANDHit(Int_t plane, Int_t bar, TVector3 pos, Float_t time, Float_t energy)
: BmnHit(plane,posHit,TVector3(0,0,0),bar){
}

BmnLANDHit::~BmnLANDHit() {
 }

ClassImp(BmnLANDHit)
