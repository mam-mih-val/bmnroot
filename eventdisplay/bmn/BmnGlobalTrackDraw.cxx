// -------------------------------------------------------------------------
// -----                  BmnGlobalTrackDraw source file                     -----
// -----            created 10/12/13 by K. Gertsenberger               -----
// ----- class to visualize reconstructed GlobalTracks in EventDisplay -----
// -------------------------------------------------------------------------

#include "BmnGlobalTrackDraw.h"
#include "BmnGlobalTrack.h"
#include "BmnGemTrack.h"
#include "MpdEventManagerEditor.h"
#include "FairLogger.h"

#include "TEveManager.h"
#include "TEvePathMark.h"
#include "TEveVector.h"
#include "TDatabasePDG.h"

#include <iostream>
using namespace std;

// default constructor
BmnGlobalTrackDraw::BmnGlobalTrackDraw()
  : FairTask("BmnGlobalTrackDraw", 0),
    fTrackList(NULL),
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
BmnGlobalTrackDraw::BmnGlobalTrackDraw(const char* name, Int_t iVerbose)
  : FairTask(name, iVerbose),
    fTrackList(NULL),
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
InitStatus BmnGlobalTrackDraw::Init()
{
    if (fVerbose > 0) cout<<"BmnGlobalTrackDraw::Init()"<<endl;

    FairRootManager* fManager = FairRootManager::Instance();

    fTrackList = (TClonesArray*) fManager->GetObject(GetName());
    if (fTrackList == 0)
    {
        LOG(ERROR)<<"BmnGlobalTrackDraw::Init()  branch GlobalTrack not found! Task will be deactivated";
        SetActive(kFALSE);
        return kERROR;
    }
    if (fVerbose > 1) cout<<"BmnGlobalTrackDraw::Init() get track list "<<fTrackList<<endl;

    fGemTrackList = (TClonesArray*) fManager->GetObject("BmnGemTrack");
    fGemHitList = (TClonesArray*) fManager->GetObject("BmnGemStripHit");

    fTof1HitList = (TClonesArray*) fManager->GetObject("BmnTof1Hit");
    fTof2HitList = (TClonesArray*) fManager->GetObject("BmnTofHit");
    fDchHitList = (TClonesArray*) fManager->GetObject("BmnDchHit");

    fEventManager = MpdEventManager::Instance();
    if (fVerbose > 1) cout<<"BmnGlobalTrackDraw::Init() get instance of MpdEventManager "<<endl;

    MinEnergyLimit = fEventManager->GetEvtMinEnergy();
    MaxEnergyLimit = fEventManager->GetEvtMaxEnergy();
    PEnergy = 0;

    return kSUCCESS;
}
// -------------------------------------------------------------------------
void BmnGlobalTrackDraw::Exec(Option_t* option)
{
    if (!IsActive())
        return;

    if (fVerbose > 1) cout<<" BmnGlobalTrackDraw::Exec "<<endl;

    Reset();

    BmnGlobalTrack* tr;
    //cout<<"fTrackList->GetEntriesFast(): "<<fTrackList->GetEntriesFast()<<". fTrackList->GetEntries(): "<<fTrackList->GetEntries()<<endl;
    for (Int_t i = 0; i < fTrackList->GetEntriesFast(); i++)
    {
        if (fVerbose > 1) cout<<"BmnGlobalTrackDraw::Exec "<<i<<endl;

        tr = (BmnGlobalTrack*) fTrackList->At(i);
        const FairTrackParam* pParamFirst = tr->GetParamFirst();

        // define whether track is primary
        bool isPrimary = ( (TMath::Abs(pParamFirst->GetX()) < 10) && (TMath::Abs(pParamFirst->GetY()) < 10) && (TMath::Abs(pParamFirst->GetZ()) < 10) );

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

        // get GEM track for global track
        BmnGemTrack* pGemTrack = (BmnGemTrack*) fGemTrackList->UncheckedAt(tr->GetGemTrackIndex());
        Int_t Np = pGemTrack->GetNHits();

        // cycle: add hits (points) to EVE path for this track
        Int_t n;
        for (n = 0; n < Np; n++)
        {
            FairHit* pHit = (FairHit*) fGemHitList->UncheckedAt(pGemTrack->GetHitIndex(n));

            track->SetPoint(n, pHit->GetX(), pHit->GetY(), pHit->GetZ());

            TEvePathMark* path = new TEvePathMark();
            TEveVector pos = TEveVector(pHit->GetX(), pHit->GetY(), pHit->GetZ());
            path->fV = pos;
            path->fTime = pHit->GetTimeStamp();
            if (n == 0)
            {
                TEveVector Mom = TEveVector(px, py, pz);
                path->fP = Mom;
            }

            // add path marker for current EVE track
            track->AddPathMark(*path);

            if (fVerbose > 2) cout<<"Path marker added "<<path<<endl;
        }

        // add TOF1 hit
        if (tr->GetTof1HitIndex() > -1)
        {
            FairHit* pHit = (FairHit*) fTof1HitList->UncheckedAt(tr->GetTof1HitIndex());

            track->SetPoint(n, pHit->GetX(), pHit->GetY(), pHit->GetZ());

            TEvePathMark* path = new TEvePathMark();
            TEveVector pos = TEveVector(pHit->GetX(), pHit->GetY(), pHit->GetZ());
            path->fV = pos;
            path->fTime = pHit->GetTimeStamp();
            if (n == 0)
            {
                TEveVector Mom = TEveVector(px, py, pz);
                path->fP = Mom;
            }

            // add path marker for current EVE track
            track->AddPathMark(*path);

            if (fVerbose > 3)
                cout<<"Path marker added "<<path<<endl;

            n++;
        }


        // add DCH hit
        // if (tr->GetDch1HitIndex() > -1)
        // {
        //     FairHit* pHit = (FairHit*) fDchHitList->UncheckedAt(tr->GetDch1HitIndex());

        //     track->SetPoint(n, pHit->GetX(), pHit->GetY(), pHit->GetZ());

        //     TEvePathMark* path = new TEvePathMark();
        //     TEveVector pos = TEveVector(pHit->GetX(), pHit->GetY(), pHit->GetZ());
        //     path->fV = pos;
        //     path->fTime = pHit->GetTimeStamp();
        //     if (n == 0)
        //     {
        //         TEveVector Mom = TEveVector(px, py, pz);
        //         path->fP = Mom;
        //     }

        //     // add path marker for current EVE track
        //     track->AddPathMark(*path);

        //     if (fVerbose > 3)
        //         cout<<"Path marker added "<<path<<endl;

        //     n++;
        // }

        // add TOF2 hit
        if (tr->GetTof2HitIndex() > -1)
        {
            FairHit* pHit = (FairHit*) fTof2HitList->UncheckedAt(tr->GetTof2HitIndex());

            track->SetPoint(n, pHit->GetX(), pHit->GetY(), pHit->GetZ());

            TEvePathMark* path = new TEvePathMark();
            TEveVector pos = TEveVector(pHit->GetX(), pHit->GetY(), pHit->GetZ());
            path->fV = pos;
            path->fTime = pHit->GetTimeStamp();
            if (n == 0)
            {
                TEveVector Mom = TEveVector(px, py, pz);
                path->fP = Mom;
            }

            // add path marker for current EVE track
            track->AddPathMark(*path);

            if (fVerbose > 3)
                cout<<"Path marker added "<<path<<endl;

            n++;
        }

        // add track to EVE track list
        fTrList->AddElement(track);

        if (fVerbose > 3)
            cout<<"track added "<<track->GetName()<<endl;
    }

    // redraw EVE scenes
    gEve->Redraw3D(kFALSE);
}

// destructor
BmnGlobalTrackDraw::~BmnGlobalTrackDraw()
{
}

void BmnGlobalTrackDraw::SetParContainers()
{
}

void BmnGlobalTrackDraw::Finish()
{
}

void BmnGlobalTrackDraw::Reset()
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
TEveTrackList* BmnGlobalTrackDraw::GetTrGroup(TParticle* P)
{
    fTrList = 0;

    // serch if there us existing track list for this particle (with given name)
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
        fTrList->SetMainColor(fEventManager->Color(P->GetPdgCode()));
        fEveTrList->Add(fTrList);
        fTrList->SetRnrLine(kTRUE);

        fEventManager->AddEventElement(fTrList, RecoTrackList);
    }

    return fTrList;
}

ClassImp(BmnGlobalTrackDraw)
