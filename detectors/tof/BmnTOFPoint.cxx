//--------------------------------------------------------------------------------------------------------------------------------------
#include <iostream>

#include "BmnTOFPoint.h"

using namespace std;

ClassImp(BmnTOFPoint)
//--------------------------------------------------------------------------------------------------------------------------------------
BmnTOFPoint::BmnTOFPoint()  : FairMCPoint() { }
//--------------------------------------------------------------------------------------------------------------------------------------
BmnTOFPoint::BmnTOFPoint(Int_t trackID, Int_t detID, TVector3 pos, TVector3 mom, Double_t tof, Double_t length, Double_t eLoss) 
 : FairMCPoint(trackID, detID, pos, mom, tof, length, eLoss) 
{ }
//--------------------------------------------------------------------------------------------------------------------------------------
BmnTOFPoint::~BmnTOFPoint() { }
//--------------------------------------------------------------------------------------------------------------------------------------
void BmnTOFPoint::Print(const Option_t* opt) const 
{
	cout<<"-I- BmnTOFPoint: trackId "<<fTrackID<<" detectorUID "<<fDetectorID;
	cout<<"\n    Position ("<<fX<<", "<<fY<<", "<<fZ<< ") cm";
	cout<<"\n    Momentum ("<<fPx<<", "<<fPy<<", "<<fPz<<") GeV";
	cout<<"\n    Time "<<fTime<<" ns,  Length "<<fLength<<" cm,  Energy loss "<<fELoss*1.0e06<<" keV.\n";
}
//--------------------------------------------------------------------------------------------------------------------------------------

