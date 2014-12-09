#ifndef CBMGEOPSDPAR_H
#define CBMGEOPSDPAR_H


#include "FairParGenericSet.h"

class TObjArray;
class FairParamList;

class CbmGeoPsdPar : public FairParGenericSet {
public:
  TObjArray            *fGeoSensNodes; // List of FairGeoNodes for sensitive volumes
  TObjArray            *fGeoPassNodes; // List of FairGeoNodes for sensitive volumes

  CbmGeoPsdPar(const char* name="CbmGeoPsdPar",
             const char* title="Psd Geometry Parameters",
             const char* context="TestDefaultContext");
  ~CbmGeoPsdPar(void);
  void clear(void);
  void putParams(FairParamList*);
  Bool_t getParams(FairParamList*);
  TObjArray             *GetGeoSensitiveNodes(){return fGeoSensNodes;}
  TObjArray             *GetGeoPassiveNodes(){return fGeoPassNodes;}

 private:

  CbmGeoPsdPar(const CbmGeoPsdPar&);
  CbmGeoPsdPar& operator=(const CbmGeoPsdPar&);

  ClassDef(CbmGeoPsdPar,1)
};

#endif /* !CBMGEOPSDPAR_H */
