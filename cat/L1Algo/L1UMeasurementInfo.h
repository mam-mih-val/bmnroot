#ifndef L1UMeasurementInfo_h
#define L1UMeasurementInfo_h 1

#include "CbmL1Def.h"


class L1UMeasurementInfo{
  
 public:
   L1UMeasurementInfo():
       cos_phi(0), sin_phi(0), sigma2(0)
   {}
       
  fvec cos_phi, sin_phi, sigma2;
  
} _fvecalignment;


#endif
