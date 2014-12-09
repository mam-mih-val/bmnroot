/** CbmPsdDigi.cxx
 **@author Alla Maevskaya <alla@inr.ru>
 **@since 3.08.20212
  **/


#include "CbmPsdDigi.h"
#include <iostream>
using std::cout;
using std::endl;


// -----   Default constructor   -------------------------------------------

CbmPsdDigi::CbmPsdDigi() 
  : TObject(),
    fModuleID(-1),
    fSectionID(-1)   
{
      fEdep=-1;
}


CbmPsdDigi::CbmPsdDigi(Int_t section, Int_t module, Float_t edep) 
  : TObject(),
    fModuleID(module),
    fSectionID(section)    
{
      fEdep = 0;
      if (section >= 0 && section < 66 && module >= 0 && module < 104)
        fEdep = edep;
      else   
	cout << "Wrong module ID " << fModuleID << " or scintillator number " << section <<  endl;
}



// -----   Destructor   ----------------------------------------------------
CbmPsdDigi::~CbmPsdDigi() { }
// -------------------------------------------------------------------------

void CbmPsdDigi::Print() {
  cout<<"PSD module : "<<fModuleID <<" section    : "<<fSectionID <<
    " ELoss "<<fEdep  <<  endl;


}


ClassImp(CbmPsdDigi)
