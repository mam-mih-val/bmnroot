//------------------------------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------
// -----                       BmnDch2 source file                      -----
// -------------------------------------------------------------------------

#include <iostream>

#include "TClonesArray.h"
#include "TLorentzVector.h"
#include "TParticle.h"
#include "TVirtualMC.h"
#include "TGeoManager.h"
#include "TGeoMatrix.h"

//#include "FairDetectorList.h"
#include "FairGeoInterface.h"
#include "FairGeoLoader.h"
#include "FairGeoNode.h"
#include "FairGeoRootBuilder.h"
#include "BmnDch2Geo.h"
#include "FairRootManager.h"
#include "BmnDch2.h"
#include "BmnDch2Point.h"
#include "FairRuntimeDb.h"
#include "BmnDch2GeoPar.h"
#include "TObjArray.h"
#include "FairRun.h"
#include "FairVolume.h"
#include "TMath.h"
#include "CbmStack.h"

#include "TParticlePDG.h"

class FairVolume;

//------------------------------------------------------------------------------------------------------------------------
BmnDch2::BmnDch2() 
 : FairDetector("DCH2", kTRUE)
{
	fPointCollection = new TClonesArray("BmnDch2Point");
	fPosIndex = 0;
	fVerboseLevel = 1;
	ResetParameters();
}
//------------------------------------------------------------------------------------------------------------------------
BmnDch2::BmnDch2(const char* name, Bool_t active)
 : FairDetector(name, active)
{  
	fPointCollection = new TClonesArray("BmnDch2Point");
	fPosIndex = 0;
	fVerboseLevel = 1;
	ResetParameters();
}
//------------------------------------------------------------------------------------------------------------------------
BmnDch2::~BmnDch2() 
{
	if(fPointCollection){fPointCollection->Delete(); delete fPointCollection; }
}
//------------------------------------------------------------------------------------------------------------------------
int BmnDch2::DistAndPoints(TVector3 p3, TVector3 p4, TVector3& pa, TVector3& pb) {                                         
    pa=(p3+p4)*0.5;
    pb=pa;
    
    //pa=p3; //del
    //pb=pa; //del
    return 0;
} 
//------------------------------------------------------------------------------------------------------------------------
TVector3 BmnDch2::GlobalToLocal(TVector3& global) {
    Double_t globPos[3];
    Double_t localPos[3];
    global.GetXYZ(globPos);
    gMC->Gmtod(globPos, localPos, 1); 
    return TVector3(localPos);
}
//------------------------------------------------------------------------------------------------------------------------
TVector3 BmnDch2::LocalToGlobal(TVector3& local) {
    Double_t globPos[3];
    Double_t localPos[3];
    local.GetXYZ(localPos);
    gMC->Gdtom(localPos, globPos, 1);  
    return TVector3(globPos);
}
//----------------------------------------------------------------------------------------------------------------------
Bool_t  BmnDch2::ProcessHits(FairVolume* vol)
{
// Set parameters at entrance of volume. Reset ELoss.
  if (gMC->IsTrackEntering()) {

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
    gMC->TrackMomentum(fMom);
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

  // Create BmnDch2Point at EXIT of active volume;
  if ((gMC->IsTrackExiting() || gMC->IsTrackStop() || gMC->IsTrackDisappeared()) && fELoss > 0) {
    TLorentzVector PosOut;
    gMC->TrackPosition(PosOut);
    fPosOut.SetXYZ(PosOut.X(), PosOut.Y(), PosOut.Z());

    // Line defined in local coordinates
    TVector3 p1(0, 0, 0); // 10 - arbitrary number...
    TVector3 p2(10,0, 0);

    // Conversion to global coordinates
    p1 = LocalToGlobal(p1);
    p2 = LocalToGlobal(p2);
    Double_t phi = TMath::ATan2 (p2.Y()-p1.Y(),p2.X()-p1.X());

    // "will-be-filled-out-soon" Points of closest approach
    TVector3 trackPosition(0,0,0); // trackPosition => point on track, fPos => point on straw

    // calculate points of closest approach between track and straw
    //int result =
    DistAndPoints(fPosIn, fPosOut, fPos, trackPosition);

    BmnDch2Point *p = AddHit(fTrackID, fVolumeID, fPos, fPos.Perp(),
                TVector3(fMom.Px(), fMom.Py(), fMom.Pz()),
                fTime, (fLength+gMC->TrackLength())/2, fELoss,
                fIsPrimary, fCharge, fPdgId, trackPosition);
    p->SetPhi(phi); //AZ

    ((CbmStack*)gMC->GetStack())->AddPoint(kDCH2);
  }

  return kTRUE;
}
//------------------------------------------------------------------------------------------------------------------------
void BmnDch2::EndOfEvent() 
{
	if(fVerboseLevel) Print();
  	fPointCollection->Clear();
  	fPosIndex = 0;
}
//------------------------------------------------------------------------------------------------------------------------
void BmnDch2::Register(){ FairRootManager::Instance()->Register("DCH2Point", "DCH2", fPointCollection, kTRUE); }
//------------------------------------------------------------------------------------------------------------------------
TClonesArray* BmnDch2::GetCollection(Int_t iColl) const 
{
	if(iColl == 0) 	return fPointCollection;
	
return NULL;
}
//------------------------------------------------------------------------------------------------------------------------
void BmnDch2::Print() const 
{
	Int_t nHits = fPointCollection->GetEntriesFast();
	cout << "-I- BmnDch2: " << nHits << " points registered in this event." << endl;
	
	if(fVerboseLevel > 1)
    		for(Int_t i=0; i<nHits; i++) (*fPointCollection)[i]->Print();
}
//------------------------------------------------------------------------------------------------------------------------
void BmnDch2::Reset(){ fPointCollection->Clear(); ResetParameters(); }
//------------------------------------------------------------------------------------------------------------------------
void BmnDch2::CopyClones(TClonesArray* cl1, TClonesArray* cl2, Int_t offset)
{
	Int_t nEntries = cl1->GetEntriesFast();
	cout << "-I- BmnDch2: " << nEntries << " entries to add." << endl;
	TClonesArray& clref = *cl2;
	BmnDch2Point* oldpoint = NULL;
	
	for(Int_t i=0; i<nEntries; i++) 
	{
		oldpoint = (BmnDch2Point*) cl1->At(i);
		Int_t index = oldpoint->GetTrackID() + offset;
		oldpoint->SetTrackID(index);
		new (clref[fPosIndex]) BmnDch2Point(*oldpoint);
		fPosIndex++;
	}
	
	cout << "-I- BmnDch2: " << cl2->GetEntriesFast() << " merged entries."  << endl;
}
//------------------------------------------------------------------------------------------------------------------------
void BmnDch2::ConstructGeometry() 
{
  TString fileName = GetGeometryFileName();
  
  if ( fileName.EndsWith(".root") ) {
    gLogger->Info(MESSAGE_ORIGIN,
                "Constructing DCH2 geometry from ROOT file %s", 
                fileName.Data());
    ConstructRootGeometry();
  }
  else if ( fileName.EndsWith(".geo") ) {
    gLogger->Info(MESSAGE_ORIGIN,
		  "Constructing DCH2 geometry from ASCII file %s", 
		  fileName.Data());
    ConstructAsciiGeometry();
  }
  else
    gLogger->Fatal(MESSAGE_ORIGIN,
		   "Geometry format of DCH2 file %S not supported.", 
		   fileName.Data());
}

// -----   ConstructAsciiGeometry   -------------------------------------------
void BmnDch2::ConstructAsciiGeometry() {
  
  FairGeoLoader*    geoLoad = FairGeoLoader::Instance();
  FairGeoInterface* geoFace = geoLoad->getGeoInterface();
  BmnDch2Geo*       DCH2Geo  = new BmnDch2Geo();
  DCH2Geo->setGeomFile(GetGeometryFileName());
  geoFace->addGeoModule(DCH2Geo);

  Bool_t rc = geoFace->readSet(DCH2Geo);
  if (rc) DCH2Geo->create(geoLoad->getGeoBuilder());
  TList* volList = DCH2Geo->getListOfVolumes();
  // store geo parameter
  FairRun *fRun = FairRun::Instance();
  FairRuntimeDb *rtdb= FairRun::Instance()->GetRuntimeDb();
  BmnDch2GeoPar* par=(BmnDch2GeoPar*)(rtdb->getContainer("BmnDch2GeoPar"));
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
  ProcessNodes( volList );

}
// ----------------------------------------------------------------------------

//Check if Sensitive-----------------------------------------------------------
Bool_t BmnDch2::CheckIfSensitive(std::string name)
{
  TString tsname = name;
  if (tsname.Contains("Active")) {
    return kTRUE;
  }
  return kFALSE;
    
    
//  if(0 == TString(name).CompareTo("DCH2DetV")) {
//    return kTRUE;
//  }
//  return kFALSE;

}
//---------------------------------------------------------  

//------------------------------------------------------------------------------------------------------------------------
BmnDch2Point* BmnDch2::AddHit(Int_t trackID, Int_t detID, TVector3 pos, Double_t radius,
			    TVector3 mom, Double_t time, Double_t length, 
			    Double_t eLoss, Int_t isPrimary, Double_t charge, Int_t pdgId, TVector3 trackPos) 
{
	TClonesArray& clref = *fPointCollection;
	Int_t size = clref.GetEntriesFast();
    //std::cout << "ELoss: " << eLoss << "\n";	
    return new(clref[size]) BmnDch2Point(trackID, detID, pos, radius, mom, time, length, eLoss, isPrimary, charge, pdgId, trackPos);
}
//------------------------------------------------------------------------------------------------------------------------

ClassImp(BmnDch2)
