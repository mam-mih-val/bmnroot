#ifndef CBMGEOSTS_H
#define CBMGEOSTS_H

#include "FairGeoSet.h"

class  CbmGeoSts : public FairGeoSet {
protected:
  char modName[200];  // name of module
  char eleName[200];  // substring for elements in module
public:
  CbmGeoSts();
  ~CbmGeoSts() {}
  const char* getModuleName(Int_t);
  const char* getEleName(Int_t);
  inline Int_t getModNumInMod(const TString&);
  ClassDef(CbmGeoSts,0) // Class for STS
};

inline Int_t CbmGeoSts::getModNumInMod(const TString& name) {
  // returns the module index from module name
  return (Int_t)(name[3]-'0')-1;
}

#endif  /* !CBMGEOSTS_H */
