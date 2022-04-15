/*************************************************************************************
 *
 *         Class BmnFdGeoPar
 *         
 ************************************************************************************/

#ifndef BMNGEOSiMDPAR_H
#define BMNGEOSiMDPAR_H

#include "FairParGenericSet.h"
#include "TObjArray.h"

class BmnSiMDGeoPar : public FairParGenericSet {
public:
  TObjArray      *fGeoSensNodes; /** List of FairGeoNodes for sensitive  volumes */
  TObjArray      *fGeoPassNodes; /** List of FairGeoNodes for passive  volumes */
  
  BmnSiMDGeoPar(const char* name="BmnSiMDGeoPar",
	     const char* title="SiMD Geometry Parameters",
             const char* context="SiMDDefaultContext");
  ~BmnSiMDGeoPar(void);
  void clear(void);
  void putParams(FairParamList*);
  Bool_t getParams(FairParamList*);
  TObjArray             *GetGeoSensitiveNodes(){return fGeoSensNodes;}
  TObjArray             *GetGeoPassiveNodes(){return fGeoPassNodes;}

 
  
  ClassDef(BmnSiMDGeoPar,1)
};

#endif /* !BMNGEOSiMDPAR_H */
