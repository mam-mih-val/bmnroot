
/**
 * \file BmnTrackingQa.cxx
 * \author Andrey Lebedev <andrey.lebedev@gsi.de> - original author for CBM experiment
 * \author Sergey Merts <sergey.merts@gmail.com> - modifications for BMN experiment
 * \date 2007-2014
 */

#include "BmnTrackingQa.h"

#include <fstream>
#include <iostream>

#include "BmnAcceptanceFunction.h"
#include "BmnDchHit.h"
#include "BmnEnums.h"
#include "BmnGemStripHit.h"
#include "BmnGemTrack.h"
#include "BmnMCPoint.h"
#include "BmnMatch.h"
#include "BmnMath.h"
#include "BmnSiliconHit.h"
#include "BmnTrackMatch.h"
#include "BmnTrackingQaReport.h"
#include "BmnTrackingQaStudyReport.h"
#include "BmnUtils.h"
#include "CbmBaseHit.h"
#include "CbmGlobalTrack.h"
#include "CbmMCTrack.h"
#include "CbmStsTrack.h"
#include "CbmTofHit.h"
#include "CbmVertex.h"
#include "FairMCEventHeader.h"
#include "FairMCPoint.h"
#include "FairRunAna.h"
#include "TClonesArray.h"
#include "TF1.h"
#include "TFitResult.h"
#include "TH1.h"
#include "TH2F.h"
#include "report/BmnHistManager.h"

using namespace std;
using namespace TMath;
using lit::FindAndReplace;
using lit::Split;

BmnTrackingQa::BmnTrackingQa(Short_t ch, TString name, TString gemConf, TString silConf) : FairTask("BmnTrackingQA", 1),
                                                                                           fHM(nullptr),
                                                                                           fOutputDir("./"),
                                                                                           fMinNofPoints(4),
                                                                                           fMinNofPointsTof(1),
                                                                                           fMinNofPointsDch(1),
                                                                                           fQuota(0.6),
                                                                                           fEtaCut(100000.0),
                                                                                           fPCut(0.0),
                                                                                           fPRangeMin(0.),
                                                                                           fPRangeMax(4.),
                                                                                           fPRangeBins(50),
                                                                                           fYRangeMin(0.),
                                                                                           fYRangeMax(4.),
                                                                                           fYRangeBins(100),
                                                                                           fEtaRangeMin(0.),
                                                                                           fEtaRangeMax(8.),
                                                                                           fEtaRangeBins(50),
                                                                                           fPtRangeMin(0.),
                                                                                           fPtRangeMax(1.),
                                                                                           fPtRangeBins(50),
                                                                                           fThetaRangeMin(0.),
                                                                                           fThetaRangeMax(40.),
                                                                                           fThetaRangeBins(50),
                                                                                           fOutName(name),
                                                                                           fConfigGem(gemConf),
                                                                                           fConfigSil(silConf),
                                                                                           fMCTracks(nullptr),
                                                                                           fSilHits(nullptr),
                                                                                           fPrimes(kFALSE),
                                                                                           fNHitsCut(2000),
                                                                                           fNStations(0),
                                                                                           fGlobalTracks(nullptr),
                                                                                           fGemTracks(nullptr),
                                                                                           fSilTracks(nullptr) {
    fChargeCut = ch;
}

static Int_t nAllMcTracks = 0;
static Int_t nAllRecoTracks = 0;
static Int_t nAllRecoMachedTracks = 0;
static Int_t nGoodRecoTracks = 0;
static Int_t nBadRecoTracks = 0;

static Int_t nWellRecoInEvent = 0;
static Int_t nBadRecoInEvent = 0;
static Int_t nAllRecoInEvent = 0;

BmnTrackingQa::~BmnTrackingQa() {
    if (fHM) delete fHM;
}

InitStatus BmnTrackingQa::Init() {
    fHM = new BmnHistManager();
    CreateHistograms();
    ReadDataBranches();
    fMCTrackCreator = BmnMCTrackCreator::Instance(fConfigGem, fConfigSil);
    return kSUCCESS;
}

void BmnTrackingQa::Exec(Option_t* opt) {
    Int_t nHits = 0;
    if (fInnerTrackerSetup[kGEM]) nHits += fGemHits->GetEntriesFast();
    if (fInnerTrackerSetup[kSILICON]) nHits += fSilHits->GetEntriesFast();
    if (fInnerTrackerSetup[kSSD]) nHits += fSsdHits->GetEntriesFast();
    if (nHits > fNHitsCut || nHits == 0) return;
    // Increase event counter
    fHM->H1("hen_EventNo_TrackingQa")->Fill(0.5);
    ReadEventHeader();
    fMCTrackCreator->Create();
    ProcessGlobal();
}

void BmnTrackingQa::Finish() {
    fHM->WriteToFile();
    BmnSimulationReport* report = new BmnTrackingQaReport(fOutName);
    report->SetOnlyPrimes(fPrimes);
    report->Create(fHM, fOutputDir);
    delete report;

    printf("nAllMC = %d\n", nAllMcTracks);
    printf("nAllRecoMached = %d\n", nAllRecoMachedTracks);
    printf("nAllReco = %d\n", nAllRecoTracks);
    printf("nGoodReco = %d\n", nGoodRecoTracks);
    printf("nBadReco = %d\n", nBadRecoTracks);
}

void BmnTrackingQa::ReadDataBranches() {
    FairRootManager* ioman = FairRootManager::Instance();
    if (nullptr == ioman) Fatal("Init", "BmnRootManager is not instantiated");

    fMCTracks = (TClonesArray*)ioman->GetObject("MCTrack");
    if (nullptr == fMCTracks) Fatal("Init", "No MCTrack array!");

    fGlobalTracks = (TClonesArray*)ioman->GetObject("BmnGlobalTrack");
    fGlobalTrackMatches = (TClonesArray*)ioman->GetObject("BmnGlobalTrackMatch");

    fGemTracks = (TClonesArray*)ioman->GetObject("BmnGemTrack");
    fSilTracks = (TClonesArray*)ioman->GetObject("BmnSiliconTrack");

    fNStations = 0;
    fInnerHits = (TClonesArray*)ioman->GetObject("BmnInnerHits");

    if (fInnerTrackerSetup[kSILICON]) {
        fSilHits = (TClonesArray*)ioman->GetObject("BmnSiliconHit");
        fSilDetector = new BmnSiliconStationSet(fConfigSil);
        fNStations += fSilDetector->GetNStations();
    }

    if (fInnerTrackerSetup[kSSD]) {
        fSsdHits = (TClonesArray*)ioman->GetObject("BmnSSDHit");
        fNStations += 4;
    }

    if (fInnerTrackerSetup[kGEM]) {
        fGemHits = (TClonesArray*)ioman->GetObject("BmnGemStripHit");
        fGemDetector = new BmnGemStripStationSet(fConfigGem);
        fNStations += fGemDetector->GetNStations();
    }

    fVertex = (TClonesArray*)ioman->GetObject("BmnVertex");
}

void BmnTrackingQa::ReadEventHeader() {
    FairMCEventHeader* evHead = (FairMCEventHeader*)FairRootManager::Instance()->GetObject("MCEventHeader.");
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

void BmnTrackingQa::CreateHistograms() {
    // Number of points distributions
    const Float_t minNofPoints = -0.5;
    const Float_t maxNofPoints = 14.5;
    const Int_t nofBinsPoints = 15;

    // Create number of object histograms
    UInt_t nofBinsC = 100000;
    Double_t maxXC = (Double_t)nofBinsC;
    CreateH1("hno_NofObjects_GlobalTracks", "Tracks per event", "Yield", nofBinsC, 1., maxXC);
    CreateH1("hno_NofObjects_GemTracks", "Tracks per event", "Yield", nofBinsC, 1., maxXC);
    CreateH1("hno_NofObjects_GemHits", "GEM hits per event", "Yield", nofBinsC, 1., maxXC);

    // Histogram stores number of events
    CreateH1("hen_EventNo_TrackingQa", "", "", 1, 0, 1.);
    CreateH1("Impact parameter", "b, fm", "Counter", 100, 0.0, 0.0);
    CreateH1("Multiplicity", "N_{prim}", "Counter", 100, 0.0, 0.0);
    CreateH2("Impact_Mult", "b, fm", "N_{prim}", "", 100, 0.0, 0.0, 100, 0.0, 0.0);

    // Physics
    CreateH2("momRes_2D", "P_{sim}/q, GeV/c", "#Delta P / P, %", "", 100, fPRangeMin, 10, 100, -10.0, 10.0);
    CreateH1("momRes_1D", "P_{sim}/q, GeV/c", "#LT#Delta P / P#GT, %", fPRangeBins / 2, fPRangeMin, 10);
    CreateH1("momMean_1D", "P_{sim}/q, GeV/c", "#Delta P / P, #mu, %", fPRangeBins / 2, fPRangeMin, 10);
    CreateH1("momRes_Mean", "#Delta P / P, %", "Counter", 100, -10.0, 10.0);

    CreateH2("MomRes_vs_Theta", "#theta_{sim}", "#Delta P / P, %", "", fThetaRangeBins * 2, fThetaRangeMin, fThetaRangeMax, 100, -10, 10);
    CreateH1("MomRes_vs_Theta_1D", "#theta_{sim}", "#Delta P / P, #sigma, %", fThetaRangeBins / 2, fThetaRangeMin, fThetaRangeMax);
    CreateH1("MomMean_vs_Theta_1D", "#theta_{sim}", "#Delta P / P, #mu, %", fThetaRangeBins / 2, fThetaRangeMin, fThetaRangeMax);

    CreateH2("MomRes_vs_Length", "Length, cm", "#Delta P / P, %", "", 400, 0, 200, 100, -10, 10);
    CreateH1("MomRes_vs_Length_1D", "Length, cm", "#Delta P / P, #sigma, %", 50, 0, 200);
    CreateH1("MomMean_vs_Length_1D", "Length, cm", "#Delta P / P, #mu, %", 50, 0, 200);

    CreateH2("MomRes_vs_nHits", "N_{hits}", "#Delta P / P, %", "", 12, 0, 12, 100, -10, 10);
    CreateH1("MomRes_vs_nHits_1D", "N_{hits}", "#Delta P / P, #sigma, %", 12, 0, 12);
    CreateH1("MomMean_vs_nHits_1D", "N_{hits}", "#Delta P / P, #mu, %", 12, 0, 12);

    CreateH2("EtaP_rec", "#eta_{rec}", "P_{rec}/q, GeV/c", "", 100, fEtaRangeMin, fEtaRangeMax, 100, fPRangeMin, fPRangeMax);
    CreateH2("EtaP_sim", "#eta_{sim}", "P_{sim}/q, GeV/c", "", 100, fEtaRangeMin, fEtaRangeMax, 100, fPRangeMin, fPRangeMax);
    CreateH2("EtaP_fakes_sim", "#eta_{sim}", "P_{sim}/q, GeV/c", "", 100, fEtaRangeMin, fEtaRangeMax, 100, fPRangeMin, fPRangeMax);
    CreateH2("EtaP_clones_sim", "#eta_{sim}", "P_{sim}/q, GeV/c", "", 100, fEtaRangeMin, fEtaRangeMax, 100, fPRangeMin, fPRangeMax);
    CreateH2("ThetaP_sim", "#theta_{sim}", "P_{sim}/q, GeV/c", "", 100, fThetaRangeMin, fThetaRangeMax, 100, fPRangeMin, fPRangeMax);
    CreateH2("ThetaP_rec", "#theta_{rec}", "P_{rec}/q, GeV/c", "", 100, fThetaRangeMin, fThetaRangeMax, 100, fPRangeMin, fPRangeMax);

    CreateH2("P_rec_P_sim", "P_{sim}/q, GeV/c", "P_{rec}/q, GeV/c", "", 100, fPRangeMin, fPRangeMax, 100, fPRangeMin, fPRangeMax);
    CreateH2("Px_rec_Px_sim", "P^{x}_{sim}/q, GeV/c", "P^{x}/q_{rec}, GeV/c", "", 100, -fPtRangeMax, fPtRangeMax, 100, -fPtRangeMax, fPtRangeMax);
    CreateH2("Py_rec_Py_sim", "P^{y}_{sim}/q, GeV/c", "P^{y}/q_{rec}, GeV/c", "", 100, -fPtRangeMax, fPtRangeMax, 100, -fPtRangeMax, fPtRangeMax);
    CreateH2("Pz_rec_Pz_sim", "P^{z}_{sim}/q, GeV/c", "P^{z}/q_{rec}, GeV/c", "", 100, fPtRangeMin, 4, 100, fPtRangeMin, 4);
    CreateH2("Pt_rec_Pt_sim", "P^{t}_{sim}/q, GeV/c", "P^{t}/q_{rec}, GeV/c", "", 100, fPtRangeMin, 4, 100, fPtRangeMin, 4);
    CreateH2("Eta_rec_Eta_sim", "#eta_{sim}", "#eta_{rec}", "", 100, fEtaRangeMin, fEtaRangeMax, 100, fEtaRangeMin, fEtaRangeMax);

    CreateH2("Tx_rec_Tx_sim", "T^{x}_{sim}", "T^{x}_{rec}", "", 100, -1.0, 1.0, 100, -1.0, 1.0);
    CreateH2("Ty_rec_Ty_sim", "T^{y}_{sim}", "T^{y}_{rec}", "", 100, -1.0, 1.0, 100, -1.0, 1.0);
    CreateH2("Nh_rec_Nh_sim", "Number of mc-points", "Number of reco-hits", "", nofBinsPoints, minNofPoints, maxNofPoints, nofBinsPoints, minNofPoints, maxNofPoints);

    CreateH2("Nh_sim_Eta_sim", "Number of mc-points", "#eta_{sim}", "", nofBinsPoints, minNofPoints, maxNofPoints, 100, fEtaRangeMin, fEtaRangeMax);
    CreateH2("Nh_rec_Eta_rec", "Number of hits", "#eta_{rec}", "", nofBinsPoints, minNofPoints, maxNofPoints, 100, fEtaRangeMin, fEtaRangeMax);
    CreateH2("Nh_sim_P_sim", "Number of mc-points", "P_{sim}", "", nofBinsPoints, minNofPoints, maxNofPoints, 100, fPRangeMin, 2 * fPRangeMax);
    CreateH2("Nh_rec_P_rec", "Number of hits", "P_{rec}", "", nofBinsPoints, minNofPoints, maxNofPoints, 100, fPRangeMin, 2 * fPRangeMax);
    CreateH2("Nh_sim_Theta_sim", "Number of mc-points", "#theta_{sim}", "", nofBinsPoints, minNofPoints, maxNofPoints, 100, fThetaRangeMin, fThetaRangeMax);
    CreateH2("Nh_rec_Theta_rec", "Number of hits", "#theta_{rec}", "", nofBinsPoints, minNofPoints, maxNofPoints, 100, fThetaRangeMin, fThetaRangeMax);

    CreateH1("Eff_vs_P", "P_{sim}/q, GeV/c", "Efficiency, %", fPRangeBins, fPRangeMin, fPRangeMax);
    CreateH1("Split_vs_P", "P_{sim}/q, GeV/c", "Counter", fPRangeBins, fPRangeMin, fPRangeMax);
    CreateH1("SplitEff_vs_P", "P_{sim}/q, GeV/c", "Splits, %", fPRangeBins, fPRangeMin, fPRangeMax);
    CreateH1("Sim_vs_P", "P_{sim}/q, GeV/c", "Counter", fPRangeBins, fPRangeMin, fPRangeMax);
    CreateH1("Rec_vs_P", "P_{sim}/q, GeV/c", "Counter", fPRangeBins, fPRangeMin, fPRangeMax);
    CreateH1("Ghost_vs_P", "P_{sim}/q, GeV/c", "Counter", fPRangeBins, fPRangeMin, fPRangeMax);
    CreateH1("Well_vs_P", "P_{sim}/q, GeV/c", "Counter", fPRangeBins, fPRangeMin, fPRangeMax);
    CreateH1("Fake_vs_P", "P_{sim}/q, GeV/c", "Ghosts, %", fPRangeBins, fPRangeMin, fPRangeMax);

    CreateH1("Eff_vs_P_wide", "P_{sim}/q, GeV/c", "Efficiency, %", fPRangeBins, fPRangeMin, 10);
    CreateH1("Split_vs_P_wide", "P_{sim}/q, GeV/c", "Counter", fPRangeBins, fPRangeMin, 10);
    CreateH1("SplitEff_vs_P_wide", "P_{sim}/q, GeV/c", "Splits, %", fPRangeBins, fPRangeMin, 10);
    CreateH1("Sim_vs_P_wide", "P_{sim}/q, GeV/c", "Counter", fPRangeBins, fPRangeMin, 10);
    CreateH1("Rec_vs_P_wide", "P_{sim}/q, GeV/c", "Counter", fPRangeBins, fPRangeMin, 10);
    CreateH1("Ghost_vs_P_wide", "P_{sim}/q, GeV/c", "Counter", fPRangeBins, fPRangeMin, 10);
    CreateH1("Well_vs_P_wide", "P_{sim}/q, GeV/c", "Counter", fPRangeBins, fPRangeMin, 10);
    CreateH1("Fake_vs_P_wide", "P_{sim}/q, GeV/c", "Ghosts, %", fPRangeBins, fPRangeMin, 10);

    CreateH1("Eff_vs_Eta", "#eta_{sim}", "Efficiency, %", fEtaRangeBins, fEtaRangeMin, fEtaRangeMax);
    CreateH1("Split_vs_Eta", "#eta_{sim}", "Counter", fEtaRangeBins, fEtaRangeMin, fEtaRangeMax);
    CreateH1("SplitEff_vs_Eta", "#eta_{sim}", "Splits, %", fEtaRangeBins, fEtaRangeMin, fEtaRangeMax);
    CreateH1("Sim_vs_Eta", "#eta_{sim}", "Counter", fEtaRangeBins, fEtaRangeMin, fEtaRangeMax);
    CreateH1("Rec_vs_Eta", "#eta_{sim}", "Counter", fEtaRangeBins, fEtaRangeMin, fEtaRangeMax);
    CreateH1("Ghost_vs_Eta", "#eta_{sim}", "Counter", fEtaRangeBins, fEtaRangeMin, fEtaRangeMax);
    CreateH1("Well_vs_Eta", "#eta_{sim}", "Counter", fEtaRangeBins, fEtaRangeMin, fEtaRangeMax);
    CreateH1("Fake_vs_Eta", "#eta_{sim}", "Ghosts, %", fEtaRangeBins, fEtaRangeMin, fEtaRangeMax);

    CreateH1("Eff_vs_Theta", "#theta_{sim}", "Efficiency, %", fThetaRangeBins, fThetaRangeMin, fThetaRangeMax);
    CreateH1("Split_vs_Theta", "#theta_{sim}", "Counter", fThetaRangeBins, fThetaRangeMin, fThetaRangeMax);
    CreateH1("SplitEff_vs_Theta", "#theta_{sim}", "Splits, %", fThetaRangeBins, fThetaRangeMin, fThetaRangeMax);
    CreateH1("Sim_vs_Theta", "#theta_{sim}", "Counter", fThetaRangeBins, fThetaRangeMin, fThetaRangeMax);
    CreateH1("Rec_vs_Theta", "#theta_{sim}", "Counter", fThetaRangeBins, fThetaRangeMin, fThetaRangeMax);
    CreateH1("Ghost_vs_Theta", "#theta_{sim}", "Counter", fThetaRangeBins, fThetaRangeMin, fThetaRangeMax);
    CreateH1("Well_vs_Theta", "#theta_{sim}", "Counter", fThetaRangeBins, fThetaRangeMin, fThetaRangeMax);
    CreateH1("Fake_vs_Theta", "#theta_{sim}", "Ghosts, %", fThetaRangeBins, fThetaRangeMin, fThetaRangeMax);

    CreateH1("Eff_vs_Nh", "Number of hits", "Efficiency, %", nofBinsPoints, minNofPoints, maxNofPoints);
    CreateH1("Ghost_vs_Nh", "Number of hits", "Counter", nofBinsPoints, minNofPoints, maxNofPoints);
    CreateH1("SplitEff_vs_Nh", "Number of hits", "Splits, %", nofBinsPoints, minNofPoints, maxNofPoints);
    CreateH1("Split_vs_Nh", "Number of hits", "Counter", nofBinsPoints, minNofPoints, maxNofPoints);
    CreateH1("Sim_vs_Nh", "Number of hits", "Counter", nofBinsPoints, minNofPoints, maxNofPoints);
    CreateH1("Rec_vs_Nh", "Number of hits", "Counter", nofBinsPoints, minNofPoints, maxNofPoints);
    CreateH1("Well_vs_Nh", "Number of hits", "Counter", nofBinsPoints, minNofPoints, maxNofPoints);
    CreateH1("Fake_vs_Nh", "Number of hits", "Ghosts, %", nofBinsPoints, minNofPoints, maxNofPoints);

    CreateH2("MomRes_vs_Chi2", "#chi^{2}", "#Delta P / P, %", "", 100, 0, 10, 100, -10, 10);
    CreateH2("Mom_vs_Chi2", "#chi^{2}", "P_{rec}/q, GeV/c", "", 100, 0, 10, 100, fPRangeMin, fPRangeMax);
    CreateH2("Mom_vs_Length", "Length, cm", "P_{rec}/q, GeV/c", "", 400, 0, 200, 100, fPRangeMin, fPRangeMax);
    CreateH1("Chi2", "#chi^{2} / NDF", "Counter", 100, 0, 10);
    CreateH1("Length", "length, cm", "Counter", 400, 0, 200);

    CreateH1("VertResX", "#DeltaV_{x}, cm", "Counter", 100, -1.0, 1.0);
    CreateH1("VertResY", "#DeltaV_{y}, cm", "Counter", 100, -1.0, 1.0);
    CreateH1("VertResZ", "#DeltaV_{z}, cm", "Counter", 100, -10.0, 10.0);
    CreateH1("VertX", "V_{x}, cm", "Counter", 300, -1.0, 1.0);
    CreateH1("VertY", "V_{y}, cm", "Counter", 300, -6.0, 0.0);
    CreateH1("VertZ", "V_{z}, cm", "Counter", 300, -10.0, +10.0);
    CreateH2("VertX_vs_Ntracks", "Number of tracks", "V_{x}, cm", "", 10, 0, 10, 300, -1.0, 1.0);
    CreateH2("VertY_vs_Ntracks", "Number of tracks", "V_{y}, cm", "", 10, 0, 10, 300, -6.0, 0.0);
    CreateH2("VertZ_vs_Ntracks", "Number of tracks", "V_{z}, cm", "", 10, 0, 10, 300, -10.0, +10.0);

    CreateH2("Eff_vs_EtaP", "#eta_{sim}", "P_{sim, GeV/c}", "", 100, fEtaRangeMin, fEtaRangeMax, 100, fPRangeMin, fPRangeMax);
    CreateH2("Clones_vs_EtaP", "#eta_{sim}", "P_{sim, GeV/c}", "", 100, fEtaRangeMin, fEtaRangeMax, 100, fPRangeMin, fPRangeMax);
    CreateH2("Fakes_vs_EtaP", "#eta_{sim}", "P_{sim, GeV/c}", "", 100, fEtaRangeMin, fEtaRangeMax, 100, fPRangeMin, fPRangeMax);

    //for first parameters
    CreateH1("ResX_f", "Residual X, cm", "", 100, -20, 20);
    CreateH1("ResY_f", "Residual Y, cm", "", 100, -20, 20);
    CreateH1("ResTx_f", "Residual t_{x}", "", 100, -0.5, 0.5);
    CreateH1("ResTy_f", "Residual t_{y}", "", 100, -0.5, 0.5);
    CreateH1("ResQp_f", "Residual q/p, (GeV/c)^{-1}", "", 100, -10.0, 10.0);
    CreateH1("ErrX_f", "Error X, cm", "", 100, -1, 1);
    CreateH1("ErrY_f", "Error Y, cm", "", 100, -1, 1);
    CreateH1("ErrTx_f", "Error t_{x}", "", 100, -0.1, 0.1);
    CreateH1("ErrTy_f", "Error t_{y}", "", 100, -0.1, 0.1);
    CreateH1("ErrQp_f", "Error q/p, (GeV/c)^{-1}", "", 100, -1.0, 1.0);
    CreateH1("PullX_f", "Pull X", "", 100, -4.0, 4.0);
    CreateH1("PullY_f", "Pull Y", "", 100, -4.0, 4.0);
    CreateH1("PullTx_f", "Pull t_{x}", "", 100, -4.0, 4.0);
    CreateH1("PullTy_f", "Pull t_{y}", "", 100, -4.0, 4.0);
    CreateH1("PullQp_f", "Pull q/p", "", 100, -4.0, 4.0);
    //for last parameters
    CreateH1("ResX_l", "Residual X, cm", "", 100, -20, 20);
    CreateH1("ResY_l", "Residual Y, cm", "", 100, -20, 20);
    CreateH1("ResTx_l", "Residual t_{x}", "", 100, -0.5, 0.5);
    CreateH1("ResTy_l", "Residual t_{y}", "", 100, -0.5, 0.5);
    CreateH1("ResQp_l", "Residual q/p, (GeV/c)^{-1}", "", 100, -10.0, 10.0);
    CreateH1("ErrX_l", "Error X, cm", "", 100, -1, 1);
    CreateH1("ErrY_l", "Error Y, cm", "", 100, -1, 1);
    CreateH1("ErrTx_l", "Error t_{x}", "", 100, -0.1, 0.1);
    CreateH1("ErrTy_l", "Error t_{y}", "", 100, -0.1, 0.1);
    CreateH1("ErrQp_l", "Error q/p, (GeV/c)^{-1}", "", 100, -1.0, 1.0);
    CreateH1("PullX_l", "Pull X", "", 100, -4.0, 4.0);
    CreateH1("PullY_l", "Pull Y", "", 100, -4.0, 4.0);
    CreateH1("PullTx_l", "Pull t_{x}", "", 100, -4.0, 4.0);
    CreateH1("PullTy_l", "Pull t_{y}", "", 100, -4.0, 4.0);
    CreateH1("PullQp_l", "Pull q/p", "", 100, -4.0, 4.0);

    CreateH1("X_f", "X, cm", "", 100, -100, 100);
    CreateH1("Y_f", "Y, cm", "", 100, -100, 100);
    CreateH1("Tx_f", "t_{x}", "", 100, -1.0, 1.0);
    CreateH1("Ty_f", "t_{y}", "", 100, -1.0, 1.0);
    CreateH1("Qp_f", "q/p, (GeV/c)^{-1}", "", 100, -20.0, 20.0);
    CreateH1("X_l", "X, cm", "", 100, -100, 100);
    CreateH1("Y_l", "Y, cm", "", 100, -100, 100);
    CreateH1("Tx_l", "t_{x}", "", 100, -1.0, 1.0);
    CreateH1("Ty_l", "t_{y}", "", 100, -1.0, 1.0);
    CreateH1("Qp_l", "q/p, (GeV/c)^{-1}", "", 100, -20.0, 20.0);

    CreateH1("Efficiency_vs_multiplicity", "N", "Efficiency, %", 100, 0.0, 100.0);
    CreateH1("Well_vs_multiplicity", "N", "Efficiency, %", 100, 0.0, 100.0);
    CreateH1("Efficiency_vs_multiplicity", "N", "Efficiency, %", 100, 0.0, 100.0);

    Int_t nMultBin = 100;
    CreateH1("Eff_vs_mult", "N", "Efficiency, %", nMultBin, 0, nMultBin);
    CreateH1("Split_vs_mult", "N", "Counter", nMultBin, 0, nMultBin);
    CreateH1("SplitEff_vs_mult", "N", "Splits, %", nMultBin, 0, nMultBin);
    CreateH1("Sim_vs_mult", "N", "Counter", nMultBin, 0, nMultBin);
    CreateH1("Rec_vs_mult", "N", "Counter", nMultBin, 0, nMultBin);
    CreateH1("Ghost_vs_mult", "N", "Counter", nMultBin, 0, nMultBin);
    CreateH1("Well_vs_mult", "N", "Counter", nMultBin, 0, nMultBin);
    CreateH1("Fake_vs_mult", "N", "Ghosts, %", nMultBin, 0, nMultBin);

    //hits residuals
    for (Int_t iSt = 0; iSt < 9; ++iSt) {
        CreateH1(Form("ResX_%dst", iSt), "ResX, cm", "Counter", 100, -0.5, 0.5);
        CreateH1(Form("ResY_%dst", iSt), "ResY, cm", "Counter", 100, -0.5, 0.5);
    }
}

void BmnTrackingQa::ProcessGlobal() {
    vector<Int_t> refs;
    vector<Int_t> splits;

    nWellRecoInEvent = 0;
    nBadRecoInEvent = 0;
    nAllRecoInEvent = 0;

    CbmVertex* vrt = (fVertex == nullptr) ? nullptr : (CbmVertex*)fVertex->At(0);

    if (vrt != nullptr) {
        fHM->H1("VertResX")->Fill(vrt->GetX() - 0.5);
        fHM->H1("VertResY")->Fill(vrt->GetY() - (-4.6));
        fHM->H1("VertResZ")->Fill(vrt->GetZ() - (-2.3));
        fHM->H1("VertX")->Fill(vrt->GetX());
        fHM->H1("VertY")->Fill(vrt->GetY());
        fHM->H1("VertZ")->Fill(vrt->GetZ());
        fHM->H1("VertX_vs_Ntracks")->Fill(fGlobalTracks->GetEntriesFast(), vrt->GetX());
        fHM->H1("VertY_vs_Ntracks")->Fill(fGlobalTracks->GetEntriesFast(), vrt->GetY());
        fHM->H1("VertZ_vs_Ntracks")->Fill(fGlobalTracks->GetEntriesFast(), vrt->GetZ());
    }

    for (Int_t iTrack = 0; iTrack < fGlobalTracks->GetEntriesFast(); iTrack++) {
        BmnGlobalTrack* glTrack = (BmnGlobalTrack*)(fGlobalTracks->At(iTrack));
        if (!glTrack) continue;
        if (glTrack->GetParamFirst()->GetQp() * fChargeCut < 0) continue;
        nAllRecoTracks++;
        nAllRecoInEvent++;
        BmnTrackMatch* globTrackMatch = (BmnTrackMatch*)(fGlobalTrackMatches->At(iTrack));
        if (!globTrackMatch) continue;
        if (globTrackMatch->GetNofLinks() == 0) continue;
        Int_t globMCId = globTrackMatch->GetMatchedLink().GetIndex();
        if (!fMCTrackCreator->TrackExists(globMCId)) continue;
        const BmnMCTrack mcTrack = fMCTrackCreator->GetTrack(globMCId);

        Int_t nSil = mcTrack.GetNofPoints(kSILICON);
        Int_t nSsd = mcTrack.GetNofPoints(kSSD);
        Int_t nGem = mcTrack.GetNofPoints(kGEM);
        Int_t nHits = 0;

        if (fInnerTrackerSetup[kSILICON]) nHits += nSil;
        if (fInnerTrackerSetup[kSSD]) nHits += nSsd;
        if (fInnerTrackerSetup[kGEM]) nHits += nGem;

        if (nHits < fMinNofPoints || nHits > fNStations) continue;

        BmnMCPoint pntFirst;
        BmnMCPoint pntLast;

        if (fInnerTrackerSetup[kGEM] && nGem != 0)
            pntLast = mcTrack.GetPoint(kGEM, nGem - 1);
        else if (fInnerTrackerSetup[kSSD] && nSsd != 0)
            pntLast = mcTrack.GetPoint(kSSD, nSsd - 1);
        else if (fInnerTrackerSetup[kSILICON] && nSil != 0)
            pntLast = mcTrack.GetPoint(kSILICON, nSil - 1);
        else
            continue;

        if (fInnerTrackerSetup[kSILICON] && nSil != 0)
            pntFirst = mcTrack.GetPoint(kSILICON, 0);
        else if (fInnerTrackerSetup[kSSD] && nSsd != 0)
            pntFirst = mcTrack.GetPoint(kSSD, 0);
        else if (fInnerTrackerSetup[kGEM] && nGem != 0)
            pntFirst = mcTrack.GetPoint(kGEM, 0);
        else
            continue;

        vector<Int_t>::iterator it = find(refs.begin(), refs.end(), globMCId);
        if (it != refs.end())
            splits.push_back(globMCId);
        else
            refs.push_back(globMCId);

        Int_t Nsil = (glTrack->GetSilTrackIndex() != -1) ? ((BmnTrack*)fSilTracks->At(glTrack->GetSilTrackIndex()))->GetNHits() : 0;
        Int_t Ngem = (glTrack->GetGemTrackIndex() != -1) ? ((BmnTrack*)fGemTracks->At(glTrack->GetGemTrackIndex()))->GetNHits() : 0;
        Int_t N_rec = Nsil + Ngem;  //glTrack->GetNHits();
        Bool_t isTrackOk = globTrackMatch->GetTrueOverAllHitsRatio() >= fQuota && N_rec >= fMinNofPoints;
        //        printf("globTrackMatch->GetTrueOverAllHitsRatio() = %f\n", globTrackMatch->GetTrueOverAllHitsRatio());
        Float_t P_sim = pntFirst.GetP();
        Float_t Px_sim = pntFirst.GetPx();
        Float_t Py_sim = pntFirst.GetPy();
        Float_t Pz_sim = pntFirst.GetPz();
        Float_t P_rec = Abs(1.0 / glTrack->GetParamFirst()->GetQp());
        Float_t Tx = glTrack->GetParamFirst()->GetTx();
        Float_t Ty = glTrack->GetParamFirst()->GetTy();
        Float_t Pz_rec = P_rec / Sqrt(Tx * Tx + Ty * Ty + 1);
        Float_t Px_rec = Pz_rec * Tx;
        Float_t Py_rec = Pz_rec * Ty;
        Float_t Pxy_rec = Sqrt(Px_rec * Px_rec + Py_rec * Py_rec);
        Float_t Pxy_sim = Sqrt(Px_sim * Px_sim + Py_sim * Py_sim);
        Float_t Theta_sim = ATan2(Pxy_sim, Pz_sim) * RadToDeg();
        Float_t Theta_rec = ATan2(Pxy_rec, Pz_rec) * RadToDeg();
        Float_t Eta_sim = 0.5 * Log((P_sim + Pz_sim) / (P_sim - Pz_sim));
        Float_t Eta_rec = 0.5 * Log((P_rec + Pz_rec) / (P_rec - Pz_rec));

        fHM->H1("Rec_vs_P")->Fill(P_sim);
        fHM->H1("Rec_vs_P_wide")->Fill(P_sim);
        fHM->H1("Rec_vs_Eta")->Fill(Eta_sim);
        fHM->H1("Rec_vs_Theta")->Fill(Theta_sim);
        fHM->H1("Rec_vs_Nh")->Fill(N_rec);
        fHM->H2("Nh_rec_Eta_rec")->Fill(N_rec, Eta_rec);
        fHM->H2("Nh_rec_P_rec")->Fill(N_rec, P_rec);
        fHM->H2("Nh_rec_Theta_rec")->Fill(N_rec, Theta_rec);

        nAllRecoMachedTracks++;
        if (!isTrackOk) {
            nBadRecoTracks++;
            nBadRecoInEvent++;
            //            badTracks++;
            fHM->H1("Ghost_vs_P")->Fill(P_sim);
            fHM->H1("Ghost_vs_P_wide")->Fill(P_sim);
            fHM->H1("Ghost_vs_Nh")->Fill(N_rec);
            fHM->H1("Ghost_vs_Eta")->Fill(Eta_sim);
            fHM->H1("Ghost_vs_Theta")->Fill(Theta_sim);
            fHM->H1("Fakes_vs_EtaP")->Fill(Eta_sim, P_sim);

            // BmnHit* hit0 = (BmnHit*)fInnerHits->At(glTrack->GetHitIndex(0));
            // Int_t stPrev = hit0->GetStation();
            // for (Int_t iHit = 1; iHit < glTrack->GetNHits(); ++iHit) {
            //     BmnHit* hit = (BmnHit*)fInnerHits->At(glTrack->GetHitIndex(iHit));
            //     Int_t st = hit->GetStation();
            //     Int_t dSt = st - stPrev;
            //     stPrev = st;
            //     if (dSt == 1) continue;
            // }

        } else {
            nGoodRecoTracks++;
            nWellRecoInEvent++;
            //            goodTracks++;
            fHM->H1("Well_vs_P_wide")->Fill(P_sim);
            fHM->H1("Well_vs_P")->Fill(P_sim);
            fHM->H1("Well_vs_Nh")->Fill(N_rec);
            fHM->H1("Well_vs_Eta")->Fill(Eta_sim);
            fHM->H1("Well_vs_Theta")->Fill(Theta_sim);

            fHM->H1("Nh_rec_Nh_sim")->Fill(CalcNumberOfMcPointInTrack(mcTrack), N_rec);

            Float_t chi2 = glTrack->GetChi2() / glTrack->GetNDF();
            fHM->H2("momRes_2D")->Fill(P_sim, (P_sim - P_rec) / P_sim * 100.0);
            fHM->H2("MomRes_vs_Chi2")->Fill(chi2, (P_sim - P_rec) / P_sim * 100.0);
            fHM->H2("MomRes_vs_Length")->Fill(glTrack->GetLength(), (P_sim - P_rec) / P_sim * 100.0);
            fHM->H2("MomRes_vs_nHits")->Fill(glTrack->GetNHits(), (P_sim - P_rec) / P_sim * 100.0);
            fHM->H2("MomRes_vs_Theta")->Fill(Theta_sim, (P_sim - P_rec) / P_sim * 100.0);
            fHM->H2("Mom_vs_Chi2")->Fill(chi2, P_rec);
            fHM->H2("Mom_vs_Length")->Fill(glTrack->GetLength(), P_rec);
            fHM->H1("Chi2")->Fill(chi2);
            fHM->H1("Length")->Fill(glTrack->GetLength());

            fHM->H2("P_rec_P_sim")->Fill(P_sim, P_rec);
            fHM->H2("Eta_rec_Eta_sim")->Fill(Eta_sim, Eta_rec);
            fHM->H2("Px_rec_Px_sim")->Fill(Px_sim, Px_rec);
            fHM->H2("Py_rec_Py_sim")->Fill(Py_sim, Py_rec);
            fHM->H2("Pt_rec_Pt_sim")->Fill(Sqrt(Px_sim * Px_sim + Pz_sim * Pz_sim), Pz_rec * Sqrt(1 + Tx * Tx));
            fHM->H2("Tx_rec_Tx_sim")->Fill(Px_sim / Pz_sim, Tx);
            fHM->H2("Ty_rec_Ty_sim")->Fill(Py_sim / Pz_sim, Ty);
            fHM->H2("Pz_rec_Pz_sim")->Fill(Pz_sim, Pz_rec);
            fHM->H2("EtaP_rec")->Fill(Eta_rec, P_rec);
            fHM->H2("ThetaP_rec")->Fill(Theta_rec, P_rec);

            FairTrackParam* pf = glTrack->GetParamFirst();
            FairTrackParam* pl = glTrack->GetParamLast();

            Double_t cov[15];

            pf->CovMatrix(cov);
            //first parameters
            fHM->H1("ResX_f")->Fill(pntFirst.GetX() - pf->GetX());
            fHM->H1("ResY_f")->Fill(pntFirst.GetY() - pf->GetY());
            fHM->H1("ResTx_f")->Fill(pntFirst.GetTx() - pf->GetTx());
            fHM->H1("ResTy_f")->Fill(pntFirst.GetTy() - pf->GetTy());
            fHM->H1("ResQp_f")->Fill(pntFirst.GetQp() - pf->GetQp());
            fHM->H1("ErrX_f")->Fill(Sqrt(cov[0]));
            fHM->H1("ErrY_f")->Fill(Sqrt(cov[5]));
            fHM->H1("ErrTx_f")->Fill(Sqrt(cov[9]));
            fHM->H1("ErrTy_f")->Fill(Sqrt(cov[12]));
            fHM->H1("ErrQp_f")->Fill(Sqrt(cov[14]));
            fHM->H1("PullX_f")->Fill((pntFirst.GetX() - pf->GetX()) / Sqrt(cov[0]));
            fHM->H1("PullY_f")->Fill((pntFirst.GetY() - pf->GetY()) / Sqrt(cov[5]));
            fHM->H1("PullTx_f")->Fill((pntFirst.GetTx() - pf->GetTx()) / Sqrt(cov[9]));
            fHM->H1("PullTy_f")->Fill((pntFirst.GetTy() - pf->GetTy()) / Sqrt(cov[12]));
            fHM->H1("PullQp_f")->Fill((pntFirst.GetQp() - pf->GetQp()) / Sqrt(cov[14]));

            pl->CovMatrix(cov);
            //last parameters
            fHM->H1("ResX_l")->Fill(pntLast.GetX() - pl->GetX());
            fHM->H1("ResY_l")->Fill(pntLast.GetY() - pl->GetY());
            fHM->H1("ResTx_l")->Fill(pntLast.GetTx() - pl->GetTx());
            fHM->H1("ResTy_l")->Fill(pntLast.GetTy() - pl->GetTy());
            fHM->H1("ResQp_l")->Fill(pntLast.GetQp() - pl->GetQp());
            fHM->H1("ErrX_l")->Fill(Sqrt(cov[0]));
            fHM->H1("ErrY_l")->Fill(Sqrt(cov[5]));
            fHM->H1("ErrTx_l")->Fill(Sqrt(cov[9]));
            fHM->H1("ErrTy_l")->Fill(Sqrt(cov[12]));
            fHM->H1("ErrQp_l")->Fill(Sqrt(cov[14]));
            fHM->H1("PullX_l")->Fill((pntLast.GetX() - pl->GetX()) / Sqrt(cov[0]));
            fHM->H1("PullY_l")->Fill((pntLast.GetY() - pl->GetY()) / Sqrt(cov[5]));
            fHM->H1("PullTx_l")->Fill((pntLast.GetTx() - pl->GetTx()) / Sqrt(cov[9]));
            fHM->H1("PullTy_l")->Fill((pntLast.GetTy() - pl->GetTy()) / Sqrt(cov[12]));
            fHM->H1("PullQp_l")->Fill((pntLast.GetQp() - pl->GetQp()) / Sqrt(cov[14]));

            fHM->H1("X_f")->Fill(pf->GetX());
            fHM->H1("Y_f")->Fill(pf->GetY());
            fHM->H1("Tx_f")->Fill(pf->GetTx());
            fHM->H1("Ty_f")->Fill(pf->GetTy());
            fHM->H1("Qp_f")->Fill(pf->GetQp());

            fHM->H1("X_l")->Fill(pl->GetX());
            fHM->H1("Y_l")->Fill(pl->GetY());
            fHM->H1("Tx_l")->Fill(pl->GetTx());
            fHM->H1("Ty_l")->Fill(pl->GetTy());
            fHM->H1("Qp_l")->Fill(pl->GetQp());

            //
            //            if (fInnerTrackerSetup[kSILICON]) {
            //                for (Int_t iHit = 0; iHit < fSilHits->GetEntriesFast(); ++iHit) {
            //                    BmnHit* hit = (BmnHit*) fSilHits->At(iHit);
            //                    fHM->H1(Form("ResX_%dst", hit->GetStation()))->Fill(hit->GetResX());
            //                    fHM->H1(Form("ResY_%dst", hit->GetStation()))->Fill(hit->GetResY());
            //                }
            //            }
            //
            //            for (Int_t iHit = 0; iHit < fGemHits->GetEntriesFast(); ++iHit) {
            //                BmnHit* hit = (BmnHit*) fGemHits->At(iHit);
            //                fHM->H1(Form("ResX_%dst", hit->GetStation() + 3))->Fill(hit->GetResX());
            //                fHM->H1(Form("ResY_%dst", hit->GetStation() + 3))->Fill(hit->GetResY());
            //            }
        }
    }

    //    printf("%d + %d = %d\n", goodTracks, badTracks, allTracks);

    Int_t nSplitInOneEvent = 0;

    for (Int_t i = 0; i < splits.size(); ++i) {  //FIXME!!!
        if (!fMCTrackCreator->TrackExists(i)) continue;
        const BmnMCTrack mcTrack = fMCTrackCreator->GetTrack(i);
        if (fPrimes && ((const CbmMCTrack*)(fMCTracks->At(i)))->GetMotherId() != -1) continue;
        //        vector<BmnMCPoint> points = mcTrack.GetPoints(kGEM);
        //        set<Int_t> uniqStations;
        //        for (Int_t iPnt = 0; iPnt < points.size(); ++iPnt)
        //            uniqStations.insert(points[iPnt].GetStationId());
        //        if (uniqStations.size() < fMinNofPoints) continue;

        Int_t nPointsPerTrack = CalcNumberOfMcPointInTrack(mcTrack);
        if (nPointsPerTrack < fMinNofPoints) continue;

        BmnMCPoint pnt;
        if (mcTrack.GetNofPoints(kSILICON) != 0)
            pnt = mcTrack.GetPoint(kSILICON, 0);
        else if (mcTrack.GetNofPoints(kSSD) != 0)
            pnt = mcTrack.GetPoint(kSSD, 0);
        else if (mcTrack.GetNofPoints(kGEM) != 0)
            pnt = mcTrack.GetPoint(kGEM, 0);

        if (pnt.GetQ() * fChargeCut < 0) continue;

        nSplitInOneEvent++;
        nWellRecoInEvent--;

        Float_t Px = pnt.GetPx();            //mcTrack->GetPx();
        Float_t Py = pnt.GetPy();            //mcTrack->GetPy();
        Float_t Pz = pnt.GetPz();            //mcTrack->GetPz();
        Float_t P = Abs(1.0 / pnt.GetQp());  //mcPoint.GetP(); //mcTrack->GetP();
        Float_t Pxy = Sqrt(Px * Px + Py * Py);
        Float_t eta = 0.5 * Log((P + Pz) / (P - Pz));
        Float_t theta = ATan2(Pxy, Pz) * RadToDeg();

        fHM->H1("Split_vs_P")->Fill(P);
        fHM->H1("Split_vs_P_wide")->Fill(P);
        fHM->H1("Split_vs_Eta")->Fill(eta);
        fHM->H1("Split_vs_Theta")->Fill(theta);
        fHM->H1("Split_vs_Nh")->Fill(nPointsPerTrack);
        fHM->H1("Well_vs_P")->Fill(P, -1);          //remove splitted tracks from efficiency
        fHM->H1("Well_vs_Eta")->Fill(eta, -1);      //remove splitted tracks from efficiency
        fHM->H1("Well_vs_Theta")->Fill(theta, -1);  //remove splitted tracks from efficiency
        fHM->H1("Well_vs_P_wide")->Fill(P, -1);     //remove splitted tracks from efficiency
        fHM->H2("Eff_vs_EtaP")->Fill(eta, P, -1);
        fHM->H2("Clones_vs_EtaP")->Fill(eta, P);
    }

    // Int_t momResStep = 10;
    // for (Int_t iBin = 0; iBin < fHM->H2("momRes_2D")->GetNbinsX(); iBin += momResStep) {
    //     TH1D* proj = fHM->H2("momRes_2D")->ProjectionY("tmp", iBin, iBin + (momResStep - 1));
    //     fHM->H1("momRes_1D")->SetBinContent(iBin, proj->GetBinCenter(proj->GetMaximumBin()));
    // }
    // for (Int_t iBin = 0; iBin < fHM->H2("MomRes_vs_Theta")->GetNbinsX(); iBin += momResStep) {
    //     TH1D* proj = fHM->H2("MomRes_vs_Theta")->ProjectionY("tmp", iBin, iBin + (momResStep - 1));
    //     fHM->H1("MomRes_vs_Theta_1D")->SetBinContent(iBin, proj->GetBinCenter(proj->GetMaximumBin()));
    // }
    Int_t nReconstructable = 0;

    for (Int_t iTrack = 0; iTrack < fMCTracks->GetEntriesFast(); iTrack++) {
        if (!fMCTrackCreator->TrackExists(iTrack)) continue;  //?
        const BmnMCTrack mcTrack = fMCTrackCreator->GetTrack(iTrack);

        Int_t nHitsPerTrack = CalcNumberOfMcPointInTrack(mcTrack);
        if (nHitsPerTrack < fMinNofPoints) continue;
        nAllMcTracks++;
        nReconstructable++;

        BmnMCPoint pnt;
        if (mcTrack.GetNofPoints(kSILICON) != 0)
            pnt = mcTrack.GetPoint(kSILICON, 0);
        else if (mcTrack.GetNofPoints(kSSD) != 0)
            pnt = mcTrack.GetPoint(kSSD, 0);
        else if (mcTrack.GetNofPoints(kGEM) != 0)
            pnt = mcTrack.GetPoint(kGEM, 0);

        if (pnt.GetQ() * fChargeCut < 0) continue;

        Float_t Px = pnt.GetPx();
        Float_t Py = pnt.GetPy();
        Float_t Pz = pnt.GetPz();
        Float_t P = pnt.GetP();
        Float_t Pxy = Sqrt(Px * Px + Py * Py);
        Float_t eta = 0.5 * Log((P + Pz) / (P - Pz));
        Float_t theta = ATan2(Pxy, Pz) * RadToDeg();

        fHM->H1("Sim_vs_P")->Fill(P);
        fHM->H1("Sim_vs_P_wide")->Fill(P);
        fHM->H1("Sim_vs_Eta")->Fill(eta);
        fHM->H1("Sim_vs_Theta")->Fill(theta);
        fHM->H1("Sim_vs_Nh")->Fill(nHitsPerTrack);
        fHM->H2("Nh_sim_Eta_sim")->Fill(nHitsPerTrack, eta);
        fHM->H2("Nh_sim_Theta_sim")->Fill(nHitsPerTrack, theta);
        fHM->H2("Nh_sim_P_sim")->Fill(nHitsPerTrack, P);
        fHM->H2("ThetaP_sim")->Fill(theta, P);
        fHM->H2("EtaP_sim")->Fill(eta, P);
    }

    fHM->H1("Sim_vs_mult")->SetBinContent(nReconstructable, fHM->H1("Sim_vs_mult")->GetBinContent(nReconstructable) + nReconstructable);
    fHM->H1("Well_vs_mult")->SetBinContent(nReconstructable, fHM->H1("Well_vs_mult")->GetBinContent(nReconstructable) + nWellRecoInEvent);
    fHM->H1("Rec_vs_mult")->SetBinContent(nReconstructable, fHM->H1("Rec_vs_mult")->GetBinContent(nReconstructable) + nAllRecoInEvent);
    fHM->H1("Split_vs_mult")->SetBinContent(nReconstructable, fHM->H1("Split_vs_mult")->GetBinContent(nReconstructable) + nSplitInOneEvent);
    fHM->H1("Ghost_vs_mult")->SetBinContent(nReconstructable, fHM->H1("Ghost_vs_mult")->GetBinContent(nReconstructable) + nBadRecoInEvent);
}

Int_t BmnTrackingQa::CalcNumberOfMcPointInTrack(BmnMCTrack mcTrack) {
    Int_t nHitsOnStation[fNStations];
    for (Int_t i = 0; i < fNStations; ++i)
        nHitsOnStation[i] = 0;
    vector<BmnMCPoint> pointsGem = mcTrack.GetPoints(kGEM);
    for (BmnMCPoint pntGEM : pointsGem) {
        nHitsOnStation[pntGEM.GetStationId()]++;
    }
    vector<BmnMCPoint> pointsSil = mcTrack.GetPoints(kSILICON);
    for (BmnMCPoint pntSIL : pointsSil)
        nHitsOnStation[pntSIL.GetStationId()]++;

    Int_t nHitsPerTrack = 0;
    Bool_t isGood = kTRUE;
    for (Int_t iSt = 0; iSt < fNStations; iSt++) {
        if (nHitsOnStation[iSt] == 0) continue;
        if (nHitsOnStation[iSt] > 1) return -1;
        nHitsPerTrack++;
    }
    return nHitsPerTrack;
}

ClassImp(BmnTrackingQa);
