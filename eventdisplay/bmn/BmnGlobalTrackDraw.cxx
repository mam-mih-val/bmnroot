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
    CSC,
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
    if (fVerbose > 0) cout << "BmnGlobalTrackDraw::Init()" << endl;

    FairRootManager* fManager = FairRootManager::Instance();

    fTrackList = (TClonesArray*) fManager->GetObject(GetName());
    if (fTrackList == 0)
    {
        LOG(ERROR) << "BmnGlobalTrackDraw::Init()  branch GlobalTrack not found! Task will be deactivated";
        SetActive(kFALSE);
        return kERROR;
    }
    if (fVerbose > 1) cout << "BmnGlobalTrackDraw::Init() get track list " << fTrackList << endl;

    fUpstreamHitList = (TClonesArray*) fManager->GetObject("BmnUpstreamHit");
    fUpstreamTrackList = (TClonesArray*) fManager->GetObject("BmnUpstreamTrack");

    fSiliconHitList = (TClonesArray*) fManager->GetObject("BmnSiliconHit");
    fSiliconTrackList = (TClonesArray*) fManager->GetObject("BmnSiliconTrack");

    fGemHitList = (TClonesArray*) fManager->GetObject("BmnGemStripHit");
    fGemTrackList = (TClonesArray*) fManager->GetObject("BmnGemTrack");

    fCscHitList = (TClonesArray*) fManager->GetObject("BmnCSCHit");

    fTof1HitList = (TClonesArray*) fManager->GetObject("BmnTof400Hit");
    fTof2HitList = (TClonesArray*) fManager->GetObject("BmnTof700Hit");

    fDchHitList = (TClonesArray*) fManager->GetObject("BmnDchHit");
    fDchTrackList = (TClonesArray*) fManager->GetObject("BmnDchTrack");

    fEventManager = MpdEventManager::Instance();
    if (fVerbose > -10) cout << "BmnGlobalTrackDraw::Init() get instance of MpdEventManager " << endl;

    MinEnergyLimit = fEventManager->GetEvtMinEnergy();
    MaxEnergyLimit = fEventManager->GetEvtMaxEnergy();
    PEnergy = 0;

    // get an order of the detectors
    //             ↓-Z    ↓-ID
    vector<tuple<Int_t, Int_t>> order_of_detectors;
    // get array of nodes
    TObjArray* nodes = gGeoManager->GetMasterVolume()->GetNodes();
    for (Int_t i = 0; i < nodes->GetEntries(); i++)
    {
        TGeoNodeMatrix* el = (TGeoNodeMatrix*) nodes->UncheckedAt(i);

        // transform detector name to lowercase
        TString detectorName(el->GetName());
        detectorName = detectorName(0, detectorName.Length() - 2);
        detectorName.ToLower();
        //cout<<"detectorName = "<<detectorName<<endl;

        if (el->GetNdaughters() == 0) continue;
        // get the Z coordinate of the node
        Int_t z_1, z_2;
        z_1 = z_2 = (Int_t) *(el->GetDaughter(0)->GetMatrix()->GetTranslation() + 2);
        for (Int_t j = 1; j < el->GetNdaughters(); j++)
        {
            Int_t z = (Int_t) *(el->GetDaughter(j)->GetMatrix()->GetTranslation() + 2);
            if (z_1 > z) z_1 = z;
            if (z_2 < z) z_2 = z;
        }

        if (detectorName == "silicon")
            order_of_detectors.push_back(make_tuple((z_2 + z_1) / 2, SILICON));
        else if (detectorName == "gems")
            order_of_detectors.push_back(make_tuple((z_2 + z_1) / 2, GEMS));
        else if (detectorName == "csc")
            order_of_detectors.push_back(make_tuple((z_2 + z_1) / 2, CSC));
        else if (detectorName == "tof400")
            order_of_detectors.push_back(make_tuple((z_2 + z_1) / 2, TOF1));
        else if (detectorName == "tof700")
            order_of_detectors.push_back(make_tuple((z_2 + z_1) / 2, TOF2));
        else if (detectorName == "dch")
        {
            if (z_2 - z_1 > 200)
            {
                order_of_detectors.push_back(make_tuple(z_1, DCH1));
                order_of_detectors.push_back(make_tuple(z_2, DCH2));
            }
            else order_of_detectors.push_back(make_tuple((z_2 + z_1) / 2, DCH));
        }
    }

    // sort by Z coordinate
    sort(order_of_detectors.begin(), order_of_detectors.end());

    for (const auto& i : order_of_detectors)
        vOrderOfDetectors.push_back(get<1>(i));

    return kSUCCESS;
}

void BmnGlobalTrackDraw::Exec(Option_t* option)
{
    if (!IsActive()) return;

    if (fVerbose > 1) cout << " BmnGlobalTrackDraw::Exec " << endl;

    Reset();

    for (Int_t i = 0; i < fTrackList->GetEntriesFast(); i++)
    {
        if (fVerbose > 1) cout << "BmnGlobalTrackDraw::Exec " << i << endl;

        BmnGlobalTrack* tr = (BmnGlobalTrack*) fTrackList->At(i);
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

        Int_t n = 0;
        auto addHitForSingleStation = [=](FairHit* pHit) mutable -> void
        {
            // add hit (point) to EVE path for this track
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

            if (fVerbose > 3) cout << "Path marker added " << path << endl;
            n++;
        };

        auto addHitsForMultiStations = [=](TClonesArray* fHitList, BmnTrack* pDetectorTrack, Int_t nHits) mutable -> void
        {
            // cycle: add hits (points) to EVE path for this track
            for (Int_t k = 0; k < nHits; k++)
            {
                FairHit* pHit = (FairHit*) fHitList->UncheckedAt(pDetectorTrack->GetHitIndex(k));
                addHitForSingleStation(pHit);
            }
        };

        // get upstream track for global track
        if (tr->GetUpstreamTrackIndex() > -1) //KG почему закомментировано
        {
            // BmnTrack* pUpstreamTrack = (BmnTrack*) fUpstreamTrackList->UncheckedAt(tr->GetUpstreamTrackIndex());
            // addHitsForMultiStations(fUpstreamTrackList, pUpstreamTrack, pUpstreamTrack->GetNHits());
        }

        for (const auto& id : vOrderOfDetectors)
        {
            switch (id)
            {
                // get Silicon track for global track
                case SILICON:
                    if (tr->GetSilTrackIndex() > -1)
                    {
                        BmnSiliconTrack* pSiliconTrack = (BmnSiliconTrack*) fSiliconTrackList->UncheckedAt(tr->GetSilTrackIndex());
                        addHitsForMultiStations(fSiliconHitList, pSiliconTrack, pSiliconTrack->GetNHits());
                    }
                    break;
                // get GEM track for global track
                case GEMS:
                    if (tr->GetGemTrackIndex() > -1)
                    {
                        BmnGemTrack* pGemTrack = (BmnGemTrack*) fGemTrackList->UncheckedAt(tr->GetGemTrackIndex());
                        addHitsForMultiStations(fGemHitList, pGemTrack, pGemTrack->GetNHits());
                    }
                    break;
                // add CSC hit
                case CSC:
                    if (tr->GetCscHitIndex(0) > -1)
                    {
                        FairHit* pHit = (FairHit*) fCscHitList->UncheckedAt(tr->GetCscHitIndex(0));
                        addHitForSingleStation(pHit);
                    }
                    if (tr->GetCscHitIndex(1) > -1)
                    {
                        FairHit* pHit = (FairHit*) fCscHitList->UncheckedAt(tr->GetCscHitIndex(1));
                        addHitForSingleStation(pHit);
                    }
                    break;
                // add TOF1 hit
                case TOF1:
                    if (tr->GetTof1HitIndex() > -1)
                    {
                        FairHit* pHit = (FairHit*) fTof1HitList->UncheckedAt(tr->GetTof1HitIndex());
                        addHitForSingleStation(pHit);
                    }
                    break;
                // add TOF2 hit
                case TOF2:
                    if (tr->GetTof2HitIndex() > -1)
                    {
                        FairHit* pHit = (FairHit*) fTof2HitList->UncheckedAt(tr->GetTof2HitIndex());
                        addHitForSingleStation(pHit);
                    }
                    break;
                // add DCH hit
                case DCH: //KG почему закомментировано?
                    if (tr->GetDch1TrackIndex() > -1)
                    {
                        // BmnDchTrack* pDchTrack = (BmnDchTrack*) fDchTrackList->UncheckedAt(tr->GetDchTrackIndex());
                        // addHitsForMultiStations(fDchTrackList, pDchTrack, pDchTrack->GetNHits());
                    }
                    break;
                // add DCH1 hit
                case DCH1://KG почему закомментировано?
                    if (tr->GetDch1TrackIndex() > -1)
                    {
                        // BmnDchTrack* pDchTrack = (BmnDchTrack*) fDchTrackList->UncheckedAt(tr->GetDch1TrackIndex());
                        // addHitsForMultiStations(fDchTrackList, pDchTrack, pDchTrack->GetNHits());
                    }
                    break;
                // add DCH2 hit
                case DCH2://KG почему закомментировано?
                    if (tr->GetDch1TrackIndex() > -1) {
                        // BmnDchTrack* pDchTrack = (BmnDchTrack*) fDchTrackList->UncheckedAt(tr->GetDch2TrackIndex());
                        // addHitsForMultiStations(fDchTrackList, pDchTrack, pDchTrack->GetNHits());
                    }
                    break;
                default:
                    cout << "[ WARNING ] Unknown detector ID" << endl;
                    break;
            }
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
