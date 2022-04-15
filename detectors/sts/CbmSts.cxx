// -------------------------------------------------------------------------
// -----                        CbmSts source file                     -----
// -----                  Created 26/07/04  by V. Friese               -----
// -------------------------------------------------------------------------
#include "CbmSts.h"

#include "CbmGeoSts.h"
#include "CbmStsPoint.h"
#include "CbmGeoStsPar.h"

#include "CbmStsDetectorId.h"
#include "FairGeoInterface.h"
#include "FairGeoLoader.h"
#include "FairGeoNode.h"
#include "FairGeoMedia.h"
#include "FairGeoRootBuilder.h"
#include "FairRootManager.h"
#include "CbmStack.h"
#include "FairRuntimeDb.h"
#include "FairRun.h"
#include "FairVolume.h"

#include "TClonesArray.h"
#include "TGeoMCGeometry.h"
#include "TParticle.h"
#include "TVirtualMC.h"
#include "TObjArray.h"
#include "TGDMLParse.h"
#include "TGeoVoxelFinder.h"
#include "TGeoManager.h"
#include "TRegexp.h"
#include "TFile.h"

#include <iostream>

using std::cout;
using std::cerr;
using std::endl;



// -----   Default constructor   -------------------------------------------
CbmSts::CbmSts()
  : FairDetector("STS", kTRUE, kGEM),
  fTrackID(0),
  fVolumeID(0),
  fDetectorId(0),
  fPosIn(0.,0.,0.),
  fPosOut(0.,0.,0.),
  fMomIn(0.,0.,0.),
  fMomOut(0.,0.,0.),
  fTime(0.),
  fLength(0.),
  fELoss(0.),
  fPosIndex(0),
  fStsCollection(NULL)
{
  ResetParameters();
  fStsCollection = new TClonesArray("CbmStsPoint");
  fPosIndex = 0;
  //kGeoSaved = kFALSE;
  //flGeoPar = new TList();
  //flGeoPar->SetName( GetName());
  fVerboseLevel = 1;
}
// -------------------------------------------------------------------------



// -----   Standard constructor   ------------------------------------------
CbmSts::CbmSts(const char* name, Bool_t active)
  : FairDetector(name, active, kGEM),
  fTrackID(0),
  fVolumeID(0),
  fDetectorId(0),
  fPosIn(0.,0.,0.),
  fPosOut(0.,0.,0.),
  fMomIn(0.,0.,0.),
  fMomOut(0.,0.,0.),
  fTime(0.),
  fLength(0.),
  fELoss(0.),
  fPosIndex(0),
  fStsCollection(NULL)
{
  ResetParameters();
  fStsCollection = new TClonesArray("CbmStsPoint");
  fPosIndex = 0;
  fVerboseLevel = 1;
}
// -------------------------------------------------------------------------



// -----   Destructor   ----------------------------------------------------
CbmSts::~CbmSts() {
  if (fStsCollection) {
    fStsCollection->Delete();
    delete fStsCollection;
  }
}
// -------------------------------------------------------------------------



// -----   Public method ProcessHits  --------------------------------------
Bool_t CbmSts::ProcessHits(FairVolume* vol) {

    // Set parameters at entrance of volume. Reset ELoss.
    if ( gMC->IsTrackEntering() ) {
        fELoss  = 0.;
        fTime   = gMC->TrackTime() * 1.0e09;
        fLength = gMC->TrackLength();

        TLorentzVector PosIn;
        gMC->TrackPosition(PosIn);
        fPosIn.SetXYZ(PosIn.X(), PosIn.Y(), PosIn.Z());

        TLorentzVector MomIn;
        gMC->TrackMomentum(MomIn);
        fMomIn.SetXYZ(MomIn.X(), MomIn.Y(), MomIn.Z());
    }

    // Sum energy loss for all steps in the active volume
    fELoss += gMC->Edep();

    // Set additional parameters at exit of active volume. Create CbmStsPoint.
    if ( gMC->IsTrackExiting()    ||
         gMC->IsTrackStop()       ||
         gMC->IsTrackDisappeared()   ) {

        fTrackID  = gMC->GetStack()->GetCurrentTrackNumber();
        fDetectorId = vol->getMCid();

        TLorentzVector PosOut;
        gMC->TrackPosition(PosOut);
        fPosOut.SetXYZ(PosOut.X(), PosOut.Y(), PosOut.Z());

        TLorentzVector MomOut;
        gMC->TrackMomentum(MomOut);
        fMomOut.SetXYZ(MomOut.X(), MomOut.Y(), MomOut.Z());

        if (fELoss == 0. ) return kFALSE;

        //correction step to avoid the seg. violation error due to invalid memory access
        TVector3 diff_pos = fPosIn - fPosOut;

        if(diff_pos.Mag() < 0.001) return kFALSE; //ignore points produced by tracks with zero length inside the current sens. volume
        if(fMomOut.Mag() == 0) return kFALSE; // ignore points produced by tracks with zero momentum inside the current sens. volume

        TVector3 corr_step = fMomOut;
        corr_step.SetMag(0.001); // 10 um
        TVector3 pos = fPosOut;
        fPosOut = pos - corr_step;
        gGeoManager->FindNode(fPosOut[0],fPosOut[1],fPosOut[2]);

        // Determine station and module numbers for the current hit ------------
        Int_t stationNum = -1; // current station number (default)
        Int_t moduleNum = -1; // current module number (default)

        TGeoVolume *currentVolume = gGeoManager->GetCurrentVolume();
        TString currentVolumeName = currentVolume->GetName();

        TRegexp expr = "^Sensor_module[0-9]+_station[0-9]+$";
        if(currentVolumeName.Contains(expr)) {
            TRegexp mod_expr = "module[0-9]+";
            TRegexp stat_expr = "station[0-9]+";

            moduleNum = TString(TString(currentVolumeName(mod_expr))(TRegexp("[0-9]+"))).Atoi();
            stationNum = TString(TString(currentVolumeName(stat_expr))(TRegexp("[0-9]+"))).Atoi();
        }

        //cout << "stationNum = " << stationNum << "\n";
        //cout << "moduleNum = " << moduleNum << "\n";
        //cout << "\n";

        if(stationNum == -1 || moduleNum == -1) return kFALSE; //check if the current point has incorrect indices
        //----------------------------------------------------------------------

        AddHit(fTrackID, fDetectorId,
               fPosIn, fPosOut,
               fMomIn, fMomOut,
               fTime, fLength, fELoss, stationNum, moduleNum);

        // Increment number of StsPoints for this track
        CbmStack* stack = (CbmStack*) gMC->GetStack();
        stack->AddPoint(kGEM);

        ResetParameters();
  }

  return kTRUE;
}

// -----   Public method EndOfEvent   -----------------------------------------
void CbmSts::BeginEvent() {

}
// -----   Public method EndOfEvent   -----------------------------------------
void CbmSts::EndOfEvent() {

  if (fVerboseLevel) Print();
//  fStsCollection->Clear();
  fStsCollection->Delete();

  ResetParameters();
}
// ----------------------------------------------------------------------------


// -----   Public method Register   -------------------------------------------
void CbmSts::Register() {
  FairRootManager::Instance()->Register("StsPoint", GetName(), fStsCollection, kTRUE);
}
// ----------------------------------------------------------------------------



// -----   Public method GetCollection   --------------------------------------
TClonesArray* CbmSts::GetCollection(Int_t iColl) const {
  if (iColl == 0) return fStsCollection;
  else return NULL;
}
// ----------------------------------------------------------------------------



// -----   Public method Print   ----------------------------------------------
void CbmSts::Print() const {
  Int_t nHits = fStsCollection->GetEntriesFast();
  cout << "-I- CbmSts: " << nHits << " points registered in this event."
       << endl;
}
// ----------------------------------------------------------------------------



// -----   Public method Reset   ----------------------------------------------
void CbmSts::Reset() {
//  fStsCollection->Clear();
  fStsCollection->Delete();
  ResetParameters();
}
// ----------------------------------------------------------------------------



// -----   Public method CopyClones   -----------------------------------------
void CbmSts::CopyClones(TClonesArray* cl1, TClonesArray* cl2, Int_t offset) {
  Int_t nEntries = cl1->GetEntriesFast();
  cout << "-I- CbmSts: " << nEntries << " entries to add." << endl;
  TClonesArray& clref = *cl2;
  CbmStsPoint* oldpoint = NULL;
   for (Int_t i=0; i<nEntries; i++) {
   oldpoint = (CbmStsPoint*) cl1->At(i);
    Int_t index = oldpoint->GetTrackID() + offset;
    oldpoint->SetTrackID(index);
    new (clref[fPosIndex]) CbmStsPoint(*oldpoint);
    fPosIndex++;
  }
   cout << " -I- CbmSts: " << cl2->GetEntriesFast() << " merged entries."
       << endl;
}
// ----------------------------------------------------------------------------



// -----  ConstructGeometry  --------------------------------------------------
void CbmSts::ConstructGeometry()
{
  TString fileName = GetGeometryFileName();
  if ( fileName.EndsWith(".root") ) {
    LOG(info) << "Constructing STS geometry from ROOT file " << fileName.Data();
    ConstructRootGeometry();
  }
  else if ( fileName.EndsWith(".geo") ) {
    LOG(info) << "Constructing STS geometry from ASCII file " << fileName.Data();
    ConstructAsciiGeometry();
  }
  else if ( fileName.EndsWith(".gdml") ) {
    LOG(info) << "Constructing STS geometry from GDML file " << fileName.Data();
    ConstructGDMLGeometry();
  }
  else
    LOG(fatal) << "Geometry format of STS file " << fileName.Data() << " not supported";
}
// ----------------------------------------------------------------------------

// -----   ConstructAsciiGeometry   -------------------------------------------
void CbmSts::ConstructAsciiGeometry() {

  FairGeoLoader*    geoLoad = FairGeoLoader::Instance();
  FairGeoInterface* geoFace = geoLoad->getGeoInterface();
  CbmGeoSts*       stsGeo  = new CbmGeoSts();
  stsGeo->setGeomFile(GetGeometryFileName());
  geoFace->addGeoModule(stsGeo);

  Bool_t rc = geoFace->readSet(stsGeo);
  if (rc) stsGeo->create(geoLoad->getGeoBuilder());
  TList* volList = stsGeo->getListOfVolumes();
  // store geo parameter
  FairRun *fRun = FairRun::Instance();
  FairRuntimeDb *rtdb= FairRun::Instance()->GetRuntimeDb();
  CbmGeoStsPar* par=(CbmGeoStsPar*)(rtdb->getContainer("CbmGeoStsPar"));
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

// -----   ConstructGDMLGeometry   -------------------------------------------
void CbmSts::ConstructGDMLGeometry()
{
    TGDMLParse parser;
    TGeoVolume* v1 = parser.GDMLReadFile(GetGeometryFileName());

    if (v1 == 0)
        LOG(fatal) << "\033[5m\033[31mFairModule::ConstructGDMLGeometry(): could construct geometry from GDML File!!  \033[0m" << GetGeometryFileName().Data();

    TGeoNode* n = v1->GetNode(0);

    gGeoManager->cd();

    /*add gdml volume to the simulation TGeoManager*/
    gGeoManager->AddVolume(v1);

    /*force rebuilding of voxels */
    TGeoVoxelFinder* voxels = v1->GetVoxels();
    if (voxels)
        voxels->SetNeedRebuild();

    /**To avoid having different names of the default matrices because we could have get the volume from another
     * TGeoManager, we reset the default matrix name
     */
    TGeoMatrix* M = n->GetMatrix();
    M->SetDefaultName();

    /*Now we can remove the matrix so that the new geomanager will rebuild it properly*/
    gGeoManager->GetListOfMatrices()->Remove(M);
    TGeoHMatrix* global = gGeoManager->GetHMatrix();
    gGeoManager->GetListOfMatrices()->Remove(global); //Remove the Identity matrix

    /**Now we can add the node to the existing cave */
    TGeoVolume* Cave = gGeoManager->GetTopVolume();
    Cave->AddNode(v1, 0, 0);

    /*
    // correction from O. Merle: in case of a TGeoVolume (v1) set the material properly
    // Assign medium to the the volume v, this has to be done in all cases:
    // case 1: For CAD converted volumes they have no mediums (only names)
    // case 2: TGeoVolumes, we need to be sure that the material is defined in this session
    FairGeoMedia* Media = FairGeoLoader::Instance()->getGeoInterface()->getMedia();
    FairGeoBuilder* geobuild = FairGeoLoader::Instance()->getGeoBuilder();
    TGeoMedium* med1 = v1->GetMedium();
    if (med1)
    {
      TGeoMaterial* mat1 = v1->GetMaterial();
      TGeoMaterial* newMat = gGeoManager->GetMaterial(mat1->GetName());
      if (newMat == 0)
      {
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
    }*/

    // now go through the herachy and set the materials properly, this is important becase the CAD converter
    // produce TGeoVolumes with materials that have only names and no properties
    ExpandNode(n);

    // Define output ROOT file with sts geometry
    TFile* outfile = new TFile("sts.root", "RECREATE");
    v1->Write();
    outfile->Close();
}
// ----------------------------------------------------------------------------

// -----   CheckIfSensitive   -------------------------------------------------
Bool_t CbmSts::CheckIfSensitive(std::string name) {

  TString volName = name;
  if ( volName.Contains("Sensor") ) return kTRUE;
  return kFALSE;
}
// ----------------------------------------------------------------------------


// -----   Private method AddHit   --------------------------------------------
CbmStsPoint* CbmSts::AddHit(Int_t trackID, Int_t detID, TVector3 posIn,
			    TVector3 posOut, TVector3 momIn,
			    TVector3 momOut, Double_t time,
			    Double_t length, Double_t eLoss,
                            Int_t stationNum, Int_t moduleNum) {
  TClonesArray& clref = *fStsCollection;
  Int_t size = clref.GetEntriesFast();
  if (fVerboseLevel>1)
    cout << "-I- CbmSts: Adding Point at (" << posIn.X() << ", " << posIn.Y()
	 << ", " << posIn.Z() << ") cm,  detector " << detID << ", track "
	 << trackID << ", energy loss " << eLoss*1e06 << " keV" << endl;

//  return new(clref[size]) CbmStsPoint(trackID, detID, posIn, posOut,
//				      momIn, momOut, time, length, eLoss);
  new(clref[size]) CbmStsPoint(trackID, detID, posIn, posOut,
                               momIn, momOut, time, length, eLoss);

  CbmStsPoint *hit = (CbmStsPoint*) clref.At(clref.GetEntriesFast() - 1);
  hit->SetStation(stationNum);
  hit->SetModule(moduleNum);
  return hit;
}
// ----------------------------------------------------------------------------


ClassImp(CbmSts)
