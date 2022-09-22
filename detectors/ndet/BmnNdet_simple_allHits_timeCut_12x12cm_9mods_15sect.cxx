/******************************************************************************
 *
 *         Class BmnNdet
 *         
 *  Adopted for BMN by:   Elena Litvinenko (EL)
 *  e-mail:   litvin@nf.jinr.ru
 *  Version:  06-11-2015
 *  Last update:  22-Feb-2012 (EL)  
 *
 *  Modified by M.Golubeva July 2022  
 *
 *****************************************************************************/

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
#include "BmnNdetGeo.h"
#include "FairGeoRootBuilder.h"
//#include "FairStack.h"
#include "CbmStack.h"
#include "BmnNdet.h"
#include "BmnNdetPoint.h"

#include "FairRootManager.h"
#include "FairVolume.h"
// add on for debug
//#include "FairGeoG3Builder.h"
#include "FairRuntimeDb.h"
#include "TObjArray.h"
#include "FairRun.h"

#include "TParticlePDG.h"

// -----   Default constructor   -------------------------------------------
BmnNdet::BmnNdet() {
  fNdetCollection        = new TClonesArray("BmnNdetPoint");
  volDetector = 0;
  fPosIndex   = 0; 
  //fEventID=-1; 
  fHitNb=0; 
  fVSCVolId=0;
  fVSCNICAVolId=0;
  fVSCVETOVolId=0;
  fVerboseLevel = 1;
  //fNHits=0; 

}

// -----   Standard constructor   ------------------------------------------
BmnNdet::BmnNdet(const char* name, Bool_t active)
  : FairDetector(name, active) {
  fNdetCollection        = new TClonesArray("BmnNdetPoint");
  fPosIndex   = 0;
  volDetector = 0;
  //fEventID=-1;
  fHitNb=0; 
  fVSCVolId=0;
  fVSCNICAVolId=0;
  fVSCVETOVolId=0;
  fVerboseLevel = 1;
  //fNHits=0; 
}

// -----   Destructor   ----------------------------------------------------
BmnNdet::~BmnNdet() {
  if (fNdetCollection) {
    fNdetCollection->Delete();
    delete fNdetCollection;
  }
  
}

// -----   Public method Intialize   ---------------------------------------
void BmnNdet::Initialize() {
  // Init function
  
  FairDetector::Initialize();
  FairRun* sim = FairRun::Instance();
  FairRuntimeDb* rtdb=sim->GetRuntimeDb();

  fVSCVolId = gMC->VolId("ndet01s");
  fVSCNICAVolId = gMC->VolId("ndet01s_NICA");
  fVSCVETOVolId = gMC->VolId("ndet01s_VETO");

   Int_t nbMods=9;
   Int_t nbSect=15;

   Double_t modX[nbMods], modY[nbMods], sectZ[nbSect];
   // Get nDet module mapping (if mapping file is set)

   for (int i = 0; i < nbMods; i++) { modX[i] = -10000; modY[i] = -10000; }

   if (!(fMappingFile_modules == "nofile")) {

     TString dummy;
     ifstream in;

     TString dir = getenv("VMCWORKDIR");
     TString path = dir + "/input/";
     in.open((path + fMappingFile_modules).Data());
     if (!in.is_open())
     {
         printf("Loading nDet modules Map from file: %s - file open error!\n", fMappingFile_modules.Data());
         //return kFATAL;
         return;
     }
     printf("Loading nDet modules Map from file: %s\n", fMappingFile_modules.Data());
     in >> dummy >> dummy >> dummy;
     while (!in.eof()) {
         int id;
         float x,y;
         in >> id >> x >> y;
         if (!in.good()) break;
         printf("%d %f %f\n",id,x,y);
         modX[id] = x;
         modY[id] = y;
     }

     in.close();

   }// if (!(fMappingFile_modules == "nofile"))

   // Get nDet sectio mapping (if mapping file is set)
   for (int i = 0; i <nbSect; i++) { sectZ[i] = -10000; }

   if (!(fMappingFile_sections == "nofile")) {

     TString dummy;
     ifstream in;

     TString dir = getenv("VMCWORKDIR");
     TString path = dir + "/input/";
     in.open((path + fMappingFile_sections).Data());
     if (!in.is_open())
     {
         printf("Loading nDet sections Map from file: %s - file open error!\n", fMappingFile_sections.Data());
         //return kFATAL;
         return;
     }
     printf("Loading nDet sections_Map from file: %s\n", fMappingFile_sections.Data());
     in >> dummy >> dummy;
     while (!in.eof()) {
         int id;
         float z;
         in >> id >> z;
         if (!in.good()) break;
         printf("%d %f\n",id,z);
         sectZ[id] = z;
     }

     in.close();

   }// if (!(fMappingFile_sections == "nofile"))
   
     for (Int_t im=0; im<nbMods; im++) {//mmods
       for (Int_t iv=0; iv<nbSect; iv++) {//14slices + Veto
	 scintPos_X[im][iv] = modX[im+1];
	 scintPos_Y[im][iv] = modY[im+1];
	 scintPos_Z[im][iv] = sectZ[iv+1];
       }
     }
   
  for (Int_t im=0; im<nbMods; im++) {
    for (Int_t iv=0; iv<nbSect; iv++) {//14slices + Veto
      if(iv==0) cout <<"MODULE " <<im+1 <<" " <<scintPos_X[im][iv] <<" " <<scintPos_Y[im][iv] <<endl;
      cout <<scintPos_Z[im][iv] <<" ";
    }//sect
    cout <<endl;
  }//mods
   
}

// -------------------------------------------------------------------------
void BmnNdet::BeginEvent(){
  // Begin of the event
  //fNHits = 0;
  //fEvNb++;
  //evNbCheck = -1;
  for (Int_t i=0; i<70; i++) //slice
    //for (Int_t ii=0; ii<50; ii++) 
    for (Int_t ii=0; ii<1000; ii++) //mod
      fTofArray[i][ii] = 100000;
 
}

//_____________________________________________________________________________
BmnNdetPoint* BmnNdet::GetHit(Int_t i) const
{
// Returns the hit for the specified layer.
// ---

  return (BmnNdetPoint*)fNdetCollection->At(i);
}

//_____________________________________________________________________________
BmnNdetPoint* BmnNdet::GetHit(Int_t vsc, Int_t mod) const
{
// Returns the hit for the specified vsc and module.
// ---

  BmnNdetPoint *hit;
  Int_t nofHits = fNdetCollection->GetEntriesFast();
  for (Int_t i=0; i<nofHits; i++) {
    hit =  GetHit(i);
    //cout <<"fEdep " <<i <<" " <<hit->GetEdep() <<endl;
    //int iVSCId = hit->GetVSCId();
    //int iMODId = hit->GetMODId();
    //if(iVSCId == vsc && iMODId == mod)
    if(hit->GetCopy() == vsc && hit->GetCopyMother() == mod)
      return hit;
  }
  return 0;
}

//_____________________________________________________________________________
BmnNdetPoint* BmnNdet::GetHitPrint(Int_t vsc, Int_t mod) const
{
// Returns the hit for the specified vsc and module.
// ---

  BmnNdetPoint *hit;
  Int_t nofHits = fNdetCollection->GetEntriesFast();

  for (Int_t i=0; i<nofHits; i++) {
    hit =  GetHit(i);
    if(hit->GetCopy() == vsc && hit->GetCopyMother() == mod)
      cout <<"GetHitPrint method " <<hit->GetCopyMother() <<" " <<hit->GetCopy()  <<" " <<hit->GetEnergyLoss() <<" " <<hit->GetZ() <<endl;
    //cout <<"GetHitPrint method " <<hit->GetCopyMother() <<" " <<hit->GetCopy()  <<" " <<hit->GetEnergyLoss() <<" " <<hit->GetNHits() <<" " <<hit->GetZ() <<endl;
  }
  return 0;
}

// -----   Public method ProcessHits  --------------------------------------
Bool_t BmnNdet::ProcessHits(FairVolume* vol) {
  // if (TMath::Abs(gMC->TrackCharge()) <= 0) return kFALSE;

  Int_t copyNoVSC,copyNoVTYVEC,copyNoVMOD,copyNoVZDC;
  Int_t copyNoVSCNICA,copyNoVTYVECNICA,copyNoVMODNICA,copyNoVZDCNICA;
  Int_t copyNoVSCVETO,copyNoVTYVECVETO,copyNoVMODVETO,copyNoVZDCVETO;
  Int_t copyNoVSCCom,copyNoVTYVECCom,copyNoVMODCom,copyNoVZDCCom;

  Int_t      ivol;
  TLorentzVector tPos1, tMom1;
  TLorentzVector tPos, tMom;

  Int_t module, module_nica; 
  Int_t slice, slice_nica;
  
  Double_t time=0;
  Double_t length =0;

  TParticle* part;
  Double_t charge;

  //Double_t timeCut = 45;
  Double_t timeCut = 55;
  Double_t  QCF=1; //quenching for Birk
  Double_t  BirkConst = 12.6; //0.126 mm/MeV for polystyrene 
  //0.126 *(0.1/0.001) = 12.6 cm/GeV
  //(0.126 mm/MeV - from Wikipedia, 0.07943mm/MeV є in Geant4)

#ifdef EDEBUG
  static Int_t lEDEBUGcounter=0;
  if (lEDEBUGcounter<1)
    std::cout << "EDEBUG-- BmnNdet::ProcessHits: entered" << gMC->CurrentVolPath() << endl;
#endif

  if (gMC->CurrentVolID(copyNoVSC) != fVSCVolId &&
      gMC->CurrentVolID(copyNoVSCNICA) != fVSCNICAVolId &&
      gMC->CurrentVolID(copyNoVSCVETO) != fVSCVETOVolId) {
    return kFALSE;
  }
  
  ivol    = vol->getMCid();
  //cout <<"fEvNb " <<gMC->CurrentEvent() <<endl;
  fEventID = gMC->CurrentEvent();

  if (gMC->CurrentVolID(copyNoVSC) == fVSCVolId || gMC->CurrentVolID(copyNoVSCNICA) == fVSCNICAVolId || gMC->CurrentVolID(copyNoVSCVETO) == fVSCVETOVolId) {
    gMC->CurrentVolOffID(0, slice); //no tyvec
    gMC->CurrentVolOffID(1, module);
    copyNoVTYVECCom = slice; copyNoVMODCom = module;
  }
  //cout <<"TEST " <<slice <<" " <<module <<" " <<fTofArray[slice][module] <<endl;

    if (gMC->IsTrackEntering()) {

     ResetParameters();
      fELoss = 0.;
      time    = 0.;
      length  = 0.;
      gMC->TrackPosition(tPos);
      gMC->TrackMomentum(tMom);

	fTrackID = gMC->GetStack()->GetCurrentTrackNumber();
	part    = gMC->GetStack()->GetCurrentTrack();
	//if(slice==15) std::cout << "ENTER " << gMC->CurrentVolPath() << " " <<part->GetPdgCode() <<" " << fTrackID <<" " <<gMC->Edep() <<" " <<module <<" " <<slice <<" " <<tMom.Pz() <<std::endl;

#ifdef EDEBUG
      gMC->TrackPosition(tPos1);
      gMC->TrackMomentum(tMom1);
#endif

    }//if (gMC->IsTrackEntering())

    if ( gMC->IsTrackInside()) {

      gMC->TrackPosition(tPos);
      gMC->TrackMomentum(tMom);
      //length += gMC->TrackStep();    

      //time   += gMC->TrackTime() * 1.0e09;
      time   = gMC->TrackTime() * 1.0e09;//nsec

      //fELoss +=gMC->Edep();
//Birk corrections

      //time cut
      if(time<timeCut) {
	if(gMC->TrackStep()>0) QCF = 1.+(BirkConst/gMC->TrackStep())*gMC->Edep();
	else QCF=1;
	fELoss +=(gMC->Edep())/QCF;
      }
            
      part    = gMC->GetStack()->GetCurrentTrack();

      fTrackID = gMC->GetStack()->GetCurrentTrackNumber();
      ////cout <<"TTT INSIDE_1 " <<slice <<" " <<module <<" " <<gMC->TrackTime() * 1.0e09 <<" " <<fTofArray[slice][module] <<endl;
      //if(gMC->TrackTime() * 1.0e09 < fTofArray[slice][module]) fTofArray[slice][module] = gMC->TrackTime() * 1.0e09;
      
      //time cut
      if(time<timeCut) {
	if(gMC->TrackTime() * 1.0e09 < fTofArray[slice][module]) fTofArray[slice][module] = gMC->TrackTime() * 1.0e09;
      }
      
      //cout <<"TTT INSIDE_2 " <<slice <<" " <<module <<" " <<gMC->TrackTime() * 1.0e09 <<" " <<fTofArray[slice][module] <<endl;

      if ( gMC->IsTrackStop() || gMC->IsTrackDisappeared() ) {
	
	part    = gMC->GetStack()->GetCurrentTrack();
	charge = part->GetPDG()->Charge() / 3. ;
	
// Create BmnNdetPoint
	fTrackID = gMC->GetStack()->GetCurrentTrackNumber();
	//if(slice==15) std::cout << "STOP_DIS " << gMC->CurrentVolPath() << " " <<part->GetPdgCode() <<" " << fTrackID <<" " <<gMC->Edep() <<" " <<module <<" " <<slice <<" " <<tMom.Pz() <<std::endl;

	if(fELoss>0) {
	  /*
//VETO //if not to wtite events with Veto 
	  //if(slice==15) return kFALSE;	  
	  if(slice==15) {
	    //std::cout << "STOP_DIS_slice15 " <<part->GetPdgCode() <<" " << fTrackID <<" " <<gMC->Edep() <<" " <<module <<" " <<slice<<std::endl;	  
	    gMC->StopTrack();
	    return kFALSE;
	  }
	  */

	  length = 1; //nb of hits in slice    
	  
	  //std::cout << "INSIDE MpdNdet::ProcessHits: TrackID: " <<part->GetPdgCode() <<" " << fTrackID << "  " <<fELoss <<" " <<length <<" " << gMC->CurrentVolPath() << " " << tPos.Z() <<"   "  <<tMom.Pz() <<" " << ivol <<" " <<gMC->CurrentVolOffName(2) << " " <<gMC->CurrentVolOffName(1) << " " << gMC->CurrentVolOffName(0) <<" " <<module <<" " <<slice <<std::endl;
	  if(copyNoVTYVECCom==slice && copyNoVMODCom==module) {//module	    
	    //if ( !GetHit(slice,module) ) {
	      //AddHit(fTrackID, ivol, slice, module, TVector3(tPos.X(), tPos.Y(), tPos.Z()),TVector3(tMom.Px(), tMom.Py(), tMom.Pz()), time, length, fELoss);
	      AddHit(fTrackID, ivol, slice, module, TVector3(scintPos_X[module-1][slice-1], scintPos_Y[module-1][slice-1], scintPos_Z[module-1][slice-1]),TVector3(tMom.Px(), tMom.Py(), tMom.Pz()), fTofArray[slice][module], length, fELoss, fEventID );//with coord
	      //}
	      /*
	    else {
	      //GetHit(slice,module)->AddVSC(fTrackID, ivol, slice, module, TVector3(tPos.X(), tPos.Y(), tPos.Z()),TVector3(tMom.Px(), tMom.Py(), tMom.Pz()), time, length, fELoss);
	      GetHit(slice,module)->AddVSC(fTrackID, ivol, slice, module, TVector3(scintPos_X[module-1][slice-1], scintPos_Y[module-1][slice-1], scintPos_Z[module-1][slice-1]),TVector3(tMom.Px(), tMom.Py(), tMom.Pz()), fTofArray[slice][module], length, fELoss, fEventID );//with coord
	    }
	      */
	  }//if(copyNoVTYVECCom==slice && copyNoVMODCom==module)	  
	  
	}//if(fELoss>0)
      }//if ( gMC->IsTrackStop() || gMC->IsTrackDisappeared() )
    }//if ( gMC->IsTrackInside())
      
    if ( gMC->IsTrackExiting()) {

      gMC->TrackPosition(tPos);
      gMC->TrackMomentum(tMom);
      part    = gMC->GetStack()->GetCurrentTrack();
      fTrackID = gMC->GetStack()->GetCurrentTrackNumber();
      ////std::cout << "EXIT " << gMC->CurrentVolPath() << " " <<part->GetPdgCode() <<" " << fTrackID <<" " <<gMC->Edep() <<" "<< tPos.Z() <<" "  <<tMom.Pz() <<" " <<gMC->TrackTime() * 1.0e09 <<std::endl;

// Create BmnNdetPoint
      fTrackID = gMC->GetStack()->GetCurrentTrackNumber();
      //time    += gMC->TrackTime() * 1.0e09;
      time    = gMC->TrackTime() * 1.0e09;
      //length  += gMC->TrackLength();
      
      ////cout <<"TTT EXIT_1 " <<slice <<" " <<module <<" " <<gMC->TrackTime() * 1.0e09 <<" " <<fTofArray[slice][module] <<endl;

      //time cut
      if(time<timeCut) {
	if(gMC->TrackTime() * 1.0e09 < fTofArray[slice][module]) fTofArray[slice][module] = gMC->TrackTime() * 1.0e09;
      }
      ////cout <<"TTT EXIT_2 " <<slice <<" " <<module <<" " <<gMC->TrackTime() * 1.0e09 <<" " <<fTofArray[slice][module] <<endl;

      //fELoss +=gMC->Edep();
//Birk corrections
      if(time<timeCut) {
	if(gMC->TrackStep()>0) QCF = 1.+(BirkConst/gMC->TrackStep())*gMC->Edep();
	else QCF = 1;
	fELoss +=(gMC->Edep())/QCF;
      }

      if(fELoss>0) {
	/*
//VETO
	//if(slice==15) return kFALSE;	  
	if(slice==15) {
	  //std::cout << "EXIT_Edep>0_slice15 " <<part->GetPdgCode() <<" " << fTrackID <<" " <<gMC->Edep() <<" " <<module <<" " <<slice<<std::endl;	  
	  gMC->StopTrack();
	  return kFALSE;
	}
	*/

	length = 1; //nb of hits in slice    

	//cout <<"TTT EXIT_2 " <<slice <<" " <<module <<" " <<gMC->TrackTime() * 1.0e09 <<" " <<fTofArray[slice][module] <<" " <<length <<" " <<gMC->Edep() <<endl;
	if(copyNoVTYVECCom==slice && copyNoVMODCom==module) {
	  //if ( !GetHit(slice,module) ) {
	    //AddHit(fTrackID, ivol, slice, module, TVector3(tPos.X(), tPos.Y(), tPos.Z()),TVector3(tMom.Px(), tMom.Py(), tMom.Pz()), time, length, fELoss);
	    AddHit(fTrackID, ivol, slice, module, TVector3(scintPos_X[module-1][slice-1], scintPos_Y[module-1][slice-1], scintPos_Z[module-1][slice-1]),TVector3(tMom.Px(), tMom.Py(), tMom.Pz()), fTofArray[slice][module], length, fELoss, fEventID );//with coord
	    //}
	    /*
	  else {
	    //GetHit(slice,module)->AddVSC(fTrackID, ivol, slice, module, TVector3(tPos.X(), tPos.Y(), tPos.Z()),TVector3(tMom.Px(), tMom.Py(), tMom.Pz()), time, length, fELoss);
	    GetHit(slice,module)->AddVSC(fTrackID, ivol, slice, module, TVector3(scintPos_X[module-1][slice-1], scintPos_Y[module-1][slice-1], scintPos_Z[module-1][slice-1]),TVector3(tMom.Px(), tMom.Py(), tMom.Pz()), fTofArray[slice][module], length, fELoss, fEventID );//with coord
	  }
	    */
	}//if(copyNoVTYVECCom==slice && copyNoVMODCom==module)	  

      }//if(fELoss>0)
    }//if ( gMC->IsTrackExiting()) {

      Int_t points = gMC->GetStack()->GetCurrentTrack()->GetMother(1);  

      points = ( points & ( ~ (1<<30) ) ) | (1 << 30);

      gMC->GetStack()->GetCurrentTrack()->SetMother(1,points);

      ((CbmStack*)gMC->GetStack())->AddPoint(kNDET);
     
    return kTRUE;
  
}

// -----   Public method EndOfEvent   -----------------------------------------
void BmnNdet::EndOfEvent() {
  if (fVerboseLevel)  Print();
  Reset();
}


// -----   Public method Register   -------------------------------------------
void BmnNdet::Register() {
  FairRootManager::Instance()->Register("NdetPoint","Ndet", fNdetCollection, kTRUE);
}

// -----   Public method GetCollection   --------------------------------------
TClonesArray* BmnNdet::GetCollection(Int_t iColl) const {
  if (iColl == 0) return fNdetCollection;

  return NULL;
}

// -----   Public method Print   ----------------------------------------------
void BmnNdet::Print() const {
  Int_t nHits = fNdetCollection->GetEntriesFast();
  cout << "-I- BmnNdet: " << nHits << " points registered in this event."
       << endl;
  
  if (fVerboseLevel>1)
    for (Int_t i=0; i<nHits; i++) (*fNdetCollection)[i]->Print();
}

// -----   Public method Reset   ----------------------------------------------
void BmnNdet::Reset() {
  fNdetCollection->Delete();
  
  fPosIndex = 0;
}

// guarda in FairRootManager::CopyClones
// -----   Public method CopyClones   -----------------------------------------
void BmnNdet::CopyClones(TClonesArray* cl1, TClonesArray* cl2, Int_t offset ) {
  Int_t nEntries = cl1->GetEntriesFast();
  //cout << "-I- BmnNdet: " << nEntries << " entries to add." << endl;
  TClonesArray& clref = *cl2;
  BmnNdetPoint* oldpoint = NULL;
  for (Int_t i=0; i<nEntries; i++) {
    oldpoint = (BmnNdetPoint*) cl1->At(i);
    Int_t index = oldpoint->GetTrackID() + offset;
    oldpoint->SetTrackID(index);
    new (clref[fPosIndex]) BmnNdetPoint(*oldpoint);
    fPosIndex++;
  }
  cout << " -I- BmnNdet: " << cl2->GetEntriesFast() << " merged entries."
       << endl;
}

 // -----   Public method ConstructGeometry   ---------------------------------
void BmnNdet::ConstructGeometry() {
  
  TString fileName = GetGeometryFileName();
  if(fileName.EndsWith(".root")) 
    {
      FairLogger::GetLogger()->Info(MESSAGE_ORIGIN, "Constructing NDET geometry from ROOT file %s", fileName.Data());
      ConstructRootGeometry();
    }
  /*
    else if ( fileName.EndsWith(".geo") ) 
    {
    FairLogger::GetLogger()->Info(MESSAGE_ORIGIN, "Constructing NDET geometry from ASCII file %s", fileName.Data());
    ConstructAsciiGeometry();
    }
    else	FairLogger::GetLogger()->Fatal(MESSAGE_ORIGIN, "Geometry format of NDET file %S not supported.", fileName.Data());    
  */
  
  FairGeoLoader*    geoLoad = FairGeoLoader::Instance();
  FairGeoInterface* geoFace = geoLoad->getGeoInterface();
  BmnNdetGeo*      ndetGeo = new BmnNdetGeo();
  ndetGeo->setGeomFile(GetGeometryFileName());
  geoFace->addGeoModule(ndetGeo);

  Bool_t rc = geoFace->readSet(ndetGeo);
  if (rc) ndetGeo->create(geoLoad->getGeoBuilder());
  TList* volList = ndetGeo->getListOfVolumes();

  // store geo parameter
  FairRun *fRun = FairRun::Instance();
  FairRuntimeDb *rtdb= FairRun::Instance()->GetRuntimeDb();
  BmnNdetGeoPar* par=(BmnNdetGeoPar*)(rtdb->getContainer("BmnNdetGeoPar"));
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
  
//Check if Sensitive-----------------------------------------------------------
Bool_t BmnNdet::CheckIfSensitive(std::string name) {
  TString tsname = name;
  if (tsname.Contains("ndet01s") || tsname.Contains("ndet01s_NICA") || tsname.Contains("ndet01s_VETO")) {
    return kTRUE;
  }
  return kFALSE;
}

// -----   Private method AddHit   --------------------------------------------
//BmnNdetPoint* BmnNdet::AddHit(Int_t trackID, Int_t detID, Int_t copyNo, Int_t copyNoMother, TVector3 pos, TVector3 mom, Double_t time, Double_t length, Double_t eLoss) {
BmnNdetPoint* BmnNdet::AddHit(Int_t trackID, Int_t detID, Int_t copyNo, Int_t copyNoMother, TVector3 pos, TVector3 mom, Double_t time, Double_t length, Double_t eLoss, UInt_t EventId) {
  TClonesArray& clref = *fNdetCollection;
  Int_t size = clref.GetEntriesFast();

  //return new(clref[size]) BmnNdetPoint(trackID, detID, copyNo, copyNoMother, pos, mom, time, length, eLoss);
  return new(clref[size]) BmnNdetPoint(trackID, detID, copyNo, copyNoMother, pos, mom, time, length, eLoss, EventId);

}


ClassImp(BmnNdet)
