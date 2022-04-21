/** Base class for material is being used by the CbmKF Kalman Filter 
 *
 * @author  S.Gorbunov
 * @version 0.0
 * @since   02.12.05
 * 
 * Description will be here 
 *
 */

#ifndef CBMKFMATERIAL_H
#define CBMKFMATERIAL_H 1

#include "CbmKFTrackInterface.h"
class TString;

class CbmKFMaterial :public TObject
{
 public:

  CbmKFMaterial():
    ID(-1),
    ZReference(-1),
    ZThickness(-1),
    RadLength(-1),
    F(1),
    Fe(0.02145)
  {}
  virtual ~CbmKFMaterial(){};

  Int_t ID;
  Double_t ZReference ;
  Double_t ZThickness ;
  Double_t RadLength  ;
  Double_t F;
  Double_t Fe;
  virtual Bool_t IsOutside( Double_t x, Double_t y ){ return 0; }
 
  virtual Int_t Pass( Double_t ZCross, Double_t ZThick, 
		      CbmKFTrackInterface &track,		    
		      Bool_t downstream, Double_t &QP0       );
 
  virtual Int_t Pass( CbmKFTrackInterface &track, Bool_t downstream, Double_t &QP0 ){
    return Pass( ZReference,  ZThickness, track, downstream, QP0);
  }
  
  virtual Int_t Pass( CbmKFTrackInterface &track, Bool_t downstream ){
    Double_t QP0 = track.GetTrack()[4];
    return Pass( ZReference,  ZThickness, track, downstream, QP0);
  }

  virtual TString Info() const;

  static Bool_t comparePDown( const CbmKFMaterial *a, const CbmKFMaterial *b ){
    return a->ZReference < b->ZReference;
  }

  static Bool_t compareP_z( const CbmKFMaterial *a, Double_t z ){
    return (a->ZReference + a->ZThickness/2) < z ;
  }
  
  static Bool_t compareP_Z( Double_t z, const CbmKFMaterial *a ){
    return z < (a->ZReference + a->ZThickness/2) ;
  }

  ClassDef(CbmKFMaterial,1)
};

class CbmKFWall: public CbmKFMaterial{
 public:
  CbmKFWall(){ }
  ~CbmKFWall(){ }

  ClassDef(CbmKFWall,1)
};

class CbmKFTube: public CbmKFMaterial{
 public:
  CbmKFTube():x(0),y(0),z(0),dz(0),r(0),R(0),rr(0),RR(0){}
  CbmKFTube(Int_t ID, Double_t x, Double_t y, Double_t z, Double_t dz, Double_t r, Double_t R, Double_t radL);
  ~CbmKFTube(){}

  TString Info() const;

  Double_t x,y,z, dz;
  Double_t r, R, rr, RR;

  Bool_t IsOutside( Double_t X, Double_t Y ){
    X-=x;
    Y-=y;
    Double_t r2 = X*X + Y*Y;
    return ( r2<rr || RR<r2 ) ;
  }
  ClassDef(CbmKFTube, 1)
};

class CbmKFBox: public CbmKFMaterial{
 public:
  CbmKFBox():x(0),y(0),z(0),dx05(0),dy05(0),dz(0){}
  CbmKFBox(Int_t ID, Double_t x, Double_t y, Double_t z, Double_t dx, Double_t dy, Double_t dz, Double_t radL);  
  ~CbmKFBox(){}

  TString Info() const;

  Double_t x,y,z, dx05,dy05,dz;

  Bool_t IsOutside( Double_t X, Double_t Y );

  ClassDef(CbmKFBox, 1)
};

class CbmKFCone :public CbmKFMaterial{
 public:
  CbmKFCone(): z1(0), z2(0), r1(0), R1(0), r2(0), R2(0) {};
  ~CbmKFCone(){}

  Double_t z1, z2;
  Double_t r1, R1, r2, R2;

  Int_t Pass( Double_t ZCross, Double_t ZThick, 
	      CbmKFTrackInterface &track, Bool_t downstream, Double_t &QP0  );  
 
  Int_t Pass( CbmKFTrackInterface &track, Bool_t downstream, Double_t &QP0 ){
    return Pass( ZReference,  ZThickness, track, downstream, QP0);
  }
  
  Int_t Pass( CbmKFTrackInterface &track, Bool_t downstream ){
    Double_t QP0 = track.GetTrack()[4];
    return Pass( ZReference,  ZThickness, track, downstream, QP0);
  }
  ClassDef(CbmKFCone, 1)
};


#endif /* !CBMKFMATERIAL_H */
