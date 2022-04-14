/*************************************************************************************
 *
 *         Class BmnHodoGeoPar
 *
 ************************************************************************************/

using namespace std;
#include "BmnHodoGeoPar.h"

#include <iomanip>
#include <iostream>

#include "FairParamList.h"

ClassImp(BmnHodoGeoPar)

    BmnHodoGeoPar::BmnHodoGeoPar(const char* name, const char* title, const char* context)
    : FairParGenericSet(name, title, context) {
    fGeoSensNodes = new TObjArray();
    fGeoPassNodes = new TObjArray();
}

BmnHodoGeoPar::~BmnHodoGeoPar(void) {
}

// probably the next funtions can be deleted

void BmnHodoGeoPar::clear(void) {
    if (fGeoSensNodes) delete fGeoSensNodes;
    if (fGeoPassNodes) delete fGeoPassNodes;
}

void BmnHodoGeoPar::putParams(FairParamList* l) {
    if (!l) return;
    l->addObject("FairGeoNodes Sensitive List", fGeoSensNodes);
    l->addObject("FairGeoNodes Passive List", fGeoPassNodes);
}

Bool_t BmnHodoGeoPar::getParams(FairParamList* l) {
    if (!l) return kFALSE;
    if (!l->fillObject("FairGeoNodes Sensitive List", fGeoSensNodes)) return kFALSE;
    if (!l->fillObject("FairGeoNodes Passive List", fGeoPassNodes)) return kFALSE;

    return kTRUE;
}
