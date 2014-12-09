/*************************************************************************************
 *
 *         Class BmnRecoilGeoPar
 *         
 *  Adopted for BMN by:   Elena Litvinenko
 *  e-mail:   litvin@nf.jinr.ru
 *  Version:  24-03-2014   
 *
 ************************************************************************************/

#ifndef BMNGEORECOILPAR_H
#define BMNGEORECOILPAR_H

#include "FairParGenericSet.h"
#include "TObjArray.h"

class BmnRecoilGeoPar : public FairParGenericSet {
public:
  TObjArray      *fGeoSensNodes; /** List of FairGeoNodes for sensitive  volumes */
  TObjArray      *fGeoPassNodes; /** List of FairGeoNodes for passive  volumes */
  
  BmnRecoilGeoPar(const char* name="BmnRecoilGeoPar",
	     const char* title="Recoil Geometry Parameters",
             const char* context="RecoilDefaultContext");
  ~BmnRecoilGeoPar(void);
  void clear(void);
  void putParams(FairParamList*);
  Bool_t getParams(FairParamList*);
  TObjArray             *GetGeoSensitiveNodes(){return fGeoSensNodes;}
  TObjArray             *GetGeoPassiveNodes(){return fGeoPassNodes;}

 
  
  ClassDef(BmnRecoilGeoPar,1)
};

#endif /* !BMNGEORECOILPAR_H */
