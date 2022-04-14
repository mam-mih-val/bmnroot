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
  
	virtual void 	Print(const Option_t* opt) const;

	// CATION: stripID MAX_VALUE = 255, moduleID MAX_VALUE = 255, regionID MAX_VALUE = 255 
  	Int_t 		GetStrip() const  {return (fDetectorID & 0x000000FF);};
  	Int_t 		GetModule() const {return (fDetectorID & 0x0000FF00) >> 8; };
   	Int_t 		GetRegion() const {return (fDetectorID & 0x00FF0000) >> 16;}; 	
   	Int_t 		GetVolumeUID() const {return fDetectorID;};

	static Int_t 	GetStrip(Int_t uid){  return (uid & 0x000000FF); };
   	static Int_t 	GetModule(Int_t uid){ return (uid & 0x0000FF00) >> 8; };
   	static Int_t 	GetRegion(Int_t uid){ return (uid & 0x00FF0000) >> 16; };  	
  	static Int_t 	GetVolumeUID(Int_t regID, Int_t modID, Int_t stripID) 
  	{ 
#ifdef DEBUG
 	Int_t uid = (regID << 16) | (modID << 8) | stripID; 
 	Int_t region =	GetRegion(uid);
 	Int_t module =	GetModule(uid);
  	Int_t strip =	GetStrip(uid);	
assert(region == regID);  
assert(module == modID); 
assert(strip == stripID); 	
	return uid;
#else  	
  	return (regID << 16) | (modID << 8) | stripID; 
#endif  	
  	};
 
ClassDef(BmnTOF1Point, 1)
};

#endif
//--------------------------------------------------------------------------------------------------------------------------------------
