//*-- AUTHOR : Denis Bertini
//*-- Created : 21/06/2005

#include "CbmGeoStsPar.h"

#include "FairParamList.h"

#include "TObjArray.h"

#include <iostream>
#include <iomanip>

ClassImp(CbmGeoStsPar)

CbmGeoStsPar::CbmGeoStsPar(const char* name,const char* title,const char* context)
           : FairParGenericSet(name,title,context),
	     fGeoSensNodes(NULL),
	     fGeoPassNodes(NULL)
{

               fGeoSensNodes = new TObjArray();
               fGeoPassNodes = new TObjArray();
}

CbmGeoStsPar::~CbmGeoStsPar(void) {
}

void CbmGeoStsPar::clear(void) {
    if(fGeoSensNodes) delete fGeoSensNodes;
    if(fGeoPassNodes) delete fGeoPassNodes;
}

void CbmGeoStsPar::putParams(FairParamList* l) {
  if (!l) return;
   l->addObject("FairGeoNodes Sensitive List", fGeoSensNodes);
   l->addObject("FairGeoNodes Passive List", fGeoPassNodes);
}

Bool_t CbmGeoStsPar::getParams(FairParamList* l) {
    if (!l) return kFALSE;
    if (!l->fillObject("FairGeoNodes Sensitive List", fGeoSensNodes)) return kFALSE;
    if (!l->fillObject("FairGeoNodes Passive List", fGeoPassNodes)) return kFALSE;

  return kTRUE;
}
