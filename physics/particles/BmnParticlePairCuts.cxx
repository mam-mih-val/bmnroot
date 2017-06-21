// @(#)bmnroot/physics/particles:$Id$
// Author: Pavel Batyuk <pavel.batyuk@jinr.ru> 2017-06-20

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// BmnParticlePairCuts                                                            //
//                                                                            //
//  A class to consider selected pair of particles                            //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#include "BmnParticlePairCuts.h"

BmnParticlePairCuts::BmnParticlePairCuts() : 
fV0XZ(0.),
fV0YZ(0.),
fVpPart1(0.), 
fVpPart2(0.), 
fV0Part1Part2(0.), 
fV0VpDist(0.) {

}

BmnParticlePairCuts::~BmnParticlePairCuts() {
    
}