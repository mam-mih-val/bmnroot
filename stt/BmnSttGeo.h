#ifndef BMNSTTGEO_H
#define BMNSTTGEO_H

#include "FairGeoSet.h"

class  BmnSttGeo : public FairGeoSet {

protected:
  char modName[20];  // name of module
  char eleName[20];  // substring for elements in module

public:
  BmnSttGeo();
  ~BmnSttGeo() {}
  const char* getModuleName(Int_t);
  const char* getEleName(Int_t);

  ClassDef(BmnSttGeo,0) // Class for Stt
};

#endif  /* !BMNSTTGEO_H */
