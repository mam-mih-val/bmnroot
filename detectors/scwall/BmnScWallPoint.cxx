#include "BmnScWallPoint.h"

#include <iostream>

#include "FairLogger.h"
#include "FairMCPoint.h"
#include "TParticle.h"
#include "TVector3.h"
#include "TVirtualMC.h"

// -----   Default constructor   -------------------------------------------
BmnScWallPoint::BmnScWallPoint()
  : FairMCPoint(),
    fX(0.0),
    fY(0.0),
    fZ(0.0),
    fPx(0.0),
    fPy(0.0),
    fPz(0.0) {
  nCopy = -1;
  nCopyMother=-1; 
}

// -----   Standard constructor   ------------------------------------------
BmnScWallPoint::BmnScWallPoint(Int_t trackID, Int_t detID, Int_t copyNo, Int_t copyNoMother, TVector3 pos, TVector3 mom, Double_t tof, Double_t length, Double_t eLoss, UInt_t EventId)
  : FairMCPoint(trackID, detID, pos, mom, tof, length, eLoss, EventId),
    fX(pos.X()),
    fY(pos.Y()),
    fZ(pos.Z()),
    fPx(mom.Px()),
    fPy(mom.Py()),
    fPz(mom.Pz()) {
  nCopy = copyNo;
  nCopyMother =  copyNoMother;
}

// -----   Destructor   ----------------------------------------------------
BmnScWallPoint::~BmnScWallPoint() {}

// -----   Public method Print   -------------------------------------------
void BmnScWallPoint::Print(const Option_t* opt) const {
  cout << "-I- BmnScWallPoint: MUO Point for track " << fTrackID  
       << " in detector " << fDetectorID <<" " <<nCopyMother <<" " <<nCopy << endl;
  cout << "    Position (" << fX << ", " << fY << ", " << fZ
       << ") cm" << endl;
  cout << "    Momentum (" << fPx << ", " << fPy << ", " << fPz
       << ") GeV" << endl;
  cout << "    Time " << fTime << " ns,  Length " << fLength
       << " cm,  Energy loss " << fELoss * 1.0e06 << " keV" << endl;
}

// -------------------------------------------------------------------------

ClassImp(BmnScWallPoint)
