// -------------------------------------------------------------------------
// -----                  MpdGlobalTrackDraw source file                     -----
// -----            created 10/12/13 by K. Gertsenberger               -----
// ----- class to visualize reconstructed GlobalTracks in EventDisplay -----
// -------------------------------------------------------------------------

#include "MpdGlobalTrackDraw.h"
#include "MpdTrack.h"
#include "MpdTpcHit.h"
#include "MpdEvent.h"
#include "MpdTpcKalmanTrack.h"

#include "TEveManager.h"
#include "TEvePathMark.h"
#include "TEveVector.h"
#include "TDatabasePDG.h"

#include <iostream>
using namespace std;


// default constructor
MpdGlobalTrackDraw::MpdGlobalTrackDraw()
  : FairTask("MpdGlobalTrackDraw", 0),
    fTrackList(NULL),
    fTrPr(NULL),
    fEventManager(NULL),
    fEveTrList(NULL),
    fEvent(""),
    fTrList(NULL),
    MinEnergyLimit(-1.),
    MaxEnergyLimit(-1.),
    PEnergy(-1.)
{
}

// standard constructor
MpdGlobalTrackDraw::MpdGlobalTrackDraw(const char* name, Int_t iVerbose)
  : FairTask(name, iVerbose),
    fTrackList(NULL),
    fTrPr(NULL),
    fEventManager(NULL),
    fEveTrList(new TObjArray(16)),
    fEvent(""),
    fTrList(NULL),
    MinEnergyLimit(-1.),
    MaxEnergyLimit(-1.),
    PEnergy(-1.)
{
}

// initialization of the track drawing task
InitStatus MpdGlobalTrackDraw::Init()
{
    if (fVerbose > 1)
        cout<<"MpdGlobalTrackDraw::Init()"<<endl;

    FairRootManager* fManager = FairRootManager::Instance();

    MpdEvent* fDstEvent = (MpdEvent*) fManager->GetObject("MPDEvent.");

    fTrackList = fDstEvent->GetGlobalTracks();
    if (fTrackList == 0)
    {
        cout<<"MpdGlobalTrackDraw::Init() branch "<<GetName()<<" not found! Task will be deactivated"<<endl;
        SetActive(kFALSE);
    }
    fKalmanTrackList = (TClonesArray*)fManager->GetObject("TpcKalmanTrack");
    if (fKalmanTrackList == 0)
    {
        cout<<"MpdGlobalTrackDraw::Init() branch TpcKalmanTrack not found! Task will be deactivated"<<endl;
        SetActive(kFALSE);
    }
    fTpcHitList = (TClonesArray*)fManager->GetObject("TpcHit");
    if (fTpcHitList == 0)
    {
        cout<<"MpdGlobalTrackDraw::Init() branch TpcHit not found! Task will be deactivated"<<endl;
        SetActive(kFALSE);
    }

    if(fVerbose > 2)
        cout<<"MpdGlobalTrackDraw::Init() get track list "<<fTrackList<<endl;
    if(fVerbose > 2)
        cout<<"MpdGlobalTrackDraw::Init() create propagator"<<endl;

    fEventManager = FairEventManager::Instance();
    if(fVerbose > 2)
        cout<<"MpdGlobalTrackDraw::Init() get instance of FairEventManager "<<endl;

    fEvent = "Current Event";
    MinEnergyLimit = fEventManager->GetEvtMinEnergy();
    MaxEnergyLimit = fEventManager->GetEvtMaxEnergy();
    PEnergy = 0;

    if (IsActive())
        return kSUCCESS;
    else
        return kERROR;
}
// -------------------------------------------------------------------------
void MpdGlobalTrackDraw::Exec(Option_t* option)
{
    if (!IsActive()) return;
    if (fVerbose > 1)
        cout<<" MpdGlobalTrackDraw::Exec "<<endl;

    Reset();

    MpdTrack* tr;

    for (Int_t i = 0; i < fTrackList->GetEntriesFast(); i++)
    {
        if (fVerbose > 2)
            cout<<"FairMCTracks::Exec "<<i<<endl;

        tr = (MpdTrack*)fTrackList->At(i);

        // get current kalman track
        //for (int j= 0; j < fKalmanTrackList->GetEntriesFast(); j++)
        //{
        //    MpdTpcKalmanTrack* kt = (MpdTpcKalmanTrack*) fKalmanTrackList->UncheckedAt(j);
        //    cout<<kt->GetTrackID()<<endl;
        //}//tr->GetID()
        MpdTpcKalmanTrack* kalman_track = (MpdTpcKalmanTrack*) fKalmanTrackList->UncheckedAt(i);

        // get hits collection for reconstructed track
        TObjArray* trHits = kalman_track->GetTrHits(); // track hits of MpdKalmanHit type
        Int_t Np = trHits->GetEntriesFast();

        // get PDG particle code
        Float_t maxPDG = tr->GetPidProbElectron();
        int particlePDG = 11;
        if (maxPDG < tr->GetPidProbPion()){
            maxPDG = tr->GetPidProbPion();
            particlePDG = 211;
        }
        if (maxPDG < tr->GetPidProbKaon()){
            maxPDG = tr->GetPidProbKaon();
            particlePDG = 321;
        }
        if (maxPDG < tr->GetPidProbProton()){
            maxPDG = tr->GetPidProbProton();
            particlePDG = 2212;
        }
        if (tr->GetPt() < 0)
            particlePDG *= -1;

        // get momentum
        Double_t px = tr->GetPx(), py = tr->GetPy(), pz = tr->GetPz();

        // calculate energy
        Double_t mass = 0.0, energy = 0.0;
        TParticlePDG* fParticlePDG = TDatabasePDG::Instance()->GetParticle(particlePDG);
        if (fParticlePDG)
            mass = fParticlePDG->Mass();
        if (mass >= 0)
            energy  = TMath::Sqrt(mass*mass + px*px+py*py+pz*pz);

        // define whether track is primary
        bool isPrimary = kalman_track->GetChi2Vertex() < 10;

        // first inner hit of current track in TPC
        MpdTpcHit* firstTpcHit = (MpdTpcHit*) fTpcHitList->UncheckedAt( ((MpdKalmanHit*) trHits->UncheckedAt(Np-1))->GetIndex() );

        //TParticle(Int_t pdg, Int_t status, Int_t mother1, Int_t mother2, Int_t daughter1, Int_t daughter2, Double_t px, Double_t py, Double_t pz, Double_t etot, Double_t vx, Double_t vy, Double_t vz, Double_t time)
        TParticle* P = new TParticle(particlePDG, i, -1, -1, -1, -1, px, py, pz, energy, firstTpcHit->GetX(), firstTpcHit->GetY(), firstTpcHit->GetZ(), firstTpcHit->GetTimeStamp());

        PEnergy = energy;
        MinEnergyLimit = TMath::Min(PEnergy, MinEnergyLimit);
        MaxEnergyLimit = TMath::Max(PEnergy, MaxEnergyLimit);
        if (fVerbose > 2)
            cout<<"MinEnergyLimit "<<MinEnergyLimit<<" MaxEnergyLimit "<<MaxEnergyLimit<<endl;

        // skip secondary tracks if primary flag is set
        if (fEventManager->IsPriOnly() && (!isPrimary))
            continue;

        // skip particles with unequal PDG if PDG number is set
        if ((fEventManager->GetCurrentPDG() != 0) && (fEventManager->GetCurrentPDG() != particlePDG))
            continue;

        // skip tracks whose energy doesn't lie in selected limits
        if (fVerbose > 2)
            cout<<"PEnergy "<<PEnergy<<" Min "<<fEventManager->GetMinEnergy()<<" Max "<<fEventManager->GetMaxEnergy()<<endl;
        if ((PEnergy < fEventManager->GetMinEnergy()) || (PEnergy > fEventManager->GetMaxEnergy()))
            continue;

        // get EVE track list for this particle
        fTrList = GetTrGroup(P);
        // create EVE track corresponding kalman track
        TEveTrack* track = new TEveTrack(P, particlePDG, fTrPr);
        // set line color corresponding PDG particle code
        track->SetLineColor(fEventManager->Color(particlePDG));

        // cycle: add hits (points) to EVE path for this track
        for (Int_t n = 0; n < Np; n++)
        {
            MpdKalmanHit* kalmanHit = (MpdKalmanHit*) trHits->UncheckedAt(Np-n-1);
            MpdTpcHit* tpcHit = (MpdTpcHit*) fTpcHitList->UncheckedAt(kalmanHit->GetIndex());

            track->SetPoint(n, tpcHit->GetX(), tpcHit->GetY(), tpcHit->GetZ());

            TEvePathMark* path = new TEvePathMark();
            TEveVector pos = TEveVector(tpcHit->GetX(), tpcHit->GetY(), tpcHit->GetZ());
            path->fV = pos;
            path->fTime = tpcHit->GetTimeStamp();
            if (n == 0)
            {
                TEveVector Mom = TEveVector(px, py, pz);
                path->fP = Mom;
            }

            // add path marker for current EVE track
            track->AddPathMark(*path);

            if (fVerbose > 3)
                cout<<"Path marker added "<<path<<endl;
        }

        // add track to EVE track list
        fTrList->AddElement(track);

        if (fVerbose > 3)
            cout<<"track added "<<track->GetName()<<endl;
    }

    // pass new defined energy range to Event Manager
    fEventManager->SetEvtMaxEnergy(MaxEnergyLimit);
    fEventManager->SetEvtMinEnergy(MinEnergyLimit);

    // redraw EVE scenes
    gEve->Redraw3D(kFALSE);
}

// destructor
MpdGlobalTrackDraw::~MpdGlobalTrackDraw()
{
}

void MpdGlobalTrackDraw::SetParContainers()
{
}

void MpdGlobalTrackDraw::Finish()
{
}

void MpdGlobalTrackDraw::Reset()
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
TEveTrackList* MpdGlobalTrackDraw::GetTrGroup(TParticle* P)
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

        if (fEventManager->EveRecoTracks == NULL)
        {
            fEventManager->EveRecoTracks = new TEveElementList("Reco tracks");
            gEve->AddElement(fEventManager->EveRecoTracks, fEventManager);
            fEventManager->EveRecoTracks->SetRnrState(kFALSE);
        }

        gEve->AddElement(fTrList, fEventManager->EveRecoTracks);
        fTrList->SetRnrLine(kTRUE);
    }

    return fTrList;
}

ClassImp(MpdGlobalTrackDraw)
