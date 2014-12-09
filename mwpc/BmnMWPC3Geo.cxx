/////////////////////////////////////////////////////////////
// BmnMWPC3Geo
//
// Class for geometry of MWPC3
//
/////////////////////////////////////////////////////////////

#include "BmnMWPC3Geo.h"
#include "FairGeoNode.h"

ClassImp(BmnMWPC3Geo)

BmnMWPC3Geo::BmnMWPC3Geo() {
  // Constructor
  fName="MWPC3";
  maxSectors=0;
  maxModules=24;
}

const char* BmnMWPC3Geo::getModuleName(Int_t m) {
  // Returns the module name of MWPC3 number m
  //  std::cout << "MWPC3 module: " << m << "\n";
  if (m < 9) {
    sprintf(modName,"MWPC30%i",m+1);
  } else {
    sprintf(modName,"MWPC3%i",m+1);
  }
  return modName;
}

const char* BmnMWPC3Geo::getEleName(Int_t m) {
  // Returns the element name of MWPC3 number m
  //  std::cout << "MWPC3 element: " << m << "\n";
  if (m < 9) {
    sprintf(eleName,"MWPC30%i",m+1);
  } else {
    sprintf(eleName,"MWPC3%i",m+1);
  }
  return eleName;
}


