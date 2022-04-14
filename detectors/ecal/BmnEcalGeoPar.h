/*************************************************************************************
 *
 *         Class BmnEcalGeoPar
 *         
 *  Adopted for BMN by:   Elena Litvinenko
 *  e-mail:   litvin@nf.jinr.ru
 *  Version:  10-02-2016   
 *
 ************************************************************************************/

#ifndef BMNGEOECALPAR_H
#define BMNGEOECALPAR_H

#include "FairParGenericSet.h"
#include "TObjArray.h"

class BmnEcalGeoPar : public FairParGenericSet {
public:
  TObjArray      *fGeoSensNodes; /** List of FairGeoNodes for sensitive  volumes */
  TObjArray      *fGeoPassNodes; /** List of FairGeoNodes for passive  volumes */
  
  BmnEcalGeoPar(const char* name="BmnEcalGeoPar",
	     const char* title="Ecal Geometry Parameters",
             const char* context="EcalDefaultContext");
  ~BmnEcalGeoPar(void);
  void clear(void);
  void putParams(FairParamList*);
  Bool_t getParams(FairParamList*);
  TObjArray             *GetGeoSensitiveNodes(){return fGeoSensNodes;}
  TObjArray             *GetGeoPassiveNodes(){return fGeoPassNodes;}

 
  
  ClassDef(BmnEcalGeoPar,1)
};

#endif /* !BMNGEOECALPAR_H */
