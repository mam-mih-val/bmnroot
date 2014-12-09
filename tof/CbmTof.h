// -------------------------------------------------------------------------
// -----                         CbmTof header file                    -----
// -----                  Created 28/07/04  by V. Friese               -----
// -------------------------------------------------------------------------

/**  CbmTof.h
 *@author V.Friese <v.friese@gsi.de>
 **
 ** Defines the active detector TOF. Constructs the geometry and
 ** registeres MCPoints.
 **/



#ifndef CBMTOF_H
#define CBMTOF_H


#include "FairDetector.h"

#include "TLorentzVector.h"

class CbmTofPoint;
class CbmTofGeoHandler;
class FairVolume; 
class TClonesArray;
class TVector3;

class CbmTof : public FairDetector
{

 public:

  /** Default constructor **/
  CbmTof();


  /** Standard constructor.
   *@param name    detetcor name
   *@param active  sensitivity flag
   **/
  CbmTof(const char* name, Bool_t active);


  /** Destructor **/
  virtual ~CbmTof();


  /** Virtual method ProcessHits
   **
   ** Defines the action to be taken when a step is inside the
   ** active volume. Creates CbmTofPoints and adds them to the
   ** collection.
   *@param vol  Pointer to the active volume
   **/
  virtual Bool_t  ProcessHits(FairVolume* vol = 0);


  /** Virtual method EndOfEvent
   **
   ** If verbosity level is set, print hit collection at the
   ** end of the event and resets it afterwards.
   **/
  virtual void EndOfEvent();


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
   ** Constructs the TOF geometry
   **/
  virtual void ConstructGeometry();

  /** Do all initilization for the TOF detector **/
  virtual void Initialize();

 private:

  /** Track information to be stored until the track leaves the
      active volume. **/
  Int_t          fTrackID;           //!  track index
  Int_t          fVolumeID;          //!  volume id
  TLorentzVector fPos;               //!  position
  TLorentzVector fMom;               //!  momentum
  Double32_t     fTime;              //!  time
  Double32_t     fLength;            //!  length
  Double32_t     fELoss;             //!  energy loss

  Int_t fPosIndex;                   //!
  TClonesArray* fTofCollection;      //! Hit collection
  CbmTofGeoHandler *fGeoHandler;      //! Interface to gMC and gGeoManager

  /** Private method AddHit
   **
   ** Adds a CbmTofPoint to the HitCollection
   **/
  CbmTofPoint* AddHit(Int_t trackID, Int_t detID, TVector3 pos,
		      TVector3 mom, Double_t time, Double_t length, 
		      Double_t eLoss); 

  /** Private method ResetParameters
   **
   ** Resets the private members for the track parameters
   **/
  void ResetParameters();

  /** Method to construct the ascii geometry
   **
   **/
  void ConstructASCIIGeometry();
 
  Bool_t CheckIfSensitive(std::string name);

  CbmTof(const CbmTof&);
  CbmTof& operator=(const CbmTof&);


  ClassDef(CbmTof,2)

};


inline void CbmTof::ResetParameters() {
  fTrackID = fVolumeID = 0;
  fPos.SetXYZM(0.0, 0.0, 0.0, 0.0);
  fMom.SetXYZM(0.0, 0.0, 0.0, 0.0);
  fTime = fLength = fELoss = 0;
  fPosIndex = 0;
};


#endif

