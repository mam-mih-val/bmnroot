#include <iostream>

#include "TClonesArray.h"
#include "TGeoMCGeometry.h"
#include "TGeoManager.h"
#include "TLorentzVector.h"
#include "TParticle.h"
#include "TVirtualMC.h"
#include "TGeoArb8.h"

#include "FairGeoInterface.h"
#include "FairGeoLoader.h"
#include "FairGeoNode.h"
//#include "BmnArmTrigGeo.h"
#include "FairGeoRootBuilder.h"
#include "CbmStack.h"
#include "BmnArmTrig.h"
#include "BmnArmTrigPoint.h"

#include "TVector3.h" 
#include "FairMCPoint.h"
#include "FairRootManager.h"
#include "FairVolume.h"
// add on for debug
//#include "FairGeoG3Builder.h"
#include "FairRuntimeDb.h"
#include "TObjArray.h"
#include "FairRun.h"

#include "TParticlePDG.h"

// -----   Default constructor   -------------------------------------------

BmnArmTrig::BmnArmTrig() {
    fArmTrigCollection = new TClonesArray("BmnArmTrigPoint");
    volDetector = 0;
    fPosIndex = 0;
    // fpreflag = 0;  
    //fpostflag = 0;
    // fEventID=-1; 
    fVerboseLevel = 1;

}
// -------------------------------------------------------------------------

// -----   Standard constructor   ------------------------------------------

BmnArmTrig::BmnArmTrig(const char* name, Bool_t active)
: FairDetector(name, active) {
    fArmTrigCollection = new TClonesArray("BmnArmTrigPoint");
    fPosIndex = 0;
    volDetector = 0;
    //fpreflag = 0;  
    //fpostflag = 0;
    //fEventID=-1;
    fVerboseLevel = 1;
}
// -------------------------------------------------------------------------



// -----   Destructor   ----------------------------------------------------

BmnArmTrig::~BmnArmTrig() {
    if (fArmTrigCollection) {
        fArmTrigCollection->Delete();
        delete fArmTrigCollection;
    }

}
// -------------------------------------------------------------------------

// -----   Public method Intialize   ---------------------------------------

void BmnArmTrig::Initialize() {
    // Init function

    FairDetector::Initialize();
    FairRun* sim = FairRun::Instance();
    FairRuntimeDb* rtdb = sim->GetRuntimeDb();
}
// -------------------------------------------------------------------------

void BmnArmTrig::BeginEvent() {
    // Begin of the event

}



// -----   Public method ProcessHits  --------------------------------------

Bool_t BmnArmTrig::ProcessHits(FairVolume* vol) {

    static Double_t timeIn;
    static Double_t timeOut;
    static Double_t lengthtrack;


    Int_t ivol = vol->getMCid();

    if (gMC->IsTrackEntering()) {

        ResetParameters();

        fELoss = 0.;
        fIsPrimary = 0;
        fCharge = -1;
        fPdgId = 0;

        lengthtrack = 0.;

        TLorentzVector PosIn;
        gMC->TrackPosition(PosIn);
        fPosIn.SetXYZ(PosIn.X(), PosIn.Y(), PosIn.Z());

        TLorentzVector MomIn;
        gMC->TrackMomentum(MomIn);
        fMomIn.SetXYZ(MomIn.Px(), MomIn.Py(), MomIn.Pz());

        timeIn = gMC->TrackTime() * 1.0e09;

        TParticle* part = 0;
        part = gMC->GetStack()->GetCurrentTrack();

        if (part) {
            fIsPrimary = (Int_t) part->IsPrimary();
            fCharge = (Int_t) part->GetPDG()->Charge() / 3.;
            fPdgId = (Int_t) part->GetPdgCode();
        }

    }

    Double_t eLoss = gMC->Edep();
    if (eLoss > 0)
        fELoss += eLoss;

    Double_t slengthtrack = gMC-> TrackStep();
    lengthtrack += slengthtrack;


    if (gMC->IsTrackExiting() || gMC->IsTrackStop() || gMC->IsTrackDisappeared() && fELoss > 0) {


        TLorentzVector PosOut;
        gMC->TrackPosition(PosOut);
        fPosOut.SetXYZ(PosOut.X(), PosOut.Y(), PosOut.Z());

        TLorentzVector MomOut;
        gMC->TrackMomentum(MomOut);
        fMomOut.SetXYZ(MomOut.Px(), MomOut.Py(), MomOut.Pz());

        timeOut = gMC->TrackTime() * 1.0e09;

        fTrackID = gMC->GetStack()->GetCurrentTrackNumber();

        Double_t time = gMC->TrackTime() * 1.0e09;
        Double_t length = gMC->TrackLength();

        Int_t copyNo;

        Int_t iCell;
        gMC->CurrentVolOffID(1, iCell);

        AddHit(fTrackID, ivol, copyNo, fPosIn, fPosOut,
                fMomIn, fMomOut,
                time, length, fELoss, fIsPrimary, fCharge, fPdgId, timeIn, timeOut, lengthtrack);

        ((CbmStack*) gMC->GetStack())->AddPoint(kARMTRIG);

    }

    return kTRUE;
}

// -----   Public method EndOfEvent   -----------------------------------------

void BmnArmTrig::EndOfEvent() {
    if (fVerboseLevel) Print();
    Reset();
}

// -----   Public method Register   -------------------------------------------

void BmnArmTrig::Register() {
    FairRootManager::Instance()->Register("ArmTrigPoint", "ArmTrig", fArmTrigCollection, kTRUE);
}

// -----   Public method GetCollection   --------------------------------------

TClonesArray* BmnArmTrig::GetCollection(Int_t iColl) const {
    if (iColl == 0) return fArmTrigCollection;

    return NULL;
}

// -----   Public method Print   ----------------------------------------------

void BmnArmTrig::Print() const {
    Int_t nHits = fArmTrigCollection->GetEntriesFast();
    cout << "-I- BmnArmTrig: " << nHits << " points registered in this event."
            << endl;

    if (fVerboseLevel > 1)
        for (Int_t i = 0; i < nHits; i++) (*fArmTrigCollection)[i]->Print();
}


// -----   Public method Reset   ----------------------------------------------

void BmnArmTrig::Reset() {
    fArmTrigCollection->Delete();

    fPosIndex = 0;
}

// guarda in FairRootManager::CopyClones
// -----   Public method CopyClones   -----------------------------------------

void BmnArmTrig::CopyClones(TClonesArray* cl1, TClonesArray* cl2, Int_t offset) {
    Int_t nEntries = cl1->GetEntriesFast();
    //cout << "-I- BmnArmTrig: " << nEntries << " entries to add." << endl;
    TClonesArray& clref = *cl2;
    BmnArmTrigPoint* oldpoint = NULL;
    for (Int_t i = 0; i < nEntries; i++) {
        oldpoint = (BmnArmTrigPoint*) cl1->At(i);
        Int_t index = oldpoint->GetTrackID() + offset;
        oldpoint->SetTrackID(index);
        new (clref[fPosIndex]) BmnArmTrigPoint(*oldpoint);
        fPosIndex++;
    }
    cout << " -I- BmnArmTrig: " << cl2->GetEntriesFast() << " merged entries."
            << endl;
}

//--------------------------------------------------------------------------------------------------------------------------------------

void BmnArmTrig::ConstructGeometry() {
    TString fileName = GetGeometryFileName();
    
    cout << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << fileName  << endl;
    
    if (fileName.EndsWith(".root")) {
        LOG(info) << "Constructing ArmTrig geometry from ROOT file " << fileName.Data();
        ConstructRootGeometry();
    } else if (fileName.EndsWith(".geo")) {
        LOG(info) << "Constructing ArmTrig geometry from ASCII file " << fileName.Data();
        ConstructAsciiGeometry();
    } else LOG(fatal) << "Geometry format of ArmTrig file " << fileName.Data() << " not supported.";
}
//--------------------------------------------------------------------------------------------------------------------------------------

void BmnArmTrig::ConstructAsciiGeometry() {
//    FairGeoLoader* geoLoad = FairGeoLoader::Instance();
//    FairGeoInterface* geoFace = geoLoad->getGeoInterface();
//    BmnArmTrigGeo* ArmTrigGeo = new BmnArmTrigGeo();
//    ArmTrigGeo->setGeomFile(GetGeometryFileName());
//    geoFace->addGeoModule(ArmTrigGeo);
//
//    Bool_t rc = geoFace->readSet(ArmTrigGeo);
//    if (rc) ArmTrigGeo->create(geoLoad->getGeoBuilder());
//    TList* volList = ArmTrigGeo->getListOfVolumes();
//
//    // store geo parameter
//    FairRun *fRun = FairRun::Instance();
//    FairRuntimeDb *rtdb = FairRun::Instance()->GetRuntimeDb();
//    BmnArmTrigGeoPar* par = (BmnArmTrigGeoPar*) (rtdb->getContainer("BmnArmTrigGeoPar"));
//    TObjArray *fSensNodes = par->GetGeoSensitiveNodes();
//    TObjArray *fPassNodes = par->GetGeoPassiveNodes();
//
//    TListIter iter(volList);
//    FairGeoNode *node = nullptr;
//    FairGeoVolume *aVol = nullptr;
//
//    while ((node = (FairGeoNode*) iter.Next())) {
//        aVol = dynamic_cast<FairGeoVolume*> (node);
//
//        if (node->isSensitive()) fSensNodes->AddLast(aVol);
//        else fPassNodes->AddLast(aVol);
//    }
//
//    par->setChanged();
//    par->setInputVersion(fRun->GetRunId(), 1);
//    ProcessNodes(volList);
}

//--------------------------------------------------------------------------------------------------------------------------------------

Bool_t BmnArmTrig::CheckIfSensitive(std::string name) {
    TString tsname = name;
    if (tsname.Contains("Active")) return kTRUE;

    return kFALSE;
}
//-------------------------------------------------------------------------

// -----   Private method AddHit   --------------------------------------------

BmnArmTrigPoint* BmnArmTrig::AddHit(Int_t trackID, Int_t detID, Int_t copyNo, TVector3 posIn, TVector3 posOut,
        TVector3 momIn, TVector3 momOut, Double_t time, Double_t length, Double_t eLoss, Bool_t isPrimary,
        Double_t charge, Int_t pdgId, Double_t timeIn, Double_t timeOut, Double_t lengthtrack) {

    TClonesArray& clref = *fArmTrigCollection;
    Int_t size = clref.GetEntriesFast();
    return new(clref[size]) BmnArmTrigPoint(trackID, detID, copyNo, posIn, posOut, momIn, momOut, time, length, eLoss, isPrimary,
            charge, pdgId, timeIn, timeOut, lengthtrack);
}

ClassImp(BmnArmTrig)
