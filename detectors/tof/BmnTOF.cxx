#include "BmnTOF.h"
#include "BmnTOFGeo.h"
#include "BmnTOFGeoPar.h"

#include "FairGeoInterface.h"
#include "FairGeoLoader.h"
#include "FairGeoNode.h"
#include "FairRootManager.h"
#include "FairRun.h"
#include "FairLogger.h"
#include "FairRuntimeDb.h"

#include "CbmStack.h"

#include<TVirtualMC.h>
#include<TObjArray.h>

#include <iostream>
#include <assert.h>
using namespace std;

//--------------------------------------------------------------------------------------------------------------------------------------
BmnTOF::BmnTOF() 
 : fPosIndex(0), nan(-10000000)
{
	fTofCollection = new TClonesArray("BmnTOFPoint");
	fVerboseLevel = 1;	
}

//--------------------------------------------------------------------------------------------------------------------------------------
BmnTOF::BmnTOF(const char* name, Bool_t active)
  : FairDetector(name, active), fPosIndex(0), nan(-10000000)
{
	fTofCollection = new TClonesArray("BmnTOFPoint"); 
	fVerboseLevel = 1;
}

//--------------------------------------------------------------------------------------------------------------------------------------
BmnTOF::~BmnTOF() 
{
	fTofCollection->Delete();
	delete fTofCollection;
}

//--------------------------------------------------------------------------------------------------------------------------------------
void BmnTOF::Initialize()
{
    FairDetector::Initialize();
    FairRun* sim = FairRun::Instance();
    FairRuntimeDb* rtdb = sim->GetRuntimeDb();
}

//--------------------------------------------------------------------------------------------------------------------------------------
void BmnTOF::BeginEvent()
{
}

//--------------------------------------------------------------------------------------------------------------------------------------
Bool_t BmnTOF::ProcessHits(FairVolume* vol) 
{
    // Set parameters at entrance of volume. Reset ELoss.
    if (gMC->IsTrackEntering())
    {
        fELoss  = 0.;
        fTime   = gMC->TrackTime() * 1.0e09;
        fLength = gMC->TrackLength();
        gMC->TrackPosition(fPos);
        gMC->TrackMomentum(fMom);
    }
//  cout<<"-I- BmnTOF: point X = " << fPos.X() << " Y = " << fPos.Y() << " Z = " << fPos.Z() << "\n";

    // Sum energy loss for all steps in the active volume
    fELoss += gMC->Edep();
	
    // Create TofPoint at exit of active volume
    if(fELoss > 0 &&  (gMC->IsTrackExiting() || gMC->IsTrackStop() || gMC->IsTrackDisappeared()) )
    {
        Int_t fTrackID = gMC->GetStack()->GetCurrentTrackNumber();
        assert(fTrackID >= 0);

        Int_t  region = 0, detector = 0, strip = 0;
//      Int_t  mod = 0;
        gMC->CurrentVolOffID(0, strip);
//      gMC->CurrentVolOffID(1, mod);
        gMC->CurrentVolOffID(2, detector);
//      cout<<"-I- BmnTOF: strip = " << strip << " chamber " << detector << " mod " << mod << "\n";
			
        Int_t fVolumeID = BmnTOFPoint::GetVolumeUID(region, detector, strip);

        AddPoint(fTrackID, fVolumeID, TVector3(fPos.X(),  fPos.Y(),  fPos.Z()), TVector3(fMom.Px(), fMom.Py(), fMom.Pz()), fTime, fLength, fELoss);
        ((CbmStack*)gMC->GetStack())->AddPoint(kTOF);

        ResetParameters();
    }

    return kTRUE;
}

//--------------------------------------------------------------------------------------------------------------------------------------
void BmnTOF::EndOfEvent()
{
    if (fVerboseLevel)  Print();
    Reset();
}

//--------------------------------------------------------------------------------------------------------------------------------------
void BmnTOF::Register()
{
    FairRootManager::Instance()->Register("TOF700Point","TOF", fTofCollection, kTRUE);
}

//--------------------------------------------------------------------------------------------------------------------------------------
TClonesArray* BmnTOF::GetCollection(Int_t iColl) const
{
    if (iColl == 0) return fTofCollection;

    return nullptr;
}

//--------------------------------------------------------------------------------------------------------------------------------------
void BmnTOF::Print() const
{
    Int_t nHits = fTofCollection->GetEntriesFast();
    cout<<"-I- BmnTOF: "<<nHits<<" points registered in this event.\n";

    if (fVerboseLevel > 1) for (Int_t i=0; i<nHits; i++) (*fTofCollection)[i]->Print();
}

//--------------------------------------------------------------------------------------------------------------------------------------
void BmnTOF::Reset()
{
	fTofCollection->Delete();
	fPosIndex = 0;
}

//--------------------------------------------------------------------------------------------------------------------------------------
// guarda in FairRootManager::CopyClones
void BmnTOF::CopyClones(TClonesArray* cl1, TClonesArray* cl2, Int_t offset )
{
    Int_t nEntries = cl1->GetEntriesFast();
    //cout << "-I- BmnTOF: " << nEntries << " entries to add." << endl;
    TClonesArray& clref = *cl2;
    BmnTOFPoint* oldpoint = NULL;

    for (Int_t i=0; i<nEntries; i++)
    {
        oldpoint = (BmnTOFPoint*) cl1->At(i);
        Int_t index = oldpoint->GetTrackID() + offset;
        oldpoint->SetTrackID(index);
        new (clref[fPosIndex]) BmnTOFPoint(*oldpoint);
        fPosIndex++;
    }

    cout<<" -I- BmnTOF: "<<cl2->GetEntriesFast()<<" merged entries."<< endl;
}

//--------------------------------------------------------------------------------------------------------------------------------------
void BmnTOF::ConstructGeometry()
{
    TString fileName = GetGeometryFileName();
    if (fileName.EndsWith(".root"))
    {
        LOG(info) << "Constructing TOF geometry from ROOT file " << fileName.Data();
        ConstructRootGeometry();
    }
    else if ( fileName.EndsWith(".geo") )
    {
        LOG(info) << "Constructing TOF geometry from ASCII file " << fileName.Data();
        ConstructAsciiGeometry();
    }
    else LOG(fatal) << "Geometry format of TOF file " << fileName.Data() << " not supported.";
}

//--------------------------------------------------------------------------------------------------------------------------------------
void BmnTOF::ConstructAsciiGeometry()
{
    FairGeoLoader*    geoLoad = FairGeoLoader::Instance();
    FairGeoInterface* geoFace = geoLoad->getGeoInterface();
    BmnTOFGeo*        TOFGeo  = new BmnTOFGeo();
    TOFGeo->setGeomFile(GetGeometryFileName());
    geoFace->addGeoModule(TOFGeo);

    Bool_t rc = geoFace->readSet(TOFGeo);
    if (rc) TOFGeo->create(geoLoad->getGeoBuilder());
    TList* volList = TOFGeo->getListOfVolumes();

    // store geo parameter
    FairRun *fRun = FairRun::Instance();
    FairRuntimeDb *rtdb = FairRun::Instance()->GetRuntimeDb();
    BmnTOFGeoPar* par = (BmnTOFGeoPar*)(rtdb->getContainer("BmnTOFGeoPar"));
    TObjArray *fSensNodes = par->GetGeoSensitiveNodes();
    TObjArray *fPassNodes = par->GetGeoPassiveNodes();

    TListIter iter(volList);
    FairGeoNode *node = nullptr;
    FairGeoVolume *aVol = nullptr;

    while ( (node = (FairGeoNode*)iter.Next()) )
    {
        aVol = dynamic_cast<FairGeoVolume*>(node);

        if(node->isSensitive())		fSensNodes->AddLast(aVol);
        else                        fPassNodes->AddLast(aVol);
    }

    par->setChanged();
    par->setInputVersion(fRun->GetRunId(),1);
    ProcessNodes( volList );
}

//--------------------------------------------------------------------------------------------------------------------------------------
Bool_t 	BmnTOF::CheckIfSensitive(std::string name)
{
    TString tsname = name;
    // cout<<"-I- BmnTOF: "<<name<<" volume.\n";
    if (tsname.Contains("Active")) return kTRUE;

    return kFALSE;
}

//--------------------------------------------------------------------------------------------------------------------------------------
BmnTOFPoint* BmnTOF::AddPoint(Int_t trackID, Int_t detID, TVector3 pos, TVector3 mom, Double_t time, Double_t length, Double_t eLoss)
{
    return new( (*fTofCollection)[fTofCollection->GetEntriesFast()] ) BmnTOFPoint(trackID, detID, pos, mom, time, length, eLoss);
}

ClassImp(BmnTOF)
