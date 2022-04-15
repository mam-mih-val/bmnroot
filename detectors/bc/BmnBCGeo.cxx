

#include <iostream>

#include "BmnBCGeo.h"
#include "FairGeoNode.h"
#include <iostream>

using std::cout;
using std::endl;

ClassImp(BmnBCGeo)

// -----   Default constructor   -------------------------------------------
BmnBCGeo::BmnBCGeo() {
  // Constructor
  fName="BC";
  maxSectors=10;
  maxModules=100;
 }
// -------------------------------------------------------------------------

const char* BmnBCGeo::getModule2Name(Int_t q) {
  // Returns the module name of muo number m

  sprintf(mod2Name,"BC0%i",q+1);
  return mod2Name;
  cout << "MOD2NAME: " << mod2Name << endl;
}

const char* BmnBCGeo::getEle2Name(Int_t q) {
  // Returns the element name of muo number m
 
  sprintf(ele2Name,"s%i",q+1);
  return ele2Name;
  cout << "ELE2NAME: " << ele2Name << endl;
}
