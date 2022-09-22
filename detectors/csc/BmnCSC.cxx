#include "BmnCSC.h"

#include "BmnCSCPoint.h"

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

#include "TGeoManager.h"
#include "TRegexp.h"
#include "TFile.h"

#include "CbmStack.h"

#include <iostream>

using namespace std;

BmnCSC::BmnCSC()
: FairDetector("CSC", kTRUE) {
    fPointCollection = new TClonesArray("BmnCSCPoint");
    fPosIndex = 0;
    fVerboseLevel = 1;
    ResetParameters();
}

BmnCSC::BmnCSC(const char* name, Bool_t active)
: FairDetector(name, active) {
    fPointCollection = new TClonesArray("BmnCSCPoint");
    fPosIndex = 0;
    fVerboseLevel = 1;
    ResetParameters();
}


BmnCSC::~BmnCSC() {
    if(fPointCollection) {
        fPointCollection->Delete();
        delete fPointCollection;
    }
}

//------------------------------------------------------------------------------

Bool_t BmnCSC::ProcessHits(FairVolume* vol) {

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

    // Create BmnCSCPoint at EXIT of active volume;
    if((gMC->IsTrackExiting() || gMC->IsTrackStop() || gMC->IsTrackDisappeared()) && fELoss > 0) {

        TLorentzVector PosOut;
        gMC->TrackPosition(PosOut);
        fPosOut.SetXYZ(PosOut.X(), PosOut.Y(), PosOut.Z());

        TLorentzVector MomOut;
        gMC->TrackMomentum(MomOut);
        fMomOut.SetXYZ(MomOut.X(), MomOut.Y(), MomOut.Z());

        //correction step to avoid the seg. violation error due to invalid memory access
        TVector3 diff_pos = fPosIn - fPosOut;

        if(diff_pos.Mag() < 0.001) return kFALSE; //ignore points produced by tracks with zero length inside the current sens. volume
        if(fMomOut.Mag() == 0) return kFALSE; // ignore points produced by tracks with zero momentum inside the current sens. volume

        TVector3 corr_step = fMomOut;
        corr_step.SetMag(0.001); // 10 um
        TVector3 pos = fPosOut;
	fPosOut = pos - corr_step;
        gGeoManager->FindNode(fPosOut[0],fPosOut[1],fPosOut[2]);

        // Determine station and module numbers for the current hit ------------
        Int_t stationNum = -1; // current station number (default)
        Int_t moduleNum = -1; // current module number (default)

        TGeoVolume *currentVolume = gGeoManager->GetCurrentVolume();
        TString currentVolumeName = currentVolume->GetName();
        TRegexp expr = "^CSCSensor_module[0-9]+_station[0-9]+$";
        if(currentVolumeName.Contains(expr)) {
            TRegexp mod_expt = "module[0-9]+";
            TRegexp stat_expt = "station[0-9]+";

            moduleNum = TString(TString(currentVolumeName(mod_expt))(TRegexp("[0-9]+"))).Atoi();
            stationNum = TString(TString(currentVolumeName(stat_expt))(TRegexp("[0-9]+"))).Atoi();
        }

        if(stationNum == -1 || moduleNum == -1) return kFALSE; //check if the current point has incorrect indices

        BmnCSCPoint *p = AddHit(fTrackID, fVolumeID,
                                fPosIn, fPosOut,
                                fMomIn, fMomOut,
                                fTime, fLength, fELoss,
                                fIsPrimary, fCharge, fPdgId,
                                stationNum, moduleNum);

        ((CbmStack*)gMC->GetStack())->AddPoint(kCSC);
    }

    return kTRUE;
}

void BmnCSC::EndOfEvent() {
    if(fVerboseLevel) Print();
    fPointCollection->Clear();
    fPosIndex = 0;
}

void BmnCSC::Register() {
    FairRootManager::Instance()->Register("CSCPoint", "CSC", fPointCollection, kTRUE);
}

TClonesArray* BmnCSC::GetCollection(Int_t iColl) const {
    if(iColl == 0) return fPointCollection;
    return NULL;
}

void BmnCSC::Print() const {
    Int_t nHits = fPointCollection->GetEntriesFast();
    cout << "-I- BmnCSC: " << nHits << " points registered in this event." << endl;

    if(fVerboseLevel > 1) {
        for(Int_t i = 0; i < nHits; i++) {
            (*fPointCollection)[i]->Print();
        }
    }
}

void BmnCSC::Reset() {
    fPointCollection->Clear();
    ResetParameters();
}

void BmnCSC::CopyClones(TClonesArray* cl1, TClonesArray* cl2, Int_t offset) {
    Int_t nEntries = cl1->GetEntriesFast();
    cout << "-I- BmnCSC: " << nEntries << " entries to add." << endl;
    TClonesArray& clref = *cl2;
    BmnCSCPoint* oldpoint = NULL;

    for(Int_t i = 0; i < nEntries; i++) {
        oldpoint = (BmnCSCPoint*) cl1->At(i);
        Int_t index = oldpoint->GetTrackID() + offset;
        oldpoint->SetTrackID(index);
        new (clref[fPosIndex]) BmnCSCPoint(*oldpoint);
        fPosIndex++;
    }

    cout << "-I- BmnCSC: " << cl2->GetEntriesFast() << " merged entries."  << endl;
}

void BmnCSC::ConstructGeometry() {
    TString fileName = GetGeometryFileName();

    if( fileName.EndsWith(".root") ) {
        LOG(info) << "Constructing CSC geometry from ROOT file " << fileName.Data();
        ConstructRootGeometry();
    }

    else if ( fileName.EndsWith(".gdml") ) {
        LOG(info) << "Constructing CSC geometry from GDML file " << fileName.Data();
        ConstructGDMLGeometry();
    }

    else {
        LOG(fatal) << "Geometry format of CSC file " << fileName.Data() << " not supported.";
    }
}

// -----   ConstructGDMLGeometry   ---------------------------------------------
void BmnCSC::ConstructGDMLGeometry() {
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

    //   LOG(debug) << "====================================================================";
    //   for (Int_t i=0; i<gGeoManager->GetListOfMedia()->GetEntries(); i++)
    //       gGeoManager->GetListOfMedia()->At(i)->Dump();

    Int_t newMaxInd = gGeoManager->GetListOfMedia()->GetEntries() - 1;

    gGeoManager->GetTopVolume()->AddNode(gdmlTop, 1, 0);
    ExpandNodeForGdml(gGeoManager->GetTopVolume()->GetNode(gGeoManager->GetTopVolume()->GetNdaughters()-1));

    for(Int_t k = maxInd+1; k < newMaxInd+1; k++) {
        TGeoMedium* medToDel = (TGeoMedium*)(gGeoManager->GetListOfMedia()->At(maxInd+1));
        LOG(debug) << "    removing media " << medToDel->GetName() << " with id " << medToDel->GetId() << " (k=" << k << ")";
        gGeoManager->GetListOfMedia()->Remove(medToDel);
    }
    gGeoManager->SetAllIndex();

    gFile = old;
}

void BmnCSC::ExpandNodeForGdml(TGeoNode* node) {
    LOG(debug) << "----------------------------------------- ExpandNodeForGdml for node " << node->GetName();

    TGeoVolume* curVol = node->GetVolume();

    LOG(debug) << "    volume: " << curVol->GetName();

    if(curVol->IsAssembly()) {
        LOG(debug) << "    skipping volume-assembly";
    }
    else {
        TGeoMedium* curMed = curVol->GetMedium();
        TGeoMaterial* curMat = curVol->GetMaterial();
        TGeoMedium* curMedInGeoManager = gGeoManager->GetMedium(curMed->GetName());
        TGeoMaterial* curMatOfMedInGeoManager = curMedInGeoManager->GetMaterial();
        TGeoMaterial* curMatInGeoManager = gGeoManager->GetMaterial(curMat->GetName());

        // Current medium and material assigned to the volume from GDML
        LOG(debug2) << "    curMed\t\t\t\t" << curMed << "\t" << curMed->GetName() << "\t" << curMed->GetId();
        LOG(debug2) << "    curMat\t\t\t\t" << curMat << "\t" << curMat->GetName() << "\t" << curMat->GetIndex();

        // Medium and material found in the gGeoManager - either the pre-loaded one or one from GDML
        LOG(debug2) << "    curMedInGeoManager\t\t" << curMedInGeoManager
                 << "\t" << curMedInGeoManager->GetName() << "\t" << curMedInGeoManager->GetId();
        LOG(debug2) << "    curMatOfMedInGeoManager\t\t" << curMatOfMedInGeoManager
                 << "\t" << curMatOfMedInGeoManager->GetName() << "\t" << curMatOfMedInGeoManager->GetIndex();
        LOG(debug2) << "    curMatInGeoManager\t\t" << curMatInGeoManager
                 << "\t" << curMatInGeoManager->GetName() << "\t" << curMatInGeoManager->GetIndex();

        TString matName = curMat->GetName();
        TString medName = curMed->GetName();

        if (curMed->GetId() != curMedInGeoManager->GetId()) {
            if(fFixedMedia.find(medName) == fFixedMedia.end()) {
                LOG(debug) << "    Medium needs to be fixed";
                fFixedMedia[medName] = curMedInGeoManager;
                Int_t ind = curMat->GetIndex();
                gGeoManager->RemoveMaterial(ind);
                LOG(debug) << "    removing material " << curMat->GetName() << " with index " << ind;
                for(Int_t i=ind; i<gGeoManager->GetListOfMaterials()->GetEntries(); i++) {
                    TGeoMaterial* m = (TGeoMaterial*)gGeoManager->GetListOfMaterials()->At(i);
                    m->SetIndex(m->GetIndex()-1);
                }

                LOG(debug) << "    Medium fixed";
            }
            else {
                LOG(debug) << "    Already fixed medium found in the list    ";
            }
        }
        else {
            if(fFixedMedia.find(medName) == fFixedMedia.end()) {
                LOG(debug) << "    There is no correct medium in the memory yet";

                FairGeoLoader* geoLoad = FairGeoLoader::Instance();
                FairGeoInterface* geoFace = geoLoad->getGeoInterface();
                FairGeoMedia* geoMediaBase =  geoFace->getMedia();
                FairGeoBuilder* geobuild = geoLoad->getGeoBuilder();

                FairGeoMedium* curMedInGeo = geoMediaBase->getMedium(medName);
                if(curMedInGeo == 0) {
                    LOG(fatal) << "    Media not found in Geo file: " << medName;
                    //! This should not happen.
                    //! This means that somebody uses material in GDML that is not in the media.geo file.
                    //! Most probably this is the sign to the user to check materials' names in the CATIA model.
                }
                else {
                    LOG(debug) << "    Found media in Geo file" << medName;
                    Int_t nmed = geobuild->createMedium(curMedInGeo);
                    fFixedMedia[medName] = (TGeoMedium*)gGeoManager->GetListOfMedia()->Last();
                    gGeoManager->RemoveMaterial(curMatOfMedInGeoManager->GetIndex());
                    LOG(debug) << "    removing material " << curMatOfMedInGeoManager->GetName() << " with index " << curMatOfMedInGeoManager->GetIndex();
                    for(Int_t i=curMatOfMedInGeoManager->GetIndex(); i<gGeoManager->GetListOfMaterials()->GetEntries(); i++) {
                        TGeoMaterial* m = (TGeoMaterial*)gGeoManager->GetListOfMaterials()->At(i);
                        m->SetIndex(m->GetIndex()-1);
                    }
                }

                if(curMedInGeo->getSensitivityFlag()) {
                    LOG(debug) << "    Adding sensitive  " << curVol->GetName();
                    AddSensitiveVolume(curVol);
                }
             }
            else {
                LOG(debug) << "    Already fixed medium found in the list";
                LOG(debug) << "!!! Sensitivity: " << fFixedMedia[medName]->GetParam(0);
                if(fFixedMedia[medName]->GetParam(0) == 1) {
                    LOG(debug) << "    Adding sensitive  " << curVol->GetName();
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
Bool_t BmnCSC::CheckIfSensitive(std::string name) {
    TString tsname = name;
    if(tsname.Contains("CSCSensor")) {
        return kTRUE;
    }
    return kFALSE;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
BmnCSCPoint* BmnCSC::AddHit(Int_t trackID, Int_t detID,
                            TVector3 posIn, TVector3 posOut,
                            TVector3 momIn, TVector3 momOut,
                            Double_t time, Double_t length, Double_t eLoss,
                            Int_t isPrimary, Double_t charge, Int_t pdgId,
                            Int_t stationNum, Int_t moduleNum) {

    TClonesArray& clref = *fPointCollection;
    Int_t size = clref.GetEntriesFast();
    //std::cout << "ELoss: " << eLoss << "\n";
    new(clref[size]) BmnCSCPoint(trackID, detID,
                                        posIn, posOut, momIn, momOut,
                                        time, length, eLoss,
                                        isPrimary, charge, pdgId);

    BmnCSCPoint *hit = (BmnCSCPoint*) clref.At(clref.GetEntriesFast() - 1);
    hit->SetStation(stationNum);
    hit->SetModule(moduleNum);
    return hit;
}
//------------------------------------------------------------------------------

ClassImp(BmnCSC)
