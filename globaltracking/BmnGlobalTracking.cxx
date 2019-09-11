
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
        fhXCscGemResid = new TH1F("fhXCscGemResid", "fhXCscGemResid", 300, -100.0, 100.0);
        fhYCscGemResid = new TH1F("fhYCscGemResid", "fhYCscGemResid", 300, -100.0, 100.0);
        fhXdXCscGemResid = new TH2F("fhXdXCscGemResid", "fhXdXCscGemResid", 200, -100, 100, 200, -100.0, 100.0);
        fhYdYCscGemResid = new TH2F("fhYdYCscGemResid", "fhYdYCscGemResid", 200, -100, 100, 200, -100.0, 100.0);
        fhTxdXCscGemResid = new TH2F("fhTxdXCscGemResid", "fhTxdXCscGemResid", 200, -0.5, 0.5, 200, -100.0, 100.0);
        fhTydYCscGemResid = new TH2F("fhTydYCscGemResid", "fhTydYCscGemResid", 200, -0.5, 0.5, 200, -100.0, 100.0);

        fhXSiGemResid = new TH1F("fhXSiGemResid", "fhXSiGemResid", 500, -10.0, 10.0);
        fhYSiGemResid = new TH1F("fhYSiGemResid", "fhYSiGemResid", 500, -10.0, 10.0);
        fhTxSiGemResid = new TH1F("fhTxSiGemResid", "fhTxSiGemResid", 500, -0.1, 0.1);
        fhTySiGemResid = new TH1F("fhTySiGemResid", "fhTySiGemResid", 500, -0.1, 0.1);
        fhXdXSiGemResid = new TH2F("fhXdXSiGemResid", "fhXdXSiGemResid", 200, -10, 10, 200, -10.0, 10.0);
        fhYdYSiGemResid = new TH2F("fhYdYSiGemResid", "fhYdYSiGemResid", 200, -10, 10, 200, -10.0, 10.0);
        fhTxdXSiGemResid = new TH2F("fhTxdXSiGemResid", "fhTxdXSiGemResid", 200, -0.1, 0.1, 200, -10.0, 10.0);
        fhTydYSiGemResid = new TH2F("fhTydYSiGemResid", "fhTydYSiGemResid", 200, -0.1, 0.1, 200, -10.0, 10.0);

        fhXTof1GemResid = new TH1F("fhXTof1GemResid", "fhXTof1GemResid", 500, -100.0, 100.0);
        fhYTof1GemResid = new TH1F("fhYTof1GemResid", "fhYTof1GemResid", 500, -100.0, 100.0);
        fhXdXTof1GemResid = new TH2F("fhXdXTof1GemResid", "fhXdXTof1GemResid", 200, -100, 100, 200, -100.0, 100.0);
        fhYdYTof1GemResid = new TH2F("fhYdYTof1GemResid", "fhYdYTof1GemResid", 200, -100, 100, 200, -100.0, 100.0);
        fhTxdXTof1GemResid = new TH2F("fhTxdXTof1GemResid", "fhTxdXTof1GemResid", 200, -0.5, 0.5, 200, -100.0, 100.0);
        fhTydYTof1GemResid = new TH2F("fhTydYTof1GemResid", "fhTydYTof1GemResid", 200, -0.5, 0.5, 200, -100.0, 100.0);

        fhXTof2GemResid = new TH1F("fhXTof2GemResid", "fhXTof2GemResid", 500, -100.0, 100.0);
        fhYTof2GemResid = new TH1F("fhYTof2GemResid", "fhYTof2GemResid", 500, -100.0, 100.0);
        fhXdXTof2GemResid = new TH2F("fhXdXTof2GemResid", "fhXdXTof2GemResid", 200, -100, 100, 200, -100.0, 100.0);
        fhYdYTof2GemResid = new TH2F("fhYdYTof2GemResid", "fhYdYTof2GemResid", 200, -100, 100, 200, -100.0, 100.0);
        fhTxdXTof2GemResid = new TH2F("fhTxdXTof2GemResid", "fhTxdXTof2GemResid", 200, -0.5, 0.5, 200, -100.0, 100.0);
        fhTydYTof2GemResid = new TH2F("fhTydYTof2GemResid", "fhTydYTof2GemResid", 200, -0.5, 0.5, 200, -100.0, 100.0);

        fhXDch1GemResid = new TH1F("fhXDch1GemResid", "fhXDch1GemResid", 500, -100.0, 100.0);
        fhYDch1GemResid = new TH1F("fhYDch1GemResid", "fhYDch1GemResid", 500, -100.0, 100.0);
        fhTxDch1GemResid = new TH1F("fhTxDch1GemResid", "fhTxDch1GemResid", 500, -0.5, 0.5);
        fhTyDch1GemResid = new TH1F("fhTyDch1GemResid", "fhTyDch1GemResid", 500, -0.5, 0.5);
        fhXdXDch1GemResid = new TH2F("fhXdXDch1GemResid", "fhXdXDch1GemResid", 200, -100, 100, 200, -100.0, 100.0);
        fhYdYDch1GemResid = new TH2F("fhYdYDch1GemResid", "fhYdYDch1GemResid", 200, -100, 100, 200, -100.0, 100.0);
        fhTxdXDch1GemResid = new TH2F("fhTxdXDch1GemResid", "fhTxdXDch1GemResid", 200, -0.5, 0.5, 200, -100.0, 100.0);
        fhTydYDch1GemResid = new TH2F("fhTydYDch1GemResid", "fhTydYDch1GemResid", 200, -0.5, 0.5, 200, -100.0, 100.0);

        fhXDch2GemResid = new TH1F("fhXDch2GemResid", "fhXDch2GemResid", 500, -100.0, 100.0);
        fhYDch2GemResid = new TH1F("fhYDch2GemResid", "fhYDch2GemResid", 500, -100.0, 100.0);
        fhTxDch2GemResid = new TH1F("fhTxDch2GemResid", "fhTxDch2GemResid", 500, -0.5, 0.5);
        fhTyDch2GemResid = new TH1F("fhTyDch2GemResid", "fhTyDch2GemResid", 500, -0.5, 0.5);
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
            fhdXGemSt[i] = new TH1F(str1, str1, 200, -1.0, 1.0);
            fhdYGemSt[i] = new TH1F(str2, str2, 200, -1.0, 1.0);
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
    fTof1Hits = (TClonesArray *)ioman->GetObject("BmnTof1Hit");
    fTof2Hits = (TClonesArray *)ioman->GetObject("BmnTofHit");

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

    //Alignment. FIXME: move to DB
    if (fIsExp) {
        if (fIsSRC) {
            if (fDchTracks)
                for (Int_t trIdx = 0; trIdx < fDchTracks->GetEntriesFast(); ++trIdx) {
                    BmnTrack *dchTr = (BmnTrack *)fDchTracks->At(trIdx);
                    FairTrackParam* parDch = dchTr->GetParamFirst();
                    Double_t zDCH = parDch->GetZ();
                    if (zDCH < 550) {
                        parDch->SetTx(parDch->GetTx() + 0.072);
                        parDch->SetTy(parDch->GetTy() + 0.06);
                        parDch->SetX(parDch->GetX() - 9.92);
                        parDch->SetY(parDch->GetY() - 3.34);
                    }
                    if (zDCH > 650) {
                        parDch->SetTx(parDch->GetTx() + 0.073);
                        parDch->SetTy(parDch->GetTy() + 0.071);
                        parDch->SetX(parDch->GetX() - 4.40);
                        parDch->SetY(parDch->GetY() - 3.77);
                    }
                }
            if (fCscHits)
                for (Int_t hitIdx = 0; hitIdx < fCscHits->GetEntriesFast(); ++hitIdx) {
                    BmnHit *hit = (BmnHit *)fCscHits->At(hitIdx);
                    hit->SetX(hit->GetX() - 67.71);
                    hit->SetY(hit->GetY() + 0.17);
                }
            if (fTof2Hits)
                for (Int_t hitIdx = 0; hitIdx < fTof2Hits->GetEntriesFast(); ++hitIdx) {
                    BmnHit *hit = (BmnHit *)fTof2Hits->At(hitIdx);
                    hit->SetX(hit->GetX() - 0.04);
                    hit->SetY(hit->GetY() - 5.57);
                }
        } else {
            if (fCscHits)
                for (Int_t hitIdx = 0; hitIdx < fCscHits->GetEntriesFast(); ++hitIdx) {
                    BmnHit *hit = (BmnHit *)fCscHits->At(hitIdx);
                    hit->SetX(hit->GetX() - 67.71);
                    hit->SetY(hit->GetY() + 0.44);
                }
            if (fTof1Hits)
                for (Int_t hitIdx = 0; hitIdx < fTof1Hits->GetEntriesFast(); ++hitIdx) {
                    BmnHit *hit = (BmnHit *)fTof1Hits->At(hitIdx);
                    hit->SetX(hit->GetX());
                    hit->SetY(hit->GetY());
                }
            if (fTof2Hits)
                for (Int_t hitIdx = 0; hitIdx < fTof2Hits->GetEntriesFast(); ++hitIdx) {
                    BmnHit *hit = (BmnHit *)fTof2Hits->At(hitIdx);
                    hit->SetX(hit->GetX());
                    hit->SetY(hit->GetY() - 5.78);
                }
        }
    }

    //if (fInnerTracks->GetEntriesFast() != 1) return;

    for (Int_t i = 0; i < fInnerTracks->GetEntriesFast(); ++i) {
        BmnGlobalTrack *glTrack = (BmnGlobalTrack *)fInnerTracks->At(i);

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

        //Downstream
        if (fIsSRC) MatchingSil(glTrack);
        MatchingMWPC(glTrack);

        //Upstream
        MatchingCSC(glTrack);
        if (!fIsSRC) MatchingTOF(glTrack, 1);
        MatchingDCH(glTrack, 1);
        MatchingTOF(glTrack, 2);
        MatchingDCH(glTrack, 2);

        //Refit(glTr);
        CalcdQdn(glTrack);
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

BmnStatus BmnGlobalTracking::MatchingCSC(BmnGlobalTrack *tr) {
    if (!fCscHits) return kBMNERROR;

    Double_t minChi = DBL_MAX;
    Double_t chiCut = 1000.0;
    Int_t matchedIdx = -1;

    for (Int_t hitIdx = 0; hitIdx < fCscHits->GetEntriesFast(); ++hitIdx) {
        BmnHit *hit = (BmnHit *)fCscHits->At(hitIdx);
        FairTrackParam par(*(tr->GetParamLast()));
        if (fKalman->TGeoTrackPropagate(&par, hit->GetZ(), fPDG, nullptr, nullptr, fIsField) == kBMNERROR)
            continue;
        Double_t chi = 0;
        fKalman->Update(&par, hit, chi);
        if (chi < chiCut && chi < minChi) {
            minChi = chi;
            matchedIdx = hitIdx;
        }
        if (fDoAlign) {
            // if (Abs(par.GetY() - hit->GetY()) < 3) {
            fhXCscGemResid->Fill(par.GetX() - hit->GetX());
            fhYCscGemResid->Fill(par.GetY() - hit->GetY());
            fhXdXCscGemResid->Fill(par.GetX(), par.GetX() - hit->GetX());
            fhYdYCscGemResid->Fill(par.GetY(), par.GetY() - hit->GetY());
            fhTxdXCscGemResid->Fill(par.GetTx(), par.GetX() - hit->GetX());
            fhTydYCscGemResid->Fill(par.GetTy(), par.GetY() - hit->GetY());
            // }
        }
    }

    if (matchedIdx != -1) {
        BmnHit *cscHit = (BmnHit *)fCscHits->At(matchedIdx);

        FairTrackParam par(*(tr->GetParamLast()));
        Double_t len = tr->GetLength();
        if (fKalman->TGeoTrackPropagate(&par, cscHit->GetZ(), fPDG, nullptr, &len, fIsField) != kBMNERROR) {
            Double_t chi = 0;
            fKalman->Update(&par, cscHit, chi);
            tr->SetChi2(tr->GetChi2() + chi);
            tr->SetCscHitIndex(matchedIdx);
            tr->SetNHits(tr->GetNHits() + 1);
            tr->SetLength(len);
            tr->SetParamLast(par);
            return kBMNSUCCESS;
        } else
            return kBMNERROR;
    } else
        return kBMNERROR;
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

BmnStatus BmnGlobalTracking::MatchingTOF(BmnGlobalTrack *tr, Int_t num) {
    TClonesArray *tofHits = (num == 1) ? fTof1Hits : (num == 2) ? fTof2Hits : nullptr;
    if (!tofHits) return kBMNERROR;

    Double_t minDist = DBL_MAX;
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
        Double_t dist = Sqrt(Sq(parPredict.GetX() - hit->GetX()) + Sq(parPredict.GetY() - hit->GetY()));
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
        if (dist < minDist && dist <= 5.) {
            minDist = dist;
            minHit = hit;
            minParPredLast = parPredict;
            minIdx = hitIdx;
            LenPropLast = len;
        }
    }

    if (minHit == nullptr)
        return kBMNERROR;

    FairTrackParam ParPredFirst(*(tr->GetParamFirst()));
    FairTrackParam ParPredLast(*(tr->GetParamLast()));
    ParPredFirst.SetQp(ParPredLast.GetQp());                                    //FIXME: why do we need it?
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
    return kBMNSUCCESS;
}

BmnStatus BmnGlobalTracking::MatchingSil(BmnGlobalTrack *glTr) {
    //we need this function only for SRC
    //In BM@N we use silicon and GEM hits as a whole
    if (!fSilHits) return kBMNERROR;
}

BmnStatus BmnGlobalTracking::MatchingDCH(BmnGlobalTrack *tr, Int_t num) {
    if (!fDchTracks) return kBMNERROR;

    Double_t threshDist = 5;
    Double_t minDist = DBL_MAX;
    Double_t minDX = DBL_MAX;
    Double_t minDY = DBL_MAX;
    BmnTrack *matchedDch = nullptr;
    Int_t minIdx = -1;

    //residuals after peak fitting of all-to-all histograms
    Double_t sigmaXdch1gemResid = 1.67;
    Double_t sigmaYdch1gemResid = 0.72;
    Double_t sigmaXdch2gemResid = 0.69;
    Double_t sigmaYdch2gemResid = 0.65;

    Double_t Z = (num == 1) ? 500 : 700;
    FairTrackParam parL(*(tr->GetParamLast()));
    fKalman->TGeoTrackPropagate(&parL, Z, fPDG, nullptr, nullptr, fIsField);

    for (Int_t trIdx = 0; trIdx < fDchTracks->GetEntriesFast(); ++trIdx) {
        BmnTrack *dchTr = (BmnTrack *)fDchTracks->At(trIdx);
        FairTrackParam parDch(*(dchTr->GetParamFirst()));
        Double_t zDCH = parDch.GetZ();
        if (num == 1 && zDCH > 550) continue;
        if (num == 2 && zDCH < 650) continue;
        fKalman->TGeoTrackPropagate(&parDch, Z, fPDG, nullptr, nullptr, kFALSE);
        Double_t dX = Abs(parL.GetX() - parDch.GetX());
        Double_t dY = Abs(parL.GetY() - parDch.GetY());
        Double_t xCut = (num == 1) ? 3 * sigmaXdch1gemResid : 3 * sigmaXdch2gemResid;
        Double_t yCut = (num == 1) ? 3 * sigmaYdch1gemResid : 3 * sigmaYdch2gemResid;
        if (dX < xCut && dY < yCut && dX < minDX && dY < minDY) {
            minIdx = trIdx;
            matchedDch = dchTr;
            minDX = dX;
            minDY = dY;
        }
        if (fDoAlign)
            if (num == 1) {
                fhXDch1GemResid->Fill(parL.GetX() - parDch.GetX());
                fhYDch1GemResid->Fill(parL.GetY() - parDch.GetY());
                fhTxDch1GemResid->Fill(parL.GetTx() - parDch.GetTx());
                fhTyDch1GemResid->Fill(parL.GetTy() - parDch.GetTy());
                fhXdXDch1GemResid->Fill(parL.GetX(), parL.GetX() - parDch.GetX());
                fhYdYDch1GemResid->Fill(parL.GetY(), parL.GetY() - parDch.GetY());
                fhTxdXDch1GemResid->Fill(parL.GetTx(), parL.GetX() - parDch.GetX());
                fhTydYDch1GemResid->Fill(parL.GetTy(), parL.GetY() - parDch.GetY());
            } else {
                fhXDch2GemResid->Fill(parL.GetX() - parDch.GetX());
                fhYDch2GemResid->Fill(parL.GetY() - parDch.GetY());
                fhTxDch2GemResid->Fill(parL.GetTx() - parDch.GetTx());
                fhTyDch2GemResid->Fill(parL.GetTy() - parDch.GetTy());
                fhXdXDch2GemResid->Fill(parL.GetX(), parL.GetX() - parDch.GetX());
                fhYdYDch2GemResid->Fill(parL.GetY(), parL.GetY() - parDch.GetY());
                fhTxdXDch2GemResid->Fill(parL.GetTx(), parL.GetX() - parDch.GetX());
                fhTydYDch2GemResid->Fill(parL.GetTy(), parL.GetY() - parDch.GetY());
            }
    }

    if (minIdx != -1) {
        FairTrackParam *parDch = matchedDch->GetParamFirst();
        FairTrackParam par(*(tr->GetParamLast()));
        Double_t len = tr->GetLength();
        BmnHit *tmpHit = new BmnHit(-1, TVector3(parDch->GetX(), parDch->GetY(), parDch->GetZ()), TVector3(0.02, 0.02, 0.02), -1);
        if (fKalman->TGeoTrackPropagate(&par, tmpHit->GetZ(), fPDG, nullptr, &len, fIsField) != kBMNERROR) {
            Double_t chi = 0;
            fKalman->Update(&par, tmpHit, chi);
            tr->SetChi2(tr->GetChi2() + chi);
            tr->SetNHits(tr->GetNHits() + matchedDch->GetNHits());
            tr->SetLength(len);
            tr->SetParamLast(par);

            if (num == 1)
                tr->SetDch1TrackIndex(minIdx);
            else
                tr->SetDch2TrackIndex(minIdx);
        }
    }
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
