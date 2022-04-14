//--------------------------------------------------------------------------------------------------------------------------------------
using namespace std;
#include "BmnTOF1GeoPar.h"
#include "FairParamList.h"
#include <iostream>
#include <iomanip>

//--------------------------------------------------------------------------------------------------------------------------------------
ClassImp(BmnTOF1GeoPar)

BmnTOF1GeoPar::BmnTOF1GeoPar(const char* name,const char* title,const char* context)
 : FairParGenericSet(name,title,context) 
{
	fGeoSensNodes = new TObjArray();
	fGeoPassNodes = new TObjArray();
}
//--------------------------------------------------------------------------------------------------------------------------------------
BmnTOF1GeoPar::~BmnTOF1GeoPar(void) // FIXME ???? memleak?
{

}
//--------------------------------------------------------------------------------------------------------------------------------------
// probably the next funtions can be deleted FIXME: ?????????
void 		BmnTOF1GeoPar::clear(void) 
{
	if(fGeoSensNodes) delete fGeoSensNodes;
	if(fGeoPassNodes) delete fGeoPassNodes;
}
//--------------------------------------------------------------------------------------------------------------------------------------
void 		BmnTOF1GeoPar::putParams(FairParamList *ptr) 
{
	if(!ptr) return;

	ptr->addObject("FairGeoNodes Sensitive List", fGeoSensNodes);
	ptr->addObject("FairGeoNodes Passive List", fGeoPassNodes);
}
//--------------------------------------------------------------------------------------------------------------------------------------
Bool_t 		BmnTOF1GeoPar::getParams(FairParamList *ptr) 
{
	if(!ptr) return kFALSE;

	if(!ptr->fillObject("FairGeoNodes Sensitive List", fGeoSensNodes)) return kFALSE;
	if(!ptr->fillObject("FairGeoNodes Passive List", fGeoPassNodes)) return kFALSE;

return kTRUE;
}
//--------------------------------------------------------------------------------------------------------------------------------------

