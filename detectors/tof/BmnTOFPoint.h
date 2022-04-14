//--------------------------------------------------------------------------------------------------------------------------------------
#ifndef __BMNTOFPOINT_H
#define __BMNTOFPOINT_H 1

#include<TVector3.h>

#include "FairMCPoint.h"

//--------------------------------------------------------------------------------------------------------------------------------------
class BmnTOFPoint : public FairMCPoint
{
public:
	BmnTOFPoint();
	virtual ~BmnTOFPoint();

  /** Constructor with arguments
   *@param trackID  Index of MCTrack
   *@param detID    Detector ID
   *@param pos      Ccoordinates at entrance to active volume [cm]
   *@param mom      Momentum of track at entrance [GeV]
   *@param tof      Time since event start [ns]
   *@param length   Track length since creation [cm]
   *@param eLoss    Energy deposit [GeV]
   **/ 
  	BmnTOFPoint(Int_t trackID, Int_t detID, TVector3 pos, TVector3 mom, Double_t tof, Double_t length, Double_t eLoss);
  
	virtual void 	Print(const Option_t* opt) const;

	// CATION: stripID MAX_VALUE = 255, moduleID MAX_VALUE = 255, regionID MAX_VALUE = 255 
  	Int_t 		GetStrip() const  {return (fDetectorID & 0x000000FF);};
  	Int_t 		GetChamber() const {return (fDetectorID & 0x0000FF00) >> 8; };
   	Int_t 		GetRegion() const {return (fDetectorID & 0x00FF0000) >> 16;}; 	
   	Int_t 		GetVolumeUID() const {return fDetectorID;};

	static Int_t 	GetStrip(Int_t uid){  return (uid & 0x000000FF); };
   	static Int_t 	GetChamber(Int_t uid){ return (uid & 0x0000FF00) >> 8; };
   	static Int_t 	GetRegion(Int_t uid){ return (uid & 0x00FF0000) >> 16; };  	
  	static Int_t 	GetVolumeUID(Int_t regID, Int_t chamID, Int_t stripID) 
  	{ 
#ifdef DEBUG
 	Int_t uid = (regID << 16) | (chamID << 8) | stripID; 
 	Int_t region =	GetRegion(uid);
 	Int_t chamber =	GetChamber(uid);
  	Int_t strip =	GetStrip(uid);	
assert(region == regID);  
assert(chamber == chamID); 
assert(strip == stripID); 	
	return uid;
#else  	
  	return (regID << 16) | (chamID << 8) | stripID; 
#endif  	
  	};
 
ClassDef(BmnTOFPoint, 1)
};

#endif
//--------------------------------------------------------------------------------------------------------------------------------------
