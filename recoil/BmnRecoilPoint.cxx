/////////////////////////////////////////////////////////////
//
//  BmnRecoilPoint
//
//  Geant point for Hyp detector
//
//  Created 14/08/06  by S.Spataro
//
///////////////////////////////////////////////////////////////

#include <iostream>
#include "BmnRecoilPoint.h"

// -----   Default constructor   -------------------------------------------
BmnRecoilPoint::BmnRecoilPoint() : FairMCPoint() {
//   fTrackID    = -1;
//   fDetectorID = -1;
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
BmnRecoilPoint::BmnRecoilPoint(Int_t trackID, Int_t detID, Int_t copyNo, Int_t copyNoMother, TVector3 pos,
                        TVector3 mom, Double_t tof, Double_t length,
			 Double_t eLoss, UInt_t EventId) 
  : FairMCPoint(trackID, detID, pos, mom, tof, length, eLoss, EventId) {
  nCopy = copyNo;
  nCopyMother =  copyNoMother;
  //  fTrackID    = trackID;
  //  fDetectorID = detID; 
  
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
BmnRecoilPoint::~BmnRecoilPoint() { }
// -------------------------------------------------------------------------

// -----   Public method Print   -------------------------------------------
void BmnRecoilPoint::Print(const Option_t* opt) const {
  cout << "-I- BmnRecoilPoint: MUO Point for track " << fTrackID 
       << " in detector " << fDetectorID << endl;
  cout << "    Position (" << fX << ", " << fY << ", " << fZ
       << ") cm" << endl;
  cout << "    Momentum (" << fPx << ", " << fPy << ", " << fPz
       << ") GeV" << endl;
  cout << "    Time " << fTime << " ns,  Length " << fLength 
       << " cm,  Energy loss " << fELoss*1.0e06 << " keV" << endl;
}
// -------------------------------------------------------------------------



ClassImp(BmnRecoilPoint)
