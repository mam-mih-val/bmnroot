#include "BmnHodo.h"

#include <iostream>

#include "BmnHodoGeo.h"
#include "BmnHodoPoint.h"
#include "CbmStack.h"
#include "FairGeoInterface.h"
#include "FairGeoLoader.h"
#include "FairGeoNode.h"
#include "FairGeoRootBuilder.h"
#include "FairMCPoint.h"
#include "FairRootManager.h"
#include "FairVolume.h"
#include "TClonesArray.h"
#include "TGeoArb8.h"
#include "TGeoMCGeometry.h"
#include "TGeoManager.h"
#include "TLorentzVector.h"
#include "TParticle.h"
#include "TVector3.h"
#include "TVirtualMC.h"
// add on for debug
//#include "FairGeoG3Builder.h"
#include "FairRun.h"
#include "FairRuntimeDb.h"
#include "TObjArray.h"
#include "TParticlePDG.h"

// -----   Default constructor   -------------------------------------------
BmnHodo::BmnHodo() {
    fHodoCollection = new TClonesArray("BmnHodoPoint");
    fVerboseLevel = 1;
}
// -------------------------------------------------------------------------

// -----   Standard constructor   ------------------------------------------
BmnHodo::BmnHodo(const char* name, Bool_t active)
    : FairDetector(name, active) {
    fHodoCollection = new TClonesArray("BmnHodoPoint");
    fVerboseLevel = 1;
}
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
BmnHodo::~BmnHodo() {
    if (fHodoCollection) {
        fHodoCollection->Delete();
        delete fHodoCollection;
    }
}
// -------------------------------------------------------------------------

// -----   Public method Intialize   ---------------------------------------
void BmnHodo::Initialize() {
    // Init function

    FairDetector::Initialize();
    FairRun* sim = FairRun::Instance();
    FairRuntimeDb* rtdb = sim->GetRuntimeDb();
}

// -----   Public method ProcessHits  --------------------------------------
Bool_t BmnHodo::ProcessHits(FairVolume* vol) {
    /** This method is called from the MC stepping */

    Int_t ivol = vol->getMCid();

    if (gMC->IsTrackEntering()) {
        ResetParameters();

        fELoss = 0.;

        TLorentzVector PosIn;
        gMC->TrackPosition(PosIn);
        fPos.SetXYZ(PosIn.X(), PosIn.Y(), PosIn.Z());

        TLorentzVector MomIn;
        gMC->TrackMomentum(MomIn);
        fMom.SetXYZ(MomIn.Px(), MomIn.Py(), MomIn.Pz());

        fTime = gMC->TrackTime() * 1.0e09;
        fLength = gMC->TrackLength();
    }

    fELoss += gMC->Edep();
    if (gMC->IsTrackExiting() || gMC->IsTrackStop() || gMC->IsTrackDisappeared()) {
        fTrackID = gMC->GetStack()->GetCurrentTrackNumber();
        fVolumeID = vol->getMCid();
        if (fELoss == 0.) {
            return kFALSE;
        }
        AddHit(fTrackID, fVolumeID, fPos, fMom, fTime, fLength, fELoss);

        ((CbmStack*)gMC->GetStack())->AddPoint(kHODO);
    }

    return kTRUE;
}

// -----   Public method EndOfEvent   -----------------------------------------
void BmnHodo::EndOfEvent() {
    if (fVerboseLevel) Print();
    Reset();
}

// -----   Public method Register   -------------------------------------------
void BmnHodo::Register() {
    FairRootManager::Instance()->Register("HodoPoint", "Hodo", fHodoCollection, kTRUE);
}

// -----   Public method GetCollection   --------------------------------------
TClonesArray* BmnHodo::GetCollection(Int_t iColl) const {
    if (iColl == 0) return fHodoCollection;
    return NULL;
}

// -----   Public method Print   ----------------------------------------------
void BmnHodo::Print() const {
    Int_t nHits = fHodoCollection->GetEntriesFast();
    cout << "-I- BmnHodo: " << nHits << " points registered in this event." << endl;

    if (fVerboseLevel > 1)
        for (Int_t i = 0; i < nHits; i++) (*fHodoCollection)[i]->Print();
}

// -----   Public method Reset   ----------------------------------------------
void BmnHodo::Reset() {
    fHodoCollection->Delete();
}

//--------------------------------------------------------------------------------------------------------------------------------------
void BmnHodo::ConstructGeometry() {
    TString fileName = GetGeometryFileName();
    if (fileName.EndsWith(".root")) {
        LOG(info) << "Constructing Hodo geometry from ROOT file " << fileName.Data();
        ConstructRootGeometry();
    } else
        LOG(fatal) << "Geometry format of Hodo file " << fileName.Data() << " not supported.";
}

Bool_t BmnHodo::CheckIfSensitive(std::string name) {
    TString tsname = name;
    if (tsname.Contains("sens")) return kTRUE;
    return kFALSE;
}

//-------------------------------------------------------------------------

// -----   Private method AddHit   --------------------------------------------
BmnHodoPoint* BmnHodo::AddHit(Int_t trackID, Int_t detID, TVector3 pos, TVector3 mom, Double_t time, Double_t length, Double_t eLoss) {
    TClonesArray& clref = *fHodoCollection;
    Int_t size = clref.GetEntriesFast();
    return new (clref[size]) BmnHodoPoint(trackID, detID, pos, mom, time, length, eLoss);
}

FairModule* BmnHodo::CloneModule() const { return new BmnHodo(*this); }

ClassImp(BmnHodo)
