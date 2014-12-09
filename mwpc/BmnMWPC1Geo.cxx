/////////////////////////////////////////////////////////////
// BmnMWPC1Geo
//
// Class for geometry of MWPC1
//
/////////////////////////////////////////////////////////////

#include "BmnMWPC1Geo.h"
#include "FairGeoNode.h"

ClassImp(BmnMWPC1Geo)

BmnMWPC1Geo::BmnMWPC1Geo() {
  // Constructor
  fName="MWPC1";
  maxSectors=0;
  maxModules=24;
}

const char* BmnMWPC1Geo::getModuleName(Int_t m) {
  // Returns the module name of MWPC1 number m
  //  std::cout << "MWPC1 module: " << m << "\n";
  if (m < 9) {
    sprintf(modName,"MWPC10%i",m+1);
  } else {
    sprintf(modName,"MWPC1%i",m+1);
  }
  return modName;
}

const char* BmnMWPC1Geo::getEleName(Int_t m) {
  // Returns the element name of MWPC1 number m
  //  std::cout << "MWPC1 element: " << m << "\n";
  if (m < 9) {
    sprintf(eleName,"MWPC10%i",m+1);
  } else {
    sprintf(eleName,"MWPC1%i",m+1);
  }
  return eleName;
}


