//*-- AUTHOR : Ilse Koenig
//*-- Created : 10/11/2003

/////////////////////////////////////////////////////////////
// CbmGeoTrd
//
// Class for geometry of ECAL
//
/////////////////////////////////////////////////////////////

#include "CbmGeoPsd.h"

ClassImp(CbmGeoPsd)

CbmGeoPsd::CbmGeoPsd() {
  // Constructor
  fName="psd";
  maxSectors=0;
  maxModules=10;
}

const char* CbmGeoPsd::getModuleName(Int_t m) {
  // Returns the module name of ecal number m
  sprintf(modName,"psd%i",m+1);
  return modName;
}

const char* CbmGeoPsd::getEleName(Int_t m) {
  // Returns the element name of psd number m
  sprintf(eleName,"psdd%i",m+1);
  return eleName;
}
