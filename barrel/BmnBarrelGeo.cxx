/*************************************************************************************
 *
 *         Class BmnBarrelGeo
 *         
 *  Adopted for BMN by:   Elena Litvinenko
 *  e-mail:   litvin@nf.jinr.ru
 *  Version:  10-02-2016   
 *
 ************************************************************************************/

#include <iostream>

#include "BmnBarrelGeo.h"
#include "FairGeoNode.h"
#include <iostream>

using std::cout;
using std::endl;

ClassImp(BmnBarrelGeo)

// -----   Default constructor   -------------------------------------------
BmnBarrelGeo::BmnBarrelGeo() {
  // Constructor
  fName="barrel";
  maxSectors=0;
  maxModules=4;
 }
// -------------------------------------------------------------------------

const char* BmnBarrelGeo::getModuleName(Int_t m) {
  // Returns the module name of muo number m

  sprintf(modName,"barrel0%i",m+1);
  return modName;
  cout << "MODNAME: " << modName << endl;
}

const char* BmnBarrelGeo::getEleName(Int_t m) {
  // Returns the element name of muo number m
 
  sprintf(eleName,"s%i",m+1);
  return eleName;
  cout << "ELENAME: " << eleName << endl;
}
