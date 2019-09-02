/*************************************************************************************
 *
 *         Class BmnFdGeo
 *         
 ************************************************************************************/

#ifndef BMNGEOFD_H
#define BMNGEOFD_H

#include "FairGeoSet.h"

class  BmnFDGeo : public FairGeoSet {
protected:
  char mod1Name[100];  // name of module
  char ele1Name[100];  // substring for elements in module
  
public:
  BmnFDGeo();
  ~BmnFDGeo() {}
  const char* getModule1Name(Int_t);
  const char* getEle1Name(Int_t);
  
  ClassDef(BmnFDGeo,0) // Class for Hyp
};

#endif  /* !BMNGEOFD_H */



