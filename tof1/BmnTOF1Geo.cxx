//--------------------------------------------------------------------------------------------------------------------------------------
#include<iostream>

#include "BmnTOF1Geo.h"
#include "FairGeoNode.h"

ClassImp(BmnTOF1Geo)
//--------------------------------------------------------------------------------------------------------------------------------------
BmnTOF1Geo::BmnTOF1Geo() 
{
	fName = "TOFB";
	maxSectors = 0;
	maxModules = 200;
}
//--------------------------------------------------------------------------------------------------------------------------------------
const char* BmnTOF1Geo::getModuleName(Int_t m) 
{
	sprintf(modName, "TOFB%i", m+1);

return modName;
}
//--------------------------------------------------------------------------------------------------------------------------------------
const char* BmnTOF1Geo::getEleName(Int_t m) 
{
	sprintf(eleName, "t%i", m+1);
  
return eleName;
}
//--------------------------------------------------------------------------------------------------------------------------------------

