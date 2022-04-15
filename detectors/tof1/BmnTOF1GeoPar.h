//--------------------------------------------------------------------------------------------------------------------------------------
#ifndef __BMNGEOTOF1PAR_H
#define __BMNGEOTOF1PAR_H 1

#include<TObjArray.h>

#include "FairParGenericSet.h"

//--------------------------------------------------------------------------------------------------------------------------------------
class BmnTOF1GeoPar : public FairParGenericSet 
{
public:
	TObjArray      *fGeoSensNodes; /** List of FairGeoNodes for sensitive  volumes */
	TObjArray      *fGeoPassNodes; /** List of FairGeoNodes for passive  volumes */
  
	BmnTOF1GeoPar(const char* name="BmnTOF1GeoPar", const char* title="TOF1 Geometry Parameters", const char* context="TOF1DefaultContext");
	~BmnTOF1GeoPar(void);

	void 		clear(void);
	void 		putParams(FairParamList*);
	Bool_t 		getParams(FairParamList*);

	TObjArray*	GetGeoSensitiveNodes(){ return fGeoSensNodes; }
	TObjArray*	GetGeoPassiveNodes(){ return fGeoPassNodes; } 
  
 ClassDef(BmnTOF1GeoPar,1)
};

#endif 
//--------------------------------------------------------------------------------------------------------------------------------------

