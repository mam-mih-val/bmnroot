#ifndef BMNMWPC1GEOPAR_H
#define BMNMWPC1GEOPAR_H

#include "FairParGenericSet.h"
#include "TH1F.h"
#include "TObjArray.h"

class BmnMWPC1GeoPar : public FairParGenericSet {
public:
  TObjArray            *fGeoSensNodes; // List of FairGeoNodes for sensitive volumes
  TObjArray            *fGeoPassNodes; // List of FairGeoNodes for sensitive volumes

  BmnMWPC1GeoPar(const char* name="BmnMWPC1GeoPar",
             const char* title="MWPC1 Geometry Parameters",
             const char* context="TestDefaultContext");
  ~BmnMWPC1GeoPar(void);
  void clear(void);
  void putParams(FairParamList*);
  Bool_t getParams(FairParamList*);
  TObjArray             *GetGeoSensitiveNodes(){return fGeoSensNodes;}
  TObjArray             *GetGeoPassiveNodes(){return fGeoPassNodes;}

  ClassDef(BmnMWPC1GeoPar,1)
};

#endif /* !BMNMWPC1GEOPAR_H */
