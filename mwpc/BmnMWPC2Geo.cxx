/////////////////////////////////////////////////////////////
// BmnMWPC2Geo
//
// Class for geometry of MWPC2
//
/////////////////////////////////////////////////////////////

#include "BmnMWPC2Geo.h"
#include "FairGeoNode.h"

ClassImp(BmnMWPC2Geo)

BmnMWPC2Geo::BmnMWPC2Geo() {
  // Constructor
  fName="MWPC2";
  maxSectors=0;
  maxModules=24;
}

const char* BmnMWPC2Geo::getModuleName(Int_t m) {
  // Returns the module name of MWPC2 number m
  //  std::cout << "MWPC2 module: " << m << "\n";
  if (m < 9) {
    sprintf(modName,"MWPC20%i",m+1);
  } else {
    sprintf(modName,"MWPC2%i",m+1);
  }
  return modName;
}

const char* BmnMWPC2Geo::getEleName(Int_t m) {
  // Returns the element name of MWPC2 number m
  //  std::cout << "MWPC2 element: " << m << "\n";
  if (m < 9) {
    sprintf(eleName,"MWPC20%i",m+1);
  } else {
    sprintf(eleName,"MWPC2%i",m+1);
  }
  return eleName;
}


