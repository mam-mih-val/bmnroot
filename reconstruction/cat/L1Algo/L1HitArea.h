#ifndef L1HitArea_H
#define L1HitArea_H

#include "L1Grid.h"

#include "CbmL1Def.h"

struct L1Grid;
class L1Row;
class L1SliceData;

class L1HitArea
{
  public:

    L1HitArea( const L1Grid & grid, float y, float z, float dy, float dz );

    /**
     * look up the next hit in the requested area.
     * Sets h to the coordinates and returns the index for the hit data
     */
    bool GetNext( THitI& i );

  protected:
    const L1Grid &fGrid;

    unsigned short fBZmax;   // maximal Z bin index
    unsigned short fBDY;     // Y distance of bin indexes
    unsigned int fIndYmin; // minimum index for
    unsigned short fIz;      // current Z bin index (incremented while iterating)
    THitI fHitYlst; // last possible hit index in current z-line
    THitI fIh;      // hit index iterating inside the bins
    int fNy;      // Number of bins in Y direction
};

inline L1HitArea::L1HitArea( const L1Grid & grid, float y, float z, float dy, float dz )
  : fGrid( grid ),
    fBZmax(0),
    fBDY(0),
    fIndYmin(0),
    fIz(0),
  fHitYlst( 0 ),
  fIh( 0 ),
  fNy( fGrid.Ny() )
{
  const float minY = y - dy;
  const float maxY = y + dy;
  const float minZ = z - dz;
  const float maxZ = z + dz;
  unsigned short bYmin, bZmin, bYmax; // boundary bin indexes
  fGrid.GetBinBounded( minY, minZ, &bYmin, &bZmin );
  fGrid.GetBinBounded( maxY, maxZ, &bYmax, &fBZmax );

  fBDY = ( bYmax - bYmin + 1 ); // bin index span in y direction

  fIndYmin = ( bZmin * fNy + bYmin ); // same as grid.GetBin(fMinY, fMinZ), i.e. the smallest bin index of interest
  // fIndYmin + fBDY - 1 then is the largest bin index of interest with the same Z

  fGrid.GetBinBounds( fIndYmin, y, dy, z, dz );

  fIz = bZmin;

  fIh = fGrid.FirstHitInBin( fIndYmin );
  fHitYlst = fGrid.FirstHitInBin( fIndYmin + fBDY );

  // const unsigned int hitLst = fGrid.FirstHitInBin( fBZmax * fNy + bZmin + 1 );
  // cout << fIh << " " << hitLst << endl;
  // if ( fIh >= hitLst ) fIz = fBZmax + 1; // finish area in one step

  // cout // << fBZmax << " " << fBDY << " " << fIndYmin << " " << fIz << " " << fHitYlst << " " << fIh << endl; // dbg 
  //  << "HitArea created:\n"
  //  << y << " " << z << endl
  //   << "bYmin:    " << bYmin << "\n"
  //   << "bZmin:    " << bZmin << "\n"
  //   << "bYmax:    " << bYmax << "\n"
  //   << "fBZmax:   " << fBZmax << "\n"
  //   << "fBDY:     " << fBDY << "\n"
  //   << "fIndYmin: " << fIndYmin << "\n"
  //   << "fIz:      " << fIz << "\n"
  //   << "fHitYlst: " << fHitYlst << "\n"
  //   << "fIh:      " << fIh << "\n"
  //   << "fNy:      " << fNy << "\n"
  //   << "Nz " << fGrid.Nz() << std::endl;
  L1_ASSERT ( fIndYmin + fBDY < fGrid.N()+1, fIndYmin << " + " << fBDY << " < " << fGrid.N());
}

inline bool L1HitArea::GetNext( THitI& i )
{
  bool yIndexOutOfRange = fIh >= fHitYlst;     // current y is not in the area
  bool nextZIndexOutOfRange = (fIz >= fBZmax);   // there isn't any new z-line

  if ( yIndexOutOfRange && nextZIndexOutOfRange ) { // all iterators are over the end
    return false;
  }
  
  // at least one entry in the vector has (fIh >= fHitYlst && fIz < fBZmax)
  bool needNextZ = yIndexOutOfRange && !nextZIndexOutOfRange;
    
  // skip as long as fIh is outside of the interesting bin y-index
  while ( ISLIKELY( needNextZ ) ) {
    fIz++;   // get new z-line
      // get next hit
    fIndYmin += fNy;
    fIh = fGrid.FirstHitInBin( fIndYmin ); // get first hit in cell, if z-line is new
    fHitYlst = fGrid.FirstHitInBin( fIndYmin + fBDY );
      
    yIndexOutOfRange = fIh >= fHitYlst;
    nextZIndexOutOfRange = (fIz >= fBZmax);
    needNextZ = yIndexOutOfRange && !nextZIndexOutOfRange;
  }

  L1_ASSERT ( fIh < fGrid.FirstHitInBin(fGrid.N()) || yIndexOutOfRange, fIh << " < " << fGrid.FirstHitInBin(fGrid.N()-1) << " || " <<  yIndexOutOfRange);
  
  i = fIh; // return
  fIh++; // go to next
  return !yIndexOutOfRange;
}

#endif
