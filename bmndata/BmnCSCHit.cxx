#include "BmnCSCHit.h"

BmnCSCHit::BmnCSCHit() : BmnHit() {
    fELoss  = 0.0;
    fNormSignalDiff = -1.0;
    fLowerLayerClusterSize = 0;
    fUpperLayerClusterSize = 0;
    fLowerLayerStripPos = -1.0;
    fUpperLayerStripPos = -1.0;
    fLowerLayerStripTotalSignal = 0.0;
    fUpperLayerStripTotalSignal = 0.0;
}

BmnCSCHit::BmnCSCHit(Int_t detID, TVector3 pos, TVector3 dpos, Int_t index)
: BmnHit(detID, pos, dpos, index) {
    fELoss = 0.0;
    fNormSignalDiff = -1.0;
    fLowerLayerClusterSize = 0;
    fUpperLayerClusterSize = 0;
    fLowerLayerStripPos = -1.0;
    fUpperLayerStripPos = -1.0;
    fLowerLayerStripTotalSignal = 0.0;
    fUpperLayerStripTotalSignal = 0.0;
}

BmnCSCHit::~BmnCSCHit() {

}

ClassImp(BmnCSCHit)
