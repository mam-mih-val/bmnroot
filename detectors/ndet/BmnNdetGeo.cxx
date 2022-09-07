/*************************************************************************************
 *
 *         Class BmnNdetGeo
 *         
 *  Adopted for BMN by:   Elena Litvinenko
 *  e-mail:   litvin@nf.jinr.ru
 *  Version:  06-11-2015   
 *  Modified by M.Golubeva July 2022
 *
 ************************************************************************************/

#include <iostream>

#include "BmnNdetGeo.h"
#include "FairGeoNode.h"
#include <iostream>

using std::cout;
using std::endl;

ClassImp(BmnNdetGeo)

// -----   Default constructor   -------------------------------------------
BmnNdetGeo::BmnNdetGeo() {
  // Constructor
  fName="ndet";
  maxSectors=0;
  maxModules=4;
 }
// -------------------------------------------------------------------------

const char* BmnNdetGeo::getModuleName(Int_t m) {
  // Returns the module name of muo number m

  sprintf(modName,"ndet0%i",m+1);
  return modName;
  cout << "MODNAME: " << modName << endl;
}

const char* BmnNdetGeo::getEleName(Int_t m) {
  // Returns the element name of muo number m
 
  sprintf(eleName,"s%i",m+1);
  return eleName;
  cout << "ELENAME: " << eleName << endl;
}
