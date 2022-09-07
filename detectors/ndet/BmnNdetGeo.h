/*************************************************************************************
 *
 *         Class BmnNdetGeo
 *         
 *  Adopted for BMN by:   Elena Litvinenko
 *  e-mail:   litvin@nf.jinr.ru
 *  Version:  06-11-2015   
 *  Modified by M.Golubeva July 2022  
 *
 ************************************************************************************/

#ifndef BMNGEONDET_H
#define BMNGEONDET_H

#include "FairGeoSet.h"

class  BmnNdetGeo : public FairGeoSet {
protected:
  char modName[20];  // name of module
  char eleName[20];  // substring for elements in module
  
public:
  BmnNdetGeo();
  ~BmnNdetGeo() {}
  const char* getModuleName(Int_t);
  const char* getEleName(Int_t);
  
  ClassDef(BmnNdetGeo,0) // Class for Hyp
};

#endif  /* !BMNGEONDET_H */



