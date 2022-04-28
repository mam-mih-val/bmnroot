/**
 * \file BmnClusteringQa.cxx
 * \brief FairTask for clustering performance calculation.
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2011
 */

#include "BmnClusteringQa.h"
#include "BmnClusteringQaReport.h"
#include "BmnHistManager.h"
#include "BmnHit.h"
#include "FairRootManager.h"
#include "FairMCPoint.h"
#include "CbmMCTrack.h"
#include "BmnMatch.h"
#include "FairMCEventHeader.h"
#include "BmnMath.h"
#include "BmnGemStripStation.h"
#include "BmnGemStripStationSet_RunSpring2017.h"
#include "BmnSiliconStationSet.h"
//#include "CbmStsAddress.h"

#include "TSystem.h"
#include "TClonesArray.h"
#include "TH1.h"
#include "TF1.h"
#include "TH1D.h"
#include "TH2.h"
#include "TProfile.h"
#include "TProfile2D.h"

#include <boost/assign/list_of.hpp>
#include <fstream>
#include <cmath>
#include <sstream>
using std::cout;
using std::vector;
using std::map;
using std::binary_search;
using boost::assign::list_of;
using namespace TMath;

BmnClusteringQa::BmnClusteringQa() :
fHM(NULL),
fOutputDir("./"),
fMCTracks(NULL),
fGemPoints(NULL),
fGemDigits(NULL),
//fGemClusters(NULL),
fGemHits(NULL),
fGemDigitMatches(NULL),
//fGemClusterMatches(NULL),
fGemHitMatches(NULL),
fPrimes(kFALSE),
fGemDetector(NULL),
//fTof1Points(NULL),
//fTof1Hits(NULL),
//fTof1HitMatches(NULL),
//fTof2Points(NULL),
//fTof2Hits(NULL),
//fTof2HitMatches(NULL),
fDch1Points(NULL),
fDch1Hits(NULL),
fDch1HitMatches(NULL)
//fDch2Points(NULL),
//fDch2Hits(NULL),
//fDch2HitMatches(NULL)
{
}

BmnClusteringQa::~BmnClusteringQa() {
    if (fHM) delete fHM;
}

InitStatus BmnClusteringQa::Init() {
    // Create histogram manager which is used throughout the program
    fHM = new BmnHistManager();
    TString gPathConfig = gSystem->Getenv("VMCWORKDIR");
    TString gPathSiConfig = gPathConfig + "/silicon/XMLConfigs/";
    TString confSi = "SiliconRunSpring2018.xml";
    fSilDetector = new BmnSiliconStationSet(gPathSiConfig + confSi);
    TString gPathGemConfig = gPathConfig + "/gem/XMLConfigs/";
    TString confGem = "GemRunSpring2018.xml";
    fGemDetector = new BmnGemStripStationSet(gPathGemConfig + confGem);

    ReadDataBranches();

    CreateHistograms();
    return kSUCCESS;
}

void BmnClusteringQa::Exec(Option_t* opt) {
    //FillEventCounterHistograms();
    fHM->H1("hen_EventNo_ClusteringQa")->Fill(0.5);
    ReadEventHeader();

    ProcessPoints(fGemPoints, "Gem", kGEM);
    //    ProcessPoints(fTof1Points, "Tof1", kTOF1);
    //    ProcessPoints(fDch1Points, "Dch1", kDCH1);
    //    ProcessPoints(fDch2Points, "Dch2", kDCH2);
    //    ProcessPoints(fTof2Points, "Tof2", kTOF);

    //    ProcessDigis(fGemDigis, fGemDigiMatches, "Gem", kGEM);
    //    ProcessClusters(fGemClusters, fGemClusterMatches, "Gem", kGEM);

    ProcessHits(fGemHits, fGemHitMatches, "Gem", kGEM);
    //    ProcessHits(fTof1Hits, fTof1HitMatches, "Tof1", kTOF1);
    //    ProcessHits(fDch1Hits, fDch1HitMatches, "Dch1", kDCH1);
    //    ProcessHits(fDch2Hits, fDch2HitMatches, "Dch2", kDCH2);
    //    ProcessHits(fTof2Hits, fTof2HitMatches, "Tof2", kTOF);

    FillResidualAndPullHistograms(fGemPoints, fGemHits, fGemHitMatches, "Gem", kGEM);
    //    FillResidualAndPullHistograms(fTof1Points, fTof1Hits, fTof1HitMatches, "Tof1", kTOF1);
    //    FillResidualAndPullHistograms(fDch1Points, fDch1Hits, fDch1HitMatches, "Dch1", kDCH1);
    //    FillResidualAndPullHistograms(fDch2Points, fDch2Hits, fDch2HitMatches, "Dch2", kDCH2);
    //    FillResidualAndPullHistograms(fTof2Points, fTof2Hits, fTof2HitMatches, "Tof2", kTOF);

    FillHitEfficiencyHistograms(fGemPoints, fGemHits, fGemHitMatches, "Gem", kGEM);
    //    FillHitEfficiencyHistograms(fTof1Points, fTof1Hits, fTof1HitMatches, "Tof1", kTOF1);
    //    FillHitEfficiencyHistograms(fDch1Points, fDch1Hits, fDch1HitMatches, "Dch1", kDCH1);
    //    FillHitEfficiencyHistograms(fDch2Points, fDch2Hits, fDch2HitMatches, "Dch2", kDCH2);
    //    FillHitEfficiencyHistograms(fTof2Points, fTof2Hits, fTof2HitMatches, "Tof2", kTOF);

    //    std::cout << "BmnClusteringQa::Exec: event=" << fHM->H1("hen_EventNo_ClusteringQa")->GetEntries() << std::endl;


}

void BmnClusteringQa::Finish() {
    fHM->WriteToFile();
    //    BmnSimulationReport* report = new BmnClusteringQaReport(fGemDetector->GetNStations(), 1);
    BmnSimulationReport* report = new BmnClusteringQaReport(fGemDetector->GetNStations(), fSilDetector->GetNStations());
    report->Create(fHM, fOutputDir);
    delete report;
}

void BmnClusteringQa::ReadEventHeader() {
    FairMCEventHeader* evHead = (FairMCEventHeader*) FairRootManager::Instance()->GetObject("MCEventHeader.");
    fHM->H1("Impact parameter")->Fill(evHead->GetB());
    //fHM->H1("Impact parameter")->Fill(evHead->GetRotZ());
    fHM->H1("Multiplicity")->Fill(evHead->GetNPrim());
    fHM->H2("Impact_Mult")->Fill(evHead->GetB(), evHead->GetNPrim());
}

void BmnClusteringQa::ReadDataBranches() {
    FairRootManager* ioman = FairRootManager::Instance();
    assert(ioman != NULL);

    fMCTracks = (TClonesArray*) ioman->GetObject("MCTrack");

    fGemPoints = (TClonesArray*) ioman->GetObject("StsPoint");
    fSilPoints = (TClonesArray*) ioman->GetObject("SiliconPoint");
    //    fTof1Points = (TClonesArray*) ioman->GetObject("TOF400Point");
    //    fTof2Points = (TClonesArray*) ioman->GetObject("TOF700Point");
    fDch1Points = (TClonesArray*) ioman->GetObject("DCHPoint");
    //    fDch2Points = (TClonesArray*) ioman->GetObject("DCH2Point");

    fGemHits = (TClonesArray*) ioman->GetObject("BmnGemStripHit");
    fSilHits = (TClonesArray*) ioman->GetObject("BmnSiliconHit");
    //    fTof1Hits = (TClonesArray*) ioman->GetObject("TOF1Hit");
    //    fTof2Hits = (TClonesArray*) ioman->GetObject("BmnTof2Hit");
    //fDch1Hits = (TClonesArray*) ioman->GetObject("BmnDchHit");
    //fDch2Hits = (TClonesArray*) ioman->GetObject("BmnDch2Hit0");

    fGemHitMatches = (TClonesArray*) ioman->GetObject("BmnGemStripHitMatch");
    fSilHitMatches = (TClonesArray*) ioman->GetObject("BmnSiliconHitMatch");
    //    fTof1HitMatches = (TClonesArray*) ioman->GetObject("Tof1HitMatch");
    //    fTof2HitMatches = (TClonesArray*) ioman->GetObject("Tof2HitMatch");
    //    fDch1HitMatches = (TClonesArray*) ioman->GetObject("Dch1HitMatch");
    //    fDch2HitMatches = (TClonesArray*) ioman->GetObject("Dch2HitMatch");

}

Int_t BmnClusteringQa::GetStationId(Int_t address, DetectorId detId) {
    //if (detId == kGEM) return CbmStsAddress::GetElementId(address, kGEMStation);    //FIXME!!!!!!!!!!!!!
    return 0;
}

void BmnClusteringQa::ProcessPoints(const TClonesArray* points, const string& detName, DetectorId detId) {
    string histName = "_hno_NofObjects_" + detName + "Points_Station";
    if (NULL == points || !fHM->Exists(histName)) return;
    for (Int_t i = 0; i < points->GetEntriesFast(); i++) {
        const FairMCPoint* point = (const FairMCPoint*) (points->At(i));
//        Int_t station = stationNumber("Gem", point->GetZ());
//        fHM->H1(histName)->Fill(station); //FIXME! Get Station ID from point!
    }
}

void BmnClusteringQa::ProcessDigis(
        const TClonesArray* digis,
        const TClonesArray* digiMatches,
        const string& detName,
        DetectorId detId) {
    //    if (NULL == digis || !fHM->Exists("hno_NofObjects_" + detName + "Digis_Station")) return;
    //    for (Int_t i = 0; i < digis->GetEntriesFast(); i++) {
    //        const CbmDigi* digi = static_cast<const CbmDigi*> (digis->At(i));
    //        const BmnMatch* digiMatch = static_cast<const BmnMatch*> (digiMatches->At(i));
    //        Int_t stationId = GetStationId(digi->GetAddress(), detId);
    //        fHM->H1("hno_NofObjects_" + detName + "Digis_Station")->Fill(stationId);
    //        fHM->H1("hpa_" + detName + "Digi_NofPointsInDigi_H1")->Fill(digiMatch->GetNofLinks());
    //        fHM->H1("hpa_" + detName + "Digi_NofPointsInDigi_H2")->Fill(stationId, digiMatch->GetNofLinks());
    //    }
}

void BmnClusteringQa::ProcessClusters(
        const TClonesArray* clusters,
        const TClonesArray* clusterMatches,
        const string& detName,
        DetectorId detId) {
    //    if (NULL != clusters && fHM->Exists("hno_NofObjects_" + detName + "Clusters_Station")) {
    //        for (Int_t i = 0; i < clusters->GetEntriesFast(); i++) {
    //            const CbmCluster* cluster = static_cast<const CbmCluster*> (clusters->At(i));
    //            const BmnMatch* clusterMatch = static_cast<const BmnMatch*> (clusterMatches->At(i));
    //            Int_t stationId = GetStationId(cluster->GetAddress(), detId);
    //            fHM->H1("hno_NofObjects_" + detName + "Clusters_Station")->Fill(stationId);
    //            fHM->H1("hpa_" + detName + "Cluster_NofDigisInCluster_H1")->Fill(cluster->GetNofDigis());
    //            fHM->H1("hpa_" + detName + "Cluster_NofDigisInCluster_H2")->Fill(stationId, cluster->GetNofDigis());
    //            fHM->H1("hpa_" + detName + "Cluster_NofPointsInCluster_H1")->Fill(clusterMatch->GetNofLinks());
    //            fHM->H1("hpa_" + detName + "Cluster_NofPointsInCluster_H2")->Fill(stationId, clusterMatch->GetNofLinks());
    //        }
    //    }
}

void BmnClusteringQa::ProcessHits(const TClonesArray* hits, const TClonesArray* hitMatches, const string& detName, DetectorId detId) {
//    if (NULL != hits && fHM->Exists("_hno_NofObjects_" + detName + "Hits_Station")) {
//        for (Int_t i = 0; i < hits->GetEntriesFast(); i++) {
//            const BmnMatch* hitMatch = (const BmnMatch*) (hitMatches->At(i));
//            const BmnHit* hit = (const BmnHit*) (hits->At(i));
//            Int_t stationId = hit->GetStation();
//            fHM->H1("_hno_NofObjects_" + detName + "Hits_Station")->Fill(stationId);
//            fHM->H1("_hpa_" + detName + "Hit_SigmaX_H1")->Fill(hit->GetDx());
//            fHM->H1("_hpa_" + detName + "Hit_SigmaX_H2")->Fill(stationId, hit->GetDx());
//            fHM->H1("_hpa_" + detName + "Hit_SigmaY_H1")->Fill(hit->GetDy());
//            fHM->H1("_hpa_" + detName + "Hit_SigmaY_H2")->Fill(stationId, hit->GetDy());
//            fHM->H1("_hpa_" + detName + "Hit_NofPointsInHit_H1")->Fill(hitMatch->GetNofLinks());
//            fHM->H1("_hpa_" + detName + "Hit_NofPointsInHit_H2")->Fill(stationId, hitMatch->GetNofLinks());
//        }
//    }

    for (Int_t iSt = 0; iSt < fGemDetector->GetNStations(); ++iSt) {
        TString resXname = Form("ResX_%dst_gem", iSt);
        TString resYname = Form("ResY_%dst_gem", iSt);
        TString pntXhitXname = Form("PntX_vs_HitX_%dst_gem", iSt);
        TString pntYhitYname = Form("PntY_vs_HitY_%dst_gem", iSt);
        TString occupname = Form("Occupancy_%dst_gem", iSt);
        TString pullXname = Form("PullX_%dst_gem", iSt);
        TString pullYname = Form("PullY_%dst_gem", iSt);

        /*for (Int_t i = 0; i < fGemPoints->GetEntriesFast(); ++i) {
              const FairMCPoint* pnt = (const FairMCPoint*) (fGemPoints->At(i));
              //const FairMCPoint* pnt = (const FairMCPoint*) (fGemPoints->At(hitMatch->GetMatchedLink().GetIndex()));
//            Int_t station = stationNumber("Gem", pnt->GetZ());
//            if (station != iSt) continue;
              fHM->H2(occupname.Data())->Fill(pnt->GetX(), pnt->GetY());
        }*/

        for (Int_t i = 0; i < fGemHits->GetEntriesFast(); ++i) {
            const BmnHit* hit = (const BmnHit*) (fGemHits->At(i));
            if (hit->GetStation() != iSt) continue;
            const BmnMatch* hitMatch = (const BmnMatch*) (fGemHitMatches->At(i));
            const FairMCPoint* pnt = (const FairMCPoint*) (fGemPoints->At(hitMatch->GetMatchedLink().GetIndex()));
            const Float_t resX = pnt->GetX() - hit->GetX();
            const Float_t resY = pnt->GetY() - hit->GetY();
            fHM->H1(resXname.Data())->Fill(resX);
            fHM->H1(resYname.Data())->Fill(resY);
            fHM->H2(pntXhitXname.Data())->Fill(pnt->GetX(), hit->GetX());
            fHM->H2(pntYhitYname.Data())->Fill(pnt->GetY(), hit->GetY());

            fHM->H1(pullXname)->Fill(resX / hit->GetDx());
            fHM->H1(pullYname)->Fill(resY / hit->GetDy());


            fHM->H2(occupname.Data())->Fill(pnt->GetX(), pnt->GetY());
        }
    }

    for (Int_t iSt = 0; iSt < fSilDetector->GetNStations(); ++iSt) {
    //for (Int_t iSt = 0; iSt < 1; ++iSt){
        TString occupname = Form("Occupancy_%dst_sil", iSt);
        TString resXname = Form("ResX_%dst_sil", iSt);
        TString resYname = Form("ResY_%dst_sil", iSt);
        TString pntXhitXname = Form("PntX_vs_HitX_%dst_sil", iSt);
        TString pntYhitYname = Form("PntY_vs_HitY_%dst_sil", iSt);
        TString pullXname = Form("PullX_%dst_sil", iSt);
        TString pullYname = Form("PullY_%dst_sil", iSt);


        for (Int_t i = 0; i < fSilHits->GetEntriesFast(); ++i) {
            const BmnHit* hit = (const BmnHit*) (fSilHits->At(i));
            if (hit->GetStation() != iSt) continue;
            const BmnMatch* hitMatch = (const BmnMatch*) (fSilHitMatches->At(i));
            const FairMCPoint* pnt = (const FairMCPoint*) (fSilPoints->At(hitMatch->GetMatchedLink().GetIndex()));

            const Float_t resX = pnt->GetX() - hit->GetX();
            const Float_t resY = pnt->GetY() - hit->GetY();

            fHM->H1(resXname.Data())->Fill(resX);
            fHM->H1(resYname.Data())->Fill(resY);
            fHM->H2(pntXhitXname.Data())->Fill(pnt->GetX(), hit->GetX());
            fHM->H2(pntYhitYname.Data())->Fill(pnt->GetY(), hit->GetY());

            fHM->H1(pullXname)->Fill(resX / hit->GetDx());
            fHM->H1(pullYname)->Fill(resY / hit->GetDy());
            
            fHM->H2(occupname.Data())->Fill(pnt->GetX(), pnt->GetY());
        }


    }

//    for (Int_t iSt = 0; iSt < 3; ++iSt){
//        TString occupname = Form("Occupancy_%dst_dch1", iSt);
//
//        for (Int_t i = 0; i < fDch1Hits->GetEntriesFast(); ++i) {
//            printf("%d  Number of Dch points \n",fDch1Hits->GetEntriesFast());
//            const BmnHit* hit = (const BmnHit*) (fDch1Hits->At(i));
//            if (hit->GetStation() != iSt) continue;
//            const BmnMatch* hitMatch = (const BmnMatch*) (fDch1HitMatches->At(i));
//            const FairMCPoint* pnt = (const FairMCPoint*) (fDch1Points->At(hitMatch->GetMatchedLink().GetIndex()));
//
//            fHM->H2(occupname.Data())->Fill(pnt->GetX(), pnt->GetY());
//        }
//    }
}

void BmnClusteringQa::FillEventCounterHistograms() {
    //    if (NULL != fMvdPoints && fHM->Exists("hno_NofObjects_MvdPoints_Event")) fHM->H1("hno_NofObjects_MvdPoints_Event")->Fill(fMvdPoints->GetEntriesFast());
    //    if (NULL != fMvdDigis && fHM->Exists("hno_NofObjects_MvdDigis_Event")) fHM->H1("hno_NofObjects_MvdDigis_Event")->Fill(fMvdDigis->GetEntriesFast());
    //    if (NULL != fMvdClusters && fHM->Exists("hno_NofObjects_MvdClusters_Event")) fHM->H1("hno_NofObjects_MvdClusters_Event")->Fill(fMvdClusters->GetEntriesFast());
    //    if (NULL != fMvdHits && fHM->Exists("hno_NofObjects_MvdHits_Event")) fHM->H1("hno_NofObjects_MvdHits_Event")->Fill(fMvdHits->GetEntriesFast());

//    if (NULL != fGemPoints && fHM->Exists("_hno_NofObjects_GemPoints_Event")) fHM->H1("_hno_NofObjects_GemPoints_Event")->Fill(fGemPoints->GetEntriesFast());
//    if (NULL != fGemDigits && fHM->Exists("hno_NofObjects_StsDigis_Event")) fHM->H1("hno_NofObjects_StsDigis_Event")->Fill(fGemDigits->GetEntriesFast());
    //    if (NULL != fGemClusters && fHM->Exists("hno_NofObjects_StsClusters_Event")) fHM->H1("hno_NofObjects_StsClusters_Event")->Fill(fGemClusters->GetEntriesFast());
//    if (NULL != fGemHits && fHM->Exists("_hno_NofObjects_GemHits_Event")) fHM->H1("_hno_NofObjects_GemHits_Event")->Fill(fGemHits->GetEntriesFast());

    //    if (NULL != fTof1Points && fHM->Exists("_hno_NofObjects_Tof1Points_Event")) fHM->H1("_hno_NofObjects_Tof1Points_Event")->Fill(fTof1Points->GetEntriesFast());
    //    if (NULL != fTof2Points && fHM->Exists("_hno_NofObjects_Tof2Points_Event")) fHM->H1("_hno_NofObjects_Tof2Points_Event")->Fill(fTof2Points->GetEntriesFast());
//    if (NULL != fDch1Points && fHM->Exists("_hno_NofObjects_Dch1Points_Event")) fHM->H1("_hno_NofObjects_Dch1Points_Event")->Fill(fDch1Points->GetEntriesFast());
    //    if (NULL != fDch2Points && fHM->Exists("_hno_NofObjects_Dch2Points_Event")) fHM->H1("_hno_NofObjects_Dch2Points_Event")->Fill(fDch2Points->GetEntriesFast());
    //
    //    if (NULL != fTof1Hits && fHM->Exists("_hno_NofObjects_Tof1Hits_Event")) fHM->H1("_hno_NofObjects_Tof1Hits_Event")->Fill(fTof1Hits->GetEntriesFast());
    //    if (NULL != fTof2Hits && fHM->Exists("_hno_NofObjects_Tof2Hits_Event")) fHM->H1("_hno_NofObjects_Tof2Hits_Event")->Fill(fTof2Hits->GetEntriesFast());
    //    if (NULL != fDch1Hits && fHM->Exists("_hno_NofObjects_Dch1Hits_Event")) fHM->H1("_hno_NofObjects_Dch1Hits_Event")->Fill(fDch1Hits->GetEntriesFast());
    //    if (NULL != fDch2Hits && fHM->Exists("_hno_NofObjects_Dch2Hits_Event")) fHM->H1("_hno_NofObjects_Dch2Hits_Event")->Fill(fDch2Hits->GetEntriesFast());
}

void BmnClusteringQa::FillResidualAndPullHistograms(const TClonesArray* points, const TClonesArray* hits, const TClonesArray* hitMatches, const string& detName, DetectorId detId) {
    if (NULL == points || NULL == hits || NULL == hitMatches) return;
    string nameResidualX = "_hrp_" + detName + "_ResidualX_H2";
    string nameResidualY = "_hrp_" + detName + "_ResidualY_H2";
    string namePullX = "_hrp_" + detName + "_PullX_H2";
    string namePullY = "_hrp_" + detName + "_PullY_H2";
    if (!fHM->Exists(nameResidualX) || !fHM->Exists(nameResidualY) || !fHM->Exists(namePullX) || !fHM->Exists(namePullY)) return;

    for (Int_t iHit = 0; iHit < hits->GetEntriesFast(); iHit++) {
        const BmnMatch* match = (const BmnMatch*) (hitMatches->At(iHit));
        const FairMCPoint* point = (const FairMCPoint*) (points->At(match->GetMatchedLink().GetIndex()));
        if (point == NULL) continue;
        const BmnHit* hit = (const BmnHit*) (hits->At(iHit));
        Float_t residualX = point->GetX() - hit->GetX();
        Float_t residualY = point->GetY() - hit->GetY();
        Int_t stationId = hit->GetStation();
        fHM->H2(nameResidualX)->Fill(stationId, residualX);
        fHM->H2(nameResidualY)->Fill(stationId, residualY);
        fHM->H2(namePullX)->Fill(stationId, residualX / hit->GetDx());
        fHM->H2(namePullY)->Fill(stationId, residualY / hit->GetDy());
    }
}

void BmnClusteringQa::FillHitEfficiencyHistograms(const TClonesArray* points, const TClonesArray* hits, const TClonesArray* hitMatches, const string& detName, DetectorId detId) {
    if (NULL == points || NULL == hits || NULL == hitMatches) return;
    string accName = "_hhe_" + detName + "_All_Acc_Station";
    if (NULL == points || !fHM->Exists(accName)) return;
    for (Int_t i = 0; i < points->GetEntriesFast(); i++) {
        const FairMCPoint* point = (const FairMCPoint*) (points->At(i));
        fHM->H1(accName)->Fill(GetStationId(point->GetDetectorID(), detId));
    }

    string recName = "_hhe_" + detName + "_All_Rec_Station";
    string cloneName = "_hhe_" + detName + "_All_Clone_Station";
    set<Int_t> mcPointSet; // IDs of MC points
    Int_t nofHits = hits->GetEntriesFast();
    for (Int_t iHit = 0; iHit < nofHits; iHit++) {
        const BmnMatch* match = (const BmnMatch*) (hitMatches->At(iHit));
        const BmnHit* hit = (const BmnHit*) (hits->At(iHit));
        if (mcPointSet.find(match->GetMatchedLink().GetIndex()) == mcPointSet.end()) {
            fHM->H1(recName)->Fill(hit->GetStation());
            mcPointSet.insert(match->GetMatchedLink().GetIndex());
        } else {
            fHM->H1(cloneName)->Fill(hit->GetStation());
        }
    }
}

void BmnClusteringQa::CreateHistograms() {

    Int_t nStationsGEM = fGemDetector->GetNStations();
    Int_t nStationsSil = fSilDetector->GetNStations();
    Int_t nStationsDCH1 = 3;

    for (Int_t iSt = 0; iSt < nStationsGEM; ++iSt) {
        TString occupname = Form("Occupancy_%dst_gem", iSt);
        TString resXname = Form("ResX_%dst_gem", iSt);
        TString resYname = Form("ResY_%dst_gem", iSt);
        TString pntXhitXname = Form("PntX_vs_HitX_%dst_gem", iSt);
        TString pntYhitYname = Form("PntY_vs_HitY_%dst_gem", iSt);
        TString pullXname = Form("PullX_%dst_gem", iSt);
        TString pullYname = Form("PullY_%dst_gem", iSt);

        CreateH1(resXname.Data(), "ResX, cm", "Counter", 200, -5, 5);
        CreateH1(resYname.Data(), "ResY, cm", "Counter", 200, -5, 5);
        CreateH2(pntXhitXname.Data(), "X_{MC}, cm", "X_{reco}, cm", "", 400, -70, 70, 400, -70, 70);
        CreateH2(pntYhitYname.Data(), "Y_{MC}, cm", "Y_{reco}, cm", "", 400, -70, 70, 400, -70, 70);

        CreateH2(occupname.Data(), "X_{MC}, cm", "Y_{MC}, cm", "Occupancy, #frac{part}{event * cm^{2}}", 50, -100, 100, 50, -30, 30);

        CreateH1(pullXname.Data(), "PullX", "Counter", 200, -10, 10);
        CreateH1(pullYname.Data(), "PullY", "Counter", 200, -10, 10);
    }

    for (Int_t iSt = 0; iSt < nStationsSil; ++iSt) {
        TString occupname = Form("Occupancy_%dst_sil", iSt);
        TString resXname = Form("ResX_%dst_sil", iSt);
        TString resYname = Form("ResY_%dst_sil", iSt);
        TString pntXhitXname = Form("PntX_vs_HitX_%dst_sil", iSt);
        TString pntYhitYname = Form("PntY_vs_HitY_%dst_sil", iSt);
        TString pullXname = Form("PullX_%dst_sil", iSt);
        TString pullYname = Form("PullY_%dst_sil", iSt);


        CreateH1(resXname.Data(), "ResX, cm", "Counter", 200, -1, 1);
        CreateH1(resYname.Data(), "ResY, cm", "Counter", 200, -1, 1);
        CreateH2(pntXhitXname.Data(), "X_{MC}, cm", "X_{reco}, cm", "", 400, -20, 20, 400, -20, 20);
        CreateH2(pntYhitYname.Data(), "Y_{MC}, cm", "Y_{reco}, cm", "", 400, -20, 20, 400, -20, 20);

        CreateH2(occupname.Data(), "X_{MC}, cm", "Y_{MC}, cm", "Occupancy, #frac{part}{event * cm^{2}}", 50, -20, 20, 50, -20, 20);

        CreateH1(pullXname.Data(), "PullX", "Counter", 200, -10, 10);
        CreateH1(pullYname.Data(), "PullY", "Counter", 200, -10, 10);

    }

    for (Int_t iSt = 0; iSt < nStationsDCH1; ++iSt) {

        TString occupname = Form("Occupancy_%dst_dch1", iSt);

        CreateH2(occupname.Data(), "X_{MC}, cm", "Y_{MC}, cm", "Occupancy, #frac{part}{event * cm^{2}}", 50, -20, 20, 50, -20, 20);
    }

    // Histogram stores number of events
    CreateH1("hen_EventNo_ClusteringQa", "", "", 1, 0, 1.);
    CreateH1("Impact parameter", "b, fm", "Counter", 50, 0.0, 0.0);
    CreateH1("Multiplicity", "N_{prim}", "Counter", 60, 0.0, 0.0);
    CreateH2("Impact_Mult", "b, fm", "N_{prim}", "", 400, 0.0, 0.0, 400, 0.0, 0.0);
}

void BmnClusteringQa::CreateH1(const string& name, const string& xTitle, const string& yTitle, Int_t nofBins, Double_t minBin, Double_t maxBin) {
    TH1F* h = new TH1F(name.c_str(), string(name + ";" + xTitle + ";" + yTitle).c_str(), nofBins, minBin, maxBin);
    fHM->Add(name, h);
}

void BmnClusteringQa::CreateH2(
        const string& name,
        const string& xTitle,
        const string& yTitle,
        const string& zTitle,
        Int_t nofBinsX,
        Double_t minBinX,
        Double_t maxBinX,
        Int_t nofBinsY,
        Double_t minBinY,
        Double_t maxBinY) {
    TH2F* h = new TH2F(name.c_str(), (name + ";" + xTitle + ";" + yTitle + ";" + zTitle).c_str(), nofBinsX, minBinX, maxBinX, nofBinsY, minBinY, maxBinY);
    fHM->Add(name, h);
}

/*void BmnClusteringQa::CreateNofObjectsHistograms(DetectorId detId, const string& detName) {
    if (!fDet.GetDet(detId)) return;
    assert(detId == kTOF || detId == kGEM || detId == kTOF1 || detId == kDCH);
    Int_t nofBins = 5000;
    Double_t minX = 0;
    Double_t maxX = 5000;
    string name = "_hno_NofObjects_" + detName;
    fHM->Create1<TH1F > (name + "Points_Event", name + "Points_Event;Points per event;Counter", nofBins, minX, maxX);
    //        fHM->Create1<TH1F > (name + "Digis_Event", name + "Digis_Event;Digis per event;Counter", nofBins, minX, maxX);
    //        fHM->Create1<TH1F > (name + "Clusters_Event", name + "Clusters_Event;Clusters per event;Counter", nofBins, minX, maxX);
    fHM->Create1<TH1F > (name + "Hits_Event", name + "Hits_Event;Hits per event;Counter", nofBins, minX, maxX);
}*/

/*void BmnClusteringQa::CreateNofObjectsHistograms(DetectorId detId, const string& detName, const string& parameter, const string& xTitle) {
    if (!fDet.GetDet(detId)) return;
    Int_t nofBins = 15;
    Double_t minX = -0.5;
    Double_t maxX = 14.5;
    string name = "_hno_NofObjects_" + detName;
    fHM->Create1<TH1F > (name + "Points_" + parameter, name + "Points_" + parameter + ";" + xTitle + ";Points per event", nofBins, minX, maxX);
    fHM->Create1<TH1F > (name + "Digis_" + parameter, name + "Digis_" + parameter + ";" + xTitle + ";Digis per event", nofBins, minX, maxX);
    //    fHM->Create1<TH1F > (name + "Clusters_" + parameter, name + "Clusters_" + parameter + ";" + xTitle + ";Clusters per event", nofBins, minX, maxX);
    fHM->Create1<TH1F > (name + "Hits_" + parameter, name + "Hits_" + parameter + ";" + xTitle + ";Hits per event", nofBins, minX, maxX);
}*/

/*void BmnClusteringQa::CreateClusterParametersHistograms(DetectorId detId, const string& detName) {
    if (!fDet.GetDet(detId)) return;
    Int_t nofBinsStation = 17;
    Double_t minStation = 0;
    Double_t maxStation = 17;
    Int_t nofBins = 100;
    Double_t min = -0.5;
    Double_t max = 99.5;
    Int_t nofBinsSigma = 100;
    Double_t minSigma = 0.;
    Double_t maxSigma = 10;
    Int_t nofBinsResidual = 100;
    Double_t minResidual = -5.0;
    Double_t maxResidual = 5.0;
    Int_t nofBinsPull = 100;
    Double_t minPull = -2.0;
    Double_t maxPull = 2.0;

    string nameH1 = "_hpa_" + detName + "Cluster_NofDigisInCluster_H1";
    //    fHM->Create1<TH1F > (nameH1, nameH1 + ";Number of digis;Yield", nofBins, min, max);
    string nameH2 = "_hpa_" + detName + "Cluster_NofDigisInCluster_H2";
    //    fHM->Create2<TH2F > (nameH2, nameH2 + ";Station;Number of digis;Yield", nofBinsStation, minStation, max, nofBins, min, max);
    //    nameH1 = "hpa_" + detName + "Cluster_NofPointsInCluster_H1";
    //    fHM->Create1<TH1F > (nameH1, nameH1 + ";Number of points;Yield", nofBins, min, max);
    //    nameH2 = "hpa_" + detName + "Cluster_NofPointsInCluster_H2";
    //    fHM->Create2<TH2F > (nameH2, nameH2 + ";Station;Number of points;Yield", nofBinsStation, minStation, max, nofBins, min, max);
    //    nameH1 = "hpa_" + detName + "Digi_NofPointsInDigi_H1";
    //    fHM->Create1<TH1F > (nameH1, nameH1 + ";Number of points;Yield", nofBins, min, max);
    //    nameH2 = "hpa_" + detName + "Digi_NofPointsInDigi_H2";
    //    fHM->Create2<TH2F > (nameH2, nameH2 + ";Station;Number of points;Yield", nofBinsStation, minStation, maxStation, nofBins, min, max);
    nameH1 = "_hpa_" + detName + "Hit_NofPointsInHit_H1";
    fHM->Create1<TH1F > (nameH1, nameH1 + ";Number of points;Yield", nofBins, min, max);
    nameH2 = "_hpa_" + detName + "Hit_NofPointsInHit_H2";
    fHM->Create2<TH2F > (nameH2, nameH2 + ";Station;Number of points;Yield", nofBinsStation, minStation, max, nofBins, min, max);
    nameH1 = "_hpa_" + detName + "Hit_SigmaX_H1";
    fHM->Create1<TH1F > (nameH1, nameH1 + ";#sigma_{X} [cm];Yield", nofBinsSigma, minSigma, maxSigma);
    nameH2 = "_hpa_" + detName + "Hit_SigmaX_H2";
    fHM->Create2<TH2F > (nameH2, nameH2 + ";Station;#sigma_{X} [cm];Yield", nofBinsStation, minStation, maxStation, nofBinsSigma, minSigma, maxSigma);
    nameH1 = "_hpa_" + detName + "Hit_SigmaY_H1";
    fHM->Create1<TH1F > (nameH1, nameH1 + ";#sigma_{Y} [cm];Yield", nofBinsSigma, minSigma, maxSigma);
    nameH2 = "_hpa_" + detName + "Hit_SigmaY_H2";
    fHM->Create2<TH2F > (nameH2, nameH2 + ";Station;#sigma_{Y} [cm];Yield", nofBinsStation, minStation, maxStation, nofBinsSigma, minSigma, maxSigma);

    // Residual and pull histograms
    nameH2 = "_hrp_" + detName + "_ResidualX_H2";
    fHM->Create2<TH2F > (nameH2, nameH2 + ";Station;Residual X [cm];Yield", nofBinsStation, minStation, maxStation, nofBinsResidual, minResidual, maxResidual);
    nameH2 = "_hrp_" + detName + "_ResidualY_H2";
    fHM->Create2<TH2F > (nameH2, nameH2 + ";Station;Residual Y [cm];Yield", nofBinsStation, minStation, maxStation, nofBinsResidual, minResidual, maxResidual);
    nameH2 = "_hrp_" + detName + "_PullX_H2";
    fHM->Create2<TH2F > (nameH2, nameH2 + ";Station;Pull X;Yield", nofBinsStation, minStation, maxStation, nofBinsPull, minPull, maxPull);
    nameH2 = "_hrp_" + detName + "_PullY_H2";
    fHM->Create2<TH2F > (nameH2, nameH2 + ";Station;Pull Y;Yield", nofBinsStation, minStation, maxStation, nofBinsPull, minPull, maxPull);
}*/

/*void BmnClusteringQa::CreateHitEfficiencyHistograms(DetectorId detId, const string& detName, const string& parameter, const string& xTitle, Int_t nofBins, Double_t minBin, Double_t maxBin) {
    if (!fDet.GetDet(detId)) return;
    vector<string> types = list_of("Acc")("Rec")("Eff")("Clone")("CloneProb");
    vector<string> cat = list_of("All");
    for (Int_t iCat = 0; iCat < cat.size(); iCat++) {
        for (Int_t iType = 0; iType < types.size(); iType++) {
            string yTitle = (types[iType] == "Eff") ? "Efficiency [%]" : (types[iType] == "CloneProb") ? "Probability [%]" : "Counter";
            string histName = "_hhe_" + detName + "_" + cat[iCat] + "_" + types[iType] + "_" + parameter;
            string histTitle = histName + ";" + xTitle + ";" + yTitle;
            fHM->Add(histName, new TH1F(histName.c_str(), histTitle.c_str(), nofBins, minBin, maxBin));
        }
    }
}*/

ClassImp(BmnClusteringQa);
