#ifndef BMNGEOBCPAR_H
#define BMNGEOBCPAR_H

#include "FairParGenericSet.h"
#include "TObjArray.h"

class BmnBCGeoPar : public FairParGenericSet {
public:
  TObjArray      *fGeoSensNodes; /** List of FairGeoNodes for sensitive  volumes */
  TObjArray      *fGeoPassNodes; /** List of FairGeoNodes for passive  volumes */
  
  BmnBCGeoPar(const char* name="BmnBCGeoPar",
	     const char* title="BC Geometry Parameters",
             const char* context="BCDefaultContext");
  ~BmnBCGeoPar(void);
  void clear(void);
  void putParams(FairParamList*);
  Bool_t getParams(FairParamList*);
  TObjArray             *GetGeoSensitiveNodes(){return fGeoSensNodes;}
  TObjArray             *GetGeoPassiveNodes(){return fGeoPassNodes;}

 
  
  ClassDef(BmnBCGeoPar,1)
};

#endif /* !BMNGEOBCPAR_H */
