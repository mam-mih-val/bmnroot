/*************************************************************************************
 *
 *         Class BmnEcal
 *         
 *  Adopted for BMN by:   Elena Litvinenko
 *  e-mail:   litvin@nf.jinr.ru
 *  Version:  10-02-2016
 *  Updated:  09-08-2019 by Petr Alekseev <pnaleks@gmail.com>
 *
************************************************************************************/

#ifndef BMNECAL_H
#define BMNECAL_H

#include "BmnEcalPoint.h"

#include "TClonesArray.h"
#include "TVector3.h"
#include "TLorentzVector.h"
#include "FairDetector.h"
#include "BmnEcalGeoPar.h"


class BmnEcal : public FairDetector
{
 public:

  /** Default constructor **/
  BmnEcal();


  /** Standard constructor.
   *@param name    detetcor name
   *@param active  sensitivity flag
   **/
  BmnEcal(const char* name, Bool_t active);


  /** Destructor **/
  virtual ~BmnEcal();


  /** Virtual method Initialize
   ** Initialises detector. Stores volume IDs for MUO detector and mirror.
   **/
  virtual void Initialize();


  /** Virtual method ProcessHits
   **
   ** Defines the action to be taken when a step is inside the
   ** active volume. Creates BmnEcalPoints and BmnEcalMirrorPoints and adds 
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

  virtual Bool_t CheckIfSensitive(std::string name);
  
  /** SetToFile
   **
   ** Allows to disable writing the brunch into the output tree
   *@param toFile if kFALSE, branch will NOT be saved to the tree
   **/
  void SetToFile(Bool_t toFile) { fToFile = toFile; }
  
  BmnEcalPoint* AddHit(Int_t trackID, Int_t detID,  Int_t copyNo, Int_t copyNoMother,
		      TVector3 pos, TVector3 mom,
		      Double_t tof, Double_t length, Double_t eLoss);

 private:
  Int_t          fTrackID;           //!  track index
  Int_t          fVolumeID;          //!  volume id
  TLorentzVector fPos;               //!  position
  TLorentzVector fMom;               //!  momentum
  Double32_t     fTime;              //!  time
  Double32_t     fLength;            //!  length
  Double32_t     fELoss;             //!  energy loss
    
  TClonesArray* fEcalCollection;     //! Hit collection

  Bool_t fToFile = kTRUE;	//! if kFALSE, branch will NOT be saved to the tree
  
  BmnEcal(const BmnEcal&) = delete;
  BmnEcal operator=(const BmnEcal&) = delete;

  ClassDef(BmnEcal,3)
}; 

#endif
