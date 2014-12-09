/*************************************************************************************
 *
 *         Class BmnRecoilGeo
 *         
 *  Adopted for BMN by:   Elena Litvinenko
 *  e-mail:   litvin@nf.jinr.ru
 *  Version:  24-03-2014   
 *
 ************************************************************************************/

#include <iostream>

#include "BmnRecoilGeo.h"
#include "FairGeoNode.h"
#include <iostream>

using std::cout;
using std::endl;

ClassImp(BmnRecoilGeo)

// -----   Default constructor   -------------------------------------------
BmnRecoilGeo::BmnRecoilGeo() {
  // Constructor
  fName="recoil";
  maxSectors=0;
  maxModules=4;
 }
// -------------------------------------------------------------------------

const char* BmnRecoilGeo::getModuleName(Int_t m) {
  // Returns the module name of muo number m

  sprintf(modName,"recoil0%i",m+1);
  return modName;
  cout << "MODNAME: " << modName << endl;
}

const char* BmnRecoilGeo::getEleName(Int_t m) {
  // Returns the element name of muo number m
 
  sprintf(eleName,"s%i",m+1);
  return eleName;
  cout << "ELENAME: " << eleName << endl;
}
