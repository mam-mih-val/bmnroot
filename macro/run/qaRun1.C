
#include "TChain.h"
#include "TClonesArray.h"
#include "TH2F.h"
#include "TCanvas.h"
#include "TLine.h"
#include "TVector3.h"
#include "reco_testRun_chain.C"

#include <vector>

using namespace TMath;
using namespace std;

void qaRun1(Int_t runId = 650, Int_t nEvents = 0) {

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

    Int_t events = (nEvents == 0) ? dstTree->GetEntries() : nEvents;

    //    vector<TLine*> fits;
    for (Int_t iEv = 0; iEv < events; iEv++) {
        dstTree->GetEntry(iEv);
        if (iEv % 1000 == 0) cout << "RUN: " << runId << "\t Event: " << iEv + 1 << "/" << events << endl;
        for (Int_t glIdx = 0; glIdx < recoTracks->GetEntriesFast(); ++glIdx) {
            CbmTrack* track = (CbmTrack*) recoTracks->At(glIdx);
            FairTrackParam* par = track->GetParamFirst();
            Float_t Tx = par->GetTx();
            Float_t Ty = par->GetTy();
            Float_t Az = par->GetQp() / Sqrt(Tx * Tx + Ty * Ty + 1.0);
            Float_t Ax = Az * Tx;
            Float_t Ay = Az * Ty;
            Float_t Vx = par->GetX();
            Float_t Vy = par->GetY();
            Float_t Vz = par->GetZ();
            h_nHits->Fill(track->GetNofHits());
            for (Int_t i = 0; i < track->GetNofHits(); ++i) {
                BmnDchHit* hit = (BmnDchHit*) dchHits->At(track->GetHitIndex(i));
                Short_t dchId = hit->GetDchId();
                Float_t Hx = hit->GetX();
                Float_t Hy = hit->GetY();
                Float_t Hz = hit->GetZ();

                //3D
                Float_t dist = DistFromPointToLine3D(TVector3(Ax, Ay, Az), TVector3(Vx, Vy, Vz), TVector3(Hx, Hy, Hz));
                if (dchId == 1) {
                    h_dch1_dist_3d->Fill(dist);
                    h_dch1_dist_x->Fill(Hx, dist);
                    h_dch1_dist_y->Fill(Hy, dist);
                    h_dch1_dist_z->Fill(Hz, dist);
                    h_xy1->Fill(Hx, Hy);

                } else if (dchId == 2) {
                    h_dch2_dist_3d->Fill(dist);
                    h_dch2_dist_x->Fill(Hx, dist);
                    h_dch2_dist_y->Fill(Hy, dist);
                    h_dch2_dist_z->Fill(Hz, dist);
                    h_xy2->Fill(Hx, Hy);
                }
            }

            //                TLine* lineZX = new TLine(Vz, Vx, zMax, Tx * (zMax - Vz) + Vx);
            //                fits.push_back(lineZX);
            //            TLine* lineXY = new TLine(Vx, Vy, zMax, Px / Pz * (zMax - Vz) + Vx);
        }
    } // event loop

    //    TCanvas* c3 = new TCanvas("c3", "c3", 800, 800);
    //    h_nHitsInTrack->Draw("");
    //    c3->SaveAs("nHitsInTrack.png");
    //
    //    TCanvas* c4 = new TCanvas("c4", "c4", 1600, 800);
    //    h_zx_tracks->Draw("P");
    //    h_zx_tracks->SetMarkerStyle(24);
    //    h_zx_tracks->SetMarkerSize(0.6);
    //    h_zx_tracks->SetMarkerColor(kBlue);
    //    for (Int_t i = 0; i < fits.size(); ++i) {
    //        fits.at(i)->SetLineColor(kRed);
    //        fits.at(i)->Draw();
    //    }
    //    c4->SaveAs("tracks.png");
}

Float_t DistFromPointToLine2D(Float_t A, Float_t B, Float_t C, Float_t Mx, Float_t My) {
    // Distance from point (Mx, My) to line Ax + By + C = 0 is equal:
    // d = |A * Mx + B * My + C| / Sqrt(A^2 + B^2)
    return Abs(A * Mx + B * My + C) / Sqrt(A * A + B * B);
}

Float_t DistFromPointToLine3D(TVector3 direction, TVector3 vertex, TVector3 hit) {
    TVector3 VertHit = vertex - hit;
    TVector3 VectProd = VertHit.Cross(direction);
    Float_t d = VectProd.Mag() / direction.Mag();
    return d;
}