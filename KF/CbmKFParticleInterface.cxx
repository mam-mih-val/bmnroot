#include "CbmKFParticleInterface.h"

#define cnst static const fvec

//for particle finding
#include "CbmL1Track.h"
#include "CbmStsTrack.h"
#include "CbmKFTrack.h"

#include <map>
using std::map;

const float CbmKFParticleInterface::DefaultCuts[2][3]  = {{3.,3.,-100.},{3.,3.,-100.}};

CbmKFParticleInterface::CbmKFParticleInterface():KFPart()
{
}

CbmKFParticleInterface::CbmKFParticleInterface(CbmKFTrackInterface* Track[]):KFPart()
{
  KFPart.Create(Track);
}

CbmKFParticleInterface::CbmKFParticleInterface(CbmKFParticle* part[], const int nPart):KFPart()
{
  KFPart.Create(part,nPart);
}

CbmKFParticleInterface::~CbmKFParticleInterface()
{
}

CbmKFParticleInterface::CbmKFParticleInterface(const CbmKFParticleInterface& c):KFPart()
{
  KFPart = c.KFPart;
}

CbmKFParticleInterface CbmKFParticleInterface::operator=(const CbmKFParticleInterface& c)
{
  KFPart = c.KFPart;
  return c;
}

void CbmKFParticleInterface::Construct(CbmKFTrackInterface* vDaughters[][fvecLen],int NDaughters,
                                       CbmKFVertexInterface *Parent,float Mass,float CutChi2)
{
  const int ND = NDaughters;

  CbmKFParticle_simd* vDaughters_part = new CbmKFParticle_simd[ND];

  for( int tr=0; tr<NDaughters; ++tr )
    vDaughters_part[tr].Create(vDaughters[tr]);

  Construct( vDaughters_part, NDaughters, Parent, Mass, CutChi2 );
  delete[] vDaughters_part;
}

void CbmKFParticleInterface::Construct( CbmKFParticle_simd* vDaughters, int NDaughters,
                                        CbmKFVertexInterface *Parent, float Mass, float CutChi2, bool isAtVtxGuess )
{
  KFPart.CleanDaughtersId();
  KFPart.SetNDaughters(NDaughters);
  for( int tr=0; tr<NDaughters; tr++ )
  {
    KFPart.AddDaughterId( vDaughters[tr].Id() );
  }

  int MaxIter=3;
  if(!(KFPart.fIsVtxGuess))
  {
    fvec Ty[2];
    fvec Y[2];
    fvec Z[2];

    for( int tr=0; tr<2; tr++ )
    {
      Y[tr]  = vDaughters[tr].GetY();
      Z[tr]  = vDaughters[tr].GetZ();
      Ty[tr] = vDaughters[tr].GetPy()/vDaughters[tr].GetPz();
    }

    fvec r0[3];
    r0[0] = 0.;
    r0[1] = 0.;
    r0[2] = (Ty[0]*Z[0]-Ty[1]*Z[1] + Y[1] -Y[0])/(Ty[0]-Ty[1]);

    CbmKFParticle_simd Daughter0 = vDaughters[0];
    Extrapolate( &Daughter0, Daughter0.r , GetDStoPoint(Daughter0, r0) );

    KFPart.fVtxGuess[0] = Daughter0.GetX();
    KFPart.fVtxGuess[1] = Daughter0.GetY();
    KFPart.fVtxGuess[2] = Daughter0.GetZ();

    if(!(KFPart.fIsVtxErrGuess))
    {
      KFPart.fVtxErrGuess[0] = 10.*sqrt(Daughter0.C[0] );
      KFPart.fVtxErrGuess[1] = 10.*sqrt(Daughter0.C[2] );
      KFPart.fVtxErrGuess[2] = 10.*sqrt(Daughter0.C[5] );
    }
 //   KFPart.fVtxErrGuess[0] = 0.01*KFPart.fVtxGuess[0];
 //   KFPart.fVtxErrGuess[1] = 0.01*KFPart.fVtxGuess[1];
 //   KFPart.fVtxErrGuess[2] = 0.01*KFPart.fVtxGuess[2];
  }
  else
  {
    MaxIter = 1;
  }

  cnst ZERO = 0.0, ONE = 1.;
  cnst TWO = 2.;	
  /**if( CbmKF::Instance()->vTargets.empty() ){
    r[0] = r[1] = r[2] = 0.;
    C[0] = C[2] = C[5] = 1.;
    C[1] = C[3] = C[4] = 0;
  }else{
    CbmKFTube &t = CbmKF::Instance()->vTargets[0];
    r[0] = r[1] = 0.;
    r[2] = t.z;
    C[0] = C[2] = t.R*t.R/9.;
    C[5] = t.dz/6.;
    C[1] = C[3] = C[4] = 0;
  }**/

  KFPart.r[3] = ZERO;
  KFPart.r[4] = ZERO;
  KFPart.r[5] = ZERO;
  KFPart.r[6] = ZERO;
  KFPart.r[7] = ZERO;

  KFPart.r[0] = KFPart.fVtxGuess[0];
  KFPart.r[1] = KFPart.fVtxGuess[1];
  KFPart.r[2] = KFPart.fVtxGuess[2];

  KFPart.C[0] = KFPart.fVtxErrGuess[0] * KFPart.fVtxErrGuess[0];
  KFPart.C[2] = KFPart.fVtxErrGuess[1] * KFPart.fVtxErrGuess[1];
  KFPart.C[5] = KFPart.fVtxErrGuess[2] * KFPart.fVtxErrGuess[2];
  KFPart.C[1] = KFPart.C[3] = KFPart.C[4] = ZERO;


  for ( Int_t iteration =0; iteration<MaxIter;++iteration )
  {
    fvec r0[8], C0[6];

    for( int i=0; i<8; i++) r0[i] = KFPart.r[i];
    for( int i=0; i<6; i++) C0[i] = KFPart.C[i];

    KFPart.r[3] = ZERO;
    KFPart.r[4] = ZERO;
    KFPart.r[5] = ZERO;
    KFPart.r[6] = ZERO;

    for(Int_t i=0;i<36;++i) KFPart.C[i]=ZERO;

    KFPart.C[0] = KFPart.fVtxErrGuess[0]*KFPart.fVtxErrGuess[0];
    KFPart.C[2] = KFPart.fVtxErrGuess[1]*KFPart.fVtxErrGuess[1];
    KFPart.C[5] = KFPart.fVtxErrGuess[2]*KFPart.fVtxErrGuess[2];

    KFPart.C[35] = ONE;

    L1FieldValue B = vDaughters[0].fField.Get(r0[2]);
    B.x *= 0.000299792458; //multiply by c
    B.y *= 0.000299792458;
    B.z *= 0.000299792458;

    KFPart.NDF  =  -3.;
    KFPart.Chi2 =  ZERO;
    KFPart.Q = ZERO;

    for( int tr=0; tr<NDaughters; ++tr )
      {
	CbmKFParticle_simd Daughter = vDaughters[tr] ;
        if(!isAtVtxGuess)
	  Extrapolate(&Daughter, Daughter.r , GetDStoPoint(Daughter, r0));

	fvec *m = Daughter.r;
	fvec *Cd = Daughter.C;

	fvec d[3] = { r0[0]-m[0], r0[1]-m[1], r0[2]-m[2] };
	fvec SigmaS = .1+10.*sqrt( (d[0]*d[0]+d[1]*d[1]+d[2]*d[2])/
				       (m[3]*m[3]+m[4]*m[4]+m[5]*m[5])  );

	fvec h[6];

	h[0] = m[3]*SigmaS;
	h[1] = m[4]*SigmaS;
	h[2] = m[5]*SigmaS; 
	h[3] = ( h[1]*B.z-h[2]*B.y )*Daughter.Q;
	h[4] = ( h[2]*B.x-h[0]*B.z )*Daughter.Q;
	h[5] = ( h[0]*B.y-h[1]*B.x )*Daughter.Q;

	//* Fit of daughter momentum (x,y,z) to r0[0,1,2] vertex
	{
	  fvec zeta[3] = { r0[0]-m[0], r0[1]-m[1], r0[2]-m[2] };

	  fvec Vv[6] = 
	    { Cd[ 0] + h[0]*h[0], 
	      Cd[ 1] + h[1]*h[0], Cd[ 2] + h[1]*h[1],			     
	      Cd[ 3] + h[2]*h[0], Cd[ 4] + h[2]*h[1], Cd[ 5] + h[2]*h[2] };
	  
	  fvec Vvp[9]=
	    { Cd[ 6] + h[0]*h[3], Cd[ 7] + h[1]*h[3], Cd[ 8] + h[2]*h[3],
	      Cd[10] + h[0]*h[4], Cd[11] + h[1]*h[4], Cd[12] + h[2]*h[4],
	      Cd[15] + h[0]*h[5], Cd[16] + h[1]*h[5], Cd[17] + h[2]*h[5] };

/**	  if( CutChi2 >0. ){

	    fvec Si[6]={ Vv[0]+C0[0],
			     Vv[1]+C0[1], Vv[2]+C0[2],
			     Vv[3]+C0[3], Vv[4]+C0[4], Vv[5]+C0[5] };
	    fvec S[6] = { Si[2]*Si[5] - Si[4]*Si[4],
			      Si[3]*Si[4] - Si[1]*Si[5],
			      Si[0]*Si[5] - Si[3]*Si[3],
			      Si[1]*Si[4] - Si[2]*Si[3],
			      Si[1]*Si[3] - Si[0]*Si[4],
			      Si[0]*Si[2] - Si[1]*Si[1] };		 	
	    fvec det = ( Si[0]*S[0] + Si[1]*S[1] + Si[3]*S[3] );
	    fvec chi2 = 
	      ( +(S[0]*zeta[0] + S[1]*zeta[1] + S[3]*zeta[2])*zeta[0]
		+(S[1]*zeta[0] + S[2]*zeta[1] + S[4]*zeta[2])*zeta[1]
		+(S[3]*zeta[0] + S[4]*zeta[1] + S[5]*zeta[2])*zeta[2]);
	    if( chi2 > 2*CutChi2*det ) continue;
	  }
**/
	  fvec S[6] = 
	    { Vv[2]*Vv[5] - Vv[4]*Vv[4],
	      Vv[3]*Vv[4] - Vv[1]*Vv[5], Vv[0]*Vv[5] - Vv[3]*Vv[3],
	      Vv[1]*Vv[4] - Vv[2]*Vv[3], Vv[1]*Vv[3] - Vv[0]*Vv[4], 
	      Vv[0]*Vv[2] - Vv[1]*Vv[1] };		 
	
	  fvec s = ( Vv[0]*S[0] + Vv[1]*S[1] + Vv[3]*S[3] );

          fvec init = fvec(s > 1.E-20);
          s = rcp(s) & init;
//          for(int j = 0; j<fvecLen; j++)
//	    s[j] = ( s[j] > 1.E-20 )  ?1./s[j] :0;

	  S[0]*=s; S[1]*=s; S[2]*=s; S[3]*=s; S[4]*=s; S[5]*=s;

	  fvec Sz[3] = { (S[0]*zeta[0]+S[1]*zeta[1]+S[3]*zeta[2]),
			     (S[1]*zeta[0]+S[2]*zeta[1]+S[4]*zeta[2]),
			     (S[3]*zeta[0]+S[4]*zeta[1]+S[5]*zeta[2]) };
	  
	  fvec x = m[3] + Vvp[0]*Sz[0] + Vvp[1]*Sz[1] + Vvp[2]*Sz[2];	  
	  fvec y = m[4] + Vvp[3]*Sz[0] + Vvp[4]*Sz[1] + Vvp[5]*Sz[2];	  
	  fvec z = m[5] + Vvp[6]*Sz[0] + Vvp[7]*Sz[1] + Vvp[8]*Sz[2];	  

	  h[0] = x*SigmaS;
	  h[1] = y*SigmaS;
	  h[2] = z*SigmaS; 
	  h[3] = ( h[1]*B.z-h[2]*B.y )*Daughter.Q;
	  h[4] = ( h[2]*B.x-h[0]*B.z )*Daughter.Q;
	  h[5] = ( h[0]*B.y-h[1]*B.x )*Daughter.Q;
	}
	
	fvec* V = Cd;

	V[ 0] += h[0]*h[0];
	V[ 1] += h[1]*h[0];
	V[ 2] += h[1]*h[1];
	V[ 3] += h[2]*h[0];
	V[ 4] += h[2]*h[1];
	V[ 5] += h[2]*h[2];
	
	V[ 6] += h[3]*h[0];
	V[ 7] += h[3]*h[1];
	V[ 8] += h[3]*h[2];
	V[ 9] += h[3]*h[3];

	V[10] += h[4]*h[0];
	V[11] += h[4]*h[1];
	V[12] += h[4]*h[2];
	V[13] += h[4]*h[3];
	V[14] += h[4]*h[4];

	V[15] += h[5]*h[0];
	V[16] += h[5]*h[1];
	V[17] += h[5]*h[2];
	V[18] += h[5]*h[3];
	V[19] += h[5]*h[4];
	V[20] += h[5]*h[5];

// 	V[21] = Cd[21];
// 	V[22] = Cd[22];
// 	V[23] = Cd[23];
// 	V[24] = Cd[24];
// 	V[25] = Cd[25];
// 	V[26] = Cd[26];
// 	V[27] = Cd[27];

	//* 
	KFPart.NDF += TWO;
	KFPart.Q   += Daughter.Q;

	//*

	fvec S[6];
	{
	  fvec Si[6] = { KFPart.C[0]+V[0], 
			   KFPart.C[1]+V[1], KFPart.C[2]+V[2], 
			   KFPart.C[3]+V[3], KFPart.C[4]+V[4], KFPart.C[5]+V[5] };
	  
	  S[0] = Si[2]*Si[5] - Si[4]*Si[4];
	  S[1] = Si[3]*Si[4] - Si[1]*Si[5];
	  S[2] = Si[0]*Si[5] - Si[3]*Si[3];
	  S[3] = Si[1]*Si[4] - Si[2]*Si[3];
	  S[4] = Si[1]*Si[3] - Si[0]*Si[4];
	  S[5] = Si[0]*Si[2] - Si[1]*Si[1];	 
	
	  fvec s = ( Si[0]*S[0] + Si[1]*S[1] + Si[3]*S[3] );

          fvec init = fvec(s > 1.E-20);
          s = rcp(s) & init;
//          for(int j=0; j<fvecLen; j++)
//	    s[j] = ( s[j] > 1.E-20 )  ?1./s[j] :0;	  

	  S[0]*=s;
	  S[1]*=s;
	  S[2]*=s;
	  S[3]*=s;
	  S[4]*=s;
	  S[5]*=s;
	}
	
	//* Residual (measured - estimated)

	fvec zeta[3] = { m[0]-KFPart.r[0], m[1]-KFPart.r[1], m[2]-KFPart.r[2] };
	
	//* Add the daughter momentum to the particle momentum

	KFPart.r[ 3] += m[ 3];
	KFPart.r[ 4] += m[ 4];
	KFPart.r[ 5] += m[ 5];
	KFPart.r[ 6] += m[ 6];

	KFPart.C[ 9] += V[ 9];
	KFPart.C[13] += V[13];
	KFPart.C[14] += V[14];
	KFPart.C[18] += V[18];
	KFPart.C[19] += V[19];
	KFPart.C[20] += V[20];
	KFPart.C[24] += V[24];
	KFPart.C[25] += V[25];
	KFPart.C[26] += V[26];
	KFPart.C[27] += V[27];

	//* CHt = CH' - D'
	
	fvec CHt0[7], CHt1[7], CHt2[7];

	CHt0[0]=KFPart.C[ 0] ;      CHt1[0]=KFPart.C[ 1] ;      CHt2[0]=KFPart.C[ 3] ;
	CHt0[1]=KFPart.C[ 1] ;      CHt1[1]=KFPart.C[ 2] ;      CHt2[1]=KFPart.C[ 4] ;
	CHt0[2]=KFPart.C[ 3] ;      CHt1[2]=KFPart.C[ 4] ;      CHt2[2]=KFPart.C[ 5] ;
	CHt0[3]=KFPart.C[ 6]-V[ 6]; CHt1[3]=KFPart.C[ 7]-V[ 7]; CHt2[3]=KFPart.C[ 8]-V[ 8];
	CHt0[4]=KFPart.C[10]-V[10]; CHt1[4]=KFPart.C[11]-V[11]; CHt2[4]=KFPart.C[12]-V[12];
	CHt0[5]=KFPart.C[15]-V[15]; CHt1[5]=KFPart.C[16]-V[16]; CHt2[5]=KFPart.C[17]-V[17];
	CHt0[6]=KFPart.C[21]-V[21]; CHt1[6]=KFPart.C[22]-V[22]; CHt2[6]=KFPart.C[23]-V[23];

	//* Kalman gain K = CH'*S
	
	fvec K0[7], K1[7], K2[7];

	for(Int_t i=0;i<7;++i){
	  K0[i] = CHt0[i]*S[0] + CHt1[i]*S[1] + CHt2[i]*S[3];
	  K1[i] = CHt0[i]*S[1] + CHt1[i]*S[2] + CHt2[i]*S[4];
	  K2[i] = CHt0[i]*S[3] + CHt1[i]*S[4] + CHt2[i]*S[5];
	}

	//* New estimation of the vertex position r += K*zeta

	for(Int_t i=0;i<7;++i) 
	  KFPart.r[i] += K0[i]*zeta[0] + K1[i]*zeta[1] + K2[i]*zeta[2];

	//* New covariance matrix C -= K*(CH')'

	for(Int_t i=0, k=0;i<7;++i){
	  for(Int_t j=0;j<=i;++j,++k) 
	    KFPart.C[k] -= K0[i]*CHt0[j] + K1[i]*CHt1[j] + K2[i]*CHt2[j];
	}

	//* Calculate Chi^2 & NDF

	KFPart.Chi2 += (S[0]*zeta[0] + S[1]*zeta[1] + S[3]*zeta[2])*zeta[0]
	  +     (S[1]*zeta[0] + S[2]*zeta[1] + S[4]*zeta[2])*zeta[1]
	  +     (S[3]*zeta[0] + S[4]*zeta[1] + S[5]*zeta[2])*zeta[2];

      } // add tracks 

    if( iteration==0 ){
      r0[3] = KFPart.r[3];
      r0[4] = KFPart.r[4];
      r0[5] = KFPart.r[5];
      r0[6] = KFPart.r[6];
      if( Parent ){
	fvec dx, dy, dz;
        dx = Parent->GetRefX()-KFPart.r[0];
        dy = Parent->GetRefY()-KFPart.r[1];
  	dz = Parent->GetRefZ()-KFPart.r[2];

        r0[7] = KFPart.r[7] = sqrt( (dx*dx+dy*dy+dz*dz)
			     /(KFPart.r[3]*KFPart.r[3]+KFPart.r[4]*KFPart.r[4]+KFPart.r[5]*KFPart.r[5]) );
      }
    }

    if( Mass>=0 ) 
    {
      fvec vMass = Mass;
      MeasureMass( &KFPart, r0, vMass );
    }
    if( Parent ) MeasureProductionVertex( &KFPart, r0, Parent );
  }// iterations

  // calculate a field region for the constructed particle
  L1FieldValue field[3];
  fvec zField[3] = {0, KFPart.r[2]/2, KFPart.r[2]};

  for(int iPoint=0; iPoint<3; iPoint++)
  {
    for(int iD=0; iD<NDaughters; ++iD)
    {
      L1FieldValue b = vDaughters[iD].fField.Get(zField[iPoint]);
      field[iPoint].x += b.x;
      field[iPoint].y += b.y;
      field[iPoint].z += b.z;
    }
    field[iPoint].x /= NDaughters;
    field[iPoint].y /= NDaughters;
    field[iPoint].z /= NDaughters;
  }

  KFPart.fField.Set( field[2], zField[2], field[1], zField[1], field[0], zField[0] );

  KFPart.AtProductionVertex = 0;
}

void CbmKFParticleInterface::MeasureMass(CbmKFParticle_simd*  Particle,  fvec r0[], fvec Mass )
{
  fvec *r    = Particle->GetParameters();
  fvec *C    = Particle->GetCovMatrix();

  fvec H[8] = {0};
  H[0] = H[1] = H[2] = 0.;
  H[3] = -2*r0[3]; 
  H[4] = -2*r0[4]; 
  H[5] = -2*r0[5]; 
  H[6] =  2*r0[6];
  H[7] = 0; 
  fvec m2 = r0[6]*r0[6] - r0[3]*r0[3] - r0[4]*r0[4] - r0[5]*r0[5]; 

  fvec zeta = Mass*Mass - m2;
  for(Int_t i=0;i<8;++i) zeta -= H[i]*(r[i]-r0[i]);

  fvec S = 0.;
  fvec CHt[8];
  for (Int_t i=0;i<8;++i ){
    CHt[i] = 0.0;
    for (Int_t j=0;j<8;++j) CHt[i] += Cij(i,j)*H[j];
    S += H[i]*CHt[i];
  }

  fvec init = fvec(S > 1.E-20);
  S = rcp(S) & init;

  Particle->Chi2 += zeta*zeta*S;
  Particle->NDF  += (fvec(1.) & init);
  for( Int_t i=0, ii=0; i<8; ++i ){
    fvec Ki = CHt[i]*S;
    r[i] += Ki*zeta;
    for(Int_t j=0;j<=i;++j) C[ii++] -= Ki*CHt[j];
  }
}


void CbmKFParticleInterface::MeasureProductionVertex( CbmKFParticle_simd*  Particle, fvec r0[],
                                                      CbmKFVertexInterface *Parent)
{
  fvec *r = Particle->GetParameters();
  fvec *C = Particle->GetCovMatrix();

  fvec m[3], V[6];

  m[0] = Parent->GetRefX();
  m[1] = Parent->GetRefY();
  m[2] = Parent->GetRefZ();
  double *V1 = Parent->GetCovMatrix();
  for(int i=0; i<6; i++) V[i]=V1[i];

  r[7] = r0[7];
  Extrapolate(r0, -r0[7]);
  Convert(Particle, r0, 1);
 
  fvec Ai[6];
  Ai[0] = C[4]*C[4] - C[2]*C[5];
  Ai[1] = C[1]*C[5] - C[3]*C[4];
  Ai[3] = C[2]*C[3] - C[1]*C[4];
  fvec det = 1./(C[0]*Ai[0] + C[1]*Ai[1] + C[3]*Ai[3]);
  Ai[0] *= det;
  Ai[1] *= det;
  Ai[3] *= det;
  Ai[2] = ( C[3]*C[3] - C[0]*C[5] )*det;
  Ai[4] = ( C[0]*C[4] - C[1]*C[3] )*det;
  Ai[5] = ( C[1]*C[1] - C[0]*C[2] )*det;

  fvec B[5][3];

  B[0][0] = C[ 6]*Ai[0] + C[ 7]*Ai[1] + C[ 8]*Ai[3];
  B[0][1] = C[ 6]*Ai[1] + C[ 7]*Ai[2] + C[ 8]*Ai[4];
  B[0][2] = C[ 6]*Ai[3] + C[ 7]*Ai[4] + C[ 8]*Ai[5];

  B[1][0] = C[10]*Ai[0] + C[11]*Ai[1] + C[12]*Ai[3];
  B[1][1] = C[10]*Ai[1] + C[11]*Ai[2] + C[12]*Ai[4];
  B[1][2] = C[10]*Ai[3] + C[11]*Ai[4] + C[12]*Ai[5];

  B[2][0] = C[15]*Ai[0] + C[16]*Ai[1] + C[17]*Ai[3];
  B[2][1] = C[15]*Ai[1] + C[16]*Ai[2] + C[17]*Ai[4];
  B[2][2] = C[15]*Ai[3] + C[16]*Ai[4] + C[17]*Ai[5];

  B[3][0] = C[21]*Ai[0] + C[22]*Ai[1] + C[23]*Ai[3];
  B[3][1] = C[21]*Ai[1] + C[22]*Ai[2] + C[23]*Ai[4];
  B[3][2] = C[21]*Ai[3] + C[22]*Ai[4] + C[23]*Ai[5];

  B[4][0] = C[28]*Ai[0] + C[29]*Ai[1] + C[30]*Ai[3];
  B[4][1] = C[28]*Ai[1] + C[29]*Ai[2] + C[30]*Ai[4];
  B[4][2] = C[28]*Ai[3] + C[29]*Ai[4] + C[30]*Ai[5];

  fvec z[3] = { m[0]-r[0], m[1]-r[1], m[2]-r[2] };

  {
    fvec AV[6] = { C[0]-V[0], C[1]-V[1], C[2]-V[2], 
		     C[3]-V[3], C[4]-V[4], C[5]-V[5] };
    fvec AVi[6];
    AVi[0] = AV[4]*AV[4] - AV[2]*AV[5];
    AVi[1] = AV[1]*AV[5] - AV[3]*AV[4];
    AVi[2] = AV[3]*AV[3] - AV[0]*AV[5] ;
    AVi[3] = AV[2]*AV[3] - AV[1]*AV[4];
    AVi[4] = AV[0]*AV[4] - AV[1]*AV[3] ;
    AVi[5] = AV[1]*AV[1] - AV[0]*AV[2] ;

    det = 1./( AV[0]*AVi[0] + AV[1]*AVi[1] + AV[3]*AVi[3] );

    Particle->NDF  += 2.;
    Particle->Chi2 += 
      ( +(AVi[0]*z[0] + AVi[1]*z[1] + AVi[3]*z[2])*z[0]
	+(AVi[1]*z[0] + AVi[2]*z[1] + AVi[4]*z[2])*z[1]
	+(AVi[3]*z[0] + AVi[4]*z[1] + AVi[5]*z[2])*z[2] )*det;
  }

  r[0] = m[0];
  r[1] = m[1];
  r[2] = m[2];
  r[3]+= B[0][0]*z[0] + B[0][1]*z[1] + B[0][2]*z[2];
  r[4]+= B[1][0]*z[0] + B[1][1]*z[1] + B[1][2]*z[2];
  r[5]+= B[2][0]*z[0] + B[2][1]*z[1] + B[2][2]*z[2];
  r[6]+= B[3][0]*z[0] + B[3][1]*z[1] + B[3][2]*z[2];
  r[7]+= B[4][0]*z[0] + B[4][1]*z[1] + B[4][2]*z[2];

  fvec d0, d1, d2;

  C[0] = V[0];
  C[1] = V[1];
  C[2] = V[2];
  C[3] = V[3];
  C[4] = V[4];
  C[5] = V[5];

  d0= B[0][0]*V[0] + B[0][1]*V[1] + B[0][2]*V[3] - C[ 6];
  d1= B[0][0]*V[1] + B[0][1]*V[2] + B[0][2]*V[4] - C[ 7];
  d2= B[0][0]*V[3] + B[0][1]*V[4] + B[0][2]*V[5] - C[ 8];

  C[ 6]+= d0;
  C[ 7]+= d1;
  C[ 8]+= d2;
  C[ 9]+= d0*B[0][0] + d1*B[0][1] + d2*B[0][2];

  d0= B[1][0]*V[0] + B[1][1]*V[1] + B[1][2]*V[3] - C[10];
  d1= B[1][0]*V[1] + B[1][1]*V[2] + B[1][2]*V[4] - C[11];
  d2= B[1][0]*V[3] + B[1][1]*V[4] + B[1][2]*V[5] - C[12];

  C[10]+= d0;
  C[11]+= d1;
  C[12]+= d2;
  C[13]+= d0*B[0][0] + d1*B[0][1] + d2*B[0][2];
  C[14]+= d0*B[1][0] + d1*B[1][1] + d2*B[1][2];

  d0= B[2][0]*V[0] + B[2][1]*V[1] + B[2][2]*V[3] - C[15];
  d1= B[2][0]*V[1] + B[2][1]*V[2] + B[2][2]*V[4] - C[16];
  d2= B[2][0]*V[3] + B[2][1]*V[4] + B[2][2]*V[5] - C[17];

  C[15]+= d0;
  C[16]+= d1;
  C[17]+= d2;
  C[18]+= d0*B[0][0] + d1*B[0][1] + d2*B[0][2];
  C[19]+= d0*B[1][0] + d1*B[1][1] + d2*B[1][2];
  C[20]+= d0*B[2][0] + d1*B[2][1] + d2*B[2][2];

  d0= B[3][0]*V[0] + B[3][1]*V[1] + B[3][2]*V[3] - C[21];
  d1= B[3][0]*V[1] + B[3][1]*V[2] + B[3][2]*V[4] - C[22];
  d2= B[3][0]*V[3] + B[3][1]*V[4] + B[3][2]*V[5] - C[23];

  C[21]+= d0;
  C[22]+= d1;
  C[23]+= d2;
  C[24]+= d0*B[0][0] + d1*B[0][1] + d2*B[0][2];
  C[25]+= d0*B[1][0] + d1*B[1][1] + d2*B[1][2];
  C[26]+= d0*B[2][0] + d1*B[2][1] + d2*B[2][2];
  C[27]+= d0*B[3][0] + d1*B[3][1] + d2*B[3][2];

  d0= B[4][0]*V[0] + B[4][1]*V[1] + B[4][2]*V[3] - C[28];
  d1= B[4][0]*V[1] + B[4][1]*V[2] + B[4][2]*V[4] - C[29];
  d2= B[4][0]*V[3] + B[4][1]*V[4] + B[4][2]*V[5] - C[30];

  C[28]+= d0;
  C[29]+= d1;
  C[30]+= d2;
  C[31]+= d0*B[0][0] + d1*B[0][1] + d2*B[0][2];
  C[32]+= d0*B[1][0] + d1*B[1][1] + d2*B[1][2];
  C[33]+= d0*B[2][0] + d1*B[2][1] + d2*B[2][2];
  C[34]+= d0*B[3][0] + d1*B[3][1] + d2*B[3][2];
  C[35]+= d0*B[4][0] + d1*B[4][1] + d2*B[4][2];

  Extrapolate(Particle, r0, r[7]);
  Convert(Particle, r0, 0);
}


void CbmKFParticleInterface::Convert( CbmKFParticle_simd*  Particle, fvec r0[], bool ToProduction )
{
  fvec *C = Particle->GetCovMatrix();

  L1FieldValue B = KFPart.fField.Get(r0[2]);
  B.x *= 0.000299792458; //multiply by c
  B.y *= 0.000299792458;
  B.z *= 0.000299792458;

  fvec h[6];

  h[0] = r0[3];
  h[1] = r0[4];
  h[2] = r0[5];
  if( ToProduction ){ h[0]=-h[0]; h[1]=-h[1]; h[2]=-h[2]; } 
  h[3] = h[1]*B.z-h[2]*B.y;
  h[4] = h[2]*B.x-h[0]*B.z;
  h[5] = h[0]*B.y-h[1]*B.x;

  fvec c;

  c = C[28]+h[0]*C[35];
  C[ 0]+= h[0]*(c+C[28]);
  C[28] = c;

  C[ 1]+= h[1]*C[28] + h[0]*C[29];
  c = C[29]+h[1]*C[35];
  C[ 2]+= h[1]*(c+C[29]);
  C[29] = c;

  C[ 3]+= h[2]*C[28] + h[0]*C[30];
  C[ 4]+= h[2]*C[29] + h[1]*C[30];
  c = C[30]+h[2]*C[35];
  C[ 5]+= h[2]*(c+C[30]);
  C[30] = c;

  C[ 6]+= h[3]*C[28] + h[0]*C[31];
  C[ 7]+= h[3]*C[29] + h[1]*C[31];
  C[ 8]+= h[3]*C[30] + h[2]*C[31];
  c = C[31]+h[3]*C[35];
  C[ 9]+= h[3]*(c+C[31]);
  C[31] = c;

  C[10]+= h[4]*C[28] + h[0]*C[32];
  C[11]+= h[4]*C[29] + h[1]*C[32];
  C[12]+= h[4]*C[30] + h[2]*C[32];
  C[13]+= h[4]*C[31] + h[3]*C[32];
  c = C[32]+h[4]*C[35];
  C[14]+= h[4]*(c+C[32]);
  C[32] = c;

  C[15]+= h[5]*C[28] + h[0]*C[33];
  C[16]+= h[5]*C[29] + h[1]*C[33];
  C[17]+= h[5]*C[30] + h[2]*C[33];
  C[18]+= h[5]*C[31] + h[3]*C[33];
  C[19]+= h[5]*C[32] + h[4]*C[33];
  c = C[33]+h[5]*C[35];
  C[20]+= h[5]*(c+C[33]);
  C[33] = c;

  C[21]+= h[0]*C[34];
  C[22]+= h[1]*C[34];
  C[23]+= h[2]*C[34];
  C[24]+= h[3]*C[34];
  C[25]+= h[4]*C[34];
  C[26]+= h[5]*C[34];
}

void CbmKFParticleInterface::ExtrapolateLine(CbmKFParticle_simd*  Particle, fvec r0[], fvec dS )
{
  fvec *r = Particle->GetParameters();
  fvec *C = Particle->GetCovMatrix();

  if( r0 && r0!=r ){
    r0[0]+= dS*r0[3];
    r0[1]+= dS*r0[4];
    r0[2]+= dS*r0[5];
  }

  r[0]+= dS*r[3];
  r[1]+= dS*r[4];
  r[2]+= dS*r[5];

  fvec C6  = C[ 6] + dS*C[ 9];
  fvec C11 = C[11] + dS*C[14];
  fvec C17 = C[17] + dS*C[20];
  fvec SC13 = dS*C[13];
  fvec SC18 = dS*C[18];
  fvec SC19 = dS*C[19];

  C[ 0] += dS*( C[ 6] + C6  );
  C[ 2] += dS*( C[11] + C11 );
  C[ 5] += dS*( C[17] + C17 );

  C[ 7] += SC13;
  C[ 8] += SC18;
  C[12] += SC19;

  C[ 1] += dS*( C[10] + C[ 7] );
  C[ 3] += dS*( C[15] + C[ 8] );
  C[ 4] += dS*( C[16] + C[12] );

  C[ 6]  = C6;
  C[10] += SC13;
  C[11]  = C11;
  C[15] += SC18;
  C[16] += SC19;
  C[17]  = C17;

  C[21] += dS*C[24];
  C[22] += dS*C[25];
  C[23] += dS*C[26];
  C[28] += dS*C[31];
  C[29] += dS*C[32];
  C[30] += dS*C[33];
}

void CbmKFParticleInterface::Extrapolate(CbmKFParticle_simd*  Particle, fvec r0[], fvec dS )
{
  if( Particle->Q[0]==0 ){
    ExtrapolateLine( Particle, r0, dS );
    return;
  }

  fvec *r = Particle->GetParameters();
  fvec *C = Particle->GetCovMatrix();

  const float c_light = 0.000299792458;

  fvec c = Particle->Q*c_light;

  // construct coefficients 

  fvec 
    px   = r[3],
    py   = r[4],
    pz   = r[5];

  fvec sx=0, sy=0, sz=0, syy=0, syz=0, syyy=0, Sx=0, Sy=0, Sz=0, Syy=0, Syz=0, Syyy=0;

  { // get field integrals

    L1FieldValue B[3];
    fvec p0[3], p1[3], p2[3];

    // line track approximation

    p0[0] = r[0];
    p0[1] = r[1];
    p0[2] = r[2];

    p2[0] = r[0] + px*dS;
    p2[1] = r[1] + py*dS;
    p2[2] = r[2] + pz*dS;

    p1[0] = 0.5*(p0[0]+p2[0]);
    p1[1] = 0.5*(p0[1]+p2[1]);
    p1[2] = 0.5*(p0[2]+p2[2]);

    B[0] = Particle->fField.Get(p0[2]);
    B[1] = Particle->fField.Get(p1[2]);
    B[2] = Particle->fField.Get(p2[2]);

    // first order track approximation

    fvec Sy1 = ( 7*B[0].y + 6*B[1].y-B[2].y  )*c*dS*dS/96.;
    fvec Sy2 = (   B[0].y + 2*B[1].y         )*c*dS*dS/6.;

    p1[0] -= Sy1*pz;
    p1[2] += Sy1*px;
    p2[0] -= Sy2*pz;
    p2[2] += Sy2*px;

    B[0] = Particle->fField.Get(p0[2]);
    B[1] = Particle->fField.Get(p1[2]);
    B[2] = Particle->fField.Get(p2[2]);

    sx = c*( B[0].x + 4*B[1].x + B[2].x )*dS/6.;
    sy = c*( B[0].y + 4*B[1].y + B[2].y )*dS/6.;
    sz = c*( B[0].z + 4*B[1].z + B[2].z )*dS/6.;

    Sx = c*( B[0].x + 2*B[1].x)*dS*dS/6.;
    Sy = c*( B[0].y + 2*B[1].y)*dS*dS/6.;
    Sz = c*( B[0].z + 2*B[1].z)*dS*dS/6.;

    fvec c2[3][3]    =   { {  5, -4, -1},{  44,  80,  -4},{ 11, 44, 5} }; // /=360.    
    fvec C2[3][3]    =   { { 38,  8, -4},{ 148, 208, -20},{  3, 36, 3} }; // /=2520.
    for(Int_t n=0; n<3; n++)
      for(Int_t m=0; m<3; m++) 
	{
	  syz += c2[n][m]*B[n].y*B[m].z;
	  Syz += C2[n][m]*B[n].y*B[m].z;
	}
 
    syz  *= c*c*dS*dS/360.;
    Syz  *= c*c*dS*dS*dS/2520.;

    syy  = c*( B[0].y + 4*B[1].y + B[2].y )*dS;
    syyy = syy*syy*syy / 1296;
    syy  = syy*syy/72;

    Syy = ( B[0].y*( 38*B[0].y + 156*B[1].y  -   B[2].y )+
	    B[1].y*(             208*B[1].y  +16*B[2].y )+
	    B[2].y*(                           3*B[2].y )
	    )*dS*dS*dS*c*c/2520.;
    Syyy = 
      (
       B[0].y*( B[0].y*( 85*B[0].y + 526*B[1].y  - 7*B[2].y )+
                B[1].y*(            1376*B[1].y  +84*B[2].y )+
                B[2].y*(                          19*B[2].y )  )+
       B[1].y*( B[1].y*(             1376*B[1].y +256*B[2].y )+
                B[2].y*(                           62*B[2].y )  )+
       B[2].y*B[2].y  *(                            3*B[2].y )
       )*dS*dS*dS*dS*c*c*c/90720.;
  }

  fvec J[11];
//   for( int i=0; i<8; i++ ) for( int j=0; j<8; j++) J[i][j]=0;
// 
//   J[0][0]=1; J[0][1]=0; J[0][2]=0; J[0][3]=dS-Syy;  J[0][4]=Sx;  J[0][5]=Syyy-Sy;
//   J[1][0]=0; J[1][1]=1; J[1][2]=0; J[1][3]=-Sz;     J[1][4]=dS;  J[1][5]=Sx+Syz;
//   J[2][0]=0; J[2][1]=0; J[2][2]=1; J[2][3]=Sy-Syyy; J[2][4]=-Sx; J[2][5]=dS-Syy;
// 
//   J[3][0]=0; J[3][1]=0; J[3][2]=0; J[3][3]=1-syy;   J[3][4]=sx;  J[3][5]=syyy-sy;
//   J[4][0]=0; J[4][1]=0; J[4][2]=0; J[4][3]=-sz;     J[4][4]=1;   J[4][5]=sx+syz;
//   J[5][0]=0; J[5][1]=0; J[5][2]=0; J[5][3]=sy-syyy; J[5][4]=-sx; J[5][5]=1-syy;
//   J[6][6] = J[7][7] = 1;


  J[0]=dS-Syy;  J[1]=Sx;  J[2]=Syyy-Sy;
  J[3]=-Sz;     J[4]=dS;  J[5]=Sx+Syz;

  J[6]=1-syy;   J[7]=sx;  J[8]=syyy-sy;
  J[9]=-sz;               J[10]=sx+syz;



  r[0]+=  J[0]*px + J[1]*py + J[2]*pz;
  r[1]+=  J[3]*px + J[4]*py + J[5]*pz;
  r[2]+= -J[2]*px - J[1]*py + J[0]*pz;
  r[3] =  J[6]*px + J[7]*py + J[8]*pz;
  r[4] =  J[9]*px +      py + J[10]*pz;
  r[5] = -J[8]*px - J[7]*py + J[6]*pz;

  if( r0 && r0!=r ){
    px   = r0[3];
    py   = r0[4];
    pz   = r0[5];

    r0[0]+=  J[0]*px  + J[1]*py + J[2]*pz;
    r0[1]+=  J[3]*px  + J[4]*py + J[5]*pz;
    r0[2]+= -J[2]*px  - J[1]*py + J[0]*pz;
    r0[3] =  J[6]*px  + J[7]*py + J[8]*pz;
    r0[4] =  J[9]*px  +      py + J[10]*pz;
    r0[5] = -J[8]*px  - J[7]*py + J[6]*pz;
  }

  multQSQt1( J, C);
}

void CbmKFParticleInterface::multQSQt( const fvec Q[], fvec S[] )
{
  fvec A[64]={0};

  for( Int_t i=0, n=0; i<8; i++ )
    for( Int_t j=0; j<8; j++, ++n )
      for( Int_t k=0; k<8; ++k ) A[n]+= S[IJ(i,k)] * Q[ j*8+k];

  for( Int_t i=0; i<8; i++ )
  {
    for( Int_t j=0; j<=i; j++ )
    {
      Int_t n = IJ(i,j);
      S[n] = 0 ;
      for( Int_t k=0; k<8; k++ )  S[n] += Q[ i*8+k ] * A[ k*8+j ];
    }
  }
}

void CbmKFParticleInterface::multQSQt1( const fvec J[11], fvec S[] )
{
  const fvec A00 = S[0 ]+S[6 ]*J[0]+S[10]*J[1]+S[15]*J[2];
  const fvec A10 = S[1 ]+S[7 ]*J[0]+S[11]*J[1]+S[16]*J[2];
  const fvec A20 = S[3 ]+S[8 ]*J[0]+S[12]*J[1]+S[17]*J[2];
  const fvec A30 = S[6 ]+S[9 ]*J[0]+S[13]*J[1]+S[18]*J[2];
  const fvec A40 = S[10]+S[13]*J[0]+S[14]*J[1]+S[19]*J[2];
  const fvec A50 = S[15]+S[18]*J[0]+S[19]*J[1]+S[20]*J[2];
  const fvec A60 = S[21]+S[24]*J[0]+S[25]*J[1]+S[26]*J[2];
  const fvec A70 = S[28]+S[31]*J[0]+S[32]*J[1]+S[33]*J[2];

  S[0 ] = A00+J[0]*A30+J[1]*A40+J[2]*A50;
  S[1 ] = A10+J[3]*A30+J[4]*A40+J[5]*A50;
  S[3 ] = A20-J[2]*A30-J[1]*A40+J[0]*A50;
  S[6 ] = J[6]*A30+J[7]*A40+J[8]*A50;
  S[10] = J[9]*A30+        A40+J[10]*A50;
  S[15] = -J[8]*A30-J[7]*A40+J[6]*A50;
  S[21] = A60;
  S[28] = A70;

//  const fvec A01 = S[1 ]+S[6 ]*J[3]+S[10]*J[4]+S[15]*J[5];
  const fvec A11 = S[2 ]+S[7 ]*J[3]+S[11]*J[4]+S[16]*J[5];
  const fvec A21 = S[4 ]+S[8 ]*J[3]+S[12]*J[4]+S[17]*J[5];
  const fvec A31 = S[7 ]+S[9 ]*J[3]+S[13]*J[4]+S[18]*J[5];
  const fvec A41 = S[11]+S[13]*J[3]+S[14]*J[4]+S[19]*J[5];
  const fvec A51 = S[16]+S[18]*J[3]+S[19]*J[4]+S[20]*J[5];
  const fvec A61 = S[22]+S[24]*J[3]+S[25]*J[4]+S[26]*J[5];
  const fvec A71 = S[29]+S[31]*J[3]+S[32]*J[4]+S[33]*J[5];

  S[2 ] = A11+J[3]*A31+J[4]*A41+J[5]*A51;
  S[4 ] = A21-J[2]*A31-J[1]*A41+J[0]*A51;
  S[7 ] = J[6]*A31+J[7]*A41+J[8]*A51;
  S[11] = J[9]*A31+        A41+J[10]*A51;
  S[16] = -J[8]*A31-J[7]*A41+J[6]*A51;
  S[22] = A61;
  S[29] = A71;

//  const fvec A02 = S[3 ]+S[6 ]*J[2][3]+S[10]*J[2][4]+S[15]*J[2][5];
//  const fvec A12 = S[4 ]+S[7 ]*J[2][3]+S[11]*J[2][4]+S[16]*J[2][5];
  const fvec A22 = S[5 ]-S[8 ]*J[2]-S[12]*J[1]+S[17]*J[0];
  const fvec A32 = S[8 ]-S[9 ]*J[2]-S[13]*J[1]+S[18]*J[0];
  const fvec A42 = S[12]-S[13]*J[2]-S[14]*J[1]+S[19]*J[0];
  const fvec A52 = S[17]-S[18]*J[2]-S[19]*J[1]+S[20]*J[0];
  const fvec A62 = S[23]-S[24]*J[2]-S[25]*J[1]+S[26]*J[0];
  const fvec A72 = S[30]-S[31]*J[2]-S[32]*J[1]+S[33]*J[0];

  S[5 ] = A22-J[2]*A32-J[1]*A42+J[0]*A52;
  S[8 ] = J[6]*A32+J[7]*A42+J[8]*A52;
  S[12] = J[9]*A32+        A42+J[10]*A52;
  S[17] = -J[8]*A32-J[7]*A42+J[6]*A52;
  S[23] = A62;
  S[30] = A72;

//  const fvec A03 = S[6] *J[6]+S[10]*J[7]+S[15]*J[8];
//  const fvec A13 = S[7] *J[6]+S[11]*J[7]+S[16]*J[8];
//  const fvec A23 = S[8] *J[6]+S[12]*J[7]+S[17]*J[8];
  const fvec A33 = S[9] *J[6]+S[13]*J[7]+S[18]*J[8];
  const fvec A43 = S[13]*J[6]+S[14]*J[7]+S[19]*J[8];
  const fvec A53 = S[18]*J[6]+S[19]*J[7]+S[20]*J[8];
  const fvec A63 = S[24]*J[6]+S[25]*J[7]+S[26]*J[8];
  const fvec A73 = S[31]*J[6]+S[32]*J[7]+S[33]*J[8];

//  const fvec A04 = S[6] *J[9]+S[10]*J[4][4]+S[15]*J[10];
//  const fvec A14 = S[7] *J[9]+S[11]*J[4][4]+S[16]*J[10];
//  const fvec A24 = S[8] *J[9]+S[12]*J[4][4]+S[17]*J[10];
  const fvec A34 = S[9] *J[9]+S[13]+S[18]*J[10];
  const fvec A44 = S[13]*J[9]+S[14]+S[19]*J[10];
  const fvec A54 = S[18]*J[9]+S[19]+S[20]*J[10];
  const fvec A64 = S[24]*J[9]+S[25]+S[26]*J[10];
  const fvec A74 = S[31]*J[9]+S[32]+S[33]*J[10];

//  const fvec A05 = S[6] *J[5][3]+S[10]*J[5][4]+S[15]*J[6];
//  const fvec A15 = S[7] *J[5][3]+S[11]*J[5][4]+S[16]*J[6];
//  const fvec A25 = S[8] *J[5][3]+S[12]*J[5][4]+S[17]*J[6];
  const fvec A35 = -S[9] *J[8]-S[13]*J[7]+S[18]*J[6];
  const fvec A45 = -S[13]*J[8]-S[14]*J[7]+S[19]*J[6];
  const fvec A55 = -S[18]*J[8]-S[19]*J[7]+S[20]*J[6];
  const fvec A65 = -S[24]*J[8]-S[25]*J[7]+S[26]*J[6];
  const fvec A75 = -S[31]*J[8]-S[32]*J[7]+S[33]*J[6];


  S[9 ] = J[6]*A33+J[7]*A43+J[8]*A53;
  S[13] = J[9]*A33+        A43+J[10]*A53;
  S[18] = -J[8]*A33-J[7]*A43+J[6]*A53;
  S[24] = A63;
  S[31] = A73;

  S[14] = J[9]*A34+        A44+J[10]*A54;
  S[19] = -J[8]*A34-J[7]*A44+J[6]*A54;
  S[25] = A64;
  S[32] = A74;

  S[20] = -J[8]*A35-J[7]*A45+J[6]*A55;
  S[26] = A65;
  S[33] = A75;

//S[27] = S27;
//S[34] = S34;
//S[35] = S35;
}

void CbmKFParticleInterface::Extrapolate( fvec r0[], fvec dS )
{
  Extrapolate(&KFPart, r0, dS);
}

void CbmKFParticleInterface::TransportToProductionVertex()
{
  if( KFPart.AtProductionVertex ) return;
  fvec r0[8];
  for( int i=0; i<8; i++ ) r0[i] = KFPart.r[i];
  Extrapolate(r0, -(KFPart.r[7]));
  Convert(&KFPart, r0, 1);    
  KFPart.AtProductionVertex = 1;
}

void CbmKFParticleInterface::TransportToDecayVertex()
{
  if( !KFPart.AtProductionVertex ) return;
  fvec r0[8];
  for( int i=0; i<8; i++ ) r0[i] = KFPart.r[i];
  Extrapolate(r0, KFPart.r[7]);
  Convert(&KFPart, r0, 0);
  KFPart.AtProductionVertex = 0;
}

fvec CbmKFParticleInterface::GetDStoPoint( const CbmKFParticle_simd&  Particle, const fvec xyz[] ) const
{
//   const fvec *r = Particle.GetParameters();
// 
//   const fvec dx = xyz[0] - r[0];
//   const fvec dy = xyz[1] - r[1];
//   const fvec dz = xyz[2] - r[2];
//   const fvec p2 = r[3]*r[3] + r[4]*r[4]+r[5]*r[5];
// 
//   L1FieldValue B = const_cast<L1FieldRegion*>(&Particle.fField)->Get(r[2]);
//   B.x *= 0.000299792458; //multiply by c
//   B.y *= 0.000299792458;
//   B.z *= 0.000299792458;
// 
//   fvec bq1 = B.x*Particle.Q;
//   fvec bq2 = B.y*Particle.Q;
//   fvec bq3 = B.z*Particle.Q;
//   fvec a = dx*r[3]+dy*r[4]+dz*r[5];
//   fvec B2 = B.x*B.x+B.y*B.y+B.z*B.z;
//   fvec bq = Particle.Q*sqrt(B2);
//   fvec pB = r[3]*B.x+r[4]*B.y+r[5]*B.z;
//   fvec rB = dx*B.x+dy*B.y+dz*B.z;
// 
//   fvec small = 0.0001;
//   fvec init = fvec(B2 > 0.0001);
//   B2 = (init & B2) + (fvec(!init) & small);
//   fvec pt2 = p2 - pB*pB/B2;
//   a = a - pB*rB/B2;
// 
//   fvec dS = atan2(bq*a, pt2 + bq1*(dz*r[4]-dy*r[5]) - bq2*(dz*r[3]-dx*r[5])+bq3*(dy*r[3]-dx*r[4]))/bq;
//   fvec dS2 = a/pt2;
//   fvec smallBq = 1.e-8;
//   fvec initBq = fvec(fabs(bq) > smallBq);
//   dS = (initBq & dS) + (fvec(!initBq) & dS2);
//   fvec initPt2 = fvec(pt2 > small);
//   dS = (initPt2 & dS) + (fvec(!initPt2) & fvec(0));
// 
//   //fvec dSm = rB/pB;
//  return dS;

  const fvec *r = Particle.GetParameters();
  fvec p2 = r[3]*r[3] + r[4]*r[4] + r[5]*r[5];  
//  if( p2<1.e-4 ) p2 = 1;
  return ( r[3]*(xyz[0]-r[0]) + r[4]*(xyz[1]-r[1]) + r[5]*(xyz[2]-r[2]) )/p2;
}

fvec CbmKFParticleInterface::GetDStoPoint( const fvec xyz[] ) const
{
  return GetDStoPoint( KFPart, xyz );
}

void CbmKFParticleInterface::GetKFVertex( CbmKFVertex vtx[]  )
{
  float r0[fvecLen], r1[fvecLen], r2[fvecLen];
  float C_temp[6][fvecLen];
  float Chi2_temp[fvecLen];
  int NDF_temp[fvecLen];

  for(int j=0; j<fvecLen; j++)
  {
    r0[j] = KFPart.r[0][j];
    r1[j] = KFPart.r[1][j];
    r2[j] = KFPart.r[2][j];
    for( int i=0; i<6; i++) C_temp[i][j] = KFPart.C[i][j];

    Chi2_temp[j] = KFPart.Chi2[j];
    NDF_temp[j] = (int) KFPart.NDF[j];
  }

  for(int j=0; j<fvecLen; j++)
    vtx[j] = GetKFVertexJ(j);
}

CbmKFVertex CbmKFParticleInterface::GetKFVertexJ(int j)
{
  CbmKFVertex vtx;
  vtx.GetRefX() = KFPart.r[0][j];
  vtx.GetRefY() = KFPart.r[1][j];
  vtx.GetRefZ() = KFPart.r[2][j];
  for( int i=0; i<6; i++) vtx.GetCovMatrix()[i] = KFPart.C[i][j];
  vtx.GetRefChi2() = KFPart.Chi2[j];
  vtx.GetRefNDF()  = (int) KFPart.NDF[j];

  return vtx;
}

void CbmKFParticleInterface::GetKFVertexJ(int j, CbmKFVertex *vtx)
{
  vtx->GetRefX() = KFPart.r[0][j];
  vtx->GetRefY() = KFPart.r[1][j];
  vtx->GetRefZ() = KFPart.r[2][j];
  for( int i=0; i<6; i++) vtx->GetCovMatrix()[i] = KFPart.C[i][j];
  vtx->GetRefChi2() = KFPart.Chi2[j];
  vtx->GetRefNDF()  = (int) KFPart.NDF[j];
}

void CbmKFParticleInterface::GetKFParticle(CbmKFParticle &Part, int iPart)
{
  Part.SetId(static_cast<int>(KFPart.Id()[iPart]));

  Part.CleanDaughtersId();
  Part.SetNDaughters(KFPart.DaughterIds().size());
  for( unsigned int i = 0; i < KFPart.DaughterIds().size(); i++ )
    Part.AddDaughter(static_cast<int>(KFPart.DaughterIds()[i][iPart]));

  Part.SetPDG( static_cast<int>(KFPart.GetPDG()[iPart]) );

  for(int iP=0; iP<8; iP++)
    Part.GetParameters()[iP] = KFPart.GetParameters()[iP][iPart];
  for(int iC=0; iC<36; iC++)
    Part.GetCovMatrix()[iC] = KFPart.GetCovMatrix()[iC][iPart];

  Part.NDF = static_cast<int>(KFPart.GetNDF()[iPart]);
  Part.Chi2 = KFPart.GetChi2()[iPart];
  Part.Q    = KFPart.GetQ()[iPart];
  Part.AtProductionVertex = KFPart.AtProductionVertex;

  Part.fieldRegion[0] = KFPart.fField.cx0[iPart];
  Part.fieldRegion[1] = KFPart.fField.cx1[iPart];
  Part.fieldRegion[2] = KFPart.fField.cx2[iPart];
  Part.fieldRegion[3] = KFPart.fField.cy0[iPart];
  Part.fieldRegion[4] = KFPart.fField.cy1[iPart];
  Part.fieldRegion[5] = KFPart.fField.cy2[iPart];
  Part.fieldRegion[6] = KFPart.fField.cz0[iPart];
  Part.fieldRegion[7] = KFPart.fField.cz1[iPart];
  Part.fieldRegion[8] = KFPart.fField.cz2[iPart];
  Part.fieldRegion[9] = KFPart.fField.z0[iPart];
}

void CbmKFParticleInterface::GetKFParticle(CbmKFParticle* Part, int nPart)
{
  for(int i=0; i<nPart; i++)
    GetKFParticle(Part[i],i);
}

void CbmKFParticleInterface::SetVtxGuess( fvec &x, fvec &y, fvec &z )
{
  KFPart.SetVtxGuess(x,y,z);
}

void CbmKFParticleInterface::SetVtxErrGuess( fvec &dx, fvec &dy, fvec &dz )
{
  KFPart.SetVtxErrGuess(dx,dy,dz);
}

void CbmKFParticleInterface::GetMomentum( fvec &P, fvec &Error )
{
  KFPart.GetMomentum( P, Error );
}

void CbmKFParticleInterface::GetMass( fvec &M, fvec &Error )
{
  KFPart.GetMass ( M, Error );
}

//template<class T> 
void CbmKFParticleInterface::FindParticles(vector<CbmKFTrack> &vRTracks, vector<float>& ChiToPrimVtx,
                                           vector<L1FieldRegion>& vField, vector<CbmKFParticle>& Particles,
                                           CbmKFVertex& PrimVtx, const vector<int>& vTrackPDG, const float cuts[2][3])
{
  //* Finds particles (K0s and Lambda) from a given set of tracks

  static const int NTrackTypes = 8;

  int pdgPos[NTrackTypes]={-11,-13, 211, 321, 2212,  211, 321, 2212};
  int pdgNeg[NTrackTypes]={ 11, 13,-211,-321,-2212, -211,-321,-2212};

  vector<short> idPosSec[NTrackTypes];
  vector<short> idNegSec[NTrackTypes];

  vector<short> idPosPrim[NTrackTypes];
  vector<short> idNegPrim[NTrackTypes];

  for(unsigned short iTr=0; iTr < vRTracks.size(); iTr++)
  {
    CbmKFTrack &kfTrack = vRTracks[iTr];
    bool ok = 1;
    for(unsigned short iT=0; iT<5; iT++)
      ok = ok && finite(kfTrack.GetTrack()[iT]);
    for(unsigned short iC=0; iC<15; iC++)
      ok = ok && finite(kfTrack.GetCovMatrix()[iC]);
      ok = ok && (kfTrack.GetCovMatrix()[0] < 1. && kfTrack.GetCovMatrix()[0] > 0.)
              && (kfTrack.GetCovMatrix()[2] < 1. && kfTrack.GetCovMatrix()[2] > 0.)
              && (kfTrack.GetCovMatrix()[5] < 1. && kfTrack.GetCovMatrix()[5] > 0.)
              && (kfTrack.GetCovMatrix()[9] < 1. && kfTrack.GetCovMatrix()[9] > 0.)
              && (kfTrack.GetCovMatrix()[14] < 1. && kfTrack.GetCovMatrix()[14] > 0.);
      ok = ok && kfTrack.GetRefChi2() < 10*kfTrack.GetRefNDF();
    if(!ok) continue;

    const int pdg = abs(vTrackPDG[iTr]);

    short pdgIndex = -1;
    switch (pdg)
    {
      case   11: pdgIndex = 0; break;
      case   13: pdgIndex = 1; break;
      case  211: pdgIndex = 2; break;
      case  321: pdgIndex = 3; break;
      case 2212: pdgIndex = 4; break;
    }

    short incr = 3;
    short pdgIndexMax = pdgIndex+incr;

    if(pdgIndex<2) 
    {
      incr = 1;
      pdgIndexMax = pdgIndex;
    }

    if(pdgIndex < 0)
    {
      pdgIndex = 5;
      pdgIndexMax = 7;
      incr = 1;
    }

    if( ChiToPrimVtx[iTr] < cuts[0][0] )
    {
      if(kfTrack.GetTrack()[4] >= 0.)
      {
        for(int ipdg = pdgIndex; ipdg<=pdgIndexMax; ipdg+=incr )
          idPosPrim[ipdg].push_back(iTr);
      }
      if(kfTrack.GetTrack()[4] < 0.)
        for(int ipdg = pdgIndex; ipdg<=pdgIndexMax; ipdg+=incr )
          idNegPrim[ipdg].push_back(iTr);
    }
    else
    {
      if(kfTrack.GetTrack()[4] >= 0.)
        for(int ipdg = pdgIndex; ipdg<=pdgIndexMax; ipdg+=incr )
          idPosSec[ipdg].push_back(iTr);
      if(kfTrack.GetTrack()[4] < 0.)
        for(int ipdg = pdgIndex; ipdg<=pdgIndexMax; ipdg+=incr )
          idNegSec[ipdg].push_back(iTr);
    }
  }

  const int nPart = idPosSec[5].size() * idNegSec[5].size()+
                    idPosSec[5].size() * idNegSec[7].size()+
                    idPosSec[7].size() * idNegSec[5].size()+
                    idPosPrim[2].size() * idNegPrim[3].size() + 
                    idPosPrim[3].size() * idNegPrim[2].size() + 
                    idPosPrim[3].size() * idNegPrim[3].size() + 
                    idPosPrim[4].size() * idNegPrim[3].size() + 
                    idPosPrim[3].size() * idNegPrim[4].size() + 
                    idPosPrim[0].size() * idNegPrim[0].size() + 
                    idPosPrim[1].size() * idNegPrim[1].size();

//std::cout << "NPart estim " << nPart << std::endl;
  Particles.reserve(vRTracks.size() + nPart);

  const float massLambdaPDG = 1.115683;
  const float massK0sPDG = 0.497614;
  const float massKsiPDG = 1.32171;

  for(unsigned short iTr=0; iTr < vRTracks.size(); iTr++) {
    CbmKFTrack& kfTrack = vRTracks[iTr] ;
    kfTrack.SetId(iTr);
    CbmKFParticle tmp(&kfTrack);
    tmp.SetPDG(211);//vTrackPDG[iTr]);
    tmp.SetId(Particles.size());
    Particles.push_back(tmp);
  }

  vector<float> vLambdaTopoChi2Ndf;
  vector<CbmKFParticle> vLambdaSec;
  vector<CbmKFParticle> vLambdaPrim;

  vector<float> vLambdaBarTopoChi2Ndf;
  vector<CbmKFParticle> vLambdaBarSec;
  vector<CbmKFParticle> vLambdaBarPrim;

  vector<float> vK0sTopoChi2Ndf;
  vector<CbmKFParticle> vK0sPrim;

  vector<CbmKFParticle> vXiPrim;
  vector<CbmKFParticle> vXiSec;
  vector<CbmKFParticle> vXiBarPrim;

  vector<CbmKFParticle> vXiStarPrim;
  vector<CbmKFParticle> vXiStarBarPrim;

  const float SecCuts[3] = {3.f,5.f,4.f};
  //K0s -> pi+ pi-
  Find2DaughterDecay(vRTracks, vField, Particles, pdgNeg[5], pdgPos[5], 310,
                     idNegSec[5], idPosSec[5], PrimVtx, cuts[0], 0, &vK0sTopoChi2Ndf,
                     SecCuts, massK0sPDG, 0.0022, &vK0sPrim, 0);
  //Lambda -> p pi-
  Find2DaughterDecay(vRTracks, vField, Particles, pdgNeg[5], pdgPos[7], 3122,
                     idNegSec[5], idPosSec[7], PrimVtx, cuts[1], 0,&vLambdaTopoChi2Ndf,
                     SecCuts, massLambdaPDG, 0.0012, &vLambdaPrim, &vLambdaSec);
  //Lambda_bar -> pi+ p-
  Find2DaughterDecay(vRTracks, vField, Particles, pdgPos[5], pdgNeg[4], -3122,
                     idPosSec[5], idNegSec[4], PrimVtx, cuts[1], 0, &vLambdaBarTopoChi2Ndf,
                     SecCuts, massLambdaPDG, 0.0012, &vLambdaBarPrim, &vLambdaBarSec);
  //K*0 -> K+ pi-
  Find2DaughterDecay(vRTracks, vField, Particles, pdgPos[3], pdgNeg[2], 313,
                     idPosPrim[3], idNegPrim[2], PrimVtx, cuts[1], 1);
  //K*0_bar -> pi+ K-
  Find2DaughterDecay(vRTracks, vField, Particles, pdgNeg[3], pdgPos[2], -313,
                     idNegPrim[3], idPosPrim[2], PrimVtx, cuts[1], 1);
  //Lambda* -> p K-
  Find2DaughterDecay(vRTracks, vField, Particles, pdgNeg[3], pdgPos[4], 3124,
                     idNegPrim[3], idPosPrim[4], PrimVtx, cuts[1], 1);
  //Lambda*_bar -> K+ p-
  Find2DaughterDecay(vRTracks, vField, Particles, pdgNeg[4], pdgPos[3], -3124,
                     idNegPrim[4], idPosPrim[3], PrimVtx, cuts[1], 1);
  //phi -> K+ K-
  Find2DaughterDecay(vRTracks, vField, Particles, pdgNeg[3], pdgPos[3], 333,
                     idNegPrim[3], idPosPrim[3], PrimVtx, cuts[1], 1);
//   //rho -> pi+ pi-
//   Find2DaughterDecay(vRTracks, vField, Particles, pdgNeg[2], pdgPos[2], 113,
//                      idNegPrim[2], idPosPrim[2],
//                      PrimVtx, cuts[1]);
  //gamma -> e+ e-
  Find2DaughterDecay(vRTracks, vField, Particles, pdgNeg[0], pdgPos[0], 22,
                     idNegPrim[0], idPosPrim[0], PrimVtx, cuts[1], 1);
  Find2DaughterDecay(vRTracks, vField, Particles, pdgNeg[0], pdgPos[0], 22,
                     idNegSec[0], idPosSec[0], PrimVtx, cuts[1], 0);
  Find2DaughterDecay(vRTracks, vField, Particles, pdgNeg[0], pdgPos[0], 22,
                     idNegSec[0], idPosPrim[0], PrimVtx, cuts[1], 0);
  Find2DaughterDecay(vRTracks, vField, Particles, pdgNeg[0], pdgPos[0], 22,
                     idNegPrim[0], idPosSec[0], PrimVtx, cuts[1], 0);
  //JPsi-> e+ e-
  Find2DaughterDecay(vRTracks, vField, Particles, pdgNeg[0], pdgPos[0], 443,
                     idNegPrim[0], idPosPrim[0], PrimVtx, cuts[1], 1, 1.f);
  //JPsi-> mu+ mu-
  Find2DaughterDecay(vRTracks, vField, Particles, pdgNeg[1], pdgPos[1], 100443,
                     idNegPrim[1], idPosPrim[1], PrimVtx, cuts[1], 1, 1.f);
  //rho, omega, phi -> e+ e-
  Find2DaughterDecay(vRTracks, vField, Particles, pdgNeg[0], pdgPos[0], 100113,
                     idNegPrim[0], idPosPrim[0], PrimVtx, cuts[1], 1, 0.2f);
  //rho, omega, phi -> mu+ mu-
  const float PCut = 1.f;
  Find2DaughterDecay(vRTracks, vField, Particles, pdgNeg[1], pdgPos[1], 200113,
                     idNegPrim[1], idPosPrim[1], PrimVtx, cuts[1], 1, 0.2f, -100, 0, &PCut);

  ExtrapolateToPV(vLambdaPrim,PrimVtx);
  ExtrapolateToPV(vLambdaBarPrim,PrimVtx);
  ExtrapolateToPV(vK0sPrim,PrimVtx);
  
  // Find Xi-
  float cutXi[3] = {3.,5.,6.};
  FindTrackV0Decay(3312, Particles, vLambdaSec, vRTracks, vField, pdgNeg[5], idNegSec[5],
                   PrimVtx, cutXi, 0, 0, &vXiPrim, massKsiPDG, 0.002 );


  float cutLL[3] = {6.,10000000.,3.};
  float cutLL2[3] = {6.,3.,3.};
  vector<CbmKFParticle> vLL;
  FindTrackV0Decay(3002, vLL, vLambdaSec, vRTracks, vField, pdgNeg[5], idNegSec[5],
                   PrimVtx, cutLL, 0, &ChiToPrimVtx);
  // Find H0->Lambda p pi-
  //Find Omega*-
  FindTrackV0Decay(3001, Particles, vLL, vRTracks, vField, pdgPos[4], idPosSec[4],
                   PrimVtx, cutLL2, 0, &ChiToPrimVtx);
  // Find Xi+
  float cutXiPlus[3] = {3.,5.,6.};
  FindTrackV0Decay(-3312, Particles, vLambdaBarSec, vRTracks, vField, pdgPos[5], idPosSec[5],
                   PrimVtx, cutXiPlus, 0, 0, &vXiBarPrim, massKsiPDG, 0.002);
  //Find Omega-
  float cutOmega[3] = {3.,3.,3.};
  FindTrackV0Decay(3334, Particles, vLambdaSec, vRTracks, vField, pdgNeg[6], idNegSec[6],
                   PrimVtx, cutOmega, 0, &ChiToPrimVtx);
  //Find Omega+
  float cutOmegaPlus[3] = {3.,3.,3.};
  FindTrackV0Decay(-3334, Particles, vLambdaBarSec, vRTracks, vField, pdgPos[6], idPosSec[6],
                   PrimVtx, cutOmegaPlus, 0, &ChiToPrimVtx);
  //Find Xi*-
  float cutXiStarMinus[3] = {-100.,10000.,3.};
  FindTrackV0Decay(1003314, Particles, vLambdaPrim, vRTracks, vField, pdgNeg[3], idNegPrim[3],
                   PrimVtx, cutXiStarMinus, 1);
  //Find Xi*+
  float cutXiStarPlus[3] = {-100.,10000.,3.};
  FindTrackV0Decay(-1003314, Particles, vLambdaBarPrim, vRTracks, vField, pdgPos[3], idPosPrim[3],
                   PrimVtx, cutXiStarPlus, 1);

  ExtrapolateToPV(vXiPrim,PrimVtx);
  ExtrapolateToPV(vXiBarPrim,PrimVtx);

  //Find Xi*0
  float cutXiStar0[3] = {-100.,10000.,3.};
  FindTrackV0Decay(3324, Particles, vXiPrim, vRTracks, vField, pdgPos[5], idPosPrim[5],
                   PrimVtx, cutXiStar0, 1, 0, &vXiStarPrim);
  //Find Xi*0 bar
  float cutXiBarStar0[3] = {-100.,10000.,3.};
  FindTrackV0Decay(-3324, Particles, vXiBarPrim, vRTracks, vField, pdgNeg[5], idNegPrim[5],
                   PrimVtx, cutXiBarStar0, 1, 0, &vXiStarBarPrim);
  //Find Omega*-
  const float cutOmegaStar[2] = {-100., 3.};
  for(unsigned int iPart=0; iPart<vXiStarPrim.size(); iPart++)
    CombineTrackPart(vRTracks, vField, Particles, vXiStarPrim[iPart], pdgNeg[3], 
                     1003334, idNegPrim[3], cutOmegaStar, 0);
  //Find Omega*+
  for(unsigned int iPart=0; iPart<vXiStarBarPrim.size(); iPart++)
    CombineTrackPart(vRTracks, vField, Particles, vXiStarBarPrim[iPart], pdgPos[3],
                     -1003334, idPosPrim[3], cutOmegaStar, 0);
  // Find K*+
  float cutKStarPlus[3] = {-100.,10000.,3.};
  FindTrackV0Decay(323, Particles, vK0sPrim, vRTracks, vField, pdgPos[5], idPosPrim[5],
                   PrimVtx, cutKStarPlus, 1, 0);
  // Find K*-
  float cutKStarMinus[3] = {-100.,10000.,3.};
  FindTrackV0Decay(-323, Particles, vK0sPrim, vRTracks, vField, pdgNeg[5], idNegPrim[5],
                   PrimVtx, cutKStarMinus, 1, 0);
  // Find Sigma*+
  float cutSigmaStarPlus[3] = {-100.,10000.,3.};
  FindTrackV0Decay(3224, Particles, vLambdaPrim, vRTracks, vField, pdgPos[5], idPosPrim[5],
                   PrimVtx, cutSigmaStarPlus, 1, 0);
  // Find Sigma*+ bar
  float cutSigmaStarPlusBar[3] = {-100.,10000.,3.};
  FindTrackV0Decay(-3114, Particles, vLambdaBarPrim, vRTracks, vField, pdgPos[5], idPosPrim[5],
                   PrimVtx, cutSigmaStarPlusBar, 1, 0);
  // Find Sigma*-
  float cutSigmaStarMinus[3] = {-100.,10000.,3.};
  FindTrackV0Decay(3114, Particles, vLambdaPrim, vRTracks, vField, pdgNeg[5], idNegPrim[5],
                   PrimVtx, cutSigmaStarMinus, 1, 0);
  // Find Sigma*- bar
  float cutSigmaStarMinusBar[3] = {-100.,10000.,3.};
  FindTrackV0Decay(-3224, Particles, vLambdaBarPrim, vRTracks, vField, pdgNeg[5], idNegPrim[5],
                   PrimVtx, cutSigmaStarMinusBar, 1, 0);
  // Find H-dibarion
  vector<CbmKFParticle> vHdibarion;
  float cutHdb[3] = {3.,3.,3.};
  for(unsigned short iL=0; iL < vLambdaSec.size(); iL++)
  {
    CbmKFParticle_simd vDaughters[2] = {CbmKFParticle_simd(),CbmKFParticle_simd(vLambdaSec[iL])};

    vector<int> daughterIds;
    for(unsigned int iD=0; iD<vLambdaSec[iL].DaughterIds().size(); iD++)
      daughterIds.push_back(vLambdaSec[iL].DaughterIds()[iD]);
    FindHyperons(3000, vDaughters, daughterIds, vLambdaSec, vHdibarion, PrimVtx, cutHdb, iL+1);
  }

  for(unsigned int iH=0; iH<vHdibarion.size(); iH++)
  {
    vHdibarion[iH].SetId(Particles.size());
    Particles.push_back(vHdibarion[iH]);
  }
  //                          chi2_prim  chi2_geo    z     pt  chi2_topo
  const float cutsD[8][8] = {{    6.,        3.,   0.04,  0.3,     3.},  //D0 -> pi+ K-
                             {    6.,        3.,   0.04,  0.3,     3.},  //D+ -> K- pi+ pi+
                             {    6.,        3.,   0.04,  0.3,     3.},  //D0 -> pi+ pi+ pi- K-
                             {    6.,        3.,   0.04,  0.3,     3.},  //Ds+ -> K- K+ pi+
                             {    6.,        3.,   0.04,  0.3,     3.},  //Lambdac -> pi+ K- p
                             {    6.,        3.,  -100.,  0.3,  -100.},  //D*0  -> D+ pi-
                             {    6.,        3.,  -100.,  0.3,  -100.},  //D*+  -> D0 pi+
                             {    6.,        3.,  -100.,  0.3,  -100.}}; //D*+4 -> D04 pi+

  const int DMesLambdcDaughterPDG[5] = { 211, -321, -211, 321, 2212 };
  const int DMesLambdcMotherPDG[8] = { 421, 411, 100421, 431, 4122, 10421, 10411, 20411 };
  vector<short>* DMesLambdcIdTrack[5] = {&idPosSec[5],
                                         &idNegSec[3],
                                         &idNegSec[5],
                                         &idPosSec[3],
                                         &idPosSec[4]};
  FindDMesLambdac(vRTracks, vField, Particles, DMesLambdcDaughterPDG, DMesLambdcMotherPDG,
                  DMesLambdcIdTrack, PrimVtx, cutsD, ChiToPrimVtx);

  const int DMesLambdcBarDaughterPDG[5] = { -211, 321, 211, -321, -2212 };
  const int DMesLambdcBarMotherPDG[8] = { -421, -411, -100421, -431, -4122, -10421, -10411, -20411 };
  vector<short>* DMesLambdcBarIdTrack[5] = {&idNegSec[5],
                                            &idPosSec[3],
                                            &idPosSec[5],
                                            &idNegSec[3],
                                            &idNegSec[4]};
  FindDMesLambdac(vRTracks, vField, Particles, DMesLambdcBarDaughterPDG, DMesLambdcBarMotherPDG,
                  DMesLambdcBarIdTrack, PrimVtx, cutsD, ChiToPrimVtx);
//  std::cout << "NPart  " << Particles.size() << std::endl;
}

// void CbmKFParticleInterface::FindParticles(vector<CbmL1Track>& vRTracks, vector<CbmKFParticle>& Particles,
//                                            CbmKFVertex& PrimVtx, const vector<int>& vTrackPDG, const float cuts[2][3])
// {
//   FindParticlesT(vRTracks, Particles, PrimVtx, vTrackPDG, cuts);
// }
// 
// void CbmKFParticleInterface::FindParticles(vector<CbmStsTrack>& vRTracks, vector<CbmKFParticle>& Particles,
//                                            CbmKFVertex& PrimVtx, const vector<int>& vTrackPDG, const float cuts[2][3])
// {
//   FindParticlesT(vRTracks, Particles, PrimVtx, vTrackPDG, cuts);
// }

void CbmKFParticleInterface::ExtrapolateToPV(vector<CbmKFParticle>& vParticles, CbmKFVertex& PrimVtx)
{
  fvec pVtx[3] = {PrimVtx.GetRefX(),PrimVtx.GetRefY(),PrimVtx.GetRefZ()};
  for(unsigned int iL=0; iL<vParticles.size(); iL += fvecLen)
  {
    CbmKFParticle* parts[fvecLen];
    unsigned int nPart = vParticles.size();

    unsigned int nEntries = (iL + fvecLen < nPart) ? fvecLen : (nPart - iL);

    for(unsigned short iv=0; iv<nEntries; iv++)
      parts[iv] = &vParticles[iL+iv];


    CbmKFParticleInterface tmp(parts,nEntries);
    tmp.Extrapolate( tmp.KFPart.r, tmp.GetDStoPoint(pVtx) );

    for(unsigned int iv=0; iv<nEntries; iv++)
    {
      tmp.GetKFParticle(vParticles[iL+iv], iv);
    }
  }
}

fvec CbmKFParticleInterface::GetChi2BetweenParticles(CbmKFParticle_simd &p1, CbmKFParticle_simd &p2)
{
  const fvec& x1 = p1.GetX();
  const fvec& y1 = p1.GetY();
  const fvec& z1 = p1.GetZ();

  const fvec& x2 = p2.GetX();
  const fvec& y2 = p2.GetY();
  const fvec& z2 = p2.GetZ();

  const fvec dx = x1 - x2;
  const fvec dy = y1 - y2;
  const fvec dz = z1 - z2;

  const fvec& c0 = p1.GetCovariance(0) + p2.GetCovariance(0);
  const fvec& c1 = p1.GetCovariance(1) + p2.GetCovariance(1);
  const fvec& c2 = p1.GetCovariance(2) + p2.GetCovariance(2);
  const fvec& c3 = p1.GetCovariance(3) + p2.GetCovariance(3);
  const fvec& c4 = p1.GetCovariance(4) + p2.GetCovariance(4);
  const fvec& c5 = p1.GetCovariance(5) + p2.GetCovariance(5);

  const fvec r2 = dx*dx + dy*dy + dz*dz;
  const fvec err2 = c0*dx*dx + c2*dy*dy + c5*dz*dz + 2*( c1*dx*dy + c3*dx*dz + c4*dy*dz );

  return (r2*r2/err2);
}

//template<class T>
void CbmKFParticleInterface::Find2DaughterDecay(vector<CbmKFTrack>& vTracks,
                                                const vector<L1FieldRegion>& vField,
                                                vector<CbmKFParticle>& Particles,
                                                const int DaughterNegPDG,
                                                const int DaughterPosPDG,
                                                const int MotherPDG,
                                                vector<short>& idNeg,
                                                vector<short>& idPos,
                                                CbmKFVertex& PrimVtx,
                                                const float* cuts,
                                                bool isPrimary,
                                                vector<float>* vMotherTopoChi2Ndf,
                                                const float* secCuts,
                                                const float massMotherPDG,
                                                const float massMotherPDGSigma,
                                                vector<CbmKFParticle>* vMotherPrim,
                                                vector<CbmKFParticle>* vMotherSec )
{
  const int NPositive = idPos.size();
  CbmKFParticle mother_temp;

  CbmKFParticleInterface mother;
  mother.SetPDG( MotherPDG );
  CbmKFParticleInterface motherTopo;

  const short NPosVect = NPositive%fvecLen == 0 ? NPositive/fvecLen : NPositive/fvecLen+1;
  vector<CbmKFParticle_simd> posPart(NPosVect);

  // create particles from tracks
  CbmKFTrackInterface* vvPos[fvecLen];

  int iPosVect = 0;
  for(unsigned short iTrP=0; iTrP < NPositive; iTrP += fvecLen)
  {
    const unsigned short NTracks = (iTrP + fvecLen < NPositive) ? fvecLen : (NPositive - iTrP);

    L1FieldRegion posField;
    for(unsigned short iv=0; iv<NTracks; iv++)
    {
      vvPos[iv] = &vTracks[idPos[iTrP+iv]];

      int entrSIMDPos = idPos[iTrP+iv] % fvecLen;
      int entrVecPos  = idPos[iTrP+iv] / fvecLen;
      posField.SetOneEntry(iv,vField[entrVecPos],entrSIMDPos);
    }

    posPart[iPosVect].Create(vvPos,NTracks,0,&DaughterPosPDG);
    posPart[iPosVect].SetField(posField);
    fvec posId;
    for(int iv=0; iv<NTracks; iv++)
      posId[iv] = idPos[iTrP+iv];
    posPart[iPosVect].SetId(posId);
    iPosVect++;
  }

  for(unsigned short iTrN=0; iTrN < idNeg.size(); iTrN++)
  {
    CbmKFTrack &kfTrackNeg = vTracks[idNeg[iTrN]];
    CbmKFParticle_simd vDaughters[2] = {CbmKFParticle_simd(kfTrackNeg,0,&DaughterNegPDG),
                                        CbmKFParticle_simd()};
    int entrSIMD = idNeg[iTrN] % fvecLen;
    int entrVec  = idNeg[iTrN] / fvecLen;
    vDaughters[0].SetField(vField[entrVec],1,entrSIMD);
    vDaughters[0].SetId(idNeg[iTrN]);


    for(unsigned short iTrP=0; iTrP < NPositive; iTrP += fvecLen)
    {
      const unsigned short NTracks = (iTrP + fvecLen < NPositive) ? fvecLen : (NPositive - iTrP);

      vDaughters[1] = posPart[iTrP/fvecLen];

      if(isPrimary)
      {
        fvec vtxGuess[3] = {PrimVtx.GetRefX(),
                            PrimVtx.GetRefY(),
                            PrimVtx.GetRefZ()};
        fvec errGuess[3] = {100*sqrt(PrimVtx.GetCovMatrix()[0]),
                            100*sqrt(PrimVtx.GetCovMatrix()[2]),
                            100*sqrt(PrimVtx.GetCovMatrix()[5])};
        mother.SetVtxGuess(vtxGuess[0], vtxGuess[1], vtxGuess[2]);
        mother.SetVtxErrGuess(errGuess[0], errGuess[1], errGuess[2]);
        mother.Construct(vDaughters, 2, 0, -1, -1, 1);
      }
      else
        mother.Construct(vDaughters, 2, 0);

      if(vMotherTopoChi2Ndf)
      {
        motherTopo = mother;
        motherTopo.MeasureProductionVertex(&motherTopo.KFPart, motherTopo.GetParameters(), &PrimVtx);
      }

// if( (fabs(DaughterNegPDG) == 211) && (fabs(DaughterPosPDG) == 211) )
// {
// fvec pl0[3] = {vDaughters[0].GetPx(),vDaughters[0].GetPy(),vDaughters[0].GetPz()};
// fvec pl1[3] = {vDaughters[1].GetPx(),vDaughters[1].GetPy(),vDaughters[1].GetPz()};
// 
// fvec pcm[3] = {pl0[0]+pl1[0], pl0[1]+pl1[1], pl0[2]+pl1[2]};
// fvec ecm = vDaughters[0].GetE()+vDaughters[1].GetE();
// 
// fvec gamma = sqrt(fvec(1.) - (pcm[0]*pcm[0]+pcm[1]*pcm[1]+pcm[2]*pcm[2])/(ecm*ecm));
// 
// fvec e0 = gamma*(vDaughters[0].GetE() - (pcm[0]*pl0[0]+pcm[1]*pl0[1]+pcm[2]*pl0[2])/ecm);
// fvec e1 = gamma*(vDaughters[1].GetE() - (pcm[0]*pl1[0]+pcm[1]*pl1[1]+pcm[2]*pl1[2])/ecm);
// if( fabs(e0[0]-e1[0]) > 1.e-7 )
// std::cout << e0 << "  " << e1 << std::endl;
// }

      // CbmKFParticle mother_temp[fvecLen];
      // mother.GetKFParticle(mother_temp, NTracks);

      for(int iv=0; iv<NTracks; iv++)
      {
        if(!finite(mother.GetChi2()[iv])) continue;
        if(!(mother.GetChi2()[iv] > 0.0f)) continue;
        if(!(mother.GetChi2()[iv]==mother.GetChi2()[iv])) continue;
        if(mother.GetZ()[iv] < cuts[2]) continue;

        if( mother.GetChi2()[iv]/mother.GetNDF()[iv] < cuts[1] )
        {
          mother.GetKFParticle(mother_temp, iv);
          mother_temp.SetId(Particles.size());
          Particles.push_back(mother_temp);

          float motherTopoChi2Ndf(0);
          if(vMotherTopoChi2Ndf)
            motherTopoChi2Ndf = motherTopo.GetChi2()[iv]/motherTopo.GetNDF()[iv];
//            vMotherTopoChi2Ndf->push_back(motherTopo.GetChi2()[iv]/motherTopo.GetNDF()[iv]);

          if(vMotherPrim)
          {
            Double_t mass, errMass;

            mother_temp.GetMass(mass, errMass);
            if( (fabs(mass - massMotherPDG)/massMotherPDGSigma) > secCuts[0] ) continue;

            if( motherTopoChi2Ndf < secCuts[1] )
            {
              vMotherPrim->push_back(mother_temp);
            }
            else if(vMotherSec)
            {
              if( mother_temp.GetZ() < secCuts[2] ) continue;
              vMotherSec->push_back(mother_temp);
            }
          }
        }
      }
    }
  }
}

//template<class T>
void CbmKFParticleInterface::Find2DaughterDecay(vector<CbmKFTrack>& vTracks,
                                                const vector<L1FieldRegion>& vField,
                                                vector<CbmKFParticle>& Particles,
                                                const int DaughterNegPDG,
                                                const int DaughterPosPDG,
                                                const int MotherPDG,
                                                vector<short>& idNeg,
                                                vector<short>& idPos,
                                                CbmKFVertex& PrimVtx,
                                                const float* cuts,
                                                bool isPrimary,
                                                const float PtCut,
                                                const float Chi2PrimCut,
                                                vector<float>* ChiToPrimVtx,
                                                const float* PCut)
{
  vector<short> idPosPt;
  vector<short> idNegPt;

  for(unsigned int iEl=0; iEl<idPos.size(); iEl++)
  {
    CbmKFTrack& kfTrack = vTracks[idPos[iEl]];
    const float tx = kfTrack.GetTrack()[2];
    const float ty = kfTrack.GetTrack()[3];
    const float p = fabs(1/kfTrack.GetTrack()[4]);
    const float t2 = tx*tx+ty*ty;
    float pt = p*(sqrt(t2/(1+t2)));
    if(PCut)
      if(p < *PCut) continue;
    if(pt<PtCut) continue;
    if(Chi2PrimCut > -1)
      if(ChiToPrimVtx->at(idPos[iEl]) < Chi2PrimCut) continue;
    idPosPt.push_back(idPos[iEl]);
  }

  for(unsigned int iEl=0; iEl<idNeg.size(); iEl++)
  {
    CbmKFTrack& kfTrack = vTracks[idNeg[iEl]];
    const float tx = kfTrack.GetTrack()[2];
    const float ty = kfTrack.GetTrack()[3];
    const float p = fabs(1/kfTrack.GetTrack()[4]);
    const float t2 = tx*tx+ty*ty;
    float pt = p*(sqrt(t2/(1+t2)));
    if(pt<PtCut) continue;
    if(Chi2PrimCut > -1)
      if(ChiToPrimVtx->at(idNeg[iEl]) < Chi2PrimCut) continue;
    idNegPt.push_back(idNeg[iEl]);
  }

  Find2DaughterDecay(vTracks, vField,
                     Particles, DaughterNegPDG, DaughterPosPDG, MotherPDG,
                     idNegPt, idPosPt,
                     PrimVtx, cuts, isPrimary);
}

//template<class T>
void CbmKFParticleInterface::FindTrackV0Decay(const int MotherPDG,
                                              vector<CbmKFParticle>& Particles,
                                              vector<CbmKFParticle>& vV0,
                                              vector<CbmKFTrack>& vTracks,
                                              const vector<L1FieldRegion>& vField,
                                              const int DaughterPDG,
                                              vector<short>& idTrack,
                                              CbmKFVertex& PrimVtx,
                                              const float* cuts,
                                              bool isPrimary,
                                              vector<float>* ChiToPrimVtx,
                                              vector<CbmKFParticle>* vHyperonPrim,
                                              float hyperonPrimMass,
                                              float hyperonPrimMassErr,
                                              vector<CbmKFParticle>* vHyperonSec)
{
  CbmKFParticle hyperon_temp;
  CbmKFParticleInterface hyperon;
  hyperon.SetPDG( MotherPDG );

  for(unsigned short iV0=0; iV0 < vV0.size(); iV0++)
  {
    unsigned short nElements = 0;
    CbmKFParticle_simd vDaughters[2]= {CbmKFParticle_simd(vV0[iV0]),CbmKFParticle_simd()};

    CbmKFTrackInterface* vvTr[fvecLen];
    L1FieldRegion field;
    fvec trId;

    for(unsigned short iTr=0; iTr < idTrack.size(); iTr++)
    {
      bool ok = 1;
      if(ChiToPrimVtx)
        if( (ChiToPrimVtx->at(idTrack[iTr]) < 7) ) ok=0; //TODO 7 for Omega

      if(ok)
      {
        trId[nElements] = idTrack[iTr];
        vvTr[nElements] = &vTracks[idTrack[iTr]];

        int entrSIMD = idTrack[iTr] % fvecLen;
        int entrVec  = idTrack[iTr] / fvecLen;
        field.SetOneEntry(nElements,vField[entrVec],entrSIMD);

        nElements++;
      }
      else if( (iTr != idTrack.size()-1) ) continue;

      if( (nElements == fvecLen) || ((iTr == idTrack.size()-1)&&(nElements>0)) )
      {
        vDaughters[1].Create(vvTr,nElements,0,&DaughterPDG);
        vDaughters[1].SetField(field);
        vDaughters[1].SetId(trId);

        if(isPrimary)
        {
          fvec vtxGuess[3] = {PrimVtx.GetRefX(),
                              PrimVtx.GetRefY(),
                              PrimVtx.GetRefZ()};
          fvec errGuess[3] = {100*sqrt(PrimVtx.GetCovMatrix()[0]),
                              100*sqrt(PrimVtx.GetCovMatrix()[2]),
                              100*sqrt(PrimVtx.GetCovMatrix()[5])};
          hyperon.SetVtxGuess(vtxGuess[0], vtxGuess[1], vtxGuess[2]);
          hyperon.SetVtxErrGuess(errGuess[0], errGuess[1], errGuess[2]);
          hyperon.Construct(vDaughters, 2, 0, -1, -1, 1);
        }
        else
          hyperon.Construct(vDaughters, 2, 0);

        CbmKFParticleInterface hyperonTopo(hyperon);
        hyperonTopo.MeasureProductionVertex(&hyperonTopo.KFPart, hyperonTopo.GetParameters(), &PrimVtx);

        for(unsigned int iv=0; iv<nElements; iv++)
        {
          bool isSameTrack = 0;
          for(unsigned short iD=0; iD<vV0[iV0].DaughterIds().size(); iD++)
            if(vV0[iV0].DaughterIds()[iD] == trId[iv]) isSameTrack=1;

          if(isSameTrack) continue;
          if(!finite(hyperon.GetChi2()[iv])) continue;
          if(!(hyperon.GetChi2()[iv] > 0.0f)) continue;
          if(!(hyperon.GetChi2()[iv]==hyperon.GetChi2()[iv])) continue;
          if((hyperon.GetZ()[iv] < cuts[0]) || ((vV0[iV0].GetZ() - hyperon.GetZ()[iv]) < 0)) continue;

          if(hyperonTopo.GetChi2()[iv]/hyperonTopo.GetNDF()[iv] > cuts[1] ) continue;

          if( hyperon.GetChi2()[iv]/hyperon.GetNDF()[iv] > cuts[2] ) continue;
          hyperon.GetKFParticle(hyperon_temp, iv);

          hyperon_temp.SetId(Particles.size());
          Particles.push_back(hyperon_temp);

          if(vHyperonPrim)
          {
            Double_t mass, errMass;

            hyperon_temp.GetMass(mass, errMass);
            if( (fabs(mass - hyperonPrimMass)/hyperonPrimMassErr) <= 3 )
              vHyperonPrim->push_back(hyperon_temp);
            else
              if( vHyperonSec )
                vHyperonSec->push_back(hyperon_temp);
          }
        }
        nElements=0;
      }
    }
  }
}

void CbmKFParticleInterface::FindHyperons(int PDG,
                                          CbmKFParticle_simd vDaughters[2],
                                          vector<int>& daughterIds,
                                          vector<CbmKFParticle>& vLambdaSec,
                                          vector<CbmKFParticle>& vHyperon,
                                          CbmKFVertex& PrimVtx,
                                          const float *cuts,
                                          int startIndex)
{
  CbmKFParticle* lambdas[fvecLen];
  int nLambdasSec = vLambdaSec.size();

  for(unsigned short iL=startIndex; iL < vLambdaSec.size(); iL += fvecLen)
  {
    unsigned int nEntries = (iL + fvecLen < nLambdasSec) ? fvecLen : (nLambdasSec - iL);

    for(unsigned short iv=0; iv<nEntries; iv++)
      lambdas[iv] = &vLambdaSec[iL+iv];

    vDaughters[0].Create(lambdas,nEntries);

    CbmKFParticleInterface hyperon;

    hyperon.SetPDG( PDG );
    hyperon.Construct(vDaughters, 2, 0);

    CbmKFParticleInterface hyperonTopo(hyperon);
    hyperonTopo.MeasureProductionVertex(&hyperonTopo.KFPart, hyperonTopo.GetParameters(), &PrimVtx);

    for(unsigned int iv=0; iv<nEntries; iv++)
    {
      bool isSameTrack = 0;
      for(unsigned short iD=0; iD<lambdas[iv]->DaughterIds().size(); iD++)
        for(unsigned short iD0=0; iD0<daughterIds.size(); iD0++)
          if(lambdas[iv]->DaughterIds()[iD] == daughterIds[iD0]) isSameTrack=1;

      if(isSameTrack) continue;
       if(!finite(hyperon.GetChi2()[iv])) continue;
      if(!(hyperon.GetChi2()[iv] > 0.0f)) continue;
      if(!(hyperon.GetChi2()[iv]==hyperon.GetChi2()[iv])) continue;
      if((hyperon.GetZ()[iv] < cuts[0]) || ((lambdas[iv]->GetZ() - hyperon.GetZ()[iv]) < 0)) continue;

      if(hyperonTopo.GetChi2()[iv]/hyperonTopo.GetNDF()[iv] > cuts[1] ) continue;

      if( hyperon.GetChi2()[iv]/hyperon.GetNDF()[iv] > cuts[2] ) continue;
      CbmKFParticle hyperon_temp;
      hyperon.GetKFParticle(hyperon_temp, iv);
      vHyperon.push_back(hyperon_temp);
    }
  }
}

//template<class T>
void CbmKFParticleInterface::FindDMesLambdac(vector<CbmKFTrack>& vTracks,
                                             const vector<L1FieldRegion>& vField,
                                             vector<CbmKFParticle>& Particles,
                                             const int DaughterPDG[5], //pi, K_b, pi_b, K, p
                                             const int MotherPDG[8], //D0, D+, D0_4, Ds+, Lc
                                             vector<short>* idTrack[5], //pi, K_b, pi_b, K, p
                                             CbmKFVertex& PrimVtx,
                                             const float cuts[8][8],
                                             vector<float> ChiToPrimVtx)
{
  vector<short> id[5]; //pi, K_b, pi_b, K, p
  for(int iId=0; iId<5; iId++)
  {
    for(unsigned int iTr=0; iTr<idTrack[iId]->size(); iTr++)
    {
      CbmKFTrack& kfTrack = vTracks[idTrack[iId]->at(iTr)];
      const float tx = kfTrack.GetTrack()[2];
      const float ty = kfTrack.GetTrack()[3];
      const float p = fabs(1/kfTrack.GetTrack()[4]);
      const float t2 = tx*tx+ty*ty;
      float pt = p*(sqrt(t2/(1+t2)));
      if(pt<cuts[0][3]) continue;
      if(ChiToPrimVtx[idTrack[iId]->at(iTr)] < cuts[0][0]) continue;
      id[iId].push_back(idTrack[iId]->at(iTr));
    }
  }

  vector<CbmKFParticle> kpi;
  vector<CbmKFParticle> kpipi;
  vector<CbmKFParticle> kpipipi;
  vector<CbmKFParticle> kpik;
  vector<CbmKFParticle> kpip;

  const float cutskpi[3] = {3., 3., -100.};
  Find2DaughterDecay(vTracks, vField, kpi, DaughterPDG[0], DaughterPDG[1], MotherPDG[0],
                     id[0], id[1], PrimVtx, cutskpi, 0);

  for(unsigned int iKPi=0; iKPi<kpi.size(); iKPi++)
  {
    unsigned short startPiIndex = kpi[iKPi].DaughterIds()[0]+1;

    CombineTrackPart(vTracks,vField,kpipi,kpi[iKPi],DaughterPDG[0], MotherPDG[1], id[0], cuts[1], startPiIndex, 1);
    CombineTrackPart(vTracks,vField,kpik ,kpi[iKPi],DaughterPDG[3], MotherPDG[3], id[3], cuts[3], 0, 1);
    CombineTrackPart(vTracks,vField,kpip ,kpi[iKPi],DaughterPDG[4], MotherPDG[4], id[4], cuts[4], 0, 1);
  }
  for(unsigned int iKPiPi=0; iKPiPi<kpipi.size(); iKPiPi++)
    CombineTrackPart(vTracks,vField,kpipipi,kpipi[iKPiPi],DaughterPDG[2], MotherPDG[2], id[2], cuts[2], 0, 1);

  SelectParticleCandidates(Particles, kpi,     PrimVtx, cuts[0]);
  SelectParticleCandidates(Particles, kpipi,   PrimVtx, cuts[1]);
  SelectParticleCandidates(Particles, kpipipi, PrimVtx, cuts[2]);
  SelectParticleCandidates(Particles, kpik,    PrimVtx, cuts[3]);
  SelectParticleCandidates(Particles, kpip,    PrimVtx, cuts[4]);

  vector<CbmKFParticle> d0pi;
  vector<CbmKFParticle> d2pi;
  vector<CbmKFParticle> d4pi;

  for(unsigned int iKPiPi=0; iKPiPi<kpipi.size(); iKPiPi++)
    CombineTrackPart(vTracks,vField,d0pi,kpipi[iKPiPi],DaughterPDG[2], MotherPDG[5], id[2], cuts[5], 0, 0);

  for(unsigned int iKPi=0; iKPi<kpi.size(); iKPi++)
    CombineTrackPart(vTracks,vField,d2pi,kpi[iKPi],DaughterPDG[0], MotherPDG[6], id[0], cuts[6], 0, 0);

  for(unsigned int iKPiPiPi=0; iKPiPiPi<kpipipi.size(); iKPiPiPi++)
    CombineTrackPart(vTracks,vField,d4pi,kpipipi[iKPiPiPi],DaughterPDG[0], MotherPDG[7], id[0], cuts[7], 0, 0);

  SelectParticleCandidates(Particles, d0pi, PrimVtx, cuts[5]);
  SelectParticleCandidates(Particles, d2pi, PrimVtx, cuts[6]);
  SelectParticleCandidates(Particles, d4pi, PrimVtx, cuts[7]);
}

//template<class T>
void CbmKFParticleInterface::CombineTrackPart(vector<CbmKFTrack>& vTracks,
                                              const vector<L1FieldRegion>& vField,
                                              vector<CbmKFParticle>& Particles,
                                              CbmKFParticle& part,
                                              const int DaughterPDG,
                                              const int MotherPDG,
                                              vector<short>& id,
                                              const float* cuts,
                                              const unsigned short startIndex,
                                              const bool IsSamePart)
{
  CbmKFParticle_simd vDaughters[2] = {CbmKFParticle_simd(part),CbmKFParticle_simd()};

  CbmKFTrackInterface* vTr[fvecLen];

  fvec vtxGuess[3] = {part.GetX(), part.GetY(), part.GetZ()};
  fvec errGuess[3] = {fvec(10.f*sqrt(part.GetCovMatrix()[0])),
                      fvec(10.f*sqrt(part.GetCovMatrix()[2])),
                      fvec(10.f*sqrt(part.GetCovMatrix()[5]))};

  const unsigned short NTr = id.size();
  for(unsigned short iTr=startIndex; iTr < NTr; iTr += fvecLen)
  {
    const unsigned short NTracks = (iTr + fvecLen < NTr) ? fvecLen : (NTr - iTr);

    L1FieldRegion trField;
    for(unsigned short iv=0; iv<NTracks; iv++)
    {
      vTr[iv] = &vTracks[id[iTr+iv]];

      int entrSIMDPos = id[iTr+iv] % fvecLen;
      int entrVecPos  = id[iTr+iv] / fvecLen;
      trField.SetOneEntry(iv,vField[entrVecPos],entrSIMDPos);
    }

    vDaughters[1].Create(vTr,NTracks,0,&DaughterPDG);
    vDaughters[1].SetField(trField);
    fvec trId;
    for(int iv=0; iv<NTracks; iv++)
      trId[iv] = id[iTr+iv];
    vDaughters[1].SetId(trId);

    CbmKFParticleInterface mother;
    mother.SetPDG( MotherPDG );
    if(IsSamePart)
    {
      mother.SetVtxGuess(vtxGuess[0], vtxGuess[1], vtxGuess[2]);
      mother.SetVtxErrGuess(errGuess[0], errGuess[1], errGuess[2]);
    }
    mother.Construct(vDaughters, 2, 0);

    for(int iv=0; iv<NTracks; iv++)
    {
      if(!finite(mother.GetChi2()[iv])) continue;
      if(!(mother.GetChi2()[iv] > 0.0f)) continue;
      if(!(mother.GetChi2()[iv]==mother.GetChi2()[iv])) continue;

      if( mother.GetChi2()[iv]/mother.GetNDF()[iv] > cuts[1] ) continue;

      bool isSameTrack = 0;
      for(unsigned short iD=0; iD<part.DaughterIds().size(); iD++)
        if(part.DaughterIds()[iD] == id[iTr+iv]) isSameTrack=1;
      if(isSameTrack) continue;

      CbmKFParticle mother_temp;
      mother.GetKFParticle(mother_temp, iv);
      mother_temp.CleanDaughtersId();
      mother_temp.AddDaughter(id[iTr+iv]);
      for(unsigned short iD=0; iD<part.DaughterIds().size(); iD++)
        mother_temp.AddDaughter(part.DaughterIds()[iD]);
      Particles.push_back(mother_temp);
    }
  }
}

void CbmKFParticleInterface::SelectParticleCandidates(vector<CbmKFParticle>& Particles,
                                                      vector<CbmKFParticle>& vCandidates,
                                                      CbmKFVertex& PrimVtx,
                                                      const float cuts[5])
{
  CbmKFParticle* cand[fvecLen];
  int nCand = vCandidates.size();

  for(unsigned short iC=0; iC < nCand; iC += fvecLen)
  {
    unsigned int nEntries = (iC + fvecLen < nCand) ? fvecLen : (nCand - iC);

    for(unsigned short iv=0; iv<nEntries; iv++)
      cand[iv] = &vCandidates[iC+iv];

    CbmKFParticleInterface candTopo(cand,nEntries);

    candTopo.MeasureProductionVertex(&candTopo.KFPart, candTopo.GetParameters(), &PrimVtx);

    for(unsigned int iv=0; iv<nEntries; iv++)
    {
      if(!finite(candTopo.GetChi2()[iv])) continue;
      if(!(candTopo.GetChi2()[iv] > 0.0f)) continue;
      if(!(candTopo.GetChi2()[iv]==candTopo.GetChi2()[iv])) continue;
      if(candTopo.GetZ()[iv] < cuts[2]) continue;

      if(candTopo.GetChi2()[iv]/candTopo.GetNDF()[iv] > cuts[4] ) continue;

      Particles.push_back(vCandidates[iC+iv]);
    }
  }
}



//template<class T> 
void CbmKFParticleInterface::ConstructPVT(vector<CbmKFTrack>& vRTracks)
{

}

// void CbmKFParticleInterface::ConstructPV(vector<CbmL1Track>& vRTracks)
// {
//   ConstructPVT(vRTracks);
// }
// 
// void CbmKFParticleInterface::ConstructPV(vector<CbmStsTrack>& vRTracks)
// {
//   ConstructPVT(vRTracks);
// }

#undef cnst
