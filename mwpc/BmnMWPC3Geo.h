#ifndef BMNMWPC3GEO_H
#define BMNMWPC3GEO_H

#include "FairGeoSet.h"

class  BmnMWPC3Geo : public FairGeoSet {

protected:
  char modName[20];  // name of module
  char eleName[20];  // substring for elements in module

public:
  BmnMWPC3Geo();
  ~BmnMWPC3Geo() {}
  const char* getModuleName(Int_t);
  const char* getEleName(Int_t);

  ClassDef(BmnMWPC3Geo,0) // Class for MWPC3
};

#endif  /* !BMNMWPC3GEO_H */
