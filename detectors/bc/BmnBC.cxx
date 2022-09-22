

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
#include "BmnBCGeo.h"
#include "FairGeoRootBuilder.h"
#include "CbmStack.h"
#include "BmnBC.h"
#include "BmnBCPoint.h"

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

//const Double_t shiftElBC4const = {}

// -----   Default constructor   -------------------------------------------
BmnBC::BmnBC() {
  fBCCollection        = new TClonesArray("BmnBCPoint");
  volDetector = 0;
  fPosIndex   = 0; 
  // fpreflag = 0;  
  //fpostflag = 0;
 // fEventID=-1; 
  fVerboseLevel = 1;

}
// -------------------------------------------------------------------------

// -----   Standard constructor   ------------------------------------------
BmnBC::BmnBC(const char* name, Bool_t active)
  : FairDetector(name, active) {
    fBCCollection        = new TClonesArray("BmnBCPoint");
    fPosIndex   = 0;
    volDetector = 0;
    //fpreflag = 0;  
    //fpostflag = 0;
    //fEventID=-1;
    fVerboseLevel = 1;
}
// -------------------------------------------------------------------------



// -----   Destructor   ----------------------------------------------------
BmnBC::~BmnBC() {
  if (fBCCollection) {
    fBCCollection->Delete();
    delete fBCCollection;
  }
  
}
// -------------------------------------------------------------------------



// -----   Public method Intialize   ---------------------------------------
void BmnBC::Initialize() {
  // Init function
  
  FairDetector::Initialize();
  FairRun* sim = FairRun::Instance();
  FairRuntimeDb* rtdb=sim->GetRuntimeDb();
}
// -------------------------------------------------------------------------
void BmnBC::BeginEvent(){
  // Begin of the event
  
}



// -----   Public method ProcessHits  --------------------------------------
Bool_t BmnBC::ProcessHits(FairVolume* vol) {

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
    std::cout << "EDEBUG-- BmnBC::ProcessHits: entered" << gMC->CurrentVolPath() << endl;
#endif

    if (gMC->IsTrackEntering()) {

      ResetParameters();
     
      fELoss = 0.;
      fIsPrimary = 0;
      fCharge = -1;
      fPdgId = 0;
	    lightYield = 0.;
	   
		
        TLorentzVector PosIn;
        gMC->TrackPosition(PosIn);
        fPosIn.SetXYZ(PosIn.X(), PosIn.Y(), PosIn.Z());

        if (PosIn.Z()<-810) {fStation = 1;} 
        else if(PosIn.Z()>-810 && PosIn.Z()< -730 ){fStation = 2;}
        else if(PosIn.Z()>-730 && PosIn.Z()< -472 ){fStation = 3;}
        else{fStation = 4;}


        TLorentzVector MomIn;
        gMC->TrackMomentum(MomIn);
        fMomIn.SetXYZ(MomIn.Px(), MomIn.Py(), MomIn.Pz());
		
		  timeIn = gMC->TrackTime() * 1.0e09;
		
#ifdef EDEBUG
      //gMC->TrackPosition(tPos1);
      //gMC->TrackMomentum(tMom1);
#endif
  
	}
	  Double_t frac6_2[7] = {1.23094, 1.24035, 1.1012, 1.1276, 1.17377, 1.27562, 1.24795};
    Double_t eLoss   = gMC->Edep();
    if (eLoss > 0.00005) 
      fELoss += eLoss;
	
    if ( gMC->IsTrackExiting() || gMC->IsTrackStop() || gMC->IsTrackDisappeared() && fELoss > 0 ) {

        TLorentzVector PosOut;
        gMC->TrackPosition(PosOut);
        fPosOut.SetXYZ(PosOut.X(), PosOut.Y(), PosOut.Z());
        TParticle *part1 = gMC->GetStack()->GetCurrentTrack();
        Int_t ch = 0;
        if (part1) {
          ch = (Int_t)(part1->GetPDG()->Charge() / 3.);
		    }

        if (PosOut.Z()<-810) {fStation = 1;}
        else if(PosOut.Z()>-810 && PosOut.Z()< -730 ){fStation = 2;}
        else if(PosOut.Z()>-730 && PosOut.Z()< -472 ){fStation = 3;}
        else{
          fStation = 4;
          // for future corrections in Eloss 
          //Double_t y =  PosOut.Y();
          //  if(y<-2){
          //    Double_t a = 0;
          //    Double_t b = 0;
          //    if(ch == 1){
          //      a = (frac6_2[0]*fELoss-fELoss)/(24);
          //      b = -a*(-2);
          //      fELoss = fELoss+b*y+a*y*y;
          //    }else if(ch == 2){
          //      a = (frac6_2[1]*fELoss-fELoss)/(24);
          //      b = -a*(-2);
          //      fELoss = fELoss+b*y+a*y*y;
          //    }else if(ch == 3){
          //      a = (frac6_2[2]*fELoss-fELoss)/(24);
          //      b = -a*(-2);
          //      fELoss = fELoss+b*y+a*y*y;
          //    }else if(ch == 4){
          //      a = (frac6_2[3]*fELoss-fELoss)/(24);
          //      b = -a*(-2);
          //      fELoss = fELoss+b*y+a*y*y;
          //    }else if(ch == 5){
          //      a = (frac6_2[4]*fELoss-fELoss)/(24);
          //      b = -a*(-2);
          //      fELoss = fELoss+b*y+a*y*y;
          //    }else if(ch == 6){
          //      a = (frac6_2[5]*fELoss-fELoss)/(24);
          //      b = -a*(-2);
          //      fELoss = fELoss+b*y+a*y*y;
          //    }else if(ch == 7){
          //      a = (frac6_2[6]*fELoss-fELoss)/(24);
          //      b = -a*(-2);
          //      fELoss = fELoss+b*y+a*y*y;
          //    }
          //  }
          }
          //delete part1;

        TLorentzVector MomOut;
        gMC->TrackMomentum(MomOut);
        fMomOut.SetXYZ(MomOut.Px(), MomOut.Py(), MomOut.Pz());
        
		timeOut  = gMC->TrackTime() * 1.0e09;
      
#ifndef EDEBUG
      if (fELoss == 0. ) return kFALSE;
#endif

    // TParticle* part    = gMC->GetStack()->GetCurrentTrack();
   // Double_t charge = part->GetPDG()->Charge() / 3. ;


	// Correction for all charge states
if (gMC->TrackCharge()!=0) { // Return the charge of the track currently transported
    Double_t BirkC1Mod = 0;
    // Apply correction for higher charge states
      if (TMath::Abs(gMC->TrackCharge())>=2)
        BirkC1Mod = BirkC1 * 7.2/12.6;
      else
        BirkC1Mod = BirkC1;

    if (gMC->TrackStep() > 0)
    {
      Double_t dedxcm = gMC -> Edep() * 1000./gMC -> TrackStep(); //[MeV/cm]
      Double_t curLightYield = gMC -> Edep() * 1000./(1. + BirkC1Mod * dedxcm + BirkC2 * dedxcm * dedxcm); //[MeV]
      curLightYield /= 1000.; //[GeV]
      lightYield += curLightYield;
	  	
	  lengthtrack = gMC-> TrackStep(); 	
     	 	  
    }

} 
		
      // Create BmnBcPoint
      fTrackID = gMC->GetStack()->GetCurrentTrackNumber();
	   		
       TParticle* part = 0;
        part = gMC->GetStack()->GetCurrentTrack();
        if (part) {
            fIsPrimary = (Int_t)part->IsPrimary();
            fCharge = (Int_t)part->GetPDG()->Charge() / 3.;
            fPdgId = (Int_t)part->GetPdgCode();
       	  	
		    }
    
		
      Double_t time    = gMC->TrackTime() * 1.0e09;
      Double_t length  = gMC->TrackLength();
     

      Int_t copyNo;
      Int_t ivol1 = gMC->CurrentVolID(copyNo);
      //      ivol1 = vol->getVolumeId();
      Int_t iCell ;
      gMC->CurrentVolOffID(1, iCell); 
 	
	
		

#ifdef EDEBUG
      if (lEDEBUGcounter<100) {
	std::cout << "EDEBUG-- BmnBC::ProcessHits: TrackID:" << fTrackID << 
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
	   AddHit(fTrackID, ivol, copyNo,  fPosIn, fPosOut,
                                fMomIn, fMomOut,
	       time, length, fELoss, fIsPrimary, fCharge, fPdgId, lightYield, timeIn, timeOut, lengthtrack);
#else

	//      AddHit(fTrackID, ivol, copyNo, iCell, TVector3(tPos.X(), tPos.Y(), tPos.Z()),
      AddHit(fTrackID, ivol, copyNo,  fPosIn, fPosOut,
                                fMomIn, fMomOut,
	       time, length, fELoss, fStation, fIsPrimary, fCharge, fPdgId, lightYield, timeIn, timeOut, lengthtrack);
#endif


      ((CbmStack*)gMC->GetStack())->AddPoint(kBC);

    }
  
    return kTRUE;
  
    //  }
#undef EDEBUG
}

// ----------------------------------------------------------------------------

// -----   Public method EndOfEvent   -----------------------------------------
void BmnBC::EndOfEvent() {
  if (fVerboseLevel)  Print();
  Reset();
}


// -----   Public method Register   -------------------------------------------
void BmnBC::Register() {
  FairRootManager::Instance()->Register("BCPoint","BC", fBCCollection, kTRUE);
}


// -----   Public method GetCollection   --------------------------------------
TClonesArray* BmnBC::GetCollection(Int_t iColl) const {
   if (iColl == 0) return fBCCollection;

  return NULL;
}


// -----   Public method Print   ----------------------------------------------
void BmnBC::Print() const {
    Int_t nHits = fBCCollection->GetEntriesFast();
    cout << "-I- BmnBC: " << nHits << " points registered in this event."
 	<< endl;

    if (fVerboseLevel>1)
      for (Int_t i=0; i<nHits; i++) (*fBCCollection)[i]->Print();
}




// -----   Public method Reset   ----------------------------------------------
void BmnBC::Reset() {
   fBCCollection->Delete();
 
  fPosIndex = 0;
}


// guarda in FairRootManager::CopyClones
// -----   Public method CopyClones   -----------------------------------------
void BmnBC::CopyClones(TClonesArray* cl1, TClonesArray* cl2, Int_t offset ) {
  Int_t nEntries = cl1->GetEntriesFast();
  //cout << "-I- BmnBD1: " << nEntries << " entries to add." << endl;
  TClonesArray& clref = *cl2;
  BmnBCPoint* oldpoint = NULL;
  for (Int_t i=0; i<nEntries; i++) {
    oldpoint = (BmnBCPoint*) cl1->At(i);
    Int_t index = oldpoint->GetTrackID() + offset;
    oldpoint->SetTrackID(index);
    new (clref[fPosIndex]) BmnBCPoint(*oldpoint);
    fPosIndex++;
  }
  cout << " -I- BmnBC: " << cl2->GetEntriesFast() << " merged entries."
       << endl;
}

//--------------------------------------------------------------------------------------------------------------------------------------
void 			BmnBC::ConstructGeometry() 
{
	TString fileName = GetGeometryFileName();
	if(fileName.EndsWith(".root")) 
	{
		//gLogger->Info(MESSAGE_ORIGIN, "Constructing BC geometry from ROOT file %s", fileName.Data());
    LOG(info) << "Constructing BC geometry from ROOT file " << fileName.Data();
		ConstructRootGeometry();
	}
	else if ( fileName.EndsWith(".geo") ) 
	{
		//gLogger->Info(MESSAGE_ORIGIN, "Constructing BC geometry from ASCII file %s", fileName.Data());
    LOG(info)  << "Constructing BC geometry from ROOT file " << fileName.Data();
		ConstructAsciiGeometry();
	}
	else LOG(info)  << "Constructing BC geometry from ROOT file " << fileName.Data();;
      
}
//--------------------------------------------------------------------------------------------------------------------------------------
void 			BmnBC::ConstructAsciiGeometry() 
{
	FairGeoLoader*    geoLoad = FairGeoLoader::Instance();
	FairGeoInterface* geoFace = geoLoad->getGeoInterface();
	BmnBCGeo*       BCGeo  = new BmnBCGeo();
	BCGeo->setGeomFile(GetGeometryFileName());
	geoFace->addGeoModule(BCGeo);

	Bool_t rc = geoFace->readSet(BCGeo);
	if (rc) BCGeo->create(geoLoad->getGeoBuilder());
	TList* volList = BCGeo->getListOfVolumes();

	// store geo parameter
	FairRun *fRun = FairRun::Instance();
	FairRuntimeDb *rtdb = FairRun::Instance()->GetRuntimeDb();
	BmnBCGeoPar* par = (BmnBCGeoPar*)(rtdb->getContainer("BmnBCGeoPar"));
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

Bool_t 			BmnBC::CheckIfSensitive(std::string name)
{
  TString tsname = name;
  if (tsname.Contains("Active")) return kTRUE;
  
return kFALSE;
}
//-------------------------------------------------------------------------

//--------------------------------------------------------------------------------------------

// -----   Private method AddHit   --------------------------------------------
BmnBCPoint* BmnBC::AddHit( Int_t trackID, Int_t detID, Int_t copyNo,
			    TVector3 posIn, TVector3 posOut,
                TVector3 momIn, TVector3 momOut, 
                            Double_t time, 
			    Double_t length, Double_t eLoss, Int_t fStat, Bool_t isPrimary, Double_t charge, Int_t pdgId, 
					Double_t lightYield, Double_t timeIn, Double_t timeOut, Double_t lengthtrack) {
  
  TClonesArray& clref = *fBCCollection;
  Int_t size = clref.GetEntriesFast();
  return new(clref[size]) BmnBCPoint(trackID, detID, copyNo, 
                                      posIn, posOut, momIn, momOut,
				      time, length, eLoss, fStation, isPrimary, charge, pdgId, lightYield, timeIn, timeOut, lengthtrack);
 }



// ----


ClassImp(BmnBC)
