#include <cfloat>

#include "BmnParticlePairCut.h"

BmnParticlePairCut::BmnParticlePairCut() :
fDCA0(0.),
fDCA12(0.),
fDCA1(0.),
fDCA2(0.), fPath(0.),
fNhitsGemPos(0), fNhitsGemNeg(0) {
    fBackground = -1.;
    fSignalAndBackground = -1.;
    fMass = -1.;
    fWidth = -1.;
    
    fOrigin = "source";
}