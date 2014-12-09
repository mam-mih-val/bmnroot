#include "CbmGeoPsdPar.h"

#include "FairParamList.h"

#include "TObjArray.h"

ClassImp(CbmGeoPsdPar)

CbmGeoPsdPar::CbmGeoPsdPar(const char* name,const char* title,const char* context)
: FairParGenericSet(name,title,context), 
  fGeoSensNodes(new TObjArray()),
  fGeoPassNodes(new TObjArray())
{
}

CbmGeoPsdPar::~CbmGeoPsdPar(void) {
}

void CbmGeoPsdPar::clear(void) {
    if(fGeoSensNodes) delete fGeoSensNodes;
    if(fGeoPassNodes) delete fGeoPassNodes;
}

void CbmGeoPsdPar::putParams(FairParamList* l) {
  if (!l) return;
   l->addObject("FairGeoNodes Sensitive List", fGeoSensNodes);
   l->addObject("FairGeoNodes Passive List", fGeoPassNodes);
}

Bool_t CbmGeoPsdPar::getParams(FairParamList* l) {
    if (!l) return kFALSE;
    if (!l->fillObject("FairGeoNodes Sensitive List", fGeoSensNodes)) return kFALSE;
    if (!l->fillObject("FairGeoNodes Passive List", fGeoPassNodes)) return kFALSE;

  return kTRUE;
}
