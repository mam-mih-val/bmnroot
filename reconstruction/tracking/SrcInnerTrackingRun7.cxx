#include "SrcInnerTrackingRun7.h"

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
//-----------------------------------------

using namespace std;
using namespace TMath;

SrcInnerTrackingRun7::SrcInnerTrackingRun7(Int_t run, Bool_t target) : fSteering(nullptr), fSteerFile("") {
    fEventNo = 0;
    fIsTarget = target;
    fGemHitsArray = nullptr;
    fHitsArray = nullptr;
    fGlobTracksArray = nullptr;
    fGemTracksArray = nullptr;
    fField = nullptr;
    fGemHitsBranchName = "BmnGemStripHit";
    fGlobTracksBranchName = "BmnGlobalTrack";
    fGemTracksBranchName = "BmnGemTrack";
    fGemDetector = nullptr;
    fHitXCutMin = nullptr;
    fHitXCutMax = nullptr;
    fHitYCutMin = nullptr;
    fHitYCutMax = nullptr;
    fNHitsCut = 0;
}

SrcInnerTrackingRun7::~SrcInnerTrackingRun7() {
    delete fKalman;
    delete fGemDetector;
    delete[] fHitXCutMin;
    delete[] fHitXCutMax;
    delete[] fHitYCutMin;
    delete[] fHitYCutMax;
}

InitStatus SrcInnerTrackingRun7::Init() {
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

    if (!fGemHitsArray) {
        cout << "SrcInnerTrackingRun7::Init(): branch " << fGemHitsBranchName << " not found! Task will be deactivated" << endl;
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
    TString gPathGemConfig = gPathConfig + "/parameters/gem/XMLConfigs/GemRunSRCSpring2018.xml";
    fGemDetector = new BmnGemStripStationSet(gPathGemConfig);

    fNStations = fGemDetector->GetNStations() - 4;  // first four stations are in arms
    fHitXCutMin = new Double_t[fNStations];
    fHitXCutMax = new Double_t[fNStations];
    fHitYCutMin = new Double_t[fNStations];
    fHitYCutMax = new Double_t[fNStations];

    BmnFieldMap* field = (BmnFieldMap*)FairRunAna::Instance()->GetField();
    Bool_t isField = !(field->IsFieldOff());
    fSteering = new BmnSteering(isField ? "SRC_run7_withField.dat" : "SRC_run7_noField.dat");
    
    if (fVerbose > 1) cout << "======================== GEM tracking init finished ===================" << endl;

    return kSUCCESS;
}

void SrcInnerTrackingRun7::Exec(Option_t* opt) {
    if (fVerbose > 1) cout << "\n======================== GEM tracking exec started ====================" << endl;
    if (fVerbose > 1) cout << "\n Event number: " << fEventNo << endl;

    if (!IsActive()) return;

    clock_t tStart = clock();

    fGemTracksArray->Delete();
    fGlobTracksArray->Delete();
    fHitsArray->Delete();

    fEventNo++;
    Int_t nHitsCut = 200;  //fSteering->GetNHitsCutTotal();

    for (Int_t iHit = 0; iHit < fGemHitsArray->GetEntriesFast(); ++iHit) {
        BmnGemStripHit hit = *((BmnGemStripHit*)fGemHitsArray->At(iHit));
        if (hit.GetStation() < 4) continue;
        if (Abs(hit.GetStripTotalSignalInUpperLayer() - hit.GetStripTotalSignalInLowerLayer()) / (hit.GetStripTotalSignalInUpperLayer() + hit.GetStripTotalSignalInLowerLayer()) > 0.5) continue;
        hit.SetStation(hit.GetStation() - 4);  //shift for correct station numbering
        hit.SetIndex(iHit);
        hit.SetDetId(kGEM);
        hit.SetDxyz(0.5, 0.5, 0.5);
        new ((*fHitsArray)[fHitsArray->GetEntriesFast()]) BmnHit(hit);
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
    FindTracks_6of6();
    //FindTracks_5of6();
    FindTracks_4of6();
    //FindTracks_3of6();
    //cout << "FindTracks_4of4_OnLastStations: " << fGlobTracksArray->GetEntriesFast() << " tracks" << endl;

    //DrawHits();

    clock_t tFinish = clock();
    if (fVerbose > 0) cout << "SrcInnerTrackingRun7: " << fGlobTracksArray->GetEntriesFast() << " tracks" << endl;

    workTime += ((Double_t)(tFinish - tStart)) / CLOCKS_PER_SEC;

    if (fVerbose > 1) cout << "\n======================== GEM tracking exec finished ===================" << endl;
    return;
}

BmnStatus SrcInnerTrackingRun7::FindTracks_6of6() {
    vector<BmnHit*> sortedHits[fNStations];
    SortHits(sortedHits);

    vector<BmnTrack> candidates;
    vector<BmnTrack> sortedCandidates;

    for (BmnHit* hit5 : sortedHits[5]) {
        for (BmnHit* hit4 : sortedHits[4]) {
            if (Abs(hit5->GetX() - hit4->GetX()) > 5 || Abs(hit5->GetY() - hit4->GetY()) > 2) continue;
            for (BmnHit* hit3 : sortedHits[3]) {
                if (Abs(hit4->GetX() - hit3->GetX()) > 5 || Abs(hit4->GetY() - hit3->GetY()) > 2) continue;
                for (BmnHit* hit2 : sortedHits[2]) {
                    if (Abs(hit3->GetX() - hit2->GetX()) > 5 || Abs(hit3->GetY() - hit2->GetY()) > 2) continue;
                    for (BmnHit* hit1 : sortedHits[1]) {
                        if (Abs(hit2->GetX() - hit1->GetX()) > 5 || Abs(hit2->GetY() - hit1->GetY()) > 2) continue;
                        for (BmnHit* hit0 : sortedHits[0]) {
                            if (Abs(hit1->GetX() - hit0->GetX()) > 5 || Abs(hit1->GetY() - hit0->GetY()) > 2) continue;
                            BmnTrack cand;
                            cand.AddHit(hit5->GetRefIndex(), hit5);
                            cand.AddHit(hit4->GetRefIndex(), hit4);
                            cand.AddHit(hit3->GetRefIndex(), hit3);
                            cand.AddHit(hit2->GetRefIndex(), hit2);
                            cand.AddHit(hit1->GetRefIndex(), hit1);
                            cand.AddHit(hit0->GetRefIndex(), hit0);
                            cand.SortHits();
                            CalculateTrackParams(&cand);
                            cand.SetNDF(cand.GetNHits() * 2 - 5);
                            candidates.push_back(cand);
                        }
                    }
                }
            }
        }
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

BmnStatus SrcInnerTrackingRun7::FindTracks_3of6() {
    vector<BmnHit*> sortedHits[fNStations];
    SortHits(sortedHits);

    vector<BmnTrack> candidates;
    vector<BmnTrack> sortedCandidates;

    FindCandidateByThreeStations(0, 1, 2, candidates, sortedHits);
    FindCandidateByThreeStations(1, 2, 3, candidates, sortedHits);
    FindCandidateByThreeStations(2, 3, 4, candidates, sortedHits);
    FindCandidateByThreeStations(3, 4, 5, candidates, sortedHits);

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

BmnStatus SrcInnerTrackingRun7::FindTracks_5of6() {
    vector<BmnHit*> sortedHits[fNStations];
    SortHits(sortedHits);

    vector<BmnTrack> candidates;
    vector<BmnTrack> sortedCandidates;

    FindCandidateByFiveStations(0, 1, 2, 3, 4, candidates, sortedHits);
    FindCandidateByFiveStations(0, 1, 2, 3, 5, candidates, sortedHits);
    FindCandidateByFiveStations(0, 1, 2, 4, 5, candidates, sortedHits);
    FindCandidateByFiveStations(0, 1, 3, 4, 5, candidates, sortedHits);
    FindCandidateByFiveStations(0, 2, 3, 4, 5, candidates, sortedHits);
    FindCandidateByFiveStations(1, 2, 3, 4, 5, candidates, sortedHits);

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

BmnStatus SrcInnerTrackingRun7::FindTracks_4of6() {
    vector<BmnHit*> sortedHits[fNStations];
    SortHits(sortedHits);

    vector<BmnTrack> candidates;
    vector<BmnTrack> sortedCandidates;

    FindCandidateByFourStations(0, 1, 2, 3, candidates, sortedHits);
    FindCandidateByFourStations(1, 2, 3, 4, candidates, sortedHits);
    FindCandidateByFourStations(2, 3, 4, 5, candidates, sortedHits);

    FindCandidateByFourStations(1, 3, 4, 5, candidates, sortedHits);
    FindCandidateByFourStations(0, 2, 3, 4, candidates, sortedHits);
    FindCandidateByFourStations(1, 2, 3, 5, candidates, sortedHits);
    FindCandidateByFourStations(0, 1, 2, 4, candidates, sortedHits);

    FindCandidateByFourStations(0, 3, 4, 5, candidates, sortedHits);
    FindCandidateByFourStations(0, 1, 2, 5, candidates, sortedHits);
    FindCandidateByFourStations(1, 2, 4, 5, candidates, sortedHits);
    FindCandidateByFourStations(0, 2, 4, 5, candidates, sortedHits);
    FindCandidateByFourStations(0, 2, 3, 5, candidates, sortedHits);
    FindCandidateByFourStations(0, 1, 4, 5, candidates, sortedHits);
    FindCandidateByFourStations(0, 1, 3, 5, candidates, sortedHits);
    FindCandidateByFourStations(0, 1, 3, 4, candidates, sortedHits);

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

BmnStatus SrcInnerTrackingRun7::FindCandidateByFiveStations(Short_t st0, Short_t st1, Short_t st2, Short_t st3, Short_t st4, vector<BmnTrack>& candidates, vector<BmnHit*>* sortedHits) {
    for (BmnHit* hit4 : sortedHits[st4]) {
        for (BmnHit* hit3 : sortedHits[st3]) {
            if (Abs(hit4->GetX() - hit3->GetX()) > 5 || Abs(hit4->GetY() - hit3->GetY()) > 2) continue;
            for (BmnHit* hit2 : sortedHits[st2]) {
                if (Abs(hit3->GetX() - hit2->GetX()) > 5 || Abs(hit3->GetY() - hit2->GetY()) > 2) continue;
                for (BmnHit* hit1 : sortedHits[st1]) {
                    if (Abs(hit2->GetX() - hit1->GetX()) > 5 || Abs(hit2->GetY() - hit1->GetY()) > 2) continue;
                    for (BmnHit* hit0 : sortedHits[st0]) {
                        if (Abs(hit1->GetX() - hit0->GetX()) > 5 || Abs(hit1->GetY() - hit0->GetY()) > 2) continue;
                        BmnTrack cand;
                        cand.AddHit(hit4->GetRefIndex(), hit4);
                        cand.AddHit(hit3->GetRefIndex(), hit3);
                        cand.AddHit(hit2->GetRefIndex(), hit2);
                        cand.AddHit(hit1->GetRefIndex(), hit1);
                        cand.AddHit(hit0->GetRefIndex(), hit0);
                        cand.SortHits();
                        CalculateTrackParams(&cand);
                        candidates.push_back(cand);
                    }
                }
            }
        }
    }

    return kBMNSUCCESS;
}

BmnStatus SrcInnerTrackingRun7::FindCandidateByFourStations(Short_t st0, Short_t st1, Short_t st2, Short_t st3, vector<BmnTrack>& candidates, vector<BmnHit*>* sortedHits) {
    for (BmnHit* hit3 : sortedHits[st3]) {
        for (BmnHit* hit2 : sortedHits[st2]) {
            if (Abs(hit3->GetX() - hit2->GetX()) > 5 || Abs(hit3->GetY() - hit2->GetY()) > 2) continue;
            for (BmnHit* hit1 : sortedHits[st1]) {
                if (Abs(hit2->GetX() - hit1->GetX()) > 5 || Abs(hit2->GetY() - hit1->GetY()) > 2) continue;
                for (BmnHit* hit0 : sortedHits[st0]) {
                    if (Abs(hit1->GetX() - hit0->GetX()) > 5 || Abs(hit1->GetY() - hit0->GetY()) > 2) continue;
                    BmnTrack cand;
                    cand.AddHit(hit3->GetRefIndex(), hit3);
                    cand.AddHit(hit2->GetRefIndex(), hit2);
                    cand.AddHit(hit1->GetRefIndex(), hit1);
                    cand.AddHit(hit0->GetRefIndex(), hit0);
                    cand.SortHits();
                    CalculateTrackParams(&cand);
                    candidates.push_back(cand);
                }
            }
        }
    }
    vector<Short_t> restSt(2);
    for (Int_t i = 0; i < fNStations; ++i) {
        if (i == st0 || i == st1 || i == st2 || i == st3) continue;
        restSt.push_back(i);
    }
    for (BmnTrack& cand : candidates) {
        for (Int_t i = 0; i < 2; ++i) {
            MatchHit(&cand, sortedHits[restSt[i]], restSt[i] > st3);
        }

        // NDF = (N counts in ZX plane + N counts in ZY plane) - 2 parameters of Line in ZY plane - 3 parameters of Circle in ZX plane
        // Check it!!!
        cand.SetNDF(cand.GetNHits() * 2 - 5);
    }

    return kBMNSUCCESS;
}

BmnStatus SrcInnerTrackingRun7::FindCandidateByThreeStations(Short_t st0, Short_t st1, Short_t st2, vector<BmnTrack>& candidates, vector<BmnHit*>* sortedHits) {
    for (BmnHit* hit2 : sortedHits[st2]) {
        for (BmnHit* hit1 : sortedHits[st1]) {
            if (Abs(hit2->GetX() - hit1->GetX()) > 10 || Abs(hit2->GetY() - hit1->GetY()) > 5) continue;
            for (BmnHit* hit0 : sortedHits[st0]) {
                if (Abs(hit1->GetX() - hit0->GetX()) > 10 || Abs(hit1->GetY() - hit0->GetY()) > 5) continue;
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
    vector<Short_t> restSt(3);
    for (Int_t i = 0; i < fNStations; ++i) {
        if (i == st0 || i == st1 || i == st2) continue;
        restSt.push_back(i);
    }
    for (BmnTrack& cand : candidates) {
        for (Int_t i = 0; i < 3; ++i) {
            MatchHit(&cand, sortedHits[restSt[i]], restSt[i] > st2);
        }

        // NDF = (N counts in ZX plane + N counts in ZY plane) - 2 parameters of Line in ZY plane - 3 parameters of Circle in ZX plane
        // Check it!!!
        cand.SetNDF(cand.GetNHits() * 2 - 5);
    }

    return kBMNSUCCESS;
}

BmnStatus SrcInnerTrackingRun7::MatchHit(BmnTrack* cand, vector<BmnHit*> sortedHits, Bool_t downstream) {
    //Double_t cutX = 0.45;
    //Double_t cutY = 0.45;
    if (sortedHits.size() == 0) return kBMNERROR;
    FairTrackParam par = downstream ? *(cand->GetParamLast()) : *(cand->GetParamFirst());
    Double_t minDist = DBL_MAX;
    //Double_t minDX = DBL_MAX;
    //Double_t minDY = DBL_MAX;
    BmnHit* minHit = nullptr;
    fKalman->TGeoTrackPropagate(&par, sortedHits.at(0)->GetZ(), 2212, nullptr, nullptr);
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

    return kBMNSUCCESS;
}

BmnStatus SrcInnerTrackingRun7::SortTracks(vector<BmnTrack>& inTracks, vector<BmnTrack>& sortedTracks) {
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
    return kBMNSUCCESS;
}

void SrcInnerTrackingRun7::Finish() {
    ofstream outFile;
    outFile.open("QA/timing.txt");
    outFile << "Track Finder Time: " << workTime << endl;
    cout << "Work time of the GEM tracking: " << workTime << endl;
    return;
}

BmnStatus SrcInnerTrackingRun7::SortHits(vector<BmnHit*>* sortedHits) {
    for (Int_t iHit = 0; iHit < fHitsArray->GetEntriesFast(); ++iHit) {
        BmnHit* hit = (BmnHit*)fHitsArray->At(iHit);
        if (!hit) continue;
        if (hit->IsUsed()) continue;
        hit->SetRefIndex(iHit);
        Int_t station = hit->GetStation();
        if (station < 0) continue;
        //if (hit->GetX() > fHitXCutMax[station] || hit->GetX() < fHitXCutMin[station]) continue;
        //if (hit->GetY() > fHitYCutMax[station] || hit->GetY() < fHitYCutMin[station]) continue;
        sortedHits[station].push_back(hit);
    }

    return kBMNSUCCESS;
}

BmnStatus SrcInnerTrackingRun7::TrackUpdateByLine(vector<BmnTrack>& cands) {
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

BmnStatus SrcInnerTrackingRun7::TrackUpdateByKalman(vector<BmnTrack>& cands) {
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

    return kBMNSUCCESS;
}

BmnStatus SrcInnerTrackingRun7::TrackSelection(vector<BmnTrack>& sortedTracks) {
    CheckSharedHits(sortedTracks);
    for (Int_t iTr = 0; iTr < sortedTracks.size(); ++iTr) {
        BmnTrack tr = sortedTracks[iTr];
        if (tr.GetFlag() == 666 || !IsParCorrect(tr.GetParamFirst()) || !IsParCorrect(tr.GetParamLast())) continue;
        BmnTrack gemTr;
        BmnGlobalTrack globTr;

        for (Int_t iHit = 0; iHit < tr.GetNHits(); ++iHit) {
            BmnHit* hit = (BmnHit*)fHitsArray->At(tr.GetHitIndex(iHit));
            globTr.AddHit(hit->GetRefIndex(), hit);
            DetectorId detId = hit->GetDetId();
            if (detId == kGEM)
                gemTr.AddHit(hit->GetIndex(), hit);
            else
                continue;
        }
        globTr.SortHits();
        gemTr.SortHits();

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

void SrcInnerTrackingRun7::SetHitsUsing(BmnTrack* tr, Bool_t use) {
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
    return;
}

BmnStatus SrcInnerTrackingRun7::CalcCovMatrix(BmnTrack* tr) {
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

BmnStatus SrcInnerTrackingRun7::CalculateTrackParams(BmnTrack* tr) {

    BmnFieldMap* field = (BmnFieldMap*)FairRunAna::Instance()->GetField();
    Bool_t isField = !(field->IsFieldOff());

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
    return kBMNSUCCESS;
}

TVector2 SrcInnerTrackingRun7::CalcMeanSigma(vector<Double_t> QpSegm) {
    Double_t QpSum = 0.;
    for (Int_t iSegm = 0; iSegm < QpSegm.size(); iSegm++)
        QpSum += QpSegm[iSegm];

    Double_t QpMean = QpSum / QpSegm.size();

    Double_t sqSigmaSum = 0.;
    for (Int_t iSegm = 0; iSegm < QpSegm.size(); iSegm++)
        sqSigmaSum += Sq(QpSegm[iSegm] - QpMean);

    return TVector2(QpMean, Sqrt(sqSigmaSum / QpSegm.size()));
}

Double_t SrcInnerTrackingRun7::CalcQp(BmnTrack* track, Bool_t doSimple) {
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

Double_t SrcInnerTrackingRun7::CalculateLength(BmnTrack* tr) {
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

BmnStatus SrcInnerTrackingRun7::CheckSharedHits(vector<BmnTrack>& sortedTracks) {
    set<Int_t> hitsId;

    const Int_t kNSharedHits = 0;  //fSteering->GetNSharedHits();

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

BmnStatus SrcInnerTrackingRun7::DrawHits() {
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
