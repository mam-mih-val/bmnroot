#include "BmnGemStripHit.h"

BmnGemStripHit::BmnGemStripHit() : BmnHit() {

}

BmnGemStripHit::BmnGemStripHit(Int_t detID, TVector3 pos, TVector3 dpos, Int_t index)
: BmnHit(detID, pos, dpos, index) {
    
}

BmnGemStripHit::~BmnGemStripHit() {
    
}

ClassImp(BmnGemStripHit)