#include <fstream>
#include <stdio.h>
#include <iostream>
#include "TMath.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TCanvas.h"
#include "TChain.h"
#include "TFile.h"
#include "TTree.h"
#include "TClonesArray.h"
#include "TBranch.h"
#include "TGeoManager.h"
using namespace std;
using namespace TMath;

const Int_t nWires = 96;
const Int_t nPlanes = 6;
const Int_t big = 100;
const Float_t dw = 0.25; // cm
const Double_t sq3 = Sqrt(3.);
const Double_t Chi2_Max = 20;
// Geometry for RUN3
const Float_t Zmid = 16.5; //average plane of chamber 2
const Float_t Zmid3 = -16.5; //average plane of chamber 3
const Float_t Z_Ch1 = -150; //average plane of chamber 1
const Float_t Z_Ch2 = 453; // global
const Float_t Z_Ch3 = 486; // global

void SegmentFinder(Int_t chNum, Int_t wires_Ch[nPlanes][big], Float_t XVU_Ch[nPlanes][big], Int_t Nhits_Ch[big], Int_t iw_Ch[nPlanes], Int_t &Nseg, Int_t wires_glob[nWires][nPlanes], Float_t xuv_glob[nWires][nPlanes], Int_t minHits, Short_t code) {
    //xuv_glob     - coordinates of all hits
    //wires_glob   - wires of all hits
    Float_t delta = (chNum == 1) ? 0.5 : 1.15; //condition for the nearest "lighted" wires
    Int_t cntr_sumWW_x = (chNum == 1) ? 95 : 96;//center wires Wx1+Wx2 due to x-slope
    Int_t min_sumWW_x =  (chNum == 1) ? 2 : 3; //min sum of wires to 95 ^ 96
    Int_t cntr_sumWW =  95 ;//center wires W1+W2 due to u,v-slope
    Int_t min_sumWW =  2 ; //min sum of wires to 95 
    Int_t minHits4_5= minHits;

    // code :
    // 1  {X-, V-, U+}
    // 2  {X-, V+, U+}
    // 3  {X-, V-, U-}
    // 4  {X+, V-, U+}
    // 5  {X+, V+, U+}
    // 6  {X+, V-, U-}
    // 7  {X+, V-, U+}
    // 8  {X-, V-, U-}
    Int_t x, v, u, x1, v1, u1;

    switch (code) {
        case 1:
            x = 0;
            v = 1;
            u = 2;
            x1 = 3;
            v1 = 4;
            u1 = 5;
            break;
        case 2:
            x = 0;
            v = 4;
            u = 2;
            x1 = 3;
            v1 = 1;
            u1 = 5;
            break;
        case 3:
            x = 0;
            v = 1;
            u = 5;
            x1 = 3;
            v1 = 4;
            u1 = 2;
            break;
        case 4:
            x = 3;
            v = 1;
            u = 2;
            x1 = 0;
            v1 = 4;
            u1 = 5;
            break;
        case 5:
            x = 3;
            v = 4;
            u = 2;
            x1 = 0;
            v1 = 1;
            u1 = 5;
            break;
        case 6:
            x = 3;
            v = 1;
            u = 5;
            x1 = 0;
            v1 = 4;
            u1 = 2;
            break;
    case 7:
            x = 0;
            v = 4;
            u = 5;
            x1 = 3;
            v1 = 1;
            u1 = 2;
            break;
    case 8:
            x = 3;
            v = 4;
            u = 5;
            x1 = 0;
            v1 = 1;
            u1 = 2;
            break;

    }

    if (iw_Ch[x] > 0) {
        for (Int_t ix = 0; ix < iw_Ch[x]; ix++) {
            if (iw_Ch[v] > 0) {
                for (Int_t iv = 0; iv < iw_Ch[v]; iv++) {
                    if (iw_Ch[u] > 0) {
                        for (Int_t iu = 0; iu < iw_Ch[u]; iu++) {
                            if (Nseg > big - 1) break;
                            Bool_t it_was = 0;
                            if (Nseg > 0) {
                                for (Int_t iseg = 0; iseg < Nseg; iseg++) {
                                    if (wires_Ch[x][iseg] == wires_glob[ix][x] && wires_Ch[v][iseg] == wires_glob[iv][v] && wires_Ch[u][iseg] == wires_glob[iu][u]) {
                                        it_was = 1;
                                        break;
                                    }
                                }
                            }
                            if (it_was) continue;
                            if (fabs(xuv_glob[iu][u] + xuv_glob[iv][v] - xuv_glob[ix][x]) < delta) {
                                XVU_Ch[x][Nseg] = xuv_glob[ix][x];
                                XVU_Ch[v][Nseg] = xuv_glob[iv][v];
                                XVU_Ch[u][Nseg] = xuv_glob[iu][u];
                                wires_Ch[x1][Nseg] = -1;
                                wires_Ch[v1][Nseg] = -1;
                                wires_Ch[u1][Nseg] = -1;
                                wires_Ch[x][Nseg] = wires_glob[ix][x];
                                wires_Ch[v][Nseg] = wires_glob[iv][v];
                                wires_Ch[u][Nseg] = wires_glob[iu][u];
                                Nhits_Ch[Nseg] = 3;
                                if (iw_Ch[u1] > 0) {
				  for (Int_t iu2 = 0; iu2 < iw_Ch[u1]; iu2++) {
				     
				      if (abs( wires_glob[iu][u] + wires_glob[iu2][u1] - cntr_sumWW) <  min_sumWW ) {
                                            XVU_Ch[u1][Nseg] = xuv_glob[iu2][u1];
                                            wires_Ch[u1][Nseg] = wires_glob[iu2][u1];
                                            Nhits_Ch[Nseg] = 4;
					}
                                    }
                                }
                                if (iw_Ch[v1] > 0) {
				  for (Int_t iv2 = 0; iv2 < iw_Ch[v1]; iv2++) {
				   
				    if (abs( wires_glob[iv][v] + wires_glob[iv2][v1] - cntr_sumWW) <  min_sumWW ) {
                                            XVU_Ch[v1][Nseg] = xuv_glob[iv2][v1];
                                            wires_Ch[v1][Nseg] = wires_glob[iv2][v1];
                                        }
                                    }
				  if ( wires_Ch[v1][Nseg] > -1 )  Nhits_Ch[Nseg] = Nhits_Ch[Nseg] + 1;//5 points
                                }
                                if (iw_Ch[x1] > 0) {
				  for (Int_t ix2 = 0; ix2 < iw_Ch[x1]; ix2++) {
				      
				      if (abs( wires_glob[ix][x] + wires_glob[ix2][x1] - cntr_sumWW_x) <  min_sumWW_x ) {
                                            XVU_Ch[x1][Nseg] = xuv_glob[ix2][x1];
                                            wires_Ch[x1][Nseg] = wires_glob[ix2][x1];
                                         }
                                    }
				    if ( wires_Ch[x1][Nseg] > -1 )  Nhits_Ch[Nseg] = Nhits_Ch[Nseg] + 1;//6 points
                                }
                                if (Nhits_Ch[Nseg] >= minHits4_5) {
                                    Nseg++;
				    if (Nseg > 25) minHits4_5=5;
                                } else {
                                    Nhits_Ch[Nseg] = 0;
                                    wires_Ch[x1][Nseg] = -1;
                                    wires_Ch[v1][Nseg] = -1;
                                    wires_Ch[u1][Nseg] = -1;
				    wires_Ch[x][Nseg] = -1;
                                    wires_Ch[v][Nseg] = -1;
                                    wires_Ch[u][Nseg] = -1;
                                }
                            }
                        }//iu
                    }//u 
                }//iv
            }//v
        }//ix
    }//x
}

void FillFitMatrix(Double_t A[4][4], Float_t z[nPlanes], Float_t sigm2[nPlanes], Int_t h[nPlanes]) {

    // A - matrix to be filled
    // z - local z-positions of planes(layers)
    // sigm2 - square of sigma
    // h - array to include/exclude planes (h[i] = 0 or 1)

    Float_t z2[nPlanes] = {z[0] * z[0], z[1] * z[1], z[2] * z[2], z[3] * z[3], z[4] * z[4], z[5] * z[5]}; //cm

    A[0][0] = 2 * z2[0] * h[0] / sigm2[0] + z2[2] * h[2] / (2 * sigm2[2]) + z2[1] * h[1] / (2 * sigm2[1]) + 2 * z2[3] * h[3] / sigm2[3] + z2[5] * h[5] / (2 * sigm2[5]) + z2[4] * h[4] / (2 * sigm2[4]); //Ax
    A[0][1] = 2 * z[0] * h[0] / sigm2[0] + z[2] * h[2] / (2 * sigm2[2]) + z[1] * h[1] / (2 * sigm2[1]) + 2 * z[3] * h[3] / (sigm2[3]) + z[5] * h[5] / (2 * sigm2[5]) + z[4] * h[4] / (2 * sigm2[4]); //Bx
    A[0][2] = sq3 * (z2[2] * h[2] / (2 * sigm2[2]) - z2[1] * h[1] / (2 * sigm2[1]) + z2[5] * h[5] / (2 * sigm2[5]) - z2[4] * h[4] / (2 * sigm2[4])); //Ay
    A[0][3] = sq3 * (z[2] * h[2] / (2 * sigm2[2]) - z[1] * h[1] / (2 * sigm2[1]) + z[5] * h[5] / (2 * sigm2[5]) - z[4] * h[4] / (2 * sigm2[4])); //By
    A[1][0] = A[0][1];
    A[1][1] = 2 * h[0] / sigm2[0] + 0.5 * h[2] / sigm2[2] + 0.5 * h[1] / sigm2[1] + 2 * h[3] / sigm2[3] + 0.5 * h[5] / sigm2[5] + 0.5 * h[4] / sigm2[4];
    A[1][2] = sq3 * (z[2] * h[2] / sigm2[2] - z[1] * h[1] / sigm2[1] + z[5] * h[5] / sigm2[5] - z[4] * h[4] / sigm2[4]) * 0.5;
    A[1][3] = sq3 * (h[2] / sigm2[2] - h[1] / sigm2[1] + h[5] / sigm2[5] - h[4] / sigm2[4]) * 0.5;
    A[2][0] = A[0][2];
    A[2][1] = A[1][2];
    A[2][2] = 3.0 * (z2[2] * h[2] / sigm2[2] + z2[1] * h[1] / sigm2[1] + z2[5] * h[5] / sigm2[5] + z2[4] * h[4] / sigm2[4]) * 0.5;
    A[2][3] = 3.0 * (z[2] * h[2] / sigm2[2] + z[1] * h[1] / sigm2[1] + z[5] * h[5] / sigm2[5] + z[4] * h[4] / sigm2[4]) * 0.5;
    A[3][0] = A[0][3];
    A[3][1] = A[1][3];
    A[3][2] = A[2][3];
    A[3][3] = 3.0 * (0.5 * h[2] / sigm2[2] + 0.5 * h[1] / sigm2[1] + 0.5 * h[5] / sigm2[5] + 0.5 * h[4] / sigm2[4]);
}

void FillFreeCoefVector(Double_t F[4], Float_t XVU_Ch[nPlanes][big], Int_t segIdx, Float_t z[nPlanes], Float_t sigm2[nPlanes], Int_t h[nPlanes]) {
    // F - vector to be filled
    // XVU_Ch - coordinates of segment in chamber (Is it correct definition?)
    // segIdx - index of current segment
    // z - local z-positions of planes(layers)
    // sigm2 - square of sigma
    // h - array to include/exclude planes (h[i] = 0 or 1)

    F[0] = 2 * XVU_Ch[0][segIdx] * z[0] * h[0] / sigm2[0] + XVU_Ch[1][segIdx] * z[1] * h[1] / sigm2[1] + XVU_Ch[2][segIdx] * z[2] * h[2] / sigm2[2] + 2 * XVU_Ch[3][segIdx] * z[3] * h[3] / sigm2[3] + XVU_Ch[4][segIdx] * z[4] * h[4] / sigm2[4] + XVU_Ch[5][segIdx] * z[5] * h[5] / sigm2[5];
    F[1] = 2 * XVU_Ch[0][segIdx] * h[0] / sigm2[0] + XVU_Ch[1][segIdx] * h[1] / sigm2[1] + XVU_Ch[2][segIdx] * h[2] / sigm2[2] + 2 * XVU_Ch[3][segIdx] * h[3] / sigm2[3] + XVU_Ch[4][segIdx] * h[4] / sigm2[4] + XVU_Ch[5][segIdx] * h[5] / sigm2[5];
    F[2] = sq3 * (-XVU_Ch[1][segIdx] * z[1] * h[1] / sigm2[1] + XVU_Ch[2][segIdx] * z[2] * h[2] / sigm2[2] - XVU_Ch[4][segIdx] * z[4] * h[4] / sigm2[4] + XVU_Ch[5][segIdx] * z[5] * h[5] / sigm2[5]);
    F[3] = sq3 * (-XVU_Ch[1][segIdx] * h[1] / sigm2[1] + XVU_Ch[2][segIdx] * h[2] / sigm2[2] - XVU_Ch[4][segIdx] * h[4] / sigm2[4] + XVU_Ch[5][segIdx] * h[5] / sigm2[5]);
}

void InverseMatrix(Double_t A[4][4], Double_t b[4][4]) {
    /**** Gaussian algorithm for 4x4 matrix inversion ****/

    Double_t factor;
    Double_t temp[4];

    // Set b to I
    for (Int_t i1 = 0; i1 < 4; i1++)
        for (Int_t j1 = 0; j1 < 4; j1++)
            if (i1 == j1) b[i1][j1] = 1.0;
            else b[i1][j1] = 0.0;

    for (Int_t i1 = 0; i1 < 4; i1++) {
        for (Int_t j1 = i1 + 1; j1 < 4; j1++) {
            if (fabs(A[i1][i1]) < fabs(A[j1][i1])) {
                for (Int_t l1 = 0; l1 < 4; l1++) temp[l1] = A[i1][l1];
                for (Int_t l1 = 0; l1 < 4; l1++) A[i1][l1] = A[j1][l1];
                for (Int_t l1 = 0; l1 < 4; l1++) A[j1][l1] = temp[l1];
                for (Int_t l1 = 0; l1 < 4; l1++) temp[l1] = b[i1][l1];
                for (Int_t l1 = 0; l1 < 4; l1++) b[i1][l1] = b[j1][l1];
                for (Int_t l1 = 0; l1 < 4; l1++) b[j1][l1] = temp[l1];
            }
        }
        factor = A[i1][i1];
        for (Int_t j1 = 4 - 1; j1>-1; j1--) {
            b[i1][j1] /= factor;
            A[i1][j1] /= factor;
        }
        for (Int_t j1 = i1 + 1; j1 < 4; j1++) {
            factor = -A[j1][i1];
            for (Int_t k1 = 0; k1 < 4; k1++) {
                A[j1][k1] += A[i1][k1] * factor;
                b[j1][k1] += b[i1][k1] * factor;
            }
        }
    } // i1
    for (Int_t i1 = 3; i1 > 0; i1--) {
        for (Int_t j1 = i1 - 1; j1>-1; j1--) {
            factor = -A[j1][i1];
            for (Int_t k1 = 0; k1 < 4; k1++) {
                A[j1][k1] += A[i1][k1] * factor;
                b[j1][k1] += b[i1][k1] * factor;
            }
        }
    } // i1
    //end inverse
}

void ProcessSegments( Int_t chNum, Float_t sigma, Float_t z_loc[nPlanes], Int_t Min_hits, Int_t &Nseg, Int_t Nhits_Ch[big], Int_t Wires_Ch[nPlanes][big], Float_t XVU_Ch[nPlanes][big], Int_t &Nbest_Ch, Int_t ind_best_Ch[5],  Double_t Chi2_ndf_Ch[big], Double_t Chi2_ndf_best_Ch[5], Double_t par_ab_Ch[4][big]) {
  Float_t sigma2 = sigma * sigma;
  Float_t sigm2[nPlanes] = {sigma2, sigma2, sigma2, sigma2, sigma2, sigma2};

  Int_t h6[nPlanes] = {1, 1, 1, 1, 1, 1};
    
    
  for (Int_t Nhitm = nPlanes; Nhitm > Min_hits - 1; Nhitm--) {
    Int_t ifNhitm = 0;

    for (Int_t iseg = 0; iseg < Nseg; iseg++) {

      if (Nhits_Ch[iseg] != Nhitm) continue;
      ifNhitm = 1;
  
      Int_t h[nPlanes] = {0, 0, 0, 0, 0, 0};
      for (Int_t i = 0; i < nPlanes; i++)
	if (Wires_Ch[i][iseg] > -1) h[i] = 1;

      //linear fit
      Double_t A[4][4]; //coef matrix
      Double_t F[4]; //free coef 

      if (Nhits_Ch[iseg] == nPlanes) //case 6-point segment
	FillFitMatrix(A, z_loc, sigm2, h6);
      else
	FillFitMatrix(A, z_loc, sigm2, h);

      FillFreeCoefVector(F, XVU_Ch, iseg, z_loc, sigm2, h);

      Double_t A0[4][4];
      for (Int_t i1 = 0; i1 < 4; i1++)
	for (Int_t j1 = 0; j1 < 4; j1++)
	  A0[i1][j1] = A[i1][j1];

      Double_t b[4][4];
      InverseMatrix(A, b);

      Double_t sum;
      Double_t A1[4][4];

      for (Int_t i1 = 0; i1 < 4; ++i1)
	for (j1 = 0; j1 < 4; ++j1) {
	  sum = 0;
	  for (Int_t k1 = 0; k1 < 4; ++k1)
	    sum += A0[i1][k1] * b[k1][j1];
	  A1[i1][j1] = sum;
	}

      for (Int_t i1 = 0; i1 < 4; i1++) {
	par_ab_Ch[i1][iseg] = 0;
	for (j1 = 0; j1 < 4; j1++) {
	  par_ab_Ch[i1][iseg] += b[i1][j1] * F[j1];
	}
      } // i1

      Float_t dX[nPlanes];
      Chi2_ndf_Ch[iseg] = 0;
      for (Int_t i1 = 0; i1 < nPlanes; i1++) {
	if (Wires_Ch[i1][iseg]>-1) {
	  if (i1 == 0 || i1 == 3) dX[i1] = XVU_Ch[i1][iseg] - par_ab_Ch[0][iseg] * z_loc[i1] - par_ab_Ch[1][iseg];
	  if (i1 == 2 || i1 == 5) dX[i1] = XVU_Ch[i1][iseg] - 0.5 * (par_ab_Ch[0][iseg] + sq3 * par_ab_Ch[2][iseg]) * z_loc[i1] - 0.5 * (par_ab_Ch[1][iseg] + sq3 * par_ab_Ch[3][iseg]);
	  if (i1 == 1 || i1 == 4) dX[i1] = XVU_Ch[i1][iseg] - 0.5 * (par_ab_Ch[0][iseg] - sq3 * par_ab_Ch[2][iseg]) * z_loc[i1] - 0.5 * (par_ab_Ch[1][iseg] - sq3 * par_ab_Ch[3][iseg]);
	  Chi2_ndf_Ch[iseg] = Chi2_ndf_Ch[iseg] + dX[i1] * dX[i1] / sigm2[i1];
	}
      }

      if (Nhits_Ch[iseg] > 4)
	Chi2_ndf_Ch[iseg] = Chi2_ndf_Ch[iseg] / (Nhits_Ch[iseg] - 4);

      if (Chi2_ndf_Ch[iseg] > Chi2_Max) {
	if (Nhits_Ch[iseg] <= Min_hits) Nhits_Ch[iseg] = 0;
	else { //reject most distant point
	  Float_t Max_dev = 0;
	  Int_t irej = -1;
	  for (Int_t i1 = 0; i1 < nPlanes; i1++)
	    if (Wires_Ch[i1][iseg]>-1)
	      if (fabs(dX[i1]) > Max_dev) {
		irej = i1;
		Max_dev = fabs(dX[i1]);
	      }
	  Wires_Ch[irej][iseg] = -1;
	  Nhits_Ch[iseg]--;// reject most distant point
	  Chi2_ndf_Ch[iseg] = 0;//recalculate Chi2 with old(?) parameters
	  for (Int_t ii = 0; ii < nPlanes; ii++) {
	    if (Wires_Ch[ii][iseg]>-1) {
	      if (ii == 0 || ii == 3) dX[ii] = XVU_Ch[ii][iseg] - par_ab_Ch[0][iseg] * z_loc[ii] - par_ab_Ch[1][iseg];
	      if (ii == 2 || ii == 5) dX[ii] = XVU_Ch[ii][iseg] - 0.5 * (par_ab_Ch[0][iseg] + sq3 * par_ab_Ch[2][iseg]) * z_loc[ii] - 0.5 * (par_ab_Ch[1][iseg] + sq3 * par_ab_Ch[3][iseg]);
	      if (ii == 1 || ii == 4) dX[ii] = XVU_Ch[ii][iseg] - 0.5 * (par_ab_Ch[0][iseg] - sq3 * par_ab_Ch[2][iseg]) * z_loc[ii] - 0.5 * (par_ab_Ch[1][iseg] - sq3 * par_ab_Ch[3][iseg]);
	      Chi2_ndf_Ch[iseg] = Chi2_ndf_Ch[iseg] + dX[ii] * dX[ii] / sigm2[ii];
	    }
	  }
	}//else
      }
    } // iseg

    if (!ifNhitm) continue;

    if (Nbest_Ch == 0) {

      Double_t Chi2_best = 9999;
      Int_t iseg_best = -1;
      for (Int_t iseg = 0; iseg < Nseg; iseg++) {
	if (Nhits_Ch[iseg] != Nhitm) continue;
	if (Chi2_ndf_Ch[iseg] >= Chi2_best) continue;
	Chi2_best = Chi2_ndf_Ch[iseg];
	iseg_best = iseg;
      } // iseg

      if (iseg_best == -1) continue;
	    
      ind_best_Ch[0] = iseg_best;
      Chi2_ndf_best_Ch[0] = Chi2_best;
      Nbest_Ch = 1;

      //reject(common points)
      for (Int_t iseg = 0; iseg < Nseg; iseg++) {
	if (iseg == iseg_best)continue;
	Int_t u_cross = 0;
	Int_t v_cross = 0;
	Int_t x_cross = 0;
	for (Int_t i1 = 0; i1 < nPlanes; i1++) {
	  if (Wires_Ch[i1][iseg]>-1) {
	    if (Wires_Ch[i1][iseg] == Wires_Ch[i1][iseg_best]) Nhits_Ch[iseg] = 0;
	  }
	}
      }
    }
	
    if (Nbest_Ch == 1) {
      Double_t Chi2_best = 9999;
      Int_t iseg_best = -1;
      for (Int_t iseg = 0; iseg < Nseg; iseg++) {
	if (iseg == ind_best_Ch[0])continue;
	if (Nhits_Ch[iseg] != Nhitm) continue;
	if (Chi2_ndf_Ch[iseg] > Chi2_best) continue;
	Chi2_best = Chi2_ndf_Ch[iseg];
	iseg_best = iseg;
      } // iseg

      if (iseg_best>-1) {
	ind_best_Ch[Nbest_Ch] = iseg_best;
	Chi2_ndf_best_Ch[Nbest_Ch] = Chi2_best;
	Nbest_Ch++;
	
	if (chNum == 2 || chNum == 3) {
	  //reject(common points)
	  for (Int_t iseg = 0; iseg < Nseg; iseg++) {
	    if (iseg == iseg_best)continue;
	    Int_t u_cross = 0;
	    Int_t v_cross = 0;
	    Int_t x_cross = 0;
	    for (Int_t i1 = 0; i1 < nPlanes; i1++) {
	      if (Wires_Ch[i1][iseg]>-1) {
		if (Wires_Ch[i1][iseg] == Wires_Ch[i1][iseg_best]) Nhits_Ch[iseg] = 0;
	      }
	    }
	  }//iseg
	}
      }
    }
	
    if (chNum == 2 || chNum == 3) {
      if (Nbest_Ch == 2) {

	Double_t Chi2_best = 20;
	Int_t iseg_best = -1;
	for (Int_t iseg = 0; iseg < Nseg; iseg++) {
	  if (iseg == ind_best_Ch[0] || iseg == ind_best_Ch[1])continue;
	  if (Nhits_Ch[iseg] != Nhitm) continue;
	  if (Chi2_ndf_Ch[iseg] > Chi2_best) continue;
	  Chi2_best = Chi2_ndf_Ch[iseg];
	  iseg_best = iseg;
	} // iseg

	if (iseg_best>-1) {

	  ind_best_Ch[Nbest_Ch] = iseg_best;
	  Chi2_ndf_best_Ch[Nbest_Ch] = Chi2_best;
	  Nbest_Ch++;

	  //reject(common points)
	  for (Int_t iseg = 0; iseg < Nseg; iseg++) {
	    if (iseg == iseg_best)continue;
	    Int_t u_cross = 0;
	    Int_t v_cross = 0;
	    Int_t x_cross = 0;
	    for (Int_t i1 = 0; i1 < nPlanes; i1++) {
	      if (Wires_Ch[i1][iseg]>-1) {
		if (Wires_Ch[i1][iseg] == Wires_Ch[i1][iseg_best]) Nhits_Ch[iseg] = 0;
	      }
	    }
	  }
	}
      }
    }
  } // Nhitm
}
void FillEfficiency( Int_t Wires_Ch[nPlanes][big], Int_t Nhits_Ch[big], Int_t segIdx, TH1F *Denom_Ch, TH1F *Nomin_Ch, Int_t min_distX, Int_t min_distY ){
        // segIdx - index of current segment ch2 or ch3 // Int_t ind_best_Ch[5]
	//4p&4p -> all matched / Efficiency per layer
		   
	if (fabs(min_distX)<4 && fabs(min_distY)<3.5) { 
	  for(int i1 = 0 ; i1 < 6; i1++){
	    
	    if( Wires_Ch[i1][segIdx]>-1 && Nhits_Ch[segIdx]== 4)  continue;

	    Denom_Ch->Fill(i1);
			 
	    if( Wires_Ch[i1][segIdx]>-1){
	      Nomin_Ch->Fill(i1);		    
	    }
	  }
	}//min_distX  
}// FillEfficiency

void FillResid(Int_t Wires_Ch[nPlanes][big],  
	       Int_t segIdx, 
	       Float_t XVU_Ch[nPlanes][big],  
	       Double_t par_ab_Ch[4][big], 
	       Float_t z_loc[nPlanes],
	       TH1F *hResid_Ch[nPlanes]) {
  Float_t dX[nPlanes];
  for (Int_t i1 = 0; i1 < nPlanes; i1++) {
    if (Wires_Ch[i1][segIdx] > -1) {
      Float_t xvuBest = XVU_Ch[i1][segIdx];
      if (i1 == 0 || i1 == 3) dX[i1] = xvuBest - par_ab_Ch[0][segIdx] * z_loc[i1] - par_ab_Ch[1][segIdx];
      if (i1 == 2 || i1 == 5) dX[i1] = xvuBest - 0.5 * (par_ab_Ch[0][segIdx] + sq3 * par_ab_Ch[2][segIdx]) * z_loc[i1] - 0.5 * (par_ab_Ch[1][segIdx] + sq3 * par_ab_Ch[3][segIdx]);
      if (i1 == 1 || i1 == 4) dX[i1] = xvuBest - 0.5 * (par_ab_Ch[0][segIdx] - sq3 * par_ab_Ch[2][segIdx]) * z_loc[i1] - 0.5 * (par_ab_Ch[1][segIdx] - sq3 * par_ab_Ch[3][segIdx]);
      hResid_Ch[i1]->Fill(dX[i1]);
    }
  }
}//FillResid
   
Int_t read_mwpc1_688() {

    gROOT->LoadMacro("$VMCWORKDIR/macro/run/bmnloadlibs.C");
    bmnloadlibs(); // load libraries

// Use :: modificator before global name

    TFile in("/dataBMN/bmndata1/run3/root/bmn_run0688_hrb.root", "READ");

    TTree* T = (TTree*) in.Get("BMN_RAW");

    BmnMwpcDigit* bmndigit = new BmnMwpcDigit();
    TClonesArray* clar = new TClonesArray("BmnMwpcDigit", 100);
    TBranch* BMN = T->GetBranch("bmn_mwpc");
    BMN->SetAddress(&clar);

    TFile* fReco = new TFile("bmndst_run688.root", "RECREATE");
    TTree* tReco = new TTree("cbmsim", "bmndst_run688");

    //    TClonesArray* dchHits = new TClonesArray("BmnDchHit");
    TClonesArray* mwpcHits = new TClonesArray("BmnMwpcHit");
    //    TClonesArray* dchTracks = new TClonesArray("CbmTrack");
    TClonesArray* mwpcTracks1 = new TClonesArray("CbmTrack");
    TClonesArray* mwpcTracks2 = new TClonesArray("CbmTrack");
    TClonesArray* mwpcTracks3 = new TClonesArray("CbmTrack");
    TClonesArray* mwpcMatchedTracks = new TClonesArray("CbmTrack");

    //    tReco->Branch("BmnDchHit", &dchHits);
    tReco->Branch("BmnMwpcHit", &mwpcHits);
    //    tReco->Branch("DchTracks", &dchTracks);
    tReco->Branch("Mwpc1Segmants", &mwpcTracks1);
    tReco->Branch("Mwpc2Segmants", &mwpcTracks2);
    tReco->Branch("Mwpc3Segmants", &mwpcTracks3);
    tReco->Branch("MwpcMatchedTracks", &mwpcMatchedTracks);
    
        TH1F *Nomin_Ch2 =  new TH1F("Nomin23_Ch2", "Nomin23_Ch2",6,0,6);
	TH1F *Nomin_Ch3 =  new TH1F("Nomin23_Ch3", "Nomin23_Ch3",6,0,6);

	TH1F *Denom_Ch2 =  new TH1F("Denom23_Ch2", "Denom23_Ch2",6,0,6);
	TH1F *Denom_Ch3 =  new TH1F("Denom23_Ch3", "Denom23_Ch3",6,0,6);

	TH1F *Effic23_Ch2 =  new TH1F("Effic23_Ch2", "Layer Efficiency23 (MWPC2)",6,0,6);
	TH1F *Effic23_Ch3 =  new TH1F("Effic23_Ch3", "Layer Efficiency23 (MWPC3)",6,0,6);

	TH1F *Nomin_Ch2_9p =  new TH1F("Nomin_Ch2_9p", "Nomin_Ch2_9p",6,0,6);
	TH1F *Nomin_Ch3_9p =  new TH1F("Nomin_Ch3_9p", "Nomin_Ch3_9p",6,0,6);

	TH1F *Denom_Ch2_9p =  new TH1F("Denom_Ch2_9p", "Denom_Ch2_9p",6,0,6);
	TH1F *Denom_Ch3_9p =  new TH1F("Denom_Ch3_9p", "Denom_Ch3_9p",6,0,6);

	TH1F *Effic23_Ch2_9p =  new TH1F("Effic23_Ch2_9p", "Layer Efficiency ch2 ch3 min9p (MWPC2)",6,0,6);
	TH1F *Effic23_Ch3_9p =  new TH1F("Effic23_Ch3_9p", "Layer Efficiency ch2 ch3 min9p (MWPC3)",6,0,6);

	TH1F *Nomin23_overlap = new TH1F("Nomin23_overlap", "Nomin23_overlap" , 2,1,3);
	TH1F *Denom23_overlap = new TH1F("Denom23_overlap", "Denom23_overlap" , 2,1,3);
	TH1F *Effic23_overlap = new TH1F("Effic23_overlap", "Effic23_overlap" , 2,1,3);

	TH1F *hChi2_ndf_Ch23 =  new TH1F("hChi2_ndf_Ch23", "Chi2_ndf_Ch23",100,0,100);

	TH1F *hResid_Ch1[6];
	TH1F *hResid_Ch2[6];
	TH1F *hResid_Ch3[6];
	TH1F *hResid23_Ch2[6];
	TH1F *hResid23_Ch3[6];

	TString namHist;

	 for ( int i1=0; i1<6; i1++ ) {
	namHist = "hResid_Ch1_";
	namHist+=i1+1;
	hResid_Ch1[i1]  = new TH1F(namHist, namHist, 100,-0.50,0.50);
	namHist = "hResid_Ch2_";
	namHist+=i1+1;
	hResid_Ch2[i1]  = new TH1F(namHist, namHist, 100,-0.50,0.50);
	namHist = "hResid_Ch3_";
	namHist+=i1+1;
	hResid_Ch3[i1]  = new TH1F(namHist, namHist, 100,-0.50,0.50);
	namHist = "hResid23_Ch2_";
	namHist+=i1+1;
	hResid23_Ch2[i1]  = new TH1F(namHist, namHist, 100,-0.50,0.50);
	namHist = "hResid23_Ch3_";
	namHist+=i1+1;
	hResid23_Ch3[i1]  = new TH1F(namHist, namHist, 100,-0.50,0.50);
	 }
	 
    Int_t Nevents = T->GetEntries();
    
    // 
    // Nevents = 1000;

    Int_t nentries;

    Short_t wn, pn, ts, pl;
    Int_t wire_Ch1[nWires][nPlanes] = {0};
    Float_t xuv_Ch1[nWires][nPlanes] = {0};
    Int_t wire_Ch2[nWires][nPlanes] = {0};
    Float_t xuv_Ch2[nWires][nPlanes] = {0};
    Int_t wire_Ch3[nWires][nPlanes] = {0};
    Float_t xuv_Ch3[nWires][nPlanes] = {0};
    Float_t dw = 0.25; // cm
    Double_t sq3 = sqrt(3.);

    for (Int_t iev = 1; iev < Nevents; ++iev) {

        T->GetEntry(iev, 1);
        if (iev % 100 == 0) cout << "  --- Ev " << iev << endl;

        //        dchHits->Clear();
        mwpcHits->Clear();
        //        dchTracks->Clear();
        mwpcTracks1->Clear();
        mwpcTracks2->Clear();
        mwpcTracks3->Clear();
        mwpcMatchedTracks->Clear();

        //  Ch :        3  2  2  2   2   2   2   3   3   3   3   3   1  1  1  1  1  1
        Int_t pln[18] = {15, 8, 7, 6, 11, 10, 9, 14, 13, 12, 17, 16, 0, 1, 2, 3, 4, 5}; //  9.IX.15

        //                          x-    v-    u+    x+    v+   u-
        Float_t z1_loc[nPlanes] = {-2.5, -1.5, -0.5, 0.5, 1.5, 2.5}; //cm
        Float_t z2_loc[nPlanes] = {1.5, 0.5, -0.5, -1.5, -2.5, 2.5}; //cm
        Float_t z3_loc[nPlanes] = {1.5, 2.5, -2.5, -1.5, -0.5, 0.5}; //cm

        Int_t iw[18] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
        Int_t iw_Ch1[nPlanes] = {0, 0, 0, 0, 0, 0};
        Int_t iw_Ch2[nPlanes] = {0, 0, 0, 0, 0, 0};
        Int_t iw_Ch3[nPlanes] = {0, 0, 0, 0, 0, 0};

        nentries = clar->GetEntries();

        for (Int_t j = 0; j < nentries; ++j) {
            bmndigit = (BmnMwpcDigit*) clar->At(j);
            wn = bmndigit->GetWireNumber();
            pl = bmndigit->GetPlane();

            pn = pln[pl];
            Int_t pn0 = 6;
            if (pn < 6) pn0 = 0;
            if (pn > 11) pn0 = 12;
            ts = bmndigit->GetTime();
            bool repeat = 0;
            if (iw[pn] > 0) {
                for (Int_t ix = 0; ix < iw[pn]; ix++) {
                    if (wn == wire_Ch1[ix][pn - pn0] && pn0 == 0) {
                        repeat = 1;
                        break;
                    }
                    if (wn == wire_Ch2[ix][pn - pn0] && pn0 == 6) {
                        repeat = 1;
                        break;
                    }
                    if (wn == wire_Ch3[ix][pn - pn0] && pn0 == 12) {
                        repeat = 1;
                        break;
                    }
                }
            }
            if (repeat) continue;
            if (pn < 6) {
                //                isCh1Work = kTRUE;
                wire_Ch1[iw[pn]][pn] = wn;
                xuv_Ch1[iw[pn]][pn] = (wn - 46.75) * dw;
            } else if (pn > 11) {
                //                isCh2Work = kTRUE;
                wire_Ch3[iw[pn]][pn - pn0] = wn;
                xuv_Ch3[iw[pn]][pn - pn0] = (wn - 46.75) * dw;
            } else {
                //                isCh3Work = kTRUE;
                wire_Ch2[iw[pn]][pn - pn0] = wn;
                xuv_Ch2[iw[pn]][pn - pn0] = (wn - 46.75) * dw;
            }

            if (pn == 0 || pn == 1 || pn == 5) xuv_Ch1[iw[pn]][pn] = -xuv_Ch1[iw[pn]][pn];
            if (pn == 6 || pn == 7 || pn == 11) xuv_Ch2[iw[pn]][pn - pn0] = -xuv_Ch2[iw[pn]][pn - pn0];
            if (pn == 12 || pn == 13 || pn == 17) xuv_Ch3[iw[pn]][pn - pn0] = -xuv_Ch3[iw[pn]][pn - pn0];

            iw[pn]++;
            if (pn0 < 6) iw_Ch1[pn - pn0] = iw[pn];
            else if (pn0 > 11) iw_Ch3[pn - pn0] = iw[pn];
            else iw_Ch2[pn - pn0] = iw[pn];
        }

        Int_t Min_hits = 5;
        Double_t Chi2_Max = 20;

        //---------   Ch1   ---------//
        //        if (isCh1Work) {
	Float_t Z_to_target= 150;//cm

	//slope and shift Ch1 for parametrs
	Float_t x1_sh = 0;
	Float_t y1_sh = 8.5; //cm
	Float_t x1_slope_sh = -0.03; // + -> -
	Float_t y1_slope_sh = 0.08;

        Int_t Nseg_Ch1 = 0;
        Int_t Wires_Ch1[nPlanes][big] = {-1};
        Float_t XVU_Ch1[nPlanes][big];
        Int_t Nhits_Ch1[big] = {0};

        SegmentFinder(1, Wires_Ch1, XVU_Ch1, Nhits_Ch1, iw_Ch1, Nseg_Ch1, wire_Ch1, xuv_Ch1, Min_hits, 1);
        SegmentFinder(1, Wires_Ch1, XVU_Ch1, Nhits_Ch1, iw_Ch1, Nseg_Ch1, wire_Ch1, xuv_Ch1, Min_hits, 2);
	SegmentFinder(1, Wires_Ch1, XVU_Ch1, Nhits_Ch1, iw_Ch1, Nseg_Ch1, wire_Ch1, xuv_Ch1, Min_hits, 3);
        SegmentFinder(1, Wires_Ch1, XVU_Ch1, Nhits_Ch1, iw_Ch1, Nseg_Ch1, wire_Ch1, xuv_Ch1, Min_hits, 4);
        SegmentFinder(1, Wires_Ch1, XVU_Ch1, Nhits_Ch1, iw_Ch1, Nseg_Ch1, wire_Ch1, xuv_Ch1, Min_hits, 5);
	SegmentFinder(1, Wires_Ch1, XVU_Ch1, Nhits_Ch1, iw_Ch1, Nseg_Ch1, wire_Ch1, xuv_Ch1, Min_hits, 6);
	SegmentFinder(1, Wires_Ch1, XVU_Ch1, Nhits_Ch1, iw_Ch1, Nseg_Ch1, wire_Ch1, xuv_Ch1, Min_hits, 7);
	SegmentFinder(1, Wires_Ch1, XVU_Ch1, Nhits_Ch1, iw_Ch1, Nseg_Ch1, wire_Ch1, xuv_Ch1, Min_hits, 8);

	Int_t Nbest_Ch1 = 0;
        Int_t ind_best_Ch1[5] = {0, 0, 0, 0, 0};
        Double_t Chi2_ndf_best_Ch1[5] = {-999, -999, -999, -999, -999};
        Double_t Chi2_ndf_Ch1[big];
        Double_t par_ab_Ch1[4][big] = {999, 999, 999, 999};

        if (Nseg_Ch1 > 0)
            ProcessSegments(1, 0.0722, z1_loc, Min_hits, Nseg_Ch1, Nhits_Ch1, Wires_Ch1, XVU_Ch1, Nbest_Ch1, ind_best_Ch1, Chi2_ndf_Ch1, Chi2_ndf_best_Ch1, par_ab_Ch1);

	//create track and put it into TClonesArray
	if (Nbest_Ch1 > 0) {
	//  for (Int_t iBest = 0; iBest < Nbest_Ch1; iBest++) {
	Int_t iBest = 0;
	if (Nhits_Ch1[ind_best_Ch1[iBest]] < 5) continue;
	  Float_t dX[nPlanes];
	  //	  Int_t iBest = ind_best_Ch1[iBest];
	  
	  FillResid( Wires_Ch1,ind_best_Ch1[iBest], XVU_Ch1, par_ab_Ch1, z1_loc, hResid_Ch1);

	  //local parameters to Global parameters
	  //                                     ax              alpha                                      ax^2    
	  par_ab_Ch1[0][ind_best_Ch1[iBest]] += x1_slope_sh + x1_slope_sh* par_ab_Ch1[0][ind_best_Ch1[iBest]]* par_ab_Ch1[0][ind_best_Ch1[iBest]];
	  par_ab_Ch1[2][ind_best_Ch1[iBest]] += y1_slope_sh + y1_slope_sh* par_ab_Ch1[2][ind_best_Ch1[iBest]]* par_ab_Ch1[2][ind_best_Ch1[iBest]];
	  par_ab_Ch1[1][ind_best_Ch1[iBest]] += x1_sh;
	  par_ab_Ch1[3][ind_best_Ch1[iBest]] += y1_sh;

	  Float_t X1_v = par_ab_Ch1[0][ind_best_Ch1[iBest]] * Z_to_target + par_ab_Ch1[1][ind_best_Ch1[iBest]];
	  Float_t Y1_v = par_ab_Ch1[2][ind_best_Ch1[iBest]] * Z_to_target + par_ab_Ch1[3][ind_best_Ch1[iBest]];

            CbmTrack TracksCh1;
            FairTrackParam ParamsTrackCh1;
            Float_t z0 = Z_Ch1;
            Float_t x0 = par_ab_Ch1[1][ind_best_Ch1[iBest]]; //FIXME!!! Check formula x0 - x coordinate of point in which line crosses mid-plane
            Float_t y0 = par_ab_Ch1[3][ind_best_Ch1[iBest]]; //FIXME!!! Check formula y0 - y coordinate of point in which line crosses mid-plane
	   
            ParamsTrackCh1.SetPosition(TVector3(x0, y0, z0));
            ParamsTrackCh1.SetTx(par_ab_Ch1[0][ind_best_Ch1[iBest]]);
            ParamsTrackCh1.SetTy(par_ab_Ch1[2][ind_best_Ch1[iBest]]);
            TracksCh1.SetParamFirst(&ParamsTrackCh1);
            TracksCh1.SetChiSq(Chi2_ndf_Ch1[ind_best_Ch1[iBest]]);
            TracksCh1.SetNDF(Nhits_Ch1[ind_best_Ch1[iBest]]); //WARNING!!! now it is number of hits in matched track!
            new((*mwpcTracks1)[mwpcTracks1->GetEntriesFast()]) CbmTrack(TracksCh1);
	     
	    //}//iBest 
	}//Nbest_Ch1 > 0

        //        }
        //---------   Ch2   ---------//
        //        if (isCh2Work) {

	Float_t x2_sh = 1.0 - 73.0; //-0.6 //cm
	Float_t y2_sh = 0;
	Float_t x2_slope_sh =   0.030;// 0; + -> -//-0.0025//0.005; 0.015;//0.015;//0.02//.025// .032
	Float_t y2_slope_sh =  - 0.01;//0.006;//0.01// - 0.02;

        Int_t Nbest_Ch2 = 0;
        Int_t ind_best_Ch2[5] = {0, 0, 0, 0, 0};
        Double_t par_ab_Ch2[4][100] = {999, 999, 999, 999};

        Int_t Nseg_Ch2 = 0;
        Int_t Wires_Ch2[nPlanes][big] = {-1};
        Float_t XVU_Ch2[nPlanes][big];
        Int_t Nhits_Ch2[big] = {0};
        Min_hits = 4;

        SegmentFinder(2, Wires_Ch2, XVU_Ch2, Nhits_Ch2, iw_Ch2, Nseg_Ch2, wire_Ch2, xuv_Ch2, Min_hits, 1);
        SegmentFinder(2, Wires_Ch2, XVU_Ch2, Nhits_Ch2, iw_Ch2, Nseg_Ch2, wire_Ch2, xuv_Ch2, Min_hits, 2);
	SegmentFinder(2, Wires_Ch2, XVU_Ch2, Nhits_Ch2, iw_Ch2, Nseg_Ch2, wire_Ch2, xuv_Ch2, Min_hits, 3);
        SegmentFinder(2, Wires_Ch2, XVU_Ch2, Nhits_Ch2, iw_Ch2, Nseg_Ch2, wire_Ch2, xuv_Ch2, Min_hits, 4);
        SegmentFinder(2, Wires_Ch2, XVU_Ch2, Nhits_Ch2, iw_Ch2, Nseg_Ch2, wire_Ch2, xuv_Ch2, Min_hits, 5);
	SegmentFinder(2, Wires_Ch2, XVU_Ch2, Nhits_Ch2, iw_Ch2, Nseg_Ch2, wire_Ch2, xuv_Ch2, Min_hits, 6);
	SegmentFinder(2, Wires_Ch2, XVU_Ch2, Nhits_Ch2, iw_Ch2, Nseg_Ch2, wire_Ch2, xuv_Ch2, Min_hits, 7);
	SegmentFinder(2, Wires_Ch2, XVU_Ch2, Nhits_Ch2, iw_Ch2, Nseg_Ch2, wire_Ch2, xuv_Ch2, Min_hits, 8);

        Double_t Chi2_ndf_best_Ch2[5] = {-999, -999, -999, -999, -999};
        Double_t Chi2_ndf_Ch2[100];

        if (Nseg_Ch2 > 0)
            ProcessSegments(2, 0.0722, z2_loc, Min_hits, Nseg_Ch2, Nhits_Ch2, Wires_Ch2, XVU_Ch2, Nbest_Ch2, ind_best_Ch2, Chi2_ndf_Ch2, Chi2_ndf_best_Ch2, par_ab_Ch2);
    
        //create track and put it into TClonesArray
       if (Nbest_Ch2 > 0) {
	 for (Int_t iBest = 0; iBest < Nbest_Ch2; iBest++) {

	   if (Nhits_Ch2[ind_best_Ch2[iBest]] > 4) {

	     FillResid( Wires_Ch2,ind_best_Ch2[iBest], XVU_Ch2, par_ab_Ch2, z2_loc, hResid_Ch2);
	   } // > 4
		//local parameters to Global parameters
		par_ab_Ch2[0][ind_best_Ch2[iBest]] += x2_slope_sh + x2_slope_sh* par_ab_Ch2[0][ind_best_Ch2[iBest]]* par_ab_Ch2[0][ind_best_Ch2[iBest]];
		par_ab_Ch2[2][ind_best_Ch2[iBest]] += y2_slope_sh + y2_slope_sh* par_ab_Ch2[2][ind_best_Ch2[iBest]]* par_ab_Ch2[2][ind_best_Ch2[iBest]];
		par_ab_Ch2[1][ind_best_Ch2[iBest]] += x2_sh;
		par_ab_Ch2[3][ind_best_Ch2[iBest]] += y2_sh;

            CbmTrack TracksCh2;
            FairTrackParam ParamsTrackCh2;
            Float_t z0 = Z_Ch2;
            Float_t x0 = par_ab_Ch2[1][ind_best_Ch2[iBest]]; //Check formula x0 - x coordinate of point in which line crosses mid-plane
            Float_t y0 = par_ab_Ch2[3][ind_best_Ch2[iBest]]; //Check formula y0 - y coordinate of point in which line crosses mid-plane
            ParamsTrackCh2.SetPosition(TVector3(x0, y0, z0));
            ParamsTrackCh2.SetTx(par_ab_Ch2[0][ind_best_Ch2[iBest]]);
            ParamsTrackCh2.SetTy(par_ab_Ch2[2][ind_best_Ch2[iBest]]);
            TracksCh2.SetParamFirst(&ParamsTrackCh2);
            TracksCh2.SetChiSq(Chi2_ndf_Ch2[ind_best_Ch2[iBest]]);
            TracksCh2.SetNDF(Nhits_Ch2[ind_best_Ch2[iBest]]); //WARNING!!! now it is number of hits in matched track!
            new((*mwpcTracks2)[mwpcTracks2->GetEntriesFast()]) CbmTrack(TracksCh2);
	  
	 }//iBest 
       }//Nbest_Ch2 > 0

        //---------   Ch3   ---------//
        //        if (isCh3Work) {

        Float_t x3_sh = -2.0 - 73.0; //1.6 //cm
	Float_t y3_sh = 0.1; //cm
	Float_t x3_slope_sh =  0.038 ;//  0;// 0.005// 0.015// 0.028//.033 //.040
	Float_t y3_slope_sh = - 0.01;//0.006// 0.01//-0.02

        Int_t Nbest_Ch3 = 0;
        Int_t ind_best_Ch3[5] = {0, 0, 0, 0, 0};
	Double_t par_ab_Ch3[4][100] = {999, 999, 999, 999};

        Int_t Nseg_Ch3 = 0;
        Int_t Wires_Ch3[nPlanes][big] = {-1};
        Float_t XVU_Ch3[nPlanes][big];
        Int_t Nhits_Ch3[big] = {0};
        Min_hits = 4;

        SegmentFinder(3, Wires_Ch3, XVU_Ch3, Nhits_Ch3, iw_Ch3, Nseg_Ch3, wire_Ch3, xuv_Ch3, Min_hits, 1);
        SegmentFinder(3, Wires_Ch3, XVU_Ch3, Nhits_Ch3, iw_Ch3, Nseg_Ch3, wire_Ch3, xuv_Ch3, Min_hits, 2);
	SegmentFinder(3, Wires_Ch3, XVU_Ch3, Nhits_Ch3, iw_Ch3, Nseg_Ch3, wire_Ch3, xuv_Ch3, Min_hits, 3);
        SegmentFinder(3, Wires_Ch3, XVU_Ch3, Nhits_Ch3, iw_Ch3, Nseg_Ch3, wire_Ch3, xuv_Ch3, Min_hits, 4);
        SegmentFinder(3, Wires_Ch3, XVU_Ch3, Nhits_Ch3, iw_Ch3, Nseg_Ch3, wire_Ch3, xuv_Ch3, Min_hits, 5);
	SegmentFinder(3, Wires_Ch3, XVU_Ch3, Nhits_Ch3, iw_Ch3, Nseg_Ch3, wire_Ch3, xuv_Ch3, Min_hits, 6);
	SegmentFinder(3, Wires_Ch3, XVU_Ch3, Nhits_Ch3, iw_Ch3, Nseg_Ch3, wire_Ch3, xuv_Ch3, Min_hits, 7);
	SegmentFinder(3, Wires_Ch3, XVU_Ch3, Nhits_Ch3, iw_Ch3, Nseg_Ch3, wire_Ch3, xuv_Ch3, Min_hits, 8);


        Double_t Chi2_ndf_best_Ch3[5] = {-999, -999, -999, -999, -999};
        Double_t Chi2_ndf_Ch3[100];

        if (Nseg_Ch3 > 0)
            ProcessSegments(3, 0.0722, z3_loc, Min_hits, Nseg_Ch3, Nhits_Ch3, Wires_Ch3, XVU_Ch3, Nbest_Ch3, ind_best_Ch3, Chi2_ndf_Ch3, Chi2_ndf_best_Ch3, par_ab_Ch3);

      if (Nbest_Ch3 > 0) {
	for (Int_t iBest = 0; iBest < Nbest_Ch3; iBest++) {
	  if (Nhits_Ch3[ind_best_Ch3[iBest]] > 4) {

	   FillResid( Wires_Ch3,ind_best_Ch3[iBest], XVU_Ch3, par_ab_Ch3, z3_loc, hResid_Ch3);
	  } // > 4

	    //local parameters to Global parameters
	    par_ab_Ch3[0][ind_best_Ch3[iBest]] += x3_slope_sh + x3_slope_sh* par_ab_Ch3[0][ind_best_Ch3[iBest]]* par_ab_Ch3[0][ind_best_Ch3[iBest]];
	    par_ab_Ch3[2][ind_best_Ch3[iBest]] += y3_slope_sh + y3_slope_sh* par_ab_Ch3[2][ind_best_Ch3[iBest]]* par_ab_Ch3[2][ind_best_Ch3[iBest]];
	    par_ab_Ch3[1][ind_best_Ch3[iBest]] += x3_sh;
	    par_ab_Ch3[3][ind_best_Ch3[iBest]] += y3_sh;
	  
            CbmTrack TracksCh3;
            FairTrackParam ParamsTrackCh3;
            Float_t z0 = Z_Ch3;
            Float_t x0 = par_ab_Ch3[1][ind_best_Ch3[iBest]]; // Check formula x0 - x coordinate of point in which line crosses mid-plane
            Float_t y0 = par_ab_Ch3[3][ind_best_Ch3[iBest]]; // Check formula y0 - y coordinate of point in which line crosses mid-plane
            ParamsTrackCh3.SetPosition(TVector3(x0, y0, z0));
            ParamsTrackCh3.SetTx(par_ab_Ch3[0][ind_best_Ch3[iBest]]);
            ParamsTrackCh3.SetTy(par_ab_Ch3[2][ind_best_Ch3[iBest]]);
            TracksCh3.SetParamFirst(&ParamsTrackCh3);
            TracksCh3.SetChiSq(Chi2_ndf_Ch3[ind_best_Ch3[iBest]]);
            TracksCh3.SetNDF(Nhits_Ch3[ind_best_Ch3[iBest]]); //WARNING!!! now it is number of hits in matched track!
            new((*mwpcTracks3)[mwpcTracks3->GetEntriesFast()]) CbmTrack(TracksCh3);
	  
	}//iBest++
      }//Nbest_Ch3 > 0

        // ===    Ch2-Ch3 Matching

        Int_t best = -1;
        Int_t best3 = -1;

        Int_t best_Ch2_gl[2] = {-1, -1};
        Int_t best_Ch3_gl[2] = {-1, -1};
        Int_t Nbest_Ch23_gl = 0;
	Float_t Chi2_match[2] = {999, 999};
	Float_t sig_dx= 0.85;
	Float_t sig_dy= 0.76;
	Float_t sig_dax= 0.063;
	Float_t sig_day= 0.045;

        if (Nbest_Ch2 > 0 && Nbest_Ch3 > 0) {

	  // for (int pair1 = 0; pair1 < 2; pair1++) {
	  Int_t pair1 = 0;
	    
	    if ( pair1==1 && Nbest_Ch2 < 2 ) break;
	    if ( pair1==1 && Nbest_Ch3 < 2 ) break;
	    
	  Float_t min_Chi2m = 100; //400
	  Float_t min_distX = 99;
	  Float_t min_distY = 99;
	  Float_t dAx23;
	  Float_t dAy23;
	    
		 for (Int_t bst = 0; bst < Nbest_Ch2; bst++) {
		   	   if (pair1== 1 && best_Ch2_gl[0]== bst) continue;

		   Float_t x2mid = par_ab_Ch2[0][ind_best_Ch2[bst]] * Zmid + par_ab_Ch2[1][ind_best_Ch2[bst]] ;
		   Float_t y2mid = par_ab_Ch2[2][ind_best_Ch2[bst]] * Zmid + par_ab_Ch2[3][ind_best_Ch2[bst]] ;
               
		   for (Int_t bst3 = 0; bst3 < Nbest_Ch3; bst3++) {
		       if (pair1== 1 && best_Ch3_gl[0]== bst3) continue;

		     Float_t x3mid =  par_ab_Ch3[0][ind_best_Ch3[bst3]] * Zmid3 + par_ab_Ch3[1][ind_best_Ch3[bst3]] ;
		     Float_t y3mid =  par_ab_Ch3[2][ind_best_Ch3[bst3]] * Zmid3 + par_ab_Ch3[3][ind_best_Ch3[bst3]] ;
		  		  
		     dAx23 = par_ab_Ch2[0][ind_best_Ch2[bst]] - par_ab_Ch3[0][ind_best_Ch3[bst3]];
		     dAy23 = par_ab_Ch2[2][ind_best_Ch2[bst]] - par_ab_Ch3[2][ind_best_Ch3[bst3]];
		     min_distX = x2mid - x3mid; //min
		     min_distY = y2mid - y3mid; //min

		     Float_t Chi2_m = ( min_distX*min_distX/(sig_dx*sig_dx) + min_distY*min_distY/(sig_dy*sig_dy) + 
					dAx23*dAx23 /(sig_dax*sig_dax)+ dAy23*dAy23 /(sig_day*sig_day) );
		     Float_t DAx23;
		     Float_t DAy23;
		     Float_t Min_distX;
		     Float_t Min_distY;
		     
		     if (Chi2_m <  min_Chi2m) {
		       min_Chi2m = Chi2_m; //min
		       best = bst;
		       best3 = bst3;

		       Nbest_Ch23_gl= pair1+1;

		       best_Ch2_gl[pair1] = best;
		       best_Ch3_gl[pair1] = best3;
		       Chi2_match[pair1] =Chi2_m;

		      	DAx23=dAx23;
			DAy23=dAy23;
			Min_distX= min_distX;
			Min_distY= min_distY;
		       
		     }//Chi2_m
		   }//bst3++
		 }//bst++

		 if ( Nbest_Ch23_gl == pair1+1 ) {

		 FillEfficiency( Wires_Ch2, Nhits_Ch2,ind_best_Ch2[best], Denom_Ch2,Nomin_Ch2, min_distX, min_distY);//ch2 8point
		 FillEfficiency( Wires_Ch3, Nhits_Ch3,ind_best_Ch3[best3], Denom_Ch3,Nomin_Ch3, min_distX, min_distY);//ch3 8point

            //min 9p (4+5)
		  if (Nhits_Ch2[ind_best_Ch2[best]]+Nhits_Ch3[ind_best_Ch3[best3]]> 8){//proviso 9p

		    FillEfficiency( Wires_Ch2, Nhits_Ch2,ind_best_Ch2[best], Denom_Ch2_9p,Nomin_Ch2_9p, min_distX, min_distY);//ch2 9point
		    FillEfficiency( Wires_Ch3, Nhits_Ch3,ind_best_Ch3[best3], Denom_Ch3_9p,Nomin_Ch3_9p, min_distX, min_distY);//ch3 9point

		  }//>8
		 } // Nbest_Ch23_gl == pair1+1		
		 //	}//pair1++

	} //Nbest_Ch2>0 Nbest_Ch3>0

	  if (Nbest_Ch23_gl > 0) {

	    Float_t Z_mid23 = 469.5; //cm
            Float_t Z_to_target = -469.5; //cm
            Float_t X23_v[2];
            Float_t Y23_v[2];

            for (Int_t bst = 0; bst < Nbest_Ch23_gl; bst++) {

                Int_t bst2 = best_Ch2_gl[bst];
                Int_t bst3 = best_Ch3_gl[bst];

                Int_t best2 = ind_best_Ch2[bst2]; // it's index!
                Int_t best3 = ind_best_Ch3[bst3]; // it's index!
	
		Float_t	X2_v = par_ab_Ch2[0][best2] * (-Z_Ch2) + par_ab_Ch2[1][best2];
               	Float_t	Y2_v = par_ab_Ch2[2][best2] * (-Z_Ch2) + par_ab_Ch2[3][best2];

		Float_t	X3_v = par_ab_Ch3[0][best3] * (-Z_Ch3) + par_ab_Ch3[1][best3];
               	Float_t	Y3_v = par_ab_Ch3[2][best3] * (-Z_Ch3) + par_ab_Ch3[3][best3];

                X23_v[bst] = 0.5 * (X2_v + X3_v);
                Y23_v[bst] = 0.5 * (Y2_v + Y3_v);
		/*
		   //create track and put it into TClonesArray
                CbmTrack matchedTrack;
                FairTrackParam matchedParams;
                Float_t z0 = Z_mid23;
		Float_t x0 = par_ab_Ch23[1][bst]; //Check formula x0 - x coordinate of point in which line crosses mid-plane
		Float_t y0 = par_ab_Ch23[3][bst]; //Check formula y0 - y coordinate of point in which line crosses mid-plane
		matchedParams.SetPosition(TVector3(x0, y0, z0));
		matchedParams.SetTx(par_ab_Ch23[0][bst]);
		matchedParams.SetTy(par_ab_Ch23[2][bst]);
                matchedTrack.SetParamFirst(&matchedParams);
		matchedTrack.SetChiSq(Chi2_ndf_Ch23[bst]);
                matchedTrack.SetNDF(Nhits_Ch2[best2] + Nhits_Ch3[best3]); //WARNING!!! now it is number of hits in matched track!
                new((*mwpcMatchedTracks)[mwpcMatchedTracks->GetEntriesFast()]) CbmTrack(matchedTrack);
		*/
	    }//bst++

	  }//Nbest_Ch23_gl>0

        tReco->Fill();
	
    } //  iev


    Nomin_Ch2->Write();
    Nomin_Ch3->Write();

    Denom_Ch2->Write();
    Denom_Ch3->Write();

    Effic23_Ch2->Divide(Nomin_Ch2,Denom_Ch2,1,1);
    Effic23_Ch3->Divide(Nomin_Ch3,Denom_Ch3,1,1);

    Effic23_Ch2->Write();
    Effic23_Ch3->Write();


    Nomin_Ch2_9p->Write();
    Nomin_Ch3_9p->Write();

    Denom_Ch2_9p->Write();
    Denom_Ch3_9p->Write();

    Effic23_Ch2_9p->Divide(Nomin_Ch2_9p,Denom_Ch2_9p,1,1);
    Effic23_Ch3_9p->Divide(Nomin_Ch3_9p,Denom_Ch3_9p,1,1);

    Effic23_Ch2_9p->Write();
    Effic23_Ch3_9p->Write();

    for (int i1=0; i1<6; i1++ ){
	  hResid_Ch1[i1]->Write();
	  hResid_Ch2[i1]->Write();
	  hResid_Ch3[i1]->Write();
	  hResid23_Ch2[i1]->Write();
	  hResid23_Ch3[i1]->Write();
	}

    tReco->Write();
    fReco->Close();
    return 0;
}
