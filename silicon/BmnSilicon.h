#ifndef BMNSILICON_H
#define BMNSILICON_H

#include "BmnSiliconPoint.h"

#include "FairDetector.h"
#include "TVector3.h"
#include "TGeoMedium.h"
#include "TLorentzVector.h"
#include "TClonesArray.h"

#include <map>


class BmnSilicon : public FairDetector {

  public:

    // *@param name    detector name
    // *@param active  sensitivity flag
    BmnSilicon(const char* name, Bool_t active);

    BmnSilicon();
    virtual ~BmnSilicon();

    // Defines the action to be taken when a step is inside the
    // active volume. Creates BmnSiliconPoints and adds them to the collection.
    // @param vol Pointer to the active volume
    virtual Bool_t ProcessHits(FairVolume* vol = 0);

    // If verbosity level is set, print hit collection at the
    // end of the event and resets it afterwards.
    virtual void EndOfEvent();

    // Registers the hit collection in the ROOT manager.
    virtual void Register();

    // Accessor to the hit collection
    virtual TClonesArray* GetCollection(Int_t iColl) const;

    // Screen output of hit collection.
    virtual void Print() const;

    // Clears the hit collection
    virtual void Reset();

    // *@param cl1     Origin
    // *@param cl2     Target
    // *@param offset  Index offset
    virtual void CopyClones(TClonesArray* cl1, TClonesArray* cl2, Int_t offset);

    // Constructs the Silicon geometry
    virtual void ConstructGeometry();

    // Construct the geometry from a GDML geometry file
    virtual void ConstructGDMLGeometry();

    void ExpandNodeForGdml(TGeoNode* node);

    // Check whether a volume is sensitive.
    // The decision is based on the volume name. Only used in case
    // of GDML and ROOT geometry.
    // @param name    Volume name
    // @value         kTRUE if volume is sensitive, else kFALSE
    virtual Bool_t CheckIfSensitive(std::string name);

  private:

    // Track information to be stored until the track leaves the active volume.
    Int_t          fTrackID;           //!  track index
    Int_t          fVolumeID;          //!  volume id
    TVector3       fPosIn;             //!  position (in)
    TVector3       fPosOut;            //!  position (out)
    TVector3       fMomIn;             //!  momentum (in)
    TVector3       fMomOut;            //!  momentum (out)
    Double32_t     fTime;              //!  time
    Double32_t     fLength;            //!  length
    Double32_t     fELoss;             //!  energy loss
    Int_t          fIsPrimary;         //!  is track primary?
    Double_t       fCharge;            //!  track charge
    Int_t          fPdgId;             //!  pdg id of particle

    Int_t fPosIndex;                   //!
    TClonesArray* fPointCollection;    //! Hit collection

    std::map<TString, TGeoMedium*> fFixedMedia; // List of media "repaired" after importing GMDL


    // Adds a BmnSiliconPoint to the HitCollection
    BmnSiliconPoint* AddHit(Int_t trackID, Int_t detID,
                            TVector3 posIn, TVector3 posOut,
                            TVector3 momIn, TVector3 momOut,
                            Double_t time, Double_t length, Double_t eLoss,
                            Int_t isPrimary, Double_t charge, Int_t pdgId);

    // Resets the private members for the track parameters
    void ResetParameters();
    BmnSilicon(const BmnSilicon&) = delete;
    BmnSilicon operator=(const BmnSilicon&) = delete;

    ClassDef(BmnSilicon,1)
};

//------------------------------------------------------------------------------
inline void BmnSilicon::ResetParameters() {
    fTrackID = -1;
    fVolumeID = 0;
    fPosIn.SetXYZ(0.0, 0.0, 0.0);
    fPosOut.SetXYZ(0.0, 0.0, 0.0);
    fMomIn.SetXYZ(0.0, 0.0, 0.0);
    fMomOut.SetXYZ(0.0, 0.0, 0.0);
    fTime = fLength = fELoss = 0.0;
    fPosIndex = 0;
};
//------------------------------------------------------------------------------

#endif /* BMNSILICON_H */
