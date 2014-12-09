#ifndef CBMGEOTOFPAR_H
#define CBMGEOTOFPAR_H

#include "FairParGenericSet.h"

class FairParamList;

class TObjArray;

class CbmGeoTofPar : public FairParGenericSet {
public:
  TObjArray            *fGeoSensNodes; // List of FairGeoNodes for sensitive volumes
  TObjArray            *fGeoPassNodes; // List of FairGeoNodes for sensitive volumes

  CbmGeoTofPar(const char* name="CbmGeoTofPar",
             const char* title="Tof Geometry Parameters",
             const char* context="TestDefaultContext");
  ~CbmGeoTofPar(void);
  void clear(void);
  void putParams(FairParamList*);
  Bool_t getParams(FairParamList*);
  TObjArray             *GetGeoSensitiveNodes(){return fGeoSensNodes;}
  TObjArray             *GetGeoPassiveNodes(){return fGeoPassNodes;}

 private:

  CbmGeoTofPar(const CbmGeoTofPar&);
  CbmGeoTofPar& operator=(const CbmGeoTofPar&);

  ClassDef(CbmGeoTofPar,1)
};

#endif /* !CBMGEOTOFPAR_H */
