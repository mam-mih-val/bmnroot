//--------------------------------------------------------------------------------------------------------------------------------------
#ifndef __BMNGEOTOF_H
#define __BMNGEOTOF_H 1

#include "FairGeoSet.h"

//--------------------------------------------------------------------------------------------------------------------------------------
class  BmnTOFGeo : public FairGeoSet 
{
protected:
	char modName[20];  // name of module
	char eleName[20];  // substring for elements in module
  
public:
	BmnTOFGeo();
	~BmnTOFGeo() {}

	const char* getModuleName(Int_t);
	const char* getEleName(Int_t);
  
ClassDef(BmnTOFGeo, 0) // Class for Hyp
};

#endif
//--------------------------------------------------------------------------------------------------------------------------------------


