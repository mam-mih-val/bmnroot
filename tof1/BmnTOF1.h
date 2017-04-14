//--------------------------------------------------------------------------------------------------------------------------------------
#ifndef __BMNTOF1_H
#define __BMNTOF1_H 1


#include "TClonesArray.h"
#include "TVector3.h"
#include "TLorentzVector.h"
#include "FairDetector.h"
#include "BmnTOF1GeoPar.h"

using namespace std;


class TClonesArray;
class BmnTOF1Point;
class FairVolume;

//--------------------------------------------------------------------------------------------------------------------------------------
class BmnTOF1 : public FairDetector
{
public:
	BmnTOF1();
	BmnTOF1(const char* name, Bool_t active);
	virtual ~BmnTOF1();

	virtual void Initialize();
	virtual Bool_t ProcessHits(FairVolume* vol = nullptr);
	virtual void EndOfEvent();
	virtual void BeginEvent();
	virtual void Register();
	virtual TClonesArray* GetCollection(Int_t iColl) const;
	virtual void Print() const;    
	virtual void Reset();
	virtual void CopyClones(TClonesArray* cl1, TClonesArray* cl2, Int_t offset);
	virtual void ConstructGeometry();
	virtual void ConstructAsciiGeometry();
 	virtual Bool_t CheckIfSensitive(std::string name);

	BmnTOF1Point* AddPoint(Int_t trackID, Int_t detID, TVector3 pos, TVector3 mom, Double_t tof, Double_t length, Double_t eLoss);

private:
	TLorentzVector 	fPos;               //!  position
	TLorentzVector 	fMom;               //!  momentum
	Double32_t     	fTime;              //!  time
	Double32_t     	fLength;            //!  length
	Double32_t     	fELoss;             //!  energy loss
	Int_t 		fPosIndex;		//! 
  	const double	nan;			//!
  	
  	TClonesArray	*fTofCollection;	//! Hit collection
  
	// reset all parameters   
	void ResetParameters();

ClassDef(BmnTOF1, 3)
}; 
//--------------------------------------------------------------------------------------------------------------------------------------
inline void BmnTOF1::ResetParameters() 
{
	fPos.SetXYZM(nan, nan, nan, nan);
	fMom.SetXYZM(nan, nan, nan, nan);
	fTime = fLength = fELoss = nan;
	fPosIndex = 0;
};


#endif
//--------------------------------------------------------------------------------------------------------------------------------------

