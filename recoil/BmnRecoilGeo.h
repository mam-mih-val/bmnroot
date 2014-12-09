/*************************************************************************************
 *
 *         Class BmnRecoilGeo
 *         
 *  Adopted for BMN by:   Elena Litvinenko
 *  e-mail:   litvin@nf.jinr.ru
 *  Version:  24-03-2014   
 *
 ************************************************************************************/

#ifndef BMNGEORECOIL_H
#define BMNGEORECOIL_H

#include "FairGeoSet.h"

class  BmnRecoilGeo : public FairGeoSet {
protected:
  char modName[20];  // name of module
  char eleName[20];  // substring for elements in module
  
public:
  BmnRecoilGeo();
  ~BmnRecoilGeo() {}
  const char* getModuleName(Int_t);
  const char* getEleName(Int_t);
  
  ClassDef(BmnRecoilGeo,0) // Class for Hyp
};

#endif  /* !BMNGEORECOIL_H */



