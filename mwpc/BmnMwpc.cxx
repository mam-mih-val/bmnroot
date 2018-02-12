//------------------------------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------
// -----                       BmnMWPC source file                      -----
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
#include "BmnMwpcGeo.h"
#include "FairRootManager.h"
#include "BmnMwpc.h"
#include "BmnMwpcPoint.h"
#include "FairRuntimeDb.h"
#include "BmnMwpcGeoPar.h"
#include "TObjArray.h"
#include "FairRun.h"
#include "FairVolume.h"
#include "TMath.h"
#include "CbmStack.h"

#include "TParticlePDG.h"
#include "TGDMLParse.h"
#include "FairGeoMedia.h"

//------------------------------------------------------------------------------------------------------------------------
BmnMwpc::BmnMwpc() 
 : FairDetector("MWPC", kTRUE)
{
	fPointCollection = new TClonesArray("BmnMwpcPoint");
	fPosIndex = 0;
	fVerboseLevel = 1;
	ResetParameters();
}
//------------------------------------------------------------------------------------------------------------------------
BmnMwpc::BmnMwpc(const char* name, Bool_t active)
 : FairDetector(name, active)
{  
	fPointCollection = new TClonesArray("BmnMwpcPoint");
	fPosIndex = 0;
	fVerboseLevel = 1;
	ResetParameters();
}
//------------------------------------------------------------------------------------------------------------------------
BmnMwpc::~BmnMwpc() 
{
	if(fPointCollection){fPointCollection->Delete(); delete fPointCollection; }
}
//------------------------------------------------------------------------------------------------------------------------
int BmnMwpc::DistAndPoints(TVector3 p3, TVector3 p4, TVector3& pa, TVector3& pb) {                                         
    pa=(p3+p4)*0.5;
    pb=pa;
    
    //pa=p3; //del
    //pb=pa; //del
    return 0;
} 
//------------------------------------------------------------------------------------------------------------------------
TVector3 BmnMwpc::GlobalToLocal(TVector3& global) {
    Double_t globPos[3];
    Double_t localPos[3];
    global.GetXYZ(globPos);
    gMC->Gmtod(globPos, localPos, 1); 
    return TVector3(localPos);
}
//------------------------------------------------------------------------------------------------------------------------
TVector3 BmnMwpc::LocalToGlobal(TVector3& local) {
    Double_t globPos[3];
    Double_t localPos[3];
    local.GetXYZ(localPos);
    gMC->Gdtom(localPos, globPos, 1);  
    return TVector3(globPos);
}
//----------------------------------------------------------------------------------------------------------------------
Bool_t  BmnMwpc::ProcessHits(FairVolume* vol)
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

  // Create BmnMwpcPoint at EXIT of active volume;
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

    BmnMwpcPoint *p = AddHit(fTrackID, fVolumeID, fPos, fPos.Perp(),
                TVector3(fMom.Px(), fMom.Py(), fMom.Pz()),
                fTime, (fLength+gMC->TrackLength())/2, fELoss,
                fIsPrimary, fCharge, fPdgId, trackPosition);
    p->SetPhi(phi); //AZ

    ((CbmStack*)gMC->GetStack())->AddPoint(kMWPC);
  }

  return kTRUE;
}
//------------------------------------------------------------------------------------------------------------------------
void BmnMwpc::EndOfEvent() 
{
	if(fVerboseLevel) Print();
  	fPointCollection->Clear();
  	fPosIndex = 0;
}
//------------------------------------------------------------------------------------------------------------------------
void BmnMwpc::Register(){ FairRootManager::Instance()->Register("MWPCPoint", "MWPC", fPointCollection, kTRUE); }
//------------------------------------------------------------------------------------------------------------------------
TClonesArray* BmnMwpc::GetCollection(Int_t iColl) const 
{
	if(iColl == 0) 	return fPointCollection;
	
return NULL;
}
//------------------------------------------------------------------------------------------------------------------------
void BmnMwpc::Print() const 
{
	Int_t nHits = fPointCollection->GetEntriesFast();
	cout << "-I- BmnMwpc: " << nHits << " points registered in this event." << endl;
	
	if(fVerboseLevel > 1)
    		for(Int_t i=0; i<nHits; i++) (*fPointCollection)[i]->Print();
}
//------------------------------------------------------------------------------------------------------------------------
void BmnMwpc::Reset(){ fPointCollection->Clear(); ResetParameters(); }
//------------------------------------------------------------------------------------------------------------------------
void BmnMwpc::CopyClones(TClonesArray* cl1, TClonesArray* cl2, Int_t offset)
{
	Int_t nEntries = cl1->GetEntriesFast();
	cout << "-I- BmnMwpc: " << nEntries << " entries to add." << endl;
	TClonesArray& clref = *cl2;
	BmnMwpcPoint* oldpoint = NULL;
	
	for(Int_t i=0; i<nEntries; i++) 
	{
		oldpoint = (BmnMwpcPoint*) cl1->At(i);
		Int_t index = oldpoint->GetTrackID() + offset;
		oldpoint->SetTrackID(index);
		new (clref[fPosIndex]) BmnMwpcPoint(*oldpoint);
		fPosIndex++;
	}
	
	cout << "-I- BmnMwpc: " << cl2->GetEntriesFast() << " merged entries."  << endl;
}
//------------------------------------------------------------------------------------------------------------------------
void BmnMwpc::ConstructGeometry() 
{
  TString fileName = GetGeometryFileName();
  
  if ( fileName.EndsWith(".root") ) {
    gLogger->Info(MESSAGE_ORIGIN,
                "Constructing MWPC geometry from ROOT file %s", 
                fileName.Data());
    ConstructRootGeometry();
  }
  else if ( fileName.EndsWith(".geo") ) {
    gLogger->Info(MESSAGE_ORIGIN,
		  "Constructing MWPC geometry from ASCII file %s", 
		  fileName.Data());
    ConstructAsciiGeometry();
  }
  else if ( fileName.EndsWith(".gdml") )
  {
    gLogger->Info(MESSAGE_ORIGIN,
          "Constructing MWPC geometry from GDML file %s",
          fileName.Data());
    ConstructGDMLGeometry();
  }
  else
  {
    gLogger->Fatal(MESSAGE_ORIGIN,
           "Geometry format of MWPC file %s not supported.",
           fileName.Data());
  }
}

// -----   ConstructAsciiGeometry   -------------------------------------------
void BmnMwpc::ConstructAsciiGeometry() {
  
  FairGeoLoader*    geoLoad = FairGeoLoader::Instance();
  FairGeoInterface* geoFace = geoLoad->getGeoInterface();
  BmnMwpcGeo*       MWPCGeo  = new BmnMwpcGeo();
  MWPCGeo->setGeomFile(GetGeometryFileName());
  geoFace->addGeoModule(MWPCGeo);

  Bool_t rc = geoFace->readSet(MWPCGeo);
  if (rc) MWPCGeo->create(geoLoad->getGeoBuilder());
  TList* volList = MWPCGeo->getListOfVolumes();
  // store geo parameter
  FairRun *fRun = FairRun::Instance();
  FairRuntimeDb *rtdb= FairRun::Instance()->GetRuntimeDb();
  BmnMwpcGeoPar* par=(BmnMwpcGeoPar*)(rtdb->getContainer("BmnMwpcGeoPar"));
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

/*
// -----   ConstructGDMLGeometry   -------------------------------------------
void BmnMwpc::ConstructGDMLGeometry()
{
    TGDMLParse parser;
    TGeoVolume* v1 = parser.GDMLReadFile(GetGeometryFileName());

    v1->Dump();

    if (v1 == 0)
        fLogger->Fatal(MESSAGE_ORIGIN, "\033[5m\033[31mFairModule::ConstructGDMLGeometry(): could construct geometry from GDML File!!  \033[0m", GetGeometryFileName().Data());

    TGeoNode* n = v1->GetNode(0);

    n->Dump();

    gGeoManager->cd();

    // add gdml volume to the simulation TGeoManager
    gGeoManager->AddVolume(v1);

    // force rebuilding of voxels
    TGeoVoxelFinder* voxels = v1->GetVoxels();
    if (voxels)
        voxels->SetNeedRebuild();

    //To avoid having different names of the default matrices because we could have get the volume from another
    //TGeoManager, we reset the default matrix name
    //TGeoMatrix* M = n->GetMatrix();
    //M->SetDefaultName();

    //Now we can remove the matrix so that the new geomanager will rebuild it properly
    //gGeoManager->GetListOfMatrices()->Remove(M);
    //TGeoHMatrix* global = gGeoManager->GetHMatrix();
    //gGeoManager->GetListOfMatrices()->Remove(global); //Remove the Identity matrix

    //Now we can add the node to the existing cave
    TGeoVolume* Cave = gGeoManager->GetTopVolume();
    Cave->AddNode(v1, 0, 0);

    // correction from O. Merle: in case of a TGeoVolume (v1) set the material properly
    // Assign medium to the the volume v, this has to be done in all cases:
    // case 1: For CAD converted volumes they have no mediums (only names)
    // case 2: TGeoVolumes, we need to be sure that the material is defined in this session
    FairGeoMedia* Media = FairGeoLoader::Instance()->getGeoInterface()->getMedia();
    FairGeoBuilder* geobuild = FairGeoLoader::Instance()->getGeoBuilder();
    TGeoMedium* med1 = v1->GetMedium();
    med1->Dump();
    if (med1)
    {
      TGeoMaterial* mat1 = v1->GetMaterial();
      TGeoMaterial* newMat = gGeoManager->GetMaterial(mat1->GetName());
      if (newMat == 0)
      {
        cout<<"The Material is not defined in the TGeoManager"<<endl;
        // The Material is not defined in the TGeoManager, we try to create one if we have enough information about it
        FairGeoMedium* FairMedium = Media->getMedium(mat1->GetName());
        if (!FairMedium)
        {
          fLogger->Fatal(MESSAGE_ORIGIN,"Material %s is not defined in ASCII file nor in Root file we Stop creating geometry", mat1->GetName());
          //     FairMedium=new FairGeoMedium(mat1->GetName());
          //      Media->addMedium(FairMedium);
        }

        Int_t nmed = geobuild->createMedium(FairMedium);
        v1->SetMedium(gGeoManager->GetMedium(nmed));
        gGeoManager->SetAllIndex();
      }
      else
      {
        cout<<"Material is already available in the TGeoManager"<<endl;
        // Material is already available in the TGeoManager and we can set it
        TGeoMedium* med2 = gGeoManager->GetMedium(mat1->GetName());
        v1->SetMedium(med2);
      }
    }
    else
    {
      if (strcmp(v1->ClassName(),"TGeoVolumeAssembly") != 0)
      {
        //[R.K.-3.3.08]  // When there is NO material defined, set it to avoid conflicts in Geant
        fLogger->Fatal(MESSAGE_ORIGIN,"The volume  %s  Has no medium information and not an Assembly so we have to quit", v1->GetName());
      }
    }

    // now go through the herachy and set the materials properly, this is important becase the CAD converter
    // produce TGeoVolumes with materials that have only names and no properties
    ExpandNode(gGeoManager->GetTopVolume()->GetNode(gGeoManager->GetTopVolume()->GetNdaughters()-1));


    // Define output ROOT file with dch geometry
    TFile* outfile = new TFile("dch.root", "RECREATE");
    v1->Write();
    outfile->Close();
}
// ----------------------------------------------------------------------------
*/

// -----   ConstructGDMLGeometry   -------------------------------------------
void BmnMwpc::ConstructGDMLGeometry()
{
   TFile *old = gFile;
   TGDMLParse parser;
   TGeoVolume* gdmlTop;

   // Before importing GDML
   Int_t maxInd = gGeoManager->GetListOfMedia()->GetEntries() - 1;

   gdmlTop = parser.GDMLReadFile(GetGeometryFileName());

   // Cheating - reassigning media indices after GDML import (need to fix this in TGDMLParse class!!!)
   //   for (Int_t i=0; i<gGeoManager->GetListOfMedia()->GetEntries(); i++)
   //      gGeoManager->GetListOfMedia()->At(i)->Dump();
   // After importing GDML
   Int_t j = gGeoManager->GetListOfMedia()->GetEntries() - 1;
   Int_t curId;
   TGeoMedium* m;
   do {
      m = (TGeoMedium*)gGeoManager->GetListOfMedia()->At(j);
      curId = m->GetId();
      m->SetId(curId+maxInd);
      j--;
   } while (curId > 1);
   //   LOG(DEBUG) << "====================================================================" << FairLogger::endl;
   //   for (Int_t i=0; i<gGeoManager->GetListOfMedia()->GetEntries(); i++)
   //      gGeoManager->GetListOfMedia()->At(i)->Dump();

   Int_t newMaxInd = gGeoManager->GetListOfMedia()->GetEntries() - 1;

   gGeoManager->GetTopVolume()->AddNode(gdmlTop, 1, 0);
   ExpandNodeForGdml(gGeoManager->GetTopVolume()->GetNode(gGeoManager->GetTopVolume()->GetNdaughters()-1));

   for (Int_t k = maxInd+1; k < newMaxInd+1; k++) {
      TGeoMedium* medToDel = (TGeoMedium*)(gGeoManager->GetListOfMedia()->At(maxInd+1));
      LOG(DEBUG) << "    removing media " << medToDel->GetName() << " with id " << medToDel->GetId() << " (k=" << k << ")" << FairLogger::endl;
      gGeoManager->GetListOfMedia()->Remove(medToDel);
   }
   gGeoManager->SetAllIndex();

   gFile = old;
}

void BmnMwpc::ExpandNodeForGdml(TGeoNode* node)
{
   LOG(DEBUG) << "----------------------------------------- ExpandNodeForGdml for node " << node->GetName() << FairLogger::endl;

   TGeoVolume* curVol = node->GetVolume();

   LOG(DEBUG) << "    volume: " << curVol->GetName() << FairLogger::endl;

   if (curVol->IsAssembly()) {
      LOG(DEBUG) << "    skipping volume-assembly" << FairLogger::endl;
   }
   else
   {
      TGeoMedium* curMed = curVol->GetMedium();
      TGeoMaterial* curMat = curVol->GetMaterial();
      TGeoMedium* curMedInGeoManager = gGeoManager->GetMedium(curMed->GetName());
      TGeoMaterial* curMatOfMedInGeoManager = curMedInGeoManager->GetMaterial();
      TGeoMaterial* curMatInGeoManager = gGeoManager->GetMaterial(curMat->GetName());

      // Current medium and material assigned to the volume from GDML
      LOG(DEBUG2) << "    curMed\t\t\t\t" << curMed << "\t" << curMed->GetName() << "\t" << curMed->GetId() << FairLogger::endl;
      LOG(DEBUG2) << "    curMat\t\t\t\t" << curMat << "\t" << curMat->GetName() << "\t" << curMat->GetIndex() << FairLogger::endl;

      // Medium and material found in the gGeoManager - either the pre-loaded one or one from GDML
      LOG(DEBUG2) << "    curMedInGeoManager\t\t" << curMedInGeoManager
               << "\t" << curMedInGeoManager->GetName() << "\t" << curMedInGeoManager->GetId() << FairLogger::endl;
      LOG(DEBUG2) << "    curMatOfMedInGeoManager\t\t" << curMatOfMedInGeoManager
               << "\t" << curMatOfMedInGeoManager->GetName() << "\t" << curMatOfMedInGeoManager->GetIndex() << FairLogger::endl;
      LOG(DEBUG2) << "    curMatInGeoManager\t\t" << curMatInGeoManager
               << "\t" << curMatInGeoManager->GetName() << "\t" << curMatInGeoManager->GetIndex() << FairLogger::endl;

      TString matName = curMat->GetName();
      TString medName = curMed->GetName();

      if (curMed->GetId() != curMedInGeoManager->GetId()) {
         if (fFixedMedia.find(medName) == fFixedMedia.end()) {
            LOG(DEBUG) << "    Medium needs to be fixed" << FairLogger::endl;
            fFixedMedia[medName] = curMedInGeoManager;
            Int_t ind = curMat->GetIndex();
            gGeoManager->RemoveMaterial(ind);
            LOG(DEBUG) << "    removing material " << curMat->GetName()
               << " with index " << ind << FairLogger::endl;
            for (Int_t i=ind; i<gGeoManager->GetListOfMaterials()->GetEntries(); i++) {
               TGeoMaterial* m = (TGeoMaterial*)gGeoManager->GetListOfMaterials()->At(i);
               m->SetIndex(m->GetIndex()-1);
            }

            LOG(DEBUG) << "    Medium fixed" << FairLogger::endl;
         }
         else
         {
            LOG(DEBUG) << "    Already fixed medium found in the list    " << FairLogger::endl;
         }
      }
      else
      {
         if (fFixedMedia.find(medName) == fFixedMedia.end()) {
            LOG(DEBUG) << "    There is no correct medium in the memory yet" << FairLogger::endl;

            FairGeoLoader* geoLoad = FairGeoLoader::Instance();
            FairGeoInterface* geoFace = geoLoad->getGeoInterface();
            FairGeoMedia* geoMediaBase =  geoFace->getMedia();
            FairGeoBuilder* geobuild = geoLoad->getGeoBuilder();

            FairGeoMedium* curMedInGeo = geoMediaBase->getMedium(medName);
            if (curMedInGeo == 0)
            {
               LOG(FATAL) << "    Media not found in Geo file: " << medName << FairLogger::endl;
               //! This should not happen.
               //! This means that somebody uses material in GDML that is not in the media.geo file.
               //! Most probably this is the sign to the user to check materials' names in the CATIA model.
            }
            else
            {
               LOG(DEBUG) << "    Found media in Geo file" << medName << FairLogger::endl;
               Int_t nmed = geobuild->createMedium(curMedInGeo);
               fFixedMedia[medName] = (TGeoMedium*)gGeoManager->GetListOfMedia()->Last();
               gGeoManager->RemoveMaterial(curMatOfMedInGeoManager->GetIndex());
               LOG(DEBUG) << "    removing material " << curMatOfMedInGeoManager->GetName()
                  << " with index " << curMatOfMedInGeoManager->GetIndex() << FairLogger::endl;
               for (Int_t i=curMatOfMedInGeoManager->GetIndex(); i<gGeoManager->GetListOfMaterials()->GetEntries(); i++) {
                  TGeoMaterial* m = (TGeoMaterial*)gGeoManager->GetListOfMaterials()->At(i);
                  m->SetIndex(m->GetIndex()-1);
               }
            }

            if (curMedInGeo->getSensitivityFlag()) {
               LOG(DEBUG) << "    Adding sensitive  " << curVol->GetName() << FairLogger::endl;
               AddSensitiveVolume(curVol);
            }
         }
         else
         {
            LOG(DEBUG) << "    Already fixed medium found in the list" << FairLogger::endl;
            LOG(DEBUG) << "!!! Sensitivity: " << fFixedMedia[medName]->GetParam(0) << FairLogger::endl;
            if (fFixedMedia[medName]->GetParam(0) == 1) {
               LOG(DEBUG) << "    Adding sensitive  " << curVol->GetName() << FairLogger::endl;
               AddSensitiveVolume(curVol);
            }
         }
      }

      curVol->SetMedium(fFixedMedia[medName]);
      gGeoManager->SetAllIndex();

//      gGeoManager->GetListOfMaterials()->Print();
//      gGeoManager->GetListOfMedia()->Print();

   }

   //! Recursevly go down the tree of nodes
   if (curVol->GetNdaughters() != 0)
   {
      TObjArray* NodeChildList = curVol->GetNodes();
      TGeoNode* curNodeChild;
      for (Int_t j=0; j<NodeChildList->GetEntriesFast(); j++)
      {
         curNodeChild = (TGeoNode*)NodeChildList->At(j);
         ExpandNodeForGdml(curNodeChild);
      }
   }
}

//Check if Sensitive-----------------------------------------------------------
Bool_t BmnMwpc::CheckIfSensitive(std::string name)
{
  TString tsname = name;
  if (tsname.Contains("Active")) {
    return kTRUE;
  }
  return kFALSE;
    
    
//  if(0 == TString(name).CompareTo("MWPCDetV")) {
//    return kTRUE;
//  }
//  return kFALSE;

}
//---------------------------------------------------------  

//------------------------------------------------------------------------------------------------------------------------
BmnMwpcPoint* BmnMwpc::AddHit(Int_t trackID, Int_t detID, TVector3 pos, Double_t radius,
			    TVector3 mom, Double_t time, Double_t length, 
			    Double_t eLoss, Int_t isPrimary, Double_t charge, Int_t pdgId, TVector3 trackPos) 
{
	TClonesArray& clref = *fPointCollection;
	Int_t size = clref.GetEntriesFast();
    //std::cout << "ELoss: " << eLoss << "\n";	
    return new(clref[size]) BmnMwpcPoint(trackID, detID, pos, radius, mom, time, length, eLoss, isPrimary, charge, pdgId, trackPos);
}
//------------------------------------------------------------------------------------------------------------------------

ClassImp(BmnMwpc)
