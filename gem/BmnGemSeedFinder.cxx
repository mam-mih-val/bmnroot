
#include "BmnGemSeedFinder.h"

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

map<ULong_t, Int_t> addresses; // map for calculating addresses of hits in histogram {x/R, y/R}
const UInt_t kNHITSFORSEED = 7; // we search seeds only in first kNHITSFORSEED stations

using std::cout;
using namespace TMath;

BmnGemSeedFinder::BmnGemSeedFinder() : fEventNo(0) {

    fGemHitsArray = NULL;
    fGemSeedsArray = NULL;
    fMakeQA = kTRUE;
    fHisto = NULL;
    fHitsBranchName = "BmnGemStripHit";
    fSeedsBranchName = "BmnGemTrack";
}

BmnGemSeedFinder::~BmnGemSeedFinder() {
    if (isHistogramsInitialized) delete fHisto;
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

    if (!isHistogramsInitialized && fMakeQA) {
        fHisto = new BmnGemSeedFinderQA();
        fHisto->Initialize();
        isHistogramsInitialized = kTRUE;
    }

    fNBins = 3000;
    fMin = -1.0;
    fMax = -fMin;
    fWidth = (fMax - fMin) / fNBins;

    cout << "======================== Seed finder init finished ========================" << endl;
}

void BmnGemSeedFinder::Exec(Option_t* opt) {
    
    cout << "\n======================== GEM seed finder exec started =====================\n" << endl;
    cout << "Event number: " << fEventNo++ << endl;

    clock_t tStart = clock();
    fGemSeedsArray->Clear();
    addresses.clear();
  
    //Needed for searching seeds by addresses 
    for (Int_t hitIdx = 0; hitIdx < fGemHitsArray->GetEntriesFast(); ++hitIdx) {
        BmnGemStripHit* hit = GetHit(hitIdx);
        if (hit->GetRefIndex() < 0) continue; //FIXME!!! Now only for test! (Excluding fake hits)
        if (hit->GetStation() > kNHITSFORSEED - 1) continue;
        const Float_t R = Sqrt(Sqr(hit->GetX()) + Sqr(hit->GetY()) + Sqr(hit->GetZ()));
        const Float_t newX = hit->GetX() / R;
        const Float_t newY = hit->GetY() / R;
        Int_t xAddr = ceil((newX - fMin) / fWidth);
        Int_t yAddr = ceil((newY - fMin) / fWidth);
        ULong_t addr = yAddr * fNBins + xAddr;
        hit->SetAddr(addr);
        hit->SetXaddr(xAddr);
        hit->SetYaddr(yAddr);
        addresses.insert(pair<ULong_t, Int_t > (addr, hitIdx));
    }
    DoSeeding();

    cout << "\nGEM_SEEDING: Number of found seeds: " << fGemSeedsArray->GetEntriesFast() << endl;

    clock_t tFinish = clock();
    workTime += ((Float_t) (tFinish - tStart)) / CLOCKS_PER_SEC;

    if (fMakeQA) {

        for (Int_t hitIdx = 0; hitIdx < fGemHitsArray->GetEntriesFast(); ++hitIdx) {
            BmnGemStripHit* hit = GetHit(hitIdx);
            Float_t x = hit->GetX();
            Float_t y = hit->GetY();
            Float_t z = hit->GetZ();
            Float_t R = Sqrt(x * x + y * y + z * z);
            Int_t station = hit->GetStation() + 1;
            if (station > kNHITSFORSEED) continue;
            fHisto->_hyRxR_all->Fill(x / R, y / R, station);
            fHisto->_hXYAllHits->Fill(x, y, station);
            fHisto->_hZYAllHits->Fill(z, y, station);
            fHisto->_hZXAllHits->Fill(z, x, station);
            fHisto->_hStationAllHits->Fill(station);
            if (hit->IsUsed()) {
                fHisto->_hXYGoodHits->Fill(x, y, station);
                fHisto->_hZYGoodHits->Fill(z, y, station);
                fHisto->_hZXGoodHits->Fill(z, x, station);
                fHisto->_hStationUsedHits->Fill(station);
                fHisto->_hyRxR_good->Fill(x / R, y / R, station);
            } else {
                fHisto->_hXYBadHits->Fill(x, y, station);
                fHisto->_hZYBadHits->Fill(z, y, station);
                fHisto->_hZXBadHits->Fill(z, x, station);
                fHisto->_hStationNotUsedHits->Fill(station);
                fHisto->_hyRxR_bad->Fill(x / R, y / R, station);
            }
        }
        for (Int_t iTr = 0; iTr < fGemSeedsArray->GetEntriesFast(); ++iTr) {
            BmnGemTrack* track = (BmnGemTrack*) fGemSeedsArray->At(iTr);
            for (Int_t iHit = 0; iHit < track->GetNHits(); ++iHit) {
                BmnGemStripHit* hit = GetHit(track->GetHitIndex(iHit));
                if (!hit) continue;
                fHisto->_hXYFoundHits->Fill(hit->GetX(), hit->GetY(), iTr + 1);
                fHisto->_hZYFoundHits->Fill(hit->GetZ(), hit->GetY(), iTr + 1);
                fHisto->_hZXFoundHits->Fill(hit->GetZ(), hit->GetX(), iTr + 1);
            }
        }


        Int_t GemTrackId = -1;
        Int_t max = -1;

        for (Int_t i = 0; i < fGemSeedsArray->GetEntriesFast(); ++i) {
            max = -1;
            GemTrackId = -1;
            map<Int_t, Int_t> indexes; //pairs of trackId and number of hits corresponded this trackId
            BmnGemTrack* track = (BmnGemTrack*) fGemSeedsArray->At(i);
            allFoundCntr += track->GetNHits();
            if (fMakeQA) {
                fHisto->_hNumOfHitsDistr->Fill(track->GetNHits());
                TVector3 mom;
                track->GetParamLast()->Momentum(mom);
                fHisto->_hMomentumDistr->Fill(mom.Mag());
                fHisto->_hPx->Fill(mom.X());
                fHisto->_hPy->Fill(mom.Y());
                fHisto->_hPz->Fill(mom.Z());
                fHisto->_hPt->Fill(Sqrt(Sqr(mom.X()) + Sqr(mom.Z())));
            }

            BmnGemStripHit* firstHit = GetHit(track->GetHitIndex(0));
            Int_t refId = firstHit->GetRefIndex();
            if (refId < 0) continue;
            CbmStsPoint* firstPoint = (CbmStsPoint*) fMCPointsArray->At(refId);
            if (!firstPoint) {
                cout << "GEM_SEEDING: There is no MC-point corresponded to current hit" << endl;
                continue;
            }
            indexes.insert(pair<Int_t, Int_t > (firstPoint->GetTrackID(), 1));
            wellFoundCntr++; //increment by the first hit
            for (Int_t j = 1; j < track->GetNHits(); ++j) { //loop over hits from the second to the last. Needed for comparing id of hits
                BmnGemStripHit* curHit = GetHit(track->GetHitIndex(j));
                if (!curHit) continue;
                Int_t refId = curHit->GetRefIndex();
                if (refId < 0) continue;
                CbmStsPoint* curPoint = (CbmStsPoint*) fMCPointsArray->At(refId);
                if (!curPoint) {
                    cout << "GEM_SEEDING: There is no MC-point corresponded to current hit" << endl;
                    continue;
                }
                if (firstPoint->GetTrackID() == curPoint->GetTrackID()) {
                    wellFoundCntr++;
                    fHisto->_hStationWellHits->Fill(curHit->GetStation() + 1);
                } else {
                    wrongFoundCntr++;
                    fHisto->_hStationWrongHits->Fill(curHit->GetStation() + 1);
                }

                if (indexes.find(curPoint->GetTrackID()) == indexes.end()) {
                    indexes.insert(pair<Int_t, Int_t > (curPoint->GetTrackID(), 1));
                } else {
                    (indexes.find(curPoint->GetTrackID())->second)++;
                }

            }

            for (map<Int_t, Int_t>::iterator it = indexes.begin(); it != indexes.end(); it++) {
                if ((*it).second > max) {
                    max = (*it).second;
                    GemTrackId = (*it).first;
                }
                if ((*it).second > track->GetNHits() * thresh) goodTrackCntr++;
            }
            track->SetRef(GemTrackId);
            if (fMakeQA) {
                fHisto->_hNumMcTrack->Fill(indexes.size());
            }
        }

        allTrackCntr += fGemSeedsArray->GetEntriesFast();
        for (Int_t hitIdx = 0; hitIdx < fGemHitsArray->GetEntriesFast(); ++hitIdx) {
            if (GetHit(hitIdx)->GetStation() + 1 > kNHITSFORSEED) continue;
            allHitCntr++;
        }
    }


    cout << "\n======================== Seed finder exec finished ========================" << endl;

}

BmnStatus BmnGemSeedFinder::DoSeeding() {

    for (Int_t i = 0; i < 5; ++i) {
        for (Int_t j = 0; j < 4; ++j)
            FindSeeds(i, j, kTRUE); // from station #i, in gate = 2 * j + 1, only hits presented in every station 
        for (Int_t j = 0; j < 4; ++j)
            FindSeeds(i, j, kFALSE); // from station #i, in gate = 2 * j + 1
    }

//        TH1F* hTheta = new TH1F("tmp", "tmp", 240, -6, 6);
//        for (Int_t iHit = 0; iHit < fGemHitsArray->GetEntriesFast(); ++iHit) {
//            BmnGemStripHit* hit = GetHit(iHit);
//            if (hit->IsUsed()) continue;
//            Float_t z = hit->GetZ();
//            Float_t y = hit->GetY();
//            Float_t theta = ATan2(y, z) * RadToDeg();
//            if (Abs(theta) > 6) continue; //use only hits in narrow acceptance
//            hTheta->Fill(theta);
//        }

    return kBMNSUCCESS;
}

void BmnGemSeedFinder::Finish() {

    addresses.clear();
//    cout.precision(2);
//    cout.setf(ios::fixed, ios::floatfield);

//    cout << "\n\t-----------------------------------------------------------------------------------------" << endl;
//    cout << "\t|                                 Efficiency of seeding                                 |" << endl;
//    cout << "\t-----------------------------------------------------------------------------------------" << endl;
//    cout << "\t|  Percent of connected hits:\t\t\t|\t" << allFoundCntr << " / " << allHitCntr << "\t|  " << allFoundCntr * 100.0 / allHitCntr << "%\t|" << endl;
//    cout << "\t|  Percent of well connected hits:\t\t|\t" << wellFoundCntr << " / " << allFoundCntr << "\t|  " << wellFoundCntr * 100.0 / allFoundCntr << "%\t|" << endl;
//    cout << "\t|  Percent of wrong connected hits:\t\t|\t" << wrongFoundCntr << " / " << allFoundCntr << "\t|  " << wrongFoundCntr * 100.0 / allFoundCntr << "%\t|" << endl;
//    cout << "\t|  Percent of well found tracks (thr = " << thresh << "):\t|\t" << goodTrackCntr << " / " << allTrackCntr << "\t|  " << goodTrackCntr * 100.0 / allTrackCntr << "%\t|" << endl;
//    cout << "\t|  Work time: full / per one event:\t\t|\t" << workTime << " sec.\t|  " << workTime / fEventNo << " sec.\t|" << endl;
//    cout << "\t-----------------------------------------------------------------------------------------" << endl;

    if (fMakeQA) {
        toDirectory("QA/GEM/SEEDS");
        fHisto->Write();
        gFile->cd();
    }
}

// ========================================================== //
//                                                            //
//                Search track-candidates in                  //
//            2D histogram for {x/R, y/R} - space.            //
//                 R = Sqrt(x^2 + y^2 + z^2)                  //
//                                                            //
// ========================================================== //

void BmnGemSeedFinder::FindSeeds(Int_t startStation, Int_t gate, Bool_t isIdeal) {

    //search only kNHITSFORSEED first hits of track
    //search in left and right directions

    Int_t trCntr = 0;
    trCntr += SearchTrackCandidates(startStation, gate, isIdeal, kTRUE);
    trCntr += SearchTrackCandidates(startStation, gate, isIdeal, kFALSE);

    cout << "GEM_SEEDING: Number of candidates ( Stat = " << startStation << ", gate = " << gate * 2 + 1 << ", isIdeal = " << isIdeal << "): " << trCntr << endl;
}

UInt_t BmnGemSeedFinder::SearchTrackCandidates(Int_t startStation, Int_t gate, Bool_t isIdeal, Bool_t isLeft) {
    UInt_t trCntr = 0;
    for (Int_t iHit = 0; iHit < fGemHitsArray->GetEntriesFast(); ++iHit) {

        BmnGemStripHit* hit = GetHit(iHit);

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
                if ((hitCntr > 1) && Abs(i - startBin) > 2 * maxDist) break; //condition to finish search is dist < 2 * MaxDist
                //                if (hitCntr == kNHITSFORSEED) break;
            }
        } else { //search track-candidate in right direction
            for (Int_t i = xAddr; i < fNBins; ++i) {
                SearchTrackCandInLine(i, yAddr, &trackCand, &hitCntr, &maxDist, &dist, &startBin, &prevStation, gate, isIdeal);
                if ((hitCntr > 1) && Abs(i - startBin) > 2 * maxDist) break; //condition to finish search is dist < 2 * MaxDist
                //                if (hitCntr == kNHITSFORSEED) break;
            }
        }

        trackCand.SortHits();
        Int_t nHitsForCand = ((gate * 2 + 1) > 4) ? 4 : 5;
        if (trackCand.GetNHits() < nHitsForCand) { // don't fit track by circle with less then 4 hits
            for (Int_t i = 0; i < trackCand.GetNHits(); ++i)
                GetHit(trackCand.GetHitIndex(i))->SetUsing(kFALSE);
            continue;
        }
        TVector3 circPar = CircleFit(&trackCand);
        TVector3 linePar = LineFit(&trackCand);
        if (circPar.Z() == 0.0) continue;
        trCntr++;
        trackCand.SortHits();
        if (CalculateTrackParams(&trackCand, circPar, linePar)) {
            new((*fGemSeedsArray)[fGemSeedsArray->GetEntriesFast()]) BmnGemTrack(trackCand);
        }
    }
    return trCntr;
}

void BmnGemSeedFinder::SearchTrackCandInLine(const Int_t i, const Int_t y, BmnGemTrack* tr, Int_t* hitCntr, Int_t* maxDist, Int_t* dist, Int_t* startBin, Int_t* prevStation, Int_t gate, Bool_t isIdeal) {

    BmnGemStripHit* hit = NULL;

    for (Int_t j = y - gate; j <= y + gate; ++j) {
        ULong_t addr = j * fNBins + i;
        if (addresses.find(addr) == addresses.end()) continue;
        hit = GetHit(addresses.find(addr)->second);

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
        //hit->SetChange(kTRUE);
        tr->AddHit(addresses.find(addr)->second, hit);
    }
}

Bool_t BmnGemSeedFinder::CalculateTrackParams(BmnGemTrack* tr, TVector3 circPar, TVector3 linePar) {
    //Needed for start approximation of track parameters

    Float_t R = circPar.Z(); // radius of fit-circle
    Float_t Xc = circPar.X(); // x-coordinate of fit-circle center
    Float_t Zc = circPar.Y(); // z-coordinate of fit-circle center
    fField = FairRunAna::Instance()->GetField();
    const Int_t nHits = tr->GetNHits();
    BmnGemStripHit* lastHit = GetHit(tr->GetHitIndex(nHits - 1));
    BmnGemStripHit* firstHit = GetHit(tr->GetHitIndex(0));
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
        BmnGemStripHit* hit = GetHit(tr->GetHitIndex(i));
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
//    par.Print();

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

void BmnGemSeedFinder::FindXYRSeeds(TH1F* h) {
    for (Int_t hitIdx = 0; hitIdx < fGemHitsArray->GetEntriesFast(); ++hitIdx) {
        BmnGemStripHit* hit = GetHit(hitIdx);
        if (hit->IsUsed()) continue;
        const Float_t newY = hit->GetY() / Sqrt(Sqr(hit->GetX()) + Sqr(hit->GetY()) + Sqr(hit->GetZ()));
        hit->SetDetectorID(ceil((newY - h->GetXaxis()->GetXmin()) / h->GetBinWidth(7))); // 7 - not important
        h->Fill(newY);
    }
}

TVector3 BmnGemSeedFinder::CircleFit(BmnGemTrack* track) {

    const Float_t nHits = track->GetNHits();
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
        //        BmnGemStripHit* hit = GetHit(track->GetHitIndex(i));
        FairHit* hit = (FairHit*) fGemHitsArray->At(track->GetHitIndex(i));
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
        BmnGemStripHit* hit = GetHit(track->GetHitIndex(i));
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
            BmnGemStripHit* hit = GetHit(track->GetHitIndex(i));
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
            BmnGemStripHit* hit = GetHit(track->GetHitIndex(i));
            Di = Dist(hit->GetX(), hit->GetZ(), Xc, Zc) - R;
            Sw += Wi;
            Swd += Wi * Sqr(Di);
        }

        Sig = Swd / Sw;
        thresh = C * Sig;
    }

    Float_t chi2 = 0.0;
    for (Int_t i = 0; i < nHits; ++i) {
        BmnGemStripHit* hit = GetHit(track->GetHitIndex(i));
        chi2 += Sqr(R - Dist(hit->GetX(), hit->GetZ(), Xc, Zc)) / R;
    }

    chi2 *= nHits;
    track->SetChi2(chi2);
    track->SetNDF(nHits - 1);
    if (chi2 > ChisquareQuantile(0.95, nHits - 1)) {
        for (Int_t i = 0; i < nHits; ++i) {
            BmnGemStripHit* hit = GetHit(track->GetHitIndex(i));
            hit->SetUsing(kFALSE);
        }
        //        cout << "Fitting was failed:\n";
        return TVector3(0.0, 0.0, 0.0);
    }

    return TVector3(Xc, Zc, R);
}

TVector3 BmnGemSeedFinder::LineFit(BmnGemTrack* track) {

    //Least Square Method//
    Float_t Zi = 0.0, Yi = 0.0; // coordinates of current track point
    Float_t a = 0.0, b = 0.0; // parameters of line: y = a * z + b
    Float_t SumZ = 0.0, SumY = 0.0, SumZY = 0.0, SumZ2 = 0.0;
    const Float_t nHits = track->GetNHits();
    for (Int_t i = 0; i < nHits; ++i) {
        BmnGemStripHit* hit = GetHit(track->GetHitIndex(i));
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

Float_t BmnGemSeedFinder::Dist(Float_t x1, Float_t y1, Float_t x2, Float_t y2) {
    if (Sqr(x1 - x2) + Sqr(y1 - y2) < 0.0) {
        return 0.0;
    } else {
        return Sqrt(Sqr(x1 - x2) + Sqr(y1 - y2));
    }
}

Float_t BmnGemSeedFinder::Sqr(Float_t x) {
    return x * x;
}

BmnGemStripHit* BmnGemSeedFinder::GetHit(Int_t i) {
    BmnGemStripHit* hit = (BmnGemStripHit*) fGemHitsArray->At(i);
    if (!hit) cout << "-W- Wrong attempting to get hit number " << i << " from fGemHitsArray, which contains " << fGemHitsArray->GetEntriesFast() << " elements" << endl;
    return hit;
}