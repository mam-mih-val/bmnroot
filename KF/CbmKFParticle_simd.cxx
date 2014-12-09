//3456789012345678901234567890123456789012345678901234567890123456789012
/** Implementation of the CbmKFParticle_simd class
 *
 * @author  S.Gorbunov, I.Kisel
 * @version 1.0
 * @since   06.02.06
 *
 */

#include "CbmKFParticle_simd.h"
#include "CbmKFParticle.h"
#include "CbmKF.h"
#include "CbmKFMath.h"
#include "CbmKFTrack.h"
#include "CbmStsKFTrackFitter.h"
#include "TMath.h"

#include "CbmKFHit.h"
#include "CbmStsHit.h"
//#include "TDatabasePDG.h"

#include "CbmKFParticleDatabase.h"

//#include "CbmKFField.h"

#include "TStopwatch.h"

#include <cmath>
#include <vector>

#define cnst static const fvec

using namespace std;

CbmKFParticle_simd::CbmKFParticle_simd(): fId(-1), fDaughterIds(), NDF(0), Chi2(0), Q(0), fPDG(0), AtProductionVertex(0), fIsVtxGuess(0), fIsVtxErrGuess(0), fField()
{
  cnst ZERO = 0.0, ONE = 1.;

  for(int i=0; i<8; i++)
    r[i] = ZERO;
  for(int i=0; i<36; i++)
    C[i] = ZERO;
  for(int i=0; i<3; i++)
  {
    fVtxGuess[i] = ZERO;
    fVtxErrGuess[i] = ONE;
  }

}


CbmKFParticle_simd::CbmKFParticle_simd( CbmKFParticle &part):
 fId(part.Id()),
 fDaughterIds(),
 NDF(part.GetNDF()),
 Chi2(part.GetChi2()),
 Q(part.GetQ()),
 fPDG(part.GetPDG()),
 AtProductionVertex(part.GetAtProductionVertex()),
 fIsVtxGuess(0),
 fIsVtxErrGuess(0),
 fField()
{
  SetNDaughters(part.NDaughters());
  for( int i = 0; i < part.NDaughters(); ++i ) {
    fDaughterIds.push_back( part.DaughterIds()[i] );
  }
  
  for( int i = 0; i < 8; ++i )
    r[i] = part.GetParameters()[i];
  for( int i = 0; i < 36; ++i )
    C[i] = part.GetCovMatrix()[i];

  fField = L1FieldRegion(part.fieldRegion);
}


CbmKFParticle_simd::CbmKFParticle_simd( CbmKFParticle *part[]): fId(-1), fDaughterIds(), NDF(0), Chi2(0), Q(0), fPDG(0), AtProductionVertex(0), fIsVtxGuess(0), fIsVtxErrGuess(0), fField(){
  Create(part);
}

void CbmKFParticle_simd::Create(CbmKFParticle *parts[], int N) {

  { // check
    bool ok = 1;
    const int nD = (parts[0])->NDaughters();
    for ( int ie = 1; ie < N; ie++ ) {
      const CbmKFParticle &part = *(parts[ie]);
      ok &= part.NDaughters() == nD;
    }
//    assert(ok);
    if (!ok) {
      std::cout << " void CbmKFParticle_simd::Create(CbmKFParticle *parts[], int N) " << std::endl;
      exit(1);
    }
  }
  
  fDaughterIds.resize( (parts[0])->NDaughters(), fvec(-1) );

  for ( int ie = 0; ie < N; ie++ ) {
    CbmKFParticle &part = *(parts[ie]);

    fId[ie] = part.Id();
    fDaughterIds.back()[ie] = part.Id();

    fPDG[ie] = part.GetPDG();
    
    for( int i = 0; i < 8; ++i )
      r[i][ie] = part.GetParameters()[i];
    for( int i = 0; i < 36; ++i )
      C[i][ie] = part.GetCovMatrix()[i];

    NDF[ie] = part.GetNDF();
    Chi2[ie] = part.GetChi2();
    Q[ie] = part.GetQ();
    AtProductionVertex = part.GetAtProductionVertex(); // CHECKME

    L1FieldRegion field(part.fieldRegion);
    fField.SetOneEntry( ie, field, 0 );
  }
}

CbmKFParticle_simd::CbmKFParticle_simd(CbmKFTrackInterface &Track, Int_t *qHypo, const Int_t *pdg): fId(-1), fDaughterIds(), NDF(0), Chi2(0), Q(0), fPDG(0), AtProductionVertex(1), fIsVtxGuess(0), fIsVtxErrGuess(0), fField()
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

  NDF  = Tr.GetRefNDF();
  Chi2 = Tr.GetRefChi2();

  fvec a = m[2], b = m[3], qp = m[4];
  
  //* convert the track to (x,y,px,py,pz,e,t/p) parameterization

  fvec c2 = 1./(1. + a*a + b*b);
  fvec pq = 1./qp;
  fvec p2 = pq*pq;
  fvec pz = sqrt(p2*c2);
  fvec px = a*pz;
  fvec py = b*pz;
  fvec E = sqrt( TrMass*TrMass + p2 );

  fvec H[3] = { -px*c2, -py*c2, -pz*pq };
  fvec HE = -pq*p2/E;
	  
  r[0] = m[0];
  r[1] = m[1];
  r[2] = m[5];
  r[3] = px;
  r[4] = py;
  r[5] = pz;
  r[6] = E;
  r[7] = 0;

  fvec cxpz = H[0]*V[ 3] + H[1]*V[ 6] + H[2]*V[10];
  fvec cypz = H[0]*V[ 4] + H[1]*V[ 7] + H[2]*V[11];
  fvec capz = H[0]*V[ 5] + H[1]*V[ 8] + H[2]*V[12];
  fvec cbpz = H[0]*V[ 8] + H[1]*V[ 9] + H[2]*V[13];
  fvec cqpz = H[0]*V[12] + H[1]*V[13] + H[2]*V[14];
  fvec cpzpz = H[0]*H[0]*V[5] +H[1]*H[1]*V[9] + H[2]*H[2]*V[14] 
    + 2*( H[0]*H[1]*V[8] +H[0]*H[2]*V[12] +H[1]*H[2]*V[13]);

  C[ 0] = V[0];
  C[ 1] = V[1];
  C[ 2] = V[2];
  C[ 3] = 0;
  C[ 4] = 0;
  C[ 5] = 0;
  C[ 6] = V[3]*pz + a*cxpz; 
  C[ 7] = V[4]*pz + a*cypz; 
  C[ 8] = 0;
  C[ 9] = V[5]*pz*pz + 2*a*pz*capz + a*a*cpzpz;
  C[10] = V[6]*pz+b*cxpz; 
  C[11] = V[7]*pz+b*cypz; 
  C[12] = 0;
  C[13] = V[8]*pz*pz + a*pz*cbpz + b*pz*capz + a*b*cpzpz;
  C[14] = V[9]*pz*pz + 2*b*pz*cbpz + b*b*cpzpz;
  C[15] = cxpz; 
  C[16] = cypz; 
  C[17] = 0;
  C[18] = capz*pz + a*cpzpz;
  C[19] = cbpz*pz + b*cpzpz;
  C[20] = cpzpz;
  C[21] = HE*V[10];
  C[22] = HE*V[11];
  C[23] = 0;
  C[24] = HE*( V[12]*pz + a*cqpz );
  C[25] = HE*( V[13]*pz + b*cqpz );
  C[26] = HE*cqpz;
  C[27] = HE*HE*V[14];	    
  C[28] = C[29] = C[30] = C[31] = C[32] = C[33] = C[34] = 0;
  C[35] = 1.;

  for(int j=0; j<fvecLen; j++)
    Q[j] = (qp[j]>0.) ?1 :( (qp[j]<0) ?-1 :0);
}

CbmKFParticle_simd::CbmKFParticle_simd( CbmKFTrackInterface* Track[], Int_t *qHypo, const Int_t *pdg): fId(-1), fDaughterIds(), NDF(0), Chi2(0), Q(0), fPDG(0), AtProductionVertex(0), fIsVtxGuess(0), fIsVtxErrGuess(0), fField()
{
  Create(Track,fvecLen,qHypo,pdg);
}

void CbmKFParticle_simd::Create(CbmKFTrackInterface* Track[], int NTracks, Int_t *qHypo, const Int_t *pdg)
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

    NDF[j] = Track[j]->GetRefNDF();
    Chi2[j] = Track[j]->GetRefChi2();

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
  fvec E = sqrt( TrMass*TrMass + p2 );
    
  fvec H[3] = { -px*c2, -py*c2, -pz*pq };
  fvec HE = -pq*p2/E;
	  
  r[0] = m[0];
  r[1] = m[1];
  r[2] = m[5];
  r[3] = px;
  r[4] = py;
  r[5] = pz;
  r[6] = E;
  r[7] = 0;

  fvec cxpz = H[0]*V[ 3] + H[1]*V[ 6] + H[2]*V[10];
  fvec cypz = H[0]*V[ 4] + H[1]*V[ 7] + H[2]*V[11];
  fvec capz = H[0]*V[ 5] + H[1]*V[ 8] + H[2]*V[12];
  fvec cbpz = H[0]*V[ 8] + H[1]*V[ 9] + H[2]*V[13];
  fvec cqpz = H[0]*V[12] + H[1]*V[13] + H[2]*V[14];
  fvec cpzpz = H[0]*H[0]*V[5] +H[1]*H[1]*V[9] + H[2]*H[2]*V[14] 
    + 2*( H[0]*H[1]*V[8] +H[0]*H[2]*V[12] +H[1]*H[2]*V[13]);

  C[ 0] = V[0];
  C[ 1] = V[1];
  C[ 2] = V[2];
  C[ 3] = 0.f;
  C[ 4] = 0.f;
  C[ 5] = 0.f;
  C[ 6] = V[3]*pz + a*cxpz; 
  C[ 7] = V[4]*pz + a*cypz; 
  C[ 8] = 0.f;
  C[ 9] = V[5]*pz*pz + 2*a*pz*capz + a*a*cpzpz;
  C[10] = V[6]*pz+b*cxpz; 
  C[11] = V[7]*pz+b*cypz; 
  C[12] = 0.f;
  C[13] = V[8]*pz*pz + a*pz*cbpz + b*pz*capz + a*b*cpzpz;
  C[14] = V[9]*pz*pz + 2*b*pz*cbpz + b*b*cpzpz;
  C[15] = cxpz; 
  C[16] = cypz; 
  C[17] = 0.f;
  C[18] = capz*pz + a*cpzpz;
  C[19] = cbpz*pz + b*cpzpz;
  C[20] = cpzpz;
  C[21] = HE*V[10];
  C[22] = HE*V[11];
  C[23] = 0.f;
  C[24] = HE*( V[12]*pz + a*cqpz );
  C[25] = HE*( V[13]*pz + b*cqpz );
  C[26] = HE*cqpz;
  C[27] = HE*HE*V[14];	    
  C[28] = C[29] = C[30] = C[31] = C[32] = C[33] = C[34] = 0;
  C[35] = 1.f;

  for(int j=0; j<fvecLen; j++)
    Q[j] = (qp[j]>0.) ?1 :( (qp[j]<0) ?-1 :0);
  AtProductionVertex = 1;
  fIsVtxGuess = false;
  fIsVtxErrGuess = false;
}

void CbmKFParticle_simd::SetField(const L1FieldRegion &field, bool isOneEntry, const int iVec)
{
  if(!isOneEntry)
    fField = field;
  else
    fField.SetOneEntry(field,iVec);
}

void CbmKFParticle_simd::GetKFTrack( CbmKFTrackInterface** Track)
{
  fvec T[6],Cov[15];

  fvec px=r[3], py=r[4],  pz=r[5];
  fvec p = sqrt( px*px + py*py + pz*pz );
  fvec pzi = 1/pz;
  fvec qp=1/p;

  for(int j=0; j<fvecLen; j++)
    if( Q[j]!=0 ) qp[j] = Q[j]*qp[j];

  T[0] = r[0];   // dX = dx -T[2]*dz
  T[1] = r[1];   // dY = dy -T[3]*dz
  T[2] = px*pzi; // dtx= (dpx - T[2]*dpz)/pz
  T[3] = py*pzi; // dty= (dpy - T[3]*dpz)/pz
  T[4] = qp;     // dq = (px*dpx + py*dpy + pz*dpz)*(-q/p^3)
  T[5] = r[2];
  
  fvec qp3 = qp*qp*qp;
  fvec pzi2= pzi*pzi;

  fvec cXpz= C[15]-T[2]*C[17];
  fvec cYpz= C[16]-T[3]*C[17];
  fvec cqpx = -qp3*( px*C[ 9] + py*C[13] + pz*C[18] );
  fvec cqpy = -qp3*( px*C[13] + py*C[14] + pz*C[19] );
  fvec cqpz = -qp3*( px*C[18] + py*C[19] + pz*C[20] );

  Cov[ 0]= C[0] + T[2]*( - 2*C[3] + T[2]*C[5] );
  Cov[ 1]= C[1] - T[2]*C[4] + T[3]*( -C[3] + T[2]*C[5] );
  Cov[ 2]= C[2] + T[3]*( - 2*C[4] + T[3]*C[5]);
  Cov[ 3]= pzi*( C[6] - T[2]*(C[8]     + cXpz) );
  Cov[ 4]= pzi*( C[7] - T[3]*C[8] - T[2]*cYpz );
  Cov[ 5]= pzi2*( C[9] + T[2]*( -2*C[18] + T[2]*C[20]) );
  Cov[ 6]= pzi*( C[10]-T[2]*C[12]-T[3]*cXpz );
  Cov[ 7]= pzi*( C[11]-T[3]*(C[12]    +cYpz) );
  Cov[ 8]= pzi2*( C[13] - T[2]*C[19] -T[3]*C[18] + T[2]*T[3]*C[20] );
  Cov[ 9]= pzi2*( C[14] + T[3]*( -2*C[19] + T[3]*C[20]) );
  Cov[10]=-qp3*(px*C[6]+py*C[10]+pz*C[15]) - T[2]*cqpz;
  Cov[11]=-qp3*(px*C[7]+py*C[11]+pz*C[16]) - T[3]*cqpz;
  Cov[12]= pzi*( cqpx - T[2]*cqpz );
  Cov[13]= pzi*( cqpy - T[3]*cqpz );
  Cov[14]=-qp3*( px*cqpx + py*cqpy + pz*cqpz );

  for(int j=0; j<fvecLen; j++)
  {
    for(int i=0;i<6;i++) Track[j]->GetTrack()[i] =  T[i][j];
    for(int i=0;i<15;i++) Track[j]->GetCovMatrix()[i] = Cov[i][j];

    Track[j]->GetRefNDF() = (int) NDF[j];
    Track[j]->GetRefChi2() = Chi2[j];
  }
}


void CbmKFParticle_simd::GetMomentum( fvec &P, fvec &Error )
{
  fvec x = r[3];
  fvec y = r[4];
  fvec z = r[5];
  fvec x2 = x*x;
  fvec y2 = y*y;
  fvec z2 = z*z;
  fvec p2 = x2+y2+z2;
  P = sqrt(p2);
  Error = sqrt( (x2*C[9]+y2*C[14]+z2*C[20] 
		 + 2*(x*y*C[13]+x*z*C[18]+y*z*C[19]) )/p2 );
}

void CbmKFParticle_simd::GetMass( fvec &M, fvec &Error )
{
  // S = sigma^2 of m2/2

  fvec S = (  r[3]*r[3]*C[9] + r[4]*r[4]*C[14] + r[5]*r[5]*C[20] 
		  + r[6]*r[6]*C[27] 
		+2*( + r[3]*r[4]*C[13] + r[5]*(r[3]*C[18] + r[4]*C[19]) 
		     - r[6]*( r[3]*C[24] + r[4]*C[25] + r[5]*C[26] )   )
		 );
  fvec m2 = r[6]*r[6] - r[3]*r[3] - r[4]*r[4] - r[5]*r[5];
  for(int j=0; j<fvecLen; j++)
  {
    M[j]     = (            m2[j]>1.e-20 ) ? sqrt(m2[j])      :0.  ;
    Error[j] = ( S[j]>=0 && m2[j]>1.e-20 ) ? sqrt(S[j]/m2[j]) :1.e4;
  }
}


void CbmKFParticle_simd::GetDecayLength( fvec &L, fvec &Error )
{
  fvec x = r[3];
  fvec y = r[4];
  fvec z = r[5];
  fvec t = r[7];
  fvec x2 = x*x;
  fvec y2 = y*y;
  fvec z2 = z*z;
  fvec p2 = x2+y2+z2;
  L = t*sqrt(p2);
  Error = sqrt( p2*C[35] + t*t/p2*(x2*C[9]+y2*C[14]+z2*C[20]
				   + 2*(x*y*C[13]+x*z*C[18]+y*z*C[19]) )
		+ 2*t*(x*C[31]+y*C[32]+z*C[33]) 
		);
}

void CbmKFParticle_simd::GetLifeTime( fvec &TauC, fvec &Error ){
  fvec m, dm;
  GetMass( m, dm );
  fvec cTM = (-r[3]*C[31] - r[4]*C[32] - r[5]*C[33] + r[6]*C[34]);
  TauC = r[7]*m;
  Error = sqrt( m*m*C[35] + 2*r[7]*cTM + r[7]*r[7]*dm*dm);
}

void CbmKFParticle_simd::SetVtxGuess( fvec &x, fvec &y, fvec &z )
{
  fVtxGuess[0] = x;
  fVtxGuess[1] = y;
  fVtxGuess[2] = z;
  fIsVtxGuess = 1;
}

void CbmKFParticle_simd::SetVtxErrGuess( fvec &dx, fvec &dy, fvec &dz )
{
  fVtxErrGuess[0] = dx;
  fVtxErrGuess[1] = dy;
  fVtxErrGuess[2] = dz;
  fIsVtxErrGuess = 1;
}

#undef cnst

