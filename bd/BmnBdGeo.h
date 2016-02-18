/*************************************************************************************
 *
 *         Class BmnBdGeo
 *         
 *  Adopted for BMN by:   Elena Litvinenko
 *  e-mail:   litvin@nf.jinr.ru
 *  Version:  10-02-2016   
 *
 ************************************************************************************/

#ifndef BMNGEOBD_H
#define BMNGEOBD_H

#include "FairGeoSet.h"

class  BmnBdGeo : public FairGeoSet {
protected:
  char modName[20];  // name of module
  char eleName[20];  // substring for elements in module
  
public:
  BmnBdGeo();
  ~BmnBdGeo() {}
  const char* getModuleName(Int_t);
  const char* getEleName(Int_t);
  
  ClassDef(BmnBdGeo,0) // Class for Hyp
};

#endif  /* !BMNGEOBD_H */



