#include "BmnGemAlignmentCorrections.h"

BmnGemAlignmentCorrections::BmnGemAlignmentCorrections(Int_t stat, Int_t mod, TVector3 corr) {
    fStation = stat;
    fModule = mod;
    fX = corr.X();
    fY = corr.Y();
    fZ = corr.Z();
}


