// -------------------------------------------------------------------------
// -----                  BmnTrackDrawH source file                  -----
// -----            created 05/10/15 by K. Gertsenberger               -----
// ----- class to visualize GlobalTracks from *.root in EventDisplay   -----
// -------------------------------------------------------------------------

#include "BmnTrackDrawH.h"
#include "BmnTrack.h"
#include "FairHit.h"
#include "MpdEventManagerEditor.h"
#include "FairLogger.h"

#include "TEveManager.h"
#include "TEvePathMark.h"
#include "TEveVector.h"
#include "TDatabasePDG.h"

#include <iostream>
using namespace std;

// default constructor
BmnTrackDrawH::BmnTrackDrawH()
  : FairTask("BmnTrackDrawH", 0),
    fTrackList(NULL),
    fHitsBranchName(""),
    fHitList(NULL),
    fTrPr(NULL),
    fEventManager(NULL),
    fEveTrList(NULL),
    fTrList(NULL),
    MinEnergyLimit(-1.),
    MaxEnergyLimit(-1.),
    PEnergy(-1.)
{
}

// standard constructor
BmnTrackDrawH::BmnTrackDrawH(const char* name, TString hitsBranchName, Int_t iVerbose)
  : FairTask(name, iVerbose),
    fTrackList(NULL),
    fHitsBranchName(hitsBranchName),
    fHitList(NULL),
    fTrPr(NULL),
    fEventManager(NULL),
    fEveTrList(new TObjArray(16)),
    fTrList(NULL),
    MinEnergyLimit(-1.),
    MaxEnergyLimit(-1.),
    PEnergy(-1.)
{
}

// initialization of the track drawing task
InitStatus BmnTrackDrawH::Init()
{
    if (fVerbose > 0) cout<<"BmnTrackDrawH::Init()"<<endl;

    fEventManager = MpdEventManager::Instance();
    if (fVerbose > 1) cout<<"BmnTrackDrawH::Init() get instance of EventManager: "<<fEventManager<<endl;

    FairRootManager* fManager = FairRootManager::Instance();
    if (fVerbose > 1) cout<<"BmnTrackDrawH::Init() get instance of FairRootManager: "<<fManager<<endl;

    fTrackList = (TClonesArray*) fManager->GetObject(GetName());
    if(fTrackList == 0)
    {
        LOG(error)<<"BmnTrackDrawH::Init() branch "<<GetName()<<" not found! Task will be deactivated";
        SetActive(kFALSE);
    }
    if (fVerbose > 1) cout<<"BmnTrackDrawH::Init() get track list "<<fTrackList<<" from branch '"<<GetName()<<"'"<<endl;

    fHitList = (TClonesArray*) fManager->GetObject(fHitsBranchName);
    if(fHitList == 0)
    {
        LOG(error)<<"BmnTrackDrawH::Init()  branch "<<fHitsBranchName<<" not found! Task will be deactivated";
        SetActive(kFALSE);
    }
    if (fVerbose > 1) cout<<"BmnTrackDrawH::Init() get list of hits "<<fHitList<<" from branch '"<<fHitsBranchName<<"'"<<endl;

    MinEnergyLimit = fEventManager->GetEvtMinEnergy();
    MaxEnergyLimit = fEventManager->GetEvtMaxEnergy();
    PEnergy = 0;

    return kSUCCESS;
}

// -------------------------------------------------------------------------
void BmnTrackDrawH::Exec(Option_t* option)
{
    if (fVerbose > 0) cout<<"BmnTrackDrawH::Exec() "<<endl;

    if (!IsActive())
        return;

    Reset();

    if (fVerbose > 0) cout<<"BmnTrackDrawH::Exec(): the number of tracks is "<<fTrackList->GetEntriesFast()<<endl;

    BmnTrack* current_track;
    for (Int_t i = 0; i < fTrackList->GetEntriesFast(); i++)
    {
        if (fVerbose > 2) cout<<"BmnTrackDrawH::Exec() "<<i<<endl;

        current_track = (BmnTrack*) fTrackList->At(i);
        const FairTrackParam* pParamFirst = current_track->GetParamFirst();

        // define whether track is primary
        bool isPrimary = ( (TMath::Abs(pParamFirst->GetX())<10) && (TMath::Abs(pParamFirst->GetY())<10) && (TMath::Abs(pParamFirst->GetZ())<10) );

        // skip secondary tracks if primary flag is set
        if (fEventManager->IsPriOnly() && (!isPrimary))
            continue;

        // get PDG particle code, without identification - Rootino
        int particlePDG = 0;

        // get momentum
        TVector3 mom;
        pParamFirst->Momentum(mom);
        Double_t px = mom.X(), py = mom.Y(), pz = mom.Z();

        // create particle
        TParticlePDG* fParticlePDG = TDatabasePDG::Instance()->GetParticle(particlePDG);
        TParticle* P = new TParticle(particlePDG, i, -1, -1, -1, -1, px, py, pz, 0, pParamFirst->GetX(), pParamFirst->GetY(), pParamFirst->GetZ(), 0);

        // get EVE track list for this particle
        fTrList = GetTrGroup(P);
        // create EVE track corresponding global track
        TEveTrack* track = new TEveTrack(P, particlePDG, fTrPr);
        // set line color corresponding PDG particle code
        track->SetLineColor(fEventManager->Color(particlePDG));
        track->SetLineWidth(2);

        Int_t Np = current_track->GetNHits();

        // cycle: add hits (points) to EVE path for this track
        if (fVerbose > 2) cout<<"BmnTrackDrawH::Exec(): number of track hits = "<<Np<<endl;
        for (Int_t n = 0; n < Np; n++)
        {
            FairHit* pHit = NULL;
            pHit = (FairHit*) fHitList->UncheckedAt(current_track->GetHitIndex(n));

            track->SetPoint(n, pHit->GetX(), pHit->GetY(), pHit->GetZ());

            TEvePathMark* path = new TEvePathMark();
            TEveVector pos = TEveVector(pHit->GetX(), pHit->GetY(), pHit->GetZ());
            if (fVerbose > 3) cout<<"BmnTrackDrawH::Exec(): point "<<n<<": X="<<pHit->GetX()<<" Y="<<pHit->GetY()<<" Z="<<pHit->GetZ()<<endl;
            path->fV = pos;
            path->fTime = pHit->GetTimeStamp();
            if (n == 0)
            {
                TEveVector Mom = TEveVector(px, py, pz);
                path->fP = Mom;
            }

            // add path marker for current EVE track
            track->AddPathMark(*path);

            if (fVerbose > 3) cout<<"BmnTrackDrawH::Exec: path marker added "<<path<<endl;
        }

        // add track to EVE track list
        fTrList->AddElement(track);

        if (fVerbose > 2) cout<<"BmnTrackDrawH::Exec: track was added "<<track->GetName()<<endl;
    }

    // redraw EVE scenes
    //gEve->Redraw3D(kFALSE);
}

// destructor
BmnTrackDrawH::~BmnTrackDrawH()
{
}

void BmnTrackDrawH::SetParContainers()
{
}

void BmnTrackDrawH::Finish()
{
}

void BmnTrackDrawH::Reset()
{
    // clear EVE track lists (fEveTrList)
    for (Int_t i = 0; i < fEveTrList->GetEntriesFast(); i++)
    {
        TEveTrackList*  ele = (TEveTrackList*) fEveTrList->At(i);
        gEve->RemoveElement(ele, fEventManager->EveRecoTracks);
    }
    fEveTrList->Clear();
}

// return pointer to EVE track list for given particle name. if list don't exist then create it
TEveTrackList* BmnTrackDrawH::GetTrGroup(TParticle* P)
{
    fTrList = 0;

    // search if there us existing track list for this particle (with given name)
    for (Int_t i = 0; i < fEveTrList->GetEntriesFast(); i++)
    {
        TEveTrackList* TrListIn = (TEveTrackList*) fEveTrList->At(i);
        if (strcmp(TrListIn->GetName(), P->GetName()) == 0)
        {
            fTrList = TrListIn;
            break;
        }
    }

    // create new track list for new particle's name
    if (fTrList == 0)
    {
        fTrPr = new TEveTrackPropagator();
        fTrList = new  TEveTrackList(P->GetName(), fTrPr);
        // set track color by particle PDG from MpdEventManager
        fTrList->SetMainColor(fEventManager->Color(P->GetPdgCode()));
        fEveTrList->Add(fTrList);
        fTrList->SetRnrLine(kTRUE);

        fEventManager->AddEventElement(fTrList, RecoTrackList);
    }

    return fTrList;
}

ClassImp(BmnTrackDrawH)
