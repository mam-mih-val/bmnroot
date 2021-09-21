
#include "L1HitsSortHelper.h"

#include <vector>
#include <algorithm>
#include "L1StsHit.h"
#include "L1HitPoint.h"
#include "L1Grid.h"

using std::vector;

L1HitsSortHelper::L1HitsSortHelper( vector<L1StsHit> &hits, vector<L1HitPoint> &points, vector<THitI> &indices, const L1Grid* grid, THitI* iStart, THitI* iStop, int nStations ): fD(), fHits(hits), fPoints(points), fIndices(indices), fGrid(grid), fStsHitsUnusedStartIndex( iStart ), fStsHitsUnusedStopIndex( iStop ), fNStations(nStations) {
  L1_ASSERT( hits.size() ==  points.size(), hits.size() << " " << points.size() );
  const int NHits = hits.size();
  fD.resize( NHits );
  for ( int iS = 0; iS < fNStations; ++iS )
    for( THitI i = fStsHitsUnusedStartIndex[iS]; i < fStsHitsUnusedStopIndex[iS]; i++ )
  {
    fD[i].h = &(hits[i]);
    fD[i].p = &(points[i]);
    fD[i].i = indices[i];
    fD[i].bin = fGrid[iS].GetBinBounded( fD[i].p->Xs(), fD[i].p->Ys() );
  }
}

void L1HitsSortHelper::Sort(){
  for ( int iS = 0; iS < fNStations; ++iS ) {
    std::sort(fD.begin()+fStsHitsUnusedStartIndex[iS], fD.begin()+fStsHitsUnusedStopIndex[iS], L1HitsSortHelperData::compare);
  }
    
  vector<L1StsHit> hits = fHits;
  vector<L1HitPoint> points = fPoints;
  const int NHits = fD.size();
  for ( int i = 0; i < NHits; i++ ) {
    hits[i] = *fD[i].h;
    points[i] = *fD[i].p;
    fIndices[i] = fD[i].i;
  }
  fHits = hits;
  fPoints = points;
}

