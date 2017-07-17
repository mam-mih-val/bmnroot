// -------------------------------------------------------------------------
// -----                        FairMCStack source file                  -----
// -----                  Created 09/10/08  by M. Al-Turany            -----
// -------------------------------------------------------------------------

#include "FairMCStack.h"
#ifdef BMNROOT
#include "CbmMCTrack.h"
#else
#include "FairMCTrack.h"
#endif
#include "FairRootManager.h"
#include "FairLogger.h"

#include "TGeoTrack.h"
#include "TClonesArray.h"
#include "TEveManager.h"
#include "TParticlePDG.h"
#include "TDatabasePDG.h"

#include <iostream>
using namespace std;


// -----   Default constructor   -------------------------------------------
FairMCStack::FairMCStack()
{
}

// -----   Standard constructor   ------------------------------------------
FairMCStack::FairMCStack(const char* name, Int_t iVerbose)
  : FairTask(name, iVerbose),
   fEveTrList(new TObjArray(16))
{
}

// -------------------------------------------------------------------------
InitStatus FairMCStack::Init()
{
  if (fVerbose > 1)
      cout<<"FairMCStack::Init()"<<endl;

  FairRootManager* fManager = FairRootManager::Instance();

  fTrackList = (TClonesArray*) fManager->GetObject("MCTrack");
  if (fTrackList == 0)
  {
    LOG(ERROR)<<"FairMCStack::Init() branch "<<GetName()<<" not found! Task will be deactivated"<<FairLogger::endl;
    SetActive(kFALSE);
  }
  if (fVerbose > 2)
      cout<<"FairMCStack::Init() get track list"<<fTrackList<<endl;

  fEventManager = FairEventManager::Instance();
  if (fVerbose > 2)
      cout<<"FairMCStack::Init() get instance of FairEventManager"<<endl;

  MinEnergyLimit = fEventManager->GetEvtMinEnergy();
  MaxEnergyLimit = fEventManager->GetEvtMaxEnergy();
  PEnergy = 0;

  fPro = new FairGeanePro();
  gMC3 = (TGeant3*) gMC;

  x1[0] = 0; x1[1] = 0; x1[2] = 0;
  p1[0] = 0; p1[1] = 0; p1[2] = 0;
  x2[0] = 0; x2[1] = 0; x2[2] = 0;
  p2[0] = 0; p2[1] = 0; p2[2] = 0;
  for (Int_t i = 0; i < 15; i++)
      ein[i] = 0;

  Float_t length[1];
  length[0] = 100.0;
  //gMC3->Eufill(1, ein, length);

  fTrajFilter = FairTrajFilter::Instance();

  if (IsActive())
      return kSUCCESS;
  else
      return kERROR;
}

// -------------------------------------------------------------------------
void FairMCStack::Exec(Option_t* /*option*/)
{
  if (IsActive())
  {
    if (fVerbose > 1) cout<<"FairMCStack::Exec"<<endl;
    Reset();

    for (Int_t i = 0; i < fTrackList->GetEntriesFast(); i++)
    {
      if (fVerbose > 2) cout<<"FairMCStack::Exec "<<i<<endl;

      #ifdef BMNROOT
        CbmMCTrack* tr = (CbmMCTrack*) fTrackList->At(i);
      #else
        FairMCTrack* tr = (FairMCTrack*) fTrackList->At(i);
      #endif

      TVector3 Ptot;
      tr->GetMomentum(Ptot);
      Int_t  MotherId =tr->GetMotherId();
      TVector3 Vertex;
      tr->GetStartVertex(Vertex);

      Double_t time = tr->GetStartT()*1e-09;
      x1[0] = Vertex.x(); x1[1] = Vertex.y(); x1[2] = Vertex.z();
      p1[0] = Ptot.Px(); p1[1] = Ptot.Py(); p1[2] = Ptot.Pz();

      //TParticle* P = (TParticle*) tr->GetParticle();
      TParticlePDG* fParticlePDG = TDatabasePDG::Instance()->GetParticle(tr->GetPdgCode());

      Double_t mass = 0, ene = 0;
      if (fParticlePDG)
        mass = fParticlePDG->Mass();
      if (mass >= 0)
        ene = TMath::Sqrt(mass*mass + Ptot.Mag2());

      //TParticle(Int_t pdg, Int_t status, Int_t mother1, Int_t mother2, Int_t daughter1, Int_t daughter2, Double_t px, Double_t py, Double_t pz, Double_t etot, Double_t vx, Double_t vy, Double_t vz, Double_t time)
      TParticle* P = new TParticle(tr->GetPdgCode(), i, MotherId, -1, -1, -1, Ptot.Px(), Ptot.Py(),Ptot.Pz(),ene, Vertex.x(), Vertex.z(), Vertex.z(), time);

      PEnergy = P->Energy();
      MinEnergyLimit = TMath::Min(PEnergy, MinEnergyLimit);
      MaxEnergyLimit = TMath::Max(PEnergy, MaxEnergyLimit);
      if (fVerbose > 2)
          cout<<"MinEnergyLimit "<<MinEnergyLimit<<" MaxEnergyLimit "<<MaxEnergyLimit<<endl;

      if ((fEventManager->IsPriOnly() && (P->GetMother(0) > -1)))
          continue;
      if ((fEventManager->GetCurrentPDG() != 0) && (fEventManager->GetCurrentPDG() != tr->GetPdgCode()))
          continue;
      if (fVerbose > 2)
          cout<<"PEnergy "<<PEnergy<<" Min "<<fEventManager->GetMinEnergy()<<" Max "<<fEventManager->GetMaxEnergy()<<endl;
      if ((PEnergy < fEventManager->GetMinEnergy()) || (PEnergy > fEventManager->GetMaxEnergy()))
          continue;

      //Int_t Np = tr->GetNpoints();
      fTrList = GetTrGroup(P);
      TEveTrack* track= new TEveTrack(P, tr->GetPdgCode(), fTrPr);
      track->SetLineColor(fEventManager->Color(tr->GetPdgCode()));

      // PROPAGATION
      // Int_t GeantCode = TDatabasePDG::Instance()->ConvertPdgToGeant3(tr->GetPdgCode());
      // gMC3->Ertrak(x1, p1, x2, p2,G eantCode, "L");
      fPro->PropagateToLength(100.0);
      fPro->Propagate(x1, p1, x2, p2, tr->GetPdgCode());
      TGeoTrack* tr1 = fTrajFilter->GetCurrentTrk();

      const Double_t* point;
      Int_t Np = tr1->GetNpoints();
      for (Int_t n = 0; n < Np; n++)
      {
        point = tr1->GetPoint(n);
        track->SetPoint(n, point[0], point[1], point[2]);
        TEveVector pos = TEveVector(point[0], point[1], point[2]);

        TEvePathMark* path = new TEvePathMark();
        path->fV = pos ;
        path->fTime = point[3];
        if (n == 0)
        {
          TEveVector Mom = TEveVector(P->Px(), P->Py(),P->Pz());
          path->fP = Mom;
        }

        track->AddPathMark(*path);
        if (fVerbose > 3) cout<<"Path marker added "<<path<<endl;
      }

      fTrList->AddElement(track);
      if (fVerbose > 3) cout<<"Track added "<<track->GetName()<<endl;
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
}

// -----   Destructor   ----------------------------------------------------
FairMCStack::~FairMCStack()
{
}

// -------------------------------------------------------------------------
void FairMCStack::SetParContainers()
{
}

// -------------------------------------------------------------------------
void FairMCStack::Finish()
{
}

// -------------------------------------------------------------------------
void FairMCStack::Reset()
{
  for (Int_t i = 0; i < fEveTrList->GetEntriesFast(); i++)
  {
    TEveTrackList* ele = (TEveTrackList*) fEveTrList->At(i);
    gEve->RemoveElement(ele, fEventManager);
  }
  fEveTrList->Clear();
}

TEveTrackList* FairMCStack::GetTrGroup(TParticle* P)
{
  fTrList = 0;
  for (Int_t i = 0; i < fEveTrList->GetEntriesFast(); i++)
  {
    TEveTrackList* TrListIn = (TEveTrackList*) fEveTrList->At(i);

    if (strcmp(TrListIn->GetName(), P->GetName()) == 0)
    {
      fTrList= TrListIn;
      break;
    }
  }

  if (fTrList == 0)
  {
    fTrPr = new TEveTrackPropagator();
    fTrList = new TEveTrackList(P->GetName(), fTrPr);
    fTrList->SetMainColor(fEventManager->Color(P->GetPdgCode()));
    fTrList->SetRnrLine(kTRUE);

    fEveTrList->Add(fTrList);
    gEve->AddElement(fTrList, fEventManager);
  }

  return fTrList;
}

ClassImp(FairMCStack)
