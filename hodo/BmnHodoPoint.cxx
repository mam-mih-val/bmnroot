#include "BmnHodoPoint.h"

#include <iostream>

#include "FairLogger.h"
#include "FairMCPoint.h"
#include "TParticle.h"
#include "TVector3.h"
#include "TVirtualMC.h"

// -----   Default constructor   -------------------------------------------
BmnHodoPoint::BmnHodoPoint()
    : FairMCPoint(),
      fX(0.0),
      fY(0.0),
      fZ(0.0),
      fPx(0.0),
      fPy(0.0),
      fPz(0.0) {
}
// -------------------------------------------------------------------------

// -----   Standard constructor   ------------------------------------------
BmnHodoPoint::BmnHodoPoint(Int_t trackID, Int_t detID, TVector3 pos, TVector3 mom, Double_t tof, Double_t length, Double_t eLoss)
    : FairMCPoint(trackID, detID, pos, mom, tof, length, eLoss),
      fX(pos.X()),
      fY(pos.Y()),
      fZ(pos.Z()),
      fPx(mom.Px()),
      fPy(mom.Py()),
      fPz(mom.Pz()) {
}
// -----   Destructor   ----------------------------------------------------
BmnHodoPoint::~BmnHodoPoint() {}
// -------------------------------------------------------------------------

// -----   Public method Print   -------------------------------------------
void BmnHodoPoint::Print(const Option_t* opt) const {
    cout << "-I- BmnHodoPoint: MUO Point for track " << fTrackID
         << " in detector " << fDetectorID << endl;
    cout << "    Position (" << fX << ", " << fY << ", " << fZ
         << ") cm" << endl;
    cout << "    Momentum (" << fPx << ", " << fPy << ", " << fPz
         << ") GeV" << endl;
    cout << "    Time " << fTime << " ns,  Length " << fLength
         << " cm,  Energy loss " << fELoss * 1.0e06 << " keV" << endl;
}
// -------------------------------------------------------------------------

ClassImp(BmnHodoPoint)
