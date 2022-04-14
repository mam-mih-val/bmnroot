/******************************************************************************
 *
 *         Class BmnHodo
 *         
 *  Created by:   Sergei Merts
 *  e-mail:   merts@jinr.ru
 *  Version:  28-03-2021
 *
 *  Modified by M.Golubeva April 2021
 *
 *****************************************************************************/

#ifndef BMNHODO_H
#define BMNHODO_H

#include "BmnHodoGeoPar.h"
#include "BmnHodoPoint.h"
#include "FairDetector.h"
#include "TClonesArray.h"
#include "TLorentzVector.h"
#include "TParticle.h"
#include "TVector3.h"
#include "TVirtualMC.h"


class BmnHodo : public FairDetector
{
  public:

    /** Default constructor **/
    BmnHodo();

    /** Standard constructor.
     *@param name    detetcor name
     *@param active  sensitivity flag
    **/
    BmnHodo(const char* name, Bool_t active);

    /** Destructor **/
    virtual ~BmnHodo();

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
  
    BmnHodoPoint* GetHit(Int_t i) const;
    BmnHodoPoint* GetHit(Int_t sticksens, Int_t stick) const;
    Int_t GetHodoStickSensVolId() { return fHodoStickSensVolId; }
  
    BmnHodoPoint* AddHit(Int_t trackID, Int_t detID, Int_t copyNo, Int_t copyNoMother, TVector3 pos, TVector3 mom, Double_t tof, Double_t length, Double_t eLoss);

  private:
    Int_t          fTrackID;        //!  track index
    Int_t          fVolumeID;       //!  volume id
    Int_t          fEventID;        //!  event id
    Int_t          fHodoStickSensVolId;
    TVector3       fPos;            //!  position (in)
    TVector3       fMom;            //!  momentum (in)
    Double32_t     fTime;           //!  time
    Double32_t     fLength;         //!  length
    Double32_t     fELoss;          //!  energy loss
    Int_t          volDetector;     //!  MC volume ID of MUO
  
    TClonesArray* fHodoCollection;  //! Hit collection

    // reset all parameters
    void ResetParameters();
    BmnHodo(const BmnHodo&) = delete;
    BmnHodo operator=(const BmnHodo&) = delete;

  ClassDef(BmnHodo, 0)
};


//-----------------------------------------------------------------------------
inline void BmnHodo::ResetParameters() {
    fTrackID = fVolumeID = 0;
    fPos.SetXYZ(0.0, 0.0, 0.0);
    fMom.SetXYZ(0.0, 0.0, 0.0);
    fTime = fLength = fELoss = 0;
};
//-----------------------------------------------------------------------------

#endif
