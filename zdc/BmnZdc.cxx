
/*************************************************************************************
 *
 *         Class BmnZdc
 *         
 *  Adopted for BMN by:   Elena Litvinenko (EL)
 *  e-mail:   litvin@nf.jinr.ru
 *  Version:  06-11-2015
 *  Last update:  22-Feb-2012 (EL)  
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
#include "BmnZdcGeo.h"
#include "FairGeoRootBuilder.h"
#include "CbmStack.h"
#include "BmnZdc.h"
#include "BmnZdcPoint.h"

#include "FairRootManager.h"
#include "FairVolume.h"
// add on for debug
//#include "FairGeoG3Builder.h"
#include "FairRuntimeDb.h"
#include "TObjArray.h"
#include "FairRun.h"

#include "TParticlePDG.h"

// -----   Default constructor   -------------------------------------------
BmnZdc::BmnZdc() {
  fZdcCollection        = new TClonesArray("BmnZdcPoint");
  volDetector = 0;
  fPosIndex   = 0; 
  // fpreflag = 0;  
  //fpostflag = 0;
  fEventID=-1; 
  fVerboseLevel = 1;

}
// -------------------------------------------------------------------------

// -----   Standard constructor   ------------------------------------------
BmnZdc::BmnZdc(const char* name, Bool_t active)
  : FairDetector(name, active) {
    fZdcCollection        = new TClonesArray("BmnZdcPoint");
    fPosIndex   = 0;
    volDetector = 0;
    //fpreflag = 0;  
    //fpostflag = 0;
    fEventID=-1;
    fVerboseLevel = 1;
}
// -------------------------------------------------------------------------



// -----   Destructor   ----------------------------------------------------
BmnZdc::~BmnZdc() {
  if (fZdcCollection) {
    fZdcCollection->Delete();
    delete fZdcCollection;
  }
  
}
// -------------------------------------------------------------------------



// -----   Public method Intialize   ---------------------------------------
void BmnZdc::Initialize() {
  // Init function
  
  FairDetector::Initialize();
  FairRun* sim = FairRun::Instance();
  FairRuntimeDb* rtdb=sim->GetRuntimeDb();
}
// -------------------------------------------------------------------------
void BmnZdc::BeginEvent(){
  // Begin of the event
  
}



// -----   Public method ProcessHits  --------------------------------------
Bool_t BmnZdc::ProcessHits(FairVolume* vol) {

  // if (TMath::Abs(gMC->TrackCharge()) <= 0) return kFALSE;

  Int_t      ivol    = vol->getMCid();
  TLorentzVector tPos1, tMom1;

  //#define EDEBUG
#ifdef EDEBUG
  static Int_t lEDEBUGcounter=0;
  if (lEDEBUGcounter<1)
    std::cout << "EDEBUG-- BmnZdc::ProcessHits: entered" << gMC->CurrentVolPath() << endl;
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
#else
      if ((fELoss == 0. ) && 
	  (!((gMC->GetStack()->GetCurrentTrack()->GetPdgCode()==2112)&&(gMC->GetStack()->GetCurrentTrack()->GetMother(0)==-1)))
) return kFALSE;
#endif

      TParticle* part    = gMC->GetStack()->GetCurrentTrack();
      Double_t charge = part->GetPDG()->Charge() / 3. ;

      // Create BmnZdcPoint
      fTrackID = gMC->GetStack()->GetCurrentTrackNumber();
      Double_t time    = gMC->TrackTime() * 1.0e09;
      Double_t length  = gMC->TrackLength();
      TLorentzVector tPos, tMom;
      gMC->TrackPosition(tPos);
      gMC->TrackMomentum(tMom);

      Int_t copyNo;
      Int_t ivol1 = gMC->CurrentVolID(copyNo);
      //      ivol1 = vol->getVolumeId();
      Int_t iCell, iCell2 ;
      gMC->CurrentVolOffID(1, iCell); 
      gMC->CurrentVolOffID(2, iCell2); 
 	

#ifdef EDEBUG
      static Bool_t already=0;
      if (lEDEBUGcounter<200) {
	std::cout << "EDEBUG-- BmnZdc::ProcessHits: TrackID:" << fTrackID << 
	  //	  " ELoss: " << fELoss << 
	  //	  	  "   particle: " << (part->GetName()) << 
	  "   " << gMC->CurrentVolPath() << " " << tPos.X()<< " " << tPos.Y() << " " << tPos.Z() << "  " << part->GetPDG()->PdgCode()<<
	  //          "   " << (gMC->GetStack()->GetCurrentTrack()->GetMother(1)) << 
	  //	   	  "   "  << ivol << "=="<< gMC->CurrentVolID(copyNo) << ","<< copyNo <<
	  "   "  << ivol << "  " << vol->getRealName() << "   " <<
	  "   "  << gMC->CurrentVolOffID(1,iCell) << " " <<  gMC->CurrentVolOffName(1) << 
	  "   " << iCell << " " << iCell2 << "  "<< gMC->CurrentVolOffName(2) << 
	  "   " << vol->getModId()<< " " << 
	  //	  " " <<  gMC->CurrentVolOffName(2) << " " <<  gMC->CurrentVolOffName(1) << " " << gMC->CurrentVolOffName(0) << " " 
 	  	  // "   " << vol->getRealName() << "  " << gMC->CurrentVolPath() <<
	  	  // "   ivol,iCell,copyNo= " << ivol << ","<< iCell << ","<< copyNo << 
	  	  // "   " << vol->getRealName() << "  "<< gMC->CurrentVolName() << "  "<< gMC->CurrentVolPath() <<
	  	  // "   "  << ivol << ","<< vol->getVolumeId() << " : "<< gMC->CurrentVolID(copyNo) << ","<< copyNo <<
	          //  "  "<< gMC->CurrentVolOffName(2) << "  " <<
	  //	           "  "<< gMC->CurrentVolOffName(2) << "  "<< gMC->CurrentVolOffName(3) <<
	  std::endl;
	//	vol->getGeoNode()->getMotherNode()->getMotherNode()->getCenterPosition().print();
	lEDEBUGcounter++;
      } 
      if ((iCell==2)&&(lEDEBUGcounter>=100)&&(!already)) {
	already=1;
	lEDEBUGcounter=0;
      }
//       if ((part->GetPdgCode())==321) {
// 	std::cout << "EDEBUG-- BmnZdc::ProcessHits(..)  K+:  " << fTrackID << "   " << (  gMC->IsTrackExiting()) << "  " <<
// 	  (gMC->IsTrackStop()) << "  " << (gMC->IsTrackDisappeared()) << "   " << fELoss << "  " << time << std::endl;
//       }
//#endif

//       if(copyNo==1)
// 	AddHit(fTrackID, ivol, copyNo, iCell, TVector3(tPos1.X(), tPos1.Y(), tPos1.Z()),
// 	       TVector3(tMom1.Px(), tMom1.Py(), tMom1.Pz()),
// 	       time, length, fELoss);
//       else 

	AddHit(fTrackID, ivol, copyNo, iCell2, TVector3(tPos.X(), tPos.Y(), tPos.Z()),
	       TVector3(tMom.Px(), tMom.Py(), tMom.Pz()),
	       time, length, fELoss);
#else

      AddHit(fTrackID, ivol, copyNo, iCell2, TVector3(tPos.X(), tPos.Y(), tPos.Z()),
	     TVector3(tMom.Px(), tMom.Py(), tMom.Pz()),
	     time, length, fELoss);
#endif

      Int_t points = gMC->GetStack()->GetCurrentTrack()->GetMother(1);
//       Int_t nZdcPoints = (points & (1<<30)) >> 30;
//       nZdcPoints ++;
//       if (nZdcPoints > 1) nZdcPoints = 1;
//      points = ( points & ( ~ (1<<30) ) ) | (nZdcPoints << 30);
      points = ( points & ( ~ (1<<30) ) ) | (1 << 30);
      gMC->GetStack()->GetCurrentTrack()->SetMother(1,points);

      ((CbmStack*)gMC->GetStack())->AddPoint(kZDC);

    }
   
//     Int_t copyNo;  
//     gMC->CurrentVolID(copyNo);
//     TString nam = gMC->GetMC()->GetName();
    //    cout<<"name "<<gMC->GetMC()->GetName()<<endl;
    //    ResetParameters();
  
    return kTRUE;
  
    //  }
#undef EDEBUG
}

// ----------------------------------------------------------------------------

// -----   Public method EndOfEvent   -----------------------------------------
void BmnZdc::EndOfEvent() {
  if (fVerboseLevel)  Print();
  Reset();
}


// -----   Public method Register   -------------------------------------------
void BmnZdc::Register() {
  FairRootManager::Instance()->Register("ZdcPoint","Zdc", fZdcCollection, kTRUE);
}


// -----   Public method GetCollection   --------------------------------------
TClonesArray* BmnZdc::GetCollection(Int_t iColl) const {
   if (iColl == 0) return fZdcCollection;

  return NULL;
}


// -----   Public method Print   ----------------------------------------------
void BmnZdc::Print() const {
    Int_t nHits = fZdcCollection->GetEntriesFast();
    cout << "-I- BmnZdc: " << nHits << " points registered in this event."
 	<< endl;

    if (fVerboseLevel>1)
      for (Int_t i=0; i<nHits; i++) (*fZdcCollection)[i]->Print();
}




// -----   Public method Reset   ----------------------------------------------
void BmnZdc::Reset() {
   fZdcCollection->Delete();
 
  fPosIndex = 0;
}


// guarda in FairRootManager::CopyClones
// -----   Public method CopyClones   -----------------------------------------
void BmnZdc::CopyClones(TClonesArray* cl1, TClonesArray* cl2, Int_t offset ) {
  Int_t nEntries = cl1->GetEntriesFast();
  //cout << "-I- BmnZdc: " << nEntries << " entries to add." << endl;
  TClonesArray& clref = *cl2;
  BmnZdcPoint* oldpoint = NULL;
  for (Int_t i=0; i<nEntries; i++) {
    oldpoint = (BmnZdcPoint*) cl1->At(i);
    Int_t index = oldpoint->GetTrackID() + offset;
    oldpoint->SetTrackID(index);
    new (clref[fPosIndex]) BmnZdcPoint(*oldpoint);
    fPosIndex++;
  }
  cout << " -I- BmnZdc: " << cl2->GetEntriesFast() << " merged entries."
       << endl;
}

 // -----   Public method ConstructGeometry   ----------------------------------
void BmnZdc::ConstructGeometry() {
 FairGeoLoader*    geoLoad = FairGeoLoader::Instance();
  FairGeoInterface* geoFace = geoLoad->getGeoInterface();
  BmnZdcGeo*      zdcGeo = new BmnZdcGeo();
  zdcGeo->setGeomFile(GetGeometryFileName());
  geoFace->addGeoModule(zdcGeo);

  Bool_t rc = geoFace->readSet(zdcGeo);
  if (rc) zdcGeo->create(geoLoad->getGeoBuilder());
  TList* volList = zdcGeo->getListOfVolumes();

  // store geo parameter
  FairRun *fRun = FairRun::Instance();
  FairRuntimeDb *rtdb= FairRun::Instance()->GetRuntimeDb();
  BmnZdcGeoPar* par=(BmnZdcGeoPar*)(rtdb->getContainer("BmnZdcGeoPar"));
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
BmnZdcPoint* BmnZdc::AddHit(Int_t trackID, Int_t module_groupID, Int_t copyNo, Int_t copyNoMother,
			    TVector3 pos, TVector3 mom, Double_t time, 
			    Double_t length, Double_t eLoss) {
  TClonesArray& clref = *fZdcCollection;
  Int_t size = clref.GetEntriesFast();
  return new(clref[size]) BmnZdcPoint(trackID, module_groupID, copyNo, copyNoMother,pos, mom, 
				      time, length, eLoss);
 }



// ----


ClassImp(BmnZdc)
