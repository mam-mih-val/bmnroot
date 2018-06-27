#include "BmnSSD.h"

#include "BmnSSDPoint.h"

#include "TVirtualMC.h"
#include "TParticle.h"
#include "TParticlePDG.h"
#include "TGDMLParse.h"
#include "TGeoManager.h"

#include "FairRootManager.h"
#include "FairVolume.h"
#include "FairMCPoint.h"
#include "FairGeoMedia.h"
#include "FairGeoRootBuilder.h"

#include "CbmStack.h"

#include <iostream>

using namespace std;

BmnSSD::BmnSSD()
: FairDetector("SSD", kTRUE) {
    fPointCollection = new TClonesArray("BmnSSDPoint");
    fPosIndex = 0;
    fVerboseLevel = 1;
    ResetParameters();
}

BmnSSD::BmnSSD(const char* name, Bool_t active)
: FairDetector(name, active) {
    fPointCollection = new TClonesArray("BmnSSDPoint");
    fPosIndex = 0;
    fVerboseLevel = 1;
    ResetParameters();
}


BmnSSD::~BmnSSD() {
    if(fPointCollection) {
        fPointCollection->Delete();
        delete fPointCollection;
    }
}

//------------------------------------------------------------------------------

Bool_t BmnSSD::ProcessHits(FairVolume* vol)
{
    // Set parameters at entrance of volume. Reset ELoss.
    if(gMC->IsTrackEntering()) {

        ResetParameters();
        fELoss  = 0.;
        fTime   = gMC->TrackTime() * 1.0e09;
        fLength = gMC->TrackLength();
        fIsPrimary = 0;
        fCharge = -1;
        fPdgId = 0;

        TLorentzVector PosIn;
        gMC->TrackPosition(PosIn);
        fPosIn.SetXYZ(PosIn.X(), PosIn.Y(), PosIn.Z());

        TLorentzVector MomIn;
        gMC->TrackMomentum(MomIn);
        fMomIn.SetXYZ(MomIn.X(), MomIn.Y(), MomIn.Z());


        TParticle* part = 0;
        part = gMC->GetStack()->GetCurrentTrack();
        if (part) {
            fIsPrimary = (Int_t)part->IsPrimary();
            fCharge = (Int_t)part->GetPDG()->Charge();
            fPdgId = (Int_t)part->GetPdgCode();
        }

        fVolumeID = vol->getMCid();

        fTrackID = gMC->GetStack()->GetCurrentTrackNumber();
    }

    // Sum energy loss for all steps in the active volume
    fELoss += gMC->Edep();

    // Create BmnSSDPoint at EXIT of active volume;
    if((gMC->IsTrackExiting() || gMC->IsTrackStop() || gMC->IsTrackDisappeared()) && fELoss > 0) {

        TLorentzVector PosOut;
        gMC->TrackPosition(PosOut);
        fPosOut.SetXYZ(PosOut.X(), PosOut.Y(), PosOut.Z());

        TLorentzVector MomOut;
        gMC->TrackPosition(MomOut);
        fMomOut.SetXYZ(MomOut.X(), MomOut.Y(), MomOut.Z());

        BmnSSDPoint *p = AddHit(fTrackID, fVolumeID,
                                fPosIn, fPosOut,
                                fMomIn, fMomOut,
                                fTime, fLength, fELoss,
                                fIsPrimary, fCharge, fPdgId);

        ((CbmStack*)gMC->GetStack())->AddPoint(kSSD);
    }

    return kTRUE;
}

void BmnSSD::EndOfEvent() {
    if(fVerboseLevel) Print();
    fPointCollection->Clear();
    fPosIndex = 0;
}

void BmnSSD::Register() {
    FairRootManager::Instance()->Register("SSDPoint", "SSD", fPointCollection, kTRUE);
}

TClonesArray* BmnSSD::GetCollection(Int_t iColl) const {
    if(iColl == 0) return fPointCollection;
    return NULL;
}

void BmnSSD::Print() const {
    Int_t nHits = fPointCollection->GetEntriesFast();
    cout << "-I- BmnSSD: " << nHits << " points registered in this event." << endl;

    if(fVerboseLevel > 1) {
        for(Int_t i = 0; i < nHits; i++) {
            (*fPointCollection)[i]->Print();
        }
    }
}

void BmnSSD::Reset() {
    fPointCollection->Clear();
    ResetParameters();
}

void BmnSSD::CopyClones(TClonesArray* cl1, TClonesArray* cl2, Int_t offset) {
    Int_t nEntries = cl1->GetEntriesFast();
    cout << "-I- BmnSSD: " << nEntries << " entries to add." << endl;
    TClonesArray& clref = *cl2;
    BmnSSDPoint* oldpoint = NULL;

    for(Int_t i = 0; i < nEntries; i++) {
        oldpoint = (BmnSSDPoint*) cl1->At(i);
        Int_t index = oldpoint->GetTrackID() + offset;
        oldpoint->SetTrackID(index);
        new (clref[fPosIndex]) BmnSSDPoint(*oldpoint);
        fPosIndex++;
    }

    cout << "-I- BmnSSD: " << cl2->GetEntriesFast() << " merged entries."  << endl;
}

void BmnSSD::ConstructGeometry() {
    TString fileName = GetGeometryFileName();

    if( fileName.EndsWith(".root") ) {
        gLogger->Info(MESSAGE_ORIGIN, "Constructing SSD geometry from ROOT file %s", fileName.Data());
        ConstructRootGeometry();
    }

    else if ( fileName.EndsWith(".gdml") ) {
        gLogger->Info(MESSAGE_ORIGIN, "Constructing SSD geometry from GDML file %s", fileName.Data());
        ConstructGDMLGeometry();
    }

    else {
        gLogger->Fatal(MESSAGE_ORIGIN, "Geometry format of SSD file %s not supported.", fileName.Data());
    }
}

// -----   ConstructGDMLGeometry   ---------------------------------------------
void BmnSSD::ConstructGDMLGeometry() {
    TFile *old = gFile;
    TGDMLParse parser;
    TGeoVolume* gdmlTop;

    // Before importing GDML
    Int_t maxInd = gGeoManager->GetListOfMedia()->GetEntries() - 1;

    gdmlTop = parser.GDMLReadFile(GetGeometryFileName());

    // Cheating - reassigning media indices after GDML import (need to fix this in TGDMLParse class!!!)
    //   for (Int_t i=0; i<gGeoManager->GetListOfMedia()->GetEntries(); i++)
    //      gGeoManager->GetListOfMedia()->At(i)->Dump();
    // After importing GDML
    Int_t j = gGeoManager->GetListOfMedia()->GetEntries() - 1;
    Int_t curId;
    TGeoMedium* m;
    do {
         m = (TGeoMedium*)gGeoManager->GetListOfMedia()->At(j);
         curId = m->GetId();
         m->SetId(curId+maxInd);
         j--;
    } while (curId > 1);

    //   LOG(DEBUG) << "====================================================================" << FairLogger::endl;
    //   for (Int_t i=0; i<gGeoManager->GetListOfMedia()->GetEntries(); i++)
    //       gGeoManager->GetListOfMedia()->At(i)->Dump();

    Int_t newMaxInd = gGeoManager->GetListOfMedia()->GetEntries() - 1;

    gGeoManager->GetTopVolume()->AddNode(gdmlTop, 1, 0);
    ExpandNodeForGdml(gGeoManager->GetTopVolume()->GetNode(gGeoManager->GetTopVolume()->GetNdaughters()-1));

    for(Int_t k = maxInd+1; k < newMaxInd+1; k++) {
        TGeoMedium* medToDel = (TGeoMedium*)(gGeoManager->GetListOfMedia()->At(maxInd+1));
        LOG(DEBUG) << "    removing media " << medToDel->GetName() << " with id " << medToDel->GetId() << " (k=" << k << ")" << FairLogger::endl;
        gGeoManager->GetListOfMedia()->Remove(medToDel);
    }
    gGeoManager->SetAllIndex();

    gFile = old;
}

void BmnSSD::ExpandNodeForGdml(TGeoNode* node) {
    LOG(DEBUG) << "----------------------------------------- ExpandNodeForGdml for node " << node->GetName() << FairLogger::endl;

    TGeoVolume* curVol = node->GetVolume();

    LOG(DEBUG) << "    volume: " << curVol->GetName() << FairLogger::endl;

    if(curVol->IsAssembly()) {
        LOG(DEBUG) << "    skipping volume-assembly" << FairLogger::endl;
    }
    else {
        TGeoMedium* curMed = curVol->GetMedium();
        TGeoMaterial* curMat = curVol->GetMaterial();
        TGeoMedium* curMedInGeoManager = gGeoManager->GetMedium(curMed->GetName());
        TGeoMaterial* curMatOfMedInGeoManager = curMedInGeoManager->GetMaterial();
        TGeoMaterial* curMatInGeoManager = gGeoManager->GetMaterial(curMat->GetName());

        // Current medium and material assigned to the volume from GDML
        LOG(DEBUG2) << "    curMed\t\t\t\t" << curMed << "\t" << curMed->GetName() << "\t" << curMed->GetId() << FairLogger::endl;
        LOG(DEBUG2) << "    curMat\t\t\t\t" << curMat << "\t" << curMat->GetName() << "\t" << curMat->GetIndex() << FairLogger::endl;

        // Medium and material found in the gGeoManager - either the pre-loaded one or one from GDML
        LOG(DEBUG2) << "    curMedInGeoManager\t\t" << curMedInGeoManager
                 << "\t" << curMedInGeoManager->GetName() << "\t" << curMedInGeoManager->GetId() << FairLogger::endl;
        LOG(DEBUG2) << "    curMatOfMedInGeoManager\t\t" << curMatOfMedInGeoManager
                 << "\t" << curMatOfMedInGeoManager->GetName() << "\t" << curMatOfMedInGeoManager->GetIndex() << FairLogger::endl;
        LOG(DEBUG2) << "    curMatInGeoManager\t\t" << curMatInGeoManager
                 << "\t" << curMatInGeoManager->GetName() << "\t" << curMatInGeoManager->GetIndex() << FairLogger::endl;

        TString matName = curMat->GetName();
        TString medName = curMed->GetName();

        if (curMed->GetId() != curMedInGeoManager->GetId()) {
            if(fFixedMedia.find(medName) == fFixedMedia.end()) {
                LOG(DEBUG) << "    Medium needs to be fixed" << FairLogger::endl;
                fFixedMedia[medName] = curMedInGeoManager;
                Int_t ind = curMat->GetIndex();
                gGeoManager->RemoveMaterial(ind);
                LOG(DEBUG) << "    removing material " << curMat->GetName() << " with index " << ind << FairLogger::endl;
                for(Int_t i=ind; i<gGeoManager->GetListOfMaterials()->GetEntries(); i++) {
                    TGeoMaterial* m = (TGeoMaterial*)gGeoManager->GetListOfMaterials()->At(i);
                    m->SetIndex(m->GetIndex()-1);
                }

                LOG(DEBUG) << "    Medium fixed" << FairLogger::endl;
            }
            else {
                LOG(DEBUG) << "    Already fixed medium found in the list    " << FairLogger::endl;
            }
        }
        else {
            if(fFixedMedia.find(medName) == fFixedMedia.end()) {
                LOG(DEBUG) << "    There is no correct medium in the memory yet" << FairLogger::endl;

                FairGeoLoader* geoLoad = FairGeoLoader::Instance();
                FairGeoInterface* geoFace = geoLoad->getGeoInterface();
                FairGeoMedia* geoMediaBase =  geoFace->getMedia();
                FairGeoBuilder* geobuild = geoLoad->getGeoBuilder();

                FairGeoMedium* curMedInGeo = geoMediaBase->getMedium(medName);
                if(curMedInGeo == 0) {
                    LOG(FATAL) << "    Media not found in Geo file: " << medName << FairLogger::endl;
                    //! This should not happen.
                    //! This means that somebody uses material in GDML that is not in the media.geo file.
                    //! Most probably this is the sign to the user to check materials' names in the CATIA model.
                }
                else {
                    LOG(DEBUG) << "    Found media in Geo file" << medName << FairLogger::endl;
                    Int_t nmed = geobuild->createMedium(curMedInGeo);
                    fFixedMedia[medName] = (TGeoMedium*)gGeoManager->GetListOfMedia()->Last();
                    gGeoManager->RemoveMaterial(curMatOfMedInGeoManager->GetIndex());
                    LOG(DEBUG) << "    removing material " << curMatOfMedInGeoManager->GetName() << " with index " << curMatOfMedInGeoManager->GetIndex() << FairLogger::endl;
                    for(Int_t i=curMatOfMedInGeoManager->GetIndex(); i<gGeoManager->GetListOfMaterials()->GetEntries(); i++) {
                        TGeoMaterial* m = (TGeoMaterial*)gGeoManager->GetListOfMaterials()->At(i);
                        m->SetIndex(m->GetIndex()-1);
                    }
                }

                if(curMedInGeo->getSensitivityFlag()) {
                    LOG(DEBUG) << "    Adding sensitive  " << curVol->GetName() << FairLogger::endl;
                    AddSensitiveVolume(curVol);
                }
             }
            else {
                LOG(DEBUG) << "    Already fixed medium found in the list" << FairLogger::endl;
                LOG(DEBUG) << "!!! Sensitivity: " << fFixedMedia[medName]->GetParam(0) << FairLogger::endl;
                if(fFixedMedia[medName]->GetParam(0) == 1) {
                    LOG(DEBUG) << "    Adding sensitive  " << curVol->GetName() << FairLogger::endl;
                    AddSensitiveVolume(curVol);
                }
            }
        }

        curVol->SetMedium(fFixedMedia[medName]);
        gGeoManager->SetAllIndex();

        //gGeoManager->GetListOfMaterials()->Print();
        // gGeoManager->GetListOfMedia()->Print();

    }

    //! Recursevly go down the tree of nodes
    if(curVol->GetNdaughters() != 0) {
        TObjArray* NodeChildList = curVol->GetNodes();
        TGeoNode* curNodeChild;
        for (Int_t j = 0; j < NodeChildList->GetEntriesFast(); j++) {
            curNodeChild = (TGeoNode*)NodeChildList->At(j);
            ExpandNodeForGdml(curNodeChild);
        }
    }
}

//--- Check if Sensitive -------------------------------------------------------
Bool_t BmnSSD::CheckIfSensitive(std::string name) {
    TString tsname = name;
    if(tsname.Contains("Sensor")) {
        return kTRUE;
    }
    return kFALSE;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
BmnSSDPoint* BmnSSD::AddHit(Int_t trackID, Int_t detID,
                            TVector3 posIn, TVector3 posOut,
                            TVector3 momIn, TVector3 momOut,
                            Double_t time, Double_t length, Double_t eLoss,
                            Int_t isPrimary, Double_t charge, Int_t pdgId) {

    TClonesArray& clref = *fPointCollection;
    Int_t size = clref.GetEntriesFast();
    //std::cout << "ELoss: " << eLoss << "\n";
    return new(clref[size]) BmnSSDPoint(trackID, detID,
                                            posIn, posOut, momIn, momOut,
                                            time, length, eLoss,
                                            isPrimary, charge, pdgId);
}
//------------------------------------------------------------------------------

ClassImp(BmnSSD)
