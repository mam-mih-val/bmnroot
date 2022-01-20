/*
 *====================================================================
 *
 *  SIMD Kalman filter(KF) track fitter
 *
 *  Authors: M.Zyzak, I.Kulakov
 *  Primary authors: I.Kisel, S.Gorbunov
 *
 *  e-mail : I.Kisel@gsi.de, M.Zyzak@gsi.de
 *
 *====================================================================
 */



#include "L1Algo.h"
#include "L1TrackPar.h"
#include "L1Extrapolation.h"
#include "L1AddMaterial.h"
#include "L1Filtration.h" // for KFTrackFitter_simple

#include <iostream>
#include <vector>

using std::cout;
using std::endl;
using std::vector;


namespace NS_L1TrackFitter{
  const fvec c_light = 0.000299792458, c_light_i = 1./c_light;
  const fvec ZERO = 0.;
  const fvec ONE = 1.;
  const fvec vINF = 0.1;
}
using namespace NS_L1TrackFitter;

  /// Fit reconstracted track like it fitted during the reconstruction.
void L1Algo::KFTrackFitter_simple()  // TODO: Add pipe.
{
//  cout << " Start KF Track Fitter " << endl;
  int start_hit = 0; // for interation in vRecoHits[]

  for(unsigned int itrack = 0; itrack < vTracks.size(); itrack++)
  {
    L1Track &t =  vTracks[itrack]; // current track

    // get hits of current track
    std::vector<unsigned short int> hits; // array of indeses of hits of current track
    hits.clear();
    int nHits = t.NHits;
    for( int i = 0; i < nHits; i++ ){
      hits.push_back( vRecoHits[start_hit++]);
    }

    L1TrackPar T; // fitting parametr coresponding to current track

    fvec qp0 = 0;
    //fvec qp0 = 2./t.Momentum;
    for(int iter=0; iter<3; iter++ )
    {
      //cout<<" Back 1"<<endl;
      { // fit backward
        L1StsHit &hit0 = vStsHits[hits[nHits-1]];
        L1StsHit &hit1 = vStsHits[hits[nHits-2]];
        L1StsHit &hit2 = vStsHits[hits[nHits-3]];

        int ista0 = vSFlag[hit0.f]/4;
        int ista1 = vSFlag[hit1.f]/4;
        int ista2 = vSFlag[hit2.f]/4;

      //cout<<"back: ista012="<<ista0<<" "<<ista1<<" "<<ista2<<endl;
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
        if( iter==0 ){
          T.tx = (x1-x0)*dzi;
          T.ty = (y1-y0)*dzi;
        }

        T.qp = qp0;
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

//        static L1FieldValue fB0, fB1, fB2 _fvecalignment;
//        static L1FieldRegion fld _fvecalignment;
        L1FieldValue fB0, fB1, fB2 _fvecalignment;
        L1FieldRegion fld _fvecalignment;
        fvec fz0 = sta1.z; // suppose field is smoth
        fvec fz1 = sta2.z;
        fvec fz2 = sta0.z;


        sta1.fieldSlice.GetFieldValue( x1, y1, fB0 );
        sta2.fieldSlice.GetFieldValue( x2, y2, fB1 );
        sta0.fieldSlice.GetFieldValue( x0, y0, fB2 );

        fld.Set( fB2, fz2, fB1, fz1, fB0, fz0 );

        int ista = ista2;
      //cout<<"\nfit, iter=:"<<iter<<endl;
        for( int i = nHits-2; i >= 0; i--){
  //  if( fabs(T.qp[0])>2. ) break;  // iklm. Don't know it need for
          L1StsHit &hit = vStsHits[hits[i]];
          ista = vSFlag[hit.f]/4;

          L1Station &sta = vStations[ista];
          
          L1Extrapolate( T, vStsZPos[hit.iz], qp0, fld );
//           T.L1Extrapolate( sta.z, qp0, fld );
//         L1Extrapolate( T, hit.z, qp0, fld );
          L1AddMaterial( T, sta.materialInfo, qp0 );
//         if (ista==NMvdStations-1) L1AddPipeMaterial( T, qp0);

          fvec u = static_cast<fscal>( vStsStrips[hit.f] );
          fvec v = static_cast<fscal>( vStsStripsB[hit.b] );
          L1Filter( T, sta.frontInfo, u );
          L1Filter( T, sta.backInfo,  v );
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

      // write received parametres in track
        t.TFirst[0] = T.x[0];
        t.TFirst[1] = T.y[0];
        t.TFirst[2] = T.tx[0];
        t.TFirst[3] = T.ty[0];
        t.TFirst[4] = T.qp[0];
        t.TFirst[5] = T.z[0];

        t.CFirst[0] = T.C00[0];
        t.CFirst[1] = T.C10[0];
        t.CFirst[2] = T.C11[0];
        t.CFirst[3] = T.C20[0];
        t.CFirst[4] = T.C21[0];
        t.CFirst[5] = T.C22[0];
        t.CFirst[6] = T.C30[0];
        t.CFirst[7] = T.C31[0];
        t.CFirst[8] = T.C32[0];
        t.CFirst[9] = T.C33[0];
        t.CFirst[10] = T.C40[0];
        t.CFirst[11] = T.C41[0];
        t.CFirst[12] = T.C42[0];
        t.CFirst[13] = T.C43[0];
        t.CFirst[14] = T.C44[0];

        t.chi2 = T.chi2[0];
        t.NDF = static_cast<int>( T.NDF[0] );
        qp0 = T.qp[0];
      } // fit backward

      // fit forward
      {
  //T.qp = first_trip->GetQpOrig(MaxInvMom);

        L1StsHit &hit0 = vStsHits[hits[0]];
        L1StsHit &hit1 = vStsHits[hits[1]];
        L1StsHit &hit2 = vStsHits[hits[2]];

        int ista0 = GetFStation( vSFlag[hit0.f] );
        int ista1 = GetFStation( vSFlag[hit1.f] );
        int ista2 = GetFStation( vSFlag[hit2.f] );

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
        //fvec z1 = vStsZPos[hit1.iz];

        fvec u2  = static_cast<fscal>( vStsStrips[hit2.f] );
        fvec v2  = static_cast<fscal>( vStsStripsB[hit2.b] );
        fvec x2,y2;
        StripsToCoor(u2, v2, x2, y2, sta2);
        //fvec z2 = vStsZPos[hit2.iz];

        //fvec dzi = 1./(z1-z0);

  //fvec qp0 = first_trip->GetQpOrig(MaxInvMom);

        const fvec vINF = .1;
        T.chi2 = 0.;
        T.NDF = 2.;
        T.x  = x0;
        T.y  = y0;
//         T.tx = (x1-x0)*dzi;
//         T.ty = (y1-y0)*dzi;
//         qp0 = 0;
        T.qp = qp0;
        T.z  = z0;
        T.C00 = sta0.XYInfo.C00;
        T.C10 = sta0.XYInfo.C10;
        T.C11 = sta0.XYInfo.C11;
        T.C20 = T.C21 = 0;
        T.C30 = T.C31 = T.C32 = 0;
        T.C40 = T.C41 = T.C42 = T.C43 = 0;
        T.C22 = T.C33 = vINF;
        T.C44 = 1.;

//        static L1FieldValue fB0, fB1, fB2 _fvecalignment;
//        static L1FieldRegion fld _fvecalignment;
        L1FieldValue fB0, fB1, fB2 _fvecalignment;
        L1FieldRegion fld _fvecalignment;
        fvec fz0 = sta1.z;
        fvec fz1 = sta2.z;
        fvec fz2 = sta0.z;

        sta1.fieldSlice.GetFieldValue( x1, y1, fB0 );
        sta2.fieldSlice.GetFieldValue( x2, y2, fB1 );
        sta0.fieldSlice.GetFieldValue( x0, y0, fB2 );

        fld.Set( fB2, fz2, fB1, fz1, fB0, fz0 );
        int ista = ista2;

        for( int i=1; i<nHits; i++){
          L1StsHit &hit = vStsHits[hits[i]];
          ista = vSFlag[hit.f]/4;
          L1Station &sta = vStations[ista];
          fvec u = static_cast<fscal>( vStsStrips[hit.f] );
          fvec v = static_cast<fscal>( vStsStripsB[hit.b] );

          L1Extrapolate( T, vStsZPos[hit.iz], qp0, fld );
//           T.L1Extrapolate( sta.z, qp0, fld );
//           L1Extrapolate( T, hit.z, qp0, fld );
          L1AddMaterial( T, sta.materialInfo, qp0 );
//           if (ista==NMvdStations) L1AddPipeMaterial( T, qp0);
          L1Filter( T, sta.frontInfo, u );
          L1Filter( T, sta.backInfo,  v );

          fB0 = fB1;
          fB1 = fB2;
          fz0 = fz1;
          fz1 = fz2;
          fvec x,y;
          StripsToCoor(u, v, x, y, sta);
          sta.fieldSlice.GetFieldValue( x, y, fB2 );
          fz2 = sta.z;
          fld.Set( fB2, fz2, fB1, fz1, fB0, fz0 );
        }

  // write received parametres in track
        t.TLast[0] = T.x[0];
        t.TLast[1] = T.y[0];
        t.TLast[2] = T.tx[0];
        t.TLast[3] = T.ty[0];
        t.TLast[4] = T.qp[0];
        t.TLast[5] = T.z[0];

        t.CLast[0] = T.C00[0];
        t.CLast[1] = T.C10[0];
        t.CLast[2] = T.C11[0];
        t.CLast[3] = T.C20[0];
        t.CLast[4] = T.C21[0];
        t.CLast[5] = T.C22[0];
        t.CLast[6] = T.C30[0];
        t.CLast[7] = T.C31[0];
        t.CLast[8] = T.C32[0];
        t.CLast[9] = T.C33[0];
        t.CLast[10] = T.C40[0];
        t.CLast[11] = T.C41[0];
        t.CLast[12] = T.C42[0];
        t.CLast[13] = T.C43[0];
        t.CLast[14] = T.C44[0];
 
        t.chi2 += T.chi2[0];
        t.NDF += static_cast<int>( T.NDF[0] );
      }
      qp0 = T.qp[0];
    }
  } // for(int itrack
}

void L1Algo::L1KFTrackFitter( bool extrapolateToTheEndOfSTS )
{
//  cout << " Start L1 Track Fitter " << endl;
  int start_hit = 0; // for interation in vRecoHits[]

//  static L1FieldValue fB0, fB1, fB2 _fvecalignment;
//  static L1FieldRegion fld _fvecalignment;
  L1FieldValue fB0, fB1, fB2 _fvecalignment;
  L1FieldRegion fld _fvecalignment;

  const int nHits = NStations;
  int iVec=0, i=0;
  int nTracks_SIMD = fvecLen;
  L1TrackPar T; // fitting parametr coresponding to current track

  L1Track *t[fvecLen];

  L1Station *sta = vStations;
  fvec x[MaxNStations], u[MaxNStations], v[MaxNStations], y[MaxNStations], z[MaxNStations];
  fvec du[MaxNStations], dv[MaxNStations]; //AZ
  fvec x_first, y_first, x_last, y_last; 
  fvec Sy[MaxNStations], w[MaxNStations];
  fvec y_temp, x_temp;
  fvec fz0, fz1, fz2, dz, z_start, z_end;
  L1FieldValue fB[MaxNStations], fB_temp _fvecalignment;

  fvec ZSta[MaxNStations];
  for(int iHit = 0; iHit<nHits; iHit++)
  {
    ZSta[iHit] = sta[iHit].z;
  }

  unsigned short N_vTracks = vTracks.size();

  for(unsigned short itrack = 0; itrack < N_vTracks; itrack+=fvecLen)
  {
    if(N_vTracks - itrack < static_cast<unsigned short>(fvecLen))
      nTracks_SIMD = N_vTracks - itrack;

    for(i=0; i<nTracks_SIMD; i++)
      t[i] = & vTracks[itrack+i]; // current track

    // get hits of current track
    for(i=0; i<nHits; i++)
    {
      w[i] = ZERO;
      z[i] = ZSta[i];
    }
    
    for(iVec=0; iVec<nTracks_SIMD; iVec++)
    {
      int nHitsTrack = t[iVec]->NHits;
      //cout << "AZ: hits = " << nHitsTrack << endl;
      int iSta[MaxNStations];
      for(i = 0; i < nHitsTrack; i++ )
      {
        L1StsHit &hit = vStsHits[vRecoHits[start_hit++]];
        const int ista = vSFlag[hit.f]/4;
        iSta[i] = ista;
        w[ista][iVec] = 1.;

        u[ista][iVec]  = vStsStrips[hit.f] ;
        v[ista][iVec]  = vStsStripsB[hit.b];
        du[ista][iVec]  = vStsStrips[hit.f].errf() * vStsStrips[hit.f].errf(); //AZ
        dv[ista][iVec]  = vStsStripsB[hit.b].errf() * vStsStripsB[hit.b].errf(); //AZ
        StripsToCoor(u[ista], v[ista], x_temp, y_temp, sta[ista]);
        x[ista][iVec]  = x_temp[iVec];
        y[ista][iVec]  = y_temp[iVec];
        z[ista][iVec]  = vStsZPos[hit.iz];
        sta[ista].fieldSlice.GetFieldValue( x[ista], y[ista], fB_temp );
        fB[ista].x[iVec] = fB_temp.x[iVec];
        fB[ista].y[iVec] = fB_temp.y[iVec];
        fB[ista].z[iVec] = fB_temp.z[iVec];
        if(i == 0) {
          z_start[iVec] = z[ista][iVec];
          x_first[iVec] = x[ista][iVec];
          y_first[iVec] = y[ista][iVec];
        }
        else if(i == nHitsTrack-1) {
          z_end[iVec] = z[ista][iVec];
          x_last[iVec] = x[ista][iVec];
          y_last[iVec] = y[ista][iVec];
        }
      }
      
        // get field integrals
#if 0
      for(i = nHits - 1; i >= 0; i-- ){
        L1Station &st = vStations[i];
        Sy[i] = st.Sy;
      }
#else
      fscal prevZ = z_end[iVec];
      fscal cursy = 0., curSy = 0.;
      for(i = nHitsTrack - 1; i >= 0; i-- ){
        const int ista = iSta[i];
        L1Station &st = vStations[ista];
        const fscal curZ = z[ista][iVec];
        fscal dZ = curZ - prevZ;
        fscal By = st.fieldSlice.cy[0][0];
        curSy += dZ*cursy + dZ*dZ*By/2.;
        cursy += dZ*By;
        Sy[ista][iVec] = curSy;
        prevZ = curZ;
      }
#endif // 0
    }

//fit backward

    GuessVec( T, x, y, z, Sy, w, nHits, &z_end);

    for( int iter = 0; iter < 2; iter++  ) { // 1.5 iterations
      
      fvec qp0 = T.qp;

      i = nHits-1;

      FilterFirst( T, x_last, y_last, sta[i] );

      // L1AddMaterial( T, sta[i].materialInfo, qp0 );

      fz1 = z[i];
      sta[i].fieldSlice.GetFieldValue( T.x, T.y, fB1 );
      fB1.Combine( fB[i], w[i] );

      fz2 = z[i-2];
      dz = fz2-fz1;
      sta[i].fieldSlice.GetFieldValue( T.x + T.tx*dz, T.y + T.ty*dz, fB2 );
      fB2.Combine( fB[i-2], w[i-2] );
      fld.Set( fB2, fz2, fB1, fz1, fB0, fz0 );
      for( --i; i>=0; i-- )
      {
        fz0 = z[i];
        dz = (fz1-fz0);
        sta[i].fieldSlice.GetFieldValue( T.x - T.tx*dz, T.y - T.ty*dz, fB0 );
        fB0.Combine( fB[i], w[i] );
        fld.Set( fB0, fz0, fB1, fz1, fB2, fz2 );

        //fvec zero = ZERO;
        fvec initialised = fvec(z[i] < z_end) & fvec(z_start <= z[i]);
          // cout << z_start << " " << z_end << " " << initialised << endl;
        fvec w1 = (w[i] & (initialised));
        fvec wIn = (ONE & (initialised));
            
        L1Extrapolate( T, z[i], qp0, fld, &w1 );
        if(i == NMvdStations - 1) L1AddPipeMaterial( T, qp0, wIn );
#ifdef USE_RL_TABLE
        L1AddMaterial( T, fRadThick[i].GetRadThick(T.x, T.y), qp0, wIn );
#else
        L1AddMaterial( T, sta[i].materialInfo, qp0, wIn );
#endif
        //L1Filter( T, sta[i].frontInfo, u[i], w1 );
        //L1Filter( T, sta[i].backInfo,  v[i], w1 );
        L1Filter( T, sta[i].frontInfo, u[i], w1, &du[i]); //AZ
        L1Filter( T, sta[i].backInfo,  v[i], w1, &dv[i]); //AZ
        fB2 = fB1; 
        fz2 = fz1;
        fB1 = fB0; 
        fz1 = fz0;
      }
      // L1AddHalfMaterial( T, sta[i].materialInfo, qp0 );

      for(iVec=0; iVec<nTracks_SIMD; iVec++)
      {
        t[iVec]->TFirst[0] = T.x[iVec];
        t[iVec]->TFirst[1] = T.y[iVec];
        t[iVec]->TFirst[2] = T.tx[iVec];
        t[iVec]->TFirst[3] = T.ty[iVec];
        t[iVec]->TFirst[4] = T.qp[iVec];
        t[iVec]->TFirst[5] = T.z[iVec];
      
        t[iVec]->CFirst[0] = T.C00[iVec];
        t[iVec]->CFirst[1] = T.C10[iVec];
        t[iVec]->CFirst[2] = T.C11[iVec];
        t[iVec]->CFirst[3] = T.C20[iVec];
        t[iVec]->CFirst[4] = T.C21[iVec];
        t[iVec]->CFirst[5] = T.C22[iVec];
        t[iVec]->CFirst[6] = T.C30[iVec];
        t[iVec]->CFirst[7] = T.C31[iVec];
        t[iVec]->CFirst[8] = T.C32[iVec];
        t[iVec]->CFirst[9] = T.C33[iVec];
        t[iVec]->CFirst[10] = T.C40[iVec];
        t[iVec]->CFirst[11] = T.C41[iVec];
        t[iVec]->CFirst[12] = T.C42[iVec];
        t[iVec]->CFirst[13] = T.C43[iVec];
        t[iVec]->CFirst[14] = T.C44[iVec];

        t[iVec]->chi2 = T.chi2[iVec];
        t[iVec]->NDF = (int)T.NDF[iVec];
      }

      if ( iter == 1 ) continue; // only 1.5 iterations
        // fit forward

      i = 0;
      FilterFirst( T, x_first, y_first, sta[i] ); // TODO different station. won't work with MVD
      // L1AddMaterial( T, sta[i].materialInfo, qp0 );
      qp0 = T.qp;

      fz1 = z[i];
      sta[i].fieldSlice.GetFieldValue( T.x, T.y, fB1 );
      fB1.Combine( fB[i], w[i] );

      fz2 = z[i+2];
      dz = fz2-fz1;
      sta[i].fieldSlice.GetFieldValue( T.x + T.tx*dz, T.y + T.ty*dz, fB2 );
      fB2.Combine( fB[i+2], w[i+2] );
      fld.Set( fB2, fz2, fB1, fz1, fB0, fz0 );

      for( ++i; i<nHits; i++ )
      {
        fz0 = z[i];
        dz = (fz1-fz0);
        sta[i].fieldSlice.GetFieldValue( T.x - T.tx*dz, T.y - T.ty*dz, fB0 );
        fB0.Combine( fB[i], w[i] );
        fld.Set( fB0, fz0, fB1, fz1, fB2, fz2 );

        //fvec zero = ZERO;
        fvec initialised = fvec(z[i] <= z_end) & fvec(z_start < z[i]);
        fvec w1  = (w[i] & (initialised));
        fvec wIn = (ONE  & (initialised));
            
        L1Extrapolate( T, z[i], qp0, fld,&w1 );
        if(i == NMvdStations) L1AddPipeMaterial( T, qp0, wIn );
#ifdef USE_RL_TABLE
        L1AddMaterial( T, fRadThick[i].GetRadThick(T.x, T.y), qp0, wIn );
#else
        L1AddMaterial( T, sta[i].materialInfo, qp0, wIn );
#endif
        //L1Filter( T, sta[i].frontInfo, u[i], w1 );
        //L1Filter( T, sta[i].backInfo,  v[i], w1 );
        L1Filter( T, sta[i].frontInfo, u[i], w1, &du[i]); //AZ
        L1Filter( T, sta[i].backInfo,  v[i], w1, &dv[i]); //AZ

        fB2 = fB1; 
        fz2 = fz1;
        fB1 = fB0; 
        fz1 = fz0;
      }
      // L1AddHalfMaterial( T, sta[i].materialInfo, qp0 );

      { // extrapolate to 1 m
        L1TrackPar Tout = T;
        if ( extrapolateToTheEndOfSTS ) {
            // extrapolate to the last station
          i = 0;
          fz1 = z[i];
          sta[i].fieldSlice.GetFieldValue( T.x, Tout.y, fB1 );
          fB1.Combine( fB[i], w[i] );

          fz2 = z[i+2];
          dz = fz2-Tout.z;
          sta[i].fieldSlice.GetFieldValue( Tout.x + Tout.tx*dz, Tout.y + Tout.ty*dz, fB2 );
          fB2.Combine( fB[i+2], w[i+2] );
          fld.Set( fB2, fz2, fB1, fz1, fB0, fz0 );

          for( ++i; i<NStations; i++ )
          {
            fz0 = z[i];
            dz = (Tout.z-fz0);
            sta[i].fieldSlice.GetFieldValue( Tout.x - Tout.tx*dz, Tout.y - Tout.ty*dz, fB0 );
            fB0.Combine( fB[i], w[i] );
            fld.Set( fB0, fz0, fB1, fz1, fB2, fz2 );

            //fvec zero = ZERO;
            fvec initialised = fvec(z[i] > z_end);
            fvec wIn = (ONE  & (initialised));
            
            L1Extrapolate( Tout, z[i], qp0, fld,&wIn );
            if(i == NMvdStations) L1AddPipeMaterial( Tout, qp0, wIn );
#ifdef USE_RL_TABLE
            L1AddMaterial( Tout, fRadThick[i].GetRadThick(Tout.x, Tout.y), qp0, wIn );
#else
            L1AddMaterial( Tout, sta[i].materialInfo, qp0, wIn );
#endif

            fB2 = fB1; 
            fz2 = fz1;
            fB1 = fB0; 
            fz1 = fz0;
          }
            // extrapolate to 1m
          {
            const fvec zFinal = 100.f;
            i = NStations - 1;
            fvec initialised = fvec(zFinal > Tout.z); // 3cm safe distance, needed because of diff position of sensors
            L1Extrapolate( Tout, zFinal, qp0, fld, &initialised ); // extra with old field
          }
          
        }
        for(iVec=0; iVec<nTracks_SIMD; iVec++)
        {
          t[iVec]->TLast[0] = Tout.x[iVec];
          t[iVec]->TLast[1] = Tout.y[iVec];
          t[iVec]->TLast[2] = Tout.tx[iVec];
          t[iVec]->TLast[3] = Tout.ty[iVec];
          t[iVec]->TLast[4] = Tout.qp[iVec];
          t[iVec]->TLast[5] = Tout.z[iVec];
      
          t[iVec]->CLast[0] = Tout.C00[iVec];
          t[iVec]->CLast[1] = Tout.C10[iVec];
          t[iVec]->CLast[2] = Tout.C11[iVec];
          t[iVec]->CLast[3] = Tout.C20[iVec];
          t[iVec]->CLast[4] = Tout.C21[iVec];
          t[iVec]->CLast[5] = Tout.C22[iVec];
          t[iVec]->CLast[6] = Tout.C30[iVec];
          t[iVec]->CLast[7] = Tout.C31[iVec];
          t[iVec]->CLast[8] = Tout.C32[iVec];
          t[iVec]->CLast[9] = Tout.C33[iVec];
          t[iVec]->CLast[10] = Tout.C40[iVec];
          t[iVec]->CLast[11] = Tout.C41[iVec];
          t[iVec]->CLast[12] = Tout.C42[iVec];
          t[iVec]->CLast[13] = Tout.C43[iVec];
          t[iVec]->CLast[14] = Tout.C44[iVec];

          t[iVec]->chi2 = Tout.chi2[iVec];
          t[iVec]->NDF = (int)Tout.NDF[iVec];
        }
      }
    } // iter
  }
}

void L1Algo::GuessVec( L1TrackPar &t, fvec *xV, fvec *yV, fvec *zV, fvec *Sy, fvec *wV, int NHits, fvec *zCur )
  // gives nice initial approximation for x,y,tx,ty - almost same as KF fit. qp - is shifted by 4%, residual - ~3.5% (KF fit residual - 1%).
{
  fvec A0, A1=ZERO, A2=ZERO, A3=ZERO, A4=ZERO, A5=ZERO, a0, a1=ZERO, a2=ZERO,
  b0, b1=ZERO, b2=ZERO;
  fvec z0, x, y, z, S, w, wz, wS;

  int i=NHits-1;
  if(zCur)
    z0 = *zCur;
  else
    z0 = zV[i];
  w = wV[i];
  A0 = w;
  a0 = w*xV[i];
  b0 = w*yV[i];
  for( i=0 ; i<NHits; i++ ){
    x = xV[i];
    y = yV[i];
    w = wV[i];
    z = zV[i] - z0;
    S = Sy[i];
    wz = w*z;
    wS = w*S;
    A0+=w;
    A1+=wz;  A2+=wz*z;
    A3+=wS;  A4+=wS*z; A5+=wS*S;
    a0+=w*x; a1+=wz*x; a2+=wS*x;
    b0+=w*y; b1+=wz*y; b2+=wS*y;
  }

  fvec A3A3 = A3*A3;
  fvec A3A4 = A3*A4;
  fvec A1A5 = A1*A5;
  fvec A2A5 = A2*A5;
  fvec A4A4 = A4*A4;

  fvec det = rcp(-A2*A3A3 + A1*( A3A4+A3A4 - A1A5) + A0*(A2A5-A4A4));
  fvec Ai0 = ( -A4A4 + A2A5 );
  fvec Ai1 = (  A3A4 - A1A5 );
  fvec Ai2 = ( -A3A3 + A0*A5 );
  fvec Ai3 = ( -A2*A3 + A1*A4 );
  fvec Ai4 = (  A1*A3 - A0*A4 );
  fvec Ai5 = ( -A1*A1 + A0*A2 );

  fvec L, L1;
  t.x = (Ai0*a0 + Ai1*a1 + Ai3*a2)*det;
  t.tx = (Ai1*a0 + Ai2*a1 + Ai4*a2)*det;
  fvec txtx1 = 1.+t.tx*t.tx;
  L    = (Ai3*a0 + Ai4*a1 + Ai5*a2)*det *rcp(txtx1);
  L1 = L*t.tx;
  A1 = A1 + A3*L1;
  A2 = A2 + ( A4 + A4 + A5*L1 )*L1;
  b1+= b2 * L1;
  det = rcp(-A1*A1+A0*A2);

  t.y = (  A2*b0 - A1*b1 )*det;
  t.ty = ( -A1*b0 + A0*b1 )*det;
  t.qp = -L*c_light_i*rsqrt(txtx1 +t.ty*t.ty);
  t.z = z0;
}

void L1Algo::FilterFirst( L1TrackPar &track,fvec &x, fvec &y, L1Station &st )
{
  // initialize covariance matrix
  track.C00= st.XYInfo.C00;
  track.C10= st.XYInfo.C10;      track.C11= st.XYInfo.C11;
  track.C20= ZERO;         track.C21= ZERO;      track.C22= vINF;
  track.C30= ZERO;         track.C31= ZERO;      track.C32= ZERO;    track.C33= vINF;
  track.C40= ZERO;         track.C41= ZERO;      track.C42= ZERO;    track.C43= ZERO;     track.C44= ONE;

  track.x = x;
  track.y = y;
  track.NDF = -3.0;
  track.chi2 = ZERO;
}
