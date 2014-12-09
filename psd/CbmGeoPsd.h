#ifndef CBMGEOPSD_H
#define CBMGEOPSD_H

#include "FairGeoSet.h"

class  CbmGeoPsd : public FairGeoSet {
protected:
  char modName[20];  // name of module
  char eleName[20];  // substring for elements in module
public:
  CbmGeoPsd();
  ~CbmGeoPsd() {}
  const char* getModuleName(Int_t);
  const char* getEleName(Int_t);
  inline Int_t getModNumInMod(const TString&);
  ClassDef(CbmGeoPsd,0) // Class for Trd
};

inline Int_t CbmGeoPsd::getModNumInMod(const TString& name) {
  // returns the module index from module name
  return (Int_t)(name[3]-'0')-1;
}

#endif  /* !CBMGEOECAL_H */
