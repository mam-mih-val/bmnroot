
/** CbmPsdEventData.cxx
 **@author Alla Maevskaya <alla@inr.ru>
 **@since 3.08.20212
  **/


#include "CbmPsdEventData.h"
#include <iostream>
using std::cout;
using std::endl;


// -----   Default constructor   -------------------------------------------
CbmPsdEventData::CbmPsdEventData() 
{
  fRP = -9999.;
  fB  = -1.;
  fEdepInner = 0;
  fEdepOuter = 0;
  fEdepTotal = 0;
}  
// -----   constructor   -------------------------------------------
//CbmPsdEventData::CbmPsdEventData(const char *name) 
//  : fRP(-9999),
//    fB(-1) 
//{
//  TString namst = "RP_";
//  namst += name;
//  SetName(namst.Data());
//  SetTitle(namst.Data());
  
//}

CbmPsdEventData::CbmPsdEventData(Float_t rp, Float_t b, Float_t edepi, Float_t edepo) 
{ 
  fRP = rp;
  fB  = b;
  fEdepInner = edepi;
  fEdepOuter = edepo;
  fEdepTotal = edepi+edepo;
}



// -----   Destructor   ----------------------------------------------------
CbmPsdEventData::~CbmPsdEventData() { }
// -------------------------------------------------------------------------

void CbmPsdEventData::Print() {
  cout<<"REACTION PLANE : "<<fRP <<" IMPACT PARAMETER "<<fB<<endl;
  cout<<"EDEP INNER     : "<<fEdepInner <<" EDEP OUTER "<<fEdepOuter<<endl;
  cout<<"EDEP TOTAL     : "<<fEdepTotal<<endl;

}

ClassImp(CbmPsdEventData)
