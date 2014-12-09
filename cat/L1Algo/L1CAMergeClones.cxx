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
 *  Merge Clones
 *
 */

#include "L1Algo.h"
#include "L1Extrapolation.h"

#include <iostream>

// using namespace std;
using std::cout;
using std::endl;
using std::vector;

void L1Algo::InvertCholetsky(fvec a[15])
{
  fvec d[5], uud, u[5][5];
  for(int i=0; i<5; i++) 
  {
    d[i]=0.f;
    for(int j=0; j<5; j++) 
      u[i][j]=0.f;
  }

  for(int i=0; i<5; i++)
  {
    uud=0.f;
    for(int j=0; j<i; j++) 
      uud += u[j][i]*u[j][i]*d[j];
    uud = a[i*(i+3)/2] - uud;

    fvec smallval = 1.e-12;
    fvec initialised = fvec( uud<smallval );
    uud = ((!initialised) & uud) + (smallval & initialised);

    d[i] = uud/fabs(uud);
    u[i][i] = sqrt(fabs(uud));

    for(int j=i+1; j<5; j++) 
    {
      uud = 0.f;
      for(int k=0; k<i; k++)
        uud += u[k][i]*u[k][j]*d[k];
      uud = a[j*(j+1)/2+i]/*a[i][j]*/ - uud;
      u[i][j] = d[i]/u[i][i]*uud;
    }
  }

  fvec u1[5];

  for(int i=0; i<5; i++)
  {
    u1[i] = u[i][i];
    u[i][i] = 1.f/u[i][i];
  }
  for(int i=0; i<4; i++)
  {
    u[i][i+1] = - u[i][i+1]*u[i][i]*u[i+1][i+1];
  }
  for(int i=0; i<3; i++)
  {
    u[i][i+2] = u[i][i+1]*u1[i+1]*u[i+1][i+2]-u[i][i+2]*u[i][i]*u[i+2][i+2];
  }
  for(int i=0; i<2; i++)
  {
    u[i][i+3] = u[i][i+2]*u1[i+2]*u[i+2][i+3] - u[i][i+3]*u[i][i]*u[i+3][i+3];
    u[i][i+3] -= u[i][i+1]*u1[i+1]*(u[i+1][i+2]*u1[i+2]*u[i+2][i+3] - u[i+1][i+3]);
  }
  u[0][4] = u[0][2]*u1[2]*u[2][4] - u[0][4]*u[0][0]*u[4][4];
  u[0][4] += u[0][1]*u1[1]*(u[1][4] - u[1][3]*u1[3]*u[3][4] - u[1][2]*u1[2]*u[2][4]);
  u[0][4] += u[3][4]*u1[3]*(u[0][3] - u1[2]*u[2][3]*(u[0][2] - u[0][1]*u1[1]*u[1][2]));

  for(int i=0; i<5; i++)
    a[i+10] = u[i][4]*d[4]*u[4][4];
  for(int i=0; i<4; i++)
    a[i+6] = u[i][3]*u[3][3]*d[3] + u[i][4]*u[3][4]*d[4];
  for(int i=0; i<3; i++)
    a[i+3] = u[i][2]*u[2][2]*d[2] + u[i][3]*u[2][3]*d[3] + u[i][4]*u[2][4]*d[4];
  for(int i=0; i<2; i++)
    a[i+1] = u[i][1]*u[1][1]*d[1] + u[i][2]*u[1][2]*d[2] + u[i][3]*u[1][3]*d[3] + u[i][4]*u[1][4]*d[4];
  a[0] = u[0][0]*u[0][0]*d[0] + u[0][1]*u[0][1]*d[1] + u[0][2]*u[0][2]*d[2] + u[0][3]*u[0][3]*d[3] + u[0][4]*u[0][4]*d[4];
}

void L1Algo::MultiplySS(fvec const C[15], fvec const V[15], fvec K[5][5])
{
  K[0][0] = C[0]*V[ 0] + C[1]*V[ 1] + C[3]*V[ 3] + C[6]*V[ 6] + C[10]*V[10];
  K[0][1] = C[0]*V[ 1] + C[1]*V[ 2] + C[3]*V[ 4] + C[6]*V[ 7] + C[10]*V[11];
  K[0][2] = C[0]*V[ 3] + C[1]*V[ 4] + C[3]*V[ 5] + C[6]*V[ 8] + C[10]*V[12];
  K[0][3] = C[0]*V[ 6] + C[1]*V[ 7] + C[3]*V[ 8] + C[6]*V[ 9] + C[10]*V[13];
  K[0][4] = C[0]*V[10] + C[1]*V[11] + C[3]*V[12] + C[6]*V[13] + C[10]*V[14];

  K[1][0] = C[1]*V[ 0] + C[2]*V[ 1] + C[4]*V[ 3] + C[7]*V[ 6] + C[11]*V[10];
  K[1][1] = C[1]*V[ 1] + C[2]*V[ 2] + C[4]*V[ 4] + C[7]*V[ 7] + C[11]*V[11];
  K[1][2] = C[1]*V[ 3] + C[2]*V[ 4] + C[4]*V[ 5] + C[7]*V[ 8] + C[11]*V[12];
  K[1][3] = C[1]*V[ 6] + C[2]*V[ 7] + C[4]*V[ 8] + C[7]*V[ 9] + C[11]*V[13];
  K[1][4] = C[1]*V[10] + C[2]*V[11] + C[4]*V[12] + C[7]*V[13] + C[11]*V[14];

  K[2][0] = C[3]*V[ 0] + C[4]*V[ 1] + C[5]*V[ 3] + C[8]*V[ 6] + C[12]*V[10];
  K[2][1] = C[3]*V[ 1] + C[4]*V[ 2] + C[5]*V[ 4] + C[8]*V[ 7] + C[12]*V[11];
  K[2][2] = C[3]*V[ 3] + C[4]*V[ 4] + C[5]*V[ 5] + C[8]*V[ 8] + C[12]*V[12];
  K[2][3] = C[3]*V[ 6] + C[4]*V[ 7] + C[5]*V[ 8] + C[8]*V[ 9] + C[12]*V[13];
  K[2][4] = C[3]*V[10] + C[4]*V[11] + C[5]*V[12] + C[8]*V[13] + C[12]*V[14];

  K[3][0] = C[6]*V[ 0] + C[7]*V[ 1] + C[8]*V[ 3] + C[9]*V[ 6] + C[13]*V[10];
  K[3][1] = C[6]*V[ 1] + C[7]*V[ 2] + C[8]*V[ 4] + C[9]*V[ 7] + C[13]*V[11];
  K[3][2] = C[6]*V[ 3] + C[7]*V[ 4] + C[8]*V[ 5] + C[9]*V[ 8] + C[13]*V[12];
  K[3][3] = C[6]*V[ 6] + C[7]*V[ 7] + C[8]*V[ 8] + C[9]*V[ 9] + C[13]*V[13];
  K[3][4] = C[6]*V[10] + C[7]*V[11] + C[8]*V[12] + C[9]*V[13] + C[13]*V[14];

  K[4][0] = C[10]*V[ 0] + C[11]*V[ 1] + C[12]*V[ 3] + C[13]*V[ 6] + C[14]*V[10];
  K[4][1] = C[10]*V[ 1] + C[11]*V[ 2] + C[12]*V[ 4] + C[13]*V[ 7] + C[14]*V[11];
  K[4][2] = C[10]*V[ 3] + C[11]*V[ 4] + C[12]*V[ 5] + C[13]*V[ 8] + C[14]*V[12];
  K[4][3] = C[10]*V[ 6] + C[11]*V[ 7] + C[12]*V[ 8] + C[13]*V[ 9] + C[14]*V[13];
  K[4][4] = C[10]*V[10] + C[11]*V[11] + C[12]*V[12] + C[13]*V[13] + C[14]*V[14];
}

void L1Algo::MultiplyMS(fvec const C[5][5], fvec const V[15], fvec K[15])
{
  K[0] = C[0][0]*V[0] + C[0][1]*V[1] + C[0][2]*V[3] + C[0][3]*V[6] + C[0][4]*V[10];

  K[1] = C[1][0]*V[0] + C[1][1]*V[1] + C[1][2]*V[3] + C[1][3]*V[6] + C[1][4]*V[10];
  K[2] = C[1][0]*V[1] + C[1][1]*V[2] + C[1][2]*V[4] + C[1][3]*V[7] + C[1][4]*V[11];

  K[3] = C[2][0]*V[0] + C[2][1]*V[1] + C[2][2]*V[3] + C[2][3]*V[6] + C[2][4]*V[10];
  K[4] = C[2][0]*V[1] + C[2][1]*V[2] + C[2][2]*V[4] + C[2][3]*V[7] + C[2][4]*V[11];
  K[5] = C[2][0]*V[3] + C[2][1]*V[4] + C[2][2]*V[5] + C[2][3]*V[8] + C[2][4]*V[12];

  K[6] = C[3][0]*V[0] + C[3][1]*V[1] + C[3][2]*V[3] + C[3][3]*V[6] + C[3][4]*V[10];
  K[7] = C[3][0]*V[1] + C[3][1]*V[2] + C[3][2]*V[4] + C[3][3]*V[7] + C[3][4]*V[11];
  K[8] = C[3][0]*V[3] + C[3][1]*V[4] + C[3][2]*V[5] + C[3][3]*V[8] + C[3][4]*V[12];
  K[9] = C[3][0]*V[6] + C[3][1]*V[7] + C[3][2]*V[8] + C[3][3]*V[9] + C[3][4]*V[13];

  K[10] = C[4][0]*V[ 0] + C[4][1]*V[ 1] + C[4][2]*V[ 3] + C[4][3]*V[ 6] + C[4][4]*V[10];
  K[11] = C[4][0]*V[ 1] + C[4][1]*V[ 2] + C[4][2]*V[ 4] + C[4][3]*V[ 7] + C[4][4]*V[11];
  K[12] = C[4][0]*V[ 3] + C[4][1]*V[ 4] + C[4][2]*V[ 5] + C[4][3]*V[ 8] + C[4][4]*V[12];
  K[13] = C[4][0]*V[ 6] + C[4][1]*V[ 7] + C[4][2]*V[ 8] + C[4][3]*V[ 9] + C[4][4]*V[13];
  K[14] = C[4][0]*V[10] + C[4][1]*V[11] + C[4][2]*V[12] + C[4][3]*V[13] + C[4][4]*V[14];
}

void L1Algo::MultiplySR(fvec const C[15], fvec const r_in[5], fvec r_out[5])
{
  r_out[0] = r_in[0]*C[ 0] + r_in[1]*C[ 1] + r_in[2]*C[ 3] +r_in[3]*C[ 6] + r_in[4]*C[10];
  r_out[1] = r_in[0]*C[ 1] + r_in[1]*C[ 2] + r_in[2]*C[ 4] +r_in[3]*C[ 7] + r_in[4]*C[11];
  r_out[2] = r_in[0]*C[ 3] + r_in[1]*C[ 4] + r_in[2]*C[ 5] +r_in[3]*C[ 8] + r_in[4]*C[12];
  r_out[3] = r_in[0]*C[ 6] + r_in[1]*C[ 7] + r_in[2]*C[ 8] +r_in[3]*C[ 9] + r_in[4]*C[13];
  r_out[4] = r_in[0]*C[10] + r_in[1]*C[11] + r_in[2]*C[12] +r_in[3]*C[13] + r_in[4]*C[14];
}

void L1Algo::FilterTracks(fvec const r[5], fvec const C[15], fvec const m[5], fvec const V[15], fvec R[5], fvec W[15], fvec *chi2)
{
  fvec S[15];
  for(int i=0; i<15; i++)
  {
    if(W)  W[i] = C[i];
    S[i] = C[i] + V[i];
  }

  InvertCholetsky(S);
 
  fvec dzeta[5];

  for(int i=0; i<5; i++) dzeta[i] = m[i] - r[i];

  if(W && R)
  {
    for(int i=0; i<5; i++)
      R[i] = r[i];
      
    fvec K[5][5];
    MultiplySS(C,S,K);
      
    fvec KC[15];
    MultiplyMS(K,C,KC);
    for(int i=0; i< 15; i++)
      W[i] -= KC[i];

    fvec kd;
    for(int i=0; i<5; i++)
    {
      kd = 0.f;
      for(int j=0; j<5; j++)
        kd += K[i][j]*dzeta[j];
      R[i] += kd;
    }
  }
  if(chi2)
  {
    fvec S_dzeta[5];
    MultiplySR(S, dzeta, S_dzeta);
    *chi2 = dzeta[0]*S_dzeta[0] + dzeta[1]*S_dzeta[1] + dzeta[2]*S_dzeta[2] + dzeta[3]*S_dzeta[3] + dzeta[4]*S_dzeta[4];
  }
}

void L1Algo::CAMergeClones()
{
  vector<unsigned short> FirstHit;
  vector<unsigned short> LastHit;
  vector<THitI> FirstHitIndex;
  vector<THitI> LastHitIndex;
  vector<unsigned short> Neighbour;
  vector<float> TrackChi2;
  vector<bool> IsNext;
  vector<bool> IsUsed;

  vector< L1Track > vTracksNew;
  vTracksNew.reserve(vTracks.size());
  vector< THitI > vRecoHitsNew;
  vRecoHitsNew.reserve(vRecoHits.size());

  FirstHit.resize(vTracks.size());
  LastHit.resize(vTracks.size());
  FirstHitIndex.resize(vTracks.size());
  LastHitIndex.resize(vTracks.size());
  IsUsed.resize(vTracks.size());
  TrackChi2.resize(vTracks.size());
  Neighbour.resize(vTracks.size());
  IsNext.resize(vTracks.size());

  THitI start_hit = 0;
  unsigned short ista = 0;

  for(unsigned short iTr = 0; iTr < vTracks.size(); iTr++)
  {
    FirstHitIndex[iTr] = start_hit;
    ista = vSFlag[vStsHits[vRecoHits[start_hit]].f]/4;
    FirstHit[iTr]=ista;
    start_hit += vTracks[iTr].NHits-1;
    LastHitIndex[iTr] = start_hit;
    ista = vSFlag[vStsHits[vRecoHits[start_hit]].f]/4;
    LastHit[iTr]=ista;
    start_hit++;

    IsUsed[iTr] = 0;
    Neighbour[iTr] = 50000;
    TrackChi2[iTr] = 100000;
    IsNext[iTr] = 0;
  }

  L1KFTrackFitter();
  //KFTrackFitter_simple();

  L1TrackPar Tb;
  L1TrackPar Tf;
  L1FieldValue fBm, fBb, fBf _fvecalignment;
  L1FieldRegion fld _fvecalignment;

  for(int iTr = 0; iTr < static_cast<unsigned short>(vTracks.size()); iTr++)
  {
    if(static_cast<int>(vTracks[iTr].NHits) > 6) continue;
    for(int jTr = 0; jTr < static_cast<unsigned short>(vTracks.size()); jTr++)
    {
      if(iTr == jTr) continue;
      if(static_cast<int>(vTracks[iTr].NHits) > 6) continue;

      unsigned short dist = 0;
      unsigned short stab=0, staf=0;
      bool IsNextTemp=0;
//if((vTracks[iTr].TFirst[4] - vTracks[jTr].TFirst[4])*(vTracks[iTr].TFirst[4] - vTracks[jTr].TFirst[4]) 
// > 9*(vTracks[iTr].CFirst[14]+vTracks[jTr].CFirst[14]) ) continue;
      if(FirstHit[iTr] > LastHit[jTr])
      {
        dist = FirstHit[iTr] - LastHit[jTr];

        stab = FirstHit[iTr];
        staf = LastHit[jTr];
        IsNextTemp = 1;

        Tb.x  = vTracks[iTr].TFirst[0];
        Tb.y  = vTracks[iTr].TFirst[1];
        Tb.tx = vTracks[iTr].TFirst[2];
        Tb.ty = vTracks[iTr].TFirst[3];
        Tb.qp = vTracks[iTr].TFirst[4];
        Tb.z  = vTracks[iTr].TFirst[5];
        Tb.C00 = vTracks[iTr].CFirst[0];
        Tb.C10 = vTracks[iTr].CFirst[1];
        Tb.C11 = vTracks[iTr].CFirst[2];
        Tb.C20 = vTracks[iTr].CFirst[3];
        Tb.C21 = vTracks[iTr].CFirst[4];
        Tb.C22 = vTracks[iTr].CFirst[5];
        Tb.C30 = vTracks[iTr].CFirst[6];
        Tb.C31 = vTracks[iTr].CFirst[7];
        Tb.C32 = vTracks[iTr].CFirst[8];
        Tb.C33 = vTracks[iTr].CFirst[9];
        Tb.C40 = vTracks[iTr].CFirst[10];
        Tb.C41 = vTracks[iTr].CFirst[11];
        Tb.C42 = vTracks[iTr].CFirst[12];
        Tb.C43 = vTracks[iTr].CFirst[13];
        Tb.C44 = vTracks[iTr].CFirst[14];

        Tf.x  = vTracks[jTr].TLast[0];
        Tf.y  = vTracks[jTr].TLast[1];
        Tf.tx = vTracks[jTr].TLast[2];
        Tf.ty = vTracks[jTr].TLast[3];
        Tf.qp = vTracks[jTr].TLast[4];
        Tf.z  = vTracks[jTr].TLast[5];
        Tf.C00 = vTracks[jTr].CLast[0];
        Tf.C10 = vTracks[jTr].CLast[1];
        Tf.C11 = vTracks[jTr].CLast[2];
        Tf.C20 = vTracks[jTr].CLast[3];
        Tf.C21 = vTracks[jTr].CLast[4];
        Tf.C22 = vTracks[jTr].CLast[5];
        Tf.C30 = vTracks[jTr].CLast[6];
        Tf.C31 = vTracks[jTr].CLast[7];
        Tf.C32 = vTracks[jTr].CLast[8];
        Tf.C33 = vTracks[jTr].CLast[9];
        Tf.C40 = vTracks[jTr].CLast[10];
        Tf.C41 = vTracks[jTr].CLast[11];
        Tf.C42 = vTracks[jTr].CLast[12];
        Tf.C43 = vTracks[jTr].CLast[13];
        Tf.C44 = vTracks[jTr].CLast[14];
//std::cout << "!!!!!!! Chi2 !!!!!!      "<<vTracks[iTr].TFirst[0]<<"  "<<vTracks[jTr].TLast[0]<<std::endl;
      }
      if(FirstHit[jTr] > LastHit[iTr])
      {
        dist = FirstHit[jTr] - LastHit[iTr];

        stab = FirstHit[jTr];
        staf = LastHit[iTr];

        Tb.x  = vTracks[jTr].TFirst[0];
        Tb.y  = vTracks[jTr].TFirst[1];
        Tb.tx = vTracks[jTr].TFirst[2];
        Tb.ty = vTracks[jTr].TFirst[3];
        Tb.qp = vTracks[jTr].TFirst[4];
        Tb.z  = vTracks[jTr].TFirst[5];
        Tb.C00 = vTracks[jTr].CFirst[0];
        Tb.C10 = vTracks[jTr].CFirst[1];
        Tb.C11 = vTracks[jTr].CFirst[2];
        Tb.C20 = vTracks[jTr].CFirst[3];
        Tb.C21 = vTracks[jTr].CFirst[4];
        Tb.C22 = vTracks[jTr].CFirst[5];
        Tb.C30 = vTracks[jTr].CFirst[6];
        Tb.C31 = vTracks[jTr].CFirst[7];
        Tb.C32 = vTracks[jTr].CFirst[8];
        Tb.C33 = vTracks[jTr].CFirst[9];
        Tb.C40 = vTracks[jTr].CFirst[10];
        Tb.C41 = vTracks[jTr].CFirst[11];
        Tb.C42 = vTracks[jTr].CFirst[12];
        Tb.C43 = vTracks[jTr].CFirst[13];
        Tb.C44 = vTracks[jTr].CFirst[14];

        Tf.x  = vTracks[iTr].TLast[0];
        Tf.y  = vTracks[iTr].TLast[1];
        Tf.tx = vTracks[iTr].TLast[2];
        Tf.ty = vTracks[iTr].TLast[3];
        Tf.qp = vTracks[iTr].TLast[4];
        Tf.z  = vTracks[iTr].TLast[5];
        Tf.C00 = vTracks[iTr].CLast[0];
        Tf.C10 = vTracks[iTr].CLast[1];
        Tf.C11 = vTracks[iTr].CLast[2];
        Tf.C20 = vTracks[iTr].CLast[3];
        Tf.C21 = vTracks[iTr].CLast[4];
        Tf.C22 = vTracks[iTr].CLast[5];
        Tf.C30 = vTracks[iTr].CLast[6];
        Tf.C31 = vTracks[iTr].CLast[7];
        Tf.C32 = vTracks[iTr].CLast[8];
        Tf.C33 = vTracks[iTr].CLast[9];
        Tf.C40 = vTracks[iTr].CLast[10];
        Tf.C41 = vTracks[iTr].CLast[11];
        Tf.C42 = vTracks[iTr].CLast[12];
        Tf.C43 = vTracks[iTr].CLast[13];
        Tf.C44 = vTracks[iTr].CLast[14];
      }

      if(dist == 0) continue;
      //if(((Tf.qp - Tb.qp)*(Tf.qp - Tb.qp)/(Tb.C44+Tf.C44))[0] > 25*10*7) continue;

      unsigned short stam;

      vStations[staf].fieldSlice.GetFieldValue( Tf.x, Tf.y, fBf );
      vStations[stab].fieldSlice.GetFieldValue( Tb.x, Tb.y, fBb );
      if(dist > 1) stam = staf + 1;
      else stam = staf - 1;
      fvec zm = vStations[stam].z;
      fvec xm = 0.5*(Tf.x + Tf.tx*(zm - Tf.z) + Tb.x + Tb.tx*(zm - Tb.z));
      fvec ym = 0.5*(Tb.y + Tb.ty*(zm - Tb.z) + Tb.y + Tb.ty*(zm - Tb.z));
      vStations[stam].fieldSlice.GetFieldValue( xm, ym, fBm );
      fld.Set( fBb, Tb.z, fBm, zm, fBf, Tf.z );

      fvec zMiddle = 0.5*(Tb.z + Tf.z);

      L1Extrapolate( Tf, zMiddle, Tf.qp, fld );
      L1Extrapolate( Tb, zMiddle, Tb.qp, fld );

      fvec Chi2Tracks = 0.f;
      FilterTracks(&(Tf.x),&(Tf.C00),&(Tb.x),&(Tb.C00),0,0,&Chi2Tracks);
      if(Chi2Tracks[0] > 50 ) continue;
      if(Chi2Tracks[0] < TrackChi2[iTr] || Chi2Tracks[0] < TrackChi2[jTr])
      {
//std::cout << "!!!!!!! Chi2 !!!!!!      "<<Chi2Tracks<<"     " <<iTr<<"  " <<jTr<< std::endl;
        if(Neighbour[iTr] < static_cast<unsigned short>(50000))
        {
          Neighbour[Neighbour[iTr]] = 50000;
          TrackChi2[Neighbour[iTr]] = 100000;
          IsNext[Neighbour[iTr]] = 0;
        }
        if(Neighbour[jTr] < static_cast<unsigned short>(50000))
        {
          Neighbour[Neighbour[jTr]] = 50000;
          TrackChi2[Neighbour[jTr]] = 100000;
          IsNext[Neighbour[jTr]] = 0;
        }
        Neighbour[iTr] = jTr;
        Neighbour[jTr] = iTr;
        TrackChi2[iTr] = Chi2Tracks[0];
        TrackChi2[jTr] = Chi2Tracks[0];
        IsNext[iTr] = IsNextTemp;
        IsNext[jTr] = (!IsNextTemp);
      }
    }
  }
  for(int iTr = 0; iTr < static_cast<unsigned short>(vTracks.size()); iTr++)
  {
    if(IsUsed[iTr]) continue;

    vTracksNew.push_back(vTracks[iTr]);
    if(!IsNext[iTr])
      for(THitI HI = FirstHitIndex[iTr]; HI <= LastHitIndex[iTr]; HI++)
        vRecoHitsNew.push_back(vRecoHits[HI]);

    if(Neighbour[iTr] < 50000)
    {
      IsUsed[Neighbour[iTr]] = 1;
      vTracksNew.back().NHits += vTracks[Neighbour[iTr]].NHits;
      for(THitI HI = FirstHitIndex[Neighbour[iTr]]; HI <= LastHitIndex[Neighbour[iTr]]; HI++)
        vRecoHitsNew.push_back(vRecoHits[HI]);
    }

    if(IsNext[iTr])
      for(THitI HI = FirstHitIndex[iTr]; HI <= LastHitIndex[iTr]; HI++)
        vRecoHitsNew.push_back(vRecoHits[HI]);
  }
  vTracks.resize(vTracksNew.size());
  for(unsigned short iTr=0; iTr < vTracksNew.size(); iTr++)
    vTracks[iTr] = vTracksNew[iTr];
  vRecoHits.resize(vRecoHitsNew.size());
  for(THitI iHi=0; iHi < vRecoHitsNew.size(); iHi++)
    vRecoHits[iHi] = vRecoHitsNew[iHi];

//std::cout << "!!!!!!!!!!!!!!!!!!!!!!!!!!!   new "<<vTracksNew.size()<<"  old "<< vTracks.size()<<std::endl;
}
