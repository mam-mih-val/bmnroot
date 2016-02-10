/*************************************************************************************
 *
 *         Class BmnBarrelGeo
 *         
 *  Adopted for BMN by:   Elena Litvinenko
 *  e-mail:   litvin@nf.jinr.ru
 *  Version:  10-02-2016   
 *
 ************************************************************************************/

#ifndef BMNGEOBARREL_H
#define BMNGEOBARREL_H

#include "FairGeoSet.h"

class  BmnBarrelGeo : public FairGeoSet {
protected:
  char modName[20];  // name of module
  char eleName[20];  // substring for elements in module
  
public:
  BmnBarrelGeo();
  ~BmnBarrelGeo() {}
  const char* getModuleName(Int_t);
  const char* getEleName(Int_t);
  
  ClassDef(BmnBarrelGeo,0) // Class for Hyp
};

#endif  /* !BMNGEOBARREL_H */



