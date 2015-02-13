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


  /** Standard constructor.
   *@param name    detetcor name
   *@param active  sensitivity flag
   **/
	BmnTOF1(const char* name, Bool_t active);


	virtual ~BmnTOF1();


  /** Virtual method Initialize
   ** Initialises detector. Stores volume IDs for MUO detector and mirror.
   **/
	virtual void Initialize();


  /** Virtual method ProcessHits
   **
   ** Defines the action to be taken when a step is inside the
   ** active volume. Creates BmnTOF1Points and adds 
   ** them to the collections.
   *@param vol  Pointer to the active volume
   **/
	virtual Bool_t ProcessHits(FairVolume* vol = nullptr);


  /** Virtual method EndOfEvent
   **
   ** If verbosity level is set, print hit collection at the
   ** end of the event and resets it afterwards.
   **/
	virtual void EndOfEvent();


	virtual void BeginEvent();
  /** Virtual method Register
   **
   ** Registers the hit collection in the ROOT manager.
   **/
	virtual void Register();


  /** Accessor to the hit collection **/
	virtual TClonesArray* GetCollection(Int_t iColl) const;


  /** Virtual method Print
   **
   ** Screen output of hit collection.
   **/
	virtual void Print() const;    


  /** Virtual method Reset
   **
   ** Clears the hit collection
   **/
	virtual void Reset();


  /** Virtual method CopyClones
   **
   ** Copies the hit collection with a given track index offset
   *@param cl1     Origin
   *@param cl2     Target
   *@param offset  Index offset
   **/
	virtual void CopyClones(TClonesArray* cl1, TClonesArray* cl2, Int_t offset);


  /** Virtual method Construct geometry
   **
   **/
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

ClassDef(BmnTOF1,2)
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

