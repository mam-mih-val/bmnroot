#ifndef BMNBDGEO_H
#define BMNBDGEO_H

#include "FairGeoSet.h"

class BmnBdGeo : public FairGeoSet {
protected:
  char mod2Name[100];  // name of module
  char ele2Name[100];  // substring for elements in module
  
public:
  BmnBdGeo();
  ~BmnBdGeo() {}
  const char* getModule2Name(Int_t);
  const char* getEle2Name(Int_t);
  
  ClassDef(BmnBdGeo,0)
};

#endif  /* !BMNBDGEO_H */



