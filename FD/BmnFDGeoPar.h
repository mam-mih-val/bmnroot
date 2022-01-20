
#ifndef BMNGEOFDPAR_H
#define BMNGEOFDPAR_H

#include "FairParGenericSet.h"
#include "TObjArray.h"

class BmnFDGeoPar : public FairParGenericSet {
public:
  TObjArray      *fGeoSensNodes; /** List of FairGeoNodes for sensitive  volumes */
  TObjArray      *fGeoPassNodes; /** List of FairGeoNodes for passive  volumes */
  
  BmnFDGeoPar(const char* name="BmnFDGeoPar",
	     const char* title="FD Geometry Parameters",
             const char* context="FDDefaultContext");
  ~BmnFDGeoPar(void);
  void clear(void);
  void putParams(FairParamList*);
  Bool_t getParams(FairParamList*);
  TObjArray             *GetGeoSensitiveNodes(){return fGeoSensNodes;}
  TObjArray             *GetGeoPassiveNodes(){return fGeoPassNodes;}

 
  
  ClassDef(BmnFDGeoPar,1)
};

#endif /* !BMNGEOFDPAR_H */
