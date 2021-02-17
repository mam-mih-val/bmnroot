#include "BmnArmTrigHit.h"

BmnArmTrigHit::BmnArmTrigHit():BmnHit() {
}

BmnArmTrigHit::BmnArmTrigHit(Int_t detID, TVector3 pos, TVector3 dpos, Int_t index)
: BmnHit(detID, pos, dpos, index) {
}

BmnArmTrigHit::~BmnArmTrigHit() {

}

ClassImp(BmnArmTrigHit)
