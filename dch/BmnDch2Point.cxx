//------------------------------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------
// -----                      BmnDch2Point source file                  -----
// -------------------------------------------------------------------------


#include <iostream>
#include "BmnDch2Point.h"


//------------------------------------------------------------------------------------------------------------------------
BmnDch2Point::BmnDch2Point() : FairMCPoint() { }
//------------------------------------------------------------------------------------------------------------------------
BmnDch2Point::BmnDch2Point(Int_t trackID, Int_t detID, TVector3 pos, Double_t radius, TVector3 mom, Double_t tof, 
			Double_t length, Double_t eLoss, Int_t isPrimary, Double_t charge, Int_t pdgId, TVector3 trackPos)
                        : FairMCPoint(trackID, detID, pos, mom, tof, length, eLoss),
			fIsPrimary(isPrimary), fCharge(charge), fRadius(radius), fPdgId(pdgId)
{ 
    fTX = trackPos.X();
    fTY = trackPos.Y();
    fTZ = trackPos.Z();    
}
//------------------------------------------------------------------------------------------------------------------------
BmnDch2Point::~BmnDch2Point() { }
//------------------------------------------------------------------------------------------------------------------------
Double_t BmnDch2Point::GetDistance() {
    TVector3 dist = TVector3(fX, fY, fZ) - TVector3(fTX,fTY,fTZ);
    return dist.Mag();
}
//------------------------------------------------------------------------------------------------------------------------
void BmnDch2Point::Print(const Option_t* opt) const 
{
	cout 	<< "-I- BmnDch2Point: DCH2 point for track " << fTrackID 
       		<< " in detector " << fDetectorID << endl;
	cout 	<< "    Position (" << fX << ", " << fY << ", " << fZ
       		<< ") cm" << endl;
	cout 	<< "    Momentum (" << fPx << ", " << fPy << ", " << fPz
       		<< ") GeV" << endl;
	cout 	<< "    Time " << fTime << " ns,  Length " << fLength 
       		<< " cm,  Energy loss " << fELoss*1.0e06 << " keV" << endl;
}
//------------------------------------------------------------------------------------------------------------------------
ClassImp(BmnDch2Point)
