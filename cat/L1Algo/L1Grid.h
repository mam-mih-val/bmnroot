//-*- Mode: C++ -*-
// $Id: AliHLTTPCCAGrid.h,v 1.1.1.1 2010/07/26 20:55:38 ikulakov Exp $
// ************************************************************************
// This file is property of and copyright by the ALICE HLT Project        *
// ALICE Experiment at CERN, All rights reserved.                         *
// See cxx source for full Copyright notice                               *
//                                                                        *
//*************************************************************************

#ifndef L1GRID_H
#define L1GRID_H

#include "CbmL1Def.h"
#include <algorithm>
#include <string.h>

#include <assert.h>
#include <cstdio>

using namespace std;

   /// Copy to memory block [@dest, @dest+@num] num number of times the value of i of type @T with size @typesize.
  /// uses binary expansion of copied volume for speed up
template< typename T>
void memset( T *dest, T i, size_t num ) {
  const size_t tsize = sizeof(T);
  unsigned int lastBin = 0;
  dest[0] = i;
  while ( lastBin+1 < num ) {
    unsigned int l = lastBin + 1;
    l = lastBin + l + 1 > num ? num - lastBin - 1 : l;
    memcpy( dest + lastBin + 1, dest, l*tsize );
    lastBin += l;
  }
}


/**
 * @class L1Grid
 *
 * 2-dimensional grid of pointers.
 * pointers to (y,z)-like objects are assigned to the corresponding grid bin
 * used by L1Tracker to speed-up the hit operations
 * grid axis are named Z,Y to be similar to TPC row coordinates.
 */
class L1Grid {
 public:
  L1Grid():
    fN(0), fNy(0), fNz(0), fYMinOverStep(0.), fZMinOverStep(0.),
    fStepYInv(0.), fStepZInv(0.), fFirstHitInBin(NULL){}

  ~L1Grid(){ if ( fFirstHitInBin ) delete[] fFirstHitInBin; }
   
  void Create( float yMin, float yMax, float zMin, float zMax, float sy, float sz  );
  void Fill( const L1HitPoint* points, THitI n ); // call after sort
  
  int GetBin( float Y, float Z ) const;

  unsigned int GetBinBounded( const float &Y, const float &Z ) const;
  void GetBinBounded( const float &Y, const float &Z, unsigned short *bY, unsigned short *bZ ) const;
  void GetBinBounds( unsigned int iBin, float &Ymin, float &Ymax, float &Zmin, float &Zmax) const;
    
  unsigned int   N()        const { return fN;  }
  unsigned short Ny()       const { return fNy; }
  unsigned short Nz()       const { return fNz; }

  THitI FirstHitInBin(int i) const  { return fFirstHitInBin[i]; }
  
 private:

  unsigned int   fN;       //* total N bins
  unsigned short fNy;      //* N bins in Y
  unsigned short fNz;      //* N bins in Z
  float fYMinOverStep;     //* minimal Y value * fStepYInv
  float fZMinOverStep;     //* minimal Z value * fStepZInv
  float fStepYInv; //* inverse bin size in Y
  float fStepZInv; //* inverse bin size in Z

  THitI* fFirstHitInBin;

  L1Grid(const L1Grid&);
  L1Grid& operator=(const L1Grid&);
};

inline unsigned int L1Grid::GetBinBounded( const float &Y, const float &Z ) const
{
  //* get the bin pointer

  unsigned short yBin, zBin;
  GetBinBounded( Y, Z, &yBin, &zBin );
  return (unsigned int)zBin * (unsigned int)fNy + (unsigned int)yBin;
}

inline void L1Grid::GetBinBounded( const float &Y, const float &Z, unsigned short *bY, unsigned short *bZ ) const
{
  const float &yBin = ( Y * fStepYInv - fYMinOverStep );
  const float &zBin = ( Z * fStepZInv - fZMinOverStep );

  *bY = max( 0.f, min( float( fNy - 1 ), yBin ) );
  *bZ = max( 0.f, min( float( fNz - 1 ), zBin ) );
}

inline void L1Grid::Create( float yMin, float yMax, float zMin, float zMax, float sy, float sz )
{
  //* Create the grid
  fStepYInv = 1.f / sy;
  fStepZInv = 1.f / sz;

  fYMinOverStep = yMin * fStepYInv;
  fZMinOverStep = zMin * fStepZInv;

  fNy = static_cast<unsigned short>( yMax * fStepYInv - fYMinOverStep + 1.f );
  fNz = static_cast<unsigned short>( zMax * fStepZInv - fZMinOverStep + 1.f );

  unsigned int Nold = fN;
  fN = fNy * fNz;
  
  if (Nold < fN || Nold == 0) {
    delete[] fFirstHitInBin; 
    fFirstHitInBin = new THitI[fN+1]; // one extra bin is needed by area to determine number of hits in the last bin
  }
}

inline void L1Grid::Fill( const L1HitPoint* points, THitI n ) // call after sort
{
  unsigned int lastBin = 0;
  fFirstHitInBin[lastBin] = 0;
  for( THitI i = 0; i < n; i++ ) {
    const unsigned int currBin = GetBinBounded(points[i].Xs(), points[i].Ys());
    L1_assert( lastBin <= currBin );
    memset( fFirstHitInBin + lastBin + 1, i, currBin - lastBin );
    lastBin = currBin;
  }
  memset( fFirstHitInBin + lastBin + 1, n, fN - lastBin );
  // for( THitI i = 0; i < fN+1; i++ ) cout << fFirstHitInBin[i] << " ";
  // cout << endl;
}

inline int L1Grid::GetBin( float Y, float Z ) const
{
  //* get the bin pointer
  const int yBin = static_cast<int>( Y * fStepYInv - fYMinOverStep );
  const int zBin = static_cast<int>( Z * fStepZInv - fZMinOverStep );
  assert( yBin >= 0 );
  assert( zBin >= 0 );
  assert( yBin < static_cast<int>( fNy ) );
  assert( zBin < static_cast<int>( fNz ) );
  const int bin = zBin * fNy + yBin;
  return bin;
}

inline void L1Grid::GetBinBounds( unsigned int iBin, float &Ymin, float &Ymax, float &Zmin, float &Zmax) const
{
  int zBin = iBin / fNy;
  int yBin = iBin % fNy;
  Ymin = (fYMinOverStep + yBin)/fStepYInv;
  Zmin = (fZMinOverStep + zBin)/fStepZInv;
  Ymax = Ymin + 1. / fStepYInv;
  Zmax = Zmin + 1. / fStepZInv;
}

#endif
