/*************************************************************************************
 *
 *         Class BmnBd
 *         
 *  Adopted for BMN by:   Elena Litvinenko
 *  e-mail:   litvin@nf.jinr.ru
 *  Version:  10-02-2016   
 *
 ************************************************************************************/

#ifndef BMNBD_H
#define BMNBD_H


#include "TClonesArray.h"
#include "TVector3.h"
#include "TLorentzVector.h"
#include "FairDetector.h"
#include "BmnBdGeoPar.h"

using namespace std;


class TClonesArray;
class BmnBdPoint;
class FairVolume;

class BmnBd : public FairDetector
{

 public:

  /** Default constructor **/
  BmnBd();


  /** Standard constructor.
   *@param name    detetcor name
   *@param active  sensitivity flag
   **/
  BmnBd(const char* name, Bool_t active);


  /** Destructor **/
  virtual ~BmnBd();


  /** Virtual method Initialize
   ** Initialises detector. Stores volume IDs for MUO detector and mirror.
   **/
  virtual void Initialize();


  /** Virtual method ProcessHits
   **
   ** Defines the action to be taken when a step is inside the
   ** active volume. Creates BmnBdPoints and BmnBdMirrorPoints and adds 
   ** them to the collections.
   *@param vol  Pointer to the active volume
   **/
  virtual Bool_t ProcessHits(FairVolume* vol = 0);


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
  virtual void CopyClones(TClonesArray* cl1, TClonesArray* cl2,
			  Int_t offset);


  /** Virtual method Construct geometry
   **
   **/
  virtual void ConstructGeometry();

  BmnBdPoint* AddHit(Int_t trackID, Int_t detID,  Int_t copyNo, 
		      TVector3 pos, TVector3 mom,
		      Double_t tof, Double_t length, Double_t eLoss);

 private:
  Int_t          fTrackID;           //!  track index
  Int_t          fVolumeID;          //!  volume id
  Int_t          fEventID;           //!  event id
  TLorentzVector fPos;               //!  position
  TLorentzVector fMom;               //!  momentum
  Double32_t     fTime;              //!  time
  Double32_t     fLength;            //!  length
  Double32_t     fELoss;             //!  energy loss
  Int_t fPosIndex;      //! 
  Int_t volDetector;     //!  MC volume ID of MUO
  
  TClonesArray* fBdCollection;        //! Hit collection
  
  // reset all parameters   
  void ResetParameters();

  ClassDef(BmnBd,2)

}; 


//------------------------------------------------------------------------------------------------------------------------
inline void BmnBd::ResetParameters() 
{
	fTrackID = fVolumeID = 0;
	fPos.SetXYZM(0.0, 0.0, 0.0, 0.0);
	fMom.SetXYZM(0.0, 0.0, 0.0, 0.0);
	fTime = fLength = fELoss = 0;
	fPosIndex = 0;
};
//------------------------------------------------------------------------------------------------------------------------

#endif
