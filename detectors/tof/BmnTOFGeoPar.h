//--------------------------------------------------------------------------------------------------------------------------------------
#ifndef __BMNGEOTOFPAR_H
#define __BMNGEOTOFPAR_H 1

#include<TObjArray.h>

#include "FairParGenericSet.h"

//--------------------------------------------------------------------------------------------------------------------------------------
class BmnTOFGeoPar : public FairParGenericSet 
{
public:
	TObjArray      *fGeoSensNodes; /** List of FairGeoNodes for sensitive  volumes */
	TObjArray      *fGeoPassNodes; /** List of FairGeoNodes for passive  volumes */
  
	BmnTOFGeoPar(const char* name="BmnTOFGeoPar", const char* title="TOF Geometry Parameters", const char* context="TOFDefaultContext");
	~BmnTOFGeoPar(void);

	void 		clear(void);
	void 		putParams(FairParamList*);
	Bool_t 		getParams(FairParamList*);

	TObjArray*	GetGeoSensitiveNodes(){ return fGeoSensNodes; }
	TObjArray*	GetGeoPassiveNodes(){ return fGeoPassNodes; } 
  
 ClassDef(BmnTOFGeoPar,1)
};

#endif 
//--------------------------------------------------------------------------------------------------------------------------------------

