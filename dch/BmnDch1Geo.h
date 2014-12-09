#ifndef BMNDCH1GEO_H
#define BMNDCH1GEO_H

#include "FairGeoSet.h"

class  BmnDch1Geo : public FairGeoSet {

protected:
  char modName[20];  // name of module
  char eleName[20];  // substring for elements in module

public:
  BmnDch1Geo();
  ~BmnDch1Geo() {}
  const char* getModuleName(Int_t);
  const char* getEleName(Int_t);

  ClassDef(BmnDch1Geo,0) // Class for DCH1
};

#endif  /* !BMNDCH1GEO_H */
