#include "BmnAlignCorrections.h"

BmnAlignCorrections::BmnAlignCorrections(TVector3 corr) {
    fX = corr.X();
    fY = corr.Y();
    fZ = corr.Z();
}


