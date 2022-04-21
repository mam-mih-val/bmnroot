#ifndef L1Station_h
#define L1Station_h 1

#include "L1Field.h"
#include "L1UMeasurementInfo.h"
#include "L1XYMeasurementInfo.h"
#include "L1MaterialInfo.h"

class L1Station{

 public:
  L1Station():
  z(0), Rmin(0), Rmax(0), Sy(0),
  materialInfo(),
  fieldSlice(),
  frontInfo(), backInfo(), xInfo(), yInfo(),
  XYInfo(){}
  
  fvec z, Rmin, Rmax, Sy;
  L1MaterialInfo materialInfo;
  L1FieldSlice fieldSlice;
  L1UMeasurementInfo frontInfo, backInfo, xInfo, yInfo;
  L1XYMeasurementInfo XYInfo;

} _fvecalignment;

#endif
