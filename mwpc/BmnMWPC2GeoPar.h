#ifndef BMNMWPC2GEOPAR_H
#define BMNMWPC2GEOPAR_H

#include "FairParGenericSet.h"
#include "TH1F.h"
#include "TObjArray.h"

class BmnMWPC2GeoPar : public FairParGenericSet {
public:
  TObjArray            *fGeoSensNodes; // List of FairGeoNodes for sensitive volumes
  TObjArray            *fGeoPassNodes; // List of FairGeoNodes for sensitive volumes

  BmnMWPC2GeoPar(const char* name="BmnMWPC2GeoPar",
             const char* title="MWPC2 Geometry Parameters",
             const char* context="TestDefaultContext");
  ~BmnMWPC2GeoPar(void);
  void clear(void);
  void putParams(FairParamList*);
  Bool_t getParams(FairParamList*);
  TObjArray             *GetGeoSensitiveNodes(){return fGeoSensNodes;}
  TObjArray             *GetGeoPassiveNodes(){return fGeoPassNodes;}

  ClassDef(BmnMWPC2GeoPar,1)
};

#endif /* !BMNMWPC2GEOPAR_H */
