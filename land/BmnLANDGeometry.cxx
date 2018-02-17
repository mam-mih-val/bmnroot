// @(#)bmnroot/mwpc:$Id$
// Author: Maria Patsyuk <mpatsyuk@mit.edu> 2018-02-10

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// BmnLANDGeometry                                                            //
//                                                                            //
// A class containing geometry params. of                                     //
// the LAND.                                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
#include "BmnLANDGeometry.h"

BmnLANDGeometry::BmnLANDGeometry() :
fDebug(kFALSE) {
  fGlobalX = 130.9;
  fGlobalZ = 1425.;
  fGlobalAngle = 5.2; //degrees
}

BmnLANDGeometry::~BmnLANDGeometry() {

}
