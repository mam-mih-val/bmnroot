#ifndef BMNDCH2GEO_H
#define BMNDCH2GEO_H

#include "FairGeoSet.h"

class  BmnDch2Geo : public FairGeoSet {

protected:
  char modName[20];  // name of module
  char eleName[20];  // substring for elements in module

public:
  BmnDch2Geo();
  ~BmnDch2Geo() {}
  const char* getModuleName(Int_t);
  const char* getEleName(Int_t);

  ClassDef(BmnDch2Geo,0) // Class for DCH2
};

#endif  /* !BMNDCH2GEO_H */
