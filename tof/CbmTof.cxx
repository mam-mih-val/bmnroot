// -------------------------------------------------------------------------
// -----                       CbmTof source file                      -----
// -----                  Created 28/07/04  by V. Friese               -----
// -------------------------------------------------------------------------

#include "CbmTof.h"

#include "CbmTofPoint.h"
#include "CbmTofGeoHandler.h"
#include "CbmGeoTofPar.h"
#include "CbmGeoTof.h"
#include "CbmDetectorList.h"
#include "CbmStack.h"

#include "FairGeoInterface.h"
#include "FairGeoLoader.h"
#include "FairGeoNode.h"
#include "FairRootManager.h"
#include "FairRuntimeDb.h"
#include "FairRun.h"
#include "FairVolume.h"

#include "TClonesArray.h"
#include "TVirtualMC.h"
#include "TObjArray.h"

#include <iostream>

using std::cout;
using std::endl;

// -----   Default constructor   -------------------------------------------
CbmTof::CbmTof() 
  : FairDetector("TOF", kTRUE, kTOF),
    fTrackID(-1),
    fVolumeID(-1),
    fPos(),
    fMom(),
    fTime(-1.),
    fLength(-1.),
    fELoss(-1.),
    fPosIndex(0),
    fTofCollection(new TClonesArray("CbmTofPoint")),
	fGeoHandler(new CbmTofGeoHandler())
{
  fVerboseLevel = 1;
}
// -------------------------------------------------------------------------



// -----   Standard constructor   ------------------------------------------
CbmTof::CbmTof(const char* name, Bool_t active)
  : FairDetector(name, active, kTOF),
    fTrackID(-1),
    fVolumeID(-1),
    fPos(),
    fMom(),
    fTime(-1.),
    fLength(-1.),
    fELoss(-1.),
    fPosIndex(0),
    fTofCollection(new TClonesArray("CbmTofPoint")),
    fGeoHandler(new CbmTofGeoHandler())
{
  fVerboseLevel = 1;
}
// -------------------------------------------------------------------------




// -----   Destructor   ----------------------------------------------------
CbmTof::~CbmTof() {
 if (fTofCollection) {
   fTofCollection->Delete(); 
   delete fTofCollection;
 }
 if (fGeoHandler) {
   delete fGeoHandler;
 }
}
// -------------------------------------------------------------------------

void CbmTof::Initialize()
{
  FairDetector::Initialize();

  // Initialize the CbmTofGeoHandler helper class from the
  // TVirtualMC interface
  Bool_t isSimulation=kTRUE;
  Int_t bla = fGeoHandler->Init(isSimulation);

}

// -----   Public method ProcessHits  --------------------------------------
Bool_t  CbmTof::ProcessHits(FairVolume* vol)
{

  // Set parameters at entrance of volume. Reset ELoss.
  if ( gMC->IsTrackEntering() ) {
    fELoss  = 0.;
    fTime   = gMC->TrackTime() * 1.0e09;
    fLength = gMC->TrackLength();
    gMC->TrackPosition(fPos);
    gMC->TrackMomentum(fMom);
  }

  // Sum energy loss for all steps in the active volume
  fELoss += gMC->Edep();

  // Create CbmTofPoint at exit of active volume
  if ( gMC->TrackCharge()!=0 &&
      (gMC->IsTrackExiting()    ||
       gMC->IsTrackStop()       ||
//       gMC->IsTrackDisappeared()) && gMC->TrackLength() > 700.
        gMC->IsTrackDisappeared())
       ) {
  
    fTrackID       = gMC->GetStack()->GetCurrentTrackNumber();

    fVolumeID = fGeoHandler->GetUniqueDetectorId();
    LOG(DEBUG2)<<"Process Hit Volume ID : "<< vol->GetName() << FairLogger::endl;
    LOG(DEBUG2)<<"Det System: "<<fGeoHandler->GetDetSystemId(fVolumeID)<<FairLogger::endl;
    LOG(DEBUG2)<<"SMtype: "<<fGeoHandler->GetSMType(fVolumeID)<<FairLogger::endl;
    LOG(DEBUG2)<<"SModule: "<<fGeoHandler->GetSModule(fVolumeID)<<FairLogger::endl;
    LOG(DEBUG2)<<"Counter: "<<fGeoHandler->GetCounter(fVolumeID)<<FairLogger::endl;
    LOG(DEBUG2)<<"Gap: "<<fGeoHandler->GetGap(fVolumeID)<<FairLogger::endl;
    LOG(DEBUG2)<<"Cell: "<<fGeoHandler->GetCell(fVolumeID)<<FairLogger::endl;
    LOG(DEBUG2)<<"Region: "<<fGeoHandler->GetRegion(fVolumeID)<<FairLogger::endl;
    LOG(DEBUG2)<<"X,Y,Z: "<< fPos.X()<<" "<<fPos.Y()<<" "<<fPos.Z()<< FairLogger::endl;
    LOG(DEBUG2)<<"*************"<<FairLogger::endl;
/*    
    cout << "Process Hit Volume ID : "<< vol->GetName() << endl;
    cout << "Det System: "<<fGeoHandler->GetDetSystemId(fVolumeID) << endl;
    cout << "SMtype: "<<fGeoHandler->GetSMType(fVolumeID) << endl;
    cout << "SModule: "<<fGeoHandler->GetSModule(fVolumeID) << endl;
    cout << "Counter: "<<fGeoHandler->GetCounter(fVolumeID) << endl;
    cout << "Gap: "<<fGeoHandler->GetGap(fVolumeID) << endl;
    cout << "Cell: "<<fGeoHandler->GetCell(fVolumeID) << endl;
    cout << "Region: "<<fGeoHandler->GetRegion(fVolumeID) << endl;
    cout << "X,Y,Z: "<< fPos.X()<<" "<<fPos.Y()<<" "<<fPos.Z()<< endl;
*/
   //fVolumeID = ((region-1)<<24) + ((module-1)<<14) + ((cell-1)<<4) + (gap-1);

    AddHit(fTrackID, fVolumeID, TVector3(fPos.X(),  fPos.Y(),  fPos.Z()),
	   TVector3(fMom.Px(), fMom.Py(), fMom.Pz()), fTime, fLength, 
	   fELoss);

    // Increment number of tof points for TParticle
    CbmStack* stack = (CbmStack*) gMC->GetStack();
    stack->AddPoint(kTOF);

    ResetParameters();
  }

  return kTRUE;
}
// -------------------------------------------------------------------------



// -----   Public method EndOfEvent   --------------------------------------
void CbmTof::EndOfEvent() {
  if (fVerboseLevel) Print();
  fTofCollection->Delete();
  fPosIndex = 0;
}
// -------------------------------------------------------------------------



// -----   Public method Register   ----------------------------------------
void CbmTof::Register() {
  FairRootManager::Instance()->Register("TofPoint", "Tof", fTofCollection, kTRUE);
}
// -------------------------------------------------------------------------



// -----   Public method GetCollection   -----------------------------------
TClonesArray* CbmTof::GetCollection(Int_t iColl) const {
  if (iColl == 0) return fTofCollection;
  else return NULL;
}
// -------------------------------------------------------------------------



// -----   Public method Print   -------------------------------------------
void CbmTof::Print() const {
  Int_t nHits = fTofCollection->GetEntriesFast();
  cout << "-I- CbmTof: " << nHits << " points registered in this event." 
       << endl;
  if (fVerboseLevel>1)
    for (Int_t i=0; i<nHits; i++) (*fTofCollection)[i]->Print();
}
// -------------------------------------------------------------------------



// -----   Public method Reset   -------------------------------------------
void CbmTof::Reset() {
  fTofCollection->Delete();
  ResetParameters();
}
// -------------------------------------------------------------------------



// -----   Public method CopyClones   --------------------------------------
void CbmTof::CopyClones(TClonesArray* cl1, TClonesArray* cl2, Int_t offset){
  Int_t nEntries = cl1->GetEntriesFast();
  cout << "-I- CbmTof: " << nEntries << " entries to add." << endl;
  TClonesArray& clref = *cl2;
  CbmTofPoint* oldpoint = NULL;
   for (Int_t i=0; i<nEntries; i++) {
   oldpoint = (CbmTofPoint*) cl1->At(i);
    Int_t index = oldpoint->GetTrackID() + offset;
    oldpoint->SetTrackID(index);
    new (clref[fPosIndex]) CbmTofPoint(*oldpoint);
    fPosIndex++;
  }
  cout << "-I- CbmTof: " << cl2->GetEntriesFast() << " merged entries." 
       << endl;
}
// -------------------------------------------------------------------------
void CbmTof::ConstructGeometry()
{
  TString fileName=GetGeometryFileName();
  if (fileName.EndsWith(".geo")) {
    LOG(INFO)<<"Constructing TOF geometry from ASCII file "<<fileName<<FairLogger::endl;
    ConstructASCIIGeometry();
  } else if (fileName.EndsWith(".root")) {
    LOG(INFO)<<"Constructing TOF geometry from ROOT file "<<fileName<<FairLogger::endl;
    ConstructRootGeometry();
  } else {
    std::cout << "Geometry format not supported." << std::endl;
  }
}
 
// -----   Public method ConstructGeometry   -------------------------------
void CbmTof::ConstructASCIIGeometry() {
  
  Int_t count=0;
  Int_t count_tot=0;

  FairGeoLoader*    geoLoad = FairGeoLoader::Instance();
  FairGeoInterface* geoFace = geoLoad->getGeoInterface();
  CbmGeoTof*       tofGeo  = new CbmGeoTof();
  tofGeo->setGeomFile(GetGeometryFileName());
  geoFace->addGeoModule(tofGeo);

  Bool_t rc = geoFace->readSet(tofGeo);
  if (rc) tofGeo->create(geoLoad->getGeoBuilder());
  TList* volList = tofGeo->getListOfVolumes();

  // store geo parameter
  FairRun *fRun = FairRun::Instance();
  FairRuntimeDb *rtdb= FairRun::Instance()->GetRuntimeDb();
  CbmGeoTofPar* par=(CbmGeoTofPar*)(rtdb->getContainer("CbmGeoTofPar"));
  TObjArray *fSensNodes = par->GetGeoSensitiveNodes();
  TObjArray *fPassNodes = par->GetGeoPassiveNodes();

  TListIter iter(volList);
  FairGeoNode* node   = NULL;
  FairGeoVolume *aVol=NULL;

  while( (node = (FairGeoNode*)iter.Next()) ) {
      aVol = dynamic_cast<FairGeoVolume*> ( node );
       if ( node->isSensitive()  ) {
           fSensNodes->AddLast( aVol );
	   count++;
       }else{
           fPassNodes->AddLast( aVol );
       }
       count_tot++;
  }
  par->setChanged();
  par->setInputVersion(fRun->GetRunId(),1);  
  ProcessNodes ( volList );
}
// -------------------------------------------------------------------------
Bool_t CbmTof::CheckIfSensitive(std::string name)
{
  TString tsname = name; 
  if (tsname.Contains("Cell")){
    return kTRUE;
  }
  if (tsname.Contains("Gas")){
    return kTRUE;
  }
  return kFALSE;
}


// -----   Private method AddHit   -----------------------------------------
CbmTofPoint* CbmTof::AddHit(Int_t trackID, Int_t detID, TVector3 pos,
			    TVector3 mom, Double_t time, Double_t length, 
			    Double_t eLoss) {
  TClonesArray& clref = *fTofCollection;
  Int_t size = clref.GetEntriesFast();
  return new(clref[size]) CbmTofPoint(trackID, detID, pos, mom,
				      time, length, eLoss);
}
// -------------------------------------------------------------------------



ClassImp(CbmTof)
