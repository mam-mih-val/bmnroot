#ifndef BMNMWPC1GEO_H
#define BMNMWPC1GEO_H

#include "FairGeoSet.h"

class  BmnMWPC1Geo : public FairGeoSet {

protected:
  char modName[20];  // name of module
  char eleName[20];  // substring for elements in module

public:
  BmnMWPC1Geo();
  ~BmnMWPC1Geo() {}
  const char* getModuleName(Int_t);
  const char* getEleName(Int_t);

  ClassDef(BmnMWPC1Geo,0) // Class for MWPC1
};

#endif  /* !BMNMWPC1GEO_H */
