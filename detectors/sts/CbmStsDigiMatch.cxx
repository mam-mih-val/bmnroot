//* $Id: */

// -------------------------------------------------------------------------
// -----                    CbmStsDigiMatch source file                -----
// -----                  Created 31/08/06  by V. Friese               -----
// -------------------------------------------------------------------------

#include "CbmStsDigiMatch.h"

#include <iostream>

using std::cout;
using std::endl;


// -----   Default onstructor   --------------------------------------------
CbmStsDigiMatch::CbmStsDigiMatch() { 
  fRefIndex[0] = fRefIndex[1] = fRefIndex[2] = -1;
};

// -------------------------------------------------------------------------



// -----   Standard constructor   ------------------------------------------
CbmStsDigiMatch::CbmStsDigiMatch(Int_t iPoint) { 
  if ( iPoint < 0 && iPoint != -666 ) {
    cout << "-W- CbmStsDigiMatch: Illegal StsPoint index " 
	 << iPoint << endl;
    fRefIndex[0] = -1;
  }
  else fRefIndex[0] = iPoint;
  fRefIndex[1] = fRefIndex[2] = -1;
};
// -------------------------------------------------------------------------



// -----   Destructor   ----------------------------------------------------
CbmStsDigiMatch::~CbmStsDigiMatch() { };
// -------------------------------------------------------------------------



// -----   Public method AddPoint   ----------------------------------------
Int_t CbmStsDigiMatch::AddPoint(Int_t iPoint) {
  if ( iPoint < 0 ) {
    cout << "-W- CbmStsDigiMatch::AddPoint: Illegal StsPoint index " 
	 << iPoint << endl;
    return 0;
  }
  for (Int_t i=0; i<3; i++) {
    if ( fRefIndex[i] == -1 ) {
      fRefIndex[i] = iPoint;
      return i+1;
    }
  }
  return 4;
}
// -------------------------------------------------------------------------

  

// -----   Public method GetRefIndex   -------------------------------------
Int_t CbmStsDigiMatch::GetRefIndex(Int_t i) const {
  if ( i<0 || i>2 ) {
    cout << "-W- CbmStsDigiMatch::GetRefIndex: Illegal index number "
	 << i << endl;
    return -1;
  }
  return fRefIndex[i];
}
// -------------------------------------------------------------------------



ClassImp(CbmStsDigiMatch)
