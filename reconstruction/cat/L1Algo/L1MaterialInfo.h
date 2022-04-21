#ifndef L1MaterialInfo_h
#define L1MaterialInfo_h

#include "CbmL1Def.h"
#include <vector>
using std::vector;

class L1MaterialInfo{
 public:
 L1MaterialInfo(): thick(0), RL(0),  RadThick(0), logRadThick(0) {};
   
  fvec thick, RL,  RadThick, logRadThick;
} _fvecalignment;


class L1Material{
 public:

 L1Material() : table(), NBins(0), RMax(0.), iD(0.) {}

  vector<vector<float> > table;
  
  // static const int NBins = 100; // TODO file?
  // static const float RMax = 60.f;
  // static const float iD = 0.5*NBins/60.f;//RMax!;

  void SetBins( int n, float r ) {
    NBins = n;
    RMax = r;
    iD = 0.5*NBins/RMax;
  }

  float GetRadThick( float x, float y ) {
    x = ( x < RMax && x >= -RMax ) ? x : 0;
    y = ( y < RMax && y >= -RMax ) ? y : 0;
    // L1_ASSERT( x < RMax && x >= -RMax, "x = " << x );
    // L1_ASSERT( y < RMax && y >= -RMax, "y = " << y );
    return table[ (x + RMax)*iD ][ (y + RMax)*iD ];
  }
  fvec GetRadThick( fvec x, fvec y ) {
    fvec r;
    for ( int i = 0; i < fvecLen; i++ )
      r[i] = GetRadThick(x[i], y[i]);
    return r;
  }

  int NBins;
  float RMax;
  float iD;

} _fvecalignment;

#endif
