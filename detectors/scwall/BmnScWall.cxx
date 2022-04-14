#include "BmnScWall.h"

#include <iostream>

#include "BmnScWallGeo.h"
#include "BmnScWallPoint.h"
#include "CbmStack.h"
#include "FairGeoInterface.h"
#include "FairGeoLoader.h"
#include "FairGeoNode.h"
#include "FairGeoRootBuilder.h"
#include "FairMCPoint.h"
#include "FairRootManager.h"
#include "FairVolume.h"
#include "TClonesArray.h"
#include "TGeoArb8.h"
#include "TGeoMCGeometry.h"
#include "TGeoManager.h"
#include "TLorentzVector.h"
#include "TParticle.h"
#include "TVector3.h"
#include "TVirtualMC.h"
// add on for debug
//#include "FairGeoG3Builder.h"
#include "FairRun.h"
#include "FairRuntimeDb.h"
#include "TObjArray.h"
#include "TParticlePDG.h"

// -----   Default constructor   -------------------------------------------
BmnScWall::BmnScWall() {
    fScWallCollection = new TClonesArray("BmnScWallPoint");
    volDetector = 0;
    fEventID=-1; 
    fCellSmallCutVolId=0;
    fCellSmallTrapVolId=0;
    fCellLargeCutVolId=0;
    fCellLargeTrapVolId=0;
    fVerboseLevel = 1;
}

// -----   Standard constructor   ------------------------------------------
BmnScWall::BmnScWall(const char* name, Bool_t active)
    : FairDetector(name, active) {
    fScWallCollection = new TClonesArray("BmnScWallPoint");
    volDetector = 0;
    fEventID=-1; 
    fCellSmallCutVolId=0;
    fCellSmallTrapVolId=0;
    fCellLargeCutVolId=0;
    fCellLargeTrapVolId=0;
    fVerboseLevel = 1;
}

// -----   Destructor   ----------------------------------------------------
BmnScWall::~BmnScWall() {
    if (fScWallCollection) {
        fScWallCollection->Delete();
        delete fScWallCollection;
    }
}

// -----   Public method Intialize   ---------------------------------------
void BmnScWall::Initialize() {
    // Init function

    FairDetector::Initialize();
    FairRun* sim = FairRun::Instance();
    FairRuntimeDb* rtdb = sim->GetRuntimeDb();

    fCellSmallCutVolId = gMC->VolId("scwall_sens_small_cutted");
    fCellSmallTrapVolId = gMC->VolId("scwall_sens_small_trap");
    fCellLargeCutVolId = gMC->VolId("scwall_sens_large_cutted");
    fCellLargeTrapVolId = gMC->VolId("scwall_sens_large_trap");
}

//_____________________________________________________________________________
BmnScWallPoint* BmnScWall::GetHit(Int_t i) const
{
// Returns the hit for the specified slice.
// ---

  return (BmnScWallPoint*)fScWallCollection->At(i);
}


//_____________________________________________________________________________
BmnScWallPoint* BmnScWall::GetHit(Int_t slice, Int_t cell) const
{
// Returns the hit for the specified vsc and module.
// ---

  BmnScWallPoint *hit;
  Int_t nofHits = fScWallCollection->GetEntriesFast();
  for (Int_t i=0; i<nofHits; i++) {
    hit =  GetHit(i);
    if(hit->GetCopy() == slice && hit->GetCopyMother() == cell)
      return hit;
  }

  return 0;
}

// -----   Public method ProcessHits  --------------------------------------
Bool_t BmnScWall::ProcessHits(FairVolume* vol) {
    /** This method is called from the MC stepping */

  Int_t copyNoCellSmallCut;
  Int_t copyNoCellSmallTrap;
  Int_t copyNoCellLargeCut;
  Int_t copyNoCellLargeTrap;
  Int_t copyNoSLICECom, copyNoCELLCom;

  Int_t      ivol;
  TLorentzVector tPos1, tMom1;
  TLorentzVector tPos, tMom;

  Int_t slice, cell;

  Double_t time=0;
  Double_t length =0;

  TParticle* part;
  Double_t charge;

  Double_t  QCF=1; //quenching for Birk
  Double_t  BirkConst = 12.6; //0.126 mm/MeV for polystyrene 
  //0.126 *(0.1/0.001) = 12.6 cm/GeV
  //(0.126 mm/MeV - from Wikipedia, 0.07943mm/MeV in Geant4)

  if (gMC->CurrentVolID(copyNoCellSmallCut) != fCellSmallCutVolId &&
      gMC->CurrentVolID(copyNoCellSmallTrap) != fCellSmallTrapVolId &&
      gMC->CurrentVolID(copyNoCellLargeCut) != fCellLargeCutVolId &&
      gMC->CurrentVolID(copyNoCellLargeTrap) != fCellLargeTrapVolId )
{
    return kFALSE;
  }

  ivol    = vol->getMCid();

  if (gMC->CurrentVolID(copyNoCellSmallCut) == fCellSmallCutVolId ||
      gMC->CurrentVolID(copyNoCellSmallTrap) == fCellSmallTrapVolId ||
      gMC->CurrentVolID(copyNoCellLargeCut) == fCellLargeCutVolId ||
      gMC->CurrentVolID(copyNoCellLargeTrap) != fCellLargeTrapVolId ) {

    gMC->CurrentVolOffID(0, slice);//scwall
    gMC->CurrentVolOffID(1, cell);
    copyNoSLICECom = slice; copyNoCELLCom = cell;  
  }
	 
  //cout <<"2_1_0 " << ivol <<" " <<gMC->CurrentVolOffName(2) << " " <<gMC->CurrentVolOffName(1) << " " << gMC->CurrentVolOffName(0) <<std::endl;

  if (gMC->IsTrackEntering()) {

    ResetParameters();

    fELoss = 0.;
    time    = 0.;
    length  = 0.;

    TLorentzVector PosIn;
    gMC->TrackPosition(PosIn);
    fPos.SetXYZ(PosIn.X(), PosIn.Y(), PosIn.Z());
    
    TLorentzVector MomIn;
    gMC->TrackMomentum(MomIn);
    fMom.SetXYZ(MomIn.Px(), MomIn.Py(), MomIn.Pz());
    
    fTime = gMC->TrackTime() * 1.0e09;
    fLength = gMC->TrackLength();
  }//if (gMC->IsTrackEntering())

  if ( gMC->IsTrackInside()) {

    gMC->TrackPosition(tPos);
    gMC->TrackMomentum(tMom);
    length += gMC->TrackStep();    
    
    //fELoss +=gMC->Edep();
//Birk corrections
    if(gMC->TrackStep()>0) QCF = 1.+(BirkConst/gMC->TrackStep())*gMC->Edep();
    else QCF = 1;
    fELoss +=(gMC->Edep())/QCF;
    
    time   += gMC->TrackTime() * 1.0e09;//nsec
    
    if ( gMC->IsTrackStop() || gMC->IsTrackDisappeared() ) {
      
      part    = gMC->GetStack()->GetCurrentTrack();
      charge = part->GetPDG()->Charge() / 3. ;
      
// Create BmnZdcPoint
      fTrackID = gMC->GetStack()->GetCurrentTrackNumber();
      //time    = gMC->TrackTime() * 1.0e09;
      //length  = gMC->TrackLength();
      //gMC->TrackPosition(tPos);
      //gMC->TrackMomentum(tMom);
      //gMC->CurrentVolOffID(2, module);
      //gMC->CurrentVolOffID(1, slice);
      
      if(fELoss>0) {	  
	//std::cout << "INSIDE MpdZdc::ProcessHits: TrackID:" <<part->GetPdgCode() <<" " << fTrackID << "  " <<fELoss <<" " << gMC->CurrentVolPath() << " " << tPos.Z() <<"   "  <<tMom.Pz() <<" " << ivol <<" " <<gMC->CurrentVolOffName(2) << " " <<gMC->CurrentVolOffName(1) << " " << gMC->CurrentVolOffName(0) <<std::endl;	    
	if(copyNoSLICECom==slice && copyNoCELLCom==cell) {
	  if ( !GetHit(slice,cell) ) {
	    AddHit(fTrackID, ivol, slice, cell, TVector3(tPos.X(), tPos.Y(), tPos.Z()),TVector3(tMom.Px(), tMom.Py(), tMom.Pz()),time, length, fELoss);
	  }
	  else {
	    GetHit(slice,cell)->AddCELL(fTrackID, ivol, slice, cell, TVector3(tPos.X(), tPos.Y(), tPos.Z()),TVector3(tMom.Px(), tMom.Py(), tMom.Pz()),time, length, fELoss);
	  }
	}//if(copyNoSLICECom==slice && copyNoCELLCom==cell)	  

      }//if(fELoss>0)
    }//if ( gMC->IsTrackStop() || gMC->IsTrackDisappeared() )
  }//if ( gMC->IsTrackInside())

  if ( gMC->IsTrackExiting()) {

    part    = gMC->GetStack()->GetCurrentTrack();
    
// Create BmnZdcPoint
    fTrackID = gMC->GetStack()->GetCurrentTrackNumber();
    time    += gMC->TrackTime() * 1.0e09;
    length  += gMC->TrackLength();
    
    //fELoss +=gMC->Edep();
//Birk corrections
    if(gMC->TrackStep()>0) QCF = 1.+(BirkConst/gMC->TrackStep())*gMC->Edep();
    else QCF = 1;
    fELoss +=(gMC->Edep())/QCF;
     
    gMC->TrackPosition(tPos);
    gMC->TrackMomentum(tMom);
    
    if(fELoss>0) {
      if(copyNoSLICECom==slice && copyNoCELLCom==cell) {
	if ( !GetHit(slice,cell) ) {
	  AddHit(fTrackID, ivol, slice, cell, TVector3(tPos.X(), tPos.Y(), tPos.Z()),TVector3(tMom.Px(), tMom.Py(), tMom.Pz()),time, length, fELoss);
	}
	else {
	  GetHit(slice,cell)->AddCELL(fTrackID, ivol, slice, cell, TVector3(tPos.X(), tPos.Y(), tPos.Z()),TVector3(tMom.Px(), tMom.Py(), tMom.Pz()),time, length, fELoss);
	}
      }//if(copyNoVTYVECCom==copyNoVTYVEC	  
    }//if(fELoss>0)
  }//if ( gMC->IsTrackExiting()) {

  Int_t points = gMC->GetStack()->GetCurrentTrack()->GetMother(1);  
  points = ( points & ( ~ (1<<30) ) ) | (1 << 30);
  
  gMC->GetStack()->GetCurrentTrack()->SetMother(1,points);
  
  ((CbmStack*)gMC->GetStack())->AddPoint(kSCWALL);


    /*//S.Merts
    fELoss += gMC->Edep();
    if (gMC->IsTrackExiting() || gMC->IsTrackStop() || gMC->IsTrackDisappeared()) {
        fTrackID = gMC->GetStack()->GetCurrentTrackNumber();
        fVolumeID = vol->getMCid();
        if (fELoss == 0.) {
            return kFALSE;
        }
        AddHit(fTrackID, fVolumeID, fPos, fMom, fTime, fLength, fELoss);

        ((CbmStack*)gMC->GetStack())->AddPoint(kSCWALL);
    }
    */

    return kTRUE;
}

// -----   Public method EndOfEvent   -----------------------------------------
void BmnScWall::EndOfEvent() {
  if (fVerboseLevel) Print();
  Reset();
}

// -----   Public method Register   -------------------------------------------
void BmnScWall::Register() {
  FairRootManager::Instance()->Register("ScWallPoint", "ScWall", fScWallCollection, kTRUE);
}

// -----   Public method GetCollection   --------------------------------------
TClonesArray* BmnScWall::GetCollection(Int_t iColl) const {
  if (iColl == 0) return fScWallCollection;
  return NULL;
}

// -----   Public method Print   ----------------------------------------------
void BmnScWall::Print() const {
  Int_t nHits = fScWallCollection->GetEntriesFast();
  cout << "-I- BmnScWall: " << nHits << " points registered in this event." << endl;

  if (fVerboseLevel > 1)
    for (Int_t i = 0; i < nHits; i++) (*fScWallCollection)[i]->Print();
}

// -----   Public method Reset   ----------------------------------------------
void BmnScWall::Reset() {
  fScWallCollection->Delete();
}

//------   ConstructGeometry     ----------------------------------------------
void BmnScWall::ConstructGeometry() {
  TString fileName = GetGeometryFileName();
  if (fileName.EndsWith(".root")) {
    LOG(info) << "Constructing ScWall geometry from ROOT file " << fileName.Data();
    ConstructRootGeometry();
  } else
    LOG(fatal) << "Geometry format of ScWall file " << fileName.Data() << " not supported.";
}

//------   CheckIfSensitive     -----------------------------------------------
Bool_t BmnScWall::CheckIfSensitive(std::string name) {
  TString tsname = name;
  if (tsname.Contains("sens")) return kTRUE;
  return kFALSE;
}

// -----   Private method AddHit   --------------------------------------------
BmnScWallPoint* BmnScWall::AddHit(Int_t trackID, Int_t detID, Int_t copyNo, Int_t copyNoMother, TVector3 pos, TVector3 mom, Double_t time, Double_t length, Double_t eLoss) {
  TClonesArray& clref = *fScWallCollection;
  Int_t size = clref.GetEntriesFast();
  return new (clref[size]) BmnScWallPoint(trackID, detID, copyNo, copyNoMother, pos, mom, time, length, eLoss);
}

ClassImp(BmnScWall)
