
/** CbmPsdHit.cxx
 **@author Alla Maevskaya <alla@inr.ru>
 **@since 3.08.20212
  **/


#include "CbmPsdHit.h"
#include <iostream>
using std::cout;
using std::endl;


// -----   Default constructor   -------------------------------------------
CbmPsdHit::CbmPsdHit() 
  : TObject(),
    fModuleID(-1) 
{
  
      fEdep=0;
  
}
CbmPsdHit::CbmPsdHit(Int_t module, Float_t edep) 
  : TObject(),
    fModuleID(module)
{
    fEdep = 0; 
    if (module >=0 && module < 104)
	fEdep = edep;
    else
	cout<<"Wrong module ID << " << module  <<  endl;
  
}



// -----   Destructor   ----------------------------------------------------
CbmPsdHit::~CbmPsdHit() { }
// -------------------------------------------------------------------------

void CbmPsdHit::Print() {
  cout<<"PSD module : "<<fModuleID <<" ELoss "<<fEdep  <<  endl;

}

ClassImp(CbmPsdHit)
