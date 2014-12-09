#ifndef L1Field_h
#define L1Field_h 1

#include "CbmL1Def.h"
#include <iostream>
using std::cout;
using std::ostream;
using std::endl;


class L1FieldValue{


  public:
    L1FieldValue():x(0),y(0),z(0){};
        
    fvec x, y, z;

    void Combine( L1FieldValue &B, fvec w )
    {
      x+= w*( B.x - x );
      y+= w*( B.y - y );
      z+= w*( B.z - z );
    }

    friend ostream& operator<<(ostream& out, L1FieldValue &B){
      return out << B.x[0] << " | " << B.y[0] << " | " << B.z[0];
    };
} _fvecalignment;



class L1FieldSlice{

  public:

    fvec cx[21], cy[21], cz[21]; // polinom coeff.

    L1FieldSlice()
    {
      for( int i=0; i<21; i++ ) cx[i] = cy[i] = cz[i] = 0;
    }

    void GetFieldValue( const fvec &x, const fvec &y, L1FieldValue &B ) const
    {
      fvec x2 = x*x;
      fvec y2 = y*y;
      fvec xy = x*y;

      fvec x3 = x2*x;
      fvec y3 = y2*y;
      fvec xy2 = x*y2;
      fvec x2y = x2*y;

      fvec x4 = x3*x;
      fvec y4 = y3*y;
      fvec xy3 = x*y3;
      fvec x2y2 = x2*y2;
      fvec x3y = x3*y;

      fvec x5 = x4*x;
      fvec y5 = y4*y;
      fvec xy4 = x*y4;
      fvec x2y3 = x2*y3;
      fvec x3y2 = x3*y2;
      fvec x4y = x4*y;
      
      B.x = cx[0] +cx[1]*x +cx[2]*y +cx[3]*x2 +cx[4]*xy +cx[5]*y2 +cx[6]*x3 +cx[7]*x2y +cx[8]*xy2 +cx[9]*y3
          +cx[10]*x4 +cx[11]*x3y +cx[12]*x2y2 +cx[13]*xy3 +cx[14]*y4
          +cx[15]*x5 +cx[16]*x4y +cx[17]*x3y2 +cx[18]*x2y3 +cx[19]*xy4 +cx[20]*y5;

      B.y = cy[0] +cy[1]*x +cy[2]*y +cy[3]*x2 +cy[4]*xy +cy[5]*y2 +cy[6]*x3 +cy[7]*x2y +cy[8]*xy2 +cy[9]*y3
          +cy[10]*x4 +cy[11]*x3y +cy[12]*x2y2 +cy[13]*xy3 +cy[14]*y4
          +cy[15]*x5 +cy[16]*x4y +cy[17]*x3y2 +cy[18]*x2y3 +cy[19]*xy4 +cy[20]*y5;

      B.z = cz[0] +cz[1]*x +cz[2]*y +cz[3]*x2 +cz[4]*xy +cz[5]*y2 +cz[6]*x3 +cz[7]*x2y +cz[8]*xy2 +cz[9]*y3
          +cz[10]*x4 +cz[11]*x3y +cz[12]*x2y2 +cz[13]*xy3 +cz[14]*y4
          +cz[15]*x5 +cz[16]*x4y +cz[17]*x3y2 +cz[18]*x2y3 +cz[19]*xy4 +cz[20]*y5;
    }
} _fvecalignment;



class L1FieldRegion{

  public:
    L1FieldRegion():
      cx0(0), cx1(0), cx2(0),
      cy0(0), cy1(0), cy2(0),
      cz0(0), cz1(0), cz2(0),
      z0(0)
    {}

    L1FieldRegion(float reg[10]):
      cx0(reg[0]), cx1(reg[1]), cx2(reg[2]),
      cy0(reg[3]), cy1(reg[4]), cy2(reg[5]),
      cz0(reg[6]), cz1(reg[7]), cz2(reg[8]),
      z0(reg[9])
    {}

    fvec cx0, cx1, cx2 ; // Bx(z) = cx0 + cx1*(z-z0) + cx2*(z-z0)^2
    fvec cy0, cy1, cy2 ; // By(z) = cy0 + cy1*(z-z0) + cy2*(z-z0)^2
    fvec cz0, cz1, cz2 ; // Bz(z) = cz0 + cz1*(z-z0) + cz2*(z-z0)^2
    fvec z0;

    L1FieldValue Get(const fvec z){
      fvec dz = (z-z0);
      fvec dz2 = dz*dz;
      L1FieldValue B;
      B.x = cx0 + cx1*dz + cx2*dz2;
      B.y = cy0 + cy1*dz + cy2*dz2;
      B.z = cz0 + cz1*dz + cz2*dz2;
      return B;
    }
    
    void Set( const L1FieldValue &B0, const fvec B0z,
              const L1FieldValue &B1, const fvec B1z,
              const L1FieldValue &B2, const fvec B2z )
    {
      z0 = B0z;
      fvec dz1 = B1z-B0z, dz2 = B2z-B0z;
      fvec det = rcp(fvec(dz1*dz2*(dz2-dz1)));
      fvec w21 = -dz2*det;
      fvec w22 = dz1*det;
      fvec w11 = -dz2*w21;
      fvec w12 = -dz1*w22;
      
      fvec dB1 = B1.x - B0.x;
      fvec dB2 = B2.x - B0.x;
      cx0 = B0.x;
      cx1 = dB1*w11 + dB2*w12 ;
      cx2 = dB1*w21 + dB2*w22 ;
      
      dB1 = B1.y - B0.y;
      dB2 = B2.y - B0.y;
      cy0 = B0.y;
      cy1 = dB1*w11 + dB2*w12 ;
      cy2 = dB1*w21 + dB2*w22  ;

      dB1 = B1.z - B0.z;
      dB2 = B2.z - B0.z;
      cz0 = B0.z;
      cz1 = dB1*w11 + dB2*w12 ;
      cz2 = dB1*w21 + dB2*w22   ;
    }
  
    void Set( const L1FieldValue &B0, const fvec B0z,
              const L1FieldValue &B1, const fvec B1z )
    {
      z0 = B0z[0];
      fvec dzi = rcp(fvec( B1z - B0z));
      cx0 = B0.x;
      cy0 = B0.y;
      cz0 = B0.z;
      cx1 = ( B1.x - B0.x )*dzi;
      cy1 = ( B1.y - B0.y )*dzi;
      cz1 = ( B1.z - B0.z )*dzi;
      cx2 = cy2 = cz2 = 0;
    }

    void Shift( fvec z)
    {
      fvec dz = z-z0;
      fvec cx2dz = cx2*dz;
      fvec cy2dz = cy2*dz;
      fvec cz2dz = cz2*dz;
      z0 = z[0];
      cx0+= ( cx1 + cx2dz )*dz;
      cy0+= ( cy1 + cy2dz )*dz;
      cz0+= ( cz1 + cz2dz )*dz;
      cx1+= cx2dz + cx2dz;
      cy1+= cy2dz + cy2dz;
      cz1+= cz2dz + cz2dz;
    }

    void SetOneEntry( const int i0, const L1FieldRegion &f1, const int i1 )
    {
      cx0[i0] = f1.cx0[i1];
      cx1[i0] = f1.cx1[i1];
      cx2[i0] = f1.cx2[i1];
      cy0[i0] = f1.cy0[i1];
      cy1[i0] = f1.cy1[i1];
      cy2[i0] = f1.cy2[i1];
      cz0[i0] = f1.cz0[i1];
      cz1[i0] = f1.cz1[i1];
      cz2[i0] = f1.cz2[i1];
      z0[i0] = f1.z0[i1];
    }

    void SetOneEntry( const L1FieldRegion &f1, const int i1 )
    {
      cx0 = f1.cx0[i1];
      cx1 = f1.cx1[i1];
      cx2 = f1.cx2[i1];
      cy0 = f1.cy0[i1];
      cy1 = f1.cy1[i1];
      cy2 = f1.cy2[i1];
      cz0 = f1.cz0[i1];
      cz1 = f1.cz1[i1];
      cz2 = f1.cz2[i1];
      z0 = f1.z0[i1];
    }

    void GetOneEntry(float reg[10], const int iVec)
    {
      reg[0] = cx0[iVec];
      reg[1] = cx1[iVec];
      reg[2] = cx2[iVec];
      reg[3] = cy0[iVec];
      reg[4] = cy1[iVec];
      reg[5] = cy2[iVec];
      reg[6] = cz0[iVec];
      reg[7] = cz1[iVec];
      reg[8] = cz2[iVec];
      reg[9] = z0[iVec];
    }
}  _fvecalignment;


#endif
