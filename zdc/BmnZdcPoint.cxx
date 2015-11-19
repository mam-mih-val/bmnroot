/////////////////////////////////////////////////////////////
//
//  BmnZdcPoint
//
//  Geant point for Hyp detector
//
//  Created 14/08/06  by S.Spataro
//
///////////////////////////////////////////////////////////////

#include <iostream>
#include "BmnZdcPoint.h"

// -----   Default constructor   -------------------------------------------
BmnZdcPoint::BmnZdcPoint() : FairMCPoint() {
//   fTrackID    = -1;
//   fGroupID = -1;
//   //  fEventID    = -1;
//   fX          = fY  = fZ =  0.;
//   fPx         = fPy = fPz = 0.;
//   fTime       =  0.;
//   fLength     =  0.;
//   fELoss      =  0.;
 
  nCopy = -1;
  nCopyMother=-1;
}
// -------------------------------------------------------------------------



// -----   Standard constructor   ------------------------------------------
BmnZdcPoint::BmnZdcPoint(Int_t trackID, Int_t module_groupID, Int_t copyNo, Int_t copyNoMother, TVector3 pos,
                        TVector3 mom, Double_t tof, Double_t length,
			 Double_t eLoss, UInt_t EventId) 
  : FairMCPoint(trackID, module_groupID, pos, mom, tof, length, eLoss, EventId) {
  nCopy = copyNo;
  nCopyMother =  copyNoMother;
  //  fTrackID    = trackID;
  //  fGroupID = module_groupID; 
  
  //  fX          = pos.X();
  //  fY          = pos.Y();
  //  fZ          = pos.Z();
  //  fPx         = mom.Px();
  //  fPy         = mom.Py();
  //  fPz         = mom.Pz();
  //  fTime       = tof;
  //  fLength     = length;
  //  fELoss      = eLoss;
 
}


// -----   Destructor   ----------------------------------------------------
BmnZdcPoint::~BmnZdcPoint() { }
// -------------------------------------------------------------------------

// -----   Public method Print   -------------------------------------------
void BmnZdcPoint::Print(const Option_t* opt) const {
  cout << "-I- BmnZdcPoint: MUO Point for track " << fTrackID 
       << " in detector " << GetDetectorID() << endl;
  cout << "    Position (" << fX << ", " << fY << ", " << fZ
       << ") cm" << endl;
  cout << "    Momentum (" << fPx << ", " << fPy << ", " << fPz
       << ") GeV" << endl;
  cout << "    Time " << fTime << " ns,  Length " << fLength 
       << " cm,  Energy loss " << fELoss*1.0e06 << " keV" << endl;
}
// -------------------------------------------------------------------------



ClassImp(BmnZdcPoint)
