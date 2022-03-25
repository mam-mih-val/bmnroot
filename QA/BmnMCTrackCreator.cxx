/**
 * \file BmnMCTrackCreator.cxx
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \author Sergey Merts <sergey.merts@gmail.com> - modifications for BMN experiment
 * \date 2011-2014
 **/
#include "BmnMCTrackCreator.h"
#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "FairGeoNode.h"
#include "FairMCPoint.h"
#include "CbmGeoStsPar.h"
#include "CbmStsSensor.h"
#include "CbmStsDigiScheme.h"
#include "CbmGeoStsPar.h"
#include "CbmStsDigiPar.h"
#include "CbmMCTrack.h"
#include "CbmStsPoint.h"
#include "CbmBaseHit.h"
#include "TDatabasePDG.h"
#include "TGeoManager.h"
#include "TClonesArray.h"

using namespace std;

BmnMCTrackCreator::BmnMCTrackCreator(TString gem, TString sil, TString csc) :
fMCTracks(nullptr),
fNSiliconStations(0),
fSilPoints(nullptr),
fTof400Points(nullptr),
fTof700Points(nullptr),
fCscPoints(nullptr),
fDchPoints(nullptr),
fGemPoints(nullptr) {
    ReadDataBranches();
    fSilDetector = new BmnSiliconStationSet(sil);
    fGemDetector = new BmnGemStripStationSet(gem);
    fCscDetector = new BmnCSCStationSet(csc);
}

BmnMCTrackCreator::~BmnMCTrackCreator() {
    delete fGemDetector;
    delete fSilDetector;
    delete fCscDetector;
}

BmnMCTrackCreator* BmnMCTrackCreator::Instance(TString gem, TString sil, TString csc) {
    static BmnMCTrackCreator instance(gem, sil, csc);
    return &instance;
}

void BmnMCTrackCreator::Create() {
    fBmnMCTracks.clear();
    fNSiliconStations = fSilDetector->GetNStations();
    fNGemStations = fGemDetector->GetNStations();
    fNCscStations = fCscDetector->GetNStations();
    AddPoints(kSILICON, fSilPoints);
    AddPoints(kSSD, fSsdPoints);
    AddPoints(kGEM, fGemPoints);
    AddPoints(kTOF1, fTof400Points);
    AddPoints(kTOF, fTof700Points);
    AddPoints(kCSC, fCscPoints);
    AddPoints(kDCH, fDchPoints);
    // printf("fSilDetector->GetNStations() = %d\n", fSilDetector->GetNStations());
    // printf("fGemDetector->GetNStations() = %d\n", fGemDetector->GetNStations());
    
    //   std::cout << "BmnMCTrackCreator: nof MC tracks=" << fBmnMCTracks.size() << std::endl;
    //   std::map<Int_t, BmnMCTrack>::iterator it;
    //   for (it = fBmnMCTracks.begin(); it != fBmnMCTracks.end(); it++) {
    //       printf("n gem pnts = %d\n", (*it).second.GetNofPoints(kGEM));
    //       printf("n sil pnts = %d\n", (*it).second.GetNofPoints(kSILICON));
    //       //std::cout << (*it).first << " => " << (*it).second;
    //   }
}

void BmnMCTrackCreator::ReadDataBranches() {
    FairRootManager* ioman = FairRootManager::Instance();
    fMCTracks = (TClonesArray*) ioman->GetObject("MCTrack");
    fSilPoints = (TClonesArray*) ioman->GetObject("SiliconPoint");
    fSsdPoints = (TClonesArray*) ioman->GetObject("SSDPoint");
    fGemPoints = (TClonesArray*) ioman->GetObject("StsPoint");
    fTof400Points = (TClonesArray*) ioman->GetObject("TOF400Point");
    fTof700Points = (TClonesArray*) ioman->GetObject("TOF700Point");
    fCscPoints = (TClonesArray*) ioman->GetObject("CSCPoint");
    fDchPoints = (TClonesArray*) ioman->GetObject("DCHPoint");
}

void BmnMCTrackCreator::AddPoints(DetectorId detId, const TClonesArray* array) {
    if (!array) return;
    Int_t nofPoints = array->GetEntriesFast();
    for (Int_t iPoint = 0; iPoint < nofPoints; iPoint++) {
        FairMCPoint* fairPoint = (FairMCPoint*) (array->At(iPoint));
        BmnMCPoint bmnPoint;
        Int_t stationId = -1;
        if (detId == kSILICON) {
            stationId = fSilDetector->GetPointStationOwnership(fairPoint->GetZ());
            // printf("stSil = %d\n", stationId);
        } else if (detId == kSSD) {
            // BmnSSDHitProducer hp;
            // stationId = hp.DefineStationByZ(fairPoint->GetZ(), 0);
        } else if (detId == kGEM) {
            stationId = fGemDetector->GetPointStationOwnership(fairPoint->GetZ()) + fNSiliconStations;
        } else if (detId == kCSC) {
            stationId = fCscDetector->GetPointStationOwnership(fairPoint->GetZ()) + fNSiliconStations + fNGemStations;
        } else if (detId == kTOF1) {
            stationId = fNSiliconStations + fNGemStations + fNCscStations + 1;
        } else if (detId == kTOF) {
            stationId = fNSiliconStations + fNGemStations + fNCscStations + 2;
        } else if (detId == kDCH) {
            stationId = fNSiliconStations + fNGemStations + fNCscStations + 3;
        }
        
        if (stationId < 0) continue;
        FairMCPointCoordinatesAndMomentumToBmnMCPoint(fairPoint, &bmnPoint);
        FairMCPointToBmnMCPoint(fairPoint, &bmnPoint, iPoint, stationId);
        fBmnMCTracks[fairPoint->GetTrackID()].AddPoint(detId, bmnPoint);
    }
}

void BmnMCTrackCreator::FairMCPointToBmnMCPoint(const FairMCPoint* fairPoint, BmnMCPoint* bmnPoint, Int_t refId, Int_t stationId) {
    bmnPoint->SetRefId(refId);
    bmnPoint->SetStationId(stationId);
    const CbmMCTrack* mcTrack = (const CbmMCTrack*) (fMCTracks->At(fairPoint->GetTrackID()));
    if (mcTrack == nullptr) return; //FIXME! it shouldn't happen
    TParticlePDG* pdgParticle = TDatabasePDG::Instance()->GetParticle(mcTrack->GetPdgCode());
    Float_t charge = (pdgParticle != nullptr) ? pdgParticle->Charge() : 0.;
    Float_t q = (charge > 0) ? 1. : -1.;
    bmnPoint->SetQ(charge / 3); //Потому что, сука, кварки!
}

void BmnMCTrackCreator::FairMCPointCoordinatesAndMomentumToBmnMCPoint(const FairMCPoint* fairPoint, BmnMCPoint* bmnPoint) {
    bmnPoint->SetXIn(fairPoint->GetX());
    bmnPoint->SetYIn(fairPoint->GetY());
    bmnPoint->SetZIn(fairPoint->GetZ());
    bmnPoint->SetPxIn(fairPoint->GetPx());
    bmnPoint->SetPyIn(fairPoint->GetPy());
    bmnPoint->SetPzIn(fairPoint->GetPz());
    bmnPoint->SetXOut(fairPoint->GetX());
    bmnPoint->SetYOut(fairPoint->GetY());
    bmnPoint->SetZOut(fairPoint->GetZ());
    bmnPoint->SetPxOut(fairPoint->GetPx());
    bmnPoint->SetPyOut(fairPoint->GetPy());
    bmnPoint->SetPzOut(fairPoint->GetPz());
}

