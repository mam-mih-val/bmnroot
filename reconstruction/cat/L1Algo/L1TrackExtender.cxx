#include "L1Algo.h"
#include "L1TrackPar.h"
#include "L1Branch.h"
#include "L1Track.h"
#include "L1Extrapolation.h"
#include "L1Filtration.h"
#include "L1AddMaterial.h"
#include "L1HitPoint.h"
#include "L1HitArea.h"

#include <iostream>

// using namespace std;
using std::cout;
using std::endl;
using std::vector;



   /// Fit track
   /// t - track with hits
   /// T - track params
   /// dir - 0 - forward, 1 - backward
   /// qp0 - momentum for extrapolation
   /// initialize - should be params ititialized. 1 - yes.
void L1Algo::BranchFitterFast(const L1Branch &t, L1TrackPar& T, const bool dir, const fvec qp0, const bool initParams)
{
  L1_assert(t.StsHits.size() >= 3);
  
    // get hits of current track
  const std::vector<THitI>& hits = t.StsHits; // array of indeses of hits of current track
  const int nHits = t.StsHits.size();

  const signed short int step = -2*static_cast<int>(dir) + 1; // increment for station index
  const int iFirstHit = (dir) ? nHits-1 : 0;
  const int iLastHit  = (dir) ? 0 : nHits-1;
  
  L1StsHit &hit0 = vStsHits[hits[iFirstHit]];
  L1StsHit &hit1 = vStsHits[hits[iFirstHit + step]];
  L1StsHit &hit2 = vStsHits[hits[iFirstHit + 2*step]];

  int ista0 = GetFStation(vSFlag[hit0.f]);
  int ista1 = GetFStation(vSFlag[hit1.f]);
  int ista2 = GetFStation(vSFlag[hit2.f]);

  L1Station &sta0 = vStations[ista0];
  L1Station &sta1 = vStations[ista1];
  L1Station &sta2 = vStations[ista2];

  fvec u0  = static_cast<fscal>( vStsStrips[hit0.f] );
  fvec v0  = static_cast<fscal>( vStsStripsB[hit0.b] );
  fvec x0,y0;
  StripsToCoor(u0, v0, x0, y0, sta0);
  fvec z0 = vStsZPos[hit0.iz];

  fvec u1  = static_cast<fscal>( vStsStrips[hit1.f] );
  fvec v1  = static_cast<fscal>( vStsStripsB[hit1.b] );
  fvec x1,y1;
  StripsToCoor(u1, v1, x1, y1, sta1);
  fvec z1 = vStsZPos[hit1.iz];

  fvec u2  = static_cast<fscal>( vStsStrips[hit2.f] );
  fvec v2  = static_cast<fscal>( vStsStripsB[hit2.b] );
  fvec x2,y2;
  StripsToCoor(u2, v2, x2, y2, sta2);
  //fvec z2 = vStsZPos[hit2.iz];

  fvec dzi = 1./(z1-z0);

  const fvec vINF = .1;
  T.x  = x0;
  T.y  = y0;
  if( initParams ){
    T.tx = (x1-x0)*dzi;
    T.ty = (y1-y0)*dzi;
    T.qp = qp0;
  }

  T.z  = z0;
  T.chi2 = 0.;
  T.NDF = 2.;
  T.C00 = sta0.XYInfo.C00;
  T.C10 = sta0.XYInfo.C10;
  T.C11 = sta0.XYInfo.C11;

  T.C20 = T.C21 = 0;
  T.C30 = T.C31 = T.C32 = 0;
  T.C40 = T.C41 = T.C42 = T.C43 = 0;
  T.C22 = T.C33 = vINF;
  T.C44 = 1.;

  L1FieldValue fB0, fB1, fB2 _fvecalignment;
  L1FieldRegion fld _fvecalignment;
  fvec fz0 = sta1.z; // suppose field is smoth
  fvec fz1 = sta2.z;
  fvec fz2 = sta0.z;


  sta1.fieldSlice.GetFieldValue( x1, y1, fB0 );
  sta2.fieldSlice.GetFieldValue( x2, y2, fB1 );
  sta0.fieldSlice.GetFieldValue( x0, y0, fB2 );

  fld.Set( fB2, fz2, fB1, fz1, fB0, fz0 );

  int ista_prev = ista1;
  int ista = ista2;

  for( int i = iFirstHit + step; step*i <= step*iLastHit; i += step){
    L1StsHit &hit = vStsHits[hits[i]];
    ista_prev = ista;
    ista = GetFStation(vSFlag[hit.f]);

    L1Station &sta = vStations[ista];
          
    L1Extrapolate( T, vStsZPos[hit.iz], qp0, fld );
    L1AddMaterial( T, sta.materialInfo, qp0 );
    if ( (step*ista <= step*(NMvdStations + (step+1)/2 - 1)) && (step*ista_prev >= step*(NMvdStations + (step+1)/2 - 1 - step)) )
      L1AddPipeMaterial( T, qp0 );
  
    fvec u = static_cast<fscal>( vStsStrips[hit.f] );
    fvec v = static_cast<fscal>( vStsStripsB[hit.b] );
    fvec du  = static_cast<fscal>( vStsStrips[hit.f].errf() ); //AZ
    fvec dv  = static_cast<fscal>( vStsStripsB[hit.b].errf() ); //AZ
    //L1Filter( T, sta.frontInfo, u );
    //L1Filter( T, sta.backInfo,  v );
    fvec w = 1.0; //AZ
    du *= du; //AZ
    dv *= dv; //AZ
    L1Filter( T, sta.frontInfo, u, w, &du); //AZ
    L1Filter( T, sta.backInfo,  v, w, &dv); //AZ
    fB0 = fB1;
    fB1 = fB2;
    fz0 = fz1;
    fz1 = fz2;
    fvec x,y;
    StripsToCoor(u, v, x, y, sta);
    sta.fieldSlice.GetFieldValue( x, y, fB2 );

    fz2 = sta.z;
    fld.Set( fB2, fz2, fB1, fz1, fB0, fz0 );
  } // i

} // void L1Algo::BranchFitterFast

  /// like BranchFitterFast but more precise
void L1Algo::BranchFitter(const L1Branch &t, L1TrackPar& T, const bool dir, const fvec qp0, const bool initParams)
{
  BranchFitterFast (t, T, dir, qp0, initParams);
  for (int i = 0; i < 1; i++) {
    BranchFitterFast (t, T, !dir, T.qp, false);
    BranchFitterFast (t, T, dir, T.qp, false);  
  }
} // void L1Algo::BranchFitter

   /// Find additional hits for existing track
   /// t - track with hits
   /// T - track params
   /// dir - 0 - forward, 1 - backward
   /// qp0 - momentum for extrapolation
   /// initialize - should be params ititialized. 1 - yes.
void L1Algo::FindMoreHits(L1Branch &t, L1TrackPar& T, const bool dir, const fvec qp0) // TODO take into account pipe
{
  std::vector<THitI> newHits;
  newHits.clear();
  
  const signed short int step = - 2*static_cast<int>(dir) + 1; // increment for station index
  const int iFirstHit = (dir) ? 2 : t.StsHits.size()-3;
//  int ista = GetFStation(vSFlag[vStsHits[t.StsHits[iFirstHit]].f]) + 2*step; // current station. set to the end of track

  const L1StsHit &hit0 = vStsHits[t.StsHits[iFirstHit         ]]; // optimize
  const L1StsHit &hit1 = vStsHits[t.StsHits[iFirstHit + step  ]];
  const L1StsHit &hit2 = vStsHits[t.StsHits[iFirstHit + 2*step]];

  const int ista0 = GetFStation(vSFlag[hit0.f]);
  const int ista1 = GetFStation(vSFlag[hit1.f]);
  const int ista2 = GetFStation(vSFlag[hit2.f]);

  const L1Station &sta0 = vStations[ista0];
  const L1Station &sta1 = vStations[ista1];
  const L1Station &sta2 = vStations[ista2];

  fvec u0  = static_cast<fscal>( vStsStrips[hit0.f] );
  fvec v0  = static_cast<fscal>( vStsStripsB[hit0.b] );
  fvec x0,y0;

  StripsToCoor(u0, v0, x0, y0, sta0);
  //fvec z0 = vStsZPos[hit0.iz];

  fvec u1  = static_cast<fscal>( vStsStrips[hit1.f] );
  fvec v1  = static_cast<fscal>( vStsStripsB[hit1.b] );
  fvec x1,y1;
  StripsToCoor(u1, v1, x1, y1, sta1);
  //fvec z1 = vStsZPos[hit1.iz];

  fvec u2  = static_cast<fscal>( vStsStrips[hit2.f] );
  fvec v2  = static_cast<fscal>( vStsStripsB[hit2.b] );
  fvec x2,y2;
  StripsToCoor(u2, v2, x2, y2, sta2);
  //fvec z2 = vStsZPos[hit2.iz];

  //fvec dzi = 1./(z1-z0);

  L1FieldValue fB0, fB1, fB2 _fvecalignment;
  L1FieldRegion fld _fvecalignment;
  fvec fz0 = sta1.z;
  fvec fz1 = sta2.z;
  fvec fz2 = sta0.z;

  sta1.fieldSlice.GetFieldValue( x1, y1, fB0 );
  sta2.fieldSlice.GetFieldValue( x2, y2, fB1 );
  sta0.fieldSlice.GetFieldValue( x0, y0, fB2 );

  fld.Set( fB2, fz2, fB1, fz1, fB0, fz0 );

  int ista = ista2 + 2*step; // skip one station. if there would be hit it has to be found on previous stap
  if (ista2 == FIRSTCASTATION) 
    ista = ista2 + step;

  const fvec Pick_gather2 = Pick_gather*Pick_gather;
  for( ; (ista < NStations) && (ista >= 0); ista += step ){ // CHECKME why ista2?

    L1Station &sta = vStations[ista];
          
    L1Extrapolate( T, sta.z, qp0, fld );

    fscal r2_best = 1e8; // best distance to hit
    int iHit_best = -1;  // index of the best hit

    const fscal iz = 1/T.z[0];
    L1HitArea area( vGrid[ ista ], T.x[0]*iz, T.y[0]*iz, (sqrt(Pick_gather*(T.C00 + sta.XYInfo.C00))+MaxDZ*fabs(T.tx))[0]*iz, (sqrt(Pick_gather*(T.C11 + sta.XYInfo.C11))+MaxDZ*fabs(T.ty))[0]*iz );
    THitI ih = 0;
    while( area.GetNext( ih ) ) {
      ih += StsHitsUnusedStartIndex[ista];
      L1StsHit &hit = (*vStsHitsUnused)[ih];

      if( GetFUsed( vSFlag[hit.f] | vSFlagB[hit.b] ) ) continue; // if used

      fscal xh, yh, zh;
      GetHitCoor(hit, xh, yh, zh, sta); // faster
      // L1HitPoint &point = (*vStsHitPointsUnused)[ih];
      // fscal xh = point.x, yh = point.y, zh = point.z;
      
      fvec y, C11;
      L1ExtrapolateYC11Line( T, zh, y, C11 );
        
      fscal dym_est = ( Pick_gather*sqrt(fabs(C11+sta.XYInfo.C11)) )[0];
      fscal y_minus_new = y[0] - dym_est;
      if (yh < y_minus_new) continue;  // CHECKME take into account overlaping?

      fvec x, C00;
      L1ExtrapolateXC00Line( T, zh, x, C00 );
      
      fscal dx = xh - x[0];
      fscal dy = yh - y[0];
      fscal d2 = dx*dx + dy*dy;
      if( d2 > r2_best ) continue;
      
      fscal dxm_est2 = ( Pick_gather2*(fabs(C00+sta.XYInfo.C00)) )[0];
      if (dx*dx > dxm_est2) continue;
      
      r2_best = d2;
      iHit_best = ih;
    }
    if( iHit_best < 0 ) break;
    
    newHits.push_back(RealIHit[iHit_best]);

    L1StsHit &hit = (*vStsHitsUnused)[iHit_best];
    fvec u = static_cast<fvec>(vStsStrips[hit.f]);
    fvec v = static_cast<fvec>(vStsStripsB[hit.b]);
    fvec du = static_cast<fvec>(vStsStrips[hit.f].errf()); //AZ
    fvec dv = static_cast<fvec>(vStsStripsB[hit.b].errf()); //AZ
    fvec x, y, z;
    StripsToCoor(u, v, x, y, sta);
    z = vStsZPos[hit.iz];
      
    L1ExtrapolateLine( T, z );
    L1AddMaterial( T, sta.materialInfo, qp0 );
    //L1Filter( T, sta.frontInfo, u );
    //L1Filter( T, sta.backInfo,  v );
    fvec w = 1.0; //AZ
    du *= du; //AZ
    dv *= dv; //AZ
    L1Filter( T, sta.frontInfo, u, w, &du ); //AZ
    L1Filter( T, sta.backInfo,  v, w, &dv ); //AZ

    fB0 = fB1;
    fB1 = fB2;
    fz0 = fz1;
    fz1 = fz2;
    sta.fieldSlice.GetFieldValue( x, y, fB2 );
    fz2 = sta.z;
    fld.Set( fB2, fz2, fB1, fz1, fB0, fz0 );
  }

    // save hits
  if (dir) { // backward
    const unsigned int NOldHits = t.StsHits.size();
    const unsigned int NNewHits = newHits.size();
    t.StsHits.resize(NNewHits + NOldHits);
    for (int i = NOldHits-1; i >= 0 ; i--) { 
      t.StsHits[NNewHits+i] = t.StsHits[i];
    }
    for (unsigned int i = 0, ii = NNewHits-1; i < NNewHits; i++,ii--) { 
      t.StsHits[i] = newHits[ii];
    }
  }
  else { // forward
    const unsigned int NOldHits = t.StsHits.size();
    t.StsHits.resize(newHits.size()+NOldHits);
    for (unsigned int i = 0; i < newHits.size(); i++) { 
      t.StsHits[NOldHits+i] = (newHits[i]);
    }
  }

} // void L1Algo::FindMoreHits

  /// Try to extrapolate and find additional hits on other stations
fscal L1Algo::BranchExtender(L1Branch &t) // TODO Simdize
{
  //  const unsigned int minNHits = 3;
  
  L1TrackPar T;

    // forward
  bool dir = 0;

  BranchFitter (t, T, dir);
  // BranchFitterFast (t, T, dir, 0, true);
  
//  if (t.StsHits.size() < minNHits) return T.chi2[0];
  FindMoreHits(t, T, dir, T.qp);

    // backward
  dir = 1;
  BranchFitterFast (t, T, dir, T.qp, false); // 577


  FindMoreHits(t, T, dir, T.qp);

  return T.chi2[0];
}
