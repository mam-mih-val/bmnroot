/*
 *=====================================================
 *
 *  CBM Level 1 Reconstruction
 *
 *  Authors: I.Kisel,  S.Gorbunov, I.Kulakov, M.Zyzak
 * Documentation: V.Akishina
 *
 *  e-mail : i.kulakov@gsi.de
 *
 *=====================================================
 *
 *  Finds tracks using the Cellular Automaton algorithm
 *
 */

#include "TMath.h"
#include "TStopwatch.h"

#include "L1Algo.h"
#include "L1TrackPar.h"
#include "L1Branch.h"
#include "L1Track.h"
#include "L1Extrapolation.h"
#include "L1Filtration.h"
#include "L1AddMaterial.h"
#include "L1HitPoint.h"
#include "L1Grid.h"
#include "L1HitArea.h"

#include "L1Portion.h"

#include "L1HitsSortHelper.h"

#include "L1Timer.h"

#ifdef DRAW
#include "utils/L1AlgoDraw.h"
#endif
#ifdef PULLS
#include "utils/L1AlgoPulls.h"
#endif
#ifdef TRIP_PERFORMANCE
#include "utils/L1AlgoEfficiencyPerformance.h"
#endif
#ifdef DOUB_PERFORMANCE
#include "utils/L1AlgoEfficiencyPerformance.h"
#endif

#ifdef TBB
#include "L1AlgoTBB.h"
#endif

#include <iostream>
#include <map>
#include <list>
#include <stdio.h>
#include <algorithm>

// using namespace std;
using std::cout;
using std::endl;
using std::vector;

          /// Prepare the portion of data of left hits of a triplet: all hits except the last and the second last station will be procesesed in the algorythm, 
		  /// the data is orginesed in order to be used by SIMD
inline void L1Algo::f10(  // input
                int start_lh, int n1,  L1HitPoint *vStsHits_l,
                  // output
                fvec *u_front, fvec *u_back, fvec *zPos,
                THitI* hitsl_1,
		fvec *du_front, fvec *du_back //AZ
                )
{
  const int end_lh = start_lh+n1;
  for (int ilh = start_lh, i1 = 0; ilh < end_lh; ilh++, i1++){
    int i1_V= i1/fvecLen;
    int i1_4= i1%fvecLen;
    L1HitPoint &hitl = vStsHits_l[ilh];

    hitsl_1[i1] = ilh;
    u_front[i1_V][i1_4] = hitl.U();
    u_back [i1_V][i1_4] = hitl.V();
    zPos   [i1_V][i1_4] = hitl.Z();
    if (du_front) du_front[i1_V][i1_4] = hitl.DU(); //AZ
    if (du_back) du_back [i1_V][i1_4] = hitl.DV(); //AZ
  }
}


         /// Get the field approximation. Add the target to parameters estimation. Propagaete to the middle station of a triplet.
inline void L1Algo::f11(  /// input 1st stage of singlet search
                int istal, int istam, /// indexes of left and middle stations of a triplet
                int n1_V,

                fvec *u_front, fvec *u_back,  fvec *zPos,
                  // output
//                 L1TrackPar *T_1,
                L1TrackPar *T_1,
                L1FieldRegion *fld_1,
		fvec *du_front, fvec *du_back //AZ
		)
{
  L1Station &stal = vStations[istal];
  L1Station &stam = vStations[istam];
  fvec zstal = stal.z;
  fvec zstam = stam.z;

  L1FieldRegion fld0;      // by  left   hit, target and "center" (station in-between). Used here for extrapolation to target and to middle hit
    L1FieldValue centB, l_B _fvecalignment; // field for singlet creation
  L1FieldValue m_B _fvecalignment; // field for the next extrapolations

  for( int i1_V=0; i1_V<n1_V; i1_V++ ){
    L1TrackPar &T = T_1[i1_V];
    L1FieldRegion &fld1 = fld_1[i1_V]; // by  middle hit, left hit and "center" . Will be used for extrapolation to right hit

        // get the magnetic field along the track.
        // (suppose track is straight line with origin in the target)
    fvec &u = u_front[i1_V];
    fvec &v = u_back [i1_V];
    fvec xl, yl; // left(1-st) hit coor
    fvec zl = zPos   [i1_V];

    fvec dzli = 1./(zl-targZ);
    StripsToCoor(u, v, xl, yl, stal);

    fvec tx = (xl - targX)*dzli;
    fvec ty = (yl - targY)*dzli;

            // estimate field for singlet creation
    int istac = istal/2; // "center" station
//     if (istal >= NMvdStations) // to make it in the same way for both with and w\o mvd
//       istac = (istal-NMvdStations)/2+NMvdStations;
    L1Station &stac = vStations[istac];
    fvec zstac = stac.z;
    stac.fieldSlice.GetFieldValue( targX + tx*(zstac - targZ), targY + ty*(zstac - targZ), centB );
    stal.fieldSlice.GetFieldValue( targX + tx*(zstal - targZ), targY + ty*(zstal - targZ), l_B );
    if( istac != istal  ){
      fld0.Set( l_B, stal.z, centB, stac.z, targB, targZ );
    }
    else{
      fld0.Set( l_B, zstal, targB, targZ );
    }
          // estimate field for the next extrapolations
    stam.fieldSlice.GetFieldValue( targX + tx*(zstam - targZ), targY + ty*(zstam - targZ), m_B );
#define USE_3HITS
#ifndef USE_3HITS
    if( istac != istal ){
      fld1.Set( m_B, zstam, l_B, zstal, centB, zstac  );
    }
    else{
      fld1.Set( m_B, zstam, l_B, zstal, targB, targZ  );
    }
#else // if USE_3HITS  // the best now
    L1FieldValue r_B _fvecalignment;
    L1Station &star = vStations[istam+1];
    fvec zstar = star.z;
    star.fieldSlice.GetFieldValue( targX + tx*(zstar - targZ), targY + ty*(zstar - targZ), r_B );
    fld1.Set( r_B, zstar, m_B, zstam, l_B, zstal);
#endif // USE_3HITS

    T.chi2 = 0.;
    T.NDF = 2.;
    if ( (isec == kAllSecIter) || (isec == kAllSecEIter) || (isec == kAllSecJumpIter) ) T.NDF = 0;
    T.tx = tx;
    T.ty = ty;
    T.qp = 0.;
    T.C20 = T.C21 = 0;
    T.C30 = T.C31 = T.C32 = 0;
    T.C40 = T.C41 = T.C42 = T.C43 = 0;
    T.C22 = T.C33 = MaxSlope*MaxSlope/9.; T.C44 = MaxInvMom/3.*MaxInvMom/3.;
    
// #define BEGIN_FROM_TARGET
#ifndef BEGIN_FROM_TARGET // the best now

    T.x  = xl;
    T.y  = yl;
    T.z  = zl;
    T.C00 = stal.XYInfo.C00;
    T.C10 = stal.XYInfo.C10;
    T.C11 = stal.XYInfo.C11;
    //AZ - compute track covariance from hits
    if (du_front) {
      fvec &du = du_front[i1_V];
      fvec &dv = du_back [i1_V];
      //*
      fvec c_f = stal.frontInfo.cos_phi;
      fvec s_f = stal.frontInfo.sin_phi;
      fvec c_b = stal.backInfo.cos_phi;
      fvec s_b = stal.backInfo.sin_phi;
      fvec det = c_f*s_b - s_f*c_b;
      det *=det;
      //cout <<  " 1: " << T.C00 << " " << T.C10 << " " << T.C11 << endl;
      //T.C00 = ( s_b*s_b*stal.frontInfo.sigma2 + s_f*s_f*stal.backInfo.sigma2 )/det;
      //T.C10 =-( s_b*c_b*stal.frontInfo.sigma2 + s_f*c_f*stal.backInfo.sigma2 )/det;
      //T.C11 = ( c_b*c_b*stal.frontInfo.sigma2 + c_f*c_f*stal.backInfo.sigma2 )/det;
      //cout << " 2: " << T.C00 << " " << T.C10 << " " << T.C11 << endl;
      T.C00 = ( s_b*s_b*du*du + s_f*s_f*dv*dv )/det;
      T.C10 =-( s_b*c_b*du*du + s_f*c_f*dv*dv )/det;
      T.C11 = ( c_b*c_b*du*du + c_f*c_f*dv*dv )/det;
      //*/
    }
    //AZ
    
    //add the target
    {
      fvec eX, eY, J04, J14;
      fvec dz;
      dz = targZ - zl;
      L1ExtrapolateJXY0(T.tx, T.ty, dz, fld0, eX, eY, J04, J14 );
      fvec J[6];
      J[0]= dz; J[1] = 0; J[2]= J04;
      J[3] = 0; J[4]= dz; J[5]= J14;
      L1FilterVtx( T, targX, targY, TargetXYInfo, eX, eY, J );
    }
#else  // TODO: doesn't work. Why?

    T.x  = targX;
    T.y  = targY;

    T.z  = targZ;
    T.C00 = TargetXYInfo.C00;
    T.C10 = TargetXYInfo.C10;
    T.C11 = TargetXYInfo.C11;

    
      // extrapolate to left hit
    L1Extrapolate0( T, zl, fld0 );
    for (int ista = 0; ista <= istal-1; ista++){
      if ( ( isec != kAllPrimEIter ) && ( isec != kAllSecEIter ) ) {
#ifdef USE_RL_TABLE
        L1AddMaterial( T, fRadThick[ista].GetRadThick(T.x, T.y), MaxInvMom );
#else
        L1AddMaterial( T, vStations[ista].materialInfo, MaxInvMom );
#endif
        if (ista == NMvdStations - 1) L1AddPipeMaterial( T, MaxInvMom );
      }
      else {
#ifdef USE_RL_TABLE
        L1AddMaterial( T, fRadThick[ista].GetRadThick(T.x, T.y), MaxInvMom, 1, 0.000511f*0.000511f );
#else
        L1AddMaterial( T, vStations[ista].materialInfo, MaxInvMom, 1, 0.000511f*0.000511f );
#endif
        if (ista == NMvdStations - 1) L1AddPipeMaterial( T, MaxInvMom, 1, 0.000511f*0.000511f );
      }
    }
      // add left hit
    L1Filter( T, stal.frontInfo, u );
    L1Filter( T, stal.backInfo,  v );
#endif

    if ( ( isec != kAllPrimEIter ) && ( isec != kAllSecEIter ) ) {
#ifdef USE_RL_TABLE
    L1AddMaterial( T, fRadThick[istal].GetRadThick(T.x, T.y), MaxInvMom );
#else
    L1AddMaterial( T, stal.materialInfo, MaxInvMom );
#endif
    if ( (istam >= NMvdStations) && (istal <= NMvdStations - 1) )  L1AddPipeMaterial( T, MaxInvMom );
    }
    else {
#ifdef USE_RL_TABLE
      L1AddMaterial( T, fRadThick[istal].GetRadThick(T.x, T.y), MaxInvMom, 1, 0.000511f*0.000511f );
#else
      L1AddMaterial( T, stal.materialInfo, MaxInvMom, 1, 0.000511f*0.000511f );
#endif
      if ( (istam >= NMvdStations) && (istal <= NMvdStations - 1) )  L1AddPipeMaterial( T, MaxInvMom, 1, 0.000511f*0.000511f );
    } 
    L1Extrapolate0( T, zstam, fld0 ); // TODO: fld1 doesn't work!
//     L1Extrapolate( T, zstam, T.qp, fld1 );
    
  }// i1_V
}


          /// Find the doublets. Reformat data in the portion of doublets.
inline void L1Algo::f20(  // input
                int n1, L1Station &stam,
                L1HitPoint *vStsHits_l, L1HitPoint *vStsHits_m, int NHits_m,
                L1TrackPar *T_1, 
                THitI* hitsl_1,
                  // output
                int &n2,
                vector<THitI> &i1_2,
                int &start_mhit,
#ifdef DOUB_PERFORMANCE
                vector<THitI> &hitsl_2,
#endif // DOUB_PERFORMANCE
                vector<THitI> &hitsm_2,
                vector<bool> &lmDuplets
                )
{
  n2 = 0; // number of doublet
  for (int i1 = 0; i1 < n1; i1++){ // for each singlet
    const int i1_V = i1/fvecLen;
    const int i1_4 = i1%fvecLen;
    const L1TrackPar& T1 = T_1[i1_V];

    const int n2Saved = n2;

    const THitI nl = vStsHits_l[hitsl_1[i1]].N();
    fvec Pick_m22 = (DOUBLET_CHI2_CUT - T1.chi2); // if make it bigger the found hits will be rejected later because of the chi2 cut.

      // -- collect possible doublets --

    const fscal iz = 1/T1.z[i1_4];
    L1HitArea area( vGrid[ &stam - vStations ], T1.x[i1_4]*iz, T1.y[i1_4]*iz, (sqrt(Pick_m22*(T1.C00 + stam.XYInfo.C00))+MaxDZ*fabs(T1.tx))[i1_4]*iz, (sqrt(Pick_m22*(T1.C11 + stam.XYInfo.C11))+MaxDZ*fabs(T1.ty))[i1_4]*iz );
    THitI imh = 0;
    while( area.GetNext( imh ) ) {
      const L1HitPoint &hitm = vStsHits_m[imh];
        // check y-boundaries

        // - check whether hit belong to the window ( track position +\- errors ) -
      const fscal &zm = hitm.Z();

        // check lower boundary
      fvec y, C11;
      L1ExtrapolateYC11Line( T1, zm, y, C11 );
      const fscal dy_est2 = ( Pick_m22 * fabs(C11 + stam.XYInfo.C11) )[i1_4];

      const fscal dy = hitm.Y() - y[i1_4];
      const fscal dy2 = dy*dy;
      if ( dy2 > dy_est2 && dy < 0 ) continue;

        // check upper boundary
      const unsigned short int nm = hitm.N();
      if ( ( nl != nm ) ) continue;

        // check x-boundaries
      fvec x, C00;
      L1ExtrapolateXC00Line( T1, zm, x, C00 );
      const fscal dx_est2 = (Pick_m22 * fabs(C00 + stam.XYInfo.C00))[i1_4];
      const fscal dx = hitm.X() - x[i1_4];
      if ( dx*dx > dx_est2 ) continue;

        // check chi2
      fvec C10;
      L1ExtrapolateC10Line( T1, zm, C10 );
      fvec chi2 = T1.chi2;
      fvec du2 = hitm.DU(); //AZ
      du2 *= du2; //AZ
      //L1FilterChi2XYC00C10C11( stam.frontInfo, x, y, C00, C10, C11, chi2, hitm.U() );
      L1FilterChi2XYC00C10C11( stam.frontInfo, x, y, C00, C10, C11, chi2, hitm.U(), &du2); //AZ
#ifdef DO_NOT_SELECT_TRIPLETS
      if (isec!=TRACKS_FROM_TRIPLETS_ITERATION)
#endif
      if ( chi2[i1_4] > DOUBLET_CHI2_CUT ) continue;
      fvec dv2 = hitm.DV(); //AZ
      dv2 *= dv2; //AZ
      //L1FilterChi2           ( stam.backInfo,  x, y, C00, C10, C11, chi2, hitm.V() );
      L1FilterChi2           ( stam.backInfo,  x, y, C00, C10, C11, chi2, hitm.V(), &dv2); //AZ
#ifdef DO_NOT_SELECT_TRIPLETS
      if (isec!=TRACKS_FROM_TRIPLETS_ITERATION)
#endif
      if ( chi2[i1_4] > DOUBLET_CHI2_CUT ) continue;

      i1_2.push_back(i1);
#ifdef DOUB_PERFORMANCE
      hitsl_2.push_back(hitsl_1[i1]);
#endif // DOUB_PERFORMANCE
      hitsm_2.push_back(imh);

      n2++;
    }
    
    lmDuplets[hitsl_1[i1]] = (n2Saved < n2);
  }  // for i1
}


          /// Add the middle hits to parameters estimation. Propagate to right station.
          /// Find the triplets(right hit). Reformat data in the portion of triplets.
inline void L1Algo::f30(  // input
                L1HitPoint *vStsHits_r, L1Station &stam, L1Station &star,

                int istam, int istar,
                L1HitPoint *vStsHits_m,
                L1TrackPar *T_1, L1FieldRegion *fld_1,
                THitI *hitsl_1,

                int n2,
                vector<THitI> &hitsm_2,
                vector<THitI> &i1_2,

                const vector<bool> &mrDuplets,
                  // output
                int &n3,
                nsL1::vector<L1TrackPar>::TSimd &T_3,
                vector<THitI> &hitsl_3,  vector<THitI> &hitsm_3,  vector<THitI> &hitsr_3,
                nsL1::vector<fvec>::TSimd &u_front_3, nsL1::vector<fvec>::TSimd &u_back_3, nsL1::vector<fvec>::TSimd &z_Pos_3,
		nsL1::vector<fvec>::TSimd *du_front_3, nsL1::vector<fvec>::TSimd *du_back_3 //AZ
                )
{
  THitI hitsl_2[fvecLen];
  THitI hitsm_2_tmp[fvecLen];
  fvec fvec_0;
  //fvec fvec_1 = 1.; //AZ
  L1TrackPar L1TrackPar_0;

  int n3_V = 0, n3_4 = 0;
    
  T_3.push_back(L1TrackPar_0);
  u_front_3.push_back(fvec_0);
  u_back_3.push_back(fvec_0);
  z_Pos_3.push_back(fvec_0);
  //AZ
  if (du_front_3) {
    du_front_3->push_back(fvec_0);
    du_back_3->push_back(fvec_0);
    //du_front_3->push_back(fvec_1);
    //du_back_3->push_back(fvec_1);
  }
  //AZ

    // ---- Add the middle hits to parameters estimation. Propagate to right station. ----
  if (istar < NStations){

    for (int i2 = 0; i2 < n2;) {

      L1TrackPar T2;
      L1FieldRegion f2;
      // pack the data
      fvec u_front_2;
      fvec u_back_2;
      fvec zPos_2;
      fvec du_front_2, du_back_2; //AZ
      //fvec du_front_2 = 1.0, du_back_2 = 1.0; //AZ
      int n2_4 = 0;
      for (; n2_4 < fvecLen && i2 < n2; n2_4++, i2++){

        if (!mrDuplets[hitsm_2[i2]]) {
          n2_4--;
          continue;
        }

        const int i1 = i1_2[i2];
        const int i1_V = i1/fvecLen;
        const int i1_4 = i1%fvecLen;

        const L1TrackPar &T1 = T_1[i1_V];
        const L1FieldRegion &f1 = fld_1[i1_V];
        T2.SetOneEntry(n2_4, T1, i1_4);
        f2.SetOneEntry(n2_4, f1, i1_4);
         
        const int imh = hitsm_2[i2];
        const L1HitPoint &hitm = vStsHits_m[imh];
        u_front_2[n2_4] = hitm.U();
        u_back_2 [n2_4] = hitm.V();
        zPos_2   [n2_4] = hitm.Z();
        du_front_2[n2_4] = hitm.DU() * hitm.DU(); //AZ
        du_back_2 [n2_4] = hitm.DV() * hitm.DV(); //AZ

        hitsl_2[n2_4] = hitsl_1[i1];
        hitsm_2_tmp[n2_4] = hitsm_2[i2];
      }  // n2_4

        // add middle hit
      L1ExtrapolateLine( T2, zPos_2 );
      //L1Filter( T2, stam.frontInfo, u_front_2 );
      //L1Filter( T2, stam.backInfo,  u_back_2 );
      fvec ww = 1.; //AZ
      L1Filter( T2, stam.frontInfo, u_front_2, ww, &du_front_2); //AZ
      L1Filter( T2, stam.backInfo,  u_back_2, ww, &du_back_2); //AZ

      if ( ( isec != kAllPrimEIter ) && ( isec != kAllSecEIter ) ) {
#ifdef USE_RL_TABLE
        L1AddMaterial( T2, fRadThick[istam].GetRadThick(T2.x, T2.y), T2.qp );
#else
        L1AddMaterial( T2, stam.materialInfo, T2.qp );
#endif
        if ( (istar >= NMvdStations) && (istam <= NMvdStations - 1) ) L1AddPipeMaterial( T2, T2.qp );
      }
      else {
#ifdef USE_RL_TABLE
        L1AddMaterial( T2, fRadThick[istam].GetRadThick(T2.x, T2.y), T2.qp, 1, 0.000511f*0.000511f );
#else
        L1AddMaterial( T2, stam.materialInfo, T2.qp, 1, 0.000511f*0.000511f );
#endif
        if ( (istar >= NMvdStations) && (istam <= NMvdStations - 1) ) L1AddPipeMaterial( T2, T2.qp, 1, 0.000511f*0.000511f );
      }
        // extrapolate to the right hit station
      L1Extrapolate( T2, star.z, T2.qp, f2 );

        // ---- Find the triplets(right hit). Reformat data in the portion of triplets. ----

      for (int i2_4 = 0; i2_4 < n2_4; i2_4++){

          //     THitI nm = vStsHits_m[hitsm_2[i2]].n;
        if ( T2.C00[i2_4] < 0 ||  T2.C11[i2_4] < 0 ||  T2.C22[i2_4] < 0 ||  T2.C33[i2_4] < 0 ||  T2.C44[i2_4] < 0 ) continue;

        fvec Pick_r22 = (TRIPLET_CHI2_CUT - T2.chi2);// if make it bigger the found hits will be rejected later because of the chi2 cut.
          // find first possible hit
        
#ifdef DO_NOT_SELECT_TRIPLETS
        if ( isec == TRACKS_FROM_TRIPLETS_ITERATION )
          Pick_r22 = Pick_r2+1;
#endif

        const fscal iz = 1/T2.z[i2_4];
        L1HitArea area( vGrid[ &star - vStations ], T2.x[i2_4]*iz, T2.y[i2_4]*iz, (sqrt(Pick_r22*(T2.C00 + stam.XYInfo.C00))+MaxDZ*fabs(T2.tx))[i2_4]*iz, (sqrt(Pick_r22*(T2.C11 + stam.XYInfo.C11))+MaxDZ*fabs(T2.ty))[i2_4]*iz );
        THitI irh = 0;
         while( area.GetNext( irh ) ) {
                
          const L1HitPoint &hitr = vStsHits_r[irh];

          const fscal zr = hitr.Z();
          const fscal yr = hitr.Y();
        
            // - check whether hit belong to the window ( track position +\- errors ) -
            // check lower boundary
          fvec y, C11;
          L1ExtrapolateYC11Line( T2, zr, y, C11 );
          const fscal dy_est2 = (Pick_r22*(fabs(C11 + star.XYInfo.C11)))[i2_4]; // TODO for FastPrim dx < dy - other sort is optimal. But not for doublets
          const fscal dy = yr - y[i2_4];
          const fscal dy2 = dy*dy;
          if ( dy2 > dy_est2 && dy < 0 ) continue; // if (yr < y_minus_new[i2_4]) continue;
        
            // check upper boundary
          if ( dy2 > dy_est2 ) continue; // if (yr > y_plus_new [i2_4] ) continue;
        
            // check x-boundaries
          fvec x, C00;
          L1ExtrapolateXC00Line( T2, zr, x, C00 );
          const fscal dx_est2 = (Pick_r22*(fabs(C00 + star.XYInfo.C00)))[i2_4];
          const fscal dx = hitr.X() - x[i2_4];
          if ( dx*dx > dx_est2 ) continue;

            // check chi2  // not effective
          fvec C10;
          L1ExtrapolateC10Line( T2, zr, C10 );
          fvec chi2 = T2.chi2;
	  fvec du2 = hitr.DU(), dv2 = hitr.DV(); //AZ
	  du2 *= du2; //AZ
	  dv2 *= dv2; //AZ
          //L1FilterChi2XYC00C10C11( star.frontInfo, x, y, C00, C10, C11, chi2, hitr.U() );
          //L1FilterChi2           ( star.backInfo,  x, y, C00, C10, C11, chi2, hitr.V() );
          L1FilterChi2XYC00C10C11( star.frontInfo, x, y, C00, C10, C11, chi2, hitr.U(), &du2); //AZ
          L1FilterChi2           ( star.backInfo,  x, y, C00, C10, C11, chi2, hitr.V(), &dv2); //AZ
#ifdef DO_NOT_SELECT_TRIPLETS
          if (isec!=TRACKS_FROM_TRIPLETS_ITERATION)
#endif
          if ( chi2[i2_4] > TRIPLET_CHI2_CUT || C00[i2_4] < 0 || C11[i2_4] < 0 ) continue; // chi2_triplet < CHI2_CUT
          
            // pack triplet
          L1TrackPar &T3 = T_3[n3_V];

          hitsl_3.push_back(hitsl_2[i2_4]);
          hitsm_3.push_back(hitsm_2_tmp[i2_4]);
          hitsr_3.push_back(irh);

          T3.SetOneEntry(n3_4, T2, i2_4);
          u_front_3[n3_V][n3_4] = hitr.U();
          u_back_3 [n3_V][n3_4] = hitr.V();
          z_Pos_3  [n3_V][n3_4] = zr;
	  //AZ
	  if (du_front_3) {
	    (*du_front_3)[n3_V][n3_4] = hitr.DU() * hitr.DU();
	    (*du_back_3) [n3_V][n3_4] = hitr.DV() * hitr.DV();
	  }
	  //AZ

          n3++;               
          n3_V = n3/fvecLen;  
          n3_4 = n3%fvecLen;

          if (!n3_4){
            T_3.push_back(L1TrackPar_0);
            u_front_3.push_back(fvec_0);
            u_back_3.push_back(fvec_0);
            z_Pos_3.push_back(fvec_0);
	    //AZ
	    if (du_front_3) {
	      du_front_3->push_back(fvec_0);
	      du_back_3->push_back(fvec_0);
	      //du_front_3->push_back(fvec_1);
	      //du_back_3->push_back(fvec_1);
	    }
	    //AZ
          }
        }
      } // i2_4
    }   // i2_V
  }  // if istar
}


  /// Add the right hits to parameters estimation.
inline void L1Algo::f31(  // input
                int n3_V,
                L1Station &star,
                nsL1::vector<fvec>::TSimd &u_front, nsL1::vector<fvec>::TSimd &u_back, nsL1::vector<fvec>::TSimd &z_Pos, 
                  // output
                //                L1TrackPar *T_3
                nsL1::vector<L1TrackPar>::TSimd &T_3,
		nsL1::vector<fvec>::TSimd *du_front2, nsL1::vector<fvec>::TSimd *du_back2 //AZ
               )
{
  fvec ww = 1.; //AZ
  for( int i3_V=0; i3_V<n3_V; i3_V++){
    L1ExtrapolateLine( T_3[i3_V], z_Pos[i3_V] );
    //AZ
    if (du_front2) {
      L1Filter( T_3[i3_V], star.frontInfo, u_front[i3_V], ww, &(*du_front2)[i3_V]); //AZ
      L1Filter( T_3[i3_V], star.backInfo,  u_back [i3_V], ww, &(*du_back2)[i3_V] ); //AZ
    } else { //AZ
      L1Filter( T_3[i3_V], star.frontInfo, u_front[i3_V] );    // 2.1/100 sec
      L1Filter( T_3[i3_V], star.backInfo,  u_back [i3_V] );   // 2.0/100 sec
    }
  };
  
}


          /// Refit Triplets.
inline void L1Algo::f32( // input // TODO not updated after gaps introduction
    int n3, int istal,
                nsL1::vector<L1TrackPar>::TSimd &T_3,
                vector<THitI> &hitsl_3,  vector<THitI> &hitsm_3,  vector<THitI> &hitsr_3,
                int nIterations
                       )
{
  const int NHits = 3; // triplets

    // prepare data
  int ista[NHits] = {
    istal,
    istal + 1,
    istal + 2
  };
  
  L1Station sta[3];
  for (int is = 0; is < NHits; is++){
    sta[is] = vStations[ista[is]];
  };

  for( int i3=0; i3<n3; i3++){
    int i3_V = i3/fvecLen;
    int i3_4 = i3%fvecLen;

    L1TrackPar &T3 = T_3[i3_V];
    fscal qp0 = T3.qp[i3_4];

      // prepare data
    THitI ihit[NHits] = {
      RealIHit[hitsl_3[i3] + StsHitsUnusedStartIndex[ista[0]]],
      RealIHit[hitsm_3[i3] + StsHitsUnusedStartIndex[ista[1]]],
      RealIHit[hitsr_3[i3] + StsHitsUnusedStartIndex[ista[2]]]
    };

    fscal u[NHits], v[NHits], x[NHits], y[NHits], z[NHits];
    fvec du2[NHits], dv2[NHits], ww = 1.; //AZ
    for (int ih = 0; ih < NHits; ih++){
      L1StsHit &hit = vStsHits[ihit[ih]];
      u[ih] = vStsStrips[hit.f];
      v[ih] = vStsStripsB[hit.b];
      du2[ih] = vStsStrips[hit.f].errf() * vStsStrips[hit.f].errf(); //AZ
      dv2[ih] = vStsStripsB[hit.b].errf() * vStsStripsB[hit.b].errf(); //AZ
      StripsToCoor(u[ih], v[ih], x[ih], y[ih], sta[ih]);
      z[ih] = vStsZPos[hit.iz];
    };

      // initialize parameters
    L1TrackPar T;
    
    const fvec vINF = .1;
    T.x  = x[0];
    T.y  = y[0];

    fvec dz01 = 1./(z[1]-z[0]);
    T.tx = (x[1]-x[0])*dz01;
    T.ty = (y[1]-y[0])*dz01;

    T.qp = qp0;
    T.z  = z[0];
    T.chi2 = 0.;
    T.NDF = 2.;
    T.C00 = sta[0].XYInfo.C00;
    T.C10 = sta[0].XYInfo.C10;
    T.C11 = sta[0].XYInfo.C11;

    T.C20 = T.C21 = 0;
    T.C30 = T.C31 = T.C32 = 0;
    T.C40 = T.C41 = T.C42 = T.C43 = 0;
    T.C22 = T.C33 = vINF;
    T.C44 = 1.;

      // find field along the track
    L1FieldValue B[3] _fvecalignment;
    L1FieldRegion fld _fvecalignment;

    fvec tx[3] = {
      (x[1]-x[0])/(z[1]-z[0]),
      (x[2]-x[0])/(z[2]-z[0]),
      (x[2]-x[1])/(z[2]-z[1])
    };
    fvec ty[3] = {
      (y[1]-y[0])/(z[1]-z[0]),
      (y[2]-y[0])/(z[2]-z[0]),
      (y[2]-y[1])/(z[2]-z[1])
    };
    for (int ih = 0; ih < NHits; ih++){
      fvec dz = (sta[ih].z - z[ih]);
      sta[ih].fieldSlice.GetFieldValue( x[ih] + tx[ih]*dz, y[ih] + ty[ih]*dz, B[ih] );
    };
    fld.Set( B[0], sta[0].z, B[1], sta[1].z, B[2], sta[2].z );

      // fit
    for( int ih = 1; ih < NHits; ih++){
      L1Extrapolate( T, z[ih], T.qp, fld );

      if ( ( isec != kAllPrimEIter ) && ( isec != kAllSecEIter ) ) {
#ifdef USE_RL_TABLE
        L1AddMaterial( T, fRadThick[ista[ih]].GetRadThick(T.x, T.y), T.qp );
#else
        L1AddMaterial( T, sta[ih].materialInfo, T.qp );
#endif
      
        if (ista[ih] == NMvdStations - 1) L1AddPipeMaterial( T, T.qp );
      }
      else {
#ifdef USE_RL_TABLE
        L1AddMaterial( T, fRadThick[ista[ih]].GetRadThick(T.x, T.y), T.qp, 1, 0.000511f*0.000511f );
#else
        L1AddMaterial( T, sta[ih].materialInfo, T.qp, 1, 0.000511f*0.000511f );
#endif
      
        if (ista[ih] == NMvdStations - 1) L1AddPipeMaterial( T, T.qp, 1, 0.000511f*0.000511f );
      }  
      
      //L1Filter( T, sta[ih].frontInfo, u[ih] );
      //L1Filter( T, sta[ih].backInfo,  v[ih] );
      L1Filter( T, sta[ih].frontInfo, u[ih], ww, &du2[ih]); //AZ
      L1Filter( T, sta[ih].backInfo,  v[ih], ww, &dv2[ih]); //AZ
    }

      // repeat several times in order to improve precision
    for (int iiter = 0; iiter < nIterations; iiter++){
        // fit backward
      // keep tx,ty,q/p
      int ih = NHits-1;
      T.x  = x[ih];
      T.y  = y[ih];
      T.z  = z[ih];
      T.chi2 = 0.;
      T.NDF = 2.;
      T.C00 = sta[ih].XYInfo.C00;
      T.C10 = sta[ih].XYInfo.C10;
      T.C11 = sta[ih].XYInfo.C11;
      //AZ
      fvec c_f = sta[ih].frontInfo.cos_phi;
      fvec s_f = sta[ih].frontInfo.sin_phi;
      fvec c_b = sta[ih].backInfo.cos_phi;
      fvec s_b = sta[ih].backInfo.sin_phi;
      fvec det = c_f*s_b - s_f*c_b;
      det *=det;
      fvec du22 = du2[ih], dv22 = dv2[ih];
      //du2 *= du2;
      //dv2 *= dv2;
      T.C00 = ( s_b*s_b*du22 + s_f*s_f*dv22 )/det;
      T.C10 =-( s_b*c_b*du22 + s_f*c_f*dv22 )/det;
      T.C11 = ( c_b*c_b*du22 + c_f*c_f*dv22 )/det;
      //AZ

      T.C20 = T.C21 = 0;
      T.C30 = T.C31 = T.C32 = 0;
      T.C40 = T.C41 = T.C42 = T.C43 = 0;
      T.C22 = T.C33 = vINF;
      T.C44 = 1.;

//       L1Filter( T, sta[ih].frontInfo, u[ih] );
//       L1Filter( T, sta[ih].backInfo,  v[ih] );
      for( ih = NHits-2; ih >= 0; ih--){
        L1Extrapolate( T, z[ih], T.qp, fld );

        if ( ( isec != kAllPrimEIter ) && ( isec != kAllSecEIter ) ) {
#ifdef USE_RL_TABLE
          L1AddMaterial( T, fRadThick[ista[ih]].GetRadThick(T.x, T.y), T.qp );
#else
          L1AddMaterial( T, sta[ih].materialInfo, T.qp );
#endif
          if (ista[ih] == NMvdStations) L1AddPipeMaterial( T, T.qp );
        }
        else {
#ifdef USE_RL_TABLE
          L1AddMaterial( T, fRadThick[ista[ih]].GetRadThick(T.x, T.y), T.qp, 1, 0.000511f*0.000511f );
#else
          L1AddMaterial( T, sta[ih].materialInfo, T.qp, 1, 0.000511f*0.000511f );
#endif
          if (ista[ih] == NMvdStations) L1AddPipeMaterial( T, T.qp, 1, 0.000511f*0.000511f );
        }
          
        //L1Filter( T, sta[ih].frontInfo, u[ih] );
        //L1Filter( T, sta[ih].backInfo,  v[ih] );
	L1Filter( T, sta[ih].frontInfo, u[ih], ww, &du2[ih]); //AZ
	L1Filter( T, sta[ih].backInfo,  v[ih], ww, &dv2[ih]); //AZ
      }
        // fit forward
      ih = 0;
      T.x  = x[ih];
      T.y  = y[ih];
      T.z  = z[ih];
      T.chi2 = 0.;
      T.NDF = 2.;
      T.C00 = sta[ih].XYInfo.C00;
      T.C10 = sta[ih].XYInfo.C10;
      T.C11 = sta[ih].XYInfo.C11;
      //AZ
      c_f = sta[ih].frontInfo.cos_phi;
      s_f = sta[ih].frontInfo.sin_phi;
      c_b = sta[ih].backInfo.cos_phi;
      s_b = sta[ih].backInfo.sin_phi;
      det = c_f*s_b - s_f*c_b;
      det *=det;
      du22 = du2[ih];
      dv22 = dv2[ih];
      //du2 *= du2;
      //dv2 *= dv2;
      T.C00 = ( s_b*s_b*du22 + s_f*s_f*dv22 )/det;
      T.C10 =-( s_b*c_b*du22 + s_f*c_f*dv22 )/det;
      T.C11 = ( c_b*c_b*du22 + c_f*c_f*dv22 )/det;
      //AZ

      T.C20 = T.C21 = 0;
      T.C30 = T.C31 = T.C32 = 0;
      T.C40 = T.C41 = T.C42 = T.C43 = 0;
      T.C22 = T.C33 = vINF;
      T.C44 = 1.;

//       L1Filter( T, sta[ih].frontInfo, u[ih] );
//       L1Filter( T, sta[ih].backInfo,  v[ih] );
      for( ih = 1; ih < NHits; ih++){
        L1Extrapolate( T, z[ih], T.qp, fld );

        if ( ( isec != kAllPrimEIter ) && ( isec != kAllSecEIter ) ) {
#ifdef USE_RL_TABLE
          L1AddMaterial( T, fRadThick[ista[ih]].GetRadThick(T.x, T.y), T.qp );
#else
          L1AddMaterial( T, sta[ih].materialInfo, T.qp );
#endif
          if (ista[ih] == NMvdStations + 1) L1AddPipeMaterial( T, T.qp );
        }
        else {
#ifdef USE_RL_TABLE
          L1AddMaterial( T, fRadThick[ista[ih]].GetRadThick(T.x, T.y), T.qp, 1, 0.000511f*0.000511f );
#else
          L1AddMaterial( T, sta[ih].materialInfo, T.qp, 1, 0.000511f*0.000511f );
#endif
          if (ista[ih] == NMvdStations + 1) L1AddPipeMaterial( T, T.qp, 1, 0.000511f*0.000511f );
        }
          
        //L1Filter( T, sta[ih].frontInfo, u[ih] );
        //L1Filter( T, sta[ih].backInfo,  v[ih] );
	L1Filter( T, sta[ih].frontInfo, u[ih], ww, &du2[ih]); //AZ
	L1Filter( T, sta[ih].backInfo,  v[ih], ww, &dv2[ih]); //AZ
      }
    } // for iiter

    T3.SetOneEntry(i3_4, T,i3_4);
  }//i3
} // f32


          /// Select triplets. Save them into vTriplets.
inline void L1Algo::f4(  // input
                int n3, int istal, int istam, int istar,
                nsL1::vector<L1TrackPar>::TSimd &T_3,

                vector<THitI> &hitsl_3,  vector<THitI> &hitsm_3,  vector<THitI> &hitsr_3,
                // output
                unsigned int &nstaltriplets,
                std::vector<L1Triplet> &vTriplets_part,
                unsigned int *TripStartIndexH, unsigned int *TripStopIndexH
// #ifdef XXX
//                 ,unsigned int &stat_n_trip
// #endif
               )
{

  for( int i3=0; i3<n3; i3++){
    int i3_V = i3/fvecLen;
    int i3_4 = i3%fvecLen;
    L1TrackPar &T3 = T_3[i3_V];
    
      // select
    fscal chi2 = T3.chi2[i3_4];
#ifdef DO_NOT_SELECT_TRIPLETS
    if (isec!=TRACKS_FROM_TRIPLETS_ITERATION)
#endif
    if ( !finite(chi2) || chi2 < 0 || chi2 > TRIPLET_CHI2_CUT )  continue; 
    // if ( isec == kAllSecIter && T3.qp[i3_4] +sqrt( T3.C44[i3_4] ) < 1./10. ) continue; // why does it cut so much of ExtraSec ?
    
      // prepare data
    fscal MaxInvMomS = MaxInvMom[0];
    fscal qp = MaxInvMomS + T3.qp[i3_4];
    if( qp < 0 )            qp = 0;          
    if( qp > MaxInvMomS*2 )  qp = MaxInvMomS*2;
    fscal Cqp = 5.*sqrt(fabs(T3.C44[i3_4]));  

    fscal scale = 255/(MaxInvMom[0]*2);

    qp  = (static_cast<unsigned int>(qp*scale) )%256;
    Cqp = (static_cast<unsigned int>(Cqp*scale))%256;
    Cqp += 1;

    if( Cqp < 0  ) Cqp = 0;
    if( Cqp > 20 ) Cqp = 20;
    qp = static_cast<unsigned char>( qp );

    THitI ihitl = hitsl_3[i3] + StsHitsUnusedStartIndex[istal];
    THitI ihitm = hitsm_3[i3] + StsHitsUnusedStartIndex[istam];
    THitI ihitr = hitsr_3[i3] + StsHitsUnusedStartIndex[istar];
    L1_ASSERT( ihitl < StsHitsUnusedStopIndex[istal], ihitl << " < " << StsHitsUnusedStopIndex[istal] );
    L1_ASSERT( ihitm < StsHitsUnusedStopIndex[istam], ihitm << " < " << StsHitsUnusedStopIndex[istam] );
    L1_ASSERT( ihitr < StsHitsUnusedStopIndex[istar], ihitr << " < " << StsHitsUnusedStopIndex[istar] );
    
      // save
    L1Triplet trip;
    trip.Set( ihitl, ihitm, ihitr,
              istal, istam, istar,
              0, static_cast<unsigned char>( qp ), chi2);   // 1.2/1000 sec
    
    trip.Cqp   = static_cast<unsigned char>( Cqp );

          //count statistics

    vTriplets_part.push_back(trip);  // 0.67/1000 sec

    if ( TripStopIndexH[ihitl] == 0 )
      TripStartIndexH[ihitl] = nstaltriplets;
    TripStopIndexH[ihitl] = ++nstaltriplets;  // < 0.001/100000

// #ifdef XXX
//          stat_n_trip++;
// #endif
  }//i3
} // f4()


          /// Find neighbours of triplets. Calculate level of triplets.
inline void L1Algo::f5(  // input
                 // output
               unsigned int *TripStartIndexH, unsigned int *TripStopIndexH,
               int *nlevel
               )
{
//     cout << "Start  Finding levels of triplets "  << endl;
#ifdef TRACKS_FROM_TRIPLETS
      if( isec != TRACKS_FROM_TRIPLETS_ITERATION )
#endif

  for  (int istal = NStations - 4; istal >= FIRSTCASTATION; istal--){
    for (int tripType = 0; tripType < 3; tripType++) { // tT = 0 - 123triplet, tT = 1 - 124triplet, tT = 2 - 134triplet
      if ( ( ((isec != kFastPrimJumpIter) && (isec != kAllPrimJumpIter) && (isec != kAllSecJumpIter)) && (tripType != 0)          ) ||
           ( ((isec == kFastPrimJumpIter) || (isec == kAllPrimJumpIter) || (isec == kAllSecJumpIter)) && (tripType != 0) && (istal == NStations - 4) )
        ) continue;

      int istam = istal + 1;
      int istar = istal + 2;
      switch (tripType){
        case 1:
          istar++;
          break;
        case 2:
          istam++;
          istar++;
          break;  
      }

      unsigned int offset_m = TripStartIndex[istam];

      for (int it = TripStartIndex[istal]; it < TripStopIndex[istal]; it++){
        L1Triplet *trip = &(vTriplets[it]);
        if ( istam != trip->GetMSta() ) continue;
        if ( istar != trip->GetRSta() ) continue;

        unsigned char level = 0;
        float  chi2 = trip->GetChi2();
        unsigned char  qp = trip->GetQp();

        THitI ihitl = trip->GetLHit();
        THitI ihitm = trip->GetMHit();
        THitI ihitr = trip->GetRHit();
        L1_ASSERT( ihitl < StsHitsUnusedStopIndex[istal], ihitl << " < " << StsHitsUnusedStopIndex[istal] );
        L1_ASSERT( ihitm < StsHitsUnusedStopIndex[istam], ihitm << " < " << StsHitsUnusedStopIndex[istam] );
        L1_ASSERT( ihitr < StsHitsUnusedStopIndex[istar], ihitr << " < " << StsHitsUnusedStopIndex[istar] );

          // neighbours should have 2 common hits
        unsigned int first_triplet;
        unsigned int last_triplet;
        unsigned int iN = TripStartIndexH[ihitm];   // first posible neighbour
        unsigned int lastN = TripStopIndexH[ihitm]; // last posible neighbour

          //   // find first triplet with 2 common hits
          // for (; (iN < lastN); ++iN){
          //   unsigned int jhitm;
          //   jhitm = vTriplets[iN].GetMHit();
          //   if (jhitm == ihitr) break;
          // };
        
        if (iN < lastN) { // if exist at least one trip
          L1_ASSERT(iN >= offset_m, iN << " >= " << offset_m);
          
          first_triplet = iN;
        
            // // find last triplets with 2 common hits
            // for (; iN < lastN; ++iN){
            //   if (vTriplets[iN].GetLHit() != ihitm) break;
            //   unsigned int jhitm;
            //   jhitm = vTriplets[iN].GetMHit();
            //   if (jhitm != ihitr) break;
            // };
            // last_triplet = iN-1;
          last_triplet = lastN;

          vector<unsigned int> neighCands; // save neighbour candidates
          neighCands.reserve(8); // ~average is 1-2 for central, up to 5
          for (iN = first_triplet; iN <= last_triplet; ++iN){

            if (vTriplets[iN].GetMSta() != istar) continue; // neighbours should have 2 common hits
            if (vTriplets[iN].GetMHit() != ihitr) continue;
          
            L1Triplet *tripn = &vTriplets[iN];

            fscal qp2 = tripn->GetQp();
            fscal Cqp1 = trip->Cqp;
            fscal Cqp2 = tripn->Cqp;
            if ( fabs(qp - qp2) > PickNeighbour * (Cqp1 + Cqp2) )  continue; // neighbours should have same qp
          
              // calculate level
            unsigned char jlevel = tripn->GetLevel();
            if ( level <= jlevel ) level = jlevel + 1;
            if (level == jlevel + 1) neighCands.push_back(iN);
          }
          for (unsigned int in = 0; in < neighCands.size(); in++) {
            const int nLevel = vTriplets[neighCands[in]].GetLevel();
            if (level == nLevel + 1) trip->neighbours.push_back(neighCands[in] - offset_m);
          }

        }; // if at least one triplet exists 

          // save information
        trip->Set( ihitl, ihitm, ihitr,
        istal, istam, istar,
        level, qp, chi2 );

        nlevel[level]++;

      }// vTriplets
    } // tripType
  } // istal
}

                                /// ------------------- doublets on station ----------------------

inline void L1Algo::DupletsStaPort(  /// creates duplets: input: @istal - start station number, @istam - last station number, @ip - index of portion, @&n_g1 - number of elements in portion, @*portionStopIndex
                      int istal, int istam,
                      unsigned int ip,
                      vector<int> &n_g1, unsigned int *portionStopIndex,

                        // output
                      L1TrackPar *T_1, /// @*T_1 - singlets perameters, @*fld_1 - field aproximation, @*hitsl_1- left hits of triplets, @&lmDuplets - existance of a doublet starting from the left hit, 
                      L1FieldRegion *fld_1,  ///  @&n_2 - number of douplets,@&i1_2 - index of 1st hit in portion indexed by doublet index, @&hitsm_2 - index of middle hit in hits array indexed by doublet index
                      THitI *hitsl_1,
                      
                      vector<bool> &lmDuplets,
                      
                      int &n_2,
                      vector<THitI> &i1_2,
                      vector<THitI> &hitsm_2
                      )
{
  if ( istam < NStations ) {
      L1Station &stam = vStations[istam];

            // prepare data
      L1HitPoint *vStsHits_l = &((*vStsHitPointsUnused)[0]) + StsHitsUnusedStartIndex[istal];
      L1HitPoint *vStsHits_m = &((*vStsHitPointsUnused)[0]) + StsHitsUnusedStartIndex[istam];

      int NHits_m = StsHitsUnusedStopIndex[istam] - StsHitsUnusedStartIndex[istam];



      int start_mhit = 0;   // hit on the middle stantion to start find new doublets



        fvec u_front[Portion/fvecLen], u_back[Portion/fvecLen];
        fvec du_front[Portion/fvecLen], du_back[Portion/fvecLen]; //AZ
        fvec zPos[Portion/fvecLen];

          /// prepare the portion of left hits data

        int n1 = n_g1[ip];
        
        f10(  // input
            (ip - portionStopIndex[istal+1]) * Portion, n1, vStsHits_l,
              // output
            u_front, u_back, zPos,
            hitsl_1,
	    du_front, du_back //AZ
          );

        for (int i = 0; i < n1; i++)
          L1_ASSERT(hitsl_1[i] < StsHitsUnusedStopIndex[istal] - StsHitsUnusedStartIndex[istal],
            hitsl_1[i] << " < " << StsHitsUnusedStopIndex[istal] - StsHitsUnusedStartIndex[istal]);
        
        int n1_V = (n1+fvecLen-1)/fvecLen;

          /// Get the field approximation. Add the target to parameters estimation. Propagaete to middle station.

        f11(istal, istam,
            n1_V,

            u_front, u_back, zPos,
              // output
            T_1, fld_1,
	    du_front, du_back //AZ
          );

          /// Find the doublets. Reformat data in the portion of doublets.

#ifdef DOUB_PERFORMANCE
        vector<THitI> hitsl_2;
#endif // DOUB_PERFORMANCE

        f20(  // input
            n1, stam,
            vStsHits_l, vStsHits_m, NHits_m,
            T_1, 
            hitsl_1,
              // output
            n_2,
            i1_2,
            start_mhit,
#ifdef DOUB_PERFORMANCE
            hitsl_2,
#endif // DOUB_PERFORMANCE
            hitsm_2,
            lmDuplets
           );

        for (unsigned int i = 0; i < hitsm_2.size(); i++)
          L1_ASSERT(hitsm_2[i] < StsHitsUnusedStopIndex[istam] - StsHitsUnusedStartIndex[istam], hitsm_2[i] << " " << StsHitsUnusedStopIndex[istam] - StsHitsUnusedStartIndex[istam]);

#ifdef DOUB_PERFORMANCE
        THitI* RealIHitL = &(RealIHit[StsHitsUnusedStartIndex[istal]]);
        THitI* RealIHitM = &(RealIHit[StsHitsUnusedStartIndex[istam]]);
        for (int i = 0; i < n2; i++){
          // int i_4 = i%4;
          // int i_V = i/4;
          THitI iHits[2] = {
            RealIHitL[hitsl_2[i]],
            RealIHitM[hitsm_2[i]]
          };
          fL1Eff_doublets->AddOne(iHits);
        }
#endif // DOUB_PERFORMANCE
  }
}


                            /// ------------------- Triplets on station ----------------------

inline void L1Algo::TripletsStaPort(   /// creates triplets: input: @istal - start station number, @istam - middle station number, @istar - last station number, @ip - index of portion, @&n_g1 - numer of elements in portion, @*portionStopIndex
                            int istal, int istam, int istar,
							///@nstaltriplets - , @*portionStopIndex, @*T_1 - track parameters for singlets, @*fld_1 - field approximation for singlets, @&n_2 - number of doublets in portion
							///  @&n_2 - number of douplets,@&i1_2 - index of 1st hit in portion indexed by doublet index, @&hitsm_2 - index of middle hit in hits array indexed by doublet index
                            unsigned int& nstaltriplets,
                            L1TrackPar *T_1,
                            L1FieldRegion *fld_1,
                            THitI *hitsl_1,

                            int &n_2, unsigned int *portionStopIndex,
                            vector<THitI> &i1_2,
                            vector<THitI> &hitsm_2,
                            
                            const vector<bool> &mrDuplets,
							/// output: @*vTriplets_part - array of triplets, @*TripStartIndexH, @*TripStopIndexH - start/stop index of a triplet in the array
                            
                              // output
                            std::vector<L1Triplet> *vTriplets_part,
                            unsigned int *TripStartIndexH, unsigned int *TripStopIndexH
                            )
{

  if (istar < NStations )
  {
        // prepare data
      L1Station &stam = vStations[istam];
      L1Station &star = vStations[istar];

      L1HitPoint *vStsHits_m = &((*vStsHitPointsUnused)[0]) + StsHitsUnusedStartIndex[istam];

      L1HitPoint *vStsHits_r = 0;
      vStsHits_r = &((*vStsHitPointsUnused)[0]) + StsHitsUnusedStartIndex[istar];

        int n3=0, n3_V;

        nsL1::vector<L1TrackPar>::TSimd T_3;

        vector<THitI> hitsl_3, hitsm_3, hitsr_3;


          /// Add the middle hits to parameters estimation. Propagate to right station.


        nsL1::vector<fvec>::TSimd u_front3, u_back3, z_pos3;
	nsL1::vector<fvec>::TSimd du_front3, du_back3; //AZ

        T_3.reserve(MaxPortionTriplets/fvecLen);
        hitsl_3.reserve(MaxPortionTriplets);
        hitsm_3.reserve(MaxPortionTriplets);
        hitsr_3.reserve(MaxPortionTriplets);
        u_front3.reserve(MaxPortionTriplets/fvecLen);
        u_back3.reserve(MaxPortionTriplets/fvecLen);
        z_pos3.reserve(MaxPortionTriplets/fvecLen);
        du_front3.reserve(MaxPortionTriplets/fvecLen); //AZ
        du_back3.reserve(MaxPortionTriplets/fvecLen); //AZ

          /// Find the triplets(right hit). Reformat data in the portion of triplets.

        f30(  // input
            vStsHits_r, stam, star,

            istam, istar,
            vStsHits_m,
            T_1, fld_1,
            hitsl_1,

            n_2,
            hitsm_2,
            i1_2,

            mrDuplets,
              // output
            n3,
            T_3,
            hitsl_3, hitsm_3, hitsr_3,
            u_front3, u_back3, z_pos3,
            &du_front3, &du_back3 //AZ
           );
        n3_V = (n3+fvecLen-1)/fvecLen;

        for (unsigned int i = 0; i < hitsl_3.size(); i++)
          L1_assert(hitsl_3[i] < StsHitsUnusedStopIndex[istal] - StsHitsUnusedStartIndex[istal]);
        for (unsigned int i = 0; i < hitsm_3.size(); i++)
          L1_assert(hitsm_3[i] < StsHitsUnusedStopIndex[istam] - StsHitsUnusedStartIndex[istam]);
        for (unsigned int i = 0; i < hitsr_3.size(); i++)
          L1_assert(hitsr_3[i] < StsHitsUnusedStopIndex[istar] - StsHitsUnusedStartIndex[istar]);

//        if (n3 >= MaxPortionTriplets) cout << "isec: " << isec << " stantion: " << istal << " portion number: " << ip << " CATrackFinder: Warning: Too many Triplets created in portion" << endl;




          /// Add the right hits to parameters estimation.
        f31(  // input
            n3_V,
            star,
            u_front3, u_back3, z_pos3,
              // output
            T_3,
	    &du_front3, &du_back3 //AZ
           );

                  /// refit
//         f32(  n3, istal, _RealIHit,          T_3,         hitsl_3, hitsm_3, hitsr_3, 0 );


#ifdef TRIP_PERFORMANCE
        THitI* RealIHitL = &(RealIHit[StsHitsUnusedStartIndex[istal]]);
        THitI* RealIHitM = &(RealIHit[StsHitsUnusedStartIndex[istam]]);
        THitI* RealIHitR = &(RealIHit[StsHitsUnusedStartIndex[istar]]);
        for (int i = 0; i < n3; i++){
          int i_4 = i%4;
          int i_V = i/4;
          THitI iHits[3] = {
            RealIHitL[hitsl_3[i]],
            RealIHitM[hitsm_3[i]],
            RealIHitR[hitsr_3[i]]
          };
#ifdef PULLS
          if ( fL1Eff_triplets->AddOne(iHits) )
            fL1Pulls->AddOne(T_3[i_V], i_4, iHits[2]);
#else
          fL1Eff_triplets->AddOne(iHits);
#endif
          if (T_3[i_V].chi2[i_4] < TRIPLET_CHI2_CUT)
            fL1Eff_triplets2->AddOne(iHits);
        }
#endif // TRIP_PERFORMANCE

          /// Fill Triplets.

        f4( // input
          n3, istal, istam, istar,
          T_3,
          hitsl_3, hitsm_3, hitsr_3,
            // output
          nstaltriplets,
          vTriplets_part[istal],
          TripStartIndexH, TripStopIndexH
          );
  }
}

    ///**********************************************************************************************************************
    ///**********************************************************************************************************************
    ///**********************************************************************************************************************
    ///*                                                                                                                    *
    ///*                                                                                                                    *
    ///*                                                                                                                    *
    ///*                                          CATrackFinder procedure                                                   *
    ///*                                                                                                                    *
    ///*                                                                                                                    *
    ///*                                                                                                                    *
    ///**********************************************************************************************************************
    ///**********************************************************************************************************************
    ///**********************************************************************************************************************




void L1Algo::CATrackFinder()
{

//   cout << "Start TrackFinder" << endl;

//   cout<<" total STS hits "<<vStsHits.size()<<endl;
    //cout<<" total STS hits, strips, back strips "
    //<<vStsHits.size()<<" "<<vStsStrips.size()<<" "<<vStsStripsB.size()<<endl;

#ifdef PULLS
  static L1AlgoPulls *l1Pulls_ = new L1AlgoPulls();
  fL1Pulls = l1Pulls_;
  fL1Pulls->Init();
#endif
#ifdef TRIP_PERFORMANCE
  static L1AlgoEfficiencyPerformance<3> *l1Eff_triplets_ = new L1AlgoEfficiencyPerformance<3>();
  fL1Eff_triplets = l1Eff_triplets_;
  fL1Eff_triplets->Init();
  static L1AlgoEfficiencyPerformance<3> *l1Eff_triplets2_ = new L1AlgoEfficiencyPerformance<3>();
  fL1Eff_triplets2 = l1Eff_triplets2_;
  fL1Eff_triplets2->Init();
#endif
#ifdef DOUB_PERFORMANCE
  static L1AlgoEfficiencyPerformance<2> *l1Eff_doublets_ = new L1AlgoEfficiencyPerformance<2>();
  fL1Eff_doublets = l1Eff_doublets_;
  fL1Eff_doublets->Init();
#endif
  
#ifdef DRAW
  static L1AlgoDraw draw;
  draw.InitL1Draw(this);
#endif

  cout.precision(6);

  TStopwatch c_time;   // for performance time
#if defined(XXX) || defined(COUNTERS)
  static unsigned int stat_N = 0; // number of events
  stat_N++;
#endif
  
#ifdef XXX
  TStopwatch c_timerG; // global
  TStopwatch c_timerI; // for iterations
  
  L1CATFIterTimerInfo gti; // global
  gti.Add("init  ");
  gti.Add("iterts");
  gti.Add("merge ");
  
  L1CATFTimerInfo ti;
  ti.SetNIter( fNFindIterations ); // for iterations
  ti.Add("init  ");
  // ti.Add("dblte1");
  // ti.Add("dblte2");
  ti.Add("tripl1");
  // ti.Add("tripl2");
  // ti.Add("tripl3");
  ti.Add("cpTrls"); 
  ti.Add("nghbrs"); 
  ti.Add("tracks"); 
  ti.Add("finish");
  
  static L1CATFIterTimerInfo stat_gti = gti;
  static L1CATFTimerInfo stat_ti = ti;
  

#endif

#ifdef COUNTERS
  static unsigned int stat_nStartHits = 0;
  static unsigned int stat_nHits[fNFindIterations] = {0};
  static unsigned int stat_nSinglets[fNFindIterations] = {0};
//  static unsigned int stat_nDoublets[fNFindIterations] = {0};
  static unsigned int stat_nTriplets[fNFindIterations] = {0};
#endif
  

  c_time.Start();

#ifdef XXX
    c_timerG.Start();
#endif

  vTracks.clear();
  vRecoHits.clear();

    // set all hits as usable
  for( vector<unsigned char>::iterator is= vSFlag.begin(); is!=vSFlag.end(); ++is)
    SetFUnUsed(*is);
  for( vector<unsigned char>::iterator is= vSFlagB.begin(); is!=vSFlagB.end(); ++is)
    SetFUnUsed(*is);


#ifdef XXX
  unsigned int
    // stat_max_n_trip = 0,
    stat_max_trip_size = 0,
    // stat_max_n_dup = 0,
    stat_max_BranchSize = 0,
    stat_max_n_branches = 0;
#endif

#ifdef DRAW
//   draw.DrawInfo();
    draw.ClearVeiw();
    draw.DrawInputHits();
    draw.DrawTarget();
    draw.DrawMCTracks();
    draw.SaveCanvas("MC_");
    draw.DrawAsk();
#endif

  vector< L1StsHit > vStsDontUsedHits_A;  /// arrays for copy from vStsHits only hits, which aren't used in created tracks
  vector< L1StsHit > vStsDontUsedHits_B;

  vector< L1HitPoint > vStsDontUsedHitsxy_A;  /// arrays for contain prepared information
  vector< L1HitPoint > vStsDontUsedHitsxy_B;

  vStsHitsUnused = &vStsDontUsedHits_B;      /// array of hits uses on current iteration
  std::vector< L1StsHit > *vStsHitsUnused_buf = &vStsDontUsedHits_A; /// buffer for copy

  vStsHitPointsUnused = &vStsDontUsedHitsxy_B;/// array of info for hits uses on current iteration
  std::vector< L1HitPoint > *vStsHitPointsUnused_buf = &vStsDontUsedHitsxy_A;

  vStsHitsUnused_buf->clear();
  vStsHitPointsUnused_buf->clear();
  vector<THitI> RealIHit_v; /// index of hit in vStsHits indexed by index of hit in vStsHitsUnused;
  RealIHit_v.resize(vStsHits.size());
  RealIHit = &(RealIHit_v[0]);


    /// fill the arrays for the first iteration
  int ihN = 0;
  for(int ista = 0; ista < NStations; ista++){
    StsHitsUnusedStartIndex[ista] = ihN;
    for(THitI ih = StsHitsStartIndex[ista]; ih < StsHitsStopIndex[ista]; ih++){
      L1StsHit &hit = vStsHits[ih];
      
      const L1HitPoint p = CreateHitPoint(hit,ista);
      //if (!( int(p.y/p.x*10000)%100 > 75 )) continue;
        
      RealIHit[ihN++] = ih;
      vStsDontUsedHits_B  .push_back( hit );
      vStsDontUsedHitsxy_B.push_back( p );
    }
    StsHitsUnusedStopIndex[ista]  =  ihN;
  }
  
#ifndef L1_NO_ASSERT
    for (int istal = NStations - 1; istal >= 0; istal--){
      L1_ASSERT( StsHitsStopIndex[istal] <= (vStsHits).size()             , StsHitsStopIndex[istal] << " <= " << (vStsHits).size());
      L1_ASSERT( StsHitsUnusedStopIndex[istal] <= (*vStsHitsUnused).size(), StsHitsUnusedStopIndex[istal] << " <= " << (*vStsHitsUnused).size());
    }
#endif // L1_NO_ASSERT

         // create Grid
    const float hitDensity = sqrt( vStsHitsUnused->size() );/// hitDensity - square root of number of hits, helps to keep the number of hits per bin (sell of grid) the same
    float yStep = 0.5/hitDensity; // empirics. 0.01*sqrt(2374) ~= 0.5
    if (yStep > 0.3) yStep = 0.3;
    const float xStep = yStep*3;

    for( int iS = 0; iS < NStations; iS++ ) {
      L1Grid &grid = vGrid[iS];
      grid.Create(-1,1,-0.6,0.6,xStep,yStep);
    }

      // sort hits by grid and y/z
    L1HitsSortHelper sh ( *vStsHitsUnused, *vStsHitPointsUnused, RealIHit_v, vGrid, StsHitsUnusedStartIndex, StsHitsUnusedStopIndex, NStations );
	/// sort hits by grid and y/z
	 ///  input: array of unused hits, unused points, RealIHit_v - array of initial hit indexes, grid, 
	 ///  StsHitsUnusedStartIndex, StsHitsUnusedStopIndex - indexes of 1st and last hits on station, NStations - number of stations
    sh.Sort();
  
#ifdef XXX
  c_timerG.Stop();
  gti["init  "] = c_timerG;
  c_timerG.Start(1);
#endif

#ifdef COUNTERS
  cout << " Begin " << endl;
  stat_nStartHits += vStsHitsUnused->size();
  cout << " NStartHits = " << stat_nStartHits/stat_N << endl;
#endif // COUNTERS
    // iterations of finding:
    // kFastPrimIter = 0, // primary fast track
    // kFastPrimJumpIter, // primary fast tracks with gaps. can be dissabled by macro
    // kAllPrimIter,      // primary all track
    // kAllPrimJumpIter,  // primary tracks with gaps. can be dissabled by macro
    // kAllSecIter        // secondary all track
  for (isec = 0; isec < fNFindIterations; isec++){ // all finder
    
#ifdef COUNTERS
  unsigned int nSinglets = 0;
  // unsigned int nDoublets = 0;
  // unsigned int nTriplets = 0;
#endif
    
#ifdef XXX
//     cout << " Begin of iteration " << isec << endl;
     TStopwatch c_timer;
#endif


    for( int iS = 0; iS < NStations; iS++ ) {
      L1Grid &grid = vGrid[iS];
      grid.Fill(&((*vStsHitPointsUnused)[StsHitsUnusedStartIndex[iS]]),StsHitsUnusedStopIndex[iS]-StsHitsUnusedStartIndex[iS]);
    }
	/// input: 1st hit on station, number of hits on station


     // --- SET PARAMETERS FOR THE ITERATION ---

       // first station used in the triplets finding
     FIRSTCASTATION = 0;
       // if ( (isec == kAllSecIter) || (isec == kAllSecEIter) || (isec == kAllSecJumpIter) )
       //   FIRSTCASTATION = 2;

     DOUBLET_CHI2_CUT = 2.706; // prob = 0.1
     TRIPLET_CHI2_CUT = 5;
     switch ( isec ) {
       case kFastPrimIter:
         TRIPLET_CHI2_CUT = 7.815;// prob = 0.05
         break;
       case kAllPrimIter:
       case kAllPrimEIter:
         TRIPLET_CHI2_CUT = 7.815; // prob = 0.05
         break;
       case kAllPrimJumpIter:
         TRIPLET_CHI2_CUT = 6.252; // prob = 0.1
         break;
       case kAllSecIter:
       case kAllSecEIter:
         TRIPLET_CHI2_CUT = 6.252;//2.706; // prob = 0.1
         break;
     }
     
    Pick_gather = 3.0; /// coefficient for size of region for attach new hits to the created track
    if ( (isec == kAllPrimIter) || (isec == kAllPrimEIter) || (isec == kAllPrimJumpIter) || (isec == kAllSecIter) || (isec == kAllSecEIter) || (isec == kAllSecJumpIter) )
      Pick_gather = 4.0;
    Pick_gather = 7.0; //AZ

    PickNeighbour = 1.0; // (PickNeighbour < dp/dp_error)  =>  triplets are neighbours
    // if ( (isec == kFastPrimIter) )
    //   PickNeighbour = 0.5; // TODO understand why works with 0.2
    PickNeighbour = 6.0; //AZ

    MaxInvMom = 1.0/0.5;                     // max considered q/p
    if ( (isec == kAllPrimJumpIter) || (isec == kAllSecIter) || (isec == kAllSecJumpIter) ) MaxInvMom =  1.0/0.1;
    if ( (isec == kAllPrimIter) || (isec == kAllPrimEIter) || (isec == kAllSecEIter) ) MaxInvMom = 1./0.05;

    MaxSlope = 1.1;
    if ( // (isec == kAllPrimIter) || (isec == kAllPrimEIter) || (isec == kAllPrimJumpIter) ||
         (isec == kAllSecIter) || (isec == kAllSecEIter) || (isec == kAllSecJumpIter) ) MaxSlope =  1.5;
    
      // define the target
    targX = 0; targY = 0; targZ = 0;      //  suppose, what target will be at (0,0,0)
    
    float SigmaTargetX = 0, SigmaTargetY = 0; // target constraint [cm]
    if ( (isec == kFastPrimIter) || (isec == kFastPrimIter2) || (isec == kFastPrimJumpIter) ||
         (isec == kAllPrimIter) || (isec == kAllPrimEIter) || (isec == kAllPrimJumpIter) ){ // target
      targB = vtxFieldValue;
      if ( (isec == kFastPrimIter) || (isec == kAllPrimIter) || (isec == kAllPrimEIter) )
        //AZ SigmaTargetX = SigmaTargetY = 0.01; // target
        SigmaTargetX = SigmaTargetY = 1.0; // target
      else
        //AZ SigmaTargetX = SigmaTargetY = 0.1;
        SigmaTargetX = SigmaTargetY = 5.0;
    }
    if ( (isec == kAllSecIter) || (isec == kAllSecEIter) || (isec == kAllSecJumpIter) ) { //use outer radius of the 1st station as a constraint
      L1Station &st = vStations[0];
      SigmaTargetX = SigmaTargetY = 10;//st.Rmax[0];
      targZ = 0.;//-1.;
      st.fieldSlice.GetFieldValue( 0, 0, targB );
    }
    //AZ SigmaTargetX = SigmaTargetY = 1.0; //AZ
    targZ = 0.0; //AZ

    TargetXYInfo.C00 = SigmaTargetX * SigmaTargetX;
    TargetXYInfo.C10 = 0;
    TargetXYInfo.C11 = SigmaTargetY * SigmaTargetY;

      /// Set correction in order to take into account overlaping and iff z.
      /// The reason is that low momentum tracks are too curved and goes not from target direction. That's why sort by hit_y/hit_z is not work idealy
      /// If sort by y then it is max diff between same station's modules (~0.4cm)
    MaxDZ = 0;
    if (  (isec == kAllPrimIter) || (isec == kAllPrimEIter) || (isec == kAllPrimJumpIter) ||
          (isec == kAllSecIter ) || (isec == kAllSecEIter)|| (isec == kAllSecJumpIter ) ) MaxDZ = 0.1;
    MaxDZ = 2; //AZ

    
    if (NStations > MaxNStations) cout << " CATrackFinder: Error: Too many Stantions" << endl;


    //     static unsigned int TripStartIndexH[MaxArrSize], TripStopIndexH[MaxArrSize]; // index region for triplets, which begins from hit, indexed by index of lhit.
    vector<unsigned int> TripStartIndexH_v, TripStopIndexH_v;
    vector<unsigned int> TripStartIndexHG124_v, TripStopIndexHG124_v;
    vector<unsigned int> TripStartIndexHG134_v, TripStopIndexHG134_v;
            
    TripStartIndexH_v.resize(vStsHitsUnused->size());
    TripStopIndexH_v .resize(vStsHitsUnused->size());
    TripStartIndexHG124_v.resize(vStsHitsUnused->size());
    TripStopIndexHG124_v .resize(vStsHitsUnused->size());
    TripStartIndexHG134_v.resize(vStsHitsUnused->size());
    TripStopIndexHG134_v .resize(vStsHitsUnused->size());
    unsigned int *TripStartIndexH = &(TripStartIndexH_v[0]), *TripStopIndexH = &(TripStopIndexH_v[0]);
    unsigned int *TripStartIndexHG124 = &(TripStartIndexHG124_v[0]), *TripStopIndexHG124 = &(TripStopIndexHG124_v[0]);
    unsigned int *TripStartIndexHG134 = &(TripStartIndexHG134_v[0]), *TripStopIndexHG134 = &(TripStopIndexHG134_v[0]);
        
    for(unsigned int i = 0; i < vStsHitsUnused->size(); i++){
      TripStartIndexH[i] = 0; // actually don't need this one, will be initialized at f4(..)
      TripStopIndexH[i] = 0;
      TripStartIndexHG124[i] = 0;
      TripStopIndexHG124[i] = 0;
      TripStartIndexHG134[i] = 0;
      TripStopIndexHG134[i] = 0;
    }

    vector <L1Triplet> vTriplets_part[MaxNStations];  // temporary arrays for parallelizing save triplets
    vector <L1Triplet> vTripletsG124_part[MaxNStations]; // gaped triplets. type 124
    vector <L1Triplet> vTripletsG134_part[MaxNStations]; // gaped triplets. type 134

    { // constr of tripletss

      // global arrays for keep data of doublets on all stations
    vector<int> n_g1;
    n_g1.reserve(MaxNPortion);
    unsigned int portionStopIndex[MaxNStations];  //number of portion on each station

#ifndef L1_NO_ASSERT
    for (int istal = NStations - 1; istal >= 0; istal--){
      L1_ASSERT( StsHitsUnusedStopIndex[istal] >= StsHitsUnusedStartIndex[istal], StsHitsUnusedStopIndex[istal] << " >= " << StsHitsUnusedStartIndex[istal]);
      L1_ASSERT( StsHitsUnusedStopIndex[istal] <= (*vStsHitsUnused).size(), StsHitsUnusedStopIndex[istal] << " <= " << (*vStsHitsUnused).size());
    }
#endif // L1_NO_ASSERT
    //unsigned int nPortions = 0; /// number of portions
    {   
	 /// possible left hits of triplets are splited in portions of 16 (4 SIMDs) to use memory faster
      portionStopIndex[NStations-1] = 0;
      unsigned int ip = 0;  //index of curent portion

      for (int istal = NStations-2; istal >= FIRSTCASTATION; istal--){  //start downstream chambers
        int NHits_l = StsHitsUnusedStopIndex[istal] - StsHitsUnusedStartIndex[istal];

        int NHits_l_P = NHits_l/Portion;

        for( int ipp = 0; ipp < NHits_l_P; ipp++ ){
//           n_g1[ip++] = Portion;
          n_g1.push_back(Portion);
#ifdef COUNTERS
          nSinglets += Portion;
#endif
          ip++;
        } // start_lh - portion of left hits

//         n_g1[ip++] = NHits_l - NHits_l_P*Portion;
        n_g1.push_back(NHits_l - NHits_l_P*Portion);
#ifdef COUNTERS
        nSinglets += n_g1.back();
#endif
        ip++;
        portionStopIndex[istal] = ip;
      }// lstations
      //nPortions = ip;
    }
    
#ifdef COUNTERS
  stat_nSinglets[isec] += nSinglets;
#endif
  
      ///   CREATE doublets
#ifdef XXX
    c_timer.Stop();
    ti[isec]["init  "] = c_timer;
    c_timer.Start(1);
#endif

    const unsigned int vPortion = Portion/fvecLen;
    L1TrackPar T_1[vPortion];
    L1FieldRegion fld_1[vPortion];
    THitI hitsl_1[Portion];
    L1TrackPar TG_1[vPortion];
    L1FieldRegion fldG_1[vPortion];
    THitI hitslG_1[Portion];
    vector<bool> lmDuplets[MaxNStations]; // is exist a doublet started from indexed by left hit
    vector<bool> lmDupletsG[MaxNStations]; // is exist a doublet started from indexed by left hit
    for (int istal = NStations-2; istal >= FIRSTCASTATION; istal--){//  //start downstream chambers
      unsigned int nstaltriplets = 0; // find triplets begin from this stantion
      unsigned int nstaltripletsG124 = 0; // find triplets begin from this stantion
      unsigned int nstaltripletsG134 = 0; // find triplets begin from this stantion

      int NHitsSta = StsHitsStopIndex[istal] - StsHitsStartIndex[istal];
      lmDuplets[istal].resize(NHitsSta);
      lmDupletsG[istal].resize(NHitsSta);
      for(unsigned int ip = portionStopIndex[istal+1]; ip < portionStopIndex[istal]; ip++ ){
        // nsL1::vector<L1TrackPar>::TSimd T_1; // estimation of parameters
        // nsL1::vector<L1FieldRegion>::TSimd fld_1; // magnetic field
        // vector<THitI> hitsl_1; // left hits indexed by number of singlets in portion(i2)
        vector<THitI> hitsm_2; /// middle hits indexed by number of doublets in portion(i1)
        vector<THitI> i1_2; /// index in portion of singlets(i1) indexed by index in portion of doublets(i2)
        int n_2; /// number of doublets in portion

        hitsm_2.reserve(MaxPortionDoublets);
        i1_2.reserve(MaxPortionDoublets);

        DupletsStaPort(  // input
          istal, istal + 1,
          ip,
          n_g1, portionStopIndex,
                      
            // output
          T_1,
          fld_1,
          hitsl_1,

          lmDuplets[istal],

          n_2,
          i1_2,
          hitsm_2
          );
        TripletsStaPort(  // input
          istal, istal + 1,  istal + 2,
          nstaltriplets,
          T_1,
          fld_1,
          hitsl_1,

          n_2, portionStopIndex,
          i1_2,
          hitsm_2,

          lmDuplets[istal+1],
            // output
          vTriplets_part,
          TripStartIndexH, TripStopIndexH
          );
        if ( (isec == kFastPrimJumpIter) || (isec == kAllPrimJumpIter) || (isec == kAllSecJumpIter) ) {
          vector<THitI> hitsmG_2;
          vector<THitI> i1G_2;
          int nG_2;
          hitsmG_2.reserve(MaxPortionDoublets);
          i1G_2.reserve(MaxPortionDoublets);
          
          DupletsStaPort(  // input
            istal, istal + 2,
            ip,
            n_g1, portionStopIndex,
              // output
            TG_1,
            fldG_1,
            hitslG_1,
                      
            lmDupletsG[istal],

            nG_2,
            i1G_2,
            hitsmG_2
            );
          TripletsStaPort(  // input
            istal, istal + 1,  istal + 3,
            nstaltripletsG124,
            T_1,
            fld_1,
            hitsl_1,

            n_2, portionStopIndex,
            i1_2,
            hitsm_2,

            lmDupletsG[istal+1],
              // output
            vTripletsG124_part,
            TripStartIndexHG124, TripStopIndexHG124
            );
          TripletsStaPort(  // input
            istal, istal + 2,  istal + 3,
            nstaltripletsG134,
            TG_1,
            fldG_1,
            hitslG_1,

            nG_2, portionStopIndex,
            i1G_2,
            hitsmG_2,

            lmDuplets[istal+2],
              // output
            vTripletsG134_part,
            TripStartIndexHG134, TripStopIndexHG134
            );
        }
      }// l-stations
    }
#ifdef XXX
    c_timer.Stop();
    ti[isec]["tripl1"] = c_timer;
#endif
  } // constr of triplets
// #ifdef XXX
//         c_timer.Stop();
//         ti[isec]["tripls"] = c_timer;
// #endif

// #ifdef XXX
//     cout<<"isec: " << isec <<  "  n hits, dup, tr_c, trip: "
//         <<istat_nhits<<" "<<istat_nduplets<<" "<<istat_n_triplet_c<<" "<<istat_n_triplets<<endl;
// #endif

          /// save triplets in vTriplets
// #ifdef XXX
//     cout << " copy triplets in vTriplets " << endl;
// #endif
#ifdef XXX
    c_timer.Start(1);
#endif

    vTriplets.clear();
    int tSize = 0;
    for (int istal = NStations - 2; istal >= FIRSTCASTATION; istal--){
      tSize += vTriplets_part[istal].size(); 
      tSize += vTripletsG124_part[istal].size();
      tSize += vTripletsG134_part[istal].size();
    }
    vTriplets.reserve(tSize); // somehow resize is slower...
    
    for (int istal = NStations - 2; istal >= FIRSTCASTATION; istal--){
      TripStartIndex[istal] = vTriplets.size();

      for (unsigned int i = 0; i < vTriplets_part[istal].size(); i++){
        vTriplets.push_back(vTriplets_part[istal][i]);
      }
      for (unsigned int i = 0; i < vTripletsG124_part[istal].size(); i++){ // it's initialized, so for all iterations it's ok
        vTriplets.push_back(vTripletsG124_part[istal][i]); /// save triplets in vTriplets with gaps
      }
      for (unsigned int i = 0; i < vTripletsG134_part[istal].size(); i++){
        vTriplets.push_back(vTripletsG134_part[istal][i]); /// save triplets in vTriplets with gaps
      }

      for (THitI i = StsHitsUnusedStartIndex[istal]; i < StsHitsUnusedStopIndex[istal]; i++){
        TripStartIndexH[i] += TripStartIndex[istal];
        TripStopIndexH[i]  += TripStartIndex[istal]
          + TripStopIndexHG124[i] - TripStartIndexHG124[i]
          + TripStopIndexHG134[i] - TripStartIndexHG134[i];
      }

      TripStopIndex[istal] = vTriplets.size();
    }

#ifdef COUNTERS
    stat_nTriplets[isec] += vTriplets.size();
#endif
    
// #ifdef DRAW
//     {
//       draw.ClearVeiw();
//   //   draw.DrawInfo();
//       draw.DrawRestHits(StsHitsUnusedStartIndex, StsHitsUnusedStopIndex, RealIHit);
//       draw.DrawTriplets(vTriplets,RealIHit);
//       TString name = "Trip_";
//       name += isec;
//       name += "_";
//       draw.SaveCanvas(name);
//       draw.DrawAsk();
//     }
// #endif

// #ifdef XXX
//     { // calculate occupied memory size // TODO

// //       int sizeD, sizeN, sizeT, sizeF, sizeH, sizeTrip;
// //       sizeD = sizeof(THitI)*MaxArrSize*MaxNStations*2; // Duplets_*,
// //       sizeN = sizeof(int)*MaxNPortion*2; // n_g*
// //       sizeT = sizeof(L1TrackPar)*MaxNPortion*Portion/fvecLen; // T_g1
// //       sizeF = sizeof(L1FieldRegion)*MaxNPortion*Portion/fvecLen; // fld_g1
// //       sizeH = sizeof(THitI)*MaxNPortion*(Portion + MaxPortionDoublets*2); // hitsl_g1, hitsm_g2,i1_g2
// //       sizeTrip = sizeof(L1Triplet)*vTriplets.size();

//       int sizeD2 = 0, sizeN2, sizeT2, sizeF2, sizeH2, sizeTrip2;
//       int ndoublets = 0;
//       for(int i = 0; i < NStations; i++){
//         sizeD2 += Duplets_hits[i].size();
//         sizeD2 += Duplets_start[i].size();
//         ndoublets += Duplets_hits[i].size();
//         sizeD2*=sizeof(THitI);
//       }

//       cout << "number of doublets: " << ndoublets << endl;
//       cout << "number of triplets: " << vTriplets.size() << endl;
// /*
//       sizeN2 = ( n_g1.size() + n_g2.size() )*sizeof(int);
//       sizeT2 = T_g1.CalcSize();
//       sizeF2 = fld_g1.CalcSize();
//       sizeH2 = ( hitsl_g1.CalcSize() + hitsm_g2.CalcSize() + i1_g2.CalcSize());

//       cout << sizeD/1000 << " D " << sizeD2/1000 << " [1000elem]" << endl;
//       cout << sizeN/1000 << " N " << sizeN2/1000 << " [1000elem]" << endl;
//       cout << sizeT/1000 << " T " << sizeT2/1000 << " [1000elem]" << endl;
//       cout << sizeF/1000 << " F " << sizeF2/1000 << " [1000elem]" << endl;
//       cout << sizeH/1000 << " H " << sizeH2/1000 << " [1000elem]" << endl;

//       long int size = sizeD2 + sizeN2 + sizeT2+ sizeF2 + sizeH2 + sizeTrip;
//       cout << "Size: " << size/1000 << " KB" << endl;*/
//     }
// #endif
#ifdef XXX
    c_timer.Stop();
    ti[isec]["cpTrls"] = c_timer;
#endif

          /// Find neighbours of triplets.
// #ifdef XXX
//     cout << " Find neighbours of triplets. " << endl;
// #endif
#ifdef XXX
    c_timer.Start(1);
#endif

    int nlevels[MaxNStations];  // number of triplets with some number of neighbours.
    for (int il = 0; il < NStations; ++il) nlevels[il] = 0;

     f5(   // input
          // output
        TripStartIndexH, TripStopIndexH,
        nlevels
      );
#ifdef XXX
    c_timer.Stop();
    ti[isec]["nghbrs"] = c_timer;
#endif


    ///====================================================================
    ///=                                                                  =
    ///=        Collect track candidates. CREATE TRACKS                   =
    ///=                                                                  =
    ///====================================================================

// #ifdef XXX
//     cout<<"---- Collect track candidates. ----"<<endl;
// #endif
#ifdef XXX
    c_timer.Start(1);
#endif
    int min_level = 0; // min level for start triplet. So min track length = min_level+3.
//    if (isec == kFastPrimJumpIter) min_level = 1;
    if ( (isec == kAllSecIter) || (isec == kAllSecEIter) || (isec == kAllSecJumpIter) )
      min_level = 1; // only the long low momentum tracks
#ifdef TRACKS_FROM_TRIPLETS
    if (isec == TRACKS_FROM_TRIPLETS_ITERATION)
      min_level = 0;
#endif
      //    if (isec == -1) min_level = NStations-3 - 3; //only the longest tracks


      // collect consequtive: the longest tracks, shorter, more shorter and so on
    for (int ilev = NStations-3; ilev >= min_level; ilev--){ // choose length
      vector<L1Branch> vtrackcandidate; vtrackcandidate.clear();

        //  how many levels to check
      int nlevel = (NStations-2)-ilev+1;
      int ntracks = 0;

      for( int istaF = FIRSTCASTATION; istaF <= NStations-3-ilev; istaF++ ){ // choose first track-station

        // if( ( (isec == kAllSecIter) || (isec == kAllSecEIter) || (isec == kAllSecJumpIter) ) &&
        //     ( istaF >= NStations-6) ) break; // ghost supression !!!  too strong for Lambda. According to I.Vassiliev NStations-4 is good

        int trip_first = TripStartIndex[istaF];
        int trip_end   = TripStopIndex[istaF];
        for( int itrip=trip_first; itrip<trip_end; itrip++ ){
          L1Triplet *first_trip = &vTriplets[itrip];

            // ghost supression !!!
#ifndef FIND_GAPED_TRACKS
          if( /*(isec == kFastPrimIter) ||*/ (isec == kAllPrimIter) || (isec == kAllPrimEIter) || (isec == kAllSecIter) || (isec == kAllSecEIter) || (isec == kAllSecJumpIter) ) {
#else
          if( (isec == kFastPrimIter) || (isec == kFastPrimIter2) || (isec == kFastPrimJumpIter) ||
              (isec == kAllPrimIter) || (isec == kAllPrimEIter) || (isec == kAllPrimJumpIter) ||
              (isec == kAllSecIter) || (isec == kAllSecEIter) || (isec == kAllSecJumpIter) ) {
#endif
#ifdef TRACKS_FROM_TRIPLETS
            if (isec != TRACKS_FROM_TRIPLETS_ITERATION)
#endif
            {
              if ( isec != kFastPrimIter && isec != kAllPrimIter && isec != kAllPrimEIter && isec != kAllSecEIter )
              if ( first_trip->GetLevel() == 0 ) continue; // ghost suppression // find track with 3 hits only if it was created from a chain of triplets, but not from only one triplet
              if ( (ilev == 0) &&
                   (GetFStation(vSFlag[(*vStsHitsUnused)[first_trip->GetLHit()].f]) != 0) ) continue;  // ghost supression // collect only MAPS tracks-triplets
            }
            if ( first_trip->GetLevel() < ilev ) continue; // try only triplets, which can start track with ilev+3 length. w\o it have more ghosts, but efficiency either
          }
          
          if( GetFUsed( vSFlag[(*vStsHitsUnused)[first_trip->GetLHit()].f] | vSFlagB[(*vStsHitsUnused)[first_trip->GetLHit()].b] ) ) continue;

           // OK, search the best track candidate in the branches starting from the first_trip
          
          L1Triplet *curr_trip = first_trip;
          L1Branch curr_tr;
          curr_tr.StsHits.push_back(RealIHit[first_trip->GetLHit()]);
          unsigned char curr_L = 1;
          fscal curr_chi2 = first_trip->GetChi2();

          L1Branch best_tr = curr_tr;
          fscal best_chi2 = curr_chi2;
          unsigned char best_L = curr_L;
          int NCalls = 1;
#ifdef TRACKS_FROM_TRIPLETS
          best_tr.StsHits.push_back(RealIHit[first_trip->GetMHit()]);
          best_tr.StsHits.push_back(RealIHit[first_trip->GetRHit()]);
          best_L += 2;
#else
          CAFindTrack(istaF, best_tr, best_L, best_chi2, curr_trip, curr_tr, curr_L, curr_chi2, NCalls);
          // if (best_L >= 3){
          //   BranchExtender(best_tr);
          //   best_L = best_tr.StsHits.size();
          // }
#endif

          // if( ( (isec == kAllSecIter) || (isec == kAllSecEIter)  || (isec == kAllSecJumpIter) ) &&
          //     (GetFStation((*vStsHitsUnused)[best_tr.StsHits[0]].f ) >= 4) ) break; // ghost supression
          
          // {
          //   if ( best_L < 3 ) continue;
          //   BranchExtender(best_tr);
          //   best_L = best_tr.StsHits.size();
          // }

          if ( best_L < ilev + 2 ) continue;      // lose maximum one hit
          if ( best_L < min_level + 3 ) continue; // should find all hits for min_level

          int ndf = best_L*2-5;
          best_chi2 = best_chi2/ndf; //normalize

          // if (best_chi2 > TRACK_CHI2_CUT) { // never works because of way neibour triplets and chi2 defined !
          //   cout << " A " << endl;
          //   continue;
          // }
          
          // BranchExtender(best_tr);
          // best_L = best_tr.StsHits.size();

//          if( (isec == kAllPrimIter) || (isec == kAllPrimEIter) || (isec == kAllPrimJumpIter) || (isec == kAllSecIter) || (isec == kAllSecEIter) || (isec == kAllSecJumpIter) )
#ifndef TRACKS_FROM_TRIPLETS
            if( fGhostSuppression ){
              if( best_L == 3 ){
                  // if( isec == kAllSecIter ) continue; // too /*short*/ secondary track
                if( ( (isec == kAllSecIter) || (isec == kAllSecEIter) || (isec == kAllSecJumpIter) ) && (istaF != 0) ) continue; // too /*short*/ non-MAPS track
                if( (isec != kAllSecIter) && (isec != kAllSecEIter) && (isec != kAllSecJumpIter) && (best_chi2 > 5.0) ) continue;
              }
            }
#endif
          // BranchExtender(best_tr);
          // best_L = best_tr.StsHits.size();
          
             // store candidate
          best_tr.Set( istaF, best_L, best_chi2, first_trip->GetQpOrig(MaxInvMom[0]));
          vtrackcandidate.push_back(best_tr);
          
          ntracks++;
        }  // itrip
        //cout<<" level "<<ilev<<", station "<<ista<<" ok"<<endl;
      } // istaF

      // cout <<"Level "<< ilev <<" track candidates "<< ntracks << endl;
      // cout << " total number of track candidates " << vtrackcandidate.size() <<endl;

      if (--nlevel == 0) break;
        // -- make competition between tracks of the same length --
        // sort track candidates
      vector<L1Branch*> vptrackcandidate;  // vptrackcandidate - array of pointers to vtrackcandidate
      vptrackcandidate.clear();

      for (vector<L1Branch>::iterator trIt = vtrackcandidate.begin();
                                       trIt != vtrackcandidate.end(); ++trIt){

        vptrackcandidate.push_back(&(*trIt));

      }
      //if (isec <= 1)
      sort(vptrackcandidate.begin(), vptrackcandidate.end(), L1Branch::comparePChi2);
      //else
      //sort(vptrackcandidate.begin(), vptrackcandidate.end(), L1Branch::comparePMomentum);



        // choose and save tracks
//       int nUsed[10] = {0,0,0,0,0, 0,0,0,0,0};
      ntracks = 0;
      for (vector<L1Branch*>::iterator   trIt = vptrackcandidate.begin();
                                        trIt != vptrackcandidate.end(); ++trIt){
        L1Branch *tr = *trIt;
#ifdef EXTEND_TRACKS
        BranchExtender(*tr);
#endif // EXTEND_TRACKS
        
          // check if some hits have been used already
        int nused = 0;
        for (vector<THitI>::iterator phIt = tr->StsHits.begin();
            phIt != tr->StsHits.end(); ++phIt){
          L1StsHit &h = vStsHits[*phIt];
          if ( GetFUsed( vSFlag[h.f] | vSFlagB[h.b] ) ) nused++;
        }
#ifdef TRACKS_FROM_TRIPLETS
        if ( isec != TRACKS_FROM_TRIPLETS_ITERATION )
#endif
        if (nused > 0) continue; // don't allow tracks have shared hits. Track will be shorter, leave it for the next iteration

        // if ( (isec == kFastPrimIter || isec == kAllPrimIter) && (nused > 2) ||
        //      (isec != kFastPrimIter) && (isec != kAllPrimIter) && (nused > 0) ){/*nUsed[nused+1]++;*/ continue;}

        //=======================================================
        // gather MAPS hits using the Kalman filter
        // was here, skipped for a moment
        //=======================================================

        // if(isec <= kAllSecIter){ // check chi2. gives almost nothing
        //   L1TrackPar T;
        //   BranchFitterFast(*tr, T, 0);
        //   if (T.chi2[0]/T.NDF[0] > TRACK_CHI2_CUT) continue;
        // }
        
        // BranchExtender(*tr);
        
          // store track
        for (vector<THitI>::iterator phitIt = tr->StsHits.begin();
                                                  phitIt != tr->StsHits.end(); ++phitIt){
          L1StsHit &h = vStsHits[*phitIt];
#ifdef TRACKS_FROM_TRIPLETS
          if ( isec != TRACKS_FROM_TRIPLETS_ITERATION )
#endif
          {
            SetFUsed( vSFlag[h.f] );
            SetFUsed( vSFlagB[h.b] );
          }
#ifdef TRACKS_FROM_TRIPLETS
          if ( isec == TRACKS_FROM_TRIPLETS_ITERATION )
#endif
          vRecoHits.push_back(*phitIt);
        }

        L1Track t;
        t.NHits = tr->StsHits.size();
        t.Momentum = tr->Momentum;
        for( int i=0; i<6; i++) t.TFirst[i] = t.TLast[i]=0; // CHECKME don't need this
        for( int i=0; i<15; i++ ) t.CFirst[i] = t.CLast[i] = 10;
        t.chi2 = tr->chi2;
        t.NDF = t.NHits*2-5;

#ifdef TRACKS_FROM_TRIPLETS
        t.NDF = 3; // 3 hits + target
        if ( isec == kAllSecIter || (isec == kAllSecEIter) )
          t.NDF = 1; // 3 hits
        if ( isec == TRACKS_FROM_TRIPLETS_ITERATION )
#endif
        vTracks.push_back(t);
        ntracks++;

      } // i_trackCandidate

//       for (int iu = 0; iu < 10; iu++) cout << iu+1 << " " << nUsed[iu] << endl;
#ifdef XXX
      unsigned int Bsize=0, nb=0;
      for (vector<L1Branch*>::iterator   trIt = vptrackcandidate.begin();
                                         trIt != vptrackcandidate.end(); ++trIt){
        L1Branch *tr = *trIt;
        Bsize+= sizeof(L1Branch) + sizeof(THitI) * tr->StsHits.size();
        nb++;
      }

      if( Bsize>stat_max_BranchSize ) stat_max_BranchSize = Bsize;
      if( nb>stat_max_n_branches ) stat_max_n_branches = nb;
#endif

      //cout<< " track candidates stored: "<< ntracks <<endl;
    } //ilev
      
#ifdef XXX
    c_timer.Stop();
    ti[isec]["tracks"] = c_timer;
    c_timer.Start(1);
#endif
      // renew hits array
// #ifdef XXX
//     cout << " renew hits arrays " << endl;
// #endif

    int StsHitsUnusedStartIndex_temp;
    int ista = 0;
    int nDontUsedHits = 0;
    const int HSize = vStsHitsUnused->size();
    vStsHitsUnused_buf->clear();
    vStsHitPointsUnused_buf->clear();
    vStsHitsUnused_buf->reserve(HSize); // in fact a memory is already reserved after clear
    vStsHitPointsUnused_buf->reserve(HSize);
    for(; ista < NStations; ista++){

      StsHitsUnusedStartIndex_temp = StsHitsUnusedStartIndex[ista];
      StsHitsUnusedStartIndex[ista] = nDontUsedHits;
      for(THitI ih = StsHitsUnusedStartIndex_temp; ih < StsHitsUnusedStopIndex[ista]; ih++){
        const L1StsHit &hit = (*vStsHitsUnused)[ih];
        if( GetFUsed( vSFlag[hit.f] | vSFlagB[hit.b] ) ){ continue;} // if used
        vStsHitsUnused_buf->push_back(hit);
        vStsHitPointsUnused_buf->push_back((*vStsHitPointsUnused)[ih]);

        RealIHit[nDontUsedHits] = RealIHit[ih];
        nDontUsedHits++;
      }
      StsHitsUnusedStopIndex[ista] = nDontUsedHits;
    }
    std::vector< L1StsHit > *vStsHitsUnused_temp = vStsHitsUnused;
    vStsHitsUnused = vStsHitsUnused_buf;
    vStsHitsUnused_buf = vStsHitsUnused_temp;
    std::vector< L1HitPoint > *vStsHitsUnused_temp2 = vStsHitPointsUnused;
    vStsHitPointsUnused = vStsHitPointsUnused_buf;
    vStsHitPointsUnused_buf = vStsHitsUnused_temp2;

    vStsHitsUnused_buf->clear();
    vStsHitPointsUnused_buf->clear();
// cout << " clear hits array end " << endl;

//     cout<<"End of collecting track candidates "<<endl;
#ifdef XXX
    c_timer.Stop();
    ti[isec]["finish"] = c_timer;
#endif
#ifdef XXX
//     if( stat_max_n_trip<stat_n_trip ) stat_max_n_trip = vTriplets.size();
    unsigned int tsize = vTriplets.size()*sizeof(L1Triplet);
    if( stat_max_trip_size < tsize ) stat_max_trip_size = tsize;
#endif
// #ifdef DRAW
//     draw.ClearVeiw();
// //   draw.DrawInfo();
//     draw.DrawRestHits(StsHitsUnusedStartIndex, StsHitsUnusedStopIndex, RealIHit);
//     draw.DrawRecoTracks();
//     draw.SaveCanvas("Reco_"+isec+"_");
//     draw.DrawAsk();
// #endif

// #ifdef PULLS
//       fL1Pulls->Build(1);
// #endif
#ifdef COUNTERS
    stat_nHits[isec] += vStsHitsUnused->size();
    
    cout << "iter = " << isec << endl;
    cout << " NSinglets = " << stat_nSinglets[isec]/stat_N << endl;
//    cout << " NDoublets = " << stat_nDoublets[isec]/stat_N << endl;
    cout << " NTriplets = " << stat_nTriplets[isec]/stat_N << endl;
    cout << " NHitsUnused = " << stat_nHits[isec]/stat_N << endl;
#endif // COUNTERS
    
  } // for (int isec

#ifdef XXX
  c_timerG.Stop();
  gti["iterts"] = c_timerG;
  c_timerG.Start(1);
#endif

#ifndef TRACKS_FROM_TRIPLETS
#ifdef MERGE_CLONES
   CAMergeClones();
#endif
#endif
  
#ifdef XXX
  c_timerG.Stop();
  gti["merge "] = c_timerG;
#endif

  //cout<< "Total number of tracks found " << vTracks.size() <<endl;

  //==================================

  c_time.Stop();

//   cout << "End TrackFinder" << endl;
//  CATime = (double(c_time.CpuTime()));
  CATime = (double(c_time.RealTime()));

#ifdef XXX
  

  cout << endl << " --- Timers, ms --- " << endl;
  ti.Calc();
  stat_ti += ti;
  L1CATFTimerInfo tmp_ti = stat_ti/0.001/stat_N; // ms
  
  tmp_ti.PrintReal();
  stat_gti += gti;
  L1CATFIterTimerInfo tmp_gti = stat_gti/0.001/stat_N; // ms
  tmp_gti.PrintReal( 1 );

#if 0
  static long int NTimes =0, NHits=0, HitSize =0, NStrips=0, StripSize =0, NStripsB=0, StripSizeB =0,
    NDup=0, DupSize=0, NTrip=0, TripSize=0, NBranches=0, BranchSize=0, NTracks=0, TrackSize=0 ;

  NTimes++;
  NHits += vStsHitsUnused->size();
  HitSize += vStsHitsUnused->size()*sizeof(L1StsHit);
  NStrips+= vStsStrips.size();
  StripSize += vStsStrips.size()*sizeof(fscal) + vSFlag.size()*sizeof(unsigned char);
  NStripsB+= vSFlagB.size();
  StripSizeB += vStsStripsB.size()*sizeof(fscal) + vSFlagB.size()*sizeof(unsigned char);
  NDup += stat_max_n_dup;
  DupSize += stat_max_n_dup*sizeof(/*short*/ int);
  NTrip += stat_max_n_trip;
  TripSize += stat_max_trip_size;

  NBranches += stat_max_n_branches;
  BranchSize += stat_max_BranchSize;
  NTracks += vTracks.size();
  TrackSize += sizeof(L1Track)*vTracks.size() + sizeof(THitI)*vRecoHits.size();
  int k = 1024*NTimes;

  cout<<"L1 Event size: \n"
      <<HitSize/k<<"kB for "<<NHits/NTimes<<" hits, \n"
      <<StripSize/k<<"kB for "<<NStrips/NTimes<<" strips, \n"
      <<StripSizeB/k<<"kB for "<<NStripsB/NTimes<<" stripsB, \n"
      <<DupSize/k<<"kB for "<<NDup/NTimes<<" doublets, \n"
      <<TripSize/k<<"kB for "<<NTrip/NTimes<<" triplets\n"
      <<BranchSize/k<<"kB for "<<NBranches/NTimes<<" branches, \n"
      <<TrackSize/k<<"kB for "<<NTracks/NTimes<<" tracks. "<<endl;
  cout<<" L1 total event size = "
      <<( HitSize + StripSize +  StripSizeB + DupSize + TripSize + BranchSize + TrackSize )/k
      <<" Kb"<<endl;
#endif // 0
  //cout << "===> ... CA Track Finder    " << endl << endl;
#endif

#ifdef DRAW
  draw.ClearVeiw();
  draw.DrawInputHits();
//   draw.DrawInfo();
  draw.DrawRecoTracks();
  draw.SaveCanvas("Reco_");
  draw.DrawAsk();
#endif 
#ifdef PULLS
  static int iEvee = 0;
  iEvee++;
  if (iEvee%1 == 0)
    fL1Pulls->Build(1);
#endif
#ifdef DOUB_PERFORMANCE
  fL1Eff_doublets->CalculateEff();
  fL1Eff_doublets->Print("Doublets performance.",1);
#endif
#ifdef TRIP_PERFORMANCE
  fL1Eff_triplets->CalculateEff();
  fL1Eff_triplets->Print("Triplet performance",1);
//   fL1Eff_triplets2->CalculateEff();
//   fL1Eff_triplets2->Print("Triplet performance. After chi2 cut");
#endif

}


   /** *************************************************************
   *                                                              *
   *     The routine performs recursive search for tracks         *
   *                                                              *
   *     I. Kisel                                    06.03.05     *
   *     I.Kulakov                                    2012        *
   *                                                              *
   ****************************************************************/
  
void L1Algo::CAFindTrack(int ista,
                         L1Branch &best_tr, unsigned char &best_L, fscal &best_chi2,
                         const L1Triplet* curr_trip,
                         L1Branch &curr_tr, unsigned char &curr_L, fscal &curr_chi2,
                         int &NCalls )
						  /// recursive search for tracks
						 /// input: @ista - station index, @&best_tr - best track for the privious call, @&best_L -
                         /// output: @&NCalls - number of function calls 						 
{
  if (curr_trip->GetLevel() == 0){ // the end of the track -> check and store
    
      // -- finish with current track
      // add rest of hits
    THitI ihitm = curr_trip->GetMHit();
    THitI ihitr = curr_trip->GetRHit();
    if( !GetFUsed( vSFlag[(*vStsHitsUnused)[ihitm].f] | vSFlagB[(*vStsHitsUnused)[ihitm].b] )  ){
      curr_tr.StsHits.push_back(RealIHit[ihitm]);
      curr_L++;
    }
    if( !GetFUsed( vSFlag[(*vStsHitsUnused)[ihitr].f] | vSFlagB[(*vStsHitsUnused)[ihitr].b] ) ){
      curr_tr.StsHits.push_back(RealIHit[ihitr]);
      curr_L++;
    }
    if( curr_chi2 > TRACK_CHI2_CUT * (curr_L*2-5) ) return;

//      // try to find more hits
// #ifdef EXTEND_TRACKS
// //    if ( isec == kFastPrimJumpIter || isec == kAllPrimJumpIter || isec == kAllSecJumpIter )
//     if (curr_L >= 3){
//       //curr_chi2 = BranchExtender(curr_tr);
//       BranchExtender(curr_tr);
//       curr_L = curr_tr.StsHits.size();
//         //      if( 2*curr_chi2 > (2*(curr_L*2-5) + 1) * 4*4 ) return;
//     }
// #endif // EXTEND_TRACKS
    
      // -- select the best
    if ( (curr_L > best_L ) || ( (curr_L == best_L) && (curr_chi2 < best_chi2) ) ){
      best_tr   = curr_tr;
      best_chi2 = curr_chi2;
      best_L    = curr_L;
    }
    
    return;
  }


    // try to extend. try all possible triplets
  int offset = TripStartIndex[curr_trip->GetMSta()];
  int NNeighs = curr_trip->neighbours.size();
  for (int in = 0; in < NNeighs; in++) {
    int index = curr_trip->neighbours[in] + offset;
    L1Triplet *new_trip = &vTriplets[index];

      // check new triplet
    const fscal qp1 = curr_trip->GetQp();
    const fscal qp2 = new_trip->GetQp();
    fscal dqp = fabs(qp1 - qp2);
    fscal Cqp = curr_trip->Cqp;
    Cqp      += new_trip->Cqp;
    if ( dqp > PickNeighbour * Cqp  ) continue; // bad neighbour // CHECKME why do we need recheck it?? (it really change result)

    if ( GetFUsed( vSFlag[(*vStsHitsUnused)[new_trip->GetLHit()].f] | vSFlagB[(*vStsHitsUnused)[new_trip->GetLHit()].b] ) ){ 
        //  no used hits allowed -> compare and store track
      if ( ( curr_L > best_L ) || ( (curr_L == best_L) && (curr_chi2 < best_chi2) ) ){
        best_tr   = curr_tr;
        best_chi2 = curr_chi2;
        best_L    = curr_L;
      }
    }
    else{ //  add new triplet to the current track

          // restore current track
        // save current tracklet
      L1Branch new_tr = curr_tr;
      unsigned char new_L = curr_L;
      fscal new_chi2 = curr_chi2;

        // add new hit
      new_tr.StsHits.push_back(RealIHit[new_trip->GetLHit()]);
      new_L += 1;
      dqp = dqp/Cqp*5.;  // CHECKME: understand 5, why no sqrt(5)?
      new_chi2 += dqp*dqp;
  //   dqp = dqp/Cqp; // IKu
  //   new_chi2 += dqp*dqp*5.;
      
      if( new_chi2 > TRACK_CHI2_CUT * new_L ) continue;

      const int new_ista = ista + new_trip->GetMSta() - new_trip->GetLSta();
      CAFindTrack(new_ista, best_tr, best_L, best_chi2, new_trip, new_tr, new_L, new_chi2, NCalls);
      NCalls++;
    } // add triplet to track
  } // for neighbours

  return;
}
