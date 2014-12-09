#ifndef BMNDCH1GEOPAR_H
#define BMNDCH1GEOPAR_H

#include "FairParGenericSet.h"
#include "TH1F.h"
#include "TObjArray.h"

class BmnDch1GeoPar : public FairParGenericSet {
public:
  TObjArray            *fGeoSensNodes; // List of FairGeoNodes for sensitive volumes
  TObjArray            *fGeoPassNodes; // List of FairGeoNodes for sensitive volumes

  BmnDch1GeoPar(const char* name="BmnDch1GeoPar",
             const char* title="DCH1 Geometry Parameters",
             const char* context="TestDefaultContext");
  ~BmnDch1GeoPar(void);
  void clear(void);
  void putParams(FairParamList*);
  Bool_t getParams(FairParamList*);
  TObjArray             *GetGeoSensitiveNodes(){return fGeoSensNodes;}
  TObjArray             *GetGeoPassiveNodes(){return fGeoPassNodes;}

  ClassDef(BmnDch1GeoPar,1)
};

#endif /* !BMNDCH1GEOPAR_H */
