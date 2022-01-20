/*
 *=====================================================
 *
 *  CBM Level 1 Reconstruction
 *
 *  Authors: M.Zyzak
 *
 *  e-mail :
 *
 *=====================================================
 *
 *  SIMD Fitter for CbmL1Track class
 *
 */

#include "CbmL1PFFitter.h"

#include "CbmL1.h"
#include "TClonesArray.h"
#include "CbmMvdHit.h"
#include "CbmStsHit.h"
#include "CbmStsTrack.h"

//L1Algo tools
#include "L1Algo.h"
#include "CbmL1Track.h"
#include "L1TrackPar.h"
#include "L1Station.h"
#include "L1Extrapolation.h"
#include "L1MaterialInfo.h"

#include "FairRootManager.h"
#include "TDatabasePDG.h"

#include "CbmKFVertex.h"
#include "CbmKFParticleDatabase.h"

using std::vector;

namespace NS_L1TrackFitter{
  const fvec c_light = 0.000299792458, c_light_i = 1./c_light;
  const fvec ZERO = 0.;
  const fvec ONE = 1.;
  const fvec vINF = 0.1;
}
using namespace NS_L1TrackFitter;
 
CbmL1PFFitter::CbmL1PFFitter()
{
}

CbmL1PFFitter::~CbmL1PFFitter()
{
}

inline void CbmL1PFFitter::AddMaterial( L1TrackPar &T, fvec radThick, fvec qp0, fvec &mass2, fvec &w)
{
  static const fvec ZERO = 0.0f, ONE = 1.;

  fvec tx = T.tx;
  fvec ty = T.ty;
  fvec txtx = tx*tx;
  fvec tyty = ty*ty;
  fvec txtx1 = txtx + ONE;
  fvec h = txtx + tyty;
  fvec t = sqrt(txtx1 + tyty);
  fvec h2 = h*h;
  fvec qp0t = qp0*t;
  
  static const fvec c1=0.0136f, c2=c1*0.038f, c3=c2*0.5f, c4=-c3/2.0f, c5=c3/3.0f, c6=-c3/4.0f;
    
  fvec s0 = (c1+c2*log(radThick) + c3*h + h2*(c4 + c5*h +c6*h2) )*qp0t;    
  fvec a = ( (ONE+mass2*qp0*qp0t)*radThick*s0*s0 );
// std::cout <<" a " << a << std::endl;
//  a=0.000005;
  fvec zero = ZERO;
  fvec init = fvec(w>zero);
  T.C22 += (init & (txtx1*a));
  T.C32 += (init & (tx*ty*a)); T.C33 += (init & ((ONE+tyty)*a)); 
}

inline void CbmL1PFFitter::AddPipeMaterial( L1TrackPar &T, fvec qp0, fvec &mass2 , fvec &w)
{
  static const fvec ZERO = 0.0f, ONE = 1.f;

  static const fscal RadThick=0.0009f;//0.5/18.76;
  static const fscal logRadThick=log(RadThick);
  fvec tx = T.tx;
  fvec ty = T.ty;
  fvec txtx = tx*tx;
  fvec tyty = ty*ty;
  fvec txtx1 = txtx + ONE;
  fvec h = txtx + tyty;
  fvec t = sqrt(txtx1 + tyty);
  fvec h2 = h*h;
  fvec qp0t = qp0*t;

  static const fvec c1=0.0136f, c2=c1*0.038f, c3=c2*0.5f, c4=-c3/2.0f, c5=c3/3.0f, c6=-c3/4.0f;
  fvec s0 = (c1+c2*(logRadThick) + c3*h + h2*(c4 + c5*h +c6*h2) )*qp0t;
  fvec a = ( (ONE+mass2*qp0*qp0t)*RadThick*s0*s0 );
  fvec zero = ZERO;
  fvec init = fvec(w>zero);
  T.C22 += (init & (txtx1*a));
  T.C32 += (init & (tx*ty*a)); T.C33 += (init & ((ONE+tyty)*a)); 
}

void CbmL1PFFitter::FilterFirst( L1TrackPar &track,fvec &x, fvec &y, fvec &w, L1Station &st )
{
  fvec w1 = ONE - w;
  fvec sigma21 = w*st.XYInfo.C00 + w1*vINF;
  fvec sigma22 = w*st.XYInfo.C10;
  fvec sigma23 = w*st.XYInfo.C11 + w1*vINF;
  // initialize covariance matrix
  track.C00= sigma21;
  track.C10= sigma22;      track.C11= sigma23;
/*  track.C00= vINF;
  track.C10= ZERO;      track.C11= vINF;*/
  track.C20= ZERO;         track.C21= ZERO;      track.C22= vINF;
  track.C30= ZERO;         track.C31= ZERO;      track.C32= ZERO;    track.C33= vINF;
  track.C40= ZERO;         track.C41= ZERO;      track.C42= ZERO;    track.C43= ZERO;     track.C44= vINF;

  track.x = w*x + w1*track.x;
  track.y = w*y + w1*track.y;
  track.NDF = -3.0;
  track.chi2 = ZERO;
}

void CbmL1PFFitter::FilterLast( L1TrackPar &track,fvec &x, fvec &y, fvec &w, L1Station &st )
{
  fvec w1 = ONE - w;
  fvec sigma21 = w*st.XYInfo.C00 + w1*vINF;
  fvec sigma22 = w*st.XYInfo.C10;
  fvec sigma23 = w*st.XYInfo.C11 + w1*vINF;
  // initialize covariance matrix
  track.C00= sigma21;
  track.C10= sigma22;      track.C11= sigma23;
  track.C20= ZERO;         track.C21= ZERO;      track.C22= vINF;
  track.C30= ZERO;         track.C31= ZERO;      track.C32= ZERO;    track.C33= vINF;
  track.C40= ZERO;         track.C41= ZERO;      track.C42= ZERO;    track.C43= ZERO;     track.C44= ONE;

  track.x = w*x + w1*track.x;
  track.y = w*y + w1*track.y;
  track.NDF = -3.0;
  track.chi2 = ZERO;
}


void CbmL1PFFitter::Filter( L1TrackPar &T, L1UMeasurementInfo &info, fvec &u , fvec &w)
{
  register fvec wi, zeta, zetawi, HCH;
  register fvec F0, F1, F2, F3, F4;
  register fvec K1, K2, K3, K4;

  zeta = info.cos_phi*T.x + info.sin_phi*T.y - u;

  // F = CH'
  F0 = info.cos_phi*T.C00 + info.sin_phi*T.C10;
  F1 = info.cos_phi*T.C10 + info.sin_phi*T.C11;

  HCH = ( F0*info.cos_phi + F1*info.sin_phi );

  F2 = info.cos_phi*T.C20 + info.sin_phi*T.C21;
  F3 = info.cos_phi*T.C30 + info.sin_phi*T.C31;
  F4 = info.cos_phi*T.C40 + info.sin_phi*T.C41;

  wi = 1./(info.sigma2 +HCH);

  wi *= w;

  zetawi = zeta *wi;
  T.chi2 +=  zeta * zetawi ;
  T.NDF  += w;

  K1 = F1*wi;
  K2 = F2*wi;
  K3 = F3*wi;
  K4 = F4*wi;

  T.x  -= F0*zetawi;
  T.y  -= F1*zetawi;
  T.tx -= F2*zetawi;
  T.ty -= F3*zetawi;
  T.qp -= F4*zetawi;

  T.C00-= F0*F0*wi;
  T.C10-= K1*F0;
  T.C11-= K1*F1;
  T.C20-= K2*F0;
  T.C21-= K2*F1;
  T.C22-= K2*F2;
  T.C30-= K3*F0;
  T.C31-= K3*F1;
  T.C32-= K3*F2;
  T.C33-= K3*F3;
  T.C40-= K4*F0;
  T.C41-= K4*F1;
  T.C42-= K4*F2;
  T.C43-= K4*F3;
  T.C44-= K4*F4;
}

void CbmL1PFFitter::Fit(vector<CbmL1Track> &Tracks, fvec mass)
{
//  cout << " Start L1 Track Fitter " << endl;
//  int start_hit = 0; // for interation in vRecoHits[]

  fvec mass2 = mass*mass;

  L1FieldValue fB0, fB1, fB2 _fvecalignment;
  L1FieldRegion fld _fvecalignment;

  TClonesArray *listStsHits = CbmL1::Instance()->listStsHits;
  TClonesArray *listMvdHits = CbmL1::Instance()->listMvdHits;
  int NMvdStations = CbmL1::Instance()->algo->NMvdStations;
  
  static int nHits = CbmL1::Instance()->algo->NStations;
  int iVec=0, i=0;
  int nTracks_SIMD = fvecLen;
  L1TrackPar T; // fitting parametr coresponding to current track

  CbmL1Track *t[fvecLen];

  int ista;
  L1Station *sta = CbmL1::Instance()->algo->vStations;
  fvec* x = new fvec[nHits];
  fvec* u = new fvec[nHits];
  fvec* v = new fvec[nHits];
  fvec* y = new fvec[nHits];
  fvec* z = new fvec[nHits];
  fvec* w = new fvec[nHits];
  fvec y_temp;
  fvec fz0, fz1, fz2, dz, z_start, z_end;
  L1FieldValue* fB = new L1FieldValue[nHits];
  L1FieldValue fB_temp _fvecalignment;

  unsigned short N_vTracks = Tracks.size();

  for(unsigned short itrack = 0; itrack < N_vTracks; itrack++)
  {
    Tracks[itrack].mass = mass[0];
  }

  for(unsigned short itrack = 0; itrack < N_vTracks; itrack+=fvecLen)
  {
    if(N_vTracks - itrack < static_cast<unsigned short>(fvecLen))
      nTracks_SIMD = N_vTracks - itrack;

    for(i=0; i<nTracks_SIMD; i++)
    {
      t[i] = & Tracks[itrack+i]; // current track
      T.x[i]  = t[i]->TLast[0];
      T.y[i]  = t[i]->TLast[1];
      T.tx[i] = t[i]->TLast[2];
      T.ty[i] = t[i]->TLast[3];
      T.qp[i] = t[i]->TLast[4];
      T.z[i]  = t[i]->TLast[5];
      T.C00[i] = t[i]->CLast[0];
      T.C10[i] = t[i]->CLast[1];
      T.C11[i] = t[i]->CLast[2];
      T.C20[i] = t[i]->CLast[3];
      T.C21[i] = t[i]->CLast[4];
      T.C22[i] = t[i]->CLast[5];
      T.C30[i] = t[i]->CLast[6];
      T.C31[i] = t[i]->CLast[7];
      T.C32[i] = t[i]->CLast[8];
      T.C33[i] = t[i]->CLast[9];
      T.C40[i] = t[i]->CLast[10];
      T.C41[i] = t[i]->CLast[11];
      T.C42[i] = t[i]->CLast[12];
      T.C43[i] = t[i]->CLast[13];
      T.C44[i] = t[i]->CLast[14];
    }

    // get hits of current track
    for(i=0; i<nHits; i++)
    {
      w[i] = ZERO;
      z[i] = sta[i].z;
    }

    for(iVec=0; iVec<nTracks_SIMD; iVec++)
    {
      int nHitsTrack = t[iVec]->GetNOfHits();
      for(i = 0; i < nHitsTrack; i++ )
      {
        float posx = 0.f, posy = 0.f, posz = 0.f;
        CbmL1HitStore &hs = CbmL1::Instance()->vHitStore[t[iVec]->StsHits[i]];
        if( hs.ExtIndex<0 ){
          CbmMvdHit *hit = L1_DYNAMIC_CAST<CbmMvdHit*>(listMvdHits->At(-hs.ExtIndex-1));
          posx = hit->GetX();
          posy = hit->GetY();
          posz = hit->GetZ();
          ista = posz < 7.f ? 0 : 1;
        }else{
          CbmStsHit *hit = L1_DYNAMIC_CAST<CbmStsHit*>(listStsHits->At(hs.ExtIndex));
          posx = hit->GetX();
          posy = hit->GetY();
          posz = hit->GetZ();
          ista = hit->GetStationNr() - 1 + NMvdStations;
        }
//std::cout << ista << std::endl;
        w[ista][iVec] = 1.f;

        x[ista][iVec] = posx;
        y[ista][iVec] = posy;
        u[ista][iVec] = posx*sta[ista].frontInfo.cos_phi[0] + posy*sta[ista].frontInfo.sin_phi[0];
        v[ista][iVec] = posx* sta[ista].backInfo.cos_phi[0] + posy*sta[ista].backInfo.sin_phi[0];
        z[ista][iVec] = posz;
        sta[ista].fieldSlice.GetFieldValue( x[ista], y[ista], fB_temp );
        fB[ista].x[iVec] = fB_temp.x[iVec];
        fB[ista].y[iVec] = fB_temp.y[iVec];
        fB[ista].z[iVec] = fB_temp.z[iVec];
        if(i == 0) z_start[iVec] = posz;
        if(i == nHitsTrack-1) z_end[iVec] = posz;
      }
    }

//fit backward

    fvec qp0 = T.qp;

    i= nHits-1;

    FilterFirst( T, x[i],y[i],w[i], sta[i] );
    AddMaterial( T, CbmL1::Instance()->algo->fRadThick[i].GetRadThick(T.x, T.y), qp0 , mass2, w[i]);

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

      //fvec one = ONE;
      fvec zero = ZERO;
      fvec initialised = fvec(z[i] > z_end) & fvec(z_start > z[i]);
      fvec w1 = (zero & initialised) + (w[i] & (!initialised));

      L1Extrapolate( T, z[i], qp0, fld, &w1 );
      if(i == NMvdStations - 1) AddPipeMaterial( T, qp0, mass2, w1);
      AddMaterial( T, CbmL1::Instance()->algo->fRadThick[i].GetRadThick(T.x, T.y), qp0, mass2, w1 );
      Filter( T, sta[i].frontInfo, u[i], w[i] );
      Filter( T, sta[i].backInfo,  v[i], w[i] );
      fB2 = fB1; 
      fz2 = fz1;
      fB1 = fB0; 
      fz1 = fz0;
    }

    for(iVec=0; iVec<nTracks_SIMD; iVec++)
    {
      t[iVec]->T[0] = T.x[iVec];
      t[iVec]->T[1] = T.y[iVec];
      t[iVec]->T[2] = T.tx[iVec];
      t[iVec]->T[3] = T.ty[iVec];
      t[iVec]->T[4] = T.qp[iVec];
      //t[iVec]->TFirst[5] = z_start[iVec];
      t[iVec]->T[5] = T.z[iVec];
      
      t[iVec]->C[0] = T.C00[iVec];
      t[iVec]->C[1] = T.C10[iVec];
      t[iVec]->C[2] = T.C11[iVec];
      t[iVec]->C[3] = T.C20[iVec];
      t[iVec]->C[4] = T.C21[iVec];
      t[iVec]->C[5] = T.C22[iVec];
      t[iVec]->C[6] = T.C30[iVec];
      t[iVec]->C[7] = T.C31[iVec];
      t[iVec]->C[8] = T.C32[iVec];
      t[iVec]->C[9] = T.C33[iVec];
      t[iVec]->C[10] = T.C40[iVec];
      t[iVec]->C[11] = T.C41[iVec];
      t[iVec]->C[12] = T.C42[iVec];
      t[iVec]->C[13] = T.C43[iVec];
      t[iVec]->C[14] = T.C44[iVec];

      t[iVec]->chi2 = T.chi2[iVec];
      t[iVec]->NDF = static_cast<int>(T.NDF[iVec]);
    }

    // fit forward

    i= 0;
    FilterLast( T, x[i],y[i],w[i], sta[i] );
    qp0 = T.qp;
    AddMaterial( T, CbmL1::Instance()->algo->fRadThick[i].GetRadThick(T.x, T.y), qp0, mass2, w[i] );

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

      //fvec one = ONE;
      fvec zero = ZERO;
      fvec initialised = fvec(z[i] > z_end) & fvec(z_start > z[i]);
      fvec w1 = (zero & initialised) + (w[i] & (!initialised));

      L1Extrapolate( T, z[i], qp0, fld,&w1 );
      if(i == NMvdStations ) AddPipeMaterial( T, qp0, mass2, w1 );
      AddMaterial( T, CbmL1::Instance()->algo->fRadThick[i].GetRadThick(T.x, T.y), qp0, mass2, w1 );
      Filter( T, sta[i].frontInfo, u[i], w[i] );
      Filter( T, sta[i].backInfo,  v[i], w[i] );

      fB2 = fB1; 
      fz2 = fz1;
      fB1 = fB0; 
      fz1 = fz0;
    }

    for(iVec=0; iVec<nTracks_SIMD; iVec++)
    {
      t[iVec]->TLast[0] = T.x[iVec];
      t[iVec]->TLast[1] = T.y[iVec];
      t[iVec]->TLast[2] = T.tx[iVec];
      t[iVec]->TLast[3] = T.ty[iVec];
      t[iVec]->TLast[4] = T.qp[iVec];
//      t[iVec]->TLast[5] = z_end[iVec];
      t[iVec]->TLast[5] = T.z[iVec];
      
      t[iVec]->CLast[0] = T.C00[iVec];
      t[iVec]->CLast[1] = T.C10[iVec];
      t[iVec]->CLast[2] = T.C11[iVec];
      t[iVec]->CLast[3] = T.C20[iVec];
      t[iVec]->CLast[4] = T.C21[iVec];
      t[iVec]->CLast[5] = T.C22[iVec];
      t[iVec]->CLast[6] = T.C30[iVec];
      t[iVec]->CLast[7] = T.C31[iVec];
      t[iVec]->CLast[8] = T.C32[iVec];
      t[iVec]->CLast[9] = T.C33[iVec];
      t[iVec]->CLast[10] = T.C40[iVec];
      t[iVec]->CLast[11] = T.C41[iVec];
      t[iVec]->CLast[12] = T.C42[iVec];
      t[iVec]->CLast[13] = T.C43[iVec];
      t[iVec]->CLast[14] = T.C44[iVec];

      t[iVec]->chi2 = T.chi2[iVec];
      t[iVec]->NDF = static_cast<int>(T.NDF[iVec]);
    }
  }

  delete[] x;
  delete[] u;
  delete[] v;
  delete[] y;
  delete[] z;
  delete[] w;
  delete[] fB;
}

void CbmL1PFFitter::GetChiToVertex(vector<CbmL1Track> &Tracks, vector<float> &chiToVtx, CbmKFVertex &primVtx)
{
  vector<L1FieldRegion> B;
  CalculateFieldRegion(Tracks,B);

  int nTracks_SIMD = fvecLen;
  L1TrackPar T; // fitting parametr coresponding to current track

  CbmL1Track *t[fvecLen];

  int nStations = CbmL1::Instance()->algo->NStations;
  int NMvdStations = CbmL1::Instance()->algo->NMvdStations;
  L1Station *sta = CbmL1::Instance()->algo->vStations;
  fvec* zSta = new fvec[nStations];
  for(int iSta=0; iSta<nStations; iSta++)
    zSta[iSta] = sta[iSta].z;

  unsigned short N_vTracks = Tracks.size();

  for(unsigned short itrack = 0; itrack < N_vTracks; itrack+=fvecLen)
  {
    if(N_vTracks - itrack < static_cast<unsigned short>(fvecLen))
      nTracks_SIMD = N_vTracks - itrack;

    fvec mass2;
    for(int iVec=0; iVec<nTracks_SIMD; iVec++)
    {
      t[iVec] = & Tracks[itrack+iVec]; // current track
      T.x[iVec]  = t[iVec]->T[0];
      T.y[iVec]  = t[iVec]->T[1];
      T.tx[iVec] = t[iVec]->T[2];
      T.ty[iVec] = t[iVec]->T[3];
      T.qp[iVec] = t[iVec]->T[4];
      T.z[iVec]  = t[iVec]->T[5];
      T.C00[iVec] = t[iVec]->C[0];
      T.C10[iVec] = t[iVec]->C[1];
      T.C11[iVec] = t[iVec]->C[2];
      T.C20[iVec] = t[iVec]->C[3];
      T.C21[iVec] = t[iVec]->C[4];
      T.C22[iVec] = t[iVec]->C[5];
      T.C30[iVec] = t[iVec]->C[6];
      T.C31[iVec] = t[iVec]->C[7];
      T.C32[iVec] = t[iVec]->C[8];
      T.C33[iVec] = t[iVec]->C[9];
      T.C40[iVec] = t[iVec]->C[10];
      T.C41[iVec] = t[iVec]->C[11];
      T.C42[iVec] = t[iVec]->C[12];
      T.C43[iVec] = t[iVec]->C[13];
      T.C44[iVec] = t[iVec]->C[14];
      mass2[iVec] = t[iVec]->mass*t[iVec]->mass;
    }

    L1FieldRegion& fld = B[itrack/fvecLen];
    for(int iSt= nStations-4; iSt>=0; iSt--)
    {
      //fvec zero = ZERO;
      fvec w=ONE;
      fvec initialized = fvec(T.z > (zSta[iSt]+2.5));
      w = fvec(w & initialized);

      L1Extrapolate( T, zSta[iSt], T.qp, fld, &w );
      if(iSt == NMvdStations - 1) AddPipeMaterial( T, T.qp, mass2, w);
      AddMaterial( T, CbmL1::Instance()->algo->fRadThick[iSt].GetRadThick(T.x, T.y), T.qp, mass2, w);
    }
    fvec ONE=1;
    if( NMvdStations <= 0 ) AddPipeMaterial( T, T.qp, mass2, ONE);
    L1Extrapolate( T, primVtx.GetRefZ(), T.qp, fld);

    Double_t Cv[3] = { primVtx.GetCovMatrix()[0], primVtx.GetCovMatrix()[1], primVtx.GetCovMatrix()[2] };

    fvec dx = T.x - primVtx.GetRefX();
    fvec dy = T.y - primVtx.GetRefY();
    fvec c[3] = { T.C00, T.C10, T.C11 };
    c[0]+= Cv[0];  c[1]+= Cv[1];  c[2]+= Cv[2];
    fvec d = c[0]*c[2] - c[1]*c[1] ;
    fvec chi = sqrt( fabs( 0.5*(dx*dx*c[0]-2*dx*dy*c[1]+dy*dy*c[2])/d ) );
    fvec isNull = fvec(fabs(d)<1.e-20);
    chi = fvec(fvec(!isNull) & chi) + fvec(isNull & fvec(0));

    for(int iVec=0; iVec<nTracks_SIMD; iVec++)
      chiToVtx.push_back(chi[iVec]);
  }
  delete[] zSta;
}

void CbmL1PFFitter::CalculateFieldRegion(vector<CbmL1Track> &Tracks, vector<L1FieldRegion> &field)
{
  L1FieldRegion fld _fvecalignment;

  TClonesArray *listStsHits = CbmL1::Instance()->listStsHits;
  TClonesArray *listMvdHits = CbmL1::Instance()->listMvdHits;
  int NMvdStations = CbmL1::Instance()->algo->NMvdStations;

  int nTracks_SIMD = fvecLen;
  L1TrackPar T; // fitting parametr coresponding to current track

  CbmL1Track *t[fvecLen];

  int ista;
  L1Station *sta = CbmL1::Instance()->algo->vStations;
  L1FieldValue fB[3], fB_temp _fvecalignment;
  fvec zField[3];

  unsigned short N_vTracks = Tracks.size();

  for(unsigned short itrack = 0; itrack < N_vTracks; itrack+=fvecLen)
  {
    if(N_vTracks - itrack < static_cast<unsigned short>(fvecLen))
      nTracks_SIMD = N_vTracks - itrack;

    for(int i=0; i<nTracks_SIMD; i++)
      t[i] = & Tracks[itrack+i]; // current track

    for(int iVec=0; iVec<nTracks_SIMD; iVec++)
    {
      for(int iH = 0; iH < 2; iH++ ) //it is assumed, that track has at least 3 hits
      {
        float posx = 0.f, posy = 0.f, posz = 0.f;
        CbmL1HitStore &hs = CbmL1::Instance()->vHitStore[t[iVec]->StsHits[iH]];
        if( hs.ExtIndex<0 ){
          CbmMvdHit *hit = L1_DYNAMIC_CAST<CbmMvdHit*>(listMvdHits->At(-hs.ExtIndex-1));
          posx = hit->GetX();
          posy = hit->GetY();
          posz = hit->GetZ();
          ista = posz < 7.f ? 0 : 1;
        }else{
          CbmStsHit *hit = L1_DYNAMIC_CAST<CbmStsHit*>(listStsHits->At(hs.ExtIndex));
          posx = hit->GetX();
          posy = hit->GetY();
          posz = hit->GetZ();
          ista = hit->GetStationNr() - 1 + NMvdStations;
        }
        sta[ista].fieldSlice.GetFieldValue( posx, posy, fB_temp );
        fB[iH+1].x[iVec] = fB_temp.x[iVec];
        fB[iH+1].y[iVec] = fB_temp.y[iVec];
        fB[iH+1].z[iVec] = fB_temp.z[iVec];
        zField[iH+1][iVec] = posz;
      }
    }

    fB[0] = CbmL1::Instance()->algo->GetvtxFieldValue();
    zField[0] = 0;
    fld.Set( fB[2], zField[2], fB[1], zField[1], fB[0], zField[0] );
    field.push_back(fld);
  }
}

void CbmL1PFFitter::Fit(vector<CbmStsTrack> &Tracks, int pidHypo)
{
//  cout << " Start L1 Track Fitter " << endl;
//  int start_hit = 0; // for interation in vRecoHits[]

  fvec mass = TDatabasePDG::Instance()->GetParticle(pidHypo)->Mass();

  fvec mass2 = mass*mass;

  L1FieldValue fB0, fB1, fB2 _fvecalignment;
  L1FieldRegion fld _fvecalignment;

  FairRootManager *fManger = FairRootManager::Instance();
  TClonesArray *listStsHits = (TClonesArray *)  fManger->GetObject("StsHit");
  int NMvdStations = CbmL1::Instance()->algo->NMvdStations;
  TClonesArray *listMvdHits=0;
  if(NMvdStations>0.)
    listMvdHits = (TClonesArray *)  fManger->GetObject("MvdHit");

  static int nHits = CbmL1::Instance()->algo->NStations;
  int iVec=0, i=0;
  int nTracks_SIMD = fvecLen;
  L1TrackPar T; // fitting parametr coresponding to current track

  CbmStsTrack *t[fvecLen];

  int ista;
  L1Station *sta = CbmL1::Instance()->algo->vStations;
  fvec* x = new fvec[nHits];
  fvec* u = new fvec[nHits];
  fvec* v = new fvec[nHits];
  fvec* y = new fvec[nHits];
  fvec* z = new fvec[nHits];
  fvec* w = new fvec[nHits];
  fvec y_temp;
  fvec fz0, fz1, fz2, dz, z_start, z_end;
  L1FieldValue* fB = new L1FieldValue[nHits];
  L1FieldValue fB_temp _fvecalignment;

  unsigned short N_vTracks = Tracks.size();

  for(unsigned short itrack = 0; itrack < N_vTracks; itrack++)
  {
    Tracks[itrack].SetPidHypo(pidHypo);
  }

  for(unsigned short itrack = 0; itrack < N_vTracks; itrack+=fvecLen)
  {
    if(N_vTracks - itrack < static_cast<unsigned short>(fvecLen))
      nTracks_SIMD = N_vTracks - itrack;

    for(i=0; i<nTracks_SIMD; i++)
    {
      t[i] = & Tracks[itrack+i]; // current track
      T.x[i]  = t[i]->GetParamLast()->GetX();
      T.y[i]  = t[i]->GetParamLast()->GetY();
      T.tx[i] = t[i]->GetParamLast()->GetTx();
      T.ty[i] = t[i]->GetParamLast()->GetTy();
      T.qp[i] = t[i]->GetParamLast()->GetQp();
      T.z[i]  = t[i]->GetParamLast()->GetZ();
      T.C00[i] = t[i]->GetParamLast()->GetCovariance(0,0);
      T.C10[i] = t[i]->GetParamLast()->GetCovariance(1,0);
      T.C11[i] = t[i]->GetParamLast()->GetCovariance(1,1);
      T.C20[i] = t[i]->GetParamLast()->GetCovariance(2,0);
      T.C21[i] = t[i]->GetParamLast()->GetCovariance(2,1);
      T.C22[i] = t[i]->GetParamLast()->GetCovariance(2,2);
      T.C30[i] = t[i]->GetParamLast()->GetCovariance(3,0);
      T.C31[i] = t[i]->GetParamLast()->GetCovariance(3,1);
      T.C32[i] = t[i]->GetParamLast()->GetCovariance(3,2);
      T.C33[i] = t[i]->GetParamLast()->GetCovariance(3,3);
      T.C40[i] = t[i]->GetParamLast()->GetCovariance(4,0);
      T.C41[i] = t[i]->GetParamLast()->GetCovariance(4,1);
      T.C42[i] = t[i]->GetParamLast()->GetCovariance(4,1);
      T.C43[i] = t[i]->GetParamLast()->GetCovariance(4,3);
      T.C44[i] = t[i]->GetParamLast()->GetCovariance(4,4);
    }

    // get hits of current track
    for(i=0; i<nHits; i++)
    {
      w[i] = ZERO;
      z[i] = sta[i].z;
    }

    for(iVec=0; iVec<nTracks_SIMD; iVec++)
    {
      int nHitsTrackMvd = t[iVec]->GetNMvdHits();
      int nHitsTrackSts = t[iVec]->GetNStsHits();
      int nHitsTrack = nHitsTrackMvd + nHitsTrackSts;
      for(i = 0; i < nHitsTrack; i++ )
      {
        float posx = 0.f, posy = 0.f, posz = 0.f;

        if(i<nHitsTrackMvd)
        {
          if(!listMvdHits) continue;
          int hitIndex = t[iVec]->GetMvdHitIndex(i);
          CbmMvdHit *hit = L1_DYNAMIC_CAST<CbmMvdHit*>(listMvdHits->At(hitIndex));

          posx = hit->GetX();
          posy = hit->GetY();
          posz = hit->GetZ();
          ista = posz < 7.f ? 0 : 1;
        }
        else
        {
          if(!listStsHits) continue;
          int hitIndex = t[iVec]->GetStsHitIndex(i - nHitsTrackMvd);
          CbmStsHit *hit = L1_DYNAMIC_CAST<CbmStsHit*>(listStsHits->At(hitIndex));

          posx = hit->GetX();
          posy = hit->GetY();
          posz = hit->GetZ();
          ista = hit->GetStationNr() - 1 + NMvdStations;
        }

        w[ista][iVec] = 1.f;

        x[ista][iVec] = posx;
        y[ista][iVec] = posy;
        u[ista][iVec] = posx*sta[ista].frontInfo.cos_phi[0] + posy*sta[ista].frontInfo.sin_phi[0];
        v[ista][iVec] = posx* sta[ista].backInfo.cos_phi[0] + posy*sta[ista].backInfo.sin_phi[0];
        z[ista][iVec] = posz;
        sta[ista].fieldSlice.GetFieldValue( x[ista], y[ista], fB_temp );
        fB[ista].x[iVec] = fB_temp.x[iVec];
        fB[ista].y[iVec] = fB_temp.y[iVec];
        fB[ista].z[iVec] = fB_temp.z[iVec];
        if(i == 0) z_start[iVec] = posz;
        if(i == nHitsTrack-1) z_end[iVec] = posz;
      }
    }

//fit backward

    fvec qp0 = T.qp;

    i= nHits-1;

    FilterFirst( T, x[i],y[i],w[i], sta[i] );
    AddMaterial( T, CbmL1::Instance()->algo->fRadThick[i].GetRadThick(T.x, T.y), qp0 , mass2, w[i]);

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

      //fvec one = ONE;
      fvec zero = ZERO;
      fvec initialised = fvec(z[i] > z_end) & fvec(z_start > z[i]);
      fvec w1 = (zero & initialised) + (w[i] & (!initialised));

      L1Extrapolate( T, z[i], qp0, fld, &w1 );
      if(i == NMvdStations - 1) AddPipeMaterial( T, qp0, mass2, w1);
      AddMaterial( T, CbmL1::Instance()->algo->fRadThick[i].GetRadThick(T.x, T.y), qp0, mass2, w1 );
      Filter( T, sta[i].frontInfo, u[i], w[i] );
      Filter( T, sta[i].backInfo,  v[i], w[i] );
      fB2 = fB1; 
      fz2 = fz1;
      fB1 = fB0; 
      fz1 = fz0;
    }

    for(iVec=0; iVec<nTracks_SIMD; iVec++)
    {
      t[iVec]->GetParamFirst()->SetX(T.x[iVec]);
      t[iVec]->GetParamFirst()->SetY(T.y[iVec]);
      t[iVec]->GetParamFirst()->SetTx(T.tx[iVec]);
      t[iVec]->GetParamFirst()->SetTy(T.ty[iVec]);
      t[iVec]->GetParamFirst()->SetQp(T.qp[iVec]);
      //t[iVec]->TFirst[5] = z_start[iVec];
      t[iVec]->GetParamFirst()->SetZ(T.z[iVec]);

      t[iVec]->GetParamFirst()->SetCovariance(0,0,T.C00[iVec]);
      t[iVec]->GetParamFirst()->SetCovariance(1,0,T.C10[iVec]);
      t[iVec]->GetParamFirst()->SetCovariance(1,1,T.C11[iVec]);
      t[iVec]->GetParamFirst()->SetCovariance(2,0,T.C20[iVec]);
      t[iVec]->GetParamFirst()->SetCovariance(2,1,T.C21[iVec]);
      t[iVec]->GetParamFirst()->SetCovariance(2,2,T.C22[iVec]);
      t[iVec]->GetParamFirst()->SetCovariance(3,0,T.C30[iVec]);
      t[iVec]->GetParamFirst()->SetCovariance(3,1,T.C31[iVec]);
      t[iVec]->GetParamFirst()->SetCovariance(3,2,T.C32[iVec]);
      t[iVec]->GetParamFirst()->SetCovariance(3,3,T.C33[iVec]);
      t[iVec]->GetParamFirst()->SetCovariance(4,0,T.C40[iVec]);
      t[iVec]->GetParamFirst()->SetCovariance(4,1,T.C41[iVec]);
      t[iVec]->GetParamFirst()->SetCovariance(4,2,T.C42[iVec]);
      t[iVec]->GetParamFirst()->SetCovariance(4,3,T.C43[iVec]);
      t[iVec]->GetParamFirst()->SetCovariance(4,4,T.C44[iVec]);

      t[iVec]->SetChi2(T.chi2[iVec]);
      t[iVec]->SetNDF(static_cast<int>(T.NDF[iVec]));
    }

    // fit forward

    i= 0;
    FilterLast( T, x[i],y[i],w[i], sta[i] );
    qp0 = T.qp;
    AddMaterial( T, CbmL1::Instance()->algo->fRadThick[i].GetRadThick(T.x, T.y), qp0, mass2, w[i] );

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

      //fvec one = ONE;
      fvec zero = ZERO;
      fvec initialised = fvec(z[i] > z_end) & fvec(z_start > z[i]);
      fvec w1 = (zero & initialised) + (w[i] & (!initialised));

      L1Extrapolate( T, z[i], qp0, fld,&w1 );
      if(i == NMvdStations ) AddPipeMaterial( T, qp0, mass2, w1 );
      AddMaterial( T, CbmL1::Instance()->algo->fRadThick[i].GetRadThick(T.x, T.y), qp0, mass2, w1 );
      Filter( T, sta[i].frontInfo, u[i], w[i] );
      Filter( T, sta[i].backInfo,  v[i], w[i] );

      fB2 = fB1; 
      fz2 = fz1;
      fB1 = fB0; 
      fz1 = fz0;
    }

    for(iVec=0; iVec<nTracks_SIMD; iVec++)
    {
      t[iVec]->GetParamLast()->SetX(T.x[iVec]);
      t[iVec]->GetParamLast()->SetY(T.y[iVec]);
      t[iVec]->GetParamLast()->SetTx(T.tx[iVec]);
      t[iVec]->GetParamLast()->SetTy(T.ty[iVec]);
      t[iVec]->GetParamLast()->SetQp(T.qp[iVec]);
      //t[iVec]->TFirst[5] = z_start[iVec];
      t[iVec]->GetParamLast()->SetZ(T.z[iVec]);

      t[iVec]->GetParamLast()->SetCovariance(0,0,T.C00[iVec]);
      t[iVec]->GetParamLast()->SetCovariance(1,0,T.C10[iVec]);
      t[iVec]->GetParamLast()->SetCovariance(1,1,T.C11[iVec]);
      t[iVec]->GetParamLast()->SetCovariance(2,0,T.C20[iVec]);
      t[iVec]->GetParamLast()->SetCovariance(2,1,T.C21[iVec]);
      t[iVec]->GetParamLast()->SetCovariance(2,2,T.C22[iVec]);
      t[iVec]->GetParamLast()->SetCovariance(3,0,T.C30[iVec]);
      t[iVec]->GetParamLast()->SetCovariance(3,1,T.C31[iVec]);
      t[iVec]->GetParamLast()->SetCovariance(3,2,T.C32[iVec]);
      t[iVec]->GetParamLast()->SetCovariance(3,3,T.C33[iVec]);
      t[iVec]->GetParamLast()->SetCovariance(4,0,T.C40[iVec]);
      t[iVec]->GetParamLast()->SetCovariance(4,1,T.C41[iVec]);
      t[iVec]->GetParamLast()->SetCovariance(4,2,T.C42[iVec]);
      t[iVec]->GetParamLast()->SetCovariance(4,3,T.C43[iVec]);
      t[iVec]->GetParamLast()->SetCovariance(4,4,T.C44[iVec]);
    }
  }

  delete[] x;
  delete[] u;
  delete[] v;
  delete[] y;
  delete[] z;
  delete[] w;
  delete[] fB;
}

void CbmL1PFFitter::GetChiToVertex(vector<CbmStsTrack> &Tracks, vector<L1FieldRegion> &field, vector<float> &chiToVtx, CbmKFVertex &primVtx, float chiPrim)
{
  chiToVtx.reserve(Tracks.size());

  int nTracks_SIMD = fvecLen;
  L1TrackPar T; // fitting parametr coresponding to current track

  CbmStsTrack *t[fvecLen];

  int nStations = CbmL1::Instance()->algo->NStations;
  int NMvdStations = CbmL1::Instance()->algo->NMvdStations;
  L1Station *sta = CbmL1::Instance()->algo->vStations;
  fvec* zSta = new fvec[nStations];
  for(int iSta=0; iSta<nStations; iSta++)
    zSta[iSta] = sta[iSta].z;

  field.reserve(Tracks.size());

  L1FieldRegion fld _fvecalignment;
  L1FieldValue fB[3], fB_temp _fvecalignment;
  fvec zField[3];

  FairRootManager *fManger = FairRootManager::Instance();
  TClonesArray *listStsHits = (TClonesArray *)  fManger->GetObject("StsHit");
  TClonesArray *listMvdHits=0;
  if(NMvdStations>0.)
    listMvdHits = (TClonesArray *)  fManger->GetObject("MvdHit");

  unsigned short N_vTracks = Tracks.size();
  int ista;
  for(unsigned short itrack = 0; itrack < N_vTracks; itrack+=fvecLen)
  {
    if(N_vTracks - itrack < static_cast<unsigned short>(fvecLen))
      nTracks_SIMD = N_vTracks - itrack;

    fvec mass2;
    for(int iVec=0; iVec<nTracks_SIMD; iVec++)
    {
      t[iVec] = & Tracks[itrack+iVec]; // current track
      T.x[iVec]  = t[iVec]->GetParamFirst()->GetX();
      T.y[iVec]  = t[iVec]->GetParamFirst()->GetY();
      T.tx[iVec] = t[iVec]->GetParamFirst()->GetTx();
      T.ty[iVec] = t[iVec]->GetParamFirst()->GetTy();
      T.qp[iVec] = t[iVec]->GetParamFirst()->GetQp();
      T.z[iVec]  = t[iVec]->GetParamFirst()->GetZ();
      T.C00[iVec] = t[iVec]->GetParamFirst()->GetCovariance(0,0);
      T.C10[iVec] = t[iVec]->GetParamFirst()->GetCovariance(1,0);
      T.C11[iVec] = t[iVec]->GetParamFirst()->GetCovariance(1,1);
      T.C20[iVec] = t[iVec]->GetParamFirst()->GetCovariance(2,0);
      T.C21[iVec] = t[iVec]->GetParamFirst()->GetCovariance(2,1);
      T.C22[iVec] = t[iVec]->GetParamFirst()->GetCovariance(2,2);
      T.C30[iVec] = t[iVec]->GetParamFirst()->GetCovariance(3,0);
      T.C31[iVec] = t[iVec]->GetParamFirst()->GetCovariance(3,1);
      T.C32[iVec] = t[iVec]->GetParamFirst()->GetCovariance(3,2);
      T.C33[iVec] = t[iVec]->GetParamFirst()->GetCovariance(3,3);
      T.C40[iVec] = t[iVec]->GetParamFirst()->GetCovariance(4,0);
      T.C41[iVec] = t[iVec]->GetParamFirst()->GetCovariance(4,1);
      T.C42[iVec] = t[iVec]->GetParamFirst()->GetCovariance(4,1);
      T.C43[iVec] = t[iVec]->GetParamFirst()->GetCovariance(4,3);
      T.C44[iVec] = t[iVec]->GetParamFirst()->GetCovariance(4,4);
//      float mass = TDatabasePDG::Instance()->GetParticle(t[iVec]->GetPidHypo())->Mass();
      const float mass = CbmKFParticleDatabase::Instance()->GetMass(t[iVec]->GetPidHypo());
      mass2[iVec] = mass*mass;

      int nHitsTrackMvd = t[iVec]->GetNMvdHits();
      for(int iH = 0; iH < 2; iH++ )
      {
        float posx = 0.f, posy = 0.f, posz = 0.f;

        if(iH<nHitsTrackMvd)
        {
          if(!listMvdHits) continue;
          int hitIndex = t[iVec]->GetMvdHitIndex(iH);
          CbmMvdHit *hit = L1_DYNAMIC_CAST<CbmMvdHit*>(listMvdHits->At(hitIndex));

          posx = hit->GetX();
          posy = hit->GetY();
          posz = hit->GetZ();
          ista = posz < 7.f ? 0 : 1;
        }
        else
        {
          if(!listStsHits) continue;
          int hitIndex = t[iVec]->GetStsHitIndex(iH - nHitsTrackMvd);
          CbmStsHit *hit = L1_DYNAMIC_CAST<CbmStsHit*>(listStsHits->At(hitIndex));

          posx = hit->GetX();
          posy = hit->GetY();
          posz = hit->GetZ();
          ista = hit->GetStationNr()-1+NMvdStations;
        }

        sta[ista].fieldSlice.GetFieldValue( posx, posy, fB_temp );
        fB[iH+1].x[iVec] = fB_temp.x[iVec];
        fB[iH+1].y[iVec] = fB_temp.y[iVec];
        fB[iH+1].z[iVec] = fB_temp.z[iVec];
        zField[iH+1][iVec] = posz;
      }
    }

    fB[0] = CbmL1::Instance()->algo->GetvtxFieldValue();
    zField[0] = 0;
    fld.Set( fB[2], zField[2], fB[1], zField[1], fB[0], zField[0] );
    field.push_back(fld);

    for(int iSt= nStations-4; iSt>=0; iSt--)
    {
      //fvec zero = ZERO;
      fvec w=ONE;
      fvec initialized = fvec(T.z > (zSta[iSt]+2.5));
      w = fvec(w & initialized);

      L1Extrapolate( T, zSta[iSt], T.qp, fld, &w );
      if(iSt == NMvdStations - 1) AddPipeMaterial( T, T.qp, mass2, w);
      AddMaterial( T, CbmL1::Instance()->algo->fRadThick[iSt].GetRadThick(T.x, T.y), T.qp, mass2, w);
    }
    fvec ONE=1;
    if( NMvdStations <= 0 ) AddPipeMaterial( T, T.qp, mass2, ONE);
    L1Extrapolate( T, primVtx.GetRefZ(), T.qp, fld);

    Double_t Cv[3] = { primVtx.GetCovMatrix()[0], primVtx.GetCovMatrix()[1], primVtx.GetCovMatrix()[2] };

    fvec dx = T.x - primVtx.GetRefX();
    fvec dy = T.y - primVtx.GetRefY();
    fvec c[3] = { T.C00, T.C10, T.C11 };
    c[0]+= Cv[0];  c[1]+= Cv[1];  c[2]+= Cv[2];
    fvec d = c[0]*c[2] - c[1]*c[1] ;
    fvec chi = sqrt( fabs( 0.5*(dx*dx*c[0]-2*dx*dy*c[1]+dy*dy*c[2])/d ) );
    fvec isNull = fvec(fabs(d)<1.e-20);
    chi = fvec(fvec(!isNull) & chi) + fvec(isNull & fvec(0));

    for(int iVec=0; iVec<nTracks_SIMD; iVec++)
      chiToVtx.push_back(chi[iVec]);

    if(chiPrim>0)
    {
      for(int iVec=0; iVec<nTracks_SIMD; iVec++)
      {
        if( chi[iVec] < chiPrim )
        {
          t[iVec]->GetParamFirst()->SetX(T.x[iVec]);
          t[iVec]->GetParamFirst()->SetY(T.y[iVec]);
          t[iVec]->GetParamFirst()->SetTx(T.tx[iVec]);
          t[iVec]->GetParamFirst()->SetTy(T.ty[iVec]);
          t[iVec]->GetParamFirst()->SetQp(T.qp[iVec]);
          t[iVec]->GetParamFirst()->SetZ(T.z[iVec]);

          t[iVec]->GetParamFirst()->SetCovariance(0,0,T.C00[iVec]);
          t[iVec]->GetParamFirst()->SetCovariance(1,0,T.C10[iVec]);
          t[iVec]->GetParamFirst()->SetCovariance(1,1,T.C11[iVec]);
          t[iVec]->GetParamFirst()->SetCovariance(2,0,T.C20[iVec]);
          t[iVec]->GetParamFirst()->SetCovariance(2,1,T.C21[iVec]);
          t[iVec]->GetParamFirst()->SetCovariance(2,2,T.C22[iVec]);
          t[iVec]->GetParamFirst()->SetCovariance(3,0,T.C30[iVec]);
          t[iVec]->GetParamFirst()->SetCovariance(3,1,T.C31[iVec]);
          t[iVec]->GetParamFirst()->SetCovariance(3,2,T.C32[iVec]);
          t[iVec]->GetParamFirst()->SetCovariance(3,3,T.C33[iVec]);
          t[iVec]->GetParamFirst()->SetCovariance(4,0,T.C40[iVec]);
          t[iVec]->GetParamFirst()->SetCovariance(4,1,T.C41[iVec]);
          t[iVec]->GetParamFirst()->SetCovariance(4,2,T.C42[iVec]);
          t[iVec]->GetParamFirst()->SetCovariance(4,3,T.C43[iVec]);
          t[iVec]->GetParamFirst()->SetCovariance(4,4,T.C44[iVec]);
        }
      }
    }
  }
  delete[] zSta;
}

void CbmL1PFFitter::CalculateFieldRegion(vector<CbmStsTrack> &Tracks, vector<L1FieldRegion> &field)
{
  field.reserve(Tracks.size());

  L1FieldRegion fld _fvecalignment;

  FairRootManager *fManger = FairRootManager::Instance();
  TClonesArray *listStsHits = (TClonesArray *)  fManger->GetObject("StsHit");
  TClonesArray *listMvdHits=0;
  int NMvdStations = CbmL1::Instance()->algo->NMvdStations;
  if(NMvdStations>0.)
    listMvdHits = (TClonesArray *)  fManger->GetObject("MvdHit");

  int nTracks_SIMD = fvecLen;
  L1TrackPar T; // fitting parametr coresponding to current track

  CbmStsTrack *t[fvecLen];

  int ista;
  L1Station *sta = CbmL1::Instance()->algo->vStations;
  L1FieldValue fB[3], fB_temp _fvecalignment;
  fvec zField[3];

  unsigned short N_vTracks = Tracks.size();

  for(unsigned short itrack = 0; itrack < N_vTracks; itrack+=fvecLen)
  {
    if(N_vTracks - itrack < static_cast<unsigned short>(fvecLen))
      nTracks_SIMD = N_vTracks - itrack;

    for(int i=0; i<nTracks_SIMD; i++)
      t[i] = & Tracks[itrack+i]; // current track

    for(int iVec=0; iVec<nTracks_SIMD; iVec++)
    {
      int nHitsTrackMvd = t[iVec]->GetNMvdHits();
      for(int iH = 0; iH < 2; iH++ )
      {
        float posx = 0.f, posy = 0.f, posz = 0.f;

        if(iH<nHitsTrackMvd)
        {
          if(!listMvdHits) continue;
          int hitIndex = t[iVec]->GetMvdHitIndex(iH);
          CbmMvdHit *hit = L1_DYNAMIC_CAST<CbmMvdHit*>(listMvdHits->At(hitIndex));

          posx = hit->GetX();
          posy = hit->GetY();
          posz = hit->GetZ();
          ista = posz < 7.f ? 0 : 1;
        }
        else
        {
          if(!listStsHits) continue;
          int hitIndex = t[iVec]->GetStsHitIndex(iH - nHitsTrackMvd);
          CbmStsHit *hit = L1_DYNAMIC_CAST<CbmStsHit*>(listStsHits->At(hitIndex));

          posx = hit->GetX();
          posy = hit->GetY();
          posz = hit->GetZ();
          ista = hit->GetStationNr()-1+NMvdStations;
        }

        sta[ista].fieldSlice.GetFieldValue( posx, posy, fB_temp );
        fB[iH+1].x[iVec] = fB_temp.x[iVec];
        fB[iH+1].y[iVec] = fB_temp.y[iVec];
        fB[iH+1].z[iVec] = fB_temp.z[iVec];
        zField[iH+1][iVec] = posz;
      }
    }

    fB[0] = CbmL1::Instance()->algo->GetvtxFieldValue();
    zField[0] = 0;
    fld.Set( fB[2], zField[2], fB[1], zField[1], fB[0], zField[0] );
    field.push_back(fld);
  }
}
