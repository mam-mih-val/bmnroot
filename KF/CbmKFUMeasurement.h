/** Class for 1-D measurement calculations for CbmKFHit hits 
 *
 * @author  S.Gorbunov
 * @version 0.0
 * @since   02.12.05
 * 
 * Description will be here 
 *
 */

#ifndef CBMKFMEASUREMENT_H
#define CBMKFMEASUREMENT_H 1

#include "CbmKFTrackInterface.h"

class CbmKFUMeasurement :public TObject
{
 public:
  CbmKFUMeasurement():
    z(),
    u(),
    sigma2(),
    phi(),
    phi_s(),
    phi_c(),
    phi_ss(),
    phi_cc(),
    phi_2sc()
  {}
  ~CbmKFUMeasurement(){}

  Double_t z;
  Double_t u;
  Double_t sigma2;
  Double_t phi;

  void Set( Double_t z, Double_t u, Double_t phi, Double_t sigma2 );
  Int_t Filter( CbmKFTrackInterface &track );

  Double_t phi_s;
  Double_t phi_c;
  Double_t phi_ss;
  Double_t phi_cc;
  Double_t phi_2sc;
  
  ClassDef(CbmKFUMeasurement,1)
};

#endif /* !CBMKFUMEASUREMENT_H */
