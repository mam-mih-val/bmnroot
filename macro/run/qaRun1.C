
#include "TChain.h"
#include "TClonesArray.h"
#include "TH2F.h"
#include "TCanvas.h"
#include "TLine.h"

#include <vector>

using namespace TMath;
using namespace std;

void qaRun1(Int_t runId = 650) {

    /* Load basic libraries */
    gROOT->LoadMacro("$VMCWORKDIR/macro/run/bmnloadlibs.C");
    bmnloadlibs(); // load bmn libraries

    TChain *dstTree = new TChain("cbmsim");
    dstTree->Add(TString::Format("bmndst_run%d.root", runId));

    TClonesArray *recoTracks;
    dstTree->SetBranchAddress("RecoTracks", &recoTracks);
    TClonesArray *dchHits;
    dstTree->SetBranchAddress("BmnDchHit", &dchHits);
    TClonesArray *mwpcHits;
    dstTree->SetBranchAddress("BmnMwpcHit", &mwpcHits);

    Int_t events = dstTree->GetEntries();

    UInt_t nBins1 = 400;
    Float_t boundX = InnerRadiusOfOctagon;
    Float_t boundY = boundX;
    Float_t zMin = 500;
    Float_t zMax = 800;

    TH2F* h_xy1 = new TH2F("hxy_DCH1", "hxy_DCH1", nBins1, -boundX, boundX, nBins1, -boundY, boundY);
    TH2F* h_xy2 = new TH2F("hxy_DCH2", "hxy_DCH2", nBins1, -boundX, boundX, nBins1, -boundY, boundY);
    TH2F* h_xRyR = new TH2F("hxRyR_DCH", "hxRyR_DCH", 1000, -0.4, 0.4, 1000, -0.4, 0.4);

    TH2F* h_xy_tracks = new TH2F("hxy_tracks", "hxy_tracks", nBins1, -boundX, boundX, nBins1, -boundX, boundX);
    TH2F* h_zx_tracks = new TH2F("hzx_tracks", "hzx_tracks", nBins1, zMin, zMax, nBins1, -boundX, boundX);

    UInt_t nBins2 = 400;
    Float_t res_bound = 0.0; // cm
    TH1F* h_dch1_dist = new TH1F("h_dch1_dist", "h_dch1_dist", nBins2, 0.0, res_bound);
    TH1F* h_dch2_dist = new TH1F("h_dch2_dist", "h_dch2_dist", nBins2, 0.0, res_bound);
    TH1F* h_dch1_xRes = new TH1F("h_dch1_xRes", "h_dch1_xRes", nBins2, -res_bound, res_bound);
    TH1F* h_dch2_xRes = new TH1F("h_dch2_xRes", "h_dch2_xRes", nBins2, -res_bound, res_bound);
    TH1F* h_dch1_yRes = new TH1F("h_dch1_yRes", "h_dch1_yRes", nBins2, -res_bound, res_bound);
    TH1F* h_dch2_yRes = new TH1F("h_dch2_yRes", "h_dch2_yRes", nBins2, -res_bound, res_bound);
    TH1I* h_nHitsInTrack = new TH1I("h_nHitsInTrack", "h_nHitsInTrack", 30, 0, 30);

    vector<TLine*> fits;
    for (Int_t iEv = 0; iEv < events; iEv++) {
        dstTree->GetEntry(iEv);
        if (iEv % 1000 == 0) cout << "Event: " << iEv + 1 << "/" << events << endl;
        if (CheckEvent(dchHits)) {
            //        cout << " Number of tracks = " << recoTracks->GetEntriesFast() << endl;
            for (Int_t glIdx = 0; glIdx < recoTracks->GetEntriesFast(); ++glIdx) {
                CbmTrack* track = (CbmTrack*) recoTracks->At(glIdx);
                FairTrackParam* par = track->GetParamFirst();
                Float_t Tx = par->GetTx();
                Float_t Ty = par->GetTy();
                Float_t Vx = par->GetX();
                Float_t Vy = par->GetY();
                Float_t Vz = par->GetZ();
                for (Int_t i = 0; i < track->GetNofHits(); ++i) {
                    BmnDchHit* hit = (BmnDchHit*) dchHits->At(track->GetHitIndex(i));
                    Short_t dchId = hit->GetDchId();
                    if (dchId == 1) {
                        Float_t Hx = hit->GetX();
                        Float_t Hy = hit->GetY();
                        Float_t Hz = hit->GetZ();
                        Float_t Mx = Tx * (Hz - Vz) + Vx;
                        Float_t My = Ty * (Hz - Vz) + Vy;
                        Float_t d = Sqrt((Hx - Mx) * (Hx - Mx) + (Hy - My) * (Hy - My));
                        h_dch1_dist->Fill(d);
                        h_dch1_xRes->Fill(Hx - Mx);
                        h_dch1_yRes->Fill(Hy - My);
                    } else if (dchId == 2) {
                        Float_t Hx = hit->GetX();
                        Float_t Hy = hit->GetY();
                        Float_t Hz = hit->GetZ();
                        Float_t Mx = Tx * (Hz - Vz) + Vx;
                        Float_t My = Ty * (Hz - Vz) + Vy;
                        Float_t d = Sqrt((Hx - Mx) * (Hx - Mx) + (Hy - My) * (Hy - My));
                        h_dch2_dist->Fill(d);
                        h_dch2_xRes->Fill(Hx - Mx);
                        h_dch2_yRes->Fill(Hy - My);
                    }
                    h_nHitsInTrack->Fill(track->GetNofHits());
                }

                TLine* lineZX = new TLine(Vz, Vx, zMax, Tx * (zMax - Vz) + Vx);
                fits.push_back(lineZX);
                //            TLine* lineXY = new TLine(Vx, Vy, zMax, Px / Pz * (zMax - Vz) + Vx);
            }

            for (Int_t iHit = 0; iHit < dchHits->GetEntriesFast(); ++iHit) {
                BmnDchHit* hit1 = (BmnDchHit*) dchHits->At(iHit);
                Float_t Hx = hit1->GetX();
                Float_t Hy = hit1->GetY();
                Float_t Hz = hit1->GetZ();
                Float_t R = Sqrt(Hx * Hx + Hy * Hy + Hz * Hz);
                Short_t dchId = hit1->GetDchId();
                h_zx_tracks->Fill(Hz, Hx);
                h_xRyR->Fill(Hx / R, Hy / R);
                if (dchId == 1) {
                    h_xy1->Fill(Hx, Hy);
                } else if (dchId == 2) {
                    h_xy2->Fill(Hx, Hy);
                }
            }
            //break; //TEST to get one good event
        } else {
//            cout << "Skip bad event" << endl;
        }
    } // event loop

    TCanvas* c1 = new TCanvas("c1", "c1", 1800, 600);
    c1->Divide(3, 1);
    c1->cd(1);
    h_xy1->Draw("box");
    c1->cd(2);
    h_xy2->Draw("box");
    c1->cd(3);
    h_xRyR->Draw("box");
    c1->SaveAs("xyDch_hits.png");


    TCanvas* c2 = new TCanvas("c2", "c2", 1800, 600);
    c2->Divide(3, 2);
    c2->cd(1);
    h_dch1_dist->Draw("");
    c2->cd(2);
    h_dch1_xRes->Draw("");
    c2->cd(3);
    h_dch1_yRes->Draw("");
    c2->cd(4);
    h_dch2_dist->Draw("");
    c2->cd(5);
    h_dch2_xRes->Draw("");
    c2->cd(6);
    h_dch2_yRes->Draw("");
    c2->SaveAs("xyDch_res.png");

    TCanvas* c3 = new TCanvas("c3", "c3", 800, 800);
    h_nHitsInTrack->Draw("");
    c3->SaveAs("nHitsInTrack.png");

    TCanvas* c4 = new TCanvas("c4", "c4", 1600, 800);
    h_zx_tracks->Draw("P");
    h_zx_tracks->SetMarkerStyle(24);
    h_zx_tracks->SetMarkerSize(0.6);
    h_zx_tracks->SetMarkerColor(kBlue);
    for (Int_t i = 0; i < fits.size(); ++i) {
        fits.at(i)->SetLineColor(kRed);
        fits.at(i)->Draw();
    }
    c4->SaveAs("tracks.png");
}

Bool_t CheckEvent(TClonesArray* hits) {
    
    Bool_t lay0 = kFALSE;
    Bool_t lay1 = kFALSE;
    Bool_t lay2 = kFALSE;
    Bool_t lay3 = kFALSE;

    for (Int_t i = 0; i < hits->GetEntriesFast(); ++i) {
        BmnDchHit* hit = (BmnDchHit*) hits->At(i);
        UInt_t lay = hit->GetLayer();
        switch (lay) {
            case 0: lay0 = kTRUE;
                break;
            case 1: lay1 = kTRUE;
                break;
            case 2: lay2 = kTRUE;
                break;
            case 3: lay3 = kTRUE;
                break;
        }
    }
//    cout << lay0 << " " << lay1 << " " << lay2 << " " << lay3 << endl;

    return (lay0 && lay1 && lay2 && lay3);
}
