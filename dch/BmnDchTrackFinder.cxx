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

#include <Rtypes.h>
#include <TMath.h>

#include "FairLogger.h"
#include "TH1F.h"
#include "TH2F.h"

#include "BmnDchTrackFinder.h"
#include "FairMCPoint.h"
#include "FitWLSQ.h"
static Double_t workTime = 0.0;
//added
TString fhTestFlnm;
TList fhList;
TH1D* Ntrack1;
TH1D* Ntrack2;

TH2F* hXvsAx;
TH1F* hYa_wireOccupancy;
TH1F* hYb_wireOccupancy;
TH1F* haX2_aX1;
TH1F* haY2_aY1;
TH1F* hX1_X2_glob_all;
TH1F* hY1_Y2_glob_all;
TH1F* haX2_aX1m;
TH1F* haY2_aY1m;
TH1F* hX1_X2_glob_m;
TH1F* hY1_Y2_glob_m;
TH1F* hx1Da_Db;
TH1F* hx2Da_Db;
TH1F* hy1Da_Db;
TH1F* hy2Da_Db;
TH1F* haX1;
TH1F* haY1;
TH1F* haX2;
TH1F* haY2;
TH1F* hx1;
TH1F* hy1;
TH1F* hx2;
TH1F* hy2;
TH1F* hX;
TH1F* hY;
TH1F* haX;
TH1F* haY;
TH2F* hlocXY1;
TH2F* hlocXY2;
TH1F* hSlot_1xa_time;
TH1F* hSlot_1xb_time;

//end

BmnDchTrackFinder::BmnDchTrackFinder(Bool_t isExp) : expData(isExp) {
    //added temporary hists
    fhTestFlnm = "test.BmnDCHTracking.root";
    hSlot_1xa_time = new TH1F("hSlot_1xa_time", "times_for_plane_DC1_xa", 2000, 0, 2000);
    hSlot_1xb_time = new TH1F("hSlot_1xb_time", "times_for_plane_DC1_xb", 2000, 0, 2000);
    Ntrack1 = new TH1D("Ntrack1", "Ntrack1", 20, 0, 20);
    Ntrack2 = new TH1D("Ntrack2", "Ntrack2", 20, 0, 20);
    hYa_wireOccupancy = new TH1F("hYa_wireOccupancy", " ya wire nr ", 250, 0, 250);
    hYb_wireOccupancy = new TH1F("hYb_wireOccupancy", " yb wire nr ", 250, 0, 250);
    haX2_aX1 = new TH1F("haX2_aX1", "aX2 - aX1", 200, -0.25, 0.25);
    haY2_aY1 = new TH1F("haY2_aY1", "aY2 - aY1", 200, -0.25, 0.25);
    hX1_X2_glob_all = new TH1F("hX1_X2_glob_all", " Xdc2 - Xdc1 extrapol Zmid, all closest", 100, -50, 50);
    hY1_Y2_glob_all = new TH1F("hY1_Y2_glob_all", " Ydc2 - Ydc1 extrapol Zmid, all closest", 100, -50, 50);
    haX2_aX1m = new TH1F("haX2_aX1m", "aX2 - aX1m", 200, -0.25, 0.25);
    haY2_aY1m = new TH1F("haY2_aY1m", "aY2 - aY1m", 200, -0.25, 0.25);
    hX1_X2_glob_m = new TH1F("hX1_X2_glob_m", " Xdc2 - Xdc1 extrapol Zmid, m closest", 100, -50, 50);
    hY1_Y2_glob_m = new TH1F("hY1_Y2_glob_m", " Ydc2 - Ydc1 extrapol Zmid, m closest", 100, -50, 50);
    hx1Da_Db = new TH1F("hx1Da_Db", "d_a + d_b - 5mm dc1", 200, -0.5, 0.5);
    hx2Da_Db = new TH1F("hx2Da_Db", "d_a + d_b - 5mm dc2", 200, -0.5, 0.5);
    hy1Da_Db = new TH1F("hy1Da_Db", "d_a + d_b - 5mm dc1", 200, -0.5, 0.5);
    hy2Da_Db = new TH1F("hy2Da_Db", "d_a + d_b - 5mm dc2", 200, -0.5, 0.5);
    haX1 = new TH1F("haX1", "aX1", 300, -0.05, 0.15);
    haY1 = new TH1F("haY1", "aY1", 200, -0.1, 0.1);
    haX2 = new TH1F("haX2", "aX2", 300, -0.05, 0.15);
    haY2 = new TH1F("haY2", "aY2", 200, -0.1, 0.1);
    hx1 = new TH1F("hx1", "X1", 130, -80, 50);
    hy1 = new TH1F("hy1", "Y1", 200, -30, 50);
    hx2 = new TH1F("hx2", "X2", 130, -80, 50);
    hy2 = new TH1F("hy2", "Y2", 200, -30, 50);
    hX = new TH1F("hX", "X", 80, -80, 0);
    hY = new TH1F("hY", "Y", 90, -10, 20);
    haX = new TH1F("haX", "aX", 80, 0.0, 0.2);
    haY = new TH1F("haY", "aY", 100, -0.1, 0.15);
    hlocXY1 = new TH2F("hlocXY1", " (X,Y) local coord of a seg in dc1", 240, -120, 120, 240, -120, 120);
    hlocXY2 = new TH2F("hlocXY2", " (X,Y) local coord of a seg in dc2", 240, -120, 120, 240, -120, 120);
    hXvsAx = new TH2F("hXvsAx", " X coord of a seg in dc1 vs x slope", 240, -120, 120, 100, -10, 10);
    fhList.Add(hSlot_1xa_time);
    fhList.Add(hSlot_1xb_time);
    fhList.Add(haX2_aX1);
    fhList.Add(haY2_aY1);
    fhList.Add(hX1_X2_glob_all);
    fhList.Add(hY1_Y2_glob_all);
    fhList.Add(haX2_aX1m);
    fhList.Add(haY2_aY1m);
    fhList.Add(hX1_X2_glob_m);
    fhList.Add(hY1_Y2_glob_m);

    fhList.Add(hYa_wireOccupancy);
    fhList.Add(hYb_wireOccupancy);
    fhList.Add(Ntrack1);
    fhList.Add(Ntrack2);

    fhList.Add(hx1Da_Db);
    fhList.Add(hx2Da_Db);
    fhList.Add(hy1Da_Db);
    fhList.Add(hy2Da_Db);
    fhList.Add(haX1);
    fhList.Add(haY1);
    fhList.Add(haX2);
    fhList.Add(haY2);
    fhList.Add(hx1);
    fhList.Add(hy1);
    fhList.Add(hx2);
    fhList.Add(hy2);
    fhList.Add(hX);
    fhList.Add(hY);
    fhList.Add(haX);
    fhList.Add(haY);
    fhList.Add(hlocXY1);
    fhList.Add(hlocXY2);
    fhList.Add(hXvsAx);
    //end

    fEventNo = 0;
    N = 15;  //needs to be adjusted according to hit multiplicity
    tracksDch = "BmnDchTrack";
    InputDigitsBranchName = "DCH";
    fTransferFunctionName = "transfer_func.txt";

    prev_wire = -1;
    prev_time = -1;

    nChambers = 2;
    nWires = 4;
    nLayers = 2;
    nSegmentsMax = 100;

    Z_dch1 = 510.2;
    Z_dch2 = 709.5;
    Z_dch_mid = (Z_dch1 + Z_dch2) / 2.;
    dZ_dch_mid = Z_dch2 - Z_dch_mid;  // > 0
    dZ_dch = Z_dch2 - Z_dch1;

    // Some alignment corrections

    //argon
    x1_sh = 0;      //15.77;//10.5;
    x2_sh = -5.9;   //-0.97;//4.33;
    y1_sh = 0;      //-2.86;//2.47;
    y2_sh = -6.14;  //7.67;//2.34;

    x1_slope_sh = 0.;      //-0.077;//-0.07;
    y1_slope_sh = 0.;      //0.07;//0.062;
    x2_slope_sh = -0.007;  //-0.07;//-0.077;
    y2_slope_sh = -0.007;  //0.069;//0.077;

    //src
    x1_sh = 0;       //11.2;//-8.56;//run6 10.9;// 5.34;//5.14;//10.31; //
    x2_sh = 8.60;    //9.-3.5;//0.0;//run6 5.56;//0;//7.95;// //
    y1_sh = 0;       //1.76;//5.76;//run6 -1.19;//0;//-1.03;
    y2_sh = -12.40;  //1.8;//-7.1;//run6 -2.47;//-1.38;//-3.7;

    x1_slope_sh = 0;
    y1_slope_sh = 0;
    x2_slope_sh = 0.;
    y2_slope_sh = 0.007;

    scale = 0.5;
}

BmnDchTrackFinder::~BmnDchTrackFinder() {
    // Delete 1d-arrays

    delete[] nSegments;
    delete[] has7DC;
    delete[] x_mid;
    delete[] y_mid;
    delete[] nhits;
    delete[] pairID;
    delete[] aX;
    delete[] aY;
    delete[] leng;
    delete[] imp;
    // Delete 2d-arrays and 3d-arrays
    for (Int_t iChamber = 0; iChamber < nChambers; iChamber++) {
        delete[] x_global[iChamber];
        delete[] y_global[iChamber];
        delete[] Chi2[iChamber];
        delete[] pairs[iChamber];
        delete[] segment_size[iChamber];
        for (Int_t iDim = 0; iDim < N; iDim++) {
            delete[] v[iChamber][iDim];
            delete[] u[iChamber][iDim];
            delete[] y[iChamber][iDim];
            delete[] x[iChamber][iDim];
            delete[] sigm_v[iChamber][iDim];
            delete[] sigm_u[iChamber][iDim];
            delete[] sigm_y[iChamber][iDim];
            delete[] sigm_x[iChamber][iDim];
            delete[] v_Single[iChamber][iDim];
            delete[] u_Single[iChamber][iDim];
            delete[] y_Single[iChamber][iDim];
            delete[] x_Single[iChamber][iDim];
            delete[] Sigm_v_single[iChamber][iDim];
            delete[] Sigm_u_single[iChamber][iDim];
            delete[] Sigm_y_single[iChamber][iDim];
            delete[] Sigm_x_single[iChamber][iDim];
        }
        for (Int_t iDim = 0; iDim < 4; iDim++) {
            delete[] params[iChamber][iDim];
            delete[] params_sigmas[iChamber][iDim];
        }
        for (Int_t iDim = 0; iDim < 8; iDim++) {
            delete[] rh_segment[iChamber][iDim];
            delete[] rh_sigm_segment[iChamber][iDim];
        }
        delete[] v[iChamber];
        delete[] u[iChamber];
        delete[] y[iChamber];
        delete[] x[iChamber];
        delete[] sigm_v[iChamber];
        delete[] sigm_u[iChamber];
        delete[] sigm_y[iChamber];
        delete[] sigm_x[iChamber];
        delete[] v_Single[iChamber];
        delete[] u_Single[iChamber];
        delete[] y_Single[iChamber];
        delete[] x_Single[iChamber];
        delete[] Sigm_v_single[iChamber];
        delete[] Sigm_u_single[iChamber];
        delete[] Sigm_y_single[iChamber];
        delete[] Sigm_x_single[iChamber];
        delete[] params[iChamber];
        delete[] params_sigmas[iChamber];
        delete[] rh_segment[iChamber];
        delete[] rh_sigm_segment[iChamber];
        for (Int_t iWire = 0; iWire < nWires; iWire++)
            delete[] singles[iChamber][iWire];
        delete[] singles[iChamber];
    }
    delete[] x_global;
    delete[] y_global;
    delete[] Chi2;
    delete[] pairs;
    delete[] segment_size;
    delete[] v;
    delete[] u;
    delete[] y;
    delete[] x;
    delete[] sigm_v;
    delete[] sigm_u;
    delete[] sigm_y;
    delete[] sigm_x;
    delete[] v_Single;
    delete[] u_Single;
    delete[] y_Single;
    delete[] x_Single;
    delete[] Sigm_v_single;
    delete[] Sigm_u_single;
    delete[] Sigm_y_single;
    delete[] Sigm_x_single;
    delete[] params;
    delete[] params_sigmas;
    delete[] rh_segment;
    delete[] rh_sigm_segment;
    delete[] singles;
}

void BmnDchTrackFinder::Exec(Option_t* opt) {
    if (!IsActive())
        return;
    fEventNo++;
    clock_t tStart = clock();
    PrepareArraysToProcessEvent();
    if (fVerbose) cout << "======================== DCH track finder exec started ====================" << endl;
    if (fVerbose) cout << "Event number: " << fEventNo << endl;
    //temporary containers
    // Order used: va1, vb1, ua1, ub1, ya1, yb1, xa1, xb1 (dch1, 0 - 7) - va2, vb2, ua2, ub2, ya2, yb2, xa2, xb2 (dch2, 8 - 15)
    const Int_t nDim = 80;     //max number of fired wires per plane
    const Int_t nPlanes = 16;  // Total number of planes in both DCHs (0-7, 8-15)

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
        BmnDchDigit* digit = (BmnDchDigit*)fBmnDchDigitsArray->UncheckedAt(iDig);
        //skip identical events
        if (!written) {
            written = kTRUE;
            if (digit->GetTime() == prev_time && digit->GetWireNumber() == prev_wire) {
                goodEv = kFALSE;
            } else {
                prev_time = Int_t(digit->GetTime());
                prev_wire = Int_t(digit->GetWireNumber());
            }
        }  //!written

        if (!goodEv)
            return;

        // Order used: va1(0), vb1(1), ua1(2), ub1(3), ya1(4), yb1(5), xa1(6), xb1(7) ->
        //             va2(8), vb2(9), ua2(10), ub2(11), ya2(12), yb2(13), xa2(14), xb2(15)
        Short_t plane = digit->GetPlane();
        Short_t wire = digit->GetWireNumber();
        Double_t time = digit->GetTime();
        Bool_t secondaries = kFALSE;
        if (wire > 239) wire = wire - 128;  //recalculate last amplifier
        if (plane == 4) {
            if (wire > 143 && wire < 176) {
                if (wire > 159 && wire < 176) {
                    wire = wire - 16;
                } else {
                    wire = wire + 16;
                }
            }
        }

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
        if (plane == 6) hSlot_1xa_time->Fill(time);
        if (plane == 7) hSlot_1xb_time->Fill(time);
        if (plane == 4) hYa_wireOccupancy->Fill(wire);
        if (plane == 5) hYb_wireOccupancy->Fill(wire);
    }

    const Int_t nWires_ = 4;
    const Int_t nLayers_ = 2;
    TString wireNames[nWires_] = {"v", "u", "y", "x"};
    TString layNames[nLayers_] = {"a", "b"};
    Int_t cntr = 0;

    for (Int_t iDch = 0; iDch < nChambers; iDch++) {
        for (Int_t iWire = 0; iWire < nWires; iWire++) {
            Int_t start = 2 * iWire + (nPlanes / 2) * iDch;
            Int_t finish = start + 1;
            Double_t*** coord = (iWire == 0) ? v : (iWire == 1) ? u : (iWire == 2) ? y : x;
            Double_t*** sigma = (iWire == 0) ? sigm_v : (iWire == 1) ? sigm_u : (iWire == 2) ? sigm_y : sigm_x;

            pairs[iDch][iWire] = Reconstruction(iDch + 1, wireNames[iWire], pairs[iDch][iWire], it[start], it[finish],
                                                wires[start], wires[finish], times[start], times[finish], used[start], used[finish],
                                                coord[iDch], sigma[iDch]);

            for (Int_t iLayer = 0; iLayer < nLayers; iLayer++) {
                Double_t*** single_coord = (iWire == 0) ? v_Single : (iWire == 1) ? u_Single : (iWire == 2) ? y_Single : x_Single;
                Double_t*** single_sigma = (iWire == 0) ? Sigm_v_single : (iWire == 1) ? Sigm_u_single : (iWire == 2) ? Sigm_y_single : Sigm_x_single;
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

        has7DC[iDch] = FitDchSegments(iDch + 1, segment_size[iDch], rh_segment[iDch], rh_sigm_segment[iDch], params[iDch], Chi2[iDch], x_global[iDch], y_global[iDch]);  // Fit found segments
        SelectLongestAndBestSegments(iDch + 1, segment_size[iDch], rh_segment[iDch], Chi2[iDch]);                                                                        // Save only longest and best chi2 segments
        FillSegmentParametersSigmas(iDch + 1, rh_segment[iDch], rh_sigm_segment[iDch], Chi2[iDch], params_sigmas[iDch]);                                                 //calculate segment parameters errors
        CreateDchTrack(iDch + 1, Chi2[iDch], params[iDch], segment_size[iDch], rh_segment[iDch], params_sigmas[iDch]);                                                   // Fill segment parameters
    }

    // Try to match the reconstructed segments from the two chambers
    SegmentsToBeMatched();
    CreateDchTrack();

    if (fVerbose) cout << "======================== DCH track finder exec finished ===================" << endl;
    clock_t tFinish = clock();
    workTime += ((Double_t)(tFinish - tStart)) / CLOCKS_PER_SEC;
}

void BmnDchTrackFinder::SegmentsToBeMatched() {
    while (true) {
        Int_t match_dc1_seg = -1;
        Double_t ax(0.), ay(0.), xMean(0.), yMean(0.);

        int best1 = -1;
        int best2 = -1;
        double chi2_Match_min = 200.;

        Double_t dx = -999;
        Double_t dy = -999;
        Double_t daX = -999;
        Double_t daY = -999;

        for (Int_t segdc2Nr = 0; segdc2Nr < nSegments[1]; segdc2Nr++) {
            if (Chi2[1][segdc2Nr] > 30.)
                continue;

            Double_t chi2_match = 0;
            for (int segdc1Nr = 0; segdc1Nr < nSegments[0]; segdc1Nr++) {
                if (Chi2[0][segdc1Nr] > 30.)
                    continue;

                dx = x_global[1][segdc2Nr] - x_global[0][segdc1Nr];
                dy = y_global[1][segdc2Nr] - y_global[0][segdc1Nr];
                xMean = 0.5 * (x_global[0][segdc1Nr] + x_global[1][segdc2Nr]);
                yMean = 0.5 * (y_global[0][segdc1Nr] + y_global[1][segdc2Nr]);
                ax = (params[1][1][segdc2Nr] - params[0][1][segdc1Nr]) / dZ_dch;
                ay = (params[1][3][segdc2Nr] - params[0][3][segdc1Nr]) / dZ_dch;
                match_dc1_seg = segdc1Nr;
                daX = params[1][0][segdc2Nr] - params[0][0][segdc1Nr];
                daY = params[1][2][segdc2Nr] - params[0][2][segdc1Nr];
                hX1_X2_glob_all->Fill(dx);
                hY1_Y2_glob_all->Fill(dy);
                haX2_aX1->Fill(daX);
                haY2_aY1->Fill(daY);

                double sigma2_dx = ((params_sigmas[1][0][segdc2Nr] * params_sigmas[1][0][segdc2Nr]) +
                                    (params_sigmas[0][0][segdc1Nr] * params_sigmas[0][0][segdc1Nr])) *
                                       99.75 * 99.75 +
                                   ((params_sigmas[0][1][segdc1Nr]) * (params_sigmas[0][1][segdc1Nr])) +
                                   ((params_sigmas[1][1][segdc2Nr]) * (params_sigmas[1][1][segdc2Nr]));

                double sigma2_dy = ((params_sigmas[1][2][segdc2Nr] * params_sigmas[1][2][segdc2Nr]) +
                                    (params_sigmas[0][2][segdc1Nr] * params_sigmas[0][2][segdc1Nr])) *
                                       99.75 * 99.75 +
                                   ((params_sigmas[0][3][segdc1Nr]) * (params_sigmas[0][3][segdc1Nr])) +
                                   ((params_sigmas[1][3][segdc2Nr]) * (params_sigmas[1][3][segdc2Nr]));

                double sigma2_dax = 9. * ((params_sigmas[1][0][segdc2Nr] * params_sigmas[1][0][segdc2Nr]) +
                                          (params_sigmas[0][0][segdc1Nr] * params_sigmas[0][0][segdc1Nr]));

                double sigma2_day = 9. * ((params_sigmas[1][2][segdc2Nr] * params_sigmas[1][2][segdc2Nr]) +
                                          (params_sigmas[0][2][segdc1Nr] * params_sigmas[0][2][segdc1Nr]));

                double chi2_Match = (((dx * dx) / sigma2_dx) + ((dy * dy) / sigma2_dy) + ((daX * daX) / sigma2_dax) + ((daY * daY) / sigma2_day)) / (segment_size[0][segdc1Nr] + segment_size[1][segdc2Nr] - 8);

                if ((segment_size[0][segdc1Nr] + segment_size[1][segdc2Nr]) < 12) continue;
                if (fabs(dx) > 8. || fabs(dy) > 10. || fabs(daX) > 0.1 || fabs(daY) > 0.14) continue;
                chi2_Match_min = chi2_Match;
                best1 = segdc1Nr;
                best2 = segdc2Nr;
                hX1_X2_glob_m->Fill(dx);
                hY1_Y2_glob_m->Fill(dy);
                haX2_aX1m->Fill(daX);
                haY2_aY1m->Fill(daY);
            }  // segdc1Nr

        }  // segdc2Nr
        if (chi2_Match_min > 199.) break;
        nSegmentsToBeMatched++;
        pairID[nSegmentsToBeMatched] = 1000 * (best1 + 1) + best2 + 1;
        nhits[nSegmentsToBeMatched] = segment_size[0][best1] + segment_size[1][best2];
        x_mid[nSegmentsToBeMatched] = 0.5 * (x_global[0][best1] + x_global[1][best2]);  //par_ab1[1][best1];//xDC1_glob[best1];//xMean;
        y_mid[nSegmentsToBeMatched] = 0.5 * (y_global[0][best1] + y_global[1][best2]);  //yDC1_glob[best1];//yMean;
        aX[nSegmentsToBeMatched] = -(params[1][1][best2] - params[0][1][best1]) / dZ_dch;
        aY[nSegmentsToBeMatched] = (params[1][3][best2] - params[0][3][best1]) / dZ_dch;
        Chi2_match[nSegmentsToBeMatched] = chi2_Match_min;
        haX->Fill(aX[nSegmentsToBeMatched]);
        haY->Fill(aY[nSegmentsToBeMatched]);
        hX->Fill(x_mid[nSegmentsToBeMatched]);
        hY->Fill(y_mid[nSegmentsToBeMatched]);
        Chi2[0][best1] = 999;
        Chi2[1][best2] = 999;

    }  //while
}

Int_t BmnDchTrackFinder::BuildXYSegments(Int_t dchID,
                                         Int_t pairU, Int_t pairV, Int_t pairX, Int_t pairY, Int_t single_xa, Int_t single_xb, Int_t single_ya, Int_t single_yb,
                                         Double_t** x_ab, Double_t** y_ab, Double_t** u_ab, Double_t** v_ab,
                                         Double_t** sigm_x_ab, Double_t** sigm_y_ab, Double_t** sigm_u_ab, Double_t** sigm_v_ab,
                                         Double_t** rh_seg, Double_t** rh_sigm_seg,
                                         Double_t** x_single, Double_t** y_single, Double_t** sigm_x_single, Double_t** sigm_y_single) {
    Double_t sqrt_2 = sqrt(2.);
    Double_t isqrt_2 = 1. / sqrt_2;

    Int_t nDC_segments = (dchID == 1) ? nSegments[0] : nSegments[1];

    for (Int_t i = 0; i < pairU; i++) {
        if (nDC_segments > 25 * N - 1)
            break;
        Double_t u_coord = (u_ab[0][i] + u_ab[1][i]) / 2;
        Double_t u_slope = (u_ab[0][i] - u_ab[1][i]) / (z_loc[4] - z_loc[5]);
        Double_t UX = u_coord + u_slope * 0.5 * (z_loc[1] + z_loc[0] - z_loc[5] - z_loc[4]);
        Double_t UY = u_coord + u_slope * 0.5 * (z_loc[3] + z_loc[2] - z_loc[5] - z_loc[4]);
        for (Int_t j = 0; j < pairV; j++) {
            if (nDC_segments > 25 * N - 1)
                break;
            Double_t v_coord = (v_ab[0][j] + v_ab[1][j]) / 2;
            Double_t v_slope = (v_ab[0][j] - v_ab[1][j]) / (z_loc[6] - z_loc[7]);
            Double_t VX = v_coord + v_slope * 0.5 * (z_loc[1] + z_loc[0] - z_loc[6] - z_loc[7]);
            Double_t VY = v_coord + v_slope * 0.5 * (z_loc[3] + z_loc[2] - z_loc[6] - z_loc[7]);
            Bool_t foundX = kFALSE;
            Double_t x_est = isqrt_2 * (VX - UX);
            Double_t y_est = isqrt_2 * (UY + VY);
            if (pairX > 0) {
                Double_t dX_thresh = 1.2;
                for (Int_t k = 0; k < pairX; k++) {
                    Double_t x_coord = (x_ab[0][k] + x_ab[1][k]) / 2;
                    if (nDC_segments > 25 * N - 1)
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
                    if (nDC_segments > 25 * N - 1)
                        break;
                }  //k
            }      //(pair_x2>0)

            Bool_t foundY = kFALSE;
            if (pairY > 0) {
                Double_t dY_thresh = 1.2;
                for (Int_t m = 0; m < pairY; m++) {
                    if (nDC_segments > 25 * N - 1)
                        break;
                    Double_t y_coord = (y_ab[0][m] + y_ab[1][m]) / 2;
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
                        Double_t min_a = 999;
                        Double_t min_b = 999;
                        for (Int_t kk = 0; kk < single_xa; kk++) {
                            if (Abs(x_single[1][kk] - x_est) > 1.2)
                                continue;  //????? 0.5 needs to be reviewed

                            if (Abs(x_single[0][kk] - x_est) < min_a) {
                                min_a = Abs(x_single[0][kk] - x_est);
                                rh_seg[0][nDC_segments] = x_single[0][kk];
                                rh_sigm_seg[0][nDC_segments] = sigm_x_single[0][kk];
                                foundX = kTRUE;
                            }
                        }  //for kk
                        for (Int_t kk = 0; kk < single_xb; kk++) {
                            if (Abs(x_single[1][kk] - x_est) > 1.2)
                                continue;  //????? 0.5 needs to be reviewed
                            if (Abs(x_single[1][kk] - x_est) < min_b) {
                                min_b = Abs(x_single[1][kk] - x_est);
                                rh_seg[1][nDC_segments] = x_single[1][kk];
                                rh_sigm_seg[1][nDC_segments] = sigm_x_single[1][kk];
                                foundX = kTRUE;
                            }
                        }  //for kk
                        if (nDC_segments > 25 * N - 1)
                            break;
                    }  //!foundX
                }      //m
                if (foundX && !foundY) {
                    Double_t min_a = 999;
                    Double_t min_b = 999;
                    for (Int_t kk = 0; kk < single_ya; kk++) {
                        if (Abs(y_single[0][kk] - y_est) > 1.2)
                            continue;  //????? 0.5 needs to be reviewed
                        if (Abs(y_single[0][kk] - y_est) < min_a) {
                            min_a = Abs(y_single[0][kk] - y_est);
                            rh_seg[2][nDC_segments] = y_single[0][kk];
                            rh_sigm_seg[2][nDC_segments] = sigm_y_single[0][kk];
                            foundY = kTRUE;
                        }
                    }  //for kk
                    for (Int_t kk = 0; kk < single_yb; kk++) {
                        if (Abs(y_single[1][kk] - y_est) > 1.2)
                            continue;  //????? 0.5 needs to be reviewed
                        if (Abs(y_single[1][kk] - y_est) < min_b) {
                            min_b = Abs(y_single[1][kk] - y_est);
                            rh_seg[3][nDC_segments] = y_single[1][kk];
                            rh_sigm_seg[3][nDC_segments] = sigm_y_single[1][kk];
                            foundY = kTRUE;
                        }
                    }  //for kk
                }
            }  //(pair_y2>0)
            if (foundX || foundY) nDC_segments++;
        }
    }
    return nDC_segments;
}

Int_t BmnDchTrackFinder::BuildUVSegments(Int_t dchID, Int_t pairU, Int_t pairV, Int_t pairX, Int_t pairY, Int_t single_ua, Int_t single_ub, Int_t single_va, Int_t single_vb,
                                         Double_t** x_ab, Double_t** y_ab, Double_t** u_ab, Double_t** v_ab,
                                         Double_t** sigm_x_ab, Double_t** sigm_y_ab, Double_t** sigm_u_ab, Double_t** sigm_v_ab,
                                         Double_t** rh_seg, Double_t** rh_sigm_seg,
                                         Double_t** u_single, Double_t** v_single, Double_t** sigm_u_single, Double_t** sigm_v_single) {
    Double_t sqrt_2 = sqrt(2.);
    Double_t isqrt_2 = 1. / sqrt_2;

    Int_t nDC_segments = (dchID == 1) ? nSegments[0] : nSegments[1];

    for (Int_t i = 0; i < pairX; i++) {
        if (nDC_segments > 25 * N - 1)
            break;
        Double_t x_coord = (x_ab[0][i] + x_ab[1][i]) / 2;
        Double_t x_slope = (x_ab[0][i] - x_ab[1][i]) / (z_loc[0] - z_loc[1]);
        Double_t XU = x_coord + x_slope * 0.5 * (z_loc[5] + z_loc[4] - z_loc[1] - z_loc[0]);
        Double_t XV = x_coord + x_slope * 0.5 * (z_loc[7] + z_loc[6] - z_loc[1] - z_loc[0]);

        for (Int_t j = 0; j < pairY; j++) {
            Double_t y_coord = (y_ab[0][j] + y_ab[1][j]) / 2;
            Double_t y_slope = (y_ab[0][j] - y_ab[1][j]) / (z_loc[2] - z_loc[3]);
            Double_t YU = y_coord + y_slope * 0.5 * (z_loc[5] + z_loc[4] - z_loc[3] - z_loc[2]);
            Double_t YV = y_coord + y_slope * 0.5 * (z_loc[7] + z_loc[6] - z_loc[3] - z_loc[2]);
            Bool_t foundU = kFALSE;
            Double_t u_est = isqrt_2 * (YU - XU);
            Double_t v_est = isqrt_2 * (YV + XV);

            Double_t dU_thresh = 1.2;
            for (Int_t k = 0; k < pairU; k++) {
                Double_t u_coord = (u_ab[0][k] + u_ab[1][k]) / 2;

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
                if (nDC_segments > 25 * N - 1)
                    break;
            }

            Bool_t foundV = kFALSE;

            Double_t dV_thresh = 1.2;
            for (Int_t m = 0; m < pairV; m++) {
                if (nDC_segments > 25 * N - 1)
                    break;
                Double_t v_coord = (v_ab[0][m] + v_ab[1][m]) / 2;

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
                    Double_t min_a = 999;
                    Double_t min_b = 999;
                    for (Int_t kk = 0; kk < single_ua; kk++) {
                        if (Abs(u_single[0][kk] - u_est) > 1.2)
                            continue;  //????? 0.5 needs to be reviewed
                        if (Abs(u_single[0][kk] - u_est) < min_a) {
                            min_a = Abs(u_single[0][kk] - u_est);
                            rh_seg[4][nDC_segments] = u_single[0][kk];
                            rh_sigm_seg[4][nDC_segments] = sigm_u_single[0][kk];
                            foundU = kTRUE;
                        }
                    }  //for kk
                    for (Int_t kk = 0; kk < single_ub; kk++) {
                        if (Abs(u_single[1][kk] - u_est) > 1.2)
                            continue;  //????? 0.5 needs to be reviewed
                        if (Abs(u_single[1][kk] - u_est) < min_b) {
                            min_b = Abs(u_single[1][kk] - u_est);
                            rh_seg[5][nDC_segments] = u_single[1][kk];
                            rh_sigm_seg[5][nDC_segments] = sigm_u_single[1][kk];
                            foundU = kTRUE;
                        }
                    }  //for kk
                    if (nDC_segments > 25 * N - 1)
                        break;
                }  //!foundU

                if (nDC_segments > 25 * N - 1)
                    break;
            }  //m
            //            }//(pair_v2>0)
            if (!foundV && foundU) {
                Double_t min_a = 999;
                Double_t min_b = 999;
                for (Int_t kk = 0; kk < single_va; kk++) {
                    if (Abs(v_single[0][kk] - v_est) > 1.2)
                        continue;  //????? 0.5 needs to be reviewed
                    if (Abs(v_single[0][kk] - v_est) < min_a) {
                        min_a = Abs(v_single[0][kk] - v_est);
                        rh_seg[6][nDC_segments] = v_single[0][kk];
                        rh_sigm_seg[6][nDC_segments] = sigm_v_single[0][kk];
                        foundV = kTRUE;
                    }
                }  //for kk
                for (Int_t kk = 0; kk < single_vb; kk++) {
                    if (Abs(v_single[1][kk] - v_est) > 1.2)
                        continue;  //????? 0.5 needs to be reviewed
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

Bool_t BmnDchTrackFinder::FitDchSegments(Int_t dchID, Int_t* size_seg, Double_t** rh_seg, Double_t** rh_sigm_seg, Double_t** par_ab, Double_t* chi2, Double_t* x_glob, Double_t* y_glob) {
    Int_t nDC_segments = (dchID == 1) ? nSegments[0] : nSegments[1];
    Bool_t hasSuffNumberOfSegments = kFALSE;
    for (Int_t j = 0; j < nDC_segments; j++) {
        Int_t worst_hit = -1;
        Double_t max_resid = 0;

        Double_t _rh_seg[8];
        Double_t _rh_sigm_seg[8];
        Double_t _par_ab[4];

        for (Int_t i = 0; i < 8; i++)
            if (Abs(rh_seg[i][j] + 999.) > FLT_EPSILON)
                size_seg[j]++;

        for (Int_t rej = 0; rej < 2; rej++) {  //allow 2 passes max 8->7 & 7->6
            for (Int_t i = 0; i < 8; i++) {
                _rh_seg[i] = rh_seg[i][j];
                _rh_sigm_seg[i] = rh_sigm_seg[i][j];
            }

            fit_seg(z_loc, _rh_seg, _rh_sigm_seg, _par_ab, -1, -1);  //usual fit without skipping any plane
            for (Int_t i = 0; i < 4; i++)
                par_ab[i][j] = _par_ab[i];

            chi2[j] = 0;

            Double_t resid(LDBL_MAX);
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
                    rh_seg[worst_hit][j] = -999.;  //erase worst hit and refit
                    size_seg[j]--;
                    max_resid = 0;
                    continue;
                }
        }

        // Add shifts to slopes and coords
        Double_t x_slope_sh = 0.0;//(dchID == 1) ? x1_slope_sh : x2_slope_sh;
        Double_t y_slope_sh = 0.0;//(dchID == 1) ? y1_slope_sh : y2_slope_sh;
        Double_t x_sh = 0.0;//(dchID == 1) ? x1_sh : x2_sh;
        Double_t y_sh = 0.0;//(dchID == 1) ? y1_sh : y2_sh;

        par_ab[0][j] += x_slope_sh + x_slope_sh * par_ab[0][j] * par_ab[0][j];
        par_ab[2][j] += y_slope_sh + y_slope_sh * par_ab[2][j] * par_ab[2][j];
        par_ab[1][j] += x_sh;
        par_ab[3][j] += y_sh;

        Int_t coeff = (dchID == 1) ? 1 : -1;
        x_glob[j] = coeff * par_ab[0][j] * dZ_dch_mid + par_ab[1][j];
        y_glob[j] = coeff * par_ab[2][j] * dZ_dch_mid + par_ab[3][j];
        if (size_seg[j] > 5)
            hasSuffNumberOfSegments = kTRUE;
    }
    return hasSuffNumberOfSegments;
}

void BmnDchTrackFinder::CompareDaDb(Double_t d, Double_t& ele) {
    ele = (d < 0.02) ? (0.08 * 0.08) : (d >= 0.02 && d < 0.1) ? (0.06 * 0.06) : (d >= 0.1 && d < 0.4) ? (0.025 * 0.025) : (d >= 0.4 && d < 0.41) ? (0.08 * 0.08) : (0.10 * 0.10);
}

void BmnDchTrackFinder::CompareDaDb(Double_t d, Double_t& ele1, Double_t& ele2) {
    ele1 = (d < 0.02) ? (0.08 * 0.08) : (d >= 0.02 && d < 0.1) ? (0.06 * 0.06) : (d >= 0.1 && d < 0.4) ? (0.025 * 0.025) : (d >= 0.4 && d < 0.41) ? (0.08 * 0.08) : (0.10 * 0.10);
    ele2 = ele1;
}

void BmnDchTrackFinder::SelectLongestAndBestSegments(Int_t dchID, Int_t* size_seg, Double_t** rh_seg, Double_t* chi2) {
    Int_t nDC_segments = (dchID == 1) ? nSegments[0] : nSegments[1];
    for (Int_t max_size = 8; max_size > 5; max_size--)
        for (Int_t it1 = 0; it1 < nDC_segments; it1++) {
            if (size_seg[it1] != max_size || chi2[it1] > 998.)
                continue;
            for (Int_t it2 = 0; it2 < nDC_segments; it2++) {
                if (it2 == it1 || chi2[it2] > 998.)
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

void BmnDchTrackFinder::FillSegmentParametersSigmas(Int_t dchID, Double_t** rh_seg, Double_t** rh_sigm_seg, Double_t* chi2, Double_t** sigm_seg_par) {
    Int_t nDC_segments = (dchID == 1) ? nSegments[0] : nSegments[1];
    for (Int_t it1 = 0; it1 < nDC_segments; it1++) {
        if (chi2[it1] > 990.) continue;
        //calculate segment param errors
        Double_t XSum = 0.;
        Double_t XSumZ = 0.;
        Double_t XSumZZ = 0.;
        Double_t YSum = 0.;
        Double_t YSumZ = 0.;
        Double_t YSumZZ = 0.;
        for (int i = 0; i < 8; i++) {
            if (rh_seg[i][it1] == -999 || i == 2 || i == 3) continue;
            if (i == 0 || i == 1) {
                XSum += 1. / rh_sigm_seg[i][it1];
                XSumZ += z_loc[i] / rh_sigm_seg[i][it1];
                XSumZZ += z_loc[i] * z_loc[i] / rh_sigm_seg[i][it1];
            } else {
                XSum += 1. / (2. * rh_sigm_seg[i][it1]);
                XSumZ += z_loc[i] / (2. * rh_sigm_seg[i][it1]);
                XSumZZ += z_loc[i] * z_loc[i] / (2. * rh_sigm_seg[i][it1]);
            }
        }
        for (int i = 0; i < 8; i++) {
            if (rh_seg[i][it1] == -999 || i == 0 || i == 1) continue;
            if (i == 2 || i == 3) {
                YSum += 1. / rh_sigm_seg[i][it1];
                YSumZ += z_loc[i] / rh_sigm_seg[i][it1];
                YSumZZ += z_loc[i] * z_loc[i] / rh_sigm_seg[i][it1];
            } else {
                YSum += 1. / (2. * rh_sigm_seg[i][it1]);
                YSumZ += z_loc[i] / (2. * rh_sigm_seg[i][it1]);
                YSumZZ += z_loc[i] * z_loc[i] / (2. * rh_sigm_seg[i][it1]);
            }
        }
        Double_t Xdelta = XSum * XSumZZ - XSumZ * XSumZ;
        Double_t Ydelta = YSum * YSumZZ - YSumZ * YSumZ;
        sigm_seg_par[0][it1] = sqrt(XSum / Xdelta);
        sigm_seg_par[1][it1] = sqrt(XSumZZ / Xdelta);
        sigm_seg_par[2][it1] = sqrt(YSum / Ydelta);
        sigm_seg_par[3][it1] = sqrt(YSumZZ / Ydelta);
    }
}

void BmnDchTrackFinder::CreateDchTrack(Int_t dchID, Double_t* chi2Arr, Double_t** parArr, Int_t* sizeArr, Double_t** rh_seg, Double_t** sigm_seg_par) {
    Int_t nDC_segments = (dchID == 1) ? nSegments[0] : nSegments[1];
    Int_t good_dc_segs = 0;
    for (Int_t iSegment = 0; iSegment < nDC_segments; iSegment++) {
        if (chi2Arr[iSegment] > 990.)
            continue;
        good_dc_segs++;
        FairTrackParam trackParam;
        Double_t z0 = (dchID == 1) ? Z_dch1 : Z_dch2;
        Double_t x0 = parArr[1][iSegment];
        Double_t y0 = parArr[3][iSegment];
        Double_t x_align = 0.0;//(dchID == 1) ? -10.89 : +4.93;
        Double_t y_align = 0.0;//(dchID == 1) ? -2.95 : +9.12;
        Double_t tx_align = 0.0;//(dchID == 1) ? +0.073 + 0.049 : +0.070 + 0.052;
        Double_t ty_align = 0.0;//(dchID == 1) ? +0.054 : +0.062 - 0.047;
        trackParam.SetPosition(TVector3(-x0 + x_align, y0 + y_align, z0));
        trackParam.SetTx(-parArr[0][iSegment] + tx_align);
        trackParam.SetTy(parArr[2][iSegment] + ty_align);
        trackParam.SetCovariance(0, 0, sigm_seg_par[0][iSegment] * sigm_seg_par[0][iSegment]);  // bx^2
        trackParam.SetCovariance(1, 1, sigm_seg_par[1][iSegment] * sigm_seg_par[1][iSegment]);  // ax^2
        trackParam.SetCovariance(2, 2, sigm_seg_par[2][iSegment] * sigm_seg_par[2][iSegment]);  // by^2
        trackParam.SetCovariance(3, 3, sigm_seg_par[3][iSegment] * sigm_seg_par[3][iSegment]);  // ay^2
        BmnDchTrack* track = new ((*fDchTracks)[fDchTracks->GetEntriesFast()]) BmnDchTrack();
        track->SetChi2(chi2Arr[iSegment]);
        track->SetNHits(sizeArr[iSegment]);
        track->SetParamFirst(trackParam);
        if (dchID == 1) {
            haX1->Fill(-parArr[0][iSegment]);
            haY1->Fill(parArr[2][iSegment]);
            hx1->Fill(parArr[1][iSegment]);
            hy1->Fill(parArr[3][iSegment]);
            hlocXY1->Fill(rh_seg[0][iSegment], rh_seg[2][iSegment]);
            hXvsAx->Fill(parArr[1][iSegment], -parArr[0][iSegment]);
        } else {
            haX2->Fill(-parArr[0][iSegment]);
            haY2->Fill(parArr[2][iSegment]);
            hx2->Fill(parArr[1][iSegment]);
            hy2->Fill(parArr[3][iSegment]);
            hlocXY2->Fill(rh_seg[0][iSegment], rh_seg[2][iSegment]);
        }
    }
    if (dchID == 1) {
        Ntrack1->Fill(good_dc_segs);
    } else {
        Ntrack2->Fill(good_dc_segs);
    }
}

void BmnDchTrackFinder::CreateDchTrack() {
    for (Int_t iSeg = 0; iSeg < nSegmentsToBeMatched + 1; iSeg++) {
        FairTrackParam trackParam;
        Double_t z0 = Z_dch_mid;
        Double_t x0 = -x_mid[iSeg];
        Double_t y0 = y_mid[iSeg];
        // trackParam.SetPosition(TVector3(x0 - 3.25, y0 + 4.91, z0));  // Go to right reference frame
        // trackParam.SetTx(aX[iSeg] + 0.073);                          // Go to right reference frame
        // trackParam.SetTy(aY[iSeg] + 0.061);
        trackParam.SetPosition(TVector3(x0, y0, z0));  // Go to right reference frame
        trackParam.SetTx(aX[iSeg]);                          // Go to right reference frame
        trackParam.SetTy(aY[iSeg]);
        Int_t iseg1 = int((pairID[iSeg] - 1000) / 1000);
        Int_t iseg2 = (pairID[iSeg] % 1000) - 1;
        Double_t sigma2_dx = ((params_sigmas[1][0][iseg2] * params_sigmas[1][0][iseg2]) +
                              (params_sigmas[0][0][iseg1] * params_sigmas[0][0][iseg1])) *
                                 99.75 * 99.75 +
                             ((params_sigmas[0][1][iseg1]) * (params_sigmas[0][1][iseg1])) +
                             ((params_sigmas[1][1][iseg2]) * (params_sigmas[1][1][iseg2]));

        Double_t sigma2_dy = ((params_sigmas[1][2][iseg2] * params_sigmas[1][2][iseg2]) +
                              (params_sigmas[0][2][iseg1] * params_sigmas[0][2][iseg1])) *
                                 99.75 * 99.75 +
                             ((params_sigmas[0][3][iseg1]) * (params_sigmas[0][3][iseg1])) +
                             ((params_sigmas[1][3][iseg2]) * (params_sigmas[1][3][iseg2]));

        Double_t sigma2_dax = 9. * ((params_sigmas[1][0][iseg2] * params_sigmas[1][0][iseg2]) +
                                    (params_sigmas[0][0][iseg1] * params_sigmas[0][0][iseg1]));

        Double_t sigma2_day = 9. * ((params_sigmas[1][2][iseg2] * params_sigmas[1][2][iseg2]) +
                                    (params_sigmas[0][2][iseg1] * params_sigmas[0][2][iseg1]));
        trackParam.SetCovariance(0, 0, sigma2_dx);
        trackParam.SetCovariance(1, 1, sigma2_dax);
        trackParam.SetCovariance(2, 2, sigma2_dy);
        trackParam.SetCovariance(3, 3, sigma2_day);
        BmnDchTrack* track = new ((*fDchTracks)[fDchTracks->GetEntriesFast()]) BmnDchTrack();
        track->SetChi2(Chi2_match[iSeg]);
        track->SetNHits(nhits[iSeg]);
        track->SetFlag(pairID[iSeg]);
        track->SetParamFirst(trackParam);
    }
}

Double_t BmnDchTrackFinder::CalculateResidual(Int_t i, Int_t j, Double_t** rh_seg, Double_t** par_ab) {
    Double_t sqrt_2 = sqrt(2.);
    Double_t isqrt_2 = 1 / sqrt_2;

    return (i < 2) ? rh_seg[i][j] - z_loc[i] * par_ab[0][j] - par_ab[1][j] : (i >= 2 && i < 4) ? rh_seg[i][j] - z_loc[i] * par_ab[2][j] - par_ab[3][j] : (i >= 4 && i < 6) ? rh_seg[i][j] - isqrt_2 * z_loc[i] * (par_ab[2][j] - par_ab[0][j]) - isqrt_2 * (par_ab[3][j] - par_ab[1][j]) : rh_seg[i][j] - isqrt_2 * z_loc[i] * (par_ab[2][j] + par_ab[0][j]) - isqrt_2 * (par_ab[3][j] + par_ab[1][j]);
}

InitStatus BmnDchTrackFinder::Init() {
    if (!expData) {
        cout << "BmnDchTrackFinder::Init(): simulation data is not supported! Task will be deactivated" << endl;
        SetActive(kFALSE);
        return kERROR;
    }

    if (fVerbose) cout << "BmnDchTrackFinder::Init()" << endl;
    FairRootManager* ioman = FairRootManager::Instance();

    fBmnDchDigitsArray = (TClonesArray*)ioman->GetObject(InputDigitsBranchName);
    if (!fBmnDchDigitsArray) {
        cout << "BmnDchTrackFinder::Init(): branch " << InputDigitsBranchName << " not found! Task will be deactivated" << endl;
        SetActive(kFALSE);
        return kERROR;
    }

    // Create and register track arrays

    fDchTracks = new TClonesArray(tracksDch.Data());
    ioman->Register(tracksDch.Data(), "DCH", fDchTracks, kTRUE);

    ifstream fin;
    TString dir = getenv("VMCWORKDIR");
    dir += "/input/";
    fin.open((TString(dir + fTransferFunctionName)).Data(), ios::in);
    for (Int_t fi = 0; fi < 16; fi++) {
        fin >> t_dc[0][fi] >> t_dc[1][fi] >> t_dc[2][fi] >> t_dc[3][fi] >>
            pol_par_dc[0][0][fi] >> pol_par_dc[0][1][fi] >> pol_par_dc[0][2][fi] >> pol_par_dc[0][3][fi] >> pol_par_dc[0][4][fi] >>
            pol_par_dc[1][0][fi] >> pol_par_dc[1][1][fi] >> pol_par_dc[1][2][fi] >> pol_par_dc[1][3][fi] >> pol_par_dc[1][4][fi] >>
            pol_par_dc[2][0][fi] >> pol_par_dc[2][1][fi] >> pol_par_dc[2][2][fi] >> pol_par_dc[2][3][fi] >> pol_par_dc[2][4][fi] >>
            scaling[fi];
    }
    fin.close();

    // z local xa->vb (cm)
    Double_t arr1[8] = {7.8, 6.6, 3.0, 1.8, -1.8, -3.0, -6.6, -7.8};
    for (Int_t iSize = 0; iSize < 8; iSize++)
        z_loc[iSize] = arr1[iSize];

    // z global dc 1 & dc 2 (cm)
    Double_t arr2[16] = {-45.7, -46.9, -51.5, -52.7, -57.3, -58.5, -63.1, -64.3, 64.3, 63.1, 58.5, 57.3, 52.7, 51.5, 46.9, 45.7};
    for (Int_t iSize = 0; iSize < 16; iSize++)
        z_glob[iSize] = arr2[iSize];

    has7DC = new Bool_t[nChambers];
    x_mid = new Double_t[25 * N];
    y_mid = new Double_t[25 * N];
    pairID = new Int_t[25 * N];
    nhits = new Int_t[25 * N];
    aX = new Double_t[25 * N];
    aY = new Double_t[25 * N];
    imp = new Double_t[25 * N];
    leng = new Double_t[25 * N];
    Chi2_match = new Double_t[25 * N];
    v = new Double_t**[nChambers];
    u = new Double_t**[nChambers];
    y = new Double_t**[nChambers];
    x = new Double_t**[nChambers];
    v_Single = new Double_t**[nChambers];
    u_Single = new Double_t**[nChambers];
    y_Single = new Double_t**[nChambers];
    x_Single = new Double_t**[nChambers];
    sigm_v = new Double_t**[nChambers];
    sigm_u = new Double_t**[nChambers];
    sigm_y = new Double_t**[nChambers];
    sigm_x = new Double_t**[nChambers];
    Sigm_v_single = new Double_t**[nChambers];
    Sigm_u_single = new Double_t**[nChambers];
    Sigm_y_single = new Double_t**[nChambers];
    Sigm_x_single = new Double_t**[nChambers];
    segment_size = new Int_t*[nChambers];
    Chi2 = new Double_t*[nChambers];
    x_global = new Double_t*[nChambers];
    y_global = new Double_t*[nChambers];
    params = new Double_t**[nChambers];
    params_sigmas = new Double_t**[nChambers];
    rh_segment = new Double_t**[nChambers];
    rh_sigm_segment = new Double_t**[nChambers];
    for (Int_t iChamber = 0; iChamber < nChambers; iChamber++) {
        v[iChamber] = new Double_t*[N];
        u[iChamber] = new Double_t*[N];
        y[iChamber] = new Double_t*[N];
        x[iChamber] = new Double_t*[N];
        v_Single[iChamber] = new Double_t*[N];
        u_Single[iChamber] = new Double_t*[N];
        y_Single[iChamber] = new Double_t*[N];
        x_Single[iChamber] = new Double_t*[N];
        sigm_v[iChamber] = new Double_t*[N];
        sigm_u[iChamber] = new Double_t*[N];
        sigm_y[iChamber] = new Double_t*[N];
        sigm_x[iChamber] = new Double_t*[N];
        Sigm_v_single[iChamber] = new Double_t*[N];
        Sigm_u_single[iChamber] = new Double_t*[N];
        Sigm_y_single[iChamber] = new Double_t*[N];
        Sigm_x_single[iChamber] = new Double_t*[N];
        segment_size[iChamber] = new Int_t[75 * N];
        Chi2[iChamber] = new Double_t[75 * N];
        x_global[iChamber] = new Double_t[75 * N];
        y_global[iChamber] = new Double_t[75 * N];
        params[iChamber] = new Double_t*[4];
        params_sigmas[iChamber] = new Double_t*[4];
        rh_segment[iChamber] = new Double_t*[8];
        rh_sigm_segment[iChamber] = new Double_t*[8];
        for (Int_t iDim = 0; iDim < N; iDim++) {
            v[iChamber][iDim] = new Double_t[75 * N];
            u[iChamber][iDim] = new Double_t[75 * N];
            y[iChamber][iDim] = new Double_t[75 * N];
            x[iChamber][iDim] = new Double_t[75 * N];
            v_Single[iChamber][iDim] = new Double_t[20 * N];
            u_Single[iChamber][iDim] = new Double_t[20 * N];
            y_Single[iChamber][iDim] = new Double_t[20 * N];
            x_Single[iChamber][iDim] = new Double_t[20 * N];
            sigm_v[iChamber][iDim] = new Double_t[75 * N];
            sigm_u[iChamber][iDim] = new Double_t[75 * N];
            sigm_y[iChamber][iDim] = new Double_t[75 * N];
            sigm_x[iChamber][iDim] = new Double_t[75 * N];
            Sigm_v_single[iChamber][iDim] = new Double_t[20 * N];
            Sigm_u_single[iChamber][iDim] = new Double_t[20 * N];
            Sigm_y_single[iChamber][iDim] = new Double_t[20 * N];
            Sigm_x_single[iChamber][iDim] = new Double_t[20 * N];
        }
        for (Int_t iDim = 0; iDim < 4; iDim++) {
            params[iChamber][iDim] = new Double_t[75 * N];
            params_sigmas[iChamber][iDim] = new Double_t[75 * N];
        }
        for (Int_t iDim = 0; iDim < 8; iDim++) {
            rh_segment[iChamber][iDim] = new Double_t[75 * N];
            rh_sigm_segment[iChamber][iDim] = new Double_t[75 * N];
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
    nSegmentsToBeMatched = -1;
    fDchTracks->Clear();

    // Array cleaning and initializing
    for (Int_t iChamber = 0; iChamber < nChambers; iChamber++) {
        for (Int_t iWire = 0; iWire < nWires; iWire++) {
            pairs[iChamber][iWire] = 0;
            for (Int_t iLayer = 0; iLayer < nLayers; iLayer++)
                singles[iChamber][iWire][iLayer] = 0;
        }

        has7DC[iChamber] = kFALSE;

        for (Int_t iDim1 = 0; iDim1 < 4; iDim1++)
            for (Int_t iDim2 = 0; iDim2 < 75 * N; iDim2++) {
                params[iChamber][iDim1][iDim2] = -999.;
                params_sigmas[iChamber][iDim1][iDim2] = -999.;
            }
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
        for (Int_t iDim1 = 0; iDim1 < 8; iDim1++)
            for (Int_t iDim2 = 0; iDim2 < 75 * N; iDim2++) {
                rh_segment[iChamber][iDim1][iDim2] = -999.;
                rh_sigm_segment[iChamber][iDim1][iDim2] = 1.;
            }
    }
    for (Int_t iSegment = 0; iSegment < nSegmentsMax; iSegment++)
        nSegments[iSegment] = 0;
    for (Int_t iDim = 0; iDim < 25 * N; iDim++) {
        x_mid[iDim] = -999.;
        y_mid[iDim] = -999.;
        pairID[iDim] = -999.;
        nhits[iDim] = 0.;
        aX[iDim] = -999.;
        aY[iDim] = -999.;
        leng[iDim] = -999.;
        imp[iDim] = -999.;
    }
}

void BmnDchTrackFinder::Finish() {
    //added
    //===============================================================================================================

    TFile file(fhTestFlnm.Data(), "RECREATE");
    fhList.Write();
    file.Close();

    //===============================================================================================================

    ///end

    cout << "Work time of the DCH track finder: " << workTime << " s" << endl;
}

Int_t BmnDchTrackFinder::Reconstruction(Int_t dchID, TString wire, Int_t pair, Int_t it_a, Int_t it_b,
                                        Double_t* wirenr_a, Double_t* wirenr_b, Double_t* time_a, Double_t* time_b,
                                        Bool_t* used_a, Bool_t* used_b,
                                        Double_t** _ab, Double_t** sigm_ab) {
    const Int_t arrIdxShift = (dchID == 2) ? 8 : 0;
    const Int_t arrIdxStart = (wire == "x") ? 0 : (wire == "y") ? 2 : (wire == "u") ? 4 : 6;

    Double_t a_pm[2], b_pm[2];

    for (Int_t i = 0; i < it_a; ++i)
        for (Int_t j = 0; j < it_b; ++j) {
            if (pair > 25 * N - 1)
                break;
            if ((wirenr_a[i] != wirenr_b[j] && wirenr_a[i] != wirenr_b[j] + 1))
                continue;
            Int_t func_nr_a = -1;
            Int_t func_nr_b = -1;
            for (Int_t t_it = 0; t_it < 3; t_it++){
                if (time_a[i] >= t_dc[t_it][0 + arrIdxStart + arrIdxShift] && time_a[i] < t_dc[t_it + 1][0 + arrIdxStart + arrIdxShift]) {
                    func_nr_a = t_it;
                    break;
                }
            }
            if (func_nr_a == -1) break;
            Double_t time = time_a[i] - t_dc[0][0 + arrIdxStart + arrIdxShift];
            Double_t d_a = 0;
            Double_t d_b = 0;

            d_a = scale * (pol_par_dc[func_nr_a][0][0 + arrIdxStart + arrIdxShift] +
                           pol_par_dc[func_nr_a][1][0 + arrIdxStart + arrIdxShift] * time +
                           pol_par_dc[func_nr_a][2][0 + arrIdxStart + arrIdxShift] * Power(time, 2) +
                           pol_par_dc[func_nr_a][3][0 + arrIdxStart + arrIdxShift] * Power(time, 3) +
                           pol_par_dc[func_nr_a][4][0 + arrIdxStart + arrIdxShift] * Power(time, 4));

            for (Int_t t_it = 0; t_it < 3; t_it++)
                if (time_b[j] >= t_dc[t_it][1 + arrIdxStart + arrIdxShift] && time_b[j] < t_dc[t_it + 1][1 + arrIdxStart + arrIdxShift]) {
                    func_nr_b = t_it;
                    break;
                }
            if (func_nr_b == -1) continue;
            time = time_b[j] - t_dc[0][1 + arrIdxStart + arrIdxShift];

            d_b = scale * (pol_par_dc[func_nr_b][0][1 + arrIdxStart + arrIdxShift] +
                           pol_par_dc[func_nr_b][1][1 + arrIdxStart + arrIdxShift] * time +
                           pol_par_dc[func_nr_b][2][1 + arrIdxStart + arrIdxShift] * Power(time, 2) +
                           pol_par_dc[func_nr_b][3][1 + arrIdxStart + arrIdxShift] * Power(time, 3) +
                           pol_par_dc[func_nr_b][4][1 + arrIdxStart + arrIdxShift] * Power(time, 4));

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
            if (arrIdxStart == 0) {
                if (arrIdxShift == 0) hx1Da_Db->Fill(d_a + d_b - .5);
                if (arrIdxShift == 8) hx2Da_Db->Fill(d_a + d_b - .5);
            }
            if (arrIdxStart == 2) {
                if (arrIdxShift == 0) hy1Da_Db->Fill(d_a + d_b - .5);
                if (arrIdxShift == 8) hy2Da_Db->Fill(d_a + d_b - .5);
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
                                              Double_t** _single, Double_t** sigm_single) {
    const Int_t arrIdxStart = (wire == "x") ? 0 : (wire == "y") ? 2 : (wire == "u") ? 4 : 6;

    const Int_t arrIdx1 = (lay == "a") ? 0 : 1;
    const Int_t arrIdx2 = (dchID == 2) ? 8 : 0;
    const Double_t coeff = (lay == "a") ? 119 : 118.5;

    for (Int_t i = 0; i < it; ++i) {
        if (used[i])
            continue;

        Int_t func_nr = -1;
        for (Int_t t_it = 0; t_it < 3; t_it++) {
            if (time_[i] >= t_dc[t_it][0 + arrIdxStart + arrIdx1 + arrIdx2] && time_[i] < t_dc[t_it + 1][0 + arrIdxStart + arrIdx1 + arrIdx2]) {
                func_nr = t_it;
                break;
            }
        }
        if (func_nr == -1) continue;
        Double_t time = time_[i] - t_dc[0][0 + arrIdxStart + arrIdx1 + arrIdx2];
        Double_t d = 0;

        d = scale * (pol_par_dc[func_nr][0][0 + arrIdx1 + arrIdx2] +
                     pol_par_dc[func_nr][1][0 + arrIdx1 + arrIdx2] * time +
                     pol_par_dc[func_nr][2][0 + arrIdxStart + arrIdx1 + arrIdx2] * Power(time, 2) +
                     pol_par_dc[func_nr][3][0 + arrIdxStart + arrIdx1 + arrIdx2] * Power(time, 3) +
                     pol_par_dc[func_nr][4][0 + arrIdxStart + arrIdx1 + arrIdx2] * Power(time, 4));

        _single[0 + arrIdx1][single] = wirenr[i] - coeff + d;
        _single[0 + arrIdx1][single + 1] = wirenr[i] - coeff - d;

        CompareDaDb(d, sigm_single[0 + arrIdx1][single], sigm_single[0 + arrIdx1][single + 1]);

        single += 2;
    }
    return single;
}
ClassImp(BmnDchTrackFinder)
