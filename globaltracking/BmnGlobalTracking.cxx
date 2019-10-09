
/** BmnGlobalTracking.cxx
 * \author Sergey Merts <Sergey.Merts@gmail.com>
 * @since 2014
 * @version 1.0
 **/

#include <Fit/FitResult.h>
#include <iterator>
#include <map>
#include <vector>

#include "BmnEventHeader.h"
#include "BmnGlobalTracking.h"
#include "BmnMwpcGeometry.h"
#include "BmnSiliconTrack.h"
#include "FitWLSQ.h"
#include "TH1F.h"
#include "omp.h"

using namespace TMath;

static Float_t workTime = 0.0;
//-----------------------------------------

BmnGlobalTracking::BmnGlobalTracking() : fDoAlign(kFALSE),
                                         fInnerTracks(nullptr),
                                         fSiliconTracks(nullptr),
                                         fGemHits(nullptr),
                                         fGemTracks(nullptr),
                                         fSilHits(nullptr),
                                         fCscHits(nullptr),
                                         fGemVertex(nullptr),
                                         fTof1Hits(nullptr),
                                         fTof2Hits(nullptr),
                                         fDchHits(nullptr),
                                         fMCTracks(nullptr),
                                         fEvHead(nullptr),
                                         fIsField(kTRUE),
                                         fPDG(2212),
                                         fChiSqCut(100.),
                                         fVertex(nullptr),
                                         fIsSRC(kFALSE),
                                         fKalman(nullptr),
                                         fEventNo(0) {
}

BmnGlobalTracking::BmnGlobalTracking(Bool_t isField, Bool_t isExp, Bool_t doAlign) : fInnerTracks(nullptr),
                                                                                     fSiliconTracks(nullptr),
                                                                                     fGemHits(nullptr),
                                                                                     fCscHits(nullptr),
                                                                                     fGemTracks(nullptr),
                                                                                     fGemVertex(nullptr),
                                                                                     fTof1Hits(nullptr),
                                                                                     fTof2Hits(nullptr),
                                                                                     fDchHits(nullptr),
                                                                                     fMCTracks(nullptr),
                                                                                     fEvHead(nullptr),
                                                                                     fPDG(2212),
                                                                                     fChiSqCut(100.),
                                                                                     fVertex(nullptr),
                                                                                     fPeriod(7),
                                                                                     fIsSRC(kFALSE),
                                                                                     fDoAlign(doAlign),
                                                                                     fIsExp(isExp),
                                                                                     fIsField(isField),
                                                                                     fEventNo(0) {
    fKalman = new BmnKalmanFilter();

    if (fDoAlign) {
        fhXCscGemResid = new TH1F("fhXCscGemResid", "fhXCscGemResid", 600, -100.0, 100.0);
        fhYCscGemResid = new TH1F("fhYCscGemResid", "fhYCscGemResid", 600, -100.0, 100.0);
        fhXdXCscGemResid = new TH2F("fhXdXCscGemResid", "fhXdXCscGemResid", 200, -100, 100, 200, -100.0, 100.0);
        fhYdYCscGemResid = new TH2F("fhYdYCscGemResid", "fhYdYCscGemResid", 200, -100, 100, 200, -100.0, 100.0);
        fhTxdXCscGemResid = new TH2F("fhTxdXCscGemResid", "fhTxdXCscGemResid", 200, -0.5, 0.5, 200, -100.0, 100.0);
        fhTydYCscGemResid = new TH2F("fhTydYCscGemResid", "fhTydYCscGemResid", 200, -0.5, 0.5, 200, -100.0, 100.0);

        fhXSiGemResid = new TH1F("fhXSiGemResid", "fhXSiGemResid", 600, -10.0, 10.0);
        fhYSiGemResid = new TH1F("fhYSiGemResid", "fhYSiGemResid", 600, -10.0, 10.0);
        fhTxSiGemResid = new TH1F("fhTxSiGemResid", "fhTxSiGemResid", 600, -0.1, 0.1);
        fhTySiGemResid = new TH1F("fhTySiGemResid", "fhTySiGemResid", 600, -0.1, 0.1);
        fhXdXSiGemResid = new TH2F("fhXdXSiGemResid", "fhXdXSiGemResid", 200, -10, 10, 200, -10.0, 10.0);
        fhYdYSiGemResid = new TH2F("fhYdYSiGemResid", "fhYdYSiGemResid", 200, -10, 10, 200, -10.0, 10.0);
        fhTxdXSiGemResid = new TH2F("fhTxdXSiGemResid", "fhTxdXSiGemResid", 200, -0.1, 0.1, 200, -10.0, 10.0);
        fhTydYSiGemResid = new TH2F("fhTydYSiGemResid", "fhTydYSiGemResid", 200, -0.1, 0.1, 200, -10.0, 10.0);

        fhXTof1GemResid = new TH1F("fhXTof1GemResid", "fhXTof1GemResid", 600, -100.0, 100.0);
        fhYTof1GemResid = new TH1F("fhYTof1GemResid", "fhYTof1GemResid", 600, -100.0, 100.0);
        fhXdXTof1GemResid = new TH2F("fhXdXTof1GemResid", "fhXdXTof1GemResid", 200, -100, 100, 200, -100.0, 100.0);
        fhYdYTof1GemResid = new TH2F("fhYdYTof1GemResid", "fhYdYTof1GemResid", 200, -100, 100, 200, -100.0, 100.0);
        fhTxdXTof1GemResid = new TH2F("fhTxdXTof1GemResid", "fhTxdXTof1GemResid", 200, -0.5, 0.5, 200, -100.0, 100.0);
        fhTydYTof1GemResid = new TH2F("fhTydYTof1GemResid", "fhTydYTof1GemResid", 200, -0.5, 0.5, 200, -100.0, 100.0);

        fhXTof2GemResid = new TH1F("fhXTof2GemResid", "fhXTof2GemResid", 600, -100.0, 100.0);
        fhYTof2GemResid = new TH1F("fhYTof2GemResid", "fhYTof2GemResid", 600, -100.0, 100.0);
        fhXdXTof2GemResid = new TH2F("fhXdXTof2GemResid", "fhXdXTof2GemResid", 200, -100, 100, 200, -100.0, 100.0);
        fhYdYTof2GemResid = new TH2F("fhYdYTof2GemResid", "fhYdYTof2GemResid", 200, -100, 100, 200, -100.0, 100.0);
        fhTxdXTof2GemResid = new TH2F("fhTxdXTof2GemResid", "fhTxdXTof2GemResid", 200, -0.5, 0.5, 200, -100.0, 100.0);
        fhTydYTof2GemResid = new TH2F("fhTydYTof2GemResid", "fhTydYTof2GemResid", 200, -0.5, 0.5, 200, -100.0, 100.0);

        fhXDch1GemResid = new TH1F("fhXDch1GemResid", "fhXDch1GemResid", 600, -100.0, 100.0);
        fhYDch1GemResid = new TH1F("fhYDch1GemResid", "fhYDch1GemResid", 600, -100.0, 100.0);
        fhTxDch1GemResid = new TH1F("fhTxDch1GemResid", "fhTxDch1GemResid", 600, -0.5, 0.5);
        fhTyDch1GemResid = new TH1F("fhTyDch1GemResid", "fhTyDch1GemResid", 600, -0.5, 0.5);
        fhXdXDch1GemResid = new TH2F("fhXdXDch1GemResid", "fhXdXDch1GemResid", 200, -100, 100, 200, -100.0, 100.0);
        fhYdYDch1GemResid = new TH2F("fhYdYDch1GemResid", "fhYdYDch1GemResid", 200, -100, 100, 200, -100.0, 100.0);
        fhTxdXDch1GemResid = new TH2F("fhTxdXDch1GemResid", "fhTxdXDch1GemResid", 200, -0.5, 0.5, 200, -100.0, 100.0);
        fhTydYDch1GemResid = new TH2F("fhTydYDch1GemResid", "fhTydYDch1GemResid", 200, -0.5, 0.5, 200, -100.0, 100.0);

        fhXDch2GemResid = new TH1F("fhXDch2GemResid", "fhXDch2GemResid", 600, -100.0, 100.0);
        fhYDch2GemResid = new TH1F("fhYDch2GemResid", "fhYDch2GemResid", 600, -100.0, 100.0);
        fhTxDch2GemResid = new TH1F("fhTxDch2GemResid", "fhTxDch2GemResid", 600, -0.5, 0.5);
        fhTyDch2GemResid = new TH1F("fhTyDch2GemResid", "fhTyDch2GemResid", 600, -0.5, 0.5);
        fhXdXDch2GemResid = new TH2F("fhXdXDch22GemResid", "fhXdXDch2GemResid", 200, -100, 100, 200, -100.0, 100.0);
        fhYdYDch2GemResid = new TH2F("fhYdYDch2GemResid", "fhYdYDch2GemResid", 200, -100, 100, 200, -100.0, 100.0);
        fhTxdXDch2GemResid = new TH2F("fhTxdXDch2GemResid", "fhTxdXDch2GemResid", 200, -0.5, 0.5, 200, -100.0, 100.0);
        fhTydYDch2GemResid = new TH2F("fhTydYDch2GemResid", "fhTydYDch2GemResid", 200, -0.5, 0.5, 200, -100.0, 100.0);

        const Int_t nSt = 6;

        fhdXGemSt = new TH1F *[nSt];
        fhdYGemSt = new TH1F *[nSt];
        fhdTxGemSt = new TH1F *[nSt];
        fhdTyGemSt = new TH1F *[nSt];
        fhXdXGemSt = new TH2F *[nSt];
        fhYdYGemSt = new TH2F *[nSt];

        for (Int_t i = 0; i < nSt; ++i) {
            TString str1 = Form("fhdXGemSt_%d", i);
            TString str2 = Form("fhdYGemSt_%d", i);
            TString str3 = Form("fhdTxGemSt_%d", i);
            TString str4 = Form("fhdTyGemSt_%d", i);
            TString str5 = Form("fhXdXGemSt_%d", i);
            TString str6 = Form("fhYdYGemSt_%d", i);
            fhdXGemSt[i] = new TH1F(str1, str1, 400, -1.0, 1.0);
            fhdYGemSt[i] = new TH1F(str2, str2, 400, -1.0, 1.0);
            fhXdXGemSt[i] = new TH2F(str5, str5, 200, -20.0, 20.0, 200, -1.0, 1.0);
            fhYdYGemSt[i] = new TH2F(str6, str6, 200, -20.0, 20.0, 200, -1.0, 1.0);
        }
    }
}

BmnGlobalTracking::~BmnGlobalTracking() {
}

InitStatus BmnGlobalTracking::Init() {
    if (fVerbose > 1)
        cout << "BmnGlobalTracking::Init started\n";

    FairRootManager *ioman = FairRootManager::Instance();
    if (!ioman)
        Fatal("Init", "FairRootManager is not instantiated");

    fDet.DetermineSetup();
    if (fVerbose > 1)
        cout << fDet.ToString();

    fSilHits = (TClonesArray *)ioman->GetObject("BmnSiliconHit");
    fGemHits = (TClonesArray *)ioman->GetObject("BmnGemStripHit");
    fCscHits = (TClonesArray *)ioman->GetObject("BmnCSCHit");
    fTof1Hits = (TClonesArray *)ioman->GetObject("BmnTof400Hit");
    fTof2Hits = (TClonesArray *)ioman->GetObject("BmnTof700Hit");

    fInnerTracks = (TClonesArray *)ioman->GetObject("BmnGlobalTrack");
    fGemTracks = (TClonesArray *)ioman->GetObject("BmnGemTrack");

    if (!fInnerTracks) {
        cout << "BmnGlobalTracking::Init(): branch BmnInnerTrack not found! Task will be deactivated" << endl;
        SetActive(kFALSE);
        return kERROR;
    }

    fSiliconTracks = (TClonesArray *)ioman->GetObject("BmnSiliconTrack");
    if (!fSiliconTracks)
        cout << "BmnGlobalTracking::Init(): branch BmnSiliconTrack not found!" << endl;

    fDchTracks = (TClonesArray *)ioman->GetObject("BmnDchTrack");
    if (!fDchTracks)
        cout << "BmnGlobalTracking::Init(): branch BmnDchTrack not found!" << endl;

    //Artificial dch hits (on the first dch plain)
    fDchHits = new TClonesArray("BmnHit", 100);  //out
    ioman->Register("BmnDchHit", "DCH", fDchHits, kTRUE);

    fEvHead = (TClonesArray *)ioman->GetObject("EventHeader");
    if (!fEvHead)
        if (fVerbose > 1)
            cout << "Init. No EventHeader array!" << endl;

    if (fVerbose > 1)
        cout << "BmnGlobalTracking::Init finished\n";
    return kSUCCESS;
}

void BmnGlobalTracking::Exec(Option_t *opt) {
    if (!IsActive())
        return;

    if (fVerbose > 1)
        cout << "\n======================== Global tracking exec started =====================\n"
             << endl;
    fEventNo++;
    clock_t tStart = clock();

    if (!fInnerTracks) return;
    fDchHits->Delete();

    //Alignment. FIXME: move to DB
    if (fIsExp) {
        if (fIsSRC) {
            if (fDchTracks)
                for (Int_t trIdx = 0; trIdx < fDchTracks->GetEntriesFast(); ++trIdx) {
                    BmnTrack *dchTr = (BmnTrack *)fDchTracks->At(trIdx);
                    FairTrackParam *parDch = dchTr->GetParamFirst();
                    Double_t zDCH = parDch->GetZ();
                    BmnHit dchHit;

                    if (zDCH < 550) {
                        parDch->SetTx(parDch->GetTx() + 0.073);
                        parDch->SetTy(parDch->GetTy() + 0.06);
                        parDch->SetX(parDch->GetX() - 10.46);
                        parDch->SetY(parDch->GetY() - 3.76);
                        dchHit.SetStation(0);
                    }
                    if (zDCH > 650) {
                        parDch->SetTx(parDch->GetTx() + 0.074);
                        parDch->SetTy(parDch->GetTy() + 0.071);
                        parDch->SetX(parDch->GetX() - 4.78);
                        parDch->SetY(parDch->GetY() - 4.04);
                        dchHit.SetStation(1);
                    }
                    dchHit.SetXYZ(parDch->GetX(), parDch->GetY(), zDCH);
                    dchHit.SetDxyz(0.1, 0.1, 0.0);
                    dchHit.SetIndex(trIdx);  //index of dch track instead of index of hit. In order to have fast link hit->track
                    new ((*fDchHits)[fDchHits->GetEntriesFast()]) BmnHit(dchHit);
                }
            if (fCscHits)
                for (Int_t hitIdx = 0; hitIdx < fCscHits->GetEntriesFast(); ++hitIdx) {
                    BmnHit *hit = (BmnHit *)fCscHits->At(hitIdx);
                    hit->SetX(hit->GetX() - 15.88);
                    hit->SetY(hit->GetY() - 5.95);
                }
            if (fTof2Hits)
                for (Int_t hitIdx = 0; hitIdx < fTof2Hits->GetEntriesFast(); ++hitIdx) {
                    BmnHit *hit = (BmnHit *)fTof2Hits->At(hitIdx);
                    //Panin
                    //hit->SetX(hit->GetX() - 0.47);
                    //hit->SetY(hit->GetY() - 10.18);
                    //Petukhov
                    hit->SetX(hit->GetX() - 1.61);
                    hit->SetY(hit->GetY() - 6.80);
                }
        } else {
            if (fDchTracks)
                for (Int_t trIdx = 0; trIdx < fDchTracks->GetEntriesFast(); ++trIdx) {
                    BmnTrack *dchTr = (BmnTrack *)fDchTracks->At(trIdx);
                    FairTrackParam *parDch = dchTr->GetParamFirst();
                    Double_t zDCH = parDch->GetZ();
                    BmnHit dchHit;

                    if (zDCH < 550) {
                        parDch->SetTx(parDch->GetTx() + 0.072);
                        parDch->SetTy(parDch->GetTy() + 0.06);
                        parDch->SetX(parDch->GetX() - 9.92);
                        parDch->SetY(parDch->GetY() - 3.34);
                        dchHit.SetStation(0);
                    }
                    if (zDCH > 650) {
                        parDch->SetTx(parDch->GetTx() + 0.073);
                        parDch->SetTy(parDch->GetTy() + 0.071);
                        parDch->SetX(parDch->GetX() - 4.40);
                        parDch->SetY(parDch->GetY() - 3.77);
                        dchHit.SetStation(1);
                    }
                    dchHit.SetXYZ(parDch->GetX(), parDch->GetY(), zDCH);
                    dchHit.SetDxyz(0.03, 0.03, 0.0);
                    dchHit.SetIndex(trIdx);  //index of dch track instead of index of hit. In order to have fast link hit->track
                    new ((*fDchHits)[fDchHits->GetEntriesFast()]) BmnHit(dchHit);
                }
            if (fCscHits)
                for (Int_t hitIdx = 0; hitIdx < fCscHits->GetEntriesFast(); ++hitIdx) {
                    BmnHit *hit = (BmnHit *)fCscHits->At(hitIdx);
                    hit->SetX(hit->GetX() + 0.89);
                    hit->SetY(hit->GetY());
                }
            if (fTof1Hits)
                for (Int_t hitIdx = 0; hitIdx < fTof1Hits->GetEntriesFast(); ++hitIdx) {
                    BmnHit *hit = (BmnHit *)fTof1Hits->At(hitIdx);
                    hit->SetX(hit->GetX() - 1.83);
                    hit->SetY(hit->GetY() + 0.34);
                }
            if (fTof2Hits)
                for (Int_t hitIdx = 0; hitIdx < fTof2Hits->GetEntriesFast(); ++hitIdx) {
                    BmnHit *hit = (BmnHit *)fTof2Hits->At(hitIdx);
                    hit->SetX(hit->GetX() + 0.42);
                    hit->SetY(hit->GetY() - 9.64 + 4.47);
                }
        }
    }

    //if (fInnerTracks->GetEntriesFast() != 1) return;

    for (Int_t i = 0; i < fInnerTracks->GetEntriesFast(); ++i) {
        BmnGlobalTrack *glTrack = (BmnGlobalTrack *)fInnerTracks->At(i);
        //if (glTrack->GetNHits() < 5) continue;

        if (fDoAlign) {
            BmnGemTrack *gemTrack = (BmnGemTrack *)fGemTracks->At(glTrack->GetGemTrackIndex());
            for (Int_t hitIdx = 0; hitIdx < gemTrack->GetNHits(); ++hitIdx) {
                BmnGemStripHit *hit = (BmnGemStripHit *)fGemHits->At(gemTrack->GetHitIndex(hitIdx));
                Int_t stId = (fIsSRC) ? hit->GetStation() - 4 : hit->GetStation();
                fhdXGemSt[stId]->Fill(hit->GetResX());
                fhdYGemSt[stId]->Fill(hit->GetResY());
                fhXdXGemSt[stId]->Fill(hit->GetX(), hit->GetResX());
                fhYdYGemSt[stId]->Fill(hit->GetY(), hit->GetResY());
            }
        }

        //Upstream
        if (fIsSRC) MatchingSil(glTrack);
        MatchingMWPC(glTrack);

        //Downstream
        MatchingCSC(glTrack);
        if (!fIsSRC) MatchingTOF(glTrack, 1);
        MatchingDCH(glTrack, 1);
        MatchingTOF(glTrack, 2);
        MatchingDCH(glTrack, 2);

        Refit(glTrack);
        CalcdQdn(glTrack);

        // NDF = (N counts in ZX plane + N counts in ZY plane) - 2 parameters of Line in ZY plane - 3 parameters of Circle in ZX plane
        // Check it!!!
        glTrack->SetNDF(glTrack->GetNHits() * 2 - 5);
    }

    //CalculateLength();

    clock_t tFinish = clock();
    workTime += ((Float_t)(tFinish - tStart)) / CLOCKS_PER_SEC;

    //if (fVerbose) cout << "GLOBAL_TRACKING: Number of merged tracks: " << fGlobalTracks->GetEntriesFast() << endl;
    if (fVerbose > 1)
        cout << "\n======================== Global tracking exec finished ====================\n"
             << endl;
}

void BmnGlobalTracking::CalcdQdn(BmnGlobalTrack *tr) {
    BmnGemTrack *gemTrack = (BmnGemTrack *)fGemTracks->At(tr->GetGemTrackIndex());
    Double_t totSigLow = 0.0;
    Double_t totSigUp = 0.0;
    for (Int_t hitIdx = 0; hitIdx < gemTrack->GetNHits(); ++hitIdx) {
        BmnGemStripHit *hit = (BmnGemStripHit *)fGemHits->At(gemTrack->GetHitIndex(hitIdx));
        totSigLow += hit->GetStripTotalSignalInLowerLayer();
        totSigUp += hit->GetStripTotalSignalInUpperLayer();
    }
    totSigLow /= gemTrack->GetNHits();
    totSigUp /= gemTrack->GetNHits();
    tr->SetdQdNLower(totSigLow);
    tr->SetdQdNUpper(totSigUp);
}

void BmnGlobalTracking::CalcSiliconDist(Int_t stat, BmnGlobalTrack *glTr, map<Double_t, pair<Int_t, Int_t>> &silDists) {
    const Double_t distCut = 1.;
    for (Int_t hitIdx = 0; hitIdx < fSilHits->GetEntriesFast(); hitIdx++) {
        BmnSiliconHit *hit = (BmnSiliconHit *)fSilHits->UncheckedAt(hitIdx);
        if (stat != hit->GetStation())
            continue;

        FairTrackParam parPredict = *glTr->GetParamFirst();

        if (fKalman->TGeoTrackPropagate(&parPredict, hit->GetZ(), fPDG, nullptr, nullptr, fIsField) == kBMNERROR)
            continue;

        Double_t dist = Sqrt(Sq(parPredict.GetX() - hit->GetX()) + Sq(parPredict.GetY() - hit->GetY()));
        if (dist > distCut)
            continue;
        silDists.insert(pair<Double_t, pair<Int_t, Int_t>>(dist, make_pair(glTr->GetGemTrackIndex(), hitIdx)));

        glTr->SetParamFirst(parPredict);
    }
}

Int_t BmnGlobalTracking::FindNearestHit(BmnGlobalTrack *tr, TClonesArray *hits, Float_t distCut) {
    if (!hits || !tr)
        return kBMNERROR;

    Double_t minDist = DBL_MAX;
    Int_t minIdx = -1;

    for (Int_t hitIdx = 0; hitIdx < hits->GetEntriesFast(); ++hitIdx) {
        BmnHit *hit = (BmnHit *)hits->At(hitIdx);
        FairTrackParam parPredict(*(tr->GetParamLast()));
        if (fKalman->TGeoTrackPropagate(&parPredict, hit->GetZ(), fPDG, nullptr, nullptr, fIsField) == kBMNERROR)
            continue;
        Double_t dist = Sqrt(Sq(parPredict.GetX() - hit->GetX()) + Sq(parPredict.GetY() - hit->GetY()));
        if (dist < minDist && dist < distCut) {
            minDist = dist;
            minIdx = hitIdx;
        }
    }

    return minIdx;
}

BmnStatus BmnGlobalTracking::MatchingMWPC(BmnGlobalTrack *tr) {
    if (!fSilHits) return kBMNERROR;
}

BmnStatus BmnGlobalTracking::MatchingCSC(BmnGlobalTrack *tr) {
    if (!fCscHits) return kBMNERROR;

    //residuals after peak fitting of all-to-all histograms
    Double_t sigmaXcscgemResid = 1.25;
    Double_t sigmaYcscgemResid = 0.79;
    Double_t xCut = 3 * sigmaXcscgemResid;
    Double_t yCut = 3 * sigmaYcscgemResid;

    Double_t minDX = DBL_MAX;
    Double_t minDY = DBL_MAX;
    Double_t dX = DBL_MAX;
    Double_t dY = DBL_MAX;

    Double_t minDist = DBL_MAX;
    BmnHit *minHit = nullptr;  // Pointer to the nearest hit
    Int_t minIdx = -1;

    for (Int_t hitIdx = 0; hitIdx < fCscHits->GetEntriesFast(); ++hitIdx) {
        BmnHit *hit = (BmnHit *)fCscHits->At(hitIdx);
        FairTrackParam par(*(tr->GetParamLast()));
        if (fKalman->TGeoTrackPropagate(&par, hit->GetZ(), fPDG, nullptr, nullptr, fIsField) == kBMNERROR)
            continue;
        dX = par.GetX() - hit->GetX();
        dY = par.GetY() - hit->GetY();
        Double_t dist = Sqrt(Sq(par.GetX() - hit->GetX()) + Sq(par.GetY() - hit->GetY()));
        if (fDoAlign) {
            fhXCscGemResid->Fill(par.GetX() - hit->GetX());
            fhYCscGemResid->Fill(par.GetY() - hit->GetY());
            fhXdXCscGemResid->Fill(par.GetX(), par.GetX() - hit->GetX());
            fhYdYCscGemResid->Fill(par.GetY(), par.GetY() - hit->GetY());
            fhTxdXCscGemResid->Fill(par.GetTx(), par.GetX() - hit->GetX());
            fhTydYCscGemResid->Fill(par.GetTy(), par.GetY() - hit->GetY());
        }
        if (Abs(dX) < xCut && Abs(dY) < yCut && Abs(dX) < minDX && Abs(dY) < minDY) {
            minDX = dX;
            minDY = dY;
            minHit = hit;
            minIdx = hitIdx;
        }
    }

    if (minHit == nullptr)
        return kBMNERROR;

    FairTrackParam par(*(tr->GetParamLast()));
    Double_t len = tr->GetLength();
    fKalman->TGeoTrackPropagate(&par, minHit->GetZ(), fPDG, nullptr, &len, fIsField);
    minHit->SetResXY(par.GetX() - minHit->GetX(), par.GetY() - minHit->GetY());
    Double_t chi = 0;
    fKalman->Update(&par, minHit, chi);
    tr->SetChi2(tr->GetChi2() + chi);
    tr->SetCscHitIndex(minIdx);
    tr->SetNHits(tr->GetNHits() + 1);
    tr->SetLength(len);
    tr->SetParamLast(par);
    return kBMNSUCCESS;
}

BmnStatus BmnGlobalTracking::MatchingTOF(BmnGlobalTrack *tr, Int_t num) {
    TClonesArray *tofHits = (num == 1) ? fTof1Hits : (num == 2) ? fTof2Hits : nullptr;
    if (!tofHits) return kBMNERROR;

    //residuals after peak fitting of all-to-all histograms
    Double_t sigmaXtof1gemResid = 1000;
    Double_t sigmaYtof1gemResid = 1000;
    Double_t sigmaXtof2gemResid = 1.34;
    Double_t sigmaYtof2gemResid = 1.38;
    Double_t xCut = (num == 1) ? 3 * sigmaXtof1gemResid : 3 * sigmaXtof2gemResid;
    Double_t yCut = (num == 1) ? 3 * sigmaYtof1gemResid : 3 * sigmaYtof2gemResid;

    Double_t minDX = DBL_MAX;
    Double_t minDY = DBL_MAX;
    Double_t dX = DBL_MAX;
    Double_t dY = DBL_MAX;

    BmnHit *minHit = nullptr;  // Pointer to the nearest hit
    Int_t minIdx = -1;
    Double_t LenPropLast = 0., LenPropFirst = 0.;
    FairTrackParam minParPredLast;  // predicted track parameters for closest hit
    for (Int_t hitIdx = 0; hitIdx < tofHits->GetEntriesFast(); ++hitIdx) {
        BmnHit *hit = (BmnHit *)tofHits->At(hitIdx);
        if (hit->IsUsed())
            continue;  // skip Tof hit used before
        FairTrackParam parPredict(*(tr->GetParamLast()));
        Double_t len = 0.;
        if (fKalman->TGeoTrackPropagate(&parPredict, hit->GetZ(), fPDG, nullptr, &len, fIsField) == kBMNERROR) continue;
        dX = parPredict.GetX() - hit->GetX();
        dY = parPredict.GetY() - hit->GetY();
        if (fDoAlign) {
            if (num == 1) {
                fhXTof1GemResid->Fill(parPredict.GetX() - hit->GetX());
                fhYTof1GemResid->Fill(parPredict.GetY() - hit->GetY());
                fhXdXTof1GemResid->Fill(parPredict.GetX(), parPredict.GetX() - hit->GetX());
                fhYdYTof1GemResid->Fill(parPredict.GetY(), parPredict.GetY() - hit->GetY());
                fhTxdXTof1GemResid->Fill(parPredict.GetTx(), parPredict.GetX() - hit->GetX());
                fhTydYTof1GemResid->Fill(parPredict.GetTy(), parPredict.GetY() - hit->GetY());
            } else if (num == 2) {
                //if (Abs(parPredict.GetY() - hit->GetY()) < 2) {
                fhXTof2GemResid->Fill(parPredict.GetX() - hit->GetX());
                fhYTof2GemResid->Fill(parPredict.GetY() - hit->GetY());
                fhXdXTof2GemResid->Fill(parPredict.GetX(), parPredict.GetX() - hit->GetX());
                fhYdYTof2GemResid->Fill(parPredict.GetY(), parPredict.GetY() - hit->GetY());
                fhTxdXTof2GemResid->Fill(parPredict.GetTx(), parPredict.GetX() - hit->GetX());
                fhTydYTof2GemResid->Fill(parPredict.GetTy(), parPredict.GetY() - hit->GetY());
                //}
            }
        }
        if (Abs(dX) < xCut && Abs(dY) < yCut && Abs(dX) < minDX && Abs(dY) < minDY) {
            minDX = dX;
            minDY = dY;
            minHit = hit;
            minParPredLast = parPredict;
            minIdx = hitIdx;
            LenPropLast = len;
        }
    }

    if (minHit == nullptr)
        return kBMNERROR;

    minHit->SetResXY(minDX, minDY);

    FairTrackParam ParPredFirst(*(tr->GetParamFirst()));
    Double_t zTarget = (fVertex) ? fVertex->GetZ() : (fIsSRC) ? -647.5 : -2.3;  // z of target by default
    if (fKalman->TGeoTrackPropagate(&ParPredFirst, zTarget, fPDG, nullptr, &LenPropFirst, fIsField) == kBMNERROR)
        return kBMNERROR;
    if (num == 1)
        tr->SetTof1HitIndex(minIdx);
    else
        tr->SetTof2HitIndex(minIdx);

    Double_t l = LenPropFirst + tr->GetLength() + LenPropLast;
    tr->SetBeta(l / minHit->GetTimeStamp() / (TMath::C() * 1e-7), num);
    minHit->SetUsing(kTRUE);
    minHit->SetLength(l);  // length from target to Tof hit
    tr->SetNHits(tr->GetNHits() + 1);
    tr->SetParamLast(minParPredLast);
    return kBMNSUCCESS;
}

BmnStatus BmnGlobalTracking::MatchingSil(BmnGlobalTrack *glTr) {
    //we need this function only for SRC
    //In BM@N we use silicon and GEM hits as a whole
    if (!fSilHits) return kBMNERROR;
}

BmnStatus BmnGlobalTracking::MatchingDCH(BmnGlobalTrack *tr, Int_t num) {
    if (!fDchTracks) return kBMNERROR;

    Double_t minDX = DBL_MAX;
    Double_t minDY = DBL_MAX;
    BmnHit *minHit = nullptr;

    //residuals after peak fitting of all-to-all histograms
    Double_t sigmaXdch1gemResid = 2.16;
    Double_t sigmaYdch1gemResid = 0.75;
    Double_t sigmaXdch2gemResid = 0.90;
    Double_t sigmaYdch2gemResid = 0.53;
    Double_t xCut = (num == 1) ? 4 * sigmaXdch1gemResid : 4 * sigmaXdch2gemResid;
    Double_t yCut = (num == 1) ? 4 * sigmaYdch1gemResid : 4 * sigmaYdch2gemResid;

    for (Int_t iHit = 0; iHit < fDchHits->GetEntriesFast(); ++iHit) {
        BmnHit *hit = (BmnHit *)fDchHits->At(iHit);
        if (!hit) continue;
        if (hit->GetStation() != num - 1) continue;
        FairTrackParam par(*(tr->GetParamLast()));
        if (fKalman->TGeoTrackPropagate(&par, hit->GetZ(), fPDG, nullptr, nullptr, fIsField) == kBMNERROR)
            continue;
        //Double_t dist = Sqrt(Sq(par.GetX() - hit->GetX()) + Sq(par.GetY() - hit->GetY()));
        Double_t dX = par.GetX() - hit->GetX();
        Double_t dY = par.GetY() - hit->GetY();
        if (fDoAlign) {
            BmnTrack *dchTr = (BmnTrack *)fDchTracks->At(hit->GetIndex());
            if (!dchTr) continue;
            FairTrackParam dchPar(*(dchTr->GetParamFirst()));
            if (num == 1) {
                fhXDch1GemResid->Fill(dX);
                fhYDch1GemResid->Fill(dY);
                fhTxDch1GemResid->Fill(par.GetTx() - dchPar.GetTx());
                fhTyDch1GemResid->Fill(par.GetTy() - dchPar.GetTy());
                fhXdXDch1GemResid->Fill(par.GetX(), dX);
                fhYdYDch1GemResid->Fill(par.GetY(), dY);
                fhTxdXDch1GemResid->Fill(par.GetTx(), dX);
                fhTydYDch1GemResid->Fill(par.GetTy(), dY);
            } else {
                fhXDch2GemResid->Fill(dX);
                fhYDch2GemResid->Fill(dY);
                fhTxDch2GemResid->Fill(par.GetTx() - dchPar.GetTx());
                fhTyDch2GemResid->Fill(par.GetTy() - dchPar.GetTy());
                fhXdXDch2GemResid->Fill(par.GetX(), dX);
                fhYdYDch2GemResid->Fill(par.GetY(), dY);
                fhTxdXDch2GemResid->Fill(par.GetTx(), dX);
                fhTydYDch2GemResid->Fill(par.GetTy(), dY);
            }
        }
        if (Abs(dX) < xCut && Abs(dY) < yCut && Abs(dX) < minDX && Abs(dY) < minDY) {
            minHit = hit;
            minDX = dX;
            minDY = dY;
        }
    }

    if (minHit == nullptr)
        return kBMNERROR;

    FairTrackParam par(*(tr->GetParamLast()));
    Double_t len = tr->GetLength();
    fKalman->TGeoTrackPropagate(&par, minHit->GetZ(), fPDG, nullptr, &len, fIsField);
    minHit->SetResXY(minDX, minDY);
    Double_t chi = 0;
    fKalman->Update(&par, minHit, chi);
    tr->SetChi2(tr->GetChi2() + chi);
    if (num == 1)
        tr->SetDch1TrackIndex(minHit->GetIndex());
    else
        tr->SetDch2TrackIndex(minHit->GetIndex());
    BmnTrack *matchedDch = (BmnTrack *)fDchTracks->At(minHit->GetIndex());
    tr->SetNHits(tr->GetNHits() + matchedDch->GetNHits());
    tr->SetLength(len);
    tr->SetParamLast(par);
    return kBMNSUCCESS;
}

BmnStatus BmnGlobalTracking::RefitToDetector(BmnGlobalTrack *tr, Int_t hitId, TClonesArray *hitArr, FairTrackParam *par, Int_t *nodeIdx, vector<BmnFitNode> *nodes) {
    if (tr->GetTof2HitIndex() != -1) {
        BmnHit *hit = (BmnHit *)hitArr->At(hitId);
        Float_t Ze = hit->GetZ();
        Double_t length = 0;
        vector<Double_t> F(25);
        if (fKalman->TGeoTrackPropagate(par, Ze, 2212, &F, &length, fIsField) == kBMNERROR) {
            tr->SetFlag(kBMNBAD);
            return kBMNERROR;
        }

        nodes->at(*nodeIdx).SetPredictedParam(par);
        nodes->at(*nodeIdx).SetF(F);
        Double_t chi2Hit = 0.;
        if (fKalman->Update(par, hit, chi2Hit) == kBMNERROR) {
            tr->SetFlag(kBMNBAD);
            return kBMNERROR;
        }
        //        tr->SetParamLast(par);
        //        tr->SetParamFirst(par);

        nodes->at(*nodeIdx).SetUpdatedParam(par);
        nodes->at(*nodeIdx).SetChiSqFiltered(chi2Hit);
        tr->SetChi2(tr->GetChi2() + chi2Hit);
        tr->SetLength(tr->GetLength() + length);
        (*nodeIdx)--;
    }
    return kBMNSUCCESS;
}

BmnStatus BmnGlobalTracking::Refit(BmnGlobalTrack *tr) {
    FairTrackParam par = *(tr->GetParamLast());
    if (tr->GetDch2TrackIndex() != -1) {
        BmnHit *hit = (BmnHit *)fDchHits->At(tr->GetDch2TrackIndex());
        fKalman->TGeoTrackPropagate(&par, hit->GetZ(), 2212, nullptr, nullptr, fIsField);
        Double_t chi = 0.0;
        fKalman->Update(&par, hit, chi);
    }
    //Skip Tof700, it's not coordinate detector
    if (tr->GetDch1TrackIndex() != -1) {
        BmnHit *hit = (BmnHit *)fDchHits->At(tr->GetDch1TrackIndex());
        fKalman->TGeoTrackPropagate(&par, hit->GetZ(), 2212, nullptr, nullptr, fIsField);
        Double_t chi = 0.0;
        fKalman->Update(&par, hit, chi);
    }
    if (tr->GetCscHitIndex() != -1) {
        BmnHit *hit = (BmnHit *)fCscHits->At(tr->GetCscHitIndex());
        fKalman->TGeoTrackPropagate(&par, hit->GetZ(), 2212, nullptr, nullptr, fIsField);
        Double_t chi = 0.0;
        fKalman->Update(&par, hit, chi);
    }
    BmnGemTrack *gemTrack = (BmnGemTrack *)fGemTracks->At(tr->GetGemTrackIndex());
    for (Int_t hitIdx = gemTrack->GetNHits() - 1; hitIdx >= 0; hitIdx--) {
        BmnGemStripHit *hit = (BmnGemStripHit *)fGemHits->At(gemTrack->GetHitIndex(hitIdx));
        Double_t chi = 0.0;
        fKalman->TGeoTrackPropagate(&par, hit->GetZ(), 2212, nullptr, nullptr, fIsField);
        fKalman->Update(&par, hit, chi);
    }
    // cout << "BEFORE REFIT:\n";
    // tr->GetParamFirst()->Print();
    // cout << "AFTER  REFIT:\n";
    // par.Print();
    tr->SetParamFirst(par);

    //    vector<BmnFitNode> nodes(tr->GetNHits());
    //    Int_t nodeIdx = tr->GetNHits() - 1;
    //    FairTrackParam par = *(tr->GetParamLast());
    //    //    FairTrackParam par = *(tr->GetParamFirst());
    //
    //    //TOF2 part
    //    //    if (fDet.GetDet(kTOF) && tr->GetTof2HitIndex() != -1 && fTof2Hits) {
    //    //        if (RefitToDetector(tr, tr->GetTof2HitIndex(), fTof2Hits, &par, &nodeIdx, &nodes) == kBMNERROR) return kBMNERROR;
    //    //    }
    //
    //    //DCH1 part
    //    //        if (fDet.GetDet(kDCH) && tr->GetDchHitIndex() != -1 && fDchHits) {
    //    //            if (RefitToDetector(tr, tr->GetDchHitIndex(), fDchHits, &par, &nodeIdx, &nodes) == kBMNERROR) return kBMNERROR;
    //    //        }
    //
    //    //TOF1 part
    //    if (fDet.GetDet(kTOF1) && tr->GetTof1HitIndex() != -1 && fTof1Hits) {
    //        if (RefitToDetector(tr, tr->GetTof1HitIndex(), fTof1Hits, &par, &nodeIdx, &nodes) == kBMNERROR) return kBMNERROR;
    //    }
    //
    //    //GEM part
    //    if (fDet.GetDet(kGEM) && tr->GetGemTrackIndex() != -1) {
    //        BmnGemTrack* gemTr = (BmnGemTrack*) fInnerTracks->At(tr->GetGemTrackIndex());
    //        for (Int_t i = gemTr->GetNHits() - 1; i >= 0; --i) {
    //            if (RefitToDetector(tr, i, fGemHits, &par, &nodeIdx, &nodes) == kBMNERROR) return kBMNERROR;
    //        }
    //    }
    //
    //    tr->SetParamFirst(par);
    //    tr->SetFitNodes(nodes);
    return kBMNSUCCESS;
}

void BmnGlobalTracking::Finish() {
    delete fKalman;

    if (fDoAlign) {
        TFile matchResid("matchResid.root", "RECREATE");

        // matchResid.Add(fhXSiGemResid);
        // matchResid.Add(fhYSiGemResid);
        // matchResid.Add(fhTxSiGemResid);
        // matchResid.Add(fhTySiGemResid);
        // matchResid.Add(fhXdXSiGemResid);
        // matchResid.Add(fhYdYSiGemResid);
        // matchResid.Add(fhTxdXSiGemResid);
        // matchResid.Add(fhTydYSiGemResid);

        matchResid.Add(fhXCscGemResid);
        matchResid.Add(fhYCscGemResid);
        matchResid.Add(fhXdXCscGemResid);
        matchResid.Add(fhYdYCscGemResid);
        matchResid.Add(fhTxdXCscGemResid);
        matchResid.Add(fhTydYCscGemResid);

        matchResid.Add(fhXTof1GemResid);
        matchResid.Add(fhYTof1GemResid);
        matchResid.Add(fhXdXTof1GemResid);
        matchResid.Add(fhYdYTof1GemResid);
        matchResid.Add(fhTxdXTof1GemResid);
        matchResid.Add(fhTydYTof1GemResid);

        matchResid.Add(fhXTof2GemResid);
        matchResid.Add(fhYTof2GemResid);
        matchResid.Add(fhXdXTof2GemResid);
        matchResid.Add(fhYdYTof2GemResid);
        matchResid.Add(fhTxdXTof2GemResid);
        matchResid.Add(fhTydYTof2GemResid);

        matchResid.Add(fhXDch1GemResid);
        matchResid.Add(fhYDch1GemResid);
        matchResid.Add(fhTxDch1GemResid);
        matchResid.Add(fhTyDch1GemResid);
        matchResid.Add(fhXdXDch1GemResid);
        matchResid.Add(fhYdYDch1GemResid);
        matchResid.Add(fhTxdXDch1GemResid);
        matchResid.Add(fhTydYDch1GemResid);

        matchResid.Add(fhXDch2GemResid);
        matchResid.Add(fhYDch2GemResid);
        matchResid.Add(fhTxDch2GemResid);
        matchResid.Add(fhTyDch2GemResid);
        matchResid.Add(fhXdXDch2GemResid);
        matchResid.Add(fhYdYDch2GemResid);
        matchResid.Add(fhTxdXDch2GemResid);
        matchResid.Add(fhTydYDch2GemResid);

        for (Int_t i = 0; i < 6; ++i) {
            fhdXGemSt[i]->Fit("gaus", "WWSQR", "", -0.1, 0.1);
            fhdYGemSt[i]->Fit("gaus", "WWSQR", "", -0.1, 0.1);
            matchResid.Add(fhdXGemSt[i]);
            matchResid.Add(fhdYGemSt[i]);
            matchResid.Add(fhXdXGemSt[i]);
            matchResid.Add(fhYdYGemSt[i]);
        }

        matchResid.Write();
        matchResid.Close();
    }

    cout << "Work time of the Global matching: " << workTime << endl;
}

void BmnGlobalTracking::CalculateLength() {
    //    if (fGlobalTracks == nullptr) return;
    //
    //    /* Calculate the length of the global track
    //     * starting with (0, 0, 0) and adding all
    //     * distances between hits
    //     */
    //    for (Int_t iTr = 0; iTr < fGlobalTracks->GetEntriesFast(); iTr++) {
    //        BmnGlobalTrack* glTr = (BmnGlobalTrack*) fGlobalTracks->At(iTr);
    //        vector<Float_t> X, Y, Z;
    //        X.push_back(0.);
    //        Y.push_back(0.);
    //        Z.push_back(0.);
    //
    //        if (fDet.GetDet(kTOF1)) {
    //            if (glTr->GetTof1HitIndex() > -1 && fTof1Hits) {
    //                const BmnHit* hit = (BmnHit*) fTof1Hits->At(glTr->GetTof1HitIndex());
    //                if (!hit) continue;
    //                X.push_back(hit->GetX());
    //                Y.push_back(hit->GetY());
    //                Z.push_back(hit->GetZ());
    //            }
    //        }
    //        if (fDet.GetDet(kDCH)) {
    //            //            if (glTr->GetDch1HitIndex() > -1 && fDch1Hits) {
    //            //                const BmnHit* hit = (BmnHit*) fDch1Hits->At(glTr->GetDch1HitIndex());
    //            //                if (!hit) continue;
    //            //                X.push_back(hit->GetX());
    //            //                Y.push_back(hit->GetY());
    //            //                Z.push_back(hit->GetZ());
    //            //            }
    //        }
    //        if (fDet.GetDet(kTOF)) {
    //            if (glTr->GetTof2HitIndex() > -1 && fTof2Hits) {
    //                const BmnHit* hit = (BmnHit*) fTof2Hits->At(glTr->GetTof2HitIndex());
    //                if (!hit) continue;
    //                X.push_back(hit->GetX());
    //                Y.push_back(hit->GetY());
    //                Z.push_back(hit->GetZ());
    //            }
    //        }
    //        // Calculate distances between hits
    //        Float_t length = 0.;
    //        for (Int_t i = 0; i < X.size() - 1; i++) {
    //            Float_t dX = X[i] - X[i + 1];
    //            Float_t dY = Y[i] - Y[i + 1];
    //            Float_t dZ = Z[i] - Z[i + 1];
    //            length += Sqrt(dX * dX + dY * dY + dZ * dZ);
    //        }
    //        glTr->SetLength(length);
    //    }
}

ClassImp(BmnGlobalTracking);
