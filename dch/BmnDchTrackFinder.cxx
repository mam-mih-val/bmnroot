// @(#)bmnroot/dch:$Id$
// Author: Pavel Batyuk (VBLHEP) <pavel.batyuk@jinr.ru> 2017-01-21

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// BmnDchTrackFinder                                                          //
//                                                                            //
// Implementation of an algorithm developed by                                // 
// N.Voytishin and V.Paltchik (LIT)                                           //
// to the BmnRoot software                                                    //
//                                                                            //
// The algorithm serves for searching for track segments                      //
// in the Drift Chambers of the BM@N experiment                               //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#include "BmnDchTrackFinder.h"

static Float_t workTime = 0.0;

BmnDchTrackFinder::BmnDchTrackFinder(Bool_t isExp) :
fSegmentMatching(kFALSE),
has7DC1(kFALSE),
has7DC2(kFALSE),
expData(isExp) {
    fEventNo = 0;
    tracksDch = "BmnDchTrack";
    InputDigitsBranchName = "DCH";

    prev_wire = -1;
    prev_time = -1;

    nChambers = 2;
    nWires = 4;
    nLayers = 2;
    nSegmentsMax = 100;

    Z_dch1 = 529.65;
    Z_dch2 = 727.65;
    Z_dch_mid = 628.65;

    x1_sh = 15.0;
    x2_sh = 12.2;
    y1_sh = 0.2;
    y2_sh = -0.3;

    x1_slope_sh = -0.08;
    y1_slope_sh = 0.051;
    x2_slope_sh = -0.08;
    y2_slope_sh = 0.055;

    scale = 0.5;
}

BmnDchTrackFinder::~BmnDchTrackFinder() {

}

void BmnDchTrackFinder::Exec(Option_t* opt) {
    clock_t tStart = clock();
    PrepareArraysToProcessEvent();
    cout << "\n======================== DCH track finder exec started =====================\n" << endl;
    cout << "Event number: " << fEventNo++ << endl;

    //temporary containers
    // Order used: va1, vb1, ua1, ub1, ya1, yb1, xa1, xb1 (dch1, 0 - 7) - va2, vb2, ua2, ub2, ya2, yb2, xa2, xb2 (dch2, 8 - 15)
    const Int_t nDim = 20;
    const Int_t nPlanes = 16; // Total number of planes in both DCHs (0-7, 8-15)

    Double_t times[nPlanes][nDim];
    Double_t wires[nPlanes][nDim];
    Int_t it[nPlanes];
    Bool_t used[nPlanes][nDim];
    for (Int_t iPlanes = 0; iPlanes < nPlanes; iPlanes++) {
        it[iPlanes] = 0;
        for (Int_t iDim = 0; iDim < nDim; iDim++) {
            used[iPlanes][iDim] = kFALSE;
            times[iPlanes][iDim] = 0.;
            wires[iPlanes][iDim] = 0.;
        }
    }

    Bool_t goodEv = kTRUE;
    Bool_t written = kFALSE;

    for (Int_t iDig = 0; iDig < fBmnDchDigitsArray->GetEntriesFast(); ++iDig) {
        BmnDchDigit* digit = (BmnDchDigit*) fBmnDchDigitsArray->UncheckedAt(iDig);

        //skip identical events
        if (!written) {
            written = kTRUE;
            if (digit->GetTime() == prev_time && digit->GetWireNumber() == prev_wire) {
                goodEv = kFALSE;
            } else {
                prev_time = Int_t(digit->GetTime());
                prev_wire = Int_t(digit->GetWireNumber());
            }
        }//!written

        if (!goodEv)
            return;

        // Order used: va1(0), vb1(1), ua1(2), ub1(3), ya1(4), yb1(5), xa1(6), xb1(7) -> 
        //             va2(8), vb2(9), ua2(10), ub2(11), ya2(12), yb2(13), xa2(14), xb2(15)
        Short_t plane = digit->GetPlane();
        Short_t wire = digit->GetWireNumber();
        Double_t time = digit->GetTime();
        Bool_t secondaries = kFALSE;

        for (Int_t sec = 0; sec < it[plane] - 1; sec++)
            if (wire == wires[plane][sec]) {
                secondaries = kTRUE;
                break;
            }

        if (it[plane] == (nDim - 1) || secondaries)
            continue;

        wires[plane][it[plane]] = wire;
        times[plane][it[plane]] = time;
        it[plane]++;
    }

    TString wireNames[nWires] = {"v", "u", "y", "x"};
    TString layNames[nLayers] = {"a", "b"};
    Int_t cntr = 0;

    for (Int_t iDch = 0; iDch < nChambers; iDch++) {
        for (Int_t iWire = 0; iWire < nWires; iWire++) {
            Int_t start = 2 * iWire + (nPlanes / 2) * iDch;
            Int_t finish = start + 1;
            Float_t*** coord = (iWire == 0) ? v : (iWire == 1) ? u : (iWire == 2) ? y : x;
            Float_t*** sigma = (iWire == 0) ? sigm_v : (iWire == 1) ? sigm_u : (iWire == 2) ? sigm_y : sigm_x;

            pairs[iDch][iWire] = Reconstruction(iDch + 1, wireNames[iWire], pairs[iDch][iWire], it[start], it[finish],
                    wires[start], wires[finish], times[start], times[finish], used[start], used[finish],
                    coord[iDch], sigma[iDch]);

            for (Int_t iLayer = 0; iLayer < nLayers; iLayer++) {
                Float_t*** single_coord = (iWire == 0) ? v_Single : (iWire == 1) ? u_Single : (iWire == 2) ? y_Single : x_Single;
                Float_t*** single_sigma = (iWire == 0) ? Sigm_v_single : (iWire == 1) ? Sigm_u_single : (iWire == 2) ? Sigm_y_single : Sigm_x_single;
                singles[iDch][iWire][iLayer] = ReconstructionSingle(iDch + 1, wireNames[iWire], layNames[iLayer],
                        singles[iDch][iWire][iLayer], it[cntr], wires[cntr], times[cntr], used[cntr], single_coord[iDch], single_sigma[iDch]);
                cntr++;
            }
        }
        nSegments[iDch] = BuildUVSegments(iDch + 1,
                pairs[iDch][1], pairs[iDch][0], pairs[iDch][3], pairs[iDch][2], singles[iDch][1][0], singles[iDch][1][1], singles[iDch][0][0], singles[iDch][0][1],
                x[iDch], y[iDch], u[iDch], v[iDch], sigm_x[iDch], sigm_y[iDch], sigm_u[iDch], sigm_v[iDch], rh_segment[iDch], rh_sigm_segment[iDch],
                u_Single[iDch], v_Single[iDch], Sigm_u_single[iDch], Sigm_v_single[iDch]);

        nSegments[iDch] = BuildXYSegments(iDch + 1,
                pairs[iDch][1], pairs[iDch][0], pairs[iDch][3], pairs[iDch][2], singles[iDch][3][0], singles[iDch][3][1], singles[iDch][2][0], singles[iDch][2][1],
                x[iDch], y[iDch], u[iDch], v[iDch], sigm_x[iDch], sigm_y[iDch], sigm_u[iDch], sigm_v[iDch], rh_segment[iDch], rh_sigm_segment[iDch],
                x_Single[iDch], y_Single[iDch], Sigm_x_single[iDch], Sigm_y_single[iDch]);

        FitDchSegments(iDch + 1, segment_size[iDch], rh_segment[iDch], rh_sigm_segment[iDch], params[iDch], Chi2[iDch], x_global[iDch], y_global[iDch]); // Fit found segments
        SelectLongestAndBestSegments(iDch + 1, segment_size[iDch], rh_segment[iDch], Chi2[iDch]); // Save only longest and best chi2 segments
        CreateDchTrack(iDch + 1, Chi2[iDch], params[iDch], segment_size[iDch]); // Fill segment parameters
    }

    // Try to match the reconstructed segments from the two chambers
    // Not implemented yet (TO DO A.S.A.P.)
    cout << "\n======================== DCH track finder exec finished ========================" << endl;
    clock_t tFinish = clock();
    workTime += ((Float_t) (tFinish - tStart)) / CLOCKS_PER_SEC;
}

Int_t BmnDchTrackFinder::BuildXYSegments(Int_t dchID,
        Int_t pairU, Int_t pairV, Int_t pairX, Int_t pairY, Int_t single_xa, Int_t single_xb, Int_t single_ya, Int_t single_yb,
        Float_t** x_ab, Float_t** y_ab, Float_t** u_ab, Float_t** v_ab,
        Float_t** sigm_x_ab, Float_t** sigm_y_ab, Float_t** sigm_u_ab, Float_t** sigm_v_ab,
        Float_t** rh_seg, Float_t** rh_sigm_seg,
        Float_t** x_single, Float_t** y_single, Float_t** sigm_x_single, Float_t** sigm_y_single) {

    Double_t sqrt_2 = sqrt(2.);
    Double_t isqrt_2 = 1. / sqrt_2;

    Int_t nDC_segments = (dchID == 1) ? nSegments[0] : nSegments[1];
    for (Int_t i = 0; i < pairU; i++) {
        if (nDC_segments > 48)
            break;
        Float_t u_coord = (u_ab[0][i] + u_ab[1][i]) / 2;

        for (Int_t j = 0; j < pairV; j++) {
            if (nDC_segments > 48)
                break;
            Float_t v_coord = (v_ab[0][j] + v_ab[1][j]) / 2;
            Bool_t foundX = kFALSE;
            Float_t x_est = isqrt_2 * (v_coord - u_coord);
            Float_t y_est = isqrt_2 * (u_coord + v_coord);
            if (pairX > 0) {
                Double_t dX_thresh = 1.5;
                for (Int_t k = 0; k < pairX; k++) {
                    Float_t x_coord = (x_ab[0][k] + x_ab[1][k]) / 2;
                    if (nDC_segments > 48)
                        break;
                    if (Abs(x_coord - x_est) > dX_thresh)
                        continue;
                    dX_thresh = Abs(x_coord - x_est);

                    rh_seg[0][nDC_segments] = x_ab[0][k];
                    rh_seg[1][nDC_segments] = x_ab[1][k];
                    rh_seg[4][nDC_segments] = u_ab[0][i];
                    rh_seg[5][nDC_segments] = u_ab[1][i];
                    rh_seg[6][nDC_segments] = v_ab[0][j];
                    rh_seg[7][nDC_segments] = v_ab[1][j];
                    rh_sigm_seg[0][nDC_segments] = sigm_x_ab[0][k];
                    rh_sigm_seg[1][nDC_segments] = sigm_x_ab[1][k];
                    rh_sigm_seg[4][nDC_segments] = sigm_u_ab[0][i];
                    rh_sigm_seg[5][nDC_segments] = sigm_u_ab[1][i];
                    rh_sigm_seg[6][nDC_segments] = sigm_v_ab[0][j];
                    rh_sigm_seg[7][nDC_segments] = sigm_v_ab[1][j];

                    foundX = kTRUE;
                    if (nDC_segments > 48)
                        break;
                }//k
            }//(pair_x2>0)

            Bool_t foundY = kFALSE;
            if (pairY > 0) {
                Double_t dY_thresh = 1.0;
                for (Int_t m = 0; m < pairY; m++) {
                    if (nDC_segments > 48)
                        break;
                    Float_t y_coord = (y_ab[0][m] + y_ab[1][m]) / 2;
                    if (Abs(y_coord - y_est) > dY_thresh)
                        continue;
                    dY_thresh = Abs(y_coord - y_est);
                    foundY = kTRUE;
                    rh_seg[2][nDC_segments] = y_ab[0][m];
                    rh_seg[3][nDC_segments] = y_ab[1][m];
                    rh_seg[4][nDC_segments] = u_ab[0][i];
                    rh_seg[5][nDC_segments] = u_ab[1][i];
                    rh_seg[6][nDC_segments] = v_ab[0][j];
                    rh_seg[7][nDC_segments] = v_ab[1][j];
                    rh_sigm_seg[2][nDC_segments] = sigm_y_ab[0][m];
                    rh_sigm_seg[3][nDC_segments] = sigm_y_ab[1][m];
                    rh_sigm_seg[4][nDC_segments] = sigm_u_ab[0][i];
                    rh_sigm_seg[5][nDC_segments] = sigm_u_ab[1][i];
                    rh_sigm_seg[6][nDC_segments] = sigm_v_ab[0][j];
                    rh_sigm_seg[7][nDC_segments] = sigm_v_ab[1][j];
                    if (!foundX) {
                        Float_t min_a = 999;
                        Float_t min_b = 999;
                        for (Int_t kk = 0; kk < single_xa; kk++) {
                            if (Abs(x_single[1][kk] - x_est) > 1.5)
                                continue; //????? 0.5 needs to be reviewed

                            if (Abs(x_single[0][kk] - x_est) < min_a) {
                                min_a = Abs(x_single[0][kk] - x_est);
                                rh_seg[0][nDC_segments] = x_single[0][kk];
                                rh_sigm_seg[0][nDC_segments] = sigm_x_single[0][kk];
                                foundX = kTRUE;
                            }
                        }//for kk                                                                                                                                                           
                        for (Int_t kk = 0; kk < single_xb; kk++) {

                            if (Abs(x_single[1][kk] - x_est) > 1.5)
                                continue; //????? 0.5 needs to be reviewed                                                                                     
                            if (Abs(x_single[1][kk] - x_est) < min_b) {
                                min_b = Abs(x_single[1][kk] - x_est);
                                rh_seg[1][nDC_segments] = x_single[1][kk];
                                rh_sigm_seg[1][nDC_segments] = sigm_x_single[1][kk];
                                foundX = kTRUE;
                            }
                        }//for kk 
                        if (nDC_segments > 48)
                            break;
                    }//!foundX
                }//m         
                if (foundX && !foundY) {
                    Float_t min_a = 999;
                    Float_t min_b = 999;
                    for (Int_t kk = 0; kk < single_ya; kk++) {
                        if (Abs(y_single[0][kk] - y_est) > 1.5)
                            continue; //????? 0.5 needs to be reviewed                                                                                     
                        if (Abs(y_single[0][kk] - y_est) < min_a) {
                            min_a = Abs(y_single[0][kk] - y_est);
                            rh_seg[2][nDC_segments] = y_single[0][kk];
                            rh_sigm_seg[2][nDC_segments] = sigm_y_single[0][kk];
                            foundY = kTRUE;
                        }
                    }//for kk                                                                                                                                                          
                    for (Int_t kk = 0; kk < single_yb; kk++) {
                        if (Abs(y_single[1][kk] - y_est) > 1.5)
                            continue; //????? 0.5 needs to be reviewed                                                                                     
                        if (Abs(y_single[1][kk] - y_est) < min_b) {
                            min_b = Abs(y_single[1][kk] - y_est);
                            rh_seg[3][nDC_segments] = y_single[1][kk];
                            rh_sigm_seg[3][nDC_segments] = sigm_y_single[1][kk];
                            foundY = kTRUE;
                        }
                    }//for kk 
                }
            }//(pair_y2>0)
            if (foundX || foundY) nDC_segments++;
        }
    }
    return nDC_segments;
}

Int_t BmnDchTrackFinder::BuildUVSegments(Int_t dchID, Int_t pairU, Int_t pairV, Int_t pairX, Int_t pairY, Int_t single_ua, Int_t single_ub, Int_t single_va, Int_t single_vb,
        Float_t** x_ab, Float_t** y_ab, Float_t** u_ab, Float_t** v_ab,
        Float_t** sigm_x_ab, Float_t** sigm_y_ab, Float_t** sigm_u_ab, Float_t** sigm_v_ab,
        Float_t** rh_seg, Float_t** rh_sigm_seg,
        Float_t** u_single, Float_t** v_single, Float_t** sigm_u_single, Float_t** sigm_v_single) {

    Double_t sqrt_2 = sqrt(2.);
    Double_t isqrt_2 = 1. / sqrt_2;

    Int_t nDC_segments = (dchID == 1) ? nSegments[0] : nSegments[1];

    for (Int_t i = 0; i < pairX; i++) {
        if (nDC_segments > 48)
            break;
        Float_t x_coord = (x_ab[0][i] + x_ab[1][i]) / 2;
        Float_t XU = x_coord;
        Float_t XV = x_coord;

        for (Int_t j = 0; j < pairY; j++) {
            Float_t y_coord = (y_ab[0][j] + y_ab[1][j]) / 2;
            Float_t YU = y_coord;
            Float_t YV = y_coord;
            Bool_t foundU = kFALSE;
            Float_t u_est = isqrt_2 * (YU - XU);
            Float_t v_est = isqrt_2 * (YV + XV);

            Double_t dU_thresh = 1.3;
            for (Int_t k = 0; k < pairU; k++) {
                Float_t u_coord = (u_ab[0][k] + u_ab[1][k]) / 2;

                if (Abs(u_coord - u_est) > dU_thresh)
                    continue;
                dU_thresh = Abs(u_coord - u_est);

                rh_seg[0][nDC_segments] = x_ab[0][i];
                rh_seg[1][nDC_segments] = x_ab[1][i];
                rh_seg[2][nDC_segments] = y_ab[0][j];
                rh_seg[3][nDC_segments] = y_ab[1][j];
                rh_seg[4][nDC_segments] = u_ab[0][k];
                rh_seg[5][nDC_segments] = u_ab[1][k];
                rh_sigm_seg[0][nDC_segments] = sigm_x_ab[0][i];
                rh_sigm_seg[1][nDC_segments] = sigm_x_ab[1][i];
                rh_sigm_seg[2][nDC_segments] = sigm_y_ab[0][j];
                rh_sigm_seg[3][nDC_segments] = sigm_y_ab[1][j];
                rh_sigm_seg[4][nDC_segments] = sigm_u_ab[0][k];
                rh_sigm_seg[5][nDC_segments] = sigm_u_ab[1][k];

                foundU = kTRUE;
                if (nDC_segments > 48)
                    break;
            }

            Bool_t foundV = kFALSE;

            Double_t dV_thresh = 1.3;
            for (Int_t m = 0; m < pairV; m++) {
                if (nDC_segments > 48)
                    break;
                Float_t v_coord = (v_ab[0][m] + v_ab[1][m]) / 2;

                if (Abs(v_coord - v_est) > dV_thresh)
                    continue;
                dV_thresh = Abs(v_coord - v_est);

                foundV = kTRUE;
                rh_seg[0][nDC_segments] = x_ab[0][i];
                rh_seg[1][nDC_segments] = x_ab[1][i];
                rh_seg[2][nDC_segments] = y_ab[0][j];
                rh_seg[3][nDC_segments] = y_ab[1][j];
                rh_seg[6][nDC_segments] = v_ab[0][m];
                rh_seg[7][nDC_segments] = v_ab[1][m];
                rh_sigm_seg[0][nDC_segments] = sigm_x_ab[0][i];
                rh_sigm_seg[1][nDC_segments] = sigm_x_ab[1][i];
                rh_sigm_seg[2][nDC_segments] = sigm_y_ab[0][j];
                rh_sigm_seg[3][nDC_segments] = sigm_y_ab[1][j];
                rh_sigm_seg[6][nDC_segments] = sigm_v_ab[0][m];
                rh_sigm_seg[7][nDC_segments] = sigm_v_ab[1][m];

                if (!foundU) {
                    Float_t min_a = 999;
                    Float_t min_b = 999;
                    for (Int_t kk = 0; kk < single_ua; kk++) {
                        if (Abs(u_single[0][kk] - u_est) > 1.5)
                            continue; //????? 0.5 needs to be reviewed
                        if (Abs(u_single[0][kk] - u_est) < min_a) {
                            min_a = Abs(u_single[0][kk] - u_est);
                            rh_seg[4][nDC_segments] = u_single[0][kk];
                            rh_sigm_seg[4][nDC_segments] = sigm_u_single[0][kk];
                            foundU = kTRUE;
                        }
                    }//for kk
                    for (Int_t kk = 0; kk < single_ub; kk++) {
                        if (Abs(u_single[1][kk] - u_est) > 1.5)
                            continue; //????? 0.5 needs to be reviewed
                        if (Abs(u_single[1][kk] - u_est) < min_b) {
                            min_b = Abs(u_single[1][kk] - u_est);
                            rh_seg[5][nDC_segments] = u_single[1][kk];
                            rh_sigm_seg[5][nDC_segments] = sigm_u_single[1][kk];
                            foundU = kTRUE;
                        }
                    }//for kk
                    if (nDC_segments > 48)
                        break;
                }//!foundU

                if (nDC_segments > 48)
                    break;
            }//m
            //            }//(pair_v2>0)
            if (!foundV && foundU) {
                Float_t min_a = 999;
                Float_t min_b = 999;
                for (Int_t kk = 0; kk < single_va; kk++) {
                    if (Abs(v_single[0][kk] - v_est) > 1.5)
                        continue; //????? 0.5 needs to be reviewed                                                                                     
                    if (Abs(v_single[0][kk] - v_est) < min_a) {
                        min_a = Abs(v_single[0][kk] - v_est);
                        rh_seg[6][nDC_segments] = v_single[0][kk];
                        rh_sigm_seg[6][nDC_segments] = sigm_v_single[0][kk];
                        foundV = kTRUE;
                    }
                }//for kk                                                                                                                                                           
                for (Int_t kk = 0; kk < single_vb; kk++) {
                    if (Abs(v_single[1][kk] - v_est) > 1.5)
                        continue; //????? 0.5 needs to be reviewed                                                                                     
                    if (Abs(v_single[1][kk] - v_est) < min_b) {
                        min_b = Abs(v_single[1][kk] - v_est);
                        rh_seg[7][nDC_segments] = v_single[1][kk];
                        rh_sigm_seg[7][nDC_segments] = sigm_v_single[1][kk];
                        foundV = kTRUE;
                    }
                }
            }
            if (foundV || foundU) nDC_segments++;
        }
    }
    return nDC_segments;
}

void BmnDchTrackFinder::FitDchSegments(Int_t dchID, Int_t* size_seg, Float_t** rh_seg, Float_t** rh_sigm_seg, Float_t** par_ab, Float_t* chi2, Float_t* x_glob, Float_t* y_glob) {
    Int_t nDC_segments = (dchID == 1) ? nSegments[0] : nSegments[1];
    for (Int_t j = 0; j < nDC_segments; j++) {
        Int_t worst_hit = -1;
        Double_t max_resid = 0;

        Float_t _rh_seg[8];
        Float_t _rh_sigm_seg[8];
        Float_t _par_ab[4];

        for (Int_t i = 0; i < 8; i++)
            if (Abs(rh_seg[i][j] + 999.) > FLT_EPSILON)
                size_seg[j]++;

        for (Int_t rej = 0; rej < 2; rej++) {//allow 2 passes max 8->7 & 7->6
            for (Int_t i = 0; i < 8; i++) {
                _rh_seg[i] = rh_seg[i][j];
                _rh_sigm_seg[i] = rh_sigm_seg[i][j];
            }

            fit_seg(z_loc, _rh_seg, _rh_sigm_seg, _par_ab, -1, -1); //usual fit without skipping any plane
            for (Int_t i = 0; i < 4; i++)
                par_ab[i][j] = _par_ab[i];

            chi2[j] = 0;

            Float_t resid(LDBL_MAX);
            for (Int_t i = 0; i < 8; i++) {
                if (Abs(rh_seg[i][j] + 999.) < FLT_EPSILON)
                    continue;

                resid = CalculateResidual(i, j, rh_seg, par_ab);
                chi2[j] += (resid * resid) / rh_sigm_seg[i][j];
                if (Abs(resid) > max_resid) {
                    worst_hit = i;
                    max_resid = Abs(resid);
                }
            }

            chi2[j] /= (size_seg[j] - 4);

            //if chi2 is big and seg_size = min erase this seg
            if (chi2[j] > 30.)
                if (size_seg[j] == 6) {
                    chi2[j] = 999.;
                    break;
                } else {
                    rh_seg[worst_hit][j] = -999.; //erase worst hit and refit
                    size_seg[j]--;
                    max_resid = 0;
                    continue;
                }
        }

        // Add shifts to slopes and coords
        Float_t x_slope_sh = (dchID == 1) ? x1_slope_sh : x2_slope_sh;
        Float_t y_slope_sh = (dchID == 1) ? y1_slope_sh : y2_slope_sh;
        Float_t x_sh = (dchID == 1) ? x1_sh : x2_sh;
        Float_t y_sh = (dchID == 1) ? y1_sh : y2_sh;

        par_ab[0][j] += x_slope_sh + x_slope_sh * par_ab[0][j] * par_ab[0][j];
        par_ab[2][j] += y_slope_sh + y_slope_sh * par_ab[2][j] * par_ab[2][j];
        par_ab[1][j] += x_sh;
        par_ab[3][j] += y_sh;

        x_glob[j] = par_ab[0][j]*(-99.5) + par_ab[1][j];
        y_glob[j] = par_ab[2][j]*(-99.5) + par_ab[3][j];

        Bool_t has7DC = (dchID == 1) ? has7DC1 : has7DC2;
        if (size_seg[j] > 6)
            has7DC = kTRUE;
    }
}

void BmnDchTrackFinder::CompareDaDb(Float_t d, Float_t& ele) {
    ele = (d < 0.02) ? (0.08 * 0.08) :
            (d >= 0.02 && d < 0.1) ? (0.06 * 0.06) :
            (d >= 0.1 && d < 0.4) ? (0.025 * 0.025) :
            (d >= 0.4 && d < 0.41) ? (0.08 * 0.08) :
            (0.10 * 0.10);
}

void BmnDchTrackFinder::CompareDaDb(Float_t d, Float_t& ele1, Float_t& ele2) {
    ele1 = (d < 0.02) ? (0.08 * 0.08) :
            (d >= 0.02 && d < 0.1) ? (0.06 * 0.06) :
            (d >= 0.1 && d < 0.4) ? (0.025 * 0.025) :
            (d >= 0.4 && d < 0.41) ? (0.08 * 0.08) :
            (0.10 * 0.10);
    ele2 = ele1;
}

void BmnDchTrackFinder::SelectLongestAndBestSegments(Int_t dchID, Int_t* size_seg, Float_t** rh_seg, Float_t* chi2) {
    Int_t nDC_segments = (dchID == 1) ? nSegments[0] : nSegments[1];
    for (Int_t max_size = 8; max_size > 5; max_size--)
        for (Int_t it1 = 0; it1 < nDC_segments; it1++) {
            if (size_seg[it1] != max_size)
                continue;
            for (Int_t it2 = 0; it2 < nDC_segments; it2++) {
                if (it2 == it1)
                    continue;
                for (Int_t hit = 0; hit < 4; hit++)
                    if (rh_seg[2 * hit][it1] == rh_seg[2 * hit][it2] &&
                            rh_seg[2 * hit + 1][it1] == rh_seg[2 * hit + 1][it2] &&
                            (chi2[it1] <= chi2[it2] || size_seg[it1] > size_seg[it2])) {
                        chi2[it2] = 999.;
                        break;
                    }
            }
        }
}

void BmnDchTrackFinder::CreateDchTrack(Int_t dchID, Float_t* chi2Arr, Float_t** parArr, Int_t* sizeArr) {
    Int_t nDC_segments = (dchID == 1) ? nSegments[0] : nSegments[1];
    for (Int_t iSegment = 0; iSegment < nDC_segments; iSegment++) {
        if (chi2Arr[iSegment] > 50)
            continue;
        FairTrackParam trackParam;
        Float_t z0 = (dchID == 1) ? Z_dch1 : Z_dch2;
        Float_t x0 = parArr[1][iSegment];
        Float_t y0 = parArr[3][iSegment];
        trackParam.SetPosition(TVector3(-x0, y0, z0));
        trackParam.SetTx(-parArr[0][iSegment]);
        trackParam.SetTy(parArr[2][iSegment]);

        BmnDchTrack* track = new((*fDchTracks)[fDchTracks->GetEntriesFast()]) BmnDchTrack();
        track->SetChi2(chi2Arr[iSegment]);
        track->SetNHits(sizeArr[iSegment]);
        track->SetParamFirst(trackParam);
    }
}

Float_t BmnDchTrackFinder::CalculateResidual(Int_t i, Int_t j, Float_t** rh_seg, Float_t** par_ab) {
    Double_t sqrt_2 = sqrt(2.);
    Double_t isqrt_2 = 1 / sqrt_2;

    return (i < 2) ? rh_seg[i][j] - z_loc[i] * par_ab[0][j] - par_ab[1][j] :
            (i >= 2 && i < 4) ? rh_seg[i][j] - z_loc[i] * par_ab[2][j] - par_ab[3][j] :
            (i >= 4 && i < 6) ? rh_seg[i][j] - isqrt_2 * z_loc[i] * (par_ab[2][j] - par_ab[0][j]) - isqrt_2 * (par_ab[3][j] - par_ab[1][j]) :
            rh_seg[i][j] - isqrt_2 * z_loc[i] * (par_ab[2][j] + par_ab[0][j]) - isqrt_2 * (par_ab[3][j] + par_ab[1][j]);
}

InitStatus BmnDchTrackFinder::Init() {
    if (!expData)
        return kERROR;
    cout << "BmnDchTrackFinder::Init()" << endl;
    FairRootManager* ioman = FairRootManager::Instance();

    fBmnDchDigitsArray = (TClonesArray*) ioman->GetObject(InputDigitsBranchName);

    fDchTracks = new TClonesArray(tracksDch.Data());
    ioman->Register(tracksDch.Data(), "DCH", fDchTracks, kTRUE);

    ifstream fin;
    TString dir = getenv("VMCWORKDIR");
    dir += "/input/";
    fin.open((TString(dir + "transfer_func.txt")).Data(), ios::in);
    for (Int_t fi = 0; fi < 16; fi++)
        fin >> t_dc[0][fi] >> t_dc[1][fi] >> t_dc[2][fi] >> t_dc[3][fi] >> t_dc[4][fi] >>
            pol_par_dc[0][0][fi] >> pol_par_dc[0][1][fi] >> pol_par_dc[0][2][fi] >> pol_par_dc[0][3][fi] >> pol_par_dc[0][4][fi] >>
            pol_par_dc[1][0][fi] >> pol_par_dc[1][1][fi] >> pol_par_dc[1][2][fi] >> pol_par_dc[1][3][fi] >> pol_par_dc[1][4][fi] >>
            pol_par_dc[2][0][fi] >> pol_par_dc[2][1][fi] >> pol_par_dc[2][2][fi] >> pol_par_dc[2][3][fi] >> pol_par_dc[2][4][fi] >>
            scaling[fi];
    fin.close();

    // z local xa->vb (cm) 
    Double_t arr1[4 * N] = {9.3, 8.1, 3.5, 2.3, -2.3, -3.5, -8.1, -9.3};
    for (Int_t iSize = 0; iSize < 4 * N; iSize++)
        z_loc[iSize] = arr1[iSize];

    // z global dc 1 & dc 2 (cm)
    Double_t arr2[8 * N] = {-45.7, -46.9, -51.5, -52.7, -57.3, -58.5, -63.1, -64.3, 64.3, 63.1, 58.5, 57.3, 52.7, 51.5, 46.9, 45.7};
    for (Int_t iSize = 0; iSize < 8 * N; iSize++)
        z_glob[iSize] = arr2[iSize];

    v = new Float_t**[nChambers];
    u = new Float_t**[nChambers];
    y = new Float_t**[nChambers];
    x = new Float_t**[nChambers];
    v_Single = new Float_t**[nChambers];
    u_Single = new Float_t**[nChambers];
    y_Single = new Float_t**[nChambers];
    x_Single = new Float_t**[nChambers];
    sigm_v = new Float_t**[nChambers];
    sigm_u = new Float_t**[nChambers];
    sigm_y = new Float_t**[nChambers];
    sigm_x = new Float_t**[nChambers];
    Sigm_v_single = new Float_t**[nChambers];
    Sigm_u_single = new Float_t**[nChambers];
    Sigm_y_single = new Float_t**[nChambers];
    Sigm_x_single = new Float_t**[nChambers];
    segment_size = new Int_t*[nChambers];
    Chi2 = new Float_t*[nChambers];
    x_global = new Float_t*[nChambers];
    y_global = new Float_t*[nChambers];
    params = new Float_t**[nChambers];
    rh_segment = new Float_t**[nChambers];
    rh_sigm_segment = new Float_t**[nChambers];
    for (Int_t iChamber = 0; iChamber < nChambers; iChamber++) {
        v[iChamber] = new Float_t*[N];
        u[iChamber] = new Float_t*[N];
        y[iChamber] = new Float_t*[N];
        x[iChamber] = new Float_t*[N];
        v_Single[iChamber] = new Float_t*[N];
        u_Single[iChamber] = new Float_t*[N];
        y_Single[iChamber] = new Float_t*[N];
        x_Single[iChamber] = new Float_t*[N];
        sigm_v[iChamber] = new Float_t*[N];
        sigm_u[iChamber] = new Float_t*[N];
        sigm_y[iChamber] = new Float_t*[N];
        sigm_x[iChamber] = new Float_t*[N];
        Sigm_v_single[iChamber] = new Float_t*[N];
        Sigm_u_single[iChamber] = new Float_t*[N];
        Sigm_y_single[iChamber] = new Float_t*[N];
        Sigm_x_single[iChamber] = new Float_t*[N];
        segment_size[iChamber] = new Int_t[75 * N];
        Chi2[iChamber] = new Float_t[75 * N];
        x_global[iChamber] = new Float_t[75 * N];
        y_global[iChamber] = new Float_t[75 * N];
        params[iChamber] = new Float_t*[2 * N];
        rh_segment[iChamber] = new Float_t*[4 * N];
        rh_sigm_segment[iChamber] = new Float_t*[4 * N];
        for (Int_t iDim = 0; iDim < N; iDim++) {
            v[iChamber][iDim] = new Float_t[75 * N];
            u[iChamber][iDim] = new Float_t[75 * N];
            y[iChamber][iDim] = new Float_t[75 * N];
            x[iChamber][iDim] = new Float_t[75 * N];
            v_Single[iChamber][iDim] = new Float_t[20 * N];
            u_Single[iChamber][iDim] = new Float_t[20 * N];
            y_Single[iChamber][iDim] = new Float_t[20 * N];
            x_Single[iChamber][iDim] = new Float_t[20 * N];
            sigm_v[iChamber][iDim] = new Float_t[75 * N];
            sigm_u[iChamber][iDim] = new Float_t[75 * N];
            sigm_y[iChamber][iDim] = new Float_t[75 * N];
            sigm_x[iChamber][iDim] = new Float_t[75 * N];
            Sigm_v_single[iChamber][iDim] = new Float_t[20 * N];
            Sigm_u_single[iChamber][iDim] = new Float_t[20 * N];
            Sigm_y_single[iChamber][iDim] = new Float_t[20 * N];
            Sigm_x_single[iChamber][iDim] = new Float_t[20 * N];
        }
        for (Int_t iDim = 0; iDim < 2 * N; iDim++)
            params[iChamber][iDim] = new Float_t[75 * N];
        for (Int_t iDim = 0; iDim < 4 * N; iDim++) {
            rh_segment[iChamber][iDim] = new Float_t[75 * N];
            rh_sigm_segment[iChamber][iDim] = new Float_t[75 * N];
        }
    }
    pairs = new Int_t*[nChambers];
    for (Int_t iChamber = 0; iChamber < nChambers; iChamber++)
        pairs[iChamber] = new Int_t[nWires];
    singles = new Int_t**[nChambers];
    for (Int_t iChamber = 0; iChamber < nChambers; iChamber++) {
        singles[iChamber] = new Int_t*[nWires];
        for (Int_t iWire = 0; iWire < nWires; iWire++)
            singles[iChamber][iWire] = new Int_t[nLayers];
    }
    nSegments = new Int_t[nSegmentsMax];
    return kSUCCESS;
}

void BmnDchTrackFinder::PrepareArraysToProcessEvent() {
    fDchTracks->Clear();
    has7DC1 = kFALSE;
    has7DC2 = kFALSE;

    // Array cleaning and initializing 
    for (Int_t iChamber = 0; iChamber < nChambers; iChamber++)
        for (Int_t iWire = 0; iWire < nWires; iWire++) {
            pairs[iChamber][iWire] = 0;
            for (Int_t iLayer = 0; iLayer < nLayers; iLayer++)
                singles[iChamber][iWire][iLayer] = 0;
        }
    for (Int_t iChamber = 0; iChamber < nChambers; iChamber++) {
        for (Int_t iDim1 = 0; iDim1 < 2 * N; iDim1++)
            for (Int_t iDim2 = 0; iDim2 < 75 * N; iDim2++)
                params[iChamber][iDim1][iDim2] = -999.;
        for (Int_t iDim1 = 0; iDim1 < 75 * N; iDim1++) {
            segment_size[iChamber][iDim1] = 0;
            Chi2[iChamber][iDim1] = 50.;
            x_global[iChamber][iDim1] = -999.;
            y_global[iChamber][iDim1] = -999.;
        }
        for (Int_t iDim1 = 0; iDim1 < N; iDim1++) {
            for (Int_t iDim2 = 0; iDim2 < 75 * N; iDim2++) {
                v[iChamber][iDim1][iDim2] = -999.;
                u[iChamber][iDim1][iDim2] = -999.;
                y[iChamber][iDim1][iDim2] = -999.;
                x[iChamber][iDim1][iDim2] = -999.;
                sigm_v[iChamber][iDim1][iDim2] = 1.;
                sigm_u[iChamber][iDim1][iDim2] = 1.;
                sigm_y[iChamber][iDim1][iDim2] = 1.;
                sigm_x[iChamber][iDim1][iDim2] = 1.;
            }
            for (Int_t iDim3 = 0; iDim3 < 20 * N; iDim3++) {
                v_Single[iChamber][iDim1][iDim3] = -999.;
                u_Single[iChamber][iDim1][iDim3] = -999.;
                y_Single[iChamber][iDim1][iDim3] = -999.;
                x_Single[iChamber][iDim1][iDim3] = -999.;
                Sigm_v_single[iChamber][iDim1][iDim3] = 1.;
                Sigm_u_single[iChamber][iDim1][iDim3] = 1.;
                Sigm_y_single[iChamber][iDim1][iDim3] = 1.;
                Sigm_x_single[iChamber][iDim1][iDim3] = 1.;
            }
        }
        for (Int_t iDim1 = 0; iDim1 < 4 * N; iDim1++)
            for (Int_t iDim2 = 0; iDim2 < 75 * N; iDim2++) {
                rh_segment[iChamber][iDim1][iDim2] = -999.;
                rh_sigm_segment[iChamber][iDim1][iDim2] = 1.;
            }
    }
    for (Int_t iSegment = 0; iSegment < nSegmentsMax; iSegment++)
        nSegments[iSegment] = 0;
}

void BmnDchTrackFinder::Finish() {
    // Delete 1d-arrays
    delete [] nSegments;
    // Delete 2d-arrays and 3d-arrays
    for (Int_t iChamber = 0; iChamber < nChambers; iChamber++) {
        delete [] x_global[iChamber];
        delete [] y_global[iChamber];
        delete [] Chi2[iChamber];
        delete [] pairs[iChamber];
        delete [] segment_size[iChamber];
        for (Int_t iDim = 0; iDim < N; iDim++) {
            delete [] v[iChamber][iDim];
            delete [] u[iChamber][iDim];
            delete [] y[iChamber][iDim];
            delete [] x[iChamber][iDim];
            delete [] sigm_v[iChamber][iDim];
            delete [] sigm_u[iChamber][iDim];
            delete [] sigm_y[iChamber][iDim];
            delete [] sigm_x[iChamber][iDim];
            delete [] v_Single[iChamber][iDim];
            delete [] u_Single[iChamber][iDim];
            delete [] y_Single[iChamber][iDim];
            delete [] x_Single[iChamber][iDim];
            delete [] Sigm_v_single[iChamber][iDim];
            delete [] Sigm_u_single[iChamber][iDim];
            delete [] Sigm_y_single[iChamber][iDim];
            delete [] Sigm_x_single[iChamber][iDim];
        }
        for (Int_t iDim = 0; iDim < 2 * N; iDim++)
            delete [] params[iChamber][iDim];
        for (Int_t iDim = 0; iDim < 4 * N; iDim++) {
            delete [] rh_segment[iChamber][iDim];
            delete [] rh_sigm_segment[iChamber][iDim];
        }
        delete [] v[iChamber];
        delete [] u[iChamber];
        delete [] y[iChamber];
        delete [] x[iChamber];
        delete [] sigm_v[iChamber];
        delete [] sigm_u[iChamber];
        delete [] sigm_y[iChamber];
        delete [] sigm_x[iChamber];
        delete [] v_Single[iChamber];
        delete [] u_Single[iChamber];
        delete [] y_Single[iChamber];
        delete [] x_Single[iChamber];
        delete [] Sigm_v_single[iChamber];
        delete [] Sigm_u_single[iChamber];
        delete [] Sigm_y_single[iChamber];
        delete [] Sigm_x_single[iChamber];
        delete [] params[iChamber];
        delete [] rh_segment[iChamber];
        delete [] rh_sigm_segment[iChamber];
        for (Int_t iWire = 0; iWire < nWires; iWire++)
            delete [] singles[iChamber][iWire];
        delete [] singles[iChamber];
    }
    delete [] x_global;
    delete [] y_global;
    delete [] Chi2;
    delete [] pairs;
    delete [] segment_size;
    delete [] v;
    delete [] u;
    delete [] y;
    delete [] x;
    delete [] sigm_v;
    delete [] sigm_u;
    delete [] sigm_y;
    delete [] sigm_x;
    delete [] v_Single;
    delete [] u_Single;
    delete [] y_Single;
    delete [] x_Single;
    delete [] Sigm_v_single;
    delete [] Sigm_u_single;
    delete [] Sigm_y_single;
    delete [] Sigm_x_single;
    delete [] params;
    delete [] rh_segment;
    delete [] rh_sigm_segment;
    delete [] singles;
    cout << "Work time of the DCH track finder: " << workTime << " s" << endl;
}

Int_t BmnDchTrackFinder::Reconstruction(Int_t dchID, TString wire, Int_t pair, Int_t it_a, Int_t it_b,
        Double_t* wirenr_a, Double_t* wirenr_b, Double_t* time_a, Double_t* time_b,
        Bool_t* used_a, Bool_t* used_b,
        Float_t** _ab, Float_t** sigm_ab) {

    const Int_t arrIdxShift = (dchID == 2) ? 8 : 0;
    const Int_t arrIdxStart = (wire == "x") ? 0 : (wire == "y") ? 2 : (wire == "u") ? 4 : 6;

    Float_t a_pm[2], b_pm[2];

    for (Int_t i = 0; i < it_a; ++i)
        for (Int_t j = 0; j < it_b; ++j) {
            if (pair > 48)
                break;
            if ((wirenr_a[i] != wirenr_b[j] && wirenr_a[i] != wirenr_b[j] + 1))
                continue;
            Int_t func_nr_a = -1;
            Int_t func_nr_b = -1;
            for (Int_t t_it = 0; t_it < 4; t_it++)
                if (time_a[i] >= t_dc[t_it][0 + arrIdxStart + arrIdxShift] && time_a[i] < t_dc[t_it + 1][0 + arrIdxStart + arrIdxShift]) {
                    func_nr_a = t_it;
                    break;
                }

            Double_t time = time_a[i];
            Double_t d_a = 0;
            Double_t d_b = 0;

            if (func_nr_a == 1 || func_nr_a == 2) d_a = scale * (pol_par_dc[1][0][0 + arrIdxStart + arrIdxShift] + pol_par_dc[1][1][0 + arrIdxStart + arrIdxShift] * time +
                    pol_par_dc[1][2][0 + arrIdxStart + arrIdxShift] * Power(time, 2) +
                    pol_par_dc[1][3][0 + arrIdxStart + arrIdxShift] * Power(time, 3) +
                    pol_par_dc[1][4][0 + arrIdxStart + arrIdxShift] * Power(time, 4));

            else if (func_nr_a == 0) d_a = 0;

            else if (func_nr_a == 3) d_a = scale * (pol_par_dc[2][0][0 + arrIdxStart + arrIdxShift] + pol_par_dc[2][1][0 + arrIdxStart + arrIdxShift] * time +
                    pol_par_dc[2][2][0 + arrIdxStart + arrIdxShift] * Power(time, 2) +
                    pol_par_dc[2][3][0 + arrIdxStart + arrIdxShift] * Power(time, 3) +
                    pol_par_dc[2][4][0 + arrIdxStart + arrIdxShift] * Power(time, 4));

            for (Int_t t_it = 0; t_it < 4; t_it++)
                if (time_b[j] >= t_dc[t_it][1 + arrIdxStart + arrIdxShift] && time_b[j] < t_dc[t_it + 1][1 + arrIdxStart + arrIdxShift]) {
                    func_nr_b = t_it;
                    break;
                }

            time = time_b[j];

            if (func_nr_b == 1 || func_nr_b == 2) d_b = scale * (pol_par_dc[1][0][1 + arrIdxStart + arrIdxShift] + pol_par_dc[1][1][1 + arrIdxStart + arrIdxShift] * time +
                    pol_par_dc[1][2][1 + arrIdxStart + arrIdxShift] * Power(time, 2) +
                    pol_par_dc[1][3][1 + arrIdxStart + arrIdxShift] * Power(time, 3) +
                    pol_par_dc[1][4][1 + arrIdxStart + arrIdxShift] * Power(time, 4));

            else if (func_nr_b == 0) d_b = 0;

            else if (func_nr_b == 3) d_b = scale * (pol_par_dc[2][0][1 + arrIdxStart + arrIdxShift] + pol_par_dc[2][1][1 + arrIdxStart + arrIdxShift] * time +
                    pol_par_dc[2][2][1 + arrIdxStart + arrIdxShift] * Power(time, 2) +
                    pol_par_dc[2][3][1 + arrIdxStart + arrIdxShift] * Power(time, 3) +
                    pol_par_dc[2][4][1 + arrIdxStart + arrIdxShift] * Power(time, 4));

            a_pm[0] = wirenr_a[i] - 119 + d_a;
            a_pm[1] = wirenr_a[i] - 119 - d_a;
            b_pm[0] = wirenr_b[j] - 118.5 + d_b;
            b_pm[1] = wirenr_b[j] - 118.5 - d_b;

            Double_t dmin = LDBL_MAX;
            for (Int_t k = 0; k < 2; k++)
                for (Int_t m = 0; m < 2; m++)
                    if (Abs(a_pm[k] - b_pm[m]) < dmin) {
                        dmin = Abs(a_pm[k] - b_pm[m]);
                        _ab[0][pair] = a_pm[k];
                        _ab[1][pair] = b_pm[m];
                    }

            CompareDaDb(d_a, sigm_ab[0][pair]);
            CompareDaDb(d_b, sigm_ab[1][pair]);

            pair++;

            used_a[i] = kTRUE;
            used_b[j] = kTRUE;
        }
    return pair;
}

Int_t BmnDchTrackFinder::ReconstructionSingle(Int_t dchID, TString wire, TString lay, Int_t single, Int_t it,
        Double_t* wirenr, Double_t* time_, Bool_t* used,
        Float_t** _single, Float_t** sigm_single) {

    const Int_t arrIdxStart = (wire == "x") ? 0 : (wire == "y") ? 2 : (wire == "u") ? 4 : 6;

    const Int_t arrIdx1 = (lay == "a") ? 0 : 1;
    const Int_t arrIdx2 = (dchID == 2) ? 8 : 0;
    const Double_t coeff = (lay == "a") ? 119 : 118.5;

    for (Int_t i = 0; i < it; ++i) {
        if (used[i])
            continue;

        Int_t func_nr = -1;
        for (Int_t t_it = 0; t_it < 4; t_it++) {
            if (time_[i] >= t_dc[t_it][0 + arrIdxStart + arrIdx1 + arrIdx2] && time_[i] < t_dc[t_it + 1][0 + arrIdxStart + arrIdx1 + arrIdx2]) {
                func_nr = t_it;
                break;
            }
        }
        Double_t time = time_[i];
        Double_t d = 0;

        if (func_nr == 1 || func_nr == 2) d = scale * (pol_par_dc[1][0][0 + arrIdx1 + arrIdx2] + pol_par_dc[1][1][0 + arrIdx1 + arrIdx2] * time +
                pol_par_dc[1][2][0 + arrIdxStart + arrIdx1 + arrIdx2] * Power(time, 2) +
                pol_par_dc[1][3][0 + arrIdxStart + arrIdx1 + arrIdx2] * Power(time, 3) +
                pol_par_dc[1][4][0 + arrIdxStart + arrIdx1 + arrIdx2] * Power(time, 4));

        else if (func_nr == 0) d = 0;

        else if (func_nr == 3) d = scale * (pol_par_dc[2][0][0 + arrIdx1 + arrIdx2] + pol_par_dc[2][1][0 + arrIdx1 + arrIdx2] * time +
                pol_par_dc[2][2][0 + arrIdxStart + arrIdx1 + arrIdx2] * Power(time, 2) +
                pol_par_dc[2][3][0 + arrIdxStart + arrIdx1 + arrIdx2] * Power(time, 3) +
                pol_par_dc[2][4][0 + arrIdxStart + arrIdx1 + arrIdx2] * Power(time, 4));

        _single[0 + arrIdx1][single] = wirenr[i] - coeff + d;
        _single[0 + arrIdx1][single + 1] = wirenr[i] - coeff - d;

        CompareDaDb(d, sigm_single[0 + arrIdx1][single], sigm_single[0 + arrIdx1][single + 1]);

        single += 2;
    }
    return single;
}
ClassImp(BmnDchTrackFinder)

