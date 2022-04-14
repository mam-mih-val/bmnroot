/*************************************************************************************
 *
 *         Class BmnZdcGeo
 *         
 *  Adopted for BMN by:   Elena Litvinenko
 *  e-mail:   litvin@nf.jinr.ru
 *  Version:  06-11-2015   
 *
 ************************************************************************************/

#include <iostream>

#include "BmnZdcGeo.h"
#include "FairGeoNode.h"
#include <iostream>

using std::cout;
using std::endl;

ClassImp(BmnZdcGeo)

// -----   Default constructor   -------------------------------------------
BmnZdcGeo::BmnZdcGeo() {
  // Constructor
  fName="zdc";
  maxSectors=0;
  maxModules=4;
 }
// -------------------------------------------------------------------------

const char* BmnZdcGeo::getModuleName(Int_t m) {
  // Returns the module name of muo number m

  sprintf(modName,"zdc0%i",m+1);
  return modName;
  cout << "MODNAME: " << modName << endl;
}

const char* BmnZdcGeo::getEleName(Int_t m) {
  // Returns the element name of muo number m
 
  sprintf(eleName,"s%i",m+1);
  return eleName;
  cout << "ELENAME: " << eleName << endl;
}
