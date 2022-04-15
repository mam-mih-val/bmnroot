/*************************************************************************************
 *
 *         Class BmnFdGeo
 *
 ************************************************************************************/

#include <iostream>

#include "BmnSiMDGeo.h"
#include "FairGeoNode.h"
#include <iostream>

using std::cout;
using std::endl;

ClassImp(BmnSiMDGeo)

// -----   Default constructor   -------------------------------------------
BmnSiMDGeo::BmnSiMDGeo() {
  // Constructor
  fName="SiMD";
  maxSectors=10;
  maxModules=70;
 }
// -------------------------------------------------------------------------

const char* BmnSiMDGeo::getModule1Name(Int_t k) {
  // Returns the module name of muo number m

  sprintf(mod1Name,"SiMD0%i",k+1);
  return mod1Name;
  cout << "MOD1NAME: " << mod1Name << endl;
}

const char* BmnSiMDGeo::getEle1Name(Int_t k) {
  // Returns the element name of muo number m
 
  sprintf(ele1Name,"s%i",k+1);
  return ele1Name;
  cout << "ELE1NAME: " << ele1Name << endl;
}
