// -------------------------------------------------------------------------
// -----                      BmnSsdPoint source file                  -----
// -----                  Created 13.12.2018  by D. Baranov               -----
// -------------------------------------------------------------------------

#include "BmnSsdPoint.h"

#include "FairLogger.h"

#include <sstream>

using std::endl;
using std::string;
using std::stringstream;

// -----   Default constructor   -------------------------------------------
BmnSsdPoint::BmnSsdPoint()
  : FairMCPoint(),
    fX_out(0.),
    fY_out(0.),
    fZ_out(0.),
    fPx_out(0.),
    fPy_out(0.),
    fPz_out(0.),
    fPid(0),
    fIndex(0),
    fFlag(0)
{
}
// -------------------------------------------------------------------------



// -----   Standard constructor   ------------------------------------------
BmnSsdPoint::BmnSsdPoint(Int_t trackID, Int_t detID, TVector3 posIn,
			 TVector3 posOut, TVector3 momIn, TVector3 momOut,
			 Double_t tof, Double_t length, Double_t eLoss,
			 Int_t pid, Int_t eventId, Int_t index, Short_t flag)
  : FairMCPoint(trackID, detID, posIn, momIn, tof, length, eLoss, eventId),
    fX_out(posOut.X()),
    fY_out(posOut.Y()),
    fZ_out(posOut.Z()),
    fPx_out(momOut.Px()),
    fPy_out(momOut.Py()),
    fPz_out(momOut.Pz()),
    fPid(pid),
    fIndex(index),
    fFlag(flag)
{
  SetLink(FairLink(kMCTrack, trackID));
}
// -------------------------------------------------------------------------



// -----   Destructor   ----------------------------------------------------
BmnSsdPoint::~BmnSsdPoint() { }
// -------------------------------------------------------------------------



// -----   Copy constructor with event and epoch time   --------------------
BmnSsdPoint::BmnSsdPoint(const BmnSsdPoint& point, Int_t eventId,
			 Double_t eventTime, Double_t epochTime)
  : FairMCPoint(point),
    fX_out(point.fX_out),
    fY_out(point.fY_out),
    fZ_out(point.fZ_out),
    fPx_out(point.fPx_out),
    fPy_out(point.fPy_out),
    fPz_out(point.fPz_out),
    fPid(point.fPid),
    fIndex(point.fIndex),
    fFlag(point.fFlag)

{
  //  *this = point;
  if ( eventId > 0 ) fEventId = eventId;
  fTime = point.GetTime() + eventTime - epochTime;
}
// -------------------------------------------------------------------------



// -----   Point x coordinate from linear extrapolation   ------------------
Double_t BmnSsdPoint::GetX(Double_t z) const {
  if ( (fZ_out-z)*(fZ-z) >= 0. ) return (fX_out+fX)/2.;
  Double_t dz = fZ_out - fZ;
  return ( fX + (z-fZ) / dz * (fX_out-fX) );
}
// -------------------------------------------------------------------------



// -----   Point y coordinate from linear extrapolation   ------------------
Double_t BmnSsdPoint::GetY(Double_t z) const {
  if ( (fZ_out-z)*(fZ-z) >= 0. ) return (fY_out+fY)/2.;
  Double_t dz = fZ_out - fZ;
  //  if ( TMath::Abs(dz) < 1.e-3 ) return (fY_out+fY)/2.;
  return ( fY + (z-fZ) / dz * (fY_out-fY) );
}
// -------------------------------------------------------------------------



// -----   Public method IsUsable   ----------------------------------------
Bool_t BmnSsdPoint::IsUsable() const {
  Double_t dz = fZ_out - fZ;
  if ( TMath::Abs(dz) < 1.e-4 ) return kFALSE;
  return kTRUE;
}
// -------------------------------------------------------------------------



// -----   String output   -------------------------------------------------
string BmnSsdPoint::ToString() const
{
   stringstream ss;
   ss << "SsdPoint: track ID " << fTrackID << ", detector ID "
      << fDetectorID << endl;
   ss << "          IN  Position (" << fX << ", " << fY
      << ", " << fZ << ") cm" << endl;
   ss << "          OUT Position (" << fX_out << ", " << fY_out
      << ", " << fZ_out << ") cm" << endl;
   ss << "    Momentum (" << fPx << ", " << fPy << ", " << fPz
      << ") GeV" << endl;
   ss << "    Time " << fTime << " ns,  Length " << fLength
       << " cm,  Energy loss " << fELoss*1.0e06 << " keV" << endl;
   return ss.str();
}
// -------------------------------------------------------------------------


ClassImp(BmnSsdPoint)
