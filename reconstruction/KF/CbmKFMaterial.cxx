#include "CbmKFMaterial.h"

#include "CbmKFMath.h"
#include "CbmKF.h"
#include "TString.h"

#include <cmath>

using std::fabs;

ClassImp(CbmKFMaterial);

TString CbmKFMaterial::Info() const{
  char tmp[255];
  sprintf( tmp, " ( id, z, dz, RadL )= ( %i, %f, %f, %f )",
	   ID, ZReference, ZThickness, RadLength );
  return tmp;
}


Int_t CbmKFMaterial::Pass( Double_t ZCross, Double_t ZThick, 
			   CbmKFTrackInterface &track,	    
			   Bool_t downstream, Double_t &QP0        ){
  Bool_t err = 0;
  Double_t *T = track.GetTrack();
  Double_t *C = track.GetCovMatrix();  
  err = err || CbmKF::Instance()->Propagate( T, C, ZCross, QP0 );
  if( err ) return err;
  if( IsOutside( T[0], T[1] ) ) return 0;
  Double_t Q5, Q8, Q9, Ecor;  
  err = err || CbmKFMath::GetNoise( ZThick/RadLength, F, Fe, 
				    T[2], T[3], QP0, 
				    track.GetMass(), track.IsElectron(), downstream, 
				    &Q5,&Q8,&Q9,&Ecor);
  if( err ) return err;
  C[5]+=Q5;
  C[8]+=Q8;
  C[9]+=Q9;
  QP0 *= Ecor;
  T[ 4] *= Ecor;
  C[10] *= Ecor;
  C[11] *= Ecor;
  C[12] *= Ecor;
  C[13] *= Ecor;
  C[14] *= Ecor*Ecor;
  return 0;
}  

ClassImp(CbmKFWall);
ClassImp(CbmKFTube);

CbmKFTube::CbmKFTube(Int_t ID_, Double_t x_, Double_t y_, Double_t z_, Double_t dz_, Double_t r_, Double_t R_, Double_t radL_):
x(x_),
y(y_),
z(z_),
dz(dz_),
r(r_),
R(R_),
rr(r_*r_),
RR(R_*R_)
{
  ID = ID_;
  ZReference =z_;
  ZThickness = dz_;
  RadLength =radL_ ;
  F = 1;
}

TString CbmKFTube::Info() const{
  char tmp[255];
  sprintf(tmp, " tube ( id, x,y,z, dz, r, R, RadL )= ( %i, %f, %f, %f, %f, %f, %f, %f )",
	  ID, x, y, z, dz, r, R, RadLength );
  return tmp;
}


ClassImp(CbmKFBox);

CbmKFBox::CbmKFBox(Int_t ID_, Double_t x_, Double_t y_, Double_t z_, Double_t dx_, Double_t dy_, Double_t dz_, Double_t radL_):
x(x_),
y(y_),
z(z_),
dx05(dx_/2),
dy05(dy_/2),
dz(dz_)
{
  ID = ID_;
  ZReference =z_;
  ZThickness = dz_;
  RadLength =radL_ ;
  F = 1;
}

Bool_t CbmKFBox::IsOutside( Double_t X, Double_t Y ){
  return ( TMath::Abs(X-x)>dx05 || TMath::Abs(Y-y)>dy05 ) ;
}

TString CbmKFBox::Info() const{
  char tmp[255];
  sprintf(tmp, " box ( id, x,y,z, dx, dy, dz, RadL )= ( %i, %f, %f, %f, %f, %f, %f, %f )",
	  ID, x, y, z, dx05*2, dy05*2, dz, RadLength );
  return tmp;
}


ClassImp(CbmKFCone);

Int_t CbmKFCone::Pass( Double_t ZCross, Double_t ZThick, 
		       CbmKFTrackInterface &track,		    
		       Bool_t downstream, Double_t &QP0       ){

  Bool_t err = 0;
  CbmKF *KF = CbmKF::Instance();
  Double_t *T = track.GetTrack();
  Double_t *C = track.GetCovMatrix();  
  
  Double_t zthick = ZThickness, zcross=ZReference;
  Double_t T_tmp[6];
  for(Int_t j=0; j<6; j++ ) T_tmp[j] = T[j];
  err = err || KF->Propagate( T_tmp, 0, ZReference, QP0);
  if( err ) return err;
  Double_t cz1, ct1, cz2, ct2;
  {
    Double_t iz1, iz2, iZ1, iZ2;
    Bool_t err1 = CbmKFMath::intersectCone( z1, z2, r1,  r2, T_tmp, &iz1, &iz2 );
    Bool_t err2 = CbmKFMath::intersectCone( z1, z2, R1,  R2, T_tmp, &iZ1, &iZ2 );
    if ( err1 || iz1<z1 || iz1>z2 ) iz1 = -200;
    if ( err1 || iz2<z1 || iz2>z2 ) iz2 = -200;
    if ( err2 || iZ1<z1 || iZ1>z2 ) iZ1 = -200;
    if ( err2 || iZ2<z1 || iZ2>z2 ) iZ2 = -200;
    
    if ( iz1>-200 && iZ1>-200 )
      {
	cz1 = ( iz1 + iZ1 )/2;
	ct1 = fabs( iZ1 - iz1 );	
      }
    else
      {
	cz1 = ZCross; //TODO Check me
	ct1 = 0;  
      }
    
    if ( iz2>-200 && iZ2>-200 )
      {
	cz2 = ( iz2 + iZ2 )/2;
	ct2 = fabs( iZ2 - iz2 );	
      }
    else
      {
	cz2 = ZCross; //TODO Check me
	ct2 = 0;
      }
  
    if ( iz1<=-200 && iz2<=-200 && iZ1>-200 && iZ2>-200 )
      {
	cz1 = ( iZ1 + iZ2 )/2;
	ct1 = fabs( iZ2 - iZ1 );	
	cz2 = ZCross; //TODO Check me
	ct2 = 0;	
      }    
  }
  Double_t dz1 = cz1 - T[5];
  Double_t dz2 = cz2 - T[5];
  if( fabs(dz1)<fabs(dz2) ){
    zthick = ct1;
    zcross = cz1;
  }else{
    zthick = ct2;
    zcross = cz2;
  }
  
  err = err || KF->Propagate( T, C, zcross, QP0 );
  
  Double_t Q5, Q8, Q9, Ecor;  
  err = err || CbmKFMath::GetNoise( zthick/RadLength, F, Fe,
				    T[2], T[3], QP0, 
				    track.GetMass(), track.IsElectron(),downstream, 
				    &Q5,&Q8,&Q9,&Ecor);
  if( err ) return err;
  C[5]+=Q5;
  C[8]+=Q8;
  C[9]+=Q9;
  QP0 *= Ecor;
  T[ 4] *= Ecor;
  C[10] *= Ecor;
  C[11] *= Ecor;
  C[12] *= Ecor;
  C[13] *= Ecor;
  C[14] *= Ecor*Ecor;
  return 0;
}  
