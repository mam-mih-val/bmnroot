#include "CbmKFMath.h"  

#include "FairField.h"
#include "FairTrackParam.h"

#include <cmath>

using std::sqrt;
using std::exp;
using std::log;
using std::fabs;

ClassImp(CbmKFMath)

Bool_t CbmKFMath::intersectCone( Double_t zCone, Double_t ZCone, Double_t rCone, Double_t RCone ,
				     const Double_t x[], 
				     Double_t *z1, Double_t *z2
				     )
{    
  Double_t t = (RCone-rCone)/(ZCone-zCone);
  Double_t A = (RCone*zCone-rCone*ZCone)/(ZCone-zCone);
  Double_t x0 = x[0]-x[5]*x[2];
  Double_t y0 = x[1]-x[5]*x[3];
  Double_t tx = x[2];
  Double_t ty = x[3];
  Double_t a = tx*tx+ty*ty-t*t;
  Double_t b = 2*(tx*x0+ty*y0 + t*A);
  Double_t c = x0*x0+y0*y0-A*A;
  
  if ( fabs(a)<1.E-4 ) return 1;
  Double_t D = b*b - 4*a*c;
  if ( D<0. ) return 1;
  D = sqrt(D);
  *z1 = (-b + D )/(2*a);
  *z2 = (-b - D )/(2*a);
  return 0;
}


void CbmKFMath::multQSQt( Int_t N, const Double_t Q[], const Double_t S[], Double_t S_out[] )
  {
    Double_t A[N*N];

    {for( Int_t i=0, n=0; i<N; i++ )
      {
        for( Int_t j=0; j<N; j++, ++n )
	  {
            A[n] = 0 ;
            for( Int_t k=0; k<N; ++k ) A[n]+= S[indexS(i,k)] * Q[ j*N+k];
	  }
      }}
    
    {for( Int_t i=0; i<N; i++ )
      {
        for( Int_t j=0; j<=i; j++ )
	  {
            Int_t n = indexS(i,j);
            S_out[n] = 0 ;
            for( Int_t k=0; k<N; k++ )  S_out[n] += Q[ i*N+k ] * A[ k*N+j ];
	  }
      }}
  }

void CbmKFMath::multQtSQ( Int_t N, const Double_t Q[], const Double_t S[], Double_t S_out[] )
{
  Double_t A[N*N];
  
  for( Int_t i=0, n=0; i<N; i++ ){
    for( Int_t j=0; j<N; j++, ++n ){
      A[n] = 0 ;
      for( Int_t k=0; k<N; ++k ) A[n]+= S[indexS(i,k)] * Q[ k*N+j];
    }
  }
  
  for( Int_t i=0; i<N; i++ ){
    for( Int_t j=0; j<=i; j++ ){
      Int_t n = indexS(i,j);
      S_out[n] = 0 ;
      for( Int_t k=0; k<N; k++ )  S_out[n] += Q[ k*N+i ] * A[ k*N+j ];
    }
  }
}

void CbmKFMath::multSSQ( const Double_t *A, const Double_t *B, Double_t *C, Int_t n )
  {
    for( Int_t i=0; i<n; ++i )
      {
	for( Int_t j=0; j<n; ++j )
	  {
	    Int_t ind = i*n+j;
	    C[ind] = 0;
	    for( Int_t k=0; k<n; ++k ) C[ind] += A[indexS(i,k)] * B[indexS(k,j)];
	  }
      }
  }

void CbmKFMath::four_dim_inv( Double_t a[4][4] )
  {
    /**** Gaussian algorithm for 4x4 matrix inversion ****/
    Int_t i,j,k,l;
    Double_t factor;
    Double_t temp[4];
    Double_t b[4][4];
    
    // Set b to I
    for(i=0;i<4;i++) for(j=0;j<4;j++)
      if(i==j) b[i][j]=1.0; else b[i][j]=0.0;
    
    for(i=0;i<4;i++){
      for(j=i+1;j<4;j++)
	if (fabs(a[i][i])<fabs(a[j][i])){
	  for(l=0;l<4;l++) temp[l]=a[i][l];
	  for(l=0;l<4;l++) a[i][l]=a[j][l];
	  for(l=0;l<4;l++) a[j][l]=temp[l];
	  for(l=0;l<4;l++) temp[l]=b[i][l];
	  for(l=0;l<4;l++) b[i][l]=b[j][l];
	  for(l=0;l<4;l++) b[j][l]=temp[l];
	}
      factor=a[i][i];
      for(j=4-1;j>-1;j--) {
	b[i][j]/=factor;a[i][j]/=factor;
      }
      for(j=i+1;j<4;j++) {
	factor=-a[j][i];
	for(k=0;k<4;k++){
	  a[j][k]+=a[i][k]*factor;b[j][k]+=b[i][k]*factor;
	}
      }
    } 
    for(i=3;i>0;i--){
      for(j=i-1;j>-1;j--){
	factor=-a[j][i];
	for(k=0;k<4;k++){
	  a[j][k]+=a[i][k]*factor;b[j][k]+=b[i][k]*factor;
	}
      }
    }
    
    // copy b to a and return
    for(i=0;i<4;i++) for(j=0;j<4;j++) a[i][j]=b[i][j];
  }

void CbmKFMath::five_dim_inv(Double_t a[5][5])
{
  /**** Gaussian algorithm for 5x5 matrix inversion ****/
  Int_t i,j,k,l;
  Double_t factor;
  Double_t temp[5];
  Double_t b[5][5];
  
  // Set b to I
  for(i=0;i<5;i++)
      for(j=0;j<5;j++)
    if(i==j) b[i][j]=1.0; else b[i][j]=0.0;
  
    for(i=0;i<5;i++){
      for(j=i+1;j<5;j++)
	if (fabs(a[i][i])<fabs(a[j][i])){
	  for(l=0;l<5;l++) temp[l]=a[i][l];
	  for(l=0;l<5;l++) a[i][l]=a[j][l];
	  for(l=0;l<5;l++) a[j][l]=temp[l];
	  for(l=0;l<5;l++) temp[l]=b[i][l];
	  for(l=0;l<5;l++) b[i][l]=b[j][l];
	  for(l=0;l<5;l++) b[j][l]=temp[l];
	}
      factor=a[i][i];
      // cout<<"Highest element "<<a[i][i]<<endl;
      for(j=5-1;j>-1;j--){
	  b[i][j]/=factor; 
	  a[i][j]/=factor;
      }
      for(j=i+1;j<5;j++) {
	factor=-a[j][i];
	for(k=0;k<5;k++){
	  a[j][k]+=a[i][k]*factor;
	  b[j][k]+=b[i][k]*factor;
	}
      }
    } 
    for(i=4;i>0;i--){
      for(j=i-1;j>-1;j--){
	factor=-a[j][i];
	for(k=0;k<5;k++){
	  a[j][k]+=a[i][k]*factor;
	  b[j][k]+=b[i][k]*factor;
	}
      }
    }
    
    // copy b to a and return
    for(i=0;i<5;i++) for(j=0;j<5;j++) a[i][j]=b[i][j];
  }      

Bool_t CbmKFMath::invS( Double_t A[], Int_t N )
{
  
  Bool_t ret = 0;
  
  const Double_t ZERO = 1.E-20;
  
  // input: simmetric > 0 NxN matrix A = {a11,a21,a22,a31..a33,..}  
  // output: inverse A, in case of problems fill zero and return 1
  
  // A->low triangular Anew : A = Anew x Anew^T
  // method:
  // for(j=1,N) for(i=j,N) Aij=(Aii-sum_{k=1}^{j-1}Aik*Ajk )/Ajj
    //   
    
    {
      Double_t *j1 = A, *jj = A;
      for( Int_t j=1; j<=N; j1+=j++, jj+=j ){
	Double_t *ik = j1, x = 0;
	while( ik!=jj ){
	  x -= (*ik) * (*ik);
	  ik++;
	}
	x += *ik;
	if( x > ZERO ){
	  x = sqrt(x);
	  *ik = x;
	  ik++;
	  x = 1 / x;
	  for( Int_t step=1; step<=N-j; ik+=++step ){ // ik==Ai1
	  Double_t sum = 0;
	  for( Double_t *jk=j1; jk!=jj; sum += *(jk++) * *(ik++) );
	  *ik = (*ik - sum) * x; // ik == Aij
	  }
	}else{
	  Double_t *ji=jj;
	  for( Int_t i=j; i<N; i++ ) *(ji+=i) = 0.;
	  ret = 1;
	}   
      }
    }
    
    // A -> Ainv
    // method : 
    // for(i=1,N){ 
    //   Aii = 1/Aii; 
    //   for(j=1,i-1) Aij=-(sum_{k=j}^{i-1} Aik * Akj) / Aii ;
    // }
        
    {
      Double_t *ii=A,*ij=A;
      for( Int_t i = 1; i<=N; ij=ii+1, ii+=++i ){
	if( *ii > ZERO ){
	  Double_t x = -(*ii = 1./ *ii);
	  { 
	    Double_t *jj = A;
	    for( Int_t j=1; j<i; jj+=++j, ij++ ){
	      Double_t *ik = ij, *kj = jj, sum = 0.;
	      for( Int_t k=j; ik!=ii; kj+=k++, ik++ ){
		sum += *ik * *kj;
	      }
	      *kj = sum * x;
	    }
	  }
	}else{      
	  for( Double_t *ik = ij; ik!=ii+1; ik++ ){
	    *ik = 0.;
	  }
	  ret = 1;
	}
      }
    }
    
    // A -> A^T x A
    // method: 
    // Aij = sum_{k=i}^N Aki * Akj
    
    {
      Double_t *ii=A, *ij=A;
      for( Int_t i=1; i<=N; ii+=++i ){
	do{ 
	  Double_t *ki = ii, *kj = ij, sum = 0.;
	  for( Int_t k=i; k<=N; ki+=k, kj+=k++ ) sum += (*ki) * (*kj);
	  *ij = sum;
	}while( (ij++)!=ii );
      }    
    }
    return ret;    
  }
 
Double_t CbmKFMath::getDeviation( Double_t x, Double_t y, Double_t C[], Double_t vx, Double_t vy, Double_t Cv[] )
  {
    Double_t dx = x - vx;
    Double_t dy = y - vy;
    Double_t c[3] = { 0, 0, 0 };
    if( C  ) { c[0] = C [0];  c[1] = C [1];  c[2] = C [2]; }
    if( Cv ) { c[0]+= Cv[0];  c[1]+= Cv[1];  c[2]+= Cv[2]; }
    Double_t d = c[0]*c[2] - c[1]*c[1] ;
    if( fabs(d)<1.e-20 ) return 0;
    return sqrt( fabs( 0.5*(dx*dx*c[0]-2*dx*dy*c[1]+dy*dy*c[2])/d ));
  }

Double_t CbmKFMath::AnalyticQP(
				   const Double_t T[], // track parameters (x,y,tx,ty,Q/p,z)
				   const Double_t V[], // vertex parameters (x,y,z)
				   FairField *MagneticField       // magnetic field
				   )
{
  
  const Double_t c_light = 0.000299792458;  

  Double_t 
    x    = T[0],
    y    = T[1],
    tx   = T[2],
    ty   = T[3],
    qp0  = T[4],
    z    = T[5],

    vx   = V[0],
    //vy   = V[1],
    vz   = V[2],
    
    txtx = tx*tx,
    tyty = ty*ty,
    txty = tx*ty;
  
  Double_t 
    Ax = txty, 
    Ay = -txtx-1,
    Az = ty,
    Ayy = tx*(txtx*3+3),
    Ayz = -2*txty,
    Azy = Ayz,
    Ayyy = -(15*txtx*txtx+18*txtx+3),

    Bx = tyty+1, 
    By = -txty,
    Bz = -tx,
    Byy = ty*(txtx*3+1), 
    Byz = 2*txtx+1,
    Bzy = txtx-tyty,
    Byyy = -txty*(txtx*15+9);

  Double_t 
    t    = c_light*sqrt( 1. + txtx + tyty ),
    h    = t*qp0;

  // integrals
 
  Double_t Sx=0, Sy=0, Sz=0, Syy=0, Syz=0, Szy=0, Syyy=0;
 
  {
    Double_t sx=0,sy=0,sz=0,syy=0,syz=0, szy=0, syyy=0;

    Double_t r[3] = { x, y, z };
    Double_t B[3][3];

    Int_t n = int( fabs(vz-z)/5.);
    if( n<2 ) n = 2;
    Double_t dz = (vz-z)/n;

    {
      MagneticField->GetFieldValue( r, B[0] );
      r[0] += tx*dz;
      r[1] += ty*dz;
      r[2] += dz;
      MagneticField->GetFieldValue( r, B[1] );
      r[0] += tx*dz;
      r[1] += ty*dz;
      r[2] += dz;
      MagneticField->GetFieldValue( r, B[2] );

      sx = ( B[0][0] + 4*B[1][0] + B[2][0] )*dz*2/6.;
      sy = ( B[0][1] + 4*B[1][1] + B[2][1] )*dz*2/6.;
      sz = ( B[0][2] + 4*B[1][2] + B[2][2] )*dz*2/6.;

      Sx = ( B[0][0] + 2*B[1][0])*dz*dz*4/6.;
      Sy = ( B[0][1] + 2*B[1][1])*dz*dz*4/6.;
      Sz = ( B[0][2] + 2*B[1][2])*dz*dz*4/6.;

      Double_t c2[3][3]    =   { {  5, -4, -1},{  44,  80,  -4},{ 11, 44, 5} }; // /=360.
      Double_t C2[3][3]    =   { { 38,  8, -4},{ 148, 208, -20},{  3, 36, 3} }; // /=2520.
      for(Int_t k=0; k<3; k++)
	for(Int_t m=0; m<3; m++) 
	  {
	    syz += c2[k][m]*B[k][1]*B[m][2];
	    Syz += C2[k][m]*B[k][1]*B[m][2];
	    szy += c2[k][m]*B[k][2]*B[m][1];
	    Szy += C2[k][m]*B[k][2]*B[m][1];
	  }
      
      syz *= dz*dz*4/360.;
      Syz *= dz*dz*dz*8/2520.;

      szy *= dz*dz*4/360.;
      Szy *= dz*dz*dz*8/2520.;
	    
      syy  = ( B[0][1] + 4*B[1][1] + B[2][1] )*dz*2;
      syyy = syy*syy*syy / 1296;
      syy  = syy*syy/72;

      Syy = ( B[0][1]*( 38*B[0][1] + 156*B[1][1]  -   B[2][1] )+
	      B[1][1]*(              208*B[1][1]  +16*B[2][1] )+
	      B[2][1]*(                             3*B[2][1] )  
	      )*dz*dz*dz*8/2520.;      
      Syyy = 
	(
	 B[0][1]*( B[0][1]*( 85*B[0][1] + 526*B[1][1]  - 7*B[2][1] )+
		   B[1][1]*(             1376*B[1][1]  +84*B[2][1] )+
		   B[2][1]*(                            19*B[2][1] )  )+
	 B[1][1]*( B[1][1]*(             1376*B[1][1] +256*B[2][1] )+
		   B[2][1]*(                            62*B[2][1] )  )+
	 B[2][1]*B[2][1]  *(                             3*B[2][1] )       
	 )*dz*dz*dz*dz*16/90720.;

      x += tx*2*dz 
	+ h*( Sx*Ax + Sy*Ay + Sz*Az ) 
	+ h*h*( Syy*Ayy + Syz*Ayz + Szy*Azy )
	+ h*h*h*Syyy*Ayyy;
      y += ty*2*dz 
	+ h*( Sx*Bx + Sy*By + Sz*Bz ) 
	  + h*h*( Syy*Byy + Syz*Byz + Szy*Bzy )
	+ h*h*h*Syyy*Byyy;
      tx += h*( sx*Ax + sy*Ay + sz*Az ) 
	+ h*h*( syy*Ayy + syz*Ayz + szy*Azy )
	+ h*h*h*syyy*Ayyy;
      ty += h*( sx*Bx + sy*By + sz*Bz ) 
	+ h*h*( syy*Byy + syz*Byz + szy*Bzy )
	+ h*h*h*syyy*Byyy;
      z += 2*dz;
	txtx = tx*tx;
	tyty = ty*ty;
	txty = tx*ty;
  
	Ax = txty;
	Ay = -txtx-1;
	Az = ty;
	Ayy = tx*(txtx*3+3);
	Ayz = -2*txty;
	Azy = Ayz;
	Ayyy = -(15*txtx*txtx+18*txtx+3);

	Bx = tyty+1;
	By = -txty;
	Bz = -tx;
	Byy = ty*(txtx*3+1);
	Byz = 2*txtx+1;
	Bzy = txtx-tyty;
	Byyy = -txty*(txtx*15+9);

	t    = c_light*sqrt( 1. + txtx + tyty );
	h    = t*qp0;
    }

    for( Int_t i=2; i<n; i++ )
      {

	Double_t B0[3] = { B[0][0], B[0][1], B[0][2] };
	
	B[0][0] = B[1][0];
	B[0][1] = B[1][1];
	B[0][2] = B[1][2];

	B[1][0] = B[2][0];
	B[1][1] = B[2][1];
	B[1][2] = B[2][2];

	// estimate B[2] at +dz

	B[2][0] =  B0[0] -3*B[0][0] + 3*B[1][0];
	B[2][1] =  B0[1] -3*B[0][1] + 3*B[1][1];
	B[2][2] =  B0[2] -3*B[0][2] + 3*B[1][2];

	Double_t Sx_, Sy_, Sz_, Syy_, Syz_, Szy_, Syyy_;
	
	Sx_ = ( -B[0][0] + 10*B[1][0] + 3*B[2][0] )*dz*dz*4/96.;
	Sy_ = ( -B[0][1] + 10*B[1][1] + 3*B[2][1] )*dz*dz*4/96.;
	Sz_ = ( -B[0][2] + 10*B[1][2] + 3*B[2][2] )*dz*dz*4/96.;

	Syz_ = Szy_ = 0;

	Double_t c2[3][3]    =   { {  5, -52, -13},{  -28,  320,  68},{ -37, 332, 125} }; // /=5760
	Double_t C2[3][3]    =   { { 13,  -152, -29},{ -82, 1088, 170},{  -57, 576, 153} }; // /=80640
	{ for(Int_t k=0; k<3; k++)
	  for(Int_t m=0; m<3; m++) 
	    {
	      Syz_ += C2[k][m]*B[k][1]*B[m][2];
	      Szy_ += C2[k][m]*B[k][2]*B[m][1];
	    }
	}

	Syz_ *= dz*dz*dz*8/80640.;
	Szy_ *= dz*dz*dz*8/80640.;	

	Syy_ = ( B[0][1]*( 13*B[0][1] - 234*B[1][1] - 86*B[2][1] )+
		 B[1][1]*(             1088*B[1][1] +746*B[2][1] )+
		 B[2][1]*(                           153*B[2][1] )  
		 )*dz*dz*dz*8/80640.;

	Syyy_ = 
	  (
	   B[0][1]*( B[0][1]*( -43*B[0][1] + 1118*B[1][1]+451*B[2][1] )+
		     B[1][1]*(             -9824*B[1][1] -7644*B[2][1] )+
		     B[2][1]*(                           -1669*B[2][1] )  )+
	   B[1][1]*( B[1][1]*(             29344*B[1][1] +32672*B[2][1] )+
		     B[2][1]*(                            13918*B[2][1] )  )+
	   B[2][1]*B[2][1]  *(                            2157*B[2][1] )       
	   )*dz*dz*dz*dz*16/23224320.;
	
	r[2] += dz;
	r[0] = 
	  x + tx*dz 
	  + h*( Sx_*Ax + Sy_*Ay + Sz_*Az ) 
	  + h*h*( Syy_*Ayy + Syz_*Ayz + Szy_*Azy )
	  + h*h*h*Syyy_*Ayyy;
 	r[1] = y + ty*dz 
	  + h*( Sx_*Bx + Sy_*By + Sz_*Bz ) 
	  + h*h*( Syy_*Byy + Syz_*Byz + Szy_*Bzy )
	  + h*h*h*Syyy_*Byyy;
	/* 
	Syyy_+= dz*syyy+Sy_*syy+Syy_*sy+Syyy;
	Syy_ += dz*syy + sy*Sy_ + Syy;
	Syz_ += dz*syz + sz*Sy_ + Syz;
	Szy_ += dz*szy + sy*Sz_ + Szy;
	Sx_  += dz*sx + Sx;
	Sy_  += dz*sy + Sy;
	Sz_  += dz*sz + Sz;

	r[2] += dz;
	r[0] = 
	  x + tx*(r[2]-z) 
	  + h*( Sx_*Ax + Sy_*Ay + Sz_*Az ) 
	  + h*h*( Syy_*Ayy + Syz_*Ayz + Szy_*Azy )
	  + h*h*h*Syyy_*Ayyy;
 	r[1] = y + ty*(r[2]-z) 
	  + h*( Sx_*Bx + Sy_*By + Sz_*Bz ) 
	  + h*h*( Syy_*Byy + Syz_*Byz + Szy_*Bzy )
	  + h*h*h*Syyy_*Byyy;
	*/
	MagneticField->GetFieldValue( r, B[2] );

	Double_t sx_, sy_, sz_, syy_, syz_, szy_, syyy_;

	sx_ = ( -B[0][0] + 8*B[1][0] + 5*B[2][0] )*dz*2/24.;
	sy_ = ( -B[0][1] + 8*B[1][1] + 5*B[2][1] )*dz*2/24.;
	sz_ = ( -B[0][2] + 8*B[1][2] + 5*B[2][2] )*dz*2/24.;

	Sx_ = ( -B[0][0] + 10*B[1][0] + 3*B[2][0] )*dz*dz*4/96.;
	Sy_ = ( -B[0][1] + 10*B[1][1] + 3*B[2][1] )*dz*dz*4/96.;
	Sz_ = ( -B[0][2] + 10*B[1][2] + 3*B[2][2] )*dz*dz*4/96.;

	syz_ = Syz_ = szy_ = Szy_ = 0;

	{ for(Int_t k=0; k<3; k++)
	  for(Int_t m=0; m<3; m++) 
	    {
	      syz_ += c2[k][m]*B[k][1]*B[m][2];
	      Syz_ += C2[k][m]*B[k][1]*B[m][2];
	      szy_ += c2[k][m]*B[k][2]*B[m][1];
	      Szy_ += C2[k][m]*B[k][2]*B[m][1];
	    }
	}
	syz_ *= dz*dz*4/5760.;
	Syz_ *= dz*dz*dz*8/80640.;

	szy_ *= dz*dz*4/5760.;
	Szy_ *= dz*dz*dz*8/80640.;
	
	syy_  = ( B[0][1] -8*B[1][1] - 5*B[2][1] )*dz*2;
	syyy_ = -syy_*syy_*syy_/82944;
	syy_  = syy_*syy_/1152;

	Syy_ = ( B[0][1]*( 13*B[0][1] - 234*B[1][1] - 86*B[2][1] )+
		 B[1][1]*(             1088*B[1][1] +746*B[2][1] )+
		 B[2][1]*(                           153*B[2][1] )  
		 )*dz*dz*dz*8/80640.;
	
	Syyy_ = 
	  (
	   B[0][1]*( B[0][1]*( -43*B[0][1] + 1118*B[1][1]+451*B[2][1] )+
		     B[1][1]*(             -9824*B[1][1] -7644*B[2][1] )+
		     B[2][1]*(                           -1669*B[2][1] )  )+
	   B[1][1]*( B[1][1]*(             29344*B[1][1] +32672*B[2][1] )+
		     B[2][1]*(                            13918*B[2][1] )  )+
	   B[2][1]*B[2][1]  *(                            2157*B[2][1] )       
	   )*dz*dz*dz*dz*16/23224320.;

	x += tx*dz 
	  + h*( Sx_*Ax + Sy_*Ay + Sz_*Az ) 
	  + h*h*( Syy_*Ayy + Syz_*Ayz + Szy_*Azy )
	  + h*h*h*Syyy_*Ayyy;
	y += ty*dz 
	  + h*( Sx_*Bx + Sy_*By + Sz_*Bz ) 
	  + h*h*( Syy_*Byy + Syz_*Byz + Szy_*Bzy )
	  + h*h*h*Syyy_*Byyy;
	tx += h*( sx_*Ax + sy_*Ay + sz_*Az ) 
	  + h*h*( syy_*Ayy + syz_*Ayz + szy_*Azy )
	  + h*h*h*syyy_*Ayyy;
	ty += h*( sx_*Bx + sy_*By + sz_*Bz ) 
	  + h*h*( syy_*Byy + syz_*Byz + szy_*Bzy )
	  + h*h*h*syyy_*Byyy;
	z += dz;
	txtx = tx*tx;
	tyty = ty*ty;
	txty = tx*ty;
  
	Ax = txty;
	Ay = -txtx-1;
	Az = ty;
	Ayy = tx*(txtx*3+3);
	Ayz = -2*txty;
	Azy = Ayz;
	Ayyy = -(15*txtx*txtx+18*txtx+3);

	Bx = tyty+1;
	By = -txty;
	Bz = -tx;
	Byy = ty*(txtx*3+1);
	Byz = 2*txtx+1;
	Bzy = txtx-tyty;
	Byyy = -txty*(txtx*15+9);

	t    = c_light*sqrt( 1. + txtx + tyty );
	h    = t*qp0;


	Syyy+= dz*syyy+Sy_*syy+Syy_*sy+Syyy_;
	Syy += dz*syy + sy*Sy_ + Syy_;
	Syz += dz*syz + sz*Sy_ + Syz_;
	Szy += dz*szy + sy*Sz_ + Szy_;
	Sx  += dz*sx + Sx_;
	Sy  += dz*sy + Sy_;
	Sz  += dz*sz + Sz_;

	syyy+= sy_*syy+syy_*sy+syyy_;
	syy += sy*sy_ + syy_;
	syz += sz*sy_ + syz_;
	szy += sz*sz_ + szy_;
	sx  += sx_;
	sy  += sy_;
	sz  += sz_;
      }
    //cout<<x-vx<<" "<<y-vy<<" "<<z-vz<<" "<<1./qp0<<endl;
  }

  x    = T[0];
  y    = T[1];
  tx   = T[2];
  ty   = T[3];
  z    = T[5];
  txtx = tx*tx;
  tyty = ty*ty;
  txty = tx*ty;
  
  Ax = txty;
  Ay = -txtx-1;
  Az = ty;
  Ayy = tx*(txtx*3+3);
  Ayz = -2*txty;
  Azy = Ayz;
  Ayyy = -(15*txtx*txtx+18*txtx+3);

  Bx = tyty+1;
  By = -txty;
  Bz = -tx;
  Byy = ty*(txtx*3+1);
  Byz = 2*txtx+1;
  Bzy = txtx-tyty;
  Byyy = -txty*(txtx*15+9);

  t    = c_light*sqrt( 1. + txtx + tyty );
  h    = t*qp0;

  Double_t 
    
    c = (x-vx) + tx*(vz-z),
    b   =   t*( Sx*Ax   + Sy*Ay   + Sz*Az ) ,	  
    a   = t*t*( Syy*Ayy   + Syz*Ayz + Szy*Azy ),
    d   = t*t*t*( Syyy*Ayyy);

  Double_t 
    C = d*qp0*qp0*qp0 + a*qp0*qp0 + b*qp0 + c,
    B = 3*d*qp0*qp0+2*a*qp0+b,
    A = 3*d + a;

  Double_t D = B*B-4*A*C;
  if( D<0 ) D = 0;
  D = sqrt( D );
      
  Double_t dqp1 = (-B+D)/2/A;
  Double_t dqp2 = (-B-D)/2/A;
  Double_t dqp = (fabs(dqp1)<fabs(dqp2)) ?dqp1 : dqp2;
    
  return qp0+dqp;
}
  

Bool_t CbmKFMath::GetThickness( Double_t z1, Double_t z2, Double_t mz, Double_t mthick, 
			    Double_t *mz_out, Double_t *mthick_out )
{
  Double_t z, Z;
  
  if ( z1<z2 )
    {
      z = z1;
      Z = z2;
    }
  else
    {
      Z = z1;
      z = z2;
    }
  
  Double_t 
    tmin = mz - mthick/2, 
    tmax = mz + mthick/2;
  if ( tmin >= Z  ||  z >= tmax ) return 1;
  if ( z<=tmin && tmax<=Z ) // case  z |**| Z
    {
      *mz_out = mz;
      *mthick_out = mthick;
    }
  else if ( z<=tmin && tmin<Z && Z<=tmax ) // case  z |*Z*|
    {
      *mz_out      = (tmin + Z)/2;
      *mthick_out  = Z - tmin;	    
    }
  else if ( tmax<=Z && tmin<=z && z<tmax ) // case |*z*| Z
    {
      *mz_out        = (tmax + z)/2;
      *mthick_out    = tmax - z;	    
    }
  else if ( tmin<=z && Z<=tmax ) // case |*zZ*|
    {
      *mz_out        = (Z + z)/2;
      *mthick_out    = Z - z;	    
    }  
  return 0;
}


Int_t CbmKFMath::GetNoise( Double_t Lrl, Double_t F,Double_t Fe,
			   Double_t tx, Double_t ty, Double_t qp, 
			   Double_t mass, Bool_t is_electron, 
			   Bool_t downstream_direction, 
			   Double_t *Q5, Double_t *Q8, Double_t *Q9, Double_t *Ecor )
{
  *Q5 = 0;
  *Q8 = 0;
  *Q9 = 0;
  *Ecor = 1.; 
  Double_t t = sqrt(1.+tx*tx+ty*ty);
  if( !finite(t) || t>1.e4 ) return 1;
  t = sqrt(t);
  Double_t l = t*Lrl;  
  if ( l > 1.) l = 1.; // protect against l too big  
  Double_t s0 = (l>exp(-1./0.038) ) ? F*.0136*(1+0.038*log(l))*qp :0.;
  Double_t a  = (1.+mass*mass*qp*qp)*s0*s0*t*t*l;
  
  *Q5 = a*(1.+tx*tx);
  *Q8 = a*tx*ty;
  *Q9 = a*(1.+ty*ty);
  
  // Apply correction for dE/dx energy loss	  
  
  Double_t L = downstream_direction ?l :-l;
  
  if( 0&&is_electron ) // Bremsstrahlung effect for e+,e-
    { 
      *Ecor = exp( L );
    }
  
  if(1) // ionisation energy loss
    {
      Double_t m_energyLoss = Fe;
//      Double_t m_energyLoss = 0.02145;//0.060;
      Double_t corr = (1.-  fabs(qp) * L * m_energyLoss );
      if( corr>0.001*fabs(qp)) *Ecor *= 1./corr;
      else *Ecor = 1000./fabs(qp);
    }
  return 0;
}


void CbmKFMath::CopyTC2TrackParam(FairTrackParam* par, Double_t T[], Double_t C[] )
{
  if( T )
    {
      par->SetX (T[0]);                
      par->SetY (T[1]);
      par->SetZ (T[5]);                
      par->SetTx(T[2]);
      par->SetTy(T[3]);                
      par->SetQp(T[4]);
    }
  if( C )
    {
      for (Int_t i=0,iCov=0; i<5; i++) 
	for (Int_t j=0; j<=i; j++,iCov++)
	  par->SetCovariance(i,j,C[iCov]);
    }
}

void CbmKFMath::CopyTrackParam2TC(FairTrackParam* par, Double_t T[], Double_t C[] )
{
  if( T )
    {  
      T[0]=par->GetX();
      T[1]=par->GetY();
      T[2]=par->GetTx();
      T[3]=par->GetTy();
      T[4]=par->GetQp();
      T[5]=par->GetZ();
    }
  if( C )
    {
      for (Int_t i=0,iCov=0; i<5; i++) 
	for (Int_t j=0; j<=i; j++,iCov++)
	  C[iCov] = par->GetCovariance(i,j);
    }
}
