// @(#)bmnroot/physics/particles:$Id$
// Author: Pavel Batyuk <pavel.batyuk@jinr.ru> 2017-06-20

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// BmnParticlePair                                                            //
//                                                                            //
//  A class to consider selected pair of particles                            //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#include "BmnParticlePair.h"

BmnParticlePair::BmnParticlePair() :
fV0XZ(0.), fV0YZ(0.),
fDCA1(0.),
fDCA2(0.),
fDCA12X(0.), fDCA12Y(0.),
fPathX(0.), fPathY(0.),
fInvMassX(0.), fInvMassY(0.), 
fMomPart1(0.), fMomPart2(0.),
fEtaPart1(0.), fEtaPart2(0.) {

}

BmnParticlePair::~BmnParticlePair() {

}