#include "TProfile.h"
#include "TRandom.h"
#include "TMath.h"
#include "TCanvas.h"
#include "BmnGemSeedFinder.h"
#include "BmnMath.h"
#include "BmnFieldMap.h"

static Float_t workTime = 0.0;
//-----------------------------------------

map<ULong_t, Int_t> addresses; // map for calculating addresses of hits in histogram {x/R, y/R}
const UInt_t kNHITSFORSEED = 12; // we use for seeds only kNHITSFORSEED hits
const UInt_t kMAXSTATIONFORSEED = 5; // we start to search seeds only from stations in range from 0 up to kMAXSTATIONFORSEED
Float_t kCHI2CUT = 1.0; //0.7;
Float_t kSIGMACUT = 0.5;

using std::cout;
using namespace TMath;

BmnGemSeedFinder::BmnGemSeedFinder() : fEventNo(0) {
    fUseLorentz = kFALSE;
    fIsField = kFALSE;
    fGemHitsArray = NULL;
    fGemSeedsArray = NULL;
    fHitsBranchName = "BmnGemStripHit";
    fSeedsBranchName = "BmnGemTrack";
}

BmnGemSeedFinder::~BmnGemSeedFinder() {
}

InitStatus BmnGemSeedFinder::Init() {

    cout << "======================== Seed finder init started =========================" << endl;

    //Get ROOT Manager
    FairRootManager* ioman = FairRootManager::Instance();
    if (NULL == ioman) {
        Fatal("Init", "FairRootManager is not instantiated");
    }

    fGemHitsArray = (TClonesArray*) ioman->GetObject(fHitsBranchName); //in
    if (fGemHitsArray == NULL) {
        cout << "ERROR: No input hits array" << endl;
        return kERROR;
    }
    fGemSeedsArray = new TClonesArray(fSeedsBranchName, 100); //out
    ioman->Register("BmnGemSeeds", "GEM", fGemSeedsArray, kTRUE);


    fMCTracksArray = (TClonesArray*) ioman->GetObject("MCTrack");
    fMCPointsArray = (TClonesArray*) ioman->GetObject("StsPoint");

    fField = FairRunAna::Instance()->GetField();

    cout << "======================== Seed finder init finished ========================" << endl;
}

void BmnGemSeedFinder::Exec(Option_t* opt) {
    clock_t tStart = clock();
    cout << "\n======================== GEM seed finder exec started =====================\n" << endl;
    cout << "Event number: " << fEventNo++ << endl;

    fGemSeedsArray->Clear();

    //GEM inefficiency ===>
    //    const Float_t eff = 1.00;
    //    for (Int_t hitIdx = 0; hitIdx < fGemHitsArray->GetEntriesFast(); ++hitIdx) {
    //        BmnGemStripHit* hit = GetHit(hitIdx);
    //        if (gRandom->Uniform(1.0) > eff) {
    //            hit->SetUsing(kTRUE);
    //        }
    //    }
    // <=== GEM ineficiency

    const Int_t nIter = 5;
    //(0.006, 6.0, 1.05); //best parameters
    const Int_t nBins[nIter] = {2000, 2000, 2000, 1000, 1000};
    const Float_t sigX[nIter] = {0.005, 0.01, 0.05, 0.1, 0.5};
    const Float_t stpY[nIter] = {1.0, 1.0, 2.0, 2.0, 9.0};
    const Float_t thrs[nIter] = {2.5, 1.1, 1.1, 1.1, 1.1};
    const Int_t length[nIter] = {4, 4, 4, 4, 4};
    const Float_t chi2cut[nIter] = {5.0, 4.0, 3.0, 2.0, 2.0};

    //Just skip too big events
    if (fGemHitsArray->GetEntriesFast() > 500) return;

    if (fIsField) {
        for (Int_t i = 0; i < nIter; ++i) {
            addresses.clear();

            fNBins = nBins[i]; //1500;//3000;
            fMin = -0.5;
            fMax = -fMin;
            fWidth = (fMax - fMin) / fNBins;

            kSIG_X = sigX[i];
            kY_STEP = stpY[i];
            kTRS = thrs[i];
            kNHITSFORFIT = length[i];
            kCHI2CUT = chi2cut[i];
            if (fUseLorentz) {
                FillAddrWithLorentz(kSIG_X, kY_STEP, kTRS);
            } else {
                FillAddr();
            }

            DoSeeding(Int_t(kY_STEP - 1), Int_t(kY_STEP), fGemSeedsArray);
            cout << "ITER: " << i << " | fGemSeedsArray->GetEntriesFast() = " << fGemSeedsArray->GetEntriesFast() << endl;
        }

        // for "secondaries"
        kNHITSFORFIT = 4;
        kCHI2CUT = 50.0;
        kSIGMACUT = 1.0;
        FindYZSeeds();
        cout << "STRIGHT | fGemSeedsArray->GetEntriesFast() = " << fGemSeedsArray->GetEntriesFast() << endl;
    } else {
        kNHITSFORFIT = 3;
        FindStrightSeeds();
    }
    cout << "\nGEM_SEEDING: Number of found seeds: " << fGemSeedsArray->GetEntriesFast() << endl;


    for (Int_t iHit = 0; iHit < fGemHitsArray->GetEntriesFast(); ++iHit)
        GetHit(iHit)->SetUsing(kFALSE);
    clock_t tFinish = clock();
    workTime += ((Float_t) (tFinish - tStart)) / CLOCKS_PER_SEC;

    cout << "\n======================== Seed finder exec finished ========================" << endl;

}

BmnStatus BmnGemSeedFinder::FindStrightSeeds() {
    
    vector<BmnGemTrack> tgA_06;
    CollectSeeds(tgA_06);

    Float_t minDr = 1000.0;
    Int_t minIdx = -1;
    BmnGemTrack minTrack;

    for (Int_t i = 0; i < tgA_06.size(); ++i) {
        BmnGemTrack tr = tgA_06.at(i);
        if (tr.GetNHits() < kNHITSFORFIT) continue;
        TVector3 zxPar = LineFit(&tr, fGemHitsArray, "ZX");
        TVector3 zyPar = LineFit(&tr, fGemHitsArray, "ZY");
        Float_t dR = Sqrt(zxPar.Z() * zxPar.Z() + zyPar.Z() * zyPar.Z());
        if (dR < minDr) {
            FairTrackParam par = *(tr.GetParamFirst());
            par.SetX(zxPar.Y());
            par.SetY(zyPar.Y());
            par.SetZ(0.0);
            par.SetTx(zxPar.X());
            par.SetTy(zyPar.X());
            tr.SetNDF(tr.GetNHits());

            FairTrackParam par1;
            par1.SetX(zxPar.Z());
            par1.SetY(zyPar.Z());
            par1.SetZ(dR);
            tr.SetParamFirst(par);
            tr.SetParamLast(par1);

            minDr = par1.GetZ();
            minIdx = i;
            minTrack = tr;
        }
    }

    if (minIdx != -1)
        new((*fGemSeedsArray)[fGemSeedsArray->GetEntriesFast()]) BmnGemTrack(minTrack);
}

BmnStatus BmnGemSeedFinder::FindYZSeeds() {

    vector<BmnGemTrack> tgA_06;
    CollectSeeds(tgA_06);

    Float_t minDr = kSIGMACUT;
    Int_t minIdx = -1;
    BmnGemTrack minTrack;

    for (Int_t i = 0; i < tgA_06.size(); ++i) {
        BmnGemTrack tr = tgA_06.at(i);
        TVector3 zyPar = LineFit(&tr, fGemHitsArray, "ZY");
        Float_t dR = zyPar.Z();
        if (dR < minDr) {
            FairTrackParam par = *(tr.GetParamFirst());
            par.SetY(zyPar.Y());
            par.SetZ(0.0);
            par.SetTy(zyPar.X());
            tr.SetNDF(tr.GetNHits());

            FairTrackParam par1;
            par1.SetY(zyPar.Z());
            par1.SetZ(dR);
            tr.SetParamFirst(par);
            tr.SetParamLast(par1);

            minDr = par1.GetZ();
            minIdx = i;
            minTrack = tr;
        }
    }

    if (minIdx != -1) {
        BmnGemTrack tr = minTrack;
        tr.SortHits();
        if (tr.GetNHits() < kNHITSFORFIT) return kBMNERROR;
        TVector3 linePar = LineFit(&tr, fGemHitsArray, "ZY");
        if (linePar.Z() > kSIGMACUT) return kBMNERROR;
        TVector3 circPar = CircleBy3Hit(&tr, fGemHitsArray);
        tr.SortHits();
        Float_t dX = GetHit(tr.GetHitIndex(1))->GetX() - GetHit(tr.GetHitIndex(0))->GetX();
        Int_t q = (dX > 0.0) ? +1 : -1;
        if (CalculateTrackParams(&tr, &circPar, &linePar, q)) {
            new((*fGemSeedsArray)[fGemSeedsArray->GetEntriesFast()]) BmnGemTrack(tr);
            for (Int_t i = 0; i < tr.GetNHits(); ++i)
                GetHit(tr.GetHitIndex(i))->SetUsing(kTRUE);
        }
    }

}

BmnStatus BmnGemSeedFinder::CollectSeeds(vector<BmnGemTrack>& tg) {
    vector<BmnGemTrack> tgA_01;
    vector<BmnGemTrack> tgA_12;
    vector<BmnGemTrack> tgA_23;
    vector<BmnGemTrack> tgA_34;
    vector<BmnGemTrack> tgA_45;
    vector<BmnGemTrack> tgA_56;
    FillTgStation(0, tgA_01);
    FillTgStation(1, tgA_12);
    FillTgStation(2, tgA_23);
    FillTgStation(3, tgA_34);
    FillTgStation(4, tgA_45);
    FillTgStation(5, tgA_56);

    vector<BmnGemTrack> tgA_13;
    vector<BmnGemTrack> tgA_35;
    if (CombineTg(tgA_12, tgA_23, tgA_13) == kBMNERROR) return kBMNERROR;
    if (CombineTg(tgA_34, tgA_45, tgA_35) == kBMNERROR) return kBMNERROR;

    vector<BmnGemTrack> tgA_15;
    if (CombineTg(tgA_13, tgA_35, tgA_15) == kBMNERROR) return kBMNERROR;
    
    vector<BmnGemTrack> tgA_16;
    if (CombineTg(tgA_15, tgA_56, tgA_16) == kBMNERROR) return kBMNERROR;

    if (CombineTg(tgA_01, tgA_16, tg) == kBMNERROR) return kBMNERROR;
}

BmnStatus BmnGemSeedFinder::CombineTg(vector<BmnGemTrack> tg1, vector<BmnGemTrack> tg2, vector<BmnGemTrack>& tg) {

    if (tg1.size() > 100 || tg2.size() > 100) {
        return kBMNERROR;
    }
    if (tg1.size() == 0 && tg2.size() == 0) {
        return kBMNERROR;
    } else if (tg1.size() == 0 && tg2.size() != 0) {
        for (Int_t iTg = 0; iTg < tg2.size(); ++iTg) {
            tg.push_back(tg2[iTg]);
        }
    } else if (tg1.size() != 0 && tg2.size() == 0) {
        for (Int_t iTg = 0; iTg < tg1.size(); ++iTg) {
            tg.push_back(tg1[iTg]);
        }
    } else {
        for (Int_t iTg1 = 0; iTg1 < tg1.size(); ++iTg1) {
            BmnGemTrack t1 = tg1[iTg1];
            for (Int_t iTg2 = 0; iTg2 < tg2.size(); ++iTg2) {
                BmnGemTrack t2 = tg2[iTg2];
                BmnGemTrack tr;
                FairTrackParam par;
                par.SetTy(0.5 * (t2.GetParamFirst()->GetTy() + t1.GetParamFirst()->GetTy()));
                tr.SetParamFirst(par);
                //                		cout << t1.GetNHits() << " " << t2.GetNHits() << endl;
                for (Int_t i = 0; i < t1.GetNHits(); ++i)
                    tr.AddHit(t1.GetHitIndex(i), GetHit(t1.GetHitIndex(i)));
                for (Int_t i = 0; i < t2.GetNHits(); ++i)
                    tr.AddHit(t2.GetHitIndex(i), GetHit(t2.GetHitIndex(i)));
                tr.SortHits();
                tg.push_back(tr); //mean tangent
            }
        }
    }
    return kBMNSUCCESS;
}

BmnStatus BmnGemSeedFinder::FillTgStation(Short_t st, vector<BmnGemTrack>& tg) {
    for (Int_t iHit1 = 0; iHit1 < fGemHitsArray->GetEntriesFast(); ++iHit1) {
        BmnGemStripHit* hit1 = GetHit(iHit1);
        hit1->SetX(hit1->GetX() * (-1.0));
        if (hit1->IsUsed()) continue;
        if (hit1->GetStation() != st) continue;
        for (Int_t iHit2 = 0; iHit2 < fGemHitsArray->GetEntriesFast(); ++iHit2) {
            BmnGemStripHit* hit2 = GetHit(iHit2);
            if (hit2->IsUsed()) continue;
            if (hit2->GetStation() != (st + 1)) continue;
            Float_t tgA = (hit2->GetY() - hit1->GetY()) / (hit2->GetZ() - hit1->GetZ());
            if (Abs(tgA) > 1.0) continue;
            BmnGemTrack tr;
            FairTrackParam par;
            par.SetTy(tgA);
            tr.SetParamFirst(par);
            tr.AddHit(iHit1, hit1);
            tr.AddHit(iHit2, hit2);
            tr.SortHits();
            tg.push_back(tr);
        }
    }
}

BmnStatus BmnGemSeedFinder::DoSeeding(Int_t min, Int_t max, TClonesArray * arr) {

    for (Int_t i = 1/*0*/; i < 6/*kMAXSTATIONFORSEED*/; ++i)
        for (Int_t j = min; j < max; ++j)
            //            FindSeeds(i, j, kFALSE, arr); // from station #i, in gate = 2 * j + 1, only hits presented in every station
            FindSeeds(i, j, kTRUE, arr); // from station #i, in gate = 2 * j + 1, only hits presented in every station
    return kBMNSUCCESS;
}

void BmnGemSeedFinder::Finish() {
    ofstream outFile;
    outFile.open("QA/timing.txt");
    outFile << "Seed Finder Time: " << workTime << endl;
    cout << "Work time of the GEM seed finder: " << workTime << endl;
    addresses.clear();
}

// ========================================================== //
//                                                            //
//                Search track-candidates in                  //
//            2D histogram for {x/R, y/R} - space.            //
//                 R = Sqrt(x^2 + y^2 + z^2)                  //
//                                                            //
// ========================================================== //

void BmnGemSeedFinder::FindSeeds(Int_t startStation, Int_t gate, Bool_t isIdeal, TClonesArray * arr) {

    //search only kNHITSFORSEED first hits of track
    //search in left and right directions

    Int_t trCntr = 0;
    trCntr += SearchTrackCandidates(startStation, gate, isIdeal, kTRUE, arr);
    trCntr += SearchTrackCandidates(startStation, gate, isIdeal, kFALSE, arr);

    //    cout << "GEM_SEEDING: Number of candidates ( Stat = " << startStation << ", gate = " << gate * 2 + 1 << ", isIdeal = " << isIdeal << "): " << trCntr << endl;
}

UInt_t BmnGemSeedFinder::SearchTrackCandidates(Int_t startStation, Int_t gate, Bool_t isIdeal, Bool_t isLeft, TClonesArray * arr) {
    UInt_t trCntr = 0;
    for (Int_t iHit = 0; iHit < fGemHitsArray->GetEntriesFast(); ++iHit) {

        BmnGemStripHit* hit = GetHit(iHit);

        if (hit->IsUsed()) continue;
        if (startStation != hit->GetStation()) continue;

        Int_t yAddr = hit->GetYaddr();

        if ((yAddr - gate) < 0 || (yAddr + gate) > fNBins) continue;

        Int_t xAddr = hit->GetXaddr();
        BmnGemTrack trackCand;

        Int_t dist = 0;
        Int_t maxDist = 0;
        Int_t hitCntr = 0;
        Int_t startBin = 0;
        Int_t prevStation = startStation - 1; //number of starting station
        Int_t nSteps = 2;
        Short_t q = 0; // charge of particle
        if (isLeft) { //search track-candidate in left direction
            q = -1;
            for (Int_t i = xAddr; i > 0; i--) {
                SearchTrackCandInLine(i, yAddr, &trackCand, &hitCntr, &maxDist, &dist, &startBin, &prevStation, gate, isIdeal);
                if ((hitCntr > 1) && Abs(i - startBin) > nSteps * maxDist) break; //condition to finish search is dist < 2 * MaxDist
                if (hitCntr >= kNHITSFORSEED) break;
            }
        } else { //search track-candidate in right direction
            q = +1;
            for (Int_t i = xAddr; i < fNBins; ++i) {
                SearchTrackCandInLine(i, yAddr, &trackCand, &hitCntr, &maxDist, &dist, &startBin, &prevStation, gate, isIdeal);
                if ((hitCntr > 1) && Abs(i - startBin) > nSteps * maxDist) break; //condition to finish search is dist < 2 * MaxDist
                if (hitCntr >= kNHITSFORSEED) break;
            }
        }
        
        trackCand.SortHits();
        if (trackCand.GetNHits() < kNHITSFORFIT) {
            SetHitsUnused(&trackCand);
            continue;
        }

        TVector3 linePar = LineFit(&trackCand, fGemHitsArray, "ZY");
        if (linePar.Z() > kSIGMACUT) {
            SetHitsUnused(&trackCand);
            continue;
        }

        //        TVector3 spirPar = SpiralFit(&trackCand, fGemHitsArray);
        //        if (ChiSq(&spirPar, &trackCand, fGemHitsArray, "spiral") > kCHI2CUT) {
        //            SetHitsUnused(&trackCand);
        //            continue;
        //        }
        trCntr++;
        trackCand.SortHits();

        TVector3 circPar = CircleBy3Hit(&trackCand, fGemHitsArray);
        if (CalculateTrackParams(&trackCand, &circPar, &linePar, q))
            /*if (CalculateTrackParamsSpiral(&trackCand, &spirPar, &linePar, q))*/ {
            //cout << "R = " << circPar.Z() << " | chi^2 = " << ChiSq(&circPar, &trackCand, fGemHitsArray, "circle") << endl;
            //cout << "P = " << 1.0 / trackCand.GetParamFirst()->GetQp() << endl;
            for (Int_t i = 0; i < trackCand.GetNHits(); ++i) {
                //cout << GetHit(trackCand.GetHitIndex(i))->GetX() << " " << GetHit(trackCand.GetHitIndex(i))->GetY() << " " << GetHit(trackCand.GetHitIndex(i))->GetZ() << endl;
                GetHit(trackCand.GetHitIndex(i))->SetUsing(kTRUE);
            }
            new((*arr)[arr->GetEntriesFast()]) BmnGemTrack(trackCand);
        } else {
            SetHitsUnused(&trackCand);
            continue;
        }
    }
    return trCntr;
}

void BmnGemSeedFinder::SearchTrackCandInLine(const Int_t i, const Int_t y, BmnGemTrack* tr, Int_t* hitCntr, Int_t* maxDist, Int_t* dist, Int_t* startBin, Int_t* prevStation, Int_t gate, Bool_t isIdeal) {

    for (Int_t j = y - gate; j <= y + gate; ++j) {

        ULong_t addr = j * fNBins + i;
        if (addresses.find(addr) == addresses.end()) continue;

        Int_t id = addresses.find(addr)->second;
        BmnGemStripHit* hit = GetHit(id);

        if (hit->IsUsed()) continue;
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
        hit->SetUsing(kTRUE);
        tr->AddHit(id, hit);
    }
}

Bool_t BmnGemSeedFinder::CalculateTrackParams(BmnGemTrack* tr, TVector3* circPar, TVector3* linePar, Short_t q) {
    //Needed for start approximation of track parameters

    Float_t R = circPar->Z(); // radius of fit-circle
    Float_t Xc = circPar->X(); // x-coordinate of fit-circle center
    Float_t Zc = circPar->Y(); // z-coordinate of fit-circle center
    fField = FairRunAna::Instance()->GetField();
    const UInt_t nHits = tr->GetNHits();
    BmnGemStripHit* lastHit = GetHit(tr->GetHitIndex(nHits - 1));
    BmnGemStripHit* firstHit = GetHit(tr->GetHitIndex(0));
    if (!firstHit || !lastHit) return kFALSE;

    const Float_t B = linePar->X(); //angle coefficient for helicoid

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
    Float_t Q = q;
    Float_t S = 0.0003 * (fField->GetBy(firstHit->GetX(), firstHit->GetY(), firstHit->GetZ()));
    Float_t QP = Q / S / Sqrt(R * R + B * B);

    for (UInt_t i = 0; i < nHits; ++i) {
        BmnGemStripHit* hit = GetHit(tr->GetHitIndex(i));
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
    //    par.SetCovariance(4, 4, cov_const);

    Float_t lX = lastHit->GetX();
    Float_t lY = lastHit->GetY();
    Float_t lZ = lastHit->GetZ();

    Float_t fX = firstHit->GetX();
    Float_t fY = firstHit->GetY();
    Float_t fZ = firstHit->GetZ();

    //    Float_t h = (fX < Xc) ? -1.0 : 1.0;
    Float_t h = -1.0;

    Float_t Tx_first = h * (fZ - Zc) / (fX - Xc);
    Float_t Tx_last = h * (lZ - Zc) / (lX - Xc);
    Float_t Ty_last = B; // / (lX - Xc);
    Float_t Ty_first = B; // / (fX - Xc);

    par.SetPosition(TVector3(lX, lY, lZ));
    par.SetTx(Tx_last);
    par.SetTy(Ty_last); //par.SetTy(-B / (lX - Xc));
    const Float_t PxzLast = 0.0003 * fField->GetBy(lX, lY, lZ) * R; // Pt
    //        const Float_t PxzLast = 0.0003 * fField->GetBy(lX, lY, lZ) * R; // Pt
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
    const Float_t PxzFirst = 0.0003 * fField->GetBy(fX, fY, fZ) * R; // Pt
    if (Abs(PxzFirst) < 0.00001) return kFALSE;
    const Float_t PzFirst = PxzFirst / Sqrt(1 + Sqr(Tx_first));
    const Float_t PxFirst = PzFirst * Tx_first;
    const Float_t PyFirst = PzFirst * Ty_first;
    Float_t QPFirst = q / Sqrt(PxFirst * PxFirst + PyFirst * PyFirst + PzFirst * PzFirst);
    par.SetPosition(TVector3(fX, fY, fZ));
    par.SetQp(QPFirst);
    par.SetTx(Tx_first);
    par.SetTy(Ty_first); //par.SetTy(-B / (firstHit->GetX() - Xc));
    tr->SetParamFirst(par);
    if (!IsParCorrect(&par)) return kFALSE;
    //    par.Print();


    return kTRUE;
}

Bool_t BmnGemSeedFinder::CalculateTrackParamsSpiral(BmnGemTrack* tr, TVector3* spirPar, TVector3* linePar, Short_t q) {
    //Needed for start approximation of track parameters

    const UInt_t nHits = tr->GetNHits();
    BmnGemStripHit* lastHit = GetHit(tr->GetHitIndex(nHits - 1));
    //    BmnGemStripHit* preLastHit = GetHit(tr->GetHitIndex(nHits - 2));
    BmnGemStripHit* firstHit = GetHit(tr->GetHitIndex(0));
    //    BmnGemStripHit* secondHit = GetHit(tr->GetHitIndex(1));

    if (!firstHit || !lastHit) return kFALSE;

    Float_t x0 = firstHit->GetX();
    Float_t z0 = firstHit->GetZ();
    Float_t z0_2 = z0 * z0;
    Float_t x0_2 = x0 * x0;
    Float_t R = spirPar->Z();
    Float_t R2 = R * R;
    Float_t a = spirPar->X();
    Float_t b = spirPar->Y();

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

TVector3 BmnGemSeedFinder::CircleFit(BmnGemTrack * track) {

    Float_t Xi, Yi; //centered coordinates
    Float_t Mx, My; // 1-st momentum
    Float_t Ri; // radius
    Float_t Mr, Mxy, Mxx, Myy, Mxr, Myr, Mrr, Mxr2, Myr2, Cov_xy;
    const Float_t C = 3; // parameter for robust fitting (3*Sigma rule)
    Float_t Wi = 1.0; // weight for robust approach // In first approach it's equal 1.0
    Float_t Di = 0.0; //distance between circle and point
    Float_t Sig = 1.0;
    Float_t thresh = C * Sig; // threshold for weights calculating
    Float_t Sw = 0.0; // sum of weights
    Float_t Swd = 0.0; // sum of (w * d^2)
    Float_t A0, A1, A2, A22;
    Float_t GAM, DET;
    Float_t Xc, Yc, R;
    Float_t XcPrev = 1000000.0, YcPrev = 1000000.0; // coordinates of circle center on previous iteration
    Float_t Rprev = 1000000.0; //radius of circle on previous iteration

    const Float_t nHits = track->GetNHits();

    //=============== first approximation with equal weights ===============//
    Mx = My = Mxx = Myy = Mxy = Mxr = Myr = Mrr = 0.0;

    for (Int_t i = 0; i < nHits; ++i) {
        BmnGemStripHit* hit = GetHit(track->GetHitIndex(i));
        if (!(hit->IsUsed())) continue;
        Mx += Wi * hit->GetX();
        My += Wi * hit->GetZ();
        Sw += Wi;
    }
    Mx /= Sw;
    My /= Sw;

    // computing moments
    for (Int_t i = 0; i < nHits; ++i) {
        BmnGemStripHit* hit = GetHit(track->GetHitIndex(i));
        if (!(hit->IsUsed())) continue;
        Xi = hit->GetX() - Mx;
        Yi = hit->GetZ() - My;
        Ri = Xi * Xi + Yi * Yi;

        Mxy += Wi * Xi * Yi;
        Mxx += Wi * Xi * Xi;
        Myy += Wi * Yi * Yi;
        Mxr += Wi * Xi * Ri;
        Myr += Wi * Yi * Ri;
        Mrr += Wi * Ri * Ri;
    }
    Mxx /= Sw;
    Myy /= Sw;
    Mxy /= Sw;
    Mxr /= Sw;
    Myr /= Sw;
    Mrr /= Sw;

    // computing the coefficients of the characteristic polynomial

    Mr = Mxx + Myy;
    Cov_xy = Mxx * Myy - Mxy * Mxy;
    Mxr2 = Mxr * Mxr;
    Myr2 = Myr * Myr;

    A2 = 4.0 * Cov_xy - 3.0 * Mr * Mr - Mrr;
    A1 = Mrr * Mr + 4.0 * Cov_xy * Mr - Mxr2 - Myr2 - Mr * Mr * Mr;
    A0 = Mxr2 * Myy + Myr2 * Mxx - Mrr * Cov_xy - 2.0 * Mxr * Myr * Mxy + Mr * Mr * Cov_xy;
    A22 = A2 + A2;

    Float_t x = NewtonSolver(A0, A1, A2, A22);

    // computing the circle parameters (coordinates of center and radius)
    GAM = -Mr - x - x;
    DET = x * x - x * Mr + Cov_xy;
    Xc = (Mxr * (Myy - x) - Myr * Mxy) / DET / 2.;
    Yc = (Myr * (Mxx - x) - Mxr * Mxy) / DET / 2.;
    R = sqrt(Xc * Xc + Yc * Yc - GAM);

    Xc += Mx;
    Yc += My;

    //=============== end of first approx. with equal weights ================//

    for (Int_t i = 0; i < nHits; ++i) {
        BmnGemStripHit* hit = GetHit(track->GetHitIndex(i));
        if (!(hit->IsUsed())) continue;
        Xi = hit->GetX();
        Yi = hit->GetZ();
        Di = Dist(Xi, Yi, Xc, Yc) - R;
        Swd += Wi * Sqr(Di);
    }

    Sig = Sqrt(Swd / Sw);
    thresh = C * Sig;
    UInt_t cntr = 0; // tmp counter 

    //================= robust procedure with Tukey weights =================//
    while ((Abs(Xc - XcPrev) / XcPrev > 0.001 || Abs(Yc - YcPrev) / YcPrev > 0.001 || Abs(R - Rprev) / Rprev > 0.001) && (cntr < 10)) {
        ++cntr;
        Mx = My = Mxx = Myy = Mxy = Mxr = Myr = Mrr = 0.0;
        Sw = Swd = 0.0;

        for (Int_t i = 0; i < nHits; ++i) {
            BmnGemStripHit* hit = GetHit(track->GetHitIndex(i));
            if (!(hit->IsUsed())) continue;
            Di = Dist(hit->GetX(), hit->GetZ(), Xc, Yc) - R;
            if (Abs(Di) <= thresh) {
                Wi = Sqr(1 - Sqr(Di / thresh));
            } else {
                Wi = 0.0;
                hit->SetUsing(kFALSE);
                continue;
            }
            Mx += Wi * hit->GetX();
            My += Wi * hit->GetZ();
            Sw += Wi;
        }

        Mx /= Sw;
        My /= Sw;

        // computing moments (note: all moments are normed, i.e. divided by N)

        for (Int_t i = 0; i < nHits; ++i) {
            BmnGemStripHit* hit = GetHit(track->GetHitIndex(i));
            if (!(hit->IsUsed())) continue;
            Xi = hit->GetX() - Mx;
            Yi = hit->GetZ() - My;
            Ri = Xi * Xi + Yi * Yi;

            Mxy += Wi * Xi * Yi;
            Mxx += Wi * Xi * Xi;
            Myy += Wi * Yi * Yi;
            Mxr += Wi * Xi * Ri;
            Myr += Wi * Yi * Ri;
            Mrr += Wi * Ri * Ri;
        }
        Mxx /= Sw;
        Myy /= Sw;
        Mxy /= Sw;
        Mxr /= Sw;
        Myr /= Sw;
        Mrr /= Sw;

        // computing the coefficients of the characteristic polynomial

        Mr = Mxx + Myy;
        Cov_xy = Mxx * Myy - Mxy * Mxy;
        Mxr2 = Mxr * Mxr;
        Myr2 = Myr * Myr;

        A2 = 4.0 * Cov_xy - 3.0 * Mr * Mr - Mrr;
        A1 = Mrr * Mr + 4.0 * Cov_xy * Mr - Mxr2 - Myr2 - Mr * Mr * Mr;
        A0 = Mxr2 * Myy + Myr2 * Mxx - Mrr * Cov_xy - 2.0 * Mxr * Myr * Mxy + Mr * Mr * Cov_xy;
        A22 = A2 + A2;

        Float_t x = NewtonSolver(A0, A1, A2, A22);
        // computing the circle parameters (coordinates of center and radius)
        GAM = -Mr - x - x;
        DET = x * x - x * Mr + Cov_xy;
        Xc = (Mxr * (Myy - x) - Myr * Mxy) / DET / 2.0;
        Yc = (Myr * (Mxx - x) - Mxr * Mxy) / DET / 2.0;
        R = Sqrt(Xc * Xc + Yc * Yc - GAM);

        Xc += Mx;
        Yc += My;

        for (Int_t i = 0; i < nHits; ++i) {
            BmnGemStripHit* hit = GetHit(track->GetHitIndex(i));
            if (!(hit->IsUsed())) continue;
            Di = Dist(hit->GetX(), hit->GetZ(), Xc, Yc) - R;
            Swd += Wi * Sqr(Di);
        }

        Sig = Swd / Sw;
        thresh = C * Sig;
        XcPrev = Xc;
        YcPrev = Yc;
        Rprev = R;
    }

    Float_t chi2 = 0.0;
    Int_t nReal = 0;
    Float_t dX, dY, dR, sum = 0.0;
    for (Int_t i = 0; i < nHits; ++i) {
        BmnGemStripHit* hit = GetHit(track->GetHitIndex(i));
        if (!(hit->IsUsed())) continue;
        chi2 += Sqr(R - Dist(hit->GetX(), hit->GetZ(), Xc, Yc)) / R;
        dX = hit->GetX() - Xc;
        dY = hit->GetZ() - Yc;
        dR = Sqrt(dX * dX + dY * dY) - R;
        sum += dR * dR;
        nReal++;
    }
    Float_t sigma = Sqrt(sum / nReal);

    chi2 *= nReal;
    track->SetChi2(chi2);
    track->SetNDF(nReal - 1);
    //    if (chi2 > ChisquareQuantile(0.95, nReal - 1)) {
    //        for (Int_t i = 0; i < nHits; ++i) {
    //            BmnGemStripHit* hit = GetHit(track->GetHitIndex(i));
    //            hit->SetUsing(kFALSE);
    //        }
    //        return TVector3(0.0, 0.0, 0.0);
    //    }

    //    cout << "sigm = " << sigma << " chi2 = " << chi2 << " nReal = " << nReal << " R = " << R << " arb.error = " << sigma / R * 100.0 << endl;
    if (sigma / R * 100.0 > 1.0) { //test check. RMS <= 2% of Radius
        for (Int_t i = 0; i < nHits; ++i) {
            BmnGemStripHit* hit = GetHit(track->GetHitIndex(i));
            hit->SetUsing(kFALSE);
        }
        return TVector3(0.0, 0.0, 0.0);
    } else {
        return TVector3(Xc, Yc, R);
    }
}

Float_t BmnGemSeedFinder::Dist(Float_t x1, Float_t y1, Float_t x2, Float_t y2) {
    if (Sqr(x1 - x2) + Sqr(y1 - y2) <= 0.0) {
        return 0.0;
    } else {
        return Sqrt(Sqr(x1 - x2) + Sqr(y1 - y2));
    }
}

Float_t BmnGemSeedFinder::Sqr(Float_t x) {
    return x * x;
}

BmnGemStripHit * BmnGemSeedFinder::GetHit(Int_t i) {
    BmnGemStripHit* hit = (BmnGemStripHit*) fGemHitsArray->At(i);
    if (!hit) cout << "-W- Wrong attempting to get hit number " << i << " from fGemHitsArray, which contains " << fGemHitsArray->GetEntriesFast() << " elements" << endl;
    return hit;
}

Float_t BmnGemSeedFinder::NewtonSolver(Float_t A0, Float_t A1, Float_t A2, Float_t A22) {

    Double_t Dy = 0.0;
    Double_t xnew = 0.0;
    Double_t ynew = 0.0;
    Double_t yold = 1e+11;
    Double_t xold = 0.0;
    const Double_t eps = 1e-12;
    Int_t iter = 0;
    const Int_t iterMax = 20;
    do {
        ynew = A0 + xnew * (A1 + xnew * (A2 + 4.0 * xnew * xnew));
        if (fabs(ynew) > fabs(yold)) {
            xnew = 0.0;
            break;
        }
        Dy = A1 + xnew * (A22 + 16.0 * xnew * xnew);
        xold = xnew;
        xnew = xold - ynew / Dy;
        iter++;
    } while (Abs((xnew - xold) / xnew) > eps && iter < iterMax);

    if (iter == iterMax - 1) {
        xnew = 0.0;
    }

    return xnew;
}

void BmnGemSeedFinder::FillAddr() {
    //Needed for searching seeds by addresses 
    for (Int_t hitIdx = 0; hitIdx < fGemHitsArray->GetEntriesFast(); ++hitIdx) {
        BmnGemStripHit* hit = GetHit(hitIdx);
        if (hit->IsUsed()) continue; //Don't use used hits
        if (hit->GetStation() > kMAXSTATIONFORSEED + kNHITSFORSEED) continue;
        const Float_t R = Sqrt(Sqr(hit->GetX()) + Sqr(hit->GetY()) + Sqr(hit->GetZ()));
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
        addresses.insert(pair<ULong_t, Int_t > (addr, hitIdx));
    }
}

void BmnGemSeedFinder::FillAddrWithLorentz(Float_t sigma_x, Float_t yStep, Float_t trs) {

    TVector3 roughVertex(-3.0, 0.0, 0.0);

    //Needed for searching seeds by addresses 
    Float_t sigma_x2 = sigma_x * sigma_x;

    for (Int_t hitIdx = 0; hitIdx < fGemHitsArray->GetEntriesFast(); ++hitIdx) {
        BmnGemStripHit* hit = GetHit(hitIdx);

        if (hit->IsUsed()) continue; //Don't use used hits
        if (hit->GetStation() > kMAXSTATIONFORSEED + kNHITSFORSEED) continue;

        hit->SetFlag(kFALSE); // by default hits are not filtered 

        //        const Float_t oneOverR = 1.0 / Sqrt(Sqr(hit->GetX()) + Sqr(hit->GetY()) + Sqr(hit->GetZ()));
        const Float_t oneOverR = 1.0 / Sqrt(Sqr(hit->GetX() - roughVertex.X()) + Sqr(hit->GetY() - roughVertex.Y()) + Sqr(hit->GetZ() - roughVertex.Z()));
        const Float_t newX = hit->GetX() * oneOverR;
        const Float_t newY = hit->GetY() * oneOverR;

        Int_t xAddr = ceil((newX - fMin) / fWidth);
        Int_t yAddr = ceil((newY - fMin) / fWidth);
        ULong_t addr = yAddr * fNBins + xAddr;

        hit->SetAddr(addr);
        hit->SetXaddr(xAddr);
        hit->SetYaddr(yAddr);
        hit->SetIndex(hitIdx);

    }

    for (Int_t hitIdx = 0; hitIdx < fGemHitsArray->GetEntriesFast(); ++hitIdx) {
        BmnGemStripHit* hit = GetHit(hitIdx);
        if (hit->IsUsed()) continue; //Don't use used hits
        if (hit->GetStation() > kMAXSTATIONFORSEED + kNHITSFORSEED) continue;

        Int_t yAddr = hit->GetYaddr();
        Int_t xAddr = hit->GetXaddr();
        if (yAddr < 0 || yAddr >= fNBins || xAddr < 0 || xAddr >= fNBins) continue;

        const Float_t oneOverR = 1.0 / Sqrt(Sqr(hit->GetX() - roughVertex.X()) + Sqr(hit->GetY() - roughVertex.Y()) + Sqr(hit->GetZ() - roughVertex.Z()));
        const Float_t newX = hit->GetX() * oneOverR;

        Float_t potSum = 0.0; //sum of all potentials
        for (Int_t hitIdx0 = 0; hitIdx0 < fGemHitsArray->GetEntriesFast(); ++hitIdx0) {
            BmnGemStripHit* hit0 = GetHit(hitIdx0);
            if (hit0->IsUsed()) continue; //Don't use used hits
            if (hit0->GetStation() > kMAXSTATIONFORSEED + kNHITSFORSEED) continue;
            Int_t yAddr0 = hit0->GetYaddr();
            Int_t xAddr0 = hit0->GetXaddr();
            if (yAddr0 < 0 || yAddr0 >= fNBins || xAddr0 < 0 || xAddr0 >= fNBins) continue;

            if (Abs(yAddr0 - yAddr) > yStep) continue; //hits should be in a the same Y-coridor

            const Float_t newX0 = hit0->GetX() / Sqrt(Sqr(hit0->GetX() - roughVertex.X()) + Sqr(hit0->GetY() - roughVertex.Y()) + Sqr(hit0->GetZ() - roughVertex.Z()));
            Float_t pot = sigma_x2 / (sigma_x2 + Sqr(newX0 - newX));
            potSum += pot;
        }
        if (potSum > trs) {
            if (hit->GetStation() == 0) continue;
            addresses.insert(pair<ULong_t, Int_t > (hit->GetAddr(), hitIdx));
        }
    }

    //cout << "addresses.size = " << addresses.size() << endl;
}

void BmnGemSeedFinder::SetHitsUnused(BmnGemTrack * tr) {
    for (Int_t i = 0; i < tr->GetNHits(); ++i)
        GetHit(tr->GetHitIndex(i))->SetUsing(kFALSE);
}