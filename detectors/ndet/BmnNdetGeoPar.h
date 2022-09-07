/*************************************************************************************
 *
 *         Class BmnNdetGeoPar
 *         
 *  Adopted for BMN by:   Elena Litvinenko
 *  e-mail:   litvin@nf.jinr.ru
 *  Version:  06-11-2015   
 *  Modified by M.Golubeva July 2022  
 *
 ************************************************************************************/

#ifndef BMNGEONDETPAR_H
#define BMNGEONDETPAR_H

#include "FairParGenericSet.h"
#include "TObjArray.h"

class BmnNdetGeoPar : public FairParGenericSet {
public:
  TObjArray      *fGeoSensNodes; /** List of FairGeoNodes for sensitive  volumes */
  TObjArray      *fGeoPassNodes; /** List of FairGeoNodes for passive  volumes */
  
  BmnNdetGeoPar(const char* name="BmnNdetGeoPar",
	     const char* title="Ndet Geometry Parameters",
             const char* context="NdetDefaultContext");
  ~BmnNdetGeoPar(void);
  void clear(void);
  void putParams(FairParamList*);
  Bool_t getParams(FairParamList*);
  TObjArray             *GetGeoSensitiveNodes(){return fGeoSensNodes;}
  TObjArray             *GetGeoPassiveNodes(){return fGeoPassNodes;}

 
  
  ClassDef(BmnNdetGeoPar,1)
};

#endif /* !BMNGEONDETPAR_H */
