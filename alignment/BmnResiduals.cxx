#include "BmnResiduals.h"

BmnResiduals::BmnResiduals() {
    fdX = 0.;
    fdY = 0.;
    fdZ = 0.;
    fStation = -1;
    fModule = -1;
    fTrackId = -1;
    fHitId = -1;
    isResid = kFALSE;
    isField = kFALSE;
}

BmnResiduals::BmnResiduals(Int_t stat, Int_t mod, Double_t dx, Double_t dy, Double_t dz, Bool_t fieldFlag, Bool_t resFlag) {
    fdX = dx;
    fdY = dy;
    fdZ = dz;
    fStation = stat;
    fModule = mod;
    isResid = resFlag;
    isField = fieldFlag;
}
