/////////////////////////////////////////////////////////////
// BmnMwpcGeo
//
// Class for geometry of Mwpc
//
/////////////////////////////////////////////////////////////

#include "BmnMwpcGeo.h"
#include "FairGeoNode.h"

ClassImp(BmnMwpcGeo)

BmnMwpcGeo::BmnMwpcGeo() {
  // Constructor
  fName="MWPC";
  maxSectors=0;
  maxModules=24;
}

const char* BmnMwpcGeo::getModuleName(Int_t m) {
  // Returns the module name of MWPC1 number m
  //  std::cout << "MWPC1 module: " << m << "\n";
  if (m < 9) {
    sprintf(modName,"MWPC0%i",m+1);
  } else {
    sprintf(modName,"MWPC%i",m+1);
  }
  return modName;
}

const char* BmnMwpcGeo::getEleName(Int_t m) {
  // Returns the element name of MWPC1 number m
  //  std::cout << "MWPC1 element: " << m << "\n";
  if (m < 9) {
    sprintf(eleName,"MWPC0%i",m+1);
  } else {
    sprintf(eleName,"MWPC%i",m+1);
  }
  return eleName;
}