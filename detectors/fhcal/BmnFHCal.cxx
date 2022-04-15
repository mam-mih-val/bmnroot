/******************************************************************************
 *
 *         Class BmnFHCal
 *
 *  Adopted for BMN by:   Elena Litvinenko (EL)
 *  e-mail:   litvin@nf.jinr.ru
 *  Version:  06-11-2015
 *  Last update:  22-Feb-2012 (EL)
 *
 *  Modified by M.Golubeva
 *
 *****************************************************************************/

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
#include "BmnFHCalGeo.h"
#include "FairGeoRootBuilder.h"
 //#include "FairStack.h"
#include "CbmStack.h"
#include "BmnFHCal.h"
#include "BmnFHCalPoint.h"

#include "FairRootManager.h"
#include "FairVolume.h"
// add on for debug
//#include "FairGeoG3Builder.h"
#include "FairRuntimeDb.h"
#include "TObjArray.h"
#include "FairRun.h"

#include "TParticlePDG.h"

// -----   Default constructor   -------------------------------------------
BmnFHCal::BmnFHCal() {
  fFHCalCollection = new TClonesArray("BmnFHCalPoint");
  volDetector = 0;
  fPosIndex = 0;
  fEventID = -1;
  fHitNb = 0;
  fVSCVolId = 0;
  fVSCNICAVolId = 0;
  fVerboseLevel = 1;

}

// -----   Standard constructor   ------------------------------------------
BmnFHCal::BmnFHCal(const char* name, Bool_t active)
  : FairDetector(name, active) {
  fFHCalCollection = new TClonesArray("BmnFHCalPoint");
  fPosIndex = 0;
  volDetector = 0;
  fEventID = -1;
  fHitNb = 0;
  fVSCVolId = 0;
  fVSCNICAVolId = 0;
  fVerboseLevel = 1;
}

// -----   Destructor   ----------------------------------------------------
BmnFHCal::~BmnFHCal() {
  if (fFHCalCollection) {
    fFHCalCollection->Delete();
    delete fFHCalCollection;
  }

}

// -----   Public method Intialize   ---------------------------------------
void BmnFHCal::Initialize() {
  // Init function

  FairDetector::Initialize();
  FairRun* sim = FairRun::Instance();
  FairRuntimeDb* rtdb = sim->GetRuntimeDb();

  //fVSCVolId = gMC->VolId("zdc01s");
  //fVSCNICAVolId = gMC->VolId("zdc01s_NICA");
  fVSCVolId = gMC->VolId("fhcal01s");
  fVSCNICAVolId = gMC->VolId("fhcal01s_NICA");

}

// -------------------------------------------------------------------------
void BmnFHCal::BeginEvent() {
  // Begin of the event

}

//_____________________________________________________________________________
BmnFHCalPoint* BmnFHCal::GetHit(Int_t i) const {
  // Returns the hit for the specified layer.
  // ---

  return (BmnFHCalPoint*)fFHCalCollection->At(i);
}

//_____________________________________________________________________________
BmnFHCalPoint* BmnFHCal::GetHit(Int_t vsc, Int_t mod) const {
  // Returns the hit for the specified vsc and module.
  // ---

  BmnFHCalPoint* hit;
  Int_t nofHits = fFHCalCollection->GetEntriesFast();
  for (Int_t i = 0; i < nofHits; i++) {
    hit = GetHit(i);
    //cout <<"fEdep " <<i <<" " <<hit->GetEdep() <<endl;
    //int iVSCId = hit->GetVSCId();
    //int iMODId = hit->GetMODId();
    //if(iVSCId == vsc && iMODId == mod)
    if (hit->GetCopy() == vsc && hit->GetCopyMother() == mod)
      return hit;
  }

  return 0;
}

//_____________________________________________________________________________
BmnFHCalPoint* BmnFHCal::GetHitPrint(Int_t vsc, Int_t mod) const {
  // Returns the hit for the specified vsc and module.
  // ---

  BmnFHCalPoint* hit;
  Int_t nofHits = fFHCalCollection->GetEntriesFast();

  for (Int_t i = 0; i < nofHits; i++) {
    hit = GetHit(i);
    if (hit->GetCopy() == vsc && hit->GetCopyMother() == mod)
      cout << "GetHitPrint method " << hit->GetCopyMother() << " " << hit->GetCopy() << " " << hit->GetEnergyLoss() << " " << hit->GetZ() << endl;
  }
  /*
  for (Int_t i=0; i<nofHits; i++) {
    hit =  GetHit(i);
    //cout <<"fEdep " <<i <<" " <<hit->GetEdep() <<endl;
    //int iVSCId = hit->GetVSCId();
    //int iMODId = hit->GetMODId();
    //if(iVSCId == vsc && iMODId == mod)
    if(hit->GetCopy() == vsc && hit->GetCopyMother() == mod)
      return hit;
  }
  */

  return 0;
}




// -----   Public method ProcessHits  --------------------------------------
Bool_t BmnFHCal::ProcessHits(FairVolume* vol) {
  // if (TMath::Abs(gMC->TrackCharge()) <= 0) return kFALSE;

  Int_t copyNoVSC, copyNoVTYVEC, copyNoVMOD, copyNoVFHCal;
  Int_t copyNoVSCNICA, copyNoVTYVECNICA, copyNoVMODNICA, copyNoVFHCalNICA;
  Int_t copyNoVSCCom, copyNoVTYVECCom, copyNoVMODCom, copyNoVFHCalCom;

  Int_t      ivol;
  TLorentzVector tPos1, tMom1;
  TLorentzVector tPos, tMom;

  Int_t module, module_nica;
  Int_t slice, slice_nica;
  Int_t zdc, zdc_nica;

  Double_t time = 0;
  Double_t length = 0;

  TParticle* part;
  Double_t charge;

  Double_t  QCF = 1; //quenching for Birk
  Double_t  BirkConst = 12.6; //0.126 mm/MeV for polystyrene 
  //0.126 *(0.1/0.001) = 12.6 cm/GeV
  //(0.126 mm/MeV - from Wikipedia, 0.07943mm/MeV є in Geant4)


  //#define EDEBUG
#ifdef EDEBUG
  static Int_t lEDEBUGcounter = 0;
  if (lEDEBUGcounter < 1)
    std::cout << "EDEBUG-- BmnFHCal::ProcessHits: entered" << gMC->CurrentVolPath() << endl;
#endif


  if (gMC->CurrentVolID(copyNoVSC) != fVSCVolId &&
    gMC->CurrentVolID(copyNoVSCNICA) != fVSCNICAVolId) {
    return kFALSE;
  }

  ivol = vol->getMCid();

  if (gMC->CurrentVolID(copyNoVSC) == fVSCVolId || gMC->CurrentVolID(copyNoVSCNICA) == fVSCNICAVolId) {
    gMC->CurrentVolOffID(1, slice);
    gMC->CurrentVolOffID(2, module);
    //gMC->CurrentVolOffID(3, zdc);
    copyNoVTYVECCom = slice; copyNoVMODCom = module; //copyNoVZDCCom = zdc;
  }

  if (gMC->IsTrackEntering()) {

    ResetParameters();
    fELoss = 0.;
    time = 0.;
    length = 0.;
    gMC->TrackPosition(tPos);
    gMC->TrackMomentum(tMom);

#ifdef EDEBUG
    gMC->TrackPosition(tPos1);
    gMC->TrackMomentum(tMom1);
#endif
  }//if (gMC->IsTrackEntering())

  if (gMC->IsTrackInside()) {

    gMC->TrackPosition(tPos);
    gMC->TrackMomentum(tMom);
    length += gMC->TrackStep();

    //fELoss +=gMC->Edep();
//Birk corrections
    if (gMC->TrackStep() > 0) QCF = 1. + (BirkConst / gMC->TrackStep()) * gMC->Edep();
    else QCF = 1;
    fELoss += (gMC->Edep()) / QCF;

    time += gMC->TrackTime() * 1.0e09;

    if (gMC->IsTrackStop() || gMC->IsTrackDisappeared()) {

      part = gMC->GetStack()->GetCurrentTrack();
      charge = part->GetPDG()->Charge() / 3.;

      // Create BmnFHCalPoint
      fTrackID = gMC->GetStack()->GetCurrentTrackNumber();

      if (fELoss > 0) {

        //std::cout << "INSIDE MpdFHCal::ProcessHits: TrackID:" <<part->GetPdgCode() <<" " << fTrackID << "  " <<fELoss <<" " << gMC->CurrentVolPath() << " " << tPos.Z() <<"   "  <<tMom.Pz() <<" " << ivol <<" " <<gMC->CurrentVolOffName(2) << " " <<gMC->CurrentVolOffName(1) << " " << gMC->CurrentVolOffName(0) <<std::endl;

        if (copyNoVTYVECCom == slice && copyNoVMODCom == module) {//module
          if (!GetHit(slice, module)) {
            AddHit(fTrackID, ivol, slice, module, TVector3(tPos.X(), tPos.Y(), tPos.Z()), TVector3(tMom.Px(), tMom.Py(), tMom.Pz()), time, length, fELoss);
          } else {
            GetHit(slice, module)->AddVSC(fTrackID, ivol, slice, module, TVector3(tPos.X(), tPos.Y(), tPos.Z()), TVector3(tMom.Px(), tMom.Py(), tMom.Pz()), time, length, fELoss);
          }
        }//if(copyNoVTYVECCom==slice && copyNoVMODCom==module)	  

      }//if(fELoss>0)
    }//if ( gMC->IsTrackStop() || gMC->IsTrackDisappeared() )
  }//if ( gMC->IsTrackInside())

  if (gMC->IsTrackExiting()) {

    part = gMC->GetStack()->GetCurrentTrack();

    // Create BmnFHCalPoint
    fTrackID = gMC->GetStack()->GetCurrentTrackNumber();
    time += gMC->TrackTime() * 1.0e09;
    length += gMC->TrackLength();

    //fELoss +=gMC->Edep();
//Birk corrections
    if (gMC->TrackStep() > 0) QCF = 1. + (BirkConst / gMC->TrackStep()) * gMC->Edep();
    else QCF = 1;
    fELoss += (gMC->Edep()) / QCF;

    gMC->TrackPosition(tPos);
    gMC->TrackMomentum(tMom);

    if (fELoss > 0) {

      if (copyNoVTYVECCom == slice && copyNoVMODCom == module) {
        if (!GetHit(slice, module)) {
          AddHit(fTrackID, ivol, slice, module, TVector3(tPos.X(), tPos.Y(), tPos.Z()), TVector3(tMom.Px(), tMom.Py(), tMom.Pz()), time, length, fELoss);
        } else {
          GetHit(slice, module)->AddVSC(fTrackID, ivol, slice, module, TVector3(tPos.X(), tPos.Y(), tPos.Z()), TVector3(tMom.Px(), tMom.Py(), tMom.Pz()), time, length, fELoss);
        }
      }//if(copyNoVTYVECCom==slice && copyNoVMODCom==module)	  

    }//if(fELoss>0)
  }//if ( gMC->IsTrackExiting()) {

  Int_t points = gMC->GetStack()->GetCurrentTrack()->GetMother(1);

  points = (points & (~(1 << 30))) | (1 << 30);

  gMC->GetStack()->GetCurrentTrack()->SetMother(1, points);

  ((CbmStack*)gMC->GetStack())->AddPoint(kFHCAL);

  return kTRUE;

}

// -----   Public method EndOfEvent   -----------------------------------------
void BmnFHCal::EndOfEvent() {
  if (fVerboseLevel)  Print();
  Reset();
}


// -----   Public method Register   -------------------------------------------
void BmnFHCal::Register() {
  FairRootManager::Instance()->Register("FHCalPoint", "FHCal", fFHCalCollection, kTRUE);
}

// -----   Public method GetCollection   --------------------------------------
TClonesArray* BmnFHCal::GetCollection(Int_t iColl) const {
  if (iColl == 0) return fFHCalCollection;

  return NULL;
}

// -----   Public method Print   ----------------------------------------------
void BmnFHCal::Print() const {
  Int_t nHits = fFHCalCollection->GetEntriesFast();
  cout << "-I- BmnFHCal: " << nHits << " points registered in this event."
    << endl;

  if (fVerboseLevel > 1)
    for (Int_t i = 0; i < nHits; i++) (*fFHCalCollection)[i]->Print();
}

// -----   Public method Reset   ----------------------------------------------
void BmnFHCal::Reset() {
  fFHCalCollection->Delete();

  fPosIndex = 0;
}

// guarda in FairRootManager::CopyClones
// -----   Public method CopyClones   -----------------------------------------
void BmnFHCal::CopyClones(TClonesArray* cl1, TClonesArray* cl2, Int_t offset) {
  Int_t nEntries = cl1->GetEntriesFast();
  //cout << "-I- BmnFHCal: " << nEntries << " entries to add." << endl;
  TClonesArray& clref = *cl2;
  BmnFHCalPoint* oldpoint = NULL;
  for (Int_t i = 0; i < nEntries; i++) {
    oldpoint = (BmnFHCalPoint*)cl1->At(i);
    Int_t index = oldpoint->GetTrackID() + offset;
    oldpoint->SetTrackID(index);
    new (clref[fPosIndex]) BmnFHCalPoint(*oldpoint);
    fPosIndex++;
  }
  cout << " -I- BmnFHCal: " << cl2->GetEntriesFast() << " merged entries."
    << endl;
}

// -----   Public method ConstructGeometry   ---------------------------------
void BmnFHCal::ConstructGeometry() {

  TString fileName = GetGeometryFileName();
  if (fileName.EndsWith(".root")) {
    LOG(info) << ("Constructing FHCal geometry from ROOT file %s", fileName.Data());
    ConstructRootGeometry();
  }
  /*
    else if ( fileName.EndsWith(".geo") )
    {
    FairLogger::GetLogger()->Info(MESSAGE_ORIGIN, "Constructing FHCal geometry from ASCII file %s", fileName.Data());
    ConstructAsciiGeometry();
    }
    else	FairLogger::GetLogger()->Fatal(MESSAGE_ORIGIN, "Geometry format of FHCal file %S not supported.", fileName.Data());
  */

  FairGeoLoader* geoLoad = FairGeoLoader::Instance();
  FairGeoInterface* geoFace = geoLoad->getGeoInterface();
  BmnFHCalGeo* FHCalGeo = new BmnFHCalGeo();
  FHCalGeo->setGeomFile(GetGeometryFileName());
  geoFace->addGeoModule(FHCalGeo);

  Bool_t rc = geoFace->readSet(FHCalGeo);
  if (rc) FHCalGeo->create(geoLoad->getGeoBuilder());
  TList* volList = FHCalGeo->getListOfVolumes();

  // store geo parameter
  FairRun* fRun = FairRun::Instance();
  FairRuntimeDb* rtdb = FairRun::Instance()->GetRuntimeDb();
  BmnFHCalGeoPar* par = (BmnFHCalGeoPar*)(rtdb->getContainer("BmnFHCalGeoPar"));
  TObjArray* fSensNodes = par->GetGeoSensitiveNodes();
  TObjArray* fPassNodes = par->GetGeoPassiveNodes();

  TListIter iter(volList);
  FairGeoNode* node = NULL;
  FairGeoVolume* aVol = NULL;

  while ((node = (FairGeoNode*)iter.Next())) {
    aVol = dynamic_cast<FairGeoVolume*> (node);


    if (node->isSensitive()) {
      fSensNodes->AddLast(aVol);
    } else {
      fPassNodes->AddLast(aVol);
    }
  }
  par->setChanged();
  par->setInputVersion(fRun->GetRunId(), 1);

  ProcessNodes(volList);
}

//Check if Sensitive-----------------------------------------------------------
Bool_t BmnFHCal::CheckIfSensitive(std::string name) {
  TString tsname = name;
  //if (tsname.Contains("zdc01s") || tsname.Contains("zdc01s_NICA")) {
  if (tsname.Contains("fhcal01s") || tsname.Contains("fhcal01s_NICA")) {
    return kTRUE;
  }
  return kFALSE;
}

// -----   Private method AddHit   --------------------------------------------
BmnFHCalPoint* BmnFHCal::AddHit(Int_t trackID, Int_t detID, Int_t copyNo, Int_t copyNoMother, TVector3 pos, TVector3 mom, Double_t time, Double_t length, Double_t eLoss) {
  TClonesArray& clref = *fFHCalCollection;
  Int_t size = clref.GetEntriesFast();

  return new(clref[size]) BmnFHCalPoint(trackID, detID, copyNo, copyNoMother, pos, mom, time, length, eLoss);

}


ClassImp(BmnFHCal)
