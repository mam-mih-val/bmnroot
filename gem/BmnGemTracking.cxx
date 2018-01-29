
// This Class' Header ------------------
#include <TMath.h>
#include <TGraph.h>
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
#include "BmnKalmanFilter_tmp.h"
#include "TFitResult.h"
#include "BmnGemStripStationSet_RunWinter2016.h"

//-----------------------------------------
static Double_t workTime = 0.0;
//-----------------------------------------

using namespace std;
using namespace TMath;

BmnGemTracking::BmnGemTracking() {
    fPDG = 2212;
    fEventNo = 0;
    fIsField = kTRUE;
    fIsTarget = kTRUE;
    fGemHitsArray = NULL;
    fXmin = -DBL_MAX;
    fXmax = DBL_MAX;
    fYmin = -DBL_MAX;
    fYmax = DBL_MAX;
    fAddresses = NULL;
    fRoughVertex = TVector3(0.0, 0.0, -21.7);
    fLineFitCut = 0.5;
    fSigX = 0.05;
    fYstep = 3;
    fLorentzThresh = 1.01;
    fNHitsCut = 4;
    fNBins = 1000;
    fMin = -0.5;
    fNSeedsCut = 5000;
    fNHitsInGemCut = 1000;
    fMax = -fMin;
    fWidth = (fMax - fMin) / fNBins;
    fGemDistCut = 5.0;
    fKalman = new BmnKalmanFilter_tmp();
    fGemTracksArray = NULL;
    fField = NULL;
    fGoForward = kTRUE;
    fGemHitsBranchName = "BmnGemStripHit";
    fTracksBranchName = "BmnGemTrack";
    fGemDetector = NULL;
}

BmnGemTracking::~BmnGemTracking() {
    delete fKalman;
    delete fGemDetector;
}

InitStatus BmnGemTracking::Init() {

    if (fVerbose) cout << "======================== GEM tracking init started ====================" << endl;

    //Get ROOT Manager
    FairRootManager* ioman = FairRootManager::Instance();
    if (NULL == ioman) {
        Fatal("Init", "FairRootManager is not instantiated");
    }

    fGemHitsArray = (TClonesArray*) ioman->GetObject(fGemHitsBranchName); //in
    if (!fGemHitsArray)
    {
      cout<<"BmnGemTracking::Init(): branch "<<fGemHitsBranchName<<" not found! Task will be deactivated"<<endl;
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
        for (Int_t j = 0; j < fNBins; ++j) {
            fAddresses[i][j] = -1;
        }
    }

    fGemDetector = new BmnGemStripStationSet_RunSpring2017(BmnGemStripConfiguration::RunSpring2017);
    //    fGemDetector = new BmnGemStripStationSet_RunWinter2016(BmnGemStripConfiguration::RunWinter2016);

    if (fVerbose) cout << "======================== GEM tracking init finished ===================" << endl;

    return kSUCCESS;
}

void BmnGemTracking::Exec(Option_t* opt) {
    if (!IsActive())
        return;

    if (fVerbose) cout << "\n======================== GEM tracking exec started ====================" << endl;
    if (fVerbose) cout << "\n Event number: " << fEventNo << endl;
    fEventNo++;
    clock_t tStart = clock();
    fGemTracksArray->Delete();
    fYstep = 3;
    fGemDistCut = 5.0;
    fLineFitCut = 50.0;

    for (Int_t i = 0; i < fNBins; ++i)
        for (Int_t j = 0; j < fNBins; ++j)
            fAddresses[i][j] = -1;

    if (fGemHitsArray->GetEntriesFast() > fNHitsInGemCut || fGemHitsArray->GetEntriesFast() == 0)
        return;

    FillAddrWithLorentz();

    for (int j = 0;; j++) {
        vector<BmnGemTrack> seeds;
        for (Int_t i = 0; i < fNBins / fYstep; ++i) FindSeedsByCombinatoricsInCoridor(i, seeds);
        if (seeds.size() < 1 || seeds.size() > fNSeedsCut) break;
        FitSeeds(seeds);
        Int_t br = Tracking(seeds);
        //printf("[%d] ==> %d\n", j, br);
        if (br == 0) break;
        fYstep *= 3;
        //fLineFitCut *= 2;
        //fGemDistCut *= 2;
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
    cout << "Work time of the GEM track finder: " << workTime << endl;
}

Int_t BmnGemTracking::Tracking(vector<BmnGemTrack>& seeds) {
    Int_t num_of_tracks = 0;
    vector<BmnGemTrack> tracks;
    for (Int_t iTr = 0; iTr < seeds.size(); ++iTr) {
        BmnGemTrack* gemTrack = &seeds[iTr];
        if (gemTrack->GetFlag() == -1) continue;
        BmnGemTrack track;
        track.SetParamFirst(*(gemTrack->GetParamFirst()));
        track.SetParamLast(*(gemTrack->GetParamLast()));

        vector<BmnFitNode> nodes(fGemDetector->GetNStations());
        track.SetFitNodes(nodes);

        if (fGoForward) {
            BmnGemStripHit* hit0 = GetHit(gemTrack->GetHitIndex(0));
            BmnGemStripHit* hit1 = GetHit(gemTrack->GetHitIndex(1));
            BmnGemStripHit* hit2 = GetHit(gemTrack->GetHitIndex(2));
            if (!hit0 || !hit1 || !hit2) continue;
            track.AddHit(gemTrack->GetHitIndex(0), hit0);
            track.AddHit(gemTrack->GetHitIndex(1), hit1);
            track.AddHit(gemTrack->GetHitIndex(2), hit2);
            track.SortHits();

            for (Int_t iSt = hit2->GetStation() + 1; iSt < fGemDetector->GetNStations(); ++iSt)
                NearestHitMergeGem(iSt, &track);
            for (Int_t iSt = hit0->GetStation() - 1; iSt >= 0; iSt--)
                NearestHitMergeGem(iSt, &track);
        } else {
            BmnGemStripHit* lastHit = GetHit(gemTrack->GetHitIndex(gemTrack->GetNHits() - 1));
            track.AddHit(gemTrack->GetHitIndex(gemTrack->GetNHits() - 1), lastHit);
            for (Int_t iSt = lastHit->GetStation() - 1; iSt >= 0; iSt--)
                NearestHitMergeGem(iSt, &track);
        }

        if (track.GetNHits() >= fNHitsCut) {
            CalculateLength(&track);
            tracks.push_back(track);
        }
    }

    if (!fIsTarget) {
        Double_t minChi = FLT_MAX;
        BmnGemTrack* minTrack = NULL;
        for (Int_t i = 0; i < tracks.size(); ++i) {
            if (tracks.at(i).GetChi2() < minChi) {
                minChi = tracks.at(i).GetChi2();
                minTrack = &tracks.at(i);
            }
        }
        if (minTrack != NULL)
            new((*fGemTracksArray)[fGemTracksArray->GetEntriesFast()]) BmnGemTrack(*minTrack);
    } else {
        for (Int_t iTr = 0; iTr < tracks.size(); ++iTr)
            for (Int_t jTr = iTr + 1; jTr < tracks.size(); ++jTr) {
                Int_t nCopies = 0;
                for (Int_t iHit = 0; iHit < tracks[iTr].GetNHits(); iHit++) {
                    Int_t iHitIdx = tracks[iTr].GetHitIndex(iHit);
                    for (Int_t jHit = 0; jHit < tracks[jTr].GetNHits(); jHit++) {
                        Int_t jHitIdx = tracks[jTr].GetHitIndex(jHit);
                        if (iHitIdx == jHitIdx)
                            nCopies++;
                    }
                }

                if (nCopies > 0) {
                    if (Abs(tracks[iTr].GetChi2()) > Abs(tracks[jTr].GetChi2())) {
                        tracks[iTr].SetFlag(-1);
                        break;
                    } else
                        tracks[jTr].SetFlag(-1);
                }
            }
        for (Int_t iTr = 0; iTr < tracks.size(); ++iTr)
            if (tracks[iTr].GetFlag() != -1 && IsParCorrect(tracks[iTr].GetParamFirst()) && IsParCorrect(tracks[iTr].GetParamLast())) {
                //if (RefitTrack(&tracks[iTr]) == kBMNERROR) continue;
                //                fKalman = new BmnKalmanFilter_tmp();
                //                fKalman->FitSmooth(&tracks[iTr], fGemHitsArray);
                //                delete fKalman;
                new((*fGemTracksArray)[fGemTracksArray->GetEntriesFast()]) BmnGemTrack(tracks[iTr]);
                SetHitsUsing(&tracks[iTr], kTRUE);
                num_of_tracks++;
            }
    }
    return num_of_tracks;
}

void BmnGemTracking::FillAddrWithLorentz() {

    //Needed for searching seeds by addresses 
    Double_t sigma_x2 = fSigX * fSigX;

    for (Int_t hitIdx = 0; hitIdx < fGemHitsArray->GetEntriesFast(); ++hitIdx) {
        BmnGemStripHit* hit = GetHit(hitIdx);
        if (!hit) continue;

        hit->SetFlag(kFALSE); // by default hits are not filtered 
        //hit->SetDxyz(hit->GetDx() * 2, hit->GetDy() * 2, 0.0); // just for test
        //        hit->SetDxyz(1.0, 1.0, 1.0); // just for test

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
                Double_t pot = sigma_x2 / (sigma_x2 + Sqr(newX0 - newX));
                potSum += pot;
            }
            if (potSum >= fLorentzThresh) {
                hit->SetFlag(kTRUE);
                fAddresses[xAddr][yAddr] = hitIdx;
            }
        }
}

BmnStatus BmnGemTracking::RefitTrack(BmnGemTrack* track) {

    const Short_t nHits = track->GetNHits();

    Double_t *xx = new Double_t[nHits];
    Double_t *yy = new Double_t[nHits];

    BmnGemStripHit* firstHit = (BmnGemStripHit*) fGemHitsArray->At(track->GetHitIndex(0));

    Double_t fSum = 0.0;
    Double_t minField = FLT_MAX;
    Double_t maxField = -FLT_MAX;
    UInt_t nOk = 0;
    Double_t A = 0;
    Double_t B = 0;
    Double_t C = 0;

    for (Int_t iHit = 0; iHit < nHits; ++iHit) {
        BmnGemStripHit *hit = (BmnGemStripHit*) fGemHitsArray->At(track->GetHitIndex(iHit));
        if (!hit) continue;
        if (!hit->GetFlag()) continue;
        Double_t f = Abs(fField->GetBy(hit->GetX(), hit->GetY(), hit->GetZ()));
        if (f < minField) minField = f;
        if (f > maxField) maxField = f;
        fSum += f;
        nOk++;
        xx[iHit] = hit->GetZ();
        yy[iHit] = hit->GetX();
    }
    Double_t chi2 = 0.0;
    TVector3 circPar = CircleFit(track, fGemHitsArray, chi2);
    //    printf("chi^2 / NDF = %f\n", chi2 / (track->GetNHits() - 3));


    //    FitWLSQ *fit = new FitWLSQ(xx, 0.3, 0.9, 0.9, (Int_t) nHits, 3, true, true, 4);
    //    if (fit->Fit(yy)) {
    //        A = fit->param[2];
    //        B = fit->param[1];
    //        C = fit->param[0];
    //    } else {
    //        return kBMNERROR;
    //    }
    //    const Double_t R = Power(1 - (2 * A * firstHit->GetZ() + B) * (2 * A * firstHit->GetZ() + B), 1.5) / Abs(2 * A); //radii of trajectory in the first hit position
    //    const Double_t Q = (A > 0) ? +1 : -1;


    const Double_t R = circPar.Z();
    const Double_t Xc = circPar.Y();
    const Double_t Q = (Xc > 0) ? +1 : -1;


    fSum /= nOk;
    const Double_t S = 0.0003 * fSum;
    //    const Double_t S = 0.0003 * maxField;
    //    Double_t S = 0.0003 * Abs(fField->GetBy(lastHit->GetX(), lastHit->GetY(), lastHit->GetZ()));
    //    const Double_t S = 0.0003 * Abs(fField->GetBy(firstHit->GetX(), firstHit->GetY(), firstHit->GetZ()));
    const Double_t QP = Q / S / R;

    track->SetChi2(chi2);
    track->SetNDF(track->GetNHits() - 3);
    track->GetParamFirst()->SetQp(QP);

    //    delete fit;
    delete[] xx;
    delete[] yy;

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

//    SeedsByThreeStations(0, 1, 2, hitsOnStation, cand);
//    SeedsByThreeStations(1, 2, 3, hitsOnStation, cand);
//    SeedsByThreeStations(2, 3, 4, hitsOnStation, cand);
//    SeedsByThreeStations(3, 4, 5, hitsOnStation, cand);

        SeedsByThreeStations(0, 1, 2, hitsOnStation, cand);
        SeedsByThreeStations(0, 1, 3, hitsOnStation, cand);
        SeedsByThreeStations(0, 1, 4, hitsOnStation, cand);
        SeedsByThreeStations(0, 2, 3, hitsOnStation, cand);
        SeedsByThreeStations(0, 2, 4, hitsOnStation, cand);
        SeedsByThreeStations(0, 3, 4, hitsOnStation, cand);
    
        SeedsByThreeStations(1, 2, 3, hitsOnStation, cand);
        SeedsByThreeStations(1, 2, 4, hitsOnStation, cand);
        SeedsByThreeStations(1, 3, 4, hitsOnStation, cand);

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

BmnStatus BmnGemTracking::CalculateTrackParamsPol2(BmnGemTrack *tr) {

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

    par.SetPosition(TVector3(lX, lY, lZ));
    par.SetTx(Tx_last);
    par.SetTy(Ty_last);
    par.SetQp(QP);
    tr->SetParamLast(par);
    if (!IsParCorrect(&par)) return kBMNERROR;

    //update for firstParam
    par.SetPosition(TVector3(fX, fY, fZ));
    par.SetQp(QP);
    par.SetTx(Tx_first);
    par.SetTy(Ty_first);
    tr->SetParamFirst(par);
    if (!IsParCorrect(&par)) return kBMNERROR;

    return kBMNSUCCESS;
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

    Double_t chi2circ = 0.0;

    TVector3 CircParZX;
    TVector3 lineParZY = LineFit(tr, fGemHitsArray, "ZY");
    if (lineParZY.Z() > fLineFitCut) return kBMNERROR;
    if (nHits == 3) {
        CircParZX = CircleBy3Hit(tr, fGemHitsArray);
        tr->SetChi2(0.0);
        tr->SetNDF(0);
    } else {
        //        CircParZX = CircleBy3Hit(tr, fGemHitsArray);
        CircParZX = CircleFit(tr, fGemHitsArray, chi2circ);
        //    tr->SetChi2(lineParZY.Z());
        tr->SetChi2(chi2circ);
        tr->SetNDF(nHits - 3);
    }

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

    //    Double_t S = 0.0003 * fSum;
    Double_t S = 0.0003 * maxField;
    //    Double_t S = 0.0003 * minField;
    //    Double_t S = 0.0003 * Abs(fField->GetBy(lastHit->GetX(), lastHit->GetY(), lastHit->GetZ()));
    //    Double_t S = 0.0003 * Abs(fField->GetBy(midHit->GetX(), midHit->GetY(), midHit->GetZ()));
    //    Double_t S = 0.0003 * Abs(fField->GetBy(firstHit->GetX(), firstHit->GetY(), firstHit->GetZ()));
    //    Double_t QP = Q / S / Sqrt(R * R + B * B);
    Double_t QP = Q / S / R;

    Double_t sumX = 0.0;
    Double_t sumX2 = 0.0;
    Double_t sumY = 0.0;
    Double_t sumY2 = 0.0;
    Double_t sumTx = 0.0;
    Double_t sumTx2 = 0.0;
    Double_t sumXY = 0.0;
    Double_t sumXTx = 0.0;
    Double_t sumYTx = 0.0;

    for (UInt_t i = 0; i < nHits; ++i) {
        BmnGemStripHit* hit = GetHit(tr->GetHitIndex(i));
        if (!hit) continue;
        if (!hit->GetFlag()) continue;
        Double_t Xi = hit->GetX();
        Double_t Yi = hit->GetY();
        Double_t Zi = hit->GetZ();

        sumX += Xi;
        sumX2 += Xi * Xi;
        sumY += Yi;
        sumXY += Xi * Yi;
        sumXTx += Xi * (Zi - Zc) / (Xi - Xc);
        sumYTx += Yi * (Zi - Zc) / (Xi - Xc);
        sumTx += (Zi - Zc) / (Xi - Xc);
        sumTx2 += Sqr((Zi - Zc) / (Xi - Xc));
    }

    Cov_X_X = sumX2 / nHits - Sqr(sumX / nHits);
    Cov_X_Y = sumXY / nHits - sumX / nHits * sumY / nHits;
    Cov_X_Tx = -sumXTx / nHits + sumX / nHits * sumTx / nHits;
    Cov_X_Ty = 0.0;
    Cov_X_Qp = 0.0;
    Cov_Y_Y = sumY2 / nHits - Sqr(sumY / nHits);
    Cov_Y_Tx = -sumYTx / nHits + sumY / nHits * sumTx / nHits;
    Cov_Y_Ty = 0.0;
    Cov_Y_Qp = 0.0;
    Cov_Tx_Tx = sumTx2 / nHits - Sqr(sumTx / nHits);
    Cov_Tx_Ty = 0.0;
    Cov_Tx_Qp = 0.0;
    Cov_Ty_Ty = 0.0;
    Cov_Qp_Qp = 0.0;
    Cov_Ty_Qp = 0.0;

    FairTrackParam par;
    const Double_t minVal = 1e-10;

    // THE BEST
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

    par.SetCovariance(0, 0, 2.7e-05);
    par.SetCovariance(0, 1, -1.3e-07);
    par.SetCovariance(0, 2, 1.8e-07);
    par.SetCovariance(0, 3, 3.2e-08);
    par.SetCovariance(0, 4, 1.4e-08);
    par.SetCovariance(1, 1, 7.8e-03);
    par.SetCovariance(1, 2, -1.4e-07);
    par.SetCovariance(1, 3, 4.2e-05);
    par.SetCovariance(1, 4, -3.7e-07);
    par.SetCovariance(2, 2, 3.0e-06);
    par.SetCovariance(2, 3, 1.1e-07);
    par.SetCovariance(2, 4, 1.5e-07);
    par.SetCovariance(3, 3, 1.5e-06);
    par.SetCovariance(3, 4, -3.2e-09);
    par.SetCovariance(4, 4, 7.2e-07);

    //        par.SetCovariance(0, 0, Cov_X_X);
    //        par.SetCovariance(0, 1, Cov_X_Y);
    //        par.SetCovariance(0, 2, Cov_X_Tx);
    //        par.SetCovariance(0, 3, Cov_X_Ty);
    //        par.SetCovariance(0, 4, Cov_X_Qp);
    //        par.SetCovariance(1, 1, Cov_Y_Y);
    //        par.SetCovariance(1, 2, Cov_Y_Tx);
    //        par.SetCovariance(1, 3, Cov_Y_Ty);
    //        par.SetCovariance(1, 4, Cov_Y_Qp);
    //        par.SetCovariance(2, 2, Cov_Tx_Tx);
    //        par.SetCovariance(2, 3, Cov_Tx_Ty);
    //        par.SetCovariance(2, 4, Cov_Tx_Qp);
    //        par.SetCovariance(3, 3, Cov_Ty_Ty);
    //        par.SetCovariance(3, 4, Cov_Ty_Qp);
    //        par.SetCovariance(4, 4, Cov_Qp_Qp);

    Double_t lX = lastHit->GetX();
    Double_t lY = lastHit->GetY();
    Double_t lZ = lastHit->GetZ();

    Double_t fX = firstHit->GetX();
    Double_t fY = firstHit->GetY();
    Double_t fZ = firstHit->GetZ();

    Double_t h = -1.0;

    Double_t Tx_first = h * (fZ - Zc) / (fX - Xc);
    Double_t Tx_last = h * (lZ - Zc) / (lX - Xc);
    Double_t Ty_last = B; // / (lX - Xc);
    Double_t Ty_first = B; // / (fX - Xc);

    par.SetPosition(TVector3(lX, lY, lZ));
    par.SetTx(Tx_last);
    par.SetTy(Ty_last);
    par.SetQp(QP);
    tr->SetParamLast(par);
    if (!IsParCorrect(&par)) return kBMNERROR;

    //update for firstParam
    par.SetPosition(TVector3(fX, fY, fZ));
    par.SetQp(QP);
    //    par.SetQp(QPFirst);
    par.SetTx(Tx_first);
    par.SetTy(Ty_first); //par.SetTy(-B / (firstHit->GetX() - Xc));
    tr->SetParamFirst(par);
    if (!IsParCorrect(&par)) return kBMNERROR;

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

TVector2 BmnGemTracking::GetTransXY(BmnGemStripHit* hit) {
    //const Double_t oneOverR = 1.0 / Sqrt(Sqr(hit->GetX()) + Sqr(hit->GetY()) + Sqr(hit->GetZ()));
    const Double_t oneOverR = 1.0 / Sqrt(Sqr(hit->GetX() - fRoughVertex.X()) + Sqr(hit->GetY() - fRoughVertex.Y()) + Sqr(hit->GetZ() - fRoughVertex.Z()));
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

BmnGemStripHit* BmnGemTracking::GetHit(Int_t i) {
    BmnGemStripHit* hit = (BmnGemStripHit*) fGemHitsArray->At(i);
    if (!hit) return NULL;
    if (hit->IsUsed()) return NULL;
    return hit;
}

BmnStatus BmnGemTracking::NearestHitMergeGem(UInt_t station, BmnGemTrack* track) {
    BmnHit* minHit = NULL; // Pointer to hit with minimum chi-square
    Double_t minDist = FLT_MAX;
    Double_t minChiSq = DBL_MAX;
    Double_t minLen = DBL_MAX;
    Int_t minIdx = -1;
    Double_t dist = 0.0;
    FairTrackParam minParUp; // updated track parameters for closest hit
    FairTrackParam minParPred; // predicted track parameters for closest hit
    Double_t stationZ = fGemDetector->GetGemStation(station)->GetModule(0)->GetZPositionRegistered();
    //    FairTrackParam parPredict = (fGoForward) ? (*(track->GetParamFirst())) : (*(track->GetParamLast()));
    FairTrackParam parPredict = *(track->GetParamFirst());
    Bool_t goForward = (parPredict.GetZ() < stationZ);
    Double_t length = track->GetLength();

    if (fKalman->TGeoTrackPropagate(&parPredict, stationZ, fPDG, NULL, &length, fIsField) == kBMNERROR) return kBMNERROR;

    for (Int_t iHit = 0; iHit < fGemHitsArray->GetEntriesFast(); ++iHit) {
        BmnGemStripHit* hit = (BmnGemStripHit*) GetHit(iHit);
        if (!hit) continue;
        if (!hit->GetFlag()) continue;
        if (hit->GetStation() != station) continue;
        dist = Dist(parPredict.GetX(), parPredict.GetY(), hit->GetX(), hit->GetY());
        if (dist < minDist && dist < fGemDistCut) { // Check if hit is inside validation gate and closer to the track.
            Double_t chi = 0.0;
            FairTrackParam parUpdate = parPredict;
            if (fKalman->Update(&parUpdate, hit, chi) == kBMNERROR) continue;
            minChiSq = chi;
            minDist = dist;
            minHit = hit;
            minLen = length;
            minIdx = iHit;
            minParUp = parUpdate;
            minParPred = parPredict;
        }
    }

    if (minHit != NULL) {

        if (goForward)
            track->SetParamLast(minParUp);
        else
            track->SetParamFirst(minParUp);
        track->SetChi2(Abs(track->GetChi2()) + Abs(minChiSq));
        track->SetNDF(track->GetNDF() + 1);
        track->AddHit(minIdx, minHit);
        //track->SetLength(minLen);
        track->SortHits();
        //        minHit->SetFlag(kFALSE);
        BmnFitNode* node = track->GetFitNode(station);
        node->SetUpdatedParam(&minParUp);
        node->SetPredictedParam(&minParPred);
        return kBMNSUCCESS;
    } else {
        return kBMNERROR;
    }
}

Double_t BmnGemTracking::CalculateLength(BmnGemTrack* tr) {
    if (!tr) return 0.0;

    vector<Double_t> X, Y, Z;
    X.push_back(0.);
    Y.push_back(0.);
    Z.push_back(0.);
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
