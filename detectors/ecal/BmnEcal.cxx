
/*************************************************************************************
 *
 *         Class BmnEcal
 *         
 *  Adopted for BMN by:   Elena Litvinenko (EL)
 *  e-mail:   litvin@nf.jinr.ru
 *  Version:  10-02-2016
 *  Last update:  10-02-2016 (EL)  
 *  Updated:  23-06-2021 by Petr Alekseev <pnaleks@gmail.com>
 *
 ************************************************************************************/

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
#include "BmnEcalGeo.h"
#include "FairGeoRootBuilder.h"
#include "CbmStack.h"
#include "BmnEcal.h"
#include "BmnEcalPoint.h"

#include "FairRootManager.h"
#include "FairVolume.h"
#include "FairRuntimeDb.h"
#include "TObjArray.h"
#include "FairRun.h"

#include "TParticlePDG.h"

// -----   Default constructor   -------------------------------------------
BmnEcal::BmnEcal() {
	fEcalCollection = new TClonesArray("BmnEcalPoint");
	fVerboseLevel = 1;
}
// -------------------------------------------------------------------------

// -----   Standard constructor   ------------------------------------------
BmnEcal::BmnEcal(const char* name, Bool_t active)
  : FairDetector(name, active) {
	fEcalCollection = new TClonesArray("BmnEcalPoint");
	fVerboseLevel = 1;
}
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
BmnEcal::~BmnEcal() {
  if (fEcalCollection) {
    fEcalCollection->Delete();
    delete fEcalCollection;
  }
}
// -------------------------------------------------------------------------

// -----   Public method Intialize   ---------------------------------------
void BmnEcal::Initialize() {
  // Init function
  
  FairDetector::Initialize();
  FairRun* sim = FairRun::Instance();
  FairRuntimeDb* rtdb=sim->GetRuntimeDb();
}
// -------------------------------------------------------------------------

void BmnEcal::BeginEvent(){
  // Begin of the event
  
}

// -----   Public method ProcessHits  --------------------------------------
Bool_t BmnEcal::ProcessHits(FairVolume* vol) {

//#define EDEBUG
#ifdef EDEBUG
	static Int_t lEDEBUGcounter=0;
	if (lEDEBUGcounter<1)
		std::cout << "EDEBUG-- BmnEcal::ProcessHits: entered" << gMC->CurrentVolPath() << endl;
#endif
//#undef EDEBUG
	
	if (gMC->IsTrackEntering()) {
		fELoss = 0.;
		fTime = gMC->TrackTime() * 1.0e09;
		fLength = gMC->TrackLength();
		gMC->TrackPosition(fPos);
		gMC->TrackMomentum(fMom);
    }

    fELoss += gMC->Edep();

    if (gMC->IsTrackExiting() || gMC->IsTrackStop() || gMC->IsTrackDisappeared()) {
		
		if (fELoss == 0. ) return kFALSE;
		
		fTrackID = gMC->GetStack()->GetCurrentTrackNumber();
		fVolumeID = vol->getMCid();
		
		// Using data of the exiting track instead of the entering (why?)
		fTime = gMC->TrackTime() * 1.0e09;
		fLength = gMC->TrackLength();
		gMC->TrackPosition(fPos);
		gMC->TrackMomentum(fMom);
		
		Int_t copyNo;
		gMC->CurrentVolID(copyNo);
		Int_t copyNoMother;
		gMC->CurrentVolOffID(1, copyNoMother); 
		
		AddHit(fTrackID, fVolumeID, copyNo, copyNoMother,
				TVector3(fPos.X(), fPos.Y(), fPos.Z()),
				TVector3(fMom.Px(), fMom.Py(), fMom.Pz()),
				fTime, fLength, fELoss);
				
		((CbmStack*)gMC->GetStack())->AddPoint(kECAL);
	}
  
    return kTRUE;
  
}

// ----------------------------------------------------------------------------

// -----   Public method EndOfEvent   -----------------------------------------
void BmnEcal::EndOfEvent() {
  if (fVerboseLevel)  Print();
  Reset();
}


// -----   Public method Register   -------------------------------------------
void BmnEcal::Register() {
  FairRootManager::Instance()->Register("EcalPoint","Ecal", fEcalCollection, fToFile);
}


// -----   Public method GetCollection   --------------------------------------
TClonesArray* BmnEcal::GetCollection(Int_t iColl) const {
   if (iColl == 0) return fEcalCollection;

  return NULL;
}


// -----   Public method Print   ----------------------------------------------
void BmnEcal::Print() const {
    Int_t nHits = fEcalCollection->GetEntriesFast();
    cout << "-I- BmnEcal: " << nHits << " points registered in this event."
 	<< endl;

    if (fVerboseLevel>1)
      for (Int_t i=0; i<nHits; i++) (*fEcalCollection)[i]->Print();
}




// -----   Public method Reset   ----------------------------------------------
void BmnEcal::Reset() {
   fEcalCollection->Clear();
}


// guarda in FairRootManager::CopyClones
// -----   Public method CopyClones   -----------------------------------------
void BmnEcal::CopyClones(TClonesArray* cl1, TClonesArray* cl2, Int_t offset ) {
  Int_t nEntries = cl1->GetEntriesFast();
  //cout << "-I- BmnEcal: " << nEntries << " entries to add." << endl;
  TClonesArray& clref = *cl2;
  BmnEcalPoint* oldpoint = NULL;
  for (Int_t i=0; i<nEntries; i++) {
    oldpoint = (BmnEcalPoint*) cl1->At(i);
    Int_t index = oldpoint->GetTrackID() + offset;
    oldpoint->SetTrackID(index);
    new (clref[cl2->GetEntriesFast()]) BmnEcalPoint(*oldpoint);
  }
  cout << " -I- BmnEcal: " << cl2->GetEntriesFast() << " merged entries."
       << endl;
}

 // -----   Public method ConstructGeometry   ----------------------------------
void BmnEcal::ConstructGeometry() {

  TString geoFileName = GetGeometryFileName();
  if(geoFileName.EndsWith(".root")) {
    LOG(info) << "Constructing ECAL geometry from ROOT file " << geoFileName.Data();
    ConstructRootGeometry();
	return;
  }
    
 FairGeoLoader*    geoLoad = FairGeoLoader::Instance();
  FairGeoInterface* geoFace = geoLoad->getGeoInterface();
  BmnEcalGeo*      ecalGeo = new BmnEcalGeo();
  ecalGeo->setGeomFile(GetGeometryFileName());
  geoFace->addGeoModule(ecalGeo);

  Bool_t rc = geoFace->readSet(ecalGeo);
  if (rc) ecalGeo->create(geoLoad->getGeoBuilder());
  TList* volList = ecalGeo->getListOfVolumes();

  // store geo parameter
  FairRun *fRun = FairRun::Instance();
  FairRuntimeDb *rtdb= FairRun::Instance()->GetRuntimeDb();
  BmnEcalGeoPar* par=(BmnEcalGeoPar*)(rtdb->getContainer("BmnEcalGeoPar"));
  TObjArray *fSensNodes = par->GetGeoSensitiveNodes();
  TObjArray *fPassNodes = par->GetGeoPassiveNodes();

  TListIter iter(volList);
  FairGeoNode* node   = NULL;
  FairGeoVolume *aVol=NULL;

  while( (node = (FairGeoNode*)iter.Next()) ) {
    aVol = dynamic_cast<FairGeoVolume*> ( node );


    if ( node->isSensitive()  ) {
      fSensNodes->AddLast( aVol );
    }else{
      fPassNodes->AddLast( aVol );
    }
  }
  par->setChanged();
  par->setInputVersion(fRun->GetRunId(),1);

  ProcessNodes ( volList );
}
  
// -----   Public method CheckIfSensitive   -----------------------------------
Bool_t BmnEcal::CheckIfSensitive(std::string name){
    TString tsname = name;
    if (tsname.Contains("laySci")) return kTRUE; //lay
    
    return kFALSE;
}
 

// -----   Private method AddHit   --------------------------------------------
BmnEcalPoint* BmnEcal::AddHit(Int_t trackID, Int_t detID, Int_t copyNo, Int_t copyNoMother,
			    TVector3 pos, TVector3 mom, Double_t time, 
			    Double_t length, Double_t eLoss) {
  TClonesArray& clref = *fEcalCollection;
  Int_t size = clref.GetEntriesFast();
  return new(clref[size]) BmnEcalPoint(trackID, detID, copyNo, copyNoMother,pos, mom, 
				      time, length, eLoss);
 }



// ----


ClassImp(BmnEcal)
