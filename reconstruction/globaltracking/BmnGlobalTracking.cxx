/** BmnGlobalTracking.cxx
 * \author Sergey Merts <Sergey.Merts@gmail.com>
 * @since 2014
 * @version 1.0
 **/

#include "BmnGlobalTracking.h"
#include "BmnEventHeader.h"
#include "BmnMwpcGeometry.h"
#include "BmnSiliconTrack.h"
#include "BmnGemStripHit.h"
#include "BmnFieldMap.h"

#include "TVectorD.h"
#include "TFile.h"

//#include <Fit/FitResult.h>

#include <iterator>
#include <map>
#include <algorithm>
#include <vector>
#include "omp.h"

using namespace TMath;

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
    fUpsHits(nullptr),
    fMCTracks(nullptr),
    fEvHead(nullptr),
    fUpstreamTracks(nullptr),
    fPDG(2212),
    fTime(0.0),
    fChiSqCut(100.),
    fVertex(nullptr),
    fVertexL1(nullptr),
    fIsSRC(kFALSE),
    fKalman(nullptr),
    fNMatchedDch1(0),
    fNMatchedDch2(0),
    fNMatchedTof400(0),
    fNMatchedTof700(0),
    fNMatchedNearCsc(0),
    fNMatchedFarCsc(0),
    fNInnerTracks(0),
    fNGoodInnerTracks(0),
    fNGoodGlobalTracks(0),
    fInnerTrackBranchName("StsTrack"),
    fEventNo(0)
{}

BmnGlobalTracking::BmnGlobalTracking(Bool_t isExp, Bool_t doAlign) : fInnerTracks(nullptr),
    fSiliconTracks(nullptr),
    fGemHits(nullptr),
    fCscHits(nullptr),
    fGemTracks(nullptr),
    fGemVertex(nullptr),
    fTof1Hits(nullptr),
    fTof2Hits(nullptr),
    fDchHits(nullptr),
    fUpsHits(nullptr),
    fMCTracks(nullptr),
    fEvHead(nullptr),
    fUpstreamTracks(nullptr),
    fPDG(2212),
    fTime(0.0),
    fChiSqCut(100.),
    fVertex(nullptr),
    fVertexL1(nullptr),
    fPeriod(7),
    fIsSRC(kFALSE),
    fDoAlign(doAlign),
    fNMatchedDch1(0),
    fNMatchedDch2(0),
    fNMatchedTof400(0),
    fNMatchedTof700(0),
    fNMatchedNearCsc(0),
    fNMatchedFarCsc(0),
    fNInnerTracks(0),
    fNGoodInnerTracks(0),
    fNGoodGlobalTracks(0),
    fIsExp(isExp),
    fInnerTrackBranchName("StsTrack"),
    fEventNo(0)
{}

BmnGlobalTracking::BmnGlobalTracking(Bool_t isField, Bool_t isExp, Bool_t doAlign) : fInnerTracks(nullptr),
    fSiliconTracks(nullptr),
    fGemHits(nullptr),
    fCscHits(nullptr),
    fGemTracks(nullptr),
    fGemVertex(nullptr),
    fTof1Hits(nullptr),
    fTof2Hits(nullptr),
    fDchHits(nullptr),
    fUpsHits(nullptr),
    fMCTracks(nullptr),
    fEvHead(nullptr),
    fUpstreamTracks(nullptr),
    fPDG(2212),
    fTime(0.0),
    fChiSqCut(100.),
    fVertex(nullptr),
    fVertexL1(nullptr),
    fPeriod(7),
    fIsSRC(kFALSE),
    fDoAlign(doAlign),
    fNMatchedDch1(0),
    fNMatchedDch2(0),
    fNMatchedTof400(0),
    fNMatchedTof700(0),
    fNMatchedNearCsc(0),
    fNMatchedFarCsc(0),
    fNInnerTracks(0),
    fNGoodInnerTracks(0),
    fNGoodGlobalTracks(0),
    fIsExp(isExp),
    fInnerTrackBranchName("StsTrack"),
    fEventNo(0)
{}

BmnGlobalTracking::~BmnGlobalTracking() {}

InitStatus BmnGlobalTracking::Init()
{
    if (fVerbose > 1)
        cout << "BmnGlobalTracking::Init started\n";

    fKalman = new BmnKalmanFilter();

    FairRootManager* ioman = FairRootManager::Instance();
    if (!ioman)
        Fatal("Init", "FairRootManager is not instantiated");

    fSilHits = (TClonesArray*)ioman->GetObject("BmnSiliconHit");
    fGemHits = (TClonesArray*)ioman->GetObject("BmnGemStripHit");
    fCscHits = (TClonesArray*)ioman->GetObject("BmnCSCHit");
    fTof1Hits = (TClonesArray*)ioman->GetObject("BmnTof400Hit");
    fTof2Hits = (TClonesArray*)ioman->GetObject("BmnTof700Hit");

    fVertexL1 = (CbmVertex*)ioman->GetObject("PrimaryVertex.");

    fStsTracks = (TClonesArray*)ioman->GetObject(fInnerTrackBranchName);
    fStsHits = (TClonesArray*)ioman->GetObject("StsHit");
    if (fStsTracks) {
        printf("BmnGlobalTracking::Init(): branch %s was found!\n", fInnerTrackBranchName.Data());
        fGlobalTracks = new TClonesArray("BmnGlobalTrack", 100);  //out
        ioman->Register("BmnGlobalTrack", "GLOBAL", fGlobalTracks, kTRUE);
    } else {
        fInnerTracks = (TClonesArray*)ioman->GetObject("BmnGlobalTrack");
        fGemTracks = (TClonesArray*)ioman->GetObject("BmnGemTrack");
        if (!fInnerTracks) {
            cout << "BmnGlobalTracking::Init(): branch BmnInnerTrack not found! Task will be deactivated" << endl;
            SetActive(kFALSE);
            return kERROR;
        } else {
            printf("BmnGlobalTracking::Init(): branch BmnInnerTrack was found! Global tracks will be based on BMN tracks \n");
        }
    }

    fSiliconTracks = (TClonesArray*)ioman->GetObject("BmnSiliconTrack");
    if (!fSiliconTracks)
        cout << "BmnGlobalTracking::Init(): branch BmnSiliconTrack not found!" << endl;

    fMwpcTracks = (TClonesArray*)ioman->GetObject("BmnMwpcTrack");
    if (!fMwpcTracks)
        cout << "BmnGlobalTracking::Init(): branch BmnMwpcTrack not found!" << endl;

    fDchTracks = (TClonesArray*)ioman->GetObject("BmnDchTrack");
    if (!fDchTracks)
        cout << "BmnGlobalTracking::Init(): branch BmnDchTrack not found!" << endl;

    if (fIsExp) {                                    //In case of exp data we create artificial hits
        fDchHits = new TClonesArray("BmnHit", 100);  //out
        ioman->Register("BmnDchHit", "DCH", fDchHits, kTRUE);
    } else {  //In case of MC data we get hits from indput tree
        fDchHits = (TClonesArray*)ioman->GetObject("BmnDchHit");
    }
    if (fIsSRC) {
        fUpstreamTracks = (TClonesArray*)ioman->GetObject("BmnUpstreamTrack");
        if (!fUpstreamTracks)
            cout << "BmnGlobalTracking::Init(): branch BmnUpstreamTrack not found!" << endl;
        else {
            fUpsHits = new TClonesArray("BmnHit", 100);  //out
            ioman->Register("BmnUpstreamHit", "UPSTREAM", fUpsHits, kTRUE);
        }
    }

    fEvHead = (TClonesArray*)ioman->GetObject("EventHeader");
    if (!fEvHead)
        if (fVerbose > 1)
            cout << "Init. No EventHeader array!" << endl;

    if (fVerbose > 1)
        cout << "BmnGlobalTracking::Init finished\n";
    return kSUCCESS;
}

void BmnGlobalTracking::Exec(Option_t* opt) {
    TStopwatch sw;
    sw.Start();

    if (!IsActive())
        return;

    if (fVerbose > 1)
        cout << "\n======================== Global tracking exec started =====================\n" << endl;
    fEventNo++;
    if (fStsTracks)
        fGlobalTracks->Delete();
    //if (!fInnerTracks) return;
    if (fIsExp) {
        if (fDchHits) fDchHits->Delete();
    }
    if (fUpsHits) fUpsHits->Delete();

    //Alignment. FIXME: move to DB

    if (fIsExp) { //for run-7
        if (fDchTracks) {
            Double_t dchTxCorr = (fIsSRC) ? +0.001 : +0.006;
            Double_t dchTyCorr = (fIsSRC) ? -0.001 : -0.0003;
            Double_t dchXCorr = (fIsSRC) ? -8.52 : -6.97;
            Double_t dchYCorr = (fIsSRC) ? -3.01 : -2.92;
            for (Int_t trIdx = 0; trIdx < fDchTracks->GetEntriesFast(); ++trIdx) {
                BmnTrack* dchTr = (BmnTrack*)fDchTracks->At(trIdx);
                FairTrackParam* parDch = dchTr->GetParamFirst();
                Double_t zDCH = parDch->GetZ();
                if (zDCH < 550) {         //dch1
                } else if (zDCH > 650) {  //dch2
                } else {                  //global dch
                    parDch->SetTx(parDch->GetTx() + dchTxCorr);
                    parDch->SetTy(parDch->GetTy() + dchTyCorr);
                    parDch->SetX(parDch->GetX() + dchXCorr);
                    parDch->SetY(parDch->GetY() + dchYCorr);
                }
            }
        }
        if (fCscHits) {
            Double_t cscXCorr = (fIsSRC) ? -15.08 : +0.87;
            Double_t cscYCorr = (fIsSRC) ? -5.83 : -0.12;
            for (Int_t hitIdx = 0; hitIdx < fCscHits->GetEntriesFast(); ++hitIdx) {
                BmnHit* hit = (BmnHit*)fCscHits->At(hitIdx);
                hit->SetX(hit->GetX() + cscXCorr);
                hit->SetY(hit->GetY() + cscYCorr);
            }
        }
        if (fTof1Hits) {
            Double_t tof400XCorr = (fIsSRC) ? +0.00 : -2.03;
            Double_t tof400YCorr = (fIsSRC) ? +0.00 : +0.60;
            for (Int_t hitIdx = 0; hitIdx < fTof1Hits->GetEntriesFast(); ++hitIdx) {
                BmnHit* hit = (BmnHit*)fTof1Hits->At(hitIdx);
                hit->SetX(hit->GetX() + tof400XCorr);
                hit->SetY(hit->GetY() + tof400YCorr);
            }
        }
        if (fTof2Hits) {
            Double_t tof700XCorr = (fIsSRC) ? +1.26 : +2.00;
            Double_t tof700YCorr = (fIsSRC) ? -9.95 : -5.74;
            for (Int_t hitIdx = 0; hitIdx < fTof2Hits->GetEntriesFast(); ++hitIdx) {
                BmnHit* hit = (BmnHit*)fTof2Hits->At(hitIdx);
                hit->SetX(hit->GetX() + tof700XCorr);
                hit->SetY(hit->GetY() + tof700YCorr);
            }
        }

        if (fIsSRC && fUpstreamTracks)
            for (Int_t trIdx = 0; trIdx < fUpstreamTracks->GetEntriesFast(); ++trIdx) {

                BmnTrack* upTr = (BmnTrack*)fUpstreamTracks->At(trIdx);
                FairTrackParam* parUp = upTr->GetParamLast();
                if (fIsExp) {
                    parUp->SetX(parUp->GetX() - 0.81);     //- 0.93
                    parUp->SetY(parUp->GetY() - 0.83);     //+ 0.3
                    parUp->SetTx(parUp->GetTx() + 0.002);  //+ 0.00265
                    parUp->SetTy(parUp->GetTy() + 0.000);  //+ 0.00060
                }
            }
    }

    if (fStsTracks) { //for run-8
        for (Int_t i = 0; i < fStsTracks->GetEntriesFast(); ++i) {
            CbmStsTrack* cbmTrack = (CbmStsTrack*)fStsTracks->At(i);

            fNInnerTracks++;

            if (cbmTrack->GetNStsHits() < 4) continue;

            BmnGlobalTrack globTr;

            globTr.SetGemTrackIndex(i);
            globTr.SetParamFirst(*(cbmTrack->GetParamFirst()));
            globTr.SetParamLast(*(cbmTrack->GetParamLast()));
            globTr.SetNHits(cbmTrack->GetNStsHits());
            globTr.SetNDF(cbmTrack->GetNDF());
            globTr.SetChi2(cbmTrack->GetChi2());
            FairTrackParam par(*(globTr.GetParamLast()));
            fPDG = (globTr.GetP() > 0.) ? 2212 : -211;
            Double_t len = 0.0;
            Double_t zTarget = (fVertexL1) ? fVertexL1->GetZ() : 0.0;
            if (fKalman->TGeoTrackPropagate(&par, zTarget, fPDG, nullptr, &len) == kBMNERROR) continue;
            globTr.SetLength(len);

            fNGoodInnerTracks++;

            //Matching with Small CSC1
            vector<Int_t> nearCSCst = { 0, 1 };
            MatchingCSC(&globTr, nearCSCst);
            //Matching with TOF-400
            MatchingTOF(&globTr, 1);
            //Matching with Small CSC2
            vector<Int_t> farCSCst = { 2, 3 };
            MatchingCSC(&globTr, farCSCst);
            //Matching with DCH1
            MatchingDCH(&globTr, 1);
            //Matching with TOF-700
            MatchingTOF(&globTr, 2);
            //Matching with DCH2
            MatchingDCH(&globTr, 2);

            if (Refit(&globTr) == kBMNERROR) continue;

            fNGoodGlobalTracks++;
            new ((*fGlobalTracks)[fGlobalTracks->GetEntriesFast()]) BmnGlobalTrack(globTr);
        }
    } else if (fInnerTracks) {
        for (Int_t i = 0; i < fInnerTracks->GetEntriesFast(); ++i) {
            BmnGlobalTrack* glTrack = (BmnGlobalTrack*)fInnerTracks->At(i);

            //Downstream
            //Matching with Small CSC1
            vector<Int_t> nearCSCst = { 0, 1 };
            if (!fIsSRC) MatchingCSC(glTrack, nearCSCst);
            if (!fIsSRC) MatchingTOF(glTrack, 1);
            MatchingDCH(glTrack);
            MatchingTOF(glTrack, 2);

            //Upstream
            if (fIsSRC) MatchingUpstream(glTrack);

            Refit(glTrack);
            //cout << glTrack->GetP() << endl;
            if (fIsSRC) UpdateMomentum(glTrack);
            CalcdQdn(glTrack);

            // NDF = (N counts in ZX plane + N counts in ZY plane) - 2 parameters of Line in ZY plane - 3 parameters of Circle in ZX plane
            // Check it!!!
            glTrack->SetNDF(glTrack->GetNHits() * 2 - 5);
        }
    }

    sw.Stop();
    fTime += sw.RealTime();

    //if (fVerbose) cout << "GLOBAL_TRACKING: Number of merged tracks: " << fGlobalTracks->GetEntriesFast() << endl;
    if (fVerbose > 1)
        cout << "\n======================== Global tracking exec finished ====================\n" << endl;
}

void BmnGlobalTracking::CalcdQdn(BmnGlobalTrack* tr) {
    BmnGemTrack* gemTrack = (BmnGemTrack*)fGemTracks->At(tr->GetGemTrackIndex());
    Double_t totSigLow = 0.0;
    Double_t totSigUp = 0.0;
    for (Int_t hitIdx = 0; hitIdx < gemTrack->GetNHits(); ++hitIdx) {
        BmnGemStripHit* hit = (BmnGemStripHit*)fGemHits->At(gemTrack->GetHitIndex(hitIdx));
        totSigLow += hit->GetStripTotalSignalInLowerLayer();
        totSigUp += hit->GetStripTotalSignalInUpperLayer();
    }
    totSigLow /= gemTrack->GetNHits();
    totSigUp /= gemTrack->GetNHits();
    tr->SetdQdNLower(totSigLow);
    tr->SetdQdNUpper(totSigUp);
}

Int_t BmnGlobalTracking::FindNearestHit(FairTrackParam* par, TClonesArray* hits, Float_t xCut, Float_t yCut) {
    if (!hits || !par)
        return -1;

    Double_t minDX = DBL_MAX;
    Double_t minDY = DBL_MAX;
    Int_t minIdx = -1;
    BmnHit* minHit = nullptr;

    Double_t minZ = 10000.0;
    for (Int_t hitIdx = 0; hitIdx < hits->GetEntriesFast(); ++hitIdx) {
        BmnHit* hit = (BmnHit*)hits->At(hitIdx);
        if (hit->GetZ() < minZ) minZ = hit->GetZ();
    }

    FairTrackParam parMinZ(*par);
    if (fKalman->TGeoTrackPropagate(&parMinZ, minZ, fPDG, nullptr, nullptr) == kBMNERROR) return -1;

    for (Int_t hitIdx = 0; hitIdx < hits->GetEntriesFast(); ++hitIdx) {
        BmnHit* hit = (BmnHit*)hits->At(hitIdx);

        FairTrackParam param = parMinZ;
        if (fKalman->TGeoTrackPropagate(&param, hit->GetZ(), fPDG, nullptr, nullptr) == kBMNERROR)
            continue;

        Float_t dX = param.GetX() - hit->GetX();
        Float_t dY = param.GetY() - hit->GetY();

        if (Abs(dX) < xCut && Abs(dY) < yCut && Abs(dX) < Abs(minDX) && Abs(dY) < Abs(minDY)) {
            minDX = dX;
            minDY = dY;
            minIdx = hitIdx;
            minHit = hit;
        }
    }
    if (!minHit) return -1;

    minHit->SetResXY(minDX, minDY);

    return minIdx;
}

Int_t BmnGlobalTracking::FindNearestHit(FairTrackParam* par, TClonesArray* hits, Float_t xCut, Float_t yCut, vector<Int_t> stations) {
    if (!hits || !par)
        return -1;

    Double_t minDX = DBL_MAX;
    Double_t minDY = DBL_MAX;
    Int_t minIdx = -1;
    BmnHit* minHit = nullptr;

    Double_t minZ = 10000.0;
    for (Int_t hitIdx = 0; hitIdx < hits->GetEntriesFast(); ++hitIdx) {
        BmnHit* hit = (BmnHit*)hits->At(hitIdx);
        Int_t st = hit->GetStation();
        if (st != stations[0] && st != stations[1]) continue;
        if (hit->GetZ() < minZ) minZ = hit->GetZ();
    }

    FairTrackParam parMinZ(*par);
    if (fKalman->TGeoTrackPropagate(&parMinZ, minZ, fPDG, nullptr, nullptr) == kBMNERROR) return -1;

    for (Int_t hitIdx = 0; hitIdx < hits->GetEntriesFast(); ++hitIdx) {
        BmnHit* hit = (BmnHit*)hits->At(hitIdx);

        Int_t st = hit->GetStation();
        if (st != stations[0] && st != stations[1]) continue;

        FairTrackParam param = parMinZ;
        if (fKalman->TGeoTrackPropagate(&param, hit->GetZ(), fPDG, nullptr, nullptr) == kBMNERROR)
            continue;

        Float_t dX = param.GetX() - hit->GetX();
        Float_t dY = param.GetY() - hit->GetY();

        if (Abs(dX) < xCut && Abs(dY) < yCut && Abs(dX) < Abs(minDX) && Abs(dY) < Abs(minDY)) {
            minDX = dX;
            minDY = dY;
            minIdx = hitIdx;
            minHit = hit;
        }
    }
    if (!minHit) return -1;

    minHit->SetResXY(minDX, minDY);

    return minIdx;
}

BmnStatus BmnGlobalTracking::MatchingCSC(BmnGlobalTrack* glTr, vector<Int_t> stations) {

    if (fVerbose) cout << "Matching of CSC started" << endl;
    if (!fCscHits) return kBMNERROR;

    fPDG = (glTr->GetP() > 0.) ? 2212 : -211;

    //residuals after peak fitting of all-to-all histograms
    Double_t sigmaXcscgemResid = 2.0;
    Double_t sigmaYcscgemResid = 2.0;
    Double_t xCut = 3 * sigmaXcscgemResid;
    Double_t yCut = 3 * sigmaYcscgemResid;

    Double_t minDX = DBL_MAX;
    Double_t minDY = DBL_MAX;
    Double_t dX = DBL_MAX;
    Double_t dY = DBL_MAX;

    Int_t minIdx = FindNearestHit(glTr->GetParamLast(), fCscHits, xCut, yCut, stations);
    if (minIdx == -1) return kBMNERROR;

    Int_t id = (stations[0] < 2) ? 0 : 1;
    glTr->AddCscHitIndex(id, minIdx);

    BmnHit* minHit = (BmnHit*)fCscHits->At(minIdx);

    Double_t len = 0.0;
    FairTrackParam par(*(glTr->GetParamLast()));
    if (fKalman->TGeoTrackPropagate(&par, minHit->GetZ(), fPDG, nullptr, &len) == kBMNERROR) return kBMNERROR;
    Double_t chi = 0;
    if (fKalman->Update(&par, minHit, chi) == kBMNERROR) return kBMNERROR;
    glTr->SetChi2(glTr->GetChi2() + chi);
    glTr->SetParamLast(par);
    len += glTr->GetLength();
    glTr->SetLength(len);
    minHit->SetUsing(kTRUE);
    glTr->SetNHits(glTr->GetNHits() + 1);
    if (id == 0) fNMatchedNearCsc++;
    else fNMatchedFarCsc++;
    return kBMNSUCCESS;
}

BmnStatus BmnGlobalTracking::MatchingTOF(BmnGlobalTrack* tr, Int_t num) {
    if (fVerbose) cout << "Matching of TOF " << num << " started" << endl;
    TClonesArray* tofHits = (num == 1) ? fTof1Hits : (num == 2) ? fTof2Hits : nullptr;
    if (!tofHits) return kBMNERROR;

    fPDG = (tr->GetP() > 0.) ? 2212 : -211;

    //residuals after peak fitting of all-to-all histograms
    Double_t sigmaXtof1gemResid = (fIsExp) ? 0.96 : 2.0;  //1000;
    Double_t sigmaYtof1gemResid = (fIsExp) ? 0.90 : 2.0;  //1000;
    Double_t sigmaXtof2gemResid = (fIsExp) ? 3.37 : 2.0;  //6.2;                                 //1.34;
    Double_t sigmaYtof2gemResid = (fIsExp) ? 1.01 : 2.0;  //1.38;
    Double_t xCut = (num == 1) ? 3 * sigmaXtof1gemResid : 3 * sigmaXtof2gemResid;
    Double_t yCut = (num == 1) ? 3 * sigmaYtof1gemResid : 3 * sigmaYtof2gemResid;

    Double_t minDX = DBL_MAX;
    Double_t minDY = DBL_MAX;
    Double_t dX = DBL_MAX;
    Double_t dY = DBL_MAX;

    Int_t minIdx = FindNearestHit(tr->GetParamLast(), tofHits, xCut, yCut);
    if (minIdx == -1) return kBMNERROR;

    if (num == 1)
        tr->SetTof1HitIndex(minIdx);
    else
        tr->SetTof2HitIndex(minIdx);

    BmnHit* minHit = (BmnHit*)tofHits->At(minIdx);

    Double_t len = 0.0;
    FairTrackParam par(*(tr->GetParamLast()));
    if (fKalman->TGeoTrackPropagate(&par, minHit->GetZ(), fPDG, nullptr, &len) == kBMNERROR) return kBMNERROR;
    Double_t chi = 0;
    if (fKalman->Update(&par, minHit, chi) == kBMNERROR) return kBMNERROR;
    tr->SetChi2(tr->GetChi2() + chi);
    tr->SetParamLast(par);
    //Double_t zTarget = (fVertex) ? fVertex->GetZ() : (fIsSRC) ? -647.5 : -2.3;  // z of target by default
    //fKalman->TGeoTrackPropagate(&par, zTarget, fPDG, nullptr, &len);

    len += tr->GetLength();

    tr->SetBeta(len / minHit->GetTimeStamp() / (TMath::C() * 1e-7), num);
    minHit->SetUsing(kTRUE);
    minHit->SetLength(len);  // length from target to Tof hit
    tr->SetNHits(tr->GetNHits() + 1);
    tr->SetLength(len);
    if (num == 1) fNMatchedTof400++;
    else fNMatchedTof700++;
    return kBMNSUCCESS;
}

BmnStatus BmnGlobalTracking::MatchingUpstream(BmnGlobalTrack* glTr) {
    if (fVerbose) cout << "Matching of UPSTREAM started" << endl;
    //we need this function only for SRC
    //In BM@N we use silicon and GEM hits as a whole
    if (!fUpstreamTracks) return kBMNERROR;
    if (fUpstreamTracks->GetEntriesFast() == 0) return kBMNERROR;
    fPDG = (glTr->GetP() > 0.) ? 2212 : -211;

    Double_t sigma = 2.0;
    Double_t xCut = 3 * sigma;
    Double_t yCut = 3 * sigma;

    BmnTrack* minTrack = nullptr;
    Int_t minTrackId = -1;
    Double_t minDX = DBL_MAX;
    Double_t minDY = DBL_MAX;

    for (Int_t iTr = 0; iTr < fUpstreamTracks->GetEntriesFast(); ++iTr) {
        BmnTrack* upsTr = (BmnTrack*)fUpstreamTracks->At(iTr);
        if (!upsTr) continue;
        FairTrackParam glPar(*(glTr->GetParamFirst()));
        FairTrackParam upsPar(*(upsTr->GetParamLast()));
        if (fKalman->TGeoTrackPropagate(&glPar, upsPar.GetZ(), fPDG, nullptr, nullptr) == kBMNERROR)
            continue;
        Double_t dX = glPar.GetX() - upsPar.GetX();
        Double_t dY = glPar.GetY() - upsPar.GetY();
        if (Abs(dX) < xCut && Abs(dY) < yCut && Abs(dX) < minDX && Abs(dY) < minDY) {
            minTrack = upsTr;
            minTrackId = iTr;
            minDX = dX;
            minDY = dY;
        }
    }

    if (minTrack == nullptr)
        return kBMNERROR;

    FairTrackParam glPar(*(glTr->GetParamFirst()));
    FairTrackParam upsPar(*(minTrack->GetParamLast()));
    Double_t len = glTr->GetLength();
    fKalman->TGeoTrackPropagate(&glPar, upsPar.GetZ(), fPDG, nullptr, &len);

    Double_t chi = 0;
    UpdateTrackParam(&glPar, &upsPar, chi);
    glTr->SetChi2(glTr->GetChi2() + chi);
    glTr->SetUpstreamTrackIndex(minTrackId);

    glTr->SetNHits(glTr->GetNHits() + minTrack->GetNHits());
    //glTr->SetLength(len);
    glTr->SetParamFirst(glPar);
    return kBMNSUCCESS;
}

BmnStatus BmnGlobalTracking::MatchingDCH(BmnGlobalTrack* tr) {

    if (fVerbose) cout << "Matching of DCH started" << endl;

    if (!fDchTracks) return kBMNERROR;
    if (fDchTracks->GetEntriesFast() == 0) return kBMNERROR;
    fPDG = (tr->GetP() > 0.) ? 2212 : -211;

    Double_t minDX = DBL_MAX;
    Double_t minDY = DBL_MAX;
    BmnTrack* minTrack = nullptr;
    Int_t minTrackId = -1;

    //residuals after peak fitting of all-to-all histograms
    Double_t sigmaXdchGgemResid = (fIsExp) ? 5.54 : 1;
    Double_t sigmaYdchGgemResid = (fIsExp) ? 2.33 : 1;
    Double_t xCut = 3 * sigmaXdchGgemResid;
    Double_t yCut = 3 * sigmaYdchGgemResid;

    for (Int_t iTr = 0; iTr < fDchTracks->GetEntriesFast(); ++iTr) {
        BmnTrack* dchTr = (BmnTrack*)fDchTracks->At(iTr);
        if (!dchTr) continue;
        if (dchTr->GetNHits() < 10) continue; //use only global DCH tracks
        FairTrackParam glPar(*(tr->GetParamLast()));
        FairTrackParam dchPar(*(dchTr->GetParamFirst()));
        if (fKalman->TGeoTrackPropagate(&glPar, dchPar.GetZ(), fPDG, nullptr, nullptr) == kBMNERROR)
            continue;
        //Double_t dist = Sqrt(Sq(par.GetX() - hit->GetX()) + Sq(par.GetY() - hit->GetY()));
        Double_t dX = glPar.GetX() - dchPar.GetX();
        Double_t dY = glPar.GetY() - dchPar.GetY();
        if (Abs(dX) < xCut && Abs(dY) < yCut && Abs(dX) < minDX && Abs(dY) < minDY) {
            minTrack = dchTr;
            minTrackId = iTr;
            minDX = dX;
            minDY = dY;
        }
    }

    if (minTrack == nullptr)
        return kBMNERROR;

    FairTrackParam glPar(*(tr->GetParamLast()));
    FairTrackParam dchPar(*(minTrack->GetParamFirst()));
    Double_t len = tr->GetLength();
    fKalman->TGeoTrackPropagate(&glPar, dchPar.GetZ(), fPDG, nullptr, &len);
    Double_t chi = 0;
    UpdateTrackParam(&glPar, &dchPar, chi);
    tr->SetChi2(tr->GetChi2() + chi);

    tr->SetDchTrackIndex(minTrackId);
    tr->SetNHits(tr->GetNHits() + minTrack->GetNHits());
    tr->SetParamLast(glPar);
    return kBMNSUCCESS;
}


BmnStatus BmnGlobalTracking::MatchingDCH(BmnGlobalTrack* tr, Int_t num) {

    if (fVerbose) cout << "Matching of DCH started" << endl;

    if (!fDchTracks) return kBMNERROR;
    if (fDchTracks->GetEntriesFast() == 0) return kBMNERROR;
    fPDG = (tr->GetP() > 0.) ? 2212 : -211;

    //residuals after peak fitting of all-to-all histograms
    Double_t sigmaXdchGgemResid = (fIsExp) ? 5.54 : 1;
    Double_t sigmaYdchGgemResid = (fIsExp) ? 2.33 : 1;
    Double_t xCut = 3 * sigmaXdchGgemResid;
    Double_t yCut = 3 * sigmaYdchGgemResid;

    //dch1 ~510, dch2 ~710, dchGlob ~610
    Double_t zDchTh1 = 550.0;
    Double_t zDchTh2 = 650.0;

    Double_t minZ = 10000.0;
    for (Int_t iTr = 0; iTr < fDchTracks->GetEntriesFast(); ++iTr) {
        BmnTrack* dchTr = (BmnTrack*)fDchTracks->At(iTr);
        Double_t trZ = dchTr->GetParamFirst()->GetZ();
        Bool_t ok = kFALSE;
        if (trZ < zDchTh1 && num == 1) ok = kTRUE;
        if (trZ > zDchTh2 && num == 2) ok = kTRUE;
        if (!ok) continue;
        if (trZ < minZ) minZ = trZ;
    }

    FairTrackParam parMinZ(*(tr->GetParamLast()));
    if (fKalman->TGeoTrackPropagate(&parMinZ, minZ, fPDG, nullptr, nullptr) == kBMNERROR) return kBMNERROR;

    Double_t minDX = DBL_MAX;
    Double_t minDY = DBL_MAX;
    BmnTrack* minTrack = nullptr;
    Int_t minIdx = -1;

    for (Int_t iTr = 0; iTr < fDchTracks->GetEntriesFast(); ++iTr) {
        BmnTrack* dchTr = (BmnTrack*)fDchTracks->At(iTr);
        if (!dchTr) continue;

        Double_t trZ = dchTr->GetParamFirst()->GetZ();

        Bool_t ok = kFALSE;
        if (trZ < zDchTh1 && num == 1) ok = kTRUE;
        if (trZ > zDchTh2 && num == 2) ok = kTRUE;
        if (!ok) continue;

        FairTrackParam param = parMinZ;
        if (fKalman->TGeoTrackPropagate(&param, trZ, fPDG, nullptr, nullptr) == kBMNERROR)
            continue;

        Float_t dX = param.GetX() - dchTr->GetParamFirst()->GetX();
        Float_t dY = param.GetY() - dchTr->GetParamFirst()->GetY();

        if (Abs(dX) < xCut && Abs(dY) < yCut && Abs(dX) < Abs(minDX) && Abs(dY) < Abs(minDY)) {
            minTrack = dchTr;
            minIdx = iTr;
            minDX = dX;
            minDY = dY;
        }
    }
    if (minIdx == -1) return kBMNERROR;

    Double_t len = 0.0;
    FairTrackParam par(*(tr->GetParamLast()));
    FairTrackParam dchPar(*(minTrack->GetParamFirst()));
    if (fKalman->TGeoTrackPropagate(&par, dchPar.GetZ(), fPDG, nullptr, &len) == kBMNERROR) return kBMNERROR;
    Double_t chi = 0;
    UpdateTrackParam(&par, &dchPar, chi);
    tr->SetChi2(tr->GetChi2() + chi);
    tr->SetParamLast(par);
    len += tr->GetLength();
    tr->SetNHits(tr->GetNHits() + 1);
    tr->SetLength(len);

    if (num == 1) {
        tr->SetDch1TrackIndex(minIdx);
        fNMatchedDch1++;
    } else {
        tr->SetDch2TrackIndex(minIdx);
        fNMatchedDch2++;
    }
    return kBMNSUCCESS;
}

BmnStatus BmnGlobalTracking::Refit(BmnGlobalTrack* tr) {


    if (fVerbose) cout << "REFIT started" << endl;

    FairTrackParam parFirst = *(tr->GetParamFirst());
    fPDG = (parFirst.GetQp() > 0.) ? 2212 : -211;
    Double_t chi = 0.0;
    Double_t totChi2 = 0.0;

    //================Refitting to the end of the global track===========================
    if (tr->GetSilTrackIndex() != -1) {
        BmnTrack* silTrack = (BmnTrack*)fSiliconTracks->At(tr->GetSilTrackIndex());
        for (Int_t hitIdx = 0; hitIdx < silTrack->GetNHits(); hitIdx++) {
            BmnSiliconHit* hit = (BmnSiliconHit*)fSilHits->At(silTrack->GetHitIndex(hitIdx));
            if (fKalman->TGeoTrackPropagate(&parFirst, hit->GetZ(), fPDG, nullptr, nullptr) == kBMNERROR) return kBMNERROR;
            if (fKalman->Update(&parFirst, hit, chi) == kBMNERROR) return kBMNERROR;
        }
    }

    if (fStsTracks) {
        CbmStsTrack* cbmTrack = (CbmStsTrack*)fStsTracks->At(tr->GetGemTrackIndex());
        for (Int_t hitIdx = 0; hitIdx < cbmTrack->GetNStsHits(); hitIdx++) {
            CbmStsHit* hit = (CbmStsHit*)fStsHits->At(cbmTrack->GetStsHitIndex(hitIdx));
            if (fKalman->TGeoTrackPropagate(&parFirst, hit->GetZ(), fPDG, nullptr, nullptr) == kBMNERROR) return kBMNERROR;
            if (fKalman->Update(&parFirst, (BmnHit*)hit, chi) == kBMNERROR) return kBMNERROR;
        }
    } else if (fInnerTracks) {
        BmnGemTrack* gemTrack = (BmnGemTrack*)fGemTracks->At(tr->GetGemTrackIndex());
        for (Int_t hitIdx = 0; hitIdx < gemTrack->GetNHits(); hitIdx++) {
            BmnGemStripHit* hit = (BmnGemStripHit*)fGemHits->At(gemTrack->GetHitIndex(hitIdx));
            if (fKalman->TGeoTrackPropagate(&parFirst, hit->GetZ(), fPDG, nullptr, nullptr) == kBMNERROR) return kBMNERROR;
            if (fKalman->Update(&parFirst, hit, chi) == kBMNERROR) return kBMNERROR;
        }
    }

    if (tr->GetCscHitIndex(0) != -1) {
        BmnHit* hit = (BmnHit*)fCscHits->At(tr->GetCscHitIndex(0));
        if (fKalman->TGeoTrackPropagate(&parFirst, hit->GetZ(), fPDG, nullptr, nullptr) == kBMNERROR) return kBMNERROR;
        if (fKalman->Update(&parFirst, hit, chi) == kBMNERROR) return kBMNERROR;
        totChi2 += chi;
    }

    if (tr->GetTof1HitIndex() != -1) {
        BmnHit* hit = (BmnHit*)fTof1Hits->At(tr->GetTof1HitIndex());
        if (fKalman->TGeoTrackPropagate(&parFirst, hit->GetZ(), fPDG, nullptr, nullptr) == kBMNERROR) return kBMNERROR;
        if (fKalman->Update(&parFirst, hit, chi) == kBMNERROR) return kBMNERROR;
    }

    if (tr->GetCscHitIndex(1) != -1) {
        BmnHit* hit = (BmnHit*)fCscHits->At(tr->GetCscHitIndex(1));
        if (fKalman->TGeoTrackPropagate(&parFirst, hit->GetZ(), fPDG, nullptr, nullptr) == kBMNERROR) return kBMNERROR;
        if (fKalman->Update(&parFirst, hit, chi) == kBMNERROR) return kBMNERROR;
        totChi2 += chi;
    }

    if (tr->GetTof2HitIndex() != -1) {
        BmnHit* hit = (BmnHit*)fTof2Hits->At(tr->GetTof2HitIndex());
        if (fKalman->TGeoTrackPropagate(&parFirst, hit->GetZ(), fPDG, nullptr, nullptr) == kBMNERROR) return kBMNERROR;
        if (fKalman->Update(&parFirst, hit, chi) == kBMNERROR) return kBMNERROR;
    }

    if (tr->GetDchTrackIndex() != -1) {
        BmnTrack* dchTrack = (BmnTrack*)fDchTracks->At(tr->GetDchTrackIndex());
        FairTrackParam dchPar(*(dchTrack->GetParamFirst()));
        if (fKalman->TGeoTrackPropagate(&parFirst, dchPar.GetZ(), fPDG, nullptr, nullptr) == kBMNERROR) return kBMNERROR;
        UpdateTrackParam(&parFirst, &dchPar, chi);
        totChi2 += chi;
    }

    if (!IsParCorrect(&parFirst)) tr->SetFlag(-1);
    tr->SetParamLast(parFirst);

    FairTrackParam parLast = *(tr->GetParamLast());

    // =============Refitting to the vertex =================

    if (tr->GetTof2HitIndex() != -1) {
        BmnHit* hit = (BmnHit*)fTof2Hits->At(tr->GetTof2HitIndex());
        if (fKalman->TGeoTrackPropagate(&parFirst, hit->GetZ(), fPDG, nullptr, nullptr) == kBMNERROR) return kBMNERROR;
        if (fKalman->Update(&parFirst, hit, chi) == kBMNERROR) return kBMNERROR;
        totChi2 += chi;
    }

    if (tr->GetCscHitIndex(1) != -1) {
        BmnHit* hit = (BmnHit*)fCscHits->At(tr->GetCscHitIndex(1));
        if (fKalman->TGeoTrackPropagate(&parFirst, hit->GetZ(), fPDG, nullptr, nullptr) == kBMNERROR) return kBMNERROR;
        if (fKalman->Update(&parFirst, hit, chi) == kBMNERROR) return kBMNERROR;
        totChi2 += chi;
    }

    if (tr->GetTof1HitIndex() != -1) {
        BmnHit* hit = (BmnHit*)fTof1Hits->At(tr->GetTof1HitIndex());
        if (fKalman->TGeoTrackPropagate(&parFirst, hit->GetZ(), fPDG, nullptr, nullptr) == kBMNERROR) return kBMNERROR;
        if (fKalman->Update(&parFirst, hit, chi) == kBMNERROR) return kBMNERROR;
        totChi2 += chi;
    }

    if (tr->GetCscHitIndex(0) != -1) {
        BmnHit* hit = (BmnHit*)fCscHits->At(tr->GetCscHitIndex(0));
        if (fKalman->TGeoTrackPropagate(&parFirst, hit->GetZ(), fPDG, nullptr, nullptr) == kBMNERROR) return kBMNERROR;
        if (fKalman->Update(&parFirst, hit, chi) == kBMNERROR) return kBMNERROR;
        totChi2 += chi;
    }


    if (fStsTracks) {
        CbmStsTrack* cbmTrack = (CbmStsTrack*)fStsTracks->At(tr->GetGemTrackIndex());
        for (Int_t hitIdx = cbmTrack->GetNStsHits() - 1; hitIdx >= 0; hitIdx--) {
            CbmStsHit* hit = (CbmStsHit*)fStsHits->At(cbmTrack->GetStsHitIndex(hitIdx));
            if (fKalman->TGeoTrackPropagate(&parFirst, hit->GetZ(), fPDG, nullptr, nullptr) == kBMNERROR) return kBMNERROR;
            if (fKalman->Update(&parFirst, (BmnHit*)hit, chi) == kBMNERROR) return kBMNERROR;
            totChi2 += chi;
        }
    } else if (fInnerTracks) {
        BmnGemTrack* gemTrack = (BmnGemTrack*)fGemTracks->At(tr->GetGemTrackIndex());
        for (Int_t hitIdx = gemTrack->GetNHits() - 1; hitIdx >= 0; hitIdx--) {
            BmnGemStripHit* hit = (BmnGemStripHit*)fGemHits->At(gemTrack->GetHitIndex(hitIdx));
            if (fKalman->TGeoTrackPropagate(&parFirst, hit->GetZ(), fPDG, nullptr, nullptr) == kBMNERROR) return kBMNERROR;
            if (fKalman->Update(&parFirst, hit, chi) == kBMNERROR) return kBMNERROR;
            totChi2 += chi;
        }
    }

    if (tr->GetSilTrackIndex() != -1) {
        BmnTrack* silTrack = (BmnTrack*)fSiliconTracks->At(tr->GetSilTrackIndex());
        for (Int_t hitIdx = silTrack->GetNHits() - 1; hitIdx >= 0; hitIdx--) {
            BmnSiliconHit* hit = (BmnSiliconHit*)fSilHits->At(silTrack->GetHitIndex(hitIdx));
            if (fKalman->TGeoTrackPropagate(&parFirst, hit->GetZ(), fPDG, nullptr, nullptr) == kBMNERROR) return kBMNERROR;
            if (fKalman->Update(&parFirst, hit, chi) == kBMNERROR) return kBMNERROR;
            totChi2 += chi;
        }
    }

    if (tr->GetUpstreamTrackIndex() != -1) {
        BmnTrack* upsTrack = (BmnTrack*)fUpstreamTracks->At(tr->GetUpstreamTrackIndex());
        FairTrackParam upsPar(*(upsTrack->GetParamLast()));
        if (fKalman->TGeoTrackPropagate(&parFirst, upsPar.GetZ(), fPDG, nullptr, nullptr) == kBMNERROR) return kBMNERROR;
        UpdateTrackParam(&parFirst, &upsPar, chi);
        totChi2 += chi;
    }

    TVector3 pos;
    if (fVertexL1) {
        pos = TVector3(fVertexL1->GetX(), fVertexL1->GetY(), fVertexL1->GetZ());
    } else {
        pos = TVector3(0.0, 0.0, 0.0);
    }
    TVector3 dpos = TVector3(0.05, 0.05, 0.0);
    if (fKalman->TGeoTrackPropagate(&parFirst, pos.z(), fPDG, nullptr, nullptr) == kBMNERROR) return kBMNERROR;
    // update in virtual vertex hit
    BmnHit* vertexHit = new BmnHit(0, pos, dpos, -1);
    // printf("BEFORE\n"); parFirst.Print();
    if (fKalman->Update(&parFirst, vertexHit, chi) == kBMNERROR) return kBMNERROR;
    // printf("AFTER\n"); parFirst.Print();
    // printf("chiInVertex = %f\n", chi);
    tr->SetChi2InVertex(chi);

    if (!IsParCorrect(&parFirst)) tr->SetFlag(-1);
    tr->SetChi2(totChi2);
    tr->SetParamFirst(parFirst);

    return kBMNSUCCESS;
}

BmnStatus BmnGlobalTracking::UpdateMomentum(BmnGlobalTrack* tr) {
    FairTrackParam par = *(tr->GetParamFirst());
    Int_t pdg = (par.GetQp() > 0) ? 2212 : -211;
    fKalman->TGeoTrackPropagate(&par, -200, pdg, nullptr, nullptr);
    Double_t Alpha_in = ATan(par.GetTx());
    //Double_t Bdl = 2.856;     //Abs(MagFieldIntegral(par, -200.0, 1000.0, 1.0) * 0.001);
    Double_t Bdl = Abs(MagFieldIntegral(par, -200.0, 600.0, 1.0) * 0.001);
    fKalman->TGeoTrackPropagate(&par, 600, pdg, nullptr, nullptr);
    Double_t Alpha_out = ATan(par.GetTx());
    if (tr->GetDchTrackIndex() != -1) {
        BmnTrack* matchedDch = (BmnTrack*)fDchTracks->At(tr->GetDchTrackIndex());
        Alpha_out = ATan(matchedDch->GetParamFirst()->GetTx());
    }
    if (tr->GetUpstreamTrackIndex() != -1) {
        BmnTrack* upTrack = (BmnTrack*)fUpstreamTracks->At(tr->GetUpstreamTrackIndex());
        if (upTrack != nullptr)
            Alpha_in = ATan(upTrack->GetParamLast()->GetTx());
    }
    Double_t momInt = 0.3 * Bdl / (Sin(Alpha_out) - Sin(Alpha_in));
    //Double_t momInt = 0.3 * Bdl / (Alpha_out - Alpha_in);
    if (!IsNaN(momInt) && Abs(momInt) > 0.01 && Abs(momInt) < 100) {
        tr->GetParamFirst()->SetQp(1.0 / momInt);
    }
    // cout << Alpha_in << " " << Alpha_out << " " << Bdl << " " << momInt << endl;

    return kBMNSUCCESS;
}

void BmnGlobalTracking::Finish() {
    delete fKalman;
    printf("Work time of BmnGlobalTracking: %4.2f sec.\n", fTime);
    PrintStatistics();
}

Double_t BmnGlobalTracking::MagFieldIntegral(FairTrackParam& par, Double_t zMin, Double_t zMax, Double_t step) {
    /*
    field in kG
    step in cm
    */
    Int_t pdg = (par.GetQp() > 0.) ? 2212 : -211;
    FairField* field = FairRunAna::Instance()->GetField();
    fKalman->TGeoTrackPropagate(&par, zMin, pdg, NULL, NULL, kTRUE);
    Double_t z = par.GetZ();
    Double_t integral = 0.0;
    while (z < zMax) {
        z += step;
        fKalman->TGeoTrackPropagate(&par, z, pdg, NULL, NULL, kTRUE);
        integral += field->GetBy(par.GetX(), par.GetY(), par.GetZ());
    }
    integral *= step;
    return integral;
}

void BmnGlobalTracking::PrintStatistics() {
    printf("================================================================================\n");
    printf("=======  Statistics of Global tracking\n");
    printf("=======  Number of input inner tracks: %ld\n", fNInnerTracks);
    printf("=======  Number of input inner tracks with 4+ hits extrapolated to the Vertex: %ld (%4.1f%%)\n", fNGoodInnerTracks, fNGoodInnerTracks * 100.0 / fNInnerTracks);
    printf("=======  Number of tracks matched with near CSC: %ld\n", fNMatchedNearCsc);
    printf("=======  Number of tracks matched with TOF-400: %ld\n", fNMatchedTof400);
    printf("=======  Number of tracks matched with far CSC: %ld\n", fNMatchedFarCsc);
    printf("=======  Number of tracks matched with DCH-1: %ld\n", fNMatchedDch1);
    printf("=======  Number of tracks matched with TOF-700: %ld\n", fNMatchedTof700);
    printf("=======  Number of tracks matched with DCH-2: %ld\n", fNMatchedDch2);
    printf("=======  Number of refitted output tracks: %ld (%4.1f%%)\n", fNGoodGlobalTracks, fNGoodGlobalTracks * 100.0 / fNInnerTracks);
    printf("================================================================================\n");
}

ClassImp(BmnGlobalTracking);
