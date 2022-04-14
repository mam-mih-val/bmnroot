#include "BmnTOF1.h"
#include "BmnTOF1Geo.h"
#include "BmnTOF1GeoPar.h"

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
BmnTOF1::BmnTOF1()
 : fPosIndex(0), nan(-10000000)
{
	fTofCollection = new TClonesArray("BmnTOF1Point");
	fVerboseLevel = 1;	
}

//--------------------------------------------------------------------------------------------------------------------------------------
BmnTOF1::BmnTOF1(const char* name, Bool_t active)
  : FairDetector(name, active), fPosIndex(0), nan(-10000000)
{
	fTofCollection = new TClonesArray("BmnTOF1Point"); 
	fVerboseLevel = 1;
}

//--------------------------------------------------------------------------------------------------------------------------------------
BmnTOF1::~BmnTOF1() 
{
	fTofCollection->Delete();
	delete fTofCollection;
}

//--------------------------------------------------------------------------------------------------------------------------------------
void BmnTOF1::Initialize()
{
    FairDetector::Initialize();
    FairRun* sim = FairRun::Instance();
    FairRuntimeDb* rtdb = sim->GetRuntimeDb();
}

//--------------------------------------------------------------------------------------------------------------------------------------
void BmnTOF1::BeginEvent()
{
}

//--------------------------------------------------------------------------------------------------------------------------------------
Bool_t BmnTOF1::ProcessHits(FairVolume* vol) 
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

    // Sum energy loss for all steps in the active volume
    fELoss += gMC->Edep();
	
    // Create TofPoint at exit of active volume
    if (fELoss > 0 &&  (gMC->IsTrackExiting() || gMC->IsTrackStop() || gMC->IsTrackDisappeared()))
    {
        Int_t fTrackID = gMC->GetStack()->GetCurrentTrackNumber();
        assert(fTrackID>=0);

        Int_t region=0, detector, strip;
        gMC->CurrentVolOffID(0, strip);
        gMC->CurrentVolOffID(1, detector);
			
        Int_t fVolumeID = BmnTOF1Point::GetVolumeUID(region, detector, strip);

        AddPoint(fTrackID, fVolumeID, TVector3(fPos.X(),  fPos.Y(),  fPos.Z()), TVector3(fMom.Px(), fMom.Py(), fMom.Pz()), fTime, fLength, fELoss);
        ((CbmStack*)gMC->GetStack())->AddPoint(kTOF1);

        ResetParameters();
    }

    return kTRUE;
}

//--------------------------------------------------------------------------------------------------------------------------------------
void BmnTOF1::EndOfEvent()
{
    if (fVerboseLevel)  Print();
    Reset();
}

//--------------------------------------------------------------------------------------------------------------------------------------
void BmnTOF1::Register()
{
    FairRootManager::Instance()->Register("TOF400Point","TOF1", fTofCollection, kTRUE);
}

//--------------------------------------------------------------------------------------------------------------------------------------
TClonesArray* BmnTOF1::GetCollection(Int_t iColl) const
{
    if (iColl == 0) return fTofCollection;

    return nullptr;
}

//--------------------------------------------------------------------------------------------------------------------------------------
void BmnTOF1::Print() const
{
    Int_t nHits = fTofCollection->GetEntriesFast();
    cout<<"-I- BmnTOF1: "<<nHits<<" points registered in this event.\n";

    if (fVerboseLevel > 1) for (Int_t i=0; i<nHits; i++) (*fTofCollection)[i]->Print();
}

//--------------------------------------------------------------------------------------------------------------------------------------
void BmnTOF1::Reset()
{
    fTofCollection->Delete();
    fPosIndex = 0;
}

//--------------------------------------------------------------------------------------------------------------------------------------
// guarda in FairRootManager::CopyClones
void BmnTOF1::CopyClones(TClonesArray* cl1, TClonesArray* cl2, Int_t offset)
{
    Int_t nEntries = cl1->GetEntriesFast();
    //cout << "-I- BmnTOF1: " << nEntries << " entries to add." << endl;
    TClonesArray& clref = *cl2;
    BmnTOF1Point* oldpoint = NULL;

    for (Int_t i=0; i<nEntries; i++)
    {
        oldpoint = (BmnTOF1Point*) cl1->At(i);
        Int_t index = oldpoint->GetTrackID() + offset;
        oldpoint->SetTrackID(index);
        new (clref[fPosIndex]) BmnTOF1Point(*oldpoint);
        fPosIndex++;
    }

    cout<<" -I- BmnTOF1: "<<cl2->GetEntriesFast()<<" merged entries."<<endl;
}

//--------------------------------------------------------------------------------------------------------------------------------------
void BmnTOF1::ConstructGeometry()
{
    TString fileName = GetGeometryFileName();
    if (fileName.EndsWith(".root"))
    {
        LOG(info) << "Constructing TOF1 geometry from ROOT file " << fileName.Data();
        ConstructRootGeometry();
    }
    else if (fileName.EndsWith(".geo"))
    {
        LOG(info) << "Constructing TOF1 geometry from ASCII file " << fileName.Data();
        ConstructAsciiGeometry();
    }
    else LOG(fatal) << "Geometry format of TOF1 file " << fileName.Data() << " not supported.";
}

//--------------------------------------------------------------------------------------------------------------------------------------
void BmnTOF1::ConstructAsciiGeometry()
{
    FairGeoLoader*    geoLoad = FairGeoLoader::Instance();
    FairGeoInterface* geoFace = geoLoad->getGeoInterface();
    BmnTOF1Geo*       TOF1Geo = new BmnTOF1Geo();
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
Bool_t BmnTOF1::CheckIfSensitive(std::string name)
{
    TString tsname = name;
    if (tsname.Contains("Active")) return kTRUE;
  
    return kFALSE;
}

//--------------------------------------------------------------------------------------------------------------------------------------
BmnTOF1Point* BmnTOF1::AddPoint(Int_t trackID, Int_t detID, TVector3 pos, TVector3 mom, Double_t time, Double_t length, Double_t eLoss)
{
    return new( (*fTofCollection)[fTofCollection->GetEntriesFast()] ) BmnTOF1Point(trackID, detID, pos, mom, time, length, eLoss);
}

ClassImp(BmnTOF1)
