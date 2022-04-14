/////////////////////////////////////////////////////////////
// BmnDchGeo
//
// Class for geometry of DCH
//
/////////////////////////////////////////////////////////////

#include "BmnDchGeo.h"
#include "FairGeoNode.h"

ClassImp(BmnDchGeo)

BmnDchGeo::BmnDchGeo() {
  // Constructor
  maxSectors=0;
  maxModules=24;
}

const char* BmnDchGeo::getModuleName(Int_t m) {
  // Returns the module name of DCH number m
  return modName;
}

const char* BmnDchGeo::getEleName(Int_t m) {
  // Returns the element name of DCH number m
  return eleName;
}


