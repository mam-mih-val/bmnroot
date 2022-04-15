//*-- AUTHOR : Ilse Koenig
//*-- Created : 10/11/2003

/////////////////////////////////////////////////////////////
// CbmGeoSts
//
// Class for geometry of STS
//
/////////////////////////////////////////////////////////////
#include "CbmGeoSts.h"
#include "FairGeoNode.h"

#include <iostream>

using std::cout;
using std::endl;


ClassImp(CbmGeoSts)

CbmGeoSts::CbmGeoSts() {
  // Constructor
  fName="sts";
  maxSectors=0;
  maxModules=200;
}

const char* CbmGeoSts::getModuleName(Int_t m) {
  // Returns the module name of sts number m
  if ( m < 0 ) {
  	cout <<"-E- CbmGeoSts::getModuleName:: Module number "
  	       << m << " not known!" << endl;
	 return "";
	 }
  //AZ if ( m < 9 ) sprintf(modName,"stsstation0%i",m+1);
  //AZ else  sprintf(modName,"stsstation%i",m+1);
  if ( m < 9 ) sprintf(modName,"STSStation0%i",m+1);
  else  sprintf(modName,"STSStation%i",m+1);
  return modName;
}

const char* CbmGeoSts::getEleName(Int_t m) {
  // Returns the element name of sts number m
  //AZ sprintf(eleName,"sts%i",m+1);
  sprintf(eleName,"STS%i",m+1);
  return eleName;
}
