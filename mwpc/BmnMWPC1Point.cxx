//------------------------------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------
// -----                      BmnMWPC1Point source file                  -----
// -------------------------------------------------------------------------


#include <iostream>
#include "BmnMWPC1Point.h"


//------------------------------------------------------------------------------------------------------------------------
BmnMWPC1Point::BmnMWPC1Point() : FairMCPoint() { }
//------------------------------------------------------------------------------------------------------------------------
BmnMWPC1Point::BmnMWPC1Point(Int_t trackID, Int_t detID, TVector3 pos, Double_t radius, TVector3 mom, Double_t tof, 
			Double_t length, Double_t eLoss, Int_t isPrimary, Double_t charge, Int_t pdgId, TVector3 trackPos)
                        : FairMCPoint(trackID, detID, pos, mom, tof, length, eLoss),
			fIsPrimary(isPrimary), fCharge(charge), fRadius(radius), fPdgId(pdgId)
{ 
    fTX = trackPos.X();
    fTY = trackPos.Y();
    fTZ = trackPos.Z();    
}
//------------------------------------------------------------------------------------------------------------------------
BmnMWPC1Point::~BmnMWPC1Point() { }
//------------------------------------------------------------------------------------------------------------------------
Double_t BmnMWPC1Point::GetDistance() {
    TVector3 dist = TVector3(fX, fY, fZ) - TVector3(fTX,fTY,fTZ);
    return dist.Mag();
}
//------------------------------------------------------------------------------------------------------------------------
void BmnMWPC1Point::Print(const Option_t* opt) const 
{
	cout 	<< "-I- BmnMWPC1Point: MWPC1 point for track " << fTrackID 
       		<< " in detector " << fDetectorID << endl;
	cout 	<< "    Position (" << fX << ", " << fY << ", " << fZ
       		<< ") cm" << endl;
	cout 	<< "    Momentum (" << fPx << ", " << fPy << ", " << fPz
       		<< ") GeV" << endl;
	cout 	<< "    Time " << fTime << " ns,  Length " << fLength 
       		<< " cm,  Energy loss " << fELoss*1.0e06 << " keV" << endl;
}
//------------------------------------------------------------------------------------------------------------------------
ClassImp(BmnMWPC1Point)
