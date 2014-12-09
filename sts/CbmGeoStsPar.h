#ifndef CBMGEOSTSPAR_H
#define CBMGEOSTSPAR_H

#include "FairParGenericSet.h"

#include "TH1F.h"

class CbmGeoStsPar : public FairParGenericSet {
public:
  TObjArray            *fGeoSensNodes; // List of FairGeoNodes for sensitive volumes
  TObjArray            *fGeoPassNodes; // List of FairGeoNodes for sensitive volumes

  CbmGeoStsPar(const char* name="CbmGeoStsPar",
             const char* title="Sts Geometry Parameters",
             const char* context="TestDefaultContext");
  ~CbmGeoStsPar(void);
  void clear(void);
  void putParams(FairParamList*);
  Bool_t getParams(FairParamList*);
  TObjArray             *GetGeoSensitiveNodes(){return fGeoSensNodes;}
  TObjArray             *GetGeoPassiveNodes(){return fGeoPassNodes;}
private:

  CbmGeoStsPar(const CbmGeoStsPar&);
  CbmGeoStsPar operator=(const CbmGeoStsPar&);

  ClassDef(CbmGeoStsPar,1)
};

#endif /* !CBMGEOSTSPAR_H */
