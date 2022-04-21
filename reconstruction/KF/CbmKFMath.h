/** Collection of mathematical routines for the CbmKF Kalman Filter 
 *
 * @author  S.Gorbunov
 * @version 0.0
 * @since   02.12.05
 * 
 * Description will be here 
 *
 */

#ifndef CBMKFMATH_H
#define CBMKFMATH_H 1

#include "TObject.h"

class FairField;
class FairTrackParam;

class CbmKFMath :public TObject
{
 public:
  CbmKFMath(){}
  ~CbmKFMath(){}

  static Bool_t GetThickness( Double_t z1, Double_t z2, Double_t mz, Double_t mthick, 
			    Double_t *mz_out, Double_t *mthick_out );

  static Bool_t intersectCone( Double_t zCone, Double_t ZCone, Double_t rCone, Double_t RCone ,
			     const Double_t x[], 
			     Double_t *z1, Double_t *z2
			       );

  static Int_t indexS( Int_t i, Int_t j )
  {
    return ( j<=i ) ? i*(i+1)/2+j :j*(j+1)/2+i;
  }
  
  static void multQSQt( Int_t N, const Double_t Q[], const Double_t S[], Double_t S_out[] );
  static void multQtSQ( Int_t N, const Double_t Q[], const Double_t S[], Double_t S_out[] );
  static void multSSQ( const Double_t *A, const Double_t *B, Double_t *C, Int_t n );
  static void four_dim_inv( Double_t a[4][4] );
  static void five_dim_inv(Double_t a[5][5]);
  static Bool_t invS( Double_t A[], Int_t N );
  static Double_t getDeviation( Double_t x, Double_t y, Double_t C[], Double_t vx, Double_t vy, Double_t Cv[]=0 ); 
  static Double_t AnalyticQP(
			     const Double_t T[], // track parameters (x,y,tx,ty,Q/p,z)
			     const Double_t V[], // vertex parameters (x,y,z)
			     FairField *MagneticField       // magnetic field
			     );

  static Int_t GetNoise( Double_t Lrl, Double_t F, Double_t Fe,
			 Double_t tx, Double_t ty, Double_t qp, 
			 Double_t mass, Bool_t is_electron, 
			 Bool_t downstream_direction, 
			 Double_t *Q5, Double_t *Q8, Double_t *Q9, Double_t *Ecor );

  static void CopyTrackParam2TC( FairTrackParam* par, Double_t T[], Double_t C[] );
  static void CopyTC2TrackParam( FairTrackParam* par, Double_t T[], Double_t C[] );
  
  ClassDef(CbmKFMath, 1)
};

#endif /* ! CBMKFMATH_H */
