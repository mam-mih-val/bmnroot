
/*************************************************************************************
 *
 *         Class BmnBd
 *         
 *  Adopted for BMN by:   Elena Litvinenko (EL)
 *  e-mail:   litvin@nf.jinr.ru
 *  Version:  10-02-2016
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
#include "BmnBdGeo.h"
#include "FairGeoRootBuilder.h"
#include "CbmStack.h"
#include "BmnBd.h"
#include "BmnBdPoint.h"

#include "FairRootManager.h"
#include "FairVolume.h"
// add on for debug
//#include "FairGeoG3Builder.h"
#include "FairRuntimeDb.h"
#include "TObjArray.h"
#include "FairRun.h"

#include "TParticlePDG.h"

// -----   Default constructor   -------------------------------------------
BmnBd::BmnBd() {
  fBdCollection        = new TClonesArray("BmnBdPoint");
  volDetector = 0;
  fPosIndex   = 0; 
  // fpreflag = 0;  
  //fpostflag = 0;
  fEventID=-1; 
  fVerboseLevel = 1;

}
// -------------------------------------------------------------------------

// -----   Standard constructor   ------------------------------------------
BmnBd::BmnBd(const char* name, Bool_t active)
  : FairDetector(name, active) {
    fBdCollection        = new TClonesArray("BmnBdPoint");
    fPosIndex   = 0;
    volDetector = 0;
    //fpreflag = 0;  
    //fpostflag = 0;
    fEventID=-1;
    fVerboseLevel = 1;
}
// -------------------------------------------------------------------------



// -----   Destructor   ----------------------------------------------------
BmnBd::~BmnBd() {
  if (fBdCollection) {
    fBdCollection->Delete();
    delete fBdCollection;
  }
  
}
// -------------------------------------------------------------------------



// -----   Public method Intialize   ---------------------------------------
void BmnBd::Initialize() {
  // Init function
  
  FairDetector::Initialize();
  FairRun* sim = FairRun::Instance();
  FairRuntimeDb* rtdb=sim->GetRuntimeDb();
}
// -------------------------------------------------------------------------
void BmnBd::BeginEvent(){
  // Begin of the event
  
}



// -----   Public method ProcessHits  --------------------------------------
Bool_t BmnBd::ProcessHits(FairVolume* vol) {

  // if (TMath::Abs(gMC->TrackCharge()) <= 0) return kFALSE;

  Int_t      ivol    = vol->getMCid();
  TLorentzVector tPos1, tMom1;
      TLorentzVector tPos, tMom;

  //#define EDEBUG
#ifdef EDEBUG
  static Int_t lEDEBUGcounter=0;
  if (lEDEBUGcounter<1)
    std::cout << "EDEBUG-- BmnBd::ProcessHits: entered" << gMC->CurrentVolPath() << endl;
#endif

    if (gMC->IsTrackEntering()) {

      ResetParameters();
      fELoss = 0.;
#ifdef EDEBUG
      gMC->TrackPosition(tPos1);
      gMC->TrackMomentum(tMom1);
#endif
    }

    Double_t eLoss   = gMC->Edep();
    if (eLoss != 0.) 
      fELoss += eLoss;

    if ( gMC->IsTrackExiting()    ||
	 gMC->IsTrackStop()       ||
	 gMC->IsTrackDisappeared()   ) {

#ifndef EDEBUG
      if (fELoss == 0. ) return kFALSE;
#endif

      TParticle* part    = gMC->GetStack()->GetCurrentTrack();
      Double_t charge = part->GetPDG()->Charge() / 3. ;

      // Create BmnBdPoint
      fTrackID = gMC->GetStack()->GetCurrentTrackNumber();
      Double_t time    = gMC->TrackTime() * 1.0e09;
      Double_t length  = gMC->TrackLength();
      gMC->TrackPosition(tPos);
      gMC->TrackMomentum(tMom);

      Int_t copyNo;
      Int_t ivol1 = gMC->CurrentVolID(copyNo);
      //      ivol1 = vol->getVolumeId();
      Int_t iCell ;
      gMC->CurrentVolOffID(1, iCell); 
 	

#ifdef EDEBUG
      if (lEDEBUGcounter<100) {
	std::cout << "EDEBUG-- BmnBd::ProcessHits: TrackID:" << fTrackID << 
	  //	  " ELoss: " << fELoss << 
	  //	  "   particle: " << (part->GetName()) << 
	  "   " << gMC->CurrentVolPath() << " " << tPos.Z() << 
	  //          "   " << (gMC->GetStack()->GetCurrentTrack()->GetMother(1)) << 
	   	  "   "  << ivol << "=="<< gMC->CurrentVolID(copyNo) << ","<< copyNo <<
	   	  "   "  << gMC->CurrentVolOffID(1,iCell) << " " << iCell << 
	  " " <<  gMC->CurrentVolOffName(1) << " " << gMC->CurrentVolOffName(0) <<
	  //	  "   " << vol->getRealName() << "  " << gMC->CurrentVolPath() <<
	  //	  "   ivol,iCell,copyNo= " << ivol << ","<< iCell << ","<< copyNo << 
	  //	  "   " << vol->getRealName() << "  "<< gMC->CurrentVolName() << "  "<< gMC->CurrentVolPath() <<
	  //	  "   "  << ivol << ","<< vol->getVolumeId() << " : "<< gMC->CurrentVolID(copyNo) << ","<< copyNo <<
	  //          "  "<< gMC->CurrentVolOffName(2) << "  "<< gMC->CurrentVolOffName(3) <<
	  std::endl;
	lEDEBUGcounter++;
      } 

      //	AddHit(fTrackID, ivol, copyNo, iCell, TVector3(tPos.X(), tPos.Y(), tPos.Z()),
	AddHit(fTrackID, ivol, copyNo, TVector3(tPos.X(), tPos.Y(), tPos.Z()),
	       TVector3(tMom.Px(), tMom.Py(), tMom.Pz()),
	       time, length, fELoss);
#else

	//      AddHit(fTrackID, ivol, copyNo, iCell, TVector3(tPos.X(), tPos.Y(), tPos.Z()),
      AddHit(fTrackID, ivol, copyNo, TVector3(tPos.X(), tPos.Y(), tPos.Z()),
	     TVector3(tMom.Px(), tMom.Py(), tMom.Pz()),
	     time, length, fELoss);
#endif

////      Int_t points = gMC->GetStack()->GetCurrentTrack()->GetMother(1);
//       Int_t nBdPoints = (points & (1<<30)) >> 30;
//       nBdPoints ++;
//       if (nBdPoints > 1) nBdPoints = 1;
//      points = ( points & ( ~ (1<<30) ) ) | (nBdPoints << 30);
////       points = ( points & ( ~ (1<<30) ) ) | (1 << 30);
////       gMC->GetStack()->GetCurrentTrack()->SetMother(1,points);

      ((CbmStack*)gMC->GetStack())->AddPoint(kBD);

    }
  
    return kTRUE;
  
    //  }
#undef EDEBUG
}

// ----------------------------------------------------------------------------

// -----   Public method EndOfEvent   -----------------------------------------
void BmnBd::EndOfEvent() {
  if (fVerboseLevel)  Print();
  Reset();
}


// -----   Public method Register   -------------------------------------------
void BmnBd::Register() {
  FairRootManager::Instance()->Register("BdPoint","Bd", fBdCollection, kTRUE);
}


// -----   Public method GetCollection   --------------------------------------
TClonesArray* BmnBd::GetCollection(Int_t iColl) const {
   if (iColl == 0) return fBdCollection;

  return NULL;
}


// -----   Public method Print   ----------------------------------------------
void BmnBd::Print() const {
    Int_t nHits = fBdCollection->GetEntriesFast();
    cout << "-I- BmnBd: " << nHits << " points registered in this event."
 	<< endl;

    if (fVerboseLevel>1)
      for (Int_t i=0; i<nHits; i++) (*fBdCollection)[i]->Print();
}




// -----   Public method Reset   ----------------------------------------------
void BmnBd::Reset() {
   fBdCollection->Delete();
 
  fPosIndex = 0;
}


// guarda in FairRootManager::CopyClones
// -----   Public method CopyClones   -----------------------------------------
void BmnBd::CopyClones(TClonesArray* cl1, TClonesArray* cl2, Int_t offset ) {
  Int_t nEntries = cl1->GetEntriesFast();
  //cout << "-I- BmnBd: " << nEntries << " entries to add." << endl;
  TClonesArray& clref = *cl2;
  BmnBdPoint* oldpoint = NULL;
  for (Int_t i=0; i<nEntries; i++) {
    oldpoint = (BmnBdPoint*) cl1->At(i);
    Int_t index = oldpoint->GetTrackID() + offset;
    oldpoint->SetTrackID(index);
    new (clref[fPosIndex]) BmnBdPoint(*oldpoint);
    fPosIndex++;
  }
  cout << " -I- BmnBd: " << cl2->GetEntriesFast() << " merged entries."
       << endl;
}

 // -----   Public method ConstructGeometry   ----------------------------------
void BmnBd::ConstructGeometry() {
 FairGeoLoader*    geoLoad = FairGeoLoader::Instance();
  FairGeoInterface* geoFace = geoLoad->getGeoInterface();
  BmnBdGeo*      bdGeo = new BmnBdGeo();
  bdGeo->setGeomFile(GetGeometryFileName());
  geoFace->addGeoModule(bdGeo);

  Bool_t rc = geoFace->readSet(bdGeo);
  if (rc) bdGeo->create(geoLoad->getGeoBuilder());
  TList* volList = bdGeo->getListOfVolumes();

  // store geo parameter
  FairRun *fRun = FairRun::Instance();
  FairRuntimeDb *rtdb= FairRun::Instance()->GetRuntimeDb();
  BmnBdGeoPar* par=(BmnBdGeoPar*)(rtdb->getContainer("BmnBdGeoPar"));
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
  
 

// -----   Private method AddHit   --------------------------------------------
BmnBdPoint* BmnBd::AddHit(Int_t trackID, Int_t detID, Int_t copyNo,
			    TVector3 pos, TVector3 mom, Double_t time, 
			    Double_t length, Double_t eLoss) {
  TClonesArray& clref = *fBdCollection;
  Int_t size = clref.GetEntriesFast();
  return new(clref[size]) BmnBdPoint(trackID, detID, copyNo, pos, mom, 
				      time, length, eLoss);
 }



// ----


ClassImp(BmnBd)
