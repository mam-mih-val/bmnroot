// -------------------------------------------------------------------------
// -----                  BmnGlobalTrackDraw source file                     -----
// -----            created 10/12/13 by K. Gertsenberger               -----
// ----- class to visualize reconstructed GlobalTracks in EventDisplay -----
// -------------------------------------------------------------------------

#include "BmnGlobalTrackDraw.h"
#include "BmnGlobalTrack.h"
#include "BmnGemTrack.h"
#include "BmnDchTrack.h"
#include "BmnSiliconTrack.h"
#include "BmnTrack.h"
#include "MpdEventManagerEditor.h"
#include "FairLogger.h"
#include "TEveManager.h"
#include "TEvePathMark.h"
#include "TEveVector.h"
#include "TDatabasePDG.h"
#include "TGeoManager.h"

#include <iostream>
using namespace std;

enum Detectors //KG MWPC?
{
    SILICON,
    GEMS,
    CSC1,
    CSC2,
    TOF1,
    TOF2,
    DCH,
    DCH1,
    DCH2
};

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
BmnGlobalTrackDraw::BmnGlobalTrackDraw(const char *name, Int_t iVerbose)
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
    if (fVerbose > 0) cout << "BmnGlobalTrackDraw::Init()" << endl;

    FairRootManager *fManager = FairRootManager::Instance();

    fTrackList = (TClonesArray *) fManager->GetObject(GetName());
    if (fTrackList == 0)
    {
        LOG(error) << "BmnGlobalTrackDraw::Init() branch GlobalTrack not found! Task will be deactivated";
        SetActive(kFALSE);
        return kERROR;
    }
    if (fVerbose > 1) cout << "BmnGlobalTrackDraw::Init() get track list " << fTrackList << endl;

    fUpstreamHitList = (TClonesArray *) fManager->GetObject("BmnUpstreamHit");
    fUpstreamTrackList = (TClonesArray *) fManager->GetObject("BmnUpstreamTrack");

    fSiliconHitList = (TClonesArray *) fManager->GetObject("BmnSiliconHit");
    fSiliconTrackList = (TClonesArray *) fManager->GetObject("BmnSiliconTrack");

    fGemHitList = (TClonesArray *) fManager->GetObject("BmnGemStripHit");
    fGemTrackList = (TClonesArray *) fManager->GetObject("BmnGemTrack");

    fCscHitList = (TClonesArray *) fManager->GetObject("BmnCSCHit");

    fTof1HitList = (TClonesArray *) fManager->GetObject("BmnTof400Hit");
    fTof2HitList = (TClonesArray *) fManager->GetObject("BmnTof700Hit");

    fDchHitList = (TClonesArray *) fManager->GetObject("BmnDchHit");
    fDchTrackList = (TClonesArray *) fManager->GetObject("BmnDchTrack");

    fEventManager = MpdEventManager::Instance();
    if (fVerbose > -10) cout << "BmnGlobalTrackDraw::Init() get instance of MpdEventManager " << endl;

    MinEnergyLimit = fEventManager->GetEvtMinEnergy();
    MaxEnergyLimit = fEventManager->GetEvtMaxEnergy();
    PEnergy = 0;

    return kSUCCESS;
}

void BmnGlobalTrackDraw::Exec(Option_t* option)
{
    if (!IsActive()) return;

    if (fVerbose > 1) cout << " BmnGlobalTrackDraw::Exec " << endl;

    Reset();

    struct PointCompare 
    {
        bool operator()(const FairHit *a, const FairHit *b) const 
        {
            return a->GetZ() < b->GetZ();
        }
    };

    for (Int_t i = 0; i < fTrackList->GetEntriesFast(); i++)
    {
        if (fVerbose > 1) cout << "BmnGlobalTrackDraw::Exec " << i << endl;

        BmnGlobalTrack *tr = (BmnGlobalTrack  *) fTrackList->At(i);
        const FairTrackParam *pParamFirst = tr->GetParamFirst();

        // define whether track is primary
        bool isPrimary = ((TMath::Abs(pParamFirst->GetX()) < 10) && (TMath::Abs(pParamFirst->GetY()) < 10) && (TMath::Abs(pParamFirst->GetZ()) < 10));

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
        TParticlePDG *fParticlePDG = TDatabasePDG::Instance()->GetParticle(particlePDG);
        TParticle *P = new TParticle(particlePDG, i, -1, -1, -1, -1, px, py, pz, 0, pParamFirst->GetX(), pParamFirst->GetY(), pParamFirst->GetZ(), 0);

        // get EVE track list for this particle
        fTrList = GetTrGroup(P);
        // create EVE track corresponding global track
        TEveTrack *track = new TEveTrack(P, particlePDG, fTrPr);
        // set line color corresponding PDG particle code
        track->SetLineColor(fEventManager->Color(particlePDG));
        track->SetLineWidth(2);

        multiset<FairHit *, PointCompare> track_points;

        // get upstream track for global track
        if (tr->GetUpstreamTrackIndex() > -1) //KG почему закомментировано
        {
            BmnTrack* upstream_track = (BmnTrack*) fUpstreamTrackList->UncheckedAt(tr->GetUpstreamTrackIndex());
            for (Int_t k = 0; k < upstream_track->GetNHits(); k++)
                track_points.insert((FairHit *) fUpstreamTrackList->UncheckedAt(upstream_track->GetHitIndex(k)));
        }
        // get Silicon track for global track
        if (fSiliconTrackList != 0 && tr->GetSilTrackIndex() > -1)
        {
            BmnSiliconTrack* silicon_track = (BmnSiliconTrack *) fSiliconTrackList->UncheckedAt(tr->GetSilTrackIndex());
            for (Int_t k = 0; k < silicon_track->GetNHits(); k++)
                track_points.insert((FairHit *) fSiliconHitList->UncheckedAt(silicon_track->GetHitIndex(k)));
        }
        // get GEM track for global track
        if (fGemTrackList != 0 && tr->GetGemTrackIndex() > -1)
        {
            BmnGemTrack *gem_track = (BmnGemTrack *) fGemTrackList->UncheckedAt(tr->GetGemTrackIndex());
            for (Int_t k = 0; k < gem_track->GetNHits(); k++)
                track_points.insert((FairHit *) fGemHitList->UncheckedAt(gem_track->GetHitIndex(k)));
        }
        // add CSC hit
        if (fCscHitList != 0 && tr->GetCscHitIndex(0) > -1)
        {
            track_points.insert((FairHit *) fCscHitList->UncheckedAt(tr->GetCscHitIndex(0)));
        }
        if (fCscHitList != 0 && tr->GetCscHitIndex(1) > -1)
        {
            track_points.insert((FairHit *) fCscHitList->UncheckedAt(tr->GetCscHitIndex(1)));
        }
        // add TOF1 hit
        if (fTof1HitList != 0 && tr->GetTof1HitIndex() > -1)
        {
            track_points.insert((FairHit *) fTof1HitList->UncheckedAt(tr->GetTof1HitIndex()));
        }
        // add TOF2 hit
        if (fTof2HitList != 0 && tr->GetTof2HitIndex() > -1)
        {
            track_points.insert((FairHit *) fTof2HitList->UncheckedAt(tr->GetTof2HitIndex()));
        }
        // add DCH hit
        if (fDchTrackList != 0 && tr->GetDchTrackIndex() > -1)
        {
            BmnDchTrack *dch_track = (BmnDchTrack *) fDchTrackList->UncheckedAt(tr->GetDchTrackIndex());
            for (Int_t k = 0; k < dch_track->GetNHits(); k++)
                track_points.insert((FairHit *) fDchTrackList->UncheckedAt(dch_track->GetHitIndex(k)));
        }

        Int_t n = 0;
        for (auto &hit : track_points)
        {
            track->SetPoint(n, hit->GetX(), hit->GetY(), hit->GetZ());

            TEvePathMark* path = new TEvePathMark();
            TEveVector pos = TEveVector(hit->GetX(), hit->GetY(), hit->GetZ());
            path->fV = pos;
            path->fTime = hit->GetTimeStamp();
            if (n == 0)
            {
                TEveVector Mom = TEveVector(px, py, pz);
                path->fP = Mom;
            }

            // add path marker for current EVE track
            track->AddPathMark(*path);

            if (fVerbose > 3) cout << "Path marker added " << path << endl;
            n++;
        }

        // add track to EVE track list
        fTrList->AddElement(track);

        if (fVerbose > 3) cout << "Track added " << track->GetName() << endl;
    }

    // redraw EVE scenes
    gEve->Redraw3D(kFALSE);
}

// destructor
BmnGlobalTrackDraw::~BmnGlobalTrackDraw()
{
    delete[] fTrackList;
    delete[] fUpstreamHitList;
    delete[] fUpstreamTrackList;
    delete[] fSiliconHitList;
    delete[] fSiliconTrackList;
    delete[] fGemHitList;
    delete[] fGemTrackList;
    delete[] fCscHitList;
    delete[] fTof1HitList;
    delete[] fTof2HitList;
    delete[] fDchTrackList;
    delete[] fDchHitList;
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
