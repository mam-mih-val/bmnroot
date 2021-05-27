#include "BmnArmTrigHit.h"

BmnArmTrigHit::BmnArmTrigHit():BmnHit() {
}

BmnArmTrigHit::BmnArmTrigHit(Int_t detID, TVector3 pos, TVector3 dpos, Int_t index, Int_t pdg)
: BmnHit(detID, pos, dpos, index) {
    fPdgId = pdg;
}

BmnArmTrigHit::~BmnArmTrigHit() {

}

ClassImp(BmnArmTrigHit)
