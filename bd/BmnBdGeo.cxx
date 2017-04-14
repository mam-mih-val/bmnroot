/*************************************************************************************
 *
 *         Class BmnBdGeo
 *         
 *  Adopted for BMN by:   Elena Litvinenko
 *  e-mail:   litvin@nf.jinr.ru
 *  Version:  10-02-2016   
 *
 ************************************************************************************/

#include <iostream>

#include "BmnBdGeo.h"
#include "FairGeoNode.h"
#include <iostream>

using std::cout;
using std::endl;

ClassImp(BmnBdGeo)

// -----   Default constructor   -------------------------------------------
BmnBdGeo::BmnBdGeo() {
  // Constructor
  fName="bd";
  maxSectors=0;
  maxModules=4;
 }
// -------------------------------------------------------------------------

const char* BmnBdGeo::getModuleName(Int_t m) {
  // Returns the module name of muo number m

  sprintf(modName,"bd0%i",m+1);
  return modName;
  cout << "MODNAME: " << modName << endl;
}

const char* BmnBdGeo::getEleName(Int_t m) {
  // Returns the element name of muo number m
 
  sprintf(eleName,"s%i",m+1);
  return eleName;
  cout << "ELENAME: " << eleName << endl;
}
