//------------------------------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------
// -----                         BmnMWPC header file                    -----
// -------------------------------------------------------------------------

/*
 ** Defines the active detector MWPC. Constructs the geometry and
 ** registeres MCPoints.
**/



#ifndef BMNMWPC_H
#define BMNMWPC_H

#include "TClonesArray.h"
#include "TLorentzVector.h"
#include "TVector3.h"
#include "FairDetector.h"
#include "TGeoMedium.h"
#include "TString.h"
#include <map>
using namespace std;

class BmnMwpcPoint;
class FairVolume;

//------------------------------------------------------------------------------------------------------------------------
class BmnMwpc : public FairDetector
{

public:

   	// *@param name    detector name
   	// *@param active  sensitivity flag
  	BmnMwpc(const char* name, Bool_t active);

	BmnMwpc();
	virtual ~BmnMwpc();

	// Defines the action to be taken when a step is inside the
	// active volume. Creates BmnMWPC1Points and adds them to the collection.
	// @param vol  Pointer to the active volume
        virtual Bool_t  ProcessHits(FairVolume* vol = 0);

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

	// Constructs the MWPC1 geometry
 	virtual void ConstructGeometry();
    // Construct the geometry from an ASCII geometry file
    virtual void ConstructAsciiGeometry();
    // Construct the geometry from a GDML geometry file
    virtual void ConstructGDMLGeometry();
    void ExpandNodeForGdml(TGeoNode* node);
    map<TString, TGeoMedium*> fFixedMedia; // List of media "repaired" after importing GMDL
  
    // Check whether a volume is sensitive.
    // The decision is based on the volume name. Only used in case
    // of GDML and ROOT geometry.
    // @param name    Volume name
    // @value         kTRUE if volume is sensitive, else kFALSE
    virtual Bool_t CheckIfSensitive(std::string name);
 
    static Int_t GetWheel(Int_t uid){ return ((uid-1) >>3); };  //lsp  [0-1] == [inner,outer]
   	static Int_t GetProj(Int_t uid){ return ((uid-1) & 6)>>1; }; //lsp [0-3] == [x,y,u,v] 
 	static Int_t GetGasGap(Int_t uid){ return ((uid-1) & 0x0001); }; //lsp [0-1] == [inner,outer] 
  
private:

	// Track information to be stored until the track leaves the active volume.
  	Int_t          fTrackID;           //!  track index
  	Int_t          fVolumeID;          //!  volume id
  	TVector3       fPos;               //!  position
  	TVector3       fPosLocal;          //!  position local to gas chamber
  	TLorentzVector fMom;               //!  momentum
  	Double32_t     fTime;              //!  time
  	Double32_t     fLength;            //!  length
  	Double32_t     fELoss;             //!  energy loss
  	Int_t	       fIsPrimary; 	   //! is track primary?	
  	Double_t       fCharge;		   //! track charge
  	Double_t       fRadius;		   //! hit radius
  	Int_t          fPdgId;             //! pdg id of particle
  	Int_t          fwheel;
  	TVector3 fPosIn;             //!  entry position in global frame                                                                    
  	TVector3 fPosOut;            //!  exit position in global frame 
	                                                                    
    TVector3 fPosInTmp;             //!  entry position in global frame
	Int_t    fTrackIDTmp;           //!  track index
	Int_t fwheelTmp;

	Int_t fPosIndex;                   //!
  	TClonesArray* fPointCollection;      //! Hit collection

	int DistAndPoints(TVector3 p3, TVector3 p4, TVector3& pa, TVector3& pb);
        TVector3 GlobalToLocal(TVector3& global);
        TVector3 LocalToGlobal(TVector3& local);
        
	// Adds a BmnMWPC1Point to the HitCollection
  	BmnMwpcPoint* AddHit(Int_t trackID, Int_t detID, TVector3 pos, Double_t radius, TVector3 mom, Double_t time, 
  	Double_t length, Double_t eLoss, Int_t isPrimary, Double_t charge, Int_t fPdgId, TVector3 trackPos); 
 
	// Resets the private members for the track parameters
  	void ResetParameters();


  ClassDef(BmnMwpc,1) 

};

//------------------------------------------------------------------------------------------------------------------------
inline void BmnMwpc::ResetParameters() 
{
	fTrackID = -1;
	fVolumeID = fwheel = 0;
	fPos.SetXYZ(0.0, 0.0, 0.0);
	fMom.SetXYZM(0.0, 0.0, 0.0, 0.0);
	fTime = fLength = fELoss = 0;
	fPosIndex = 0;
       	
	fPosInTmp.SetXYZ(0.0, 0.0, 0.0);
        fTrackIDTmp=-1;
        fwheelTmp=-1;

}
//------------------------------------------------------------------------------------------------------------------------
#endif
