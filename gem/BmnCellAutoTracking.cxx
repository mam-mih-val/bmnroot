
// This Class' Header ------------------
#include <TMath.h>
#include <TH2F.h>
#include <TCanvas.h>
#include <TLine.h>
#include <map>
#include <vector>
#include "BmnMath.h"
#include "TStyle.h"
#include "BmnCellAutoTracking.h"
#include "FairRunAna.h"
#include "FairTrackParam.h"
#include "BmnGemStripHit.h"
#include "BmnKalmanFilter.h"
#include "BmnCellDuet.h"

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

BmnCellAutoTracking::BmnCellAutoTracking(Short_t period, Bool_t field, Bool_t target) {
    fPeriodId = period;
    fEventNo = 0;
    fIsField = field;
    fIsTarget = target;
    fGemHitsArray = NULL;
    fRoughVertex = (fPeriodId == 7) ? TVector3(0.5, -4.6, -2.3) : (fPeriodId == 6) ? TVector3(0.0, -3.5, -21.9) : TVector3(0.0, 0.0, 0.0);
    fKalman = new BmnKalmanFilter();
    fGemTracksArray = NULL;
    fField = NULL;
    fGemHitsBranchName = "BmnGemStripHit";
    fTracksBranchName = "BmnGemTrack";
    fGemDetector = NULL;
    fCellDistCut = NULL;
    fCellSlopeXZCutMin = NULL;
    fCellSlopeXZCutMax = NULL;
    fCellSlopeYZCutMin = NULL;
    fCellSlopeYZCutMax = NULL;
    fHitXCutMin = NULL;
    fHitXCutMax = NULL;
    fHitYCutMin = NULL;
    fHitYCutMax = NULL;
    fCellDiffSlopeXZCut = 0.0;
    fCellDiffSlopeYZCut = 0.0;
    fNHitsCut = 0.0;
}

BmnCellAutoTracking::~BmnCellAutoTracking() {
    delete fKalman;
    delete fGemDetector;
    delete fHitsOnStation;
}

InitStatus BmnCellAutoTracking::Init() {

    if (fVerbose) cout << "======================== GEM tracking init started ====================" << endl;

    //Get ROOT Manager
    FairRootManager* ioman = FairRootManager::Instance();
    if (NULL == ioman) {
        Fatal("Init", "FairRootManager is not instantiated");
    }

    fGemHitsArray = (TClonesArray*) ioman->GetObject(fGemHitsBranchName); //in
    if (!fGemHitsArray) {
        cout << "BmnCellAutoTracking::Init(): branch " << fGemHitsBranchName << " not found! Task will be deactivated" << endl;
        SetActive(kFALSE);
        return kERROR;
    }

    fGemTracksArray = new TClonesArray(fTracksBranchName, 100); //out
    ioman->Register("BmnGemTrack", "GEM", fGemTracksArray, kTRUE);

    fField = FairRunAna::Instance()->GetField();
    if (!fField) Fatal("Init", "No Magnetic Field found");

    TString gPathConfig = gSystem->Getenv("VMCWORKDIR");
    TString gPathGemConfig = gPathConfig + "/gem/XMLConfigs/";
    TString confGem = (fPeriodId == 7) ? "GemRunSpring2018.xml" : (fPeriodId == 6) ? "GemRunSpring2017.xml" : "GemRunSpring2017.xml";
    if (confGem == "") {
        printf(ANSI_COLOR_RED "No GEM geometry defined!\n" ANSI_COLOR_RESET);
        throw;
    }
    fGemDetector = new BmnGemStripStationSet(gPathGemConfig + confGem);

    fHitsOnStation = new vector<Int_t>[fGemDetector->GetNStations()];
    fGemDistCut = new Double_t[fGemDetector->GetNStations()];
    for (Int_t iSt = 0; iSt < fGemDetector->GetNStations(); ++iSt)
        fGemDistCut[iSt] = 1.0;


    if (fVerbose) cout << "======================== GEM tracking init finished ===================" << endl;

    return kSUCCESS;
}

void BmnCellAutoTracking::Exec(Option_t* opt) {

    if (fVerbose) cout << "\n======================== GEM tracking exec started ====================" << endl;
    if (fVerbose) cout << "\n Event number: " << fEventNo << endl;

    if (!IsActive())
        return;

    clock_t tStart = clock();

    fGemTracksArray->Delete();

    fEventNo++;

    Int_t nHitsInGemCut = 1000;
    if (fGemHitsArray->GetEntriesFast() > nHitsInGemCut || fGemHitsArray->GetEntriesFast() == 0) return;

    fCellDistCut = new Double_t[fGemDetector->GetNStations()];
    fHitXCutMin = new Double_t[fGemDetector->GetNStations()];
    fHitXCutMax = new Double_t[fGemDetector->GetNStations()];
    fHitYCutMin = new Double_t[fGemDetector->GetNStations()];
    fHitYCutMax = new Double_t[fGemDetector->GetNStations()];
    fCellSlopeXZCutMin = new Double_t[fGemDetector->GetNStations()];
    fCellSlopeXZCutMax = new Double_t[fGemDetector->GetNStations()];
    fCellSlopeYZCutMin = new Double_t[fGemDetector->GetNStations()];
    fCellSlopeYZCutMax = new Double_t[fGemDetector->GetNStations()];

    fHitXCutMin[0] = -40; //-50;
    fHitXCutMin[1] = -60; //-60;
    fHitXCutMin[2] = -70;
    fHitXCutMin[3] = -80;
    fHitXCutMin[4] = -100;
    fHitXCutMin[5] = -100;

    fHitXCutMax[0] = 40; //50;
    fHitXCutMax[1] = 60; //60;
    fHitXCutMax[2] = 70;
    fHitXCutMax[3] = 80;
    fHitXCutMax[4] = 100;
    fHitXCutMax[5] = 100;

    fHitYCutMin[0] = -6;
    fHitYCutMin[1] = -6;
    fHitYCutMin[2] = -6;
    fHitYCutMin[3] = -6;
    fHitYCutMin[4] = -6;
    fHitYCutMin[5] = -6;

    fHitYCutMax[0] = 10;
    fHitYCutMax[1] = 20;
    fHitYCutMax[2] = 30;
    fHitYCutMax[3] = 40;
    fHitYCutMax[4] = 40;
    fHitYCutMax[5] = 40;

    fCellSlopeXZCutMin[0] = -1.0;
    fCellSlopeXZCutMin[1] = -0.7;
    fCellSlopeXZCutMin[2] = -0.7;
    fCellSlopeXZCutMin[3] = -1.0;
    fCellSlopeXZCutMin[4] = -1.0;
    fCellSlopeXZCutMin[5] = -1.0;

    fCellSlopeXZCutMax[0] = 1.0;
    fCellSlopeXZCutMax[1] = 0.7;
    fCellSlopeXZCutMax[2] = 0.7;
    fCellSlopeXZCutMax[3] = 1.0;
    fCellSlopeXZCutMax[4] = 1.0;
    fCellSlopeXZCutMax[5] = 1.0;

    fCellSlopeYZCutMin[0] = -0.05;
    fCellSlopeYZCutMin[1] = -0.5;
    fCellSlopeYZCutMin[2] = -0.5;
    fCellSlopeYZCutMin[3] = -0.05;
    fCellSlopeYZCutMin[4] = -0.1;
    fCellSlopeYZCutMin[5] = -0.1;

    fCellSlopeYZCutMax[0] = 0.4;
    fCellSlopeYZCutMax[1] = 0.5;
    fCellSlopeYZCutMax[2] = 0.5;
    fCellSlopeYZCutMax[3] = 0.4;
    fCellSlopeYZCutMax[4] = 0.5;
    fCellSlopeYZCutMax[5] = 0.5; 

    const Int_t nIter = 2;
    fNHitsCut = 4;

    for (Int_t iter = 0; iter < nIter; ++iter) {
        vector<BmnGemTrack> candidates;
        vector<BmnGemTrack> sortedCandidates;
        vector<BmnCellDuet> cells[fGemDetector->GetNStations()];

//        fCellDiffSlopeYZCut = 0.1; // + iter * 0.1;
        fCellDiffSlopeYZCut = .1;
        fCellDiffSlopeXZCut = 0.1 + iter * 0.2;
        // fCellDiffSlopeXZCut = .5;
        fChiSquareCut = 1000;

        clock_t t0 = clock();
        CellsCreation(cells);
        clock_t t1 = clock();
        for (Int_t i = 0; i < fGemDetector->GetNStations(); ++i) {
            printf("Iter = %d:  nCells[%d] = %d\n", iter, i, cells[i].size());
        }
        //                printf("fGemHitsArray->GetEntriesFast() = %d\n", fGemHitsArray->GetEntriesFast());
        StateCalculation(cells);
        clock_t t2 = clock();
        CellsConnection(cells, candidates);
        printf("Number of candidates = %d\n", candidates.size());
        clock_t t3 = clock();
        if (fIsField)
            TrackUpdateByKalman(candidates);
        else
            TrackUpdateByLine(candidates);
        SortTracks(candidates, sortedCandidates);
        clock_t t4 = clock();
        TrackSelection(sortedCandidates);
        clock_t t5 = clock();

        createTime += ((Double_t) (t1 - t0)) / CLOCKS_PER_SEC;
        stateTime += ((Double_t) (t2 - t1)) / CLOCKS_PER_SEC;
        connectTime += ((Double_t) (t3 - t2)) / CLOCKS_PER_SEC;
        sortTime += ((Double_t) (t4 - t3)) / CLOCKS_PER_SEC;
        selectTime += ((Double_t) (t5 - t4)) / CLOCKS_PER_SEC;
        cout << "GEM_TRACKING: Number of found tracks: " << fGemTracksArray->GetEntriesFast() << endl;
    }


    // DrawHits();

    clock_t tFinish = clock();
    if (fVerbose) cout << "GEM_TRACKING: Number of found tracks: " << fGemTracksArray->GetEntriesFast() << endl;

    workTime += ((Double_t) (tFinish - tStart)) / CLOCKS_PER_SEC;

    if (fVerbose) cout << "\n======================== GEM tracking exec finished ===================" << endl;

}

BmnStatus BmnCellAutoTracking::SortTracks(vector<BmnGemTrack>& inTracks, vector<BmnGemTrack>& sortedTracks) {
    const Int_t n = fGemDetector->GetNStations() - fNHitsCut + 1; //3 for geometry 2018 (4, 5, 6)
    multimap <Float_t, Int_t> sortedMap[n]; // array of map<Chi2,trIdx>. Each element of array corresponds fixed number of hits on track (4, 5, 6) 
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

    //        multimap <Float_t, Int_t> sortedTracksMap; //map<Chi2,trIdx>
    //        for (Int_t iTr = 0; iTr < inTracks.size(); ++iTr)
    //            sortedTracksMap.insert(pair<Float_t, Int_t>(inTracks.at(iTr).GetChi2() / inTracks.at(iTr).GetNDF(), iTr));
    //    
    //        for (auto it : sortedTracksMap)
    //            sortedTracks.push_back(inTracks.at(it.second));
}

void BmnCellAutoTracking::Finish() {
    ofstream outFile;
    outFile.open("QA/timing.txt");
    outFile << "Track Finder Time: " << workTime << endl;
    cout << "Cells creation time: " << createTime << endl;
    cout << "States calculation time: " << stateTime << endl;
    cout << "Cells connection time: " << connectTime << endl;
    cout << "Tracks sorting time: " << sortTime << endl;
    cout << "Tracks selection time: " << selectTime << endl;
    cout << "Full work time of the GEM tracking: " << workTime << endl;
}

BmnStatus BmnCellAutoTracking::CellsCreation(vector<BmnCellDuet>* cells) {

    vector<Int_t> hitsOnStation[fGemDetector->GetNStations()];

    for (Int_t iHit = 0; iHit < fGemHitsArray->GetEntriesFast(); ++iHit) {
        BmnGemStripHit* hit = (BmnGemStripHit*) fGemHitsArray->At(iHit);
        if (!hit) continue;
        if (hit->IsUsed()) continue;
        Int_t station = hit->GetStation();
        if (hit->GetX() > fHitXCutMax[station] || hit->GetX() < fHitXCutMin[station]) continue;
        if (hit->GetY() > fHitYCutMax[station] || hit->GetY() < fHitYCutMin[station]) continue;
        for (Int_t iSt = 0; iSt < fGemDetector->GetNStations(); ++iSt)
            if (hit->GetStation() == iSt) hitsOnStation[iSt].push_back(iHit);
    }

    printf("nHits on 0 station = %d\n", hitsOnStation[0].size());
    printf("nHits on 1 station = %d\n", hitsOnStation[1].size());
    printf("nHits on 2 station = %d\n", hitsOnStation[2].size());
    printf("nHits on 3 station = %d\n", hitsOnStation[3].size());
    printf("nHits on 4 station = %d\n", hitsOnStation[4].size());
    printf("nHits on 5 station = %d\n", hitsOnStation[5].size());

    //loop for virtual duets
    for (Int_t iHit = 0; iHit < hitsOnStation[0].size(); ++iHit) {
        BmnGemStripHit* hit = (BmnGemStripHit*) fGemHitsArray->At(hitsOnStation[0].at(iHit));

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
        //        Double_t dist = Sqrt(Sq(x1 - x0) + Sq(y1 - y0) + Sq(z1 - z0));
        //        if (dist > fCellDistCut[0])
        //            continue;
        //        if (hit->IsUsed()) continue;
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

    for (Int_t iSt = 1; iSt < fGemDetector->GetNStations(); ++iSt) {
        for (Int_t iHit0 = 0; iHit0 < hitsOnStation[iSt - 1].size(); ++iHit0) {
            BmnGemStripHit* hit0 = (BmnGemStripHit*) fGemHitsArray->At(hitsOnStation[iSt - 1].at(iHit0));
            //            if (hit0->IsUsed()) continue;
            Double_t x0 = hit0->GetX();
            Double_t y0 = hit0->GetY();
            Double_t z0 = hit0->GetZ();
            for (Int_t iHit1 = 0; iHit1 < hitsOnStation[iSt].size(); ++iHit1) {
                BmnGemStripHit* hit1 = (BmnGemStripHit*) fGemHitsArray->At(hitsOnStation[iSt].at(iHit1));
                Double_t x1 = hit1->GetX();
                Double_t y1 = hit1->GetY();
                Double_t z1 = hit1->GetZ();
                //                if (hit1->IsUsed()) continue;
                //                Double_t dist = Sqrt(Sq(y1 - y0) + Sq(z1 - z0));
                //                Double_t dist = Sqrt(Sq(x1 - x0) + Sq(y1 - y0) + Sq(z1 - z0));
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
    }

    return kBMNSUCCESS;
}

BmnStatus BmnCellAutoTracking::StateCalculation(vector<BmnCellDuet>* cells) {
    for (Int_t iStartCell = 1; iStartCell < fGemDetector->GetNStations(); ++iStartCell) {
        for (Int_t iCell = iStartCell; iCell < fGemDetector->GetNStations(); ++iCell) {
            for (BmnCellDuet &duet : cells[iCell]) {
                for (BmnCellDuet leftNeigh : cells[iCell - 1]) {
                    if (duet.GetOldState() != leftNeigh.GetOldState()) continue;
                    if (duet.GetFirstIdx() != leftNeigh.GetLastIdx()) continue;
                    if (Abs(duet.GetSlopeYZ() - leftNeigh.GetSlopeYZ()) > fCellDiffSlopeYZCut) continue;
                    if (Abs(duet.GetSlopeXZ() - leftNeigh.GetSlopeXZ()) > fCellDiffSlopeXZCut) continue;
                    duet.SetNewState(duet.GetOldState() + 1);
                    break;
                }
            }
        }
        for (Int_t iCell = iStartCell; iCell < fGemDetector->GetNStations(); ++iCell)
            for (BmnCellDuet &it : cells[iCell])
                it.SetOldState(it.GetNewState());
    }
    //        for (Int_t iCell = 0; iCell < fGemDetector->GetNStations(); ++iCell) {
    //            for (BmnCellDuet &it : cells[iCell]) {
    //                printf("state[%d] = %d\n", iCell, it.GetOldState());
    //            }
    //        }
}

BmnStatus BmnCellAutoTracking::CellsConnection(vector<BmnCellDuet>* cells, vector<BmnGemTrack>& cands) {
    for (Int_t maxCell = fGemDetector->GetNStations() - 1; maxCell > 0; maxCell--) {

        BmnCellDuet curDuet;
        for (BmnCellDuet &duet : cells[maxCell]) {

            if (duet.GetOldState() != maxCell) continue;
            BmnGemTrack trackCand;
            if (duet.GetFirstIdx() == -1) continue; // skip virtual duet
            trackCand.AddHit(duet.GetFirstIdx(), (BmnGemStripHit*) fGemHitsArray->At(duet.GetFirstIdx()));
            trackCand.AddHit(duet.GetLastIdx(), (BmnGemStripHit*) fGemHitsArray->At(duet.GetLastIdx()));
            trackCand.SortHits();
            curDuet = duet;
            for (Int_t iCellLeft = maxCell - 1; iCellLeft >= 0; iCellLeft--) {
                BmnCellDuet* minLeft = NULL;
                Double_t minSlopeDiff = 1e10;
                for (BmnCellDuet &itLeft : cells[iCellLeft]) {
                    if (itLeft.GetOldState() != iCellLeft) continue;
                    if (curDuet.GetFirstIdx() != itLeft.GetLastIdx()) continue;
                    Double_t slopeDiffYZ = Abs(curDuet.GetSlopeYZ() - itLeft.GetSlopeYZ());
                    Double_t slopeDiffXZ = Abs(curDuet.GetSlopeXZ() - itLeft.GetSlopeXZ());
                    if (slopeDiffYZ < minSlopeDiff) { //FIXME!!! which slope to use???
                        minSlopeDiff = slopeDiffYZ;
                        minLeft = &itLeft;
                    }
                }
                if (minLeft != NULL) {
                    if (minLeft->GetFirstIdx() == -1) continue; // skip virtual duet
                    trackCand.AddHit(minLeft->GetFirstIdx(), (BmnGemStripHit*) fGemHitsArray->At(minLeft->GetFirstIdx()));
                    curDuet = *minLeft;
                    trackCand.SortHits();
                }
            }

            if (CalculateTrackParams(&trackCand) == kBMNERROR) continue;
            if (IsParCorrect(trackCand.GetParamFirst(), fIsField) && IsParCorrect(trackCand.GetParamLast(), fIsField)) {
                cands.push_back(trackCand);
            }
        }
    }
}

BmnStatus BmnCellAutoTracking::TrackUpdateByKalman(vector<BmnGemTrack>& cands) {
    for (BmnGemTrack &cand : cands) {
        FairTrackParam par = *(cand.GetParamFirst());
        //        printf("Before:\n");
        //        par.Print();
        Double_t chiTot = 0.0;
        for (Int_t iHit = 0; iHit < cand.GetNHits(); ++iHit) {
            BmnGemStripHit* hit = (BmnGemStripHit*) fGemHitsArray->At(cand.GetHitIndex(iHit));
            Double_t chi = 0.0;
            fKalman->TGeoTrackPropagate(&par, hit->GetZ(), 211, NULL, NULL, fIsField);
            fKalman->Update(&par, hit, chi);
            //chiTot += chi;
        }
        //        printf("Between:\n");
        //        par.Print();
        cand.SetParamLast(par);
        for (Int_t iHit = cand.GetNHits() - 1; iHit >= 0; iHit--) {
            BmnGemStripHit* hit = (BmnGemStripHit*) fGemHitsArray->At(cand.GetHitIndex(iHit));
            Double_t chi = 0.0;
            fKalman->TGeoTrackPropagate(&par, hit->GetZ(), 211, NULL, NULL, fIsField);
            fKalman->Update(&par, hit, chi);
            chiTot += chi;
        }
        //        printf("After:\n");
        //        par.Print();
        cand.SetParamFirst(par);
        //        CircleFit(&cand, fGemHitsArray, chi);
        //        printf("chi = %f\n", chi);
        cand.SetChi2(chiTot);
    }
}

void BmnCellAutoTracking::TrackUpdateByLine(vector <BmnGemTrack>& cands) {
    for (BmnGemTrack& cand : cands) {
        Double_t Tx = LineFit((BmnTrack*)& cand, fGemHitsArray, "ZX").X();
        Double_t chiX = LineFit((BmnTrack*)& cand, fGemHitsArray, "ZX").Z();
        Double_t Ty = LineFit((BmnTrack*)& cand, fGemHitsArray, "ZY").X();
        Double_t chiY = LineFit((BmnTrack*)& cand, fGemHitsArray, "ZY").Z();

        cand.SetChi2((chiX - chiY) > 0. ? chiX : chiY);

        cand.GetParamFirst()->SetTx(Tx);
        cand.GetParamFirst()->SetTy(Ty);

        cand.GetParamLast()->SetTx(Tx);
        cand.GetParamLast()->SetTy(Ty);
    }
}

BmnStatus BmnCellAutoTracking::TrackSelection(vector<BmnGemTrack>& sortedTracks) {
    //    printf("n candidates = %d\n", sortedTracks.size());
    //    for (BmnGemTrack &it : sortedTracks) {
    if (!fIsTarget) {
        if (sortedTracks.size() != 0)
            new((*fGemTracksArray)[fGemTracksArray->GetEntriesFast()]) BmnGemTrack(sortedTracks[0]);
    } else {
        CheckSharedHits(sortedTracks);
        for (Int_t iTr = 0; iTr < sortedTracks.size(); ++iTr) {
            sortedTracks[iTr].Print();
            if (sortedTracks[iTr].GetFlag() != -1 && IsParCorrect(sortedTracks[iTr].GetParamFirst(), fIsField) && IsParCorrect(sortedTracks[iTr].GetParamLast(), fIsField)) {
                CalculateLength(&sortedTracks[iTr]);
                new((*fGemTracksArray)[fGemTracksArray->GetEntriesFast()]) BmnGemTrack(sortedTracks[iTr]);
                SetHitsUsing(&sortedTracks[iTr], kTRUE);
            }
        }
    }
    //    }
}

void BmnCellAutoTracking::SetHitsUsing(BmnGemTrack* tr, Bool_t use) {
    for (Int_t i = 0; i < tr->GetNHits(); ++i) {
        BmnGemStripHit* hit = (BmnGemStripHit*) fGemHitsArray->At(tr->GetHitIndex(i));
        if (hit) hit->SetUsing(use);
    }
}

BmnStatus BmnCellAutoTracking::CalcCovMatrix(BmnGemTrack * tr) {

    //Check it! Doesn't work correct
    const UInt_t nHits = tr->GetNHits();
    TVector3 lineParZY = LineFit(tr, fGemHitsArray, "ZY");
    Double_t chi2circ = 0.0;
    TVector3 CircParZX = (nHits == 3) ? CircleBy3Hit(tr, fGemHitsArray) : CircleFit(tr, fGemHitsArray, chi2circ);

    Double_t R = CircParZX.Z(); // radius of fit-circle
    Double_t Xc = CircParZX.Y(); // x-coordinate of fit-circle center
    Double_t Zc = CircParZX.X(); // z-coordinate of fit-circle center
    fField = FairRunAna::Instance()->GetField();

    const Double_t B = lineParZY.X(); //angle coefficient for helicoid

    //Covariance matrix
    Double_t Cov_X_X(0.0), Cov_X_Y(0.0), Cov_X_Tx(0.0), Cov_X_Ty(0.0), Cov_X_Qp(0.0);
    Double_t Cov_Y_Y(0.0), Cov_Y_Tx(0.0), Cov_Y_Ty(0.0), Cov_Y_Qp(0.0);
    Double_t Cov_Tx_Tx(0.0), Cov_Tx_Ty(0.0), Cov_Tx_Qp(0.0);
    Double_t Cov_Ty_Ty(0.0), Cov_Ty_Qp(0.0);
    Double_t Cov_Qp_Qp(0.0);
    Double_t Q = (Xc > 0) ? +1 : -1;

    Double_t sumX = 0.0;
    Double_t sumY = 0.0;
    Double_t sumTx = 0.0;
    Double_t sumTy = 0.0;
    Double_t sumQP = 0.0;
    Double_t sumXX = 0.0;
    Double_t sumXY = 0.0;
    Double_t sumXTx = 0.0;
    Double_t sumXTy = 0.0;
    Double_t sumXQP = 0.0;
    Double_t sumYY = 0.0;
    Double_t sumYTx = 0.0;
    Double_t sumYTy = 0.0;
    Double_t sumYQP = 0.0;
    Double_t sumTxTx = 0.0;
    Double_t sumTxTy = 0.0;
    Double_t sumTxQP = 0.0;
    Double_t sumTyTy = 0.0;
    Double_t sumTyQP = 0.0;
    Double_t sumQPQP = 0.0;

    for (UInt_t i = 0; i < nHits; ++i) {
        BmnGemStripHit* hit = (BmnGemStripHit*) fGemHitsArray->At(tr->GetHitIndex(i));
        if (!hit) continue;
        //        if (!hit->GetFlag()) continue;
        Double_t Xi = hit->GetX();
        Double_t Yi = hit->GetY();
        Double_t Zi = hit->GetZ();

        Double_t Txi = -1.0 * (Zi - Zc) / (Xi - Xc);
        Double_t Tyi = B; // / (Xi - Xc);
        //        Double_t QPi = Q / 0.0003 * Abs(fField->GetBy(Xi, Yi, Zi)) / Sqrt(R * R + B * B);
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

    Cov_X_X = sumXX / nHits - Sq(meanX);
    Cov_X_Y = sumXY / nHits - meanX * meanY;
    Cov_X_Tx = sumXTx / nHits - meanX * meanTx;
    Cov_X_Ty = sumXTy / nHits - meanX * meanTy;
    Cov_X_Qp = sumXQP / nHits - meanX * meanQP;
    Cov_Y_Y = sumYY / nHits - Sq(meanY);
    Cov_Y_Tx = sumYTx / nHits - meanY * meanTx;
    Cov_Y_Ty = sumYTy / nHits - meanY * meanTy;
    Cov_Y_Qp = sumYQP / nHits - meanY * meanQP;
    Cov_Tx_Tx = sumTxTx / nHits - Sq(meanTx);
    Cov_Tx_Ty = sumTxTy / nHits - meanTx * meanTy;
    Cov_Tx_Qp = sumTxQP / nHits - meanTx * meanQP;
    Cov_Ty_Ty = sumTyTy / nHits - Sq(meanTy);
    Cov_Ty_Qp = sumTyQP / nHits - meanTy * meanQP;
    Cov_Qp_Qp = sumQPQP / nHits - Sq(meanQP);

    FairTrackParam par;
    //    par.SetCovariance(0, 0, 0.0001);
    //    par.SetCovariance(0, 1, 0);
    //    par.SetCovariance(0, 2, 0);
    //    par.SetCovariance(0, 3, 0);
    //    par.SetCovariance(0, 4, 0);
    //    par.SetCovariance(1, 1, 0.0001);
    //    par.SetCovariance(1, 2, 0);
    //    par.SetCovariance(1, 3, 0);
    //    par.SetCovariance(1, 4, 0);
    //    par.SetCovariance(2, 2, 0.0001);
    //    par.SetCovariance(2, 3, 0);
    //    par.SetCovariance(2, 4, 0);
    //    par.SetCovariance(3, 3, 0.0001);
    //    par.SetCovariance(3, 4, 0);
    //    par.SetCovariance(4, 4, 0.0001);

    //    par.SetCovariance(0, 0, 2.74e-05);
    //    par.SetCovariance(0, 1, -1.3e-07);
    //    par.SetCovariance(0, 2, 1.67e-07);
    //    par.SetCovariance(0, 3, 3.19e-08);
    //    par.SetCovariance(0, 4, 1.4315e-08);
    //    par.SetCovariance(1, 1, 0.00781875);
    //    par.SetCovariance(1, 2, -1.36e-07);
    //    par.SetCovariance(1, 3, 4.23125e-05);
    //    par.SetCovariance(1, 4, -3.6808e-07);
    //    par.SetCovariance(2, 2, 3.005e-06);
    //    par.SetCovariance(2, 3, 1.11e-07);
    //    par.SetCovariance(2, 4, 1.5355e-07);
    //    par.SetCovariance(3, 3, 1.5025e-06);
    //    par.SetCovariance(3, 4, -3.175e-09);
    //    par.SetCovariance(4, 4, 7.2375e-07);

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
}

BmnStatus BmnCellAutoTracking::CalculateTrackParams(BmnGemTrack * tr) {

    //Estimation of track parameters for events with magnetic field
    const UInt_t nHits = tr->GetNHits();
    if (nHits < fNHitsCut) return kBMNERROR;
    TVector3 lineParZY = LineFit(tr, fGemHitsArray, "ZY");
    tr->SetNDF(nHits - 3);
    const Double_t B = lineParZY.X(); //angle coefficient for helicoid

    Double_t fX = ((BmnGemStripHit*) fGemHitsArray->At(tr->GetHitIndex(0)))->GetX();
    Double_t fY = ((BmnGemStripHit*) fGemHitsArray->At(tr->GetHitIndex(0)))->GetY();
    Double_t fZ = ((BmnGemStripHit*) fGemHitsArray->At(tr->GetHitIndex(0)))->GetZ();

    Double_t lX = ((BmnGemStripHit*) fGemHitsArray->At(tr->GetHitIndex(nHits - 1)))->GetX();
    Double_t lY = ((BmnGemStripHit*) fGemHitsArray->At(tr->GetHitIndex(nHits - 1)))->GetY();
    Double_t lZ = ((BmnGemStripHit*) fGemHitsArray->At(tr->GetHitIndex(nHits - 1)))->GetZ();

    Double_t Tx_first = CalcTx((BmnGemStripHit*) fGemHitsArray->At(tr->GetHitIndex(0)), (BmnGemStripHit*) fGemHitsArray->At(tr->GetHitIndex(1)), (BmnGemStripHit*) fGemHitsArray->At(tr->GetHitIndex(2)));
    Double_t Tx_last = CalcTx((BmnGemStripHit*) fGemHitsArray->At(tr->GetHitIndex(nHits - 1)), (BmnGemStripHit*) fGemHitsArray->At(tr->GetHitIndex(nHits - 2)), (BmnGemStripHit*) fGemHitsArray->At(tr->GetHitIndex(nHits - 3)));
    Double_t Ty_last = B; // / (lX - Xc);
    Double_t Ty_first = B; // / (fX - Xc);    

    CalcCovMatrix(tr);

    tr->GetParamFirst()->SetPosition(TVector3(fX, fY, fZ));
    tr->GetParamFirst()->SetTx(Tx_first);
    tr->GetParamFirst()->SetTy(Ty_first);
    tr->GetParamLast()->SetPosition(TVector3(lX, lY, lZ));
    tr->GetParamLast()->SetTx(Tx_last);
    tr->GetParamLast()->SetTy(Ty_last);
    Double_t QP = fIsField ? CalcQp(tr) : 0.0;
    tr->GetParamFirst()->SetQp(QP);
    tr->GetParamLast()->SetQp(QP);

    return kBMNSUCCESS;
}

TVector2 BmnCellAutoTracking::CalcMeanSigma(vector <Double_t> QpSegm) {
    Double_t QpSum = 0.;
    for (Int_t iSegm = 0; iSegm < QpSegm.size(); iSegm++)
        QpSum += QpSegm[iSegm];

    Double_t QpMean = QpSum / QpSegm.size();

    Double_t sqSigmaSum = 0.;
    for (Int_t iSegm = 0; iSegm < QpSegm.size(); iSegm++)
        sqSigmaSum += Sq(QpSegm[iSegm] - QpMean);

    return TVector2(QpMean, Sqrt(sqSigmaSum / QpSegm.size()));
}

Double_t BmnCellAutoTracking::CalcQp(BmnGemTrack * track) {
    Bool_t fRobustRefit = kTRUE;
    Bool_t fSimpleRefit = kFALSE;
    vector <BmnGemStripHit*> hits;

    for (Int_t iHit = 0; iHit < track->GetNHits(); iHit++)
        hits.push_back((BmnGemStripHit*) fGemHitsArray->UncheckedAt(track->GetHitIndex(iHit)));

    Int_t kNSegm = track->GetNHits() - 2;

    Double_t QpRefit = 0.;
    vector <Double_t> QpSegmBefore;

    // Get q/p info from all track segments
    for (Int_t iHit = 0; iHit < kNSegm; iHit++) {
        BmnGemStripHit* first = hits[iHit];
        BmnGemStripHit* second = hits[iHit + 1];
        BmnGemStripHit* third = hits[iHit + 2];

        TVector3 CircParZX = CircleBy3Hit(first, second, third);
        Double_t R = CircParZX.Z();
        Double_t Xc = CircParZX.Y();
        Double_t Zc = CircParZX.X();

        Double_t Q = (Xc > 0) ? +1. : -1.;
        Double_t S = 0.0003 * (Abs(fField->GetBy(third->GetX(), third->GetY(), third->GetZ())) +
                Abs(fField->GetBy(second->GetX(), second->GetY(), second->GetZ())) +
                Abs(fField->GetBy(first->GetX(), first->GetY(), first->GetZ()))) / 3.;

        Double_t Pt = S * R; //actually Pt/Q, but it doesn't matter
        Double_t fX = first->GetX();
        Double_t fZ = first->GetZ();

        Double_t h = -1.0;

        Double_t Tx_first = h * (fZ - Zc) / (fX - Xc);
        TVector3 lineParZY = LineFit(track, fGemHitsArray, "ZY");
        const Double_t B = lineParZY.X(); //angle coefficient for helicoid
        Double_t Ty_first = B; // / (fX - Xc);

        Double_t Pz = Pt / Sqrt(1 + Sq(Tx_first));
        Double_t Px = Tx_first * Pz;
        Double_t Py = Ty_first * Pz;
        Double_t P = Sqrt(Sq(Px) + Sq(Py) + Sq(Pz));
        Double_t QP = Q / P;

        QpSegmBefore.push_back(QP);
    }

    // Non-robust (simple) refit when segments with bad q/p are not taken into account
    if (fSimpleRefit) {
        vector <Double_t> QpSegmAfter;
        while (kTRUE) {
            TVector2 meanSig = CalcMeanSigma(QpSegmBefore);
            Double_t mean = meanSig.X();
            Double_t sigma = meanSig.Y();
            if (std::isnan(sigma)) {
                cout << "Bad refit convergence for track segment!!" << endl;
                return kBMNERROR;
            }

            for (Int_t iSegm = 0; iSegm < QpSegmBefore.size(); iSegm++)
                if (Abs(QpSegmBefore[iSegm] - mean) - sigma <= 0.001) // Топорное сравнение FIXME
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

        vector <Double_t> d = dist(QpSegmBefore, QpRefit);

        Double_t sigma = 0.;
        for (Int_t i = 0; i < QpSegmBefore.size(); i++)
            sigma += (QpSegmBefore[i] - QpRefit) * (QpSegmBefore[i] - QpRefit);
        sigma = Sqrt(sigma / QpSegmBefore.size());

        vector <Double_t> w = W(d, sigma);
        sigma = Sigma(d, w);

        const Int_t kNIter = 20; // FIXME
        for (Int_t iIter = 1; iIter < kNIter; iIter++) {
            QpRefit = Mu(QpSegmBefore, w);
            d = dist(QpSegmBefore, QpRefit);
            w = W(d, sigma);
            sigma = Sigma(d, w);
        }
    }

    return QpRefit;
}

Double_t BmnCellAutoTracking::CalculateLength(BmnGemTrack * tr) {
    if (!tr) return 0.0;

    vector<Double_t> X, Y, Z;
    for (Int_t iGem = 0; iGem < tr->GetNHits(); iGem++) {
        BmnGemStripHit* hit = (BmnGemStripHit*) fGemHitsArray->At(tr->GetHitIndex(iGem));
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

BmnStatus BmnCellAutoTracking::CheckSharedHits(vector<BmnGemTrack>& sortedTracks) {

    set<Int_t> hitsId;

    const Int_t kNSharedHits = 0; //fSteering->GetNSharedHits();

    for (Int_t iTr = 0; iTr < sortedTracks.size(); ++iTr) {
        BmnGemTrack* tr = &(sortedTracks.at(iTr));
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
}

BmnStatus BmnCellAutoTracking::DrawHits() {

    TH2F* h_HitsZX = new TH2F("h_HitsZX", "h_HitsZX", 400, 0.0, 200.0, 400, -100.0, 100.0);
    TH2F* h_HitsZY = new TH2F("h_HitsZY", "h_HitsZY", 400, 0.0, 200.0, 400, -10.0, 50.0);
    for (Int_t i = 0; i < fGemHitsArray->GetEntriesFast(); ++i) {
        BmnGemStripHit* hit = (BmnGemStripHit*) fGemHitsArray->At(i);
        h_HitsZX->Fill(hit->GetZ(), hit->GetX());
        h_HitsZY->Fill(hit->GetZ(), hit->GetY());
    }

    TCanvas* c = new TCanvas("c", "c", 1000, 1000);
    c->Divide(1, 2);
    c->cd(1);
    h_HitsZX->SetMarkerStyle(20);
    h_HitsZX->SetMarkerSize(1.01);
    h_HitsZX->SetMarkerColor(kRed);
    h_HitsZX->Draw("P");

    c->cd(2);
    h_HitsZY->SetMarkerStyle(20);
    h_HitsZY->SetMarkerSize(1.01);
    h_HitsZY->SetMarkerColor(kRed);
    h_HitsZY->Draw("P");


    for (Int_t iTr = 0; iTr < fGemTracksArray->GetEntriesFast(); ++iTr) {
        BmnGemTrack* track = (BmnGemTrack*) fGemTracksArray->At(iTr);
        Double_t xPrev = ((BmnGemStripHit*) fGemHitsArray->At(track->GetHitIndex(0)))->GetX();
        Double_t yPrev = ((BmnGemStripHit*) fGemHitsArray->At(track->GetHitIndex(0)))->GetY();
        Double_t zPrev = ((BmnGemStripHit*) fGemHitsArray->At(track->GetHitIndex(0)))->GetZ();
        if (track->GetFlag() != -1 && IsParCorrect(track->GetParamFirst(), fIsField) && IsParCorrect(track->GetParamLast(), fIsField)) {
            for (Int_t iHit = 1; iHit < track->GetNHits(); ++iHit) {
                BmnGemStripHit* hit = (BmnGemStripHit*) fGemHitsArray->At(track->GetHitIndex(iHit));
                Double_t x = hit->GetX();
                Double_t y = hit->GetY();
                Double_t z = hit->GetZ();
                TLine* lineZX = new TLine(z, x, zPrev, xPrev);
                TLine* lineZY = new TLine(z, y, zPrev, yPrev);
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


    //    arc->Draw("same");
    c->SaveAs("hits.png");
    getchar();
    delete h_HitsZX;
    delete h_HitsZY;
    delete c;
}