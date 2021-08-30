/** BmnGlobalTracking.cxx
 * \author Sergey Merts <Sergey.Merts@gmail.com>
 * @since 2014
 * @version 1.0
 **/

#include "BmnGlobalTracking.h"

#include <Fit/FitResult.h>

#include <iterator>
#include <map>

#include "BmnEventHeader.h"
#include "BmnMwpcGeometry.h"
#include "BmnSiliconTrack.h"
#include "TFile.h"
#include "omp.h"

using namespace TMath;

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
fUpsHits(nullptr),
fMCTracks(nullptr),
fEvHead(nullptr),
fUpstreamTracks(nullptr),
fIsField(kTRUE),
fPDG(2212),
fTime(0.0),
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
fUpsHits(nullptr),
fMCTracks(nullptr),
fEvHead(nullptr),
fUpstreamTracks(nullptr),
fPDG(2212),
fTime(0.0),
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
        fhXdXDch2GemResid = new TH2F("fhXdXDch2GemResid", "fhXdXDch2GemResid", 200, -100, 100, 200, -100.0, 100.0);
        fhYdYDch2GemResid = new TH2F("fhYdYDch2GemResid", "fhYdYDch2GemResid", 200, -100, 100, 200, -100.0, 100.0);
        fhTxdXDch2GemResid = new TH2F("fhTxdXDch2GemResid", "fhTxdXDch2GemResid", 200, -0.5, 0.5, 200, -100.0, 100.0);
        fhTydYDch2GemResid = new TH2F("fhTydYDch2GemResid", "fhTydYDch2GemResid", 200, -0.5, 0.5, 200, -100.0, 100.0);

        fhXDchGGemResid = new TH1F("fhXDchGGemResid", "fhXDchGGemResid", 600, -100.0, 100.0);
        fhYDchGGemResid = new TH1F("fhYDchGGemResid", "fhYDchGGemResid", 600, -100.0, 100.0);
        fhTxDchGGemResid = new TH1F("fhTxDchGGemResid", "fhTxDchGGemResid", 600, -0.5, 0.5);
        fhTyDchGGemResid = new TH1F("fhTyDchGGemResid", "fhTyDchGGemResid", 600, -0.5, 0.5);
        fhXdXDchGGemResid = new TH2F("fhXdXDchGGemResid", "fhXdXDchGGemResid", 200, -100, 100, 200, -100.0, 100.0);
        fhYdYDchGGemResid = new TH2F("fhYdYDchGGemResid", "fhYdYDchGGemResid", 200, -100, 100, 200, -100.0, 100.0);
        fhTxdXDchGGemResid = new TH2F("fhTxdXDchGGemResid", "fhTxdXDchGGemResid", 200, -0.5, 0.5, 200, -100.0, 100.0);
        fhTydYDchGGemResid = new TH2F("fhTydYDchGGemResid", "fhTydYDchGGemResid", 200, -0.5, 0.5, 200, -100.0, 100.0);

        fhXUResid = new TH1F("fhXUResid", "fhXUResid", 600, -100, 100);
        fhYUResid = new TH1F("fhYUResid", "fhYUResid", 600, -100.0, 100.0);
        fhTxUResid = new TH1F("fhTxUResid", "fhTxUResid", 600, -0.5, 0.5);
        fhTyUResid = new TH1F("fhTyUResid", "fhTyUResid", 600, -0.5, 0.5);
        fhXdXUResid = new TH2F("fhXdXUResid", "fhXdXUResid", 200, -100, 100, 200, -100.0, 100.0);
        fhYdYUResid = new TH2F("fhYdYUResid", "fhYdYUResid", 200, -100, 100, 200, -100.0, 100.0);
        fhTxdXUResid = new TH2F("fhTxdXUResid", "fhTxdXUResid", 200, -0.5, 0.5, 200, -100.0, 100.0);
        fhTydYUResid = new TH2F("fhTydYUResid", "fhTydYUResid", 200, -0.5, 0.5, 200, -100.0, 100.0);

        const Int_t nSt = 6;

        fhdXGemSt = new TH1F * [nSt];
        fhdYGemSt = new TH1F * [nSt];
        fhdTxGemSt = new TH1F * [nSt];
        fhdTyGemSt = new TH1F * [nSt];
        fhXdXGemSt = new TH2F * [nSt];
        fhYdYGemSt = new TH2F * [nSt];

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

    FairRootManager* ioman = FairRootManager::Instance();
    if (!ioman)
        Fatal("Init", "FairRootManager is not instantiated");

    fSilHits = (TClonesArray*)ioman->GetObject("BmnSiliconHit");
    fGemHits = (TClonesArray*)ioman->GetObject("BmnGemStripHit");
    fCscHits = (TClonesArray*)ioman->GetObject("BmnCSCHit");
    fTof1Hits = (TClonesArray*)ioman->GetObject("BmnTof400Hit");
    fTof2Hits = (TClonesArray*)ioman->GetObject("BmnTof700Hit");

    fCbmStsTracks = (TClonesArray*)ioman->GetObject("StsTrack");
    if (fCbmStsTracks) {
        printf("BmnGlobalTracking::Init(): branch StsTrack was found! Global tracks will be based on CBM tracks \n");
        fGlobalTracks = new TClonesArray("BmnGlobalTrack", 100);  //out
        ioman->Register("BmnGlobalTrack", "GLOBAL", fGlobalTracks, kTRUE);
    }
    else {
        fInnerTracks = (TClonesArray*)ioman->GetObject("BmnGlobalTrack");
        fGemTracks = (TClonesArray*)ioman->GetObject("BmnGemTrack");
        if (!fInnerTracks) {
            cout << "BmnGlobalTracking::Init(): branch BmnInnerTrack not found! Task will be deactivated" << endl;
            SetActive(kFALSE);
            return kERROR;
        }
        else {
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
    }
    else {  //In case of MC data we get hits from indput tree
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
    if (fCbmStsTracks)
        fGlobalTracks->Delete();
    //if (!fInnerTracks) return;
    if (fIsExp) {
        if (fDchHits) fDchHits->Delete();
    }
    if (fUpsHits) fUpsHits->Delete();

    //Alignment. FIXME: move to DB
    if (fDchTracks) {
        Double_t dchTxCorr = (fIsSRC) ? +0.001 : +0.006;
        Double_t dchTyCorr = (fIsSRC) ? -0.001 : -0.0003;
        Double_t dchXCorr = (fIsSRC) ? -8.52 : -6.97;
        Double_t dchYCorr = (fIsSRC) ? -3.01 : -2.92;
        for (Int_t trIdx = 0; trIdx < fDchTracks->GetEntriesFast(); ++trIdx) {
            BmnTrack* dchTr = (BmnTrack*)fDchTracks->At(trIdx);
            FairTrackParam* parDch = dchTr->GetParamFirst();
            Double_t zDCH = parDch->GetZ();
            if (fIsExp) {
                if (zDCH < 550) {         //dch1
                }
                else if (zDCH > 650) {  //dch2
                }
                else {                  //global dch
                    parDch->SetTx(parDch->GetTx() + dchTxCorr);
                    parDch->SetTy(parDch->GetTy() + dchTyCorr);
                    parDch->SetX(parDch->GetX() + dchXCorr);
                    parDch->SetY(parDch->GetY() + dchYCorr);
                }

                BmnHit dchHit;
                Int_t st = (zDCH < 550) ? 0 : (zDCH > 650) ? 1 : 7;
                dchHit.SetStation(st);
                dchHit.SetXYZ(parDch->GetX(), parDch->GetY(), zDCH);
                dchHit.SetDxyz(0.02, 0.02, 0.0);
                dchHit.SetIndex(trIdx);  //index of dch track instead of index of hit. In order to have fast link hit->track
                new ((*fDchHits)[fDchHits->GetEntriesFast()]) BmnHit(dchHit);
            }
        }
    }
    if (fIsExp) {
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
                BmnHit upsHit;
                upsHit.SetXYZ(parUp->GetX(), parUp->GetY(), parUp->GetZ());
                upsHit.SetDxyz(0.02, 0.02, 0.0);
                upsHit.SetIndex(trIdx);  //index of dch track instead of index of hit. In order to have fast link hit->track
                new ((*fUpsHits)[fUpsHits->GetEntriesFast()]) BmnHit(upsHit);
            }
    }

    if (fCbmStsTracks) {
        for (Int_t i = 0; i < fCbmStsTracks->GetEntriesFast(); ++i) {
            CbmStsTrack* cbmTrack = (CbmStsTrack*)fCbmStsTracks->At(i);

            BmnGlobalTrack globTr;
            globTr.SetGemTrackIndex(i);
            globTr.SetParamFirst(*(cbmTrack->GetParamFirst()));
            globTr.SetParamLast(*(cbmTrack->GetParamLast()));
            globTr.SetNHits(cbmTrack->GetNStsHits());
            globTr.SetNDF(cbmTrack->GetNDF());
            globTr.SetChi2(cbmTrack->GetChi2());

            MatchingTOF(cbmTrack, &globTr, 1);
            MatchingTOF(cbmTrack, &globTr, 2);
            new ((*fGlobalTracks)[fGlobalTracks->GetEntriesFast()]) BmnGlobalTrack(globTr);
        }
    }

    if (fInnerTracks) {
        for (Int_t i = 0; i < fInnerTracks->GetEntriesFast(); ++i) {
            BmnGlobalTrack* glTrack = (BmnGlobalTrack*)fInnerTracks->At(i);
            //if (glTrack->GetNHits() < 5) continue;

            if (fDoAlign) {
                BmnGemTrack* gemTrack = (BmnGemTrack*)fGemTracks->At(glTrack->GetGemTrackIndex());
                for (Int_t hitIdx = 0; hitIdx < gemTrack->GetNHits(); ++hitIdx) {
                    BmnGemStripHit* hit = (BmnGemStripHit*)fGemHits->At(gemTrack->GetHitIndex(hitIdx));
                    Int_t stId = (fIsSRC) ? hit->GetStation() - 4 : hit->GetStation();
                    fhdXGemSt[stId]->Fill(hit->GetResX());
                    fhdYGemSt[stId]->Fill(hit->GetResY());
                    fhXdXGemSt[stId]->Fill(hit->GetX(), hit->GetResX());
                    fhYdYGemSt[stId]->Fill(hit->GetY(), hit->GetResY());
                }
            }

            //Downstream
            if (!fIsSRC) MatchingCSC(glTrack);
            if (!fIsSRC) MatchingTOF(glTrack, 1);
            if (fIsExp)
                MatchingDCH(glTrack, 7);
            else {
                MatchingDCH(glTrack, 0);
                MatchingDCH(glTrack, 1);
            }
            //MatchingDCH(glTrack, 0);
            MatchingTOF(glTrack, 2);
            //MatchingDCH(glTrack, 1);

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

    //CalculateLength();

    sw.Stop();
    fTime += sw.RealTime();

    //if (fVerbose) cout << "GLOBAL_TRACKING: Number of merged tracks: " << fGlobalTracks->GetEntriesFast() << endl;
    if (fVerbose > 1)
        cout << "\n======================== Global tracking exec finished ====================\n"
        << endl;
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

void BmnGlobalTracking::CalcSiliconDist(Int_t stat, BmnGlobalTrack* glTr, map<Double_t, pair<Int_t, Int_t>>& silDists) {
    const Double_t distCut = 1.;
    for (Int_t hitIdx = 0; hitIdx < fSilHits->GetEntriesFast(); hitIdx++) {
        BmnSiliconHit* hit = (BmnSiliconHit*)fSilHits->UncheckedAt(hitIdx);
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

Int_t BmnGlobalTracking::FindNearestHit(BmnGlobalTrack* tr, TClonesArray* hits, Float_t distCut) {
    if (!hits || !tr)
        return kBMNERROR;

    Double_t minDist = DBL_MAX;
    Int_t minIdx = -1;

    for (Int_t hitIdx = 0; hitIdx < hits->GetEntriesFast(); ++hitIdx) {
        BmnHit* hit = (BmnHit*)hits->At(hitIdx);
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

BmnStatus BmnGlobalTracking::MatchingMWPC(BmnGlobalTrack* tr) {
    if (!fSilHits) return kBMNERROR;

    return kBMNSUCCESS;
}

BmnStatus BmnGlobalTracking::MatchingCSC(BmnGlobalTrack* tr) {
    if (!fCscHits) return kBMNERROR;

    //residuals after peak fitting of all-to-all histograms
    Double_t sigmaXcscgemResid = 2.80;  //1.25;
    Double_t sigmaYcscgemResid = 0.68;  //0.79;
    Double_t xCut = 3 * sigmaXcscgemResid;
    Double_t yCut = 3 * sigmaYcscgemResid;

    Double_t minDX = DBL_MAX;
    Double_t minDY = DBL_MAX;
    Double_t dX = DBL_MAX;
    Double_t dY = DBL_MAX;

    Double_t minDist = DBL_MAX;
    BmnHit* minHit = nullptr;  // Pointer to the nearest hit
    Int_t minIdx = -1;

    for (Int_t hitIdx = 0; hitIdx < fCscHits->GetEntriesFast(); ++hitIdx) {
        BmnHit* hit = (BmnHit*)fCscHits->At(hitIdx);
        if (!hit) continue;
        if (!fDoAlign && hit->IsUsed()) continue;
        FairTrackParam par(*(tr->GetParamLast()));
        if (fKalman->TGeoTrackPropagate(&par, hit->GetZ(), fPDG, nullptr, nullptr, fIsField) == kBMNERROR)
            continue;
        dX = par.GetX() - hit->GetX();
        dY = par.GetY() - hit->GetY();
        //Double_t dist = Sqrt(dX * dX + dY * dY);
        if (fDoAlign) {
            if (Abs(dY) < yCut) fhXCscGemResid->Fill(dX);
            if (Abs(dX) < xCut) fhYCscGemResid->Fill(dY);
            fhXdXCscGemResid->Fill(par.GetX(), dX);
            fhYdYCscGemResid->Fill(par.GetY(), dY);
            fhTxdXCscGemResid->Fill(par.GetTx(), dX);
            fhTydYCscGemResid->Fill(par.GetTy(), dY);
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
    minHit->SetUsing(kTRUE);
    return kBMNSUCCESS;
}

BmnStatus BmnGlobalTracking::MatchingTOF(CbmStsTrack* tr, BmnGlobalTrack* glTr, Int_t num) {
    TClonesArray* tofHits = (num == 1) ? fTof1Hits : (num == 2) ? fTof2Hits : nullptr;
    if (!tofHits) return kBMNERROR;

    //residuals after peak fitting of all-to-all histograms
    Double_t sigmaXtof1gemResid = 1.0;
    Double_t sigmaYtof1gemResid = 1.0;
    Double_t sigmaXtof2gemResid = 1.0;
    Double_t sigmaYtof2gemResid = 1.0;
    Double_t xCut = (num == 1) ? 3 * sigmaXtof1gemResid : 3 * sigmaXtof2gemResid;
    Double_t yCut = (num == 1) ? 3 * sigmaYtof1gemResid : 3 * sigmaYtof2gemResid;

    Double_t minDX = DBL_MAX;
    Double_t minDY = DBL_MAX;
    Double_t dX = DBL_MAX;
    Double_t dY = DBL_MAX;

    BmnHit* minHit = nullptr;  // Pointer to the nearest hit
    Int_t minIdx = -1;
    FairTrackParam minParPredLast;  // predicted track parameters for closest hit

    FairTrackParam param;
    for (Int_t hitIdx = 0; hitIdx < tofHits->GetEntriesFast(); ++hitIdx) {
        BmnHit* hit = (BmnHit*)tofHits->At(hitIdx);
        if (!hit) continue;
        //if (hit->IsUsed()) continue;
        CbmKFTrack kfTrL = CbmKFTrack(*tr, false); //last

        kfTrL.Extrapolate(hit->GetZ());
        kfTrL.GetTrackParam(param);

        dX = param.GetX() - hit->GetX();
        dY = param.GetY() - hit->GetY();

        if (Abs(dX) < xCut && Abs(dY) < yCut && Abs(dX) < minDX && Abs(dY) < minDY) {
            minDX = dX;
            minDY = dY;
            minHit = hit;
            minParPredLast = param;
            minIdx = hitIdx;
        }
    }

    if (minHit == nullptr)
        return kBMNERROR;

    minHit->SetResXY(minDX, minDY);

    Double_t zmin = tr->GetParamFirst()->GetZ();
    Double_t zmax = tr->GetParamLast()->GetZ();
    Double_t z = zmin, leng = 0, step = 5.0; // 10.0;
    Int_t finish = 0, ok = 1;
    TVector3 pos0, pos1;
    CbmKFTrack track = CbmKFTrack(*tr, kTRUE); // first point
    track.GetTrackParam(param);
    Double_t prec = 1. / TMath::Abs(param.GetQp());
    param.Position(pos0);

    for (Int_t j = 0; j < 999; ++j) {
        z += step;
        if (z > minHit->GetZ()) { z = minHit->GetZ(); finish = 1; }
        if (z <= zmax) track.Smooth(z);
        else ok *= !(track.Extrapolate(z));
        if (!ok) break;
        track.GetTrackParam(param);
        param.Position(pos1);
        leng += (pos1 - pos0).Mag();
        if (finish) break;
        pos0 = pos1;
    }
    if (ok) {
        // Go to primary vertex
        track = CbmKFTrack(*tr, kTRUE); // first point
        track.GetTrackParam(param);
        param.Position(pos0);
        Double_t zv = (fVertex) ? fVertex->GetZ() : (fIsSRC) ? -647.5 : -2.3;  // z of target by default
        z = zmin;
        finish = 0;
        for (Int_t j = 0; j < 99; ++j) {
            z -= step;
            if (z < zv) { z = zv; finish = 1; }
            ok *= !(track.Extrapolate(z));
            if (!ok) break;
            track.GetTrackParam(param);
            param.Position(pos1);
            leng += (pos1 - pos0).Mag();
            if (finish) break;
            pos0 = pos1;
        }
    }
    glTr->SetParamLast(minParPredLast);
    if (num == 1)
        glTr->SetTof1HitIndex(minIdx);
    else
        glTr->SetTof2HitIndex(minIdx);

    glTr->SetBeta(leng / minHit->GetTimeStamp() / (TMath::C() * 1e-7), num);
    minHit->SetUsing(kTRUE);
    minHit->SetLength(leng);  // length from target to Tof hit
    glTr->SetNHits(glTr->GetNHits() + 1);
    glTr->SetLength(leng);
    return kBMNSUCCESS;
}

BmnStatus BmnGlobalTracking::MatchingTOF(BmnGlobalTrack* tr, Int_t num) {
    TClonesArray* tofHits = (num == 1) ? fTof1Hits : (num == 2) ? fTof2Hits : nullptr;
    if (!tofHits) return kBMNERROR;

    //residuals after peak fitting of all-to-all histograms
    Double_t sigmaXtof1gemResid = 0.96;  //1000;
    Double_t sigmaYtof1gemResid = 0.90;  //1000;
    Double_t sigmaXtof2gemResid = 3.37;  //6.2;                                 //1.34;
    Double_t sigmaYtof2gemResid = 1.01;  //1.38;
    Double_t xCut = (num == 1) ? 3 * sigmaXtof1gemResid : 3 * sigmaXtof2gemResid;
    Double_t yCut = (num == 1) ? 3 * sigmaYtof1gemResid : 3 * sigmaYtof2gemResid;

    Double_t minDX = DBL_MAX;
    Double_t minDY = DBL_MAX;
    Double_t dX = DBL_MAX;
    Double_t dY = DBL_MAX;

    BmnHit* minHit = nullptr;  // Pointer to the nearest hit
    Int_t minIdx = -1;
    FairTrackParam minParPredLast;  // predicted track parameters for closest hit
    for (Int_t hitIdx = 0; hitIdx < tofHits->GetEntriesFast(); ++hitIdx) {
        BmnHit* hit = (BmnHit*)tofHits->At(hitIdx);
        if (!hit) continue;
        if (!fDoAlign && hit->IsUsed()) continue;
        FairTrackParam parPredict(*(tr->GetParamLast()));
        if (fKalman->TGeoTrackPropagate(&parPredict, hit->GetZ(), fPDG, nullptr, nullptr, fIsField) == kBMNERROR) continue;
        dX = parPredict.GetX() - hit->GetX();
        dY = parPredict.GetY() - hit->GetY();
        if (fDoAlign) {
            if (num == 1) {
                if (Abs(dY) < yCut) fhXTof1GemResid->Fill(dX);
                if (Abs(dX) < xCut) fhYTof1GemResid->Fill(dY);
                fhXdXTof1GemResid->Fill(parPredict.GetX(), dX);
                fhYdYTof1GemResid->Fill(parPredict.GetY(), dY);
                fhTxdXTof1GemResid->Fill(parPredict.GetTx(), dX);
                fhTydYTof1GemResid->Fill(parPredict.GetTy(), dY);
            }
            else if (num == 2) {
                //if (Abs(parPredict.GetY() - hit->GetY()) < 2) {
                if (Abs(dY) < yCut) fhXTof2GemResid->Fill(dX);
                if (Abs(dX) < xCut) fhYTof2GemResid->Fill(dY);
                fhXdXTof2GemResid->Fill(parPredict.GetX(), dX);
                fhYdYTof2GemResid->Fill(parPredict.GetY(), dY);
                fhTxdXTof2GemResid->Fill(parPredict.GetTx(), dX);
                fhTydYTof2GemResid->Fill(parPredict.GetTy(), dY);
                //}
            }
        }
        if (Abs(dX) < xCut && Abs(dY) < yCut && Abs(dX) < minDX && Abs(dY) < minDY) {
            minDX = dX;
            minDY = dY;
            minHit = hit;
            minParPredLast = parPredict;
            minIdx = hitIdx;
        }
    }

    if (minHit == nullptr)
        return kBMNERROR;

    minHit->SetResXY(minDX, minDY);

    Double_t len = 0.0;
    FairTrackParam par(*(tr->GetParamLast()));
    Double_t zTarget = (fVertex) ? fVertex->GetZ() : (fIsSRC) ? -647.5 : -2.3;  // z of target by default
    fKalman->TGeoTrackPropagate(&par, minHit->GetZ(), fPDG, nullptr, nullptr, fIsField);
    Double_t chi = 0;
    fKalman->Update(&par, minHit, chi);
    tr->SetChi2(tr->GetChi2() + chi);
    tr->SetParamLast(par);
    fKalman->TGeoTrackPropagate(&par, zTarget, fPDG, nullptr, &len, fIsField);
    if (num == 1)
        tr->SetTof1HitIndex(minIdx);
    else
        tr->SetTof2HitIndex(minIdx);

    tr->SetBeta(len / minHit->GetTimeStamp() / (TMath::C() * 1e-7), num);
    minHit->SetUsing(kTRUE);
    minHit->SetLength(len);  // length from target to Tof hit
    tr->SetNHits(tr->GetNHits() + 1);
    tr->SetLength(len);
    return kBMNSUCCESS;
}

BmnStatus BmnGlobalTracking::MatchingUpstream(BmnGlobalTrack* glTr) {
    //we need this function only for SRC
    //In BM@N we use silicon and GEM hits as a whole
    if (fUpstreamTracks->GetEntriesFast() == 0) return kBMNERROR;
    //
    Double_t sigma = 2.0;
    Double_t xCut = 3 * sigma;
    Double_t yCut = 3 * sigma;

    BmnHit* minHit = nullptr;
    Double_t minDX = DBL_MAX;
    Double_t minDY = DBL_MAX;

    Int_t minIdx = -1;

    for (Int_t iHit = 0; iHit < fUpsHits->GetEntriesFast(); ++iHit) {
        BmnHit* hit = (BmnHit*)fUpsHits->At(iHit);
        if (!hit) continue;
        if (!fDoAlign && hit->IsUsed()) continue;
        FairTrackParam par(*(glTr->GetParamFirst()));
        fPDG = (par.GetQp() > 0.) ? 2212 : -211;
        if (fKalman->TGeoTrackPropagate(&par, hit->GetZ(), fPDG, nullptr, nullptr, fIsField) == kBMNERROR)
            continue;
        Double_t dX = par.GetX() - hit->GetX();
        Double_t dY = par.GetY() - hit->GetY();
        if (fDoAlign) {
            BmnTrack* upsTr = (BmnTrack*)fUpstreamTracks->At(hit->GetIndex());
            if (!upsTr) continue;
            FairTrackParam upsPar(*(upsTr->GetParamLast()));
            fhXUResid->Fill(dX);
            fhYUResid->Fill(dY);
            fhTxUResid->Fill(par.GetTx() - upsPar.GetTx());
            fhTyUResid->Fill(par.GetTy() - upsPar.GetTy());
            fhXdXUResid->Fill(par.GetX(), dX);
            fhYdYUResid->Fill(par.GetY(), dY);
            fhTxdXUResid->Fill(par.GetTx(), dX);
            fhTydYUResid->Fill(par.GetTy(), dY);
        }
        if (Abs(dX) < xCut && Abs(dY) < yCut && Abs(dX) < minDX && Abs(dY) < minDY) {
            minHit = hit;
            minDX = dX;
            minDY = dY;
        }
    }

    if (minHit == nullptr)
        return kBMNERROR;

    FairTrackParam par(*(glTr->GetParamFirst()));
    fPDG = (par.GetQp() > 0.) ? 2212 : -211;
    Double_t len = glTr->GetLength();
    fKalman->TGeoTrackPropagate(&par, minHit->GetZ(), fPDG, nullptr, &len, fIsField);
    minHit->SetResXY(minDX, minDY);
    Double_t chi = 0;
    fKalman->Update(&par, minHit, chi);
    glTr->SetChi2(glTr->GetChi2() + chi);
    glTr->SetUpstreamTrackIndex(minHit->GetIndex());
    BmnTrack* matchedUps = (BmnTrack*)fUpstreamTracks->At(minHit->GetIndex());
    if (matchedUps != nullptr) {
        glTr->SetNHits(glTr->GetNHits() + matchedUps->GetNHits());
        glTr->SetLength(len);
        glTr->SetParamFirst(par);
        minHit->SetUsing(kTRUE);
        return kBMNSUCCESS;
    }
    else {
        return kBMNERROR;
    }
}

BmnStatus BmnGlobalTracking::MatchingDCH(BmnGlobalTrack* tr, Int_t num) {
    /**
 * num = 0 for dch1
 * num = 1 for dch2
 * num = 7 for global dch
 **/

    if (!fDchTracks) return kBMNERROR;

    Double_t minDX = DBL_MAX;
    Double_t minDY = DBL_MAX;
    BmnHit* minHit = nullptr;

    //residuals after peak fitting of all-to-all histograms
    Double_t sigmaXdch1gemResid = 1.0;   //2.16;
    Double_t sigmaYdch1gemResid = 1.0;   //0.75;
    Double_t sigmaXdch2gemResid = 1.0;   //0.90;
    Double_t sigmaYdch2gemResid = 1.0;   //0.53;
    Double_t sigmaXdchGgemResid = 5.54;  //8.18;
    Double_t sigmaYdchGgemResid = 2.33;
    Double_t xCut = (num == 0) ? 3 * sigmaXdch1gemResid : (num == 1) ? 3 * sigmaXdch2gemResid : 3 * sigmaXdchGgemResid;
    Double_t yCut = (num == 0) ? 3 * sigmaYdch1gemResid : (num == 1) ? 3 * sigmaYdch2gemResid : 3 * sigmaYdchGgemResid;

    for (Int_t iHit = 0; iHit < fDchHits->GetEntriesFast(); ++iHit) {
        BmnHit* hit = (BmnHit*)fDchHits->At(iHit);
        if (!hit) continue;
        if (!fDoAlign && hit->IsUsed()) continue; //???
        if (!fIsExp) hit->SetStation((hit->GetZ() < 600) ? 0 : 1);
        if (hit->GetStation() != num) continue;
        FairTrackParam par(*(tr->GetParamLast()));
        fPDG = (par.GetQp() > 0.) ? 2212 : -211;
        if (fKalman->TGeoTrackPropagate(&par, hit->GetZ(), fPDG, nullptr, nullptr, fIsField) == kBMNERROR)
            continue;
        //Double_t dist = Sqrt(Sq(par.GetX() - hit->GetX()) + Sq(par.GetY() - hit->GetY()));
        Double_t dX = par.GetX() - hit->GetX();
        Double_t dY = par.GetY() - hit->GetY();
        if (fDoAlign) {
            BmnTrack* dchTr = (BmnTrack*)fDchTracks->At(hit->GetIndex());
            if (!dchTr) continue;
            FairTrackParam dchPar(*(dchTr->GetParamFirst()));
            if (num == 0) {
                fhXDch1GemResid->Fill(dX);
                fhYDch1GemResid->Fill(dY);
                fhTxDch1GemResid->Fill(par.GetTx() - dchPar.GetTx());
                fhTyDch1GemResid->Fill(par.GetTy() - dchPar.GetTy());
                fhXdXDch1GemResid->Fill(par.GetX(), dX);
                fhYdYDch1GemResid->Fill(par.GetY(), dY);
                fhTxdXDch1GemResid->Fill(par.GetTx(), dX);
                fhTydYDch1GemResid->Fill(par.GetTy(), dY);
            }
            else if (num == 1) {
                fhXDch2GemResid->Fill(dX);
                fhYDch2GemResid->Fill(dY);
                fhTxDch2GemResid->Fill(par.GetTx() - dchPar.GetTx());
                fhTyDch2GemResid->Fill(par.GetTy() - dchPar.GetTy());
                fhXdXDch2GemResid->Fill(par.GetX(), dX);
                fhYdYDch2GemResid->Fill(par.GetY(), dY);
                fhTxdXDch2GemResid->Fill(par.GetTx(), dX);
                fhTydYDch2GemResid->Fill(par.GetTy(), dY);
            }
            else if (num == 7) {
                if (Abs(dY) < 3) fhXDchGGemResid->Fill(dX);
                if (Abs(dX) < 5) fhYDchGGemResid->Fill(dY);
                if (Abs(dY) < 3) fhTxDchGGemResid->Fill(par.GetTx() - dchPar.GetTx());
                if (Abs(dX) < 5) fhTyDchGGemResid->Fill(par.GetTy() - dchPar.GetTy());
                fhXdXDchGGemResid->Fill(par.GetX(), dX);
                fhYdYDchGGemResid->Fill(par.GetY(), dY);
                fhTxdXDchGGemResid->Fill(par.GetTx(), dX);
                fhTydYDchGGemResid->Fill(par.GetTy(), dY);
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
    fPDG = (par.GetQp() > 0.) ? 2212 : -211;
    Double_t len = tr->GetLength();
    fKalman->TGeoTrackPropagate(&par, minHit->GetZ(), fPDG, nullptr, &len, fIsField);
    minHit->SetResXY(minDX, minDY);
    Double_t chi = 0;
    fKalman->Update(&par, minHit, chi);
    tr->SetChi2(tr->GetChi2() + chi);
    if (fIsExp) {
        if (num == 0)
            tr->SetDch1TrackIndex(minHit->GetIndex());
        else if (num == 1)
            tr->SetDch2TrackIndex(minHit->GetIndex());
        else if (num == 7)
            tr->SetDchTrackIndex(minHit->GetIndex());
        BmnTrack* matchedDch = (BmnTrack*)fDchTracks->At(minHit->GetIndex());
        tr->SetNHits(tr->GetNHits() + matchedDch->GetNHits());
        //tr->SetLength(len); //FIXME! NOT CORRECT
    }
    else {
        if (num == 0)
            tr->SetDch1TrackIndex(minHit->GetRefIndex());
        else if (num == 1)
            tr->SetDch2TrackIndex(minHit->GetRefIndex());
        else if (num == 7)
            tr->SetDchTrackIndex(minHit->GetRefIndex());
        tr->SetNHits(tr->GetNHits() + 1);
    }
    minHit->SetUsing(kTRUE);
    tr->SetParamLast(par);
    return kBMNSUCCESS;
}

BmnStatus BmnGlobalTracking::Refit(BmnGlobalTrack* tr) {
    FairTrackParam parFirst = *(tr->GetParamFirst());
    fPDG = (parFirst.GetQp() > 0.) ? 2212 : -211;
    Double_t chi = 0.0;
    Double_t totChi2 = 0.0;

    //================Refitting to the end of the global track===========================
    if (tr->GetSilTrackIndex() != -1) {
        BmnTrack* silTrack = (BmnTrack*)fSiliconTracks->At(tr->GetSilTrackIndex());
        for (Int_t hitIdx = 0; hitIdx < silTrack->GetNHits(); hitIdx++) {
            BmnSiliconHit* hit = (BmnSiliconHit*)fSilHits->At(silTrack->GetHitIndex(hitIdx));
            fKalman->TGeoTrackPropagate(&parFirst, hit->GetZ(), fPDG, nullptr, nullptr, fIsField);
            fKalman->Update(&parFirst, hit, chi);
        }
    }

    BmnGemTrack* gemTrack = (BmnGemTrack*)fGemTracks->At(tr->GetGemTrackIndex());
    for (Int_t hitIdx = 0; hitIdx < gemTrack->GetNHits(); hitIdx++) {
        BmnGemStripHit* hit = (BmnGemStripHit*)fGemHits->At(gemTrack->GetHitIndex(hitIdx));
        fKalman->TGeoTrackPropagate(&parFirst, hit->GetZ(), fPDG, nullptr, nullptr, fIsField);
        fKalman->Update(&parFirst, hit, chi);
    }

    if (tr->GetCscHitIndex() != -1) {
        BmnHit* hit = (BmnHit*)fCscHits->At(tr->GetCscHitIndex());
        fKalman->TGeoTrackPropagate(&parFirst, hit->GetZ(), fPDG, nullptr, nullptr, fIsField);
        fKalman->Update(&parFirst, hit, chi);
    }

    if (tr->GetTof1HitIndex() != -1) {
        BmnHit* hit = (BmnHit*)fTof1Hits->At(tr->GetTof1HitIndex());
        fKalman->TGeoTrackPropagate(&parFirst, hit->GetZ(), fPDG, nullptr, nullptr, fIsField);
        fKalman->Update(&parFirst, hit, chi);
    }

    if (tr->GetTof2HitIndex() != -1) {
        BmnHit* hit = (BmnHit*)fTof2Hits->At(tr->GetTof2HitIndex());
        fKalman->TGeoTrackPropagate(&parFirst, hit->GetZ(), fPDG, nullptr, nullptr, fIsField);
        fKalman->Update(&parFirst, hit, chi);
    }

    if (tr->GetDchTrackIndex() != -1) {
        BmnHit* hit = (BmnHit*)fDchHits->At(tr->GetDchTrackIndex());
        fKalman->TGeoTrackPropagate(&parFirst, hit->GetZ(), fPDG, nullptr, nullptr, fIsField);
        fKalman->Update(&parFirst, hit, chi);
        totChi2 += chi;
    }

    if (!IsParCorrect(&parFirst, fIsField)) tr->SetFlag(-1);
    tr->SetParamLast(parFirst);

    FairTrackParam parLast = *(tr->GetParamLast());

    // =============Refitting to the vertex =================

    if (tr->GetTof2HitIndex() != -1) {
        BmnHit* hit = (BmnHit*)fTof2Hits->At(tr->GetTof2HitIndex());
        fKalman->TGeoTrackPropagate(&parFirst, hit->GetZ(), fPDG, nullptr, nullptr, fIsField);
        fKalman->Update(&parFirst, hit, chi);
        totChi2 += chi;
    }

    if (tr->GetTof1HitIndex() != -1) {
        BmnHit* hit = (BmnHit*)fTof1Hits->At(tr->GetTof1HitIndex());
        fKalman->TGeoTrackPropagate(&parFirst, hit->GetZ(), fPDG, nullptr, nullptr, fIsField);
        fKalman->Update(&parFirst, hit, chi);
        totChi2 += chi;
    }

    if (tr->GetCscHitIndex() != -1) {
        BmnHit* hit = (BmnHit*)fCscHits->At(tr->GetCscHitIndex());
        fKalman->TGeoTrackPropagate(&parLast, hit->GetZ(), fPDG, nullptr, nullptr, fIsField);
        fKalman->Update(&parLast, hit, chi);
        totChi2 += chi;
    }

    for (Int_t hitIdx = gemTrack->GetNHits() - 1; hitIdx >= 0; hitIdx--) {
        BmnGemStripHit* hit = (BmnGemStripHit*)fGemHits->At(gemTrack->GetHitIndex(hitIdx));
        fKalman->TGeoTrackPropagate(&parLast, hit->GetZ(), fPDG, nullptr, nullptr, fIsField);
        fKalman->Update(&parLast, hit, chi);
        totChi2 += chi;
    }

    if (tr->GetSilTrackIndex() != -1) {
        BmnTrack* silTrack = (BmnTrack*)fSiliconTracks->At(tr->GetSilTrackIndex());
        for (Int_t hitIdx = silTrack->GetNHits() - 1; hitIdx >= 0; hitIdx--) {
            BmnSiliconHit* hit = (BmnSiliconHit*)fSilHits->At(silTrack->GetHitIndex(hitIdx));
            fKalman->TGeoTrackPropagate(&parLast, hit->GetZ(), fPDG, nullptr, nullptr, fIsField);
            fKalman->Update(&parLast, hit, chi);
            totChi2 += chi;
        }
    }

    if (tr->GetUpstreamTrackIndex() != -1) {
        BmnHit* hit = (BmnHit*)fUpsHits->At(tr->GetUpstreamTrackIndex());
        fKalman->TGeoTrackPropagate(&parLast, hit->GetZ(), fPDG, nullptr, nullptr, fIsField);
        fKalman->Update(&parLast, hit, chi);
        totChi2 += chi;
    }
    if (!IsParCorrect(&parLast, fIsField)) tr->SetFlag(-1);
    tr->SetParamFirst(parLast);

    return kBMNSUCCESS;
}

BmnStatus BmnGlobalTracking::UpdateMomentum(BmnGlobalTrack* tr) {
    FairTrackParam par = *(tr->GetParamFirst());
    Int_t pdg = (par.GetQp() > 0) ? 2212 : -211;
    fKalman->TGeoTrackPropagate(&par, -200, pdg, nullptr, nullptr, fIsField);
    Double_t Alpha_in = ATan(par.GetTx());
    //Double_t Bdl = 2.856;     //Abs(MagFieldIntegral(par, -200.0, 1000.0, 1.0) * 0.001);
    Double_t Bdl = Abs(MagFieldIntegral(par, -200.0, 600.0, 1.0) * 0.001);
    fKalman->TGeoTrackPropagate(&par, 600, pdg, nullptr, nullptr, fIsField);
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

    if (fDoAlign) {
        TFile matchResid(Form("matchResid_%d.root", fRunId), "RECREATE");

        fhXCscGemResid->Fit("gaus", "WWSQR", "", -1, 1);
        fhYCscGemResid->Fit("gaus", "WWSQR", "", -1, 1);
        matchResid.Add(fhXCscGemResid);
        matchResid.Add(fhYCscGemResid);
        matchResid.Add(fhXdXCscGemResid);
        matchResid.Add(fhYdYCscGemResid);
        matchResid.Add(fhTxdXCscGemResid);
        matchResid.Add(fhTydYCscGemResid);

        fhXTof1GemResid->Fit("gaus", "WWSQR", "", -1, 1);
        fhYTof1GemResid->Fit("gaus", "WWSQR", "", -1, 1);
        matchResid.Add(fhXTof1GemResid);
        matchResid.Add(fhYTof1GemResid);
        matchResid.Add(fhXdXTof1GemResid);
        matchResid.Add(fhYdYTof1GemResid);
        matchResid.Add(fhTxdXTof1GemResid);
        matchResid.Add(fhTydYTof1GemResid);

        fhXTof2GemResid->Fit("gaus", "WWSQR", "", -1, 1);
        fhYTof2GemResid->Fit("gaus", "WWSQR", "", -1, 1);
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

        fhTxDchGGemResid->Fit("gaus", "WWSQR", "", -0.005, 0.005);
        fhXDchGGemResid->Fit("gaus", "WWSQR", "", -1, 1);
        fhTyDchGGemResid->Fit("gaus", "WWSQR", "", -0.005, 0.005);
        fhYDchGGemResid->Fit("gaus", "WWSQR", "", -1, 1);
        matchResid.Add(fhXDchGGemResid);
        matchResid.Add(fhYDchGGemResid);
        matchResid.Add(fhTxDchGGemResid);
        matchResid.Add(fhTyDchGGemResid);
        matchResid.Add(fhXdXDchGGemResid);
        matchResid.Add(fhYdYDchGGemResid);
        matchResid.Add(fhTxdXDchGGemResid);
        matchResid.Add(fhTydYDchGGemResid);

        fhXUResid->Fit("gaus", "WWSQR", "", -1, 1);
        matchResid.Add(fhXUResid);
        fhYUResid->Fit("gaus", "WWSQR", "", -1, -1);
        matchResid.Add(fhYUResid);
        fhTxUResid->Fit("gaus", "WWSQR", "", -0.005, 0.005);
        matchResid.Add(fhTxUResid);
        fhTyUResid->Fit("gaus", "WWSQR", "", -0.005, 0.005);
        matchResid.Add(fhTyUResid);
        matchResid.Add(fhXdXUResid);
        matchResid.Add(fhYdYUResid);
        matchResid.Add(fhTxdXUResid);
        matchResid.Add(fhTydYUResid);

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

    cout << "Work time of the Global matching: " << fTime << endl;
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

ClassImp(BmnGlobalTracking);
