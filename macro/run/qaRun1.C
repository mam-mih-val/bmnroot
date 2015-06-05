
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

    TChain *bmnRawTree = new TChain("BMN_RAW");
    bmnRawTree->Add(TString::Format("bmn_run0%d_eb+hrb.root", runId));

    TClonesArray *dchTracks;
    dstTree->SetBranchAddress("DchTracks", &dchTracks);
    TClonesArray *dchHits;
    dstTree->SetBranchAddress("BmnDchHit", &dchHits);
    TClonesArray *mwpcTracks0;
    dstTree->SetBranchAddress("Mwpc0Seeds", &mwpcTracks0);
    TClonesArray *mwpcTracks1;
    dstTree->SetBranchAddress("Mwpc1Seeds", &mwpcTracks1);
    TClonesArray *mwpcTracks2;
    dstTree->SetBranchAddress("Mwpc2Seeds", &mwpcTracks2);
    TClonesArray *mwpcHits;
    dstTree->SetBranchAddress("BmnMwpcHit", &mwpcHits);
    TClonesArray *mwpcDigits;
    bmnRawTree->SetBranchAddress("bmn_mwpc", &mwpcDigits);

    Int_t events = (nEvents == 0) ? dstTree->GetEntries() : nEvents;

    for (Int_t iEv = 0; iEv < events; iEv++) {
        dstTree->GetEntry(iEv);
        bmnRawTree->GetEntry(iEv);

        if (iEv % 1000 == 0) cout << "QA: \tRUN#" << runId << "\tEvent: " << iEv + 1 << "/" << events << endl;
        for (Int_t glIdx = 0; glIdx < /*dchTracks->GetEntriesFast()*/ 0; ++glIdx) {
            CbmTrack* track = (CbmTrack*) dchTracks->At(glIdx);
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
        }

        //MWPC DIGI
        Int_t digiCounter0 = 0;
        Int_t digiCounter1 = 0;
        Int_t digiCounter2 = 0;

        Int_t digiCntrPlane0_0 = 0;
        Int_t digiCntrPlane1_0 = 0;
        Int_t digiCntrPlane2_0 = 0;
        Int_t digiCntrPlane3_0 = 0;
        Int_t digiCntrPlane4_0 = 0;
        Int_t digiCntrPlane5_0 = 0;
        Int_t digiCntrPlane0_1 = 0;
        Int_t digiCntrPlane1_1 = 0;
        Int_t digiCntrPlane2_1 = 0;
        Int_t digiCntrPlane3_1 = 0;
        Int_t digiCntrPlane4_1 = 0;
        Int_t digiCntrPlane5_1 = 0;
        Int_t digiCntrPlane0_2 = 0;
        Int_t digiCntrPlane1_2 = 0;
        Int_t digiCntrPlane2_2 = 0;
        Int_t digiCntrPlane3_2 = 0;
        Int_t digiCntrPlane4_2 = 0;
        Int_t digiCntrPlane5_2 = 0;        

        for (Int_t digId = 0; digId < mwpcDigits->GetEntriesFast(); ++digId) {
            BmnMwpcDigit* digi = (BmnMwpcDigit*) mwpcDigits->At(digId);
            Short_t plane = digi->GetPlane();
            Short_t time = digi->GetTime();
//            if (plane == 0) digiCntrPlane0_0++;
//            else if (plane == 1) digiCntrPlane1_0++;
//            else if (plane == 2) digiCntrPlane2_0++;
//            else if (plane == 3) digiCntrPlane3_0++;
//            else if (plane == 4) digiCntrPlane4_0++;
//            else if (plane == 5) digiCntrPlane5_0++;
//            else if (plane == 6) digiCntrPlane0_1++;
//            else if (plane == 7) digiCntrPlane1_1++;
//            else if (plane == 8) digiCntrPlane2_1++;
//            else if (plane == 9) digiCntrPlane3_1++;
//            else if (plane == 10) digiCntrPlane4_1++;
//            else if (plane == 11) digiCntrPlane5_1++;
//            else if (plane == 12) digiCntrPlane0_2++;
//            else if (plane == 13) digiCntrPlane1_2++;
//            else if (plane == 14) digiCntrPlane2_2++;
//            else if (plane == 15) digiCntrPlane3_2++;
//            else if (plane == 16) digiCntrPlane4_2++;
//            else if (plane == 17) digiCntrPlane5_2++;

            if (plane / 6 == 0) {
                h_mwpc1_digit_time->Fill(time);
                digiCounter1++;
            } else if (plane / 6 == 1) {
                h_mwpc0_digit_time->Fill(time);
                digiCounter0++;
            } else if (plane / 6 == 2) {
                h_mwpc2_digit_time->Fill(time);
                digiCounter2++;
            }
            h_mwpc_digit_planes->Fill(plane);
        }
        h_mwpc0_digit_nDigits->Fill(digiCounter0);
        h_mwpc1_digit_nDigits->Fill(digiCounter1);
        h_mwpc2_digit_nDigits->Fill(digiCounter2);

//        h_mwpc0_digit_plane0_nDigits->Fill(digiCntrPlane0_0);
//        h_mwpc0_digit_plane1_nDigits->Fill(digiCntrPlane1_0);
//        h_mwpc0_digit_plane2_nDigits->Fill(digiCntrPlane2_0);
//        h_mwpc0_digit_plane3_nDigits->Fill(digiCntrPlane3_0);
//        h_mwpc0_digit_plane4_nDigits->Fill(digiCntrPlane4_0);
//        h_mwpc0_digit_plane5_nDigits->Fill(digiCntrPlane5_0);
//        h_mwpc1_digit_plane0_nDigits->Fill(digiCntrPlane0_1);
//        h_mwpc1_digit_plane1_nDigits->Fill(digiCntrPlane1_1);
//        h_mwpc1_digit_plane2_nDigits->Fill(digiCntrPlane2_1);
//        h_mwpc1_digit_plane3_nDigits->Fill(digiCntrPlane3_1);
//        h_mwpc1_digit_plane4_nDigits->Fill(digiCntrPlane4_1);
//        h_mwpc1_digit_plane5_nDigits->Fill(digiCntrPlane5_1);
//        h_mwpc2_digit_plane0_nDigits->Fill(digiCntrPlane0_2);
//        h_mwpc2_digit_plane1_nDigits->Fill(digiCntrPlane1_2);
//        h_mwpc2_digit_plane2_nDigits->Fill(digiCntrPlane2_2);
//        h_mwpc2_digit_plane3_nDigits->Fill(digiCntrPlane3_2);
//        h_mwpc2_digit_plane4_nDigits->Fill(digiCntrPlane4_2);
//        h_mwpc2_digit_plane5_nDigits->Fill(digiCntrPlane5_2);

        //MWPC HIT
        Int_t hitCounter0 = 0;
        Int_t hitCounter1 = 0;
        Int_t hitCounter2 = 0;

        for (Int_t hitId = 0; hitId < mwpcHits->GetEntriesFast(); ++hitId) {
            BmnMwpcHit* mwpcHit = (BmnMwpcHit*) mwpcHits->At(hitId);
            Short_t mwpcId = mwpcHit->GetMwpcId();
            Float_t X = mwpcHit->GetX();
            Float_t Y = mwpcHit->GetY();
            Float_t Z = mwpcHit->GetZ();
            if (mwpcId == 0) {
                h_mwpc0_hit_XY->Fill(X, Y);
                h_mwpc0_hit_X->Fill(X);
                h_mwpc0_hit_Y->Fill(Y);
                h_mwpc0_hit_Z->Fill(Z);
                hitCounter0++;
            } else if (mwpcId == 1) {
                h_mwpc1_hit_XY->Fill(X, Y);
                h_mwpc1_hit_X->Fill(X);
                h_mwpc1_hit_Y->Fill(Y);
                h_mwpc1_hit_Z->Fill(Z);
                hitCounter1++;
            } else if (mwpcId == 2) {
                h_mwpc2_hit_XY->Fill(X, Y);
                h_mwpc2_hit_X->Fill(X);
                h_mwpc2_hit_Y->Fill(Y);
                h_mwpc2_hit_Z->Fill(Z);
                hitCounter2++;
            }
        }
        h_mwpc0_hit_nHits->Fill(hitCounter0);
        h_mwpc1_hit_nHits->Fill(hitCounter1);
        h_mwpc2_hit_nHits->Fill(hitCounter2);

        //MWPC TRACK
        for (Int_t trId = 0; trId < mwpcTracks0->GetEntriesFast(); ++trId) {
            CbmTrack* tr0 = (CbmTrack*) mwpcTracks0->At(trId);
            Short_t chi2 = tr0->GetChiSq();
            Short_t nHits = tr0->GetNDF();
            Float_t X = tr0->GetParamFirst()->GetX();
            Float_t Y = tr0->GetParamFirst()->GetY();
            Float_t Z = tr0->GetParamFirst()->GetZ();
            Float_t Tx = tr0->GetParamFirst()->GetTx();
            Float_t Ty = tr0->GetParamFirst()->GetTy();
            h_mwpc0_track_Tx->Fill(Tx);
            h_mwpc0_track_Ty->Fill(Ty);
            h_mwpc0_track_nHits->Fill(nHits);
            h_mwpc0_track_chi2->Fill(chi2);
            h_mwpc0_track_XYstart->Fill(X, Y);
            h_mwpc0_track_Xstart->Fill(X);
            h_mwpc0_track_Ystart->Fill(Y);
        }
        for (Int_t trId = 0; trId < mwpcTracks1->GetEntriesFast(); ++trId) {
            CbmTrack* tr1 = (CbmTrack*) mwpcTracks1->At(trId);
            Short_t chi2 = tr1->GetChiSq();
            Short_t nHits = tr1->GetNDF();
            Float_t X = tr1->GetParamFirst()->GetX();
            Float_t Y = tr1->GetParamFirst()->GetY();
            Float_t Z = tr1->GetParamFirst()->GetZ();
            Float_t Tx = tr1->GetParamFirst()->GetTx();
            Float_t Ty = tr1->GetParamFirst()->GetTy();
            h_mwpc1_track_Tx->Fill(Tx);
            h_mwpc1_track_Ty->Fill(Ty);
            h_mwpc1_track_nHits->Fill(nHits);
            h_mwpc1_track_chi2->Fill(chi2);
            h_mwpc1_track_XYstart->Fill(X, Y);
            h_mwpc1_track_Xstart->Fill(X);
            h_mwpc1_track_Ystart->Fill(Y);
        }
        for (Int_t trId = 0; trId < mwpcTracks2->GetEntriesFast(); ++trId) {
            CbmTrack* tr2 = (CbmTrack*) mwpcTracks2->At(trId);
            Short_t chi2 = tr2->GetChiSq();
            Short_t nHits = tr2->GetNDF();
            Float_t X = tr2->GetParamFirst()->GetX();
            Float_t Y = tr2->GetParamFirst()->GetY();
            Float_t Z = tr2->GetParamFirst()->GetZ();
            Float_t Tx = tr2->GetParamFirst()->GetTx();
            Float_t Ty = tr2->GetParamFirst()->GetTy();
            h_mwpc2_track_Tx->Fill(Tx);
            h_mwpc2_track_Ty->Fill(Ty);
            h_mwpc2_track_nHits->Fill(nHits);
            h_mwpc2_track_chi2->Fill(chi2);
            h_mwpc2_track_XYstart->Fill(X, Y);
            h_mwpc2_track_Xstart->Fill(X);
            h_mwpc2_track_Ystart->Fill(Y);
        }

        h_mwpc0_track_nTracks->Fill(mwpcTracks0->GetEntriesFast());
        h_mwpc1_track_nTracks->Fill(mwpcTracks1->GetEntriesFast());
        h_mwpc2_track_nTracks->Fill(mwpcTracks2->GetEntriesFast());


    } // event loop    
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