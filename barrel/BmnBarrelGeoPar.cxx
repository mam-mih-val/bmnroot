/*************************************************************************************
 *
 *         Class BmnBarrelGeoPar
 *         
 *  Adopted for BMN by:   Elena Litvinenko
 *  e-mail:   litvin@nf.jinr.ru
 *  Version:  10-02-2016   
 *
 ************************************************************************************/

using namespace std;
#include "BmnBarrelGeoPar.h"
#include "FairParamList.h"
#include <iostream>
#include <iomanip>

ClassImp(BmnBarrelGeoPar)

BmnBarrelGeoPar::BmnBarrelGeoPar(const char* name,const char* title,const char* context)
           : FairParGenericSet(name,title,context) {

               fGeoSensNodes = new TObjArray();
               fGeoPassNodes = new TObjArray();

}

BmnBarrelGeoPar::~BmnBarrelGeoPar(void) {
}

// probably the next funtions can be deleted

void BmnBarrelGeoPar::clear(void) {
    if(fGeoSensNodes) delete fGeoSensNodes;
    if(fGeoPassNodes) delete fGeoPassNodes;
}

void BmnBarrelGeoPar::putParams(FairParamList* l) {
  if (!l) return;
   l->addObject("FairGeoNodes Sensitive List", fGeoSensNodes);
   l->addObject("FairGeoNodes Passive List", fGeoPassNodes);
}

Bool_t BmnBarrelGeoPar::getParams(FairParamList* l) {
    if (!l) return kFALSE;
    if (!l->fillObject("FairGeoNodes Sensitive List", fGeoSensNodes)) return kFALSE;
    if (!l->fillObject("FairGeoNodes Passive List", fGeoPassNodes)) return kFALSE;

  return kTRUE;
}
