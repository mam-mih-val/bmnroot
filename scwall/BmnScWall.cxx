#include "BmnScWall.h"

#include <iostream>

#include "BmnScWallGeo.h"
#include "BmnScWallPoint.h"
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
BmnScWall::BmnScWall() {
    fScWallCollection = new TClonesArray("BmnScWallPoint");
    fVerboseLevel = 1;
}
// -------------------------------------------------------------------------

// -----   Standard constructor   ------------------------------------------
BmnScWall::BmnScWall(const char* name, Bool_t active)
    : FairDetector(name, active) {
    fScWallCollection = new TClonesArray("BmnScWallPoint");
    fVerboseLevel = 1;
}
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
BmnScWall::~BmnScWall() {
    if (fScWallCollection) {
        fScWallCollection->Delete();
        delete fScWallCollection;
    }
}
// -------------------------------------------------------------------------

// -----   Public method Intialize   ---------------------------------------
void BmnScWall::Initialize() {
    // Init function

    FairDetector::Initialize();
    FairRun* sim = FairRun::Instance();
    FairRuntimeDb* rtdb = sim->GetRuntimeDb();
}

// -----   Public method ProcessHits  --------------------------------------
Bool_t BmnScWall::ProcessHits(FairVolume* vol) {
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

        ((CbmStack*)gMC->GetStack())->AddPoint(kSCWALL);
    }

    return kTRUE;
}

// -----   Public method EndOfEvent   -----------------------------------------
void BmnScWall::EndOfEvent() {
    if (fVerboseLevel) Print();
    Reset();
}

// -----   Public method Register   -------------------------------------------
void BmnScWall::Register() {
    FairRootManager::Instance()->Register("ScWallPoint", "ScWall", fScWallCollection, kTRUE);
}

// -----   Public method GetCollection   --------------------------------------
TClonesArray* BmnScWall::GetCollection(Int_t iColl) const {
    if (iColl == 0) return fScWallCollection;
    return NULL;
}

// -----   Public method Print   ----------------------------------------------
void BmnScWall::Print() const {
    Int_t nHits = fScWallCollection->GetEntriesFast();
    cout << "-I- BmnScWall: " << nHits << " points registered in this event." << endl;

    if (fVerboseLevel > 1)
        for (Int_t i = 0; i < nHits; i++) (*fScWallCollection)[i]->Print();
}

// -----   Public method Reset   ----------------------------------------------
void BmnScWall::Reset() {
    fScWallCollection->Delete();
}

//--------------------------------------------------------------------------------------------------------------------------------------
void BmnScWall::ConstructGeometry() {
    TString fileName = GetGeometryFileName();
    if (fileName.EndsWith(".root")) {
        LOG(info) << "Constructing ScWall geometry from ROOT file " << fileName.Data();
        ConstructRootGeometry();
    } else
        LOG(fatal) << "Geometry format of ScWall file " << fileName.Data() << " not supported.";
}

Bool_t BmnScWall::CheckIfSensitive(std::string name) {
    TString tsname = name;
    if (tsname.Contains("sens")) return kTRUE;
    return kFALSE;
}

//-------------------------------------------------------------------------

// -----   Private method AddHit   --------------------------------------------
BmnScWallPoint* BmnScWall::AddHit(Int_t trackID, Int_t detID, TVector3 pos, TVector3 mom, Double_t time, Double_t length, Double_t eLoss) {
    TClonesArray& clref = *fScWallCollection;
    Int_t size = clref.GetEntriesFast();
    return new (clref[size]) BmnScWallPoint(trackID, detID, pos, mom, time, length, eLoss);
}

FairModule* BmnScWall::CloneModule() const { return new BmnScWall(*this); }

ClassImp(BmnScWall)
