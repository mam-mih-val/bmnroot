/////////////////////////////////////////////////////////////
//
//  BmnNdetPoint
//
//  Geant point for Hyp detector
//
//  Created 14/08/06  by S.Spataro
//
//  Modified by M.Golubeva July 2022
//
///////////////////////////////////////////////////////////////

#include <iostream>
#include "BmnNdetPoint.h"

// -----   Default constructor   -------------------------------------------
BmnNdetPoint::BmnNdetPoint() : FairMCPoint() {
//   fTrackID    = -1;
//   fDetectorID = -1;
//   //  fEventID    = -1;
//   fX          = fY  = fZ =  0.;
//   fPx         = fPy = fPz = 0.;
//   fTime       =  0.;
//   fLength     =  0.;
//   fELoss      =  0.;
//   fEventId
 
  nCopy = -1;
  nCopyMother=-1; //module
//fNHits      =  0.;
}

// -----   Standard constructor   ------------------------------------------
BmnNdetPoint::BmnNdetPoint(Int_t trackID, Int_t detID, Int_t copyNo, Int_t copyNoMother, TVector3 pos,TVector3 mom, Double_t tof, Double_t length,Double_t eLoss, UInt_t EventId) 
  : FairMCPoint(trackID, detID, pos, mom, tof, length, eLoss, EventId) {
  nCopy = copyNo;
  nCopyMother =  copyNoMother;
  //fNHits = nHits;

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
  //  fNHits      = nHits;
  //  fEventId
 
}

// -----   Destructor   ----------------------------------------------------
BmnNdetPoint::~BmnNdetPoint() { }

// -----   Public method Print   -------------------------------------------
void BmnNdetPoint::Print(const Option_t* opt) const {
  cout << "-I- BmnNdetPoint: MUO Point for track " << fTrackID 
       << " in detector " << fDetectorID <<" " <<nCopyMother <<" " <<nCopy << endl;
  cout << "    Position (" << fX << ", " << fY << ", " << fZ
       << ") cm" << endl;
  cout << "    Momentum (" << fPx << ", " << fPy << ", " << fPz
       << ") GeV" << endl;
  cout << "    Time " << fTime << " ns,  Length " << fLength 
       << " cm,  Energy loss " << fELoss << " GeV " << endl;
  //<< " cm,  Energy loss " << fELoss << " GeV" <<", nHits " <<fNHits << endl;
}


ClassImp(BmnNdetPoint)
