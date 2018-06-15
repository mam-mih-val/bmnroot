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
#include "CbmMvdPoint.h"
#include "CbmGeoStsPar.h"
#include "CbmStsSensor.h"
#include "CbmStsDigiScheme.h"
#include "CbmGeoStsPar.h"
#include "CbmStsDigiPar.h"
#include "CbmMCTrack.h"
#include "CbmStsPoint.h"
#include "CbmMvdPoint.h"
#include "CbmBaseHit.h"
#include "TDatabasePDG.h"
#include "TGeoManager.h"
#include "TClonesArray.h"

using namespace std;

BmnMCTrackCreator::BmnMCTrackCreator() :
fMCTracks(NULL),
fMvdPoints(NULL),
fGemPoints(NULL),
fTof1Points(NULL),
fTof2Points(NULL),
fDchPoints(NULL) {
    ReadDataBranches();
    TString gPathSiliconConfig = TString(gSystem->Getenv("VMCWORKDIR")) + "/silicon/XMLConfigs/SiliconRunSpring2017.xml";
    fSilDetector = new BmnSiliconStationSet(gPathSiliconConfig);
    fGemDetector = new BmnGemStripStationSet_RunSpring2017(BmnGemStripConfiguration::RunSpring2017);
}

BmnMCTrackCreator::~BmnMCTrackCreator() {
    delete fGemDetector;
    delete fSilDetector;
}

BmnMCTrackCreator* BmnMCTrackCreator::Instance() {
    static BmnMCTrackCreator instance;
    return &instance;
}

void BmnMCTrackCreator::Create() {
    FillStationMaps();
    fBmnMCTracks.clear();
    //AddPoints(kMVD, fMvdPoints);
    AddPoints(kSILICON, fSilPoints);
    AddPoints(kGEM, fGemPoints);
    AddPoints(kTOF1, fTof1Points);
    AddPoints(kDCH, fDchPoints);
    AddPoints(kTOF, fTof2Points);
    std::map<Int_t, BmnMCTrack>::iterator it;
    //    for (it = fBmnMCTracks.begin(); it != fBmnMCTracks.end(); it++)
    //        it->second.CalculateNofConsecutivePoints();

    //   std::cout << "BmnMCTrackCreator: nof MC tracks=" << fLitMCTracks.size() << std::endl;
    //   std::map<Int_t, BmnMCTrack>::iterator it;
    //   for (it = fLitMCTracks.begin(); it != fLitMCTracks.end(); it++)
    //       std::cout << (*it).first << " => " << (*it).second;
}

void BmnMCTrackCreator::ReadDataBranches() {
    FairRootManager* ioman = FairRootManager::Instance();
    fMCTracks = (TClonesArray*) ioman->GetObject("MCTrack");
    //   fMvdPoints = (TClonesArray*) ioman->GetObject("MvdPoint");
    fSilPoints = (TClonesArray*) ioman->GetObject("SiliconPoint");
    fGemPoints = (TClonesArray*) ioman->GetObject("StsPoint");
    fTof1Points = (TClonesArray*) ioman->GetObject("TOF1Point");
    fTof2Points = (TClonesArray*) ioman->GetObject("TofPoint");
    fDchPoints = (TClonesArray*) ioman->GetObject("DCHPoint");
}

void BmnMCTrackCreator::AddPoints(DetectorId detId, const TClonesArray* array) {
    if (!array) return;
    Int_t nofPoints = array->GetEntriesFast();
    for (Int_t iPoint = 0; iPoint < nofPoints; iPoint++) {
        FairMCPoint* fairPoint = (FairMCPoint*) (array->At(iPoint));
        BmnMCPoint bmnPoint;
        Int_t stationId = -1;
        if (detId == kMVD) {
            //         stationId = fMvdStationsMap[iPoint];
            //         MvdPointCoordinatesAndMomentumToLitMCPoint(static_cast<CbmMvdPoint*>(fairPoint), &litPoint);
        } else if (detId == kSILICON) {
            stationId = fSilDetector->GetPointStationOwnership(fairPoint->GetZ());
            FairMCPointCoordinatesAndMomentumToBmnMCPoint(fairPoint, &bmnPoint);
        } else if (detId == kGEM) {
            stationId = fGemDetector->GetPointStationOwnership(fairPoint->GetZ());
            GemPointCoordinatesAndMomentumToBmnMCPoint((CbmStsPoint*) (fairPoint), &bmnPoint);
        } else {
            stationId = 0;
            FairMCPointCoordinatesAndMomentumToBmnMCPoint(fairPoint, &bmnPoint);
        }
        if (stationId < 0) continue;
        FairMCPointToBmnMCPoint(fairPoint, &bmnPoint, iPoint, stationId);
        fBmnMCTracks[fairPoint->GetTrackID()].AddPoint(detId, bmnPoint);
    }
}

void BmnMCTrackCreator::FairMCPointToBmnMCPoint(const FairMCPoint* fairPoint, BmnMCPoint* bmnPoint, Int_t refId, Int_t stationId) {
    bmnPoint->SetRefId(refId);
    bmnPoint->SetStationId(stationId);
    const CbmMCTrack* mcTrack = (const CbmMCTrack*) (fMCTracks->At(fairPoint->GetTrackID()));
    if (mcTrack == NULL) return; //FIXME! it shouldn't happen
    TParticlePDG* pdgParticle = TDatabasePDG::Instance()->GetParticle(mcTrack->GetPdgCode());
    Float_t charge = (pdgParticle != NULL) ? pdgParticle->Charge() : 0.;
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

void BmnMCTrackCreator::GemPointCoordinatesAndMomentumToBmnMCPoint(const CbmStsPoint* gemPoint, BmnMCPoint* bmnPoint) {
    bmnPoint->SetXIn(gemPoint->GetXIn());
    bmnPoint->SetYIn(gemPoint->GetYIn());
    bmnPoint->SetZIn(gemPoint->GetZIn());
    bmnPoint->SetPxIn(gemPoint->GetPx());
    bmnPoint->SetPyIn(gemPoint->GetPy());
    bmnPoint->SetPzIn(gemPoint->GetPz());
    bmnPoint->SetXOut(gemPoint->GetXOut());
    bmnPoint->SetYOut(gemPoint->GetYOut());
    bmnPoint->SetZOut(gemPoint->GetZOut());
    bmnPoint->SetPxOut(gemPoint->GetPxOut());
    bmnPoint->SetPyOut(gemPoint->GetPyOut());
    bmnPoint->SetPzOut(gemPoint->GetPzOut());
}

void BmnMCTrackCreator::FillStationMaps() {
    fGemStationsMap.clear();
    // GEM
    if (NULL != fGemPoints) {
        //FIXME do we need  this approach to get station numbers? --->
        //        FairRunAna* run = FairRunAna::Instance();
        //        FairRuntimeDb* runDb = run->GetRuntimeDb();
        //        CbmGeoStsPar* stsGeoPar = (CbmGeoStsPar*) runDb->getContainer("CbmGeoStsPar");
        //        TObjArray* stsNodes = stsGeoPar->GetGeoSensitiveNodes();
        //        Int_t nofStsStations = stsNodes->GetEntries();
        //        std::map<Int_t, Int_t> stsStationNrFromMcId;
        //        std::cout << "nofStations=" << nofStsStations << std::endl;
        //        for (Int_t ist = 0; ist < nofStsStations; ist++) {
        //            FairGeoNode* stsNode = (FairGeoNode*) stsNodes->At(ist);
        //            std::string stsNodeName(stsNode->GetName());
        //            std::cout << "stsNode:" << stsNode->GetName() << std::endl;
        //            std::string stsStationNr = stsNodeName.substr(10, 2);
        //            Int_t stationNr = atoi(stsStationNr.c_str());
        //            stsStationNrFromMcId[stsNode->getMCid()] = stationNr - 1;
        //        }
        //
        //        CbmStsDetectorId stsDetectorId;
        //        Int_t nofStsPoints = fGemPoints->GetEntriesFast();
        //        for (Int_t iPoint = 0; iPoint < nofStsPoints; iPoint++) {
        //            const FairMCPoint* point = (const FairMCPoint*) (fGemPoints->At(iPoint));
        //            Int_t stationId = stsStationNrFromMcId[point->GetDetectorID()];
        //            fGemStationsMap[iPoint] = stationId;
        //        }
        //<---

        //        for (Int_t iPoint = 0; iPoint < fGemPoints->GetEntriesFast(); iPoint++) {
        //            const CbmStsPoint* point = (const CbmStsPoint*) (fGemPoints->At(iPoint));
        //            Float_t xin = point->GetXIn();
        //            Float_t yin = point->GetYIn();
        //            Float_t zin = point->GetZIn();
        //            gGeoManager->FindNode(xin, yin, zin);
        //            TGeoNode* curNode = gGeoManager->GetCurrentNode(); // only needed for old geometries
        //            CbmStsSensor* sensor = (fStsDigiScheme->IsNewGeometry()) ?
        //                    fStsDigiScheme->GetSensorByName(fStsDigiScheme->GetCurrentPath()) :
        //                    fStsDigiScheme->GetSensorByName(curNode->GetName());
        //            if (sensor != NULL) {
        //                Int_t stationId = sensor->GetStationNr() - 1;
        //                fGemStationsMap[iPoint] = stationId;
        //            }
        //        }
    } // end GEM
}
