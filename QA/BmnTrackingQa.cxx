
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
#include "CbmTofHit.h"
#include "BmnDchHit.h"
#include "FairRunAna.h"
#include "FairMCEventHeader.h"
#include "TFitResult.h"

#include "TH1.h"
#include "TF1.h"
#include "TH2F.h"
#include "TClonesArray.h"
#include "BmnEnums.h"

#include <boost/assign/list_of.hpp>

#include <fstream>
#include <iostream>

using namespace std;
using namespace TMath;
using boost::assign::list_of;
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
    if (fTrackCategories.empty()) FillDefaultTrackCategories();

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
    ProcessGlobal();
    //    ProcessGlobalTracks();
    ProcessMcTracks();
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

    fGlobalTracks = (TClonesArray*) ioman->GetObject("GlobalTrack");
    fGlobalTrackMatches = (TClonesArray*) ioman->GetObject("GlobalTrackMatch");

    if (NULL == fGlobalTracks) {
        Fatal("Init", "No GlobalTrack array!");
    }

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
    if (fDet.GetDet(kDCH1)) {
        fDch1Points = (TClonesArray*) ioman->GetObject("DCH1Point");
        if (NULL == fDch1Points) {
            cout << "BmnTrackingQA::Init: No DCH1Point array!" << endl;
        }
        fDch1Hits = (TClonesArray*) ioman->GetObject("BmnDch1Hit0");
        if (NULL == fDch1Hits) {
            cout << "BmnTrackingQA::Init: No BmnDch1Hit0 array!" << endl;
        }
    }
    if (fDet.GetDet(kDCH2)) {
        fDch2Points = (TClonesArray*) ioman->GetObject("DCH2Point");
        if (NULL == fDch2Points) {
            cout << "BmnTrackingQA::Init: No DCH2Point array!" << endl;
        }
        fDch2Hits = (TClonesArray*) ioman->GetObject("BmnDch2Hit0");
        if (NULL == fDch2Hits) {
            cout << "BmnTrackingQA::Init: No BmnDch2Hit0 array!" << endl;
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

void BmnTrackingQa::FillDefaultTrackCategories() {
    //   vector<string> tmp = list_of("All")("Primary")("Secondary")("Reference")
    //         ("Electron")("Proton")("PionPlus")
    //         ("PionMinus")("KaonPlus")("KaonMinus");
    vector<string> tmp = list_of("All"); //("Primary")("Secondary")("Reference");
    fTrackCategories = tmp;
}

void BmnTrackingQa::CreateH1Efficiency(const string& name, const string& parameter, const string& xTitle, Int_t nofBins, Double_t minBin, Double_t maxBin, const string& opt) {
    assert(opt == "track" || opt == "track_pid");
    vector<string> types = list_of("Acc")("Rec")("Eff");
    vector<string> cat = fTrackCategories;

    for (Int_t iCat = 0; iCat < cat.size(); iCat++) {
        for (Int_t iType = 0; iType < 3; iType++) {
            string yTitle = (types[iType] == "Eff") ? "Efficiency [%]" : "Counter";
            string histName = name + "_" + cat[iCat] + "_" + types[iType] + "_" + parameter;
            string histTitle = histName + ";" + xTitle + ";" + yTitle;
            fHM->Add(histName, new TH1F(histName.c_str(), histTitle.c_str(), nofBins, minBin, maxBin));
        }
    }
}

void BmnTrackingQa::CreateH2Efficiency(
        const string& name,
        const string& parameter,
        const string& xTitle,
        const string& yTitle,
        Int_t nofBinsX,
        Double_t minBinX,
        Double_t maxBinX,
        Int_t nofBinsY,
        Double_t minBinY,
        Double_t maxBinY,
        const string& opt) {
    assert(opt == "track" || opt == "track_pid");
    vector<string> types = list_of("Acc")("Rec")("Eff");
    vector<string> cat = fTrackCategories;

    for (Int_t iCat = 0; iCat < cat.size(); iCat++) {
        for (Int_t iType = 0; iType < 3; iType++) {
            string zTitle = (types[iType] == "Eff") ? "Efficiency [%]" : "Counter";
            string histName = name + "_" + cat[iCat] + "_" + types[iType] + "_" + parameter;
            string histTitle = histName + ";" + xTitle + ";" + yTitle + ";" + zTitle;
            fHM->Add(histName, new TH2F(histName.c_str(), histTitle.c_str(), nofBinsX, minBinX, maxBinX, nofBinsY, minBinY, maxBinY));
        }
    }
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
    //    if (fDet.GetDet(kDCH1)) detectors.push_back("Dch1");
    //    if (fDet.GetDet(kDCH2)) detectors.push_back("Dch2");
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
    if (fDet.GetDet(kDCH1)) detectors.push_back("Dch1");
    if (fDet.GetDet(kDCH2)) detectors.push_back("Dch2");
    if (fDet.GetDet(kTOF)) detectors.push_back("Tof2");
    string name("");
    for (Int_t i = 0; i < detectors.size(); ++i) {
        name += detectors[i];
        trackVariants.insert(name);
    }
    vector<string> trackVariantsVector(trackVariants.begin(), trackVariants.end());
    return trackVariantsVector;
}

string BmnTrackingQa::LocalEfficiencyNormalization(const string& detName) {
    set<string> trackVariants;
    vector<string> detectors;
    if (fDet.GetDet(kGEM)) detectors.push_back("Gem");
    string name("");
    for (Int_t i = 0; i < detectors.size(); i++) {
        name += detectors[i];
        if (detectors[i] == detName) break;
    }
    return name;
}

void BmnTrackingQa::CreateHistograms() {
    fDet.DetermineSetup();

    // Number of points distributions
    const Float_t minNofPoints = 0.;
    const Float_t maxNofPoints = 15.;
    const Int_t nofBinsPoints = 15;

    // Reconstruction efficiency histograms
    // Local efficiency histograms

    // GEM
    CreateH1Efficiency("hte_Gem_Gem", "Np", "Number of points", nofBinsPoints, minNofPoints, maxNofPoints, "track");
    //CreateH1Efficiency("hte_Gem_Gem", "Angle", "Polar angle [deg]", fAngleRangeBins, fAngleRangeMin, fAngleRangeMax, "track");

    //    if (fDet.GetDet(kTOF1)) {
    //        string norm = LocalEfficiencyNormalization("Tof1");
    //        string histName = "hte_Tof1_" + norm;
    //        CreateH1Efficiency(histName, "p", "P [GeV/c]", fPRangeBins, fPRangeMin, fPRangeMax, "track");
    //        CreateH1Efficiency(histName, "y", "Rapidity", fYRangeBins, fYRangeMin, fYRangeMax, "track");
    //        CreateH1Efficiency(histName, "pt", "P_{t} [GeV/c]", fPtRangeBins, fPtRangeMin, fPtRangeMax, "track");
    //        //    CreateEfficiencyHistogram(histName, "Np", "Number of points", nofBinsPoints, minNofPoints, maxNofPoints, "track");
    //        CreateH1Efficiency(histName, "Angle", "Polar angle [deg]", fAngleRangeBins, fAngleRangeMin, fAngleRangeMax, "track");
    //        CreateH2Efficiency(histName, "YPt", "Rapidity_{sim}", "Pt_{sim} [GeV/c]", 4 * fYRangeBins, fYRangeMin, fYRangeMax, 4 * fPtRangeBins, fPtRangeMin, fPtRangeMax, "track");
    //    }
    //    if (fDet.GetDet(kTOF)) {
    //        string norm = LocalEfficiencyNormalization("Tof2");
    //        string histName = "hte_Tof2_" + norm;
    //        CreateH1Efficiency(histName, "p", "P [GeV/c]", fPRangeBins, fPRangeMin, fPRangeMax, "track");
    //        CreateH1Efficiency(histName, "y", "Rapidity", fYRangeBins, fYRangeMin, fYRangeMax, "track");
    //        CreateH1Efficiency(histName, "pt", "P_{t} [GeV/c]", fPtRangeBins, fPtRangeMin, fPtRangeMax, "track");
    //        //    CreateEfficiencyHistogram(histName, "Np", "Number of points", nofBinsPoints, minNofPoints, maxNofPoints, "track");
    //        CreateH1Efficiency(histName, "Angle", "Polar angle [deg]", fAngleRangeBins, fAngleRangeMin, fAngleRangeMax, "track");
    //        CreateH2Efficiency(histName, "YPt", "Rapidity", "P_{t} [GeV/c]", fYRangeBins, fYRangeMin, fYRangeMax, fPtRangeBins, fPtRangeMin, fPtRangeMax, "track");
    //    }
    //    if (fDet.GetDet(kDCH1)) {
    //        string norm = LocalEfficiencyNormalization("Dch1");
    //        string histName = "hte_Dch1_" + norm;
    //        CreateH1Efficiency(histName, "p", "P [GeV/c]", fPRangeBins, fPRangeMin, fPRangeMax, "track");
    //        CreateH1Efficiency(histName, "y", "Rapidity", fYRangeBins, fYRangeMin, fYRangeMax, "track");
    //        CreateH1Efficiency(histName, "pt", "P_{t} [GeV/c]", fPtRangeBins, fPtRangeMin, fPtRangeMax, "track");
    //        //    CreateEfficiencyHistogram(histName, "Np", "Number of points", nofBinsPoints, minNofPoints, maxNofPoints, "track");
    //        CreateH1Efficiency(histName, "Angle", "Polar angle [deg]", fAngleRangeBins, fAngleRangeMin, fAngleRangeMax, "track");
    //        CreateH2Efficiency(histName, "YPt", "Rapidity", "P_{t} [GeV/c]", fYRangeBins, fYRangeMin, fYRangeMax, fPtRangeBins, fPtRangeMin, fPtRangeMax, "track");
    //    }
    //    if (fDet.GetDet(kDCH2)) {
    //        string norm = LocalEfficiencyNormalization("Dch2");
    //        string histName = "hte_Dch2_" + norm;
    //        CreateH1Efficiency(histName, "p", "P [GeV/c]", fPRangeBins, fPRangeMin, fPRangeMax, "track");
    //        CreateH1Efficiency(histName, "y", "Rapidity", fYRangeBins, fYRangeMin, fYRangeMax, "track");
    //        CreateH1Efficiency(histName, "pt", "P_{t} [GeV/c]", fPtRangeBins, fPtRangeMin, fPtRangeMax, "track");
    //        //    CreateEfficiencyHistogram(histName, "Np", "Number of points", nofBinsPoints, minNofPoints, maxNofPoints, "track");
    //        CreateH1Efficiency(histName, "Angle", "Polar angle [deg]", fAngleRangeBins, fAngleRangeMin, fAngleRangeMax, "track");
    //        CreateH2Efficiency(histName, "YPt", "Rapidity", "P_{t} [GeV/c]", fYRangeBins, fYRangeMin, fYRangeMax, fPtRangeBins, fPtRangeMin, fPtRangeMax, "track");
    //    }

    // Global efficiency histograms
    vector<string> histoNames = CreateGlobalTrackingHistogramNames();
    for (Int_t iHist = 0; iHist < histoNames.size(); iHist++) {
        string name = histoNames[iHist];
        string opt = "track";
        // Tracking efficiency
        CreateH1Efficiency(name, "p", "P [GeV/c]", fPRangeBins, fPRangeMin, fPRangeMax, opt);
        CreateH1Efficiency(name, "y", "Rapidity", fYRangeBins, fYRangeMin, fYRangeMax, opt);
        CreateH1Efficiency(name, "pt", "P_{t} [GeV/c]", fPtRangeBins, fPtRangeMin, fPtRangeMax, opt);
        CreateH2Efficiency(name, "YPt", "Rapidity_{sim}", "Pt_{sim} [GeV/c]", fYRangeBins, fYRangeMin, fYRangeMax, fPtRangeBins, fPtRangeMin, fPtRangeMax, opt);
    }

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
    if (fDet.GetDet(kDCH1)) CreateH1("hno_NofObjects_Dch1Hits", "DCH1 hits per event", "Yield", nofBinsC, 1., maxXC);
    if (fDet.GetDet(kDCH2)) CreateH1("hno_NofObjects_Dch2Hits", "DCH2 hits per event", "Yield", nofBinsC, 1., maxXC);
    if (fDet.GetDet(kTOF)) CreateH1("hno_NofObjects_Tof2Hits", "TOF2 hits per event", "Yield", nofBinsC, 1., maxXC);

    // Histogram stores number of events
    CreateH1("hen_EventNo_TrackingQa", "", "", 1, 0, 1.);
    CreateH1("Impact parameter", "b, fm", "Counter", 50, 0.0, 0.0);
    CreateH1("Multiplicity", "N_{prim}", "Counter", 50, 0.0, 0.0);
    CreateH2("Impact_Mult", "b, fm", "N_{prim}", "", 400, 0.0, 0.0, 400, 0.0, 0.0);

    // Physics
    CreateH2("momRes_2D_glob", "P_{sim}, GeV/c", "#Delta P / P, %", "", 4 * fPRangeBins, fPRangeMin, fPRangeMax, 4 * fPRangeBins, -50.0, 50.0);
    CreateH2("momRes_2D_gem", "P_{sim}, GeV/c", "#Delta P / P, %", "", 400, fPRangeMin, fPRangeMax, 400, 0.0, 100.0);
    CreateH2("EtaP_rec_gem", "#eta_{rec}", "P_{rec}, GeV/c", "", 400, fEtaRangeMin, fEtaRangeMax, 400, fPRangeMin, fPRangeMax);
    CreateH2("EtaP_rec_glob", "#eta_{rec}", "P_{rec}, GeV/c", "", 400, fEtaRangeMin, fEtaRangeMax, 400, fPRangeMin, fPRangeMax);
    CreateH2("EtaP_sim", "#eta_{sim}", "P_{sim}, GeV/c", "", 400, fEtaRangeMin, fEtaRangeMax, 400, fPRangeMin, fPRangeMax);
    CreateH1("momRes_1D_glob", "P_{sim}, GeV/c", "#LT#Delta P / P#GT, %", fPRangeBins, fPRangeMin, fPRangeMax);
    CreateH1("momRes_1D_gem", "P_{sim}, GeV/c", "#LT#Delta P / P#GT, %", fPRangeBins, fPRangeMin, fPRangeMax);
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
    
    CreateH2("MomRes_vs_Chi2_gem", "#chi^{2}", "#Delta P / P, %", "", 400, 0, 100, 400, 0, 100);

    //    cout << fHM->ToString();
}

void BmnTrackingQa::ProcessGem() {
    vector<Int_t> refs;
    vector<Int_t> splits;

    for (Int_t iTrack = 0; iTrack < fGemTracks->GetEntriesFast(); iTrack++) {
        BmnGemTrack* track = (BmnGemTrack*) (fGemTracks->At(iTrack));
        if (track->GetFlag() == kBMNBAD) continue;
        BmnTrackMatch* gemTrackMatch = (BmnTrackMatch*) (fGemMatches->At(iTrack));
        if (!track || !gemTrackMatch) continue;
        if (gemTrackMatch->GetNofLinks() == 0) continue;
        Int_t gemMCId = gemTrackMatch->GetMatchedLink().GetIndex();
        vector<Int_t>::iterator it = find(refs.begin(), refs.end(), gemMCId);

        if (it != refs.end() && gemMCId != -1) {
            splits.push_back(gemMCId);
//            cout << "gemMCId =  " << gemMCId << " Nhits = " << track->GetNHits() << " ";
//            track->GetParamFirst()->Print();
//            for (Int_t iPnt = 0; iPnt < fGemPoints->GetEntriesFast(); ++iPnt) {
//                FairMCPoint* pnt = (FairMCPoint*) fGemPoints->GAt(iPnt);
//                if (pnt->GetTrackID() == gemMCId) {
//                    
//                }
//            }
        } else
            refs.push_back(gemMCId);

        CbmMCTrack* mcTrack = (CbmMCTrack*) (fMCTracks->At(gemMCId));
        if (!mcTrack) continue;

        Bool_t isTrackOk = gemTrackMatch->GetTrueOverAllHitsRatio() >= fQuota && track->GetNHits() >= fMinNofPointsGem;

        Float_t Px_sim = mcTrack->GetPx();
        Float_t Py_sim = mcTrack->GetPy();
        Float_t Pz_sim = mcTrack->GetPz();
        Float_t P_sim = mcTrack->GetP();
        Float_t Pt_sim = Sqrt(Px_sim * Px_sim + Pz_sim * Pz_sim);
        Float_t Pxy_sim = Sqrt(Px_sim * Px_sim + Py_sim * Py_sim);
        Float_t Eta_sim = 0.5 * Log((P_sim + Pz_sim) / (P_sim - Pz_sim));
        Float_t Theta_sim = ATan2(Pxy_sim, Pz_sim) * RadToDeg();
        Int_t N_sim = mcTrack->GetNPoints(kGEM);

        Float_t P_rec = Abs(1.0 / track->GetParamFirst()->GetQp());
        Float_t Tx = track->GetParamFirst()->GetTx();
        Float_t Ty = track->GetParamFirst()->GetTy();
        Float_t Pz_rec = P_rec / Sqrt(Tx * Tx + Ty * Ty + 1);
        Float_t Px_rec = Pz_rec * Tx;
        Float_t Py_rec = Pz_rec * Ty;
        Float_t Eta_rec = 0.5 * Log((P_rec + Pz_rec) / (P_rec - Pz_rec));

        fHM->H1("Rec_vs_P_gem")->Fill(P_sim);
        fHM->H1("Rec_vs_Eta_gem")->Fill(Eta_sim);
        fHM->H1("Rec_vs_Theta_gem")->Fill(Theta_sim);
        fHM->H2("Nh_rec_Eta_rec_gem")->Fill(N_sim, Eta_sim);
        fHM->H2("Nh_rec_P_rec_gem")->Fill(N_sim, P_sim);

        if (!isTrackOk) {
            fHM->H1("Ghost_vs_P_gem")->Fill(P_sim);
            fHM->H1("Ghost_vs_Eta_gem")->Fill(Eta_sim);
            fHM->H1("Ghost_vs_Theta_gem")->Fill(Theta_sim);
            fHM->H1("Ghost_vs_Nh_gem")->Fill(track->GetNHits());
        } else {
            fHM->H1("Well_vs_P_gem")->Fill(P_sim);
            fHM->H1("Well_vs_Eta_gem")->Fill(Eta_sim);
            fHM->H1("Well_vs_Theta_gem")->Fill(Theta_sim);
            fHM->H1("Well_vs_Nh_gem")->Fill(track->GetNHits());

            fHM->H2("momRes_2D_gem")->Fill(P_sim, Abs(P_sim - P_rec) / P_sim * 100.0);
            fHM->H2("MomRes_vs_Chi2_gem")->Fill(track->GetChi2(), Abs(P_sim - P_rec) / P_sim * 100.0);
            fHM->H2("P_rec_P_sim_gem")->Fill(P_sim, P_rec);
            fHM->H2("Eta_rec_Eta_sim_gem")->Fill(Eta_sim, Eta_rec);
            fHM->H2("Px_rec_Px_sim_gem")->Fill(Px_sim, Px_rec);
            fHM->H2("Py_rec_Py_sim_gem")->Fill(Py_sim, Py_rec);
            fHM->H2("Pt_rec_Pt_sim_gem")->Fill(Pt_sim, Pz_rec * Sqrt(1 + Tx * Tx));
            fHM->H2("Tx_rec_Tx_sim_gem")->Fill(Px_sim / Pz_sim, Tx);
            fHM->H2("Ty_rec_Ty_sim_gem")->Fill(Py_sim / Pz_sim, Ty);
            fHM->H2("Pz_rec_Pz_sim_gem")->Fill(Pz_sim, Pz_rec);
            fHM->H2("EtaP_rec_gem")->Fill(Eta_rec, P_rec);
        }
    }

    Int_t momResStep = 40;
    for (Int_t iBin = 0; iBin < fHM->H2("momRes_2D_gem")->GetNbinsX(); iBin += momResStep) {
        TH1D* proj = fHM->H2("momRes_2D_gem")->ProjectionY("tmp", iBin, iBin + (momResStep - 1));
        proj->Fit("gaus", "SQRww", "", 0.0, 100.0);
        TF1 *fit = proj->GetFunction("gaus");
        Float_t mean = (fit->GetParameter(1) < 20.0) ? fit->GetParameter(1) : 0.0;
        Float_t sigma = fit->GetParameter(2);
        Float_t mom = fHM->H2("momRes_2D_gem")->GetXaxis()->GetBinCenter(iBin);
        Int_t nBins = fHM->H1("momRes_1D_gem")->GetXaxis()->GetNbins();
        Int_t bin = (mom - fPRangeMin) / (fPRangeMax - fPRangeMin) * nBins;
        fHM->H1("momRes_1D_gem")->SetBinContent(bin, mean);
//        fHM->H1("momRes_1D_gem")->SetBinError(bin, sigma);
        fHM->H1("momRes_1D_gem")->SetBinError(bin, 0.0);
    }

    for (Int_t i = 0; i < splits.size(); ++i) {
        const CbmMCTrack* mcTrack = (const CbmMCTrack*) (fMCTracks->At(splits.at(i)));
        if (mcTrack->GetNPoints(kGEM) < fMinNofPointsGem) continue;
        if (fPrimes && mcTrack->GetMotherId() != -1) continue;

        Float_t Px = mcTrack->GetPx();
        Float_t Py = mcTrack->GetPy();
        Float_t Pz = mcTrack->GetPz();
        Float_t P = mcTrack->GetP();
        Float_t Pxy = Sqrt(Px * Px + Py * Py);
        Float_t eta = 0.5 * Log((P + Pz) / (P - Pz));
        Float_t theta = ATan2(Pxy, Pz) * RadToDeg();

        fHM->H1("Split_vs_P_gem")->Fill(P);
        fHM->H1("Split_vs_Eta_gem")->Fill(eta);
        fHM->H1("Split_vs_Theta_gem")->Fill(theta);
        fHM->H1("Well_vs_P_gem")->Fill(P, -1); //remove splitted tracks from efficiency
        fHM->H1("Well_vs_Eta_gem")->Fill(eta, -1); //remove splitted tracks from efficiency
        fHM->H1("Well_vs_Theta_gem")->Fill(theta, -1); //remove splitted tracks from efficiency
    }

    for (Int_t iTrack = 0; iTrack < fMCTracks->GetEntriesFast(); iTrack++) {
        const CbmMCTrack* mcTrack = (const CbmMCTrack*) (fMCTracks->At(iTrack));
        if (mcTrack->GetNPoints(kGEM) < fMinNofPointsGem) continue;
        if (fPrimes && mcTrack->GetMotherId() != -1) continue;

        Float_t Px = mcTrack->GetPx();
        Float_t Py = mcTrack->GetPy();
        Float_t Pz = mcTrack->GetPz();
        Float_t P = mcTrack->GetP();
        Float_t Pxy = Sqrt(Px * Px + Py * Py);
        Float_t eta = 0.5 * Log((P + Pz) / (P - Pz));
        Float_t theta = ATan2(Pxy, Pz) * RadToDeg();

        fHM->H2("EtaP_sim")->Fill(eta, P);
        fHM->H1("Sim_vs_P_gem")->Fill(P);
        fHM->H1("Sim_vs_Eta_gem")->Fill(eta);
        fHM->H1("Sim_vs_Theta_gem")->Fill(theta);
        fHM->H2("Nh_sim_Eta_sim_gem")->Fill(mcTrack->GetNPoints(kGEM), eta);
        fHM->H2("Nh_sim_P_sim_gem")->Fill(mcTrack->GetNPoints(kGEM), P);
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
            fHM->H1("Ghost_vs_Nh_glob")->Fill(track->GetNofHits());
        } else {
            fHM->H1("Well_vs_P_glob")->Fill(P_sim);
            fHM->H1("Well_vs_Nh_glob")->Fill(track->GetNofHits());

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

//void BmnTrackingQa::ProcessGlobalTracks() {
//    // Clear all maps for MC to reco matching
//    map<string, multimap<Int_t, Int_t> >::iterator it;
//    for (it = fMcToRecoMap.begin(); it != fMcToRecoMap.end(); it++) {
//        multimap<Int_t, Int_t>& mcRecoMap = (*it).second;
//        mcRecoMap.clear();
//        //(*it).second.clear();
//    }
//
//    for (Int_t iTrack = 0; iTrack < fGlobalTracks->GetEntriesFast(); iTrack++) {
//        const BmnGlobalTrack* globalTrack = (const BmnGlobalTrack*) (fGlobalTracks->At(iTrack));
//        if (globalTrack->GetFlag() == kBMNBAD) continue;
//
//        // get track segments indices
//        Int_t gemId = globalTrack->GetGemTrackIndex();
//        Int_t tof1Id = globalTrack->GetTof1HitIndex();
//        Int_t tof2Id = globalTrack->GetTof2HitIndex();
//        Int_t dch1Id = globalTrack->GetDch1HitIndex();
//        Int_t dch2Id = globalTrack->GetDch2HitIndex();
//
//        // check track segments
//        Bool_t isGemOk = gemId > -1 && fDet.GetDet(kGEM);
//        Bool_t isSeedOk = gemId > -1 && fDet.GetDet(kGEM);
//        Bool_t isTof1Ok = tof1Id > -1 && fDet.GetDet(kTOF1) && fTof1Hits;
//        Bool_t isTof2Ok = tof2Id > -1 && fDet.GetDet(kTOF) && fTof2Hits;
//        Bool_t isDch1Ok = dch1Id > -1 && fDet.GetDet(kDCH1) && fDch1Hits;
//        Bool_t isDch2Ok = dch2Id > -1 && fDet.GetDet(kDCH2) && fDch2Hits;
//
//        Float_t P_rec_glob = Abs(1.0 / globalTrack->GetParamFirst()->GetQp());
//        Float_t Tx_glob = globalTrack->GetParamFirst()->GetTx();
//        Float_t Ty_glob = globalTrack->GetParamFirst()->GetTy();
//        Float_t Pz_rec_glob = P_rec_glob / Sqrt(Tx_glob * Tx_glob + Ty_glob * Ty_glob + 1);
//        Float_t Px_rec_glob = Pz_rec_glob * Tx_glob;
//        Float_t Py_rec_glob = Pz_rec_glob * Ty_glob;
//        Float_t Eta_rec_glob = 0.5 * Log((P_rec_glob + Pz_rec_glob) / (P_rec_glob - Pz_rec_glob));
//
//        Int_t refId = globalTrack->GetRefId();
//        if (refId < 0) continue;
//        const CbmMCTrack* mcTrack = (const CbmMCTrack*) (fMCTracks->At(refId));
//        Float_t P_sim = mcTrack->GetP();
//        Float_t Px_sim = mcTrack->GetPx();
//        Float_t Py_sim = mcTrack->GetPy();
//        Float_t Pz_sim = mcTrack->GetPz();
//        Float_t Eta_sim = 0.5 * Log((P_sim + Pz_sim) / (P_sim - Pz_sim));
//
//        Float_t Eta_rec_seed, Eta_rec_gem;
//        Float_t P_rec_seed, P_rec_gem;
//        if (isGemOk) {
//            BmnGemTrack* gemTrack = (BmnGemTrack*) (fGemTracks->At(gemId));
//            P_rec_gem = Abs(1.0 / gemTrack->GetParamFirst()->GetQp());
//            Float_t Tx_gem = gemTrack->GetParamFirst()->GetTx();
//            Float_t Ty_gem = gemTrack->GetParamFirst()->GetTy();
//            Float_t Pz_rec_gem = P_rec_gem / Sqrt(Tx_gem * Tx_gem + Ty_gem * Ty_gem + 1);
//            Float_t Px_rec_gem = Pz_rec_gem * Tx_gem;
//            Eta_rec_gem = 0.5 * Log((P_rec_gem + Pz_rec_gem) / (P_rec_gem - Pz_rec_gem));
//            fHM->H2("EtaP_rec_gem")->Fill(Eta_rec_gem, P_rec_gem);
//            fHM->H2("P_rec_P_sim_gem")->Fill(P_sim, P_rec_gem);
//            fHM->H2("Eta_rec_Eta_sim_gem")->Fill(Eta_sim, Eta_rec_gem);
//        }
//
//        if (isSeedOk) {
//            BmnGemTrack* gemSeed = (BmnGemTrack*) (fGemSeeds->At(gemId));
//            P_rec_seed = Abs(1.0 / gemSeed->GetParamFirst()->GetQp());
//            Float_t Tx_seed = gemSeed->GetParamFirst()->GetTx();
//            Float_t Ty_seed = gemSeed->GetParamFirst()->GetTy();
//            Float_t Pz_rec_seed = P_rec_seed / Sqrt(Tx_seed * Tx_seed + Ty_seed * Ty_seed + 1);
//            Float_t Py_rec_seed = Pz_rec_seed * Ty_seed;
//            Eta_rec_seed = 0.5 * Log((P_rec_seed + Pz_rec_seed) / (P_rec_seed - Pz_rec_seed));
//            fHM->H2("EtaP_rec_seed")->Fill(Eta_rec_seed, P_rec_seed);
//            fHM->H2("P_rec_P_sim_seed")->Fill(P_sim, P_rec_seed);
//            fHM->H2("Eta_rec_Eta_sim_seed")->Fill(Eta_sim, Eta_rec_seed);
//        }
//
//        //        fHM->H2("momRes_2D")->Fill(P_sim, Abs(P_sim - P_rec_glob) / P_sim * 100.0);
//        //        fHM->H2("P_rec_P_sim_glob")->Fill(P_sim, P_rec_glob);
//        //        fHM->H2("Eta_rec_Eta_sim_glob")->Fill(Eta_sim, Eta_rec_glob);
//        //        fHM->H2("Px_rec_Px_sim")->Fill(Px_sim, Px_rec_glob);
//        //        fHM->H2("Py_rec_Py_sim")->Fill(Py_sim, Py_rec_glob);
//        //        fHM->H2("Pz_rec_Pz_sim")->Fill(Pz_sim, Pz_rec_glob);
//        //        fHM->H2("EtaP_rec_glob")->Fill(Eta_rec_glob, P_rec_glob);
//        //        fHM->H2("EtaP_sim")->Fill(Eta_sim, P_sim);
//        //        for (Int_t iBin = 0; iBin < fHM->H2("momRes_2D")->GetNbinsX(); iBin += 1) {
//        //            fHM->H2("momRes_1D")->SetBinContent(iBin, fHM->H2("momRes_2D")->ProjectionY("tmp", iBin, iBin)->GetMean());
//        //            fHM->H2("momRes_1D")->SetBinError(iBin, fHM->H2("momRes_2D")->ProjectionY("tmp", iBin, iBin)->GetStdDev(1));
//        //            //            fHM->H2("momRes_1D")->SetBinError(iBin, 0.0); //FIXME! MAKE CORRECT CALCULATION OF ERRORS! 
//        //        }
//
//        // check the quality of global track ---->
//        const BmnTrackMatch* glTrackMatch = (const BmnTrackMatch*) (fGlobalTrackMatches->At(iTrack));
//        if (!glTrackMatch) continue;
//        Bool_t isTrackOk = glTrackMatch->GetTrueOverAllHitsRatio() >= fQuota;
//        if (!isTrackOk) { // ghost track
//            fHM->H1("ghostGlobDistr")->Fill(P_sim);
//        }
//        fHM->H1("recoGlobDistr")->Fill(P_sim);
//        // <---- check the quality of global track
//
//        //check the quality of track segments
//        const BmnTrackMatch* gemTrackMatch;
//        const BmnTrackMatch* gemSeedMatch;
//        if (isGemOk) {
//            //            cout << "N fGemMatches = " << fGemMatches->GetEntriesFast() << endl;
//            gemTrackMatch = (const BmnTrackMatch*) (fGemMatches->At(gemId));
//            isGemOk = gemTrackMatch->GetTrueOverAllHitsRatio() >= fQuota; //CheckTrackQuality(stsTrackMatch, kGEM);
//            FillTrackQualityHistograms(gemTrackMatch, kGEM);
//            if (!isGemOk) { // ghost track
//                Int_t nofHits = gemTrackMatch->GetNofHits();
//                fHM->H1("hng_NofGhosts_Gem_Nh")->Fill(nofHits);
//                fHM->H1("ghostGemDistr")->Fill(P_sim);
//            } else {
//                fHM->H1("wellGemDistr")->Fill(P_sim);
//            }
//            fHM->H1("recoGemDistr")->Fill(P_sim);
//        }
//
//        // Get MC indices of track segments
//        Int_t gemMCId = -1, tof1MCId = -1, tof2MCId = -1, dch1MCId = -1, dch2MCId = -1;
//        if (isGemOk) {
//            gemMCId = gemTrackMatch->GetMatchedLink().GetIndex();
//        }
//        if (isTof1Ok) {
//            const BmnHit* tofHit = (const BmnHit*) (fTof1Hits->At(tof1Id));
//            const FairMCPoint* tofPoint = (const FairMCPoint*) (fTof1Points->At(tofHit->GetRefIndex()));
//            if (tofPoint != NULL) tof1MCId = tofPoint->GetTrackID();
//        }
//        if (isTof2Ok) {
//            const BmnHit* tofHit = (const BmnHit*) (fTof2Hits->At(tof2Id));
//            const FairMCPoint* tofPoint = (const FairMCPoint*) (fTof2Points->At(tofHit->GetRefIndex()));
//            if (tofPoint != NULL) tof2MCId = tofPoint->GetTrackID();
//        }
//        if (isDch1Ok) {
//            const BmnHit* dchHit = (const BmnHit*) (fDch1Hits->At(dch1Id));
//            const FairMCPoint* dchPoint = (const FairMCPoint*) (fDch1Points->At(dchHit->GetRefIndex()));
//            if (dchPoint != NULL) dch1MCId = dchPoint->GetTrackID();
//        }
//        if (isDch2Ok) {
//            const BmnHit* dchHit = (const BmnHit*) (fDch2Hits->At(dch2Id));
//            const FairMCPoint* dchPoint = (const FairMCPoint*) (fDch2Points->At(dchHit->GetRefIndex()));
//            if (dchPoint != NULL) dch2MCId = dchPoint->GetTrackID();
//        }
//
//        map<string, multimap<Int_t, Int_t> >::iterator it;
//        for (it = fMcToRecoMap.begin(); it != fMcToRecoMap.end(); it++) {
//            string name = (*it).first;
//            multimap<Int_t, Int_t>& mcRecoMap = (*it).second;
//            Bool_t gem = (name.find("Gem") != string::npos) ? isGemOk && gemMCId != -1 : kTRUE;
//            Bool_t tof1 = (name.find("Tof1") != string::npos) ? isTof1Ok && gemMCId == tof1MCId : kTRUE;
//            Bool_t tof2 = (name.find("Tof2") != string::npos) ? isTof2Ok && gemMCId == tof2MCId : kTRUE;
//            Bool_t dch1 = (name.find("Dch1") != string::npos) ? isDch1Ok && gemMCId == dch1MCId : kTRUE;
//            Bool_t dch2 = (name.find("Dch2") != string::npos) ? isDch2Ok && gemMCId == dch2MCId : kTRUE;
//
//            if (gem && tof1 && tof2 && dch1 && dch2) {
//                pair<Int_t, Int_t> tmp = make_pair(gemMCId, iTrack);
//                mcRecoMap.insert(tmp);
//            }
//        }
//    }
//}

void BmnTrackingQa::FillTrackQualityHistograms(const BmnTrackMatch* trackMatch, DetectorId detId) {
    string detName = (detId == kGEM) ? "Gem" : (detId == kTOF1) ? "Tof1" : (detId == kDCH1) ? "Dch1" : (detId == kDCH2) ? "Dch2" : (detId == kTOF) ? "Tof2" : "";
    assert(detName != "");
    string histName = "hth_" + detName + "_TrackHits";
    fHM->H1(histName + "_All")->Fill(trackMatch->GetNofHits());
    fHM->H1(histName + "_True")->Fill(trackMatch->GetNofTrueHits());
    fHM->H1(histName + "_Fake")->Fill(trackMatch->GetNofWrongHits());
    fHM->H1(histName + "_TrueOverAll")->Fill(trackMatch->GetTrueOverAllHitsRatio());
    fHM->H1(histName + "_FakeOverAll")->Fill(trackMatch->GetWrongOverAllHitsRatio());
}

void BmnTrackingQa::ProcessMcTracks() {
    vector<TH1*> effHistos = fHM->H1Vector("(hte|hpe)_.*_Eff_.*");
    Int_t nofEffHistos = effHistos.size();

    Int_t nofMcTracks = fMCTracks->GetEntriesFast();
    for (Int_t iMCTrack = 0; iMCTrack < nofMcTracks; iMCTrack++) {
        const CbmMCTrack* mcTrack = (const CbmMCTrack*) (fMCTracks->At(iMCTrack));

        // Check accepted tracks cutting on minimal number of MC points
        if (!fMCTrackCreator->TrackExists(iMCTrack)) continue;

        const BmnMCTrack& bmnMCTrack = fMCTrackCreator->GetTrack(iMCTrack);
        //        Int_t nofPointsGem = bmnMCTrack.GetNofPointsInDifferentStations(kGEM);
        Int_t nofPointsGem = bmnMCTrack.GetNofPoints(kGEM);
        Int_t nofPointsTof1 = bmnMCTrack.GetNofPoints(kTOF1);
        Int_t nofPointsTof2 = bmnMCTrack.GetNofPoints(kTOF);
        Int_t nofPointsDch1 = bmnMCTrack.GetNofPoints(kDCH1);
        Int_t nofPointsDch2 = bmnMCTrack.GetNofPoints(kDCH2);

        //        cout << "nofPointsGem = " << nofPointsGem << " nofPointsTof1 = " << nofPointsTof1 << " nofPointsTof2 = " << nofPointsTof2 << " nofPointsDch1 = " << nofPointsDch1 << " nofPointsDch2 = " << nofPointsDch2 << endl;

        // Check local tracks
        //Bool_t gemConsecutive = (fUseConsecutivePointsInGem) ? bmnMCTrack.GetNofConsecutivePoints(kGEM) >= fMinNofPointsGem : kTRUE;

        Bool_t isGemOk = nofPointsGem >= fMinNofPointsGem && fDet.GetDet(kGEM); // && gemConsecutive;
        Bool_t isTof1Ok = nofPointsTof1 >= fMinNofPointsTof && fDet.GetDet(kTOF1);
        Bool_t isTof2Ok = nofPointsTof2 >= fMinNofPointsTof && fDet.GetDet(kTOF);
        Bool_t isDch1Ok = nofPointsDch1 >= fMinNofPointsDch && fDet.GetDet(kDCH1);
        Bool_t isDch2Ok = nofPointsDch2 >= fMinNofPointsDch && fDet.GetDet(kDCH2);

        // calculate polar angle
        TVector3 mom;
        mcTrack->GetMomentum(mom);
        Double_t angle = abs(mom.Theta() * 180 / TMath::Pi());
        Double_t mcP = mcTrack->GetP();
        Double_t mcY = mcTrack->GetRapidity();
        Double_t mcPt = mcTrack->GetPt();

        // Fill parameter name to value map
        map<string, vector<Double_t> > parMap;

        vector<Double_t> tmp = list_of(mcP);
        parMap["p"] = tmp;

        vector<Double_t> tmp1 = list_of(mcY);
        parMap["y"] = tmp1;

        vector<Double_t> tmp2 = list_of(mcPt);
        parMap["pt"] = tmp2;

        vector<Double_t> tmp3 = list_of(angle);
        parMap["Angle"] = tmp3;

        vector<Double_t> tmp4 = list_of(0);
        parMap["Np"] = tmp4; // FIXME : correct to number of points in concrete detector!
        // Currently as a  temporary solution it is reassigned later

        vector<Double_t> tmp5 = list_of(mcY)(mcPt);
        parMap["YPt"] = tmp5;

        if (isGemOk) {
            fHM->H1("allGemDistr")->Fill(mcP);
            fHM->H1("allGlobDistr")->Fill(mcP);
        }

        for (Int_t iHist = 0; iHist < nofEffHistos; iHist++) {
            TH1* hist = effHistos[iHist];
            string histName = hist->GetName();
            vector<string> split = Split(histName, '_');
            string effName = split[1];
            string normName = split[2];
            string catName = split[3];
            string histTypeName = split[0];
            string parName = split[5];
            assert(parMap.count(parName) != 0);

            vector<Double_t> par = list_of(0);
            if (parName == "Np") {
                vector<Double_t> tmp = list_of((effName == "Gem") ? nofPointsGem : (effName == "Tof1") ? nofPointsTof1 : (effName == "Tof2") ? nofPointsTof2 : (effName == "Dch1") ? nofPointsDch1 : (effName == "Dch2") ? nofPointsDch2 : 0);
                par = tmp;
            } else {
                par = parMap[parName];
            }

            Bool_t gem = (normName.find("Gem") != string::npos) ? isGemOk : kTRUE;
            Bool_t tof1 = (normName.find("Tof1") != string::npos) ? isTof1Ok : kTRUE;
            Bool_t tof2 = (normName.find("Tof2") != string::npos) ? isTof2Ok : kTRUE;
            Bool_t dch1 = (normName.find("Dch1") != string::npos) ? isDch1Ok : kTRUE;
            Bool_t dch2 = (normName.find("Dch2") != string::npos) ? isDch2Ok : kTRUE;

            //            Bool_t accOk = gem && tof1 && tof2 && dch1 && dch2;
            Bool_t accOk = gem;
            if (accOk) FillGlobalReconstructionHistos(iMCTrack, fMcToRecoMap[effName], histName, histTypeName, effName, catName, par);
        } // Loop over efficiency histograms
    } // Loop over MCTracks
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

    fHM->H1("hno_NofObjects_GlobalTracks")->Fill(fGlobalTracks->GetEntriesFast());
    if (fDet.GetDet(kGEM)) {
        fHM->H1("hno_NofObjects_GemTracks")->Fill(fGemTracks->GetEntriesFast());
        fHM->H1("hno_NofObjects_GemHits")->Fill(fGemHits->GetEntriesFast());
    }
    if (fDet.GetDet(kTOF1) && fTof1Hits) fHM->H1("hno_NofObjects_Tof1Hits")->Fill(fTof1Hits->GetEntriesFast());
    if (fDet.GetDet(kDCH1) && fDch1Hits) fHM->H1("hno_NofObjects_Dch1Hits")->Fill(fDch1Hits->GetEntriesFast());
    if (fDet.GetDet(kDCH2) && fDch2Hits) fHM->H1("hno_NofObjects_Dch2Hits")->Fill(fDch2Hits->GetEntriesFast());
    if (fDet.GetDet(kTOF) && fTof2Hits) fHM->H1("hno_NofObjects_Tof2Hits")->Fill(fTof2Hits->GetEntriesFast());
}

ClassImp(BmnTrackingQa);
