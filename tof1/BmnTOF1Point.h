//--------------------------------------------------------------------------------------------------------------------------------------
#ifndef __BMNTOF1POINT_H
#define __BMNTOF1POINT_H 1

#include<TVector3.h>

#include "FairMCPoint.h"

//--------------------------------------------------------------------------------------------------------------------------------------
class BmnTOF1Point : public FairMCPoint
{
public:
	BmnTOF1Point();
	virtual ~BmnTOF1Point();

  /** Constructor with arguments
   *@param trackID  Index of MCTrack
   *@param detID    Detector ID
   *@param pos      Ccoordinates at entrance to active volume [cm]
   *@param mom      Momentum of track at entrance [GeV]
   *@param tof      Time since event start [ns]
   *@param length   Track length since creation [cm]
   *@param eLoss    Energy deposit [GeV]
   **/ 
  	BmnTOF1Point(Int_t trackID, Int_t detID, TVector3 pos, TVector3 mom, Double_t tof, Double_t length, Double_t eLoss);
	BmnTOF1Point(const BmnTOF1Point& point) { *this = point; };
  
	virtual void 	Print(const Option_t* opt) const;

	// FIXME: padID MAX_VALUE = 255, moduleID MAX_VALUE = 255
  	Int_t 		GetPad() const   {return (fDetectorID & 0xFF);};
  	Int_t 		GetModule() const {return ((fDetectorID>>8) & 0xFF);};
   	Int_t 		GetDetectorID() const {return fDetectorID;};

	static Int_t 	GetPad(Int_t uid){ return (uid & 0xFF); };
   	static Int_t 	GetModule(Int_t uid){ return ((uid>>8) & 0xFF); };
  	static Int_t 	GetVolumeUID(Int_t modID, Int_t padID) { return (modID<<8) + padID; };
 
ClassDef(BmnTOF1Point, 1)
};

#endif
//--------------------------------------------------------------------------------------------------------------------------------------
