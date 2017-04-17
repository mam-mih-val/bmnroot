//--------------------------------------------------------------------------------------------------------------------------------------
using namespace std;
#include "BmnTOFGeoPar.h"
#include "FairParamList.h"
#include <iostream>
#include <iomanip>

//--------------------------------------------------------------------------------------------------------------------------------------
ClassImp(BmnTOFGeoPar)

BmnTOFGeoPar::BmnTOFGeoPar(const char* name,const char* title,const char* context)
 : FairParGenericSet(name,title,context) 
{
	fGeoSensNodes = new TObjArray();
	fGeoPassNodes = new TObjArray();
}
//--------------------------------------------------------------------------------------------------------------------------------------
BmnTOFGeoPar::~BmnTOFGeoPar(void) // FIXME ???? memleak?
{

}
//--------------------------------------------------------------------------------------------------------------------------------------
// probably the next funtions can be deleted FIXME: ?????????
void 		BmnTOFGeoPar::clear(void) 
{
	if(fGeoSensNodes) delete fGeoSensNodes;
	if(fGeoPassNodes) delete fGeoPassNodes;
}
//--------------------------------------------------------------------------------------------------------------------------------------
void 		BmnTOFGeoPar::putParams(FairParamList *ptr) 
{
	if(!ptr) return;

	ptr->addObject("FairGeoNodes Sensitive List", fGeoSensNodes);
	ptr->addObject("FairGeoNodes Passive List", fGeoPassNodes);
}
//--------------------------------------------------------------------------------------------------------------------------------------
Bool_t 		BmnTOFGeoPar::getParams(FairParamList *ptr) 
{
	if(!ptr) return kFALSE;

	if(!ptr->fillObject("FairGeoNodes Sensitive List", fGeoSensNodes)) return kFALSE;
	if(!ptr->fillObject("FairGeoNodes Passive List", fGeoPassNodes)) return kFALSE;

return kTRUE;
}
//--------------------------------------------------------------------------------------------------------------------------------------

