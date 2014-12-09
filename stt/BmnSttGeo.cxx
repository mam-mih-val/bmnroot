/////////////////////////////////////////////////////////////
// BmnSttGeo
//
// Class for geometry of Stt
//
/////////////////////////////////////////////////////////////

#include "BmnSttGeo.h"
#include "FairGeoNode.h"

ClassImp(BmnSttGeo)

BmnSttGeo::BmnSttGeo() {
  // Constructor
  fName="Stt";
  maxSectors=0;
  maxModules=24;
}

const char* BmnSttGeo::getModuleName(Int_t m) {
  // Returns the module name of Stt number m
  //  std::cout << "Stt module: " << m << "\n";
  if (m < 9) {
    sprintf(modName,"stt0%i",m+1);
  } else {
    sprintf(modName,"stt%i",m+1);
  }
  return modName;
}

const char* BmnSttGeo::getEleName(Int_t m) {
  // Returns the element name of Stt number m
  //  std::cout << "Stt element: " << m << "\n";
  if (m < 9) {
    sprintf(eleName,"stt0%i",m+1);
  } else {
    sprintf(eleName,"stt%i",m+1);
  }
  return eleName;
}


