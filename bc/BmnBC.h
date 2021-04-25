/*************************************************************************************
 *
 *         Class BmnBC
 *         
 *  Adopted for BMN by:  Nikita Lashmanov,  Andrey Dryuk
 *  e-mail:  nikita10630@mail.ru,  andredryuk@gmail.com
 *  Version:  20.12.2020  
 *
 ************************************************************************************/

#ifndef BMNBC_H
#define BMNBC_H

#include "FairDetector.h"
#include "TClonesArray.h"
#include "TVector3.h"
#include "TLorentzVector.h"
#include "FairDetector.h"
#include "BmnBCGeoPar.h"
#include "BmnBCPoint.h"
#include "TVirtualMC.h"
#include "TParticle.h"


class BmnBC : public FairDetector
{
  public:

    /** Default constructor **/
    BmnBC();

    /** Standard constructor.
     *@param name    detetcor name
     *@param active  sensitivity flag
    **/
    BmnBC(const char* name, Bool_t active);

    /** Destructor **/
    virtual ~BmnBC();


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
    virtual void CopyClones(TClonesArray* cl1, TClonesArray* cl2, Int_t offset);

    /** Virtual method Construct geometry **/
    virtual void ConstructGeometry();

    virtual void ConstructAsciiGeometry();
    virtual Bool_t CheckIfSensitive(std::string name);
  
    BmnBCPoint* AddHit(Int_t trackID, Int_t detID,  Int_t copyNo,
                       TVector3 posIn, TVector3 posOut,
                       TVector3 momIn, TVector3 momOut,
                       Double_t tof, Double_t length, Double_t eLoss, Int_t fStat,
                       Bool_t isPrimary, Double_t charge, Int_t pdgId, Double_t lightYield,
                       Double_t timeIn, Double_t timeOut, Double_t lengthtrack);

  private:
    Int_t          fTrackID;           //!  track index
    Int_t          fVolumeID;          //!  volume id
    //Int_t        fEventID;           //!  event id
    TVector3       fPosIn;             //!  position (in)
    TVector3       fPosOut;            //!  position (out)
    TVector3       fMomIn;             //!  momentum (in)
    TVector3       fMomOut;
    Double32_t     fTime;              //!  time
    Double32_t     fLength;            //!  length
    Double32_t     fELoss;             //!  energy loss
    Int_t          fPosIndex;          //!
    Int_t          volDetector;        //!  MC volume ID of MUO
    Int_t	       fIsPrimary;         //!  is track primary?
    Double_t       fCharge;            //!  track charge
    Int_t          fPdgId;             //!  pdg id of particle
    Int_t          fStation;
    Float_t        fLightYield;
    TClonesArray*  fBCCollection;      //! Hit collection
    Double32_t     fTimeIn;            //!  time
    Double32_t     fTimeOut;           //!  time
    Double32_t     fLengthtrack;       //!  length
          
	
    // reset all parameters
    void ResetParameters();
    BmnBC(const BmnBC&) = delete;
    BmnBC operator=(const BmnBC&) = delete;

  ClassDef(BmnBC,2)
}; 


//------------------------------------------------------------------------------------------------------------------------
inline void BmnBC::ResetParameters() 
{
	fTrackID = fVolumeID = 0;
	fPosIn.SetXYZ(0.0, 0.0, 0.0);
    fPosOut.SetXYZ(0.0, 0.0, 0.0);
    fMomIn.SetXYZ(0.0, 0.0, 0.0);
    fMomOut.SetXYZ(0.0, 0.0, 0.0);
	fTime = fLength = fELoss = 0;
	fTimeIn = fTimeOut = 0;
	fLengthtrack = 0;
	fPosIndex = 0;
	fLightYield = 0;
  };
//------------------------------------------------------------------------------------------------------------------------

#endif
