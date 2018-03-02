// -------------------------------------------------------------------------
// -----                        BmnTrackDrawP source file               -----
// -----                  Created 02/12/15  by K. Gertsenberger        -----
// -------------------------------------------------------------------------

#include "BmnTrackDrawP.h"
#include "BmnTrack.h"
#include "CbmStack.h"
#include "MpdEventManagerEditor.h"

#include "TROOT.h"
#include "TGeant3.h"
#include "TGeant3TGeo.h"
#include "TEveTrackPropagator.h"
#include "TEveManager.h"
#include "TDatabasePDG.h"

#include <iostream>
using namespace std;

// -----   Default constructor   -------------------------------------------
BmnTrackDrawP::BmnTrackDrawP()
    : FairTask("BmnTrackDrawP", 0),
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
// -------------------------------------------------------------------------

// -----   Standard constructor   ------------------------------------------
BmnTrackDrawP::BmnTrackDrawP(const char* name, Int_t iVerbose)
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
// -------------------------------------------------------------------------


InitStatus BmnTrackDrawP::Init()
{
    if (fVerbose > 1)
        cout<<"BmnTrackDrawP::Init()"<<endl;

    fEventManager = MpdEventManager::Instance();
    if (fVerbose > 2)
        cout<<"BmnTrackDrawP::Init() get instance of EventManager"<<endl;

    FairRootManager* fManager = FairRootManager::Instance();
    if (fVerbose > 2)
        cout<<"BmnExpTrackDraw::Init() get instance of FairRootManager: "<<fManager<<endl;

    fTrackList = (TClonesArray*)fManager->GetObject(GetName());
    if (fVerbose > 2)
        cout<<"BmnTrackDrawP::Init() get track list " <<fTrackList<<" from branch '"<<GetName()<<"'"<<endl;

    MinEnergyLimit = fEventManager->GetEvtMinEnergy();
    MaxEnergyLimit = fEventManager->GetEvtMaxEnergy();
    PEnergy = 0;

    if (gMC == NULL)
        InitGeant3();

    fPro = new FairGeanePro();

    fTrajFilter = FairTrajFilter::Instance();

    return kSUCCESS;
}

void BmnTrackDrawP::InitGeant3()
{
    /** Private method for setting Geane configuration and cuts*/
    TString work = getenv("VMCWORKDIR");
    TString work_config = work + "/gconfig/";
    TString LibMacro =  work_config + "g3libs.C";
    TString ConfigMacro = work_config + "g3Config.C";
    TString cuts = work_config + "SetCuts.C";

    // Now load the Config and Cuts
    gROOT->LoadMacro(LibMacro.Data());
    gROOT->ProcessLine("g3libs()");

    //gROOT->LoadMacro(ConfigMacro.Data());
    //gROOT->ProcessLine("Config()");

    TGeant3* geant3 = new TGeant3TGeo("C++ Interface to Geant3");
    //TGeant3* geant3 = new  TGeant3("C++ Interface to Geant3");
    // create Cbm Specific Stack
    CbmStack *st = new CbmStack();
    // Set minimum number of points to store the track
    // The default value is one, which means each track
    // needs at least 1 point in any detector
    st->SetMinPoints(1);
    geant3->SetStack(st);
    // ******* GEANT3  specific configuration for simulated Runs  *******
    geant3->SetTRIG(1);         //Number of events to be processed
    geant3->SetSWIT(4, 100);
    geant3->SetDEBU(0, 0, 1);
    geant3->SetRAYL(1);
    geant3->SetSTRA(0); //1);
    geant3->SetAUTO(0);         //Select automatic STMIN etc... calc. (AUTO 1) or manual (AUTO 0)
    geant3->SetABAN(0);         //Restore 3.16 behaviour for abandoned tracks
    geant3->SetOPTI(2);         //Select optimisation level for GEANT geometry searches (0,1,2)
    geant3->SetERAN(5.e-7);
    geant3->SetCKOV(1);     // cerenkov photons

    gROOT->LoadMacro(cuts);
    gROOT->ProcessLine("SetCuts()");

    //gMC->SetMagField(fxField);
    //gMC->Init();
    gMC->BuildPhysics();
}

// -------------------------------------------------------------------------
void BmnTrackDrawP::Exec(Option_t* option)
{
    if (!IsActive())
        return;

    if (fVerbose > 1)
        cout<<"BmnTrackDrawP::Exec"<< endl;

    Reset();

    BmnTrack* current_track;
    cout<<"Tracks: "<<fTrackList->GetEntriesFast()<<endl;
    for (Int_t i = 0; i < fTrackList->GetEntriesFast(); i++)
    {
        if (fVerbose > 2)
            cout<<"BmnTrackDrawP::Exec "<<i<<endl;

        current_track = (BmnTrack*) fTrackList->At(i);
        FairTrackParam* pParamFirst = (FairTrackParam*) current_track->GetParamFirst();

        // define whether track AL-TURAis primary
        bool isPrimary = ( (TMath::Abs(pParamFirst->GetX())<10) && (TMath::Abs(pParamFirst->GetY())<10) && (TMath::Abs(pParamFirst->GetZ())<10) );

        // skip secondary tracks if primary flag is set
        if (fEventManager->IsPriOnly() && (!isPrimary))
            continue;

        // set PDG particle code
        int particlePDG = 2212; // 0; // without identification - Rootino

        // get momentum
        TVector3 mom;
        pParamFirst->SetQp(1);
        pParamFirst->Momentum(mom);
        Double_t px = mom.X(), py = mom.Y(), pz = mom.Z();

        // create particle
        TParticlePDG* fParticlePDG = TDatabasePDG::Instance()->GetParticle(particlePDG);
        //TParticle(Int_t pdg, Int_t status, Int_t mother1, Int_t mother2, Int_t daughter1, Int_t daughter2, Double_t px, Double_t py, Double_t pz, Double_t etot, Double_t vx, Double_t vy, Double_t vz, Double_t time)
        TParticle* P = new TParticle(particlePDG, i, -1, -1, -1, -1, px, py, pz, 0, pParamFirst->GetX(), pParamFirst->GetY(), pParamFirst->GetZ(), 0);

        // get EVE track list for this particle
        fTrList = GetTrGroup(P);
        // create EVE track corresponding global track
        TEveTrack* track = new TEveTrack(P, particlePDG, fTrPr);
        // set line color corresponding PDG particle code
        track->SetLineColor(fEventManager->Color(particlePDG));

        // propagate the tracks by Geane
        x1[0] = pParamFirst->GetX();
        x1[1] = pParamFirst->GetY();
        x1[2] = pParamFirst->GetZ();
        p1[0] = px;
        p1[1] = py;
        p1[2] = pz;

        x2[0] = 0;
        x2[1] = 0;
        x2[2] = 0;
        p2[0] = 0;
        p2[1] = 0;
        p2[2] = 0;

        cout<<"Propagate track from ("<<pParamFirst->GetX()<<","<<pParamFirst->GetY()<<","<<pParamFirst->GetZ()<<") with momentum: ("
           <<px<<","<<py<<","<<pz<<")"<<endl;

        Bool_t isSuccess = fPro->Propagate(x1, p1, x2, p2, particlePDG);

        cout<<"isSuccess: "<<isSuccess<<". Track end point: ("<<x2[0]<<","<<x2[1]<<","<<x2[2]<<") with momentum: ("<<p2[0]<<","<<p2[1]<<","<<p2[2]<<")"<<endl;

        /*FairTrackParH* fTrackParH = new FairTrackParH(tr1.GetPos()*TMath::Cos(tr1.Theta())*TMath::Cos(tr1.Phi()),
                                                      tr1.GetPos()*TMath::Cos(tr1.Theta())*TMath::Sin(tr1.Phi()), tr1.GetPos()*TMath::Cos(tr1.Theta()),
                                                      tr1.Theta(), tr1.Phi(), 1/tr1.Momentum(), tr1.GetCovariance());
        fGeanePro->Init(fTrackParH);
        fGeanePro->PropagateToVolume("tof1", 1, 0);
        fGeanePro->Propagate(211);*/

        TGeoTrack* geo_track = fTrajFilter->GetCurrentTrk();
        Int_t Np = geo_track->GetNpoints();
        // cycle: add hits (points) to EVE path for this track
        cout<<"Points: "<<Np<<endl;
        for (Int_t n = 0; n < Np; n++)
        {
            const Double_t* point = geo_track->GetPoint(n);

            track->SetPoint(n, point[0], point[1], point[2]);

            TEvePathMark* path = new TEvePathMark();
            TEveVector pos = TEveVector(point[0], point[1],point[2]);
            //cout<<"Point: X="<<point[0]<<" Y="<<point[1]<<" Z="<<point[2]<<endl;
            path->fV = pos;
            path->fTime = point[3];
            if (n == 0)
            {
                TEveVector Mom = TEveVector(px, py, pz);
                path->fP = Mom;
            }

            if (fVerbose > 3)
                cout<<"Path marker added "<<path<<endl;

            // add path marker for current EVETChain* bmn_data_tree;  //! track
            track->AddPathMark(*path);

            if (fVerbose > 3)
                cout<<"Path marker added "<<path<<endl;
         }

         // add track to EVE track list
         fTrList->AddElement(track);

         if (fVerbose > 3)
             cout<<"track added "<<track->GetName()<<endl;
    }

    fEventManager->AddEventElement(fTrList, RecoTrackList);

    // redraw EVE scenes
    gEve->Redraw3D(kFALSE);
}

// -----   Destructor   ----------------------------------------------------
BmnTrackDrawP::~BmnTrackDrawP()
{
}

// -------------------------------------------------------------------------
void BmnTrackDrawP::SetParContainers()
{
}

// -------------------------------------------------------------------------
void BmnTrackDrawP::Finish()
{
}

// -------------------------------------------------------------------------
void BmnTrackDrawP::Reset()
{
    // clear EVE track lists (fEveTrList)
    for (Int_t i = 0; i < fEveTrList->GetEntriesFast(); i++)
    {
        TEveTrackList*  ele = (TEveTrackList*) fEveTrList->At(i);
        gEve->RemoveElement(ele, fEventManager->EveRecoTracks);
    }

    fEveTrList->Clear();
}

TEveTrackList* BmnTrackDrawP::GetTrGroup(TParticle* P)
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

ClassImp(BmnTrackDrawP)
