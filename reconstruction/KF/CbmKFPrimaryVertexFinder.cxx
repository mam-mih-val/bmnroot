/** Implementation of the CbmKFPrimaryVertexFinder class
 *
 * @author  S.Gorbunov, I.Kisel
 * @version 1.0
 * @since   06.02.06
 *
 */
#include "CbmKFPrimaryVertexFinder.h"

#include "CbmKF.h"
#include "CbmKFTrack.h"

#include <vector>

using std::vector;

ClassImp(CbmKFPrimaryVertexFinder)

void CbmKFPrimaryVertexFinder::Clear()
{
  Tracks.clear();
}
  
void CbmKFPrimaryVertexFinder::AddTrack( CbmKFTrackInterface* Track )
{
  Tracks.push_back(Track);
}

void CbmKFPrimaryVertexFinder::SetTracks( vector<CbmKFTrackInterface*> &vTr )
{
  Tracks = vTr;
}

void CbmKFPrimaryVertexFinder::Fit( CbmKFVertexInterface &vtx )
{
  //* Constants 

  const Double_t CutChi2  = 3.5*3.5;
  const Int_t   MaxIter = 3;

  //* Vertex state vector and the covariance matrix

  Double_t r[3], *C = vtx.GetCovMatrix();
  
  //* Initialize the vertex 
  
  for( Int_t i=0; i<6; i++ ) C[i]=0;
  if( CbmKF::Instance()->vTargets.empty() ){
    r[0] = r[1] = r[2] = 0.;
    C[0] = C[2] = 5.;
    C[5] = 0.25;
  }else{
    CbmKFTube &t = CbmKF::Instance()->vTargets[0];
    r[0] = r[1] = 0.;
    r[2] = t.z;
    C[0] = C[2] = t.RR/3.5/3.5;
    C[5] = (t.dz/2/3.5)*(t.dz/2/3.5);
  }
  
  //* Iteratively fit vertex - number of iterations fixed at MaxIter

  for( Int_t iteration = 0; iteration < MaxIter; ++iteration ){ 

    //* Store vertex from previous iteration
    
    Double_t r0[3], C0[6];
    
    for( Int_t i=0; i<3; i++ ) r0[i] = r[i];
    for( Int_t i=0; i<6; i++ ) C0[i] = C[i];
    
    //* Initialize the vertex covariance, Chi^2 & NDF
    
    C[0] = 100.;
    C[1] =   0.; C[2] = 100.;
    C[3] =   0.; C[4] =   0.; C[5] = 100.;
    
    vtx.GetRefNDF()  = -3;
    vtx.GetRefChi2() =  0.;
    vtx.GetRefNTracks() = 0;
  
    for( vector<CbmKFTrackInterface*>::iterator itr  = Tracks.begin();
	                                        itr != Tracks.end()  ; ++itr ){

      CbmKFTrack T(**itr);
      Bool_t err = T.Extrapolate( r0[2] );
      if( err ) continue;
	
      Double_t *m = T.GetTrack();
      Double_t *V = T.GetCovMatrix();
      Double_t a = 0, b = 0;
      {
	Double_t zeta[2] = { r0[0]-m[0],  r0[1]-m[1] };
	
	//* Check track Chi^2 deviation from the r0 vertex estimate
	
	Double_t S[3] = { (C0[2]+V[2]), -(C0[1]+V[1]), (C0[0]+V[0]) };
	Double_t    s =  S[2]*S[0] - S[1]*S[1];
	Double_t chi2 =  zeta[0]*zeta[0]*S[0] + 2*zeta[0]*zeta[1]*S[1] 
	                                      +   zeta[1]*zeta[1]*S[2];
	if( chi2 > s*CutChi2 ) continue;
	
	//* Fit of vertex track slopes (a,b) to r0 vertex
	
	s = V[0]*V[2] - V[1]*V[1];
	if ( s < 1.E-20 ) continue;
	s = 1./s;
	a = m[2] + s*(   (  V[3]*V[2] - V[4]*V[1] )*zeta[0] 
		       + (- V[3]*V[1] + V[4]*V[0] )*zeta[1] );
	b = m[3] + s*(   (  V[6]*V[2] - V[7]*V[1] )*zeta[0] 
		       + (- V[6]*V[1] + V[7]*V[0] )*zeta[1] );
      }
	
      //** Update the vertex (r,C) with the track estimate (m,V) :
	
      //* Linearized measurement matrix H = { { 1, 0, -a}, { 0, 1, -b} };      

      //* Residual (measured - estimated) 
      
      Double_t zeta[2] = { m[0] - ( r[0] - a*(r[2]-r0[2]) ), 
			   m[1] - ( r[1] - b*(r[2]-r0[2]) ) };
      
      //* CHt = CH'	    
      
      Double_t CHt[3][2] = { { C[0] - a*C[3], C[1] - b*C[3]},
			     { C[1] - a*C[4], C[2] - b*C[4]},
			     { C[3] - a*C[5], C[4] - b*C[5]} };

      //* S = (H*C*H' + V )^{-1} 
      
      Double_t S[3] = { V[0] + CHt[0][0] - a*CHt[2][0], 
			V[1] + CHt[1][0] - b*CHt[2][0],
			V[2] + CHt[1][1] - b*CHt[2][1] };
      
      //* Invert S
      {
	Double_t w = S[0]*S[2] - S[1]*S[1];
	if ( w < 1.E-20 ) continue;
	w = 1./w;
	Double_t S0 = S[0];
	S[0] =  w*S[2];
	S[1] = -w*S[1];
	S[2] =  w*S0;
      }

      //* Calculate Chi^2
      
      vtx.GetRefChi2()+=  zeta[0]*zeta[0]*S[0] + 2*zeta[0]*zeta[1]*S[1]
	+ zeta[1]*zeta[1]*S[2] + T.GetRefChi2();
      vtx.GetRefNDF() += 2 + T.GetRefNDF();            
      vtx.GetRefNTracks()++;

     //* Kalman gain K = CH'*S
      
      Double_t K[3][2];
	
      for( Int_t i=0; i<3; ++i ){  K[i][0] = CHt[i][0]*S[0] + CHt[i][1]*S[1] ;
                                   K[i][1] = CHt[i][0]*S[1] + CHt[i][1]*S[2] ; }
      
      //* New estimation of the vertex position r += K*zeta
      
      for( Int_t i=0; i<3; ++i ) r[i]+= K[i][0]*zeta[0] + K[i][1]*zeta[1];	  
      
      //* New covariance matrix C -= K*(CH')'
      
      C[0] -= K[0][0]*CHt[0][0] + K[0][1]*CHt[0][1];
      C[1] -= K[1][0]*CHt[0][0] + K[1][1]*CHt[0][1];
      C[2] -= K[1][0]*CHt[1][0] + K[1][1]*CHt[1][1];
      C[3] -= K[2][0]*CHt[0][0] + K[2][1]*CHt[0][1];
      C[4] -= K[2][0]*CHt[1][0] + K[2][1]*CHt[1][1];
      C[5] -= K[2][0]*CHt[2][0] + K[2][1]*CHt[2][1];	
      
    }//* itr        

  }//* finished iterations
  
  //* Copy state vector to output (covariance matrix was used directly )
  
  vtx.GetRefX() = r[0];
  vtx.GetRefY() = r[1];
  vtx.GetRefZ() = r[2];
}
