#ifndef _L1StsHit_h_
#define _L1StsHit_h_

#include "L1Strip.h"

typedef unsigned /*short*/ int THitI; // strip index type
typedef unsigned short int TZPosI;    // strip z-coor index type

class L1StsHit
{
  public:
  TStripI f, b; // front and back strip indices
  TZPosI iz; // index of z coor. in L1Algo::vStsZPos
  
  unsigned short int n; // number of event
  
  L1StsHit():f(0),b(0),iz(0),n(0) {}
  L1StsHit(L1StsHit &h, unsigned short int _n, /*short*/ int sh, /*short*/ int shB):f(h.f + sh),b(h.b + shB),iz(0),n(_n){}

};

#endif
