#ifndef BMNMWPC3GEOPAR_H
#define BMNMWPC3GEOPAR_H

#include "FairParGenericSet.h"
#include "TH1F.h"
#include "TObjArray.h"

class BmnMWPC3GeoPar : public FairParGenericSet {
public:
  TObjArray            *fGeoSensNodes; // List of FairGeoNodes for sensitive volumes
  TObjArray            *fGeoPassNodes; // List of FairGeoNodes for sensitive volumes

  BmnMWPC3GeoPar(const char* name="BmnMWPC3GeoPar",
             const char* title="MWPC3 Geometry Parameters",
             const char* context="TestDefaultContext");
  ~BmnMWPC3GeoPar(void);
  void clear(void);
  void putParams(FairParamList*);
  Bool_t getParams(FairParamList*);
  TObjArray             *GetGeoSensitiveNodes(){return fGeoSensNodes;}
  TObjArray             *GetGeoPassiveNodes(){return fGeoPassNodes;}

  ClassDef(BmnMWPC3GeoPar,1)
};

#endif /* !BMNMWPC3GEOPAR_H */
