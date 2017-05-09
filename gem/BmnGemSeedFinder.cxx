#include "TProfile.h"
#include "TRandom.h"
#include "TMath.h"
#include "TVector3.h"
#include "TCanvas.h"
#include "BmnGemSeedFinder.h"
#include "BmnKalmanFilter_tmp.h"
#include "BmnMath.h"
#include "BmnFieldMap.h"

static Float_t workTime = 0.0;
//-----------------------------------------

const UInt_t kNHITSFORSEED = 12; // we use for seeds only kNHITSFORSEED hits
const UInt_t kMAXSTATIONFORSEED = 5; // we start to search seeds only from stations in range from 0 up to kMAXSTATIONFORSEED

using namespace std;
using namespace TMath;

BmnGemSeedFinder::BmnGemSeedFinder() : fEventNo(0) {
    fUseLorentz = kFALSE;
    fGoForward = kTRUE;
    fIsField = kFALSE;
    fIsTarget = kTRUE;
    fGemHitsArray = NULL;
    fGemSeedsArray = NULL;
    fHitsBranchName = "BmnGemStripHit";
    fSeedsBranchName = "BmnGemTrack";
    fXmin = -DBL_MAX;
    fXmax = DBL_MAX;
    fYmin = -DBL_MAX;
    fYmax = DBL_MAX;
    fAddresses = NULL;
    fRoughVertex = TVector3(0.0, 0.0, 0.0);
    fLineFitCut = 0.5;
}

BmnGemSeedFinder::~BmnGemSeedFinder() {
}

InitStatus BmnGemSeedFinder::Init() {

    if (fVerbose) cout << "======================== Seed finder init started =========================" << endl;

    //Get ROOT Manager
    FairRootManager* ioman = FairRootManager::Instance();
    if (!ioman) Fatal("Init", "FairRootManager is not instantiated");

    fGemHitsArray = (TClonesArray*) ioman->GetObject(fHitsBranchName); //in
    if (!fGemHitsArray) Fatal("Init", "No input hits array");

    fGemSeedsArray = new TClonesArray(fSeedsBranchName, 100); //out
    ioman->Register("BmnGemSeed", "GEM", fGemSeedsArray, kTRUE);

    fField = FairRunAna::Instance()->GetField();
    if (!fField) Fatal("Init", "No Magnetic Field found");
    // Chi2 is less restricted when doing alignment
    if (!fIsField)
        fLineFitCut = 50.; // LDBL_MAX

    const Int_t nIter = 5;
    //(0.006, 6.0, 1.05); //best parameters
    const Int_t nBins[nIter] = {2000, 2000, 2000, 1000, 1000};
    const Float_t sigX[nIter] = {0.005, 0.1, 0.05, 0.1, 0.005};
    const Float_t stpY[nIter] = {1.0, 4.0, 2.0, 2.0, 20.0};
    const Float_t thrs[nIter] = {2.5, 1.1, 1.1, 1.1, 1.5};
    const Int_t length[nIter] = {4, 4, 4, 4, 4};
    Int_t i = 4;//(fIsTarget) ? 1 : 4;

    fNBins = nBins[i];
    fMin = -0.5;
    fMax = -fMin;
    fWidth = (fMax - fMin) / fNBins;

    kSIG_X = sigX[i];
    kY_STEP = stpY[i];
    kTRS = thrs[i];
    kNHITSFORFIT = length[i];

    fAddresses = new Int_t*[fNBins];
    for (Int_t i = 0; i < fNBins; ++i) {
        fAddresses[i] = new Int_t[fNBins];
        for (Int_t j = 0; j < fNBins; ++j)
            fAddresses[i][j] = -1;
    }

    fDetector = new BmnGemStripStationSet_RunSpring2017(BmnGemStripConfiguration::RunSpring2017);

    if (fVerbose) cout << "======================== Seed finder init finished ========================" << endl;
}

void BmnGemSeedFinder::Exec(Option_t* opt) {
    clock_t tStart = clock();
    if (fVerbose) cout << "\n======================== GEM seed finder exec started =====================\n" << endl;
    if (fVerbose) cout << "Event number: " << fEventNo++ << endl;

    fGemSeedsArray->Delete();

    for (Int_t i = 0; i < fNBins; ++i)
        for (Int_t j = 0; j < fNBins; ++j)
            fAddresses[i][j] = -1;

    //Just skip too big events
    if (fGemHitsArray->GetEntriesFast() > 100 || fGemHitsArray->GetEntriesFast() == 0) return;

    //GEM inefficiency ===>
    //    const Float_t eff = 1.00;
    //    for (Int_t hitIdx = 0; hitIdx < fGemHitsArray->GetEntriesFast(); ++hitIdx) {
    //        BmnGemStripHit* hit = GetHit(hitIdx);
    //        if (gRandom->Uniform(1.0) > eff) {
    //            hit->SetUsing(kTRUE);
    //        }
    //    }
    // <=== GEM ineficiency

    vector<BmnGemTrack> seeds;
    //    

    //    UInt_t NhitsInStation[6] = {0};
    //    for (Int_t iSt = 1; iSt < 7; ++iSt) {
    //        for (Int_t iHit = 0; iHit < fGemHitsArray->GetEntriesFast(); ++iHit) {
    //            BmnGemStripHit* hit = GetHit(iHit);
    //            if (!hit) continue;
    //            if (hit->GetStation() != iSt) continue;
    //            NhitsInStation[iSt - 1]++;
    //        }
    //        if (NhitsInStation[iSt - 1] > 1) return;
    //    }

    //    for (Int_t iHit = 0; iHit < fGemHitsArray->GetEntriesFast(); ++iHit) {
    //        BmnGemStripHit* hit = GetHit(iHit);
    //        if (!hit) continue;
    //        printf("(%3.4f, %3.4f, %3.4f)\n", hit->GetX(), hit->GetY(), hit->GetZ());
    //    }

    if (fUseLorentz) {
        FillAddrWithLorentz(kSIG_X, kY_STEP, kTRS);
    } else {
        FillAddr();
    }
    FindSeedsCombinatorics(seeds);
    //    FindSeeds(seeds);
    if (fVerbose) printf("seeds.size() = %d\n", seeds.size());
    if (seeds.size() != 0) FitSeeds(seeds);

    if (fVerbose) cout << "\nGEM_SEEDING: Number of found seeds: " << fGemSeedsArray->GetEntriesFast() << endl;

    //    for (Int_t iHit = 0; iHit < fGemHitsArray->GetEntriesFast(); ++iHit)
    //        GetHit(iHit)->SetUsing(kFALSE);
    clock_t tFinish = clock();
    workTime += ((Float_t) (tFinish - tStart)) / CLOCKS_PER_SEC;

    if (fVerbose) cout << "\n======================== GEM seed finder exec finished ====================" << endl;

}

void BmnGemSeedFinder::Finish() {
    ofstream outFile;
    outFile.open("QA/timing.txt");
    outFile << "Seed Finder Time: " << workTime << endl;
    cout << "Work time of the GEM seed finder: " << workTime << endl;
    delete fDetector;
}

void BmnGemSeedFinder::GoToStation(vector<BmnGemStripHit*>& hits, vector<BmnGemStripHit*> *hitsOnStation, Int_t stat, vector<BmnGemTrack>& cand) {
    Int_t nextStat = stat + 1;
    Int_t nStat = 4; //fDetector->GetNStations();

    if (hitsOnStation[stat].size() < 1) {
        if (stat == nStat - 1) {
            BmnGemTrack track;
            for (Int_t iHit = 0; iHit < hits.size(); iHit++) {
                BmnGemStripHit* trHit = ((BmnGemStripHit*) hits.at(iHit));
                track.AddHit(trHit->GetIndex(), trHit);
            }
            track.SortHits();
            cand.push_back(track);
            return;
        }
        GoToStation(hits, hitsOnStation, nextStat, cand);
    } else {
        for (Int_t iSize = 0; iSize < hitsOnStation[stat].size(); iSize++) {
            BmnGemStripHit* hit = (BmnGemStripHit*) hitsOnStation[stat].at(iSize);
            hits.push_back(hit);

            if (nextStat == nStat) {
                BmnGemTrack track;
                for (Int_t iHit = 0; iHit < hits.size(); iHit++) {
                    BmnGemStripHit* trHit = ((BmnGemStripHit*) hits.at(iHit));
                    track.AddHit(trHit->GetIndex(), trHit);
                }
                track.SortHits();
                cand.push_back(track);
                hits.pop_back();
                continue;
            }
            GoToStation(hits, hitsOnStation, nextStat, cand);
            hits.pop_back();
        }
    }
    nextStat--;
}

BmnStatus BmnGemSeedFinder::FindSeedsCombinatorics(vector<BmnGemTrack>& cand) {

    //    //================= RECURSIVE ===========//
    //    // Fill hits over stations
    //    vector <BmnGemStripHit*> stat[fDetector->GetNStations()];
    //    for (Int_t iHit = 0; iHit < fGemHitsArray->GetEntriesFast(); iHit++) {
    //        BmnGemStripHit* hit = (BmnGemStripHit*) fGemHitsArray->UncheckedAt(iHit);
    //        stat[hit->GetStation()].push_back(hit);
    //    }
    //
    //    vector <BmnGemStripHit*> hits;
    //    GoToStation(hits, stat, 0, cand);
    //    //======================================//


    vector<Int_t> hitsOnStation[fDetector->GetNStations()];

    for (Int_t iHit = 0; iHit < fGemHitsArray->GetEntriesFast(); ++iHit) {
        BmnGemStripHit* hit = GetHit(iHit);
        if (!hit) continue;
        if (!hit->GetFlag()) continue;
        for (Int_t iSt = 0; iSt < fDetector->GetNStations(); ++iSt)
            if (hit->GetStation() == iSt) hitsOnStation[iSt].push_back(iHit);
    }

    if (fGoForward) {
        for (Int_t iHit0 = 0; iHit0 < hitsOnStation[0].size(); ++iHit0) {
            BmnGemStripHit* hit0 = (BmnGemStripHit*) fGemHitsArray->At(hitsOnStation[0].at(iHit0));
            for (Int_t iHit1 = 0; iHit1 < hitsOnStation[1].size(); ++iHit1) {
                BmnGemStripHit* hit1 = (BmnGemStripHit*) fGemHitsArray->At(hitsOnStation[1].at(iHit1));
                for (Int_t iHit2 = 0; iHit2 < hitsOnStation[2].size(); ++iHit2) {
                    BmnGemStripHit* hit2 = (BmnGemStripHit*) fGemHitsArray->At(hitsOnStation[2].at(iHit2));
                    //                    for (Int_t iHit3 = 0; iHit3 < hitsOnStation[3].size(); ++iHit3) {
                    //                        BmnGemStripHit* hit3 = (BmnGemStripHit*) fGemHitsArray->At(hitsOnStation[3].at(iHit3));
                    BmnGemTrack trCnd;
                    trCnd.AddHit(hitsOnStation[0].at(iHit0), hit0);
                    trCnd.AddHit(hitsOnStation[1].at(iHit1), hit1);
                    trCnd.AddHit(hitsOnStation[2].at(iHit2), hit2);
                    //                        trCnd.AddHit(hitsOnStation[3].at(iHit3), hit3);
                    trCnd.SortHits();
                    TVector3 lineParZY = LineFit(&trCnd, fGemHitsArray, "ZY");
                    if (lineParZY.Z() > fLineFitCut) continue;
                    cand.push_back(trCnd);
                    //                    }
                }
            }
        }
    } else {
        for (Int_t iHit5 = 0; iHit5 < hitsOnStation[5].size(); ++iHit5) {
            BmnGemStripHit* hit5 = (BmnGemStripHit*) fGemHitsArray->At(hitsOnStation[5].at(iHit5));
            for (Int_t iHit4 = 0; iHit4 < hitsOnStation[4].size(); ++iHit4) {
                BmnGemStripHit* hit4 = (BmnGemStripHit*) fGemHitsArray->At(hitsOnStation[4].at(iHit4));
                for (Int_t iHit3 = 0; iHit3 < hitsOnStation[3].size(); ++iHit3) {
                    BmnGemStripHit* hit3 = (BmnGemStripHit*) fGemHitsArray->At(hitsOnStation[3].at(iHit3));
                    BmnGemTrack trCnd;
                    trCnd.AddHit(hitsOnStation[5].at(iHit5), hit5);
                    trCnd.AddHit(hitsOnStation[4].at(iHit4), hit4);
                    trCnd.AddHit(hitsOnStation[3].at(iHit3), hit3);
                    trCnd.SortHits();
                    TVector3 lineParZY = LineFit(&trCnd, fGemHitsArray, "ZY");
                    if (lineParZY.Z() > fLineFitCut) continue;
                    cand.push_back(trCnd);
                }
            }
        }
    }

    return kBMNSUCCESS;
}

BmnStatus BmnGemSeedFinder::FindSeeds(vector<BmnGemTrack>& cand) {

    //    for (Int_t iHit = 0; iHit < fGemHitsArray->GetEntriesFast(); ++iHit) {
    //        BmnGemStripHit* hit = GetHit(iHit);
    //        const Float_t oneOverR = 1.0 / Sqrt(Sqr(hit->GetX()) + Sqr(hit->GetY()) + Sqr(hit->GetZ()));
    //        const Float_t newX = hit->GetX() * oneOverR;
    //        const Float_t newY = hit->GetY() * oneOverR;
    //        printf("station = %d\tx = %f\ty = %f\txR = %fX\tyR = %f\tR = %f\n", hit->GetStation(), hit->GetX(), hit->GetY(), newX, newY, 1.0/oneOverR);
    //    }
    //
    //    for (map<ULong_t, Int_t>::iterator it = addresses.begin(); it != addresses.end(); ++it) {
    //        printf("st_in_addr = %d\taddr = %d\n", GetHit(it->second)->GetStation(), it->first);
    //    }

    const Int_t kNSteps = 6;
    for (Int_t iSt = 0; iSt < fDetector->GetNStations(); ++iSt)
        for (Int_t j = Int_t(kY_STEP - 1); j < Int_t(kY_STEP); ++j) {
            for (Int_t iHit = 0; iHit < fGemHitsArray->GetEntriesFast(); ++iHit) {
                BmnGemStripHit* hit = GetHit(iHit);
                if (!hit) continue;
                if (!hit->GetFlag()) continue;
//                if (hit->IsUsed()) continue;
                if (iSt != hit->GetStation()) continue;

                Int_t yAddr = hit->GetYaddr();
                if ((yAddr - j) < 0 || (yAddr + j) > fNBins) continue;

                Int_t xAddr = hit->GetXaddr();
                BmnGemTrack trackCand;

                Int_t dist = 0;
                Int_t maxDist = 0;
                Int_t hitCntr = 0;
                Int_t startBin = 0;
                Int_t prevStation = iSt - 1; //number of starting station         
                for (Int_t i = xAddr; i > 0; i--) {
                    //                    SearchTrackCandInLine(i, yAddr, &trackCand, &hitCntr, &maxDist, &dist, &startBin, &prevStation, j, kTRUE);
                    SearchTrackCandInLine(i, yAddr, &trackCand, &hitCntr, &maxDist, &dist, &startBin, &prevStation, j, kFALSE);
                    if ((hitCntr > 1) && Abs(i - startBin) > kNSteps * maxDist) break; //condition to finish search is dist < 2 * MaxDist
                    if (hitCntr >= kNHITSFORSEED) break;
                }
                trackCand.SortHits();
                if (trackCand.GetNHits() < kNHITSFORFIT) continue;
                cand.push_back(trackCand);
            }

            for (Int_t iHit = 0; iHit < fGemHitsArray->GetEntriesFast(); ++iHit) {
                BmnGemStripHit* hit = GetHit(iHit);
                if (!hit) continue;
                if (!hit->GetFlag()) continue;
//                if (hit->IsUsed()) continue;
                if (iSt != hit->GetStation()) continue;

                Int_t yAddr = hit->GetYaddr();

                if ((yAddr - j) < 0 || (yAddr + j) > fNBins) continue;

                Int_t xAddr = hit->GetXaddr();
                BmnGemTrack trackCand;

                Int_t dist = 0;
                Int_t maxDist = 0;
                Int_t hitCntr = 0;
                Int_t startBin = 0;
                Int_t prevStation = iSt - 1; //number of starting station
                for (Int_t i = xAddr; i < fNBins; ++i) {
                    //                    SearchTrackCandInLine(i, yAddr, &trackCand, &hitCntr, &maxDist, &dist, &startBin, &prevStation, j, kTRUE);
                    SearchTrackCandInLine(i, yAddr, &trackCand, &hitCntr, &maxDist, &dist, &startBin, &prevStation, j, kFALSE);
                    if ((hitCntr > 1) && Abs(i - startBin) > kNSteps * maxDist) break; //condition to finish search is dist < 2 * MaxDist
                    if (hitCntr >= kNHITSFORSEED) break;
                }
                trackCand.SortHits();
                if (trackCand.GetNHits() < kNHITSFORFIT) continue;
                cand.push_back(trackCand);
            }
        }

    //    for (Int_t iTr = 0; iTr < cand.size(); ++iTr) {
    //        BmnGemTrack* tr = &(cand.at(iTr));
    //        for (Int_t iHit = 0; iHit < fGemHitsArray->GetEntriesFast(); ++iHit) {
    //            BmnGemStripHit* hit = GetHit(iHit);
    //            if (!hit) continue;
    //            UInt_t st = hit->GetStation();
    //            Bool_t used = kFALSE;
    //            for (Int_t iTrHit = 0; iTrHit < tr->GetNHits(); ++iTrHit) {
    //                UInt_t stTr = GetHit(tr->GetHitIndex(iTrHit))->GetStation();
    //                if (stTr == st) {
    //                    used = kTRUE;
    //                    break;
    //                }
    //            }
    //            if (used) continue;
    //            tr->AddHit(iHit, hit);
    //            tr->SortHits();
    //        }
    //    }
}

BmnStatus BmnGemSeedFinder::FitSeeds(vector<BmnGemTrack> cand) {

    //Four cases:
    //  _________________________________
    // |                |                |
    // |   Field = 0    |   Field = 1    |
    // |   Targ  = 0    |   Targ  = 0    |
    // |________________|________________|
    // |                |                |
    // |   Field = 0    |   Field = 1    |
    // |   Targ  = 1    |   Targ  = 1    |
    // |________________|________________|

    for (Int_t i = 0; i < cand.size(); ++i) {
        BmnGemTrack* trackCand = &(cand.at(i));
        if (fIsField) {
            if (CalculateTrackParamsCircle(trackCand) == kBMNERROR) {
                trackCand->SetFlag(kBMNBAD);
                continue;
            }
        } else {
            if (CalculateTrackParamsLine(trackCand) == kBMNERROR) {
                trackCand->SetFlag(kBMNBAD);
                continue;
            }
        }
    }

    if (fIsTarget) {
        for (Int_t i = 0; i < cand.size(); ++i) {
            BmnGemTrack* trackCand = &(cand.at(i));
            //ADD check for parameters quality (chi2, length, ...)
            if (trackCand->GetFlag() == kBMNBAD) continue;
            new((*fGemSeedsArray)[fGemSeedsArray->GetEntriesFast()]) BmnGemTrack(*trackCand);
        }
    } else {
        Float_t minChi = 10.0; //FLT_MAX;
        //        BmnGemTrack minTrack;
        for (Int_t i = 0; i < cand.size(); ++i) {
            BmnGemTrack trackCand = cand.at(i);
            //if (trackCand.GetFlag() == kBMNBAD) continue;
            Float_t chi = trackCand.GetChi2(); // / trackCand.GetNDF();
            if (chi > minChi) continue;
            if (trackCand.GetNHits() < 3) continue;
            new((*fGemSeedsArray)[fGemSeedsArray->GetEntriesFast()]) BmnGemTrack(trackCand);
            //            minChi = chi;
            //            minTrack = trackCand;
        }

        //        if (minTrack.GetNHits() != 0)
        //            new((*fGemSeedsArray)[fGemSeedsArray->GetEntriesFast()]) BmnGemTrack(minTrack);
    }
}

void BmnGemSeedFinder::SearchTrackCandInLine(const Int_t i, const Int_t y, BmnGemTrack* tr, Int_t* hitCntr, Int_t* maxDist, Int_t* dist, Int_t* startBin, Int_t* prevStation, Int_t gate, Bool_t isIdeal) {

    for (Int_t j = y - gate; j <= y + gate; ++j) {

        Int_t id = fAddresses[i][j];
        if (id == -1) continue;

        BmnGemStripHit* hit = GetHit(id);
        if (!hit) continue;
        if (!hit->GetFlag()) continue;
        Short_t st = hit->GetStation();
        if (isIdeal) {
            if (st != (*prevStation) + 1) {
                continue;
            }
        } else {
            if (st <= (*prevStation)) {
                continue;
            }
        }

        Int_t xAddr = hit->GetXaddr();
        (*hitCntr)++;
        (*prevStation) = st;
        if ((*hitCntr) != 1) {
            (*dist) = Abs(xAddr - (*startBin));
            if ((*dist) > (*maxDist)) (*maxDist) = (*dist);
        }
        (*startBin) = xAddr;
        tr->AddHit(id, hit);
        tr->SortHits();
    }
}

BmnStatus BmnGemSeedFinder::CalculateTrackParamsLine(BmnGemTrack * tr) {

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

BmnStatus BmnGemSeedFinder::CalculateTrackParamsCircle(BmnGemTrack * tr) {

    //Estimation of track parameters for events with magnetic field
    const UInt_t nHits = tr->GetNHits();
    //    if (nHits < kNHITSFORFIT) return kBMNERROR;
    BmnGemStripHit* lastHit = GetHit(tr->GetHitIndex(nHits - 1));
    BmnGemStripHit* firstHit = GetHit(tr->GetHitIndex(0));
    if (!firstHit || !lastHit) return kBMNERROR;
    if (!lastHit->GetFlag()) return kBMNERROR;
    if (!firstHit->GetFlag()) return kBMNERROR;

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

    Float_t R = CircParZX.Z(); // radius of fit-circle
    Float_t Xc = CircParZX.Y(); // x-coordinate of fit-circle center
    Float_t Zc = CircParZX.X(); // z-coordinate of fit-circle center
    fField = FairRunAna::Instance()->GetField();

    const Float_t B = lineParZY.X(); //angle coefficient for helicoid

    Float_t Xmean(0.0), Ymean(0.0), Zmean(0.0); // <Xi> , <Yi> , <Zi>
    Float_t ZXmean(0.0); // <(Zi-Zc)/(Xi-Xc)>
    Float_t ZX2mean(0.0); // <(Zi-Zc)/(Xi-Xc)^2>
    Float_t XZXmean(0.0); // <Xi*(Zi-Zc)/(Xi-Xc)>
    Float_t YZXmean(0.0); // <Yi*(Zi-Zc)/(Xi-Xc)>
    Float_t OneXmean(0.0); // <1/(Xi-Xc)>
    Float_t XOneXmean(0.0); // <Xi/(Xi-Xc)>
    Float_t YOneXmean(0.0); // <Yi/(Xi-Xc)>
    Float_t QPmean(0.0);
    Float_t XQPmean(0.0);
    Float_t YQPmean(0.0);
    Float_t ZXQPmean(0.0);
    Float_t OneXQPmean(0.0);

    //Covariance matrix
    Float_t Cov_X_X(0.0), Cov_X_Y(0.0), Cov_X_Tx(0.0), Cov_X_Ty(0.0), Cov_X_Qp(0.0);
    Float_t Cov_Y_Y(0.0), Cov_Y_Tx(0.0), Cov_Y_Ty(0.0), Cov_Y_Qp(0.0);
    Float_t Cov_Tx_Tx(0.0), Cov_Tx_Ty(0.0), Cov_Tx_Qp(0.0);
    Float_t Cov_Ty_Ty(0.0), Cov_Ty_Qp(0.0);
    Float_t Cov_Qp_Qp(0.0);
    Float_t Q = (lastHit->GetX() - firstHit->GetX() > 0) ? +1 : -1; //tr->GetParamFirst()->GetQp()) > 0.0 ? +1 : -1;

    Float_t fSum = 0.0;
    for (UInt_t i = 0; i < nHits; ++i) {
        BmnGemStripHit* hit = GetHit(tr->GetHitIndex(i));
        if (!hit) continue;
        if (!hit->GetFlag()) continue;
        fSum += fField->GetBy(hit->GetX(), hit->GetY(), hit->GetZ());
    }
    fSum /= nHits;

    Float_t S = 0.0003 * Abs(fSum);
//    Float_t S = 0.0003 * Abs(fField->GetBy(lastHit->GetX(), lastHit->GetY(), lastHit->GetZ()));
//            Float_t S = 0.0003 * Abs(fField->GetBy(firstHit->GetX(), firstHit->GetY(), firstHit->GetZ()));
    //    Float_t S = 0.0003 * (fField->GetBy(lastHit->GetX(), lastHit->GetY(), lastHit->GetZ()));
    Float_t QP = Q / S / Sqrt(R * R + B * B);

    for (UInt_t i = 0; i < nHits; ++i) {
        BmnGemStripHit* hit = GetHit(tr->GetHitIndex(i));
        if (!hit) continue;
        if (!hit->GetFlag()) continue;
        Float_t Xi = hit->GetX();
        Float_t Yi = hit->GetY();
        Float_t Zi = hit->GetZ();
        QPmean += QP;
        Xmean += Xi;
        Ymean += Yi;
        Zmean += Zi;
        ZXmean += (Zi - Zc) / (Xi - Xc);
        ZXQPmean += (Zi - Zc) / (Xi - Xc) * QP;
        ZX2mean += (Zi - Zc) / (Xi - Xc) / (Xi - Xc);
        XZXmean += (Zi - Zc) / (Xi - Xc) * Xi;
        YZXmean += (Zi - Zc) / (Xi - Xc) * Yi;
        OneXmean += 1.0 / (Xi - Xc);
        OneXQPmean += 1.0 / (Xi - Xc) * QP;
        XOneXmean += Xi / (Xi - Xc);
        YOneXmean += Yi / (Xi - Xc);
        XQPmean += Xi * QP;
        YQPmean += Yi * QP;

        Cov_X_X += Sqr(Xi);
        Cov_X_Y += Xi * Yi;
        Cov_Y_Y += Sqr(Yi);
        Cov_Tx_Tx += Sqr((Zi - Zc) / (Xi - Xc));
        Cov_Ty_Ty += Sqr(1.0 / (Xi - Xc));
        Cov_Qp_Qp += Sqr(QP);
    }
    Xmean /= nHits;
    Ymean /= nHits;
    Zmean /= nHits;
    ZXmean /= nHits;
    ZX2mean /= nHits;
    XZXmean /= nHits;
    YZXmean /= nHits;
    OneXmean /= nHits;
    XOneXmean /= nHits;
    YOneXmean /= nHits;
    XQPmean /= nHits;
    YQPmean /= nHits;
    QPmean /= nHits;
    ZXQPmean /= nHits;
    OneXQPmean /= nHits;

    Cov_X_X = Cov_X_X / nHits - Sqr(Xmean);
    Cov_X_Y = Cov_X_Y / nHits - Xmean * Ymean;
    Cov_X_Tx = Xmean * ZXmean - XZXmean;
    Cov_X_Ty = 0.0; //B * (Xmean * OneXmean - XOneXmean);
    Cov_Y_Y = Cov_Y_Y / nHits - Sqr(Ymean);
    Cov_Y_Tx = Ymean * ZXmean - YZXmean;
    Cov_Y_Ty = 0.0; //B * (Ymean * OneXmean - YOneXmean);
    Cov_Tx_Ty = B * (ZX2mean - ZXmean * OneXmean);
    Cov_Tx_Tx = Cov_Tx_Tx / nHits - Sqr(ZXmean);
    Cov_Ty_Ty = 0.0; //Sqr(B) * (Cov_Ty_Ty / nHits - OneXmean * OneXmean);
    Cov_Qp_Qp = Cov_Qp_Qp / nHits - Sqr(QPmean);
    Cov_X_Qp = XQPmean - Xmean * QPmean;
    Cov_Y_Qp = YQPmean - Ymean * QPmean;
    Cov_Tx_Qp = -1.0 * ZXQPmean + ZXmean * QPmean;
    Cov_Ty_Qp = 0.0; //B * OneXQPmean - OneXmean * QPmean;

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

    Float_t lX = lastHit->GetX();
    Float_t lY = lastHit->GetY();
    Float_t lZ = lastHit->GetZ();

    Float_t fX = firstHit->GetX();
    Float_t fY = firstHit->GetY();
    Float_t fZ = firstHit->GetZ();

    Float_t h = -1.0;

    Float_t Tx_first = h * (fZ - Zc) / (fX - Xc);
    Float_t Tx_last = h * (lZ - Zc) / (lX - Xc);
    Float_t Ty_last = B; // / (lX - Xc);
    Float_t Ty_first = B; // / (fX - Xc);

    par.SetPosition(TVector3(lX, lY, lZ));
    par.SetTx(Tx_last);
    par.SetTy(Ty_last); //par.SetTy(-B / (lX - Xc));
    const Float_t PxzLast = 0.0003 * fField->GetBy(lX, lY, lZ) * R; // Pt
    if (Abs(PxzLast) < 0.00001) return kBMNERROR;
    const Float_t PzLast = PxzLast / Sqrt(1 + Sqr(Tx_last));
    const Float_t PxLast = PzLast * Tx_last;
    const Float_t PyLast = PzLast * Ty_last;
    Float_t QPLast = Q / Sqrt(PxLast * PxLast + PyLast * PyLast + PzLast * PzLast);
    //    par.SetQp(QPLast);
    par.SetQp(QP);
    tr->SetParamLast(par);
    if (!IsParCorrect(&par)) return kBMNERROR;

    //update for firstParam
    const Float_t PxzFirst = 0.0003 * fField->GetBy(fX, fY, fZ) * R; // Pt
    //    const Float_t PxzFirst = PxzLast;
    if (Abs(PxzFirst) < 0.00001) return kBMNERROR;
    const Float_t PzFirst = PxzFirst / Sqrt(1 + Sqr(Tx_first));
    const Float_t PxFirst = PzFirst * Tx_first;
    const Float_t PyFirst = PzFirst * Ty_first;
    Float_t QPFirst = Q / Sqrt(PxFirst * PxFirst + PyFirst * PyFirst + PzFirst * PzFirst);
    par.SetPosition(TVector3(fX, fY, fZ));
    par.SetQp(QP);
    //    par.SetQp(QPFirst);
    par.SetTx(Tx_first);
    par.SetTy(Ty_first); //par.SetTy(-B / (firstHit->GetX() - Xc));
    tr->SetParamFirst(par);
    if (!IsParCorrect(&par)) return kBMNERROR;

    return kBMNSUCCESS;
}

Bool_t BmnGemSeedFinder::CalculateTrackParamsSpiral(BmnGemTrack * tr) {
    //Needed for start approximation of track parameters

    const UInt_t nHits = tr->GetNHits();
    BmnGemStripHit* lastHit = GetHit(tr->GetHitIndex(nHits - 1));
    //    BmnGemStripHit* preLastHit = GetHit(tr->GetHitIndex(nHits - 2));
    BmnGemStripHit* firstHit = GetHit(tr->GetHitIndex(0));
    //    BmnGemStripHit* secondHit = GetHit(tr->GetHitIndex(1));

    if (!firstHit || !lastHit) return kFALSE;

    TVector3 lineParZY = LineFit(tr, fGemHitsArray, "ZY");
    if (lineParZY.Z() > fLineFitCut) return kBMNERROR;
    TVector3 spirPar = SpiralFit(tr, fGemHitsArray);
    Float_t q = (lastHit->GetX() - firstHit->GetX() > 0) ? +1 : -1; //tr->GetParamFirst()->GetQp()) > 0.0 ? +1 : -1;

    Float_t x0 = firstHit->GetX();
    Float_t z0 = firstHit->GetZ();
    Float_t z0_2 = z0 * z0;
    Float_t x0_2 = x0 * x0;
    Float_t R = spirPar.Z();
    Float_t R2 = R * R;
    Float_t a = spirPar.X();
    Float_t b = spirPar.Y();

    Float_t tmp = x0_2 * R2 / (z0_2 + x0_2) - (z0_2 + x0_2) / 4;
    Float_t D = z0_2 + 4 * tmp;
    if (D < 0) return kFALSE;
    Float_t Zc = 0.5 * (Sqrt(D) - z0);
    Float_t Xc = Sqrt(R2 - z0_2);

    const Float_t B = lineParZY.X(); //angle coefficient for helix

    Float_t Xmean(0.0), Ymean(0.0), Zmean(0.0); // <Xi> , <Yi> , <Zi>
    Float_t ZXmean(0.0); // <(Zi-Zc)/(Xi-Xc)>
    Float_t ZX2mean(0.0); // <(Zi-Zc)/(Xi-Xc)^2>
    Float_t XZXmean(0.0); // <Xi*(Zi-Zc)/(Xi-Xc)>
    Float_t YZXmean(0.0); // <Yi*(Zi-Zc)/(Xi-Xc)>
    Float_t OneXmean(0.0); // <1/(Xi-Xc)>
    Float_t XOneXmean(0.0); // <Xi/(Xi-Xc)>
    Float_t YOneXmean(0.0); // <Yi/(Xi-Xc)>
    Float_t QPmean(0.0);
    Float_t XQPmean(0.0);
    Float_t YQPmean(0.0);
    Float_t ZXQPmean(0.0);
    Float_t OneXQPmean(0.0);

    //Covariance matrix
    Float_t Cov_X_X(0.0), Cov_X_Y(0.0), Cov_X_Tx(0.0), Cov_X_Ty(0.0), Cov_X_Qp(0.0);
    Float_t Cov_Y_Y(0.0), Cov_Y_Tx(0.0), Cov_Y_Ty(0.0), Cov_Y_Qp(0.0);
    Float_t Cov_Tx_Tx(0.0), Cov_Tx_Ty(0.0), Cov_Tx_Qp(0.0);
    Float_t Cov_Ty_Ty(0.0), Cov_Ty_Qp(0.0);
    Float_t Cov_Qp_Qp(0.0);
    Float_t S = 0.0;

    for (UInt_t i = 0; i < nHits; ++i) {
        BmnGemStripHit* hit = GetHit(i);
        if (!hit) continue;
        Float_t Xi = hit->GetX();
        Float_t Yi = hit->GetY();
        Float_t Zi = hit->GetZ();
        S = 0.0003 * fField->GetBy(Xi, Yi, Zi);
        Float_t QPi = q / S / Sqrt(R2 + B * B);
        //        cout << "Bx(" << Xi << ", " << Yi << ", " << Zi << ") = " << fField->GetBx(Xi, Yi, Zi) << endl;
        //        cout << "By(" << Xi << ", " << Yi << ", " << Zi << ") = " << fField->GetBy(Xi, Yi, Zi) << endl;
        //        cout << "Bz(" << Xi << ", " << Yi << ", " << Zi << ") = " << fField->GetBz(Xi, Yi, Zi) << endl;

        QPmean += QPi;
        Xmean += Xi;
        Ymean += Yi;
        Zmean += Zi;
        ZXmean += (Zi - Zc) / (Xi - Xc);
        ZXQPmean += (Zi - Zc) / (Xi - Xc) * QPi;
        ZX2mean += (Zi - Zc) / (Xi - Xc) / (Xi - Xc);
        XZXmean += (Zi - Zc) / (Xi - Xc) * Xi;
        YZXmean += (Zi - Zc) / (Xi - Xc) * Yi;
        OneXmean += 1.0 / (Xi - Xc);
        OneXQPmean += 1.0 / (Xi - Xc) * QPi;
        XOneXmean += Xi / (Xi - Xc);
        YOneXmean += Yi / (Xi - Xc);
        XQPmean += Xi * QPi;
        YQPmean += Yi * QPi;

        Cov_X_X += Sqr(Xi);
        Cov_X_Y += Xi * Yi;
        Cov_Y_Y += Sqr(Yi);
        Cov_Tx_Tx += Sqr((Zi - Zc) / (Xi - Xc));
        Cov_Ty_Ty += Sqr(1.0 / (Xi - Xc));
        Cov_Qp_Qp += Sqr(QPi);
    }
    Xmean /= nHits;
    Ymean /= nHits;
    Zmean /= nHits;
    ZXmean /= nHits;
    ZX2mean /= nHits;
    XZXmean /= nHits;
    YZXmean /= nHits;
    OneXmean /= nHits;
    XOneXmean /= nHits;
    YOneXmean /= nHits;
    XQPmean /= nHits;
    YQPmean /= nHits;
    QPmean /= nHits;
    ZXQPmean /= nHits;
    OneXQPmean /= nHits;

    Cov_X_X = Cov_X_X / nHits - Sqr(Xmean);
    Cov_X_Y = Cov_X_Y / nHits - Xmean * Ymean;
    Cov_X_Tx = Xmean * ZXmean - XZXmean;
    Cov_X_Ty = 0.0; //B * (Xmean * OneXmean - XOneXmean);
    Cov_Y_Y = Cov_Y_Y / nHits - Sqr(Ymean);
    Cov_Y_Tx = Ymean * ZXmean - YZXmean;
    Cov_Y_Ty = 0.0; //B * (Ymean * OneXmean - YOneXmean);
    Cov_Tx_Ty = B * (ZX2mean - ZXmean * OneXmean);
    Cov_Tx_Tx = Cov_Tx_Tx / nHits - Sqr(ZXmean);
    Cov_Ty_Ty = Sqr(B) * (Cov_Ty_Ty / nHits - OneXmean * OneXmean);
    Cov_Qp_Qp = Cov_Qp_Qp / nHits - Sqr(QPmean);
    Cov_X_Qp = XQPmean - Xmean * QPmean;
    Cov_Y_Qp = YQPmean - Ymean * QPmean;
    Cov_Tx_Qp = -1.0 * ZXQPmean + ZXmean * QPmean;
    Cov_Ty_Qp = 0.0; //B * OneXQPmean - OneXmean * QPmean;

    FairTrackParam par;
    const Float_t cov_const = 1e-15;
    par.SetCovariance(0, 0, Cov_X_X);
    par.SetCovariance(0, 1, Cov_X_Y);
    par.SetCovariance(0, 2, Cov_X_Tx);
    par.SetCovariance(0, 3, Cov_X_Ty);
    par.SetCovariance(0, 4, Cov_X_Qp);
    //    par.SetCovariance(0, 4, cov_const);
    par.SetCovariance(1, 1, Cov_Y_Y);
    par.SetCovariance(1, 2, Cov_Y_Tx);
    par.SetCovariance(1, 3, Cov_Y_Ty);
    par.SetCovariance(1, 4, Cov_Y_Qp);
    //    par.SetCovariance(1, 4, cov_const);
    par.SetCovariance(2, 2, Cov_Tx_Tx);
    par.SetCovariance(2, 3, Cov_Tx_Ty);
    par.SetCovariance(2, 4, Cov_Tx_Qp);
    //    par.SetCovariance(2, 4, cov_const);
    par.SetCovariance(3, 3, Cov_Ty_Ty);
    par.SetCovariance(3, 4, Cov_Ty_Qp);
    //    par.SetCovariance(3, 4, cov_const);
    par.SetCovariance(4, 4, Cov_Qp_Qp);
    //        par.SetCovariance(4, 4, cov_const);

    Float_t lX = lastHit->GetX();
    Float_t lY = lastHit->GetY();
    Float_t lZ = lastHit->GetZ();

    Float_t fX = firstHit->GetX();
    Float_t fY = firstHit->GetY();
    Float_t fZ = firstHit->GetZ();

    Float_t fR = Sqrt(fZ * fZ + fX * fX);
    Float_t lR = Sqrt(lZ * lZ + lX * lX);
    Float_t tgThetaF = Tan((fR - a) / b);
    Float_t sinThetaF = Sin((fR - a) / b);
    Float_t cosThetaF = Cos((fR - a) / b);
    Float_t tgThetaL = Tan((lR - a) / b);

    Float_t Tx_first = (b * tgThetaF + fR) / (b - fR * tgThetaF);
    Float_t Tx_last = (b * tgThetaL + fR) / (b - fR * tgThetaL);
    //    Float_t Tx_first = (b * fX + fZ * fR) / (b * fZ - fX * fR);
    //    Float_t Tx_last = (b * lX + lZ * lR) / (b * lZ - lX * lR);
    Float_t Ty_last = B; // / (lX - Xc);
    Float_t Ty_first = B; // / (fX - Xc);

    par.SetPosition(TVector3(lX, lY, lZ));
    par.SetTx(Tx_last);
    par.SetTy(Ty_last); //par.SetTy(-B / (lX - Xc));
    Float_t Rlast = Sqrt(Sqr(lX - Xc) + Sqr(lZ - Zc));
    const Float_t PxzLast = 0.0003 * fField->GetBy(lX, lY, lZ) * Rlast; // Pt
    if (Abs(PxzLast) < 0.00001) return kFALSE;
    const Float_t PzLast = PxzLast / Sqrt(1 + Sqr(Tx_last));
    const Float_t PxLast = PzLast * Tx_last;
    const Float_t PyLast = PzLast * Ty_last;
    Float_t QPLast = q / Sqrt(PxLast * PxLast + PyLast * PyLast + PzLast * PzLast);
    par.SetQp(QPLast);
    tr->SetParamLast(par);
    if (!IsParCorrect(&par)) return kFALSE;
    //    par.Print();

    //update for firstParam
    Float_t Rfirst = Sqrt(Sqr(fX - Xc) + Sqr(fZ - Zc));
    //    cout << Rfirst << " " << R << " " << fField->GetBy(fX, fY, fZ) << endl;
    const Float_t PxzFirst = 0.0003 * fField->GetBy(fX, fY, fZ) * Rfirst; // Pt
    if (Abs(PxzFirst) < 0.00001) return kFALSE;
    const Float_t PzFirst = PxzFirst / Sqrt(1 + Sqr(Tx_first));
    const Float_t PxFirst = PzFirst * Tx_first;
    const Float_t PyFirst = PzFirst * Ty_first;

    //    Float_t Ax = Sqrt(1 + Sqr(Tx_first) + Sqr(Ty_first)) * (Tx_first * Ty_first * fField->GetBx(fX, fY, fZ) - (1 + Sqr(Tx_first)) * fField->GetBy(fX, fY, fZ) + Ty_first * fField->GetBz(fX, fY, fZ));
    //    Float_t Ay = Sqrt(1 + Sqr(Tx_first) + Sqr(Ty_first)) * ((1 + Sqr(Ty_first)) * fField->GetBx(fX, fY, fZ) - Tx_first * Ty_first * fField->GetBy(fX, fY, fZ) - Tx_first * fField->GetBz(fX, fY, fZ));
    //    cout << "Ax = " << Ax << " | Ay = " << Ay << endl;
    //    const Float_t k = 2.99792458 * 10e-4;
    //    Float_t dTxdz = (-1.0 / fR / sinThetaF) * (b * b + fR * fR) / Sqr(b * cosThetaF - fR * sinThetaF);
    //    
    //    Float_t qpTmp = dTxdz / k / Ax;   
    Float_t QPFirst = q / Sqrt(PxFirst * PxFirst + PyFirst * PyFirst + PzFirst * PzFirst);

    //    cout << QPFirst << " " << qpTmp << endl;

    par.SetPosition(TVector3(fX, fY, fZ));
    par.SetQp(QPFirst);
    par.SetTx(Tx_first);
    par.SetTy(Ty_first); //par.SetTy(-B / (firstHit->GetX() - Xc));
    tr->SetParamFirst(par);
    if (!IsParCorrect(&par)) return kFALSE;
    //    par.Print();


    return kTRUE;
}

Bool_t BmnGemSeedFinder::CalculateTrackParamsParabolicSpiral(BmnGemTrack* tr, TLorentzVector* spirPar, TVector3* linePar, Short_t q) {
    //Needed for start approximation of track parameters

    const UInt_t nHits = tr->GetNHits();
    BmnGemStripHit* lastHit = GetHit(tr->GetHitIndex(nHits - 1));
    BmnGemStripHit* firstHit = GetHit(tr->GetHitIndex(0));

    if (!firstHit || !lastHit) return kFALSE;

    Float_t x0 = firstHit->GetX();
    Float_t z0 = firstHit->GetZ();
    Float_t z0_2 = z0 * z0;
    Float_t x0_2 = x0 * x0;
    Float_t R = spirPar->T();
    Float_t R2 = R * R;
    Float_t a = spirPar->X();
    Float_t b = spirPar->Y();
    Float_t c = spirPar->Z();

    Float_t tmp = x0_2 * R2 / (z0_2 + x0_2) - (z0_2 + x0_2) / 4;
    Float_t D = z0_2 + 4 * tmp;
    if (D < 0) return kFALSE;
    Float_t Zc = 0.5 * (Sqrt(D) - z0);
    Float_t Xc = Sqrt(R2 - z0_2);

    const Float_t B = linePar->X(); //angle coefficient for helix

    Float_t Xmean(0.0), Ymean(0.0), Zmean(0.0); // <Xi> , <Yi> , <Zi>
    Float_t ZXmean(0.0); // <(Zi-Zc)/(Xi-Xc)>
    Float_t ZX2mean(0.0); // <(Zi-Zc)/(Xi-Xc)^2>
    Float_t XZXmean(0.0); // <Xi*(Zi-Zc)/(Xi-Xc)>
    Float_t YZXmean(0.0); // <Yi*(Zi-Zc)/(Xi-Xc)>
    Float_t OneXmean(0.0); // <1/(Xi-Xc)>
    Float_t XOneXmean(0.0); // <Xi/(Xi-Xc)>
    Float_t YOneXmean(0.0); // <Yi/(Xi-Xc)>
    Float_t QPmean(0.0);
    Float_t XQPmean(0.0);
    Float_t YQPmean(0.0);
    Float_t ZXQPmean(0.0);
    Float_t OneXQPmean(0.0);

    //Covariance matrix
    Float_t Cov_X_X(0.0), Cov_X_Y(0.0), Cov_X_Tx(0.0), Cov_X_Ty(0.0), Cov_X_Qp(0.0);
    Float_t Cov_Y_Y(0.0), Cov_Y_Tx(0.0), Cov_Y_Ty(0.0), Cov_Y_Qp(0.0);
    Float_t Cov_Tx_Tx(0.0), Cov_Tx_Ty(0.0), Cov_Tx_Qp(0.0);
    Float_t Cov_Ty_Ty(0.0), Cov_Ty_Qp(0.0);
    Float_t Cov_Qp_Qp(0.0);
    Float_t S = 0.0;

    for (UInt_t i = 0; i < nHits; ++i) {
        BmnGemStripHit* hit = GetHit(i);
        if (!hit) continue;
        Float_t Xi = hit->GetX();
        Float_t Yi = hit->GetY();
        Float_t Zi = hit->GetZ();
        S = 0.0003 * fField->GetBy(hit->GetX(), hit->GetY(), hit->GetZ());
        Float_t QPi = q / S / Sqrt(R2 + B * B);

        QPmean += QPi;
        Xmean += Xi;
        Ymean += Yi;
        Zmean += Zi;
        ZXmean += (Zi - Zc) / (Xi - Xc);
        ZXQPmean += (Zi - Zc) / (Xi - Xc) * QPi;
        ZX2mean += (Zi - Zc) / (Xi - Xc) / (Xi - Xc);
        XZXmean += (Zi - Zc) / (Xi - Xc) * Xi;
        YZXmean += (Zi - Zc) / (Xi - Xc) * Yi;
        OneXmean += 1.0 / (Xi - Xc);
        OneXQPmean += 1.0 / (Xi - Xc) * QPi;
        XOneXmean += Xi / (Xi - Xc);
        YOneXmean += Yi / (Xi - Xc);
        XQPmean += Xi * QPi;
        YQPmean += Yi * QPi;

        Cov_X_X += Sqr(Xi);
        Cov_X_Y += Xi * Yi;
        Cov_Y_Y += Sqr(Yi);
        Cov_Tx_Tx += Sqr((Zi - Zc) / (Xi - Xc));
        Cov_Ty_Ty += Sqr(1.0 / (Xi - Xc));
        Cov_Qp_Qp += Sqr(QPi);
    }
    Xmean /= nHits;
    Ymean /= nHits;
    Zmean /= nHits;
    ZXmean /= nHits;
    ZX2mean /= nHits;
    XZXmean /= nHits;
    YZXmean /= nHits;
    OneXmean /= nHits;
    XOneXmean /= nHits;
    YOneXmean /= nHits;
    XQPmean /= nHits;
    YQPmean /= nHits;
    QPmean /= nHits;
    ZXQPmean /= nHits;
    OneXQPmean /= nHits;

    Cov_X_X = Cov_X_X / nHits - Sqr(Xmean);
    Cov_X_Y = Cov_X_Y / nHits - Xmean * Ymean;
    Cov_X_Tx = Xmean * ZXmean - XZXmean;
    Cov_X_Ty = 0.0; //B * (Xmean * OneXmean - XOneXmean);
    Cov_Y_Y = Cov_Y_Y / nHits - Sqr(Ymean);
    Cov_Y_Tx = Ymean * ZXmean - YZXmean;
    Cov_Y_Ty = 0.0; //B * (Ymean * OneXmean - YOneXmean);
    Cov_Tx_Ty = B * (ZX2mean - ZXmean * OneXmean);
    Cov_Tx_Tx = Cov_Tx_Tx / nHits - Sqr(ZXmean);
    Cov_Ty_Ty = Sqr(B) * (Cov_Ty_Ty / nHits - OneXmean * OneXmean);
    Cov_Qp_Qp = Cov_Qp_Qp / nHits - Sqr(QPmean);
    Cov_X_Qp = XQPmean - Xmean * QPmean;
    Cov_Y_Qp = YQPmean - Ymean * QPmean;
    Cov_Tx_Qp = -1.0 * ZXQPmean + ZXmean * QPmean;
    Cov_Ty_Qp = 0.0; //B * OneXQPmean - OneXmean * QPmean;

    FairTrackParam par;
    const Float_t cov_const = 1e-15;
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

    Float_t lX = lastHit->GetX();
    Float_t lY = lastHit->GetY();
    Float_t lZ = lastHit->GetZ();

    Float_t fX = firstHit->GetX();
    Float_t fY = firstHit->GetY();
    Float_t fZ = firstHit->GetZ();

    Float_t fR = Sqrt(fZ * fZ + fX * fX);
    Float_t lR = Sqrt(lZ * lZ + lX * lX);
    Float_t ft = ATan2(fX, fZ);
    Float_t lt = ATan2(lX, lZ);

    Float_t Tx_f = ((2 * a * ft + b) * Tan(ft) + fR) / (2 * a * ft + b - fR * Tan(ft));
    Float_t Tx_l = ((2 * a * lt + b) * Tan(lt) + lR) / (2 * a * lt + b - lR * Tan(lt));

    Float_t Ty_l = B;
    Float_t Ty_f = B;

    par.SetPosition(TVector3(lX, lY, lZ));
    par.SetTx(Tx_l);
    par.SetTy(Ty_l);
    Float_t Rlast = Sqrt(Sqr(lX - Xc) + Sqr(lZ - Zc));
    const Float_t PxzLast = 0.0003 * fField->GetBy(lX, lY, lZ) * Rlast; // Pt
    if (Abs(PxzLast) < 0.00001) return kFALSE;
    const Float_t PzLast = PxzLast / Sqrt(1 + Sqr(Tx_l));
    const Float_t PxLast = PzLast * Tx_l;
    const Float_t PyLast = PzLast * Ty_l;
    Float_t QPLast = q / Sqrt(PxLast * PxLast + PyLast * PyLast + PzLast * PzLast);
    par.SetQp(QPLast);
    tr->SetParamLast(par);
    if (!IsParCorrect(&par)) return kFALSE;

    //update for firstParam
    Float_t Rfirst = Sqrt(Sqr(fX - Xc) + Sqr(fZ - Zc));
    const Float_t PxzFirst = 0.0003 * fField->GetBy(fX, fY, fZ) * Rfirst; // Pt
    if (Abs(PxzFirst) < 0.00001) return kFALSE;
    const Float_t PzFirst = PxzFirst / Sqrt(1 + Sqr(Tx_f));
    const Float_t PxFirst = PzFirst * Tx_f;
    const Float_t PyFirst = PzFirst * Ty_f;
    Float_t QPFirst = q / Sqrt(PxFirst * PxFirst + PyFirst * PyFirst + PzFirst * PzFirst);
    par.SetPosition(TVector3(fX, fY, fZ));
    par.SetQp(QPFirst);
    par.SetTx(Tx_f);
    par.SetTy(Ty_f);
    tr->SetParamFirst(par);
    if (!IsParCorrect(&par)) return kFALSE;

    return kTRUE;
}

Float_t BmnGemSeedFinder::Dist(Float_t x1, Float_t y1, Float_t x2, Float_t y2) {
    if (Sqr(x1 - x2) + Sqr(y1 - y2) <= 0.0) {
        return 0.0;
    } else {
        return Sqrt(Sqr(x1 - x2) + Sqr(y1 - y2));
    }
}

BmnGemStripHit * BmnGemSeedFinder::GetHit(Int_t i) {
    if (i < 0 || i > fGemHitsArray->GetEntriesFast()) return NULL;
    BmnGemStripHit* hit = (BmnGemStripHit*) fGemHitsArray->At(i);
    if (!hit) return NULL;

    Bool_t skip = kFALSE;
    for (Int_t iSkip = 0; iSkip < skipStations.size(); ++iSkip)
        if (hit->GetStation() == skipStations.at(iSkip)) {
            skip = kTRUE;
            break;
        }
    if (skip) return NULL;
    if (hit->GetY() > fYmax || hit->GetY() < fYmin) return NULL;
    if (hit->GetX() > fXmax || hit->GetX() < fXmin) return NULL;
    //    if (hit->GetClusterSizeInLowerLayer() == 1 && hit->GetClusterSizeInUpperLayer() == 1) return NULL;
    //tmp!!!
    //    if (hit->GetStation() == 1 && (hit->GetX() > 0.7 || hit->GetX() < 0.4)) return NULL;

    //+/-0.5 sigma
    //    if (hit->GetStation() == 0 && (hit->GetX() > 1.50 || hit->GetX() < 0.50)) return NULL;
    //    if (hit->GetStation() == 1 && (hit->GetX() > 2.50 || hit->GetX() < 1.50)) return NULL;
    //    if (hit->GetStation() == 2 && (hit->GetX() > 3.50 || hit->GetX() < 2.50)) return NULL;
    //    if (hit->GetStation() == 3 && (hit->GetX() > 5.00 || hit->GetX() < 4.00)) return NULL;
    //    if (hit->GetStation() == 4 && (hit->GetX() > 6.50 || hit->GetX() < 5.50)) return NULL;
    //    if (hit->GetStation() == 5 && (hit->GetX() > 8.50 || hit->GetX() < 6.50)) return NULL;
    //+/-1.5 sigma
    //            if (hit->GetStation() == 3 && (hit->GetX() > 2.7 || hit->GetX() < 1.5)) return NULL;
    //            if (hit->GetStation() == 4 && (hit->GetX() > 4.4 || hit->GetX() < 2.5)) return NULL;
    //            if (hit->GetStation() == 5 && (hit->GetX() > 6.0 || hit->GetX() < 4.0)) return NULL;
    //            if (hit->GetStation() == 6 && (hit->GetX() > 8.0 || hit->GetX() < 6.0)) return NULL;

    //            for (Int_t j = 0; j < fGemHitsArray->GetEntriesFast(); ++j) {
    //                BmnGemStripHit* hitJ = (BmnGemStripHit*) fGemHitsArray->At(j);
    //                if (hitJ->GetStation() == hit->GetStation() && i != j)
    //                    return NULL;
    //            }

    if (!hit) {
        cout << "-W- Wrong attempting to get hit number " << i << " from fGemHitsArray, which contains " << fGemHitsArray->GetEntriesFast() << " elements" << endl;
        return NULL;
    }
    return hit;
}

void BmnGemSeedFinder::FillAddr() {
    //Needed for searching seeds by addresses 
    TVector3 roughVertex(0.0, -4.5, 0.0);
    for (Int_t hitIdx = 0; hitIdx < fGemHitsArray->GetEntriesFast(); ++hitIdx) {
        BmnGemStripHit* hit = GetHit(hitIdx);
        if (!hit) continue;
        if (hit->IsUsed()) continue; //Don't use used hits
        if (hit->GetStation() > kMAXSTATIONFORSEED + kNHITSFORSEED) continue;
        const Float_t R = Sqrt(Sqr(hit->GetX() - roughVertex.X()) + Sqr(hit->GetY() - roughVertex.Y()) + Sqr(hit->GetZ() - roughVertex.Z()));
        const Float_t newX = hit->GetX() / R;
        const Float_t newY = hit->GetY() / R;
        Int_t xAddr = ceil((newX - fMin) / fWidth);
        Int_t yAddr = ceil((newY - fMin) / fWidth);
        ULong_t addr = yAddr * fNBins + xAddr;
        hit->SetAddr(addr);
        hit->SetXaddr(xAddr);
        hit->SetYaddr(yAddr);
        hit->SetIndex(hitIdx);
        hit->SetFlag(kFALSE);
        fAddresses[xAddr][yAddr] = hitIdx;
    }
    //cout << "addresses.size = " << addresses.size() << endl;
}

void BmnGemSeedFinder::FillAddrWithLorentz(Float_t sigma_x, Float_t yStep, Float_t trs) {

    //Needed for searching seeds by addresses 
    Float_t sigma_x2 = sigma_x * sigma_x;

    for (Int_t hitIdx = 0; hitIdx < fGemHitsArray->GetEntriesFast(); ++hitIdx) {
        BmnGemStripHit* hit = GetHit(hitIdx);
        if (!hit) continue;
        if (hit->GetStation() > kMAXSTATIONFORSEED + kNHITSFORSEED) continue;

        hit->SetFlag(kFALSE); // by default hits are not filtered 

        TVector2 XYnew = GetTransXY(hit);

        Int_t xAddr = ceil((XYnew.X() - fMin) / fWidth);
        Int_t yAddr = ceil((XYnew.Y() - fMin) / fWidth);
        ULong_t addr = yAddr * fNBins + xAddr;

        hit->SetAddr(addr);
        hit->SetXaddr(xAddr);
        hit->SetYaddr(yAddr);
        hit->SetIndex(hitIdx);

    }

    for (Int_t hitIdx = 0; hitIdx < fGemHitsArray->GetEntriesFast(); ++hitIdx) {
        BmnGemStripHit* hit = GetHit(hitIdx);
        if (!hit) continue;
        if (hit->GetStation() > kMAXSTATIONFORSEED + kNHITSFORSEED) continue;

        Int_t yAddr = hit->GetYaddr();
        Int_t xAddr = hit->GetXaddr();
        if (yAddr < 0 || yAddr >= fNBins || xAddr < 0 || xAddr >= fNBins) {
            continue;
        }
        
        const Float_t newX = GetTransXY(hit).X();

        Float_t potSum = 0.0; //sum of all potentials
        for (Int_t hitIdx0 = 0; hitIdx0 < fGemHitsArray->GetEntriesFast(); ++hitIdx0) {
            BmnGemStripHit* hit0 = GetHit(hitIdx0);
            if (!hit0) continue;
            if (hit0->GetStation() > kMAXSTATIONFORSEED + kNHITSFORSEED) continue;
            Int_t yAddr0 = hit0->GetYaddr();
            Int_t xAddr0 = hit0->GetXaddr();
            if (yAddr0 < 0 || yAddr0 >= fNBins || xAddr0 < 0 || xAddr0 >= fNBins) continue;
            if (Abs(yAddr0 - yAddr) > yStep) continue; //hits should be in the same Y-coridor

            const Float_t newX0 = GetTransXY(hit0).X();
            Float_t pot = sigma_x2 / (sigma_x2 + Sqr(newX0 - newX));
            potSum += pot;
        }
        if (potSum > trs) {
            hit->SetFlag(kTRUE);
            fAddresses[xAddr][yAddr] = hitIdx;
        }
    }

    //    cout << "addresses.size = " << addresses.size() << endl;
}

void BmnGemSeedFinder::SetHitsUnused(BmnGemTrack * tr) {
    for (Int_t i = 0; i < tr->GetNHits(); ++i)
        GetHit(tr->GetHitIndex(i))->SetUsing(kFALSE);
}

TVector2 BmnGemSeedFinder::GetTransXY(BmnGemStripHit* hit) {
    //const Float_t oneOverR = 1.0 / Sqrt(Sqr(hit->GetX()) + Sqr(hit->GetY()) + Sqr(hit->GetZ()));
    //    const Float_t oneOverR = 1.0 / Sqrt(Sqr(hit->GetX() - fRoughVertex.X()) + Sqr(hit->GetY() - fRoughVertex.Y()) + Sqr(hit->GetZ() - fRoughVertex.Z()));
    const Float_t oneOverR = 1.0 / Sqrt(Sqr(hit->GetX() - fRoughVertex.X()) + Sqr(hit->GetY() - fRoughVertex.Y()) + Sqr(hit->GetZ()));
    //    const Float_t oneOverR = 1.0 / hit->GetZ();
    return TVector2((hit->GetX() - fRoughVertex.X()) * oneOverR, (hit->GetY() - fRoughVertex.Y()) * oneOverR);
}
