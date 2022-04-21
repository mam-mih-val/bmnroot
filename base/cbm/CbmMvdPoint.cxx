// -------------------------------------------------------------------------
// -----                      CbmMvdPoint source file                  -----
// -----                  Created 06/11/06  by V. Friese               -----
// -------------------------------------------------------------------------
#include "CbmMvdPoint.h"

#include <iostream>

using std::cout;
using std::endl;


// -----   Default constructor   -------------------------------------------
CbmMvdPoint::CbmMvdPoint()
  : FairMCPoint(),
    CbmMvdDetectorId(),
    fX_out(0), 
    fY_out(0), 
    fZ_out(0), 
    fPx_out(0), 
    fPy_out(0), 
    fPz_out(0),
    fPdgCode(0),
    fPointId(-1)
{
}
// -------------------------------------------------------------------------



// -----   Standard constructor   ------------------------------------------
CbmMvdPoint::CbmMvdPoint(Int_t trackID, Int_t pdgCode, Int_t stationNr, 
			 TVector3 posIn, TVector3 posOut, TVector3 momIn, 
			 TVector3 momOut, Double_t tof, Double_t length, 
			 Double_t eLoss) 
  : FairMCPoint(trackID, stationNr, posIn, momIn, tof, length, eLoss),
    CbmMvdDetectorId(),
    fX_out(posOut.X()),
    fY_out(posOut.Y()),
    fZ_out(posOut.Z()),
    fPx_out(momOut.Px()),
    fPy_out(momOut.Py()),
    fPz_out(momOut.Pz()),
    fPdgCode(pdgCode),
    fPointId(-1)
{
  SetLink(FairLink(kMCTrack, trackID));
  fDetectorID = DetectorId(stationNr);
}

// -------------------------------------------------------------------------



// -----   Destructor   ----------------------------------------------------
CbmMvdPoint::~CbmMvdPoint() { }
// -------------------------------------------------------------------------




// -----   Public method Print   -------------------------------------------
void CbmMvdPoint::Print(const Option_t* opt) const {
  cout << "-I- CbmMvdPoint: MVD Point for track " << fTrackID 
       << " in station " << GetStationNr() << endl;
  cout << "    Position (" << fX << ", " << fY << ", " << fZ
       << ") cm" << endl;
  cout << "    Momentum (" << fPx << ", " << fPy << ", " << fPz
       << ") GeV" << endl;
  cout << "    Time " << fTime << " ns,  Length " << fLength 
       << " cm,  Energy loss " << fELoss*1.0e06 << " keV" << endl;
}
// -------------------------------------------------------------------------



ClassImp(CbmMvdPoint)
