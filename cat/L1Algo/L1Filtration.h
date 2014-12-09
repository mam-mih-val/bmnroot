#ifndef L1Filtration_h
#define L1Filtration_h

#include "CbmL1Def.h"
#include "L1UMeasurementInfo.h"
#include "L1XYMeasurementInfo.h"
#include "L1TrackPar.h"

//#define cnst static const fvec
#define cnst const fvec

//inline void L1Filter( L1TrackPar &T, L1UMeasurementInfo &info, fvec u, fvec w = 1.)
inline void L1Filter( L1TrackPar &T, L1UMeasurementInfo &info, fvec u, fvec w = 1., fvec *du2 = 0) //AZ
{
  fvec wi, zeta, zetawi, HCH;
  fvec F0, F1, F2, F3, F4;
  fvec K1, K2, K3, K4;

  zeta = info.cos_phi*T.x + info.sin_phi*T.y - u;

  // F = CH'
  F0 = info.cos_phi*T.C00 + info.sin_phi*T.C10;
  F1 = info.cos_phi*T.C10 + info.sin_phi*T.C11;

  HCH = ( F0*info.cos_phi + F1*info.sin_phi );

  F2 = info.cos_phi*T.C20 + info.sin_phi*T.C21;
  F3 = info.cos_phi*T.C30 + info.sin_phi*T.C31;
  F4 = info.cos_phi*T.C40 + info.sin_phi*T.C41;

#if 0 // use mask
  const fvec mask = (HCH < info.sigma2 * 16.);
  wi = w/( (mask & info.sigma2) +HCH );
  zetawi = zeta *wi;
  T.chi2 +=  mask & (zeta * zetawi);
#else
  if (du2) wi = w / (*du2 + HCH); //AZ
  else //AZ
    wi = w/( info.sigma2 + HCH );
  //cout << wi << " " << *du2 << " " << info.sigma2 << endl; //AZ
  zetawi = zeta *wi;
  T.chi2 +=  zeta * zetawi;
#endif // 0
  T.NDF += w;

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

inline void L1FilterChi2( const L1UMeasurementInfo &info, const fvec& x, const fvec& y, const fvec& C00, 
			  //const fvec& C10, const fvec& C11, fvec& chi2, const fvec& u )
			  const fvec& C10, const fvec& C11, fvec& chi2, const fvec& u, const fvec* du2 = 0 ) //AZ
{
  fvec zeta, HCH;
  fvec F0, F1;

  zeta = info.cos_phi*x + info.sin_phi*y - u;

  // F = CH'
  F0 = info.cos_phi*C00 + info.sin_phi*C10;
  F1 = info.cos_phi*C10 + info.sin_phi*C11;

  HCH = ( F0*info.cos_phi + F1*info.sin_phi );

  if (du2) chi2 +=  zeta * zeta / (*du2 + HCH); //AZ
  else //AZ
    chi2 +=  zeta * zeta / (info.sigma2 + HCH) ;
}

inline void L1FilterChi2XYC00C10C11( const L1UMeasurementInfo &info, fvec& x, fvec& y, fvec& C00, fvec& C10, 
				     //fvec& C11, fvec& chi2, const fvec& u )
				     fvec& C11, fvec& chi2, const fvec& u, const fvec* du2 = 0 ) //AZ
{
  fvec wi, zeta, zetawi, HCH;
  fvec F0, F1;
  fvec K1;

  zeta = info.cos_phi*x + info.sin_phi*y - u;

  // F = CH'
  F0 = info.cos_phi*C00 + info.sin_phi*C10;
  F1 = info.cos_phi*C10 + info.sin_phi*C11;

  HCH = ( F0*info.cos_phi + F1*info.sin_phi );

  if (du2) wi = 1./(*du2 + HCH); //AZ
  else //AZ
    wi = 1./(info.sigma2 + HCH);
  zetawi = zeta *wi;
  chi2 +=  zeta * zetawi ;

  K1 = F1*wi;

  x  -= F0*zetawi;
  y  -= F1*zetawi;

  C00-= F0*F0*wi;
  C10-= K1*F0;
  C11-= K1*F1;
}

inline void L1FilterVtx( L1TrackPar &T, fvec x, fvec y, L1XYMeasurementInfo &info, 
			 fvec extrDx, fvec extrDy, fvec J[] )
{
  cnst TWO = 2.;

  fvec zeta0, zeta1, S00, S10, S11, si;
  fvec F00, F10, F20, F30, F40,  F01, F11, F21, F31, F41 ;
  fvec K00, K10, K20, K30, K40, K01, K11, K21, K31, K41;

  //zeta0 = T.x + J[0]*T.tx + J[1]*T.ty + J[2]*T.qp - x;
  //zeta1 = T.y + J[3]*T.tx + J[4]*T.ty + J[5]*T.qp - y;

  zeta0 = T.x + extrDx - x;
  zeta1 = T.y + extrDy - y;

    // H = 1 0 J[0] J[1] J[2]
    //     0 1 J[3] J[4] J[5]
  
  // F = CH'
  F00 = T.C00;       F01 = T.C10;    
  F10 = T.C10;       F11 = T.C11;
  F20 = J[0]*T.C22;  F21 = J[3]*T.C22;
  F30 = J[1]*T.C33;  F31 = J[4]*T.C33;
  F40 = J[2]*T.C44;  F41 = J[5]*T.C44;

  S00 = info.C00 + F00 + J[0]*F20 + J[1]*F30 + J[2]*F40;
  S10 = info.C10 + F10 + J[3]*F20 + J[4]*F30 + J[5]*F40;
  S11 = info.C11 + F11 + J[3]*F21 + J[4]*F31 + J[5]*F41;

  si = 1./(S00*S11 - S10*S10);
  //si = fvec(rcp(fvec((S00*S11 - S10*S10)[0])));
  fvec S00tmp = S00;
  S00 =  si*S11;
  S10 = -si*S10;
  S11 =  si*S00tmp;

  T.chi2+=  zeta0*zeta0*S00 + 2.*zeta0*zeta1*S10 + zeta1*zeta1*S11;
  T.NDF += TWO;

  K00 = F00*S00 + F01*S10;  K01 = F00*S10 + F01*S11;
  K10 = F10*S00 + F11*S10;  K11 = F10*S10 + F11*S11;
  K20 = F20*S00 + F21*S10;  K21 = F20*S10 + F21*S11;
  K30 = F30*S00 + F31*S10;  K31 = F30*S10 + F31*S11;
  K40 = F40*S00 + F41*S10;  K41 = F40*S10 + F41*S11;

  T.x  -= K00*zeta0 + K01*zeta1;
  T.y  -= K10*zeta0 + K11*zeta1;
  T.tx -= K20*zeta0 + K21*zeta1;
  T.ty -= K30*zeta0 + K31*zeta1;
  T.qp -= K40*zeta0 + K41*zeta1;

  T.C00-=  ( K00*F00 + K01*F01 );
  T.C10-=  ( K10*F00 + K11*F01 );
  T.C11-=  ( K10*F10 + K11*F11 );
  T.C20 = -( K20*F00 + K21*F01 );
  T.C21 = -( K20*F10 + K21*F11 );
  T.C22-=  ( K20*F20 + K21*F21 );
  T.C30 = -( K30*F00 + K31*F01 );
  T.C31 = -( K30*F10 + K31*F11 );
  T.C32 = -( K30*F20 + K31*F21 );
  T.C33-=  ( K30*F30 + K31*F31 );
  T.C40 = -( K40*F00 + K41*F01 );
  T.C41 = -( K40*F10 + K41*F11 );
  T.C42 = -( K40*F20 + K41*F21 );
  T.C43 = -( K40*F30 + K41*F31 );
  T.C44-=  ( K40*F40 + K41*F41 );
}


inline void L1FilterXY( L1TrackPar &T, fvec x, fvec y, L1XYMeasurementInfo &info )
{
  cnst TWO = 2.;

  fvec zeta0, zeta1, S00, S10, S11, si;
  fvec F00, F10, F20, F30, F40,  F01, F11, F21, F31, F41 ;
  fvec K00, K10, K20, K30, K40, K01, K11, K21, K31, K41;

  zeta0 = T.x - x;
  zeta1 = T.y - y;

  // F = CH'
  F00 = T.C00;
  F10 = T.C10;
  F20 = T.C20;
  F30 = T.C30;
  F40 = T.C40;
  F01 = T.C10;
  F11 = T.C11;
  F21 = T.C21;
  F31 = T.C31;
  F41 = T.C41;

  S00 = F00 + info.C00;
  S10 = F10 + info.C10;
  S11 = F11 + info.C11;

  si = 1./(S00*S11 - S10*S10);
  fvec S00tmp = S00;
  S00 =  si*S11;
  S10 = -si*S10;
  S11 =  si*S00tmp;

  T.chi2+=  zeta0*zeta0*S00 + 2.*zeta0*zeta1*S10 + zeta1*zeta1*S11;
  T.NDF  += TWO;

  K00 = F00*S00 + F01*S10;  K01 = F00*S10 + F01*S11;
  K10 = F10*S00 + F11*S10;  K11 = F10*S10 + F11*S11;
  K20 = F20*S00 + F21*S10;  K21 = F20*S10 + F21*S11;
  K30 = F30*S00 + F31*S10;  K31 = F30*S10 + F31*S11;
  K40 = F40*S00 + F41*S10;  K41 = F40*S10 + F41*S11;

  T.x  -= K00*zeta0 + K01*zeta1;
  T.y  -= K10*zeta0 + K11*zeta1;
  T.tx -= K20*zeta0 + K21*zeta1;
  T.ty -= K30*zeta0 + K31*zeta1;
  T.qp -= K40*zeta0 + K41*zeta1;

  T.C00-= K00*F00 + K01*F01;
  T.C10-= K10*F00 + K11*F01;
  T.C11-= K10*F10 + K11*F11;
  T.C20-= K20*F00 + K21*F01;
  T.C21-= K20*F10 + K21*F11;
  T.C22-= K20*F20 + K21*F21;
  T.C30-= K30*F00 + K31*F01;
  T.C31-= K30*F10 + K31*F11;
  T.C32-= K30*F20 + K31*F21;
  T.C33-= K30*F30 + K31*F31;
  T.C40-= K40*F00 + K41*F01;
  T.C41-= K40*F10 + K41*F11;
  T.C42-= K40*F20 + K41*F21;
  T.C43-= K40*F30 + K41*F31;
  T.C44-= K40*F40 + K41*F41;
}



/*
inline void L1Filter1D( L1TrackPar &T, fvec &u, fvec &sigma2, fvec H[] )
{

  cnst ZERO = 0.0, ONE = 1.;

  fvec wi, zeta, zetawi, HCH;
  fvec F0, F1, F2, F3, F4;
  fvec K1, K2, K3, K4;

  zeta = H[0]*T.x +H[1]*T.y +H[2]*T.tx +H[3]*T.ty +H[4]*T.qp - u;

  // F = CH'
  F0 = H[0]*T.C00 + H[1]*T.C10 + H[2]*T.C20+ H[3]*T.C30+ H[4]*T.C40;
  F1 = H[0]*T.C10 + H[1]*T.C11 + H[2]*T.C21+ H[3]*T.C31+ H[4]*T.C41;
  F2 = H[0]*T.C20 + H[1]*T.C21 + H[2]*T.C22+ H[3]*T.C32+ H[4]*T.C42;
  F3 = H[0]*T.C30 + H[1]*T.C31 + H[2]*T.C32+ H[3]*T.C33+ H[4]*T.C43;
  F4 = H[0]*T.C40 + H[1]*T.C41 + H[2]*T.C42+ H[3]*T.C43+ H[4]*T.C44;

  HCH = H[0]*F0 +H[1]*F1 +H[2]*F2 +H[3]*F3 +H[4]*F4 ;

  wi = rcp(fvec(sigma2 +HCH));
  zetawi =  zeta * wi;
  T.chi2 +=  zeta * zetawi;
  T.NDF  += ONE;

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


inline void L1Filter2D( L1TrackPar &T, fvec &x, fvec &y, L1XYMeasurementInfo &info, fvec H[] )
{
  cnst TWO = 2.;

  fvec zeta0, zeta1, S00, S10, S11, si;
  fvec F00, F10, F20, F30, F40,  F01, F11, F21, F31, F41 ;
  fvec K00, K10, K20, K30, K40, K01, K11, K21, K31, K41;

  zeta0 = H[0]*T.x +H[1]*T.y +H[2]*T.tx +H[3]*T.ty +H[4]*T.qp - x;
  zeta1 = H[5]*T.x +H[6]*T.y +H[7]*T.tx +H[8]*T.ty +H[9]*T.qp - y;

  // F = CH'
  F00 = H[0]*T.C00 + H[1]*T.C10 + H[2]*T.C20+ H[3]*T.C30+ H[4]*T.C40;
  F10 = H[0]*T.C10 + H[1]*T.C11 + H[2]*T.C21+ H[3]*T.C31+ H[4]*T.C41;
  F20 = H[0]*T.C20 + H[1]*T.C21 + H[2]*T.C22+ H[3]*T.C32+ H[4]*T.C42;
  F30 = H[0]*T.C30 + H[1]*T.C31 + H[2]*T.C32+ H[3]*T.C33+ H[4]*T.C43;
  F40 = H[0]*T.C40 + H[1]*T.C41 + H[2]*T.C42+ H[3]*T.C43+ H[4]*T.C44;
  F01 = H[5]*T.C00 + H[6]*T.C10 + H[7]*T.C20+ H[8]*T.C30+ H[9]*T.C40;
  F11 = H[5]*T.C10 + H[6]*T.C11 + H[7]*T.C21+ H[8]*T.C31+ H[9]*T.C41;
  F21 = H[5]*T.C20 + H[6]*T.C21 + H[7]*T.C22+ H[8]*T.C32+ H[9]*T.C42;
  F31 = H[5]*T.C30 + H[6]*T.C31 + H[7]*T.C32+ H[8]*T.C33+ H[9]*T.C43;
  F41 = H[5]*T.C40 + H[6]*T.C41 + H[7]*T.C42+ H[8]*T.C43+ H[9]*T.C44;

  S00 = H[0]*F00 +H[1]*F10 +H[2]*F20 +H[3]*F30 +H[4]*F40 + info.C00;
  S10 = H[5]*F00 +H[6]*F10 +H[7]*F20 +H[8]*F30 +H[9]*F40 + info.C10;
  S11 = H[5]*F01 +H[6]*F11 +H[7]*F21 +H[8]*F31 +H[9]*F41 + info.C11;

  si = rcp(fvec(S00*S11 - S10*S10));
  fvec S00tmp = S00;
  S00 =  si*S11;
  S10 = -si*S10;
  S11 =  si*S00tmp;

  T.chi2+=  zeta0*zeta0*S00 + 2.*zeta0*zeta1*S10 + zeta1*zeta1*S11;
  T.NDF  += TWO;

  K00 = F00*S00 + F01*S10;  K01 = F00*S10 + F01*S11;
  K10 = F10*S00 + F11*S10;  K11 = F10*S10 + F11*S11;
  K20 = F20*S00 + F21*S10;  K21 = F20*S10 + F21*S11;
  K30 = F30*S00 + F31*S10;  K31 = F30*S10 + F31*S11;
  K40 = F40*S00 + F41*S10;  K41 = F40*S10 + F41*S11;

  T.x  -= K00*zeta0 + K01*zeta1;
  T.y  -= K10*zeta0 + K11*zeta1;
  T.tx -= K20*zeta0 + K21*zeta1;
  T.ty -= K30*zeta0 + K31*zeta1;
  T.qp -= K40*zeta0 + K41*zeta1;

  T.C00-= K00*F00 + K01*F01;
  T.C10-= K10*F00 + K11*F01;
  T.C11-= K10*F10 + K11*F11;
  T.C20-= K20*F00 + K21*F01;
  T.C21-= K20*F10 + K21*F11;
  T.C22-= K20*F20 + K21*F21;
  T.C30-= K30*F00 + K31*F01;
  T.C31-= K30*F10 + K31*F11;
  T.C32-= K30*F20 + K31*F21;
  T.C33-= K30*F30 + K31*F31;
  T.C40-= K40*F00 + K41*F01;
  T.C41-= K40*F10 + K41*F11;
  T.C42-= K40*F20 + K41*F21;
  T.C43-= K40*F30 + K41*F31;
  T.C44-= K40*F40 + K41*F41;
}




inline void L1FilterFirst( fvec *T, L1Cov &C, fvec &Chi2, fvec &NDF, L1XYMeasurementInfo &info, fvec &x, fvec &y, fvec &w )
{
  cnst ZERO = 0.0, ONE = 1.;

  fvec w1 = !w;
  C.C00= w&info.C00 + w1&INF;
  C.C10= w&info.C10 + w1&INF;  C.C11= w&info.C11 + w1&INF;
  C.C20= ZERO;      C.C21= ZERO;      C.C22= INF;
  C.C30= ZERO;      C.C31= ZERO;      C.C32= ZERO; C.C33= INF;
  C.C40= ZERO;      C.C41= ZERO;      C.C42= ZERO; C.C43= ZERO; C.C44= INF;

  T[0] = w&x + w1&T[0];
  T[1] = w&y + w1&T[1];
  NDF = -3.0;
  Chi2 = ZERO;
}
*/

#undef cnst

#endif
