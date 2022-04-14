//--------------------------------------------------------------------------------------------------------------------------------------
#ifndef __BMNGEOTOF1_H
#define __BMNGEOTOF1_H 1

#include "FairGeoSet.h"

//--------------------------------------------------------------------------------------------------------------------------------------
class  BmnTOF1Geo : public FairGeoSet 
{
protected:
	char modName[20];  // name of module
	char eleName[20];  // substring for elements in module
  
public:
	BmnTOF1Geo();
	~BmnTOF1Geo() {}

	const char* getModuleName(Int_t);
	const char* getEleName(Int_t);
  
ClassDef(BmnTOF1Geo, 0) // Class for Hyp
};

#endif
//--------------------------------------------------------------------------------------------------------------------------------------


