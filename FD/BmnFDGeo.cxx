/*************************************************************************************
 *
 *         Class BmnFdGeo
 *
 ************************************************************************************/

#include <iostream>

#include "BmnFDGeo.h"
#include "FairGeoNode.h"
#include <iostream>

using std::cout;
using std::endl;

ClassImp(BmnFDGeo)

// -----   Default constructor   -------------------------------------------
BmnFDGeo::BmnFDGeo() {
  // Constructor
  fName="FD";
  maxSectors=10;
  maxModules=70;
 }
// -------------------------------------------------------------------------

const char* BmnFDGeo::getModule1Name(Int_t k) {
  // Returns the module name of muo number m

  sprintf(mod1Name,"FD0%i",k+1);
  return mod1Name;
  cout << "MOD1NAME: " << mod1Name << endl;
}

const char* BmnFDGeo::getEle1Name(Int_t k) {
  // Returns the element name of muo number m
 
  sprintf(ele1Name,"s%i",k+1);
  return ele1Name;
  cout << "ELE1NAME: " << ele1Name << endl;
}
