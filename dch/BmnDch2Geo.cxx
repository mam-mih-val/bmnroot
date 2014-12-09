/////////////////////////////////////////////////////////////
// BmnDch2Geo
//
// Class for geometry of DCH2
//
/////////////////////////////////////////////////////////////

#include "BmnDch2Geo.h"
#include "FairGeoNode.h"

ClassImp(BmnDch2Geo)

BmnDch2Geo::BmnDch2Geo() {
  // Constructor
  fName="DCH2";
  maxSectors=0;
  maxModules=24;
}

const char* BmnDch2Geo::getModuleName(Int_t m) {
  // Returns the module name of DCH2 number m
  //  std::cout << "DCH2 module: " << m << "\n";
  if (m < 9) {
    sprintf(modName,"DCH20%i",m+1);
  } else {
    sprintf(modName,"DCH2%i",m+1);
  }
  return modName;
}

const char* BmnDch2Geo::getEleName(Int_t m) {
  // Returns the element name of DCH2 number m
  //  std::cout << "DCH2 element: " << m << "\n";
  if (m < 9) {
    sprintf(eleName,"DCH20%i",m+1);
  } else {
    sprintf(eleName,"DCH2%i",m+1);
  }
  return eleName;
}


