// -------------------------------------------------------------------------
// -----                        MpdMCTracks source file                  -----
// -----                  Created 10/12/07  by M. Al-Turany            -----
// -------------------------------------------------------------------------
#include "MpdMCTracks.h"

#include "FairRootManager.h"
#include "FairLogger.h"

#include "TEveManager.h"    // for gEve
#include "TEvePathMark.h"
#include "TEveVector.h"
#include "TGeoTrack.h"
#include "TMathBase.h"
#include "TObjArray.h"

#include <iostream>
using namespace std;


// -----   Default constructor   -------------------------------------------
MpdMCTracks::MpdMCTracks()
  : FairTask("MpdMCTracks", 0),
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

// -----   Standard constructor   ------------------------------------------
MpdMCTracks::MpdMCTracks(const char* name, Int_t iVerbose)
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

// -----   Destructor   ----------------------------------------------------
MpdMCTracks::~MpdMCTracks()
{
}

// -------------------------------------------------------------------------
InitStatus MpdMCTracks::Init()
{
  LOG(debug)<<"MpdMCTracks::Init()";

  FairRootManager* fManager = FairRootManager::Instance();
  fEventManager = MpdEventManager::Instance();
  LOG(debug1) <<  "MpdMCTracks::Init() get instance of MpdEventManager ";

  fTrackList = (TClonesArray*) fManager->GetObject("GeoTracks");
  if (fTrackList == 0)
  {
    LOG(error)<<"MpdMCTracks::Init() branch "<<GetName()<<" not found! Task will be deactivated";
    SetActive(kFALSE);
  }
  LOG(debug1)<<"MpdMCTracks::Init() get track list "<<fTrackList;

  MinEnergyLimit = fEventManager->GetEvtMinEnergy();
  MaxEnergyLimit = fEventManager->GetEvtMaxEnergy();
  PEnergy = 0;

  if (IsActive()) return kSUCCESS;
  else return kERROR;
}

// -------------------------------------------------------------------------
void MpdMCTracks::Exec(Option_t* /*option*/)
{
  if (!IsActive())
      return;
  LOG(debug1)<<"MpdMCTracks::Exec";

  Reset();

  TGeoTrack* tr;
  const Double_t* point;
  for (Int_t i = 0; i < fTrackList->GetEntriesFast(); i++)
  {
    LOG(debug3)<<"MpdMCTracks::Exec "<<i;
    tr = (TGeoTrack*) fTrackList->At(i);

    TParticle* P = (TParticle*) tr->GetParticle();
    PEnergy = P->Energy();
    MinEnergyLimit = TMath::Min(PEnergy, MinEnergyLimit);
    MaxEnergyLimit = TMath::Max(PEnergy, MaxEnergyLimit);
    LOG(debug3)<<"MinEnergyLimit "<<MinEnergyLimit<<" MaxEnergyLimit "<<MaxEnergyLimit;

    if ((fEventManager->IsPriOnly() && (P->GetMother(0) > -1)))
        continue;
    if ((fEventManager->fCurrentPDG.size() != 0) && (fEventManager->fCurrentPDG.find(tr->GetPDG()) == fEventManager->fCurrentPDG.end()))
        continue;
    LOG(debug3)<<"PEnergy "<<PEnergy<<" Min "<<fEventManager->GetMinEnergy()<<" Max "<<fEventManager->GetMaxEnergy();
    if ((PEnergy < fEventManager->GetMinEnergy()) || (PEnergy > fEventManager->GetMaxEnergy()))
        continue;

    fTrList = GetTrGroup(P);
    TEveTrack* track = new TEveTrack(P, tr->GetPDG(), fTrPr);
    track->SetLineColor(fEventManager->Color(tr->GetPDG()));

    Int_t Np = tr->GetNpoints();
    for (Int_t n = 0; n < Np; n++)
    {
        point = tr->GetPoint(n);

        track->SetPoint(n, point[0], point[1], point[2]);
        TEveVector pos = TEveVector(point[0], point[1], point[2]);

        TEvePathMark* path = new TEvePathMark();
        path->fV = pos;
        path->fTime = point[3];
        if (n == 0)
        {
          TEveVector Mom = TEveVector(P->Px(), P->Py(), P->Pz());
          path->fP = Mom;
        }

        track->AddPathMark(*path);
        LOG(debug4)<<"Path marker added "<<path;

        delete path;
    }

    fTrList->AddElement(track);
    LOG(debug3)<<"Track added "<<track->GetName();
  }

  //for (Int_t i = 0; i < fEveTrList->GetEntriesFast(); i++)
  //{
  //  TEveTrackList* TrListIn = (TEveTrackList*) fEveTrList->At(i);
  //  TrListIn->FindMomentumLimits(TrListIn, kFALSE);
  //}
  fEventManager->SetEvtMinEnergy(MinEnergyLimit);
  fEventManager->SetEvtMaxEnergy(MaxEnergyLimit);

  //gEve->Redraw3D(kFALSE);
}

// -------------------------------------------------------------------------
void MpdMCTracks::SetParContainers()
{
}

// -------------------------------------------------------------------------
void MpdMCTracks::Finish()
{
}

// -------------------------------------------------------------------------
void MpdMCTracks::Reset()
{
  for (Int_t i = 0; i < fEveTrList->GetEntriesFast(); i++)
  {
    TEveTrackList* ele = (TEveTrackList*) fEveTrList->At(i);
    gEve->RemoveElement(ele, fEventManager->EveMCTracks);
  }
  fEveTrList->Clear();
}

TEveTrackList* MpdMCTracks::GetTrGroup(TParticle* P)
{
  fTrList = 0;
  for (Int_t i = 0; i < fEveTrList->GetEntriesFast(); i++)
  {
    TEveTrackList* TrListIn = (TEveTrackList*) fEveTrList->At(i);
    if (strcmp(TrListIn->GetName(), P->GetName()) == 0)
    {
      fTrList = TrListIn;
      break;
    }
  }

  if (fTrList == 0)
  {
    fTrPr = new TEveTrackPropagator();
    fTrList = new TEveTrackList(P->GetName(), fTrPr);
    // set track color by particle PDG from MpdEventManager
    fTrList->SetMainColor(fEventManager->Color(P->GetPdgCode()));
    fTrList->SetRnrLine(kTRUE);
    fEveTrList->Add(fTrList);

    fEventManager->AddEventElement(fTrList, MCTrackList);
  }

  return fTrList;
}

ClassImp(MpdMCTracks)
