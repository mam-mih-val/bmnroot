 /** Implementation of the CbmKFSecondaryVertexFinder class
 *
 * @author  S.Gorbunov, I.Kisel
 * @version 1.0
 * @since   06.02.06
 *
 */

#include "CbmKFSecondaryVertexFinder.h"

#include "CbmKF.h"
#include "CbmKFTrack.h"

#include <cmath>

using std::vector;
using std::fabs;

ClassImp(CbmKFSecondaryVertexFinder)

void CbmKFSecondaryVertexFinder::Clear()
{
  vTracks.clear();
  VGuess =0;
  VParent = 0;
  MassConstraint = -1;
}

void CbmKFSecondaryVertexFinder::ClearTracks()
{
  vTracks.clear();
}
  
void CbmKFSecondaryVertexFinder::AddTrack( CbmKFTrackInterface* Track )
{
  vTracks.push_back(Track);
}

void CbmKFSecondaryVertexFinder::SetTracks( vector<CbmKFTrackInterface*> &vTr )
{
  vTracks = vTr;
}

void CbmKFSecondaryVertexFinder::SetApproximation( CbmKFVertexInterface *Guess )
{
  VGuess = Guess;
}

void CbmKFSecondaryVertexFinder::SetMassConstraint( Double_t MotherMass )
{
  MassConstraint = MotherMass;
}

void CbmKFSecondaryVertexFinder::SetTopoConstraint( CbmKFVertexInterface *VP )
{
  VParent = VP;
}

void CbmKFSecondaryVertexFinder::Fit()
{
  const Int_t MaxIter=3;

  if( VGuess ){
    r[0] = VGuess->GetRefX();
    r[1] = VGuess->GetRefY();
    r[2] = VGuess->GetRefZ();
  }else {
    if( CbmKF::Instance()->vTargets.empty() ){
      r[0] = r[1] = r[2] = 0.;
    }else{
      CbmKFTube &t = CbmKF::Instance()->vTargets[0];
      r[0] = r[1] = 0.;
      r[2] = t.z;
    }
  }
 
    r[3] = 0;
    r[4] = 0;
    r[5] = 0;
    r[6] = 0;
    r[7] = 0;

  for ( Int_t iteration =0; iteration<MaxIter;++iteration ){
   
    for( int i=0; i<8; i++) r0[i] = r[i];

    r[3] = 0;
    r[4] = 0;
    r[5] = 0;
    r[6] = 0;
   
    for(Int_t i=0;i<36;++i) C[i]=0.0;    

    C[0] = C[2] = C[5] = 100.0;
    C[35] = 1.E4;
    NDF  = -3;
    Chi2 =  0.;
    
    for( vector<CbmKFTrackInterface*>::iterator tr=vTracks.begin(); tr!=vTracks.end(); ++tr )
      {
	CbmKFTrack T( **tr );

	T.Extrapolate( r0[2] );
    
	Double_t *m = T.GetTrack();
	Double_t *V = T.GetCovMatrix();

	//* Fit of vertex track slopes and momentum (a,b,qp) to r0 vertex

	Double_t a = m[2], b = m[3], qp = m[4];
	{  
	  Double_t s = V[0]*V[2] - V[1]*V[1];	 
	  s = ( s > 1.E-20 )  ?1./s :0;
	  Double_t zetas[2] = { (r0[0]-m[0])*s,  (r0[1]-m[1])*s };
	  a += ( V[ 3]*V[2] -V[ 4]*V[1])*zetas[0] 
	      +(-V[ 3]*V[1] +V[ 4]*V[0])*zetas[1];
	  b += ( V[ 6]*V[2] -V[ 7]*V[1])*zetas[0] 
	      +(-V[ 6]*V[1] +V[ 7]*V[0])*zetas[1];
	  qp+= (V[10]*V[2]-V[11]*V[1])*zetas[0] 
	      +(-V[10]*V[1]+V[11]*V[0])*zetas[1];
	}
	
	//* convert the track to (x,y,px,py,pz,e) parameterization
	double mm[6], VV[21];
	{
	  double c2 = 1./(1. + a*a + b*b);
	  double p = 1./qp;
	  double p2 = p*p;
	  double pz = sqrt(p2*c2);
	  double px = a*pz;
	  double py = b*pz;
	  double E = sqrt( T.GetMass()*T.GetMass() + p2 );
	  
	  double da = (m[2]-a);
	  double db = (m[3]-b);
	  double dq = (m[4]-qp);
	  
	  double H[3] = { -px*c2, -py*c2, -pz*p };
	  double HE = -p*p2/E;
	  
	  double dpz = H[0]*da + H[1]*db + H[2]*dq;

	  mm[0] = m[0];
	  mm[1] = m[1];
	  mm[2] = px + da + a*dpz;
	  mm[3] = py + db + b*dpz;
	  mm[4] = pz + dpz;
	  mm[5] = E + HE*dq;

	  double cxpz = H[0]*V[ 3] + H[1]*V[ 6] + H[2]*V[10];
	  double cypz = H[0]*V[ 4] + H[1]*V[ 7] + H[2]*V[11];
	  double capz = H[0]*V[ 5] + H[1]*V[ 8] + H[2]*V[12];
	  double cbpz = H[0]*V[ 8] + H[1]*V[ 9] + H[2]*V[13];
	  double cqpz = H[0]*V[12] + H[1]*V[13] + H[2]*V[14];
	  double cpzpz = H[0]*H[0]*V[5] +H[1]*H[1]*V[9] + H[2]*H[2]*V[14] 
	    + 2*( H[0]*H[1]*V[8] +H[0]*H[2]*V[12] +H[1]*H[2]*V[13]);

	  VV[ 0] = V[0];
	  VV[ 1] = V[1];
	  VV[ 2] = V[2];
	  VV[ 3] = V[3]*pz + a*cxpz; 
	  VV[ 4] = V[4]*pz + a*cypz; 
	  VV[ 5] = V[5]*pz*pz + 2*a*pz*capz + a*a*cpzpz;
	  VV[ 6] = V[6]*pz+b*cxpz; 
	  VV[ 7] = V[7]*pz+b*cypz; 
	  VV[ 8] = V[8]*pz*pz + a*pz*cbpz + b*pz*capz + a*b*cpzpz;
	  VV[ 9] = V[9]*pz*pz + 2*b*pz*cbpz + b*b*cpzpz;
	  VV[10] = cxpz; 
	  VV[11] = cypz; 
	  VV[12] = capz*pz + a*cpzpz;
	  VV[13] = cbpz*pz + b*cpzpz;
	  VV[14] = cpzpz;
	  VV[15] = HE*V[10];
	  VV[16] = HE*V[11];
	  VV[17] = HE*( V[12]*pz + a*cqpz );
	  VV[18] = HE*( V[13]*pz + b*cqpz );
	  VV[19] = HE*cqpz;
	  VV[20] = HE*HE*V[14];	  
	}
	

	//* Measure the state vector with the track estimate

	Chi2 +=  T.GetRefChi2();
	NDF  +=  T.GetRefNDF() - 3;

	//* Update the state vector with the momentum part of the track estimate

	r[ 3] += mm[ 2];
	r[ 4] += mm[ 3];
	r[ 5] += mm[ 4];
	r[ 6] += mm[ 5];

	C[ 9] += VV[ 5];
	C[13] += VV[ 8];
	C[14] += VV[ 9];
	C[18] += VV[12];
	C[19] += VV[13];
	C[20] += VV[14];
	C[24] += VV[17];
	C[25] += VV[18];
	C[26] += VV[19];
	C[27] += VV[20];

	NDF  += 3;
	Chi2 += 0.;

	//* Update the state vector with the coordinate part of the track estimate

	//* Residual (measured - estimated)

	Double_t zeta[2] = { mm[0]-(r[0]-a*(r[2]-r0[2])), 
			     mm[1]-(r[1]-b*(r[2]-r0[2])) };
      
	//* Measurement matrix H= { { 1, 0, -a, 0..0}, { 0, 1, -b,0..0} };
      
	//* S = (H*C*H' + V )^{-1} 
      
	Double_t S[3] = { VV[0] + C[0] - 2*a*C[3]          + a*a*C[5],
			  VV[1] + C[1] -   b*C[3] - a*C[4] + a*b*C[5],
			  VV[2] + C[2] - 2*b*C[4]          + b*b*C[5]  };
      
	//* Invert S
	{
	  Double_t s = S[0]*S[2] - S[1]*S[1];

	  if ( s < 1.E-20 ) continue;
	  s = 1./s;
	  Double_t S0 = S[0];
	  S[0] =  s*S[2];
	  S[1] = -s*S[1];
	  S[2] =  s*S0;
	}
	  
	//* CHt = CH' - D'
	
	Double_t CHt0[7], CHt1[7];

	CHt0[0]= C[ 0] -a*C[ 3];          CHt1[0]= C[ 1] -b*C[ 3];
	CHt0[1]= C[ 1] -a*C[ 4];          CHt1[1]= C[ 2] -b*C[ 4];
	CHt0[2]= C[ 3] -a*C[ 5];          CHt1[2]= C[ 4] -b*C[ 5];
	CHt0[3]= C[ 6] -a*C[ 8] -VV[ 3];  CHt1[3]= C[ 7] -b*C[ 8] -VV[ 4];
	CHt0[4]= C[10] -a*C[12] -VV[ 6];  CHt1[4]= C[11] -b*C[12] -VV[ 7];
	CHt0[5]= C[15] -a*C[17] -VV[10];  CHt1[5]= C[16] -b*C[17] -VV[11];
	CHt0[6]= C[21] -a*C[23] -VV[15];  CHt1[6]= C[22] -b*C[23] -VV[16];

	//* Kalman gain K = CH'*S
	
	Double_t K0[7], K1[7];

	for(Int_t i=0;i<7;++i){
	  K0[i] = CHt0[i]*S[0] + CHt1[i]*S[1];
	  K1[i] = CHt0[i]*S[1] + CHt1[i]*S[2];
	}

	//* New estimation of the vertex position r += K*zeta

	for(Int_t i=0;i<7;++i) r[i] += K0[i]*zeta[0] + K1[i]*zeta[1];
      
	//* New covariance matrix C -= K*(CH')'
      
	for(Int_t i=0, k=0;i<7;++i){
	  for(Int_t j=0;j<=i;++j,++k) 
	    C[k] -= K0[i]*CHt0[j] + K1[i]*CHt1[j];
	}

	//* Calculate Chi^2 & NDF

	NDF  += 2;
	Chi2 +=   zeta[0]*zeta[0]*S[0] + 2*zeta[0]*zeta[1]*S[1] 
	        + zeta[1]*zeta[1]*S[2] ;
	
      } // add tracks 

    // Put constraints if they exist

    AddTopoConstraint();
    AddMassConstraint();
    
  }// iterations
}


void CbmKFSecondaryVertexFinder::GetVertex( CbmKFVertexInterface  &vtx )
{
  vtx.GetRefX() = r[0];   
  vtx.GetRefY() = r[1];   
  vtx.GetRefZ() = r[2];
  for( int i=0; i<6; i++) vtx.GetCovMatrix()[i] = C[i];
  vtx.GetRefChi2() = Chi2;
  vtx.GetRefNDF()  = NDF;
}

void CbmKFSecondaryVertexFinder::GetVertex( CbmVertex  &vtx )
{
  CbmKFVertexInterface vi;
  GetVertex( vi );
  vi.GetVertex(vtx);
}


void CbmKFSecondaryVertexFinder::
     GetMotherTrack( Double_t Par[], Double_t Cov[] )
{

  if( !Par ) return;

  Double_t px=r[3], py=r[4],  pz=r[5];

  Double_t p = sqrt( px*px + py*py + pz*pz );
  double pzi = 1/pz;
  double qp=1/p;
  Par[0] = r[0];
  Par[1] = r[1];
  Par[2] = px*pzi;
  Par[3] = py*pzi;
  Par[4] = qp;
  Par[5] = r[2];
  
  if( !Cov ) return;

  double qp3=qp*qp*qp;
  Double_t J[5][6];

  for ( Int_t i=0; i<5; i++)
    for (Int_t j=0;j<6;++j) J[i][j]=0.0; 

      
  J[0][0] = 1; J[0][2] = -Par[2];
  J[1][1] = 1; J[1][2] = -Par[3];
  J[2][3] = pzi; J[2][5] =  -Par[2]*pzi;
  J[3][4] = pzi; J[3][5] =  -Par[3]*pzi;
  J[4][3] = -qp3*px; J[4][4] = -qp3*py; J[4][4] = -qp3*pz; 
  
  Double_t JC[5][6];
  
  for( Int_t i=0; i<5; i++)
    for(Int_t j=0;j<6;j++){
      JC[i][j]=0;
      for( Int_t k=0; k<6; k++) JC[i][j]+=J[i][k]*Cij(k,j);
    }
        
  Int_t ii =0 ;
  for( Int_t i=0; i<5; i++)
    for(Int_t j=i;j<5;j++, ii++){
      Cov[ii]=0;
      for( Int_t k=0; k<6; k++) Cov[ii]+=JC[i][k]*J[j][k];		  
    }
}


void CbmKFSecondaryVertexFinder::GetMass( Double_t *M, Double_t *Error_ )
{
  // S = sigma^2 of m2/2

  Double_t S = (   r[3]*r[3]*C[9] + r[4]*r[4]*C[14] + r[5]*r[5]*C[20] 
		 + r[6]*r[6]*C[27] 
		 + 2*( + r[3]*r[4]*C[13] + r[5]*(r[3]*C[18] + r[4]*C[19]) 
		       - r[6]*( r[3]*C[24] + r[4]*C[25] + r[5]*C[26] )   )
		 ); 
  Double_t m2 = r[6]*r[6] - r[3]*r[3] - r[4]*r[4] - r[5]*r[5];
  
  if( M     ) *M     = (         m2>1.e-20 ) ? sqrt(m2)   :0.  ;
  if( Error_ ) *Error_ = ( S>=0 && m2>1.e-20 ) ? sqrt(S/m2) :1.e4;
}


void CbmKFSecondaryVertexFinder::AddMassConstraint()
{
  if( MassConstraint<0 ) return;
  double H[8];
  H[0] = H[1] = H[2] = 0.;
  H[3] = -2*r0[3]; 
  H[4] = -2*r0[4]; 
  H[5] = -2*r0[5]; 
  H[6] =  2*r0[6];
  H[7] = 0; 
  double m2 = r0[6]*r0[6] - r0[3]*r0[3] - r0[4]*r0[4] - r0[5]*r0[5]; 

  double zeta = MassConstraint*MassConstraint - m2;
  for(Int_t i=0;i<8;++i) zeta -= H[i]*(r[i]-r0[i]);
  
  Double_t S = 0.;
  Double_t CHt[8];
  for (Int_t i=0;i<8;++i ){
    CHt[i] = 0.0;
    for (Int_t j=0;j<8;++j) CHt[i] += Cij(i,j)*H[j];
    S += H[i]*CHt[i];
  }
    
  if( S<1.e-20 ) return;
  S = 1./S;
  Chi2 += zeta*zeta*S;
  NDF  += 1;
  for( Int_t i=0, ii=0; i<8; ++i ){
    Double_t Ki = CHt[i]*S;
    r[i]+= Ki*zeta;
    for(Int_t j=0;j<=i;++j) C[ii++] -= Ki*CHt[j];    
  }
}




void CbmKFSecondaryVertexFinder::Extrapolate( double T )
{
  
  r0[0]+= T*r0[3];
  r0[1]+= T*r0[4];
  r0[2]+= T*r0[5];

  r[0]+= T*r[3];
  r[1]+= T*r[4];
  r[2]+= T*r[5];

  double C30 = C[ 6] + T*C[ 9];
  double C41 = C[11] + T*C[14];
  double C52 = C[17] + T*C[20];
  double T54 = T*C[19];

  C[ 0] += T*( C[ 6] + C30 );
  C[ 1] += T*( C[ 7] + C[10] );
  C[ 2] += T*( C[11] + C41 );
  C[10] += T*C[13];
  C[ 6]  = C30;
  C[11]  = C41;

  C[15] += T*C[18];
  C[16] += T54;
  C[ 3] += T*(C[ 8] + C[15]);
  C[ 4] += T*(C[12] + C[16]);
  C[ 5] += T*(C[17] + C52);
  C[17]  = C52;
  C[12] += T54;

  C[ 7] += T*C[13];
  C[ 8] += T*C[18];
  
  C[21] += T*C[24];
  C[22] += T*C[25];
  C[23] += T*C[26];
  C[28] += T*C[31];
  C[29] += T*C[32];
  C[30] += T*C[33];

}

void CbmKFSecondaryVertexFinder::AddTopoConstraint()
{
  if( !VParent ) return;
  
  double m[3], *V;
  {    
    m[0] = VParent->GetRefX();
    m[1] = VParent->GetRefY();
    m[2] = VParent->GetRefZ();
    V = VParent->GetCovMatrix();
  }

  Extrapolate(-r0[7]);

  {
    double inf = 1.e8;
    double xinf = r0[0]*inf;
    double yinf = r0[1]*inf;
    double zinf = r0[2]*inf;
    C[0] += r0[0]*xinf;
    C[1] += r0[0]*yinf;
    C[2] += r0[1]*yinf;
    C[3] += r0[0]*zinf;
    C[4] += r0[1]*zinf;
    C[5] += r0[2]*zinf;
    C[28] = -2*xinf;
    C[29] = -2*yinf;
    C[30] = -2*zinf;
    C[31] = 0;
    C[32] = 0;
    C[33] = 0;
    C[34] = 0;
    C[35] = inf;
  }
  r[7] = r0[7];

  double Ai[6];
  Ai[0] = C[4]*C[4] - C[2]*C[5];
  Ai[1] = C[1]*C[5] - C[3]*C[4];
  Ai[3] = C[2]*C[3] - C[1]*C[4];
  double det = (C[0]*Ai[0] + C[1]*Ai[1] + C[3]*Ai[3]);
  det = (det>1.e-8) ?1./det :0;
  Ai[0] *= det;
  Ai[1] *= det;
  Ai[3] *= det;
  Ai[2] = ( C[3]*C[3] - C[0]*C[5] )*det;
  Ai[4] = ( C[0]*C[4] - C[1]*C[3] )*det;
  Ai[5] = ( C[1]*C[1] - C[0]*C[2] )*det;

  double B[5][3];
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

  double z[3] = {m[0]-r[0], m[1]-r[1], m[2]-r[2]};
  r[0] = m[0];
  r[1] = m[1];
  r[2] = m[2];
  r[3]+= B[0][0]*z[0] + B[0][1]*z[1] + B[0][2]*z[2];
  r[4]+= B[1][0]*z[0] + B[1][1]*z[1] + B[1][2]*z[2];
  r[5]+= B[2][0]*z[0] + B[2][1]*z[1] + B[2][2]*z[2];
  r[6]+= B[3][0]*z[0] + B[3][1]*z[1] + B[3][2]*z[2];
  r[7]+= B[4][0]*z[0] + B[4][1]*z[1] + B[4][2]*z[2];
  
  {
    double AV[6] = { C[0]-V[0], C[1]-V[1], C[2]-V[2], 
		     C[3]-V[3], C[4]-V[4], C[5]-V[5] };
    double AVi[6];
    AVi[0] = AV[4]*AV[4] - AV[2]*AV[5];
    AVi[1] = AV[1]*AV[5] - AV[3]*AV[4];
    AVi[2] = AV[3]*AV[3] - AV[0]*AV[5] ;
    AVi[3] = AV[2]*AV[3] - AV[1]*AV[4];
    AVi[4] = AV[0]*AV[4] - AV[1]*AV[3] ;
    AVi[5] = -123; // FIXME assing correct value
    det = ( AV[0]*AVi[0] + AV[1]*AVi[1] + AV[3]*AVi[3] );
    det = (det>1.e-8) ?1./det :0;
    NDF  += 2;
    Chi2 += ( +(AVi[0]*z[0] + AVi[1]*z[1] + AVi[3]*z[2])*z[0]
	      +(AVi[1]*z[0] + AVi[2]*z[1] + AVi[4]*z[2])*z[1]
	      +(AVi[3]*z[0] + AVi[4]*z[1] + AVi[5]*z[2])*z[2] )*det;
  }

  double d0, d1, d2;
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
  
  {
    Extrapolate(r[7]);

    double CH00 = C[0]+r0[0]*C[28];
    double CH10 = C[1]+r0[0]*C[29];
    double CH11 = C[2]+r0[1]*C[29];
    double CH20 = C[3]+r0[0]*C[30];
    double CH21 = C[4]+r0[1]*C[30];
    double CH22 = C[5]+r0[2]*C[30];

    C[28]+= r0[0]*C[35];
    C[29]+= r0[1]*C[35];
    C[30]+= r0[2]*C[35];
 
    C[0] = CH00+r0[0]*C[28];
    C[1] = CH10+r0[1]*C[28];
    C[2] = CH11+r0[1]*C[29];
    C[3] = CH20+r0[2]*C[28];
    C[4] = CH21+r0[2]*C[29];
    C[5] = CH22+r0[2]*C[30];
 
    C[6]+= r0[0]*C[31];
    C[7]+= r0[1]*C[31];
    C[8]+= r0[2]*C[31];
    C[10]+= r0[0]*C[32];
    C[11]+= r0[1]*C[32];
    C[12]+= r0[2]*C[32];
    C[15]+= r0[0]*C[33];
    C[16]+= r0[1]*C[33];
    C[17]+= r0[2]*C[33];
    C[21]+= r0[0]*C[34]; 
    C[22]+= r0[1]*C[34];
    C[23]+= r0[2]*C[34];

  }
}
