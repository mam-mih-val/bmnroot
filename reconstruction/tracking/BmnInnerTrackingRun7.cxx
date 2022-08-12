#include "BmnInnerTrackingRun7.h"

#include <TCanvas.h>
#include <TH2F.h>
#include <TLine.h>
#include <TMath.h>

#include <map>
#include <vector>

#include "BmnCellDuet.h"
#include "BmnGemStripHit.h"
#include "BmnKalmanFilter.h"
#include "BmnMatch.h"
#include "BmnMath.h"
#include "BmnSiliconHit.h"
#include "FairRunAna.h"
#include "FairTrackParam.h"
#include "TStyle.h"
#include "BmnFieldMap.h"
#if defined(_OPENMP)
#include <cstdlib>

#include "omp.h"
#endif

//-----------------------------------------
static Double_t workTime = 0.0;
static Double_t hitMatchTime = 0.0;
static Double_t hitMatchTimeLoop = 0.0;
static Double_t construct_4of4_time = 0.0;
static Double_t construct_3of4_time = 0.0;
static Double_t construct_2of4_time = 0.0;
static Double_t CalculateTrackParTime = 0.0;
static Double_t SortHitsTime = 0.0;
static Double_t trackUpdateTime = 0.0;
static Double_t sortTrackTime = 0.0;
static Double_t trackSelectionTime = 0.0;
//-----------------------------------------

using namespace std;
using namespace TMath;

BmnInnerTrackingRun7::BmnInnerTrackingRun7(Int_t run, Bool_t target) : fSteering(nullptr), fSteerFile("") {
    fEventNo = 0;
    fIsTarget = target;
    fNSiliconStations = 0;
    fGemHitsArray = nullptr;
    fSilHitsArray = nullptr;
    fHitsArray = nullptr;
    fRoughVertex = TVector3(0.5, -4.6, -2.3);
    fGlobTracksArray = nullptr;
    fGemTracksArray = nullptr;
    fSilTracksArray = nullptr;
    fField = nullptr;
    fGemHitsBranchName = "BmnGemStripHit";
    fSilHitsBranchName = "BmnSiliconHit";
    fGlobTracksBranchName = "BmnGlobalTrack";
    fGemTracksBranchName = "BmnGemTrack";
    fSilTracksBranchName = "BmnSiliconTrack";
    fGemDetector = nullptr;
    fSilDetector = nullptr;
    fHitXCutMin = nullptr;
    fHitXCutMax = nullptr;
    fHitYCutMin = nullptr;
    fHitYCutMax = nullptr;
    fNHitsCut = 0;
    fDoHitAsymFiltration = kTRUE;
}

BmnInnerTrackingRun7::~BmnInnerTrackingRun7() {
    delete fKalman;
    delete fGemDetector;
    delete fSilDetector;
    delete[] fHitXCutMin;
    delete[] fHitXCutMax;
    delete[] fHitYCutMin;
    delete[] fHitYCutMax;
}

InitStatus BmnInnerTrackingRun7::Init() {
    if (fVerbose > 1) cout << "======================== GEM tracking init started ====================" << endl;

    fKalman = new BmnKalmanFilter();

    //Get ROOT Manager
    FairRootManager* ioman = FairRootManager::Instance();
    if (!ioman)
        Fatal("Init", "FairRootManager is not instantiated");

    //MC tracks for drawing during debugging
    fMCTracksArray = (TClonesArray*)ioman->GetObject("MCTrack");  //in

    //GEM
    fGemPointsArray = (TClonesArray*)ioman->GetObject("StsPoint");        //in
    fGemHitsArray = (TClonesArray*)ioman->GetObject(fGemHitsBranchName);  //in
    fGemTracksArray = new TClonesArray("BmnTrack", 100);                  //out
    ioman->Register(fGemTracksBranchName, "GEM", fGemTracksArray, kTRUE);

    //SILICON
    fSilPointsArray = (TClonesArray*)ioman->GetObject("SiliconPoint");    //in
    fSilHitsArray = (TClonesArray*)ioman->GetObject(fSilHitsBranchName);  //in
    fSilTracksArray = new TClonesArray("BmnTrack", 100);                  //out
    ioman->Register(fSilTracksBranchName, "SILICON", fSilTracksArray, kTRUE);

    if (!fGemHitsArray) {
        cout << "BmnInnerTrackingRun7::Init(): branch " << fGemHitsBranchName << " not found! Task will be deactivated" << endl;
        SetActive(kFALSE);
        return InitStatus::kERROR;
    }

    fGlobTracksArray = new TClonesArray("BmnGlobalTrack", 100);  //out
    ioman->Register(fGlobTracksBranchName, "GLOBAL", fGlobTracksArray, kTRUE);
    fHitsArray = new TClonesArray("BmnHit", 100);  //out
    ioman->Register("BmnInnerHits", "HITS", fHitsArray, kTRUE);

    fField = FairRunAna::Instance()->GetField();
    if (!fField) Fatal("Init", "No Magnetic Field found");

    TString gPathConfig = gSystem->Getenv("VMCWORKDIR");

    TString gPathGemConfig = gPathConfig + "/parameters/gem/XMLConfigs/";
    TString confGem = "GemRunSpring2018.xml";
    fGemDetector = new BmnGemStripStationSet(gPathGemConfig + confGem);

    TString gPathSiConfig = gPathConfig + "/parameters/silicon/XMLConfigs/";
    TString confSi = "SiliconRunSpring2018.xml";
    fSilDetector = new BmnSiliconStationSet(gPathSiConfig + confSi);

    Int_t nGemStations = fGemDetector->GetNStations();
    Int_t nSilStations = fSilDetector->GetNStations();

    fNSiliconStations = nSilStations;

    fNStations = nGemStations + nSilStations;

    fHitXCutMin = new Double_t[fNStations];
    fHitXCutMax = new Double_t[fNStations];
    fHitYCutMin = new Double_t[fNStations];
    fHitYCutMax = new Double_t[fNStations];
    
    BmnFieldMap* field = (BmnFieldMap*)FairRunAna::Instance()->GetField();
    Bool_t isField = !(field->IsFieldOff());
    fSteering = new BmnSteering(isField ? "BMN_run7_withField.dat" : "BMN_run7_noField.dat");

    if (fVerbose > 1) cout << "======================== GEM tracking init finished ===================" << endl;

    return kSUCCESS;
}

void BmnInnerTrackingRun7::Exec(Option_t* opt) {
    if (fVerbose > 1) cout << "\n======================== GEM tracking exec started ====================" << endl;
    if (fVerbose > 1) cout << "\n Event number: " << fEventNo << endl;

    if (!IsActive()) return;

    clock_t tStart = clock();

    fSilTracksArray->Delete();
    fGemTracksArray->Delete();
    fGlobTracksArray->Delete();
    fHitsArray->Delete();

    fEventNo++;

    Int_t nHitsCut = fSteering->GetNHitsCutTotal();

    Int_t nSilStations = fSilDetector->GetNStations();
    Int_t nGemStations = fGemDetector->GetNStations();

    Double_t a[2][9] = {{1.29, 1.50, 1.33, 1.14, 1.17, 1.14, 1.00, 1.14, 1.14},
                        {1.33, 1.17, 1.00, 1.14, 1.33, 1.33, 1.14, 1.33, 1.33}};
    Double_t b[2][9] = {{-645, -1500, -1330, -570, -585, -570, -500, -570, -570},
                        {500, 1000, 1000, 500, 500, 500, 500, 500, 500}};

    for (Int_t iHit = 0; iHit < fSilHitsArray->GetEntriesFast(); ++iHit) {
        BmnSiliconHit* hit = (BmnSiliconHit*)fSilHitsArray->At(iHit);
        Double_t Sl = hit->GetStripTotalSignalInLowerLayer();
        Double_t Su = hit->GetStripTotalSignalInUpperLayer();
        Int_t iSt = hit->GetStation();
        if (fDoHitAsymFiltration)
            if (Sl < a[0][iSt] * Su + b[0][iSt] || Sl > a[1][iSt] * Su + b[1][iSt]) {
                hit->SetType(0);
                continue;
            } else
                hit->SetType(1);
        BmnHit innerHit = *hit;
        innerHit.SetIndex(iHit);
        innerHit.SetDetId(kSILICON);
        innerHit.SetDxyz(0.5, 0.5, 0.5);
        //innerHit.SetDxyz(hit->GetDx(), hit->GetDy(), hit->GetDz());
        new ((*fHitsArray)[fHitsArray->GetEntriesFast()]) BmnHit(innerHit);
    }
    for (Int_t iHit = 0; iHit < fGemHitsArray->GetEntriesFast(); ++iHit) {
        BmnGemStripHit* hit = (BmnGemStripHit*)fGemHitsArray->At(iHit);
        Double_t Sl = hit->GetStripTotalSignalInLowerLayer();
        Double_t Su = hit->GetStripTotalSignalInUpperLayer();
        Int_t iSt = hit->GetStation() + nSilStations;
        if (fDoHitAsymFiltration)
            if (Sl < a[0][iSt] * Su + b[0][iSt] || Sl > a[1][iSt] * Su + b[1][iSt]) {
                hit->SetType(0);
                continue;
            } else
                hit->SetType(1);
        BmnHit innerHit = *hit;
        innerHit.SetStation(iSt);  //shift for correct station numbering
        innerHit.SetIndex(iHit);
        innerHit.SetDetId(kGEM);
        innerHit.SetDxyz(0.5, 0.5, 0.5);
        //innerHit.SetDxyz(hit->GetDx(), hit->GetDy(), hit->GetDz());
        new ((*fHitsArray)[fHitsArray->GetEntriesFast()]) BmnHit(innerHit);
    }

    if (fHitsArray->GetEntriesFast() > nHitsCut || fHitsArray->GetEntriesFast() == 0) return;

    for (Int_t iStat = 0; iStat < fNStations; iStat++) {
        fHitXCutMin[iStat] = fSteering->GetHitXCutMin()[iStat];
        fHitXCutMax[iStat] = fSteering->GetHitXCutMax()[iStat];
        fHitYCutMin[iStat] = fSteering->GetHitYCutMin()[iStat];
        fHitYCutMax[iStat] = fSteering->GetHitYCutMax()[iStat];
    }

    fNHitsCut = fSteering->GetNHitsCut();
    fChiSquareCut = fSteering->GetChiSquareCut();

    fDistCut = 0.45;
    fNHitsCut = 4;
    FindTracks_4of4_OnLastGEMStations();
    //cout << "FindTracks_4of4_OnLastStations: " << fGlobTracksArray->GetEntriesFast() << " tracks" << endl;
    FindTracks_3of4_OnLastGEMStations();
    //cout << "FindTracks_3of4_OnLastStations: " << fGlobTracksArray->GetEntriesFast() << " tracks" << endl;
    fNHitsCut = 5;
    FindTracks_2of2_OnFirstGEMStationsDownstream();
    //cout << "FindTracks_2of2_OnFirstGEMStationsDownstream: " << fGlobTracksArray->GetEntriesFast() << " tracks" << endl;
    FindTracks_2of2_OnFirstGEMStationsUpstream();
    //cout << "FindTracks_2of2_OnFirstGEMStationsUpstream: " << fGlobTracksArray->GetEntriesFast() << " tracks" << endl;

    //DrawHits();

    clock_t tFinish = clock();
    if (fVerbose > 0) cout << "BmnInnerTrackingRun7: " << fGlobTracksArray->GetEntriesFast() << " tracks" << endl;

    workTime += ((Double_t)(tFinish - tStart)) / CLOCKS_PER_SEC;

    if (fVerbose > 1) cout << "\n======================== GEM tracking exec finished ===================" << endl;
}

BmnStatus BmnInnerTrackingRun7::FindTracks_4of4_OnLastGEMStations() {
    //Fit of dX vs X for different stations {p0, p1, sigma} (from qgsm simulation)
    const Int_t nxRanges = 8;
    const Int_t nyRanges = 5;
    Double_t xRangeMin[nxRanges] = {-80, -60, -40, -20, 0, 20, 40, 60};
    Double_t xRangeMax[nxRanges] = {-60, -40, -20, 0, 20, 40, 60, 80};
    map<Short_t, vector<Double_t>> xRMS = {//<station, vector of RMS >
                                           {6, {7.0, 5.3, 4.4, 3.5, 3.4, 3.9, 5.1, 5.3}},
                                           {7, {6.8, 6.3, 4.8, 3.7, 3.7, 4.5, 5.3, 6.0}},
                                           {8, {6.0, 5.9, 5.2, 4.3, 3.4, 4.2, 4.8, 5.1}}};
    Double_t yRangeMin[nyRanges] = {-10, 0, 10, 20, 30};
    Double_t yRangeMax[nyRanges] = {0, 10, 20, 30, 40};
    map<Short_t, vector<Double_t>> yRMS = {//<station, vector of RMS >
                                           {6, {0.8, 0.8, 0.9, 1.0, 0.9}},
                                           {7, {0.9, 0.7, 1.0, 1.0, 0.9}},
                                           {8, {0.5, 0.6, 0.8, 0.7, 0.7}}};

    Double_t Y6[2] = {0.87, 0.16};
    Double_t Y7[2] = {0.84, 0.15};
    Double_t Y8[2] = {0.61, 0.13};
    Double_t X6[2] = {1.04, 0.19};
    Double_t X7[2] = {1.42, 0.18};
    Double_t X8[2] = {1.41, 0.14};

    vector<BmnHit*> sortedHits[fNStations];
    SortHits(sortedHits);

    vector<BmnTrack> candidates;
    vector<BmnTrack> sortedCandidates;

    clock_t t0 = 0;  //clock();
    //    Int_t nThreads = 1;
    // #if defined(_OPENMP)
    //     //char * number_of_threads = getenv("OMP_NUM_THREADS");
    //     //nThreads = atoi(number_of_threads);
    //     nThreads = 4;
    //     omp_set_num_threads(nThreads);
    // #endif
    // #pragma omp parallel for
    for (BmnHit* hit8 : sortedHits[8]) {
        for (BmnHit* hit7 : sortedHits[7]) {
            Double_t x8 = hit8->GetX();
            Double_t y8 = hit8->GetY();
            Double_t dxEx = x8 - hit7->GetX();
            Double_t dxTh = x8 * X8[1] + X8[0];
            Double_t dyEx = y8 - hit7->GetY();
            Double_t dyTh = y8 * Y8[1] + Y8[0];

            Int_t iX = 0;
            for (iX = 0; iX < nxRanges; ++iX)
                if (x8 > xRangeMin[iX] && x8 < xRangeMax[iX]) break;
            Int_t iY = 0;
            for (iY = 0; iY < nyRanges; ++iY)
                if (y8 > yRangeMin[iY] && y8 < yRangeMax[iY]) break;
            if ((iX >= nxRanges) || (iY >= nyRanges))
            {
                if (fVerbose > 0) cout<<"WARNING: x8 or y8 is out of range, the hit is skipped (x = "<<x8<<", y = "<<y8<<")"<<endl;
                continue;
            }
            Double_t rmsX = 2.0 * xRMS[8][iX];
            Double_t rmsY = 3.0 * yRMS[8][iY];//2.0 * yRMS[8][iY];

            if (Abs(dxEx - dxTh) > rmsX || Abs(dyEx - dyTh) > rmsY) continue;
            for (BmnHit* hit6 : sortedHits[6]) {
                Double_t x7 = hit7->GetX();
                Double_t y7 = hit7->GetY();
                dxEx = x7 - hit6->GetX();
                dxTh = x7 * X7[1] + X7[0];
                dyEx = y7 - hit6->GetY();
                dyTh = y7 * Y7[1] + Y7[0];

                iX = 0;
                for (iX = 0; iX < nxRanges; ++iX)
                    if (x7 > xRangeMin[iX] && x7 < xRangeMax[iX]) break;
                iY = 0;
                for (iY = 0; iY < nyRanges; ++iY)
                    if (y7 > yRangeMin[iY] && y7 < yRangeMax[iY]) break;
                if ((iX >= nxRanges) || (iY >= nyRanges))
                {
                    if (fVerbose > 0) cout<<"WARNING: x7 or y7 is out of range, the hit is skipped (x = "<<x7<<", y = "<<y7<<")"<<endl;
                    continue;
                }
                rmsX = 2.0 * xRMS[7][iX];
                rmsY = 3.0 * yRMS[7][iY];//2.0 * yRMS[7][iY];

                if (Abs(dxEx - dxTh) > rmsX || Abs(dyEx - dyTh) > rmsY) continue;
                for (BmnHit* hit5 : sortedHits[5]) {
                    Double_t x6 = hit6->GetX();
                    Double_t y6 = hit6->GetY();
                    dxEx = x6 - hit5->GetX();
                    dxTh = x6 * X6[1] + X6[0];
                    dyEx = y6 - hit5->GetY();
                    dyTh = y6 * Y6[1] + Y6[0];

                    iX = 0;
                    for (iX = 0; iX < nxRanges; ++iX)
                        if (x6 > xRangeMin[iX] && x6 < xRangeMax[iX]) break;
                    iY = 0;
                    for (iY = 0; iY < nyRanges; ++iY)
                        if (y6 > yRangeMin[iY] && y6 < yRangeMax[iY]) break;
                    if ((iX >= nxRanges) || (iY >= nyRanges))
                    {
                        if (fVerbose > 0) cout<<"WARNING: x6 or y6 is out of range, the hit is skipped (x = "<<x6<<", y = "<<y6<<")"<<endl;
                        continue;
                    }
                    rmsX = 2.0 * xRMS[6][iX];
                    rmsY = 3.0 * yRMS[6][iY];//2.0 * yRMS[6][iY];

                    if (Abs(dxEx - dxTh) > rmsX || Abs(dyEx - dyTh) > rmsY) continue;
                    BmnTrack cand;
                    cand.AddHit(hit8->GetRefIndex(), hit8);
                    cand.AddHit(hit7->GetRefIndex(), hit7);
                    cand.AddHit(hit6->GetRefIndex(), hit6);
                    cand.AddHit(hit5->GetRefIndex(), hit5);
                    cand.SortHits();
                    candidates.push_back(cand);
                }
            }
        }
    }
    clock_t t1 = 0;  //clock();
    construct_4of4_time += ((Double_t)(t1 - t0)) / CLOCKS_PER_SEC;

    //cout << "FindTracks_4of4_OnLastGEMStations::candidates = " << candidates.size() << endl;

    for (BmnTrack& cand : candidates) {
        //for (Int_t iCand = 0; iCand < candidates.size(); ++iCand) {
        //BmnTrack cand = candidates[iCand];
        if (CalculateTrackParams(&cand) == kBMNERROR) continue;
        //First two GEM
        MatchHit(&cand, sortedHits[4], kFALSE);
        MatchHit(&cand, sortedHits[3], kFALSE);
        //Silicon
        MatchHit(&cand, sortedHits[2], kFALSE);
        MatchHit(&cand, sortedHits[1], kFALSE);
        MatchHit(&cand, sortedHits[0], kFALSE);

        // NDF = (N counts in ZX plane + N counts in ZY plane) - 2 parameters of Line in ZY plane - 3 parameters of Circle in ZX plane
        // Check it!!!
        cand.SetNDF(cand.GetNHits() * 2 - 5);
    }

    // for (BmnTrack& cand : candidates) {
    //     cout << cand.GetNDF() << endl;
    //     cand.Print();
    // }
    
    BmnFieldMap* field = (BmnFieldMap*)FairRunAna::Instance()->GetField();
    Bool_t isField = !(field->IsFieldOff());
    if (isField)
        TrackUpdateByKalman(candidates);
    else
        TrackUpdateByLine(candidates);
    SortTracks(candidates, sortedCandidates);
    TrackSelection(sortedCandidates);

    return kBMNSUCCESS;
}

BmnStatus BmnInnerTrackingRun7::FindCandidateByThreeStations(Short_t st0,
                                                             Short_t st1,
                                                             Short_t st2,
                                                             vector<BmnTrack>& candidates,
                                                             vector<BmnHit*>* sortedHits) {
    clock_t t0 = 0;  //clock();
    //Fit of dX vs X (dY vs Y) for different stations {p0, p1, sigma} (from qgsm simulation)
    const Int_t nxRanges = 8;
    const Int_t nyRanges = 5;
    Double_t xRangeMin[nxRanges] = {-80, -60, -40, -20, 0, 20, 40, 60};
    Double_t xRangeMax[nxRanges] = {-60, -40, -20, 0, 20, 40, 60, 80};

    map<Short_t, vector<Double_t>> xRMS = {//<station, vector of RMS >
                                           {6, {7.0, 5.3, 4.4, 3.5, 3.4, 3.9, 5.1, 5.3}},
                                           {7, {6.8, 6.3, 4.8, 3.7, 3.7, 4.5, 5.3, 6.0}},
                                           {8, {6.0, 5.9, 5.2, 4.3, 3.4, 4.2, 4.8, 5.1}}};
    Double_t yRangeMin[nyRanges] = {-10, 0, 10, 20, 30};
    Double_t yRangeMax[nyRanges] = {0, 10, 20, 30, 40};
    map<Short_t, vector<Double_t>> yRMS = {//<station, vector of RMS >
                                           {6, {0.8, 0.8, 0.9, 1.0, 0.9}},
                                           {7, {0.9, 0.7, 1.0, 1.0, 0.9}},
                                           {8, {0.5, 0.6, 0.8, 0.7, 0.7}}};
    map<Short_t, vector<Double_t>> xMap = {//<station, <p0, p1> >
                                           {6, {1.04, 0.19}},
                                           {7, {1.42, 0.18}},
                                           {8, {1.41, 0.14}}};
    map<Short_t, vector<Double_t>> yMap = {//<station, <p0, p1> >
                                           {6, {0.87, 0.16}},
                                           {7, {0.84, 0.15}},
                                           {8, {0.61, 0.13}}};

    for (BmnHit* hit2 : sortedHits[st2]) {
        for (BmnHit* hit1 : sortedHits[st1]) {
            Double_t x2 = hit2->GetX();
            Double_t y2 = hit2->GetY();
            Double_t x1 = hit1->GetX();
            Double_t y1 = hit1->GetY();
            Double_t dxEx = Abs(x2 - x1);
            Double_t dxTh = Abs(x2 * xMap[st2][1] + xMap[st2][0]);
            Double_t dyEx = Abs(y2 - y1);
            Double_t dyTh = Abs(y2 * yMap[st2][1] + yMap[st2][0]);

            Int_t iX = 0;
            for (iX = 0; iX < nxRanges; ++iX)
                if (x2 > xRangeMin[iX] && x2 < xRangeMax[iX]) break;
            Int_t iY = 0;
            for (iY = 0; iY < nyRanges; ++iY)
                if (y2 > yRangeMin[iY] && y2 < yRangeMax[iY]) break;
            if ((iX >= nxRanges) || (iY >= nyRanges))
            {
                if (fVerbose > 0) cout<<"WARNING: x2 or y2 is out of range, the hit is skipped (x = "<<x2<<", y = "<<y2<<")"<<endl;
                continue;
            }
            Double_t rmsX = 2.0 * xRMS[st2][iX];
            Double_t rmsY = 3.0 * yRMS[st2][iY];//2.0 * yRMS[st2][iY];

            if (Abs(dxEx - dxTh) > rmsX || Abs(dyEx - dyTh) > rmsY) continue;
            for (BmnHit* hit0 : sortedHits[st0]) {
                dxEx = Abs(x1 - hit0->GetX());
                dxTh = Abs(x1 * xMap[st1][1] + xMap[st1][0]);
                dyEx = Abs(y1 - hit0->GetY());
                dyTh = Abs(y1 * yMap[st1][1] + yMap[st1][0]);

                iX = 0;
                for (iX = 0; iX < nxRanges; ++iX)
                    if (x1 > xRangeMin[iX] && x1 < xRangeMax[iX]) break;
                iY = 0;
                for (iY = 0; iY < nyRanges; ++iY)
                    if (y1 > yRangeMin[iY] && y1 < yRangeMax[iY]) break;
                if ((iX >= nxRanges) || (iY >= nyRanges))
                {
                    if (fVerbose > 0) cout<<"WARNING: x1 or y1 is out of range, the hit is skipped (x = "<<x1<<", y = "<<y1<<")"<<endl;
                    continue;
                }
                rmsX = 2.0 * xRMS[st1][iX];
                rmsY = 3.0 * yRMS[st1][iY];//2.0 * yRMS[st1][iY];

                if (Abs(dxEx - dxTh) > rmsX || Abs(dyEx - dyTh) > rmsY) continue;
                BmnTrack cand;
                cand.AddHit(hit2->GetRefIndex(), hit2);
                cand.AddHit(hit1->GetRefIndex(), hit1);
                cand.AddHit(hit0->GetRefIndex(), hit0);
                cand.SortHits();
                CalculateTrackParams(&cand);
                candidates.push_back(cand);
            }
        }
    }
    clock_t t1 = 0;  //clock();
    construct_3of4_time += ((Double_t)(t1 - t0)) / CLOCKS_PER_SEC;

    return kBMNSUCCESS;
}

BmnStatus BmnInnerTrackingRun7::FindTracks_3of4_OnLastGEMStations() {
    vector<BmnHit*> sortedHits[fNStations];
    SortHits(sortedHits);

    vector<BmnTrack> candidates;
    vector<BmnTrack> sortedCandidates;

    FindCandidateByThreeStations(5, 6, 7, candidates, sortedHits);
    FindCandidateByThreeStations(5, 7, 8, candidates, sortedHits);
    FindCandidateByThreeStations(5, 6, 8, candidates, sortedHits);
    FindCandidateByThreeStations(6, 7, 8, candidates, sortedHits);

    //cout << "FindTracks_3of4_OnLastGEMStations::candidates = " << candidates.size() << endl;

    for (BmnTrack& cand : candidates) {
        if (CalculateTrackParams(&cand) == kBMNERROR) continue;
        //First two GEM
        MatchHit(&cand, sortedHits[4], kFALSE);
        MatchHit(&cand, sortedHits[3], kFALSE);
        //Silicon
        MatchHit(&cand, sortedHits[2], kFALSE);
        MatchHit(&cand, sortedHits[1], kFALSE);
        MatchHit(&cand, sortedHits[0], kFALSE);

        // NDF = (N counts in ZX plane + N counts in ZY plane) - 2 parameters of Line in ZY plane - 3 parameters of Circle in ZX plane
        // Check it!!!
        cand.SetNDF(cand.GetNHits() * 2 - 5);
    }

    BmnFieldMap* field = (BmnFieldMap*) FairRunAna::Instance()->GetField();
    Bool_t isField = !(field->IsFieldOff());
    if (isField)
        TrackUpdateByKalman(candidates);
    else
        TrackUpdateByLine(candidates);
    SortTracks(candidates, sortedCandidates);
    TrackSelection(sortedCandidates);

    return kBMNSUCCESS;
}

BmnStatus BmnInnerTrackingRun7::MatchHit(BmnTrack* cand, vector<BmnHit*> sortedHits, Bool_t downstream) {
    clock_t t0 = 0;  //clock();
    //Double_t cutX = 0.45;
    //Double_t cutY = 0.45;
    if (sortedHits.size() == 0) return kBMNERROR;
    FairTrackParam par = downstream ? *(cand->GetParamLast()) : *(cand->GetParamFirst());
    Double_t minDist = DBL_MAX;
    //Double_t minDX = DBL_MAX;
    //Double_t minDY = DBL_MAX;
    BmnHit* minHit = nullptr;
    clock_t t0_0 = 0;  //clock();
    fKalman->TGeoTrackPropagate(&par, sortedHits.at(0)->GetZ(), 2212, nullptr, nullptr);
    clock_t t0_1 = 0;  //clock();
    for (BmnHit* hit : sortedHits) {
        //Double_t dX = Abs(par.GetX() - hit->GetX());
        //Double_t dY = Abs(par.GetY() - hit->GetY());
        Double_t d = Sqrt(Sq(par.GetX() - hit->GetX()) + Sq(par.GetY() - hit->GetY()));
        //if (dX < cutX && dY < cutY && dX < minDX && dY < minDY) {
        if (d < fDistCut && d < minDist) {
            // minDX = dX;
            // minDY = dY;
            minDist = d;
            minHit = hit;
        }
    }
    if (minHit) {
        cand->AddHit(minHit->GetRefIndex(), minHit);
        cand->SortHits();
        Double_t chi2 = 0.0;
        fKalman->Update(&par, minHit, chi2);
        cand->SetChi2(cand->GetChi2() + chi2);
    }
    if (downstream)
        cand->SetParamLast(par);
    else
        cand->SetParamFirst(par);
    clock_t t1 = 0;  //clock();
    hitMatchTime += ((Double_t)(t1 - t0)) / CLOCKS_PER_SEC;
    hitMatchTimeLoop += ((Double_t)(t0_1 - t0_0)) / CLOCKS_PER_SEC;

    return kBMNSUCCESS;
}

BmnStatus BmnInnerTrackingRun7::FindCandidateByTwoStations(Short_t st0, Short_t st1, vector<BmnTrack>& candidates, vector<BmnHit*>* sortedHits) {
    for (BmnHit* hit1 : sortedHits[st1]) {
        Double_t y1 = hit1->GetY();
        Double_t z1 = hit1->GetZ();
        for (BmnHit* hit0 : sortedHits[st0]) {
            Double_t y0 = hit0->GetY();
            Double_t z0 = hit0->GetZ();
            if (Abs(hit1->GetX() - hit0->GetX()) > 40.0 || Abs(y1 - y0) > 20.0) continue;
            BmnTrack cand;
            cand.AddHit(hit1->GetRefIndex(), hit1);
            cand.AddHit(hit0->GetRefIndex(), hit0);
            Double_t a = (y1 - y0) / (z1 - z0);
            Double_t b = (z1 * y0 - y1 * z0) / (z1 - z0);
            Double_t minDist = DBL_MAX;
            BmnHit* minHit = nullptr;
            for (BmnHit* hit : sortedHits[4]) {
                Double_t d = Abs(hit->GetY() - (a * hit->GetZ() + b));
                if (d < minDist && d < 0.5) {
                    minDist = d;
                    minHit = hit;
                }
            }
            if (minHit) {
                cand.AddHit(minHit->GetRefIndex(), minHit);
            }
            minDist = DBL_MAX;
            minHit = nullptr;
            for (BmnHit* hit : sortedHits[3]) {
                Double_t d = Abs(hit->GetY() - (a * hit->GetZ() + b));
                if (d < minDist && d < 0.5) {
                    minDist = d;
                    minHit = hit;
                }
            }
            if (minHit) {
                cand.AddHit(minHit->GetRefIndex(), minHit);
            }
            cand.SortHits();
            candidates.push_back(cand);
        }
    }

    return kBMNSUCCESS;
}

BmnStatus BmnInnerTrackingRun7::FindTracks_2of2_OnFirstGEMStationsDownstream() {
    vector<BmnHit*> sortedHits[fNStations];
    SortHits(sortedHits);
    clock_t t0 = 0;  //clock();

    vector<BmnTrack> candidates;
    vector<BmnTrack> sortedCandidates;
    for (BmnHit* hit1 : sortedHits[4]) {
        Double_t y1 = hit1->GetY();
        Double_t z1 = hit1->GetZ();
        for (BmnHit* hit0 : sortedHits[3]) {
            Double_t y0 = hit0->GetY();
            Double_t z0 = hit0->GetZ();
            if (Abs(hit1->GetX() - hit0->GetX()) > 40.0 || Abs(y1 - y0) > 10.0) continue;
            BmnTrack cand;
            cand.AddHit(hit1->GetRefIndex(), hit1);
            cand.AddHit(hit0->GetRefIndex(), hit0);
            Double_t a = (y1 - y0) / (z1 - z0);
            Double_t b = (z1 * y0 - y1 * z0) / (z1 - z0);

            Double_t minDist = DBL_MAX;
            BmnHit* minHit = nullptr;
            for (BmnHit* hit : sortedHits[5]) {
                Double_t d = Abs(hit->GetY() - (a * hit->GetZ() + b));
                if (d < minDist && d < fDistCut) {
                    minDist = d;
                    minHit = hit;
                }
            }
            if (minHit) {
                cand.AddHit(minHit->GetRefIndex(), minHit);
                cand.SortHits();

                if (CalculateTrackParams(&cand) == kBMNERROR) continue;
                // NDF = (N counts in ZX plane + N counts in ZY plane) - 2 parameters of Line in ZY plane - 3 parameters of Circle in ZX plane
                // Check it!!!
                cand.SetNDF(cand.GetNHits() * 2 - 5);

                //Last GEMs
                MatchHit(&cand, sortedHits[6], kTRUE);
                MatchHit(&cand, sortedHits[7], kTRUE);
                MatchHit(&cand, sortedHits[8], kTRUE);

                //Silicon
                MatchHit(&cand, sortedHits[2], kFALSE);
                MatchHit(&cand, sortedHits[1], kFALSE);
                MatchHit(&cand, sortedHits[0], kFALSE);

                candidates.push_back(cand);
            } else {
                minDist = DBL_MAX;
                minHit = nullptr;
                for (BmnHit* hit : sortedHits[6]) {
                    Double_t d = Abs(hit->GetY() - (a * hit->GetZ() + b));
                    if (d < minDist && d < fDistCut) {
                        minDist = d;
                        minHit = hit;
                    }
                }
                if (minHit) {
                    cand.AddHit(minHit->GetRefIndex(), minHit);
                    cand.SortHits();

                    if (CalculateTrackParams(&cand) == kBMNERROR) continue;
                    // NDF = (N counts in ZX plane + N counts in ZY plane) - 2 parameters of Line in ZY plane - 3 parameters of Circle in ZX plane
                    // Check it!!!
                    cand.SetNDF(cand.GetNHits() * 2 - 5);

                    //Last GEMs
                    MatchHit(&cand, sortedHits[7], kTRUE);
                    MatchHit(&cand, sortedHits[8], kTRUE);

                    //Silicon
                    MatchHit(&cand, sortedHits[2], kFALSE);
                    MatchHit(&cand, sortedHits[1], kFALSE);
                    MatchHit(&cand, sortedHits[0], kFALSE);

                    candidates.push_back(cand);
                } else {
                    minDist = DBL_MAX;
                    minHit = nullptr;
                    for (BmnHit* hit : sortedHits[7]) {
                        Double_t d = Abs(hit->GetY() - (a * hit->GetZ() + b));
                        if (d < minDist && d < fDistCut) {
                            minDist = d;
                            minHit = hit;
                        }
                    }
                    if (minHit) {
                        cand.AddHit(minHit->GetRefIndex(), minHit);
                        cand.SortHits();

                        if (CalculateTrackParams(&cand) == kBMNERROR) continue;
                        // NDF = (N counts in ZX plane + N counts in ZY plane) - 2 parameters of Line in ZY plane - 3 parameters of Circle in ZX plane
                        // Check it!!!
                        cand.SetNDF(cand.GetNHits() * 2 - 5);

                        //Last GEMs
                        MatchHit(&cand, sortedHits[8], kTRUE);

                        //Silicon
                        MatchHit(&cand, sortedHits[2], kFALSE);
                        MatchHit(&cand, sortedHits[1], kFALSE);
                        MatchHit(&cand, sortedHits[0], kFALSE);

                        candidates.push_back(cand);
                    } else {
                        minDist = DBL_MAX;
                        minHit = nullptr;
                        for (BmnHit* hit : sortedHits[8]) {
                            Double_t d = Abs(hit->GetY() - (a * hit->GetZ() + b));
                            if (d < minDist && d < fDistCut) {
                                minDist = d;
                                minHit = hit;
                            }
                        }
                        if (minHit) {
                            cand.AddHit(minHit->GetRefIndex(), minHit);
                            cand.SortHits();

                            if (CalculateTrackParams(&cand) == kBMNERROR) continue;
                            // NDF = (N counts in ZX plane + N counts in ZY plane) - 2 parameters of Line in ZY plane - 3 parameters of Circle in ZX plane
                            // Check it!!!
                            cand.SetNDF(cand.GetNHits() * 2 - 5);

                            //Silicon
                            MatchHit(&cand, sortedHits[2], kFALSE);
                            MatchHit(&cand, sortedHits[1], kFALSE);
                            MatchHit(&cand, sortedHits[0], kFALSE);

                            candidates.push_back(cand);
                        } else {
                            minDist = DBL_MAX;
                            minHit = nullptr;
                            for (BmnHit* hit : sortedHits[2]) {
                                Double_t d = Abs(hit->GetY() - (a * hit->GetZ() + b));
                                if (d < minDist && d < fDistCut) {
                                    minDist = d;
                                    minHit = hit;
                                }
                            }
                            if (minHit) {
                                cand.AddHit(minHit->GetRefIndex(), minHit);
                                cand.SortHits();

                                if (CalculateTrackParams(&cand) == kBMNERROR) continue;
                                // NDF = (N counts in ZX plane + N counts in ZY plane) - 2 parameters of Line in ZY plane - 3 parameters of Circle in ZX plane
                                // Check it!!!
                                cand.SetNDF(cand.GetNHits() * 2 - 5);

                                //Silicon
                                MatchHit(&cand, sortedHits[1], kFALSE);
                                MatchHit(&cand, sortedHits[0], kFALSE);

                                candidates.push_back(cand);
                            } else {
                                minDist = DBL_MAX;
                                minHit = nullptr;
                                for (BmnHit* hit : sortedHits[1]) {
                                    Double_t d = Abs(hit->GetY() - (a * hit->GetZ() + b));
                                    if (d < minDist && d < fDistCut) {
                                        minDist = d;
                                        minHit = hit;
                                    }
                                }
                                if (minHit) {
                                    cand.AddHit(minHit->GetRefIndex(), minHit);
                                    cand.SortHits();

                                    if (CalculateTrackParams(&cand) == kBMNERROR) continue;
                                    // NDF = (N counts in ZX plane + N counts in ZY plane) - 2 parameters of Line in ZY plane - 3 parameters of Circle in ZX plane
                                    // Check it!!!
                                    cand.SetNDF(cand.GetNHits() * 2 - 5);

                                    //Silicon
                                    MatchHit(&cand, sortedHits[0], kFALSE);

                                    candidates.push_back(cand);
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    clock_t t1 = 0;  //clock();
    construct_2of4_time += ((Double_t)(t1 - t0)) / CLOCKS_PER_SEC;
    //cout << "FindTracks_2of2_OnFirstGEMstStations::candidates = " << candidates.size() << endl;
    
    BmnFieldMap* field = (BmnFieldMap*)FairRunAna::Instance()->GetField();
    Bool_t isField = !(field->IsFieldOff());
    if (isField)
        TrackUpdateByKalman(candidates);
    else
        TrackUpdateByLine(candidates);
    SortTracks(candidates, sortedCandidates);
    TrackSelection(sortedCandidates);

    return kBMNSUCCESS;
}

BmnStatus BmnInnerTrackingRun7::FindTracks_2of2_OnFirstGEMStationsUpstream() {
    vector<BmnHit*> sortedHits[fNStations];
    SortHits(sortedHits);
    clock_t t0 = 0;  //clock();
    vector<BmnTrack> candidates;
    vector<BmnTrack> sortedCandidates;
    for (BmnHit* hit1 : sortedHits[4]) {
        Double_t y1 = hit1->GetY();
        Double_t z1 = hit1->GetZ();
        for (BmnHit* hit0 : sortedHits[3]) {
            Double_t y0 = hit0->GetY();
            Double_t z0 = hit0->GetZ();
            if (Abs(hit1->GetX() - hit0->GetX()) > 40.0 || Abs(y1 - y0) > 10.0) continue;
            BmnTrack cand;
            cand.AddHit(hit1->GetRefIndex(), hit1);
            cand.AddHit(hit0->GetRefIndex(), hit0);
            Double_t a = (y1 - y0) / (z1 - z0);
            Double_t b = (z1 * y0 - y1 * z0) / (z1 - z0);
            Double_t minDist = DBL_MAX;
            BmnHit* minHit = nullptr;
            for (BmnHit* hit : sortedHits[2]) {
                Double_t d = Abs(hit->GetY() - (a * hit->GetZ() + b));
                if (d < minDist && d < fDistCut) {
                    minDist = d;
                    minHit = hit;
                }
            }
            if (minHit)
                cand.AddHit(minHit->GetRefIndex(), minHit);
            else {
                minDist = DBL_MAX;
                minHit = nullptr;
                for (BmnHit* hit : sortedHits[1]) {
                    Double_t d = Abs(hit->GetY() - (a * hit->GetZ() + b));
                    if (d < minDist && d < fDistCut) {
                        minDist = d;
                        minHit = hit;
                    }
                }
                if (minHit)
                    cand.AddHit(minHit->GetRefIndex(), minHit);
            }
            cand.SortHits();
            candidates.push_back(cand);
        }
    }
    clock_t t1 = 0;  //clock();
    construct_2of4_time += ((Double_t)(t1 - t0)) / CLOCKS_PER_SEC;
    //cout << "FindTracks_2of2_OnFirstGEMstStations::candidates = " << candidates.size() << endl;

    for (BmnTrack& cand : candidates) {
        if (CalculateTrackParams(&cand) == kBMNERROR) continue;

        // //Last GEMs
        MatchHit(&cand, sortedHits[5], kTRUE);
        MatchHit(&cand, sortedHits[6], kTRUE);
        MatchHit(&cand, sortedHits[7], kTRUE);
        MatchHit(&cand, sortedHits[8], kTRUE);

        //Silicon
        MatchHit(&cand, sortedHits[2], kFALSE);
        MatchHit(&cand, sortedHits[1], kFALSE);
        MatchHit(&cand, sortedHits[0], kFALSE);

        // NDF = (N counts in ZX plane + N counts in ZY plane) - 2 parameters of Line in ZY plane - 3 parameters of Circle in ZX plane
        // Check it!!!
        cand.SetNDF(cand.GetNHits() * 2 - 5);
    }

    BmnFieldMap* field = (BmnFieldMap*)FairRunAna::Instance()->GetField();
    Bool_t isField = !(field->IsFieldOff());
    if (isField)
        TrackUpdateByKalman(candidates);
    else
        TrackUpdateByLine(candidates);
    SortTracks(candidates, sortedCandidates);
    TrackSelection(sortedCandidates);

    return kBMNSUCCESS;
}

BmnStatus BmnInnerTrackingRun7::SortTracks(vector<BmnTrack>& inTracks, vector<BmnTrack>& sortedTracks) {
    clock_t t0 = 0;                              //clock();
    const Int_t n = fNStations - fNHitsCut + 1;  //6 for geometry 2018 (4, 5, 6, 7, 8, 9)
    multimap<Float_t, Int_t> sortedMap[n];       // array of map<Chi2,trIdx>. Each element of array corresponds fixed number of hits on track (4, 5, 6)
    for (Int_t iTr = 0; iTr < inTracks.size(); ++iTr) {
        if (inTracks.at(iTr).GetNHits() < fNHitsCut || inTracks.at(iTr).GetNHits() > fNStations) continue;
        if (inTracks.at(iTr).GetChi2() / inTracks.at(iTr).GetNDF() > fChiSquareCut) continue;
        sortedMap[inTracks.at(iTr).GetNHits() - fNHitsCut].insert(pair<Float_t, Int_t>(inTracks.at(iTr).GetChi2() / inTracks.at(iTr).GetNDF(), iTr));
    }

    for (Int_t i = n - 1; i >= 0; i--) {
        for (auto it : sortedMap[i])
            sortedTracks.push_back(inTracks.at(it.second));
        sortedMap[i].clear();
    }

    //    multimap <Float_t, Int_t> sortedTracksMap; //map<Chi2,trIdx>
    //    for (Int_t iTr = 0; iTr < inTracks.size(); ++iTr)
    //        sortedTracksMap.insert(pair<Float_t, Int_t>(inTracks.at(iTr).GetChi2() / inTracks.at(iTr).GetNDF(), iTr));
    //
    //    for (auto it : sortedTracksMap)
    //        sortedTracks.push_back(inTracks.at(it.second));
    clock_t t1 = 0;  //clock();
    sortTrackTime += ((Double_t)(t1 - t0)) / CLOCKS_PER_SEC;
    return kBMNSUCCESS;
}

void BmnInnerTrackingRun7::Finish() {
    ofstream outFile;
    outFile.open("QA/timing.txt");
    outFile << "Track Finder Time: " << workTime << endl;
    if (fVerbose > 0) {
        cout << "hitMatchTime: " << hitMatchTime << endl;
        cout << "hitMatchTimeLoop: " << hitMatchTimeLoop << endl;
        cout << "construct_4of4_time: " << construct_4of4_time << endl;
        cout << "construct_3of4_time: " << construct_3of4_time << endl;
        cout << "construct_2of4_time: " << construct_2of4_time << endl;
        cout << "CalculateTrackParTime: " << CalculateTrackParTime << endl;
        cout << "SortHitsTime: " << SortHitsTime << endl;
        cout << "trackUpdateTime: " << trackUpdateTime << endl;
        cout << "sortTrackTime: " << sortTrackTime << endl;
        cout << "trackSelectionTime: " << trackSelectionTime << endl;
    }
    cout << "Work time of the GEM tracking: " << workTime << endl;
    return;
}

BmnStatus BmnInnerTrackingRun7::SortHits(vector<BmnHit*>* sortedHits) {
    clock_t t0 = 0;  //clock();
    for (Int_t iHit = 0; iHit < fHitsArray->GetEntriesFast(); ++iHit) {
        BmnHit* hit = (BmnHit*)fHitsArray->At(iHit);
        if (!hit) continue;
        if (hit->IsUsed()) continue;
        hit->SetRefIndex(iHit);
        Int_t station = hit->GetStation();
        //if (hit->GetX() > fHitXCutMax[station] || hit->GetX() < fHitXCutMin[station]) continue;
        if (hit->GetY() > fHitYCutMax[station] || hit->GetY() < fHitYCutMin[station]) continue;
        sortedHits[station].push_back(hit);
    }
    clock_t t1 = 0;  //clock();
    SortHitsTime += ((Double_t)(t1 - t0)) / CLOCKS_PER_SEC;

    return kBMNSUCCESS;
}

BmnStatus BmnInnerTrackingRun7::TrackUpdateByLine(vector<BmnTrack>& cands) {
    for (BmnTrack& cand : cands) {
        Double_t Tx = LineFit((BmnTrack*)&cand, fHitsArray, "ZX").X();
        Double_t chiX = LineFit((BmnTrack*)&cand, fHitsArray, "ZX").Z();
        Double_t Ty = LineFit((BmnTrack*)&cand, fHitsArray, "ZY").X();
        Double_t chiY = LineFit((BmnTrack*)&cand, fHitsArray, "ZY").Z();

        cand.SetChi2((chiX - chiY) > 0. ? chiX : chiY);

        cand.GetParamFirst()->SetTx(Tx);
        cand.GetParamFirst()->SetTy(Ty);

        cand.GetParamLast()->SetTx(Tx);
        cand.GetParamLast()->SetTy(Ty);
    }

    return kBMNSUCCESS;
}

BmnStatus BmnInnerTrackingRun7::TrackUpdateByKalman(vector<BmnTrack>& cands) {
    clock_t t0 = 0;  //clock();
    for (BmnTrack& cand : cands) {
        if (cand.GetNHits() < fNHitsCut) continue;
        FairTrackParam par = *(cand.GetParamFirst());
        //par.SetQp(1.0/8.0);
        Double_t chiTot = 0.0;
        for (Int_t iHit = 0; iHit < cand.GetNHits(); ++iHit) {
            BmnHit* hit = (BmnHit*)fHitsArray->At(cand.GetHitIndex(iHit));
            Double_t chi = 0.0;
            fKalman->TGeoTrackPropagate(&par, hit->GetZ(), 2212, nullptr, nullptr);
            fKalman->Update(&par, hit, chi);
        }
        cand.SetParamLast(par);
        for (Int_t iHit = cand.GetNHits() - 1; iHit >= 0; iHit--) {
            BmnHit* hit = (BmnHit*)fHitsArray->At(cand.GetHitIndex(iHit));
            Double_t chi = 0.0;
            fKalman->TGeoTrackPropagate(&par, hit->GetZ(), 2212, nullptr, nullptr);
            fKalman->Update(&par, hit, chi);
            chiTot += chi;
        }
        cand.SetParamFirst(par);
        cand.SetChi2(chiTot);
    }
    clock_t t1 = 0;  //clock();
    trackUpdateTime += ((Double_t)(t1 - t0)) / CLOCKS_PER_SEC;

    return kBMNSUCCESS;
}

BmnStatus BmnInnerTrackingRun7::TrackSelection(vector<BmnTrack>& sortedTracks) {
    clock_t t0 = 0;  //clock();

    //Switch on/off station skip in tracks

    // for (Int_t iTr = 0; iTr < sortedTracks.size(); ++iTr) {
    //     BmnTrack* tr = &(sortedTracks[iTr]);
    //     if (tr->GetFlag() == 666 || !IsParCorrect(tr->GetParamFirst()) || !IsParCorrect(tr->GetParamLast())) continue;
    //     Bool_t badTrack = kFALSE;
    //     BmnHit* hit0 = (BmnHit*)fHitsArray->At(tr->GetHitIndex(0));
    //     Int_t stPrev = hit0->GetStation();
    //     for (Int_t iHit = 1; iHit < tr->GetNHits(); ++iHit) {
    //         BmnHit* hit = (BmnHit*)fHitsArray->At(tr->GetHitIndex(iHit));
    //         Int_t st = hit->GetStation();
    //         Int_t dSt = st - stPrev - 1;
    //         stPrev = st;
    //         if (dSt == 1) { //number of skipped stations in track
    //             badTrack = kTRUE;
    //             break;
    //         }
    //     }
    //     if (badTrack) tr->SetFlag(666);
    // }
    CheckSharedHits(sortedTracks);
    for (Int_t iTr = 0; iTr < sortedTracks.size(); ++iTr) {
        BmnTrack tr = sortedTracks[iTr];
        if (tr.GetFlag() == 666 || !IsParCorrect(tr.GetParamFirst()) || !IsParCorrect(tr.GetParamLast())) continue;
        BmnTrack gemTr;
        BmnTrack silTr;
        BmnGlobalTrack globTr;

        for (Int_t iHit = 0; iHit < tr.GetNHits(); ++iHit) {
            BmnHit* hit = (BmnHit*)fHitsArray->At(tr.GetHitIndex(iHit));
            globTr.AddHit(hit->GetRefIndex(), hit);
            DetectorId detId = hit->GetDetId();
            if (detId == kSILICON)
                silTr.AddHit(hit->GetIndex(), hit);
            else if (detId == kGEM)
                gemTr.AddHit(hit->GetIndex(), hit);
            else
                continue;
        }
        globTr.SortHits();

        silTr.SortHits();
        gemTr.SortHits();

        if (silTr.GetNHits() > 0) {
            new ((*fSilTracksArray)[fSilTracksArray->GetEntriesFast()]) BmnTrack(silTr);
            globTr.SetSilTrackIndex(fSilTracksArray->GetEntriesFast() - 1);
        }
        if (gemTr.GetNHits() > 0) {
            gemTr.SetParamFirst(*(tr.GetParamFirst()));
            gemTr.SetParamLast(*(tr.GetParamLast()));
            new ((*fGemTracksArray)[fGemTracksArray->GetEntriesFast()]) BmnTrack(gemTr);
            globTr.SetGemTrackIndex(fGemTracksArray->GetEntriesFast() - 1);
        }
        globTr.SetLength(CalculateLength(&tr));
        globTr.SetParamFirst(*(tr.GetParamFirst()));
        globTr.SetParamLast(*(tr.GetParamLast()));
        globTr.SetNHits(tr.GetNHits());
        globTr.SetNDF(tr.GetNDF());
        globTr.SetChi2(tr.GetChi2());
        new ((*fGlobTracksArray)[fGlobTracksArray->GetEntriesFast()]) BmnGlobalTrack(globTr);
        SetHitsUsing(&tr, kTRUE);
        if (!fIsTarget && iTr == 0) break;
    }
    clock_t t1 = 0;  //clock();
    trackSelectionTime += ((Double_t)(t1 - t0)) / CLOCKS_PER_SEC;

    return kBMNSUCCESS;
}

void BmnInnerTrackingRun7::SetHitsUsing(BmnTrack* tr, Bool_t use) {
    for (Int_t i = 0; i < tr->GetNHits(); ++i) {
        BmnHit* hit = (BmnHit*)fHitsArray->At(tr->GetHitIndex(i));
        if (!hit) continue;
        hit->SetUsing(use);
        // for (Int_t iLink = 0; iLink < hit->GetDigitNumberMatch().GetNofLinks(); ++iLink) {
        //     BmnLink link = hit->GetDigitNumberMatch().GetLink(iLink);
        //     Int_t idxI = link.GetIndex();
        //     for (Int_t j = 0; j < fHitsArray->GetEntriesFast(); ++j) {
        //         BmnHit* hitJ = (BmnHit*)fHitsArray->At(j);
        //         if (!hitJ) continue;
        //         if (hitJ->IsUsed()) continue;
        //         for (Int_t iLink = 0; iLink < hit->GetDigitNumberMatch().GetNofLinks(); ++iLink) {
        //             BmnLink linkJ = hit->GetDigitNumberMatch().GetLink(iLink);
        //             Int_t idxJ = linkJ.GetIndex();
        //             if (idxI == idxJ) {
        //                 hitJ->SetUsing(use);
        //                 break;
        //             }
        //         }
        //     }
        // }
    }
}

BmnStatus BmnInnerTrackingRun7::CalcCovMatrix(BmnTrack* tr) {
    const UInt_t nHits = tr->GetNHits();
    Double_t chi2circ = 0.0;
    TVector3 CircParZX = CircleFit(tr, fHitsArray, chi2circ);

    Double_t Xc = CircParZX.Y();  // x-coordinate of fit-circle center
    Double_t Zc = CircParZX.X();  // z-coordinate of fit-circle center
    fField = FairRunAna::Instance()->GetField();
    const Double_t B = (LineFit(tr, fHitsArray, "ZY")).X();  //angle coefficient for helicoid

    Double_t Q = (Xc > 0) ? +1 : -1;

    Double_t sumX(0.0), sumY(0.0), sumTx(0.0), sumTy(0.0), sumQP(0.0);
    Double_t sumXX(0.0), sumXY(0.0), sumXTx(0.0), sumXTy(0.0), sumXQP(0.0);
    Double_t sumYY(0.0), sumYTx(0.0), sumYTy(0.0), sumYQP(0.0);
    Double_t sumTxTx(0.0), sumTxTy(0.0), sumTxQP(0.0);
    Double_t sumTyTy(0.0), sumTyQP(0.0);
    Double_t sumQPQP(0.0);

    for (UInt_t i = 0; i < nHits; ++i) {
        BmnHit* hit = (BmnHit*)fHitsArray->At(tr->GetHitIndex(i));
        if (!hit) continue;
        Double_t Xi = hit->GetX();
        Double_t Yi = hit->GetY();
        Double_t Zi = hit->GetZ();

        Double_t Txi = -1.0 * (Zi - Zc) / (Xi - Xc);
        Double_t Tyi = B;
        Double_t Ri = Sqrt(Sq(Xi - Xc) + Sq(Zi - Zc));
        Double_t QPi = Q / (0.0003 * Abs(fField->GetBy(Xi, Yi, Zi)) * Ri);

        sumX += Xi;
        sumY += Yi;
        sumTx += Txi;
        sumTy += Tyi;
        sumQP += QPi;
        sumXX += Xi * Xi;
        sumXY += Xi * Yi;
        sumXTx += Xi * Txi;
        sumXTy += Xi * Tyi;
        sumXQP += Xi * QPi;
        sumYY += Yi * Yi;
        sumYTx += Yi * Txi;
        sumYTy += Yi * Tyi;
        sumYQP += Yi * QPi;
        sumTxTx += Txi * Txi;
        sumTxTy += Txi * Tyi;
        sumTxQP += Txi * QPi;
        sumTyTy += Tyi * Tyi;
        sumTyQP += Tyi * QPi;
        sumQPQP += QPi * QPi;
    }

    Double_t meanX = sumX / nHits;
    Double_t meanY = sumY / nHits;
    Double_t meanTx = sumTx / nHits;
    Double_t meanTy = sumTy / nHits;
    Double_t meanQP = sumQP / nHits;

    Double_t Cov_X_X = sumXX / nHits - Sq(meanX);
    Double_t Cov_X_Y = sumXY / nHits - meanX * meanY;
    Double_t Cov_X_Tx = sumXTx / nHits - meanX * meanTx;
    Double_t Cov_X_Ty = sumXTy / nHits - meanX * meanTy;
    Double_t Cov_X_Qp = sumXQP / nHits - meanX * meanQP;
    Double_t Cov_Y_Y = sumYY / nHits - Sq(meanY);
    Double_t Cov_Y_Tx = sumYTx / nHits - meanY * meanTx;
    Double_t Cov_Y_Ty = sumYTy / nHits - meanY * meanTy;
    Double_t Cov_Y_Qp = sumYQP / nHits - meanY * meanQP;
    Double_t Cov_Tx_Tx = sumTxTx / nHits - Sq(meanTx);
    Double_t Cov_Tx_Ty = sumTxTy / nHits - meanTx * meanTy;
    Double_t Cov_Tx_Qp = sumTxQP / nHits - meanTx * meanQP;
    Double_t Cov_Ty_Ty = sumTyTy / nHits - Sq(meanTy);
    Double_t Cov_Ty_Qp = sumTyQP / nHits - meanTy * meanQP;
    Double_t Cov_Qp_Qp = sumQPQP / nHits - Sq(meanQP);

    FairTrackParam par;
    par.SetCovariance(0, 0, Cov_X_X);
    par.SetCovariance(0, 1, Cov_X_Y);
    par.SetCovariance(0, 2, Cov_X_Tx);
    par.SetCovariance(0, 3, Cov_X_Ty);
    par.SetCovariance(0, 4, Cov_X_Qp);
    par.SetCovariance(1, 1, Cov_Y_Y);
    par.SetCovariance(1, 2, Cov_Y_Tx);
    par.SetCovariance(1, 3, Cov_Y_Ty);
    par.SetCovariance(1, 4, Cov_Y_Qp);
    par.SetCovariance(2, 2, Cov_Tx_Tx);
    par.SetCovariance(2, 3, Cov_Tx_Ty);
    par.SetCovariance(2, 4, Cov_Tx_Qp);
    par.SetCovariance(3, 3, Cov_Ty_Ty);
    par.SetCovariance(3, 4, Cov_Ty_Qp);
    par.SetCovariance(4, 4, Cov_Qp_Qp);

    tr->SetParamFirst(par);
    tr->SetParamLast(par);

    return kBMNSUCCESS;
}

BmnStatus BmnInnerTrackingRun7::CalculateTrackParams(BmnTrack* tr) {
    
    BmnFieldMap* field = (BmnFieldMap*)FairRunAna::Instance()->GetField();
    Bool_t isField = !(field->IsFieldOff());
    clock_t t0 = 0;  //clock();
    //Estimation of track parameters for events with magnetic field
    const UInt_t nHits = tr->GetNHits();
    if (nHits < 3) return kBMNERROR;
    TVector3 lineParZY = LineFit(tr, fHitsArray, "ZY");
    if (lineParZY.Z() > 1) return kBMNERROR;  //cout << "chi2_lineFit = " << lineParZY.Z() << endl;
    //tr->SetNDF(nHits - (isField ? 3 : 2));
    const Double_t B = lineParZY.X();  //angle coefficient for helicoid

    Double_t Tx_first = CalcTx((BmnHit*)fHitsArray->At(tr->GetHitIndex(0)), (BmnHit*)fHitsArray->At(tr->GetHitIndex(1)), (BmnHit*)fHitsArray->At(tr->GetHitIndex(2)));
    Double_t Tx_last = CalcTx((BmnHit*)fHitsArray->At(tr->GetHitIndex(nHits - 1)), (BmnHit*)fHitsArray->At(tr->GetHitIndex(nHits - 2)), (BmnHit*)fHitsArray->At(tr->GetHitIndex(nHits - 3)));

    if (isField) CalcCovMatrix(tr);
    TVector3 firstPos;
    TVector3 lastPos;
    ((BmnHit*)fHitsArray->At(tr->GetHitIndex(0)))->Position(firstPos);
    ((BmnHit*)fHitsArray->At(tr->GetHitIndex(nHits - 1)))->Position(lastPos);
    tr->GetParamFirst()->SetPosition(firstPos);
    tr->GetParamFirst()->SetTx(Tx_first);
    tr->GetParamFirst()->SetTy(B);
    tr->GetParamLast()->SetPosition(lastPos);
    tr->GetParamLast()->SetTx(Tx_last);
    tr->GetParamLast()->SetTy(B);
    Bool_t doSimple = (nHits == 3) ? kTRUE : kFALSE;
    Double_t QP = isField ? CalcQp(tr, doSimple) : 0.0;
    tr->GetParamFirst()->SetQp(QP);
    tr->GetParamLast()->SetQp(QP);
    clock_t t1 = 0;  //clock();
    CalculateTrackParTime += ((Double_t)(t1 - t0)) / CLOCKS_PER_SEC;
    return kBMNSUCCESS;
}

TVector2 BmnInnerTrackingRun7::CalcMeanSigma(vector<Double_t> QpSegm) {
    Double_t QpSum = 0.;
    for (Int_t iSegm = 0; iSegm < QpSegm.size(); iSegm++)
        QpSum += QpSegm[iSegm];

    Double_t QpMean = QpSum / QpSegm.size();

    Double_t sqSigmaSum = 0.;
    for (Int_t iSegm = 0; iSegm < QpSegm.size(); iSegm++)
        sqSigmaSum += Sq(QpSegm[iSegm] - QpMean);

    return TVector2(QpMean, Sqrt(sqSigmaSum / QpSegm.size()));
}

Double_t BmnInnerTrackingRun7::CalcQp(BmnTrack* track, Bool_t doSimple) {
    //Bool_t fRobustRefit = kTRUE;
    //Bool_t fSimpleRefit = kFALSE;
    vector<BmnHit*> hits;

    for (Int_t iHit = 0; iHit < track->GetNHits(); iHit++)
        hits.push_back((BmnHit*)fHitsArray->At(track->GetHitIndex(iHit)));

    Int_t kNSegm = track->GetNHits() - 2;

    Double_t QpRefit = 0.;
    vector<Double_t> QpSegmBefore;

    // Get q/p info from all track segments
    for (Int_t iHit = 0; iHit < kNSegm; iHit++) {
        BmnHit* first = hits[iHit];
        BmnHit* second = hits[iHit + 1];
        BmnHit* third = hits[iHit + 2];

        TVector3 CircParZX = CircleBy3Hit(first, second, third);
        Double_t R = CircParZX.Z();
        Double_t Xc = CircParZX.Y();
        Double_t Zc = CircParZX.X();

        Double_t Q = (Xc > 0) ? +1. : -1.;
        Double_t S = 0.0003 * (Abs(fField->GetBy(third->GetX(), third->GetY(), third->GetZ())) + Abs(fField->GetBy(second->GetX(), second->GetY(), second->GetZ())) + Abs(fField->GetBy(first->GetX(), first->GetY(), first->GetZ()))) / 3.;

        Double_t Pt = S * R;  //actually Pt/Q, but it doesn't matter
        Double_t fX = first->GetX();
        Double_t fZ = first->GetZ();

        Double_t h = -1.0;

        Double_t Tx_first = h * (fZ - Zc) / (fX - Xc);
        TVector3 lineParZY = LineFit(track, fHitsArray, "ZY");
        const Double_t B = lineParZY.X();  //angle coefficient for helicoid
        Double_t Ty_first = B;             // / (fX - Xc);

        Double_t Pz = Pt / Sqrt(1 + Sq(Tx_first));
        Double_t Px = Tx_first * Pz;
        Double_t Py = Ty_first * Pz;
        Double_t P = Sqrt(Sq(Px) + Sq(Py) + Sq(Pz));
        Double_t QP = Q / P;

        QpSegmBefore.push_back(QP);
    }

    // Non-robust (simple) refit when segments with bad q/p are not taken into account
    if (doSimple) {
        vector<Double_t> QpSegmAfter;
        while (kTRUE) {
            TVector2 meanSig = CalcMeanSigma(QpSegmBefore);
            Double_t mean = meanSig.X();
            Double_t sigma = meanSig.Y();
            if (std::isnan(sigma) && fVerbose == 3) {
                cout << "Bad refit convergence for track segment!!" << endl;
                return kBMNERROR;
            }

            for (Int_t iSegm = 0; iSegm < QpSegmBefore.size(); iSegm++)
                if (Abs(QpSegmBefore[iSegm] - mean) - sigma <= 0.001)  // Топорное сравнение FIXME
                    QpSegmAfter.push_back(QpSegmBefore[iSegm]);

            if (QpSegmAfter.size() == QpSegmBefore.size()) {
                QpRefit = mean;
                break;
            } else {
                QpSegmBefore.clear();
                QpSegmBefore.resize(0);

                for (Int_t iSegm = 0; iSegm < QpSegmAfter.size(); iSegm++)
                    QpSegmBefore.push_back(QpSegmAfter[iSegm]);

                QpSegmAfter.clear();
                QpSegmAfter.resize(0);
            }
        }
    }

    // Robust refit with use of Tukey weights calculation algorithm
    // if (fRobustRefit) {
    else {
        for (Int_t iEle = 0; iEle < QpSegmBefore.size(); iEle++)
            QpRefit += QpSegmBefore[iEle];

        QpRefit /= QpSegmBefore.size();

        vector<Double_t> d = dist(QpSegmBefore, QpRefit);

        Double_t sigma = 0.;
        for (Int_t i = 0; i < QpSegmBefore.size(); i++)
            sigma += (QpSegmBefore[i] - QpRefit) * (QpSegmBefore[i] - QpRefit);
        sigma = Sqrt(sigma / QpSegmBefore.size());

        vector<Double_t> w = W(d, sigma);
        sigma = Sigma(d, w);

        const Int_t kNIter = 20;  // FIXME
        for (Int_t iIter = 1; iIter < kNIter; iIter++) {
            QpRefit = Mu(QpSegmBefore, w);
            d = dist(QpSegmBefore, QpRefit);
            w = W(d, sigma);
            sigma = Sigma(d, w);
        }
    }

    return QpRefit;
}

Double_t BmnInnerTrackingRun7::CalculateLength(BmnTrack* tr) {
    if (!tr) return 0.0;

    vector<Double_t> X, Y, Z;
    for (Int_t iGem = 0; iGem < tr->GetNHits(); iGem++) {
        BmnHit* hit = (BmnHit*)fHitsArray->At(tr->GetHitIndex(iGem));
        if (!hit) continue;
        X.push_back(hit->GetX());
        Y.push_back(hit->GetY());
        Z.push_back(hit->GetZ());
    }
    // Calculate distances between hits
    Double_t length = 0.;
    for (Int_t i = 0; i < X.size() - 1; i++) {
        Double_t dX = X[i] - X[i + 1];
        Double_t dY = Y[i] - Y[i + 1];
        Double_t dZ = Z[i] - Z[i + 1];
        length += Sqrt(dX * dX + dY * dY + dZ * dZ);
    }
    tr->SetLength(length);
    return length;
}

BmnStatus BmnInnerTrackingRun7::CheckSharedHits(vector<BmnTrack>& sortedTracks) {
    set<Int_t> hitsId;

    const Int_t kNSharedHits = 1;  //fSteering->GetNSharedHits();

    for (Int_t iTr = 0; iTr < sortedTracks.size(); ++iTr) {
        BmnTrack* tr = &(sortedTracks.at(iTr));
        if (tr->GetFlag() == 666) continue;

        Int_t nofSharedHits = 0;
        Int_t nofHits = tr->GetNHits();
        for (Int_t iHit = 0; iHit < nofHits; iHit++)
            if (hitsId.find(tr->GetHitIndex(iHit)) != hitsId.end()) {
                nofSharedHits++;
                if (nofSharedHits > kNSharedHits) {
                    tr->SetFlag(666);
                    break;
                }
            }
        if (tr->GetFlag() == 666) continue;

        for (Int_t iHit = 0; iHit < nofHits; iHit++)
            hitsId.insert(tr->GetHitIndex(iHit));
    }
    hitsId.clear();

    return kBMNSUCCESS;
}

BmnStatus BmnInnerTrackingRun7::DrawHits() {
    TH2F* h_HitsZX = new TH2F("h_HitsZX", "h_HitsZX", 400, 0.0, 200.0, 400, -100.0, 100.0);
    TH2F* h_HitsZY = new TH2F("h_HitsZY", "h_HitsZY", 400, 0.0, 200.0, 400, -10.0, 50.0);
    for (Int_t i = 0; i < fHitsArray->GetEntriesFast(); ++i) {
        BmnHit* hit = (BmnHit*)fHitsArray->At(i);
        h_HitsZX->Fill(hit->GetZ(), hit->GetX());
        h_HitsZY->Fill(hit->GetZ(), hit->GetY());
    }

    TCanvas* c = new TCanvas("c", "c", 1000, 1000);
    c->Divide(1, 2);
    c->cd(1);
    h_HitsZX->SetMarkerStyle(8);
    h_HitsZX->SetMarkerSize(0.7);
    h_HitsZX->SetMarkerColor(kRed);
    h_HitsZX->Draw("P");

    c->cd(2);
    h_HitsZY->SetMarkerStyle(8);
    h_HitsZY->SetMarkerSize(0.7);
    h_HitsZY->SetMarkerColor(kRed);
    h_HitsZY->Draw("P");

    //if (fGlobTracksArray->GetEntriesFast() <= 0) return kBMNERROR;

    for (Int_t iTr = 0; iTr < fGlobTracksArray->GetEntriesFast(); ++iTr) {
        BmnGlobalTrack* glTrack = (BmnGlobalTrack*)fGlobTracksArray->At(iTr);

        BmnTrack* gemTr = nullptr;
        BmnTrack* silTr = nullptr;

        Double_t xPrev;
        Double_t yPrev;
        Double_t zPrev;

        if (glTrack->GetSilTrackIndex() != -1) {
            silTr = (BmnTrack*)fSilTracksArray->UncheckedAt(glTrack->GetSilTrackIndex());

            xPrev = ((BmnHit*)fSilHitsArray->At(silTr->GetHitIndex(0)))->GetX();
            yPrev = ((BmnHit*)fSilHitsArray->At(silTr->GetHitIndex(0)))->GetY();
            zPrev = ((BmnHit*)fSilHitsArray->At(silTr->GetHitIndex(0)))->GetZ();

            for (Int_t iHit = 1; iHit < silTr->GetNHits(); iHit++) {
                BmnHit* hit = (BmnHit*)fSilHitsArray->UncheckedAt(silTr->GetHitIndex(iHit));
                Double_t x = hit->GetX();
                Double_t y = hit->GetY();
                Double_t z = hit->GetZ();
                TLine* lineZX = new TLine(z, x, zPrev, xPrev);
                lineZX->SetLineColor(kRed);
                lineZX->SetLineWidth(2);
                TLine* lineZY = new TLine(z, y, zPrev, yPrev);
                lineZY->SetLineColor(kRed);
                lineZY->SetLineWidth(2);
                c->cd(1);
                lineZX->Draw("same");
                c->cd(2);
                lineZY->Draw("same");
                zPrev = z;
                yPrev = y;
                xPrev = x;
            }
        }

        if (glTrack->GetGemTrackIndex() != -1) {
            gemTr = (BmnTrack*)fGemTracksArray->UncheckedAt(glTrack->GetGemTrackIndex());

            Int_t startIdx = 0;

            if (glTrack->GetSilTrackIndex() == -1) {
                xPrev = ((BmnHit*)fGemHitsArray->At(gemTr->GetHitIndex(0)))->GetX();
                yPrev = ((BmnHit*)fGemHitsArray->At(gemTr->GetHitIndex(0)))->GetY();
                zPrev = ((BmnHit*)fGemHitsArray->At(gemTr->GetHitIndex(0)))->GetZ();
                startIdx = 1;
            }

            for (Int_t iHit = startIdx; iHit < gemTr->GetNHits(); iHit++) {
                BmnHit* hit = (BmnHit*)fGemHitsArray->UncheckedAt(gemTr->GetHitIndex(iHit));
                Double_t x = hit->GetX();
                Double_t y = hit->GetY();
                Double_t z = hit->GetZ();
                TLine* lineZX = new TLine(z, x, zPrev, xPrev);
                lineZX->SetLineColor(kRed);
                lineZX->SetLineWidth(2);
                TLine* lineZY = new TLine(z, y, zPrev, yPrev);
                lineZY->SetLineColor(kRed);
                lineZY->SetLineWidth(2);
                c->cd(1);
                lineZX->Draw("same");
                c->cd(2);
                lineZY->Draw("same");
                zPrev = z;
                yPrev = y;
                xPrev = x;
            }
        }
    }

    // Int_t nMCtracks = 0;
    // for (Int_t iTr = 0; iTr < fMCTracksArray->GetEntriesFast(); ++iTr) {
    //     CbmMCTrack* tr = (CbmMCTrack*)fMCTracksArray->At(iTr);
    //     Int_t nPoints = 0;  //tr->GetNPoints(kGEM);
    //     vector<TLine*> vLineZX;
    //     vector<TLine*> vLineZY;
    //     Double_t z0 = tr->GetStartZ();
    //     Double_t x0 = tr->GetStartX();
    //     Double_t y0 = tr->GetStartY();
    //     Double_t x1, y1, z1;
    //     set<Int_t> vSt;
    //     for (Int_t i = 0; i < fSilPointsArray->GetEntriesFast(); ++i) {
    //         FairMCPoint* pnt = (FairMCPoint*)fSilPointsArray->At(i);
    //         if (pnt->GetTrackID() == iTr) {
    //             nPoints++;
    //             z1 = pnt->GetZ();
    //             x1 = pnt->GetX();
    //             y1 = pnt->GetY();
    //             vSt.insert(fSilDetector->GetPointStationOwnership(z1));
    //             vLineZX.push_back(new TLine(z0, x0, z1, x1));
    //             vLineZY.push_back(new TLine(z0, y0, z1, y1));
    //             z0 = z1;
    //             x0 = x1;
    //             y0 = y1;
    //         }
    //     }

    //     for (Int_t i = 0; i < fGemPointsArray->GetEntriesFast(); ++i) {
    //         FairMCPoint* pnt = (FairMCPoint*)fGemPointsArray->At(i);
    //         if (pnt->GetTrackID() == iTr) {
    //             nPoints++;
    //             z1 = pnt->GetZ();
    //             x1 = pnt->GetX();
    //             y1 = pnt->GetY();
    //             vSt.insert(fGemDetector->GetPointStationOwnership(z1) + fNSiliconStations);
    //             vLineZX.push_back(new TLine(z0, x0, z1, x1));
    //             vLineZY.push_back(new TLine(z0, y0, z1, y1));
    //             z0 = z1;
    //             x0 = x1;
    //             y0 = y1;
    //         }
    //     }

    //     if (nPoints > 3 && vSt.size() > 3) {
    //         nMCtracks++;
    //         c->cd(1);
    //         for (TLine* it : vLineZX) {
    //             it->SetLineColor(kBlue);
    //             it->SetLineStyle(9);
    //             it->Draw("same");
    //         }
    //         c->cd(2);
    //         for (TLine* it : vLineZY) {
    //             it->SetLineColor(kBlue);
    //             it->SetLineStyle(9);
    //             it->Draw("same");
    //         }
    //     }
    // }

    // printf("NMCTracks = %d   GlobTracks = %d\n", nMCtracks, fGlobTracksArray->GetEntriesFast());

    //    arc->Draw("same");
    c->SaveAs("hits.png");
    getchar();
    delete h_HitsZX;
    delete h_HitsZY;
    delete c;

    return kBMNSUCCESS;
}
