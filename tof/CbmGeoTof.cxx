//*-- AUTHOR : Ilse Koenig
//*-- Created : 10/11/2003

/////////////////////////////////////////////////////////////
// CbmGeoTof
//
// Class for geometry of TOF
//
/////////////////////////////////////////////////////////////

#include "CbmGeoTof.h"

ClassImp(CbmGeoTof)

CbmGeoTof::CbmGeoTof() {
  // Constructor
  fName="tof";
  maxSectors=0;
//  maxModules=24;
  maxModules=100;
}

const char* CbmGeoTof::getModuleName(Int_t m) {
  // Returns the module name of tof number m
  sprintf(modName,"tof%i",m+1);
  return modName;
}

const char* CbmGeoTof::getEleName(Int_t m) {
  // Returns the element name of tof number m
  sprintf(eleName,"t%i",m+1);
  return eleName;
}
