/*************************************************************************************
 *
 *         Class BmnZdcGeoPar
 *         
 *  Adopted for BMN by:   Elena Litvinenko
 *  e-mail:   litvin@nf.jinr.ru
 *  Version:  06-11-2015   
 *
 ************************************************************************************/

#ifndef BMNGEOZDCPAR_H
#define BMNGEOZDCPAR_H

#include "FairParGenericSet.h"
#include "TObjArray.h"

class BmnZdcGeoPar : public FairParGenericSet {
public:
  TObjArray      *fGeoSensNodes; /** List of FairGeoNodes for sensitive  volumes */
  TObjArray      *fGeoPassNodes; /** List of FairGeoNodes for passive  volumes */
  
  BmnZdcGeoPar(const char* name="BmnZdcGeoPar",
	     const char* title="Zdc Geometry Parameters",
             const char* context="ZdcDefaultContext");
  ~BmnZdcGeoPar(void);
  void clear(void);
  void putParams(FairParamList*);
  Bool_t getParams(FairParamList*);
  TObjArray             *GetGeoSensitiveNodes(){return fGeoSensNodes;}
  TObjArray             *GetGeoPassiveNodes(){return fGeoPassNodes;}

 
  
  ClassDef(BmnZdcGeoPar,1)
};

#endif /* !BMNGEOZDCPAR_H */
