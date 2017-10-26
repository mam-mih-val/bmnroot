#ifndef BMNMWPCGEOPAR_H
#define BMNMWPCGEOPAR_H

#include "FairParGenericSet.h"
#include "TH1F.h"
#include "TObjArray.h"

class BmnMwpcGeoPar : public FairParGenericSet {
public:
  TObjArray            *fGeoSensNodes; // List of FairGeoNodes for sensitive volumes
  TObjArray            *fGeoPassNodes; // List of FairGeoNodes for sensitive volumes

  BmnMwpcGeoPar(const char* name="BmnMwpcGeoPar",
             const char* title="MWPC Geometry Parameters",
             const char* context="TestDefaultContext");
  ~BmnMwpcGeoPar(void);
  void clear(void);
  void putParams(FairParamList*);
  Bool_t getParams(FairParamList*);
  TObjArray             *GetGeoSensitiveNodes(){return fGeoSensNodes;}
  TObjArray             *GetGeoPassiveNodes(){return fGeoPassNodes;}

  ClassDef(BmnMwpcGeoPar,1)
};

#endif /* !BMNMWPC1GEOPAR_H */
