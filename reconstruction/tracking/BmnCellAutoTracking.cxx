
// This Class' Header ------------------
#include "BmnCellAutoTracking.h"
#include <TCanvas.h>
#include <TH2F.h>
#include <TLine.h>
#include <TMath.h>
#include <map>
#include <vector>
#include "BmnCellDuet.h"
#include "BmnGemStripHit.h"
#include "BmnKalmanFilter.h"
#include "BmnMath.h"
#include "FairRunAna.h"
#include "FairTrackParam.h"
#include "TStyle.h"
#if defined(_OPENMP)
#include "omp.h"
#endif

//-----------------------------------------
static Double_t workTime = 0.0;
static Double_t createTime = 0.0;
static Double_t stateTime = 0.0;
static Double_t connectTime = 0.0;
static Double_t sortTime = 0.0;
static Double_t selectTime = 0.0;
//-----------------------------------------

using namespace std;
using namespace TMath;

BmnCellAutoTracking::BmnCellAutoTracking(Short_t period, Int_t run, Bool_t field, Bool_t target, TString steerFile) : fSteering(nullptr),
                                                                                                                      fSteerFile(steerFile) {
    fInnerTrackerSetup[kGEM] = kTRUE;
    fInnerTrackerSetup[kSILICON] = kTRUE;
    fInnerTrackerSetup[kSSD] = kFALSE;
    // Deciding whether we are getting MC ...
    if (run < 0) {
        // MC-input given
        if (run < -2)
            Fatal("BmnCellAutoTracking()", "Probably, run number has been changed manually in reco macro. Aborting ...");

        // After the first decision made, we are selecting setup used in when preparing the MC-input
        // When processing MC-input
        // -2 means use of the SRC-setup
        // -1 means use of the BM@N-setup
        isSRC = (run == -2) ? kTRUE : kFALSE;
    }
    // Define a setup to be used by comparing with current runID, exp. data input given
    else {
        const Int_t runTransition = 3589;  // FIXME!
        isSRC = (run < runTransition) ? kTRUE : kFALSE;
    }

    TString setup = isSRC ? "SRC" : "BMN";
    if (steerFile == "")
        fSteering = new BmnSteering(field ? TString(setup + "_run7_withField.dat") : TString(setup + "_run7_noField.dat"));  // FIXME (should be got from UniDb)
    else
        fSteering = new BmnSteering(fSteerFile);
    fPeriodId = period;
    fEventNo = 0;
    fIsField = field;
    fIsTarget = target;
    fNSiliconStations = 0;
    fGemHitsArray = nullptr;
    fSilHitsArray = nullptr;
    fSsdHitsArray = nullptr;
    fHitsArray = nullptr;
    fRoughVertex = (fPeriodId == 7) ? TVector3(0.5, -4.6, -2.3) : (fPeriodId == 6) ? TVector3(0.0, -3.5, -21.9) : TVector3(0.0, 0.0, 0.0);
    fKalman = new BmnKalmanFilter();
    fGlobTracksArray = nullptr;
    fGemTracksArray = nullptr;
    fSilTracksArray = nullptr;
    fSsdTracksArray = nullptr;
    fField = nullptr;
    fGemHitsBranchName = "BmnGemStripHit";
    fSilHitsBranchName = "BmnSiliconHit";
    fSsdHitsBranchName = "BmnSSDHit";
    fGlobTracksBranchName = "BmnGlobalTrack";
    fGemTracksBranchName = "BmnGemTrack";
    fSilTracksBranchName = "BmnSiliconTrack";
    fSsdTracksBranchName = "BmnSSDTrack";
    fNStations = fSteering->GetNStations();
    fGemDetector = nullptr;
    fSilDetector = nullptr;
    fCellDistCut = nullptr;
    fCellSlopeXZCutMin = nullptr;
    fCellSlopeXZCutMax = nullptr;
    fCellSlopeYZCutMin = nullptr;
    fCellSlopeYZCutMax = nullptr;
    fHitXCutMin = nullptr;
    fHitXCutMax = nullptr;
    fHitYCutMin = nullptr;
    fHitYCutMax = nullptr;
    fCellDiffSlopeXZCut = nullptr;
    fCellDiffSlopeYZCut = nullptr;
    fNHitsCut = 0.0;
    kCellsCut = 10000000000;  //fSteering->GetNCellsCut();
    if (fVerbose > 1) fSteering->PrintParamTable();
}

BmnCellAutoTracking::~BmnCellAutoTracking() {
    delete fKalman;
    if (fInnerTrackerSetup[kGEM]) delete fGemDetector;
    if (fInnerTrackerSetup[kSILICON]) delete fSilDetector;
    delete fCellDistCut;
    delete fHitXCutMin;
    delete fHitXCutMax;
    delete fHitYCutMin;
    delete fHitYCutMax;
    delete fCellSlopeXZCutMin;
    delete fCellSlopeXZCutMax;
    delete fCellSlopeYZCutMin;
    delete fCellSlopeYZCutMax;
    delete fCellDiffSlopeXZCut;
    delete fCellDiffSlopeYZCut;
}

InitStatus BmnCellAutoTracking::Init() {
    if (fVerbose > 1) cout << "======================== GEM tracking init started ====================" << endl;

    //Get ROOT Manager
    FairRootManager* ioman = FairRootManager::Instance();
    if (!ioman)
        Fatal("Init", "FairRootManager is not instantiated");

    //MC tracks for drawing during debugging
    fMCTracksArray = (TClonesArray*)ioman->GetObject("MCTrack");  //in

    //GEM
    if (fInnerTrackerSetup[kGEM]) {
        fGemPointsArray = (TClonesArray*)ioman->GetObject("StsPoint");        //in
        fGemHitsArray = (TClonesArray*)ioman->GetObject(fGemHitsBranchName);  //in
        fGemTracksArray = new TClonesArray("BmnTrack", 100);                  //out
        ioman->Register(fGemTracksBranchName, "GEM", fGemTracksArray, kTRUE);
    }

    //SILICON
    if (fInnerTrackerSetup[kSILICON]) {
        fSilPointsArray = (TClonesArray*)ioman->GetObject("SiliconPoint");    //in
        fSilHitsArray = (TClonesArray*)ioman->GetObject(fSilHitsBranchName);  //in
        fSilTracksArray = new TClonesArray("BmnTrack", 100);                  //out
        ioman->Register(fSilTracksBranchName, "SILICON", fSilTracksArray, kTRUE);
    }

    //SSD
    if (fInnerTrackerSetup[kSSD]) {
        fSsdPointsArray = (TClonesArray*)ioman->GetObject("SSDPoint");        //in
        fSsdHitsArray = (TClonesArray*)ioman->GetObject(fSsdHitsBranchName);  //in
        fSsdTracksArray = new TClonesArray("BmnTrack", 100);                  //out
        ioman->Register(fSsdTracksBranchName, "SSD", fSsdTracksArray, kTRUE);
    }

    if (!fGemHitsArray && !fSsdHitsArray && !fSilHitsArray) {
        cout << "BmnCellAutoTracking::Init(): branch " << fGemHitsBranchName << " not found! Task will be deactivated" << endl;
        SetActive(kFALSE);
        return kERROR;
    }

    fGlobTracksArray = new TClonesArray("BmnGlobalTrack", 100);  //out
    ioman->Register(fGlobTracksBranchName, "GLOBAL", fGlobTracksArray, kTRUE);
    fHitsArray = new TClonesArray("BmnHit", 100);  //out
    ioman->Register("BmnInnerHits", "HITS", fHitsArray, kTRUE);

    fField = FairRunAna::Instance()->GetField();
    if (!fField) Fatal("Init", "No Magnetic Field found");

    TString gPathConfig = gSystem->Getenv("VMCWORKDIR");

    if (fInnerTrackerSetup[kGEM]) {
        TString gPathGemConfig = gPathConfig + "/parameters/gem/XMLConfigs/";
        TString confGem = (fPeriodId == 7) ? "GemRun" + TString(isSRC ? "SRC" : "") + "Spring2018.xml" : (fPeriodId == 6) ? "GemRunSpring2017.xml" : "GemRunSpring2017.xml";
        fGemDetector = new BmnGemStripStationSet(gPathGemConfig + confGem);
    }

    if (fInnerTrackerSetup[kSILICON]) {
        TString gPathSiConfig = gPathConfig + "/parameters/silicon/XMLConfigs/";
        TString confSi = (fPeriodId == 7) ? "SiliconRun" + TString(isSRC ? "SRC" : "") + "Spring2018.xml" : "SiliconRunSpring2017.xml";
        fSilDetector = new BmnSiliconStationSet(gPathSiConfig + confSi);
    }
    Int_t nGemStations = (fInnerTrackerSetup[kGEM]) ? fGemDetector->GetNStations() : 0;
    Int_t nSilStations = (fInnerTrackerSetup[kSILICON]) ? fSilDetector->GetNStations() : 0;
    Int_t nSsdStations = (fInnerTrackerSetup[kSSD]) ? 4 : 0;

    if (isSRC)
        nGemStations -= 4;

    fNStations = nGemStations + nSilStations + nSsdStations;

    fCellDistCut = new Double_t[fNStations];
    fHitXCutMin = new Double_t[fNStations];
    fHitXCutMax = new Double_t[fNStations];
    fHitYCutMin = new Double_t[fNStations];
    fHitYCutMax = new Double_t[fNStations];
    fCellSlopeXZCutMin = new Double_t[fNStations];
    fCellSlopeXZCutMax = new Double_t[fNStations];
    fCellSlopeYZCutMin = new Double_t[fNStations];
    fCellSlopeYZCutMax = new Double_t[fNStations];
    fCellDiffSlopeXZCut = new Double_t[fNStations];
    fCellDiffSlopeYZCut = new Double_t[fNStations];

    if (fVerbose > 1) cout << "======================== GEM tracking init finished ===================" << endl;

    return kSUCCESS;
}

void BmnCellAutoTracking::Exec(Option_t* opt) {
    if (fVerbose > 1) cout << "\n======================== GEM tracking exec started ====================" << endl;
    if (fVerbose > 1) cout << "\n Event number: " << fEventNo << endl;

    if (!IsActive()) return;

    clock_t tStart = clock();

    if (fInnerTrackerSetup[kSILICON]) fSilTracksArray->Delete();
    if (fInnerTrackerSetup[kSSD]) fSsdTracksArray->Delete();
    if (fInnerTrackerSetup[kGEM]) fGemTracksArray->Delete();
    fGlobTracksArray->Delete();
    fHitsArray->Delete();

    fEventNo++;

    Int_t nHitsCut = fSteering->GetNHitsCutTotal();

    Int_t nSilStations = (fInnerTrackerSetup[kSILICON]) ? fSilDetector->GetNStations() : 0;
    Int_t nSsdStations = (fInnerTrackerSetup[kSSD]) ? 4 : 0;

    if (fInnerTrackerSetup[kSILICON]) {
        for (Int_t iHit = 0; iHit < fSilHitsArray->GetEntriesFast(); ++iHit) {
            BmnHit hit = *((BmnHit*)fSilHitsArray->At(iHit));
            hit.SetIndex(iHit);
            hit.SetDetId(kSILICON);
            new ((*fHitsArray)[fHitsArray->GetEntriesFast()]) BmnHit(hit);
        }
    }
    if (fInnerTrackerSetup[kSSD]) {
        for (Int_t iHit = 0; iHit < fSsdHitsArray->GetEntriesFast(); ++iHit) {
            BmnHit hit = *((BmnHit*)fSsdHitsArray->At(iHit));
            hit.SetStation(hit.GetStation() + nSilStations);
            hit.SetIndex(iHit);
            hit.SetDetId(kSSD);
            new ((*fHitsArray)[fHitsArray->GetEntriesFast()]) BmnHit(hit);
        }
    }
    if (fInnerTrackerSetup[kGEM]) {
        for (Int_t iHit = 0; iHit < fGemHitsArray->GetEntriesFast(); ++iHit) {
            BmnHit hit = *((BmnHit*)fGemHitsArray->At(iHit));
            hit.SetStation(hit.GetStation() + (isSRC ? -4 : 0) + nSilStations + nSsdStations);  //shift for correct station numbering
            hit.SetIndex(iHit);
            hit.SetDetId(kGEM);
            new ((*fHitsArray)[fHitsArray->GetEntriesFast()]) BmnHit(hit);
        }
    }

    if (fHitsArray->GetEntriesFast() > nHitsCut || fHitsArray->GetEntriesFast() == 0) return;

    for (Int_t iStat = 0; iStat < fNStations; iStat++) {
        fHitXCutMin[iStat] = fSteering->GetHitXCutMin()[iStat];
        fHitXCutMax[iStat] = fSteering->GetHitXCutMax()[iStat];
        fHitYCutMin[iStat] = fSteering->GetHitYCutMin()[iStat];
        fHitYCutMax[iStat] = fSteering->GetHitYCutMax()[iStat];
        fCellSlopeXZCutMin[iStat] = fSteering->GetCellSlopeXZCutMin()[iStat];
        fCellSlopeXZCutMax[iStat] = fSteering->GetCellSlopeXZCutMax()[iStat];
        fCellSlopeYZCutMin[iStat] = fSteering->GetCellSlopeYZCutMin()[iStat];
        fCellSlopeYZCutMax[iStat] = fSteering->GetCellSlopeYZCutMax()[iStat];
    }

    fNHitsCut = fSteering->GetNHitsCut();

    vector<BmnTrack> candidates;
    vector<BmnTrack> sortedCandidates;

    Double_t diffSlopeXZ0 = fSteering->GetDiffSlopeXZ0();
    Double_t diffSlopeYZ0 = fSteering->GetDiffSlopeYZ0();
    Double_t diffSlopeXZSlope = fSteering->GetDiffSlopeXZSlope();
    Double_t diffSlopeYZSlope = fSteering->GetDiffSlopeYZSlope();
    fChiSquareCut = fSteering->GetChiSquareCut();

    const Int_t nIter = fSteering->GetNIter();

    for (Int_t iter = 0; iter < nIter; ++iter) {
        vector<BmnCellDuet> cells[fNStations];

        for (Int_t iSt = 0; iSt < fNStations; ++iSt) {
            fCellDiffSlopeXZCut[iSt] = diffSlopeXZ0 + iter * diffSlopeXZSlope;
            fCellDiffSlopeYZCut[iSt] = diffSlopeYZ0 + iter * diffSlopeYZSlope;
        }

        clock_t t0 = clock();
        CellsCreation(cells);
        clock_t t1 = clock();
        // StateCalculation(cells);
        clock_t t2 = clock();
        vector<BmnTrack> tmpVec = CellsConnection(cells);
        for (BmnTrack tr : tmpVec)
            candidates.push_back(tr);
        clock_t t3 = clock();
        if (fIsField)
            TrackUpdateByKalman(candidates);
        else
            TrackUpdateByLine(candidates);
        clock_t t4 = clock();

        createTime += ((Double_t)(t1 - t0)) / CLOCKS_PER_SEC;
        stateTime += ((Double_t)(t2 - t1)) / CLOCKS_PER_SEC;
        connectTime += ((Double_t)(t3 - t2)) / CLOCKS_PER_SEC;
        sortTime += ((Double_t)(t4 - t3)) / CLOCKS_PER_SEC;
    }

    clock_t t5 = clock();
    SortTracks(candidates, sortedCandidates);
    TrackSelection(sortedCandidates);
    clock_t t6 = clock();
    selectTime += ((Double_t)(t6 - t5)) / CLOCKS_PER_SEC;
    //DrawHits();

    clock_t tFinish = clock();
    if (fVerbose > 0) cout << "BmnCellAutoTracking: " << fGlobTracksArray->GetEntriesFast() << " tracks" << endl;

    workTime += ((Double_t)(tFinish - tStart)) / CLOCKS_PER_SEC;

    if (fVerbose > 1) cout << "\n======================== GEM tracking exec finished ===================" << endl;
}

BmnStatus BmnCellAutoTracking::SortTracks(vector<BmnTrack>& inTracks, vector<BmnTrack>& sortedTracks) {
    const Int_t n = fNStations - fNHitsCut + 1;  //6 for geometry 2018 (4, 5, 6, 7, 8, 9)
    multimap<Float_t, Int_t> sortedMap[n];       // array of map<Chi2,trIdx>. Each element of array corresponds fixed number of hits on track (4, 5, 6)
    for (Int_t iTr = 0; iTr < inTracks.size(); ++iTr) {
        if (inTracks.at(iTr).GetNHits() < fNHitsCut) continue;
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

    return kBMNSUCCESS;
}

void BmnCellAutoTracking::Finish() {
    ofstream outFile;
    outFile.open("QA/timing.txt");
    outFile << "Track Finder Time: " << workTime << endl;
    if (fVerbose > 0) {
        cout << "Cells creation time: " << createTime << endl;
        cout << "States calculation time: " << stateTime << endl;
        cout << "Cells connection time: " << connectTime << endl;
        cout << "Tracks sorting time: " << sortTime << endl;
        cout << "Tracks selection time: " << selectTime << endl;
    }
    cout << "Work time of the GEM tracking: " << workTime << endl;
}

BmnStatus BmnCellAutoTracking::CellsCreation(vector<BmnCellDuet>* cells) {
    vector<Int_t> hitsOnStation[fNStations];

    for (Int_t iHit = 0; iHit < fHitsArray->GetEntriesFast(); ++iHit) {
        BmnHit* hit = (BmnHit*)fHitsArray->At(iHit);
        if (!hit) continue;
        if (hit->IsUsed()) continue;
        Int_t station = hit->GetStation();
        if (hit->GetX() > fHitXCutMax[station] || hit->GetX() < fHitXCutMin[station]) continue;
        if (hit->GetY() > fHitYCutMax[station] || hit->GetY() < fHitYCutMin[station]) continue;
        hitsOnStation[station].push_back(iHit);
    }

    //loop for virtual duets
    for (Int_t iHit = 0; iHit < hitsOnStation[0].size(); ++iHit) {
        BmnHit* hit = (BmnHit*)fHitsArray->At(hitsOnStation[0].at(iHit));

        Double_t x0 = fRoughVertex.X();
        Double_t y0 = fRoughVertex.Y();
        Double_t z0 = fRoughVertex.Z();
        Double_t x1 = hit->GetX();
        Double_t y1 = hit->GetY();
        Double_t z1 = hit->GetZ();
        Double_t slopeXZ = (x1 - x0) / (z1 - z0);
        if (slopeXZ > fCellSlopeXZCutMax[0] || slopeXZ < fCellSlopeXZCutMin[0]) continue;
        Double_t slopeYZ = (y1 - y0) / (z1 - z0);
        if (slopeYZ > fCellSlopeYZCutMax[0] || slopeYZ < fCellSlopeYZCutMin[0]) continue;
        BmnCellDuet duetVirt;
        duetVirt.SetFirstIdx(-1);
        duetVirt.SetLastIdx(hitsOnStation[0].at(iHit));
        duetVirt.SetSlopeXZ(slopeXZ);
        duetVirt.SetSlopeYZ(slopeYZ);
        duetVirt.SetNewState(0);
        duetVirt.SetOldState(0);
        duetVirt.SetStartPlane(-1);
        duetVirt.SetUsing(kFALSE);
        cells[0].push_back(duetVirt);
    }

    for (Int_t iSt = 1; iSt < fNStations; ++iSt)
        for (Int_t iHit0 = 0; iHit0 < hitsOnStation[iSt - 1].size(); ++iHit0) {
            BmnHit* hit0 = (BmnHit*)fHitsArray->At(hitsOnStation[iSt - 1].at(iHit0));
            Double_t x0 = hit0->GetX();
            Double_t y0 = hit0->GetY();
            Double_t z0 = hit0->GetZ();
            for (Int_t iHit1 = 0; iHit1 < hitsOnStation[iSt].size(); ++iHit1) {
                BmnHit* hit1 = (BmnHit*)fHitsArray->At(hitsOnStation[iSt].at(iHit1));
                Double_t x1 = hit1->GetX();
                Double_t y1 = hit1->GetY();
                Double_t z1 = hit1->GetZ();
                Double_t slopeXZ = (x1 - x0) / (z1 - z0);
                if (slopeXZ > fCellSlopeXZCutMax[iSt] || slopeXZ < fCellSlopeXZCutMin[iSt]) continue;
                Double_t slopeYZ = (y1 - y0) / (z1 - z0);
                if (slopeYZ > fCellSlopeYZCutMax[iSt] || slopeYZ < fCellSlopeYZCutMin[iSt]) continue;

                BmnCellDuet duet;
                duet.SetFirstIdx(hitsOnStation[iSt - 1].at(iHit0));
                duet.SetLastIdx(hitsOnStation[iSt].at(iHit1));
                duet.SetSlopeYZ(slopeYZ);
                duet.SetSlopeXZ(slopeXZ);
                duet.SetNewState(0);
                duet.SetOldState(0);
                duet.SetStartPlane(iSt - 1);
                duet.SetUsing(kFALSE);
                cells[iSt].push_back(duet);
            }
        }

    return kBMNSUCCESS;
}

BmnStatus BmnCellAutoTracking::StateCalculation(vector<BmnCellDuet>* cells) {
    for (Int_t iStartCell = 1; iStartCell < fNStations; ++iStartCell) {
        for (Int_t iCell = iStartCell; iCell < fNStations; ++iCell) {
            if (cells[iCell].size() > 10000) continue;
            for (BmnCellDuet& duet : cells[iCell]) {
                Int_t idx = duet.GetFirstIdx();
                Int_t state = duet.GetOldState();
                for (BmnCellDuet leftNeigh : cells[iCell - 1]) {
                    if (state != leftNeigh.GetOldState()) continue;
                    if (idx != leftNeigh.GetLastIdx()) continue;
                    //FIXME!!!
                    //                                        if (Abs(duet.GetSlopeXZ() - leftNeigh.GetSlopeXZ()) > fCellDiffSlopeXZCut[iCell - 1]) continue;
                    //                    if (Abs(duet.GetSlopeYZ() - leftNeigh.GetSlopeYZ()) > fCellDiffSlopeYZCut[iCell - 1]) continue;

                    duet.SetNewState(duet.GetOldState() + 1);
                    break;
                }
            }
        }
        for (Int_t iCell = iStartCell; iCell < fNStations; ++iCell)
            for (BmnCellDuet& it : cells[iCell])
                it.SetOldState(it.GetNewState());
    }
    //        for (Int_t iCell = 0; iCell < fNStations; ++iCell) {
    //            for (BmnCellDuet &it : cells[iCell]) {
    //                printf("state[%d] = %d\n", iCell, it.GetOldState());
    //            }
    //        }

    return kBMNSUCCESS;
}

vector<BmnTrack> BmnCellAutoTracking::CellsConnection(vector<BmnCellDuet>* cells) {
    vector<BmnTrack> cands;
    Int_t nThreads = 1;
#if defined(_OPENMP)
    nThreads = 2;
    //omp_set_dynamic(0);
    omp_set_num_threads(nThreads);
#endif
    vector<vector<BmnTrack>> candsThread(nThreads);
    for (Int_t maxCell = fNStations - 1; maxCell > 0; maxCell--) {
        BmnCellDuet curDuet;
        if (cells[maxCell].size() > kCellsCut)
            continue;

#pragma omp parallel firstprivate(curDuet)
        //if(cells[maxCell].size() > 20)
        {
#pragma omp for
            // for (BmnCellDuet& duet : cells[maxCell]) {
            for (Int_t i = 0; i < cells[maxCell].size(); ++i) {
                BmnCellDuet duet = cells[maxCell].at(i);
                //            if (duet.GetOldState() != maxCell) continue; //FIXME: do we need this condition???
                BmnTrack trackCand;
                if (duet.GetFirstIdx() == -1) continue;  // skip virtual duet
                trackCand.AddHit(duet.GetFirstIdx(), (BmnHit*)fHitsArray->At(duet.GetFirstIdx()));
                trackCand.AddHit(duet.GetLastIdx(), (BmnHit*)fHitsArray->At(duet.GetLastIdx()));
                trackCand.SortHits();
                curDuet = duet;
                for (Int_t iCellLeft = maxCell - 1; iCellLeft >= 0; iCellLeft--) {
                    BmnCellDuet* minLeft = nullptr;
                    Double_t minSlopeDiff = 1e10;
                    for (BmnCellDuet& itLeft : cells[iCellLeft]) {
                        //if (itLeft.GetOldState() != iCellLeft) continue; //FIXME! Maybe it is better not to use this cut?
                        if (curDuet.GetFirstIdx() != itLeft.GetLastIdx()) continue;
                        Double_t slopeDiffYZ = Abs(curDuet.GetSlopeYZ() - itLeft.GetSlopeYZ());
                        Double_t slopeDiffXZ = Abs(curDuet.GetSlopeXZ() - itLeft.GetSlopeXZ());
                        if (slopeDiffYZ < minSlopeDiff && slopeDiffYZ < fCellDiffSlopeYZCut[iCellLeft] && slopeDiffXZ < fCellDiffSlopeXZCut[iCellLeft]) {  //FIXME!!! which slope to use???
                            minSlopeDiff = slopeDiffYZ;
                            minLeft = &itLeft;
                        }
                    }
                    if (minLeft != nullptr) {
                        if (minLeft->GetFirstIdx() == -1) continue;  // skip virtual duet
                        trackCand.AddHit(minLeft->GetFirstIdx(), (BmnHit*)fHitsArray->At(minLeft->GetFirstIdx()));
                        curDuet = *minLeft;
                        trackCand.SortHits();
                    }
                }

                if (CalculateTrackParams(&trackCand) == kBMNERROR) continue;
                if (IsParCorrect(trackCand.GetParamFirst(), fIsField) && IsParCorrect(trackCand.GetParamLast(), fIsField)) {
                    Int_t threadNum = 0;
#if defined(_OPENMP)
                    threadNum = omp_get_thread_num();
#endif
                    candsThread.at(threadNum).push_back(trackCand);
                }
            }
        }
    }
    for (Int_t i = 0; i < nThreads; ++i)
        for (Int_t j = 0; j < candsThread[i].size(); ++j)
            cands.push_back(candsThread[i][j]);

    return cands;
}

BmnStatus BmnCellAutoTracking::TrackUpdateByLine(vector<BmnTrack>& cands) {
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

BmnStatus BmnCellAutoTracking::TrackUpdateByKalman(vector<BmnTrack>& cands) {
    for (BmnTrack& cand : cands) {
        FairTrackParam par = *(cand.GetParamFirst());
        //par.SetQp(1.0/8.0);
        Double_t chiTot = 0.0;
        for (Int_t iHit = 0; iHit < cand.GetNHits(); ++iHit) {
            BmnHit* hit = (BmnHit*)fHitsArray->At(cand.GetHitIndex(iHit));
            Double_t chi = 0.0;
            fKalman->TGeoTrackPropagate(&par, hit->GetZ(), 2212, nullptr, nullptr, fIsField);
            fKalman->Update(&par, hit, chi);
        }
        cand.SetParamLast(par);
        for (Int_t iHit = cand.GetNHits() - 1; iHit >= 0; iHit--) {
            BmnHit* hit = (BmnHit*)fHitsArray->At(cand.GetHitIndex(iHit));
            Double_t chi = 0.0;
            fKalman->TGeoTrackPropagate(&par, hit->GetZ(), 2212, nullptr, nullptr, fIsField);
            fKalman->Update(&par, hit, chi);
            chiTot += chi;
        }
        cand.SetParamFirst(par);
        cand.SetChi2(chiTot);
    }

    return kBMNSUCCESS;
}

// BmnStatus BmnCellAutoTracking::TrackUpdateByKalman(vector<BmnTrack>& cands) {
// BmnTrack cand;
// FairTrackParam par;
// Double_t chiTot;
// Double_t chi;
// BmnHit* hit = nullptr;
// BmnKalmanFilter* kf = nullptr;
// omp_set_num_threads(2);
// #pragma omp parallel private(cand, par, chiTot, hit, chi, kf)
// {
//             kf = new BmnKalmanFilter();
// #pragma omp for
//         //cout << omp_get_thread_num() << endl;
//         //if (omp_get_thread_num() == 0) cout << omp_get_num_threads() << endl;

//         // for (BmnTrack& cand : cands) {

//         for (Int_t iCand = 0; iCand < cands.size(); ++iCand) {
//             cand = cands[iCand];
//             par = *(cand.GetParamFirst());
//             chiTot = 0.0;
//             for (Int_t iHit = 0; iHit < cand.GetNHits(); ++iHit) {
//                 //hit = (BmnHit*)fHitsArray->At(cand.GetHitIndex(iHit));
//                 //chi = 0.0;
//                 kf->TGeoTrackPropagate(&par, 100, 211, nullptr, nullptr, fIsField);
//                 //kf->Update(&par, hit, chi);
//             }
//             // cand.SetParamLast(par);
//             // for (Int_t iHit = cand.GetNHits() - 1; iHit >= 0; iHit--) {
//             //     hit = (BmnHit*)fHitsArray->At(cand.GetHitIndex(iHit));
//             //     chi = 0.0;
//             //     //kf->TGeoTrackPropagate(&par, hit->GetZ(), 211, nullptr, nullptr, fIsField);
//             //     //kf->Update(&par, hit, chi);
//             //     chiTot += chi;
//             // }
//             cand.SetParamFirst(par);
//             cand.SetChi2(chiTot);
//         }
// }
// return kBMNSUCCESS;
// }

BmnStatus BmnCellAutoTracking::TrackSelection(vector<BmnTrack>& sortedTracks) {
    CheckSharedHits(sortedTracks);
    for (Int_t iTr = 0; iTr < sortedTracks.size(); ++iTr) {
        BmnTrack tr = sortedTracks[iTr];
        if (tr.GetFlag() == -1 || !IsParCorrect(tr.GetParamFirst(), fIsField) || !IsParCorrect(tr.GetParamLast(), fIsField)) continue;
        BmnTrack gemTr;
        BmnTrack silTr;
        BmnTrack ssdTr;
        BmnGlobalTrack globTr;
        for (Int_t iHit = 0; iHit < tr.GetNHits(); ++iHit) {
            BmnHit* hit = (BmnHit*)fHitsArray->At(tr.GetHitIndex(iHit));
            DetectorId detId = hit->GetDetId();
            if (detId == kSILICON)
                silTr.AddHit(hit->GetIndex(), hit);
            else if (detId == kSSD)
                ssdTr.AddHit(hit->GetIndex(), hit);
            else if (detId == kGEM)
                gemTr.AddHit(hit->GetIndex(), hit);
            else
                continue;
        }

        if (fInnerTrackerSetup[kSILICON]) silTr.SortHits();
        if (fInnerTrackerSetup[kSSD]) ssdTr.SortHits();
        if (fInnerTrackerSetup[kGEM]) gemTr.SortHits();

        if (silTr.GetNHits() > 0) {
            new ((*fSilTracksArray)[fSilTracksArray->GetEntriesFast()]) BmnTrack(silTr);
            globTr.SetSilTrackIndex(fSilTracksArray->GetEntriesFast() - 1);
        }
        if (ssdTr.GetNHits() > 0) {
            new ((*fSsdTracksArray)[fSsdTracksArray->GetEntriesFast()]) BmnTrack(ssdTr);
            globTr.SetSsdTrackIndex(fSsdTracksArray->GetEntriesFast() - 1);
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

    return kBMNSUCCESS;
}

void BmnCellAutoTracking::SetHitsUsing(BmnTrack* tr, Bool_t use) {
    for (Int_t i = 0; i < tr->GetNHits(); ++i) {
        BmnHit* hit = (BmnHit*)fHitsArray->At(tr->GetHitIndex(i));
        if (hit) hit->SetUsing(use);
    }
}

BmnStatus BmnCellAutoTracking::CalcCovMatrix(BmnTrack* tr) {
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

BmnStatus BmnCellAutoTracking::CalculateTrackParams(BmnTrack* tr) {
    //Estimation of track parameters for events with magnetic field
    const UInt_t nHits = tr->GetNHits();
    if (nHits < fNHitsCut) return kBMNERROR;
    TVector3 lineParZY = LineFit(tr, fHitsArray, "ZY");
    tr->SetNDF(nHits - (fIsField ? 3 : 2));
    const Double_t B = lineParZY.X();  //angle coefficient for helicoid

    Double_t Tx_first = CalcTx((BmnHit*)fHitsArray->At(tr->GetHitIndex(0)), (BmnHit*)fHitsArray->At(tr->GetHitIndex(1)), (BmnHit*)fHitsArray->At(tr->GetHitIndex(2)));
    Double_t Tx_last = CalcTx((BmnHit*)fHitsArray->At(tr->GetHitIndex(nHits - 1)), (BmnHit*)fHitsArray->At(tr->GetHitIndex(nHits - 2)), (BmnHit*)fHitsArray->At(tr->GetHitIndex(nHits - 3)));

    if (fIsField) CalcCovMatrix(tr);
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
    Double_t QP = fIsField ? CalcQp(tr) : 0.0;
    tr->GetParamFirst()->SetQp(QP);
    tr->GetParamLast()->SetQp(QP);

    return kBMNSUCCESS;
}

TVector2 BmnCellAutoTracking::CalcMeanSigma(vector<Double_t> QpSegm) {
    Double_t QpSum = 0.;
    for (Int_t iSegm = 0; iSegm < QpSegm.size(); iSegm++)
        QpSum += QpSegm[iSegm];

    Double_t QpMean = QpSum / QpSegm.size();

    Double_t sqSigmaSum = 0.;
    for (Int_t iSegm = 0; iSegm < QpSegm.size(); iSegm++)
        sqSigmaSum += Sq(QpSegm[iSegm] - QpMean);

    return TVector2(QpMean, Sqrt(sqSigmaSum / QpSegm.size()));
}

Double_t BmnCellAutoTracking::CalcQp(BmnTrack* track) {
    Bool_t fRobustRefit = kTRUE;
    Bool_t fSimpleRefit = kFALSE;
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
    if (fSimpleRefit) {
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
    if (fRobustRefit) {
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

Double_t BmnCellAutoTracking::CalculateLength(BmnTrack* tr) {
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

BmnStatus BmnCellAutoTracking::CheckSharedHits(vector<BmnTrack>& sortedTracks) {
    set<Int_t> hitsId;

    const Int_t kNSharedHits = 0;  //fSteering->GetNSharedHits();

    for (Int_t iTr = 0; iTr < sortedTracks.size(); ++iTr) {
        BmnTrack* tr = &(sortedTracks.at(iTr));
        if (tr->GetFlag() == -1) continue;

        Int_t nofSharedHits = 0;
        Int_t nofHits = tr->GetNHits();
        for (Int_t iHit = 0; iHit < nofHits; iHit++)
            if (hitsId.find(tr->GetHitIndex(iHit)) != hitsId.end()) {
                nofSharedHits++;
                if (nofSharedHits > kNSharedHits) {
                    tr->SetFlag(-1);
                    break;
                }
            }
        if (tr->GetFlag() == -1) continue;

        for (Int_t iHit = 0; iHit < nofHits; iHit++)
            hitsId.insert(tr->GetHitIndex(iHit));
    }
    hitsId.clear();

    return kBMNSUCCESS;
}

BmnStatus BmnCellAutoTracking::DrawHits() {
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

    for (Int_t iTr = 0; iTr < fGlobTracksArray->GetEntriesFast(); ++iTr) {
        BmnGlobalTrack* glTrack = (BmnGlobalTrack*)fGlobTracksArray->At(iTr);

        BmnTrack* gemTr = nullptr;
        BmnTrack* silTr = nullptr;
        BmnTrack* ssdTr = nullptr;

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
        if (glTrack->GetSsdTrackIndex() != -1) {
            ssdTr = (BmnTrack*)fSsdTracksArray->UncheckedAt(glTrack->GetSsdTrackIndex());

            xPrev = ((BmnHit*)fSsdHitsArray->At(ssdTr->GetHitIndex(0)))->GetX();
            yPrev = ((BmnHit*)fSsdHitsArray->At(ssdTr->GetHitIndex(0)))->GetY();
            zPrev = ((BmnHit*)fSsdHitsArray->At(ssdTr->GetHitIndex(0)))->GetZ();

            for (Int_t iHit = 1; iHit < ssdTr->GetNHits(); iHit++) {
                BmnHit* hit = (BmnHit*)fSsdHitsArray->UncheckedAt(ssdTr->GetHitIndex(iHit));
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

            if (glTrack->GetSilTrackIndex() == -1 && glTrack->GetSsdTrackIndex() == -1) {
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

    Int_t nMCtracks = 0;
    for (Int_t iTr = 0; iTr < fMCTracksArray->GetEntriesFast(); ++iTr) {
        CbmMCTrack* tr = (CbmMCTrack*)fMCTracksArray->At(iTr);
        Int_t nPoints = 0;  //tr->GetNPoints(kGEM);
        vector<TLine*> vLineZX;
        vector<TLine*> vLineZY;
        Double_t z0 = tr->GetStartZ();
        Double_t x0 = tr->GetStartX();
        Double_t y0 = tr->GetStartY();
        Double_t x1, y1, z1;
        set<Int_t> vSt;
        if (fInnerTrackerSetup[kSILICON])
            for (Int_t i = 0; i < fSilPointsArray->GetEntriesFast(); ++i) {
                FairMCPoint* pnt = (FairMCPoint*)fSilPointsArray->At(i);
                if (pnt->GetTrackID() == iTr) {
                    nPoints++;
                    z1 = pnt->GetZ();
                    x1 = pnt->GetX();
                    y1 = pnt->GetY();
                    vSt.insert(fSilDetector->GetPointStationOwnership(z1));
                    vLineZX.push_back(new TLine(z0, x0, z1, x1));
                    vLineZY.push_back(new TLine(z0, y0, z1, y1));
                    z0 = z1;
                    x0 = x1;
                    y0 = y1;
                }
            }
        // if (fInnerTrackerSetup[kSSD])
        //     for (Int_t i = 0; i < fSsdPointsArray->GetEntriesFast(); ++i) {
        //         FairMCPoint* pnt = (FairMCPoint*) fSsdPointsArray->At(i);
        //         if (pnt->GetTrackID() == iTr) {
        //             nPoints++;
        //             z1 = pnt->GetZ();
        //             x1 = pnt->GetX();
        //             y1 = pnt->GetY();
        //             BmnSSDHitProducer hp;
        //             vSt.insert(hp.DefineStationByZ(z1, 2));
        //             vLineZX.push_back(new TLine(z0, x0, z1, x1));
        //             vLineZY.push_back(new TLine(z0, y0, z1, y1));
        //             z0 = z1;
        //             x0 = x1;
        //             y0 = y1;
        //         }
        //     }
        if (fInnerTrackerSetup[kGEM])
            for (Int_t i = 0; i < fGemPointsArray->GetEntriesFast(); ++i) {
                FairMCPoint* pnt = (FairMCPoint*)fGemPointsArray->At(i);
                if (pnt->GetTrackID() == iTr) {
                    nPoints++;
                    z1 = pnt->GetZ();
                    x1 = pnt->GetX();
                    y1 = pnt->GetY();
                    vSt.insert(fGemDetector->GetPointStationOwnership(z1) + fNSiliconStations);
                    vLineZX.push_back(new TLine(z0, x0, z1, x1));
                    vLineZY.push_back(new TLine(z0, y0, z1, y1));
                    z0 = z1;
                    x0 = x1;
                    y0 = y1;
                }
            }
        if (nPoints > 3 && vSt.size() > 3) {
            nMCtracks++;
            c->cd(1);
            for (TLine* it : vLineZX) {
                it->SetLineColor(kBlue);
                it->SetLineStyle(9);
                it->Draw("same");
            }
            c->cd(2);
            for (TLine* it : vLineZY) {
                it->SetLineColor(kBlue);
                it->SetLineStyle(9);
                it->Draw("same");
            }
        }
    }

    printf("NMCTracks = %d   GlobTracks = %d\n", nMCtracks, fGlobTracksArray->GetEntriesFast());

    //    arc->Draw("same");
    c->SaveAs("hits.png");
    getchar();
    delete h_HitsZX;
    delete h_HitsZY;
    delete c;

    return kBMNSUCCESS;
}
