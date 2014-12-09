#include "CbmKFUMeasurement.h"

#include <cmath>

ClassImp(CbmKFUMeasurement)

void CbmKFUMeasurement::Set( Double_t z_, Double_t u_, Double_t phi_, Double_t sigma2_ ){
  this->z       = z_;
  this->u       = u_;
  this->phi     = phi_;
  this->sigma2  = sigma2_;
  phi_s   = sin(phi_);
  phi_c   = cos(phi_);
  phi_ss  = phi_s * phi_s;
  phi_cc  = phi_c * phi_c;
  phi_2sc = 2.* phi_s * phi_c; 
}

Int_t CbmKFUMeasurement::Filter( CbmKFTrackInterface &track ){
  
  Double_t *T = track.GetTrack();
  Double_t *C = track.GetCovMatrix();  

  Double_t W = sigma2+phi_cc*C[0]+phi_2sc*C[1]+phi_ss*C[2];  
  if( !finite(W) || W<1.e-10 ) return 1;
  W = 1./W;
  if( !finite(W) ) return 1;

  Double_t zeta = phi_c*T[0] + phi_s*T[1] - u;

  track.GetRefChi2() += zeta * zeta * W;
  track.GetRefNDF() += 1;
  Double_t K[5] = { phi_c*C[ 0] + phi_s*C[ 1],
		    phi_c*C[ 1] + phi_s*C[ 2],
		    phi_c*C[ 3] + phi_s*C[ 4],
		    phi_c*C[ 6] + phi_s*C[ 7],
		    phi_c*C[10] + phi_s*C[11]  };
  
  Double_t KW[5] = { K[0]*W,
		     K[1]*W,
		     K[2]*W,
		     K[3]*W,
		     K[4]*W  };
  
  T[0]-= KW[0]*zeta;
  T[1]-= KW[1]*zeta;
  T[2]-= KW[2]*zeta;
  T[3]-= KW[3]*zeta;
  T[4]-= KW[4]*zeta;
    
  C[ 0]-= K[0]*KW[0];
  C[ 1]-= K[1]*KW[0];
  C[ 2]-= K[1]*KW[1];
  C[ 3]-= K[2]*KW[0];
  C[ 4]-= K[2]*KW[1];
  C[ 5]-= K[2]*KW[2];
  C[ 6]-= K[3]*KW[0];
  C[ 7]-= K[3]*KW[1];
  C[ 8]-= K[3]*KW[2];
  C[ 9]-= K[3]*KW[3];      
  C[10]-= K[4]*KW[0];
  C[11]-= K[4]*KW[1];
  C[12]-= K[4]*KW[2];
  C[13]-= K[4]*KW[3];
  C[14]-= K[4]*KW[4];     
  return 0;
}
