/** Class for Pixel-type measurement calculations for CbmKFHit hits 
 *
 * @author  S.Gorbunov
 * @version 0.0
 * @since   02.12.05
 * 
 * Description will be here 
 *
 */

#ifndef CBMKFPIXELMEASUREMENT_H
#define CBMKFPIXELMEASUREMENT_H 1

#include "CbmKFTrackInterface.h"

#include <vector>

class CbmKFPixelMeasurement :public TObject{
 public:
  CbmKFPixelMeasurement():z(0),x(0),y(0){}
  ~CbmKFPixelMeasurement(){}

  Double_t z;
  Double_t x;
  Double_t y;
  Double_t V[3];
  Int_t Filter( CbmKFTrackInterface &track );
  static void FilterPDAF(  CbmKFTrackInterface &track, 
			   std::vector<CbmKFPixelMeasurement*> &vm,
			   double gateX, double gateY, 
			   std::vector<double> &vProb              );

  ClassDef(CbmKFPixelMeasurement, 1)
};

#endif /* !CBMKFPIXELMEASUREMENT_H */
