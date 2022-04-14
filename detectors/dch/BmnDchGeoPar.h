#ifndef BMNDCHGEOPAR_H
#define BMNDCHGEOPAR_H

#include "FairParGenericSet.h"
#include "TH1F.h"
#include "TObjArray.h"

class BmnDchGeoPar : public FairParGenericSet {
public:
  TObjArray            *fGeoSensNodes; // List of FairGeoNodes for sensitive volumes
  TObjArray            *fGeoPassNodes; // List of FairGeoNodes for sensitive volumes

  BmnDchGeoPar(const char* name="BmnDchGeoPar",
             const char* title="DCH Geometry Parameters",
             const char* context="TestDefaultContext");
  ~BmnDchGeoPar(void);
  void clear(void);
  void putParams(FairParamList*);
  Bool_t getParams(FairParamList*);
  TObjArray             *GetGeoSensitiveNodes(){return fGeoSensNodes;}
  TObjArray             *GetGeoPassiveNodes(){return fGeoPassNodes;}

  ClassDef(BmnDchGeoPar,1)
};

#endif /* !BMNDCHGEOPAR_H */
