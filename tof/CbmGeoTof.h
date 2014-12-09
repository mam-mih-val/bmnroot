#ifndef CBMGEOTOF_H
#define CBMGEOTOF_H

#include "FairGeoSet.h"

class  CbmGeoTof : public FairGeoSet {
protected:
  char modName[20];  // name of module
  char eleName[20];  // substring for elements in module
public:
  CbmGeoTof();
  ~CbmGeoTof() {}
  const char* getModuleName(Int_t);
  const char* getEleName(Int_t);
  inline Int_t getModNumInMod(const TString&);
  ClassDef(CbmGeoTof,0) // Class for Tof
};

inline Int_t CbmGeoTof::getModNumInMod(const TString& name) {
  // returns the module index from module name
  return (Int_t)(name[3]-'0')-1;
}

#endif  /* !CBMGEOTOF_H */
