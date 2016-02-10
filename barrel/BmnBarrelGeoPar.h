/*************************************************************************************
 *
 *         Class BmnBarrelGeoPar
 *         
 *  Adopted for BMN by:   Elena Litvinenko
 *  e-mail:   litvin@nf.jinr.ru
 *  Version:  10-02-2016   
 *
 ************************************************************************************/

#ifndef BMNGEOBARRELPAR_H
#define BMNGEOBARRELPAR_H

#include "FairParGenericSet.h"
#include "TObjArray.h"

class BmnBarrelGeoPar : public FairParGenericSet {
public:
  TObjArray      *fGeoSensNodes; /** List of FairGeoNodes for sensitive  volumes */
  TObjArray      *fGeoPassNodes; /** List of FairGeoNodes for passive  volumes */
  
  BmnBarrelGeoPar(const char* name="BmnBarrelGeoPar",
	     const char* title="Barrel Geometry Parameters",
             const char* context="BarrelDefaultContext");
  ~BmnBarrelGeoPar(void);
  void clear(void);
  void putParams(FairParamList*);
  Bool_t getParams(FairParamList*);
  TObjArray             *GetGeoSensitiveNodes(){return fGeoSensNodes;}
  TObjArray             *GetGeoPassiveNodes(){return fGeoPassNodes;}

 
  
  ClassDef(BmnBarrelGeoPar,1)
};

#endif /* !BMNGEOBARRELPAR_H */
