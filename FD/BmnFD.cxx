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
#include "BmnFDGeo.h"
#include "FairGeoRootBuilder.h"
#include "CbmStack.h"
#include "BmnFD.h"
#include "BmnFDPoint.h"

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
BmnFD::BmnFD() {
  fFDCollection        = new TClonesArray("BmnFDPoint");
  volDetector = 0;
  fPosIndex   = 0; 
  // fpreflag = 0;  
  //fpostflag = 0;
 // fEventID=-1; 
  fVerboseLevel = 1;

}
// -------------------------------------------------------------------------

// -----   Standard constructor   ------------------------------------------
BmnFD::BmnFD(const char* name, Bool_t active)
  : FairDetector(name, active) {
    fFDCollection        = new TClonesArray("BmnFDPoint");
    fPosIndex   = 0;
    volDetector = 0;
    //fpreflag = 0;  
    //fpostflag = 0;
    //fEventID=-1;
    fVerboseLevel = 1;
}
// -------------------------------------------------------------------------



// -----   Destructor   ----------------------------------------------------
BmnFD::~BmnFD() {
  if (fFDCollection) {
    fFDCollection->Delete();
    delete fFDCollection;
  }
  
}
// -------------------------------------------------------------------------

// -----   Public method Intialize   ---------------------------------------
void BmnFD::Initialize() {
  // Init function
  
  FairDetector::Initialize();
  FairRun* sim = FairRun::Instance();
  FairRuntimeDb* rtdb=sim->GetRuntimeDb();
}
// -------------------------------------------------------------------------
void BmnFD::BeginEvent(){
  // Begin of the event
  
}



// -----   Public method ProcessHits  --------------------------------------
Bool_t BmnFD::ProcessHits(FairVolume* vol) {

  // if (TMath::Abs(gMC->TrackCharge()) <= 0) return kFALSE;

static const Double_t dP = 1.032 ;
static const Double_t BirkC1 =  0.013/dP;
static const Double_t BirkC2 =  9.6e-6/(dP * dP);

static Double_t lightYield;	
static Double_t timeIn;	
static Double_t timeOut;	
static Double_t lengthtrack;	
static Double_t de_dx;	
	
	
  Int_t      ivol    = vol->getMCid();
  //TLorentzVector tPos1, tMom1;
  //TLorentzVector tPos, tMom;

 
  
    //#define EDEBUG
#ifdef EDEBUG
  static Int_t lEDEBUGcounter=0;
  if (lEDEBUGcounter<1)
    std::cout << "EDEBUG-- BmnBd1::ProcessHits: entered" << gMC->CurrentVolPath() << endl;
#endif

	if(gMC->IsTrackEntering()){

		ResetParameters();
     
    	fELoss = 0.;
        fIsPrimary = 0;
        fCharge = -1;
        fPdgId = 0;
	    lightYield = 0.;
	    lengthtrack = 0.;
	   		
        TLorentzVector PosIn;
        gMC->TrackPosition(PosIn);
        fPosIn.SetXYZ(PosIn.X(), PosIn.Y(), PosIn.Z());

        TLorentzVector MomIn;
        gMC->TrackMomentum(MomIn);
        fMomIn.SetXYZ(MomIn.Px(), MomIn.Py(), MomIn.Pz());
	    	
	    timeIn = gMC->TrackTime() * 1.0e09;
			
		
#ifdef EDEBUG
      //gMC->TrackPosition(tPos1);
      //gMC->TrackMomentum(tMom1);
#endif		
	
		TParticle* part = 0;
        part = gMC->GetStack()->GetCurrentTrack();
        
		if (part) {
            fIsPrimary = (Int_t)part->IsPrimary();
            fCharge = (Int_t)part->GetPDG()->Charge() / 3.;
            fPdgId = (Int_t)part->GetPdgCode();
       	}
	
	}
		
	if (gMC->TrackCharge() != 0) { 
    	Double_t BirkC1Mod = 0;
         			
		if (TMath::Abs(gMC->TrackCharge())>=2)
       				
			BirkC1Mod = BirkC1 * 7.2/12.6;
     		else
       		BirkC1Mod = BirkC1;
    		if (gMC->TrackStep() > 0){
            	
			Double_t dedxcm = gMC -> Edep() * 1000./gMC -> TrackStep(); //[MeV/cm]
           	Double_t curLightYield = gMC -> Edep() * 1000./(1. + BirkC1Mod * dedxcm + BirkC2 * dedxcm * dedxcm); 
               curLightYield /= 1000.; //[GeV]
               lightYield += curLightYield;
	        }
	 
		}		 
			
	Double_t eLoss   = gMC->Edep();
    if (eLoss > 0) 
    	fELoss += eLoss;
    	
	Double_t  slengthtrack = gMC-> TrackStep(); 	
		lengthtrack += slengthtrack;	
	
	
	if ( gMC->IsTrackExiting() || gMC->IsTrackStop() || gMC->IsTrackDisappeared() && fELoss > 0 ) {
     
      		
		TLorentzVector PosOut;
        gMC->TrackPosition(PosOut);
        fPosOut.SetXYZ(PosOut.X(), PosOut.Y(), PosOut.Z());

        TLorentzVector MomOut;
        gMC->TrackMomentum(MomOut);
        fMomOut.SetXYZ(MomOut.Px(), MomOut.Py(), MomOut.Pz());
        
		timeOut  = gMC->TrackTime() * 1.0e09;

		#ifndef EDEBUG
     
		if (fELoss == 0. ) return kFALSE;
     
		#endif
	
		fTrackID = gMC->GetStack()->GetCurrentTrackNumber();
			   	
		Double_t time    = gMC->TrackTime() * 1.0e09;
     	Double_t length  = gMC->TrackLength();
     
		Int_t copyNo;
     	Int_t ivol1 = gMC->CurrentVolID(copyNo);
      
		Int_t iCell ;
      	gMC->CurrentVolOffID(1, iCell); 
 	
	
#ifdef EDEBUG
      if (lEDEBUGcounter<100) {
	std::cout << "EDEBUG-- BmnFD::ProcessHits: TrackID:" << fTrackID << 
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

		AddHit(fTrackID, ivol, copyNo,  fPosIn, fPosOut,
                                fMomIn, fMomOut,
	       time, length, fELoss, fIsPrimary, fCharge, fPdgId, lightYield, timeIn, timeOut, lengthtrack);
#else
		
		AddHit(fTrackID, ivol, copyNo,  fPosIn, fPosOut,
                                fMomIn, fMomOut,
	       time, length, fELoss, fIsPrimary, fCharge, fPdgId, lightYield, timeIn, timeOut, lengthtrack);
#endif

////      Int_t points = gMC->GetStack()->GetCurrentTrack()->GetMother(1);
//       Int_t nBdPoints = (points & (1<<30)) >> 30;
//       nBdPoints ++;
//       if (nBdPoints > 1) nBdPoints = 1;
//      points = ( points & ( ~ (1<<30) ) ) | (nBdPoints << 30);
////       points = ( points & ( ~ (1<<30) ) ) | (1 << 30);
////       gMC->GetStack()->GetCurrentTrack()->SetMother(1,points);

		((CbmStack*)gMC->GetStack())->AddPoint(kFD);
    
	}
  
    return kTRUE;
  
	#undef EDEBUG
}

// -----   Public method EndOfEvent   -----------------------------------------
void BmnFD::EndOfEvent() {
  if (fVerboseLevel)  Print();
  Reset();
}

// -----   Public method Register   -------------------------------------------
void BmnFD::Register() {
  FairRootManager::Instance()->Register("FDPoint","FD", fFDCollection, kTRUE);
}

// -----   Public method GetCollection   --------------------------------------
TClonesArray* BmnFD::GetCollection(Int_t iColl) const {
   if (iColl == 0) return fFDCollection;

  return NULL;
}

// -----   Public method Print   ----------------------------------------------
void BmnFD::Print() const {
    Int_t nHits = fFDCollection->GetEntriesFast();
    cout << "-I- BmnFD: " << nHits << " points registered in this event."
 	<< endl;

    if (fVerboseLevel>1)
      for (Int_t i=0; i<nHits; i++) (*fFDCollection)[i]->Print();
}


// -----   Public method Reset   ----------------------------------------------
void BmnFD::Reset() {
   fFDCollection->Delete();
 
  fPosIndex = 0;
}

// guarda in FairRootManager::CopyClones
// -----   Public method CopyClones   -----------------------------------------
void BmnFD::CopyClones(TClonesArray* cl1, TClonesArray* cl2, Int_t offset ) {
  Int_t nEntries = cl1->GetEntriesFast();
  //cout << "-I- BmnFD: " << nEntries << " entries to add." << endl;
  TClonesArray& clref = *cl2;
  BmnFDPoint* oldpoint = NULL;
  for (Int_t i=0; i<nEntries; i++) {
    oldpoint = (BmnFDPoint*) cl1->At(i);
    Int_t index = oldpoint->GetTrackID() + offset;
    oldpoint->SetTrackID(index);
    new (clref[fPosIndex]) BmnFDPoint(*oldpoint);
    fPosIndex++;
  }
  cout << " -I- BmnFD: " << cl2->GetEntriesFast() << " merged entries."
       << endl;
}

//--------------------------------------------------------------------------------------------------------------------------------------
void 			BmnFD::ConstructGeometry() 
{
	TString fileName = GetGeometryFileName();
	if(fileName.EndsWith(".root")) 
	{
		LOG(info) << "Constructing FD geometry from ROOT file " << fileName.Data();
		ConstructRootGeometry();
	}
	else if ( fileName.EndsWith(".geo") ) 
	{
		LOG(info) << "Constructing FD geometry from ASCII file " << fileName.Data();
		ConstructAsciiGeometry();
	}
	else	LOG(fatal) << "Geometry format of FD file " << fileName.Data() << " not supported.";    
}
//--------------------------------------------------------------------------------------------------------------------------------------
void 			BmnFD::ConstructAsciiGeometry() 
{
	FairGeoLoader*    geoLoad = FairGeoLoader::Instance();
	FairGeoInterface* geoFace = geoLoad->getGeoInterface();
	BmnFDGeo*       FDGeo  = new BmnFDGeo();
	FDGeo->setGeomFile(GetGeometryFileName());
	geoFace->addGeoModule(FDGeo);

	Bool_t rc = geoFace->readSet(FDGeo);
	if (rc) FDGeo->create(geoLoad->getGeoBuilder());
	TList* volList = FDGeo->getListOfVolumes();

	// store geo parameter
	FairRun *fRun = FairRun::Instance();
	FairRuntimeDb *rtdb = FairRun::Instance()->GetRuntimeDb();
	BmnFDGeoPar* par = (BmnFDGeoPar*)(rtdb->getContainer("BmnFDGeoPar"));
	TObjArray *fSensNodes = par->GetGeoSensitiveNodes();
	TObjArray *fPassNodes = par->GetGeoPassiveNodes();

	TListIter iter(volList);
	FairGeoNode *node = nullptr;
	FairGeoVolume *aVol = nullptr;

	while( (node = (FairGeoNode*)iter.Next()) ) 
	{
		aVol = dynamic_cast<FairGeoVolume*>(node);

		if(node->isSensitive())		fSensNodes->AddLast(aVol);
       		else				fPassNodes->AddLast(aVol);
	}
  
	par->setChanged();
	par->setInputVersion(fRun->GetRunId(),1);
	ProcessNodes( volList );
}

//--------------------------------------------------------------------------------------------------------------------------------------

Bool_t 			BmnFD::CheckIfSensitive(std::string name)
{
  TString tsname = name;
  if (tsname.Contains("Active")) return kTRUE;
  
return kFALSE;
}
//-------------------------------------------------------------------------

// -----   Private method AddHit   --------------------------------------------
BmnFDPoint* BmnFD::AddHit( Int_t trackID, Int_t detID, Int_t copyNo, TVector3 posIn, TVector3 posOut,
                TVector3 momIn, TVector3 momOut, Double_t time, Double_t length, Double_t eLoss, Bool_t isPrimary, 
				Double_t charge, Int_t pdgId, Double_t lightYield, Double_t timeIn, Double_t timeOut, Double_t lengthtrack) {
  
	TClonesArray& clref = *fFDCollection;
  	Int_t size = clref.GetEntriesFast();
  	return new(clref[size]) BmnFDPoint(trackID, detID, copyNo, posIn, posOut, momIn, momOut, time, length, eLoss, isPrimary, 
										charge, pdgId, lightYield, timeIn, timeOut, lengthtrack);
 }

ClassImp(BmnFD)
