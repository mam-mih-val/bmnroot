
/**
 * \file BmnTrackingQa.cxx
 * \author Andrey Lebedev <andrey.lebedev@gsi.de> - original author for CBM experiment
 * \author Sergey Merts <sergey.merts@gmail.com> - modifications for BMN experiment
 * \date 2007-2014
 */

#include "BmnTrackingQa.h"
#include "BmnTrackingQaStudyReport.h"
#include "BmnTrackingQaReport.h"
#include "report/BmnHistManager.h"
#include "BmnUtils.h"
#include "BmnMCTrackCreator.h"
#include "BmnAcceptanceFunction.h"
#include "CbmGlobalTrack.h"
#include "CbmMCTrack.h"
#include "CbmBaseHit.h"
#include "FairMCPoint.h"
#include "CbmStsTrack.h"
#include "BmnGemTrack.h"
#include "BmnTrackMatch.h"
#include "BmnMatch.h"
#include "CbmTofHit.h"
#include "BmnDchHit.h"
#include "FairRunAna.h"
#include "FairMCEventHeader.h"
#include "TFitResult.h"
#include "CbmVertex.h"
#include "BmnMath.h"
#include "BmnGemStripHit.h"
#include "TH1.h"
#include "TF1.h"
#include "TH2F.h"
#include "TClonesArray.h"
#include "BmnEnums.h"

#include <fstream>
#include <iostream>

using namespace std;
using namespace TMath;
using lit::Split;
using lit::FindAndReplace;

BmnTrackingQa::BmnTrackingQa() :
FairTask("BmnTrackingQA", 1),
fHM(NULL),
fOutputDir("./"),
fDet(),
//fMinNofPointsGem(4),
fMinNofPointsGem(4),
fMinNofPointsTof(1),
fMinNofPointsDch(1),
fQuota(0.6),
fEtaCut(100000.0),
fPCut(0.0),
fUseConsecutivePointsInGem(kTRUE),
fPRangeMin(0.),
fPRangeMax(5.),
fPRangeBins(25),
fYRangeMin(0.),
fYRangeMax(4.),
fYRangeBins(100),
fEtaRangeMin(0.),
fEtaRangeMax(6.),
fEtaRangeBins(50),
fPtRangeMin(0.),
fPtRangeMax(1.),
fPtRangeBins(50),
fThetaRangeMin(0.),
fThetaRangeMax(40.),
fThetaRangeBins(50),
fHeader(),
fMcToRecoMap(),
fMCTracks(NULL),
fPrimes(kFALSE),
fGlobalTracks(NULL) {
}

BmnTrackingQa::~BmnTrackingQa() {
    if (fHM) delete fHM;
}

InitStatus BmnTrackingQa::Init() {
    // Create histogram manager which is used throughout the program

    fHM = new BmnHistManager();

    fDet.DetermineSetup();
    cout << fDet.ToString();
//    if (fTrackCategories.empty()) FillDefaultTrackCategories();

    CreateHistograms();

    ReadDataBranches();

    fMCTrackCreator = BmnMCTrackCreator::Instance();

    fMcToRecoMap.clear();
    vector<string> trackVariants = GlobalTrackVariants();
    for (Int_t i = 0; i < trackVariants.size(); i++) {
        fMcToRecoMap.insert(make_pair(trackVariants[i], multimap<Int_t, Int_t > ()));
    }
    return kSUCCESS;
}

void BmnTrackingQa::Exec(Option_t* opt) {
    // Increase event counter
    fHM->H1("hen_EventNo_TrackingQa")->Fill(0.5);
    ReadEventHeader();
    fMCTrackCreator->Create();
    ProcessGem();
    IncreaseCounters();
}

void BmnTrackingQa::Finish() {
    fHM->WriteToFile();
    BmnSimulationReport* report = new BmnTrackingQaReport(fHeader);
    report->SetOnlyPrimes(fPrimes);
    report->Create(fHM, fOutputDir);
    delete report;
}

void BmnTrackingQa::ReadDataBranches() {
    FairRootManager* ioman = FairRootManager::Instance();
    if (NULL == ioman) {
        Fatal("Init", "BmnRootManager is not instantiated");
    }

    fMCTracks = (TClonesArray*) ioman->GetObject("MCTrack");
    if (NULL == fMCTracks) {
        Fatal("Init", "No MCTrack array!");
    }

    //    fGlobalTracks = (TClonesArray*) ioman->GetObject("GlobalTrack");
    //    fGlobalTrackMatches = (TClonesArray*) ioman->GetObject("GlobalTrackMatch");
    //
    //    if (NULL == fGlobalTracks) {
    //        Fatal("Init", "No GlobalTrack array!");
    //    }

    if (fDet.GetDet(kGEM)) {
        fGemSeeds = (TClonesArray*) ioman->GetObject("BmnGemSeeds");
        if (NULL == fGemSeeds) {
            Fatal("Init", ": No GemSeeds array!");
        }
        fGemSeedMatches = (TClonesArray*) ioman->GetObject("BmnGemSeedMatch");
        if (NULL == fGemSeedMatches) {
            Fatal("Init", ": No BmnGemSeedMatch array!");
        }
        fGemTracks = (TClonesArray*) ioman->GetObject("BmnGemTracks");
        if (NULL == fGemTracks) {
            Fatal("Init", ": No GemTrack array!");
        }
        fGemMatches = (TClonesArray*) ioman->GetObject("BmnGemTrackMatch");
        if (NULL == fGemMatches) {
            Fatal("Init", ": No BmnGemTrackMatch array!");
        }
        fGemHits = (TClonesArray*) ioman->GetObject("BmnGemStripHit");
        if (NULL == fGemHits) {
            Fatal("Init", ": No BmnGemStripHit array!");
        }
        fGemPoints = (TClonesArray*) ioman->GetObject("StsPoint");
        if (NULL == fGemPoints) {
            cout << "BmnTrackingQA::Init: No GemPoint array!" << endl;
        }
        fGemHitMatches = (TClonesArray*) ioman->GetObject("BmnGemStripHitMatch");
        if (NULL == fGemHitMatches) {
            cout << "BmnTrackingQA::Init: No BmnGemStripHitMatch array!" << endl;
        }
        fVertex = (TClonesArray*) ioman->GetObject("BmnVertex");
        if (NULL == fVertex) {
            cout << "BmnTrackingQA::Init: No fVertex array!" << endl;
        }
    }
    if (fDet.GetDet(kTOF1)) {
        fTof1Points = (TClonesArray*) ioman->GetObject("TOF1Point");
        if (NULL == fTof1Points) {
            cout << "BmnTrackingQA::Init: No TOF1Point array!" << endl;
        }
        fTof1Hits = (TClonesArray*) ioman->GetObject("TOF1Hit");
        if (NULL == fTof1Hits) {
            cout << "BmnTrackingQA::Init: No BmnTof1Hit array!" << endl;
        }
    }
    if (fDet.GetDet(kTOF)) {
        fTof2Points = (TClonesArray*) ioman->GetObject("TofPoint");
        if (NULL == fTof2Points) {
            cout << "BmnTrackingQA::Init: No TofPoint array!" << endl;
        }
        fTof2Hits = (TClonesArray*) ioman->GetObject("BmnTof2Hit");
        if (NULL == fTof2Hits) {
            cout << "BmnTrackingQA::Init: No BmnTof2Hit array!" << endl;
        }
    }
}

void BmnTrackingQa::ReadEventHeader() {
    FairMCEventHeader* evHead = (FairMCEventHeader*) FairRootManager::Instance()->GetObject("MCEventHeader.");
    //    fHeader.push_back("Event generator: UrQMD");
    //    fHeader.push_back("Energy: 4 GeV/c");
    fHM->H1("Impact parameter")->Fill(evHead->GetB());
    fHM->H1("Multiplicity")->Fill(evHead->GetNPrim());
    fHM->H2("Impact_Mult")->Fill(evHead->GetB(), evHead->GetNPrim());
}

void BmnTrackingQa::CreateH1(const string& name, const string& xTitle, const string& yTitle, Int_t nofBins, Double_t minBin, Double_t maxBin) {
    TH1F* h = new TH1F(name.c_str(), string(name + ";" + xTitle + ";" + yTitle).c_str(), nofBins, minBin, maxBin);
    fHM->Add(name, h);
}

void BmnTrackingQa::CreateH2(
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

void BmnTrackingQa::CreateTrackHitsHistogram(const string& detName) {
    string type[] = {"All", "True", "Fake", "TrueOverAll", "FakeOverAll"};
    Double_t min[] = {0., 0., 0., 0., 0.};
    Double_t max[] = {20, 20, 20, 1., 1.};
    Int_t bins[] = {20, 20, 20, 20, 20};
    for (Int_t i = 0; i < 5; i++) {
        string xTitle = (i == 3 || i == 4) ? "Ratio" : "Number of hits";
        string histName = "hth_" + detName + "_TrackHits_" + type[i];
        CreateH1(histName.c_str(), xTitle, "Yeild", bins[i], min[i], max[i]);
    }
}

vector<string> BmnTrackingQa::CreateGlobalTrackingHistogramNames(const vector<string>& detectors) {
    vector<string> histos;
    Int_t nofDetectors = detectors.size();
    for (Int_t iDet = 0; iDet < nofDetectors; iDet++) {
        string histEff;
        for (Int_t i = 0; i <= iDet; i++) {
            histEff += detectors[i];
        }
        string histNorm = histEff;
        histos.push_back("hte_" + histEff + "_" + histNorm);
        for (Int_t i = iDet + 1; i < nofDetectors; i++) {
            histNorm += detectors[i];
            histos.push_back("hte_" + histEff + "_" + histNorm);
        }
    }
    return histos;
}

vector<string> BmnTrackingQa::CreateGlobalTrackingHistogramNames() {
    vector<string> detectors;
    if (fDet.GetDet(kGEM)) detectors.push_back("Gem");
    //    if (fDet.GetDet(kTOF1)) detectors.push_back("Tof1");
    //    if (fDet.GetDet(kDCH1)) detectors.push_back("Dch");
    //    if (fDet.GetDet(kTOF)) detectors.push_back("Tof2");
    vector<string> names1 = CreateGlobalTrackingHistogramNames(detectors);
    set<string> names;
    names.insert(names1.begin(), names1.end());
    vector<string> nameVector(names.begin(), names.end());
    return nameVector;
}

vector<string> BmnTrackingQa::GlobalTrackVariants() {
    set<string> trackVariants;

    vector<string> detectors;
    if (fDet.GetDet(kGEM)) detectors.push_back("Gem");
    if (fDet.GetDet(kTOF1)) detectors.push_back("Tof1");
    if (fDet.GetDet(kDCH)) detectors.push_back("Dch");
    if (fDet.GetDet(kTOF)) detectors.push_back("Tof2");
    string name("");
    for (Int_t i = 0; i < detectors.size(); ++i) {
        name += detectors[i];
        trackVariants.insert(name);
    }
    vector<string> trackVariantsVector(trackVariants.begin(), trackVariants.end());
    return trackVariantsVector;
}

void BmnTrackingQa::CreateHistograms() {
    fDet.DetermineSetup();

    // Number of points distributions
    const Float_t minNofPoints = -0.5;
    const Float_t maxNofPoints = 14.5;
    const Int_t nofBinsPoints = 15;

    // Reconstruction efficiency histograms
    // Local efficiency histograms

    // Create histograms for ghost tracks
    if (fDet.GetDet(kGEM)) CreateH1("hng_NofGhosts_Gem_Nh", "Number of hits", "Yield", nofBinsPoints, minNofPoints, maxNofPoints);

    // Create track hits histograms
    if (fDet.GetDet(kGEM)) CreateTrackHitsHistogram("Gem");

    // Create number of object histograms
    UInt_t nofBinsC = 100000;
    Double_t maxXC = (Double_t) nofBinsC;
    CreateH1("hno_NofObjects_GlobalTracks", "Tracks per event", "Yield", nofBinsC, 1., maxXC);
    if (fDet.GetDet(kGEM)) {
        CreateH1("hno_NofObjects_GemTracks", "Tracks per event", "Yield", nofBinsC, 1., maxXC);
        CreateH1("hno_NofObjects_GemHits", "GEM hits per event", "Yield", nofBinsC, 1., maxXC);
    }
    if (fDet.GetDet(kTOF1)) CreateH1("hno_NofObjects_Tof1Hits", "TOF1 hits per event", "Yield", nofBinsC, 1., maxXC);
    if (fDet.GetDet(kDCH)) CreateH1("hno_NofObjects_DchHits", "DCH1 hits per event", "Yield", nofBinsC, 1., maxXC);
    if (fDet.GetDet(kTOF)) CreateH1("hno_NofObjects_Tof2Hits", "TOF2 hits per event", "Yield", nofBinsC, 1., maxXC);

    // Histogram stores number of events
    CreateH1("hen_EventNo_TrackingQa", "", "", 1, 0, 1.);
    CreateH1("Impact parameter", "b, fm", "Counter", 50, 0.0, 0.0);
    CreateH1("Multiplicity", "N_{prim}", "Counter", 50, 0.0, 0.0);
    CreateH2("Impact_Mult", "b, fm", "N_{prim}", "", 50, 0.0, 0.0, 50, 0.0, 0.0);

    // Physics
    CreateH2("momRes_2D_glob", "P_{sim}, GeV/c", "#Delta P / P, %", "", 4 * fPRangeBins, fPRangeMin, fPRangeMax, 4 * fPRangeBins, -50.0, 50.0);
    CreateH2("momRes_2D_gem", "P_{sim}, GeV/c", "#Delta P / P, %", "", 400, fPRangeMin, fPRangeMax, 400, -10.0, 10.0);
    CreateH2("EtaP_rec_gem", "#eta_{rec}", "P_{rec}, GeV/c", "", 50, fEtaRangeMin, fEtaRangeMax, 50, fPRangeMin, fPRangeMax);
    CreateH2("EtaP_rec_glob", "#eta_{rec}", "P_{rec}, GeV/c", "", 400, fEtaRangeMin, fEtaRangeMax, 400, fPRangeMin, fPRangeMax);
    CreateH2("EtaP_sim", "#eta_{sim}", "P_{sim}, GeV/c", "", 50, fEtaRangeMin, fEtaRangeMax, 50, fPRangeMin, fPRangeMax);
    CreateH2("EtaP_fakes_sim", "#eta_{sim}", "P_{sim}, GeV/c", "", 50, fEtaRangeMin, fEtaRangeMax, 50, fPRangeMin, fPRangeMax);
    CreateH2("EtaP_clones_sim", "#eta_{sim}", "P_{sim}, GeV/c", "", 50, fEtaRangeMin, fEtaRangeMax, 50, fPRangeMin, fPRangeMax);
    CreateH2("ThetaP_sim", "#theta_{sim}", "P_{sim}, GeV/c", "", 400, fThetaRangeMin, fThetaRangeMax, 400, fPRangeMin, fPRangeMax);
    CreateH2("ThetaP_rec_gem", "#theta_{rec}", "P_{rec}, GeV/c", "", 400, fThetaRangeMin, fThetaRangeMax, 400, fPRangeMin, fPRangeMax);
    CreateH1("momRes_1D_glob", "P_{sim}, GeV/c", "#LT#Delta P / P#GT, %", fPRangeBins, fPRangeMin, fPRangeMax);
    CreateH1("momRes_1D_gem", "P_{sim}, GeV/c", "#Delta P / P, #sigma, %", fPRangeBins, fPRangeMin, fPRangeMax);
    CreateH1("momRes_Mean_gem", "#Delta P / P, %", "Counter", 400, -10.0, 10.0);
    CreateH2("P_rec_P_sim_gem", "P_{sim}, GeV/c", "P_{rec}, GeV/c", "", 400, fPRangeMin, fPRangeMax, 400, fPRangeMin, fPRangeMax);
    CreateH2("P_rec_P_sim_glob", "P_{sim}, GeV/c", "P_{rec}, GeV/c", "", 400, fPRangeMin, fPRangeMax, 400, fPRangeMin, fPRangeMax);
    CreateH2("Px_rec_Px_sim_gem", "P^{x}_{sim}, GeV/c", "P^{x}_{rec}, GeV/c", "", 400, -fPtRangeMax, fPtRangeMax, 400, -fPtRangeMax, fPtRangeMax);
    CreateH2("Py_rec_Py_sim_gem", "P^{y}_{sim}, GeV/c", "P^{y}_{rec}, GeV/c", "", 400, -fPtRangeMax, fPtRangeMax, 400, -fPtRangeMax, fPtRangeMax);
    CreateH2("Pz_rec_Pz_sim_gem", "P^{z}_{sim}, GeV/c", "P^{z}_{rec}, GeV/c", "", 400, fPtRangeMin, 4, 400, fPtRangeMin, 4);
    CreateH2("Pt_rec_Pt_sim_gem", "P^{t}_{sim}, GeV/c", "P^{t}_{rec}, GeV/c", "", 400, fPtRangeMin, 4, 400, fPtRangeMin, 4);
    CreateH2("Px_rec_Px_sim_glob", "P^{x}_{sim}, GeV/c", "P^{x}_{rec}, GeV/c", "", 400, -fPtRangeMax, fPtRangeMax, 400, -fPtRangeMax, fPtRangeMax);
    CreateH2("Py_rec_Py_sim_glob", "P^{y}_{sim}, GeV/c", "P^{y}_{rec}, GeV/c", "", 400, -fPtRangeMax, fPtRangeMax, 400, -fPtRangeMax, fPtRangeMax);
    CreateH2("Pz_rec_Pz_sim_glob", "P^{z}_{sim}, GeV/c", "P^{z}_{rec}, GeV/c", "", 400, fPtRangeMin, 4, 400, fPtRangeMin, 4);
    CreateH2("Pt_rec_Pt_sim_glob", "P^{t}_{sim}, GeV/c", "P^{t}_{rec}, GeV/c", "", 400, fPtRangeMin, 4, 400, fPtRangeMin, 4);
    CreateH2("Eta_rec_Eta_sim_gem", "#eta_{sim}", "#eta_{rec}", "", 400, fEtaRangeMin, fEtaRangeMax, 400, fEtaRangeMin, fEtaRangeMax);
    CreateH2("Eta_rec_Eta_sim_glob", "#eta_{sim}", "#eta_{rec}", "", 400, fEtaRangeMin, fEtaRangeMax, 400, fEtaRangeMin, fEtaRangeMax);

    CreateH2("Tx_rec_Tx_sim_gem", "T^{x}_{sim}", "T^{x}_{rec}", "", 400, -1.0, 1.0, 400, -1.0, 1.0);
    CreateH2("Ty_rec_Ty_sim_gem", "T^{y}_{sim}", "T^{y}_{rec}", "", 400, -1.0, 1.0, 400, -1.0, 1.0);
    CreateH2("Tx_rec_Tx_sim_glob", "T^{x}_{sim}", "T^{x}_{rec}", "", 400, -1.0, 1.0, 400, -1.0, 1.0);
    CreateH2("Ty_rec_Ty_sim_glob", "T^{y}_{sim}", "T^{y}_{rec}", "", 400, -1.0, 1.0, 400, -1.0, 1.0);

    CreateH2("Nh_sim_Eta_sim_gem", "Number of mc-points", "#eta_{sim}", "", nofBinsPoints, minNofPoints, maxNofPoints, 200, fEtaRangeMin, fEtaRangeMax);
    CreateH2("Nh_rec_Eta_rec_gem", "Number of mc-points", "#eta_{sim}", "", nofBinsPoints, minNofPoints, maxNofPoints, 200, fEtaRangeMin, fEtaRangeMax);
    CreateH2("Nh_sim_P_sim_gem", "Number of mc-points", "P_{sim}", "", nofBinsPoints, minNofPoints, maxNofPoints, 200, fPRangeMin, 2 * fPRangeMax);
    CreateH2("Nh_rec_P_rec_gem", "Number of mc-points", "P_{sim}", "", nofBinsPoints, minNofPoints, maxNofPoints, 200, fPRangeMin, 2 * fPRangeMax);
    CreateH2("Nh_sim_Theta_sim_gem", "Number of mc-points", "#theta_{sim}", "", nofBinsPoints, minNofPoints, maxNofPoints, 200, fThetaRangeMin, fThetaRangeMax);
    CreateH2("Nh_rec_Theta_rec_gem", "Number of mc-points", "#theta_{sim}", "", nofBinsPoints, minNofPoints, maxNofPoints, 200, fThetaRangeMin, fThetaRangeMax);


    CreateH1("wellGemDistr", "P_{sim}, GeV/c", "Counter", fPRangeBins, fPRangeMin, fPRangeMax);
    CreateH1("wellGlobDistr", "P_{sim}, GeV/c", "Counter", fPRangeBins, fPRangeMin, fPRangeMax);
    CreateH1("ghostGemDistr", "P_{sim}, GeV/c", "Counter", fPRangeBins, fPRangeMin, fPRangeMax);
    CreateH1("ghostGlobDistr", "P_{sim}, GeV/c", "Counter", fPRangeBins, fPRangeMin, fPRangeMax);
    CreateH1("recoGemDistr", "P_{sim}, GeV/c", "Counter", fPRangeBins, fPRangeMin, fPRangeMax);
    CreateH1("recoGlobDistr", "P_{sim}, GeV/c", "Counter", fPRangeBins, fPRangeMin, fPRangeMax);
    CreateH1("allGemDistr", "P_{sim}, GeV/c", "Counter", fPRangeBins, fPRangeMin, fPRangeMax);
    CreateH1("allGlobDistr", "P_{sim}, GeV/c", "Counter", fPRangeBins, fPRangeMin, fPRangeMax);
    CreateH1("EffGemDistr", "P_{sim}, GeV/c", "Efficiency, %", fPRangeBins, fPRangeMin, fPRangeMax);
    CreateH1("EffGlobDistr", "P_{sim}, GeV/c", "Efficiency, %", fPRangeBins, fPRangeMin, fPRangeMax);
    CreateH1("FakeGemDistr", "P_{sim}, GeV/c", "Ghosts, %", fPRangeBins, fPRangeMin, fPRangeMax);
    CreateH1("FakeGlobDistr", "P_{sim}, GeV/c", "Ghosts, %", fPRangeBins, fPRangeMin, fPRangeMax);

    CreateH1("Eff_vs_P_gem", "P_{sim}, GeV/c", "Efficiency, %", fPRangeBins, fPRangeMin, fPRangeMax);
    CreateH1("Split_vs_P_gem", "P_{sim}, GeV/c", "Counter", fPRangeBins, fPRangeMin, fPRangeMax);
    CreateH1("SplitEff_vs_P_gem", "P_{sim}, GeV/c", "Splits, %", fPRangeBins, fPRangeMin, fPRangeMax);
    CreateH1("Sim_vs_P_gem", "P_{sim}, GeV/c", "Counter", fPRangeBins, fPRangeMin, fPRangeMax);
    CreateH1("Rec_vs_P_gem", "P_{sim}, GeV/c", "Counter", fPRangeBins, fPRangeMin, fPRangeMax);
    CreateH1("Ghost_vs_P_gem", "P_{sim}, GeV/c", "Counter", fPRangeBins, fPRangeMin, fPRangeMax);
    CreateH1("Well_vs_P_gem", "P_{sim}, GeV/c", "Counter", fPRangeBins, fPRangeMin, fPRangeMax);
    CreateH1("Fake_vs_P_gem", "P_{sim}, GeV/c", "Ghosts, %", fPRangeBins, fPRangeMin, fPRangeMax);

    CreateH1("Eff_vs_Eta_gem", "#eta_{sim}", "Efficiency, %", fEtaRangeBins, fEtaRangeMin, fEtaRangeMax);
    CreateH1("Split_vs_Eta_gem", "#eta_{sim}", "Counter", fEtaRangeBins, fEtaRangeMin, fEtaRangeMax);
    CreateH1("SplitEff_vs_Eta_gem", "#eta_{sim}", "Splits, %", fEtaRangeBins, fEtaRangeMin, fEtaRangeMax);
    CreateH1("Sim_vs_Eta_gem", "#eta_{sim}", "Counter", fEtaRangeBins, fEtaRangeMin, fEtaRangeMax);
    CreateH1("Rec_vs_Eta_gem", "#eta_{sim}", "Counter", fEtaRangeBins, fEtaRangeMin, fEtaRangeMax);
    CreateH1("Ghost_vs_Eta_gem", "#eta_{sim}", "Counter", fEtaRangeBins, fEtaRangeMin, fEtaRangeMax);
    CreateH1("Well_vs_Eta_gem", "#eta_{sim}", "Counter", fEtaRangeBins, fEtaRangeMin, fEtaRangeMax);
    CreateH1("Fake_vs_Eta_gem", "#eta_{sim}", "Ghosts, %", fEtaRangeBins, fEtaRangeMin, fEtaRangeMax);

    CreateH1("Eff_vs_Theta_gem", "#theta_{sim}", "Efficiency, %", fThetaRangeBins, fThetaRangeMin, fThetaRangeMax);
    CreateH1("Split_vs_Theta_gem", "#theta_{sim}", "Counter", fThetaRangeBins, fThetaRangeMin, fThetaRangeMax);
    CreateH1("SplitEff_vs_Theta_gem", "#theta_{sim}", "Splits, %", fThetaRangeBins, fThetaRangeMin, fThetaRangeMax);
    CreateH1("Sim_vs_Theta_gem", "#theta_{sim}", "Counter", fThetaRangeBins, fThetaRangeMin, fThetaRangeMax);
    CreateH1("Rec_vs_Theta_gem", "#theta_{sim}", "Counter", fThetaRangeBins, fThetaRangeMin, fThetaRangeMax);
    CreateH1("Ghost_vs_Theta_gem", "#theta_{sim}", "Counter", fThetaRangeBins, fThetaRangeMin, fThetaRangeMax);
    CreateH1("Well_vs_Theta_gem", "#theta_{sim}", "Counter", fThetaRangeBins, fThetaRangeMin, fThetaRangeMax);
    CreateH1("Fake_vs_Theta_gem", "#theta_{sim}", "Ghosts, %", fThetaRangeBins, fThetaRangeMin, fThetaRangeMax);

    CreateH1("Ghost_vs_Nh_gem", "Number of hits", "Counter", nofBinsPoints, minNofPoints, maxNofPoints);
    CreateH1("Well_vs_Nh_gem", "Number of hits", "Counter", nofBinsPoints, minNofPoints, maxNofPoints);

    CreateH1("Eff_vs_P_glob", "P_{sim}, GeV/c", "Efficiency, %", fPRangeBins, fPRangeMin, fPRangeMax);
    CreateH1("Sim_vs_P_glob", "P_{sim}, GeV/c", "Counter", fPRangeBins, fPRangeMin, fPRangeMax);
    CreateH1("Rec_vs_P_glob", "P_{sim}, GeV/c", "Counter", fPRangeBins, fPRangeMin, fPRangeMax);
    CreateH1("Ghost_vs_P_glob", "P_{sim}, GeV/c", "Counter", fPRangeBins, fPRangeMin, fPRangeMax);
    CreateH1("Well_vs_P_glob", "P_{sim}, GeV/c", "Counter", fPRangeBins, fPRangeMin, fPRangeMax);
    CreateH1("Fake_vs_P_glob", "P_{sim}, GeV/c", "Ghosts, %", fPRangeBins, fPRangeMin, fPRangeMax);
    CreateH1("Ghost_vs_Nh_glob", "Number of hits", "Counter", nofBinsPoints, minNofPoints, maxNofPoints);
    CreateH1("Well_vs_Nh_glob", "Number of hits", "Counter", nofBinsPoints, minNofPoints, maxNofPoints);

    CreateH2("MomRes_vs_Chi2_gem", "#chi^{2}", "#Delta P / P, %", "", 400, 0, 50, 400, -10, 10);
    CreateH2("MomRes_vs_Length_gem", "Length, cm", "#Delta P / P, %", "", 400, 0, 400, 400, -10, 10);
    CreateH2("Mom_vs_Chi2_gem", "#chi^{2}", "P_{rec}, GeV/c", "", 400, 0, 50, 400, fPRangeMin, fPRangeMax);
    CreateH2("Mom_vs_Length_gem", "Length, cm", "P_{rec}, GeV/c", "", 400, 0, 400, 400, fPRangeMin, fPRangeMax);
    CreateH1("Chi2_gem", "#chi^{2} / NDF", "Counter", 400, 0, 100);
    CreateH1("Length_gem", "length, cm", "Counter", 400, 0, 400);

    CreateH1("VertResX_gem", "#DeltaV^{0}_{x}, cm", "Counter", 100, -0.5, 0.5);
    CreateH1("VertResY_gem", "#DeltaV^{0}_{y}, cm", "Counter", 100, -0.5, 0.5);
    CreateH1("VertResZ_gem", "#DeltaV^{0}_{z}, cm", "Counter", 100, -2, 2);

    CreateH2("Eff_vs_EtaP_gem", "#eta_{sim}", "P_{sim, GeV/c}", "", 50, fEtaRangeMin, fEtaRangeMax, 50, fPRangeMin, fPRangeMax);
    CreateH2("Clones_vs_EtaP_gem", "#eta_{sim}", "P_{sim, GeV/c}", "", 50, fEtaRangeMin, fEtaRangeMax, 50, fPRangeMin, fPRangeMax);
    CreateH2("Fakes_vs_EtaP_gem", "#eta_{sim}", "P_{sim, GeV/c}", "", 50, fEtaRangeMin, fEtaRangeMax, 50, fPRangeMin, fPRangeMax);

    //for first parameters
    CreateH1("ResX_f_gem", "Residual X, cm", "", 100, -20, 20);
    CreateH1("ResY_f_gem", "Residual Y, cm", "", 100, -20, 20);
    CreateH1("ResTx_f_gem", "Residual t_{x}", "", 100, -0.5, 0.5);
    CreateH1("ResTy_f_gem", "Residual t_{y}", "", 100, -0.5, 0.5);
    CreateH1("ResQp_f_gem", "Residual q/p, (GeV/c)^{-1}", "", 100, -10.0, 10.0);
    CreateH1("PullX_f_gem", "Pull X", "", 100, -4.0, 4.0);
    CreateH1("PullY_f_gem", "Pull Y", "", 100, -4.0, 4.0);
    CreateH1("PullTx_f_gem", "Pull t_{x}", "", 100, -4.0, 4.0);
    CreateH1("PullTy_f_gem", "Pull t_{y}", "", 100, -4.0, 4.0);
    CreateH1("PullQp_f_gem", "Pull q/p", "", 100, -4.0, 4.0);
    //for last parameters
    CreateH1("ResX_l_gem", "Residual X, cm", "", 100, -20, 20);
    CreateH1("ResY_l_gem", "Residual Y, cm", "", 100, -20, 20);
    CreateH1("ResTx_l_gem", "Residual t_{x}", "", 100, -0.5, 0.5);
    CreateH1("ResTy_l_gem", "Residual t_{y}", "", 100, -0.5, 0.5);
    CreateH1("ResQp_l_gem", "Residual q/p, (GeV/c)^{-1}", "", 100, -10.0, 10.0);
    CreateH1("PullX_l_gem", "Pull X", "", 100, -4.0, 4.0);
    CreateH1("PullY_l_gem", "Pull Y", "", 100, -4.0, 4.0);
    CreateH1("PullTx_l_gem", "Pull t_{x}", "", 100, -4.0, 4.0);
    CreateH1("PullTy_l_gem", "Pull t_{y}", "", 100, -4.0, 4.0);
    CreateH1("PullQp_l_gem", "Pull q/p", "", 100, -4.0, 4.0);

    CreateH1("X_f_gem", "X, cm", "", 100, -100, 100);
    CreateH1("Y_f_gem", "Y, cm", "", 100, -100, 100);
    CreateH1("Tx_f_gem", "t_{x}", "", 100, -1.0, 1.0);
    CreateH1("Ty_f_gem", "t_{y}", "", 100, -1.0, 1.0);
    CreateH1("Qp_f_gem", "q/p, (GeV/c)^{-1}", "", 100, -20.0, 20.0);
    CreateH1("X_l_gem", "X, cm", "", 100, -100, 100);
    CreateH1("Y_l_gem", "Y, cm", "", 100, -100, 100);
    CreateH1("Tx_l_gem", "t_{x}", "", 100, -1.0, 1.0);
    CreateH1("Ty_l_gem", "t_{y}", "", 100, -1.0, 1.0);
    CreateH1("Qp_l_gem", "q/p, (GeV/c)^{-1}", "", 100, -20.0, 20.0);
}

void BmnTrackingQa::ProcessGem() {
    vector<Int_t> refs;
    vector<Int_t> splits;

    CbmVertex* vrt = (CbmVertex*) fVertex->At(0);

    if (vrt != NULL) {
        FairMCEventHeader* evHead = (FairMCEventHeader*) FairRootManager::Instance()->GetObject("MCEventHeader.");
        fHM->H1("VertResX_gem")->Fill(vrt->GetX() - evHead->GetX());
        fHM->H1("VertResY_gem")->Fill(vrt->GetY() - evHead->GetY());
        fHM->H1("VertResZ_gem")->Fill(vrt->GetZ() - evHead->GetZ());
    }

    for (Int_t iTrack = 0; iTrack < fGemTracks->GetEntriesFast(); iTrack++) {
        BmnGemTrack* track = (BmnGemTrack*) (fGemTracks->At(iTrack));
        if (track->GetFlag() == kBMNBAD) continue;
        BmnTrackMatch* gemTrackMatch = (BmnTrackMatch*) (fGemMatches->At(iTrack));
        if (!track || !gemTrackMatch) continue;
        if (gemTrackMatch->GetNofLinks() == 0) continue;       
        Int_t gemMCId = gemTrackMatch->GetMatchedLink().GetIndex();

        if (!fMCTrackCreator->TrackExists(gemMCId)) continue;
        const BmnMCTrack mcTrack = fMCTrackCreator->GetTrack(gemMCId);
        const BmnMCPoint pnt = mcTrack.GetPoint(kGEM, 0);
        const BmnMCPoint pntLast = mcTrack.GetPoint(kGEM, mcTrack.GetNofPoints(kGEM) - 1);
        if (fPrimes && ((CbmMCTrack*) (fMCTracks->At(gemMCId)))->GetMotherId() != -1) continue;

        vector<Int_t>::iterator it = find(refs.begin(), refs.end(), gemMCId);
        if (it != refs.end() && gemMCId != -1) {
            splits.push_back(gemMCId);
        } else
            refs.push_back(gemMCId);

        Bool_t isTrackOk = gemTrackMatch->GetTrueOverAllHitsRatio() >= fQuota && track->GetNHits() >= fMinNofPointsGem;
        Float_t Px_sim = pnt.GetPx(); //mcTrack->GetPx();
        Float_t Py_sim = pnt.GetPy(); //mcTrack->GetPy();
        Float_t Pz_sim = pnt.GetPz(); //mcTrack->GetPz();
        Float_t P_sim = pnt.GetP(); //Sqrt(Px_sim * Px_sim + Py_sim * Py_sim + Pz_sim * Pz_sim); //mcTrack->GetP();
        Float_t Pt_sim = Sqrt(Px_sim * Px_sim + Pz_sim * Pz_sim);
        Float_t Pxy_sim = Sqrt(Px_sim * Px_sim + Py_sim * Py_sim);
        Float_t Eta_sim = 0.5 * Log((P_sim + Pz_sim) / (P_sim - Pz_sim));
        Float_t Theta_sim = ATan2(Pxy_sim, Pz_sim) * RadToDeg();
        Int_t N_sim = mcTrack.GetNofPoints(kGEM);

        Float_t P_rec = Abs(1.0 / track->GetParamFirst()->GetQp());
        Float_t Tx = track->GetParamFirst()->GetTx();
        Float_t Ty = track->GetParamFirst()->GetTy();
        Float_t Pz_rec = P_rec / Sqrt(Tx * Tx + Ty * Ty + 1);
        Float_t Px_rec = Pz_rec * Tx;
        Float_t Py_rec = Pz_rec * Ty;
        Float_t Pxy_rec = Sqrt(Px_rec * Px_rec + Py_rec * Py_rec);
        Float_t Eta_rec = 0.5 * Log((P_rec + Pz_rec) / (P_rec - Pz_rec));
        Float_t Theta_rec = ATan2(Pxy_rec, Pz_rec) * RadToDeg();
        Int_t N_rec = track->GetNHits();

        fHM->H1("Rec_vs_P_gem")->Fill(P_sim);
        fHM->H1("Rec_vs_Eta_gem")->Fill(Eta_sim);
        fHM->H1("Rec_vs_Theta_gem")->Fill(Theta_sim);
        fHM->H2("Nh_rec_Eta_rec_gem")->Fill(N_sim, Eta_sim);
        fHM->H2("Nh_rec_P_rec_gem")->Fill(N_sim, P_sim);
        fHM->H2("Nh_rec_Theta_rec_gem")->Fill(N_sim, Theta_sim);

        if (!isTrackOk) {
            fHM->H1("Ghost_vs_P_gem")->Fill(P_sim);
            fHM->H1("Ghost_vs_Eta_gem")->Fill(Eta_sim);
            fHM->H1("Ghost_vs_Theta_gem")->Fill(Theta_sim);
            fHM->H1("Ghost_vs_Nh_gem")->Fill(N_rec);
            fHM->H1("Fakes_vs_EtaP_gem")->Fill(Eta_sim, P_sim);
        } else {
            fHM->H1("Well_vs_P_gem")->Fill(P_sim);
            fHM->H1("Well_vs_Eta_gem")->Fill(Eta_sim);
            fHM->H1("Well_vs_Theta_gem")->Fill(Theta_sim);
            fHM->H1("Well_vs_Nh_gem")->Fill(N_rec);

            Float_t chi2 = track->GetChi2() / track->GetNDF();
            fHM->H2("momRes_2D_gem")->Fill(P_sim, (P_sim - P_rec) / P_sim * 100.0);
            fHM->H2("MomRes_vs_Chi2_gem")->Fill(chi2, (P_sim - P_rec) / P_sim * 100.0);
            fHM->H2("MomRes_vs_Length_gem")->Fill(track->GetLength(), (P_sim - P_rec) / P_sim * 100.0);
            fHM->H2("Mom_vs_Chi2_gem")->Fill(chi2, P_rec);
            fHM->H2("Mom_vs_Length_gem")->Fill(track->GetLength(), P_rec);
            fHM->H1("Chi2_gem")->Fill(chi2);
            fHM->H1("Length_gem")->Fill(track->GetLength());
            fHM->H2("P_rec_P_sim_gem")->Fill(P_sim, P_rec);
            fHM->H2("Eta_rec_Eta_sim_gem")->Fill(Eta_sim, Eta_rec);
            fHM->H2("Px_rec_Px_sim_gem")->Fill(Px_sim, Px_rec);
            fHM->H2("Py_rec_Py_sim_gem")->Fill(Py_sim, Py_rec);
            fHM->H2("Pt_rec_Pt_sim_gem")->Fill(Pt_sim, Pz_rec * Sqrt(1 + Tx * Tx));
            fHM->H2("Tx_rec_Tx_sim_gem")->Fill(Px_sim / Pz_sim, Tx);
            fHM->H2("Ty_rec_Ty_sim_gem")->Fill(Py_sim / Pz_sim, Ty);
            fHM->H2("Pz_rec_Pz_sim_gem")->Fill(Pz_sim, Pz_rec);
            fHM->H2("EtaP_rec_gem")->Fill(Eta_rec, P_rec);
            fHM->H2("ThetaP_rec_gem")->Fill(Theta_rec, P_rec);

            FairTrackParam* pf = track->GetParamFirst();
            FairTrackParam* pl = track->GetParamLast();

            Double_t cov[15];

            pf->CovMatrix(cov);
            //first parameters
            fHM->H1("ResX_f_gem")->Fill(pnt.GetX() - pf->GetX());
            fHM->H1("ResY_f_gem")->Fill(pnt.GetY() - pf->GetY());
            fHM->H1("ResTx_f_gem")->Fill(pnt.GetTx() - pf->GetTx());
            fHM->H1("ResTy_f_gem")->Fill(pnt.GetTy() - pf->GetTy());
            fHM->H1("ResQp_f_gem")->Fill(pnt.GetQp() - pf->GetQp());
            fHM->H1("PullX_f_gem")->Fill((pnt.GetX() - pf->GetX()) / Sqrt(cov[0]));
            fHM->H1("PullY_f_gem")->Fill((pnt.GetY() - pf->GetY()) / Sqrt(cov[5]));
            fHM->H1("PullTx_f_gem")->Fill((pnt.GetTx() - pf->GetTx()) / Sqrt(cov[9]));
            fHM->H1("PullTy_f_gem")->Fill((pnt.GetTy() - pf->GetTy()) / Sqrt(cov[12]));
            fHM->H1("PullQp_f_gem")->Fill((pnt.GetQp() - pf->GetQp()) / Sqrt(cov[14]));

            pl->CovMatrix(cov);
            //last parameters
            fHM->H1("ResX_l_gem")->Fill(pntLast.GetX() - pl->GetX());
            fHM->H1("ResY_l_gem")->Fill(pntLast.GetY() - pl->GetY());
            fHM->H1("ResTx_l_gem")->Fill(pntLast.GetTx() - pl->GetTx());
            fHM->H1("ResTy_l_gem")->Fill(pntLast.GetTy() - pl->GetTy());
            fHM->H1("ResQp_l_gem")->Fill(pntLast.GetQp() - pl->GetQp());
            fHM->H1("PullX_l_gem")->Fill((pntLast.GetX() - pl->GetX()) / Sqrt(cov[0]));
            fHM->H1("PullY_l_gem")->Fill((pntLast.GetY() - pl->GetY()) / Sqrt(cov[5]));
            fHM->H1("PullTx_l_gem")->Fill((pntLast.GetTx() - pl->GetTx()) / Sqrt(cov[9]));
            fHM->H1("PullTy_l_gem")->Fill((pntLast.GetTy() - pl->GetTy()) / Sqrt(cov[12]));
            fHM->H1("PullQp_l_gem")->Fill((pntLast.GetQp() - pl->GetQp()) / Sqrt(cov[14]));

            fHM->H1("X_f_gem")->Fill(pf->GetX());
            fHM->H1("Y_f_gem")->Fill(pf->GetY());
            fHM->H1("Tx_f_gem")->Fill(pf->GetTx());
            fHM->H1("Ty_f_gem")->Fill(pf->GetTy());
            fHM->H1("Qp_f_gem")->Fill(pf->GetQp());

            fHM->H1("X_l_gem")->Fill(pl->GetX());
            fHM->H1("Y_l_gem")->Fill(pl->GetY());
            fHM->H1("Tx_l_gem")->Fill(pl->GetTx());
            fHM->H1("Ty_l_gem")->Fill(pl->GetTy());
            fHM->H1("Qp_l_gem")->Fill(pl->GetQp());
        }
    }

    for (Int_t i = 0; i < splits.size(); ++i) { //FIXME!!!
        if (!fMCTrackCreator->TrackExists(i)) continue;
        const BmnMCTrack mcTrack = fMCTrackCreator->GetTrack(i);
        if (fPrimes && ((const CbmMCTrack*) (fMCTracks->At(i)))->GetMotherId() != -1) continue;
        if (mcTrack.GetNofPoints(kGEM) < fMinNofPointsGem) continue;
        const BmnMCPoint mcPoint = mcTrack.GetPoint(kGEM, 0);

        Float_t Px = mcPoint.GetPx(); //mcTrack->GetPx();
        Float_t Py = mcPoint.GetPy(); //mcTrack->GetPy();
        Float_t Pz = mcPoint.GetPz(); //mcTrack->GetPz();
        Float_t P =  mcPoint.GetP(); //mcTrack->GetP();
        Float_t Pxy = Sqrt(Px * Px + Py * Py);
        Float_t eta = 0.5 * Log((P + Pz) / (P - Pz));
        Float_t theta = ATan2(Pxy, Pz) * RadToDeg();

        fHM->H1("Split_vs_P_gem")->Fill(P);
        fHM->H1("Split_vs_Eta_gem")->Fill(eta);
        fHM->H1("Split_vs_Theta_gem")->Fill(theta);
        fHM->H1("Well_vs_P_gem")->Fill(P, -1); //remove splitted tracks from efficiency
        fHM->H1("Well_vs_Eta_gem")->Fill(eta, -1); //remove splitted tracks from efficiency
        fHM->H1("Well_vs_Theta_gem")->Fill(theta, -1); //remove splitted tracks from efficiency
        fHM->H2("Eff_vs_EtaP_gem")->Fill(eta, P, -1);
        fHM->H2("Clones_vs_EtaP_gem")->Fill(eta, P);
    }

    for (Int_t iTrack = 0; iTrack < fMCTracks->GetEntriesFast(); iTrack++) {

        if (!fMCTrackCreator->TrackExists(iTrack)) continue;
        const BmnMCTrack mcTrack = fMCTrackCreator->GetTrack(iTrack);
        if (mcTrack.GetNofPoints(kGEM) < fMinNofPointsGem) continue;

        if (fPrimes && ((const CbmMCTrack*) (fMCTracks->At(iTrack)))->GetMotherId() != -1) continue;

        const BmnMCPoint pnt = mcTrack.GetPoint(kGEM, 0);

        Float_t Px = pnt.GetPx();
        Float_t Py = pnt.GetPy();
        Float_t Pz = pnt.GetPz();
        Float_t P = pnt.GetP();
        Float_t Pxy = Sqrt(Px * Px + Py * Py);
        Float_t eta = 0.5 * Log((P + Pz) / (P - Pz));
        Float_t theta = ATan2(Pxy, Pz) * RadToDeg();

        fHM->H2("EtaP_sim")->Fill(eta, P);
        fHM->H2("ThetaP_sim")->Fill(theta, P);
        fHM->H1("Sim_vs_P_gem")->Fill(P);
        fHM->H1("Sim_vs_Eta_gem")->Fill(eta);
        fHM->H1("Sim_vs_Theta_gem")->Fill(theta);
        fHM->H2("Nh_sim_Eta_sim_gem")->Fill(mcTrack.GetNofPoints(kGEM), eta);
        fHM->H2("Nh_sim_Theta_sim_gem")->Fill(mcTrack.GetNofPoints(kGEM), theta);
        fHM->H2("Nh_sim_P_sim_gem")->Fill(mcTrack.GetNofPoints(kGEM), P);
    }
}

void BmnTrackingQa::ProcessGlobal() {
    for (Int_t iTrack = 0; iTrack < fGlobalTracks->GetEntriesFast(); iTrack++) {
        BmnGlobalTrack* track = (BmnGlobalTrack*) (fGlobalTracks->At(iTrack));
        BmnTrackMatch* globTrackMatch = (BmnTrackMatch*) (fGlobalTrackMatches->At(iTrack));
        if (!track || !globTrackMatch) continue;
        if (globTrackMatch->GetNofLinks() == 0) continue;
        Int_t globMCId = globTrackMatch->GetMatchedLink().GetIndex();
        CbmMCTrack* mcTrack = (CbmMCTrack*) (fMCTracks->At(globMCId));
        if (!mcTrack) continue;
        Bool_t isTrackOk = globTrackMatch->GetTrueOverAllHitsRatio() >= fQuota;
        if (fPrimes && mcTrack->GetMotherId() != -1) continue;
        //        cout << "N true hits = " << globTrackMatch->GetNofTrueHits() << " | N wrong hits = " << globTrackMatch->GetNofWrongHits() << " | N all hits = " << globTrackMatch->GetNofHits() << endl;
        Float_t P_sim = mcTrack->GetP();
        if (P_sim < fPCut) continue; //calculate efficiency only for tracks with momentum > 100 MeV/c
        Float_t P_rec = Abs(1.0 / track->GetParamFirst()->GetQp());
        if (P_rec < fPCut) continue; //calculate efficiency only for tracks with momentum > 100 MeV/c
        Float_t Px_sim = mcTrack->GetPx();
        Float_t Py_sim = mcTrack->GetPy();
        Float_t Pz_sim = mcTrack->GetPz();
        Float_t Eta_sim = 0.5 * Log((P_sim + Pz_sim) / (P_sim - Pz_sim));
        if (Eta_sim > fEtaCut) continue; //exclude spectators from efficiency calculation
        Float_t Tx = track->GetParamFirst()->GetTx();
        Float_t Ty = track->GetParamFirst()->GetTy();
        Float_t Pz_rec = P_rec / Sqrt(Tx * Tx + Ty * Ty + 1);
        Float_t Px_rec = Pz_rec * Tx;
        Float_t Py_rec = Pz_rec * Ty;
        Float_t Eta_rec = 0.5 * Log((P_rec + Pz_rec) / (P_rec - Pz_rec));
        if (Eta_rec > fEtaCut) continue; //exclude spectators from efficiency calculation

        fHM->H1("Rec_vs_P_glob")->Fill(P_sim);

        if (!isTrackOk) {
            fHM->H1("Ghost_vs_P_glob")->Fill(P_sim);
            fHM->H1("Ghost_vs_Nh_glob")->Fill(track->GetNHits());
        } else {
            fHM->H1("Well_vs_P_glob")->Fill(P_sim);
            fHM->H1("Well_vs_Nh_glob")->Fill(track->GetNHits());

            fHM->H2("momRes_2D_glob")->Fill(P_sim, (P_sim - P_rec) / P_sim * 100.0);
            fHM->H2("P_rec_P_sim_glob")->Fill(P_sim, P_rec);
            fHM->H2("Eta_rec_Eta_sim_glob")->Fill(Eta_sim, Eta_rec);
            fHM->H2("Px_rec_Px_sim_glob")->Fill(Px_sim, Px_rec);
            fHM->H2("Py_rec_Py_sim_glob")->Fill(Py_sim, Py_rec);
            fHM->H2("Pt_rec_Pt_sim_glob")->Fill(Sqrt(Px_sim * Px_sim + Pz_sim * Pz_sim), Pz_rec * Sqrt(1 + Tx * Tx));
            fHM->H2("Tx_rec_Tx_sim_glob")->Fill(Px_sim / Pz_sim, Tx);
            fHM->H2("Ty_rec_Ty_sim_glob")->Fill(Py_sim / Pz_sim, Ty);
            fHM->H2("Pz_rec_Pz_sim_glob")->Fill(Pz_sim, Pz_rec);
            fHM->H2("EtaP_rec_glob")->Fill(Eta_rec, P_rec);
            fHM->H2("EtaP_sim")->Fill(Eta_sim, P_sim);
        }
    }
    Int_t momResStep = 10;
    for (Int_t iBin = 0; iBin < fHM->H2("momRes_2D_glob")->GetNbinsX(); iBin += momResStep) {
        TH1D* proj = fHM->H2("momRes_2D_glob")->ProjectionY("tmp", iBin, iBin + (momResStep - 1));
        fHM->H1("momRes_1D_glob")->SetBinContent(iBin, proj->GetBinCenter(proj->GetMaximumBin()));
    }
    for (Int_t iTrack = 0; iTrack < fMCTracks->GetEntriesFast(); iTrack++) {
        const CbmMCTrack* mcTrack = (const CbmMCTrack*) (fMCTracks->At(iTrack));
        if (mcTrack->GetNPoints(kGEM) < fMinNofPointsGem) continue;
        if (fPrimes && mcTrack->GetMotherId() != -1) continue;
        if (mcTrack->GetP() < fPCut) continue; //calculate efficiency only for tracks with momentum > 100 MeV/c
        if (0.5 * Log((mcTrack->GetP() + mcTrack->GetPz()) / (mcTrack->GetP() - mcTrack->GetPz())) > fEtaCut) continue; //exclude spectators from efficiency calculation
        fHM->H1("Sim_vs_P_glob")->Fill(mcTrack->GetP());
    }
}

void BmnTrackingQa::FillTrackQualityHistograms(const BmnTrackMatch* trackMatch, DetectorId detId) {
    string detName = (detId == kGEM) ? "Gem" : (detId == kTOF1) ? "Tof1" : (detId == kDCH) ? "Dch" : (detId == kTOF) ? "Tof2" : "";
    assert(detName != "");
    string histName = "hth_" + detName + "_TrackHits";
    fHM->H1(histName + "_All")->Fill(trackMatch->GetNofHits());
    fHM->H1(histName + "_True")->Fill(trackMatch->GetNofTrueHits());
    fHM->H1(histName + "_Fake")->Fill(trackMatch->GetNofWrongHits());
    fHM->H1(histName + "_TrueOverAll")->Fill(trackMatch->GetTrueOverAllHitsRatio());
    fHM->H1(histName + "_FakeOverAll")->Fill(trackMatch->GetWrongOverAllHitsRatio());
}

void BmnTrackingQa::FillGlobalReconstructionHistos(Int_t mcId, const multimap<Int_t, Int_t>& mcMap, const string& histName, const string& histTypeName, const string& effName, const string& catName, const vector<Double_t>& par) {
    string accHistName = FindAndReplace(histName, "_Eff_", "_Acc_");
    string recHistName = FindAndReplace(histName, "_Eff_", "_Rec_");
    Bool_t accOk = kFALSE; // = function(fMCTracks, mcId);
    if (catName == "All") {
        accOk = BmnAcceptanceFunction::AllTrackAcceptanceFunction(fMCTracks, mcId);
    } else if (catName == "Primary") {
        accOk = BmnAcceptanceFunction::PrimaryTrackAcceptanceFunction(fMCTracks, mcId);
    } else if (catName == "Secondary") {
        accOk = BmnAcceptanceFunction::SecondaryTrackAcceptanceFunction(fMCTracks, mcId);
    }
    Bool_t recOk = (histTypeName == "hte") ? (mcMap.find(mcId) != mcMap.end() && accOk) : kFALSE;
    Int_t nofParams = par.size();
    assert(nofParams < 3 && nofParams > 0);
    if (nofParams == 1) {
        if (accOk) {
            fHM->H1(accHistName)->Fill(par[0]);
        }
        if (recOk) {
            fHM->H1(recHistName)->Fill(par[0]);
        }
    } else if (nofParams == 2) {
        if (accOk) {
            fHM->H1(accHistName)->Fill(par[0], par[1]);
        }
        if (recOk) {
            fHM->H1(recHistName)->Fill(par[0], par[1]);
        }
    }
}

void BmnTrackingQa::IncreaseCounters() {

    //    fHM->H1("hno_NofObjects_GlobalTracks")->Fill(fGlobalTracks->GetEntriesFast());
    if (fDet.GetDet(kGEM)) {
        fHM->H1("hno_NofObjects_GemTracks")->Fill(fGemTracks->GetEntriesFast());
        fHM->H1("hno_NofObjects_GemHits")->Fill(fGemHits->GetEntriesFast());
    }
    if (fDet.GetDet(kTOF1) && fTof1Hits) fHM->H1("hno_NofObjects_Tof1Hits")->Fill(fTof1Hits->GetEntriesFast());
    if (fDet.GetDet(kDCH) && fDchHits) fHM->H1("hno_NofObjects_DchHits")->Fill(fDchHits->GetEntriesFast());
    if (fDet.GetDet(kTOF) && fTof2Hits) fHM->H1("hno_NofObjects_Tof2Hits")->Fill(fTof2Hits->GetEntriesFast());
}

ClassImp(BmnTrackingQa);
