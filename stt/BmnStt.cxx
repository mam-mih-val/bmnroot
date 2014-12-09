//------------------------------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------
// -----                       BmnStt source file                      -----
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
//#include "FairStack.h"
//#include "CbmStack.h"
#include "/home/fedorisin/trunk/bmnroot/bmndata/CbmStack.h"
#include "BmnSttGeo.h"
#include "FairRootManager.h"
#include "BmnStt.h"
#include "BmnSttPoint.h"
#include "FairRuntimeDb.h"
#include "BmnSttGeoPar.h"
#include "TObjArray.h"
#include "FairRun.h"
#include "FairVolume.h"
#include "TMath.h"

#include "TParticlePDG.h"

class FairVolume;

//------------------------------------------------------------------------------------------------------------------------
BmnStt::BmnStt() 
 : FairDetector("STT", kTRUE)
{
	fSttCollection = new TClonesArray("BmnSttPoint");
	fPosIndex = 0;
	fVerboseLevel = 1;
}
//------------------------------------------------------------------------------------------------------------------------
BmnStt::BmnStt(const char* name, Bool_t active)
 : FairDetector(name, active)
{  
	fSttCollection = new TClonesArray("BmnSttPoint");
	fPosIndex = 0;
	fVerboseLevel = 1;
}
//------------------------------------------------------------------------------------------------------------------------
BmnStt::~BmnStt() 
{
	if(fSttCollection){ fSttCollection->Delete(); delete fSttCollection; }
}
//------------------------------------------------------------------------------------------------------------------------
int BmnStt::DistAndPoints(TVector3 p1, TVector3 p2, TVector3 p3, TVector3 p4, TVector3& pa, TVector3& pb) {                                         
    TVector3 A = p2 - p1;
    TVector3 B = p4 - p3; 
    TVector3 C = p1 - p3;  

    if (p3 == p4) {
      pb = p4;
      TVector3 unit = A.Unit();
      Double_t dist = unit.Dot(-C);
      pa = p1 + dist * unit;
      return 0;
    }

    Double_t numer = C.Dot(B) * B.Dot(A) - C.Dot(A) * B.Dot(B);  
    Double_t denom = A.Dot(A) * B.Dot(B) - B.Dot(A) * B.Dot(A);  
    if (denom == 0) {
      // parallel lines   
      pa.SetXYZ(-10000,-10000,-10000);  
      pb.SetXYZ(-10000,-10000,-10000);        
      return 1;  
    }
    Double_t ma = numer / denom;   
    Double_t mb = ( C.Dot(B) + B.Dot(A)*ma ) / B.Dot(B);  
    pa = p1 + A*ma;   
    pb = p3 + B*mb; 
    return 0;
} 
//------------------------------------------------------------------------------------------------------------------------
TVector3 BmnStt::GlobalToLocal(TVector3& global) {
    Double_t globPos[3];
    Double_t localPos[3];
    global.GetXYZ(globPos);
    gMC->Gmtod(globPos, localPos, 1); 
    return TVector3(localPos);
}
//------------------------------------------------------------------------------------------------------------------------
TVector3 BmnStt::LocalToGlobal(TVector3& local) {
    Double_t globPos[3];
    Double_t localPos[3];
    local.GetXYZ(localPos);
    gMC->Gdtom(localPos, globPos, 1);  
    return TVector3(globPos);
}
//----------------------------------------------------------------------------------------------------------------------
Bool_t  BmnStt::ProcessHits(FairVolume* vol)
{
  Int_t gap, cell, module, region;
  TString Volname;

  // Set parameters at entrance of volume. Reset ELoss. 
  if (gMC->IsTrackEntering()) {
    gMC->CurrentVolOffID(2, module);
    if (gMC->GetStack()->GetCurrentTrackNumber() != fTrackID || 
	gMC->CurrentVolID(gap)+1000*module != fVolumeID) {
      ResetParameters();
      fELoss  = 0.;
      fTime   = gMC->TrackTime() * 1.0e09;
      fLength = gMC->TrackLength();
      gMC->TrackPosition(fPos);
      gMC->TrackMomentum(fMom);
      TParticle* part = 0;
      part = gMC->GetStack()->GetCurrentTrack();
      if (part) {
	fIsPrimary = (Int_t)part->IsPrimary();
	fCharge = (Int_t)part->GetPDG()->Charge();
	fPdgId = (Int_t)part->GetPdgCode();
      }
      fVolumeID = gMC->CurrentVolID(gap) + 1000 * module;
      fTrackID = gMC->GetStack()->GetCurrentTrackNumber();
    } 
  }

  // Sum energy loss for all steps in the active volume
  fELoss += gMC->Edep();

  // Create BmnSttPoint at EXIT of active volume; 
  //if (gMC->IsTrackExiting() && fELoss > 0) {
  if ( (gMC->IsTrackExiting() || gMC->IsTrackStop() || gMC->IsTrackDisappeared()) && fELoss > 0. && gMC->TrackCharge() != 0) {
    //cout<<"eLoss = "<<fELoss<<endl;
    fTrackID = gMC->GetStack()->GetCurrentTrackNumber();
    //   Volname = vol->getName();
    Volname = vol->getRealName();  // EL
    //cout << Volname << endl;
    region = Volname[5] - '0';   //?????????????????????????
    gMC->CurrentVolID(gap);
    gMC->CurrentVolOffID(1, cell);
    gMC->CurrentVolOffID(2, module);
    
    Double_t phi = fPos.Phi();  
    BmnSttPoint *p = 
      AddHit(fTrackID, region, TVector3(fPos.X(),  fPos.Y(),  fPos.Z()), fPos.Perp(), TVector3(fMom.Px(), fMom.Py(), fMom.Pz()), 
	     fTime, fLength, fELoss, fIsPrimary, fCharge, fPdgId);
    p->SetPhi(phi); 
    
//    ((FairStack*)gMC->GetStack())->AddPoint(kECT);
    ((CbmStack*)gMC->GetStack())->AddPoint(kSTT);
    //ResetParameters();
  }

  return kTRUE;
}
//------------------------------------------------------------------------------------------------------------------------
void BmnStt::EndOfEvent() 
{
	if(fVerboseLevel) Print();
  	fSttCollection->Delete();
  	fPosIndex = 0;
}
//------------------------------------------------------------------------------------------------------------------------
void BmnStt::Register(){ FairRootManager::Instance()->Register("STRAWPoint", "Stt", fSttCollection, kTRUE); }
//------------------------------------------------------------------------------------------------------------------------
TClonesArray* BmnStt::GetCollection(Int_t iColl) const 
{
	if(iColl == 0) 	return fSttCollection;
	
return NULL;
}
//------------------------------------------------------------------------------------------------------------------------
void BmnStt::Print() const 
{
	Int_t nHits = fSttCollection->GetEntriesFast();
	cout << "-I- BmnStt: " << nHits << " points registered in this event." << endl;
	
	if(fVerboseLevel > 1)
    		for(Int_t i=0; i<nHits; i++) (*fSttCollection)[i]->Print();
}
//------------------------------------------------------------------------------------------------------------------------
void BmnStt::Reset(){ fSttCollection->Delete(); ResetParameters(); }
//------------------------------------------------------------------------------------------------------------------------
void BmnStt::CopyClones(TClonesArray* cl1, TClonesArray* cl2, Int_t offset)
{
	Int_t nEntries = cl1->GetEntriesFast();
	cout << "-I- BmnStt: " << nEntries << " entries to add." << endl;
	TClonesArray& clref = *cl2;
	BmnSttPoint* oldpoint = NULL;
	
	for(Int_t i=0; i<nEntries; i++) 
	{
		oldpoint = (BmnSttPoint*) cl1->At(i);
		Int_t index = oldpoint->GetTrackID() + offset;
		oldpoint->SetTrackID(index);
		new (clref[fPosIndex]) BmnSttPoint(*oldpoint);
		fPosIndex++;
	}
	
	cout << "-I- BmnStt: " << cl2->GetEntriesFast() << " merged entries."  << endl;
}
//------------------------------------------------------------------------------------------------------------------------
void BmnStt::ConstructGeometry() 
{
  
	Int_t count=0;
	Int_t count_tot=0;

        FairGeoLoader*    geoLoad = FairGeoLoader::Instance();
        FairGeoInterface* geoFace = geoLoad->getGeoInterface();
	BmnSttGeo* sttGeo  = new BmnSttGeo();
	sttGeo->setGeomFile(GetGeometryFileName());
	geoFace->addGeoModule(sttGeo);

	Bool_t rc = geoFace->readSet(sttGeo);
	if(rc) sttGeo->create(geoLoad->getGeoBuilder());
	TList* volList = sttGeo->getListOfVolumes();

	// store geo parameter
        FairRun *fRun = FairRun::Instance();
        FairRuntimeDb *rtdb = FairRun::Instance()->GetRuntimeDb();
	BmnSttGeoPar* par =(BmnSttGeoPar*)(rtdb->getContainer("BmnSttGeoPar"));
	TObjArray *fSensNodes = par->GetGeoSensitiveNodes();
	TObjArray *fPassNodes = par->GetGeoPassiveNodes();

        FairGeoNode *node   = NULL;
        FairGeoVolume *aVol = NULL;
	TListIter iter(volList);
	
        while((node = (FairGeoNode*)iter.Next()))
	{
                aVol = dynamic_cast<FairGeoVolume*> (node);
		if(node->isSensitive()){ 	fSensNodes->AddLast(aVol); count++; }
		else           		 	fPassNodes->AddLast(aVol);      
       		count_tot++;
  	}
	
	par->setChanged();
	par->setInputVersion(fRun->GetRunId(), 1);  
	ProcessNodes(volList);
}
//------------------------------------------------------------------------------------------------------------------------
BmnSttPoint* BmnStt::AddHit(Int_t trackID, Int_t detID, TVector3 pos, Double_t radius,
			    TVector3 mom, Double_t time, Double_t length, 
			    Double_t eLoss, Int_t isPrimary, Double_t charge, Int_t pdgId) 
{
	TClonesArray& clref = *fSttCollection;
	Int_t size = clref.GetEntriesFast();
    //std::cout << "ELoss: " << eLoss << "\n";	
    return new(clref[size]) BmnSttPoint(trackID, detID, pos, radius, mom, time, length, eLoss, isPrimary, charge, pdgId);
}
//------------------------------------------------------------------------------------------------------------------------

ClassImp(BmnStt)
