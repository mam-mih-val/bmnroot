// -------------------------------------------------------------------------
// -----                  BmnExpTrackDraw source file                  -----
// -----            created 05/10/15 by K. Gertsenberger               -----
// ----- class to visualize GlobalTracks from *.root in EventDisplay   -----
// -------------------------------------------------------------------------

#include "BmnExpTrackDraw.h"
#include "CbmTrack.h"
#include "FairHit.h"
#include "BmnMwpcHit.h"

#include "TEveManager.h"
#include "TEvePathMark.h"
#include "TEveVector.h"
#include "TDatabasePDG.h"

#include <iostream>
using namespace std;

// default constructor
BmnExpTrackDraw::BmnExpTrackDraw()
  : FairTask("BmnExpTrackDraw", 0),
    fTrackList(NULL),
    fHitsBranchName(""),
    fHitList(NULL),
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
BmnExpTrackDraw::BmnExpTrackDraw(const char* name, TString hitsBranchName, Int_t iVerbose)
  : FairTask(name, iVerbose),
    fTrackList(NULL),
    fHitsBranchName(hitsBranchName),
    fHitList(NULL),
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
InitStatus BmnExpTrackDraw::Init()
{
    if (fVerbose > 1)
        cout<<"BmnExpTrackDraw::Init()"<<endl;

    fEventManager = FairEventManager::Instance();
    if (fVerbose > 2)
        cout<<"BmnExpTrackDraw::Init() get instance of FairEventManager"<<endl;

    bmn_data_tree = new TChain("cbmsim");
    bmn_data_tree->Add(fEventManager->strExperimentFile);

    if (bmn_data_tree->GetFile() == NULL)
    {
      cout<<"BmnExpTrackDraw::Init() file with 'cbmsim' tree \""<<fEventManager->strExperimentFile<<"\" not found! Task will be deactivated "<<endl;
      SetActive(kFALSE);
      return kERROR;
    }

    bmn_data_tree->SetBranchAddress(GetName(), &fTrackList);
    if (!bmn_data_tree->GetBranchStatus(GetName()))
    {
      cout<<"BmnExpTrackDraw::Init() branch \""<<GetName()<<"\" not found in file ("<<fEventManager->strExperimentFile<<")! Task will be deactivated "<<endl;
      SetActive(kFALSE);
      return kERROR;
    }

    if (fVerbose > 2)
        cout<<"BmnExpTrackDraw::Init() get track list " <<fTrackList<<" ("<<fTrackList->GetEntriesFast()<<") from branch '"<<GetName()<<"'"<<endl;

    bmn_data_tree->SetBranchAddress(fHitsBranchName, &fHitList);
    if (!bmn_data_tree->GetBranchStatus(fHitsBranchName))
    {
      cout<<"BmnExpTrackDraw::Init() branch '"<<fHitsBranchName<<"' not found in file ("<<fEventManager->strExperimentFile<<")! Task will be deactivated "<<endl;
      SetActive(kFALSE);
      return kERROR;
    }

    if (fVerbose > 2)
        cout<<"BmnExpTrackDraw::Init() get list of hits "<<fHitList<<" from branch '"<<fHitsBranchName<<"'"<<endl;

    if (fEventManager->fEntryCount == 0)
        fEventManager->fEntryCount = bmn_data_tree->GetEntries();
    else
        fEventManager->fEntryCount = TMath::Min(fEventManager->fEntryCount, bmn_data_tree->GetEntries());

    if (fVerbose > 2)
        cout<<"BmnExpTrackDraw::Init() event count: "<<fEventManager->fEntryCount<<endl;

    fEvent = "Current Event";
    MinEnergyLimit = fEventManager->GetEvtMinEnergy();
    MaxEnergyLimit = fEventManager->GetEvtMaxEnergy();
    PEnergy = 0;

    return kSUCCESS;
}

// -------------------------------------------------------------------------
void BmnExpTrackDraw::Exec(Option_t* option)
{
    if (!IsActive()) return;
    if (fVerbose > 1)
        cout<<" BmnExpTrackDraw::Exec "<<endl;

    Reset();

    Int_t event_number = fEventManager->GetCurrentEvent();
    bmn_data_tree->GetEntry(event_number);

    CbmTrack* current_track;
    if (fVerbose > 1)
        cout<<" BmnExpTrackDraw::Exec: the number of tracks is "<<fTrackList->GetEntriesFast()<<endl;
    for (Int_t i = 0; i < fTrackList->GetEntriesFast(); i++)
    {
        if (fVerbose > 2)
            cout<<"BmnExpTrackDraw::Exec "<<i<<endl;

        current_track = (CbmTrack*) fTrackList->At(i);
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

        Int_t Np = current_track->GetNofHits();

        // cycle: add hits (points) to EVE path for this track
        cout<<"Points: "<<Np<<endl;
        for (Int_t n = 0; n < Np; n++)
        {
            FairHit* pHit = NULL;
            pHit = (FairHit*) fHitList->UncheckedAt(current_track->GetHitIndex(n));

            track->SetPoint(n, pHit->GetX(), pHit->GetY(), pHit->GetZ());

            TEvePathMark* path = new TEvePathMark();
            TEveVector pos = TEveVector(pHit->GetX(), pHit->GetY(), pHit->GetZ());
            //cout<<"Point: X="<<pHit->GetX()<<" Y="<<pHit->GetY()<<" Z="<<pHit->GetZ()<<endl;
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
        }

        // add track to EVE track list
        fTrList->AddElement(track);

        if (fVerbose > 3)
            cout<<"track added "<<track->GetName()<<endl;
    }

    if (fEventManager->EveRecoTracks == NULL)
    {
        fEventManager->EveRecoTracks = new TEveElementList("Reco tracks");
        gEve->AddElement(fEventManager->EveRecoTracks, fEventManager);
        fEventManager->EveRecoTracks->SetRnrState(kFALSE);
    }

    // redraw EVE scenes
    gEve->Redraw3D(kFALSE);
}

// destructor
BmnExpTrackDraw::~BmnExpTrackDraw()
{
}

void BmnExpTrackDraw::SetParContainers()
{
}

void BmnExpTrackDraw::Finish()
{
}

void BmnExpTrackDraw::Reset()
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
TEveTrackList* BmnExpTrackDraw::GetTrGroup(TParticle* P)
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
        gEve->AddElement(fTrList, fEventManager->EveRecoTracks);
        fTrList->SetRnrLine(kTRUE);
    }

    return fTrList;
}

ClassImp(BmnExpTrackDraw)
