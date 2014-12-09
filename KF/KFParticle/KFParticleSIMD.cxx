//----------------------------------------------------------------------------
// Implementation of the KFParticleSIMD class
// .
// @author  S.Gorbunov, I.Kisel, I.Kulakov, M.Zyzak
// @version 1.0
// @since   13.05.07
// 
// Class to reconstruct and store the decayed particle parameters.
// The method is described in CBM-SOFT note 2007-003, 
// ``Reconstruction of decayed particles based on the Kalman filter'', 
// http://www.gsi.de/documents/DOC-2007-May-14-1.pdf
//
// This class is ALICE interface to general mathematics in KFParticleSIMDCore
// 
//  -= Copyright &copy ALICE HLT and CBM L1 Groups =-
//____________________________________________________________________________


#include "KFParticleSIMD.h"
#include "KFParticle.h"

#include "TDatabasePDG.h"
#include "TParticlePDG.h"

#ifdef HomogeneousField
#include "AliVTrack.h"
#include "AliVVertex.h"
#endif

#ifdef NonhomogeneousField
#include "CbmKFTrack.h"
#include "CbmKFVertex.h"
#include "CbmKFParticleDatabase.h"
#endif

#ifdef HomogeneousField
fvec KFParticleSIMD::fgBz = -5.;  //* Bz compoment of the magnetic field
#endif

static const fvec Zero = 0;
static const fvec One = 1;

KFParticleSIMD::KFParticleSIMD( const KFParticleSIMD &d1, const KFParticleSIMD &d2, Bool_t gamma )
#ifdef NonhomogeneousField
  : fField()
#endif
{
  if (!gamma) {
    KFParticleSIMD mother;
    mother+= d1;
    mother+= d2;
    *this = mother;
  } else
    ConstructGamma(d1, d2);
}

void KFParticleSIMD::Create( const fvec Param[], const fvec Cov[], Int_t Charge, fvec mass /*Int_t PID*/ )
{
  // Constructor from "cartesian" track, PID hypothesis should be provided
  //
  // Param[6] = { X, Y, Z, Px, Py, Pz } - position and momentum
  // Cov [21] = lower-triangular part of the covariance matrix:
  //
  //                (  0  .  .  .  .  . )
  //                (  1  2  .  .  .  . )
  //  Cov. matrix = (  3  4  5  .  .  . ) - numbering of covariance elements in Cov[]
  //                (  6  7  8  9  .  . )
  //                ( 10 11 12 13 14  . )
  //                ( 15 16 17 18 19 20 )
  fvec C[21];
  for( int i=0; i<21; i++ ) C[i] = Cov[i];
  
//  TParticlePDG* particlePDG = TDatabasePDG::Instance()->GetParticle(PID);
//  fvec mass = (particlePDG) ? particlePDG->Mass() :0.13957;
  
  KFParticleBaseSIMD::Initialize( Param, C, Charge, mass );
}

#ifdef NonhomogeneousField
KFParticleSIMD::KFParticleSIMD( CbmKFTrackInterface* Track[], int NTracks, Int_t *qHypo, const Int_t *pdg)
    : fField()
{
  Create(Track, NTracks, qHypo, pdg);
}

void KFParticleSIMD::Create(CbmKFTrackInterface* Track[], int NTracks, Int_t *qHypo, const Int_t *pdg)
{
  fvec m[6];
  fvec V[15];
  fvec TrMass;

  fDaughterIds.push_back( fvec(-1) );
  for (int j=0; j<NTracks; j++)
  {
    fDaughterIds.back()[j] =  Track[j]->Id();

//    TrMass[j] = pdg ? TDatabasePDG::Instance()->GetParticle(*pdg)->Mass() : Track[j]->GetMass();
    TrMass[j] = pdg ? CbmKFParticleDatabase::Instance()->GetMass(*pdg) : Track[j]->GetMass();

    fNDF[j] = Track[j]->GetRefNDF();
    fChi2[j] = Track[j]->GetRefChi2();

    for(int i = 0; i < 6; i++) m[i][j] = Track[j]->GetTrack()[i];
    for(int i = 0; i < 15; i++) V[i][j] = Track[j]->GetCovMatrix()[i];

  }

  fvec a = m[2], b = m[3], qp = m[4];
  
  //* convert the track to (x,y,px,py,pz,e,t/p) parameterization

  fvec c2 = 1./(1. + a*a + b*b);
  fvec pq = 1./qp;
  fvec p2 = pq*pq;
  fvec pz = sqrt(p2*c2);
  fvec px = a*pz;
  fvec py = b*pz;
  fvec eE = sqrt( TrMass*TrMass + p2 );
    
  fvec H[3] = { -px*c2, -py*c2, -pz*pq };
  fvec HE = -pq*p2/eE;

  fP[0] = m[0];
  fP[1] = m[1];
  fP[2] = m[5];
  fP[3] = px;
  fP[4] = py;
  fP[5] = pz;
  fP[6] = eE;
  fP[7] = 0;

  fvec cxpz = H[0]*V[ 3] + H[1]*V[ 6] + H[2]*V[10];
  fvec cypz = H[0]*V[ 4] + H[1]*V[ 7] + H[2]*V[11];
  fvec capz = H[0]*V[ 5] + H[1]*V[ 8] + H[2]*V[12];
  fvec cbpz = H[0]*V[ 8] + H[1]*V[ 9] + H[2]*V[13];
  fvec cqpz = H[0]*V[12] + H[1]*V[13] + H[2]*V[14];
  fvec cpzpz = H[0]*H[0]*V[5] +H[1]*H[1]*V[9] + H[2]*H[2]*V[14] 
    + 2*( H[0]*H[1]*V[8] +H[0]*H[2]*V[12] +H[1]*H[2]*V[13]);

  fC[ 0] = V[0];
  fC[ 1] = V[1];
  fC[ 2] = V[2];
  fC[ 3] = 0.f;
  fC[ 4] = 0.f;
  fC[ 5] = 0.f;
  fC[ 6] = V[3]*pz + a*cxpz; 
  fC[ 7] = V[4]*pz + a*cypz; 
  fC[ 8] = 0.f;
  fC[ 9] = V[5]*pz*pz + 2*a*pz*capz + a*a*cpzpz;
  fC[10] = V[6]*pz+b*cxpz; 
  fC[11] = V[7]*pz+b*cypz; 
  fC[12] = 0.f;
  fC[13] = V[8]*pz*pz + a*pz*cbpz + b*pz*capz + a*b*cpzpz;
  fC[14] = V[9]*pz*pz + 2*b*pz*cbpz + b*b*cpzpz;
  fC[15] = cxpz; 
  fC[16] = cypz; 
  fC[17] = 0.f;
  fC[18] = capz*pz + a*cpzpz;
  fC[19] = cbpz*pz + b*cpzpz;
  fC[20] = cpzpz;
  fC[21] = HE*V[10];
  fC[22] = HE*V[11];
  fC[23] = 0.f;
  fC[24] = HE*( V[12]*pz + a*cqpz );
  fC[25] = HE*( V[13]*pz + b*cqpz );
  fC[26] = HE*cqpz;
  fC[27] = HE*HE*V[14];	    
  fC[28] = fC[29] = fC[30] = fC[31] = fC[32] = fC[33] = fC[34] = 0;
  fC[35] = 1.f;

  for(int j=0; j<fvecLen; j++)
    fQ[j] = (qp[j]>0.) ?1 :( (qp[j]<0) ?-1 :0);
  fAtProductionVertex = 1;
  fIsVtxGuess = false;
  fIsVtxErrGuess = false;
  fIsLinearized = false;

  fMassHypo = TrMass;
}

KFParticleSIMD::KFParticleSIMD(CbmKFTrackInterface &Track, Int_t *qHypo, const Int_t *pdg)
    : fField()
{
  fDaughterIds.push_back( Track.Id() );
  
  fvec m[6];
  fvec V[15];
  fvec TrMass;

  CbmKFTrack Tr(Track);
  for(unsigned int i=0; i<6; i++)
    m[i] = Tr.GetTrack()[i];
  for(unsigned int i=0; i<15; i++)
    V[i] = Tr.GetCovMatrix()[i];
//  TrMass = pdg ? TDatabasePDG::Instance()->GetParticle(*pdg)->Mass() :Tr.GetMass();
  TrMass = pdg ? CbmKFParticleDatabase::Instance()->GetMass(*pdg) :Tr.GetMass();

  fNDF  = Tr.GetRefNDF();
  fChi2 = Tr.GetRefChi2();

  fvec a = m[2], b = m[3], qp = m[4];
  
  //* convert the track to (x,y,px,py,pz,e,t/p) parameterization

  fvec c2 = 1./(1. + a*a + b*b);
  fvec pq = 1./qp;
  fvec p2 = pq*pq;
  fvec pz = sqrt(p2*c2);
  fvec px = a*pz;
  fvec py = b*pz;
  fvec eE = sqrt( TrMass*TrMass + p2 );
    
  fvec H[3] = { -px*c2, -py*c2, -pz*pq };
  fvec HE = -pq*p2/eE;

  fP[0] = m[0];
  fP[1] = m[1];
  fP[2] = m[5];
  fP[3] = px;
  fP[4] = py;
  fP[5] = pz;
  fP[6] = eE;
  fP[7] = 0;

  fvec cxpz = H[0]*V[ 3] + H[1]*V[ 6] + H[2]*V[10];
  fvec cypz = H[0]*V[ 4] + H[1]*V[ 7] + H[2]*V[11];
  fvec capz = H[0]*V[ 5] + H[1]*V[ 8] + H[2]*V[12];
  fvec cbpz = H[0]*V[ 8] + H[1]*V[ 9] + H[2]*V[13];
  fvec cqpz = H[0]*V[12] + H[1]*V[13] + H[2]*V[14];
  fvec cpzpz = H[0]*H[0]*V[5] +H[1]*H[1]*V[9] + H[2]*H[2]*V[14] 
    + 2*( H[0]*H[1]*V[8] +H[0]*H[2]*V[12] +H[1]*H[2]*V[13]);

  fC[ 0] = V[0];
  fC[ 1] = V[1];
  fC[ 2] = V[2];
  fC[ 3] = 0.f;
  fC[ 4] = 0.f;
  fC[ 5] = 0.f;
  fC[ 6] = V[3]*pz + a*cxpz; 
  fC[ 7] = V[4]*pz + a*cypz; 
  fC[ 8] = 0.f;
  fC[ 9] = V[5]*pz*pz + 2*a*pz*capz + a*a*cpzpz;
  fC[10] = V[6]*pz+b*cxpz; 
  fC[11] = V[7]*pz+b*cypz; 
  fC[12] = 0.f;
  fC[13] = V[8]*pz*pz + a*pz*cbpz + b*pz*capz + a*b*cpzpz;
  fC[14] = V[9]*pz*pz + 2*b*pz*cbpz + b*b*cpzpz;
  fC[15] = cxpz; 
  fC[16] = cypz; 
  fC[17] = 0.f;
  fC[18] = capz*pz + a*cpzpz;
  fC[19] = cbpz*pz + b*cpzpz;
  fC[20] = cpzpz;
  fC[21] = HE*V[10];
  fC[22] = HE*V[11];
  fC[23] = 0.f;
  fC[24] = HE*( V[12]*pz + a*cqpz );
  fC[25] = HE*( V[13]*pz + b*cqpz );
  fC[26] = HE*cqpz;
  fC[27] = HE*HE*V[14];	    
  fC[28] = fC[29] = fC[30] = fC[31] = fC[32] = fC[33] = fC[34] = 0;
  fC[35] = 1.f;

  for(int j=0; j<fvecLen; j++)
    fQ[j] = (qp[j]>0.) ?1 :( (qp[j]<0) ?-1 :0);
  fAtProductionVertex = 1;
  fIsVtxGuess = false;
  fIsVtxErrGuess = false;
  fIsLinearized = false;

  fMassHypo = TrMass;
}

KFParticleSIMD::KFParticleSIMD( CbmKFVertexInterface &vertex )
    : fField()
{
  // Constructor from CBM KF vertex
  fP[0] = vertex.GetRefX();
  fP[1] = vertex.GetRefY();
  fP[2] = vertex.GetRefZ();
  for( Int_t i=0; i<6; i++)
    fC[i] = vertex.GetCovMatrix( )[i];
  fChi2 = vertex.GetRefChi2();
  fNDF = vertex.GetRefNDF();//2*vertex.GetNContributors() - 3;
  fQ = 0;
  fAtProductionVertex = 0;
  fIsLinearized = 0;
  fSFromDecay = 0;
}

#endif

#ifdef HomogeneousField
KFParticleSIMD::KFParticleSIMD( const AliVTrack *track, Int_t PID )
{
  // Constructor from ALICE track, PID hypothesis should be provided

  Double_t r[3];
  Double_t C[21];

  for(Int_t iPart = 0; iPart<fvecLen; iPart++)
  {
    track[iPart].XvYvZv(r);
    for(Int_t i=0; i<3; i++)
      fP[i][iPart] = r[i];
    track[iPart].PxPyPz(r);
    for(Int_t i=0; i<3; i++)
      fP[i+3][iPart] = r[i];
    fQ[iPart] = track[iPart].Charge();
    track[iPart].GetCovarianceXYZPxPyPz( C );
    for(Int_t i=0; i<21; i++)
      fC[i][iPart] = C[i];
  }
  Create(fP,fC,fQ,PID);
}

KFParticleSIMD::KFParticleSIMD( const AliVVertex &vertex )
{
  // Constructor from ALICE vertex

  Double_t r[3];
  Double_t C[21];

  vertex.GetXYZ( r );
  for(Int_t i=0; i<3; i++)
    fP[i] = r[i];
  vertex.GetCovarianceMatrix( C );
  for(Int_t i=0; i<21; i++)
    fC[i] = C[i];
  fChi2 = vertex.GetChi2();
  fNDF = 2*vertex.GetNContributors() - 3;
  fQ = Zero;
  fAtProductionVertex = 0;
  fIsLinearized = 0;
  fSFromDecay = 0;
}

void KFParticleSIMD::GetExternalTrackParam( const KFParticleBaseSIMD &p, Double_t X[fvecLen], Double_t Alpha[fvecLen], Double_t P[5][fvecLen] ) 
{
  // Conversion to AliExternalTrackParam parameterization

  fvec cosA = p.GetPx(), sinA = p.GetPy(); 
  fvec pt = sqrt(cosA*cosA + sinA*sinA);
  fvec pti = Zero;
  fvec mask = fvec(pt < fvec(1.e-4));
  pti = (!mask) & (One/pt);
  cosA = if3(mask, One, cosA*pti);
  sinA = if3(mask, One, sinA*pti);

  fvec AlphaSIMD = atan2SIMD(sinA,cosA);
  fvec XSIMD     = p.GetX()*cosA + p.GetY()*sinA;
  fvec PSIMD[5];
  PSIMD[0]= p.GetY()*cosA - p.GetX()*sinA;
  PSIMD[1]= p.GetZ();
  PSIMD[2]= Zero;
  PSIMD[3]= p.GetPz()*pti;
  PSIMD[4]= p.GetQ()*pti;

  for(int iPart=0; iPart<fvecLen; iPart++)
  {
    X[iPart] = XSIMD[iPart];
    Alpha[iPart] = AlphaSIMD[iPart];
    P[0][iPart] = PSIMD[0][iPart];
    P[1][iPart] = PSIMD[1][iPart];
    P[2][iPart] = PSIMD[2][iPart];
    P[3][iPart] = PSIMD[3][iPart];
    P[4][iPart] = PSIMD[4][iPart];
  }
}

#endif

KFParticleSIMD::KFParticleSIMD(KFParticle* parts[], const int nPart)
#ifdef NonhomogeneousField
  : fField()
#endif
{
  { // check
    bool ok = 1;
    const int nD = (parts[0])->NDaughters();
    for ( int ie = 1; ie < nPart; ie++ ) {
      const KFParticle &part = *(parts[ie]);
      ok &= part.NDaughters() == nD;
    }
//    assert(ok);
    if (!ok) {
      std::cout << " void CbmKFParticle_simd::Create(CbmKFParticle *parts[], int N) " << std::endl;
      exit(1);
    }
  }
  
  fDaughterIds.resize( (parts[0])->NDaughters(), fvec(-1) );

  for ( int ie = 0; ie < nPart; ie++ ) {
    KFParticle &part = *(parts[ie]);

    fId[ie] = part.Id();
    for(int iD=0; iD<(parts[0])->NDaughters(); iD++)
      fDaughterIds[iD][ie] = part.DaughterIds()[iD];

    fPDG = part.GetPDG();
    
    for( int i = 0; i < 8; ++i )
      fP[i][ie] = part.Parameters()[i];
    for( int i = 0; i < 36; ++i )
      fC[i][ie] = part.CovarianceMatrix()[i];

    fNDF = part.GetNDF();
    fChi2[ie] = part.GetChi2();
    fQ[ie] = part.GetQ();
    fAtProductionVertex = part.GetAtProductionVertex(); // CHECKME

    fMassHypo[ie] = part.GetMassHypo();

    L1FieldRegion field(part.GetFieldCoeff());
    fField.SetOneEntry( ie, field, 0 );
  }
}

KFParticleSIMD::KFParticleSIMD( KFParticle &part)
#ifdef NonhomogeneousField
  : fField()
#endif
{

 fId = part.Id();
 fNDF = part.GetNDF();
 fChi2 = part.GetChi2();
 fQ = part.GetQ();
 fPDG = part.GetPDG();
 fAtProductionVertex = part.GetAtProductionVertex();
 fIsVtxGuess = 0;
 fIsVtxErrGuess = 0;

  SetNDaughters(part.NDaughters());
  for( int i = 0; i < part.NDaughters(); ++i ) {
    fDaughterIds.push_back( part.DaughterIds()[i] );
  }
  
  for( int i = 0; i < 8; ++i )
    fP[i] = part.Parameters()[i];
  for( int i = 0; i < 36; ++i )
    fC[i] = part.CovarianceMatrix()[i];

  fMassHypo = part.GetMassHypo();

  fField = L1FieldRegion(part.GetFieldCoeff());
}

fvec KFParticleSIMD::GetDistanceFromVertexXY( const fvec vtx[], const fvec Cv[], fvec &val, fvec &err ) const
{
  //* Calculate DCA distance from vertex (transverse impact parameter) in XY
  //* v = [xy], Cv=[Cxx,Cxy,Cyy ]-covariance matrix

  fvec ret = Zero;
  
  fvec mP[8];
  fvec mC[36];
  
  Transport( GetDStoPoint(vtx), mP, mC );  

  fvec dx = mP[0] - vtx[0];
  fvec dy = mP[1] - vtx[1];
  fvec px = mP[3];
  fvec py = mP[4];
  fvec pt = sqrt(px*px + py*py);
  fvec ex=Zero, ey=Zero;
  fvec mask = fvec( pt < fvec(1.e-4) );
  ret = mask;
  pt = if3(mask, One, pt);
  ex = (!mask) & (px/pt);
  ey = (!mask) & (py/pt);
  val = if3(mask, fvec(1.e4), dy*ex - dx*ey);

  fvec h0 = -ey;
  fvec h1 = ex;
  fvec h3 = (dy*ey + dx*ex)*ey/pt;
  fvec h4 = -(dy*ey + dx*ex)*ex/pt;
  
  err = 
    h0*(h0*GetCovariance(0,0) + h1*GetCovariance(0,1) + h3*GetCovariance(0,3) + h4*GetCovariance(0,4) ) +
    h1*(h0*GetCovariance(1,0) + h1*GetCovariance(1,1) + h3*GetCovariance(1,3) + h4*GetCovariance(1,4) ) +
    h3*(h0*GetCovariance(3,0) + h1*GetCovariance(3,1) + h3*GetCovariance(3,3) + h4*GetCovariance(3,4) ) +
    h4*(h0*GetCovariance(4,0) + h1*GetCovariance(4,1) + h3*GetCovariance(4,3) + h4*GetCovariance(4,4) );

  if( Cv ){
    err+= h0*(h0*Cv[0] + h1*Cv[1] ) + h1*(h0*Cv[1] + h1*Cv[2] ); 
  }

  err = sqrt(fabs(err));

  return ret;
}

fvec KFParticleSIMD::GetDistanceFromVertexXY( const fvec vtx[], fvec &val, fvec &err ) const
{
  return GetDistanceFromVertexXY( vtx, 0, val, err );
}


fvec KFParticleSIMD::GetDistanceFromVertexXY( const KFParticleSIMD &Vtx, fvec &val, fvec &err ) const 
{
  //* Calculate distance from vertex [cm] in XY-plane

  return GetDistanceFromVertexXY( Vtx.fP, Vtx.fC, val, err );
}

#ifdef HomogeneousField
fvec KFParticleSIMD::GetDistanceFromVertexXY( const AliVVertex &Vtx, fvec &val, fvec &err ) const 
{
  //* Calculate distance from vertex [cm] in XY-plane

  return GetDistanceFromVertexXY( KFParticleSIMD(Vtx), val, err );
}
#endif

fvec KFParticleSIMD::GetDistanceFromVertexXY( const fvec vtx[] ) const
{
  //* Calculate distance from vertex [cm] in XY-plane
  fvec val, err;
  GetDistanceFromVertexXY( vtx, 0, val, err );
  return val;
}

fvec KFParticleSIMD::GetDistanceFromVertexXY( const KFParticleSIMD &Vtx ) const 
{
  //* Calculate distance from vertex [cm] in XY-plane

  return GetDistanceFromVertexXY( Vtx.fP );
}

#ifdef HomogeneousField
fvec KFParticleSIMD::GetDistanceFromVertexXY( const AliVVertex &Vtx ) const 
{
  //* Calculate distance from vertex [cm] in XY-plane

  return GetDistanceFromVertexXY( KFParticleSIMD(Vtx).fP );
}
#endif

fvec KFParticleSIMD::GetDistanceFromParticleXY( const KFParticleSIMD &p ) const 
{
  //* Calculate distance to other particle [cm]

  fvec dS, dS1;
  GetDStoParticleXY( p, dS, dS1 );   
  fvec mP[8], mC[36], mP1[8], mC1[36];
  Transport( dS, mP, mC ); 
  p.Transport( dS1, mP1, mC1 ); 
  fvec dx = mP[0]-mP1[0]; 
  fvec dy = mP[1]-mP1[1]; 
  return sqrt(dx*dx+dy*dy);
}

fvec KFParticleSIMD::GetDeviationFromParticleXY( const KFParticleSIMD &p ) const 
{
  //* Calculate sqrt(Chi2/ndf) deviation from other particle

  fvec dS, dS1;
  GetDStoParticleXY( p, dS, dS1 );   
  fvec mP1[8], mC1[36];
  p.Transport( dS1, mP1, mC1 ); 

  fvec d[2]={ fP[0]-mP1[0], fP[1]-mP1[1] };

  fvec sigmaS = .1+10.*sqrt( (d[0]*d[0]+d[1]*d[1] )/
					(mP1[3]*mP1[3]+mP1[4]*mP1[4] )  );

  fvec h[2] = { mP1[3]*sigmaS, mP1[4]*sigmaS };       
  
  mC1[0] +=h[0]*h[0];
  mC1[1] +=h[1]*h[0]; 
  mC1[2] +=h[1]*h[1]; 

  return GetDeviationFromVertexXY( mP1, mC1 )*sqrt(2./1.);
}


fvec KFParticleSIMD::GetDeviationFromVertexXY( const fvec vtx[], const fvec Cv[] ) const 
{
  //* Calculate sqrt(Chi2/ndf) deviation from vertex
  //* v = [xyz], Cv=[Cxx,Cxy,Cyy,Cxz,Cyz,Czz]-covariance matrix

  fvec val, err;
  fvec problem = GetDistanceFromVertexXY( vtx, Cv, val, err );
  fvec ret = fvec(1.e4);
  fvec mask = fvec(problem | fvec(err<fvec(1.e-20)));
  ret = if3(mask, ret, val/err);
  return ret;
}


fvec KFParticleSIMD::GetDeviationFromVertexXY( const KFParticleSIMD &Vtx ) const  
{
  //* Calculate sqrt(Chi2/ndf) deviation from vertex
  //* v = [xyz], Cv=[Cxx,Cxy,Cyy,Cxz,Cyz,Czz]-covariance matrix

  return GetDeviationFromVertexXY( Vtx.fP, Vtx.fC );
}

#ifdef HomogeneousField
fvec KFParticleSIMD::GetDeviationFromVertexXY( const AliVVertex &Vtx ) const 
{
  //* Calculate sqrt(Chi2/ndf) deviation from vertex
  //* v = [xyz], Cv=[Cxx,Cxy,Cyy,Cxz,Cyz,Czz]-covariance matrix

  KFParticleSIMD v(Vtx);
  return GetDeviationFromVertexXY( v.fP, v.fC );
}
#endif

fvec KFParticleSIMD::GetAngle  ( const KFParticleSIMD &p ) const 
{
  //* Calculate the opening angle between two particles

  fvec dS, dS1;
  GetDStoParticle( p, dS, dS1 );   
  fvec mP[8], mC[36], mP1[8], mC1[36];
  Transport( dS, mP, mC ); 
  p.Transport( dS1, mP1, mC1 ); 
  fvec n = sqrt( mP[3]*mP[3] + mP[4]*mP[4] + mP[5]*mP[5] );
  fvec n1= sqrt( mP1[3]*mP1[3] + mP1[4]*mP1[4] + mP1[5]*mP1[5] );
  n*=n1;
  fvec a = Zero;
  fvec mask = fvec(n>fvec(1.e-8));
  a = ( mP[3]*mP1[3] + mP[4]*mP1[4] + mP[5]*mP1[5] )/n;
  a = mask & a;
  mask = fvec( fabs(a) < One);
  fvec aPos = fvec(a>=Zero);
  a = if3(mask, acos(a), if3(aPos, Zero, fvec(3.1415926535)) );
  return a;
}

fvec KFParticleSIMD::GetAngleXY( const KFParticleSIMD &p ) const 
{
  //* Calculate the opening angle between two particles in XY plane

  fvec dS, dS1;
  GetDStoParticleXY( p, dS, dS1 );   
  fvec mP[8], mC[36], mP1[8], mC1[36];
  Transport( dS, mP, mC ); 
  p.Transport( dS1, mP1, mC1 ); 
  fvec n = sqrt( mP[3]*mP[3] + mP[4]*mP[4] );
  fvec n1= sqrt( mP1[3]*mP1[3] + mP1[4]*mP1[4] );
  n*=n1;
  fvec a = Zero;
  fvec mask = fvec(n>fvec(1.e-8));
  a = ( mP[3]*mP1[3] + mP[4]*mP1[4] )/n;
  a = mask & a;
  mask = fvec( fabs(a) < One);
  fvec aPos = fvec(a>=Zero);
  a = if3(mask, acos(a), if3(aPos, Zero, fvec(3.1415926535)) );
  return a;
}

fvec KFParticleSIMD::GetAngleRZ( const KFParticleSIMD &p ) const 
{
  //* Calculate the opening angle between two particles in RZ plane

  fvec dS, dS1;
  GetDStoParticle( p, dS, dS1 );   
  fvec mP[8], mC[36], mP1[8], mC1[36];
  Transport( dS, mP, mC ); 
  p.Transport( dS1, mP1, mC1 ); 
  fvec nr = sqrt( mP[3]*mP[3] + mP[4]*mP[4] );
  fvec n1r= sqrt( mP1[3]*mP1[3] + mP1[4]*mP1[4]  );
  fvec n = sqrt( nr*nr + mP[5]*mP[5] );
  fvec n1= sqrt( n1r*n1r + mP1[5]*mP1[5] );
  n*=n1;
  fvec a = Zero;
  fvec mask = fvec(n>fvec(1.e-8));
  a = ( nr*n1r +mP[5]*mP1[5])/n;
  a = mask & a;
  mask = fvec( fabs(a) < One);
  fvec aPos = fvec(a>=Zero);
  a = if3(mask, acos(a), if3(aPos, Zero, fvec(3.1415926535)) );
  return a;
}


/*

#include "AliExternalTrackParam.h"

void KFParticleSIMD::GetDStoParticleALICE( const KFParticleSIMDBaseSIMD &p, 
					   fvec &DS, fvec &DS1 ) 
  const
{ 
  DS = DS1 = 0;   
  fvec x1, a1, x2, a2;
  fvec par1[5], par2[5], cov[15];
  for(int i=0; i<15; i++) cov[i] = 0;
  cov[0] = cov[2] = cov[5] = cov[9] = cov[14] = .001;

  GetExternalTrackParam( *this, x1, a1, par1 );
  GetExternalTrackParam( p, x2, a2, par2 );

  AliExternalTrackParam t1(x1,a1, par1, cov);
  AliExternalTrackParam t2(x2,a2, par2, cov);

  fvec xe1=0, xe2=0;
  t1.GetDCA( &t2, -GetFieldAlice(), xe1, xe2 );
  t1.PropagateTo( xe1, -GetFieldAlice() );
  t2.PropagateTo( xe2, -GetFieldAlice() );

  fvec xyz1[3], xyz2[3];
  t1.GetXYZ( xyz1 );
  t2.GetXYZ( xyz2 );
  
  DS = GetDStoPoint( xyz1 );
  DS1 = p.GetDStoPoint( xyz2 );

  return;
}
*/

  // * Pseudo Proper Time of decay = (r*pt) / |pt| * M/|pt|
fvec KFParticleSIMD::GetPseudoProperDecayTime( const KFParticleSIMD &pV, const fvec& mass, fvec* timeErr2 ) const
{ // TODO optimize with respect to time and stability
  const fvec ipt2 = 1/( Px()*Px() + Py()*Py() );
  const fvec mipt2 = mass*ipt2;
  const fvec dx = X() - pV.X();
  const fvec dy = Y() - pV.Y();

  if ( timeErr2 ) {
      // -- calculate error = sigma(f(r)) = f'Cf'
      // r = {x,y,px,py,x_pV,y_pV}
      // df/dr = { px*m/pt^2,
      //     py*m/pt^2,
      //    ( x - x_pV )*m*(1/pt^2 - 2(px/pt^2)^2),
      //    ( y - y_pV )*m*(1/pt^2 - 2(py/pt^2)^2),
      //     -px*m/pt^2,
      //     -py*m/pt^2 }
    const fvec f0 = Px()*mipt2;
    const fvec f1 = Py()*mipt2;
    const fvec mipt2derivative = mipt2*(1-2*Px()*Px()*ipt2);
    const fvec f2 = dx*mipt2derivative;
    const fvec f3 = -dy*mipt2derivative;
    const fvec f4 = -f0;
    const fvec f5 = -f1;

    const fvec& mC00 =    GetCovariance(0,0);
    const fvec& mC10 =    GetCovariance(0,1);
    const fvec& mC11 =    GetCovariance(1,1);
    const fvec& mC20 =    GetCovariance(3,0);
    const fvec& mC21 =    GetCovariance(3,1);
    const fvec& mC22 =    GetCovariance(3,3);
    const fvec& mC30 =    GetCovariance(4,0);
    const fvec& mC31 =    GetCovariance(4,1);
    const fvec& mC32 =    GetCovariance(4,3);
    const fvec& mC33 =    GetCovariance(4,4);
    const fvec& mC44 = pV.GetCovariance(0,0);
    const fvec& mC54 = pV.GetCovariance(1,0);
    const fvec& mC55 = pV.GetCovariance(1,1);

    *timeErr2 =
      f5*mC55*f5 +
      f5*mC54*f4 +
      f4*mC44*f4 +
      f3*mC33*f3 +
      f3*mC32*f2 +
      f3*mC31*f1 +
      f3*mC30*f0 +
      f2*mC22*f2 +
      f2*mC21*f1 +
      f2*mC20*f0 +
      f1*mC11*f1 +
      f1*mC10*f0 +
      f0*mC00*f0;
  }
  return ( dx*Px() + dy*Py() )*mipt2;
}

void KFParticleSIMD::GetKFParticle(KFParticle &Part, int iPart)
{
  Part.SetId(static_cast<int>(Id()[iPart]));

  Part.CleanDaughtersId();
  Part.SetNDaughters(DaughterIds().size());
  for( unsigned int i = 0; i < DaughterIds().size(); i++ )
    Part.AddDaughter(static_cast<int>(DaughterIds()[i][iPart]));

  Part.SetPDG( static_cast<int>(GetPDG()) );

  for(int iP=0; iP<8; iP++)
    Part.Parameters()[iP] = Parameters()[iP][iPart];
  for(int iC=0; iC<36; iC++)
    Part.CovarianceMatrix()[iC] = CovarianceMatrix()[iC][iPart];

  Part.NDF() = static_cast<int>(GetNDF()[iPart]);
  Part.Chi2() = GetChi2()[iPart];
  Part.Q()    = GetQ()[iPart];
  Part.SetAtProductionVertex(fAtProductionVertex);

  Part.SetFieldCoeff(fField.cx0[iPart], 0);
  Part.SetFieldCoeff(fField.cx1[iPart], 1);
  Part.SetFieldCoeff(fField.cx2[iPart], 2);
  Part.SetFieldCoeff(fField.cy0[iPart], 3);
  Part.SetFieldCoeff(fField.cy1[iPart], 4);
  Part.SetFieldCoeff(fField.cy2[iPart], 5);
  Part.SetFieldCoeff(fField.cz0[iPart], 6);
  Part.SetFieldCoeff(fField.cz1[iPart], 7);
  Part.SetFieldCoeff(fField.cz2[iPart], 8);
  Part.SetFieldCoeff(fField.z0[iPart],  9);
}

void KFParticleSIMD::GetKFParticle(KFParticle* Part, int nPart)
{
  for(int i=0; i<nPart; i++)
    GetKFParticle(Part[i],i);
}
