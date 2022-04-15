#include "BmnBdGeoPar.h"
#include "FairParamList.h"
#include <iostream>
#include <iomanip>

using namespace std;

BmnBdGeoPar::BmnBdGeoPar(const char* name,const char* title,const char* context)
  : FairParGenericSet(name,title,context)
{
    fGeoSensNodes = new TObjArray();
    fGeoPassNodes = new TObjArray();
}

void BmnBdGeoPar::clear()
{
    if(fGeoSensNodes) delete fGeoSensNodes;
    if(fGeoPassNodes) delete fGeoPassNodes;
}

void BmnBdGeoPar::putParams(FairParamList* l)
{
    if (!l) return;

    l->addObject("FairGeoNodes Sensitive List", fGeoSensNodes);
    l->addObject("FairGeoNodes Passive List", fGeoPassNodes);
}

Bool_t BmnBdGeoPar::getParams(FairParamList* l)
{
    if (!l) return kFALSE;

    if (!l->fillObject("FairGeoNodes Sensitive List", fGeoSensNodes)) return kFALSE;
    if (!l->fillObject("FairGeoNodes Passive List", fGeoPassNodes)) return kFALSE;

    return kTRUE;
}

ClassImp(BmnBdGeoPar)
