#include "BmnSSDPoint.h"

#include <iostream>

using namespace std;

BmnSSDPoint::BmnSSDPoint()
: FairMCPoint(),
  fX_out(0.0),
  fY_out(0.0),
  fZ_out(0.0),
  fPx_out(0.0),
  fPy_out(0.0),
  fPz_out(0.0),
  fIsPrimary(0), fCharge(-1.0), fPdgId(0) {

}

BmnSSDPoint::BmnSSDPoint(Int_t trackID, Int_t detID,
                         TVector3 posIn, TVector3 posOut,
                         TVector3 momIn, TVector3 momOut,
                         Double_t tof, Double_t length, Double_t eLoss,
                         Int_t isPrimary, Double_t charge, Int_t pdgId)

: FairMCPoint(trackID, detID, posIn, momIn, tof, length, eLoss),
  fX_out(posOut.X()),
  fY_out(posOut.Y()),
  fZ_out(posOut.Z()),
  fPx_out(momOut.Px()),
  fPy_out(momOut.Py()),
  fPz_out(momOut.Pz()),
  fIsPrimary(isPrimary), fCharge(charge), fPdgId(pdgId) {

}

BmnSSDPoint::~BmnSSDPoint() { }

void BmnSSDPoint::Print(const Option_t* opt) const {
    cout << "-I- BmnSSDPoint: SSD point for track " << fTrackID << " in detector " << fDetectorID << endl;
    cout << "    Position (" << fX << ", " << fY << ", " << fZ << ") cm" << endl;
    cout << "    Momentum (" << fPx << ", " << fPy << ", " << fPz << ") GeV" << endl;
    cout << "    Time " << fTime << " ns,  Length " << fLength << " cm,  Energy loss " << fELoss*1.0e06 << " keV" << endl;
}

ClassImp(BmnSSDPoint)


