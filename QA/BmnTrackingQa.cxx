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
#include "BmnDchTrack.h"
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
#include "BmnGlobalTrack.h"
#include "CbmMCTrack.h"
#include "CbmStsTrack.h"
#include "CbmTofHit.h"
#include "BmnVertex.h"
#include "FairMCEventHeader.h"
#include "FairMCPoint.h"
#include "FairRunAna.h"
#include "TClonesArray.h"
#include "TF1.h"
#include "TFitResult.h"
#include "TH1.h"
#include "TH2F.h"
#include "BmnHistManager.h"
#include "BmnSiliconPoint.h"
#include "CbmStsPoint.h"

using namespace std;
using namespace TMath;
using lit::FindAndReplace;
using lit::Split;

BmnTrackingQa::BmnTrackingQa(Short_t ch, TString name, TString gemConf, TString silConf, TString cscConf) : FairTask("BmnTrackingQA", 1),
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
fConfigCsc(cscConf),
fMCTracks(nullptr),
fSilHits(nullptr),
fTof400Hits(nullptr),
fTof700Hits(nullptr),
fCscHits(nullptr),
fDchTracks(nullptr),
fCscPoints(nullptr),
fSilPoints(nullptr),
fGemPoints(nullptr),
fInnerTrackBranchName("StsTrack"),
fPrimes(kFALSE),
fNHitsCut(1000),
fNStations(0),
fGlobalTracks(nullptr),
fGemTracks(nullptr),
fEventNo(0),
fStsHits(nullptr),
fSilTracks(nullptr) {
    fChargeCut = ch;
}

static Int_t nAllMcTracks = 0;
static Int_t nAllRecoTracks = 0;
static Int_t nGoodRecoTracks = 0;
static Int_t nBadRecoTracks = 0;
static Int_t nSplitRecoTracks = 0;

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
    return kSUCCESS;
}

void BmnTrackingQa::Exec(Option_t* opt) {
    if (fEventNo % 100 == 0) printf("Event: %d\n", fEventNo);
    fEventNo++;
    Int_t nHits = 0;
    // if (fInnerTrackerSetup[kGEM]) nHits += fGemHits->GetEntriesFast();
    // if (fInnerTrackerSetup[kSILICON]) nHits += fSilHits->GetEntriesFast();
    // if (fInnerTrackerSetup[kSSD]) nHits += fSsdHits->GetEntriesFast();
    // if (nHits > fNHitsCut || nHits == 0) return;
    // Increase event counter
    fHM->H1("hen_EventNo_TrackingQa")->Fill(0.5);
    ReadEventHeader();
    ProcessGlobal();
}

void BmnTrackingQa::Finish() {
    
    BmnSimulationReport* report = new BmnTrackingQaReport(fOutName);
    report->SetOnlyPrimes(fPrimes);
    report->Create(fHM, fOutputDir);
    fHM->WriteToFile();
    delete report;

    
    printf("nAllMC = %d\n", nAllMcTracks);
    printf("nAllReco = %d\n", nAllRecoTracks);
    printf("nGoodReco = %d\n", nGoodRecoTracks);
    printf("nBadReco = %d\n", nBadRecoTracks);
    printf("nSplitReco = %d\n", nSplitRecoTracks);
}

void BmnTrackingQa::ReadDataBranches() {
    FairRootManager* ioman = FairRootManager::Instance();
    if (nullptr == ioman) Fatal("Init", "BmnRootManager is not instantiated");

    fMCTracks = (TClonesArray*)ioman->GetObject("MCTrack");
    if (nullptr == fMCTracks) Fatal("Init", "No MCTrack array!");

    fGlobalTracks = (TClonesArray*)ioman->GetObject("BmnGlobalTrack");
    if (nullptr == fGlobalTracks) Fatal("Init", "No BmnGlobalTrack array!");
    fGlobalTrackMatches = (TClonesArray*)ioman->GetObject("BmnGlobalTrackMatch");

    fGemTracks = (TClonesArray*)ioman->GetObject("BmnGemTrack");
    fSilTracks = (TClonesArray*)ioman->GetObject("BmnSiliconTrack");

    fNStations = 0;
    fInnerHits = (TClonesArray*)ioman->GetObject("BmnInnerHits");

    fTof400Hits = (TClonesArray*)ioman->GetObject("BmnTof400Hit");
    fTof700Hits = (TClonesArray*)ioman->GetObject("BmnTof700Hit");
    fCscHits = (TClonesArray*)ioman->GetObject("BmnCSCHit");
    fCscPoints = (TClonesArray*)ioman->GetObject("CSCPoint");
    fDchTracks = (TClonesArray*)ioman->GetObject("BmnDchTrack");
    fSilPoints = (TClonesArray*)ioman->GetObject("SiliconPoint");
    fGemPoints = (TClonesArray*)ioman->GetObject("StsPoint");

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

    fStsHits = (TClonesArray*)ioman->GetObject("StsHit");
    fStsTracks = (TClonesArray*)ioman->GetObject(fInnerTrackBranchName);

    fVertex = (TClonesArray*)ioman->GetObject("BmnVertex");
    fVertexL1 = (CbmVertex*)ioman->GetObject("PrimaryVertex.");

    printf("\nBRANCHES READ!\n\n");
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
    string type[] = { "All", "True", "Fake", "TrueOverAll", "FakeOverAll" };
    Double_t min[] = { 0., 0., 0., 0., 0. };
    Double_t max[] = { 20, 20, 20, 1., 1. };
    Int_t bins[] = { 20, 20, 20, 20, 20 };
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

    CreateH2("PxRes_2D", "Px_{sim}/q, GeV/c", "#Delta Px / Px, %", "", 100, fPRangeMin, 1, 100, -10.0, 10.0);
    CreateH1("PxRes_1D", "Px_{sim}/q, GeV/c", "#LT#Delta Px / Px#GT, %", fPRangeBins / 2, fPRangeMin, 1);
    CreateH1("PxMean_1D", "Px_{sim}/q, GeV/c", "#Delta Px / Px, #mu, %", fPRangeBins / 2, fPRangeMin, 1);
    CreateH1("PxRes_Mean", "#Delta Px / Px, %", "Counter", 100, -10.0, 10.0);

    CreateH2("PyRes_2D", "Py_{sim}/q, GeV/c", "#Delta Py / Py, %", "", 100, fPRangeMin, 1, 100, -10.0, 10.0);
    CreateH1("PyRes_1D", "Py_{sim}/q, GeV/c", "#LT#Delta Py / Py#GT, %", fPRangeBins / 2, fPRangeMin, 1);
    CreateH1("PyMean_1D", "Py_{sim}/q, GeV/c", "#Delta Py / Py, #mu, %", fPRangeBins / 2, fPRangeMin, 1);
    CreateH1("PyRes_Mean", "#Delta Py / Py, %", "Counter", 100, -10.0, 10.0);

    CreateH2("PzRes_2D", "Pz_{sim}/q, GeV/c", "#Delta Pz / Pz, %", "", 100, fPRangeMin, 8, 100, -10.0, 10.0);
    CreateH1("PzRes_1D", "Pz_{sim}/q, GeV/c", "#LT#Delta Pz / Pz#GT, %", fPRangeBins / 2, fPRangeMin, 8);
    CreateH1("PzMean_1D", "Pz_{sim}/q, GeV/c", "#Delta Pz / Pz, #mu, %", fPRangeBins / 2, fPRangeMin, 8);
    CreateH1("PzRes_Mean", "#Delta Pz / Pz, %", "Counter", 100, -10.0, 10.0);

    CreateH2("MomRes_vs_Theta", "#theta_{sim}", "#Delta P / P, %", "", fThetaRangeBins * 2, fThetaRangeMin, fThetaRangeMax, 100, -10, 10);
    CreateH1("MomRes_vs_Theta_1D", "#theta_{sim}", "#Delta P / P, #sigma, %", fThetaRangeBins / 2, fThetaRangeMin, fThetaRangeMax);
    CreateH1("MomMean_vs_Theta_1D", "#theta_{sim}", "#Delta P / P, #mu, %", fThetaRangeBins / 2, fThetaRangeMin, fThetaRangeMax);

    CreateH2("MomRes_vs_Length", "Length, cm", "#Delta P / P, %", "", 400, 0, 800, 100, -10, 10);
    CreateH1("MomRes_vs_Length_1D", "Length, cm", "#Delta P / P, #sigma, %", 50, 0, 800);
    CreateH1("MomMean_vs_Length_1D", "Length, cm", "#Delta P / P, #mu, %", 50, 0, 800);

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
    CreateH2("EtaRes_2D", "#eta_{sim}", "#Delta #eta / #eta, %", "", 100, 0, 4, 100, -5.0, 5.0);
    CreateH1("EtaRes_1D", "#eta_{sim}", "#LT#Delta #eta / #eta#GT, %", fPRangeBins / 2, 0, 4);
    CreateH1("EtaMean_1D", "#eta_{sim}", "#Delta #eta / #eta, #mu, %", fPRangeBins / 2, 0, 4);
    CreateH1("EtaRes_Mean", "#Delta #eta / #eta, %", "Counter", 100, -5.0, 5.0);

    CreateH2("Tx_rec_Tx_sim", "T^{x}_{sim}", "T^{x}_{rec}", "", 100, -1.0, 1.0, 100, -1.0, 1.0);
    CreateH2("TxRes_2D", "Tx_{sim}", "#Delta Tx / Tx, %", "", 100, -1, 1, 100, -10.0, 10.0);
    CreateH1("TxRes_1D", "Tx_{sim}", "#LT#Delta Tx / Tx#GT, %", fPRangeBins / 2, -1, 1);
    CreateH1("TxMean_1D", "Tx_{sim}", "#Delta Tx / Tx, #mu, %", fPRangeBins / 2, -1, 1);
    CreateH1("TxRes_Mean", "#Delta Tx / Tx, %", "Counter", 100, -10.0, 10.0);

    CreateH2("Ty_rec_Ty_sim", "T^{y}_{sim}", "T^{y}_{rec}", "", 100, -1.0, 1.0, 100, -1.0, 1.0);
    CreateH2("TyRes_2D", "Ty_{sim}", "#Delta Ty / Ty, %", "", 100, -1, 1, 100, -10.0, 10.0);
    CreateH1("TyRes_1D", "Ty_{sim}", "#LT#Delta Ty / Ty#GT, %", fPRangeBins / 2, -1, 1);
    CreateH1("TyMean_1D", "Ty_{sim}", "#Delta Ty / Ty, #mu, %", fPRangeBins / 2, -1, 1);
    CreateH1("TyRes_Mean", "#Delta Ty / Ty, %", "Counter", 100, -10.0, 10.0);

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
    CreateH2("Mom_vs_Length", "Length, cm", "P_{rec}/q, GeV/c", "", 400, 0, 800, 100, fPRangeMin, fPRangeMax);
    CreateH1("Chi2", "#chi^{2} / NDF", "Counter", 100, 0, 10);
    CreateH1("Length", "length, cm", "Counter", 400, 0, 800);

    CreateH1("VertResX", "#DeltaV_{x}, cm", "Counter", 100, -2.0, 2.0);
    CreateH1("VertResY", "#DeltaV_{y}, cm", "Counter", 100, -2.0, 2.0);
    CreateH1("VertResZ", "#DeltaV_{z}, cm", "Counter", 100, -2.0, 2.0);
    CreateH1("VertX", "V_{x}, cm", "Counter", 300, -1.0, 1.0);
    CreateH1("VertY", "V_{y}, cm", "Counter", 300, -1.0, 1.0);
    CreateH1("VertZ", "V_{z}, cm", "Counter", 300, -2.0, +2.0);
    CreateH2("VertX_vs_Ntracks", "Number of tracks", "V_{x}, cm", "", 100, 0, 100, 300, -1.0, 1.0);
    CreateH2("VertY_vs_Ntracks", "Number of tracks", "V_{y}, cm", "", 100, 0, 100, 300, -1.0, 1.0);
    CreateH2("VertZ_vs_Ntracks", "Number of tracks", "V_{z}, cm", "", 100, 0, 100, 300, -2.0, +2.0);

    CreateH2("Eff_vs_EtaP", "#eta_{sim}", "P_{sim, GeV/c}", "", 100, fEtaRangeMin, fEtaRangeMax, 100, fPRangeMin, fPRangeMax);
    CreateH2("Clones_vs_EtaP", "#eta_{sim}", "P_{sim, GeV/c}", "", 100, fEtaRangeMin, fEtaRangeMax, 100, fPRangeMin, fPRangeMax);
    CreateH2("Fakes_vs_EtaP", "#eta_{sim}", "P_{sim, GeV/c}", "", 100, fEtaRangeMin, fEtaRangeMax, 100, fPRangeMin, fPRangeMax);

    //for first parameters
    CreateH1("ResX_f", "Residual X, cm", "", 100, -20, 20);
    CreateH1("ResY_f", "Residual Y, cm", "", 100, -20, 20);
    CreateH1("ResTx_f", "Residual t_{x}", "", 100, -0.5, 0.5);
    CreateH1("ResTy_f", "Residual t_{y}", "", 100, -0.5, 0.5);
    CreateH1("ResQp_f", "Residual q/p, (GeV/c)^{-1}", "", 100, -2.0, 2.0);
    CreateH1("ErrX_f", "Error X, cm", "", 100, -0, 1);
    CreateH1("ErrY_f", "Error Y, cm", "", 100, -0, 1);
    CreateH1("ErrTx_f", "Error t_{x}", "", 100, -0, 0.1);
    CreateH1("ErrTy_f", "Error t_{y}", "", 100, -0, 0.1);
    CreateH1("ErrQp_f", "Error q/p, (GeV/c)^{-1}", "", 100, -0, 0.2);
    CreateH1("PullX_f", "Pull X", "", 100, -5.0, 5.0);
    CreateH1("PullY_f", "Pull Y", "", 100, -5.0, 5.0);
    CreateH1("PullTx_f", "Pull t_{x}", "", 100, -5.0, 5.0);
    CreateH1("PullTy_f", "Pull t_{y}", "", 100, -5.0, 5.0);
    CreateH1("PullQp_f", "Pull q/p", "", 100, -5.0, 5.0);

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

    Int_t nMultBin = 100;
    CreateH1("Eff_vs_mult", "N", "Efficiency, %", nMultBin, 0, nMultBin);
    CreateH1("Split_vs_mult", "N", "Counter", nMultBin, 0, nMultBin);
    CreateH1("SplitEff_vs_mult", "N", "Splits, %", nMultBin, 0, nMultBin);
    CreateH1("Sim_vs_mult", "N", "Counter", nMultBin, 0, nMultBin);
    CreateH1("Rec_vs_mult", "N", "Counter", nMultBin, 0, nMultBin);
    CreateH1("Ghost_vs_mult", "N", "Counter", nMultBin, 0, nMultBin);
    CreateH1("Well_vs_mult", "N", "Counter", nMultBin, 0, nMultBin);
    CreateH1("Fake_vs_mult", "N", "Ghosts, %", nMultBin, 0, nMultBin);


    //TOF
    CreateH1("Eff_vs_P_tof400", "P_{sim}/q, GeV/c", "Efficiency, %", fPRangeBins, 0.0, 8.0);
    CreateH1("Split_vs_P_tof400", "P_{sim}/q, GeV/c", "Counter", fPRangeBins, 0.0, 8.0);
    CreateH1("SplitEff_vs_P_tof400", "P_{sim}/q, GeV/c", "Splits, %", fPRangeBins, 0.0, 8.0);
    CreateH1("Sim_vs_P_tof400", "P_{sim}/q, GeV/c", "Counter", fPRangeBins, 0.0, 8.0);
    CreateH1("Rec_vs_P_tof400", "P_{sim}/q, GeV/c", "Counter", fPRangeBins, 0.0, 8.0);
    CreateH1("Ghost_vs_P_tof400", "P_{sim}/q, GeV/c", "Counter", fPRangeBins, 0.0, 8.0);
    CreateH1("Well_vs_P_tof400", "P_{sim}/q, GeV/c", "Counter", fPRangeBins, 0.0, 8.0);
    CreateH1("Fake_vs_P_tof400", "P_{sim}/q, GeV/c", "Ghosts, %", fPRangeBins, 0.0, 8.0);
    CreateH1("Eff_vs_P_tof700", "P_{sim}/q, GeV/c", "Efficiency, %", fPRangeBins, 0.0, 8.0);
    CreateH1("Split_vs_P_tof700", "P_{sim}/q, GeV/c", "Counter", fPRangeBins, 0.0, 8.0);
    CreateH1("SplitEff_vs_P_tof700", "P_{sim}/q, GeV/c", "Splits, %", fPRangeBins, 0.0, 8.0);
    CreateH1("Sim_vs_P_tof700", "P_{sim}/q, GeV/c", "Counter", fPRangeBins, 0.0, 8.0);
    CreateH1("Rec_vs_P_tof700", "P_{sim}/q, GeV/c", "Counter", fPRangeBins, 0.0, 8.0);
    CreateH1("Ghost_vs_P_tof700", "P_{sim}/q, GeV/c", "Counter", fPRangeBins, 0.0, 8.0);
    CreateH1("Well_vs_P_tof700", "P_{sim}/q, GeV/c", "Counter", fPRangeBins, 0.0, 8.0);
    CreateH1("Fake_vs_P_tof700", "P_{sim}/q, GeV/c", "Ghosts, %", fPRangeBins, 0.0, 8.0);

    CreateH2("banana_tof400", "P_{rec}/Q, GeV/c", "#beta_{rec}", "", 500, -4, 10, 500, 0.5, 1.1);
    CreateH2("banana_tof700", "P_{rec}/Q, GeV/c", "#beta_{rec}", "", 500, -4, 10, 500, 0.5, 1.1);
    CreateH2("banana_tof400_good", "P_{rec}/Q, GeV/c", "#beta_{rec}", "", 500, -4, 10, 500, 0.5, 1.1);
    CreateH2("banana_tof700_good", "P_{rec}/Q, GeV/c", "#beta_{rec}", "", 500, -4, 10, 500, 0.5, 1.1);
    CreateH1("x_residuals_tof400_good", "dX, cm", "count", 250, -5.0, 5.0);
    CreateH1("y_residuals_tof400_good", "dY, cm", "count", 250, -5.0, 5.0);
    CreateH1("x_residuals_tof400_bad", "dX, cm", "count", 250, -5.0, 5.0);
    CreateH1("y_residuals_tof400_bad", "dY, cm", "count", 250, -5.0, 5.0);
    CreateH2("x_resi_vs_mom_tof400_good", "P/Q, GeV/c/Q", "dX, cm", "", 500, 0, 8, 500, -5.0, 5.0);
    CreateH2("y_resi_vs_mom_tof400_good", "P/Q, GeV/c/Q", "dY, cm", "", 500, 0, 8, 500, -5.0, 5.0);
    CreateH1("x_residuals_tof700_good", "dX, cm", "count", 250, -5.0, 5.0);
    CreateH1("y_residuals_tof700_good", "dY, cm", "count", 250, -5.0, 5.0);
    CreateH2("x_resi_vs_mom_tof700_good", "P/Q, GeV/c/Q", "dX, cm", "", 500, 0, 8, 500, -5.0, 5.0);
    CreateH2("y_resi_vs_mom_tof700_good", "P/Q, GeV/c/Q", "dY, cm", "", 500, 0, 8, 500, -5.0, 5.0);
    CreateH1("x_residuals_tof700_bad", "dX, cm", "count", 250, -5.0, 5.0);
    CreateH1("y_residuals_tof700_bad", "dY, cm", "count", 250, -5.0, 5.0);
    CreateH2("Mass_correlation", "M_{tof400}/Q, GeV/c^{2}/Q", "M_{tof700}/Q, GeV/c^{2}/Q", "", 500, 0, 3, 500, 0, 3);

    //CSC
    CreateH1("Eff_vs_P_csc", "P_{sim}/q, GeV/c", "Efficiency, %", fPRangeBins, 0.0, 8.0);
    CreateH1("Split_vs_P_csc", "P_{sim}/q, GeV/c", "Counter", fPRangeBins, 0.0, 8.0);
    CreateH1("SplitEff_vs_P_csc", "P_{sim}/q, GeV/c", "Splits, %", fPRangeBins, 0.0, 8.0);
    CreateH1("Sim_vs_P_csc", "P_{sim}/q, GeV/c", "Counter", fPRangeBins, 0.0, 8.0);
    CreateH1("Rec_vs_P_csc", "P_{sim}/q, GeV/c", "Counter", fPRangeBins, 0.0, 8.0);
    CreateH1("Ghost_vs_P_csc", "P_{sim}/q, GeV/c", "Counter", fPRangeBins, 0.0, 8.0);
    CreateH1("Well_vs_P_csc", "P_{sim}/q, GeV/c", "Counter", fPRangeBins, 0.0, 8.0);
    CreateH1("Fake_vs_P_csc", "P_{sim}/q, GeV/c", "Ghosts, %", fPRangeBins, 0.0, 8.0);

    CreateH1("x_residuals_csc_good", "dX, cm", "count", 250, -5.0, 5.0);
    CreateH1("y_residuals_csc_good", "dY, cm", "count", 250, -5.0, 5.0);
    CreateH2("x_resi_vs_mom_csc_good", "P/Q, GeV/c/Q", "dX, cm", "", 500, 0, 8, 500, -5.0, 5.0);
    CreateH2("y_resi_vs_mom_csc_good", "P/Q, GeV/c/Q", "dY, cm", "", 500, 0, 8, 500, -5.0, 5.0);
    CreateH1("x_residuals_csc_bad", "dX, cm", "count", 250, -5.0, 5.0);
    CreateH1("y_residuals_csc_bad", "dY, cm", "count", 250, -5.0, 5.0);

    //DCH1
    CreateH1("Eff_vs_P_dch1", "P_{sim}/q, GeV/c", "Efficiency, %", fPRangeBins, 0.0, 8.0);
    CreateH1("Split_vs_P_dch1", "P_{sim}/q, GeV/c", "Counter", fPRangeBins, 0.0, 8.0);
    CreateH1("SplitEff_vs_P_dch1", "P_{sim}/q, GeV/c", "Splits, %", fPRangeBins, 0.0, 8.0);
    CreateH1("Sim_vs_P_dch1", "P_{sim}/q, GeV/c", "Counter", fPRangeBins, 0.0, 8.0);
    CreateH1("Rec_vs_P_dch1", "P_{sim}/q, GeV/c", "Counter", fPRangeBins, 0.0, 8.0);
    CreateH1("Ghost_vs_P_dch1", "P_{sim}/q, GeV/c", "Counter", fPRangeBins, 0.0, 8.0);
    CreateH1("Well_vs_P_dch1", "P_{sim}/q, GeV/c", "Counter", fPRangeBins, 0.0, 8.0);
    CreateH1("Fake_vs_P_dch1", "P_{sim}/q, GeV/c", "Ghosts, %", fPRangeBins, 0.0, 8.0);

    CreateH1("x_residuals_dch1_good", "dX, cm", "count", 250, -5.0, 5.0);
    CreateH1("y_residuals_dch1_good", "dY, cm", "count", 250, -5.0, 5.0);
    CreateH2("x_resi_vs_mom_dch1_good", "P/Q, GeV/c/Q", "dX, cm", "", 500, 0, 8, 500, -5.0, 5.0);
    CreateH2("y_resi_vs_mom_dch1_good", "P/Q, GeV/c/Q", "dY, cm", "", 500, 0, 8, 500, -5.0, 5.0);
    CreateH1("x_residuals_dch1_bad", "dX, cm", "count", 250, -5.0, 5.0);
    CreateH1("y_residuals_dch1_bad", "dY, cm", "count", 250, -5.0, 5.0);

    //DCH2
    CreateH1("Eff_vs_P_dch2", "P_{sim}/q, GeV/c", "Efficiency, %", fPRangeBins, 0.0, 8.0);
    CreateH1("Split_vs_P_dch2", "P_{sim}/q, GeV/c", "Counter", fPRangeBins, 0.0, 8.0);
    CreateH1("SplitEff_vs_P_dch2", "P_{sim}/q, GeV/c", "Splits, %", fPRangeBins, 0.0, 8.0);
    CreateH1("Sim_vs_P_dch2", "P_{sim}/q, GeV/c", "Counter", fPRangeBins, 0.0, 8.0);
    CreateH1("Rec_vs_P_dch2", "P_{sim}/q, GeV/c", "Counter", fPRangeBins, 0.0, 8.0);
    CreateH1("Ghost_vs_P_dch2", "P_{sim}/q, GeV/c", "Counter", fPRangeBins, 0.0, 8.0);
    CreateH1("Well_vs_P_dch2", "P_{sim}/q, GeV/c", "Counter", fPRangeBins, 0.0, 8.0);
    CreateH1("Fake_vs_P_dch2", "P_{sim}/q, GeV/c", "Ghosts, %", fPRangeBins, 0.0, 8.0);

    CreateH1("x_residuals_dch2_good", "dX, cm", "count", 250, -5.0, 5.0);
    CreateH1("y_residuals_dch2_good", "dY, cm", "count", 250, -5.0, 5.0);
    CreateH2("x_resi_vs_mom_dch2_good", "P/Q, GeV/c/Q", "dX, cm", "", 500, 0, 8, 500, -5.0, 5.0);
    CreateH2("y_resi_vs_mom_dch2_good", "P/Q, GeV/c/Q", "dY, cm", "", 500, 0, 8, 500, -5.0, 5.0);
    CreateH1("x_residuals_dch2_bad", "dX, cm", "count", 250, -5.0, 5.0);
    CreateH1("y_residuals_dch2_bad", "dY, cm", "count", 250, -5.0, 5.0);

    //hits residuals
    for (Int_t iSt = 0; iSt < 9; ++iSt) {
        CreateH1(Form("ResX_%dst", iSt), "ResX, cm", "Counter", 100, -0.5, 0.5);
        CreateH1(Form("ResY_%dst", iSt), "ResY, cm", "Counter", 100, -0.5, 0.5);
    }

    printf("\nHISTOGRAMS CREATED!\n\n");

}

void BmnTrackingQa::ProcessGlobal() {

    vector<Int_t> refs;
    vector<Int_t> splits;

    nWellRecoInEvent = 0;
    nBadRecoInEvent = 0;
    nAllRecoInEvent = 0;

    BmnVertex* vrt = (fVertex == nullptr) ? nullptr : (BmnVertex*)fVertex->At(0);


    if (vrt != nullptr) {
        if (vrt->GetNTracks() > 1) {
            fHM->H1("VertResX")->Fill(vrt->GetX() - 0.0);
            fHM->H1("VertResY")->Fill(vrt->GetY() - 0.0);
            fHM->H1("VertResZ")->Fill(vrt->GetZ() - 0.0);
            fHM->H1("VertX")->Fill(vrt->GetX());
            fHM->H1("VertY")->Fill(vrt->GetY());
            fHM->H1("VertZ")->Fill(vrt->GetZ());
            fHM->H1("VertX_vs_Ntracks")->Fill(fGlobalTracks->GetEntriesFast(), vrt->GetX());
            fHM->H1("VertY_vs_Ntracks")->Fill(fGlobalTracks->GetEntriesFast(), vrt->GetY());
            fHM->H1("VertZ_vs_Ntracks")->Fill(fGlobalTracks->GetEntriesFast(), vrt->GetZ());
        }
    } else if (fVertexL1 != nullptr) {
        if (fVertexL1->GetNTracks() > 1) {
            fHM->H1("VertResX")->Fill(fVertexL1->GetX() - 0.0);
            fHM->H1("VertResY")->Fill(fVertexL1->GetY() - 0.0);
            fHM->H1("VertResZ")->Fill(fVertexL1->GetZ() - 0.0);
            fHM->H1("VertX")->Fill(fVertexL1->GetX());
            fHM->H1("VertY")->Fill(fVertexL1->GetY());
            fHM->H1("VertZ")->Fill(fVertexL1->GetZ());
            fHM->H1("VertX_vs_Ntracks")->Fill(fGlobalTracks->GetEntriesFast(), fVertexL1->GetX());
            fHM->H1("VertY_vs_Ntracks")->Fill(fGlobalTracks->GetEntriesFast(), fVertexL1->GetY());
            fHM->H1("VertZ_vs_Ntracks")->Fill(fGlobalTracks->GetEntriesFast(), fVertexL1->GetZ());
        }
    }

    for (Int_t iTrack = 0; iTrack < fGlobalTracks->GetEntriesFast(); iTrack++) {
        BmnGlobalTrack* glTrack = (BmnGlobalTrack*)(fGlobalTracks->At(iTrack));
        if (!glTrack) continue;
        nAllRecoTracks++;
        nAllRecoInEvent++;

        Float_t Tx = glTrack->GetParamFirst()->GetTx();
        Float_t Ty = glTrack->GetParamFirst()->GetTy();
        Float_t P_rec = Abs(1.0 / glTrack->GetParamFirst()->GetQp());
        Float_t Pz_rec = P_rec / Sqrt(Tx * Tx + Ty * Ty + 1);
        Float_t Px_rec = Pz_rec * Tx;
        Float_t Py_rec = Pz_rec * Ty;
        Float_t Pxy_rec = Sqrt(Px_rec * Px_rec + Py_rec * Py_rec);
        Float_t Eta_rec = 0.5 * Log((P_rec + Pz_rec) / (P_rec - Pz_rec));
        Float_t Theta_rec = ATan2(Pxy_rec, Pz_rec) * RadToDeg();

        Int_t N_rec = 0;
        if (fStsTracks) {
            N_rec = ((CbmStsTrack*)fStsTracks->At(glTrack->GetGemTrackIndex()))->GetNStsHits();
        } else {
            Int_t Nsil = (glTrack->GetSilTrackIndex() != -1) ? ((BmnTrack*)fSilTracks->At(glTrack->GetSilTrackIndex()))->GetNHits() : 0;
            Int_t Ngem = ((BmnTrack*)fGemTracks->At(glTrack->GetGemTrackIndex()))->GetNHits();
            N_rec = Nsil + Ngem;
        }

        Int_t globMCId = glTrack->GetRefIndex();
        if (globMCId == -1) { //fake tracks
            nBadRecoTracks++;
            nBadRecoInEvent++;

            fHM->H1("Ghost_vs_P")->Fill(P_rec);
            fHM->H1("Ghost_vs_P_wide")->Fill(P_rec);
            fHM->H1("Ghost_vs_Nh")->Fill(N_rec);
            fHM->H1("Ghost_vs_Eta")->Fill(Eta_rec);
            fHM->H1("Ghost_vs_Theta")->Fill(Theta_rec);
            fHM->H1("Fakes_vs_EtaP")->Fill(Eta_rec, P_rec);
        } else { //true tracks

            const CbmMCTrack* mcTrack = (CbmMCTrack*)fMCTracks->At(globMCId);
            if (!mcTrack) continue;

            Int_t N_sim = CalcNumberOfMcPointInTrack(globMCId);
            if (N_sim < fMinNofPoints) continue; //FIXME!!! Do we need it or not???

            Float_t P_sim = mcTrack->GetP();
            Float_t Px_sim = mcTrack->GetPx();
            Float_t Py_sim = mcTrack->GetPy();
            Float_t Pz_sim = mcTrack->GetPz();
            Float_t Pxy_sim = Sqrt(Px_sim * Px_sim + Py_sim * Py_sim);
            Float_t Theta_sim = ATan2(Pxy_sim, Pz_sim) * RadToDeg();
            Float_t Eta_sim = 0.5 * Log((P_sim + Pz_sim) / (P_sim - Pz_sim));

            vector<Int_t>::iterator it = find(refs.begin(), refs.end(), globMCId);
            if (it != refs.end()) {
                splits.push_back(globMCId);
            } else {
                refs.push_back(globMCId);
                nGoodRecoTracks++;
                nWellRecoInEvent++;
                fHM->H1("Well_vs_P_wide")->Fill(P_sim);
                fHM->H1("Well_vs_P")->Fill(P_sim);
                fHM->H1("Well_vs_Nh")->Fill(N_sim);
                fHM->H1("Well_vs_Eta")->Fill(Eta_sim);
                fHM->H1("Well_vs_Theta")->Fill(Theta_sim);
                fHM->H2("Nh_rec_Nh_sim")->Fill(N_sim, N_rec);
            }

            fHM->H1("Rec_vs_P")->Fill(P_sim);
            fHM->H1("Rec_vs_P_wide")->Fill(P_sim);
            fHM->H1("Rec_vs_Eta")->Fill(Eta_sim);
            fHM->H1("Rec_vs_Theta")->Fill(Theta_sim);
            fHM->H1("Rec_vs_Nh")->Fill(N_sim);
            fHM->H2("Nh_rec_Eta_rec")->Fill(N_rec, Eta_rec);
            fHM->H2("Nh_rec_P_rec")->Fill(N_rec, P_rec);
            fHM->H2("Nh_rec_Theta_rec")->Fill(N_rec, Theta_rec);

            Float_t chi2 = glTrack->GetChi2() / glTrack->GetNDF();
            fHM->H2("momRes_2D")->Fill(P_sim, (P_sim - P_rec) / P_sim * 100.0);
            fHM->H2("PxRes_2D")->Fill(Px_sim, (Px_sim - Px_rec) / Px_sim * 100.0);
            fHM->H2("PyRes_2D")->Fill(Py_sim, (Py_sim - Py_rec) / Py_sim * 100.0);
            fHM->H2("PzRes_2D")->Fill(Pz_sim, (Pz_sim - Pz_rec) / Pz_sim * 100.0);
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
            fHM->H2("EtaRes_2D")->Fill(Eta_sim, (Eta_sim - Eta_rec) / Eta_sim * 100.0);
            fHM->H2("Px_rec_Px_sim")->Fill(Px_sim, Px_rec);
            fHM->H2("Py_rec_Py_sim")->Fill(Py_sim, Py_rec);
            fHM->H2("Pt_rec_Pt_sim")->Fill(Sqrt(Px_sim * Px_sim + Pz_sim * Pz_sim), Pz_rec * Sqrt(1 + Tx * Tx));
            Float_t Tx_sim = Px_sim / Pz_sim;
            Float_t Ty_sim = Py_sim / Pz_sim;
            fHM->H2("Tx_rec_Tx_sim")->Fill(Tx_sim, Tx);
            fHM->H2("TxRes_2D")->Fill(Tx_sim, (Tx_sim - Tx) / Tx_sim * 100.0);
            fHM->H2("TyRes_2D")->Fill(Ty_sim, (Ty_sim - Ty) / Ty_sim * 100.0);
            fHM->H2("Ty_rec_Ty_sim")->Fill(Py_sim / Pz_sim, Ty);
            fHM->H2("Pz_rec_Pz_sim")->Fill(Pz_sim, Pz_rec);
            fHM->H2("EtaP_rec")->Fill(Eta_rec, P_rec);
            fHM->H2("ThetaP_rec")->Fill(Theta_rec, P_rec);

            FairTrackParam* pf = glTrack->GetParamFirst();
            FairTrackParam* pl = glTrack->GetParamLast();

            Double_t cov[15];

            pf->CovMatrix(cov);
            //first parameters
            fHM->H1("ResX_f")->Fill(mcTrack->GetStartX() - pf->GetX());
            fHM->H1("ResY_f")->Fill(mcTrack->GetStartY() - pf->GetY());
            fHM->H1("ResTx_f")->Fill(Tx_sim - pf->GetTx());
            fHM->H1("ResTy_f")->Fill(Ty_sim - pf->GetTy());
            fHM->H1("ResQp_f")->Fill(1.0 / P_sim - Abs(pf->GetQp()));
            fHM->H1("ErrX_f")->Fill(Sqrt(cov[0]));
            fHM->H1("ErrY_f")->Fill(Sqrt(cov[5]));
            fHM->H1("ErrTx_f")->Fill(Sqrt(cov[9]));
            fHM->H1("ErrTy_f")->Fill(Sqrt(cov[12]));
            fHM->H1("ErrQp_f")->Fill(Sqrt(cov[14]));
            fHM->H1("PullX_f")->Fill((mcTrack->GetStartX() - pf->GetX()) / Sqrt(cov[0]));
            fHM->H1("PullY_f")->Fill((mcTrack->GetStartY() - pf->GetY()) / Sqrt(cov[5]));
            fHM->H1("PullTx_f")->Fill((Tx_sim - pf->GetTx()) / Sqrt(cov[9]));
            fHM->H1("PullTy_f")->Fill((Ty_sim - pf->GetTy()) / Sqrt(cov[12]));
            fHM->H1("PullQp_f")->Fill((1.0 / P_sim - Abs(pf->GetQp())) / Sqrt(cov[14]));

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

            //TOF400 
            if (glTrack->GetTof1HitIndex() != -1) {
                fHM->H1("Rec_vs_P_tof400")->Fill(P_sim);
                fHM->H2("banana_tof400")->Fill(glTrack->GetP(), glTrack->GetBeta(1));
                BmnHit* tofHit = (BmnHit*)fTof400Hits->At(glTrack->GetTof1HitIndex());
                Bool_t found = kFALSE;
                for (Int_t iLnk = 0; iLnk < tofHit->GetLinksWithType(0x2).GetNLinks(); ++iLnk) {
                    if (tofHit->GetLinksWithType(0x2).GetLink(iLnk).GetIndex() == globMCId) {
                        found = kTRUE;
                        break;
                    }
                }
                if (!found) {
                    fHM->H1("Ghost_vs_P_tof400")->Fill(P_sim);
                    fHM->H1("x_residuals_tof400_bad")->Fill(tofHit->GetResX());
                    fHM->H1("y_residuals_tof400_bad")->Fill(tofHit->GetResY());
                } else {
                    fHM->H1("Well_vs_P_tof400")->Fill(P_sim);
                    fHM->H2("banana_tof400_good")->Fill(glTrack->GetP(), glTrack->GetBeta(1));
                    fHM->H1("x_residuals_tof400_good")->Fill(tofHit->GetResX());
                    fHM->H1("y_residuals_tof400_good")->Fill(tofHit->GetResY());
                    fHM->H2("x_resi_vs_mom_tof400_good")->Fill(P_sim, tofHit->GetResX());
                    fHM->H2("y_resi_vs_mom_tof400_good")->Fill(P_sim, tofHit->GetResY());
                }
            }
            //TOF700
            if (glTrack->GetTof2HitIndex() != -1) {
                fHM->H1("Rec_vs_P_tof700")->Fill(P_sim);
                fHM->H2("banana_tof700")->Fill(glTrack->GetP(), glTrack->GetBeta(2));
                BmnHit* tofHit = (BmnHit*)fTof700Hits->At(glTrack->GetTof2HitIndex());
                Bool_t found = kFALSE;
                for (Int_t iLnk = 0; iLnk < tofHit->GetLinksWithType(0x2).GetNLinks(); ++iLnk) {
                    if (tofHit->GetLinksWithType(0x2).GetLink(iLnk).GetIndex() == globMCId) {
                        found = kTRUE;
                        break;
                    }
                }
                if (!found) {
                    fHM->H1("Ghost_vs_P_tof700")->Fill(P_sim);
                    fHM->H1("x_residuals_tof700_bad")->Fill(tofHit->GetResX());
                    fHM->H1("y_residuals_tof700_bad")->Fill(tofHit->GetResY());
                } else {
                    fHM->H1("Well_vs_P_tof700")->Fill(P_sim);
                    fHM->H2("banana_tof700_good")->Fill(glTrack->GetP(), glTrack->GetBeta(2));
                    fHM->H1("x_residuals_tof700_good")->Fill(tofHit->GetResX());
                    fHM->H1("y_residuals_tof700_good")->Fill(tofHit->GetResY());
                    fHM->H2("x_resi_vs_mom_tof700_good")->Fill(P_sim, tofHit->GetResX());
                    fHM->H2("y_resi_vs_mom_tof700_good")->Fill(P_sim, tofHit->GetResY());
                }
            }
            if (glTrack->GetTof1HitIndex() != -1 && glTrack->GetTof2HitIndex() != -1) {
                Double_t beta400sq = glTrack->GetBeta(1) * glTrack->GetBeta(1);
                Double_t beta700sq = glTrack->GetBeta(2) * glTrack->GetBeta(2);
                Double_t mom2 = glTrack->GetP() * glTrack->GetP();
                Double_t mass400 = Sqrt(mom2 / beta400sq - mom2);
                Double_t mass700 = Sqrt(mom2 / beta700sq - mom2);
                fHM->H2("Mass_correlation")->Fill(mass400, mass700);
            }
            //CSC
            if (glTrack->GetCscHitIndex(0) != -1) {
                fHM->H1("Rec_vs_P_csc")->Fill(P_sim);
                BmnHit* cscHit = (BmnHit*)fCscHits->At(glTrack->GetCscHitIndex(0));
                if (cscHit->GetRefIndex() != -1) { //index of parent mc point
                    FairMCPoint* pnt = (FairMCPoint*)fCscPoints->At(cscHit->GetRefIndex());
                    if (pnt) {
                        if (globMCId != pnt->GetTrackID()) {
                            fHM->H1("Ghost_vs_P_csc")->Fill(P_sim);
                            fHM->H1("x_residuals_csc_bad")->Fill(cscHit->GetResX());
                            fHM->H1("y_residuals_csc_bad")->Fill(cscHit->GetResY());
                        } else {
                            fHM->H1("Well_vs_P_csc")->Fill(P_sim);
                            fHM->H1("x_residuals_csc_good")->Fill(cscHit->GetResX());
                            fHM->H1("y_residuals_csc_good")->Fill(cscHit->GetResY());
                            fHM->H2("x_resi_vs_mom_csc_good")->Fill(P_sim, cscHit->GetResX());
                            fHM->H2("y_resi_vs_mom_csc_good")->Fill(P_sim, cscHit->GetResY());
                        }
                    }
                }
            }
            //DCH1
            if (glTrack->GetDch1TrackIndex() != -1) {
                fHM->H1("Rec_vs_P_dch1")->Fill(P_sim);
                BmnDchTrack* dchTr = (BmnDchTrack*)fDchTracks->At(glTrack->GetDch1TrackIndex());
                if (dchTr)
                    if (globMCId != dchTr->GetTrackId()) {
                        fHM->H1("Ghost_vs_P_dch1")->Fill(P_sim);
                        fHM->H1("x_residuals_dch1_bad")->Fill(0.0);
                        fHM->H1("y_residuals_dch1_bad")->Fill(0.0);
                    } else {
                        fHM->H1("Well_vs_P_dch1")->Fill(P_sim);
                        fHM->H1("x_residuals_dch1_good")->Fill(0.0);
                        fHM->H1("y_residuals_dch1_good")->Fill(0.0);
                        fHM->H2("x_resi_vs_mom_dch1_good")->Fill(P_sim, 0.0);
                        fHM->H2("y_resi_vs_mom_dch1_good")->Fill(P_sim, 0.0);
                    }
            }
            //DCH2
            if (glTrack->GetDch2TrackIndex() != -1) {
                fHM->H1("Rec_vs_P_dch2")->Fill(P_sim);
                BmnDchTrack* dchTr = (BmnDchTrack*)fDchTracks->At(glTrack->GetDch2TrackIndex());
                if (dchTr)
                    if (globMCId != dchTr->GetTrackId()) {
                        fHM->H1("Ghost_vs_P_dch2")->Fill(P_sim);
                        fHM->H1("x_residuals_dch2_bad")->Fill(0.0);
                        fHM->H1("y_residuals_dch2_bad")->Fill(0.0);
                    } else {
                        fHM->H1("Well_vs_P_dch2")->Fill(P_sim);
                        fHM->H1("x_residuals_dch2_good")->Fill(0.0);
                        fHM->H1("y_residuals_dch2_good")->Fill(0.0);
                        fHM->H2("x_resi_vs_mom_dch2_good")->Fill(P_sim, 0.0);
                        fHM->H2("y_resi_vs_mom_dch2_good")->Fill(P_sim, 0.0);
                    }
            }
        }
    }

    Int_t nSplitInOneEvent = 0;

    for (Int_t i = 0; i < splits.size(); ++i) {
        const CbmMCTrack* mcTrack = (CbmMCTrack*)fMCTracks->At(splits[i]);
        if (!mcTrack) continue;

        Int_t nPointsPerTrack = CalcNumberOfMcPointInTrack(splits[i]);
        if (nPointsPerTrack < fMinNofPoints) continue;

        nSplitInOneEvent++;
        nSplitRecoTracks++;

        Float_t Px = mcTrack->GetPx();
        Float_t Py = mcTrack->GetPy();
        Float_t Pz = mcTrack->GetPz();
        Float_t P = mcTrack->GetP();
        Float_t Pxy = Sqrt(Px * Px + Py * Py);
        Float_t eta = 0.5 * Log((P + Pz) / (P - Pz));
        Float_t theta = ATan2(Pxy, Pz) * RadToDeg();

        fHM->H1("Split_vs_P")->Fill(P);
        fHM->H1("Split_vs_P_wide")->Fill(P);
        fHM->H1("Split_vs_Eta")->Fill(eta);
        fHM->H1("Split_vs_Theta")->Fill(theta);
        fHM->H1("Split_vs_Nh")->Fill(nPointsPerTrack);
        fHM->H2("Clones_vs_EtaP")->Fill(eta, P);
    }

    Int_t nReconstructable = 0;

    for (Int_t iTrack = 0; iTrack < fMCTracks->GetEntriesFast(); iTrack++) {
        const CbmMCTrack* mcTrack = (CbmMCTrack*)fMCTracks->At(iTrack);
        if (!mcTrack) continue;

        Int_t nHitsPerTrack = CalcNumberOfMcPointInTrack(iTrack);
        if (nHitsPerTrack < fMinNofPoints) continue;

        nAllMcTracks++;
        nReconstructable++;

        Float_t Px = mcTrack->GetPx();
        Float_t Py = mcTrack->GetPy();
        Float_t Pz = mcTrack->GetPz();
        Float_t P = mcTrack->GetP();
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

        if (mcTrack->GetNPoints(kTOF1) > 0) {
            fHM->H1("Sim_vs_P_tof400")->Fill(P);
        }

        if (mcTrack->GetNPoints(kTOF) > 0) {
            fHM->H1("Sim_vs_P_tof700")->Fill(P);
        }

        if (mcTrack->GetNPoints(kCSC) > 0) {
            fHM->H1("Sim_vs_P_csc")->Fill(P);
        }

        if (mcTrack->GetNPoints(kDCH) == 16) {
            fHM->H1("Sim_vs_P_dch1")->Fill(P);
            fHM->H1("Sim_vs_P_dch2")->Fill(P);
        }
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

Int_t BmnTrackingQa::CalcNumberOfMcPointInTrack(Int_t trId) {
    Int_t nHitsOnStation[fNStations];
    for (Int_t i = 0; i < fNStations; ++i)
        nHitsOnStation[i] = 0;

    for (Int_t i = 0; i < fSilPoints->GetEntriesFast(); ++i) {
        BmnSiliconPoint* pnt = (BmnSiliconPoint*)fSilPoints->At(i);
        if (pnt->GetTrackID() != trId) continue;
        nHitsOnStation[pnt->GetStation()]++;
    }

    for (Int_t i = 0; i < fGemPoints->GetEntriesFast(); ++i) {
        CbmStsPoint* pnt = (CbmStsPoint*)fGemPoints->At(i);
        if (pnt->GetTrackID() != trId) continue;
        nHitsOnStation[pnt->GetStation() + fSilDetector->GetNStations()]++;
    }

    Int_t nHitsPerTrack = 0;
    for (Int_t iSt = 0; iSt < fNStations; iSt++) {
        if (nHitsOnStation[iSt] == 0) continue;
        if (nHitsOnStation[iSt] > 1) return -1;
        nHitsPerTrack++;
    }
    return nHitsPerTrack;
}

ClassImp(BmnTrackingQa);
