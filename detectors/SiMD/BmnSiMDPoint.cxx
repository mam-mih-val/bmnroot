

#include <iostream>
#include "BmnSiMDPoint.h"
#include "FairMCPoint.h"
#include "FairLogger.h"
#include "TVector3.h" 
#include "TVirtualMC.h"
#include "TParticle.h"

// -----   Default constructor   -------------------------------------------
BmnSiMDPoint::BmnSiMDPoint() 
	: FairMCPoint(), 
  fXOut(0.0),
  fYOut(0.0),
  fZOut(0.0),
  fPxOut(0.0),
  fPyOut(0.0),
  fPzOut(0.0),
  fIsPrimary(0), fCharge(-1.0), fPdgId(0)
  
  {
  nCopy = -1;
		
}
// -------------------------------------------------------------------------

// -----   Standard constructor   ------------------------------------------
BmnSiMDPoint::BmnSiMDPoint(Int_t trackID, Int_t detID, Int_t copyNo,
						 TVector3 posIn, TVector3 posOut,
                         TVector3 momIn, TVector3 momOut,
						 Double_t tof, Double_t length,
			 Double_t eLoss,  Int_t isPrimary, Double_t charge, Int_t pdgId, 
			 Double_t timeIn, Double_t timeOut, Double_t lengthtrack)
  //UInt_t EventId,
	: FairMCPoint(trackID, detID, posIn, momIn, tof, length, eLoss),
		//EventId
 
  fXOut(posOut.X()),
  fYOut(posOut.Y()),
  fZOut(posOut.Z()),
  fPxOut(momOut.Px()),
  fPyOut(momOut.Py()),
  fPzOut(momOut.Pz()),
  fIsPrimary(isPrimary), 
  fCharge(charge), 
  fPdgId(pdgId) 
  
  {
   
	nCopy = copyNo;

	fTimeIn = timeIn;
	fTimeOut = timeOut;
    fLengthtrack = lengthtrack;   
  }
// -----   Destructor   ----------------------------------------------------
BmnSiMDPoint::~BmnSiMDPoint() { }
// -------------------------------------------------------------------------

// -----   Public method Print   -------------------------------------------
void BmnSiMDPoint::Print(const Option_t* opt) const {
  cout << "-I- BmnSiMDPoint: MUO Point for track " << fTrackID 
       << " in detector " << fDetectorID << endl;
  cout << "    Position (" << fX << ", " << fY << ", " << fZ
       << ") cm" << endl;
  cout << "    Momentum (" << fPx << ", " << fPy << ", " << fPz
       << ") GeV" << endl;
  cout << "    Time " << fTime << " ns,  Length " << fLength 
       << " cm,  Energy loss " << fELoss*1.0e06 << " keV" << endl;
}
// -------------------------------------------------------------------------


ClassImp(BmnSiMDPoint)
