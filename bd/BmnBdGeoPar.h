
#ifndef BMNGEOBDPAR_H
#define BMNGEOBDPAR_H

#include "FairParGenericSet.h"
#include "TObjArray.h"

class BmnBdGeoPar : public FairParGenericSet {
public:
  TObjArray      *fGeoSensNodes; /** List of FairGeoNodes for sensitive  volumes */
  TObjArray      *fGeoPassNodes; /** List of FairGeoNodes for passive  volumes */
  
  BmnBdGeoPar(const char* name="BmnBdGeoPar",
	     const char* title="BD Geometry Parameters",
             const char* context="BdDefaultContext");
  ~BmnBdGeoPar(void);
  void clear(void);
  void putParams(FairParamList*);
  Bool_t getParams(FairParamList*);
  TObjArray             *GetGeoSensitiveNodes(){return fGeoSensNodes;}
  TObjArray             *GetGeoPassiveNodes(){return fGeoPassNodes;}

 
  
  ClassDef(BmnBdGeoPar,1)
};

#endif /* !BMNGEOBDPAR_H */
