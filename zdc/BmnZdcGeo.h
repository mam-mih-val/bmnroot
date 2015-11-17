/*************************************************************************************
 *
 *         Class BmnZdcGeo
 *         
 *  Adopted for BMN by:   Elena Litvinenko
 *  e-mail:   litvin@nf.jinr.ru
 *  Version:  06-11-2015   
 *
 ************************************************************************************/

#ifndef BMNGEOZDC_H
#define BMNGEOZDC_H

#include "FairGeoSet.h"

class  BmnZdcGeo : public FairGeoSet {
protected:
  char modName[20];  // name of module
  char eleName[20];  // substring for elements in module
  
public:
  BmnZdcGeo();
  ~BmnZdcGeo() {}
  const char* getModuleName(Int_t);
  const char* getEleName(Int_t);
  
  ClassDef(BmnZdcGeo,0) // Class for Hyp
};

#endif  /* !BMNGEOZDC_H */



