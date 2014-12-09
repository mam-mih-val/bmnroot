//------------------------------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------
// -----                     BmnMWPC1Point header file                   -----
// -------------------------------------------------------------------------

#ifndef BMNMWPC1POINT_H
#define BMNMWPC1POINT_H


#include "TObject.h"
#include "TVector3.h"
#include "FairMCPoint.h"

using namespace std;

//------------------------------------------------------------------------------------------------------------------------
class BmnMWPC1Point : public FairMCPoint
{

public:

  /** Constructor with arguments
   *@param trackID  Index of MCTrack
   *@param detID    Detector ID
   *@param pos      Ccoordinates at entrance to active volume [cm]
   *@param mom      Momentum of track at entrance [GeV]
   *@param tof      Time since event start [ns]
   *@param length   Track length since creation [cm]
   *@param eLoss    Energy deposit [GeV]
   **/
  	BmnMWPC1Point(Int_t trackID, Int_t detID, TVector3 pos, Double_t radius, TVector3 mom, Double_t tof, 
  	    Double_t length, Double_t eLoss, Int_t isPrimary, Double_t charge, Int_t pdgId, TVector3 trackPos);
  	BmnMWPC1Point(const BmnMWPC1Point& point) { *this = point; };

  	BmnMWPC1Point();	
  	virtual ~BmnMWPC1Point();

	Double_t GetDistance(); // DCA between track and straw
	Double_t GetTrackX() { return fTX; }
	Double_t GetTrackY() { return fTY; }
	Double_t GetTrackZ() { return fTZ; }
	Double_t GetPdgId() { return fPdgId; }
	Double_t GetCharge() { return fCharge; }
	Double_t GetPhi() const { return fPhi; } //AZ
        Int_t GetIsPrimary() { return fIsPrimary; } 
	void SetPhi(Double_t phi) { fPhi = phi; } //AZ

	// Output to screen 
  	virtual void Print(const Option_t* opt) const;
protected:
    Int_t fIsPrimary;
    Double_t fCharge; 
    Double_t fRadius;
    Int_t fPdgId;
    Double_t fTX, fTY, fTZ; // track coordinates at DCA to straw
    Double_t fPhi; // tube rotation angle - AZ (interim solution)

  ClassDef(BmnMWPC1Point,2)

};

//------------------------------------------------------------------------------------------------------------------------
#endif





