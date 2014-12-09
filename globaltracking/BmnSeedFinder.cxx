
#include "BmnSeedFinder.h"

//some variables for efficiency calculation
static Int_t allFoundCntr = 0;
static Int_t wellFoundCntr = 0;
static Int_t wrongFoundCntr = 0;
static Int_t allHitCntr = 0;
static Int_t goodTrackCntr = 0;
static Int_t allTrackCntr = 0;
static Float_t workTime = 0.0;
//-----------------------------------------

const Float_t thresh = 0.7; // threshold for efficiency calculation (70%)

map<Long_t, Int_t> addresses; // map for calculating addresses of hits in histogram {x/R, y/R}
const UInt_t kNHITSFORSEED = 7; // we search seeds only in first kNHITSFORSEED stations

using std::cout;
using namespace TMath;

BmnSeedFinder::BmnSeedFinder() : fEventNo(0) {

    fSeedHits.clear();
    fMwpc1HitsArray = NULL;
    fMwpc2HitsArray = NULL;
    fMwpc3HitsArray = NULL;
    fGemHitsArray = NULL;
    fSeedsArray = NULL;
    fMakeQA = kTRUE;
    fSeedsBranchName = "BmnGemTrack";
    isRUN1 = kFALSE;
}

BmnSeedFinder::~BmnSeedFinder() {
}

InitStatus BmnSeedFinder::Init() {

    cout << "======================== Seed finder init started =========================" << endl;

    //Get ROOT Manager
    FairRootManager* ioman = FairRootManager::Instance();
    if (NULL == ioman) {
        Fatal("Init", "FairRootManager is not instantiated");
    }

    fDet.DetermineSetup();
    cout << fDet.ToString();

    if (fDet.GetDet(kGEM)) {
        fGemHitsArray = (TClonesArray*) ioman->GetObject("BmnGemStripHit"); //in
        fGemPointsArray = (TClonesArray*) ioman->GetObject("StsPoint");
    }
    if (fDet.GetDet(kMWPC1)) {
        fMwpc1HitsArray = (TClonesArray*) ioman->GetObject("BmnMwpc1Hit"); //in
        fMwpc1PointsArray = (TClonesArray*) ioman->GetObject("MWPC1Point");
    }
    if (fDet.GetDet(kMWPC2)) {
        fMwpc2HitsArray = (TClonesArray*) ioman->GetObject("BmnMwpc2Hit"); //in
        fMwpc2PointsArray = (TClonesArray*) ioman->GetObject("MWPC2Point");
    }
    if (fDet.GetDet(kMWPC3)) {
        fMwpc3HitsArray = (TClonesArray*) ioman->GetObject("BmnMwpc3Hit"); //in
        fMwpc3PointsArray = (TClonesArray*) ioman->GetObject("MWPC3Point");

    }

    fSeedsArray = new TClonesArray(fSeedsBranchName, 100); //out
    ioman->Register("BmnSeeds", "SEED", fSeedsArray, kTRUE);

    fMCTracksArray = (TClonesArray*) ioman->GetObject("MCTrack");

    fNBins = 3000;
    fMin = -1.0;
    fMax = -fMin;
    fWidth = (fMax - fMin) / fNBins;

    cout << "======================== Seed finder init finished ========================" << endl;
}

void BmnSeedFinder::Exec(Option_t* opt) {

    cout << "\n======================== GEM seed finder exec started =====================\n" << endl;
    cout << "Event number: " << fEventNo++ << endl;

    clock_t tStart = clock();
    fSeedsArray->Clear();
    fSeedHits.clear();
    addresses.clear();

    if (fDet.GetDet(kMWPC1)) {
        for (Int_t hitIdx = 0; hitIdx < fMwpc1HitsArray->GetEntriesFast(); ++hitIdx) {
            BmnHit* hit = (BmnHit*) fMwpc1HitsArray->At(hitIdx);
            fSeedHits.push_back(hit);
        }
    }
    if (fDet.GetDet(kMWPC2)) {
        for (Int_t hitIdx = 0; hitIdx < fMwpc2HitsArray->GetEntriesFast(); ++hitIdx) {
            BmnHit* hit = (BmnHit*) fMwpc2HitsArray->At(hitIdx);
            fSeedHits.push_back(hit);
        }
    }
    if (fDet.GetDet(kMWPC3)) {
        for (Int_t hitIdx = 0; hitIdx < fMwpc3HitsArray->GetEntriesFast(); ++hitIdx) {
            BmnHit* hit = (BmnHit*) fMwpc3HitsArray->At(hitIdx);
            fSeedHits.push_back(hit);
        }
    }
    if (fDet.GetDet(kGEM)) {
        for (Int_t hitIdx = 0; hitIdx < fGemHitsArray->GetEntriesFast(); ++hitIdx) {
            BmnHit* hit = (BmnHit*) fGemHitsArray->At(hitIdx);
            fSeedHits.push_back(hit);
        }
    }
    if (isRUN1) {
        cout << "GEOMETRY RUN1\n";
        DoSeedingRun1(1000);
        DoSeedingRun1(500);
    } else {
        cout << "STANDARD GEOMETRY\n";
        DoSeeding();
    }

    cout << "\nGEM_SEEDING: Number of found seeds: " << fSeedsArray->GetEntriesFast() << endl;

    clock_t tFinish = clock();
    workTime += ((Float_t) (tFinish - tStart)) / CLOCKS_PER_SEC;

    cout << "\n======================== Seed finder exec finished ========================" << endl;

}

BmnStatus BmnSeedFinder::DoSeeding() {

    //Needed for searching seeds by addresses 
    for (Int_t hitIdx = 0; hitIdx < fSeedHits.size(); ++hitIdx) {
        BmnHit* hit = (BmnHit*) fSeedHits.at(hitIdx);
        if (hit->GetStation() > kNHITSFORSEED - 1) continue;
        const Float_t R = Sqrt(Sqr(hit->GetX()) + Sqr(hit->GetY()) + Sqr(hit->GetZ()));
        const Float_t newX = hit->GetX() / R;
        const Float_t newY = hit->GetY() / R;
        Int_t xAddr = ceil((newX - fMin) / fWidth);
        Int_t yAddr = ceil((newY - fMin) / fWidth);
        Long_t addr = yAddr * fNBins + xAddr;
        hit->SetAddr(addr);
        hit->SetXaddr(xAddr);
        hit->SetYaddr(yAddr);
        addresses.insert(pair<Long_t, Int_t > (addr, hitIdx));
    }

    for (Int_t i = 0; i < 5; ++i) {
        for (Int_t j = 0; j < 4; ++j)
            FindSeeds(i, j, kTRUE); // from station #i, in gate = 2 * j + 1, only hits presented in every station 
        for (Int_t j = 0; j < 4; ++j)
            FindSeeds(i, j, kFALSE); // from station #i, in gate = 2 * j + 1, 
    }
    return kBMNSUCCESS;
}

BmnStatus BmnSeedFinder::DoSeedingRun1(Int_t nBins) {

//    Int_t nBins = 1000;
    Float_t minY = -0.4;
    Float_t maxY = -minY;
    Float_t width = (maxY - minY) / nBins;

    TH1F* h = new TH1F("tmp", "tmp", nBins, minY, maxY);
    for (Int_t iHit = 0; iHit < fSeedHits.size(); ++iHit) {
        BmnHit* hit = (BmnHit*) fSeedHits.at(iHit);
        if (hit->IsUsed()) continue;
        const Float_t R = Sqrt(Sqr(hit->GetX()) + Sqr(hit->GetY()) + Sqr(hit->GetZ()));
        const Float_t newX = hit->GetX() / R;
        const Float_t newY = hit->GetY() / R;
        Int_t xAddr = ceil((newX - minY) / width);
        Int_t yAddr = ceil((newY - minY) / width);
        Long_t addr = yAddr * nBins + xAddr;
        hit->SetAddr(addr);
        hit->SetXaddr(xAddr);
        hit->SetYaddr(yAddr);
        //addresses.insert(pair<Long_t, Int_t > (addr, iHit));
        h->Fill(newY);
    }

    for (Int_t iBin = 0; iBin < h->GetNbinsX(); ++iBin) {
        if (h->GetBinContent(iBin) < 3) continue;
        vector<BmnHit*> hitsInTrack;
        BmnGemTrack trackCand;
        Int_t idx = 0;
        for (Int_t iHit = 0; iHit < fSeedHits.size(); ++iHit) {
            BmnHit* hit = (BmnHit*) fSeedHits.at(iHit);
            if (hit->IsUsed()) continue;
            if (hit->GetYaddr() == iBin) {
                hit->SetUsing(kTRUE);
                trackCand.AddHit(idx++, hit);
                hitsInTrack.push_back(hit);
            }
        }
        trackCand.SortHits();
        TVector3 circPar = CircleFit(hitsInTrack);
        TVector3 linePar = LineFit(hitsInTrack);
        if (circPar.Z() == 0.0) {
            for (Int_t i = 0; i < hitsInTrack.size(); ++i) {
                ((BmnHit*) hitsInTrack.at(i))->SetUsing(kFALSE);
            }
            continue;
        }

        if (CalculateTrackParams(&trackCand, hitsInTrack, circPar, linePar)) {
            vector<Int_t> trIds;
            BmnHit* hit = (BmnHit*) hitsInTrack.at(0);
            trackCand.SetRef(hit->GetRefIndex());
            new((*fSeedsArray)[fSeedsArray->GetEntriesFast()]) BmnGemTrack(trackCand);
        } else {
            for (Int_t i = 0; i < hitsInTrack.size(); ++i) {
                ((BmnHit*) hitsInTrack.at(i))->SetUsing(kFALSE);
            }
        }
    }
    cout << "Nhits in histo = " << h->GetEntries() << endl;
    delete h;
    return kBMNSUCCESS;
}

void BmnSeedFinder::Finish() {

    cout.precision(2);
    cout.setf(ios::fixed, ios::floatfield);

    //    cout << "\n\t-----------------------------------------------------------------------------------------" << endl;
    //    cout << "\t|                                 Efficiency of seeding                                 |" << endl;
    //    cout << "\t-----------------------------------------------------------------------------------------" << endl;
    //    cout << "\t|  Percent of connected hits:\t\t\t|\t" << allFoundCntr << " / " << allHitCntr << "\t|  " << allFoundCntr * 100.0 / allHitCntr << "%\t|" << endl;
    //    cout << "\t|  Percent of well connected hits:\t\t|\t" << wellFoundCntr << " / " << allFoundCntr << "\t|  " << wellFoundCntr * 100.0 / allFoundCntr << "%\t|" << endl;
    //    cout << "\t|  Percent of wrong connected hits:\t\t|\t" << wrongFoundCntr << " / " << allFoundCntr << "\t|  " << wrongFoundCntr * 100.0 / allFoundCntr << "%\t|" << endl;
    //    cout << "\t|  Percent of well found tracks (thr = " << thresh << "):\t|\t" << goodTrackCntr << " / " << allTrackCntr << "\t|  " << goodTrackCntr * 100.0 / allTrackCntr << "%\t|" << endl;
    //    cout << "\t|  Work time: full / per one event:\t\t|\t" << workTime << " sec.\t|  " << workTime / fEventNo << " sec.\t|" << endl;
    //    cout << "\t-----------------------------------------------------------------------------------------" << endl;
    //
    //    if (fMakeQA) {
    //        toDirectory("QA/GEM/SEEDS");
    //        gFile->cd();
    //    }
}

// ========================================================== //
//                                                            //
//                Search track-candidates in                  //
//            2D histogram for {x/R, y/R} - space.            //
//                 R = Sqrt(x^2 + y^2 + z^2)                  //
//                                                            //
// ========================================================== //

void BmnSeedFinder::FindSeeds(Int_t startStation, Int_t gate, Bool_t isIdeal) {

    //search only kNHITSFORSEED first hits of track
    //search in left and right directions

    Int_t trCntr = 0;
    trCntr += SearchTrackCandidates(startStation, gate, isIdeal, kTRUE);
    trCntr += SearchTrackCandidates(startStation, gate, isIdeal, kFALSE);

    cout << "GEM_SEEDING: Number of candidates ( Stat = " << startStation << ", gate = " << gate * 2 + 1 << ", isIdeal = " << isIdeal << "): " << trCntr << endl;
}

UInt_t BmnSeedFinder::SearchTrackCandidates(Int_t startStation, Int_t gate, Bool_t isIdeal, Bool_t isLeft) {
    UInt_t trCntr = 0;
    for (Int_t iHit = 0; iHit < fSeedHits.size(); ++iHit) {

        BmnHit* hit = (BmnHit*) fSeedHits.at(iHit);

        if (hit->IsUsed()) continue;
        if (startStation != hit->GetStation()) continue;

        BmnGemTrack trackCand;

        Int_t xAddr = hit->GetXaddr();
        Int_t yAddr = hit->GetYaddr();

        Int_t dist = 0;
        Int_t maxDist = 0;
        Int_t hitCntr = 0;
        Int_t startBin = 0;
        Int_t prevStation = startStation - 1; //number of starting station
        if (isLeft) { //search track-candidate in left direction
            for (Int_t i = xAddr; i > 0; i--) {
                SearchTrackCandInLine(i, yAddr, &trackCand, &hitCntr, &maxDist, &dist, &startBin, &prevStation, gate, isIdeal);
                //                if (hitCntr == kNHITSFORSEED) break;
            }
        } else { //search track-candidate in right direction
            for (Int_t i = xAddr; i < fNBins; ++i) {
                SearchTrackCandInLine(i, yAddr, &trackCand, &hitCntr, &maxDist, &dist, &startBin, &prevStation, gate, isIdeal);
                //                if (hitCntr == kNHITSFORSEED) break;
            }
        }

        trackCand.SortHits();
        Int_t nHitsForCand = ((gate * 2 + 1) > 4) ? 4 : 5;
        if (trackCand.GetNHits() < nHitsForCand) { // don't fit track by circle with less then 4 hits
            for (Int_t i = 0; i < trackCand.GetNHits(); ++i)
                BmnHit * hit = (BmnHit*) fSeedHits.at(trackCand.GetHitIndex(i));
            hit->SetUsing(kFALSE);
            continue;
        }
        vector<BmnHit*> hitsInTrack;
        for (Int_t i = 0; i < trackCand.GetNHits(); ++i) {
            BmnHit* hit = (BmnHit*) fSeedHits.at(trackCand.GetHitIndex(i));
            hitsInTrack.push_back(hit);
        }
        TVector3 circPar = CircleFit(hitsInTrack);
        TVector3 linePar = LineFit(hitsInTrack);
        if (circPar.Z() == 0.0) {
            continue;
        } //FIXME maybe better to check not only zero-radius
        trCntr++;
        trackCand.SortHits();
        if (CalculateTrackParams(&trackCand, hitsInTrack, circPar, linePar)) {
            new((*fSeedsArray)[fSeedsArray->GetEntriesFast()]) BmnGemTrack(trackCand);
        }
    }
    return trCntr;
}

void BmnSeedFinder::SearchTrackCandInLine(const Int_t i, const Int_t y, BmnGemTrack* tr, Int_t* hitCntr, Int_t* maxDist, Int_t* dist, Int_t* startBin, Int_t* prevStation, Int_t gate, Bool_t isIdeal) {

    if (((*hitCntr) > 1) && Abs(i - (*startBin)) > 4 * (*maxDist)) return; //condition for finishing is dist < 4 * MaxDist //FIXME check this condition in mother functions

    BmnHit* hit = NULL;

    for (Int_t j = y - gate; j <= y + gate; ++j) {
        ULong_t addr = j * fNBins + i;
        if (addresses.find(addr) == addresses.end()) continue;
        hit = (BmnHit*) fSeedHits.at(addresses.find(addr)->second);

        if (hit->IsUsed()) continue;
        if (isIdeal) {
            if (hit->GetStation() != (*prevStation) + 1) {
                continue;
            }
        } else {
            if (hit->GetStation() <= (*prevStation)) {
                continue;
            }
        }
        (*hitCntr)++;
        (*prevStation) = hit->GetStation();
        if ((*hitCntr) != 1) {
            (*dist) = Abs(hit->GetXaddr() - (*startBin));
            if ((*dist) > (*maxDist)) (*maxDist) = (*dist);
        }
        (*startBin) = hit->GetXaddr();
        hit->SetUsing(kTRUE);
        tr->AddHit(addresses.find(addr)->second, hit);
    }
}

Bool_t BmnSeedFinder::CalculateTrackParams(BmnGemTrack* tr, vector<BmnHit*> hits, TVector3 circPar, TVector3 linePar) {
    //Needed for start approximation of track parameters

    Float_t R = circPar.Z(); // radius of fit-circle
    Float_t Xc = circPar.X(); // x-coordinate of fit-circle center
    Float_t Zc = circPar.Y(); // z-coordinate of fit-circle center
    fField = FairRunAna::Instance()->GetField();
    const Int_t nHits = hits.size();
    if (nHits == 0) return kFALSE;
    BmnHit* firstHit = (BmnHit*) hits.at(0);
    BmnHit* lastHit = (BmnHit*) hits.at(nHits - 1);
    for (Int_t i = 1; i < nHits; ++i) {
        BmnHit* curHit = (BmnHit*) hits.at(i);
        if (curHit->GetZ() < firstHit->GetZ()) firstHit = curHit;
        if (curHit->GetZ() > lastHit->GetZ()) lastHit = curHit;
    }
    if (!firstHit || !lastHit) return kFALSE;

    const Float_t B = linePar.X(); //angle coefficient for helicoid

    Float_t Xmean(0.0), Ymean(0.0), Zmean(0.0); // <Xi> , <Yi> , <Zi>
    Float_t ZXmean(0.0); // <(Zi-Zc)/(Xi-Xc)>
    Float_t ZX2mean(0.0); // <(Zi-Zc)/(Xi-Xc)^2>
    Float_t XZXmean(0.0); // <Xi*(Zi-Zc)/(Xi-Xc)>
    Float_t YZXmean(0.0); // <Yi*(Zi-Zc)/(Xi-Xc)>
    Float_t OneXmean(0.0); // <1/(Xi-Xc)>
    Float_t XOneXmean(0.0); // <Xi/(Xi-Xc)>
    Float_t YOneXmean(0.0); // <Yi/(Xi-Xc)>

    //Covariance matrix
    Float_t Cov_X_X(0.0), Cov_X_Y(0.0), Cov_X_Tx(0.0), Cov_X_Ty(0.0), Cov_X_Qp(0.0);
    Float_t Cov_Y_Y(0.0), Cov_Y_Tx(0.0), Cov_Y_Ty(0.0), Cov_Y_Qp(0.0);
    Float_t Cov_Tx_Tx(0.0), Cov_Tx_Ty(0.0), Cov_Tx_Qp(0.0);
    Float_t Cov_Ty_Ty(0.0), Cov_Ty_Qp(0.0);
    Float_t Cov_Qp_Qp(0.0);

    for (Int_t i = 0; i < nHits; ++i) {
        BmnHit* hit = (BmnHit*) hits.at(i);
        Float_t Xi = hit->GetX();
        Float_t Yi = hit->GetY();
        Float_t Zi = hit->GetZ();
        Xmean += Xi;
        Ymean += Yi;
        Zmean += Zi;
        ZXmean += (Zi - Zc) / (Xi - Xc);
        ZX2mean += (Zi - Zc) / (Xi - Xc) / (Xi - Xc);
        XZXmean += (Zi - Zc) / (Xi - Xc) * Xi;
        YZXmean += (Zi - Zc) / (Xi - Xc) * Yi;
        OneXmean += 1.0 / (Xi - Xc);
        XOneXmean += Xi / (Xi - Xc);
        YOneXmean += Yi / (Xi - Xc);

        Cov_X_X += Sqr(Xi);
        Cov_X_Y += Xi * Yi;
        Cov_Y_Y += Sqr(Yi);
        Cov_Tx_Tx += Sqr((Zi - Zc) / (Xi - Xc));
        Cov_Ty_Ty += Sqr(1.0 / (Xi - Xc));
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

    Cov_X_X = Cov_X_X / nHits - Sqr(Xmean);
    Cov_X_Y = Cov_X_Y / nHits - Xmean * Ymean;
    Cov_X_Tx = Cov_X_Tx = Xmean * ZXmean - XZXmean;
    Cov_X_Ty = 0.0; //Cov_X_Ty = B * (Xmean * OneXmean - XOneXmean);
    Cov_Y_Y = Cov_Y_Y / nHits - Sqr(Ymean);
    Cov_Y_Tx = Cov_Y_Tx = Ymean * ZXmean - YZXmean;
    Cov_Y_Ty = 0.0; //Cov_Y_Ty = B * (Ymean * OneXmean - YOneXmean);
    Cov_Tx_Ty = 0.0; //Cov_Tx_Ty = B * (ZX2mean - ZXmean * OneXmean);
    Cov_Tx_Tx = Cov_Tx_Tx / nHits - Sqr(ZXmean);
    Cov_Ty_Ty = 0.0; //Cov_Ty_Ty = Sqr(B) * (Cov_Ty_Ty / nHits - OneXmean * OneXmean);

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

    Float_t Tx_first = (Zc - fZ) / (fX - Xc);
    Float_t Tx_last = (Zc - lZ) / (lX - Xc);

    par.SetPosition(TVector3(lX, lY, lZ));
    par.Print();
    par.SetTx(Tx_last);
    par.SetTy(B); //par.SetTy(-B / (lX - Xc));
    const Float_t Pxz = 0.0003 * Abs(fField->GetBy(lX, lY, lZ)) * R; // Pt
    const Float_t Pz = Pxz / Sqrt(1 + Sqr(par.GetTx()));
    const Float_t Px = Pz * par.GetTx();
    const Float_t Py = Pz * par.GetTy();
    Float_t QP = 1.0 / Sqrt(Px * Px + Py * Py + Pz * Pz);
    if ((Tx_last - Tx_first) < 0) QP *= -1.0; //FIXME ???????
    par.SetQp(QP);
    tr->SetParamLast(par);

    //update for firstParam
    par.SetPosition(TVector3(fX, fY, fZ));
    par.SetTx(Tx_first);
    par.SetTy(B); //par.SetTy(-B / (firstHit->GetX() - Xc));
    tr->SetParamFirst(par);

    return kTRUE;

}

// ========================================================== //
//                                                            //
//         Creating 1D histogram from {x/r, y/r} - space.     //
//         r = Sqrt(x^2 + y^2 + z^2)                          //
//                                                            //
// ========================================================== //

void BmnSeedFinder::FindXYRSeeds(TH1F* h) {
    for (Int_t hitIdx = 0; hitIdx < fGemHitsArray->GetEntriesFast(); ++hitIdx) {
        BmnHit* hit = (BmnHit*) fGemHitsArray->At(hitIdx);
        if (hit->IsUsed()) continue;
        const Float_t newY = hit->GetY() / Sqrt(Sqr(hit->GetX()) + Sqr(hit->GetY()) + Sqr(hit->GetZ()));
        hit->SetDetectorID(ceil((newY - h->GetXaxis()->GetXmin()) / h->GetBinWidth(7))); // 7 - not important
        h->Fill(newY);
    }
}

TVector3 BmnSeedFinder::CircleFit(vector<BmnHit*> hits) {

    const Float_t nHits = hits.size();
    const Float_t C = 3; // parameter for robust fitting (3*Sigma rule)

    Float_t Xi = 0.0, Zi = 0.0; // coordinates of current track point
    Float_t Xo = 0.0, Zo = 0.0; // coordinates of vertex (point on circle) Now it's {0.0, 0.0}
    Float_t Xc = 0.0, Zc = 0.0; // coordinates of circle center
    Float_t R = 0.0; //radius of circle
    Float_t XcPrev = 0.0, ZcPrev = 0.0; // coordinates of circle center on previous iteration
    Float_t Rprev = 0.0; //radius of circle on previous iteration
    Float_t Sxx = 0.0, Szz = 0.0, Sxz = 0.0, Srx = 0.0, Srz = 0.0; // partial sums to calculate center of circle
    Float_t Wi = 1.0; // weight for robust approach // In first approach it's equal 1.0
    Float_t Di = 0.0; //distance between circle and point
    Float_t Sig = 1.0;
    Float_t thresh = C * Sig; // threshold for weights calculating
    Float_t Sw = 0.0; // sum of weights
    Float_t Swd = 0.0; // sum of (w * d^2)

    //first approximation with equal weights
    for (Int_t i = 0; i < nHits; ++i) {
        BmnHit* hit = (BmnHit*) hits.at(i);
        Xi = hit->GetX();
        Zi = hit->GetZ();
        Sxx += Wi * Sqr(Xi - Xo);
        Szz += Wi * Sqr(Zi - Zo);
        Sxz += Wi * (Xi - Xo) * (Zi - Zo);
        Srx += 0.5 * Wi * (Sqr(Xi) + Sqr(Zi) - Sqr(Xo) - Sqr(Zo)) * (Xi - Xo);
        Srz += 0.5 * Wi * (Sqr(Xi) + Sqr(Zi) - Sqr(Xo) - Sqr(Zo)) * (Zi - Zo);
    }

    Xc = (Srz * Sxz - Srx * Szz) / (Sqr(Sxz) - Sxx * Szz);
    Zc = (Srx * Sxz - Srz * Sxx) / (Sqr(Sxz) - Sxx * Szz);
    R = Dist(Xo, Zo, Xc, Zc);


    for (Int_t i = 0; i < nHits; ++i) {
        BmnHit* hit = (BmnHit*) hits.at(i);
        Xi = hit->GetX();
        Zi = hit->GetZ();
        Di = Dist(Xi, Zi, Xc, Zc) - R;
        Sw += Wi;
        Swd += Wi * Sqr(Di);
    }

    Sig = Sqrt(Swd / Sw);
    thresh = C * Sig;

    UInt_t cntr = 0; // tmp counter 

    while ((Abs(Xc - XcPrev) / XcPrev > 0.001 || Abs(Zc - ZcPrev) / ZcPrev > 0.001 || Abs(R - Rprev) / Rprev > 0.001) && (cntr < 10)) {
        ++cntr;
        Sxx = Szz = Sxz = Srx = Srz = Sw = Swd = 0.0;
        for (Int_t i = 0; i < nHits; ++i) {
            BmnHit* hit = (BmnHit*) hits.at(i);
            Xi = hit->GetX();
            Zi = hit->GetZ();
            Di = Dist(Xi, Zi, Xc, Zc) - R;
            if (Abs(Di) <= thresh) {
                Wi = Sqr(1 - Sqr(Di / thresh));
            } else {
                Wi = 0.0;
                continue;
            }

            Sxx += Wi * Sqr(Xi - Xo);
            Szz += Wi * Sqr(Zi - Zo);
            Sxz += Wi * (Xi - Xo) * (Zi - Zo);
            Srx += 0.5 * Wi * (Sqr(Xi) + Sqr(Zi) - Sqr(Xo) - Sqr(Zo)) * (Xi - Xo);
            Srz += 0.5 * Wi * (Sqr(Xi) + Sqr(Zi) - Sqr(Xo) - Sqr(Zo)) * (Zi - Zo);
        }

        XcPrev = Xc;
        ZcPrev = Zc;
        Rprev = R;

        Float_t znam = (Sqr(Sxz) - Sxx * Szz);
        if (znam == 0) continue;
        Xc = (Srz * Sxz - Srx * Szz) / znam;
        Zc = (Srx * Sxz - Srz * Sxx) / znam;
        R = Dist(Xo, Zo, Xc, Zc);

        for (Int_t i = 0; i < nHits; ++i) {
            BmnHit* hit = (BmnHit*) hits.at(i);
            Di = Dist(hit->GetX(), hit->GetZ(), Xc, Zc) - R;
            Sw += Wi;
            Swd += Wi * Sqr(Di);
        }

        Sig = Swd / Sw;
        thresh = C * Sig;
    }

    Float_t chi2 = 0.0;
    for (Int_t i = 0; i < nHits; ++i) {
        BmnHit* hit = (BmnHit*) hits.at(i);
        chi2 += Sqr(R - Dist(hit->GetX(), hit->GetZ(), Xc, Zc)) / R;
    }

    chi2 *= nHits;
    if (chi2 > ChisquareQuantile(0.95, nHits - 1)) {
        for (Int_t i = 0; i < nHits; ++i) {
            BmnHit* hit = (BmnHit*) hits.at(i);
            hit->SetUsing(kFALSE);
        }
        return TVector3(0.0, 0.0, 0.0);
    }

    return TVector3(Xc, Zc, R);
}

TVector3 BmnSeedFinder::LineFit(vector<BmnHit*> hits) {

    //Least Square Method//
    Float_t Zi = 0.0, Yi = 0.0; // coordinates of current track point
    Float_t a = 0.0, b = 0.0; // parameters of line: y = a * z + b
    Float_t SumZ = 0.0, SumY = 0.0, SumZY = 0.0, SumZ2 = 0.0;
    const Float_t nHits = hits.size();
    for (Int_t i = 0; i < nHits; ++i) {
        BmnHit* hit = (BmnHit*) hits.at(i);
        Zi = hit->GetZ();
        Yi = hit->GetY();
        SumZ += Zi;
        SumY += Yi;
        SumZY += Zi * Yi;
        SumZ2 += Sqr(Zi);
    }

    a = (nHits * SumZY - SumZ * SumY) / (nHits * SumZ2 - Sqr(SumZ));
    b = (SumY - a * SumZ) / nHits;

    return TVector3(a, b, 0.0);
}

Float_t BmnSeedFinder::Dist(Float_t x1, Float_t y1, Float_t x2, Float_t y2) {
    //FIXME
    if (Sqr(x1 - x2) + Sqr(y1 - y2) < 0.0) {
        return 0.0;
    } else {
        return Sqrt(Sqr(x1 - x2) + Sqr(y1 - y2));
    }
}

Float_t BmnSeedFinder::Sqr(Float_t x) {
    return x * x;
}
