#ifndef BMNGEOBC_H
#define BMNGEOBC_H

#include "FairGeoSet.h"

class  BmnBCGeo : public FairGeoSet {
protected:
  char mod2Name[100];  // name of module
  char ele2Name[100];  // substring for elements in module
  
public:
  BmnBCGeo();
  ~BmnBCGeo() {}
  const char* getModule2Name(Int_t);
  const char* getEle2Name(Int_t);
  
  ClassDef(BmnBCGeo,0) // Class for Hyp
};

#endif  /* !BMNGEOBC_H */



