#ifndef BMNDCHGEO_H
#define BMNDCHGEO_H

#include "FairGeoSet.h"

class  BmnDchGeo : public FairGeoSet {

protected:
  char modName[20];  // name of module
  char eleName[20];  // substring for elements in module

public:
  BmnDchGeo();
  ~BmnDchGeo() {}
  const char* getModuleName(Int_t);
  const char* getEleName(Int_t);

  ClassDef(BmnDchGeo,0) // Class for DCH
};

#endif  /* !BMNDCHGEO_H */
