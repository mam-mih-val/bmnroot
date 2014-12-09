//------------------------------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------
// -----                      BmnSttPoint source file                  -----
// -------------------------------------------------------------------------


#include <iostream>
#include "BmnSttPoint.h"


//------------------------------------------------------------------------------------------------------------------------
BmnSttPoint::BmnSttPoint() : FairMCPoint() { }
//------------------------------------------------------------------------------------------------------------------------
BmnSttPoint::BmnSttPoint(Int_t trackID, Int_t detID, TVector3 pos, Double_t radius, TVector3 mom, Double_t tof, 
			Double_t length, Double_t eLoss, Int_t isPrimary, Double_t charge, Int_t pdgId)
                        : FairMCPoint(trackID, detID, pos, mom, tof, length, eLoss),
			fIsPrimary(isPrimary), fCharge(charge), fRadius(radius), fPdgId(pdgId)
{ 
    //fTX = trackPos.X();
    //fTY = trackPos.Y();
    //fTZ = trackPos.Z();    
    fTX = pos.X();
    fTY = pos.Y();
    fTZ = pos.Z();    
}
//------------------------------------------------------------------------------------------------------------------------
BmnSttPoint::~BmnSttPoint() { }
//------------------------------------------------------------------------------------------------------------------------
void BmnSttPoint::Print(const Option_t* opt) const 
{
	cout 	<< "-I- BmnSttPoint: Stt point for track " << fTrackID 
       		<< " in detector " << fDetectorID << endl;
	cout 	<< "    Position (" << fX << ", " << fY << ", " << fZ
       		<< ") cm" << endl;
	cout 	<< "    Momentum (" << fPx << ", " << fPy << ", " << fPz
       		<< ") GeV" << endl;
	cout 	<< "    Time " << fTime << " ns,  Length " << fLength 
       		<< " cm,  Energy loss " << fELoss*1.0e06 << " keV" << endl;
}
//------------------------------------------------------------------------------------------------------------------------
ClassImp(BmnSttPoint)
