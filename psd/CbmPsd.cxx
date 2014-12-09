// -------------------------------------------------------------------------
// -----                       CbmPsd source file                     -----
// -----                  Created 27/07/04  by V. Friese               -----
// -------------------------------------------------------------------------

#include "CbmPsd.h"

#include "CbmGeoPsd.h"
#include "CbmGeoPsdPar.h"
#include "CbmPsdPoint.h"
#include "CbmDetectorList.h"
#include "CbmStack.h"

#include "FairGeoInterface.h"
#include "FairGeoLoader.h"
#include "FairGeoNode.h"
#include "FairRootManager.h"
#include "FairRun.h"
#include "FairRuntimeDb.h"
#include "FairVolume.h"

#include "TClonesArray.h"
#include "TParticle.h"
#include "TVirtualMC.h"

#include <iostream>

using std::cout;
using std::endl;

// -----   Default constructor   -------------------------------------------
CbmPsd::CbmPsd() : FairDetector("PSD", kTRUE, kZDC) {
  fPsdCollection = new TClonesArray("CbmPsdPoint");
  fPosIndex = 0;
  fDebug    = "";
}
// -------------------------------------------------------------------------



// -----   Standard constructor   ------------------------------------------
CbmPsd::CbmPsd(const char* name, Bool_t active)
  : FairDetector(name, active, kZDC) {
  fPsdCollection = new TClonesArray("CbmPsdPoint");
  fPosIndex = 0;
  fDebug    = "";
}

// -------------------------------------------------------------------------



// -----   Destructor   ----------------------------------------------------
CbmPsd::~CbmPsd() {
 if (fPsdCollection) {
   fPsdCollection->Delete(); 
   delete fPsdCollection;
 }
}
// -------------------------------------------------------------------------



// -----   Public method ProcessHits  --------------------------------------
Bool_t  CbmPsd::ProcessHits(FairVolume* vol)
{
  // Stores all particles at entrance to active detector. 

  // Set parameters at entrance of volume. ELoss is set to the
  // kinetic energy, mimicking complete stopping in the calorimeter.
/*  if ( gMC->IsTrackEntering() ) {

    gMC->TrackPosition(fPos);
    gMC->TrackMomentum(fMom);
    fTrackID  = gMC->GetStack()->GetCurrentTrackNumber();
    fVolumeID = vol->getMCid();
    fTime     = gMC->TrackTime() * 1.0e09;   // conversion to ns
    fLength   = gMC->TrackLength();
    Double_t mass = gMC->TrackMass(); 

    // Calculate kinetic energy
    Double_t ekin = TMath::Sqrt( fMom.Px()*fMom.Px() +
				 fMom.Py()*fMom.Py() +
				 fMom.Pz()*fMom.Pz() +
				 mass * mass ) - mass;
    fELoss = ekin;

    // Create CbmPsdPoint
    AddHit(fTrackID, fVolumeID, TVector3(fPos.X(),  fPos.Y(),  fPos.Z()),
	   TVector3(fMom.Px(), fMom.Py(), fMom.Pz()), fTime, fLength, 
	   fELoss);
 
    ((CbmStack*)gMC->GetStack())->AddPoint(kPSD, fTrackID);
  /!!!
  // Increment number of ecal points for TParticle
  Int_t points = gMC->GetStack()->GetCurrentTrack()->GetMother(1);
  Int_t nPsdPoints = (points & (15<<16)) >> 16;
  nPsdPoints ++;
  if (nPsdPoints > 15) nPsdPoints = 15;
  points = ( points & ( ~ (15<<16) ) ) | (nPsdPoints << 16);
  gMC->GetStack()->GetCurrentTrack()->SetMother(1,points);
  *!!!
    ResetParameters();
    
  }
*/
  return kTRUE;
}
// -------------------------------------------------------------------------



// -----   Public method EndOfEvent   --------------------------------------
void CbmPsd::EndOfEvent() {
//  cout << " No of PSD points (size)  " << (*fPsdCollection).GetEntries() << endl;
//  cout << " No of PSD points (index) " << fPosIndex << endl;
  if (fVerboseLevel) Print();
  fPsdCollection->Clear();
  fPosIndex = 0;
}
// -------------------------------------------------------------------------



// -----   Public method Register   ----------------------------------------
void CbmPsd::Register() {
  FairRootManager::Instance()->Register("PsdPoint", "Psd", 
  				       fPsdCollection, kTRUE);
}
// -------------------------------------------------------------------------



// -----   Public method GetCollection   -----------------------------------
TClonesArray* CbmPsd::GetCollection(Int_t iColl) const {
  if (iColl == 0) return fPsdCollection;
  else return NULL;
}
// -------------------------------------------------------------------------



// -----   Public method Print   -------------------------------------------
void CbmPsd::Print() const {
  Int_t nHits = fPsdCollection->GetEntriesFast();
  cout << "-I- CbmPsd: " << nHits << " points registered in this event." 
       << endl;
  if (fVerboseLevel>1)
    for (Int_t i=0; i<nHits; i++) (*fPsdCollection)[i]->Print();
}
// -------------------------------------------------------------------------



// -----   Public method Reset   -------------------------------------------
void CbmPsd::Reset() {
  fPsdCollection->Clear();
  ResetParameters();
}
// -------------------------------------------------------------------------



// -----   Public method CopyClones   --------------------------------------
void CbmPsd::CopyClones(TClonesArray* cl1, TClonesArray* cl2, Int_t offset){
  Int_t nEntries = cl1->GetEntriesFast();
  cout << "-I- CbmPsd: " << nEntries << " entries to add." << endl;
  TClonesArray& clref = *cl2;
  CbmPsdPoint* oldpoint = NULL;
   for (Int_t i=0; i<nEntries; i++) {
   oldpoint = (CbmPsdPoint*) cl1->At(i);
    Int_t index = oldpoint->GetTrackID() + offset;
    oldpoint->SetTrackID(index);
    new (clref[fPosIndex]) CbmPsdPoint(*oldpoint);
    fPosIndex++;
  }
  cout << "-I- CbmPsd: " << cl2->GetEntriesFast() << " merged entries." 
       << endl;
}
// -------------------------------------------------------------------------



// -----   Public method ConstructGeometry   -------------------------------
void CbmPsd::ConstructGeometry() {
/*
  FairGeoLoader*    geoLoad = FairGeoLoader::Instance();
  FairGeoInterface* geoFace = geoLoad->getGeoInterface();
  CbmGeoPsd*       trdGeo  = new CbmGeoPsd();
  trdGeo->setGeomFile(GetGeometryFileName());
  geoFace->addGeoModule(trdGeo);

  Bool_t rc = geoFace->readSet(trdGeo);
  if (rc) trdGeo->create(geoLoad->getGeoBuilder());
  TList* volList = trdGeo->getListOfVolumes();
 // store TRD geo parameter
  FairRun *fRun = FairRun::Instance();
  FairRuntimeDb *rtdb= FairRun::Instance()->GetRuntimeDb();
  CbmGeoPsdPar* par=(CbmGeoPsdPar*)(rtdb->getContainer("CbmGeoPsdPar"));
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
  ProcessNodes( volList ); */
}
// -------------------------------------------------------------------------



// -----   Private method AddHit   -----------------------------------------
CbmPsdPoint* CbmPsd::AddHit(Int_t trackID, Int_t detID, TVector3 pos,
			    TVector3 mom, Double_t time, Double_t length, 
			    Double_t eLoss) {
  TClonesArray& clref = *fPsdCollection;
  Int_t size = clref.GetEntriesFast();
//  cout << " Size " << size <<endl;
  return new(clref[size]) CbmPsdPoint(trackID, detID, pos, mom,
				      time, length, eLoss);
}
// -------------------------------------------------------------------------



ClassImp(CbmPsd)
