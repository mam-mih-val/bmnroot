
// This Class' Header ------------------
#include <TMath.h>
#include "BmnMath.h"
#include <TGraph.h>
#include <map>
#include <cfloat>
#include "TStyle.h"
#include "BmnGemTracking.h"
#include "TObjArray.h"
#include "TVector3.h"
#include "FairMCPoint.h"
#include "CbmMCTrack.h"
#include "CbmStsPoint.h"
#include "FairRunAna.h"
#include "TRandom.h"
#include "BmnGemStripHit.h"
#include "BmnMatrixMath.h"
#include "BmnKalmanFilter.h"
#include "TFitResult.h"
#include "TCanvas.h"

//-----------------------------------------
static Double_t workTime = 0.0;
//-----------------------------------------

static TH1F* h_Dist = new TH1F("h_Dist", "h_Dist", 100, 0.0, 10.0);
static TH1F* h_Chi = new TH1F("h_Chi", "h_Chi", 100, 0.0, 25.0);

using namespace std;
using namespace TMath;

BmnGemTracking::BmnGemTracking(Short_t period, Bool_t field, Bool_t target, TString steerFile) :
fSteering(NULL),
fSteerFile(steerFile) {
    fSteering = new BmnSteeringGemTracking(fSteerFile);
    fPeriodId = period;
    fPDG = 211;
    fEventNo = 0;
    fIsField = field;
    fIsTarget = target;
    fGemHitsArray = NULL;
    fAddresses = NULL;
    fRoughVertex = (fPeriodId == 7) ? TVector3(0.5, -4.6, -2.3) : (fPeriodId == 6) ? TVector3(0.0, -3.5, -21.9) : TVector3(0.0, 0.0, 0.0);
    fSigX = fSteering->GetSigX();
    fLorentzThresh = fSteering->GetLorentzThresh();
    fNHitsCut = fSteering->GetNHitsCut();
    fNBins = fSteering->GetNBins();
    fMin = fSteering->GetMin();
    fNSeedsCut = fSteering->GetNSeedsCut();
    fNHitsInGemCut = fSteering->GetNHitsInGemCut();
    fMax = fSteering->GetMax();
    fWidth = (fMax - fMin) / fNBins;
    fKalman = new BmnKalmanFilter();
    fGemTracksArray = NULL;
    fField = NULL;
    fGoForward = kTRUE;
    fGemHitsBranchName = "BmnGemStripHit";
    fTracksBranchName = "BmnGemTrack";
    fGemDetector = NULL;
    fUseRefit = kTRUE;

    fSteering->PrintParamTable();
}

BmnGemTracking::~BmnGemTracking() {
    delete fKalman;
    delete fGemDetector;
    delete fHitsOnStation;
}

InitStatus BmnGemTracking::Init() {

    if (fVerbose) cout << "======================== GEM tracking init started ====================" << endl;

    //Get ROOT Manager
    FairRootManager* ioman = FairRootManager::Instance();
    if (NULL == ioman) {
        Fatal("Init", "FairRootManager is not instantiated");
    }

    fGemHitsArray = (TClonesArray*) ioman->GetObject(fGemHitsBranchName); //in
    if (!fGemHitsArray) {
        cout << "BmnGemTracking::Init(): branch " << fGemHitsBranchName << " not found! Task will be deactivated" << endl;
        SetActive(kFALSE);
        return kERROR;
    }

    fGemTracksArray = new TClonesArray(fTracksBranchName, 100); //out
    ioman->Register("BmnGemTrack", "GEM", fGemTracksArray, kTRUE);

    fField = FairRunAna::Instance()->GetField();
    if (!fField) Fatal("Init", "No Magnetic Field found");

    fAddresses = new Int_t*[fNBins];
    for (Int_t i = 0; i < fNBins; ++i) {
        fAddresses[i] = new Int_t[fNBins];
        for (Int_t j = 0; j < fNBins; ++j)
            fAddresses[i][j] = -1;
    }

    TString gPathConfig = gSystem->Getenv("VMCWORKDIR");
    TString gPathGemConfig = gPathConfig + "/gem/XMLConfigs/";
    TString confGem = (fPeriodId == 7) ? "GemRunSpring2018.xml" : (fPeriodId == 6) ? "GemRunSpring2017.xml" : "GemRunSpring2017.xml";
    if (confGem == "") {
        printf(ANSI_COLOR_RED "No GEM geometry defined!\n" ANSI_COLOR_RESET);
        throw;
    }
    fGemDetector = new BmnGemStripStationSet(gPathGemConfig + confGem);

    if (fSteering->GetNStations() != fGemDetector->GetNStations()) {
        cout << "Something wrong with steering file! Please check number of GEM-stations!" << endl;
        throw;
    }

    fHitsOnStation = new vector<Int_t>[fGemDetector->GetNStations()];
    fGemDistCut = new Double_t[fGemDetector->GetNStations()];
    for (Int_t iSt = 0; iSt < fGemDetector->GetNStations(); ++iSt)
        fGemDistCut[iSt] = fSteering->GetGemDistCut()[iSt];

    if (fSteering->IsRoughVertexApproxUsed())
        fRoughVertex.SetXYZ(fSteering->GetRoughVertex().X(), fSteering->GetRoughVertex().Y(), fSteering->GetRoughVertex().Z());

    if (fVerbose) cout << "======================== GEM tracking init finished ===================" << endl;

    return kSUCCESS;
}

void BmnGemTracking::Exec(Option_t* opt) {

    if (fVerbose) cout << "\n======================== GEM tracking exec started ====================" << endl;
    if (fVerbose) cout << "\n Event number: " << fEventNo << endl;

    if (!IsActive())
        return;

    clock_t tStart = clock();

    fGemTracksArray->Delete();
    for (Int_t iSt = 0; iSt < fGemDetector->GetNStations(); ++iSt)
        fHitsOnStation[iSt].clear();

    fEventNo++;

    fYstep = fSteering->GetYStep();
    fLineFitCut = fSteering->GetLineFitCut();

    for (Int_t i = 0; i < fNBins; ++i)
        for (Int_t j = 0; j < fNBins; ++j)
            fAddresses[i][j] = -1;

    if (fGemHitsArray->GetEntriesFast() > fNHitsInGemCut || fGemHitsArray->GetEntriesFast() == 0) return;

    FillAddrWithLorentz();

    for (Int_t iHit = 0; iHit < fGemHitsArray->GetEntriesFast(); ++iHit) {
        BmnGemStripHit* hit = GetHit(iHit);
        if (!hit) continue;
        if (!hit->GetFlag()) continue;
        fHitsOnStation[hit->GetStation()].push_back(iHit);
    }

    for (Int_t j = 0;; j++) {
        vector<BmnGemTrack> seeds;
        for (Int_t i = 0; i < fNBins / fYstep; ++i) FindSeedsByCombinatoricsInCoridor(i, seeds);
        //        printf("N seeds = %d\n", seeds.size());
        if (seeds.size() < 1 || seeds.size() > fNSeedsCut) break;
        FitSeeds(seeds);
        Int_t br = Tracking(seeds);
        //        printf("N tracks = %d\n", br);
        if (br == 0) break;
        fYstep *= fSteering->GetCoeffYStep();
        fLineFitCut *= fSteering->GetCoeffLineFitCut();
        //                fGemDistCut *= fSteering->GetCoeffGemDistCut();
    }

    clock_t tFinish = clock();
    if (fVerbose) cout << "GEM_TRACKING: Number of found tracks: " << fGemTracksArray->GetEntriesFast() << endl;

    workTime += ((Double_t) (tFinish - tStart)) / CLOCKS_PER_SEC;

    if (fVerbose) cout << "\n======================== GEM tracking exec finished ===================" << endl;

}

void BmnGemTracking::Finish() {
    ofstream outFile;
    outFile.open("QA/timing.txt");
    outFile << "Track Finder Time: " << workTime << endl;
    cout << "Full work time of the GEM tracking: " << workTime << endl;

    TCanvas* c1 = new TCanvas("c1", "c1", 1000, 500);
    c1->Divide(2, 1);
    c1->cd(1);
    h_Dist->Draw();
    c1->cd(2);
    h_Chi->Draw();
    c1->SaveAs("tmp.pdf");

    delete fSteering;
}

Int_t BmnGemTracking::Tracking(vector<BmnGemTrack>& seeds) {
    Int_t num_of_tracks = 0;
    vector<BmnGemTrack> tracks;
    vector<BmnGemTrack> sortedTracks;
    for (Int_t iTr = 0; iTr < seeds.size(); ++iTr) {
        BmnGemTrack* gemTrack = &seeds[iTr];
        if (gemTrack->GetFlag() == -1) continue;
        BmnGemTrack track;
        track.SetParamFirst(*(gemTrack->GetParamFirst()));
        track.SetParamLast(*(gemTrack->GetParamLast()));

        vector<BmnFitNode> nodes(fGemDetector->GetNStations());
        track.SetFitNodes(nodes);

        BmnGemStripHit* hit0 = GetHit(gemTrack->GetHitIndex(0));
        BmnGemStripHit* hit1 = GetHit(gemTrack->GetHitIndex(1));
        BmnGemStripHit* hit2 = GetHit(gemTrack->GetHitIndex(2));
        if (!hit0 || !hit1 || !hit2) continue;
        track.AddHit(gemTrack->GetHitIndex(0), hit0);
        track.AddHit(gemTrack->GetHitIndex(1), hit1);
        track.AddHit(gemTrack->GetHitIndex(2), hit2);
        track.SortHits();

        Bool_t doBranching = kFALSE; //kTRUE; // if false then go by using Nearest Neighbor method
        if (doBranching) {
            BranchHitMerge(&track);
            tracks.push_back(track);
        } else {
            Bool_t wasSkipped = kFALSE; //no skipped stations
            for (Int_t iSt = hit2->GetStation() + 1; iSt < fGemDetector->GetNStations(); ++iSt) {
                if (track.GetFlag() == -1) break;
                NearestHitMerge(iSt, &track, wasSkipped);
            }
            for (Int_t iSt = hit0->GetStation() - 1; iSt >= 0; iSt--) {
                if (track.GetFlag() == -1) break;
                NearestHitMerge(iSt, &track, wasSkipped);
            }

            if (track.GetFlag() != -1 && track.GetNHits() >= fNHitsCut && (track.GetChi2() / track.GetNDF()) < fLineFitCut) {
                //            CalculateLength(&track);
                tracks.push_back(track);
            }
        }
    }

    SortTracks(tracks, sortedTracks);
    if (fUseRefit)
        for (Int_t iTrack = 0; iTrack < sortedTracks.size(); iTrack++)
            RefitTrack(&sortedTracks[iTrack]);

    if (!fIsTarget) {
        if (sortedTracks.size() != 0)
            new((*fGemTracksArray)[fGemTracksArray->GetEntriesFast()]) BmnGemTrack(sortedTracks.at(0));
    } else {
        CheckSharedHits(sortedTracks);
        for (Int_t iTr = 0; iTr < sortedTracks.size(); ++iTr) {
            if (sortedTracks[iTr].GetFlag() != -1 && IsParCorrect(sortedTracks[iTr].GetParamFirst(), fIsField) && IsParCorrect(sortedTracks[iTr].GetParamLast(), fIsField)) {
                CalculateLength(&sortedTracks[iTr]);
                new((*fGemTracksArray)[fGemTracksArray->GetEntriesFast()]) BmnGemTrack(sortedTracks[iTr]);
                SetHitsUsing(&sortedTracks[iTr], kTRUE);
                num_of_tracks++;
            }
        }
    }
    return num_of_tracks;
}

BmnStatus BmnGemTracking::SortTracks(vector<BmnGemTrack*>& tracks) { //FIXME!!! MEMORY LEAK! Сделать с двумя параметрами (входной и выходной векторы). Или даже исользовать функцию ниже.
    Bool_t isChi2Sort = fSteering->IsChi2SortUsed();
    Bool_t isNHitsSort = fSteering->IsNHitsSortUsed();

    if ((isChi2Sort && isNHitsSort) || (!isChi2Sort && !isNHitsSort)) {
        cout << "Something wrong with steering file!" << endl;
        throw;
    }

    if (isChi2Sort) {
        multimap <Float_t, BmnGemTrack*> sortedTracksMap; //map<Chi2,BmnGemTrack>
        for (Int_t iTr = 0; iTr < tracks.size(); ++iTr)
            sortedTracksMap.insert(pair<Float_t, BmnGemTrack*>(tracks.at(iTr)->GetChi2(), tracks.at(iTr)));
        tracks.clear();
        for (auto it : sortedTracksMap)
            tracks.push_back(it.second);
        sortedTracksMap.clear();
    } else if (isNHitsSort) {
        multimap <Int_t, BmnGemTrack*> sortedTracksMap; //map<nHits,BmnGemTrack>
        for (Int_t iTr = 0; iTr < tracks.size(); ++iTr)
            sortedTracksMap.insert(pair<Int_t, BmnGemTrack*>(tracks.at(iTr)->GetNHits(), tracks.at(iTr)));
        tracks.clear();
        multimap<Int_t, BmnGemTrack*>::reverse_iterator it;
        for (it = sortedTracksMap.rbegin(); it != sortedTracksMap.rend(); ++it)
            tracks.push_back(it->second);
        sortedTracksMap.clear();
    }

    //    const Int_t n = fGemDetector->GetNStations() - fNHitsCut + 1; //3 for geometry 2018 (4, 5, 6)
    //    multimap<Float_t, BmnGemTrack*> sortedMap[n]; // array of map<Chi2,trIdx>. Each element of array corresponds fixed number of hits on track (4, 5, 6) 
    //    for (Int_t iTr = 0; iTr < tracks.size(); ++iTr) {
    //        if (tracks.at(iTr)->GetNHits() < fNHitsCut) continue; //FIXME! Why nHits < cut???
    //        if (tracks.at(iTr)->GetChi2() < 0) continue; //FIXME! Why chi2 < 0???
    //        sortedMap[tracks.at(iTr)->GetNHits() - fNHitsCut].insert(pair<Float_t, BmnGemTrack*>(tracks.at(iTr)->GetChi2(), tracks.at(iTr)));
    //    }
    //    tracks.clear();
    //
    //    for (Int_t i = n - 1; i >= 0; i--) {
    //        for (auto it : sortedMap[i])
    //            tracks.push_back(it.second);
    //        sortedMap[i].clear();
    //    }

}

BmnStatus BmnGemTracking::SortTracks(vector<BmnGemTrack>& inTracks, vector<BmnGemTrack>& sortedTracks) {
    //    Bool_t isChi2Sort = fSteering->IsChi2SortUsed();
    //    Bool_t isNHitsSort = fSteering->IsNHitsSortUsed();
    //
    //    if ((isChi2Sort && isNHitsSort) || (!isChi2Sort && !isNHitsSort)) {
    //        cout << "Something wrong with steering file!" << endl;
    //        throw;
    //    }
    //
    //    if (isChi2Sort) {
    //        multimap <Float_t, Int_t> sortedTracksMap; //map<Chi2,trIdx>
    //        for (Int_t iTr = 0; iTr < inTracks.size(); ++iTr)
    //            sortedTracksMap.insert(pair<Float_t, Int_t>(inTracks.at(iTr).GetChi2() / inTracks.at(iTr).GetNDF(), iTr));
    //
    //        for (auto it : sortedTracksMap)
    //            sortedTracks.push_back(inTracks.at(it.second));
    //    } else if (isNHitsSort) {
    //        multimap <Int_t, Int_t> sortedTracksMap; //map<nHits,trIdx>
    //        for (Int_t iTr = 0; iTr < inTracks.size(); ++iTr)
    //            sortedTracksMap.insert(pair<Int_t, Int_t>(inTracks.at(iTr).GetNHits(), iTr));
    //
    //        multimap<Int_t, Int_t>::reverse_iterator it;
    //        for (it = sortedTracksMap.rbegin(); it != sortedTracksMap.rend(); ++it)
    //            sortedTracks.push_back(inTracks.at(it->second));
    //    }
    const Int_t n = fGemDetector->GetNStations() - fNHitsCut + 1; //3 for geometry 2018 (4, 5, 6)
    multimap<Float_t, Int_t> sortedMap[n]; // array of map<Chi2,trIdx>. Each element of array corresponds fixed number of hits on track (4, 5, 6) 
    for (Int_t iTr = 0; iTr < inTracks.size(); ++iTr) {
        if (inTracks.at(iTr).GetNHits() < fNHitsCut) continue; //FIXME! Why nHits < cut???
        if (inTracks.at(iTr).GetChi2() < 0) continue; //FIXME! Why chi2 < 0???
        sortedMap[inTracks.at(iTr).GetNHits() - fNHitsCut].insert(pair<Float_t, Int_t>(inTracks.at(iTr).GetChi2(), iTr));
    }

    for (Int_t i = n - 1; i >= 0; i--) {
        for (auto it : sortedMap[i])
            sortedTracks.push_back(inTracks.at(it.second));
        sortedMap[i].clear();
    }
}

BmnStatus BmnGemTracking::CheckSharedHits(vector<BmnGemTrack>& sortedTracks) {

    set<Int_t> hitsId;

    const Int_t kNSharedHits = fSteering->GetNSharedHits();

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

void BmnGemTracking::FillAddrWithLorentz() {

    //Needed for searching seeds by addresses 
    Double_t sigma_x2 = fSigX * fSigX;

    for (Int_t hitIdx = 0; hitIdx < fGemHitsArray->GetEntriesFast(); ++hitIdx) {
        BmnGemStripHit* hit = GetHit(hitIdx);
        if (!hit) continue;

        hit->SetFlag(kFALSE); // by default hits are not filtered 
        if (fSteering->IsHitErrorsScaleUsed()) {
            Double_t xScale = fSteering->GetErrorScales()[0];
            Double_t yScale = fSteering->GetErrorScales()[1];
            Double_t zScale = fSteering->GetErrorScales()[2];
            hit->SetDxyz(hit->GetDx() * xScale, hit->GetDy() * yScale, zScale); // just for test
        }


        TVector2 XYnew = GetTransXY(hit);

        Int_t xAddr = ceil((XYnew.X() - fMin) / fWidth);
        Int_t yAddr = ceil((XYnew.Y() - fMin) / fWidth);
        ULong_t addr = yAddr * fNBins + xAddr;

        hit->SetAddr(addr);
        hit->SetXaddr(xAddr);
        hit->SetYaddr(yAddr);
        hit->SetIndex(hitIdx);
        //        hit->SetFlag(kTRUE);
    }


    for (Int_t hitIdx = 0; hitIdx < fGemHitsArray->GetEntriesFast(); ++hitIdx) {
        BmnGemStripHit* hit = GetHit(hitIdx);
        if (!hit) continue;

        Bool_t skip = kFALSE;
        for (Int_t iSkip = 0; iSkip < skipStations.size(); ++iSkip)
            if (hit->GetStation() == skipStations.at(iSkip)) {
                skip = kTRUE;
                break;
            }
        if (skip) continue;


        Int_t yAddr = hit->GetYaddr();
        Int_t xAddr = hit->GetXaddr();
        if (yAddr < 0 || yAddr >= fNBins || xAddr < 0 || xAddr >= fNBins) {
            continue;
        }

        const Double_t newX = GetTransXY(hit).X();

        Double_t potSum = 0.0; //sum of all potentials
        for (Int_t hitIdx0 = 0; hitIdx0 < fGemHitsArray->GetEntriesFast(); ++hitIdx0) {
            BmnGemStripHit* hit0 = GetHit(hitIdx0);
            if (!hit0) continue;
            Int_t yAddr0 = hit0->GetYaddr();
            Int_t xAddr0 = hit0->GetXaddr();
            if (yAddr0 < 0 || yAddr0 >= fNBins || xAddr0 < 0 || xAddr0 >= fNBins) continue;
            if (Abs(yAddr0 - yAddr) > fYstep) continue; //hits should be in the same Y-coridor

            const Double_t newX0 = GetTransXY(hit0).X();
            Double_t pot = sigma_x2 / (sigma_x2 + Sq(newX0 - newX));
            potSum += pot;
        }
        if (potSum >= fLorentzThresh) {
            hit->SetFlag(kTRUE);
            fAddresses[xAddr][yAddr] = hitIdx;
        }
    }
}

TVector2 BmnGemTracking::CalcMeanSigma(vector <Double_t> QpSegm) {
    Double_t QpSum = 0.;
    for (Int_t iSegm = 0; iSegm < QpSegm.size(); iSegm++)
        QpSum += QpSegm[iSegm];

    Double_t QpMean = QpSum / QpSegm.size();

    Double_t sqSigmaSum = 0.;
    for (Int_t iSegm = 0; iSegm < QpSegm.size(); iSegm++)
        sqSigmaSum += Sq(QpSegm[iSegm] - QpMean);

    return TVector2(QpMean, Sqrt(sqSigmaSum / QpSegm.size()));
}

BmnStatus BmnGemTracking::RefitTrack(BmnGemTrack * track) {
    Bool_t fRobustRefit = true;
    Bool_t fSimpleRefit = false;
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

    track->GetParamFirst()->SetQp(QpRefit); // set new q/p after refit done
    return kBMNSUCCESS;
}

BmnStatus BmnGemTracking::FindSeedsByCombinatoricsInCoridor(Int_t iCorridor, vector<BmnGemTrack>& cand) {

    vector<Int_t> hitsOnStation[fGemDetector->GetNStations()];

    for (Int_t iHit = 0; iHit < fGemHitsArray->GetEntriesFast(); ++iHit) {
        BmnGemStripHit* hit = GetHit(iHit);
        if (!hit) continue;
        if (!hit->GetFlag()) continue;
        if (hit->GetYaddr() > fYstep * iCorridor || hit->GetYaddr() < fYstep * (iCorridor - 1)) continue;
        for (Int_t iSt = 0; iSt < fGemDetector->GetNStations(); ++iSt)
            if (hit->GetStation() == iSt) hitsOnStation[iSt].push_back(iHit);
    }

    for (Int_t iTrio = 0; iTrio < fSteering->GetNCombs(); iTrio++) {
        Int_t st0 = fSteering->GetStatsForSeeding()[iTrio][0];
        Int_t st1 = fSteering->GetStatsForSeeding()[iTrio][1];
        Int_t st2 = fSteering->GetStatsForSeeding()[iTrio][2];
        // cout << "iTrio = " << iTrio << " " << st0 << " " << st1 << " " << st2 << endl;
        SeedsByThreeStations(st0, st1, st2, hitsOnStation, cand);
    }

    return kBMNSUCCESS;
}

BmnStatus BmnGemTracking::FitSeeds(vector<BmnGemTrack>& cand) {
    for (Int_t i = 0; i < cand.size(); ++i) {
        BmnGemTrack* trackCand = &(cand[i]);
        if (fIsField) {
            if (CalculateTrackParamsCircle(trackCand) == kBMNERROR) {
                //if (CalculateTrackParamsPol2(trackCand) == kBMNERROR) {
                trackCand->SetFlag(-1);
                continue;
            }
        } else {
            if (CalculateTrackParamsLine(trackCand) == kBMNERROR) {
                trackCand->SetFlag(-1);
                continue;
            }
        }
    }
    return kBMNSUCCESS;
}

BmnStatus BmnGemTracking::CalculateTrackParamsPol2(BmnGemTrack * tr) {

    const UInt_t nHits = tr->GetNHits();
    BmnGemStripHit* lastHit = GetHit(tr->GetHitIndex(nHits - 1));
    BmnGemStripHit* firstHit = GetHit(tr->GetHitIndex(0));
    BmnGemStripHit* midHit = GetHit(tr->GetHitIndex(1));
    if (!firstHit || !lastHit || !midHit) return kBMNERROR;
    if (!lastHit->GetFlag()) return kBMNERROR;
    if (!firstHit->GetFlag()) return kBMNERROR;
    if (!midHit->GetFlag()) return kBMNERROR;

    TVector3 lineParZY = LineFit(tr, fGemHitsArray, "ZY");
    if (lineParZY.Z() > fLineFitCut) return kBMNERROR;

    TVector3 Pol2ParZX = Pol2By3Hit(tr, fGemHitsArray);
    tr->SetChi2(0.0);
    tr->SetNDF(0);

    const Double_t A = Pol2ParZX.X();
    const Double_t B = Pol2ParZX.Y();
    const Double_t C = Pol2ParZX.Z();

    const Double_t R = Power(1 - (2 * A * firstHit->GetZ() + B) * (2 * A * firstHit->GetZ() + B), 1.5) / Abs(2 * A); //radii of trajectory in the first hit position 
    // printf("R = %f\n", R);
    fField = FairRunAna::Instance()->GetField();
    const Double_t Q = (A > 0) ? +1 : -1;

    Double_t fSum = 0.0;
    Double_t minField = FLT_MAX;
    Double_t maxField = -FLT_MAX;
    UInt_t nOk = 0;
    for (UInt_t i = 0; i < nHits; ++i) {
        BmnGemStripHit* hit = GetHit(tr->GetHitIndex(i));
        if (!hit) continue;
        if (!hit->GetFlag()) continue;
        Double_t f = Abs(fField->GetBy(hit->GetX(), hit->GetY(), hit->GetZ()));
        if (f < minField) minField = f;
        if (f > maxField) maxField = f;
        fSum += f;
        nOk++;
    }

    fSum /= nOk;

    // Double_t S = 0.0003 * fSum;
    // Double_t S = 0.0003 * maxField;
    // Double_t S = 0.0003 * minField;
    Double_t S = 0.0003 * Abs(fField->GetBy(lastHit->GetX(), lastHit->GetY(), lastHit->GetZ()));
    // Double_t S = 0.0003 * Abs(fField->GetBy(midHit->GetX(), midHit->GetY(), midHit->GetZ()));
    // Double_t S = 0.0003 * Abs(fField->GetBy(firstHit->GetX(), firstHit->GetY(), firstHit->GetZ()));
    // Double_t QP = Q / S / Sqrt(R * R + B * B);

    Double_t QP = Q / S / R;
    //printf("S = %f \t R = %f \t P = %f\n", S / 0.0003, R, 1 / QP);

    FairTrackParam par;
    par.SetCovariance(0, 0, 1e-5);
    par.SetCovariance(0, 1, 1e-5);
    par.SetCovariance(0, 2, 1e-5);
    par.SetCovariance(0, 3, 1e-5);
    par.SetCovariance(0, 4, 1e-5);
    par.SetCovariance(1, 1, 1e-5);
    par.SetCovariance(1, 2, 1e-5);
    par.SetCovariance(1, 3, 1e-5);
    par.SetCovariance(1, 4, 1e-5);
    par.SetCovariance(2, 2, 1e-5);
    par.SetCovariance(2, 3, 1e-5);
    par.SetCovariance(2, 4, 1e-5);
    par.SetCovariance(3, 3, 1e-5);
    par.SetCovariance(3, 4, 1e-5);
    par.SetCovariance(4, 4, 1e-5);

    Double_t lX = lastHit->GetX();
    Double_t lY = lastHit->GetY();
    Double_t lZ = lastHit->GetZ();

    Double_t fX = firstHit->GetX();
    Double_t fY = firstHit->GetY();
    Double_t fZ = firstHit->GetZ();

    Double_t Tx_first = 2 * A * fZ + B;
    Double_t Tx_last = 2 * A * lZ + B;
    Double_t Ty_last = lineParZY.X();
    Double_t Ty_first = lineParZY.X();

    if (Ty_first * (fY - fRoughVertex.Y()) < 0) return kBMNERROR; //check it
    if (Tx_first * (fX - fRoughVertex.X()) < 0) return kBMNERROR; //check it

    par.SetPosition(TVector3(lX, lY, lZ));
    par.SetTx(Tx_last);
    par.SetTy(Ty_last);
    par.SetQp(QP);
    tr->SetParamLast(par);
    if (!IsParCorrect(&par, fIsField)) return kBMNERROR;

    //update for firstParam
    par.SetPosition(TVector3(fX, fY, fZ));
    par.SetQp(QP);
    par.SetTx(Tx_first);
    par.SetTy(Ty_first);
    tr->SetParamFirst(par);
    if (!IsParCorrect(&par, fIsField)) return kBMNERROR;

    return kBMNSUCCESS;
}

BmnStatus BmnGemTracking::CalcCovMatrix(BmnGemTrack * tr) {

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
        BmnGemStripHit* hit = GetHit(tr->GetHitIndex(i));
        if (!hit) continue;
        if (!hit->GetFlag()) continue;
        Double_t Xi = hit->GetX();
        Double_t Yi = hit->GetY();
        Double_t Zi = hit->GetZ();

        Double_t Txi = -1.0 * (Zi - Zc) / (Xi - Xc);
        Double_t Tyi = B;// / (Xi - Xc);
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

BmnStatus BmnGemTracking::CalculateTrackParamsCircle(BmnGemTrack * tr) {

    //Estimation of track parameters for events with magnetic field
    const UInt_t nHits = tr->GetNHits();
    //    if (nHits < fNHitsCut) return kBMNERROR;
    //cout << "I'm trying to get hit index!!! N_hits = " << nHits <<  endl;
    BmnGemStripHit* lastHit = GetHit(tr->GetHitIndex(nHits - 1));
    BmnGemStripHit* firstHit = GetHit(tr->GetHitIndex(0));
    BmnGemStripHit* midHit = GetHit(tr->GetHitIndex(1));
    if (!firstHit || !lastHit || !midHit) return kBMNERROR;
    if (!lastHit->GetFlag()) return kBMNERROR;
    if (!firstHit->GetFlag()) return kBMNERROR;
    if (!midHit->GetFlag()) return kBMNERROR;

    TVector3 lineParZY = LineFit(tr, fGemHitsArray, "ZY");
    Double_t chi2line = lineParZY.Z();
    if (chi2line > fLineFitCut) return kBMNERROR;
    Double_t chi2circ = 0.0;
    TVector3 CircParZX = (nHits == 3) ? CircleBy3Hit(tr, fGemHitsArray) : CircleFit(tr, fGemHitsArray, chi2circ);
    tr->SetChi2(chi2circ + chi2line);
    tr->SetNDF(nHits - 3);

    Double_t R = CircParZX.Z(); // radius of fit-circle
    Double_t Xc = CircParZX.Y(); // x-coordinate of fit-circle center
    Double_t Zc = CircParZX.X(); // z-coordinate of fit-circle center
    fField = FairRunAna::Instance()->GetField();

    const Double_t B = lineParZY.X(); //angle coefficient for helicoid

    Double_t Q = (Xc > 0) ? +1 : -1;

    Double_t fSum = 0.0;
    UInt_t nOk = 0;
    for (UInt_t i = 0; i < nHits; ++i) {
        BmnGemStripHit* hit = GetHit(tr->GetHitIndex(i));
        if (!hit) continue;
        if (!hit->GetFlag()) continue;
        fSum += Abs(fField->GetBy(hit->GetX(), hit->GetY(), hit->GetZ()));
        nOk++;
    }
    fSum /= nOk;

    Double_t S = 0.0003 * fSum;

    Double_t Pt = S * R; //actually Pt/Q, but it doesn't matter
    Double_t lX = lastHit->GetX();
    Double_t lY = lastHit->GetY();
    Double_t lZ = lastHit->GetZ();

    Double_t fX = firstHit->GetX();
    Double_t fY = firstHit->GetY();
    Double_t fZ = firstHit->GetZ();

    Double_t h = -1.0;

    Double_t Tx_first = h * (fZ - Zc) / (fX - Xc);
    Double_t Tx_last = h * (lZ - Zc) / (lX - Xc);
    Double_t Ty_last = B;// / (lX - Xc);
    Double_t Ty_first = B;// / (fX - Xc);

    Double_t Pz = Pt / Sqrt(1 + Sq(Tx_first));
    Double_t Px = Tx_first * Pz;
    Double_t Py = Ty_first * Pz;
    Double_t P = Sqrt(Sq(Px) + Sq(Py) + Sq(Pz));
    Double_t QP = Q / P;

    CalcCovMatrix(tr);

    tr->GetParamFirst()->SetPosition(TVector3(fX, fY, fZ));
    tr->GetParamFirst()->SetQp(QP);
    tr->GetParamFirst()->SetTx(Tx_first);
    tr->GetParamFirst()->SetTy(Ty_first);
    tr->GetParamLast()->SetPosition(TVector3(lX, lY, lZ));
    tr->GetParamLast()->SetTx(Tx_last);
    tr->GetParamLast()->SetTy(Ty_last);
    tr->GetParamLast()->SetQp(QP);

    return kBMNSUCCESS;
}

BmnStatus BmnGemTracking::CalculateTrackParamsLine(BmnGemTrack * tr) {

    //Estimation of track parameters for events w/o magnetic field
    const UInt_t nHits = tr->GetNHits();
    BmnGemStripHit* lastHit = GetHit(tr->GetHitIndex(nHits - 1));
    BmnGemStripHit* firstHit = GetHit(tr->GetHitIndex(0));
    if (!firstHit || !lastHit) return kBMNERROR;
    if (!lastHit->GetFlag()) return kBMNERROR;
    if (!firstHit->GetFlag()) return kBMNERROR;

    TVector3 lineParZY = LineFit(tr, fGemHitsArray, "ZY");
    if (lineParZY.Z() > fLineFitCut) return kBMNERROR;
    TVector3 lineParZX = LineFit(tr, fGemHitsArray, "ZX");
    if (lineParZX.Z() > fLineFitCut) return kBMNERROR;

    Float_t lX = lastHit->GetX();
    Float_t lY = lastHit->GetY();
    Float_t lZ = lastHit->GetZ();

    Float_t fX = firstHit->GetX();
    Float_t fY = firstHit->GetY();
    Float_t fZ = firstHit->GetZ();

    FairTrackParam parF;
    parF.SetPosition(TVector3(lineParZX.X() * fZ + lineParZX.Y(), lineParZY.X() * fZ + lineParZY.Y(), fZ));
    parF.SetQp(0.0);
    parF.SetTx(lineParZX.X());
    parF.SetTy(lineParZY.X());

    const Float_t cov_const = 1e-15;
    parF.SetCovariance(0, 0, cov_const);
    parF.SetCovariance(0, 1, cov_const);
    parF.SetCovariance(0, 2, cov_const);
    parF.SetCovariance(0, 3, cov_const);
    parF.SetCovariance(0, 4, cov_const);
    parF.SetCovariance(1, 1, cov_const);
    parF.SetCovariance(1, 2, cov_const);
    parF.SetCovariance(1, 3, cov_const);
    parF.SetCovariance(1, 4, cov_const);
    parF.SetCovariance(2, 2, cov_const);
    parF.SetCovariance(2, 3, cov_const);
    parF.SetCovariance(2, 4, cov_const);
    parF.SetCovariance(3, 3, cov_const);
    parF.SetCovariance(3, 4, cov_const);
    parF.SetCovariance(4, 4, cov_const);

    FairTrackParam parL;
    parL.SetPosition(TVector3(lineParZX.X() * lZ + lineParZX.Y(), lineParZY.X() * lZ + lineParZY.Y(), lZ));
    parL.SetTx(lineParZX.X());
    parL.SetTy(lineParZY.X());
    parL.SetQp(0.0);

    parL.SetCovariance(0, 0, cov_const);
    parL.SetCovariance(0, 1, cov_const);
    parL.SetCovariance(0, 2, cov_const);
    parL.SetCovariance(0, 3, cov_const);
    parL.SetCovariance(0, 4, cov_const);
    parL.SetCovariance(1, 1, cov_const);
    parL.SetCovariance(1, 2, cov_const);
    parL.SetCovariance(1, 3, cov_const);
    parL.SetCovariance(1, 4, cov_const);
    parL.SetCovariance(2, 2, cov_const);
    parL.SetCovariance(2, 3, cov_const);
    parL.SetCovariance(2, 4, cov_const);
    parL.SetCovariance(3, 3, cov_const);
    parL.SetCovariance(3, 4, cov_const);
    parL.SetCovariance(4, 4, cov_const);

    tr->SetParamLast(parL);
    tr->SetParamFirst(parF);
    tr->SetB(Sqrt(fX * fX + fY * fY));
    //tr->SetLength(Sqrt((fX - lX) * (fX - lX) + (fY - lY) * (fY - lY) + (fZ - lZ) * (fZ - lZ)));
    //    tr->SetChi2(Sqrt(lineParZX.Z() * lineParZX.Z() + lineParZY.Z() * lineParZY.Z())); //FIXME! Is it OK or we should use MAX of two chi2? Or maybe MIN?
    tr->SetChi2(lineParZX.Z());
    tr->SetNDF(nHits - 2); // -2 because of line fit (2 params)

    return kBMNSUCCESS;
}

void BmnGemTracking::SetHitsUsing(BmnGemTrack* tr, Bool_t use) {
    for (Int_t i = 0; i < tr->GetNHits(); ++i) {
        BmnGemStripHit* hit = GetHit(tr->GetHitIndex(i));
        if (hit) hit->SetUsing(use);
    }
}

TVector2 BmnGemTracking::GetTransXY(BmnGemStripHit * hit) {
    //const Double_t oneOverR = 1.0 / Sqrt(Sq(hit->GetX()) + Sq(hit->GetY()) + Sq(hit->GetZ()));
    const Double_t oneOverR = 1.0 / Sqrt(Sq(hit->GetX() - fRoughVertex.X()) + Sq(hit->GetY() - fRoughVertex.Y()) + Sq(hit->GetZ() - fRoughVertex.Z()));
    //    const Double_t oneOverR = 1.0 / hit->GetZ();
    return TVector2((hit->GetX() - fRoughVertex.X()) * oneOverR, (hit->GetY() - fRoughVertex.Y()) * oneOverR);
}

BmnStatus BmnGemTracking::SeedsByThreeStations(Int_t i0, Int_t i1, Int_t i2, vector<Int_t>* hits, vector<BmnGemTrack>& cand) {
    for (Int_t iHit0 = 0; iHit0 < hits[i0].size(); ++iHit0) {
        BmnGemStripHit* hit0 = GetHit(hits[i0].at(iHit0));
        for (Int_t iHit1 = 0; iHit1 < hits[i1].size(); ++iHit1) {
            BmnGemStripHit* hit1 = GetHit(hits[i1].at(iHit1));
            for (Int_t iHit2 = 0; iHit2 < hits[i2].size(); ++iHit2) {
                BmnGemStripHit* hit2 = GetHit(hits[i2].at(iHit2));
                BmnGemTrack trCnd;
                trCnd.AddHit(hits[i0].at(iHit0), hit0);
                trCnd.AddHit(hits[i1].at(iHit1), hit1);
                trCnd.AddHit(hits[i2].at(iHit2), hit2);
                trCnd.SortHits();
                TVector3 lineParZY = LineFit(&trCnd, fGemHitsArray, "ZY");
                if (lineParZY.Z() > fLineFitCut) continue;
                if (cand.size() == fNSeedsCut) return kBMNERROR;
                //fNFoundSeeds++;
                cand.push_back(trCnd);
            }
        }
    }
}

BmnGemStripHit * BmnGemTracking::GetHit(Int_t i) {
    BmnGemStripHit* hit = (BmnGemStripHit*) fGemHitsArray->At(i);
    if (!hit) return NULL;
    if (hit->IsUsed()) return NULL;
    return hit;
}

BmnStatus BmnGemTracking::NearestHitMerge(UInt_t station, BmnGemTrack* track, Bool_t & wasSkipped) {

    BmnHit* minHit = NULL; // Pointer to hit with minimum chi-square
    Double_t minDist = FLT_MAX;
    Double_t minChiSq = DBL_MAX;
    Int_t minIdx = -1;
    Double_t dist = 0.0;
    FairTrackParam minParPred; // predicted track parameters for closest hit
    Double_t stationZ = fGemDetector->GetGemStation(station)->GetModule(0)->GetZPositionRegistered();
    //    FairTrackParam parPredict = (fGoForward) ? (*(track->GetParamFirst())) : (*(track->GetParamLast()));
    FairTrackParam parPredict = *(track->GetParamFirst());
    Bool_t goForward = (parPredict.GetZ() < stationZ);
    if (fKalman->TGeoTrackPropagate(&parPredict, stationZ, fPDG, NULL, NULL, fIsField) == kBMNERROR) return kBMNERROR;

    for (Int_t iHit = 0; iHit < fHitsOnStation[station].size(); ++iHit) {
        BmnGemStripHit* hit = (BmnGemStripHit*) GetHit(fHitsOnStation[station].at(iHit));
        if (!hit) continue;
        if (!hit->GetFlag()) continue;
        dist = Sqrt(Sq(parPredict.GetX() - hit->GetX()) + Sq(parPredict.GetY() - hit->GetY()));
        if (dist < minDist && dist < fGemDistCut[station]) {
            minDist = dist;
            minHit = hit;
            minIdx = fHitsOnStation[station].at(iHit);
            minParPred = parPredict;
        }
    }

    if (minHit != NULL) {

        Double_t chi = 0.0;
        FairTrackParam parUpdate = parPredict;
        if (fKalman->Update(&parUpdate, minHit, chi) == kBMNERROR) return kBMNERROR;
        if (goForward)
            track->SetParamLast(parUpdate);
        else
            track->SetParamFirst(parUpdate);
        track->SetChi2(Abs(track->GetChi2()) + Abs(chi));
        track->SetNDF(track->GetNDF() + 1);
        track->AddHit(minIdx, minHit);
        track->SortHits();
        BmnFitNode* node = track->GetFitNode(station);
        node->SetUpdatedParam(&parUpdate);
        node->SetPredictedParam(&minParPred);
        return kBMNSUCCESS;
    } else {
        //        if (wasSkipped) track->SetFlag(-1);
        //        wasSkipped = kTRUE;
        return kBMNERROR;
    }
}

BmnStatus BmnGemTracking::BranchHitMerge(BmnGemTrack* startSeed) {
    BmnGemTrack* track = startSeed;
    // Vector with track branches for one input track seed
    vector<BmnGemTrack*> branches;
    // Initially start with one branch which is the same as track seed
    branches.push_back(new BmnGemTrack(*track));
    for (Int_t iStation = 0; iStation < fGemDetector->GetNStations(); iStation++) { // Loop over stations
        Double_t stationZ = fGemDetector->GetGemStation(iStation)->GetModule(0)->GetZPositionRegistered();
        FairTrackParam tpar(*(track->GetParamFirst()));
        if (fKalman->TGeoTrackPropagate(&tpar, stationZ, fPDG, NULL, NULL, fIsField) == kBMNERROR) break;
        Double_t xPred = tpar.GetX();
        Double_t yPred = tpar.GetY();

        Int_t nofBranches = branches.size();
        for (Int_t iBranch = 0; iBranch < nofBranches; iBranch++) { // Loop over branches
            BmnGemTrack* branch = branches[iBranch];
            // Loop over hits
            map<Double_t, pair<Int_t, FairTrackParam> > chiHitPar;

            for (Int_t iHit = 0; iHit < fHitsOnStation[iStation].size(); ++iHit) {
                BmnGemStripHit* hit = (BmnGemStripHit*) GetHit(fHitsOnStation[iStation].at(iHit));
                if (!hit) continue;
                if (!hit->GetFlag()) continue;
                Double_t dist = Sqrt(Sq(xPred - hit->GetX()) + Sq(yPred - hit->GetY()));
                if (dist > fGemDistCut[iStation]) continue; //hit is out of the cut, skip it
                h_Dist->Fill(dist);

                Double_t chi = numeric_limits<Double_t>::max();
                fKalman->Update(&tpar, hit, chi);
                bool hitInValidationGate = (chi < 25.0); //FIXME! move constant into parameters
                if (!hitInValidationGate) continue; // Check if hit is inside validation gate
                chiHitPar[chi] = make_pair(fHitsOnStation[iStation].at(iHit), tpar);
                h_Chi->Fill(chi);
            }
            Int_t nofHitsInValidationGate = chiHitPar.size();
            // Check if hit was added
            if (nofHitsInValidationGate == 1) {
                // Only one hit added, no additional branches are created, just update branch parameters
                const map<Double_t, pair<Int_t, FairTrackParam> >::const_iterator it = chiHitPar.begin();
                Int_t hitId = (*it).second.first;
                BmnGemStripHit* hit = (BmnGemStripHit*) GetHit(hitId);
                branch->AddHit(hitId, hit);
                FairTrackParam tmpPar = (*it).second.second;
                branch->SetParamLast(tmpPar);
                branch->SetChi2(branch->GetChi2() + (*it).first);
                branch->SortHits();
                branch->SetNDF(branch->GetNHits() - 3);
            } else if (nofHitsInValidationGate > 1) {
                // If more than one hit is in the validation gate, than create additional branches.
                // For the first hit use the same branch.
                Int_t counter = 0;
                BmnGemTrack tt(*branch);
                map<Double_t, pair<Int_t, FairTrackParam> >::const_iterator it;
                for (it = chiHitPar.begin(); it != chiHitPar.end(); it++) {
                    // Create new branch starting with the second hit
                    BmnGemTrack* tbranch = (it != chiHitPar.begin()) ? new BmnGemTrack(tt) : branch;
                    if (it != chiHitPar.begin()) branches.push_back(tbranch);
                    Int_t hitId = (*it).second.first;
                    BmnGemStripHit* hit = (BmnGemStripHit*) GetHit(hitId);
                    branch->AddHit(hitId, hit);
                    FairTrackParam tmpPar = (*it).second.second;
                    tbranch->SetParamLast(tmpPar);
                    tbranch->SetChi2(tbranch->GetChi2() + (*it).first);
                    tbranch->SetNDF(branch->GetNHits() - 3);
                    counter++;
                }
            }
        }
    }
    if (branches.size() > 0) {
        // Sort all branches by quality
        SortTracks(branches);
        // Override existing track with the best branch
        if (branches.size() > 0)
            *track = *branches.front();
    }

    for (Int_t i = 0; i < branches.size(); ++i) {
        delete branches.at(i);
    }

    branches.clear();
}

Double_t BmnGemTracking::CalculateLength(BmnGemTrack * tr) {
    if (!tr) return 0.0;

    vector<Double_t> X, Y, Z;
    for (Int_t iGem = 0; iGem < tr->GetNHits(); iGem++) {
        BmnGemStripHit* hit = GetHit(tr->GetHitIndex(iGem));
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
