#ifndef BMNSTTGEOPAR_H
#define BMNSTTGEOPAR_H

#include "FairParGenericSet.h"
#include "TH1F.h"
#include "TObjArray.h"

class BmnSttGeoPar : public FairParGenericSet {
public:
  TObjArray            *fGeoSensNodes; // List of FairGeoNodes for sensitive volumes
  TObjArray            *fGeoPassNodes; // List of FairGeoNodes for sensitive volumes

  BmnSttGeoPar(const char* name="BmnSttGeoPar",
             const char* title="Stt Geometry Parameters",
             const char* context="TestDefaultContext");
  ~BmnSttGeoPar(void);
  void clear(void);
  void putParams(FairParamList*);
  Bool_t getParams(FairParamList*);
  TObjArray             *GetGeoSensitiveNodes(){return fGeoSensNodes;}
  TObjArray             *GetGeoPassiveNodes(){return fGeoPassNodes;}

  ClassDef(BmnSttGeoPar,1)
};

#endif /* !BMNSTTGEOPAR_H */
