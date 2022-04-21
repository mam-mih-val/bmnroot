/** Collection of mathematical routines for track extrapolation in magnetic field
 *
 * @author  S.Gorbunov
 * @version 0.0
 * @since   02.12.05
 * 
 * Description will be here 
 *
 */

#ifndef CBMKFFIELDMATH_H
#define CBMKFFIELDMATH_H 1

#include "TObject.h"

class FairField;
class FairTrackParam;

class CbmKFFieldMath :public TObject
{
 public:
  CbmKFFieldMath(){}
  ~CbmKFFieldMath(){}
  
  static void ExtrapolateLine( const Double_t T_in[], const Double_t C_in[], 
			       Double_t T_out[], Double_t C_out[],
			       Double_t z_out
			       );
  
  static Int_t ExtrapolateRK4( const Double_t T_in[], const Double_t C_in[],
			       Double_t T_out[], Double_t C_out[],
			       Double_t z_out , Double_t qp0, FairField *MF );
  /*
  static void ExtrapolateAnalytic( const Double_t T_in[], const Double_t C_in[],
				   Double_t T_out[], Double_t C_out[],
				   Double_t z_out, Double_t qp0, CbmMagField *MF, 
				   Int_t order=3 );
  
  static void ExtrapolateACentral( const Double_t T_in[], const Double_t C_in[],
				   Double_t T_out[], Double_t C_out[],
				   Double_t z_out, Double_t qp0, CbmMagField *MF);
  */
  static Int_t ExtrapolateALight( const Double_t T_in[], const Double_t C_in[],
				 Double_t T_out[], Double_t C_out[],
				 Double_t z_out, Double_t qp0, FairField *MF);
  /*
  static void IntegrateField( 
			     CbmMagField *MF,      
			     Double_t r0[], Double_t r1[], Double_t r2[],
			     Double_t si  [3]      =0, Double_t Si  [3]      =0,
			     Double_t sii [3][3]   =0, Double_t Sii [3][3]   =0, 
			     Double_t siii[3][3][3]=0, Double_t Siii[3][3][3]=0
			     );
  
  static void GetCoefficients( 
			      Double_t tx, Double_t ty,
			      Double_t Xi  [3][3]      =0, Double_t Yi  [3][3]      =0,
			      Double_t Xii [3][3][3]   =0, Double_t Yii [3][3][3]   =0,
			      Double_t Xiii[3][3][3][3]=0, Double_t Yiii[3][3][3][3]=0
			      );
  */ 
  ClassDef(CbmKFFieldMath, 1)
};

#endif /* ! CBMKFFIELDMATH_H */
