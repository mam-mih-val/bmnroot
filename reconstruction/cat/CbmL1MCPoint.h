/*
 *====================================================================
 *
 *  CBM Level 1 Reconstruction 
 *  
 *  Authors: I.Kisel,  S.Gorbunov
 *
 *  e-mail : ikisel@kip.uni-heidelberg.de 
 *
 *====================================================================
 *
 *  L1 Monte Carlo information
 *
 *====================================================================
 */

#ifndef CbmL1MCPoint_H
#define CbmL1MCPoint_H

#include <vector>
using std::vector;

struct CbmL1MCPoint
{
  CbmL1MCPoint():
x(0), y(0), z(0), px(0), py(0), pz(0),
xIn(0),  yIn(0),  zIn(0),  pxIn(0),  pyIn(0),  pzIn(0),
xOut(0), yOut(0), zOut(0), pxOut(0), pyOut(0), pzOut(0),
p(0), q(0), mass(0),
pdg(0), ID(0), mother_ID(0),
iStation(0), pointId(-1), hitIds()
  {}
  
  double x, y, z, px, py, pz;
  double xIn,  yIn,  zIn,  pxIn,  pyIn,  pzIn;
  double xOut, yOut, zOut, pxOut, pyOut, pzOut;
  double p, q, mass;
  int pdg, ID, mother_ID;
  int iStation;
  int pointId;

  static bool compareIDz( const CbmL1MCPoint &a, const CbmL1MCPoint &b )
  {
    return ( a.ID < b.ID ) || ( ( a.ID == b.ID ) && (a.z < b.z) );
  }

  static bool pcompareIDz( const CbmL1MCPoint *a, const CbmL1MCPoint *b )
  {
    return ( a->ID < b->ID ) || ( ( a->ID == b->ID ) && (a->z < b->z) );
  }

  vector<int> hitIds; // indices of CbmL1StsHits in L1->vStsHits array
};

#endif
