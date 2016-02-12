/*************************************************************************************
 *
 *         Class BmnEcalGeo
 *         
 *  Adopted for BMN by:   Elena Litvinenko
 *  e-mail:   litvin@nf.jinr.ru
 *  Version:  10-02-2016   
 *
 ************************************************************************************/

#ifndef BMNGEOECAL_H
#define BMNGEOECAL_H

#include "FairGeoSet.h"

class  BmnEcalGeo : public FairGeoSet {
protected:
  char modName[20];  // name of module
  char eleName[20];  // substring for elements in module
  
public:
  BmnEcalGeo();
  ~BmnEcalGeo() {}
  const char* getModuleName(Int_t);
  const char* getEleName(Int_t);
  
  ClassDef(BmnEcalGeo,0) // Class for Hyp
};

#endif  /* !BMNGEOECAL_H */



