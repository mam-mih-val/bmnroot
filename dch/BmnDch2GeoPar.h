#ifndef BMNDCH2GEOPAR_H
#define BMNDCH2GEOPAR_H

#include "FairParGenericSet.h"
#include "TH1F.h"
#include "TObjArray.h"

class BmnDch2GeoPar : public FairParGenericSet {
public:
  TObjArray            *fGeoSensNodes; // List of FairGeoNodes for sensitive volumes
  TObjArray            *fGeoPassNodes; // List of FairGeoNodes for sensitive volumes

  BmnDch2GeoPar(const char* name="BmnDch2GeoPar",
             const char* title="DCH2 Geometry Parameters",
             const char* context="TestDefaultContext");
  ~BmnDch2GeoPar(void);
  void clear(void);
  void putParams(FairParamList*);
  Bool_t getParams(FairParamList*);
  TObjArray             *GetGeoSensitiveNodes(){return fGeoSensNodes;}
  TObjArray             *GetGeoPassiveNodes(){return fGeoPassNodes;}

  ClassDef(BmnDch2GeoPar,1)
};

#endif /* !BMNDCH2GEOPAR_H */
