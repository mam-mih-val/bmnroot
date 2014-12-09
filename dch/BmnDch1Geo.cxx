/////////////////////////////////////////////////////////////
// BmnDch1Geo
//
// Class for geometry of DCH1
//
/////////////////////////////////////////////////////////////

#include "BmnDch1Geo.h"
#include "FairGeoNode.h"

ClassImp(BmnDch1Geo)

BmnDch1Geo::BmnDch1Geo() {
  // Constructor
  fName="DCH1";
  maxSectors=0;
  maxModules=24;
}

const char* BmnDch1Geo::getModuleName(Int_t m) {
  // Returns the module name of DCH1 number m
  //  std::cout << "DCH1 module: " << m << "\n";
  if (m < 9) {
    sprintf(modName,"DCH10%i",m+1);
  } else {
    sprintf(modName,"DCH1%i",m+1);
  }
  return modName;
}

const char* BmnDch1Geo::getEleName(Int_t m) {
  // Returns the element name of DCH1 number m
  //  std::cout << "DCH1 element: " << m << "\n";
  if (m < 9) {
    sprintf(eleName,"DCH10%i",m+1);
  } else {
    sprintf(eleName,"DCH1%i",m+1);
  }
  return eleName;
}


