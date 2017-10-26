#ifndef BMNMWPCGEO_H
#define BMNMWPCGEO_H

#include "FairGeoSet.h"

class  BmnMwpcGeo : public FairGeoSet {

protected:
  char modName[20];  // name of module
  char eleName[20];  // substring for elements in module

public:
  BmnMwpcGeo();
  ~BmnMwpcGeo() {}
  const char* getModuleName(Int_t);
  const char* getEleName(Int_t);

  ClassDef(BmnMwpcGeo,0) // Class for MWPC
};

#endif  /* !BMNMWPC1GEO_H */
