
/** Abstract base class for track is being used by the CbmKF Kalman Filter 
 *
 * @author  S.Gorbunov
 * @version 0.0
 * @since   02.12.05
 * 
 * The class members describes to the CbmKF Kalman Filter routines 
 * how to get acces to data in a derived (or user) class.
 *
 * In case the methods below are not rewriten in a derived class, 
 * temporary variables will be used in CbmKF routines.
 *
 */

#include "CbmKFTrackInterface.h"

#include "CbmKFVertexInterface.h"
#include "CbmKF.h"
#include "CbmKFFieldMath.h"
#include "CbmKFMath.h"
#include "CbmKFHit.h"

#include <algorithm>

using std::vector;

ClassImp( CbmKFTrackInterface )

static Double_t gTempD[100];
static Int_t gTempI[10];

Double_t *CbmKFTrackInterface::GetTrack()     { return gTempD    ; }  
Double_t *CbmKFTrackInterface::GetCovMatrix() { return gTempD+6  ; }  
Double_t &CbmKFTrackInterface::GetRefChi2()   { return gTempD[21]; }
Int_t    &CbmKFTrackInterface::GetRefNDF()    { return gTempI[ 0]; }


Int_t CbmKFTrackInterface::Extrapolate( Double_t z_out, Double_t *QP0 ){

  Bool_t err = 0;
  CbmKF *KF = CbmKF::Instance();
  const Double_t z_in  = GetTrack()[5];
  Double_t qp0 = (QP0) ?*QP0 :GetTrack()[4];
  Double_t z, Z;
  Bool_t downstream_direction = ( z_out > z_in );
   
  if ( downstream_direction )
    {
      z = z_in;
      Z = z_out;
    }
  else
    {
      z = z_out;
      Z = z_in;
    }

  vector <CbmKFMaterial*>::iterator i, ibeg,iend;
  ibeg = lower_bound( KF->vMaterial.begin(), KF->vMaterial.end(), z, CbmKFMaterial::compareP_z );
  iend = upper_bound( KF->vMaterial.begin(), KF->vMaterial.end(), Z, CbmKFMaterial::compareP_Z );
  if( iend!=KF->vMaterial.end() &&
      (*iend)->ZReference-(*iend)->ZThickness/2<Z ) iend++;   
  if ( downstream_direction ){       
  }else{
    i = ibeg;
    ibeg = iend;
    iend = i;
    ibeg--;
    iend--;
  }

  for( i = ibeg; i!=iend; downstream_direction ?++i :--i )
    {
      Double_t zthick = (*i)->ZThickness, zcross=(*i)->ZReference;      
      if ( CbmKFMath::GetThickness( z, Z, zcross, zthick, &zcross, &zthick ) ) continue;
      
      double z0 = (downstream_direction) ?zcross-zthick/2. :zcross+zthick/2.;
      double d = (downstream_direction) ?1 :-1;
      double dz = 1.E-1*(*i)->RadLength;
      double z_ = 0;
      while( z_ + dz < zthick){
	err = err || (*i)->Pass( z0+d*(z_+dz/2.), dz, *this, downstream_direction, qp0 );
	z_+=dz;
      }
      dz = zthick - z_;
      err = err || (*i)->Pass( z0+d*(z_+dz/2.), dz, *this, downstream_direction, qp0 );
      //(*i)->Pass( zcross, zthick, *this, downstream_direction, qp0 );    
    }
  err = err || Propagate( z_out, qp0 );   
  if( QP0 ) *QP0 = qp0;
  return err;
}


Int_t CbmKFTrackInterface::Fit( Bool_t downstream )
{
  CbmKF *KF = CbmKF::Instance();
  Double_t *T = GetTrack();
  Double_t *C = GetCovMatrix();
  Int_t NHits = GetNOfHits();

  Bool_t err = 0;
  if ( NHits==0 ) return 1;
 
  // use initial momentum
  // this fixed value will be used during fit instead of T[4]

  Double_t qp0 = T[4];
  
  const Double_t INF = 10000.;
  
  GetRefChi2() = 0;
  GetRefNDF() = 0;

  // initialize covariance matrix 

  C[ 0] = INF; 
  C[ 1] = 0.; C[ 2] = INF;
  C[ 3] = 0.; C[ 4] = 0.; C[ 5] = INF;
  C[ 6] = 0.; C[ 7] = 0.; C[ 8] = 0.; C[ 9] = INF;
  C[10] = 0.; C[11] = 0.; C[12] = 0.; C[13] =  0.; C[14] = INF;

  try{

  if( downstream ){
    CbmKFHit *h = GetHit( 0 );
    err = h->Filter( *this, downstream, qp0 );
    Int_t istold = h->MaterialIndex;  
    for( Int_t i=1; i<NHits; i++ ){
      h = GetHit( i );
      Int_t ist = h->MaterialIndex;
      for(Int_t j=istold+1; j<ist; j++)
	err = err || KF->vMaterial[j]->Pass( *this, downstream, qp0 );
      err = err || h->Filter( *this, downstream, qp0 );
      istold = ist;
    }
  }else{
    CbmKFHit *h = GetHit( NHits-1 );
    err = h->Filter( *this, downstream, qp0 );    
    Int_t istold = h->MaterialIndex;  
    for( Int_t i=NHits-2; i>=0; i-- ){
      h = GetHit( i );      
      Int_t ist = h->MaterialIndex;
      for(Int_t j=istold-1; j>ist; j--)
	err = err || KF->vMaterial[j]->Pass( *this, downstream, qp0 );
      err = err || h->Filter( *this, downstream, qp0 );
      istold = ist;
    }    
  }

  // correct NDF value to number of fitted track parameters (straight line(4) or helix(5) )

  GetRefNDF() -= (KF->GetMethod()==0) ?4 :5;
  }
  catch(...){
    GetRefChi2() = 0;
    GetRefNDF() = 0;
    C[ 0] = INF; 
    C[ 1] = 0.; C[ 2] = INF;
    C[ 3] = 0.; C[ 4] = 0.; C[ 5] = INF;
    C[ 6] = 0.; C[ 7] = 0.; C[ 8] = 0.; C[ 9] = INF;
    C[10] = 0.; C[11] = 0.; C[12] = 0.; C[13] =  0.; C[14] = INF;
    T[0] = T[1] = T[2] = T[3] = T[4] = T[5] = 0;
    return 1;
  }
  if( err ){
    GetRefChi2() = 0;
    GetRefNDF() = 0;
    C[ 0] = INF; 
    C[ 1] = 0.; C[ 2] = INF;
    C[ 3] = 0.; C[ 4] = 0.; C[ 5] = INF;
    C[ 6] = 0.; C[ 7] = 0.; C[ 8] = 0.; C[ 9] = INF;
    C[10] = 0.; C[11] = 0.; C[12] = 0.; C[13] =  0.; C[14] = INF;
    T[0] = T[1] = T[2] = T[3] = T[4] = T[5] = 0;    
  }
  return err;
}


void CbmKFTrackInterface::Smooth( Double_t Z )
{
  CbmKF *KF = CbmKF::Instance();

  Double_t *T = GetTrack();
  Double_t *C = GetCovMatrix();
  Int_t NHits = GetNOfHits();

  if ( NHits==0 ) return;

  // use initial momentum
  // this fixed value will be used during fit instead of T[4]

  Double_t qp0 = T[4];

  const Double_t INF = 100.;

  GetRefChi2() = 0;
  GetRefNDF() = 0;

  // initialize covariance matrix 

  C[ 0] = INF; 
  C[ 1] = 0.; C[ 2] = INF;
  C[ 3] = 0.; C[ 4] = 0.; C[ 5] = INF;
  C[ 6] = 0.; C[ 7] = 0.; C[ 8] = 0.; C[ 9] = INF;
  C[10] = 0.; C[11] = 0.; C[12] = 0.; C[13] =  0.; C[14] = INF;

  // fit downstream
  {
    CbmKFHit *h = GetHit( 0 );
    if( KF->vMaterial[h->MaterialIndex]->ZReference <=Z ){
      h->Filter( *this, 1, qp0 );
      Int_t istold = h->MaterialIndex;  
      for( Int_t i=1; i<NHits; i++ ){
	h = GetHit( i );
	Int_t ist = h->MaterialIndex;
	Int_t j=istold+1;
	for(; j<ist; j++){
	  if( KF->vMaterial[j]->ZReference >Z ) break;
	  KF->vMaterial[j]->Pass( *this, 1, qp0 );
	}
	if( j<ist || KF->vMaterial[h->MaterialIndex]->ZReference >Z ) break;
	h->Filter( *this, 1, qp0 );
	istold = ist;
      }
    }
  }

  KF->Propagate(T, C, Z, qp0);
  double Ts[6], Cs[15];
  for(int k=0; k<6;k++) Ts[k] = T[k]; 
  for(int k=0; k<15;k++) Cs[k] = C[k];      
  C[ 0] = INF; 
  C[ 1] = 0.; C[ 2] = INF;
  C[ 3] = 0.; C[ 4] = 0.; C[ 5] = INF;
  C[ 6] = 0.; C[ 7] = 0.; C[ 8] = 0.; C[ 9] = INF;
  C[10] = 0.; C[11] = 0.; C[12] = 0.; C[13] =  0.; C[14] = INF;

  {// fit upstream
    CbmKFHit *h = GetHit( NHits-1 );
    if( KF->vMaterial[h->MaterialIndex]->ZReference >Z ){
      h->Filter( *this, 0, qp0 );    
      Int_t istold = h->MaterialIndex;  
      for( Int_t i=NHits-2; i>=0; i-- ){
	h = GetHit( i );      
	Int_t ist = h->MaterialIndex;
	Int_t j=istold-1;
	for(; j>ist; j--){
	  if( KF->vMaterial[j]->ZReference <=Z ) break;
	  KF->vMaterial[j]->Pass( *this, 0, qp0 );
	}
	if( j>ist || KF->vMaterial[h->MaterialIndex]->ZReference <=Z ) break;    
	h->Filter( *this, 0, qp0 );
	istold = ist;
      }          
    }
  }
  KF->Propagate(T, C, Z, qp0);
 
  { // smooth

    double I[15];
    for(int k=0; k<15;k++) I[k] = C[k] + Cs[k]; 
    CbmKFMath::invS(I,5);
    double K[25];
    CbmKFMath::multSSQ( C, I, K, 5);	
    double r[5];
    for(int k=0; k<5;k++) r[k] = T[k] - Ts[k]; 
    for( int k=0; k<5;k++ )
      for(int l=0;l<5;l++) T[k]-=K[k*5+l]*r[l];
   
    double A[15];

    for( int l=0; l<5; l++ ) K[ (5+1)*l ] -= 1;	
    for( int l=0; l<5; ++l )      
      for( int j=0; j<=l; ++j ){
	int ind = CbmKFMath::indexS(l,j);
	A[ind] = 0;
	for( int k=0; k<5; ++k ) A[ind] -= K[l*5+k] * C[CbmKFMath::indexS(k,j)];
      }
    for( int l=0; l<15; l++ ) C[l] = A[l];

  }

  // correct NDF value to number of fitted track parameters (straight line(4) or helix(5) )

  GetRefNDF() -= (KF->GetMethod()==0) ?4 :5;
  
}

void CbmKFTrackInterface::Fit2Vertex( CbmKFVertexInterface  &vtx )
{
  Double_t *T = GetTrack();
  Double_t *C = GetCovMatrix();
  Double_t *Cv = vtx.GetCovMatrix();


  Double_t &x  = vtx.GetRefX();
  Double_t &y  = vtx.GetRefY();
  Double_t &z  = vtx.GetRefZ();

  Extrapolate( z );
  Int_t MaxIter = 2;

  Double_t a = T[2], b = T[3];
 
  //H = {{1 0 0 0 0 }
  //     {0 1 0 0 0}};
  
  Double_t zeta[2] = { x-T[0],  y-T[1] };    
  
  Double_t CHt[5][2] = { { C[0], C[1]},
			 { C[1], C[2]},
			 { C[3], C[4]},
			 { C[6], C[7]},
			 { C[10],C[11]} };

  for( Int_t iter=0; iter<MaxIter; iter++ ){
    
    Double_t Cv0 = Cv[0] -a*(2*Cv[3] -a*Cv[5]);
    Double_t Cv1 = Cv[1] -b*Cv[3] -a*(Cv[4] -b*Cv[5]);
    Double_t Cv2 = Cv[2] -b*(2*Cv[4] -b*Cv[5]);

    Double_t S[3] = { C[0] + Cv0, C[1] + Cv1, C[2] + Cv2 };
      
    //* Invert S
    {
      Double_t s = S[0]*S[2] - S[1]*S[1];
      if ( s < 1.E-20 ) return;
      s = 1./s;
      Double_t S0 = S[0];
      S[0] =  s*S[2];
      S[1] = -s*S[1];
      S[2] =  s*S0;
    }

    //* Kalman gain K = CH'*S

    Double_t K[5][2];
    
    if( iter<MaxIter-1 ){

      for( Int_t i=2; i<4; ++i ){
	K[i][0] = CHt[i][0]*S[0] + CHt[i][1]*S[1] ;
	K[i][1] = CHt[i][0]*S[1] + CHt[i][1]*S[2] ; 
      }
    
      //* New estimation of the track slopes

      a = T[2] + K[2][0]*zeta[0] + K[2][1]*zeta[1];
      b = T[3] + K[3][0]*zeta[0] + K[3][1]*zeta[1];

      continue;
    }

    for( Int_t i=0; i<5; ++i ){
      K[i][0] = CHt[i][0]*S[0] + CHt[i][1]*S[1] ;
      K[i][1] = CHt[i][0]*S[1] + CHt[i][1]*S[2] ; 
    }
    
    //* New estimation of the track parameters r += K*zeta

    T[0] = x;
    T[1] = y;
    T[2]+= K[2][0]*zeta[0] + K[2][1]*zeta[1];
    T[3]+= K[3][0]*zeta[0] + K[3][1]*zeta[1];
    T[4]+= K[4][0]*zeta[0] + K[4][1]*zeta[1];
    T[5] = z;

    GetRefNDF()  += 2;
    GetRefChi2() +=   zeta[0]*zeta[0]*S[0] + 2*zeta[0]*zeta[1]*S[1] 
                    + zeta[1]*zeta[1]*S[2] ;

    //* New covariance matrix C -= K*(CH')'
      
    C[ 0] -= K[0][0]*CHt[0][0] + K[0][1]*CHt[0][1];
    C[ 1] -= K[1][0]*CHt[0][0] + K[1][1]*CHt[0][1];
    C[ 2] -= K[1][0]*CHt[1][0] + K[1][1]*CHt[1][1];
    C[ 3] -= K[2][0]*CHt[0][0] + K[2][1]*CHt[0][1];
    C[ 4] -= K[2][0]*CHt[1][0] + K[2][1]*CHt[1][1];
    C[ 5] -= K[2][0]*CHt[2][0] + K[2][1]*CHt[2][1];	
    C[ 6] -= K[3][0]*CHt[0][0] + K[3][1]*CHt[0][1];	
    C[ 7] -= K[3][0]*CHt[1][0] + K[3][1]*CHt[1][1];	
    C[ 8] -= K[3][0]*CHt[2][0] + K[3][1]*CHt[2][1];	
    C[ 9] -= K[3][0]*CHt[3][0] + K[3][1]*CHt[3][1];	
    C[10] -= K[4][0]*CHt[0][0] + K[4][1]*CHt[0][1];	
    C[11] -= K[4][0]*CHt[1][0] + K[4][1]*CHt[1][1];	
    C[12] -= K[4][0]*CHt[2][0] + K[4][1]*CHt[2][1];	
    C[13] -= K[4][0]*CHt[3][0] + K[4][1]*CHt[3][1];	
    C[14] -= K[4][0]*CHt[4][0] + K[4][1]*CHt[4][1]; 
  }
}

Int_t CbmKFTrackInterface::Propagate( Double_t z_out, Double_t QP0 ){
  return CbmKF::Instance()->Propagate( GetTrack(), GetCovMatrix(), z_out, QP0 );
}

Int_t CbmKFTrackInterface::Propagate( Double_t z_out ){
  return Propagate( z_out, GetTrack()[4] );
}
