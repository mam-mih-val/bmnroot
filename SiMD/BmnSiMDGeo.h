/*************************************************************************************
 *
 *         Class BmnFdGeo
 *         
 ************************************************************************************/

#ifndef BMNGEOSiMD_H
#define BMNGEOSiMD_H

#include "FairGeoSet.h"

class  BmnSiMDGeo : public FairGeoSet {
protected:
  char mod1Name[100];  // name of module
  char ele1Name[100];  // substring for elements in module
  
public:
  BmnSiMDGeo();
  ~BmnSiMDGeo() {}
  const char* getModule1Name(Int_t);
  const char* getEle1Name(Int_t);
  
  ClassDef(BmnSiMDGeo,0) // Class for Hyp
};

#endif  /* !BMNGEOSiMD_H */



