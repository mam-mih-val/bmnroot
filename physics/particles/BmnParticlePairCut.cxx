#include <cfloat>

#include "BmnParticlePairCut.h"

BmnParticlePairCut::BmnParticlePairCut() :
fDCA0(DBL_MAX),
fDCA12(DBL_MAX),
fDCA1(0.),
fDCA2(0.) {

    for (Int_t iPart = 0; iPart < 2; iPart++)
        for (Int_t iDet = 0; iDet < 2; iDet++)
            fNhits[iPart][iDet] = 0;
}