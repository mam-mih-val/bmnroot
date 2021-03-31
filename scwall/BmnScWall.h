/*************************************************************************************
 *
 *         Class BmnScWall
 *         
 *  Created by:   Sergei Merts
 *  e-mail:   merts@jinr.ru
 *  Version:  28-03-2021
 *
 ************************************************************************************/

#ifndef BMNSCWALL_H
#define BMNSCWALL_H

#include "BmnScWallGeoPar.h"
#include "FairDetector.h"
#include "TClonesArray.h"
#include "TLorentzVector.h"
#include "TParticle.h"
#include "TVector3.h"
#include "TVirtualMC.h"

using namespace std;

class TClonesArray;
class BmnScWallPoint;
class FairVolume;

class BmnScWall : public FairDetector {
   public:
    /** Default constructor **/
    BmnScWall();

    /** Standard constructor.
   *@param name    detetcor name
   *@param active  sensitivity flag
   **/
    BmnScWall(const char* name, Bool_t active);

    /** Destructor **/
    virtual ~BmnScWall();

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

    virtual void SetSpecialPhysicsCuts() { ; }
    virtual void BeginEvent() { ; }
    virtual void FinishPrimary() { ; }
    virtual void FinishRun() { ; }
    virtual void BeginPrimary() { ; }
    virtual void PostTrack() { ; }
    virtual void PreTrack() { ; }

    virtual FairModule* CloneModule() const;

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

    /** Virtual method Construct geometry
   **
   **/
    virtual void ConstructGeometry();
    virtual Bool_t CheckIfSensitive(std::string name);

    BmnScWallPoint* AddHit(Int_t trackID, Int_t detID, TVector3 pos, TVector3 mom, Double_t tof, Double_t length, Double_t eLoss);

   private:
    Int_t fTrackID;    //!  track index
    Int_t fVolumeID;   //!  volume id
    TVector3 fPos;   //!  position (in)
    TVector3 fMom;   //!  momentum (in)
    Double32_t fTime;    //!  time
    Double32_t fLength;  //!  length
    Double32_t fELoss;   //!  energy loss

    TClonesArray* fScWallCollection;  //! Hit collection

    // reset all parameters
    void ResetParameters();
    //void DefineSensitiveVolumes();

    ClassDef(BmnScWall, 0)
};

//------------------------------------------------------------------------------------------------------------------------
inline void BmnScWall::ResetParameters() {
    fTrackID = fVolumeID = 0;
    fPos.SetXYZ(0.0, 0.0, 0.0);
    fMom.SetXYZ(0.0, 0.0, 0.0);
    fTime = fLength = fELoss = 0;
};
    //------------------------------------------------------------------------------------------------------------------------

#endif
