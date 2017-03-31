#include "BmnAlignCorrections.h"

BmnAlignCorrections::BmnAlignCorrections(TVector3 corr) :
fX(0.),
fY(0.),
fZ(0.) {
    fX = corr.X();
    fY = corr.Y();
    fZ = corr.Z();
}


