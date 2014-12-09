//--------------------------------------------------------------------------------------------------------------------------------------
#include <iostream>
#include <assert.h>

#include<TClonesArray.h>
#include<TGeoMCGeometry.h>
#include<TGeoManager.h>
#include<TLorentzVector.h>
#include<TParticle.h>
#include<TVirtualMC.h>
#include<TGeoArb8.h>
#include<TObjArray.h>
#include<TParticlePDG.h>

#include "FairGeoInterface.h"
#include "FairGeoLoader.h"
#include "FairGeoNode.h"
#include "FairGeoRootBuilder.h"
#include "FairRootManager.h"
#include "FairVolume.h"
#include "FairRun.h"
#include "FairLogger.h"
// add on for debug
//#include "FairGeoG3Builder.h"
#include "FairRuntimeDb.h"

#include "CbmStack.h"

#include "BmnTOF1Geo.h"
#include "BmnTOF1.h"
#include "BmnTOF1Point.h"

ClassImp(BmnTOF1)
//--------------------------------------------------------------------------------------------------------------------------------------
BmnTOF1::BmnTOF1() 
{
	fTofCollection = new TClonesArray("BmnTOF1Point");
	volDetector = 0;
	fPosIndex   = 0; 
	fEventID=-1; 
	fVerboseLevel = 1;
}
//--------------------------------------------------------------------------------------------------------------------------------------
BmnTOF1::BmnTOF1(const char* name, Bool_t active)
  : FairDetector(name, active) 
{
	fTofCollection = new TClonesArray("BmnTOF1Point");
	fPosIndex   = 0;
	volDetector = 0;
	fEventID=-1;
	fVerboseLevel = 1;
}
//--------------------------------------------------------------------------------------------------------------------------------------
BmnTOF1::~BmnTOF1() 
{
	if(fTofCollection) 
	{
		fTofCollection->Delete();
		delete fTofCollection;
	}  
}
//--------------------------------------------------------------------------------------------------------------------------------------
void 			BmnTOF1::Initialize() 
{
	FairDetector::Initialize();
	FairRun* sim = FairRun::Instance();
	FairRuntimeDb* rtdb=sim->GetRuntimeDb();
}
//--------------------------------------------------------------------------------------------------------------------------------------
void 			BmnTOF1::BeginEvent()
{
}
//--------------------------------------------------------------------------------------------------------------------------------------
Bool_t BmnTOF1::ProcessHits(FairVolume* vol) 
{
	static const char typeB='B'; 
	static TString name;

	Int_t  module, pad; 

	// Set parameters at entrance of volume. Reset ELoss.
	if(gMC->IsTrackEntering()) 
	{
		fELoss  = 0.;
		fTime   = gMC->TrackTime() * 1.0e09;
		fLength = gMC->TrackLength();
		gMC->TrackPosition(fPos);
		gMC->TrackMomentum(fMom);		
	}

	// Sum energy loss for all steps in the active volume
	fELoss += gMC->Edep();
	
	// Create TofPoint at exit of active volume
	if(fELoss > 0 &&  (gMC->IsTrackExiting() || gMC->IsTrackStop() || gMC->IsTrackDisappeared()) ) 
	{	
		fTrackID = gMC->GetStack()->GetCurrentTrackNumber();
		
		gMC->CurrentVolOffID(1, pad);
		gMC->CurrentVolOffID(2, module);

		name = vol->GetName();
		if(typeB == name[5]) module += 100;

///cout<<"\n pad="<<pad<<" module="<<module<<" "<<vol->getRealName()<<"   "<<vol->GetName();

		fVolumeID = BmnTOF1Point::GetVolumeUID(module, pad);

assert(BmnTOF1Point::GetPad(fVolumeID) == pad);
assert(BmnTOF1Point::GetModule(fVolumeID) == module);

		AddHit(fTrackID, fVolumeID, TVector3(fPos.X(),  fPos.Y(),  fPos.Z()), TVector3(fMom.Px(), fMom.Py(), fMom.Pz()), fTime, fLength, fELoss);
assert(fTrackID>=0);
		((CbmStack*)gMC->GetStack())->AddPoint(kTOF1);

    		ResetParameters();
  	}

return kTRUE;

/*
  // if (TMath::Abs(gMC->TrackCharge()) <= 0) return kFALSE;

  Int_t      ivol    = vol->getMCid();
  TLorentzVector tPos1, tMom1;

  //#define EDEBUG
#ifdef EDEBUG
  static Int_t lEDEBUGcounter=0;
  if (lEDEBUGcounter<1)
    std::cout << "EDEBUG-- BmnTOF1::ProcessHits: entered" << gMC->CurrentVolPath() << endl;
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

      // Create BmnTOF1Point
      fTrackID = gMC->GetStack()->GetCurrentTrackNumber();
      Double_t time    = gMC->TrackTime() * 1.0e09;
      Double_t length  = gMC->TrackLength();
      TLorentzVector tPos, tMom;
      gMC->TrackPosition(tPos);
      gMC->TrackMomentum(tMom);

      Int_t copyNo;
      Int_t ivol1 = gMC->CurrentVolID(copyNo);
      //      ivol1 = vol->getVolumeId();
      Int_t iCell ;
      gMC->CurrentVolOffID(1, iCell); 
 	

#ifdef EDEBUG
      static Bool_t already=0;
      if (lEDEBUGcounter<100) {
	std::cout << "EDEBUG-- BmnTOF1::ProcessHits: TrackID:" << fTrackID << 
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
      if ((iCell==2)&&(lEDEBUGcounter>=100)&&(!already)) {
	already=1;
	lEDEBUGcounter=0;
      }
//       if ((part->GetPdgCode())==321) {
// 	std::cout << "EDEBUG-- BmnTOF1::ProcessHits(..)  K+:  " << fTrackID << "   " << (  gMC->IsTrackExiting()) << "  " <<
// 	  (gMC->IsTrackStop()) << "  " << (gMC->IsTrackDisappeared()) << "   " << fELoss << "  " << time << std::endl;
//       }
//#endif

//       if(copyNo==1)
// 	AddHit(fTrackID, ivol, copyNo, iCell, TVector3(tPos1.X(), tPos1.Y(), tPos1.Z()),
// 	       TVector3(tMom1.Px(), tMom1.Py(), tMom1.Pz()),
// 	       time, length, fELoss);
//       else 

	AddHit(fTrackID, ivol, copyNo, iCell, TVector3(tPos.X(), tPos.Y(), tPos.Z()),
	       TVector3(tMom.Px(), tMom.Py(), tMom.Pz()),
	       time, length, fELoss);
#else

      AddHit(fTrackID, ivol, copyNo, iCell, TVector3(tPos.X(), tPos.Y(), tPos.Z()),
	     TVector3(tMom.Px(), tMom.Py(), tMom.Pz()),
	     time, length, fELoss);
#endif

      Int_t points = gMC->GetStack()->GetCurrentTrack()->GetMother(1);
//       Int_t nTOF1Points = (points & (1<<30)) >> 30;
//       nTOF1Points ++;
//       if (nTOF1Points > 1) nTOF1Points = 1;
//      points = ( points & ( ~ (1<<30) ) ) | (nTOF1Points << 30);
      points = ( points & ( ~ (1<<30) ) ) | (1 << 30);
      gMC->GetStack()->GetCurrentTrack()->SetMother(1,points);

      ((CbmStack*)gMC->GetStack())->AddPoint(kTOF1);

    }
   
//     Int_t copyNo;  
//     gMC->CurrentVolID(copyNo);
//     TString nam = gMC->GetMC()->GetName();
    //    cout<<"name "<<gMC->GetMC()->GetName()<<endl;
    //    ResetParameters();
  */
    return kTRUE;
  
    //  }
#undef EDEBUG
}
//--------------------------------------------------------------------------------------------------------------------------------------
void 			BmnTOF1::EndOfEvent() 
{
	if(fVerboseLevel)  Print();
	
	Reset();
}
//--------------------------------------------------------------------------------------------------------------------------------------
void 			BmnTOF1::Register() 
{
	FairRootManager::Instance()->Register("TOF1Point","TOF1", fTofCollection, kTRUE);
}
//--------------------------------------------------------------------------------------------------------------------------------------
TClonesArray* 		BmnTOF1::GetCollection(Int_t iColl) const 
{
	if(iColl == 0) return fTofCollection;

return NULL;
}
//--------------------------------------------------------------------------------------------------------------------------------------
void 			BmnTOF1::Print() const 
{
	Int_t nHits = fTofCollection->GetEntriesFast();
	cout<<"-I- BmnTOF1: "<<nHits<<" points registered in this event.\n";

	if(fVerboseLevel>1)
		for(Int_t i=0; i<nHits; i++) (*fTofCollection)[i]->Print();
}
//--------------------------------------------------------------------------------------------------------------------------------------
void 			BmnTOF1::Reset() 
{
	fTofCollection->Delete();
 
	fPosIndex = 0;
}
//--------------------------------------------------------------------------------------------------------------------------------------
// guarda in FairRootManager::CopyClones
void 			BmnTOF1::CopyClones(TClonesArray* cl1, TClonesArray* cl2, Int_t offset ) 
{
assert(0);
	Int_t nEntries = cl1->GetEntriesFast();
	//cout << "-I- BmnTOF1: " << nEntries << " entries to add." << endl;
	TClonesArray& clref = *cl2;
	BmnTOF1Point* oldpoint = NULL;

	for(Int_t i=0; i<nEntries; i++) 
	{
		oldpoint = (BmnTOF1Point*) cl1->At(i);
		Int_t index = oldpoint->GetTrackID() + offset;
		oldpoint->SetTrackID(index);
		new (clref[fPosIndex]) BmnTOF1Point(*oldpoint);
		fPosIndex++;
	}

	cout<<" -I- BmnTOF1: "<<cl2->GetEntriesFast()<<" merged entries."<< endl;
}
//--------------------------------------------------------------------------------------------------------------------------------------
void 			BmnTOF1::ConstructGeometry() 
{
	TString fileName = GetGeometryFileName();
	if(fileName.EndsWith(".root")) 
	{
		gLogger->Info(MESSAGE_ORIGIN, "Constructing TOF1 geometry from ROOT file %s", fileName.Data());
		ConstructRootGeometry();
	}
	else if ( fileName.EndsWith(".geo") ) 
	{
		gLogger->Info(MESSAGE_ORIGIN, "Constructing TOF1 geometry from ASCII file %s", fileName.Data());
		ConstructAsciiGeometry();
	}
	else	gLogger->Fatal(MESSAGE_ORIGIN, "Geometry format of TOF1 file %S not supported.", fileName.Data());    
}
//--------------------------------------------------------------------------------------------------------------------------------------
void 			BmnTOF1::ConstructAsciiGeometry() 
{
	FairGeoLoader*    geoLoad = FairGeoLoader::Instance();
	FairGeoInterface* geoFace = geoLoad->getGeoInterface();
	BmnTOF1Geo*       TOF1Geo  = new BmnTOF1Geo();
	TOF1Geo->setGeomFile(GetGeometryFileName());
	geoFace->addGeoModule(TOF1Geo);

	Bool_t rc = geoFace->readSet(TOF1Geo);
	if (rc) TOF1Geo->create(geoLoad->getGeoBuilder());
	TList* volList = TOF1Geo->getListOfVolumes();

	// store geo parameter
	FairRun *fRun = FairRun::Instance();
	FairRuntimeDb *rtdb = FairRun::Instance()->GetRuntimeDb();
	BmnTOF1GeoPar* par = (BmnTOF1GeoPar*)(rtdb->getContainer("BmnTOF1GeoPar"));
	TObjArray *fSensNodes = par->GetGeoSensitiveNodes();
	TObjArray *fPassNodes = par->GetGeoPassiveNodes();

	TListIter iter(volList);
	FairGeoNode* node   = NULL;
	FairGeoVolume *aVol=NULL;

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
Bool_t 			BmnTOF1::CheckIfSensitive(std::string name)
{
  TString tsname = name;
  if (tsname.Contains("Active")) {
    return kTRUE;
  }
  return kFALSE;
    
//  if(0 == TString(name).CompareTo("TOF1DetV")) {
//    return kTRUE;
//  }
//  return kFALSE;
}
//--------------------------------------------------------------------------------------------------------------------------------------
BmnTOF1Point* 		BmnTOF1::AddHit(Int_t trackID, Int_t detID, TVector3 pos, TVector3 mom, Double_t time, Double_t length, Double_t eLoss) 
{
//cout<<"\n  ------>>> "<<fTofCollection->GetEntriesFast()<<"    trackID="<<trackID;

return new( (*fTofCollection)[fTofCollection->GetEntriesFast()] ) BmnTOF1Point(trackID, detID, pos, mom, time, length, eLoss);

//BmnTOF1Point *ptr = new( (*fTofCollection)[fTofCollection->GetEntriesFast()] ) BmnTOF1Point(trackID, detID, pos, mom, time, length, eLoss);
//ptr->Print(0);
//return ptr;

}
//--------------------------------------------------------------------------------------------------------------------------------------

