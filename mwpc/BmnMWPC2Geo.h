#ifndef BMNMWPC2GEO_H
#define BMNMWPC2GEO_H

#include "FairGeoSet.h"

class  BmnMWPC2Geo : public FairGeoSet {

protected:
  char modName[20];  // name of module
  char eleName[20];  // substring for elements in module

public:
  BmnMWPC2Geo();
  ~BmnMWPC2Geo() {}
  const char* getModuleName(Int_t);
  const char* getEleName(Int_t);

  ClassDef(BmnMWPC2Geo,0) // Class for MWPC2
};

#endif  /* !BMNMWPC2GEO_H */
