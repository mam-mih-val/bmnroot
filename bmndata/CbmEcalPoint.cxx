// -------------------------------------------------------------------------
// -----                      CbmEcalPoint source file                 -----
// -----                   Created 28/07/04  by V. Friese              -----
// -------------------------------------------------------------------------
#include "CbmEcalPoint.h"

#include "TVector3.h"

#include <iostream>

using std::cout;
using std::endl;

// -----   Default constructor   -------------------------------------------
CbmEcalPoint::CbmEcalPoint() : FairMCPoint() { }
// -------------------------------------------------------------------------



// -----   Standard constructor   ------------------------------------------
CbmEcalPoint::CbmEcalPoint(Int_t trackID, Int_t detID, TVector3 pos, 
			   TVector3 mom, Double_t tof, Double_t length, 
			   Double_t eLoss)
  : FairMCPoint(trackID, detID, pos, mom, tof, length, eLoss) { }
// -------------------------------------------------------------------------


// -----   Destructor   ----------------------------------------------------
CbmEcalPoint::~CbmEcalPoint() { }
// -------------------------------------------------------------------------



// -----   Public method Print   -------------------------------------------
void CbmEcalPoint::Print(const Option_t* opt) const {
  cout << "-I- CbmEcalPoint: ECAL point for track " << fTrackID 
       << " in detector " << fDetectorID << endl;
  cout << "    Position (" << fX << ", " << fY << ", " << fZ
       << ") cm" << endl;
  cout << "    Momentum (" << fPx << ", " << fPy << ", " << fPz
       << ") GeV" << endl;
  cout << "    Time " << fTime << " ns,  Length " << fLength 
       << " cm,  Energy loss " << fELoss*1.0e06 << " keV" << endl;
}
// -------------------------------------------------------------------------


ClassImp(CbmEcalPoint)
