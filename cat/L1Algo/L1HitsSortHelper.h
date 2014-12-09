#ifndef _L1HitsL1HitsSortHelper_h_
#define _L1HitsL1HitsSortHelper_h_

#include <vector>
#include <algorithm>
#include "L1StsHit.h"
#include "L1HitPoint.h"
#include "L1Grid.h"

using std::vector;

struct L1HitsSortHelperData{
  L1StsHit* h;
  L1HitPoint* p;
  unsigned int bin;
  THitI i;
  
  static bool compare(const L1HitsSortHelperData &a, const L1HitsSortHelperData &b){
    return a.bin < b.bin || ( a.bin == b.bin && a.p->Ys() < b.p->Ys() ); 
  }
};

class L1HitsSortHelper{
 public:
  L1HitsSortHelper( vector<L1StsHit> &hits, vector<L1HitPoint> &points, vector<THitI> &indices, const L1Grid* grid, THitI* iStart, THitI* iStop, int nStations );

  void Sort();
  
 private:
  vector<L1HitsSortHelperData> fD;
  vector<L1StsHit> &fHits;
  vector<L1HitPoint> &fPoints;
  vector<THitI> &fIndices;
  const L1Grid* fGrid;
  THitI *fStsHitsUnusedStartIndex, *fStsHitsUnusedStopIndex;
  int fNStations;

  L1HitsSortHelper(const L1HitsSortHelper&);
  L1HitsSortHelper& operator=(const L1HitsSortHelper&);
};
    
#endif
