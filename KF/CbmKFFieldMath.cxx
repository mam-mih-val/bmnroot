#include "CbmKFFieldMath.h"  

#include "CbmKFMath.h"  

#include "FairField.h"

#include "math.h"

//using std::sqrt;
//using std::finite;

ClassImp(CbmKFFieldMath)

void CbmKFFieldMath::ExtrapolateLine
( 
 const Double_t T_in[], const Double_t C_in[], 
 Double_t T_out[], Double_t C_out[],
 Double_t z_out
 )
{  
  if ( ! T_in ) return;
  
  Double_t dz = z_out - T_in[5];
  
  if ( T_out )
    {
      T_out[0] = T_in[0] + dz*T_in[2];
      T_out[1] = T_in[1] + dz*T_in[3];
      T_out[2] = T_in[2];
      T_out[3] = T_in[3];
      T_out[4] = T_in[4];
      T_out[5] = z_out;
    }
  
  if ( C_in && C_out )
    {
      const Double_t dzC_in8 = dz * C_in[ 8 ];  

      C_out[ 4 ] = C_in[ 4 ]  +  dzC_in8;
      C_out[ 1 ] = C_in[ 1 ]  +  dz * (  C_out[ 4 ] + C_in[ 6 ] );

      const Double_t C_in3 = C_in[ 3 ];

      C_out[ 3 ] = C_in3  +  dz * C_in[ 5 ];
      C_out[ 0 ] = C_in[ 0 ]  +  dz * ( C_out[ 3 ] + C_in3 );  

      const Double_t C_in7 = C_in[ 7 ];

      C_out[ 7 ] = C_in7  +  dz * C_in[ 9 ];
      C_out[ 2 ] = C_in[ 2 ]  +  dz * ( C_out[ 7 ] + C_in7 ); 
      C_out[ 5 ] = C_in[ 5 ];
      C_out[ 6 ] = C_in[ 6 ] + dzC_in8;
      C_out[ 8 ] = C_in[ 8 ];  
      C_out[ 9 ] = C_in[ 9 ];

      C_out[ 10] = C_in[ 10];
      C_out[ 11] = C_in[ 11];
      C_out[ 12] = C_in[ 12];
      C_out[ 13] = C_in[ 13];
      C_out[ 14] = C_in[ 14];
    }		     
}


Int_t CbmKFFieldMath::ExtrapolateRK4
( 
 const Double_t T_in [], // input track parameters (x,y,tx,ty,Q/p)
 const Double_t C_in [], // input covariance matrix
 Double_t       T_out[], // output track parameters
 Double_t       C_out[], // output covariance matrix
 Double_t       z_out  , // extrapolate to this z position
 Double_t       qp0    , // use Q/p linearisation at this value
 FairField       *MF       // magnetic field
 )
{
  //
  // Forth-order Runge-Kutta method for solution of the equation
  // of motion of a particle with parameter qp = Q /P
  //              in the magnetic field B()
  //
  //        | x |          tx
  //        | y |          ty
  // d/dz = | tx| = ft * ( ty * ( B(3)+tx*b(1) ) - (1+tx**2)*B(2) )
  //        | ty|   ft * (-tx * ( B(3)+ty+b(2)   - (1+ty**2)*B(1) )  ,
  //
  //   where  ft = c_light*qp*sqrt ( 1 + tx**2 + ty**2 ) .
  //
  //  In the following for RK step  :
  //
  //     x=x[0], y=x[1], tx=x[3], ty=x[4].
  //
  //========================================================================
  //
  //  NIM A395 (1997) 169-184; NIM A426 (1999) 268-282.
  //
  //  the routine is based on LHC(b) utility code
  //
  //========================================================================
  
  const Double_t c_light = 0.000299792458;
  
  static Double_t a[4] = {0.0, 0.5, 0.5, 1.0};
  static Double_t c[4] = {1.0/6.0, 1.0/3.0, 1.0/3.0, 1.0/6.0};
  static Double_t b[4] = {0.0, 0.5, 0.5, 1.0};
  
  Int_t step4;
  Double_t k[16],x0[4],x[4],k1[16];
  Double_t Ax[4],Ay[4],Ax_tx[4],Ay_tx[4],Ax_ty[4],Ay_ty[4];

  //----------------------------------------------------------------

  Double_t qp_in = T_in[4];
  Double_t z_in  = T_in[5];
  Double_t h     = z_out - z_in;
  Double_t hC    = h * c_light;
  x0[0] = T_in[0]; x0[1] = T_in[1];
  x0[2] = T_in[2]; x0[3] = T_in[3];
  //
  //   Runge-Kutta step
  //

  Int_t step;
  Int_t i;
  
  for (step = 0; step < 4; ++step) {
    for(i=0; i < 4; ++i) {
      if(step == 0) {
        x[i] = x0[i];
      } else {
        x[i] = x0[i] + b[step] * k[step*4-4+i];
      }
    }

    Double_t point[3] = { x[0], x[1], z_in  + a[step] * h };
    Double_t B[3];
    if ( MF ) MF->GetFieldValue( point, B );
    else { B[0] = B[1] = B[2] = 0.; }

    Double_t tx = x[2]; 
    Double_t ty = x[3]; 
    Double_t tx2 = tx * tx; 
    Double_t ty2 = ty * ty; 
    Double_t txty = tx * ty;
    Double_t tx2ty21= 1.0 + tx2 + ty2; 
    if( tx2ty21 > 1.e4 ) return 1;
    Double_t I_tx2ty21 = 1.0 / tx2ty21 * qp0;
    Double_t tx2ty2 = sqrt(tx2ty21 ) ; 
    //   Double_t I_tx2ty2 = qp0 * hC / tx2ty2 ; unsused ???
    tx2ty2 *= hC; 
    Double_t tx2ty2qp = tx2ty2 * qp0;
    Ax[step] = ( txty*B[0] + ty*B[2] - ( 1.0 + tx2 )*B[1] ) * tx2ty2;
    Ay[step] = (-txty*B[1] - tx*B[2] + ( 1.0 + ty2 )*B[0] ) * tx2ty2;

    Ax_tx[step] = Ax[step]*tx*I_tx2ty21 + (ty*B[0]-2.0*tx*B[1])*tx2ty2qp;
    Ax_ty[step] = Ax[step]*ty*I_tx2ty21 + (tx*B[0]+B[2])*tx2ty2qp;
    Ay_tx[step] = Ay[step]*tx*I_tx2ty21 + (-ty*B[1]-B[2])*tx2ty2qp;
    Ay_ty[step] = Ay[step]*ty*I_tx2ty21 + (-tx*B[1]+2.0*ty*B[0])*tx2ty2qp;

    step4 = step * 4;
    k[step4  ] = tx * h;
    k[step4+1] = ty * h;
    k[step4+2] = Ax[step] * qp0;
    k[step4+3] = Ay[step] * qp0;

  }  // end of Runge-Kutta steps

  for(i=0; i < 4; ++i) {
    T_out[i]=x0[i]+c[0]*k[i]+c[1]*k[4+i]+c[2]*k[8+i]+c[3]*k[12+i];
  }
  T_out[4]=T_in[4];
  T_out[5]=z_out;
  //
  //     Derivatives    dx/dqp
  //

  x0[0] = 0.0; x0[1] = 0.0; x0[2] = 0.0; x0[3] = 0.0;

  //
  //   Runge-Kutta step for derivatives dx/dqp

  for (step = 0; step < 4; ++step) {
    for(i=0; i < 4; ++i) {
      if(step == 0) {
        x[i] = x0[i];
      } else {
        x[i] = x0[i] + b[step] * k1[step*4-4+i];
      }
    }
    step4 = step * 4;
    k1[step4  ] = x[2] * h;
    k1[step4+1] = x[3] * h;
    k1[step4+2] = Ax[step] + Ax_tx[step] * x[2] + Ax_ty[step] * x[3];
    k1[step4+3] = Ay[step] + Ay_tx[step] * x[2] + Ay_ty[step] * x[3];

  }  // end of Runge-Kutta steps for derivatives dx/dqp

  Double_t J[25];

  for (i = 0; i < 4; ++i ) {
    J[20+i]=x0[i]+c[0]*k1[i]+c[1]*k1[4+i]+c[2]*k1[8+i]+c[3]*k1[12+i];
  }
  J[24] = 1.;
  //
  //      end of derivatives dx/dqp
  //

  //     Derivatives    dx/tx
  //

  x0[0] = 0.0; x0[1] = 0.0; x0[2] = 1.0; x0[3] = 0.0;

  //
  //   Runge-Kutta step for derivatives dx/dtx
  //

  for (step = 0; step < 4; ++step) {
    for(i=0; i < 4; ++i) {
      if(step == 0) {
        x[i] = x0[i];
      } else if ( i!=2 ){
        x[i] = x0[i] + b[step] * k1[step*4-4+i];
      }
    }
    step4 = step * 4;
    k1[step4  ] = x[2] * h;
    k1[step4+1] = x[3] * h;
    //    k1[step4+2] = Ax_tx[step] * x[2] + Ax_ty[step] * x[3];
    k1[step4+3] = Ay_tx[step] * x[2] + Ay_ty[step] * x[3];

  }  // end of Runge-Kutta steps for derivatives dx/dtx

  for (i = 0; i < 4; ++i ) {
    if(i != 2) {
      J[10+i]=x0[i]+c[0]*k1[i]+c[1]*k1[4+i]+c[2]*k1[8+i]+c[3]*k1[12+i];
    }
  }
  //      end of derivatives dx/dtx
  J[12] = 1.0;
  J[14] = 0.0;

  //     Derivatives    dx/ty
  //

  x0[0] = 0.0; x0[1] = 0.0; x0[2] = 0.0; x0[3] = 1.0;

  //
  //   Runge-Kutta step for derivatives dx/dty
  //

  for (step = 0; step < 4; ++step) {
    for(i=0; i < 4; ++i) {
      if(step == 0) {
        x[i] = x0[i];           // ty fixed
      } else if(i!=3) {
        x[i] = x0[i] + b[step] * k1[step*4-4+i];
      }

    }
    step4 = step * 4;
    k1[step4  ] = x[2] * h;
    k1[step4+1] = x[3] * h;
    k1[step4+2] = Ax_tx[step] * x[2] + Ax_ty[step] * x[3];
    //    k1[step4+3] = Ay_tx[step] * x[2] + Ay_ty[step] * x[3];

  }  // end of Runge-Kutta steps for derivatives dx/dty

  for (i = 0; i < 3; ++i ) {
    J[15+i]=x0[i]+c[0]*k1[i]+c[1]*k1[4+i]+c[2]*k1[8+i]+c[3]*k1[12+i];
  }
  //      end of derivatives dx/dty
  J[18] = 1.;
  J[19] = 0.;

  //
  //    derivatives dx/dx and dx/dy

  for(i = 0; i < 10; ++i){ J[i] = 0.;}
  J[0] = 1.; J[6] = 1.;

  // extrapolate inverse momentum

  T_out[4] = qp_in;
  
  Double_t dqp = qp_in - qp0;
  
  { for( Int_t ip=0; ip<4; ip++ )
    {
      T_out[ip]+=J[5*4+ip]*dqp;
    }
  }
   
  //          covariance matrix transport 
  
  if ( C_in&&C_out ) CbmKFMath::multQtSQ( 5, J, C_in, C_out);
  return 0;
}    // end of RK4

#ifdef XXX
/****************************************************************
 *
 *  Field Integration 
 *
 ****************************************************************/

void CbmKFFieldMath::IntegrateField
( 
 CbmMagField *MF,
 Double_t r0[], Double_t r1[], Double_t r2[],
 Double_t si  [3]      , Double_t Si  [3],
 Double_t sii [3][3]   , Double_t Sii [3][3], 
 Double_t siii[3][3][3], Double_t Siii[3][3][3]
 )
{
  Double_t dz = r2[2] - r0[2];
  
  Double_t B[3][3];
   
  if ( MF )
    {
      MF->GetFieldValue( r0, B[0] );
      MF->GetFieldValue( r1, B[1] );
      MF->GetFieldValue( r2, B[2] );
    }
  else B[0][0]=B[0][1]=B[0][2]=B[1][0]=B[1][1]=B[1][2]=B[2][0]=B[2][1]=B[2][2]=0;

  // coefficients

  Double_t 
    c1[3]       =     {  1,  4,  1} // /=6.
  , c2[3][3]    =   { {  5, -4, -1},{  44,  80,  -4},{ 11, 44, 5} } // /=360.
  , c3[3][3][3] = { { { 35, 20, -1},{-124,-256,  20},{-19,-52,-1} }, 
		    { {524,176,-52},{1760,2240,-256},{-52,176,20} }, 
		    { {125,-52,-19},{1028,1760,-124},{125,524,35} } };  // /=45360.
    
  Double_t 
    C1[3]       =     {  1,  2,  0} // /=6.
  , C2[3][3]    =   { { 38,  8, -4},{ 148, 208, -20},{  3, 36, 3} } // /=2520.
  , C3[3][3][3] = { { { 85, 28, -5},{   4, -80,   4},{-17,-20, 1} }, 
		    { {494,200,-46},{1256,1376,-184},{-94,  8,14} }, 
		    { { 15,-12, -3},{ 252, 432, -36},{ 21, 84, 3} } }; // /=90720.
    
  // integrate field
  
  for( Int_t x=0; x<3; x++)
    {
      if( si )
	{
	  si[x]=0;
	  for( Int_t n=0; n<3; n++ ) si[x] += c1[n]*B[n][x];	  
	  si[x] *= dz/6.;
	}
      
      if( Si )
	{
	  Si[x]=0;
	  for(Int_t n=0; n<3; n++ ) Si[x] += C1[n]*B[n][x];
	  Si[x] *= dz*dz/6.;
	}

      for( Int_t y=0; y<3; y++ )
	{
	  if( sii )
	    {
	      sii[x][y] = 0;
	      for(Int_t n=0; n<3; n++)
		for(Int_t m=0; m<3; m++) sii[x][y] += c2[n][m]*B[n][x]*B[m][y];
	      sii[x][y] *= dz*dz/360.;
	    }

	  if( Sii )
	    {
	      Sii[x][y] = 0;
	      for(Int_t n=0; n<3; n++)
		for(Int_t m=0; m<3; m++) Sii[x][y] += C2[n][m]*B[n][x]*B[m][y];	       
	      Sii[x][y] *= dz*dz*dz/2520.;
	    }

	  for( Int_t z=0; z<3; z++ )
	    {
	      if ( siii )
		{
		  siii[x][y][z] = 0;
		  for(Int_t n=0; n<3; n++)
		    for(Int_t m=0; m<3; m++)
		      for(Int_t k=0; k<3; k++)
			siii[x][y][z] += c3[n][m][k]*B[n][x]*B[m][y]*B[k][z];
		  siii[x][y][z] *= dz*dz*dz/45360.;
		}

	      if ( Siii )
		{
		  Siii[x][y][z] = 0;
		  for(Int_t n=0; n<3; n++)
		    for(Int_t m=0; m<3; m++)
		      for(Int_t k=0; k<3; k++)
			Siii[x][y][z] += C3[n][m][k]*B[n][x]*B[m][y]*B[k][z];	       
		  Siii[x][y][z] *= dz*dz*dz*dz/90720.;
		}
	    }
	}
    }
}
  


/****************************************************************
 *
 *  Calculate extrapolation coefficients
 *
 ****************************************************************/

void CbmKFFieldMath::GetCoefficients
( 
 const Double_t x, const Double_t y,
 Double_t Xi  [3][3]      , Double_t Yi  [3][3],
 Double_t Xii [3][3][3]   , Double_t Yii [3][3][3], 
 Double_t Xiii[3][3][3][3], Double_t Yiii[3][3][3][3]
 )
{
  Double_t 

    xx = x*x,
    xy = x*y,
    yy = y*y,

    x2 = x*2,
    x4 = x*4,
    xx31 = xx*3+1,
    xx33 = xx*3+3,
    xx82 = xx*8+2,
    xx86 = xx*8+6,
    xx153 = xx*15+3,
    xx159 = xx*15+9,

    y2 = y*2,
    y4 = y*4,
    yy31 = yy*3+1,
    yy33 = yy*3+3,
    yy82 = yy*8+2,
    yy86 = yy*8+6,
    yy153 = yy*15+3,
    yy159 = yy*15+9,
    xxyy2 = 2*(xx-yy),

    xx1053 = y*(30*xx+xx159),
    yy1053 = x*(30*yy+yy159);


  Double_t 
    X1[3]       =     {              xy,               -xx-1,        y }
  
  , X2[3][3]    =   { {          x*yy31,             -y*xx31,   2*yy+1 },
		      {         -y*xx31,              x*xx33,    -2*xy },
		      {           yy-xx,               -2*xy,       -x } }
  
  , X3[3][3][3] = { { {        xy*yy159,      -xx*yy153-yy31,   y*yy86 },
		      {  -xx*yy153-yy31,            xy*xx159,  -x*yy82 },
		      {   y2*(-xxyy2+1),             -x*yy82,    -3*xy } },
		    { {  -xx*yy153-yy31,            xy*xx159,  -x*yy82 },
		      {        xy*xx159, -3*(5*xx*xx+6*xx+1),   y*xx82 },
		      {    x2*(xxyy2+1),              y*xx82,     xx31 } },
		    { {  y*(-6*xx+yy31),       x*(xx31-6*yy),   -4*xy  },
		      { x*(3*xx-6*yy+1),            3*y*xx31,   xxyy2  },
		      {           -4*xy,               xxyy2,      -y  } 
		    }};
  Double_t 
    X1x[3]       =     { y, -x2, 0 }

  , X2x[3][3]    =   { {  yy31,  -6*xy,   0 },
		       { -6*xy, 3*xx31, -y2 },
		       {   -x2,    -y2,  -1 } }

  , X3x[3][3][3] = { { {         y*yy159,   -x2*yy153,     0 },
		       {       -x2*yy153,      xx1053, -yy82 },
		       {           -8*xy,       -yy82,  -3*y } },
		     { {       -x2*yy153,      xx1053, -yy82 },
		       {          xx1053,   -x4*xx159, 16*xy },
		       { 2*(6*xx-2*yy+1),       16*xy,   6*x } },
		     { {          -12*xy, 9*xx-6*yy+1,   -y4 },
		       {     9*xx-6*yy+1,       18*xy,    x4 },
		       {             -y4,          x4,     0 } 
		     }};
  
  Double_t 
    X1y[3]       =     { x, 0, 1 }
  
  , X2y[3][3]    =   { {  6*xy, -xx31,  y4 },
		       { -xx31,     0, -x2 },
		       {    y2,   -x2,   0 } }

  , X3y[3][3][3] = { { {       yy1053,  -y2*xx153, 16*yy+yy86 },
		       {    -y2*xx153,    x*xx159,     -16*xy },
		       { yy82-2*xxyy2,     -16*xy,       -3*x } },
		     { {    -y2*xx153,    x*xx159,     -16*xy },
		       {      x*xx159,          0,       xx82 },
		       {        -8*xy,       xx82,          0 } },
		     { { -6*xx+9*yy+1,     -12*xy,        -x4 },
		       {       -12*xy,     3*xx31,        -y4 },
		       {          -x4,        -y4,         -1 } 
		     }};


  Double_t 
   Y1[3]       =     { yy+1, -xy, -x }

 , Y2[3][3]    =   { {  y*yy33, -x*yy31,  -2*xy },
		     { -x*yy31,  y*xx31, 2*xx+1 },
		     {   -2*xy,   xx-yy,     -y } }

 , Y3[3][3][3] = { { { 3*(5*yy*yy+6*yy+1),      -xy*yy159, -x*yy82 },
		     {          -xy*yy159,  xx*yy153+yy31,  y*xx82 },
		     {            -x*yy82, -y2*(-xxyy2+1),   -yy31 } },
		   { {          -xy*yy159,  xx*yy153+yy31,  y*xx82 },
		     {      xx*yy153+yy31,      -xy*xx159, -x*xx86 },
		     {             y*xx82,  -x2*(xxyy2+1),    3*xy } },
		   { {          -3*x*yy31,  y*(6*xx-yy31),   xxyy2 },
		     {      y*(6*xx-yy31),  x*(6*yy-xx31),    4*xy },
		     {              xxyy2,           4*xy,       x } 
		   }};
  Double_t 
   Y1x[3]       =     { 0, -y, -1 }
  
 , Y2x[3][3]    =   { {     0, -yy31, -y2 },
		      { -yy31,  6*xy,  x4 },
		      {   -y2,    x2,   0 } }

 , Y3x[3][3][3] = { { {        0,         -y*yy159,       -yy82 },
		      { -y*yy159,         x2*yy153,       16*xy },
		      {    -yy82,             8*xy,           0 } },
		    { { -y*yy159,         x2*yy153,       16*xy }, 
		      { x2*yy153,          -xx1053, -16*xx-xx86 },
		      {    16*xy, -2*(6*xx-2*yy+1),         3*y } },
		    { {  -3*yy31,            12*xy,          x4 },
		      {    12*xy,     -9*xx+6*yy-1,          y4 },
		      {       x4,               y4,           1 } 
		    }};
  Double_t 
   Y1y[3]       =     { y2, -x, 0 }

 , Y2y[3][3]    =   { { 3*yy31, -6*xy, -x2 },
		      {  -6*xy,  xx31,   0 },
		      {    -x2,   -y2,  -1 } }

 , Y3y[3][3][3] = { { {    y4*yy159,      -yy1053, -16*xy },
		      {     -yy1053,     y2*xx153,   xx82 },
		      {      -16*xy, 4*xx-12*yy-2,   -6*y } },
		    { {     -yy1053,     y2*xx153,   xx82 },
		      {    y2*xx153,     -x*xx159,      0 },
		      {        xx82,         8*xy,    3*x } },
		    { {      -18*xy,  6*xx-9*yy-1,    -y4 },
		      { 6*xx-9*yy-1,        12*xy,     x4 },
		      {         -y4,           x4,      0 } 
		    }};

  if( Xi )
    { 
      for( Int_t i=0;i<3; i++ )
	{
	  Xi[0][i] = X1 [i];
	  Xi[1][i] = X1x[i];
	  Xi[2][i] = X1y[i];
	}
    }
  if( Yi )
    { 
      for( Int_t i=0;i<3; i++ )
	{
	  Yi[0][i] = Y1 [i];
	  Yi[1][i] = Y1x[i];
	  Yi[2][i] = Y1y[i];
	}
    }
  if( Xii )
    { 
      for( Int_t i=0;i<3; i++ ) for( Int_t j=0;j<3; j++ )
	{
	  Xii[0][i][j] = X2 [i][j];
	  Xii[1][i][j] = X2x[i][j];
	  Xii[2][i][j] = X2y[i][j];
	}
    }
  if( Yii )
    { 
      for( Int_t i=0;i<3; i++ ) for( Int_t j=0;j<3; j++ )
	{
	  Yii[0][i][j] = Y2 [i][j];
	  Yii[1][i][j] = Y2x[i][j];
	  Yii[2][i][j] = Y2y[i][j];
	}
    }
  if( Xiii )
    { 
      for( Int_t i=0;i<3; i++ ) for( Int_t j=0;j<3; j++ )for( Int_t k=0;k<3; k++ )
	{
	  Xiii[0][i][j][k] = X3 [i][j][k];
	  Xiii[1][i][j][k] = X3x[i][j][k];
	  Xiii[2][i][j][k] = X3y[i][j][k];
	}
    }
  if( Yiii )
    { 
      for( Int_t i=0;i<3; i++ ) for( Int_t j=0;j<3; j++ )for( Int_t k=0;k<3; k++ )
	{
	  Yiii[0][i][j][k] = Y3 [i][j][k];
	  Yiii[1][i][j][k] = Y3x[i][j][k];
	  Yiii[2][i][j][k] = Y3y[i][j][k];
	}
    }
}


/****************************************************************
 *
 *  ANALYTIC 1-3
 *
 ****************************************************************/

void CbmKFFieldMath::ExtrapolateAnalytic
( 
 const Double_t T_in [], // input track parameters (x,y,tx,ty,Q/p)
 const Double_t C_in [], // input covariance matrix
 Double_t       T_out[], // output track parameters
 Double_t       C_out[], // output covariance matrix
 Double_t       z_out  , // extrapolate to this z position
 Double_t       qp0    , // use Q/p linearisation at this value
 FairField *MF     , // magnetic field
 Int_t order
 )
{
  //
  //  Part of the exact extrapolation formula with error (c_light*B*dz)^4/4!
  //  
  //  Under investigation, finally supposed to be fast.
  //  
  
  const Double_t c_light = 0.000299792458;

  Double_t qp_in = T_in[4];
  Double_t z_in = T_in[5];
  Double_t dz = z_out - z_in;

  // construct coefficients 

  Double_t 
    tx   = T_in[2], 
    ty   = T_in[3], 
    
    A1[3][3] ,      B1[3][3],
    A2[3][3][3]   , B2[3][3][3], 
    A3[3][3][3][3], B3[3][3][3][3];

  GetCoefficients( tx, ty, A1, B1, A2, B2, A3, B3 );

  Double_t 
    t2   = 1. + tx*tx + ty*ty,
    t    = sqrt( t2 ),
    h    = qp0*c_light,
    ht = h*t;
  
  Double_t s1[3], s2[3][3], s3[3][3][3], S1[3], S2[3][3], S3[3][3][3];

  {
    Double_t r0[3], r1[3],r2[3];
	    
    r0[0] = T_in[0];
    r0[1] = T_in[1];
    r0[2] = T_in[5];
    
    r2[0] = T_in[0] + T_in[2]*dz;
    r2[1] = T_in[1] + T_in[3]*dz;
    r2[2] = z_out;
    
    r1[0] = 0.5*(r0[0]+r2[0]);
    r1[1] = 0.5*(r0[1]+r2[1]);
    r1[2] = 0.5*(r0[2]+r2[2]);
    
    
    Double_t B[3][3];   
    
    if ( MF )
      {
	MF->GetFieldValue( r0, B[0] );
	MF->GetFieldValue( r1, B[1] );
	MF->GetFieldValue( r2, B[2] );
      }
    else B[0][0]=B[0][1]=B[0][2]=B[1][0]=B[1][1]=B[1][2]=B[2][0]=B[2][1]=B[2][2]=0;
    
    Double_t Sy = (7*B[0][1] + 6*B[1][1]-B[2][1] )*dz*dz/96.;
    r1[0] = T_in[0] + tx*dz/2 + ht*Sy*A1[0][1];
    r1[1] = T_in[1] + ty*dz/2 + ht*Sy*B1[0][1];
    
    Sy = (B[0][1] + 2*B[1][1] )*dz*dz/6.;
    r2[0] = T_in[0] + tx*dz + ht*Sy*A1[0][1];
    r2[1] = T_in[1] + ty*dz + ht*Sy*B1[0][1];    

    IntegrateField( MF, r0,r1,r2, s1, S1, s2, S2, s3, S3 );
  }

  Double_t sA1 =0, sA2 =0, sA3 =0, sB1 =0, sB2 =0, sB3 =0;
  Double_t sA1x=0, sA2x=0, sA3x=0, sB1x=0, sB2x=0, sB3x=0;
  Double_t sA1y=0, sA2y=0, sA3y=0, sB1y=0, sB2y=0, sB3y=0;

  Double_t SA1 =0, SA2 =0, SA3 =0, SB1 =0, SB2 =0, SB3 =0;
  Double_t SA1x=0, SA2x=0, SA3x=0, SB1x=0, SB2x=0, SB3x=0;
  Double_t SA1y=0, SA2y=0, SA3y=0, SB1y=0, SB2y=0, SB3y=0;

  { 
    for( Int_t n=0; n<3; n++ )
      {
	sA1  += s1[n]*A1[0][n];
	sA1x += s1[n]*A1[1][n];
	sA1y += s1[n]*A1[2][n];
	sB1  += s1[n]*B1[0][n];
	sB1x += s1[n]*B1[1][n];
	sB1y += s1[n]*B1[2][n];

	SA1  += S1[n]*A1[0][n];
	SA1x += S1[n]*A1[1][n];
	SA1y += S1[n]*A1[2][n];
	SB1  += S1[n]*B1[0][n];
	SB1x += S1[n]*B1[1][n];
	SB1y += S1[n]*B1[2][n];
	
	if(order>1) for( Int_t m=0; m<3;m++)
	  {
	    sA2  += s2[n][m]*A2[0][n][m];
	    sA2x += s2[n][m]*A2[1][n][m];
	    sA2y += s2[n][m]*A2[2][n][m];
	    sB2  += s2[n][m]*B2[0][n][m];
	    sB2x += s2[n][m]*B2[1][n][m];
	    sB2y += s2[n][m]*B2[2][n][m];

	    SA2  += S2[n][m]*A2[0][n][m];
	    SA2x += S2[n][m]*A2[1][n][m];
	    SA2y += S2[n][m]*A2[2][n][m];
	    SB2  += S2[n][m]*B2[0][n][m];
	    SB2x += S2[n][m]*B2[1][n][m];
	    SB2y += S2[n][m]*B2[2][n][m];
	    
	    if(order>2) for(Int_t k=0; k<3; k++ )
	      {
		sA3  += s3[n][m][k]*A3[0][n][m][k];
		sA3x += s3[n][m][k]*A3[1][n][m][k];
		sA3y += s3[n][m][k]*A3[2][n][m][k];
		sB3  += s3[n][m][k]*B3[0][n][m][k];
		sB3x += s3[n][m][k]*B3[1][n][m][k];
		sB3y += s3[n][m][k]*B3[2][n][m][k];

		SA3  += S3[n][m][k]*A3[0][n][m][k];
		SA3x += S3[n][m][k]*A3[1][n][m][k];
		SA3y += S3[n][m][k]*A3[2][n][m][k];
		SB3  += S3[n][m][k]*B3[0][n][m][k];
		SB3x += S3[n][m][k]*B3[1][n][m][k];
		SB3y += S3[n][m][k]*B3[2][n][m][k];
	      }
	  }
      }
  }
 

  T_out[0] = T_in[0] + tx*dz + ht*( SA1 + ht*(SA2 + ht*SA3) );
  T_out[1] = T_in[1] + ty*dz + ht*( SB1 + ht*(SB2 + ht*SB3) );
  T_out[2] = T_in[2] + ht*( sA1 + ht*(sA2 + ht*sA3) );
  T_out[3] = T_in[3] + ht*( sB1 + ht*(sB2 + ht*sB3) );	      
  T_out[4] = T_in[4];
  T_out[5] = z_out;  

  Double_t J[25];
  
  // derivatives '_x

  J[0] = 1; J[1] = 0; J[2] = 0; J[3] = 0; J[4] = 0;
  
  // derivatives '_y

  J[5] = 0; J[6] = 1; J[7] = 0; J[8] = 0; J[9] = 0;
  
  
  // derivatives '_tx

  J[10] = dz + h*tx*(1./t*SA1 + h*(2*SA2 + 3*ht*SA3)) + ht*( SA1x + ht*(SA2x + ht*SA3x) );
  J[11] =      h*tx*(1./t*SB1 + h*(2*SB2 + 3*ht*SB3)) + ht*( SB1x + ht*(SB2x + ht*SB3x) );
  J[12] =  1 + h*tx*(1./t*sA1 + h*(2*sA2 + 3*ht*sA3)) + ht*( sA1x + ht*(sA2x + ht*sA3x) );
  J[13] =      h*tx*(1./t*sB1 + h*(2*sB2 + 3*ht*sB3)) + ht*( sB1x + ht*(sB2x + ht*sB3x) );
  J[14] = 0;

 
  // derivatives '_ty

  J[15] =      h*ty*(1./t*SA1 + h*(2*SA2 + 3*ht*SA3)) + ht*( SA1y + ht*(SA2y + ht*SA3y) );
  J[16] = dz + h*ty*(1./t*SB1 + h*(2*SB2 + 3*ht*SB3)) + ht*( SB1y + ht*(SB2y + ht*SB3y) );
  J[17] =      h*ty*(1./t*sA1 + h*(2*sA2 + 3*ht*sA3)) + ht*( sA1y + ht*(sA2y + ht*sA3y) );
  J[18] =  1 + h*ty*(1./t*sB1 + h*(2*sB2 + 3*ht*sB3)) + ht*( sB1y + ht*(sB2y + ht*sB3y) );
  J[19] = 0;


  // derivatives '_qp

  J[20] = c_light*t*( SA1 + ht*( 2*SA2 + 3*ht*SA3 ) );
  J[21] = c_light*t*( SB1 + ht*( 2*SB2 + 3*ht*SB3 ) );
  J[22] = c_light*t*( sA1 + ht*( 2*sA2 + 3*ht*sA3 ) );
  J[23] = c_light*t*( sB1 + ht*( 2*sB2 + 3*ht*sB3 ) );
  J[24] = 1;

  // extrapolate inverse momentum

  T_out[4] = qp_in;
  
  Double_t dqp = qp_in - qp0;
  
  { for( Int_t i=0; i<4; i++ )
    {
      T_out[i]+=J[5*4+i]*dqp;
    }
  }

  //          covariance matrix transport 
  
  if ( C_in&&C_out ) CbmKFMath::multQtSQ( 5, J, C_in, C_out);

}


/****************************************************************
 *
 *  Analytic Central
 *
 ****************************************************************/


void CbmKFFieldMath::ExtrapolateACentral
( 
 const Double_t T_in [], // input track parameters (x,y,tx,ty,Q/p,z)
 const Double_t C_in [], // input covariance matrix
 Double_t       T_out[], // output track parameters
 Double_t       C_out[], // output covariance matrix
 Double_t       z_out  , // extrapolate to this z position
 Double_t       qp0    , // use Q/p linearisation at this value
 CbmMagField *MF       // magnetic field
 )
{
  //
  //  Part of the exact extrapolation formula with error (c_light*B*dz)^?/?!
  //  
  //  Under investigation, finally supposed to be fast.
  //  
  
  const Double_t c_light = 0.000299792458;

  Double_t qp_in = T_in[4];
  Double_t z_in = T_in[5];
  Double_t dz = z_out - z_in;

  Double_t i1[3], I1[3];

  { // get integrated field

    // get field value at 3 points

    Double_t r[3][3];
	    
    r[0][0] = 0;
    r[0][1] = 0;
    r[0][2] = T_in[5];
    
    r[2][0] = 0;
    r[2][1] = 0;
    r[2][2] = z_out;
    
    r[1][0] = 0;
    r[1][1] = 0;
    r[1][2] = 0.5*(r[0][2]+r[2][2]);
    
    IntegrateField( MF, r[0],r[1],r[2], i1, I1, 0, 0, 0, 0 );

  }// get integrated field
  
  Double_t 
    x   = T_in[2], // tx !!
    y   = T_in[3], // ty !!
    
    xx = x*x,
    xy = x*y,
    yy = y*y,

    x2 = x*2,
    y2 = y*2;


  Double_t  X1 [3] = { xy, -xx-1,  y };  
  Double_t  X1x[3] = {  y,   -x2,  0 };
  Double_t  X1y[3] = {  x,     0,  1 };


  Double_t  Y1 [3] = { yy+1, -xy, -x };  
  Double_t  Y1x[3] = {    0,  -y, -1 };
  Double_t  Y1y[3] = {   y2,  -x,  0 };


  Double_t iX1 =0, iY1 =0;
  Double_t iX1x=0, iY1x=0;
  Double_t iX1y=0, iY1y=0;

  Double_t IX1 =0, IY1 =0;
  Double_t IX1x=0, IY1x=0;
  Double_t IX1y=0, IY1y=0;

  { 
    for( Int_t n=0; n<3; n++ )
      {
	if( n!=1 ) continue;
	iX1  += i1[n]*X1 [n];
	iX1x += i1[n]*X1x[n];
	iX1y += i1[n]*X1y[n];
	iY1  += i1[n]*Y1 [n];
	iY1x += i1[n]*Y1x[n];
	iY1y += i1[n]*Y1y[n];

	IX1  += I1[n]*X1 [n];
	IX1x += I1[n]*X1x[n];
	IX1y += I1[n]*X1y[n];
	IY1  += I1[n]*Y1 [n];
	IY1x += I1[n]*Y1x[n];
	IY1y += I1[n]*Y1y[n];
      }
  }
 
  Double_t 
    t2   = 1. + xx + yy,
    t    = sqrt( t2 ),
    h    = qp0*c_light,
    ht = h*t;

  T_out[0] = T_in[0] + x*dz + ht*IX1;
  T_out[1] = T_in[1] + y*dz + ht*IY1;
  T_out[2] = T_in[2] + ht*iX1;
  T_out[3] = T_in[3] + ht*iY1;	      
  T_out[4] = T_in[4];
  T_out[5] = z_out;  

  Double_t J[25];
  
  // derivatives '_x

  J[0] = 1; J[1] = 0; J[2] = 0; J[3] = 0; J[4] = 0;
  
  // derivatives '_y

  J[5] = 0; J[6] = 1; J[7] = 0; J[8] = 0; J[9] = 0;
  
  
  // derivatives '_tx

  J[10] = dz + h*x/t*IX1 + ht*IX1x;
  J[11] =      h*x/t*IY1 + ht*IY1x;
  J[12] =  1 + h*x/t*iX1 + ht*iX1x;
  J[13] =      h*x/t*iY1 + ht*iY1x;
  J[14] = 0;

 
  // derivatives '_ty

  J[15] =      h*y/t*IX1 + ht*IX1y ;
  J[16] = dz + h*y/t*IY1 + ht*IY1y ;
  J[17] =      h*y/t*iX1 + ht*iX1y ;
  J[18] =  1 + h*y/t*iY1 + ht*iY1y ;
  J[19] = 0;


  // derivatives '_qp

  J[20] = c_light*t*IX1 ;
  J[21] = c_light*t*IY1 ;
  J[22] = c_light*t*iX1 ;
  J[23] = c_light*t*iY1 ;
  J[24] = 1;

  // extrapolate inverse momentum

  T_out[4] = qp_in;
  
  Double_t dqp = qp_in - qp0;
  
  { for( Int_t i=0; i<4; i++ )
    {
      T_out[i]+=J[5*4+i]*dqp;
    }
  }

  //          covariance matrix transport 
  
  if ( C_in&&C_out ) CbmKFMath::multQtSQ( 5, J, C_in, C_out);

}

#endif

/****************************************************************
 *
 *  Analytic Light
 *
 ****************************************************************/

Int_t CbmKFFieldMath::ExtrapolateALight
( 
 const Double_t T_in [], // input track parameters (x,y,tx,ty,Q/p)
 const Double_t C_in [], // input covariance matrix
 Double_t       T_out[], // output track parameters
 Double_t       C_out[], // output covariance matrix
 Double_t       z_out  , // extrapolate to this z position
 Double_t       qp0    , // use Q/p linearisation at this value
 FairField *MF       // magnetic field
 )
{  
  //
  //  Part of the analytic extrapolation formula with error (c_light*B*dz)^4/4!
  //  
  {
    bool ok = 1;
    for( int i=0; i<6; i++ ) ok = ok && finite(T_in[i]) && (T_in[i]<1.e5);
    if( C_in ) for( int i=0; i<15; i++ ) ok = ok && finite(C_in[i]);
    if( !ok ){
      for( int i=0; i<6; i++ ) T_out[i] = 0;
      if( C_out){
	for( int i=0; i<15; i++ ) C_out[i]=0;
	C_out[0] = C_out[2] = C_out[5] = C_out[9] = C_out[14] = 100.;
      }
      return 1;
    }
  }

  const Double_t c_light = 0.000299792458;
  
  Double_t qp_in = T_in[4];
  Double_t z_in = T_in[5];
  Double_t dz = z_out - z_in;

  // construct coefficients 

  Double_t 
    x   = T_in[2], // tx !!
    y   = T_in[3], // ty !!
    
    xx = x*x,
    xy = x*y,
    yy = y*y,
    x2 = x*2,
    x4 = x*4,
    xx31 = xx*3+1,
    xx159 = xx*15+9,
    y2 = y*2;

  Double_t 
    Ax = xy, 
    Ay = -xx-1,
    Az = y,
    Ayy = x*(xx*3+3),
    Ayz = -2*xy, 
    Ayyy = -(15*xx*xx+18*xx+3),

    Ax_x = y, 
    Ay_x = -x2, 
    Az_x = 0,
    Ayy_x = 3*xx31,
    Ayz_x = -y2,
    Ayyy_x = -x4*xx159,

    Ax_y = x, 
    Ay_y = 0, 
    Az_y = 1,
    Ayy_y = 0, 
    Ayz_y = -x2,
    Ayyy_y = 0,

    Bx = yy+1, 
    By = -xy,
    Bz = -x,
    Byy = y*xx31, 
    Byz = 2*xx+1,
    Byyy = -xy*xx159,

    Bx_x =  0, 
    By_x = -y, 
    Bz_x = -1,
    Byy_x = 6*xy,
    Byz_x = x4,
    Byyy_x = -y*(45*xx+9),

    Bx_y = y2, 
    By_y = -x, 
    Bz_y = 0,
    Byy_y = xx31,
    Byz_y = 0,
    Byyy_y = -x*xx159;

  // end of coefficients calculation


  Double_t 
    t2   = 1. + xx + yy;
  if( t2>1.e4 ) return 1;
  Double_t t  = sqrt( t2 ),
    h    = qp0*c_light,
    ht = h*t;

  Double_t sx=0, sy=0, sz=0, syy=0, syz=0, syyy=0, Sx=0, Sy=0, Sz=0, Syy=0, Syz=0, Syyy=0;
  
  { // get field integrals

    Double_t B[3][3];   
    Double_t r0[3], r1[3],r2[3];

    // first order track approximation

    r0[0] = T_in[0];
    r0[1] = T_in[1];
    r0[2] = T_in[5];
  
    r2[0] = T_in[0] + T_in[2]*dz;
    r2[1] = T_in[1] + T_in[3]*dz;
    r2[2] = z_out;
  
    r1[0] = 0.5*(r0[0]+r2[0]);
    r1[1] = 0.5*(r0[1]+r2[1]);
    r1[2] = 0.5*(r0[2]+r2[2]);

    MF->GetFieldValue( r0, B[0] );
    MF->GetFieldValue( r1, B[1] );
    MF->GetFieldValue( r2, B[2] );
    
    Sy = (7*B[0][1] + 6*B[1][1]-B[2][1] )*dz*dz/96.;
    r1[0] = T_in[0] + x*dz/2 + ht*Sy*Ay;
    r1[1] = T_in[1] + y*dz/2 + ht*Sy*By;

    Sy = (B[0][1] + 2*B[1][1] )*dz*dz/6.;
    r2[0] = T_in[0] + x*dz + ht*Sy*Ay;
    r2[1] = T_in[1] + y*dz + ht*Sy*By;    

    Sy = 0;

    // integrals

    MF->GetFieldValue( r0, B[0] );
    MF->GetFieldValue( r1, B[1] );
    MF->GetFieldValue( r2, B[2] );
    
    sx = ( B[0][0] + 4*B[1][0] + B[2][0] )*dz/6.;
    sy = ( B[0][1] + 4*B[1][1] + B[2][1] )*dz/6.;
    sz = ( B[0][2] + 4*B[1][2] + B[2][2] )*dz/6.;

    Sx = ( B[0][0] + 2*B[1][0])*dz*dz/6.;
    Sy = ( B[0][1] + 2*B[1][1])*dz*dz/6.;
    Sz = ( B[0][2] + 2*B[1][2])*dz*dz/6.;

    Double_t c2[3][3]    =   { {  5, -4, -1},{  44,  80,  -4},{ 11, 44, 5} }; // /=360.    
    Double_t C2[3][3]    =   { { 38,  8, -4},{ 148, 208, -20},{  3, 36, 3} }; // /=2520.
    for(Int_t n=0; n<3; n++)
      for(Int_t m=0; m<3; m++) 
	{
	  syz += c2[n][m]*B[n][1]*B[m][2];
	  Syz += C2[n][m]*B[n][1]*B[m][2];
	}
 
    syz  *= dz*dz/360.;
    Syz  *= dz*dz*dz/2520.;
    
    syy  = ( B[0][1] + 4*B[1][1] + B[2][1] )*dz;
    syyy = syy*syy*syy / 1296;
    syy  = syy*syy/72;

    Syy = ( B[0][1]*( 38*B[0][1] + 156*B[1][1]  -   B[2][1] )+
	    B[1][1]*(              208*B[1][1]  +16*B[2][1] )+
	    B[2][1]*(                             3*B[2][1] )  
	    )*dz*dz*dz/2520.;
    Syyy = 
      (
       B[0][1]*( B[0][1]*( 85*B[0][1] + 526*B[1][1]  - 7*B[2][1] )+
		 B[1][1]*(             1376*B[1][1]  +84*B[2][1] )+
		 B[2][1]*(                            19*B[2][1] )  )+
       B[1][1]*( B[1][1]*(             1376*B[1][1] +256*B[2][1] )+
		 B[2][1]*(                            62*B[2][1] )  )+
       B[2][1]*B[2][1]  *(                             3*B[2][1] )       
       )*dz*dz*dz*dz/90720.;    
 
  }


  Double_t 

    sA1   = sx*Ax   + sy*Ay   + sz*Az  ,
    sA1_x = sx*Ax_x + sy*Ay_x + sz*Az_x,
    sA1_y = sx*Ax_y + sy*Ay_y + sz*Az_y,

    sB1   = sx*Bx   + sy*By   + sz*Bz  ,
    sB1_x = sx*Bx_x + sy*By_x + sz*Bz_x,
    sB1_y = sx*Bx_y + sy*By_y + sz*Bz_y,

    SA1   = Sx*Ax   + Sy*Ay   + Sz*Az  ,
    SA1_x = Sx*Ax_x + Sy*Ay_x + Sz*Az_x,
    SA1_y = Sx*Ax_y + Sy*Ay_y + Sz*Az_y,

    SB1   = Sx*Bx   + Sy*By   + Sz*Bz  ,
    SB1_x = Sx*Bx_x + Sy*By_x + Sz*Bz_x,
    SB1_y = Sx*Bx_y + Sy*By_y + Sz*Bz_y,


    sA2   = syy*Ayy   + syz*Ayz  ,
    sA2_x = syy*Ayy_x + syz*Ayz_x,
    sA2_y = syy*Ayy_y + syz*Ayz_y,
    sB2   = syy*Byy   + syz*Byz  ,
    sB2_x = syy*Byy_x + syz*Byz_x,
    sB2_y = syy*Byy_y + syz*Byz_y,
  
    SA2   = Syy*Ayy   + Syz*Ayz  ,
    SA2_x = Syy*Ayy_x + Syz*Ayz_x,
    SA2_y = Syy*Ayy_y + Syz*Ayz_y,
    SB2   = Syy*Byy   + Syz*Byz  ,
    SB2_x = Syy*Byy_x + Syz*Byz_x,
    SB2_y = Syy*Byy_y + Syz*Byz_y,


    sA3   = syyy*Ayyy  ,
    sA3_x = syyy*Ayyy_x,
    sA3_y = syyy*Ayyy_y,
    sB3   = syyy*Byyy  ,
    sB3_x = syyy*Byyy_x,
    sB3_y = syyy*Byyy_y,

    SA3   = Syyy*Ayyy  ,
    SA3_x = Syyy*Ayyy_x,
    SA3_y = Syyy*Ayyy_y,
    SB3   = Syyy*Byyy  ,
    SB3_x = Syyy*Byyy_x,
    SB3_y = Syyy*Byyy_y;
 

  T_out[0] = T_in[0] + x*dz + ht*( SA1 + ht*(SA2 + ht*SA3) );
  T_out[1] = T_in[1] + y*dz + ht*( SB1 + ht*(SB2 + ht*SB3) );
  T_out[2] = T_in[2] + ht*( sA1 + ht*(sA2 + ht*sA3) );
  T_out[3] = T_in[3] + ht*( sB1 + ht*(sB2 + ht*sB3) );	      
  T_out[4] = T_in[4];
  T_out[5] = z_out;  
  
  Double_t J[25];
  
  // derivatives '_x

  J[0] = 1; J[1] = 0; J[2] = 0; J[3] = 0; J[4] = 0;
  
  // derivatives '_y

  J[5] = 0; J[6] = 1; J[7] = 0; J[8] = 0; J[9] = 0;
  
  
  // derivatives '_tx

  J[10] = dz + h*x*(1./t*SA1 + h*(2*SA2 + 3*ht*SA3)) + ht*( SA1_x + ht*(SA2_x + ht*SA3_x) );
  J[11] =      h*x*(1./t*SB1 + h*(2*SB2 + 3*ht*SB3)) + ht*( SB1_x + ht*(SB2_x + ht*SB3_x) );
  J[12] =  1 + h*x*(1./t*sA1 + h*(2*sA2 + 3*ht*sA3)) + ht*( sA1_x + ht*(sA2_x + ht*sA3_x) );
  J[13] =      h*x*(1./t*sB1 + h*(2*sB2 + 3*ht*sB3)) + ht*( sB1_x + ht*(sB2_x + ht*sB3_x) );
  J[14] = 0;

 
  // derivatives '_ty

  J[15] =      h*y*(1./t*SA1 + h*(2*SA2 + 3*ht*SA3)) + ht*( SA1_y + ht*(SA2_y + ht*SA3_y) );
  J[16] = dz + h*y*(1./t*SB1 + h*(2*SB2 + 3*ht*SB3)) + ht*( SB1_y + ht*(SB2_y + ht*SB3_y) );
  J[17] =      h*y*(1./t*sA1 + h*(2*sA2 + 3*ht*sA3)) + ht*( sA1_y + ht*(sA2_y + ht*sA3_y) );
  J[18] =  1 + h*y*(1./t*sB1 + h*(2*sB2 + 3*ht*sB3)) + ht*( sB1_y + ht*(sB2_y + ht*sB3_y) );
  J[19] = 0;


  // derivatives '_qp

  J[20] = c_light*t*( SA1 + ht*( 2*SA2 + 3*ht*SA3 ) );
  J[21] = c_light*t*( SB1 + ht*( 2*SB2 + 3*ht*SB3 ) );
  J[22] = c_light*t*( sA1 + ht*( 2*sA2 + 3*ht*sA3 ) );
  J[23] = c_light*t*( sB1 + ht*( 2*sB2 + 3*ht*sB3 ) );
  J[24] = 1;

  // extrapolate inverse momentum

  T_out[4] = qp_in;
  
  Double_t dqp = qp_in - qp0;
  
  { for( Int_t i=0; i<4; i++ )
    {
      T_out[i]+=J[5*4+i]*dqp;
    }
  }

  //          covariance matrix transport 
  
  if ( C_in&&C_out ) CbmKFMath::multQtSQ( 5, J, C_in, C_out);
  return 0;
}
