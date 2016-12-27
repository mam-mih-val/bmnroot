/*-------------------------------*/
// macro that adds DCH segments to the root file
// indicated at line number 310; 
// the files required at the moment:
//             bmn_run00%d_digi.root - containing the DCH digis 
//             transfer_func.txt - containing the coeficients of the transfer functions for each layer
//
// the  pol_coord0%d.txt that correspond to each file will be added soon after the raw data is available
// the script doesn't compile for now and runs only in the interpreter mode (i.e. root -b recoRunDCH.C)
// more optimization and feature adding to be done further 
/*-------------------------------*/



#include "TMath.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TCanvas.h"
#include "TChain.h"
#include "TClonesArray.h"
#include <vector>
#include "TVector3.h"
#include "TFile.h"
#include "TGeoManager.h"
//#include "TROOT.h"
//MK


//#include "FairTrackParam.h"
//#include "BmnMwpcDigit.h"
//#include "CbmTrack.h"


#include "TString.h"
//#include "TTree.h"
//#include "TProfile2D.h"

#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <ctime>


#include "TStyle.h"
#include "TClonesArray.h"
#include "TArrayS.h"
#include "TLine.h"

#include "TBenchmark.h"
#include <Riostream.h>
//#include "eig3.cpp"
//#include "eig3.h"

#include <TPostScript.h>
#include <TProfile.h>
//#include <Math/Vector3D.h>
//#include "eig3.h"
//#include "eig3.cpp"
//#include "mylib.cpp"
//#include "6x6inv.cpp"

//#include "BmnDchDigit.h"

#include <vector>
// #include <set>

using namespace std;
using namespace TMath;


//
//  runType - "run1", "run2", "run3"
//

//some global constants

const Float_t z_loc[8] = {9.3, 8.1, 3.5, 2.3, -2.3, -3.5, -8.1, -9.3}; // z local xa->vb (cm)
const Float_t z_glob[16] = {-45.7, -46.9, -51.5, -52.7, -57.3, -58.5, -63.1, -64.3, 64.3, 63.1, 58.5, 57.3, 52.7, 51.5, 46.9, 45.7}; // z global dc 1 & dc 2 (cm)
const Float_t Z_dch1 = 529.65;//z coord in the midle of dch1	
const Float_t Z_dch2 = 727.65;//z coord in the midle of dch2
const Float_t Z_dch_mid = 628.65; //z coord between the two chambers, this z is considered the global z for the matched dch segment


double sqrt_2 = sqrt(2.);
double isqrt_2 = 1 / sqrt_2;
//shift variables                                                                                                                                                            
float intBL = 2;//switch for different fields

/*   
// run2
const Float_t x1_sh = -5.2 ;
const Float_t x2_sh = -7.8 ;
const Float_t y1_sh = -0.4;
const Float_t y2_sh = -0.7;
     
//    run3
const Float_t x1_sh = -4.6 - 21;//-5.2
const Float_t x2_sh = -8.4 - 25;//-7.8
const Float_t y1_sh = -5.7;
const Float_t y2_sh = -5.8;
         
//    run4

const Float_t x1_sh = 15.4;//
const Float_t x2_sh = 11.8;//
const Float_t y1_sh = 0.2;
const Float_t y2_sh = 7.03;
 
*/ 
const Float_t x1_sh = 15.0; //
const Float_t x2_sh = 12.2; //
const Float_t y1_sh = 0.2;
const Float_t y2_sh = -0.3;

const Float_t u1_sh = isqrt_2 * (y1_sh - x1_sh);
const Float_t u2_sh = isqrt_2 * (y2_sh - x2_sh);
const Float_t v1_sh = isqrt_2 * (x1_sh + y1_sh);
const Float_t v2_sh = isqrt_2 * (x2_sh + y2_sh);

const Float_t x1_slope_sh = -0.08;
const Float_t y1_slope_sh = 0.051; //was .05
const Float_t x2_slope_sh = -0.08;
const Float_t y2_slope_sh = 0.055; //
     
const Float_t u1_slope_sh = isqrt_2 * (y1_slope_sh - x1_slope_sh);
const Float_t u2_slope_sh = isqrt_2 * (y2_slope_sh - x2_slope_sh);
const Float_t v1_slope_sh = isqrt_2 * (y1_slope_sh + x1_slope_sh);
const Float_t v2_slope_sh = isqrt_2 * (y2_slope_sh + x2_slope_sh);

void fit_seg(float *rh_seg, float *rh_sigm_seg, float *par_ab, int skip_first, int skip_second) {

  //linear fit
  float z_loc[8] = {9.3, 8.1, 3.5, 2.3, -2.3, -3.5, -8.1, -9.3}; // z local xa->vb (cm)
  float A[4][4] = {
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0}}; //coef matrix
  float f[4] = {0}; //free coef 
  //      float sigm_sq[8] = {1,1,1,1,1,1,1,1};
  int h[8] = {0, 0, 0, 0, 0, 0, 0, 0};

  
  for (int i = 0; i < 4; i++) {
    par_ab[i] = 999;
  }
  float xDC_glob, yDC_glob = -999;

  for (int i = 0; i < 8; i++) {
    h[i] = 1;
    //out1<<"setting h[i]"<<endl;
    if (i == skip_first || i == skip_second || rh_seg[i] == -999) {
      h[i] = 0;
    }
  }//i

  A[0][0] = 2 * z_loc[0] * z_loc[0] * h[0] / rh_sigm_seg[0] + z_loc[4] * z_loc[4] * h[4] / rh_sigm_seg[4] + z_loc[6] * z_loc[6] * h[6] / rh_sigm_seg[6] +
    2 * z_loc[1] * z_loc[1] * h[1] / rh_sigm_seg[1] + z_loc[5] * z_loc[5] * h[5] / rh_sigm_seg[5] + z_loc[7] * z_loc[7] * h[7] / rh_sigm_seg[7]; //aX_a

  A[0][1] = 2 * z_loc[0] * h[0] / rh_sigm_seg[0] + z_loc[4] * h[4] / rh_sigm_seg[4] + z_loc[6] * h[6] / rh_sigm_seg[6] +
    2 * z_loc[1] * h[1] / rh_sigm_seg[1] + z_loc[5] * h[5] / rh_sigm_seg[5] + z_loc[7] * h[7] / rh_sigm_seg[7]; //bX_a

  A[0][2] = z_loc[6] * z_loc[6] * h[6] / rh_sigm_seg[6] - z_loc[4] * z_loc[4] * h[4] / rh_sigm_seg[4] +
    z_loc[7] * z_loc[7] * h[7] / rh_sigm_seg[7] - z_loc[5] * z_loc[5] * h[5] / rh_sigm_seg[5]; //aY

  A[0][3] = z_loc[6] * h[6] / rh_sigm_seg[6] - z_loc[4] * h[4] / rh_sigm_seg[4] + z_loc[7] * h[7] / rh_sigm_seg[7] - z_loc[5] * h[5] / rh_sigm_seg[5]; //bY_a

  //dChi2/d_b_x

  A[1][0] = 2 * z_loc[0] * h[0] / rh_sigm_seg[0] + z_loc[4] * h[4] / rh_sigm_seg[4] + z_loc[6] * h[6] / rh_sigm_seg[6]
    + 2 * z_loc[1] * h[1] / rh_sigm_seg[1] + z_loc[5] * h[5] / rh_sigm_seg[5] + z_loc[7] * h[7] / rh_sigm_seg[7];

  A[1][1] = 2 * h[0] / rh_sigm_seg[0] + 1 * h[4] / rh_sigm_seg[4] + 1 * h[6] / rh_sigm_seg[6]
    + 2 * h[1] / rh_sigm_seg[1] + 1 * h[5] / rh_sigm_seg[5] + 1 * h[7] / rh_sigm_seg[7]; //bX_a

  A[1][2] = z_loc[6] * h[6] / rh_sigm_seg[6] - z_loc[4] * h[4] / rh_sigm_seg[4] + z_loc[7] * h[7] / rh_sigm_seg[7] - z_loc[5] * h[5] / rh_sigm_seg[5]; //aY_a

  A[1][3] = 1 * h[7] / rh_sigm_seg[7] - 1 * h[5] / rh_sigm_seg[5] + 1 * h[6] / rh_sigm_seg[6] - 1 * h[4] / rh_sigm_seg[4]; //bY_a

  //dChi2/da_y

  A[2][0] = z_loc[6] * z_loc[6] * h[6] / rh_sigm_seg[6] - z_loc[4] * z_loc[4] * h[4] / rh_sigm_seg[4] + z_loc[7] * z_loc[7] * h[7] / rh_sigm_seg[7] - z_loc[5] * z_loc[5] * h[5] / rh_sigm_seg[5]; //aX_a

  A[2][1] = z_loc[6] * h[6] / rh_sigm_seg[6] - z_loc[4] * h[4] / rh_sigm_seg[4] + z_loc[7] * h[7] / rh_sigm_seg[7] - z_loc[5] * h[5] / rh_sigm_seg[5]; //bX_a

  A[2][2] = 2 * z_loc[2] * z_loc[2] * h[2] / rh_sigm_seg[2] + z_loc[4] * z_loc[4] * h[4] / rh_sigm_seg[4] + z_loc[6] * z_loc[6] * h[6] / rh_sigm_seg[6]
    + 2 * z_loc[3] * z_loc[3] * h[3] / rh_sigm_seg[3] + z_loc[5] * z_loc[5] * h[5] / rh_sigm_seg[5] + z_loc[7] * z_loc[7] * h[7] / rh_sigm_seg[7]; //aY_a

  A[2][3] = 2 * z_loc[2] * h[2] / rh_sigm_seg[2] + z_loc[4] * h[4] / rh_sigm_seg[4] + z_loc[6] * h[6] / rh_sigm_seg[6]
    + 2 * z_loc[3] * h[3] / rh_sigm_seg[3] + z_loc[5] * h[5] / rh_sigm_seg[5] + z_loc[7] * h[7] / rh_sigm_seg[7];

  ////dChi2/db_y

  A[3][0] = z_loc[6] * h[6] / rh_sigm_seg[6] - z_loc[4] * h[4] / rh_sigm_seg[4] + z_loc[7] * h[7] / rh_sigm_seg[7] - z_loc[5] * h[5] / rh_sigm_seg[5]; //aX_a

  A[3][1] = 1 * h[6] / rh_sigm_seg[6] - 1 * h[4] / rh_sigm_seg[4] + 1 * h[7] / rh_sigm_seg[7] - 1 * h[5] / rh_sigm_seg[5]; //bX_a

  A[3][2] = 2 * z_loc[2] * h[2] / rh_sigm_seg[2] + z_loc[4] * h[4] / rh_sigm_seg[4] + z_loc[6] * h[6] / rh_sigm_seg[6]
    + 2 * z_loc[3] * h[3] / rh_sigm_seg[3] + z_loc[5] * h[5] / rh_sigm_seg[5] + z_loc[7] * h[7] / rh_sigm_seg[7]; //aY_a

  A[3][3] = 2 * h[2] / rh_sigm_seg[2] + 1 * h[4] / rh_sigm_seg[4] + 1 * h[6] / rh_sigm_seg[6]
    + 2 * h[3] / rh_sigm_seg[3] + 1 * h[5] / rh_sigm_seg[5] + 1 * h[7] / rh_sigm_seg[7]; //bY_a


  //free coef

  //dChi2/da_x

  f[0] = 2 * z_loc[0] * rh_seg[0] * h[0] / rh_sigm_seg[0] + sqrt_2 * z_loc[6] * rh_seg[6] * h[6] / rh_sigm_seg[6] - sqrt_2 * z_loc[4] * rh_seg[4] * h[4] / rh_sigm_seg[4] +
    2 * z_loc[1] * rh_seg[1] * h[1] / rh_sigm_seg[1] + sqrt_2 * z_loc[7] * rh_seg[7] * h[7] / rh_sigm_seg[7] - sqrt_2 * z_loc[5] * rh_seg[5] * h[5] / rh_sigm_seg[5]; //j = nr of seg
  //dChi2/db_x
  f[1] = 2 * rh_seg[0] * h[0] / rh_sigm_seg[0] + sqrt_2 * rh_seg[6] * h[6] / rh_sigm_seg[6] - sqrt_2 * rh_seg[4] * h[4] / rh_sigm_seg[4] +
    2 * rh_seg[1] * h[1] / rh_sigm_seg[1] + sqrt_2 * rh_seg[7] * h[7] / rh_sigm_seg[7] - sqrt_2 * rh_seg[5] * h[5] / rh_sigm_seg[5]; //j = nr of seg
  //dChi2/da_y
  f[2] = 2 * z_loc[2] * rh_seg[2] * h[2] / rh_sigm_seg[2] + sqrt_2 * z_loc[6] * rh_seg[6] * h[6] / rh_sigm_seg[6] + sqrt_2 * z_loc[4] * rh_seg[4] * h[4] / rh_sigm_seg[4] +
    2 * z_loc[3] * rh_seg[3] * h[3] / rh_sigm_seg[3] + sqrt_2 * z_loc[7] * rh_seg[7] * h[7] / rh_sigm_seg[7] + sqrt_2 * z_loc[5] * rh_seg[5] * h[5] / rh_sigm_seg[5]; //j = nr of seg
  ////dChi2/db_y
  f[3] = 2 * rh_seg[2] * h[2] / rh_sigm_seg[2] + sqrt_2 * rh_seg[6] * h[6] / rh_sigm_seg[6] + sqrt_2 * rh_seg[4] * h[4] / rh_sigm_seg[4] +
    2 * rh_seg[3] * h[3] / rh_sigm_seg[3] + sqrt_2 * rh_seg[7] * h[7] / rh_sigm_seg[7] + sqrt_2 * rh_seg[5] * h[5] / rh_sigm_seg[5]; //j = nr of seg
 
  //inverse the matrix

  /**** Gaussian algorithm for 4x4 matrix inversion ****/
  Int_t i1, j1, k1, l1;
  Double_t factor;
  Double_t temp[4];
  Double_t b[4][4];
  Double_t A0[4][4];

  for (i1 = 0; i1 < 4; i1++) for (j1 = 0; j1 < 4; j1++) A0[i1][j1] = A[i1][j1];

  // Set b to I
  for (i1 = 0; i1 < 4; i1++) for (j1 = 0; j1 < 4; j1++)
			       if (i1 == j1) b[i1][j1] = 1.0;
			       else b[i1][j1] = 0.0;

  for (i1 = 0; i1 < 4; i1++) {
    for (j1 = i1 + 1; j1 < 4; j1++)
      if (fabs(A[i1][i1]) < fabs(A[j1][i1])) {
	for (l1 = 0; l1 < 4; l1++) temp[l1] = A[i1][l1];
	for (l1 = 0; l1 < 4; l1++) A[i1][l1] = A[j1][l1];
	for (l1 = 0; l1 < 4; l1++) A[j1][l1] = temp[l1];
	for (l1 = 0; l1 < 4; l1++) temp[l1] = b[i1][l1];
	for (l1 = 0; l1 < 4; l1++) b[i1][l1] = b[j1][l1];
	for (l1 = 0; l1 < 4; l1++) b[j1][l1] = temp[l1];
      }
    factor = A[i1][i1];
    for (j1 = 4 - 1; j1>-1; j1--) {
      b[i1][j1] /= factor;
      A[i1][j1] /= factor;
    }
    for (j1 = i1 + 1; j1 < 4; j1++) {
      factor = -A[j1][i1];
      for (k1 = 0; k1 < 4; k1++) {
	A[j1][k1] += A[i1][k1] * factor;
	b[j1][k1] += b[i1][k1] * factor;
      }
    }
  }
  for (i1 = 3; i1 > 0; i1--) {
    for (j1 = i1 - 1; j1>-1; j1--) {
      factor = -A[j1][i1];
      for (k1 = 0; k1 < 4; k1++) {
	A[j1][k1] += A[i1][k1] * factor;
	b[j1][k1] += b[i1][k1] * factor;
      }
    }
  }

  float sum;

  float A1[4][4] = {
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0}};

  for (i1 = 0; i1 < 4; ++i1) for (j1 = 0; j1 < 4; ++j1) {
      sum = 0;
   
      for (k1 = 0; k1 < 4; ++k1)
	sum += A0[i1][k1] * b[k1][j1];
      A1[i1][j1] = sum;
    }
   
  for (i1 = 0; i1 < 4; i1++) {
    par_ab[i1] = 0;
    for (j1 = 0; j1 < 4; j1++) {
      par_ab[i1] += b[i1][j1] * f[j1];
    }
  }
}//fit_seg

void recoRunDCH_VP(Int_t runId = 810) {

  gROOT->LoadMacro("$VMCWORKDIR/macro/run/bmnloadlibs.C");
  bmnloadlibs(); // load bmn libraries

  TChain *bmnTree = new TChain("cbmsim");
  bmnTree->Add(TString::Format("bmn_run0%d_digi.root", runId));

  TClonesArray *dchDigits;
  bmnTree->SetBranchAddress("DCH", &dchDigits);
  Int_t startEvent = 1;
  Int_t nEvents = bmnTree->GetEntries();
  nEvents = 1000;

  TString outName =  Form("bmn_reco_DCH_run%04d.root",runId);
  TFile* fReco = new TFile(outName, "RECREATE");//the root file where the dch hits and tracks will be filled
  TTree* tReco = new TTree("cbmsim", "test_bmn");

  TClonesArray* dchHits = new TClonesArray("BmnDchHit");
  TClonesArray* dchTracks = new TClonesArray("BmnTrack");
  TClonesArray* dchMatchedTracks = new TClonesArray("BmnTrack");

  tReco->Branch("BmnDchHit", &dchHits);
  tReco->Branch("DchTracks", &dchTracks);
  tReco->Branch("DchMatchedTracks", &dchMatchedTracks);

  //read and memorize the transfer fuctions' parameters

  float p1_xa, p0_xa, p1_xb, p0_xb, p1_ya, p0_ya, p1_yb, p0_yb, p1_ua, p0_ua, p1_ub, p0_ub, p1_va, p0_va, p1_vb, p0_vb;

  double t_dc[5][16]; //[time interval][plane number]
  double pol_par_dc[3][5][16]; //[polinom number][param number][plane number]
  double scaling[16]; //[plane number]

  ifstream fin;
  //  fin.open(TString::Format("pol_coord00%d.txt", runId),ios::in);
  fin.open("transfer_func.txt",ios::in);
  for (int fi = 0; fi < 16; fi++) {
    fin >> t_dc[0][fi] >> t_dc[1][fi] >> t_dc[2][fi] >> t_dc[3][fi] >> t_dc[4][fi] >> pol_par_dc[0][0][fi] >> pol_par_dc[0][1][fi] >> pol_par_dc[0][2][fi] >> pol_par_dc[0][3][fi] >> pol_par_dc[0][4][fi] >> pol_par_dc[1][0][fi] >> pol_par_dc[1][1][fi] >> pol_par_dc[1][2][fi] >> pol_par_dc[1][3][fi] >> pol_par_dc[1][4][fi] >> pol_par_dc[2][0][fi] >> pol_par_dc[2][1][fi] >> pol_par_dc[2][2][fi] >> pol_par_dc[2][3][fi] >> pol_par_dc[2][4][fi] >> scaling[fi];
  }

  fin.close();

  float scale[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  for (Int_t sc = 0; sc < 16; sc++) {
    scale[sc] = 0.5;//  / scaling[sc];
  }//scaling for transfer function coeffs

  int prev_wire = -1;
  int prev_time = -1;
  int skipped_ev = 0;
  int poss = 0;
  int imposs = 0;
  for (Int_t iEv = startEvent; iEv < startEvent + nEvents; iEv++) {

    bmnTree->GetEntry(iEv);

    dchHits->Clear();
    dchTracks->Clear();
    dchMatchedTracks->Clear();
    
    if (iEv % 1000 == 0) cout << "Event/last : " << iEv + 1 << "/" << startEvent + nEvents << endl;

    BmnDchDigit* digit = NULL;

    //temporary containers
    double time_xa1[20] = {-99};
    double time_xa2[20] = {-99};
    double time_xb1[20] = {-99};
    double time_xb2[20] = {-99};
    double time_ya1[20] = {-99};
    double time_ya2[20] = {-99};
    double time_yb1[20] = {-99};
    double time_yb2[20] = {-99};
    double time_ua1[20] = {-99};
    double time_ua2[20] = {-99};
    double time_ub1[20] = {-99};
    double time_ub2[20] = {-99};
    double time_va1[20] = {-99};
    double time_va2[20] = {-99};
    double time_vb1[20] = {-99};
    double time_vb2[20] = {-99};

    float wirenr_xa1[20] = {-99};
    float wirenr_xa2[20] = {-99};
    float wirenr_xb1[20] = {-99};
    float wirenr_xb2[20] = {-99};
    float wirenr_ya1[20] = {-99};
    float wirenr_ya2[20] = {-99};
    float wirenr_yb1[20] = {-99};
    float wirenr_yb2[20] = {-99};
    float wirenr_ua1[20] = {-99};
    float wirenr_ua2[20] = {-99};
    float wirenr_ub1[20] = {-99};
    float wirenr_ub2[20] = {-99};
    float wirenr_va1[20] = {-99};
    float wirenr_va2[20] = {-99};
    float wirenr_vb1[20] = {-99};
    float wirenr_vb2[20] = {-99};

    bool used_xa1[20] = {0};
    bool used_xa2[20] = {0};
    bool used_xb1[20] = {0};
    bool used_xb2[20] = {0};
    bool used_ya1[20] = {0};
    bool used_ya2[20] = {0};
    bool used_yb1[20] = {0};
    bool used_yb2[20] = {0};
    bool used_ua1[20] = {0};
    bool used_ua2[20] = {0};
    bool used_ub1[20] = {0};
    bool used_ub2[20] = {0};
    bool used_va1[20] = {0};
    bool used_va2[20] = {0};
    bool used_vb1[20] = {0};
    bool used_vb2[20] = {0};

    int it_xa1 = 0;
    int it_xa2 = 0;
    int it_xb1 = 0;
    int it_xb2 = 0;
    int it_ya1 = 0;
    int it_ya2 = 0;
    int it_yb1 = 0;
    int it_yb2 = 0;
    int it_ua1 = 0;
    int it_ua2 = 0;
    int it_ub1 = 0;
    int it_ub2 = 0;
    int it_va1 = 0;
    int it_va2 = 0;
    int it_vb1 = 0;
    int it_vb2 = 0;

    bool goodEv = true;
    bool written = false;

    for (Int_t iDig = 0; iDig < dchDigits->GetEntriesFast(); ++iDig) {

      digit = (BmnDchDigit*) dchDigits->At(iDig);
      Short_t plane = digit->GetPlane();

      //skip identical events
      if (!written) {
	written = true;
	if (digit->GetTime() == prev_time && digit->GetWireNumber() == prev_wire) {
	  goodEv = false;
	  skipped_ev++;
	} else {
	  prev_time = int(digit->GetTime());
	  prev_wire = int(digit->GetWireNumber());
	}
      }//!written

      double times = digit->GetTime();
      bool secondaries = false;
      switch (plane) {
      case 0:                                                                                  
	for (int sec = 0; sec < it_va1 - 1; sec++) {
	  if (digit->GetWireNumber() == wirenr_va1[sec]) {
	    secondaries = true;
	    break;
	  }
	}//skip secondary hits                                                                                                                      
	if (it_va1 == 19 || secondaries)break;
	wirenr_va1[it_va1] = digit->GetWireNumber();
	time_va1[it_va1] = times;
	it_va1++;
	break;
      case 1:

	for (int sec = 0; sec < it_vb1 - 1; sec++) {
	  if (digit->GetWireNumber() == wirenr_vb1[sec]) {
	    secondaries = true;
	    break;
	  }
	}//skip secondary hits                                                                                                                      
	if (it_vb1 == 19 || secondaries)break;
	wirenr_vb1[it_vb1] = digit->GetWireNumber();
	time_vb1[it_vb1] = times;
	it_vb1++;
	break;
      case 2:
	for (int sec = 0; sec < it_ua1 - 1; sec++) {
	  if (digit->GetWireNumber() == wirenr_ua1[sec]) {
	    secondaries = true;
	    break;
	  }
	}//skip secondary hits                                                                                                                      

	if (it_ua1 == 19 || secondaries)break;
	wirenr_ua1[it_ua1] = digit->GetWireNumber();
	time_ua1[it_ua1] = times;
	it_ua1++;
	break;
      case 3:
	for (int sec = 0; sec < it_ub1 - 1; sec++) {
	  if (digit->GetWireNumber() == wirenr_ub1[sec]) {
	    secondaries = true;
	    break;
	  }
	}//skip secondary hits                                                                                                                      

	if (it_ub1 == 19 || secondaries)break;
	wirenr_ub1[it_ub1] = digit->GetWireNumber();
	time_ub1[it_ub1] = times;
	it_ub1++;
	break;
      case 4:

	for (int sec = 0; sec < it_ya1 - 1; sec++) {
	  if (digit->GetWireNumber() == wirenr_ya1[sec]) {
	    secondaries = true;
	    break;
	  }
	}//skip secondary hits                                                                                                                      
	if (it_ya1 == 19 || secondaries)break;
                                                                                        
	wirenr_ya1[it_ya1] = digit->GetWireNumber();
	time_ya1[it_ya1] = times;
	it_ya1++;
	break;
      case 5:
	for (int sec = 0; sec < it_yb1 - 1; sec++) {
	  if (digit->GetWireNumber() == wirenr_yb1[sec]) {
	    secondaries = true;
	    break;
	  }
	}//skip secondary hits                                                                                                                      
	if (it_yb1 == 19 || secondaries)break;
	wirenr_yb1[it_yb1] = digit->GetWireNumber();
	time_yb1[it_yb1] = times;
	it_yb1++;
	break;
      case 6:
	for (int sec = 0; sec < it_xa1 - 1; sec++) {
	  if (digit->GetWireNumber() == wirenr_xa1[sec]) {
	    secondaries = true;
	    break;
	  }
	}//skip secondarx hits                                                                                                                      

	if (it_xa1 == 19 || secondaries)break;
	wirenr_xa1[it_xa1] = digit->GetWireNumber();
	time_xa1[it_xa1] = times;
	it_xa1++;
	break;
      case 7:
	for (int sec = 0; sec < it_xb1 - 1; sec++) {
	  if (digit->GetWireNumber() == wirenr_xb1[sec]) {
	    secondaries = true;
	    break;
	  }
	}//skip secondary hits                                                                                                                      

	if (it_xb1 == 19 || secondaries)break;
	wirenr_xb1[it_xb1] = digit->GetWireNumber();
	time_xb1[it_xb1] = times;
	it_xb1++;
	break;
      case 8:
	for (int sec = 0; sec < it_va2 - 1; sec++) {
	  if (digit->GetWireNumber() == wirenr_va2[sec]) {
	    secondaries = true;
	    break;
	  }
	}//skip secondary hits                                                                                                                      

	if (it_va2 == 19 || secondaries)break;
	wirenr_va2[it_va2] = digit->GetWireNumber();
	time_va2[it_va2] = times;
	it_va2++;
	break;
      case 9:
	for (int sec = 0; sec < it_vb2 - 1; sec++) {
	  if (digit->GetWireNumber() == wirenr_vb2[sec]) {
	    secondaries = true;
	    break;
	  }
	}//skip secondary hits                                                                                                                       

	if (it_vb2 == 19 || secondaries)break;
	wirenr_vb2[it_vb2] = digit->GetWireNumber();
	time_vb2[it_vb2] = times;
	it_vb2++;
	break;
      case 10:
	for (int sec = 0; sec < it_ua2 - 1; sec++) {
	  if (digit->GetWireNumber() == wirenr_ua2[sec]) {
	    secondaries = true;
	    break;
	  }
	}//skip secondary hits                                                                                                                       
	if (it_ua2 == 19 || secondaries)break;
	wirenr_ua2[it_ua2] = digit->GetWireNumber();
	time_ua2[it_ua2] = times;
	it_ua2++;
	break;
      case 11:
	for (int sec = 0; sec < it_ub2 - 1; sec++) {
	  if (digit->GetWireNumber() == wirenr_ub2[sec]) {
	    secondaries = true;
	    break;
	  }
	}//skip secondary hits                                                                                                                      

	if (it_ub2 == 19 || secondaries)break;
	wirenr_ub2[it_ub2] = digit->GetWireNumber();
	time_ub2[it_ub2] = times;
	it_ub2++;
	break;
      case 12:
	for (int sec = 0; sec < it_ya2 - 1; sec++) {
	  if (digit->GetWireNumber() == wirenr_ya2[sec]) {
	    secondaries = true;
	    break;
	  }
	}//skip secondary hits                                                                                                                       
	if (digit->GetWireNumber() == 111)break;
	if (it_ya2 == 19 || secondaries)break;
	wirenr_ya2[it_ya2] = digit->GetWireNumber();
	time_ya2[it_ya2] = times;
	it_ya2++;
	break;
      case 13:
	for (int sec = 0; sec < it_yb2 - 1; sec++) {
	  if (digit->GetWireNumber() == wirenr_yb2[sec]) {
	    secondaries = true;
	    break;
	  }
	}//skip secondary hits                                                                                                                       

	if (it_yb2 == 19 || secondaries)break;
	wirenr_yb2[it_yb2] = digit->GetWireNumber();
	time_yb2[it_yb2] = times;
	it_yb2++;
	break;
      case 14:

	for (int sec = 0; sec < it_xa2 - 1; sec++) {
	  if (digit->GetWireNumber() == wirenr_xa2[sec]) {
	    secondaries = true;
	    break;
	  }
	}//skip secondarx hits                                                                                                                      
	if (it_xa2 == 19 || secondaries)break;
	wirenr_xa2[it_xa2] = digit->GetWireNumber();
	time_xa2[it_xa2] = times;
	it_xa2++;
	break;
      case 15:
	for (int sec = 0; sec < it_xb2 - 1; sec++) {
	  if (digit->GetWireNumber() == wirenr_xb2[sec]) {
	    secondaries = true;
	    break;
	  }
	}//skip secondarx hits                                                                                                                      
	if (it_xb2 == 19 || secondaries)break;
	wirenr_xb2[it_xb2] = digit->GetWireNumber();
	time_xb2[it_xb2] = times;
	it_xb2++;
	break;
      }
    }//for digis in event iDig

    if (!goodEv)continue;
    float rh_segDC1[8][150];
    float rh_segDC2[8][150];
    float rh_sigm_segDC1[8][150];
    float rh_sigm_segDC2[8][150];
    float xDC1_glob[150];
    float yDC1_glob[150];
    float xDC2_glob[150];
    float yDC2_glob[150];
    float chi2_DC1[150];
    float chi2_DC2[150];
    int size_segDC1[150];
    int size_segDC2[150];
    int nDC1_segments = 0;
    int nDC2_segments = 0;
    bool has7DC1 = false;
    bool has7DC2 = false;
    
    float x1_ab[2][150];
    float y1_ab[2][150];
    float u1_ab[2][150];
    float v1_ab[2][150];
    float sigm_x1_ab[2][150];
    float sigm_y1_ab[2][150];
    float sigm_u1_ab[2][150];
    float sigm_v1_ab[2][150];
    float x2_ab[2][150];
    float y2_ab[2][150];
    float u2_ab[2][150];
    float v2_ab[2][150];
    float sigm_x2_ab[2][150];
    float sigm_y2_ab[2][150];
    float sigm_u2_ab[2][150];
    float sigm_v2_ab[2][150];

    //single hits on ab-plane
    float x1_single[2][40];
    float y1_single[2][40];
    float u1_single[2][40];
    float v1_single[2][40];
    float sigm_x1_single[2][40];
    float sigm_y1_single[2][40];
    float sigm_u1_single[2][40];
    float sigm_v1_single[2][40];
    float x2_single[2][40];
    float y2_single[2][40];
    float u2_single[2][40];
    float v2_single[2][40];
    float sigm_x2_single[2][40];
    float sigm_y2_single[2][40];
    float sigm_u2_single[2][40];
    float sigm_v2_single[2][40];

    for (int i = 0; i < 8; i++) {
      for (int j = 0; j < 150; j++) {
	rh_segDC1[i][j] = -999;
	rh_segDC2[i][j] = -999;
	rh_sigm_segDC1[i][j] = 1;
	rh_sigm_segDC2[i][j] = 1;
	if (i > 1)continue;
	x1_ab[i][j] = -999;
	y1_ab[i][j] = -999;
	u1_ab[i][j] = -999;
	v1_ab[i][j] = -999;
	x2_ab[i][j] = -999;
	y2_ab[i][j] = -999;
	u2_ab[i][j] = -999;
	v2_ab[i][j] = -999;
	sigm_x1_ab[i][j] = 1;
	sigm_y1_ab[i][j] = 1;
	sigm_u1_ab[i][j] = 1;
	sigm_v1_ab[i][j] = 1;
	sigm_x2_ab[i][j] = 1;
	sigm_y2_ab[i][j] = 1;
	sigm_u2_ab[i][j] = 1;
	sigm_v2_ab[i][j] = 1;

	if (j > 39)continue;
	x1_single[i][j] = -999;
	y1_single[i][j] = -999;
	u1_single[i][j] = -999;
	v1_single[i][j] = -999;
	x2_single[i][j] = -999;
	y2_single[i][j] = -999;
	u2_single[i][j] = -999;
	v2_single[i][j] = -999;
	sigm_x1_single[i][j] = 1;
	sigm_y1_single[i][j] = 1;
	sigm_u1_single[i][j] = 1;
	sigm_v1_single[i][j] = 1;
	sigm_x2_single[i][j] = 1;
	sigm_y2_single[i][j] = 1;
	sigm_u2_single[i][j] = 1;
	sigm_v2_single[i][j] = 1;


	if (i > 0)continue;

	size_segDC1[j] = 0;
	size_segDC2[j] = 0;
	chi2_DC1[j] = 50;
	chi2_DC2[j] = 50;
	if (j > 39)continue;

	xDC1_glob[j] = -999;
	yDC1_glob[j] = -999;
	xDC2_glob[j] = -999;
	yDC2_glob[j] = -999;
      }
    }
    int pair_x1 = 0;
    int pair_y1 = 0;
    int pair_u1 = 0;
    int pair_v1 = 0;
    int single_xa1 = 0;
    int single_ya1 = 0;
    int single_ua1 = 0;
    int single_va1 = 0;
    int single_xb1 = 0;
    int single_yb1 = 0;
    int single_ub1 = 0;
    int single_vb1 = 0;

    float xa1_pm[2] = {-999};
    float xb1_pm[2] = {-999};
    float ya1_pm[2] = {-999};
    float yb1_pm[2] = {-999};
    float ua1_pm[2] = {-999};
    float ub1_pm[2] = {-999};
    float va1_pm[2] = {-999};
    float vb1_pm[2] = {-999};

    //   ---   X   ---

    for (Int_t i = 0; i < it_xa1; ++i) {
      for (Int_t j = 0; j < it_xb1; ++j) {
	if (pair_x1 > 48)break;
	if ((wirenr_xa1[i] != wirenr_xb1[j] && wirenr_xa1[i] != wirenr_xb1[j] + 1)) continue;
	int func_nr_a = -1;
	int func_nr_b = -1;
	for (int t_it = 0; t_it < 4; t_it++) {
	  if (time_xa1[i] >= t_dc[t_it][0] && time_xa1[i] < t_dc[t_it + 1][0]) {
	    func_nr_a = t_it;
	    break;
	  }
	}
	double time_xa = time_xa1[i];
	double d_a = 0;
	double d_b = 0;
	if (func_nr_a == 1 || func_nr_a == 2)d_a = scale[0]*(pol_par_dc[1][0][0] + pol_par_dc[1][1][0] * time_xa + pol_par_dc[1][2][0] * time_xa * time_xa + pol_par_dc[1][3][0] * time_xa * time_xa * time_xa + pol_par_dc[1][4][0] * time_xa * time_xa * time_xa * time_xa);
	if (func_nr_a == 0)d_a = 0;
	if (func_nr_a == 3)d_a = scale[0]*(pol_par_dc[2][0][0] + pol_par_dc[2][1][0] * time_xa + pol_par_dc[2][2][0] * time_xa * time_xa + pol_par_dc[2][3][0] * time_xa * time_xa * time_xa + pol_par_dc[2][4][0] * time_xa * time_xa * time_xa * time_xa);

	for (int t_it = 0; t_it < 4; t_it++) {
	  if (time_xb1[j] >= t_dc[t_it][1] && time_xb1[j] < t_dc[t_it + 1][1]) {
	    func_nr_b = t_it;
	    break;
	  }
	}
	time_xa = time_xb1[j];

	if (func_nr_b == 1 || func_nr_b == 2)d_b = scale[1]*(pol_par_dc[1][0][1] + pol_par_dc[1][1][1] * time_xa + pol_par_dc[1][2][1] * time_xa * time_xa + pol_par_dc[1][3][1] * time_xa * time_xa * time_xa + pol_par_dc[1][4][1] * time_xa * time_xa * time_xa * time_xa);
	if (func_nr_b == 0)d_b = 0;
	if (func_nr_b == 3)d_b = scale[1]*(pol_par_dc[2][0][1] + pol_par_dc[2][1][1] * time_xa + pol_par_dc[2][2][1] * time_xa * time_xa + pol_par_dc[2][3][1] * time_xa * time_xa * time_xa + pol_par_dc[2][4][1] * time_xa * time_xa * time_xa * time_xa);

	xa1_pm[0] = wirenr_xa1[i] - 119 + d_a;
	xa1_pm[1] = wirenr_xa1[i] - 119 - d_a;
	xb1_pm[0] = wirenr_xb1[j] - 118.5 + d_b;
	xb1_pm[1] = wirenr_xb1[j] - 118.5 - d_b;

	double dmin1 = 999;
	for (int k = 0; k < 2; k++) {
	  for (int m = 0; m < 2; m++) {
	    if (fabs(xa1_pm[k] - xb1_pm[m]) < dmin1) {
	      dmin1 = fabs(xa1_pm[k] - xb1_pm[m]);
	      x1_ab[0][pair_x1] = xa1_pm[k];
	      x1_ab[1][pair_x1] = xb1_pm[m];
	    }
	  }
	}
	if (d_a < 0.02) {
	  sigm_x1_ab[0][pair_x1] = 0.08 * 0.08;
	}
	if (d_a >= 0.02 && d_a < 0.1) {
	  sigm_x1_ab[0][pair_x1] = 0.06 * 0.06;
	}
	if (d_a >= 0.1 && d_a < 0.4) {
	  sigm_x1_ab[0][pair_x1] = 0.025 * 0.025;
	}
	if (d_a >= 0.4 && d_a < 0.41) {
	  sigm_x1_ab[0][pair_x1] = 0.08 * 0.08;
	}
	if (d_a >= 0.41) {
	  sigm_x1_ab[0][pair_x1] = 0.10 * 0.10;
	}

	if (d_b < 0.02) {
	  sigm_x1_ab[1][pair_x1] = 0.08 * 0.08;
	}
	if (d_b >= 0.02 && d_b < 0.1) {
	  sigm_x1_ab[1][pair_x1] = 0.06 * 0.06;
	}
	if (d_b >= 0.1 && d_b < 0.4) {
	  sigm_x1_ab[1][pair_x1] = 0.025 * 0.025;
	}
	if (d_b >= 0.4 && d_b < 0.41) {
	  sigm_x1_ab[1][pair_x1] = 0.08 * 0.08;
	}
	if (d_b >= 0.41) {
	  sigm_x1_ab[1][pair_x1] = 0.10 * 0.10;
	}
	pair_x1++;

	used_xa1[i] = true;
	used_xb1[j] = true;



      }// j for2
    } // i for1  X.

    //reconstruct single X-plane hits

    for (Int_t i = 0; i < it_xa1; ++i) {
      if (used_xa1[i]) continue;

      int func_nr_a = -1;
      for (int t_it = 0; t_it < 4; t_it++) {
	if (time_xa1[i] >= t_dc[t_it][0] && time_xa1[i] < t_dc[t_it + 1][0]) {
	  func_nr_a = t_it;
	  break;
	}
      }
      double time_xa = time_xa1[i];
      double d_a = 0;
      double d_b = 0;

      if (func_nr_a == 1 || func_nr_a == 2) d_a = scale[0]*(pol_par_dc[1][0][0] + pol_par_dc[1][1][0] * time_xa + pol_par_dc[1][2][0] * time_xa * time_xa + pol_par_dc[1][3][0] * time_xa * time_xa * time_xa + pol_par_dc[1][4][0] * time_xa * time_xa * time_xa * time_xa);
      if (func_nr_a == 0)d_a = 0;
      if (func_nr_a == 3)d_a = scale[0]*(pol_par_dc[2][0][0] + pol_par_dc[2][1][0] * time_xa + pol_par_dc[2][2][0] * time_xa * time_xa + pol_par_dc[2][3][0] * time_xa * time_xa * time_xa + pol_par_dc[2][4][0] * time_xa * time_xa * time_xa * time_xa);

      x1_single[0][single_xa1] = wirenr_xa1[i] - 119 + d_a;
      x1_single[0][single_xa1 + 1] = wirenr_xa1[i] - 119 - d_a;

      if (d_a < 0.02) {
	sigm_x1_single[0][single_xa1] = 0.08 * 0.08;
	sigm_x1_single[0][single_xa1 + 1] = 0.08 * 0.08;
      }
      if (d_a >= 0.02 && d_a < 0.1) {
	sigm_x1_single[0][single_xa1] = 0.06 * 0.06;
	sigm_x1_single[0][single_xa1 + 1] = 0.06 * 0.06;
      }
      if (d_a >= 0.1 && d_a < 0.4) {
	sigm_x1_single[0][single_xa1] = 0.025 * 0.025;
	sigm_x1_single[0][single_xa1 + 1] = 0.025 * 0.025;
      }
      if (d_a >= 0.4 && d_a < 0.41) {
	sigm_x1_single[0][single_xa1] = 0.08 * 0.08;
	sigm_x1_single[0][single_xa1 + 1] = 0.08 * 0.08;
      }
      if (d_a >= 0.41) {
	sigm_x1_single[0][single_xa1] = 0.10 * 0.10;
	sigm_x1_single[0][single_xa1 + 1] = 0.10 * 0.10;
      }

      single_xa1 += 2;
    }//for single xa

    for (Int_t j = 0; j < it_xb1; ++j) {
      if (used_xb1[j]) continue;
      int func_nr_b = -1;

      double time_xa = time_xb1[j];
      double d_a = 0;
      double d_b = 0;
      for (int t_it = 0; t_it < 4; t_it++) {
	if (time_xb1[j] >= t_dc[t_it][1] && time_xb1[j] < t_dc[t_it + 1][1]) {
	  func_nr_b = t_it;
	  break;
	}
      }
      time_xa = time_xb1[j];
      if (func_nr_b == 1 || func_nr_b == 2)d_b = scale[1]*(pol_par_dc[1][0][1] + pol_par_dc[1][1][1] * time_xa + pol_par_dc[1][2][1] * time_xa * time_xa + pol_par_dc[1][3][1] * time_xa * time_xa * time_xa + pol_par_dc[1][4][1] * time_xa * time_xa * time_xa * time_xa);
      if (func_nr_b == 0)d_b = 0;
      if (func_nr_b == 3)d_b = scale[1]*(pol_par_dc[2][0][1] + pol_par_dc[2][1][1] * time_xa + pol_par_dc[2][2][1] * time_xa * time_xa + pol_par_dc[2][3][1] * time_xa * time_xa * time_xa + pol_par_dc[2][4][1] * time_xa * time_xa * time_xa * time_xa);
      x1_single[1][single_xb1] = wirenr_xb1[j] - 118.5 + d_b;
      x1_single[1][single_xb1 + 1] = wirenr_xb1[j] - 118.5 - d_b;

      if (d_b < 0.02) {
	sigm_x1_single[1][single_xb1] = 0.08 * 0.08;
	sigm_x1_single[1][single_xb1 + 1] = 0.08 * 0.08;
      }
      if (d_b >= 0.02 && d_b < 0.1) {
	sigm_x1_single[1][single_xb1] = 0.06 * 0.06;
	sigm_x1_single[1][single_xb1 + 1] = 0.06 * 0.06;
      }
      if (d_b >= 0.1 && d_b < 0.4) {
	sigm_x1_single[1][single_xb1] = 0.0250 * 0.0250;
	sigm_x1_single[1][single_xb1 + 1] = 0.0250 * 0.0250;
      }
      if (d_b >= 0.4 && d_b < 0.41) {
	sigm_x1_single[1][single_xb1] = 0.08 * 0.08;
	sigm_x1_single[1][single_xb1 + 1] = 0.08 * 0.08;
      }
      if (d_b >= 0.41) {
	sigm_x1_single[1][single_xb1] = 0.10 * 0.10;
	sigm_x1_single[1][single_xb1 + 1] = 0.10 * 0.10;
      }

      single_xb1 += 2;

    } // i for1  X. 

    //   ----   Y   ----

    for (Int_t i = 0; i < it_ya1; ++i) {
      for (Int_t j = 0; j < it_yb1; ++j) {
	if (pair_y1 > 48)break;
	if ((wirenr_ya1[i] != wirenr_yb1[j] && wirenr_ya1[i] != wirenr_yb1[j] + 1)) continue;
	int func_nr_a = -1;
	int func_nr_b = -1;
	for (int t_it = 0; t_it < 4; t_it++) {
	  if (time_ya1[i] >= t_dc[t_it][2] && time_ya1[i] < t_dc[t_it + 1][2]) {
	    func_nr_a = t_it;
	    break;
	  }
	}
	double d_a = 0;
	double d_b = 0;

	if (func_nr_a == 1 || func_nr_a == 2) d_a = scale[2]*(pol_par_dc[1][0][2] + pol_par_dc[1][1][2] * time_ya1[i] + pol_par_dc[1][2][2] * time_ya1[i] * time_ya1[i] + pol_par_dc[1][3][2] * time_ya1[i] * time_ya1[i] * time_ya1[i] + pol_par_dc[1][4][2] * time_ya1[i] * time_ya1[i] * time_ya1[i] * time_ya1[i]);
	if (func_nr_a == 0)d_a = 0;
	if (func_nr_a == 3)d_a = scale[2]*(pol_par_dc[2][0][2] + pol_par_dc[2][1][2] * time_ya1[i] + pol_par_dc[2][2][2] * time_ya1[i] * time_ya1[i] + pol_par_dc[2][3][2] * time_ya1[i] * time_ya1[i] * time_ya1[i] + pol_par_dc[2][4][2] * time_ya1[i] * time_ya1[i] * time_ya1[i] * time_ya1[i]);
	for (int t_it = 0; t_it < 4; t_it++) {
	  if (time_yb1[j] >= t_dc[t_it][3] && time_yb1[j] < t_dc[t_it + 1][3]) {
	    func_nr_b = t_it;
	    break;
	  }
	}
	if (func_nr_b == 1 || func_nr_b == 2) d_b = scale[3]*(pol_par_dc[1][0][3] + pol_par_dc[1][1][3] * time_yb1[j] + pol_par_dc[1][2][3] * time_yb1[j] * time_yb1[j] + pol_par_dc[1][3][3] * time_yb1[j] * time_yb1[j] * time_yb1[j] + pol_par_dc[1][4][3] * time_yb1[j] * time_yb1[j] * time_yb1[j] * time_yb1[j]);
	if (func_nr_b == 0)d_b = 0;
	if (func_nr_b == 3)d_b = scale[3]*(pol_par_dc[2][0][3] + pol_par_dc[2][1][3] * time_yb1[j] + pol_par_dc[2][2][3] * time_yb1[j] * time_yb1[j] + pol_par_dc[2][3][3] * time_yb1[j] * time_yb1[j] * time_yb1[j] + pol_par_dc[2][4][3] * time_yb1[j] * time_yb1[j] * time_yb1[j] * time_yb1[j]);

	ya1_pm[0] = wirenr_ya1[i] - 119 + d_a;
	ya1_pm[1] = wirenr_ya1[i] - 119 - d_a;
	yb1_pm[0] = wirenr_yb1[j] - 118.5 + d_b;
	yb1_pm[1] = wirenr_yb1[j] - 118.5 - d_b;

	double dmin1 = 999;

	for (int k = 0; k < 2; k++) {
	  for (int m = 0; m < 2; m++) {
	    if (fabs(ya1_pm[k] - yb1_pm[m]) < dmin1) {
	      dmin1 = fabs(ya1_pm[k] - yb1_pm[m]);
	      y1_ab[0][pair_y1] = ya1_pm[k];
	      y1_ab[1][pair_y1] = yb1_pm[m];
	    }
	  }
	}
	if (d_a < 0.02) {
	  sigm_y1_ab[0][pair_y1] = 0.08 * 0.08;
	}
	if (d_a >= 0.02 && d_a < 0.1) {
	  sigm_y1_ab[0][pair_y1] = 0.06 * 0.06;
	}
	if (d_a >= 0.1 && d_a < 0.4) {
	  sigm_y1_ab[0][pair_y1] = 0.025 * 0.025;
	}
	if (d_a >= 0.4 && d_a < 0.41) {
	  sigm_y1_ab[0][pair_y1] = 0.08 * 0.08;
	}
	if (d_a >= 0.41) {
	  sigm_y1_ab[0][pair_y1] = 0.10 * 0.10;
	}

	if (d_b < 0.02) {
	  sigm_y1_ab[1][pair_y1] = 0.08 * 0.08;
	}
	if (d_b >= 0.02 && d_b < 0.1) {
	  sigm_y1_ab[1][pair_y1] = 0.06 * 0.06;
	}
	if (d_b >= 0.1 && d_b < 0.4) {
	  sigm_y1_ab[1][pair_y1] = 0.025 * 0.025;
	}
	if (d_b >= 0.4 && d_b < 0.41) {
	  sigm_y1_ab[1][pair_y1] = 0.08 * 0.08;
	}
	if (d_b >= 0.41) {
	  sigm_y1_ab[1][pair_y1] = 0.10 * 0.10;
	}
	pair_y1++;


	used_ya1[i] = true;
	used_yb1[j] = true;


      }// j for2
    } // i for1  Y.

    //reconstruct single Y-plane hits

    for (Int_t i = 0; i < it_ya1; ++i) {
      if (used_ya1[i]) continue;
      double d_a = 0;
      double d_b = 0;
      int func_nr_a = -1;
      for (int t_it = 0; t_it < 4; t_it++) {
	if (time_ya1[i] >= t_dc[t_it][2] && time_ya1[i] < t_dc[t_it + 1][2]) {
	  func_nr_a = t_it;
	  break;
	}
      }
      if (func_nr_a == 1 || func_nr_a == 2) d_a = scale[2]*(pol_par_dc[1][0][2] + pol_par_dc[1][1][2] * time_ya1[i] + pol_par_dc[1][2][2] * time_ya1[i] * time_ya1[i] + pol_par_dc[1][3][2] * time_ya1[i] * time_ya1[i] * time_ya1[i] + pol_par_dc[1][4][2] * time_ya1[i] * time_ya1[i] * time_ya1[i] * time_ya1[i]);
      if (func_nr_a == 0)d_a = 0;
      if (func_nr_a == 3)d_a = scale[2]*(pol_par_dc[2][0][2] + pol_par_dc[2][1][2] * time_ya1[i] + pol_par_dc[2][2][2] * time_ya1[i] * time_ya1[i] + pol_par_dc[2][3][2] * time_ya1[i] * time_ya1[i] * time_ya1[i] + pol_par_dc[2][4][2] * time_ya1[i] * time_ya1[i] * time_ya1[i] * time_ya1[i]);

      y1_single[0][single_ya1] = wirenr_ya1[i] - 119 + d_a;
      y1_single[0][single_ya1 + 1] = wirenr_ya1[i] - 119 - d_a;

      if (d_a < 0.02) {
	sigm_y1_single[0][single_ya1] = 0.08 * 0.08;
	sigm_y1_single[0][single_ya1 + 1] = 0.08 * 0.08;
      }
      if (d_a >= 0.02 && d_a < 0.1) {
	sigm_y1_single[0][single_ya1] = 0.06 * 0.06;
	sigm_y1_single[0][single_ya1 + 1] = 0.06 * 0.06;
      }
      if (d_a >= 0.1 && d_a < 0.4) {
	sigm_y1_single[0][single_ya1] = 0.025 * 0.025;
	sigm_y1_single[0][single_ya1 + 1] = 0.025 * 0.025;
      }
      if (d_a >= 0.4 && d_a < 0.41) {
	sigm_y1_single[0][single_ya1] = 0.08 * 0.08;
	sigm_y1_single[0][single_ya1 + 1] = 0.08 * 0.08;
      }
      if (d_a >= 0.41) {
	sigm_y1_single[0][single_ya1] = 0.10 * 0.10;
	sigm_y1_single[0][single_ya1 + 1] = 0.10 * 0.10;
      }
      single_ya1 += 2;
    }//for single ya

    for (Int_t j = 0; j < it_yb1; ++j) {
      if (used_yb1[j]) continue;
      double time_ya = time_yb1[j];
      int func_nr_b = -1;
      for (int t_it = 0; t_it < 4; t_it++) {
	if (time_yb1[j] >= t_dc[t_it][3] && time_yb1[j] < t_dc[t_it + 1][3]) {
	  func_nr_b = t_it;
	  break;
	}
      }
      double d_a = 0;
      double d_b = 0;
      if (func_nr_b == 1 || func_nr_b == 2) d_b = scale[3]*(pol_par_dc[1][0][3] + pol_par_dc[1][1][3] * time_yb1[j] + pol_par_dc[1][2][3] * time_yb1[j] * time_yb1[j] + pol_par_dc[1][3][3] * time_yb1[j] * time_yb1[j] * time_yb1[j] + pol_par_dc[1][4][3] * time_yb1[j] * time_yb1[j] * time_yb1[j] * time_yb1[j]);
      if (func_nr_b == 0)d_b = 0;
      if (func_nr_b == 3)d_b = scale[3]*(pol_par_dc[2][0][3] + pol_par_dc[2][1][3] * time_yb1[j] + pol_par_dc[2][2][3] * time_yb1[j] * time_yb1[j] + pol_par_dc[2][3][3] * time_yb1[j] * time_yb1[j] * time_yb1[j] + pol_par_dc[2][4][3] * time_yb1[j] * time_yb1[j] * time_yb1[j] * time_yb1[j]);
      y1_single[1][single_yb1] = wirenr_yb1[j] - 118.5 + d_b;
      y1_single[1][single_yb1 + 1] = wirenr_yb1[j] - 118.5 - d_b;

      if (d_b < 0.02) {
	sigm_y1_single[1][single_yb1] = 0.08 * 0.08;
	sigm_y1_single[1][single_yb1 + 1] = 0.08 * 0.08;
      }
      if (d_b >= 0.02 && d_b < 0.1) {
	sigm_y1_single[1][single_yb1] = 0.06 * 0.06;
	sigm_y1_single[1][single_yb1 + 1] = 0.06 * 0.06;
      }
      if (d_b >= 0.1 && d_b < 0.4) {
	sigm_y1_single[1][single_yb1] = 0.025 * 0.025;
	sigm_y1_single[1][single_yb1 + 1] = 0.025 * 0.025;
      }
      if (d_b >= 0.4 && d_b < 0.41) {
	sigm_y1_single[1][single_yb1] = 0.10 * 0.10;
	sigm_y1_single[1][single_yb1 + 1] = 0.10 * 0.10;
      }
      if (d_b >= 0.41) {
	sigm_y1_single[1][single_yb1] = 0.10 * 0.10;
	sigm_y1_single[1][single_yb1 + 1] = 0.10 * 0.10;
      }
      single_yb1 += 2;

    } // i for1  Y.

    //   ----   U   ---

    for (Int_t i = 0; i < it_ua1; ++i) {
      for (Int_t j = 0; j < it_ub1; ++j) {
	if (pair_u1 > 48)break;
	if ((wirenr_ua1[i] != wirenr_ub1[j] && wirenr_ua1[i] != wirenr_ub1[j] + 1)) continue;
	double d_a = 0;
	double d_b = 0;

	int func_nr_a = -1;
	int func_nr_b = -1;
	for (int t_it = 0; t_it < 4; t_it++) {
	  if (time_ua1[i] >= t_dc[t_it][4] && time_ua1[i] < t_dc[t_it + 1][4]) {
	    func_nr_a = t_it;
	    break;
	  }
	}
	if (func_nr_a == 0)d_a = 0;
	if (func_nr_a == 3)d_a = scale[4]*(pol_par_dc[2][0][4] + pol_par_dc[2][1][4] * time_ua1[i] + pol_par_dc[2][2][4] * time_ua1[i] * time_ua1[i] + pol_par_dc[2][3][4] * time_ua1[i] * time_ua1[i] * time_ua1[i] + pol_par_dc[2][4][4] * time_ua1[i] * time_ua1[i] * time_ua1[i] * time_ua1[i]);
	if (func_nr_a == 1 || func_nr_a == 2) d_a = scale[4]*(pol_par_dc[1][0][4] + pol_par_dc[1][1][4] * time_ua1[i] + pol_par_dc[1][2][4] * time_ua1[i] * time_ua1[i] + pol_par_dc[1][3][4] * time_ua1[i] * time_ua1[i] * time_ua1[i] + pol_par_dc[1][4][4] * time_ua1[i] * time_ua1[i] * time_ua1[i] * time_ua1[i]);
	for (int t_it = 0; t_it < 4; t_it++) {
	  if (time_ub1[j] >= t_dc[t_it][5] && time_ub1[j] < t_dc[t_it + 1][5]) {
	    func_nr_b = t_it;
	    break;
	  }
	}

	if (func_nr_b == 0)d_b = 0;
	if (func_nr_b == 3)d_b = scale[5]*(pol_par_dc[2][0][5] + pol_par_dc[2][1][5] * time_ub1[j] + pol_par_dc[2][2][5] * time_ub1[j] * time_ub1[j] + pol_par_dc[2][3][5] * time_ub1[j] * time_ub1[j] * time_ub1[j] + pol_par_dc[2][4][5] * time_ub1[j] * time_ub1[j] * time_ub1[j] * time_ub1[j]);
	if (func_nr_b == 1 || func_nr_b == 2)d_b = scale[5]*(pol_par_dc[1][0][5] + pol_par_dc[1][1][5] * time_ub1[j] + pol_par_dc[1][2][5] * time_ub1[j] * time_ub1[j] + pol_par_dc[1][3][5] * time_ub1[j] * time_ub1[j] * time_ub1[j] + pol_par_dc[1][4][5] * time_ub1[j] * time_ub1[j] * time_ub1[j] * time_ub1[j]);
	ua1_pm[0] = wirenr_ua1[i] - 119 + d_a;
	ua1_pm[1] = wirenr_ua1[i] - 119 - d_a;
	ub1_pm[0] = wirenr_ub1[j] - 118.5 + d_b;
	ub1_pm[1] = wirenr_ub1[j] - 118.5 - d_b;
	double dmin1 = 999;

	for (int k = 0; k < 2; k++) {
	  for (int m = 0; m < 2; m++) {
	    if (fabs(ua1_pm[k] - ub1_pm[m]) < dmin1) {
	      dmin1 = fabs(ua1_pm[k] - ub1_pm[m]);
	      u1_ab[0][pair_u1] = ua1_pm[k];
	      u1_ab[1][pair_u1] = ub1_pm[m];
	    }
	  }
	}
	if (d_a < 0.02) {
	  sigm_u1_ab[0][pair_u1] = 0.08 * 0.08;
	}
	if (d_a >= 0.02 && d_a < 0.1) {
	  sigm_u1_ab[0][pair_u1] = 0.06 * 0.06;
	}
	if (d_a >= 0.1 && d_a < 0.4) {
	  sigm_u1_ab[0][pair_u1] = 0.025 * 0.025;
	}
	if (d_a >= 0.4 && d_a < 0.41) {
	  sigm_u1_ab[0][pair_u1] = 0.08 * 0.08;
	}
	if (d_a >= 0.41) {
	  sigm_u1_ab[0][pair_u1] = 0.10 * 0.10;
	}

	if (d_b < 0.02) {
	  sigm_u1_ab[1][pair_u1] = 0.08 * 0.08;
	}
	if (d_b >= 0.02 && d_b < 0.1) {
	  sigm_u1_ab[1][pair_u1] = 0.06 * 0.06;
	}
	if (d_b >= 0.1 && d_b < 0.4) {
	  sigm_u1_ab[1][pair_u1] = 0.025 * 0.025;
	}
	if (d_b >= 0.4 && d_b < 0.41) {
	  sigm_u1_ab[1][pair_u1] = 0.08 * 0.08;
	}
	if (d_b >= 0.41) {
	  sigm_u1_ab[1][pair_u1] = 0.10 * 0.10;
	}
	pair_u1++;
	used_ua1[i] = true;
	used_ub1[j] = true;
      }// j for2
    } // i for1  U.

    //reconstruct single U-plane hits

    for (Int_t i = 0; i < it_ua1; ++i) {
      if (used_ua1[i]) continue;

      double d_a = 0;
      double d_b = 0;
      int func_nr_a = -1;
      for (int t_it = 0; t_it < 4; t_it++) {
	if (time_ua1[i] >= t_dc[t_it][4] && time_ua1[i] < t_dc[t_it + 1][4]) {
	  func_nr_a = t_it;
	  break;
	}
      }
      if (func_nr_a == 0)d_a = 0;
      if (func_nr_a == 3)d_a = scale[4]*(pol_par_dc[2][0][4] + pol_par_dc[2][1][4] * time_ua1[i] + pol_par_dc[2][2][4] * time_ua1[i] * time_ua1[i] + pol_par_dc[2][3][4] * time_ua1[i] * time_ua1[i] * time_ua1[i] + pol_par_dc[2][4][4] * time_ua1[i] * time_ua1[i] * time_ua1[i] * time_ua1[i]);
      if (func_nr_a == 1 || func_nr_a == 2) d_a = scale[4]*(pol_par_dc[1][0][4] + pol_par_dc[1][1][4] * time_ua1[i] + pol_par_dc[1][2][4] * time_ua1[i] * time_ua1[i] + pol_par_dc[1][3][4] * time_ua1[i] * time_ua1[i] * time_ua1[i] + pol_par_dc[1][4][4] * time_ua1[i] * time_ua1[i] * time_ua1[i] * time_ua1[i]);

      u1_single[0][single_ua1] = (wirenr_ua1[i] - 119 + d_a);
      u1_single[0][single_ua1 + 1] = (wirenr_ua1[i] - 119 - d_a);
      if (d_a < 0.02) {
	sigm_u1_single[0][single_ua1] = 0.08 * 0.08;
	sigm_u1_single[0][single_ua1 + 1] = 0.08 * 0.08;
      }
      if (d_a >= 0.02 && d_a < 0.1) {
	sigm_u1_single[0][single_ua1] = 0.06 * 0.06;
	sigm_u1_single[0][single_ua1 + 1] = 0.06 * 0.06;
      }
      if (d_a >= 0.1 && d_a < 0.4) {
	sigm_u1_single[0][single_ua1] = 0.025 * 0.025;
	sigm_u1_single[0][single_ua1 + 1] = 0.025 * 0.025;
      }
      if (d_a >= 0.4 && d_a < 0.41) {
	sigm_u1_single[0][single_ua1] = 0.08 * 0.08;
	sigm_u1_single[0][single_ua1 + 1] = 0.08 * 0.08;
      }
      if (d_a >= 0.41) {
	sigm_u1_single[0][single_ua1] = 0.10 * 0.10;
	sigm_u1_single[0][single_ua1 + 1] = 0.10 * 0.10;
      }
      single_ua1 += 2;
    }//for single ua

    for (Int_t j = 0; j < it_ub1; ++j) {
      if (used_ub1[j]) continue;
      int func_nr_b = -1;
      double d_a = 0;
      double d_b = 0;
      for (int t_it = 0; t_it < 4; t_it++) {
	if (time_ub1[j] >= t_dc[t_it][5] && time_ub1[j] < t_dc[t_it + 1][5]) {
	  func_nr_b = t_it;
	  break;
	}
      }
      if (func_nr_b == 0)d_b = 0;
      if (func_nr_b == 3)d_b = scale[5]*(pol_par_dc[2][0][5] + pol_par_dc[2][1][5] * time_ub1[j] + pol_par_dc[2][2][5] * time_ub1[j] * time_ub1[j] + pol_par_dc[2][3][5] * time_ub1[j] * time_ub1[j] * time_ub1[j] + pol_par_dc[2][4][5] * time_ub1[j] * time_ub1[j] * time_ub1[j] * time_ub1[j]);
      if (func_nr_b == 1 || func_nr_b == 2)d_b = scale[5]*(pol_par_dc[1][0][5] + pol_par_dc[1][1][5] * time_ub1[j] + pol_par_dc[1][2][5] * time_ub1[j] * time_ub1[j] + pol_par_dc[1][3][5] * time_ub1[j] * time_ub1[j] * time_ub1[j] + pol_par_dc[1][4][5] * time_ub1[j] * time_ub1[j] * time_ub1[j] * time_ub1[j]);
      u1_single[1][single_ub1] = (wirenr_ub1[j] - 118.5 + d_b);
      u1_single[1][single_ub1 + 1] = (wirenr_ub1[j] - 118.5 - d_b);

      //hdev_occup[1]->Fill(d_b);
      if (d_b < 0.02) {
	sigm_u1_single[1][single_ub1] = 0.08 * 0.08;
	sigm_u1_single[1][single_ub1 + 1] = 0.08 * 0.08;
      }
      if (d_b >= 0.02 && d_b < 0.1) {
	sigm_u1_single[1][single_ub1] = 0.06 * 0.06;
	sigm_u1_single[1][single_ub1 + 1] = 0.06 * 0.06;
      }
      if (d_b >= 0.1 && d_b < 0.4) {
	sigm_u1_single[1][single_ub1] = 0.025 * 0.025;
	sigm_u1_single[1][single_ub1 + 1] = 0.025 * 0.025;
      }
      if (d_b >= 0.4 && d_b < 0.41) {
	sigm_u1_single[1][single_ub1] = 0.08 * 0.08;
	sigm_u1_single[1][single_ub1 + 1] = 0.08 * 0.08;
      }
      if (d_b >= 0.41) {
	sigm_u1_single[1][single_ub1] = 0.10 * 0.10;
	sigm_u1_single[1][single_ub1 + 1] = 0.10 * 0.10;
      }
      single_ub1 += 2;
    } // i for1  U.


    //   ----   V   ---

    for (Int_t i = 0; i < it_va1; ++i) {
      for (Int_t j = 0; j < it_vb1; ++j) {

	if (pair_v1 > 48)break;
	if ((wirenr_va1[i] != wirenr_vb1[j] && wirenr_va1[i] != wirenr_vb1[j] + 1)) continue;

	double d_a = 0;
	double d_b = 0;

	int func_nr_a = -1;
	int func_nr_b = -1;
	for (int t_it = 0; t_it < 4; t_it++) {
	  if (time_va1[i] >= t_dc[t_it][6] && time_va1[i] < t_dc[t_it + 1][6]) {
	    func_nr_a = t_it;
	    break;
	  }
	}

	if (func_nr_a == 0)d_a = 0;
	if (func_nr_a == 3)d_a = scale[6]*(pol_par_dc[2][0][6] + pol_par_dc[2][1][6] * time_va1[i] + pol_par_dc[2][2][6] * time_va1[i] * time_va1[i] + pol_par_dc[2][3][6] * time_va1[i] * time_va1[i] * time_va1[i] + pol_par_dc[2][4][6] * time_va1[i] * time_va1[i] * time_va1[i] * time_va1[i]);
	if (func_nr_a == 1 || func_nr_a == 2) d_a = scale[6]*(pol_par_dc[1][0][6] + pol_par_dc[1][1][6] * time_va1[i] + pol_par_dc[1][2][6] * time_va1[i] * time_va1[i] + pol_par_dc[1][3][6] * time_va1[i] * time_va1[i] * time_va1[i] + pol_par_dc[1][4][6] * time_va1[i] * time_va1[i] * time_va1[i] * time_va1[i]);
	for (int t_it = 0; t_it < 4; t_it++) {
	  if (time_vb1[j] >= t_dc[t_it][7] && time_vb1[j] < t_dc[t_it + 1][7]) {
	    func_nr_b = t_it;
	    break;
	  }
	}

	if (func_nr_b == 1 || func_nr_b == 2)d_b = scale[7]*(pol_par_dc[1][0][7] + pol_par_dc[1][1][7] * time_vb1[j] + pol_par_dc[1][2][7] * time_vb1[j] * time_vb1[j] + pol_par_dc[1][3][7] * time_vb1[j] * time_vb1[j] * time_vb1[j] + pol_par_dc[1][4][7] * time_vb1[j] * time_vb1[j] * time_vb1[j] * time_vb1[j]);

	if (func_nr_b == 0)d_b = 0;
	if (func_nr_b == 3)d_b = scale[7]*(pol_par_dc[2][0][7] + pol_par_dc[2][1][7] * time_vb1[j] + pol_par_dc[2][2][7] * time_vb1[j] * time_vb1[j] + pol_par_dc[2][3][7] * time_vb1[j] * time_vb1[j] * time_vb1[j] + pol_par_dc[2][4][7] * time_vb1[j] * time_vb1[j] * time_vb1[j] * time_vb1[j]);

	va1_pm[0] = wirenr_va1[i] - 119 + d_a;
	va1_pm[1] = wirenr_va1[i] - 119 - d_a;
	vb1_pm[0] = wirenr_vb1[j] - 118.5 + d_b;
	vb1_pm[1] = wirenr_vb1[j] - 118.5 - d_b;
	double dmin1 = 999;

	for (int k = 0; k < 2; k++) {
	  for (int m = 0; m < 2; m++) {
	    if (fabs(va1_pm[k] - vb1_pm[m]) < dmin1) {
	      dmin1 = fabs(va1_pm[k] - vb1_pm[m]);
	      v1_ab[0][pair_v1] = va1_pm[k];
	      v1_ab[1][pair_v1] = vb1_pm[m];
	    }
	  }
	}
	if (d_a < 0.02) {
	  sigm_v1_ab[0][pair_v1] = 0.08 * 0.08;
	}
	if (d_a >= 0.02 && d_a < 0.1) {
	  sigm_v1_ab[0][pair_v1] = 0.06 * 0.06;
	}
	if (d_a >= 0.1 && d_a < 0.4) {
	  sigm_v1_ab[0][pair_v1] = 0.025 * 0.025;
	}
	if (d_a >= 0.4 && d_a < 0.41) {
	  sigm_v1_ab[0][pair_v1] = 0.08 * 0.08;
	}
	if (d_a >= 0.41) {
	  sigm_v1_ab[0][pair_v1] = 0.10 * 0.10;
	}

	if (d_b < 0.02) {
	  sigm_v1_ab[1][pair_v1] = 0.08 * 0.08;
	}
	if (d_b >= 0.02 && d_b < 0.1) {
	  sigm_v1_ab[1][pair_v1] = 0.06 * 0.06;
	}
	if (d_b >= 0.1 && d_b < 0.4) {
	  sigm_v1_ab[1][pair_v1] = 0.025 * 0.025;
	}
	if (d_b >= 0.4 && d_b < 0.41) {
	  sigm_v1_ab[1][pair_v1] = 0.08 * 0.08;
	}
	if (d_b >= 0.41) {
	  sigm_v1_ab[1][pair_v1] = 0.10 * 0.10;
	}
	pair_v1++;
	used_va1[i] = true;
	used_vb1[j] = true;

      }// j for2
    } // i for1  V.

    //reconstruct single V-plane hits

    for (Int_t i = 0; i < it_va1; ++i) {
      if (used_va1[i]) continue;

      double d_a = 0;
      double d_b = 0;
      int func_nr_a = -1;
      for (int t_it = 0; t_it < 4; t_it++) {
	if (time_va1[i] >= t_dc[t_it][6] && time_va1[i] < t_dc[t_it + 1][6]) {
	  func_nr_a = t_it;
	  break;
	}
      }
      if (func_nr_a == 0)d_a = 0;
      if (func_nr_a == 3)d_a = scale[6]*(pol_par_dc[2][0][6] + pol_par_dc[2][1][6] * time_va1[i] + pol_par_dc[2][2][6] * time_va1[i] * time_va1[i] + pol_par_dc[2][3][6] * time_va1[i] * time_va1[i] * time_va1[i] + pol_par_dc[2][4][6] * time_va1[i] * time_va1[i] * time_va1[i] * time_va1[i]);
      if (func_nr_a == 1 || func_nr_a == 2) d_a = scale[6]*(pol_par_dc[1][0][6] + pol_par_dc[1][1][6] * time_va1[i] + pol_par_dc[1][2][6] * time_va1[i] * time_va1[i] + pol_par_dc[1][3][6] * time_va1[i] * time_va1[i] * time_va1[i] + pol_par_dc[1][4][6] * time_va1[i] * time_va1[i] * time_va1[i] * time_va1[i]);

      v1_single[0][single_va1] = wirenr_va1[i] - 119 + d_a;
      v1_single[0][single_va1 + 1] = wirenr_va1[i] - 119 - d_a;

      if (d_a < 0.02) {
	sigm_v1_single[0][single_va1] = 0.08 * 0.08;
	sigm_v1_single[0][single_va1 + 1] = 0.08 * 0.08;
      }
      if (d_a >= 0.02 && d_a < 0.1) {
	sigm_v1_single[0][single_va1] = 0.06 * 0.06;
	sigm_v1_single[0][single_va1 + 1] = 0.06 * 0.06;
      }
      if (d_a >= 0.1 && d_a < 0.4) {
	sigm_v1_single[0][single_va1] = 0.025 * 0.025;
	sigm_v1_single[0][single_va1 + 1] = 0.025 * 0.025;
      }
      if (d_a >= 0.4 && d_a < 0.41) {
	sigm_v1_single[0][single_va1] = 0.08 * 0.08;
	sigm_v1_single[0][single_va1 + 1] = 0.08 * 0.08;
      }
      if (d_a >= 0.41) {
	sigm_v1_single[0][single_va1] = 0.10 * 0.10;
	sigm_v1_single[0][single_va1 + 1] = 0.10 * 0.10;
      }
      single_va1 += 2;
    }//for single va

    for (Int_t j = 0; j < it_vb1; ++j) {
      if (used_vb1[j]) continue;
      double d_a = 0;
      double d_b = 0;
      int func_nr_b = -1;
      for (int t_it = 0; t_it < 4; t_it++) {
	if (time_vb1[j] >= t_dc[t_it][7] && time_vb1[j] < t_dc[t_it + 1][7]) {
	  func_nr_b = t_it;
	  break;
	}
      }
      if (func_nr_b == 1 || func_nr_b == 2)d_b = scale[7]*(pol_par_dc[1][0][7] + pol_par_dc[1][1][7] * time_vb1[j] + pol_par_dc[1][2][7] * time_vb1[j] * time_vb1[j] + pol_par_dc[1][3][7] * time_vb1[j] * time_vb1[j] * time_vb1[j] + pol_par_dc[1][4][7] * time_vb1[j] * time_vb1[j] * time_vb1[j] * time_vb1[j]);

      if (func_nr_b == 0)d_b = 0;
      if (func_nr_b == 3)d_b = scale[7]*(pol_par_dc[2][0][7] + pol_par_dc[2][1][7] * time_vb1[j] + pol_par_dc[2][2][7] * time_vb1[j] * time_vb1[j] + pol_par_dc[2][3][7] * time_vb1[j] * time_vb1[j] * time_vb1[j] + pol_par_dc[2][4][7] * time_vb1[j] * time_vb1[j] * time_vb1[j] * time_vb1[j]);

      v1_single[1][single_vb1] = wirenr_vb1[j] - 118.5 + d_b;
      v1_single[1][single_vb1 + 1] = wirenr_vb1[j] - 118.5 - d_b;
      if (d_b < 0.02) {
	sigm_v1_single[1][single_vb1] = 0.08 * 0.08;
	sigm_v1_single[1][single_vb1 + 1] = 0.08 * 0.08;
      }
      if (d_b >= 0.02 && d_b < 0.1) {
	sigm_v1_single[1][single_vb1] = 0.06 * 0.06;
	sigm_v1_single[1][single_vb1 + 1] = 0.06 * 0.06;
      }
      if (d_b >= 0.1 && d_b < 0.4) {
	sigm_v1_single[1][single_vb1] = 0.0250 * 0.0250;
	sigm_v1_single[1][single_vb1 + 1] = 0.0250 * 0.0250;
      }
      if (d_b >= 0.4 && d_b < 0.41) {
	sigm_v1_single[1][single_vb1] = 0.08 * 0.08;
	sigm_v1_single[1][single_vb1 + 1] = 0.08 * 0.08;
      }
      if (d_b >= 0.41) {
	sigm_v1_single[1][single_vb1] = 0.10 * 0.10;
	sigm_v1_single[1][single_vb1 + 1] = 0.10 * 0.10;
      }
      single_vb1 += 2;

    } // i for1  V.
      
    //  ---  DC1 Segment Building  ------

    bool found_8p_seg = false;
    float x_slope = 0.0;
    float y_slope = 0.0;

    if (pair_x1 * pair_y1 > 0) { //(x,y)first
      for (int i = 0; i < pair_x1; i++) {
	if (nDC1_segments > 48)break;
	float x_coord = (x1_ab[0][i] + x1_ab[1][i]) / 2;
	float XU = x_coord + x_slope * 0.5 * (z_loc[5] + z_loc[4] - z_loc[1] - z_loc[0]);
	float XV = x_coord + x_slope * 0.5 * (z_loc[7] + z_loc[6] - z_loc[1] - z_loc[0]);
	for (int j = 0; j < pair_y1; j++) {
	  if (nDC1_segments > 48)break;
	  float y_coord = (y1_ab[0][j] + y1_ab[1][j]) / 2;
	  float YU = y_coord + y_slope * 0.5 * (z_loc[5] + z_loc[4] - z_loc[3] - z_loc[2]);
	  float YV = y_coord + y_slope * 0.5 * (z_loc[7] + z_loc[6] - z_loc[3] - z_loc[2]);
	  float u_est = isqrt_2 * (YU - XU);
	  float v_est = isqrt_2 * (YV + XV);
	  bool foundU = false;

	  if (pair_u1 > 0) {
	    double dU_thresh = 1.3;
	    for (int k = 0; k < pair_u1; k++) {
	      float u_coord = (u1_ab[0][k] + u1_ab[1][k]) / 2;
	      if (fabs(u_coord - u_est) > dU_thresh)continue;
	      dU_thresh = fabs(u_coord - u_est);

	      rh_segDC1[0][nDC1_segments] = x1_ab[0][i];
	      rh_segDC1[1][nDC1_segments] = x1_ab[1][i];
	      rh_segDC1[2][nDC1_segments] = y1_ab[0][j];
	      rh_segDC1[3][nDC1_segments] = y1_ab[1][j];
	      rh_segDC1[4][nDC1_segments] = u1_ab[0][k];
	      rh_segDC1[5][nDC1_segments] = u1_ab[1][k];
	      rh_sigm_segDC1[0][nDC1_segments] = sigm_x1_ab[0][i];
	      rh_sigm_segDC1[1][nDC1_segments] = sigm_x1_ab[1][i];
	      rh_sigm_segDC1[2][nDC1_segments] = sigm_y1_ab[0][j];
	      rh_sigm_segDC1[3][nDC1_segments] = sigm_y1_ab[1][j];
	      rh_sigm_segDC1[4][nDC1_segments] = sigm_u1_ab[0][k];
	      rh_sigm_segDC1[5][nDC1_segments] = sigm_u1_ab[1][k];
	      foundU = true;
	      if (nDC1_segments > 48)break;
	    }//k
	  }//(pair_u1>0)
	  if (found_8p_seg && !foundU)continue;
	  bool foundV = false;
	  if (pair_v1 > 0) {
	    double dV_thresh = 1.3;
	    for (int m = 0; m < pair_v1; m++) {
	      if (nDC1_segments > 48)break;
	      float v_coord = (v1_ab[0][m] + v1_ab[1][m]) / 2;
	      if (fabs(v_coord - v_est) > dV_thresh)continue;
	      dV_thresh = fabs(v_coord - v_est);

	      foundV = true;
	      rh_segDC1[0][nDC1_segments] = x1_ab[0][i];
	      rh_segDC1[1][nDC1_segments] = x1_ab[1][i];
	      rh_segDC1[2][nDC1_segments] = y1_ab[0][j];
	      rh_segDC1[3][nDC1_segments] = y1_ab[1][j];
	      rh_segDC1[6][nDC1_segments] = v1_ab[0][m];
	      rh_segDC1[7][nDC1_segments] = v1_ab[1][m];
	      rh_sigm_segDC1[0][nDC1_segments] = sigm_x1_ab[0][i];
	      rh_sigm_segDC1[1][nDC1_segments] = sigm_x1_ab[1][i];
	      rh_sigm_segDC1[2][nDC1_segments] = sigm_y1_ab[0][j];
	      rh_sigm_segDC1[3][nDC1_segments] = sigm_y1_ab[1][j];
	      rh_sigm_segDC1[6][nDC1_segments] = sigm_v1_ab[0][m];
	      rh_sigm_segDC1[7][nDC1_segments] = sigm_v1_ab[1][m];
	      if (!foundU) {

		float min_a = 999;
		float min_b = 999;

		for (int kk = 0; kk < single_ua1; kk++) {
		  if (fabs(u1_single[0][kk] - u_est) > 1.5)continue; //????? 0.5 needs to be reviewed
		  if (fabs(u1_single[0][kk] - u_est) < min_a) {
		    min_a = fabs(u1_single[0][kk] - u_est);
		    rh_segDC1[4][nDC1_segments] = u1_single[0][kk];
		    rh_sigm_segDC1[4][nDC1_segments] = sigm_u1_single[0][kk];

		    foundU = true;
		  }
		}//for kk
		for (int kk = 0; kk < single_ub1; kk++) {
		  if (fabs(u1_single[1][kk] - u_est) > 1.5)continue; //????? 0.5 needs to be reviewed
		  if (fabs(u1_single[1][kk] - u_est) < min_b) {
		    min_a = fabs(u1_single[1][kk] - u_est);
		    rh_segDC1[5][nDC1_segments] = u1_single[1][kk];
		    rh_sigm_segDC1[5][nDC1_segments] = sigm_u1_single[1][kk];

		    foundU = true;
		  }
		}//for kk

		if (nDC1_segments > 48)break;
	      }//!foundU

	      if (nDC1_segments > 48)break;

	    }//m

	  }//(pair_v1>0)
		    
	  if (!foundV && foundU) {
	    float min_a = 999;
	    float min_b = 999;
	    for (int kk = 0; kk < single_va1; kk++) {
	      if (fabs(v1_single[0][kk] - v_est) > 1.5)continue; //????? 0.5 needs to be reviewed

	      if (fabs(v1_single[0][kk] - v_est) < min_a) {
		min_a = fabs(v1_single[0][kk] - v_est);
		rh_segDC1[6][nDC1_segments] = v1_single[0][kk];
		rh_sigm_segDC1[6][nDC1_segments] = sigm_v1_single[0][kk];
		foundV = true;
	      }
	    }//for kk
	    for (int kk = 0; kk < single_vb1; kk++) {
	      if (fabs(v1_single[1][kk] - v_est) > 1.5)continue; //????? 0.5 needs to be reviewed
	      if (fabs(v1_single[1][kk] - v_est) < min_b) {
		min_b = fabs(v1_single[1][kk] - v_est);
		rh_segDC1[7][nDC1_segments] = v1_single[1][kk];
		rh_sigm_segDC1[7][nDC1_segments] = sigm_v1_single[1][kk];
		foundV = true;
	      }
	    }//for kk

	  }//!foundV
	  if (foundV || foundU) nDC1_segments++;
	}//j
      }//i

    }//(x,y)first


    if (pair_u1 * pair_v1 > 0 && !found_8p_seg) { // (u,v) first

      for (int i = 0; i < pair_u1; i++) {
	if (nDC1_segments > 48)break;
	float u_coord = (u1_ab[0][i] + u1_ab[1][i]) / 2;
	for (int j = 0; j < pair_v1; j++) {
	  if (nDC1_segments > 48)break;
	  float v_coord = (v1_ab[0][j] + v1_ab[1][j]) / 2;
	  bool foundX = false;
	  float y_est = isqrt_2 * (u_coord + v_coord);
	  float x_est = isqrt_2 * (v_coord - u_coord);
	  if (pair_x1 > 0) {

	    double dX_thresh = 1.5;
	    for (int k = 0; k < pair_x1; k++) {
	      float x_coord = (x1_ab[0][k] + x1_ab[1][k]) / 2;
	      if (nDC1_segments > 48)break;
	      if (fabs(x_coord - x_est) > dX_thresh)continue;
	      dX_thresh = fabs(x_coord - x_est);

	      rh_segDC1[0][nDC1_segments] = x1_ab[0][k];
	      rh_segDC1[1][nDC1_segments] = x1_ab[1][k];
	      rh_segDC1[4][nDC1_segments] = u1_ab[0][i];
	      rh_segDC1[5][nDC1_segments] = u1_ab[1][i];
	      rh_segDC1[6][nDC1_segments] = v1_ab[0][j];
	      rh_segDC1[7][nDC1_segments] = v1_ab[1][j];
	      rh_sigm_segDC1[0][nDC1_segments] = sigm_x1_ab[0][k];
	      rh_sigm_segDC1[1][nDC1_segments] = sigm_x1_ab[1][k];
	      rh_sigm_segDC1[4][nDC1_segments] = sigm_u1_ab[0][i];
	      rh_sigm_segDC1[5][nDC1_segments] = sigm_u1_ab[1][i];
	      rh_sigm_segDC1[6][nDC1_segments] = sigm_v1_ab[0][j];
	      rh_sigm_segDC1[7][nDC1_segments] = sigm_v1_ab[1][j];

	      foundX = true;
	      if (nDC1_segments > 48)break;
	    }//k

	  }//(pair_x1>0)
	  if (found_8p_seg && !foundX)continue;
	  bool foundY = false;
	  if (pair_y1 > 0) {

	    double dY_thresh = 1.0;
	    for (int m = 0; m < pair_y1; m++) {
	      if (nDC1_segments > 48)break;
	      float y_coord = (y1_ab[0][m] + y1_ab[1][m]) / 2;
	      if (fabs(y_coord - y_est) > dY_thresh)continue;
	      dY_thresh = fabs(y_coord - y_est);

	      foundY = true;
	      if (nDC1_segments > 48)break;
	      rh_segDC1[2][nDC1_segments] = y1_ab[0][m];
	      rh_segDC1[3][nDC1_segments] = y1_ab[1][m];
	      rh_segDC1[4][nDC1_segments] = u1_ab[0][i];
	      rh_segDC1[5][nDC1_segments] = u1_ab[1][i];
	      rh_segDC1[6][nDC1_segments] = v1_ab[0][j];
	      rh_segDC1[7][nDC1_segments] = v1_ab[1][j];
	      rh_sigm_segDC1[2][nDC1_segments] = sigm_y1_ab[0][m];
	      rh_sigm_segDC1[3][nDC1_segments] = sigm_y1_ab[1][m];
	      rh_sigm_segDC1[4][nDC1_segments] = sigm_u1_ab[0][i];
	      rh_sigm_segDC1[5][nDC1_segments] = sigm_u1_ab[1][i];
	      rh_sigm_segDC1[6][nDC1_segments] = sigm_v1_ab[0][j];
	      rh_sigm_segDC1[7][nDC1_segments] = sigm_v1_ab[1][j];

	      if (!foundX) {

		float min_a = 999;
		float min_b = 999;
		for (int kk = 0; kk < single_xa1; kk++) {
		  if (fabs(x1_single[0][kk] - x_est) > 1.5)continue; //????? 0.5 needs to be reviewed                                                                                     
		  if (fabs(x1_single[0][kk] - x_est) < min_a) {
		    min_a = fabs(x1_single[0][kk] - x_est);
		    rh_segDC1[0][nDC1_segments] = x1_single[0][kk];
		    rh_sigm_segDC1[0][nDC1_segments] = sigm_x1_single[0][kk];
		    foundX = true;
		  }
		}//for kk                                                                                                                                                           
		for (int kk = 0; kk < single_xb1; kk++) {
		  if (fabs(x1_single[1][kk] - x_est) > 1.5)continue; //????? 0.5 needs to be reviewed                                                                                     
		  if (fabs(x1_single[1][kk] - x_est) < min_b) {
		    min_a = fabs(x1_single[1][kk] - x_est);
		    rh_segDC1[1][nDC1_segments] = x1_single[1][kk];
		    rh_sigm_segDC1[1][nDC1_segments] = sigm_x1_single[1][kk];
		    foundX = true;
		  }
		}//for kk                                                                                                                                                           
		if (nDC1_segments > 48)break;
	      }//!foundX

	    }//m
	    if (!foundY && foundX) {
	      float min_a = 999;
	      float min_b = 999;
	      for (int kk = 0; kk < single_ya1; kk++) {
		if (fabs(y1_single[0][kk] - y_est) > 1.5)continue; //????? 0.5 needs to be reviewed                                                                                     
		if (fabs(y1_single[0][kk] - y_est) < min_a) {
		  min_a = fabs(y1_single[0][kk] - y_est);
		  rh_segDC1[2][nDC1_segments] = y1_single[0][kk];
		  rh_sigm_segDC1[2][nDC1_segments] = sigm_y1_single[0][kk];
		  foundY = true;
		}
	      }//for kk                                                                                                                                                           
	      for (int kk = 0; kk < single_yb1; kk++) {
		if (fabs(y1_single[1][kk] - y_est) > 1.5)continue; //????? 0.5 needs to be reviewed                                                                                     
		if (fabs(y1_single[1][kk] - y_est) < min_b) {
		  min_b = fabs(y1_single[1][kk] - y_est);
		  rh_segDC1[3][nDC1_segments] = y1_single[1][kk];
		  rh_sigm_segDC1[3][nDC1_segments] = sigm_y1_single[1][kk];
		  foundY = true;
		}
	      }//for kk                                                                                                                                                           
	    }//!foundY                         
	  }//(pair_y1>0)
	  if (foundX || foundY) nDC1_segments++;
	}//j
      }//i
    }//(u,v) first

    ///-------------DCH2 hit reco------------------///
    float par_ab1[4][150];
    float par_ab2[4][150];
    for (int i = 0; i < 8; i++) {
      for (int j = 0; j < 150; j++) {
	if (i > 1)continue;
	par_ab1[i][j] = -999;
	par_ab2[i][j] = -999;

      }
    }
    int pair_x2 = 0;
    int pair_y2 = 0;
    int pair_u2 = 0;
    int pair_v2 = 0;
    int single_xa2 = 0;
    int single_ya2 = 0;
    int single_ua2 = 0;
    int single_va2 = 0;
    int single_xb2 = 0;
    int single_yb2 = 0;
    int single_ub2 = 0;
    int single_vb2 = 0;

    float xa2_pm[2] = {-999};
    float xb2_pm[2] = {-999};
    float ya2_pm[2] = {-999};
    float yb2_pm[2] = {-999};
    float ua2_pm[2] = {-999};
    float ub2_pm[2] = {-999};
    float va2_pm[2] = {-999};
    float vb2_pm[2] = {-999};
        
    //   ---   X   ---

    for (Int_t i = 0; i < it_xa2; ++i) {
      for (Int_t j = 0; j < it_xb2; ++j) {
	if (pair_x2 > 48)break;
	if ((wirenr_xa2[i] != wirenr_xb2[j] && wirenr_xa2[i] != wirenr_xb2[j] + 1)) continue;
	int func_nr_a = -1;
	int func_nr_b = -1;
	for (int t_it = 0; t_it < 4; t_it++) {
	  if (time_xa2[i] >= t_dc[t_it][8] && time_xa2[i] < t_dc[t_it + 1][8]) {
	    func_nr_a = t_it;
	    break;
	  }
	}
	double time_xa = time_xa2[i];
	double d_a = 0;
	double d_b = 0;

	if (func_nr_a == 1 || func_nr_a == 2) d_a = scale[8]*(pol_par_dc[1][0][8] + pol_par_dc[1][1][8] * time_xa + pol_par_dc[1][2][8] * time_xa * time_xa + pol_par_dc[1][3][8] * time_xa * time_xa * time_xa + pol_par_dc[1][4][8] * time_xa * time_xa * time_xa * time_xa);
	if (func_nr_a == 0)d_a = 0;
	if (func_nr_a == 3)d_a = scale[8]*(pol_par_dc[2][0][8] + pol_par_dc[2][1][8] * time_xa + pol_par_dc[2][2][8] * time_xa * time_xa + pol_par_dc[2][3][8] * time_xa * time_xa * time_xa + pol_par_dc[2][4][8] * time_xa * time_xa * time_xa * time_xa);

	for (int t_it = 0; t_it < 4; t_it++) {
	  if (time_xb2[j] >= t_dc[t_it][9] && time_xb2[j] < t_dc[t_it + 1][9]) {
	    func_nr_b = t_it;
	    break;
	  }
	}
	time_xa = time_xb2[j];

	if (func_nr_b == 1 || func_nr_b == 2)d_b = scale[9]*(pol_par_dc[1][0][9] + pol_par_dc[1][1][9] * time_xa + pol_par_dc[1][2][9] * time_xa * time_xa + pol_par_dc[1][3][9] * time_xa * time_xa * time_xa + pol_par_dc[1][4][9] * time_xa * time_xa * time_xa * time_xa);
	if (func_nr_b == 0)d_b = 0;
	if (func_nr_b == 3)d_b = scale[9]*(pol_par_dc[2][0][9] + pol_par_dc[2][1][9] * time_xa + pol_par_dc[2][2][9] * time_xa * time_xa + pol_par_dc[2][3][9] * time_xa * time_xa * time_xa + pol_par_dc[2][4][9] * time_xa * time_xa * time_xa * time_xa);
                
	xa2_pm[0] = wirenr_xa2[i] - 119 + d_a;
	xa2_pm[1] = wirenr_xa2[i] - 119 - d_a;
	xb2_pm[0] = wirenr_xb2[j] - 118.5 + d_b;
	xb2_pm[1] = wirenr_xb2[j] - 118.5 - d_b;

	double dmin1 = 999;
	for (int k = 0; k < 2; k++) {
	  for (int m = 0; m < 2; m++) {
	    if (fabs(xa2_pm[k] - xb2_pm[m]) < dmin1) {
	      dmin1 = fabs(xa2_pm[k] - xb2_pm[m]);
	      x2_ab[0][pair_x2] = xa2_pm[k];
	      x2_ab[1][pair_x2] = xb2_pm[m];
	    }
	  }
	}
	if (d_a < 0.02) {
	  sigm_x2_ab[0][pair_x2] = 0.08 * 0.08;
	}
	if (d_a >= 0.02 && d_a < 0.1) {
	  sigm_x2_ab[0][pair_x2] = 0.06 * 0.06;
	}
	if (d_a >= 0.1 && d_a < 0.4) {
	  sigm_x2_ab[0][pair_x2] = 0.025 * 0.025;
	}
	if (d_a >= 0.4 && d_a < 0.41) {
	  sigm_x2_ab[0][pair_x2] = 0.08 * 0.08;
	}
	if (d_a >= 0.41) {
	  sigm_x2_ab[0][pair_x2] = 0.10 * 0.10;
	}

	if (d_b < 0.02) {
	  sigm_x2_ab[1][pair_x2] = 0.08 * 0.08;
	}
	if (d_b >= 0.02 && d_b < 0.1) {
	  sigm_x2_ab[1][pair_x2] = 0.06 * 0.06;
	}
	if (d_b >= 0.1 && d_b < 0.4) {
	  sigm_x2_ab[1][pair_x2] = 0.025 * 0.025;
	}
	if (d_b >= 0.4 && d_b < 0.41) {
	  sigm_x2_ab[1][pair_x2] = 0.08 * 0.08;
	}
	if (d_b >= 0.41) {
	  sigm_x2_ab[1][pair_x2] = 0.10 * 0.10;
	}
	pair_x2++;
	used_xa2[i] = true;
	used_xb2[j] = true;
      }// j for2
    } // i for1  X.

    //reconstruct single X-plane hits

    for (Int_t i = 0; i < it_xa2; ++i) {
      if (used_xa2[i]) continue;
      double d_a = 0;
      double d_b = 0;

      int func_nr_a = -1;
      for (int t_it = 0; t_it < 4; t_it++) {
	if (time_xa2[i] >= t_dc[t_it][8] && time_xa2[i] < t_dc[t_it + 1][8]) {
	  func_nr_a = t_it;
	  break;
	}
      }
      double time_xa = time_xa2[i];
      if (func_nr_a == 1 || func_nr_a == 2)d_a = scale[8]*(pol_par_dc[1][0][8] + pol_par_dc[1][1][8] * time_xa + pol_par_dc[1][2][8] * time_xa * time_xa + pol_par_dc[1][3][8] * time_xa * time_xa * time_xa + pol_par_dc[1][4][8] * time_xa * time_xa * time_xa * time_xa);
      if (func_nr_a == 0)d_a = 0;
      if (func_nr_a == 3)d_a = scale[8]*(pol_par_dc[2][0][8] + pol_par_dc[2][1][8] * time_xa + pol_par_dc[2][2][8] * time_xa * time_xa + pol_par_dc[2][3][8] * time_xa * time_xa * time_xa + pol_par_dc[2][4][8] * time_xa * time_xa * time_xa * time_xa);

      x2_single[0][single_xa2] = wirenr_xa2[i] - 118.5 + d_a;
      x2_single[0][single_xa2 + 1] = wirenr_xa2[i] - 118.5 - d_a;
      if (d_a < 0.02) {
	sigm_x2_single[0][single_xa2] = 0.08 * 0.08;
	sigm_x2_single[0][single_xa2 + 1] = 0.08 * 0.08;
      }
      if (d_a >= 0.02 && d_a < 0.1) {
	sigm_x2_single[0][single_xa2] = 0.06 * 0.06;
	sigm_x2_single[0][single_xa2 + 1] = 0.06 * 0.06;
      }
      if (d_a >= 0.1 && d_a < 0.4) {
	sigm_x2_single[0][single_xa2] = 0.0250 * 0.0250;
	sigm_x2_single[0][single_xa2 + 1] = 0.0250 * 0.0250;
      }
      if (d_a >= 0.4 && d_a < 0.41) {
	sigm_x2_single[0][single_xa2] = 0.08 * 0.08;
	sigm_x2_single[0][single_xa2 + 1] = 0.08 * 0.08;
      }
      if (d_a >= 0.41) {
	sigm_x2_single[0][single_xa2] = 0.10 * 0.10;
	sigm_x2_single[0][single_xa2 + 1] = 0.10 * 0.10;
      }
      single_xa2 += 2;
    }//for single xa

    for (Int_t j = 0; j < it_xb2; ++j) {
      if (used_xb2[j]) continue;
      int func_nr_b = -1;
      double d_a = 0;
      double d_b = 0;

      for (int t_it = 0; t_it < 4; t_it++) {
	if (time_xb2[j] >= t_dc[t_it][9] && time_xb2[j] < t_dc[t_it + 1][9]) {
	  func_nr_b = t_it;
	  break;
	}
      }
      double time_xa = time_xb2[j];
      if (func_nr_b == 1 || func_nr_b == 2)d_b = scale[9]*(pol_par_dc[1][0][9] + pol_par_dc[1][1][9] * time_xa + pol_par_dc[1][2][9] * time_xa * time_xa + pol_par_dc[1][3][9] * time_xa * time_xa * time_xa + pol_par_dc[1][4][9] * time_xa * time_xa * time_xa * time_xa);
      if (func_nr_b == 0)d_b = 0;
      if (func_nr_b == 3)d_b = scale[9]*(pol_par_dc[2][0][9] + pol_par_dc[2][1][9] * time_xa + pol_par_dc[2][2][9] * time_xa * time_xa + pol_par_dc[2][3][9] * time_xa * time_xa * time_xa + pol_par_dc[2][4][9] * time_xa * time_xa * time_xa * time_xa);

      x2_single[1][single_xb2] = wirenr_xb2[j] - 118.5 + d_b;
      x2_single[1][single_xb2 + 1] = wirenr_xb2[j] - 118.5 - d_b;
      if (d_b < 0.02) {
	sigm_x2_single[1][single_xb2] = 0.08 * 0.08;
	sigm_x2_single[1][single_xb2 + 1] = 0.08 * 0.08;
      }
      if (d_b >= 0.02 && d_b < 0.1) {
	sigm_x2_single[1][single_xb2] = 0.06 * 0.06;
	sigm_x2_single[1][single_xb2 + 1] = 0.06 * 0.06;
      }
      if (d_b >= 0.1 && d_b < 0.4) {
	sigm_x2_single[1][single_xb2] = 0.0250 * 0.0250;
	sigm_x2_single[1][single_xb2 + 1] = 0.0250 * 0.0250;
      }
      if (d_b >= 0.4 && d_b < 0.41) {
	sigm_x2_single[1][single_xb2] = 0.08 * 0.08;
	sigm_x2_single[1][single_xb2 + 1] = 0.08 * 0.08;
      }
      if (d_b >= 0.41) {
	sigm_x2_single[1][single_xb2] = 0.1 * 0.1;
	sigm_x2_single[1][single_xb2 + 1] = 0.1 * 0.1;
      }
      single_xb2 += 2;
    }//j xb2

    //------Y-------

    for (Int_t i = 0; i < it_ya2; ++i) {
      for (Int_t j = 0; j < it_yb2; ++j) {
	if (pair_y2 > 48)break;
	if ((wirenr_ya2[i] != wirenr_yb2[j] && wirenr_ya2[i] != wirenr_yb2[j] + 1)) continue;
	double d_a = 0;
	double d_b = 0;

	int func_nr_a = -1;
	int func_nr_b = -1;
	for (int t_it = 0; t_it < 4; t_it++) {
	  if (time_ya2[i] >= t_dc[t_it][10] && time_ya2[i] < t_dc[t_it + 1][10]) {
	    func_nr_a = t_it;
	    break;
	  }
	}
	if (func_nr_a == 1 || func_nr_a == 2) d_a = scale[10]*(pol_par_dc[1][0][10] + pol_par_dc[1][1][10] * time_ya2[i] + pol_par_dc[1][2][10] * time_ya2[i] * time_ya2[i] + pol_par_dc[1][3][10] * time_ya2[i] * time_ya2[i] * time_ya2[i] + pol_par_dc[1][4][10] * time_ya2[i] * time_ya2[i] * time_ya2[i] * time_ya2[i]);

	if (func_nr_a == 0)d_a = 0;
	if (func_nr_a == 3)d_a = scale[10]*(pol_par_dc[2][0][10] + pol_par_dc[2][1][10] * time_ya2[i] + pol_par_dc[2][2][10] * time_ya2[i] * time_ya2[i] + pol_par_dc[2][3][10] * time_ya2[i] * time_ya2[i] * time_ya2[i] + pol_par_dc[2][4][10] * time_ya2[i] * time_ya2[i] * time_ya2[i] * time_ya2[i]);

	for (int t_it = 0; t_it < 4; t_it++) {
	  if (time_yb2[j] >= t_dc[t_it][11] && time_yb2[j] < t_dc[t_it + 1][11]) {
	    func_nr_b = t_it;
	    break;
	  }
	}
	if (func_nr_b == 1 || func_nr_b == 2) d_b = scale[11]*(pol_par_dc[1][0][11] + pol_par_dc[1][1][11] * time_yb2[j] + pol_par_dc[1][2][11] * time_yb2[j] * time_yb2[j] + pol_par_dc[1][3][11] * time_yb2[j] * time_yb2[j] * time_yb2[j] + pol_par_dc[1][4][11] * time_yb2[j] * time_yb2[j] * time_yb2[j] * time_yb2[j]);
	if (func_nr_b == 0)d_b = 0;
	if (func_nr_b == 3)d_b = scale[11]*(pol_par_dc[2][0][11] + pol_par_dc[2][1][11] * time_yb2[j] + pol_par_dc[2][2][11] * time_yb2[j] * time_yb2[j] + pol_par_dc[2][3][11] * time_yb2[j] * time_yb2[j] * time_yb2[j] + pol_par_dc[2][4][11] * time_yb2[j] * time_yb2[j] * time_yb2[j] * time_yb2[j]);

	ya2_pm[0] = wirenr_ya2[i] - 119 + d_a;
	ya2_pm[1] = wirenr_ya2[i] - 119 - d_a;
	yb2_pm[0] = wirenr_yb2[j] - 118.5 + d_b;
	yb2_pm[1] = wirenr_yb2[j] - 118.5 - d_b;


	double dmin1 = 999;
	for (int k = 0; k < 2; k++) {
	  for (int m = 0; m < 2; m++) {
	    if (fabs(ya2_pm[k] - yb2_pm[m]) < dmin1) {
	      dmin1 = fabs(ya2_pm[k] - yb2_pm[m]);
	      y2_ab[0][pair_y2] = ya2_pm[k];
	      y2_ab[1][pair_y2] = yb2_pm[m];
	    }
	  }
	}
	if (d_a < 0.02) {
	  sigm_y2_ab[0][pair_y2] = 0.08 * 0.08;
	}
	if (d_a >= 0.02 && d_a < 0.1) {
	  sigm_y2_ab[0][pair_y2] = 0.06 * 0.06;
	}
	if (d_a >= 0.1 && d_a < 0.4) {
	  sigm_y2_ab[0][pair_y2] = 0.025 * 0.025;
	}
	if (d_a >= 0.4 && d_a < 0.41) {
	  sigm_y2_ab[0][pair_y2] = 0.08 * 0.08;
	}
	if (d_a >= 0.41) {
	  sigm_y2_ab[0][pair_y2] = 0.10 * 0.10;
	}

	if (d_b < 0.02) {
	  sigm_y2_ab[1][pair_y2] = 0.08 * 0.08;
	}
	if (d_b >= 0.02 && d_b < 0.1) {
	  sigm_y2_ab[1][pair_y2] = 0.06 * 0.06;
	}
	if (d_b >= 0.1 && d_b < 0.4) {
	  sigm_y2_ab[1][pair_y2] = 0.025 * 0.025;
	}
	if (d_b >= 0.4 && d_b < 0.41) {
	  sigm_y2_ab[1][pair_y2] = 0.08 * 0.08;
	}
	if (d_b >= 0.41) {
	  sigm_y2_ab[1][pair_y2] = 0.10 * 0.10;
	}
	pair_y2++;
	used_ya2[i] = true;
	used_yb2[j] = true;

      }// j for2
    } // i for1  Y.

    //reconstruct single Y-plane hits

    for (Int_t i = 0; i < it_ya2; ++i) {
      if (used_ya2[i]) continue;
      double d_a = 0;
      double d_b = 0;

      int func_nr_a = -1;
      for (int t_it = 0; t_it < 4; t_it++) {
	if (time_ya2[i] >= t_dc[t_it][10] && time_ya2[i] < t_dc[t_it + 1][10]) {
	  func_nr_a = t_it;
	  break;
	}
      }
      if (func_nr_a == 1 || func_nr_a == 2) d_a = scale[10]*(pol_par_dc[1][0][10] + pol_par_dc[1][1][10] * time_ya2[i] + pol_par_dc[1][2][10] * time_ya2[i] * time_ya2[i] + pol_par_dc[1][3][10] * time_ya2[i] * time_ya2[i] * time_ya2[i] + pol_par_dc[1][4][10] * time_ya2[i] * time_ya2[i] * time_ya2[i] * time_ya2[i]);

      if (func_nr_a == 0)d_a = 0;
      if (func_nr_a == 3)d_a = scale[10]*(pol_par_dc[2][0][10] + pol_par_dc[2][1][10] * time_ya2[i] + pol_par_dc[2][2][10] * time_ya2[i] * time_ya2[i] + pol_par_dc[2][3][10] * time_ya2[i] * time_ya2[i] * time_ya2[i] + pol_par_dc[2][4][10] * time_ya2[i] * time_ya2[i] * time_ya2[i] * time_ya2[i]);

      y2_single[0][single_ya2] = wirenr_ya2[i] - 119 + d_a;
      y2_single[0][single_ya2 + 1] = wirenr_ya2[i] - 119 - d_a;
      if (d_a < 0.02) {
	sigm_y2_single[0][single_ya2] = 0.08 * 0.08;
	sigm_y2_single[0][single_ya2 + 1] = 0.08 * 0.08;
      }
      if (d_a >= 0.02 && d_a < 0.1) {
	sigm_y2_single[0][single_ya2] = 0.06 * 0.06;
	sigm_y2_single[0][single_ya2 + 1] = 0.06 * 0.06;
      }
      if (d_a >= 0.1 && d_a < 0.4) {
	sigm_y2_single[0][single_ya2] = 0.025 * 0.025;
	sigm_y2_single[0][single_ya2 + 1] = 0.025 * 0.025;
      }
      if (d_a >= 0.4 && d_a < 0.41) {
	sigm_y2_single[0][single_ya2] = 0.08 * 0.08;
	sigm_y2_single[0][single_ya2 + 1] = 0.08 * 0.08;
      }
      if (d_a >= 0.41) {
	sigm_y2_single[0][single_ya2] = 0.10 * 0.10;
	sigm_y2_single[0][single_ya2 + 1] = 0.10 * 0.10;
      }
      single_ya2 += 2;
    }//for single ya

    for (Int_t j = 0; j < it_yb2; ++j) {
      if (used_yb2[j]) continue;
      double d_a = 0;
      double d_b = 0;
      int func_nr_b = -1;
      for (int t_it = 0; t_it < 4; t_it++) {
	if (time_yb2[j] >= t_dc[t_it][11] && time_yb2[j] < t_dc[t_it + 1][11]) {
	  func_nr_b = t_it;
	  break;
	}
      }
      if (func_nr_b == 1 || func_nr_b == 2) d_b = scale[11]*(pol_par_dc[1][0][11] + pol_par_dc[1][1][11] * time_yb2[j] + pol_par_dc[1][2][11] * time_yb2[j] * time_yb2[j] + pol_par_dc[1][3][11] * time_yb2[j] * time_yb2[j] * time_yb2[j] + pol_par_dc[1][4][11] * time_yb2[j] * time_yb2[j] * time_yb2[j] * time_yb2[j]);
      if (func_nr_b == 0)d_b = 0;
      if (func_nr_b == 3)d_b = scale[11]*(pol_par_dc[2][0][11] + pol_par_dc[2][1][11] * time_yb2[j] + pol_par_dc[2][2][11] * time_yb2[j] * time_yb2[j] + pol_par_dc[2][3][11] * time_yb2[j] * time_yb2[j] * time_yb2[j] + pol_par_dc[2][4][11] * time_yb2[j] * time_yb2[j] * time_yb2[j] * time_yb2[j]);

      y2_single[1][single_yb2] = wirenr_yb2[j] - 118.5 + d_b;
      y2_single[1][single_yb2 + 1] = wirenr_yb2[j] - 118.5 - d_b;
      if (d_b < 0.02) {
	sigm_y2_single[1][single_yb2] = 0.08 * 0.08;
	sigm_y2_single[1][single_yb2 + 1] = 0.08 * 0.08;
      }
      if (d_b >= 0.02 && d_b < 0.1) {
	sigm_y2_single[1][single_yb2] = 0.06 * 0.06;
	sigm_y2_single[1][single_yb2 + 1] = 0.06 * 0.06;
      }
      if (d_b >= 0.1 && d_b < 0.4) {
	sigm_y2_single[1][single_yb2] = 0.0250 * 0.0250;
	sigm_y2_single[1][single_yb2 + 1] = 0.0250 * 0.0250;
      }
      if (d_b >= 0.4 && d_b < 0.41) {
	sigm_y2_single[1][single_yb2] = 0.1 * 0.1;
	sigm_y2_single[1][single_yb2 + 1] = 0.1 * 0.1;
      }
      if (d_b >= 0.41) {
	sigm_y2_single[1][single_yb2] = 0.10 * 0.10;
	sigm_y2_single[1][single_yb2 + 1] = 0.10 * 0.10;
      }
      single_yb2 += 2;

    } // i for1  Y.


    //   ----   U   ---

    for (Int_t i = 0; i < it_ua2; ++i) {
      for (Int_t j = 0; j < it_ub2; ++j) {
	if (pair_u2 > 48)break;
	if ((wirenr_ua2[i] != wirenr_ub2[j] && wirenr_ua2[i] != wirenr_ub2[j] + 1)) continue;

	double d_a = 0;
	double d_b = 0;

	int func_nr_a = -1;
	int func_nr_b = -1;
	for (int t_it = 0; t_it < 4; t_it++) {
	  if (time_ua2[i] >= t_dc[t_it][12] && time_ua2[i] < t_dc[t_it + 1][12]) {
	    func_nr_a = t_it;
	    break;
	  }
	}
	if (func_nr_a == 1 || func_nr_a == 2) d_a = scale[12]*(pol_par_dc[1][0][12] + pol_par_dc[1][1][12] * time_ua2[i] + pol_par_dc[1][2][12] * time_ua2[i] * time_ua2[i] + pol_par_dc[1][3][12] * time_ua2[i] * time_ua2[i] * time_ua2[i] + pol_par_dc[1][4][12] * time_ua2[i] * time_ua2[i] * time_ua2[i] * time_ua2[i]);

	if (func_nr_a == 0)d_a = 0;
	if (func_nr_a == 3)d_a = scale[12]*(pol_par_dc[1][0][12] + pol_par_dc[1][1][12] * time_ua2[i] + pol_par_dc[1][2][12] * time_ua2[i] * time_ua2[i] + pol_par_dc[1][3][12] * time_ua2[i] * time_ua2[i] * time_ua2[i] + pol_par_dc[1][4][12] * time_ua2[i] * time_ua2[i] * time_ua2[i] * time_ua2[i]);

	for (int t_it = 0; t_it < 4; t_it++) {
	  if (time_ub2[j] >= t_dc[t_it][13] && time_ub2[j] < t_dc[t_it + 1][13]) {
	    func_nr_b = t_it;
	    break;
	  }
	}

	if (func_nr_b == 0)d_b = 0;

	if (func_nr_b == 1 || func_nr_b == 2) d_b = scale[13]*(pol_par_dc[1][0][13] + pol_par_dc[1][1][13] * time_ub2[j] + pol_par_dc[1][2][13] * time_ub2[j] * time_ub2[j] + pol_par_dc[1][3][13] * time_ub2[j] * time_ub2[j] * time_ub2[j] + pol_par_dc[1][4][13] * time_ub2[j] * time_ub2[j] * time_ub2[j] * time_ub2[j]);

	if (func_nr_b == 3)d_b = scale[13]*(pol_par_dc[1][0][13] + pol_par_dc[1][1][13] * time_ub2[j] + pol_par_dc[1][2][13] * time_ub2[j] * time_ub2[j] + pol_par_dc[1][3][13] * time_ub2[j] * time_ub2[j] * time_ub2[j] + pol_par_dc[1][4][13] * time_ub2[j] * time_ub2[j] * time_ub2[j] * time_ub2[j]);

	ua2_pm[0] = wirenr_ua2[i] - 119 + d_a;
	ua2_pm[1] = wirenr_ua2[i] - 119 - d_a;
	ub2_pm[0] = wirenr_ub2[j] - 118.5 + d_b;
	ub2_pm[1] = wirenr_ub2[j] - 118.5 - d_b;

	double dmin1 = 999;
	for (int k = 0; k < 2; k++) {
	  for (int m = 0; m < 2; m++) {
	    if (fabs(ua2_pm[k] - ub2_pm[m]) < dmin1) {
	      dmin1 = fabs(ua2_pm[k] - ub2_pm[m]);
	      u2_ab[0][pair_u2] = ua2_pm[k];
	      u2_ab[1][pair_u2] = ub2_pm[m];
	    }
	  }
	}
	if (d_a < 0.02) {
	  sigm_u2_ab[0][pair_u2] = 0.08 * 0.08;
	}
	if (d_a >= 0.02 && d_a < 0.1) {
	  sigm_u2_ab[0][pair_u2] = 0.06 * 0.06;
	}
	if (d_a >= 0.1 && d_a < 0.4) {
	  sigm_u2_ab[0][pair_u2] = 0.025 * 0.025;
	}
	if (d_a >= 0.4 && d_a < 0.41) {
	  sigm_u2_ab[0][pair_u2] = 0.08 * 0.08;
	}
	if (d_a >= 0.41) {
	  sigm_u2_ab[0][pair_u2] = 0.10 * 0.10;
	}

	if (d_b < 0.02) {
	  sigm_u2_ab[1][pair_u2] = 0.08 * 0.08;
	}
	if (d_b >= 0.02 && d_b < 0.1) {
	  sigm_u2_ab[1][pair_u2] = 0.06 * 0.06;
	}
	if (d_b >= 0.1 && d_b < 0.4) {
	  sigm_u2_ab[1][pair_u2] = 0.025 * 0.025;
	}
	if (d_b >= 0.4 && d_b < 0.41) {
	  sigm_u2_ab[1][pair_u2] = 0.08 * 0.08;
	}
	if (d_b >= 0.41) {
	  sigm_u2_ab[1][pair_u2] = 0.10 * 0.10;
	}
	pair_u2++;
	used_ua2[i] = true;
	used_ub2[j] = true;

      }// j for2
    } // i for1  U.

    //reconstruct single U-plane hits

    for (Int_t i = 0; i < it_ua2; ++i) {
      if (used_ua2[i]) continue;
      double d_a = 0;
      double d_b = 0;

      int func_nr_a = -1;
      for (int t_it = 0; t_it < 4; t_it++) {
	if (time_ua2[i] >= t_dc[t_it][12] && time_ua2[i] < t_dc[t_it + 1][12]) {
	  func_nr_a = t_it;
	  break;
	}
      }
      if (func_nr_a == 1 || func_nr_a == 2) d_a = scale[12]*(pol_par_dc[1][0][12] + pol_par_dc[1][1][12] * time_ua2[i] + pol_par_dc[1][2][12] * time_ua2[i] * time_ua2[i] + pol_par_dc[1][3][12] * time_ua2[i] * time_ua2[i] * time_ua2[i] + pol_par_dc[1][4][12] * time_ua2[i] * time_ua2[i] * time_ua2[i] * time_ua2[i]);
      if (func_nr_a == 0)d_a = 0;
      if (func_nr_a == 3)d_a = scale[12]*(pol_par_dc[1][0][12] + pol_par_dc[1][1][12] * time_ua2[i] + pol_par_dc[1][2][12] * time_ua2[i] * time_ua2[i] + pol_par_dc[1][3][12] * time_ua2[i] * time_ua2[i] * time_ua2[i] + pol_par_dc[1][4][12] * time_ua2[i] * time_ua2[i] * time_ua2[i] * time_ua2[i]);

      u2_single[0][single_ua2] = (wirenr_ua2[i] - 119 + d_a);
      u2_single[0][single_ua2 + 1] = (wirenr_ua2[i] - 119 - d_a);
      if (d_a < 0.02) {
	sigm_u2_single[0][single_ua2] = 0.08 * 0.08;
	sigm_u2_single[0][single_ua2 + 1] = 0.08 * 0.08;
      }
      if (d_a >= 0.02 && d_a < 0.1) {
	sigm_u2_single[0][single_ua2] = 0.06 * 0.06;
	sigm_u2_single[0][single_ua2 + 1] = 0.06 * 0.06;
      }
      if (d_a >= 0.1 && d_a < 0.4) {
	sigm_u2_single[0][single_ua2] = 0.025 * 0.025;
	sigm_u2_single[0][single_ua2 + 1] = 0.025 * 0.025;
      }
      if (d_a >= 0.4 && d_a < 0.41) {
	sigm_u2_single[0][single_ua2] = 0.08 * 0.08;
	sigm_u2_single[0][single_ua2 + 1] = 0.08 * 0.08;
      }
      if (d_a >= 0.41) {
	sigm_u2_single[0][single_ua2] = 0.10 * 0.10;
	sigm_u2_single[0][single_ua2 + 1] = 0.10 * 0.10;
      }
      single_ua2 += 2;
    }//for single ua

    for (Int_t j = 0; j < it_ub2; ++j) {
      if (used_ub2[j]) continue;
      int func_nr_b = -1;
      double d_a = 0;
      double d_b = 0;

      for (int t_it = 0; t_it < 4; t_it++) {
	if (time_ub2[j] >= t_dc[t_it][13] && time_ub2[j] < t_dc[t_it + 1][13]) {
	  func_nr_b = t_it;
	  break;
	}
      }
      if (func_nr_b == 0)d_b = 0;

      if (func_nr_b == 1 || func_nr_b == 2) d_b = scale[13]*(pol_par_dc[1][0][13] + pol_par_dc[1][1][13] * time_ub2[j] + pol_par_dc[1][2][13] * time_ub2[j] * time_ub2[j] + pol_par_dc[1][3][13] * time_ub2[j] * time_ub2[j] * time_ub2[j] + pol_par_dc[1][4][13] * time_ub2[j] * time_ub2[j] * time_ub2[j] * time_ub2[j]);
      if (func_nr_b == 3)d_b = scale[13]*(pol_par_dc[1][0][13] + pol_par_dc[1][1][13] * time_ub2[j] + pol_par_dc[1][2][13] * time_ub2[j] * time_ub2[j] + pol_par_dc[1][3][13] * time_ub2[j] * time_ub2[j] * time_ub2[j] + pol_par_dc[1][4][13] * time_ub2[j] * time_ub2[j] * time_ub2[j] * time_ub2[j]);

      u2_single[1][single_ub2] = (wirenr_ub2[j] - 118.5 + d_b);
      u2_single[1][single_ub2 + 1] = (wirenr_ub2[j] - 118.5 - d_b);
      if (d_b < 0.02) {
	sigm_u2_single[1][single_ub2] = 0.08 * 0.08;
	sigm_u2_single[1][single_ub2 + 1] = 0.08 * 0.08;
      }
      if (d_b >= 0.02 && d_b < 0.1) {
	sigm_u2_single[1][single_ub2] = 0.060 * 0.060;
	sigm_u2_single[1][single_ub2 + 1] = 0.060 * 0.060;
      }
      if (d_b >= 0.1 && d_b < 0.4) {
	sigm_u2_single[1][single_ub2] = 0.025 * 0.025;
	sigm_u2_single[1][single_ub2 + 1] = 0.0250 * 0.0250;
      }
      if (d_b >= 0.4 && d_b < 0.41) {
	sigm_u2_single[1][single_ub2] = 0.08 * 0.08;
	sigm_u2_single[1][single_ub2 + 1] = 0.08 * 0.08;
      }
      if (d_b >= 0.41) {
	sigm_u2_single[1][single_ub2] = 0.10 * 0.10;
	sigm_u2_single[1][single_ub2 + 1] = 0.10 * 0.10;
      }
      single_ub2 += 2;

    } // i for1  U.

    //   ----   V   ---

    for (Int_t i = 0; i < it_va2; ++i) {
      for (Int_t j = 0; j < it_vb2; ++j) {
	if (pair_v2 > 48)break;
	if ((wirenr_va2[i] != wirenr_vb2[j] && wirenr_va2[i] != wirenr_vb2[j] + 1))continue;

	double d_a = 0;
	double d_b = 0;

	int func_nr_a = -1;
	int func_nr_b = -1;
	for (int t_it = 0; t_it < 4; t_it++) {
	  if (time_va2[i] >= t_dc[t_it][14] && time_va2[i] < t_dc[t_it + 1][14]) {
	    func_nr_a = t_it;
	    break;
	  }
	}
	if (func_nr_a == 1 || func_nr_a == 2)d_a = scale[14]*(pol_par_dc[1][0][14] + pol_par_dc[1][1][14] * time_va2[i] + pol_par_dc[1][2][14] * time_va2[i] * time_va2[i] + pol_par_dc[1][3][14] * time_va2[i] * time_va2[i] * time_va2[i] + pol_par_dc[1][4][14] * time_va2[i] * time_va2[i] * time_va2[i] * time_va2[i]);

	if (func_nr_a == 0)d_a = 0;
	if (func_nr_a == 3)d_a = scale[14]*(pol_par_dc[2][0][14] + pol_par_dc[2][1][14] * time_va2[i] + pol_par_dc[2][2][14] * time_va2[i] * time_va2[i] + pol_par_dc[2][3][14] * time_va2[i] * time_va2[i] * time_va2[i] + pol_par_dc[2][4][14] * time_va2[i] * time_va2[i] * time_va2[i] * time_va2[i]);

	for (int t_it = 0; t_it < 4; t_it++) {
	  if (time_vb2[j] >= t_dc[t_it][15] && time_vb2[j] < t_dc[t_it + 1][15]) {
	    func_nr_b = t_it;
	    break;
	  }
	}
	if (func_nr_b == 1 || func_nr_b == 2) d_b = scale[15]*(pol_par_dc[1][0][15] + pol_par_dc[1][1][15] * time_vb2[j] + pol_par_dc[1][2][15] * time_vb2[j] * time_vb2[j] + pol_par_dc[1][3][15] * time_vb2[j] * time_vb2[j] * time_vb2[j] + pol_par_dc[1][4][15] * time_vb2[j] * time_vb2[j] * time_vb2[j] * time_vb2[j]);

	if (func_nr_b == 0)d_b = 0;
	if (func_nr_b == 3)d_b = scale[15]*(pol_par_dc[2][0][15] + pol_par_dc[2][1][15] * time_vb2[j] + pol_par_dc[2][2][15] * time_vb2[j] * time_vb2[j] + pol_par_dc[2][3][15] * time_vb2[j] * time_vb2[j] * time_vb2[j] + pol_par_dc[2][4][15] * time_vb2[j] * time_vb2[j] * time_vb2[j] * time_vb2[j]);

	va2_pm[0] = wirenr_va2[i] - 119 + d_a;
	va2_pm[1] = wirenr_va2[i] - 119 - d_a;
	vb2_pm[0] = wirenr_vb2[j] - 118.5 + d_b;
	vb2_pm[1] = wirenr_vb2[j] - 118.5 - d_b;

	double dmin1 = 999;
	for (int k = 0; k < 2; k++) {
	  for (int m = 0; m < 2; m++) {
	    if (fabs(va2_pm[k] - vb2_pm[m]) < dmin1) {
	      dmin1 = fabs(va2_pm[k] - vb2_pm[m]);
	      v2_ab[0][pair_v2] = va2_pm[k];
	      v2_ab[1][pair_v2] = vb2_pm[m];
	    }
	  }
	}
	if (d_a < 0.02) {
	  sigm_v2_ab[0][pair_v2] = 0.08 * 0.08;
	}
	if (d_a >= 0.02 && d_a < 0.1) {
	  sigm_v2_ab[0][pair_v2] = 0.06 * 0.06;
	}
	if (d_a >= 0.1 && d_a < 0.4) {
	  sigm_v2_ab[0][pair_v2] = 0.025 * 0.025;
	}
	if (d_a >= 0.4 && d_a < 0.41) {
	  sigm_v2_ab[0][pair_v2] = 0.08 * 0.08;
	}
	if (d_a >= 0.41) {
	  sigm_v2_ab[0][pair_v2] = 0.10 * 0.10;
	}

	if (d_b < 0.02) {
	  sigm_v2_ab[1][pair_v2] = 0.08 * 0.08;
	}
	if (d_b >= 0.02 && d_b < 0.1) {
	  sigm_v2_ab[1][pair_v2] = 0.06 * 0.06;
	}
	if (d_b >= 0.1 && d_b < 0.4) {
	  sigm_v2_ab[1][pair_v2] = 0.025 * 0.025;
	}
	if (d_b >= 0.4 && d_b < 0.41) {
	  sigm_v2_ab[1][pair_v2] = 0.08 * 0.08;
	}
	if (d_b >= 0.41) {
	  sigm_v2_ab[1][pair_v2] = 0.10 * 0.10;
	}
	pair_v2++;

	used_va2[i] = true;
	used_vb2[j] = true;


      }// j for2
    } // i for1  V.

    //reconstruct single V-plane hits

    for (Int_t i = 0; i < it_va2; ++i) {
      if (used_va2[i]) continue;
      double d_a = 0;
      double d_b = 0;

      //added
      int func_nr_a = -1;
      for (int t_it = 0; t_it < 4; t_it++) {
	if (time_va2[i] >= t_dc[t_it][14] && time_va2[i] < t_dc[t_it + 1][14]) {
	  func_nr_a = t_it;
	  break;
	}
      }
      if (func_nr_a == 1 || func_nr_a == 2)d_a = scale[14]*(pol_par_dc[1][0][14] + pol_par_dc[1][1][14] * time_va2[i] + pol_par_dc[1][2][14] * time_va2[i] * time_va2[i] + pol_par_dc[1][3][14] * time_va2[i] * time_va2[i] * time_va2[i] + pol_par_dc[1][4][14] * time_va2[i] * time_va2[i] * time_va2[i] * time_va2[i]);
      if (func_nr_a == 0)d_a = 0;
      if (func_nr_a == 3)d_a = scale[14]*(pol_par_dc[2][0][14] + pol_par_dc[2][1][14] * time_va2[i] + pol_par_dc[2][2][14] * time_va2[i] * time_va2[i] + pol_par_dc[2][3][14] * time_va2[i] * time_va2[i] * time_va2[i] + pol_par_dc[2][4][14] * time_va2[i] * time_va2[i] * time_va2[i] * time_va2[i]);

      v2_single[0][single_va2] = wirenr_va2[i] - 119 + d_a;
      v2_single[0][single_va2 + 1] = wirenr_va2[i] - 119 - d_a;
      if (d_a < 0.02) {
	sigm_v2_single[0][single_va2] = 0.08 * 0.08;
	sigm_v2_single[0][single_va2 + 1] = 0.08 * 0.08;
      }
      if (d_a >= 0.02 && d_a < 0.1) {
	sigm_v2_single[0][single_va2] = 0.06 * 0.06;
	sigm_v2_single[0][single_va2 + 1] = 0.06 * 0.06;
      }
      if (d_a >= 0.1 && d_a < 0.4) {
	sigm_v2_single[0][single_va2] = 0.025 * 0.025;
	sigm_v2_single[0][single_va2 + 1] = 0.025 * 0.025;
      }
      if (d_a >= 0.4 && d_a < 0.41) {
	sigm_v2_single[0][single_va2] = 0.08 * 0.08;
	sigm_v2_single[0][single_va2 + 1] = 0.08 * 0.08;
      }
      if (d_a >= 0.41) {
	sigm_v2_single[0][single_va2] = 0.10 * 0.10;
	sigm_v2_single[0][single_va2 + 1] = 0.10 * 0.10;
      }

      single_va2 += 2;
    }//for single va

    for (Int_t j = 0; j < it_vb2; ++j) {
      if (used_vb2[j]) continue;
      double d_a = 0;
      double d_b = 0;
      int func_nr_b = -1;
      for (int t_it = 0; t_it < 4; t_it++) {
	if (time_vb2[j] >= t_dc[t_it][15] && time_vb2[j] < t_dc[t_it + 1][15]) {
	  func_nr_b = t_it;
	  break;
	}
      }
      if (func_nr_b == 1 || func_nr_b == 2) d_b = scale[15]*(pol_par_dc[1][0][15] + pol_par_dc[1][1][15] * time_vb2[j] + pol_par_dc[1][2][15] * time_vb2[j] * time_vb2[j] + pol_par_dc[1][3][15] * time_vb2[j] * time_vb2[j] * time_vb2[j] + pol_par_dc[1][4][15] * time_vb2[j] * time_vb2[j] * time_vb2[j] * time_vb2[j]);
      if (func_nr_b == 0)d_b = 0;
      if (func_nr_b == 3)d_b = scale[15]*(pol_par_dc[2][0][15] + pol_par_dc[2][1][15] * time_vb2[j] + pol_par_dc[2][2][15] * time_vb2[j] * time_vb2[j] + pol_par_dc[2][3][15] * time_vb2[j] * time_vb2[j] * time_vb2[j] + pol_par_dc[2][4][15] * time_vb2[j] * time_vb2[j] * time_vb2[j] * time_vb2[j]);

      v2_single[1][single_vb2] = wirenr_vb2[j] - 118.5 + d_b;
      v2_single[1][single_vb2 + 1] = wirenr_vb2[j] - 118.5 - d_b;
      if (d_b < 0.02) {
	sigm_v2_single[1][single_vb2] = 0.08 * 0.08;
	sigm_v2_single[1][single_vb2 + 1] = 0.08 * 0.08;
      }
      if (d_b >= 0.02 && d_b < 0.1) {
	sigm_v2_single[1][single_vb2] = 0.060 * 0.060;
	sigm_v2_single[1][single_vb2 + 1] = 0.060 * 0.060;
      }
      if (d_b >= 0.1 && d_b < 0.4) {
	sigm_v2_single[1][single_vb2] = 0.0250 * 0.0250;
	sigm_v2_single[1][single_vb2 + 1] = 0.0250 * 0.0250;
      }
      if (d_b >= 0.4 && d_b < 0.41) {
	sigm_v2_single[1][single_vb2] = 0.08 * 0.08;
	sigm_v2_single[1][single_vb2 + 1] = 0.08 * 0.08;
      }
      if (d_b >= 0.41) {
	sigm_v2_single[1][single_vb2] = 0.10 * 0.10;
	sigm_v2_single[1][single_vb2 + 1] = 0.10 * 0.10;
      }
      single_vb2 += 2;
    } // i for1  V.

    //  ---  DC2 Segment Building  ------

    found_8p_seg = false;
    x_slope = 0.0;
    y_slope = 0.0;

    if (pair_x2 * pair_y2 > 0) { //(x,y)first

      for (int i = 0; i < pair_x2; i++) {
	if (nDC2_segments > 48)break;
	float x_coord = (x2_ab[0][i] + x2_ab[1][i]) / 2;
	float XU = x_coord + x_slope * 0.5 * (z_loc[5] + z_loc[4] - z_loc[1] - z_loc[0]);
	float XV = x_coord + x_slope * 0.5 * (z_loc[7] + z_loc[6] - z_loc[1] - z_loc[0]);

	for (int j = 0; j < pair_y2; j++) {
	  if (nDC2_segments > 48)break;
	  float y_coord = (y2_ab[0][j] + y2_ab[1][j]) / 2;
	  float YU = y_coord + y_slope * 0.5 * (z_loc[5] + z_loc[4] - z_loc[3] - z_loc[2]);
	  float YV = y_coord + y_slope * 0.5 * (z_loc[7] + z_loc[6] - z_loc[3] - z_loc[2]);
	  bool foundU = false;
	  float u_est = isqrt_2 * (YU - XU);
	  float v_est = isqrt_2 * (YV + XV);
	  if (pair_u2 > 0) {
                    	
	    double dU_thresh = 1.3;
	    for (int k = 0; k < pair_u2; k++) {
	      float u_coord = (u2_ab[0][k] + u2_ab[1][k]) / 2;

	      if (fabs(u_coord - u_est) > dU_thresh)continue;
	      dU_thresh = fabs(u_coord - u_est);

	      rh_segDC2[0][nDC2_segments] = x2_ab[0][i];
	      rh_segDC2[1][nDC2_segments] = x2_ab[1][i];
	      rh_segDC2[2][nDC2_segments] = y2_ab[0][j];
	      rh_segDC2[3][nDC2_segments] = y2_ab[1][j];
	      rh_segDC2[4][nDC2_segments] = u2_ab[0][k];
	      rh_segDC2[5][nDC2_segments] = u2_ab[1][k];
	      rh_sigm_segDC2[0][nDC2_segments] = sigm_x2_ab[0][i];
	      rh_sigm_segDC2[1][nDC2_segments] = sigm_x2_ab[1][i];
	      rh_sigm_segDC2[2][nDC2_segments] = sigm_y2_ab[0][j];
	      rh_sigm_segDC2[3][nDC2_segments] = sigm_y2_ab[1][j];
	      rh_sigm_segDC2[4][nDC2_segments] = sigm_u2_ab[0][k];
	      rh_sigm_segDC2[5][nDC2_segments] = sigm_u2_ab[1][k];

	      foundU = true;
	      if (nDC2_segments > 48)break;
	    }//k
	  }//(pair_u2>0)
	  if (found_8p_seg && !foundU)continue;
	  bool foundV = false;
	  if (pair_v2 > 0) {
	    double dV_thresh = 1.3;
	    for (int m = 0; m < pair_v2; m++) {
	      if (nDC2_segments > 48)break;
	      float v_coord = (v2_ab[0][m] + v2_ab[1][m]) / 2;

	      if (fabs(v_coord - v_est) > dV_thresh)continue;
	      dV_thresh = fabs(v_coord - v_est);

	      foundV = true;
	      rh_segDC2[0][nDC2_segments] = x2_ab[0][i];
	      rh_segDC2[1][nDC2_segments] = x2_ab[1][i];
	      rh_segDC2[2][nDC2_segments] = y2_ab[0][j];
	      rh_segDC2[3][nDC2_segments] = y2_ab[1][j];
	      rh_segDC2[6][nDC2_segments] = v2_ab[0][m];
	      rh_segDC2[7][nDC2_segments] = v2_ab[1][m];
	      rh_sigm_segDC2[0][nDC2_segments] = sigm_x2_ab[0][i];
	      rh_sigm_segDC2[1][nDC2_segments] = sigm_x2_ab[1][i];
	      rh_sigm_segDC2[2][nDC2_segments] = sigm_y2_ab[0][j];
	      rh_sigm_segDC2[3][nDC2_segments] = sigm_y2_ab[1][j];
	      rh_sigm_segDC2[6][nDC2_segments] = sigm_v2_ab[0][m];
	      rh_sigm_segDC2[7][nDC2_segments] = sigm_v2_ab[1][m];
	      if (!foundU) {
		float min_a = 999;
		float min_b = 999;
		for (int kk = 0; kk < single_ua2; kk++) {
		  if (fabs(u2_single[0][kk] - u_est) > 1.5)continue; //????? 0.5 needs to be reviewed
		  if (fabs(u2_single[0][kk] - u_est) < min_a) {
		    min_a = fabs(u2_single[0][kk] - u_est);
		    rh_segDC2[4][nDC2_segments] = u2_single[0][kk];
		    rh_sigm_segDC2[4][nDC2_segments] = sigm_u2_single[0][kk];
		    foundU = true;
		  }
		}//for kk
		for (int kk = 0; kk < single_ub2; kk++) {
		  if (fabs(u2_single[1][kk] - u_est) > 1.5)continue; //????? 0.5 needs to be reviewed
		  if (fabs(u2_single[1][kk] - u_est) < min_b) {
		    min_b = fabs(u2_single[1][kk] - u_est);
		    rh_segDC2[5][nDC2_segments] = u2_single[1][kk];
		    rh_sigm_segDC2[5][nDC2_segments] = sigm_u2_single[1][kk];
		    foundU = true;
		  }
		}//for kk
		if (nDC2_segments > 48)break;
	      }//!foundU

	      if (nDC2_segments > 48)break;
	    }//m
	  }//(pair_v2>0)
	  if (!foundV && foundU) {
	    float min_a = 999;
	    float min_b = 999;
	    for (int kk = 0; kk < single_va2; kk++) {
	      if (fabs(v2_single[0][kk] - v_est) > 1.5)continue; //????? 0.5 needs to be reviewed                                                                                     
	      if (fabs(v2_single[0][kk] - v_est) < min_a) {
		min_a = fabs(v2_single[0][kk] - v_est);
		rh_segDC2[6][nDC2_segments] = v2_single[0][kk];
		rh_sigm_segDC2[6][nDC2_segments] = sigm_v2_single[0][kk];
		foundV = true;
	      }
	    }//for kk                                                                                                                                                           
	    for (int kk = 0; kk < single_vb2; kk++) {
	      if (fabs(v2_single[1][kk] - v_est) > 1.5)continue; //????? 0.5 needs to be reviewed                                                                                     
	      if (fabs(v2_single[1][kk] - v_est) < min_b) {
		min_b = fabs(v2_single[1][kk] - v_est);
		rh_segDC2[7][nDC2_segments] = v2_single[1][kk];
		rh_sigm_segDC2[7][nDC2_segments] = sigm_v2_single[1][kk];
		foundV = true;
	      }
	    }//for kk                                                                                                                           
	  }//!foundV 
	  if (foundV || foundU) nDC2_segments++;
	}//j
      }//i
    }//(x,y)first

    if (pair_u2 * pair_v2 > 0 && !found_8p_seg) { // (u,v) first
      for (int i = 0; i < pair_u2; i++) {
	if (nDC2_segments > 48)break;
	float u_coord = (u2_ab[0][i] + u2_ab[1][i]) / 2;

	for (int j = 0; j < pair_v2; j++) {
	  if (nDC2_segments > 48)break;
	  float v_coord = (v2_ab[0][j] + v2_ab[1][j]) / 2;
	  bool foundX = false;
	  float x_est = isqrt_2 * (v_coord - u_coord);
	  float y_est = isqrt_2 * (u_coord + v_coord);
	  if (pair_x2 > 0) {
	    double dX_thresh = 1.5;
	    for (int k = 0; k < pair_x2; k++) {
	      float x_coord = (x2_ab[0][k] + x2_ab[1][k]) / 2;
	      if (nDC2_segments > 48)break;
	      if (fabs(x_coord - x_est) > dX_thresh)continue;
	      dX_thresh = fabs(x_coord - x_est);

	      rh_segDC2[0][nDC2_segments] = x2_ab[0][k];
	      rh_segDC2[1][nDC2_segments] = x2_ab[1][k];
	      rh_segDC2[4][nDC2_segments] = u2_ab[0][i];
	      rh_segDC2[5][nDC2_segments] = u2_ab[1][i];
	      rh_segDC2[6][nDC2_segments] = v2_ab[0][j];
	      rh_segDC2[7][nDC2_segments] = v2_ab[1][j];
	      rh_sigm_segDC2[0][nDC2_segments] = sigm_x2_ab[0][k];
	      rh_sigm_segDC2[1][nDC2_segments] = sigm_x2_ab[1][k];
	      rh_sigm_segDC2[4][nDC2_segments] = sigm_u2_ab[0][i];
	      rh_sigm_segDC2[5][nDC2_segments] = sigm_u2_ab[1][i];
	      rh_sigm_segDC2[6][nDC2_segments] = sigm_v2_ab[0][j];
	      rh_sigm_segDC2[7][nDC2_segments] = sigm_v2_ab[1][j];

	      foundX = true;
	      if (nDC2_segments > 48)break;
	    }//k
	  }//(pair_x2>0)
	  if (found_8p_seg && !foundX)continue;
	  bool foundY = false;
	  if (pair_y2 > 0) {
	    double dY_thresh = 1.0;
	    for (int m = 0; m < pair_y2; m++) {
	      if (nDC2_segments > 48)break;
	      float y_coord = (y2_ab[0][m] + y2_ab[1][m]) / 2;
	      if (fabs(y_coord - y_est) > dY_thresh)continue;
	      dY_thresh = fabs(y_coord - y_est);
	      foundY = true;
	      rh_segDC2[2][nDC2_segments] = y2_ab[0][m];
	      rh_segDC2[3][nDC2_segments] = y2_ab[1][m];
	      rh_segDC2[4][nDC2_segments] = u2_ab[0][i];
	      rh_segDC2[5][nDC2_segments] = u2_ab[1][i];
	      rh_segDC2[6][nDC2_segments] = v2_ab[0][j];
	      rh_segDC2[7][nDC2_segments] = v2_ab[1][j];
	      rh_sigm_segDC2[2][nDC2_segments] = sigm_y2_ab[0][m];
	      rh_sigm_segDC2[3][nDC2_segments] = sigm_y2_ab[1][m];
	      rh_sigm_segDC2[4][nDC2_segments] = sigm_u2_ab[0][i];
	      rh_sigm_segDC2[5][nDC2_segments] = sigm_u2_ab[1][i];
	      rh_sigm_segDC2[6][nDC2_segments] = sigm_v2_ab[0][j];
	      rh_sigm_segDC2[7][nDC2_segments] = sigm_v2_ab[1][j];
	      if (!foundX) {
		float min_a = 999;
		float min_b = 999;
		for (int kk = 0; kk < single_xa2; kk++) {
		  if (fabs(x2_single[1][kk] - x_est) > 1.5)continue; //????? 0.5 needs to be reviewed

		  if (fabs(x2_single[0][kk] - x_est) < min_a) {
		    min_a = fabs(x2_single[0][kk] - x_est);
		    rh_segDC2[0][nDC2_segments] = x2_single[0][kk];
		    rh_sigm_segDC2[0][nDC2_segments] = sigm_x2_single[0][kk];
		    foundX = true;
		  }
		}//for kk                                                                                                                                                           
		for (int kk = 0; kk < single_xb2; kk++) {

		  if (fabs(x2_single[1][kk] - x_est) > 1.5)continue; //????? 0.5 needs to be reviewed                                                                                     
		  if (fabs(x2_single[1][kk] - x_est) < min_b) {
		    min_b = fabs(x2_single[1][kk] - x_est);
		    rh_segDC2[1][nDC2_segments] = x2_single[1][kk];
		    rh_sigm_segDC2[1][nDC2_segments] = sigm_x2_single[1][kk];
		    foundX = true;
		  }
		}//for kk 
		if (nDC2_segments > 48)break;
	      }//!foundX
	    }//m         
	    if (foundX && !foundY) {
	      float min_a = 999;
	      float min_b = 999;
	      for (int kk = 0; kk < single_ya2; kk++) {
		if (fabs(y2_single[0][kk] - y_est) > 1.5)continue; //????? 0.5 needs to be reviewed                                                                                     
		if (fabs(y2_single[0][kk] - y_est) < min_a) {
		  min_a = fabs(y2_single[0][kk] - y_est);
		  rh_segDC2[2][nDC2_segments] = y2_single[0][kk];
		  rh_sigm_segDC2[2][nDC2_segments] = sigm_y2_single[0][kk];
		  foundY = true;
		}
	      }//for kk                                                                                                                                                          
	      for (int kk = 0; kk < single_yb2; kk++) {
		if (fabs(y2_single[1][kk] - y_est) > 1.5)continue; //????? 0.5 needs to be reviewed                                                                                     
		if (fabs(y2_single[1][kk] - y_est) < min_b) {
		  min_b = fabs(y2_single[1][kk] - y_est);
		  rh_segDC2[3][nDC2_segments] = y2_single[1][kk];
		  rh_sigm_segDC2[3][nDC2_segments] = sigm_y2_single[1][kk];
		  foundY = true;
		}
	      }//for kk 
	    }
	  }//(pair_y2>0)
	  if (foundX || foundY) nDC2_segments++;
	}//j
      }//i
    }//(u,v) first

    //linear fit for dch1 segs
	
    for (int j = 0; j < nDC1_segments; j++) {
      //fit the initial seg and see if the chi2/ndof is too big try to get rid of the hit with the biggest deviation from the fit
      int worst_hit = -1;
      double max_resid = 0;
      for (int i = 0; i < 8; i++) {
	if (rh_segDC1[i][j] != -999)size_segDC1[j]++;
      }
      for (int rej = 0; rej < 2; rej++) {//allow 2 passes max 8->7 & 7->6

	float rh_seg[8] = {-999};
	float rh_sigm_seg[8] = {-999};
	float par_ab[4] = {999, 999, 999, 999};

	for (int i = 0; i < 8; i++) {
	  rh_seg[i] = rh_segDC1[i][j];
	  rh_sigm_seg[i] = rh_sigm_segDC1[i][j];
	}

	fit_seg(rh_seg, rh_sigm_seg, par_ab, -1, -1); //usual fit without skipping any plane
	for (int i = 0; i < 4; i++) {
	  par_ab1[i][j] = par_ab[i];
	}

	chi2_DC1[j] = 0;
	float resid = 999;
	float dev_coord[8] = {9, 9, 9, 9, 9, 9, 9, 9};
            
	for (int i = 0; i < 8; i++) {
	  if (i == 0 && rh_segDC1[i][j] != -999) {

	    resid = rh_segDC1[i][j] - z_loc[i] * par_ab1[0][j] - par_ab1[1][j];

	    if (fabs(resid) > max_resid) {
	      worst_hit = i;
	      max_resid = fabs(resid);
	    }
	    chi2_DC1[j] += (resid * resid) / rh_sigm_segDC1[i][j];

	  }
	  if (i == 1 && rh_segDC1[i][j] != -999) {
	    resid = rh_segDC1[i][j] - z_loc[i] * par_ab1[0][j] - par_ab1[1][j];
	    if (fabs(resid) > max_resid) {
	      worst_hit = i;
	      max_resid = fabs(resid);
	    }
	    chi2_DC1[j] += (resid * resid) / rh_sigm_segDC1[i][j];

	  }
	  if (i == 2 && rh_segDC1[i][j] != -999) {
	    resid = rh_segDC1[i][j] - z_loc[i] * par_ab1[2][j] - par_ab1[3][j];
	    if (fabs(resid) > max_resid) {
	      worst_hit = i;
	      max_resid = fabs(resid);
	    }
	    chi2_DC1[j] += (resid * resid) / rh_sigm_segDC1[i][j];

	  }
	  if (i == 3 && rh_segDC1[i][j] != -999) {
	    resid = rh_segDC1[i][j] - z_loc[i] * par_ab1[2][j] - par_ab1[3][j];

	    if (fabs(resid) > max_resid) {
	      worst_hit = i;
	      max_resid = fabs(resid);
	    }
	    chi2_DC1[j] += (resid * resid) / rh_sigm_segDC1[i][j];
	  }
	  if (i == 4 && rh_segDC1[i][j] != -999) {
	    resid = rh_segDC1[i][j] - isqrt_2 * z_loc[i]*(par_ab1[2][j] - par_ab1[0][j]) - isqrt_2 * (par_ab1[3][j] - par_ab1[1][j]);
	    if (fabs(resid) > max_resid) {
	      worst_hit = i;
	      max_resid = fabs(resid);
	    }
	    chi2_DC1[j] += (resid * resid) / rh_sigm_segDC1[i][j];

	  }
	  if (i == 5 && rh_segDC1[i][j] != -999) {
	    resid = rh_segDC1[i][j] - isqrt_2 * z_loc[i]*(par_ab1[2][j] - par_ab1[0][j]) - isqrt_2 * (par_ab1[3][j] - par_ab1[1][j]);
	    if (fabs(resid) > max_resid) {
	      worst_hit = i;
	      max_resid = fabs(resid);
	    }
	    chi2_DC1[j] += (resid * resid) / rh_sigm_segDC1[i][j];
	  }
	  if (i == 6 && rh_segDC1[i][j] != -999) {
	    resid = rh_segDC1[i][j] - isqrt_2 * z_loc[i]*(par_ab1[2][j] + par_ab1[0][j]) - isqrt_2 * (par_ab1[3][j] + par_ab1[1][j]);
	    if (fabs(resid) > max_resid) {
	      worst_hit = i;
	      max_resid = fabs(resid);
	    }
	    chi2_DC1[j] += (resid * resid) / rh_sigm_segDC1[i][j];
	  }
	  if (i == 7 && rh_segDC1[i][j] != -999) {


	    resid = rh_segDC1[i][j] - isqrt_2 * z_loc[i]*(par_ab1[2][j] + par_ab1[0][j]) - isqrt_2 * (par_ab1[3][j] + par_ab1[1][j]);
	    if (fabs(resid) > max_resid) {
	      worst_hit = i;
	      max_resid = fabs(resid);
	    }
	    chi2_DC1[j] += (resid * resid) / rh_sigm_segDC1[i][j];

	  }
	}//i

	chi2_DC1[j] /= (size_segDC1[j] - 4);

	if (chi2_DC1[j] > 30) {
	  if (size_segDC1[j] == 6) {
	    chi2_DC1[j] = 999;
	    break;
	  } else {
	    rh_segDC1[worst_hit][j] = -999; //erase worst hit and refit
	    size_segDC1[j]--;
	    max_resid = 0;
	    continue;
	  }
	} 
      }//rej 0 1 2

      //add shifts to slopes and coords
      par_ab1[0][j] += x1_slope_sh + x1_slope_sh * par_ab1[0][j] * par_ab1[0][j];
      par_ab1[2][j] += y1_slope_sh + y1_slope_sh * par_ab1[2][j] * par_ab1[2][j];
      par_ab1[1][j] += x1_sh;
      par_ab1[3][j] += y1_sh;

      xDC1_glob[j] = par_ab1[0][j]*(99.5) + par_ab1[1][j];
      yDC1_glob[j] = par_ab1[2][j]*(99.5) + par_ab1[3][j];
      if (size_segDC1[j] > 6) has7DC1 = true;
    }//for DC1 segs

    //linear fit for dch1 segs
    for (int j = 0; j < nDC2_segments; j++) {
      int worst_hit = -1;
      double max_resid = 0;
      for (int i = 0; i < 8; i++) {
	if (rh_segDC2[i][j] != -999)size_segDC2[j]++;
      }
      for (int rej = 0; rej < 2; rej++) {//allow 2 passes max 8->7 & 7->6

	//end linear fit
	float rh_seg[8] = {-999};
	float rh_sigm_seg[8] = {-999};
	float par_ab[4] = {999, 999, 999, 999};

	for (int i = 0; i < 8; i++) {
	  rh_seg[i] = rh_segDC2[i][j];
	  rh_sigm_seg[i] = rh_sigm_segDC2[i][j];
	}

	fit_seg(rh_seg, rh_sigm_seg, par_ab, -1, -1); //usual fit without skipping any plane
	for (int i = 0; i < 4; i++) {
	  par_ab2[i][j] = par_ab[i];
	}

	chi2_DC2[j] = 0;
             
	float resid = 999;
             
	for (int i = 0; i < 8; i++) {
	  if (i == 0 && rh_segDC2[i][j] != -999) {
	    resid = rh_segDC2[i][j] - z_loc[i] * par_ab2[0][j] - par_ab2[1][j];
	    chi2_DC2[j] += (resid * resid) / rh_sigm_segDC2[i][j];
	    if (fabs(resid) > max_resid) {
	      worst_hit = i;
	      max_resid = fabs(resid);
	    }
	  }
	  if (i == 1 && rh_segDC2[i][j] != -999) {
	    resid = rh_segDC2[i][j] - z_loc[i] * par_ab2[0][j] - par_ab2[1][j];

	    chi2_DC2[j] += (resid * resid) / rh_sigm_segDC2[i][j];

	    if (fabs(resid) > max_resid) {
	      worst_hit = i;
	      max_resid = fabs(resid);
	    }
	  }
	  if (i == 2 && rh_segDC2[i][j] != -999) {
	    resid = rh_segDC2[i][j] - z_loc[i] * par_ab2[2][j] - par_ab2[3][j];

	    chi2_DC2[j] += (resid * resid) / rh_sigm_segDC2[i][j];

	    if (fabs(resid) > max_resid) {
	      worst_hit = i;
	      max_resid = fabs(resid);
	    }
	  }
	  if (i == 3 && rh_segDC2[i][j] != -999) {
	    resid = rh_segDC2[i][j] - z_loc[i] * par_ab2[2][j] - par_ab2[3][j];

	    chi2_DC2[j] += (resid * resid) / rh_sigm_segDC2[i][j];

	    if (fabs(resid) > max_resid) {
	      worst_hit = i;
	      max_resid = fabs(resid);
	    }
	  }
	  if (i == 4 && rh_segDC2[i][j] != -999) {
	    resid = rh_segDC2[i][j] - isqrt_2 * z_loc[i]*(par_ab2[2][j] - par_ab2[0][j]) - isqrt_2 * (par_ab2[3][j] - par_ab2[1][j]);
	    chi2_DC2[j] += (resid * resid) / rh_sigm_segDC2[i][j];


	    if (fabs(resid) > max_resid) {
	      worst_hit = i;
	      max_resid = fabs(resid);
	    }
	  }
	  if (i == 5 && rh_segDC2[i][j] != -999) {
	    resid = rh_segDC2[i][j] - isqrt_2 * z_loc[i]*(par_ab2[2][j] - par_ab2[0][j]) - isqrt_2 * (par_ab2[3][j] - par_ab2[1][j]);
	    chi2_DC2[j] += (resid * resid) / rh_sigm_segDC2[i][j];
	    if (fabs(resid) > max_resid) {
	      worst_hit = i;
	      max_resid = fabs(resid);
	    }
	  }
	  if (i == 6 && rh_segDC2[i][j] != -999) {
	    resid = rh_segDC2[i][j] - isqrt_2 * z_loc[i]*(par_ab2[2][j] + par_ab2[0][j]) - isqrt_2 * (par_ab2[3][j] + par_ab2[1][j]);
	    chi2_DC2[j] += (resid * resid) / rh_sigm_segDC2[i][j];

	    if (fabs(resid) > max_resid) {
	      worst_hit = i;
	      max_resid = fabs(resid);
	    }
	  }
	  if (i == 7 && rh_segDC2[i][j] != -999) {
	    resid = rh_segDC2[i][j] - isqrt_2 * z_loc[i]*(par_ab2[2][j] + par_ab2[0][j]) - isqrt_2 * (par_ab2[3][j] + par_ab2[1][j]);
	    chi2_DC2[j] += (resid * resid) / rh_sigm_segDC2[i][j];

	    if (fabs(resid) > max_resid) {
	      worst_hit = i;
	      max_resid = fabs(resid);
	    }
	  }
	}//i
	chi2_DC2[j] /= (size_segDC2[j] - 4);
	//if chi2 is big and seg_size = min erase this seg
	if (chi2_DC2[j] > 30) {
	  if (size_segDC2[j] == 6) {
	    chi2_DC2[j] = 999;
	    break;
	  } else {
	    rh_segDC2[worst_hit][j] = -999;//erase worst hit and refit
	    size_segDC2[j]--;
	    max_resid = 0;
	    continue;
	  }
	} 
      }//rej 0 1 2

      //add shifts to slopes and coords
      par_ab2[0][j] += x2_slope_sh + x2_slope_sh * par_ab2[0][j] * par_ab2[0][j];
      par_ab2[2][j] += y2_slope_sh + y2_slope_sh * par_ab2[2][j] * par_ab2[2][j];
      par_ab2[1][j] += x2_sh;
      par_ab2[3][j] += y2_sh;

      xDC2_glob[j] = par_ab2[0][j]*(-99.5) + par_ab2[1][j];
      yDC2_glob[j] = par_ab2[2][j]*(-99.5) + par_ab2[3][j];
      //
      if (size_segDC2[j] > 6) has7DC2 = true;
    }//for DC2 segs

    //count the number of rh per segment
    double x_mid[50];//x glob of matched segment in the z situated between the two DCH chambers
    double y_mid[50];//y glob of matched segment in the z situated between the two DCH chambers
    double a_X[50];//x slope of the matched segment
    double a_Y[50];//y slope of the matched segment
    double imp[50];//reconstructed particle trajectory momentum 
    double leng[50];//the distance from z = 0 to the global point of the matched segment
    int seg_it = -1;
    int dc1_best[50];
    int dc2_best[50];
    float Chi2_match[50];//chi2 of the matched seg
    for (int seg = 0; seg < 50; seg++) {
      x_mid[seg] = -999;
      y_mid[seg] = -999;
      a_X[seg] = -999;
      a_Y[seg] = -999;
      imp[seg] = -999;
      leng[seg] = -999;
      Chi2_match[seg] = -999;	
      dc1_best[seg] = 0;
      dc2_best[seg] = 0;
    }

      
    float xMean = 999;
    float yMean = 999;

    //leave only longest and best chi2 segments
    //dc1
    for (int max_size = 8; max_size > 5; max_size--) {
      //find longest and best chi2 seg
      for (int sizeit1 = 0; sizeit1 < nDC1_segments; sizeit1++) {
	if (size_segDC1[sizeit1] != max_size)continue;
	for (int sizeit1_1 = 0; sizeit1_1 < nDC1_segments; sizeit1_1++) {
	  if (sizeit1_1 == sizeit1)continue;
	  for (int hit = 0; hit < 4; hit++) {
	    if (rh_segDC1[2 * hit][sizeit1] == rh_segDC1[2 * hit][sizeit1_1] && rh_segDC1[2 * hit + 1][sizeit1] == rh_segDC1[2 * hit + 1][sizeit1_1] && (chi2_DC1[sizeit1] <= chi2_DC1[sizeit1_1] || size_segDC1[sizeit1] > size_segDC1[sizeit1_1])) {
	      chi2_DC1[sizeit1_1] = 999; //mark seg as bad                                                                                                   
	      break;
	    }
	  }//hit
	}
      }
    }//max_size

    //dc2
    for (int max_size = 8; max_size > 5; max_size--) {
      //find longest and best chi2 seg                                                                                                                
      for (int sizeit2 = 0; sizeit2 < nDC2_segments; sizeit2++) {
	if (size_segDC2[sizeit2] != max_size)continue;
	for (int sizeit2_2 = 0; sizeit2_2 < nDC2_segments; sizeit2_2++) {
	  if (sizeit2_2 == sizeit2)continue;
	  for (int hit = 0; hit < 4; hit++) {
	    if (rh_segDC2[2 * hit][sizeit2] == rh_segDC2[2 * hit][sizeit2_2] && rh_segDC2[2 * hit + 1][sizeit2] == rh_segDC2[2 * hit + 1][sizeit2_2] && (chi2_DC2[sizeit2] <= chi2_DC2[sizeit2_2] || size_segDC2[sizeit2] > size_segDC2[sizeit2_2])) {
	      chi2_DC2[sizeit2_2] = 999; //mark seg as bad                                                                                                     
	      break;
	    }
	  }//hit                                                                                                                                                 
	}
      }
    }//max_size           
    //fill local segments z,x,y global coords; x-slope; y-slope; Chi2; to be continued...
    for (int sizeit1 = 0; sizeit1 < nDC1_segments; sizeit1++) {
      if (chi2_DC1[sizeit1] > 50) continue;
      BmnTrack TracksDch1;
      FairTrackParam ParamsTrackDch1;
      Float_t z0 = Z_dch1;
      Float_t x0 = par_ab1[1][sizeit1];
      Float_t y0 = par_ab1[3][sizeit1];
      ParamsTrackDch1.SetPosition(TVector3(x0, y0, z0));
      ParamsTrackDch1.SetTx(par_ab1[0][sizeit1]);
      ParamsTrackDch1.SetTy(par_ab1[2][sizeit1]);
      //  TracksDch1.SetParamFirst(ParamsTrackDch1);
      //TracksDch1.SetChi2(chi2_DC1[sizeit1]);
      //TracksDch1.SetNDF(Nhits_Dch1[ind_best_Dch1[iBest]]); //WARNING!!! now it is number of hits in matched track!                                             
      BmnTrack* tr_dch1 = new((*dchTracks)[dchTracks->GetEntriesFast()]) BmnTrack();
      tr_dch1->SetChi2(chi2_DC1[sizeit1]);
      tr_dch1->SetNHits(size_segDC1[sizeit1]);
      tr_dch1->SetParamFirst(ParamsTrackDch1);
              
    }

    for (int sizeit2 = 0; sizeit2 < nDC2_segments; sizeit2++) {
      if (chi2_DC2[sizeit2] > 50) continue;
      BmnTrack TracksDch2;
      FairTrackParam ParamsTrackDch2;
      Float_t z0 = Z_dch2;
      Float_t x0 = par_ab2[1][sizeit2];
      Float_t y0 = par_ab2[3][sizeit2];
      ParamsTrackDch2.SetPosition(TVector3(x0, y0, z0));
      ParamsTrackDch2.SetTx(par_ab2[0][sizeit2]);
      ParamsTrackDch2.SetTy(par_ab2[2][sizeit2]);
      TracksDch2.SetParamFirst(ParamsTrackDch2);
      //TracksDch2.SetChi2(chi2_DC2[sizeit2]);
      // TracksDch2.SetNDF(Nhits_Dch2[ind_best_Dch2[iBest]]); //WARNING!!! now it is number of hits in matched track!                                            
                   
      BmnTrack* tr_dch2 = new((*dchTracks)[dchTracks->GetEntriesFast()]) BmnTrack();
      tr_dch2->SetChi2(chi2_DC2[sizeit2]);
      tr_dch2->SetNHits(size_segDC2[sizeit2]);
      tr_dch2->SetParamFirst(ParamsTrackDch2);

    }

    //try to match the reconstructed segments from the two chambers
    if (has7DC1) {

      int match_dc2_seg = -1;
      float ax = -999.0;
      float ay = -999.0;
      float ax1 = -999.0;
      float ax2 = -999.0;
      float ay1 = -999.0;
      float ay2 = -999.0;
      float bx1, bx2, by1, by2;
      bx1 = bx2 = by1 = by2 = -999.0;

      for (int segdc1Nr = 0; segdc1Nr < nDC1_segments; segdc1Nr++) {
	if (chi2_DC1[segdc1Nr] > 50 || size_segDC1[segdc1Nr] < 7)continue; //skip rejected segs with chi2 = 999 
	float min_distX = 20;
	float min_distY = 15;
	float min_distSQ = 225;

	float dx = -999;
	float dy = -999;
	float daX = -999;
	float daY = -999;
	float chi2_match = 0;

	for (int segdc2Nr = 0; segdc2Nr < nDC2_segments; segdc2Nr++) {
	  if (chi2_DC2[segdc2Nr] > 50)continue; //skip rejected segs with chi2 = 999 

	  float distX = fabs(xDC1_glob[segdc1Nr] - xDC2_glob[segdc2Nr]);
	  float distY = fabs(yDC1_glob[segdc1Nr] - yDC2_glob[segdc2Nr]);
	  if (distX < min_distX && distY < min_distY) {
	    float distSQ = distX * distX + distY*distY;

	    chi2_match = (fabs(xDC1_glob[segdc1Nr] - xDC2_glob[segdc2Nr]) * fabs(xDC1_glob[segdc1Nr] - xDC2_glob[segdc2Nr]) / 49)+(distY * distY / 43.56)+((par_ab2[0][segdc2Nr] - par_ab1[0][segdc1Nr] * par_ab2[0][segdc2Nr] - par_ab1[0][segdc1Nr]) / 0.0144)+(par_ab2[2][segdc2Nr] - par_ab1[2][segdc1Nr] * par_ab2[2][segdc2Nr] - par_ab1[2][segdc1Nr]) / 0.0225;
	    if (distSQ < min_distSQ) {

	      dx = xDC2_glob[segdc2Nr] - xDC1_glob[segdc1Nr];
	      dy = yDC2_glob[segdc2Nr] - yDC1_glob[segdc1Nr];
	      xMean = 0.5 * (xDC1_glob[segdc1Nr] + xDC2_glob[segdc2Nr]);
	      yMean = 0.5 * (yDC1_glob[segdc1Nr] + yDC2_glob[segdc2Nr]);
	      ax = (par_ab2[1][segdc2Nr] - par_ab1[1][segdc1Nr]) / 199;
	      ay = (par_ab2[3][segdc2Nr] - par_ab1[3][segdc1Nr]) / 199;

	      min_distSQ = distSQ;
	      match_dc2_seg = segdc2Nr; //remember matched dc2 seg 
	      daX = par_ab2[0][segdc2Nr] - par_ab1[0][segdc1Nr];
	      daY = par_ab2[2][segdc2Nr] - par_ab1[2][segdc1Nr];
	      ax1 = par_ab1[0][segdc1Nr];
	      ax2 = par_ab2[0][segdc2Nr];
	      ay1 = par_ab1[2][segdc1Nr];
	      ay2 = par_ab2[2][segdc2Nr];
	      bx2 = par_ab2[1][segdc2Nr];
	      by2 = par_ab2[3][segdc2Nr];
	    } // min_dist
	  }//distX<min_distX && distY<min_distY

	} // segdc2Nr

	if (min_distSQ == 225) continue;

	//-n	  if(fabs(dx) < 7 && fabs(dy)<8.2){
	if (fabs(dx) < 10 && fabs(dy) < 10) {
	  if (fabs(daX) < 0.2 && fabs(daY) < 0.2) {
	    seg_it++;
	    leng[seg_it] = sqrt(628.65 * 628.65 + (xMean * xMean));
	    x_mid[seg_it] = xMean;
	    y_mid[seg_it] = yMean;
	    //a_X[seg_it] = atan(ax);
	    //a_Y[seg_it] = atan(ay);
	    a_X[seg_it] = ax;
	    a_Y[seg_it] = ay;
	    imp[seg_it] = -0.4332 / (ax + 0.006774);
	    Chi2_match[seg_it] = chi2_match;
	    chi2_DC1[segdc1Nr] = 999; //mark dch1 seg as used for future iterations
	    chi2_DC2[match_dc2_seg] = 999; //mark dch2 seg as used for future iterations
                       		
	  }// dax  day
	}//dx<0.1 dy
      } // segdc1Nr
    } // if(has7DC1)  8p+6p

    if (has7DC2) {
      int match_dc1_seg = -1;

      float ax = -999.0;
      float ay = -999.0;
      float ax1 = -999.0;
      float ax2 = -999.0;
      float ay1 = -999.0;
      float ay2 = -999.0;

      float bx1, bx2, by1, by2;
      bx1 = bx2 = by1 = by2 = -999.0;

      for (int segdc2Nr = 0; segdc2Nr < nDC2_segments; segdc2Nr++) {

	float min_distX = 20;
	float min_distY = 15;
	float min_distSQ = 225;

	float dx = -999;
	float dy = -999;
	float daX = -999;
	float daY = -999;

	if (chi2_DC2[segdc2Nr] > 50 || size_segDC2[segdc2Nr] < 7)continue;

	float chi2_match = 0;

	for (int segdc1Nr = 0; segdc1Nr < nDC1_segments; segdc1Nr++) {
	  if (chi2_DC1[segdc1Nr] > 50)continue;


	  float distX = fabs(xDC1_glob[segdc1Nr] - xDC2_glob[segdc2Nr]);
	  float distY = fabs(yDC1_glob[segdc1Nr] - yDC2_glob[segdc2Nr]);

	  if (distX < min_distX && distY < min_distY) {
	    float distSQ = distX * distX + distY*distY;

	    chi2_match = (fabs(xDC1_glob[segdc1Nr] - xDC2_glob[segdc2Nr]) * fabs(xDC1_glob[segdc1Nr] - xDC2_glob[segdc2Nr]) / 49)+(distY * distY / 43.56)+((par_ab2[0][segdc2Nr] - par_ab1[0][segdc1Nr] * par_ab2[0][segdc2Nr] - par_ab1[0][segdc1Nr]) / 0.0144)+(par_ab2[2][segdc2Nr] - par_ab1[2][segdc1Nr] * par_ab2[2][segdc2Nr] - par_ab1[2][segdc1Nr]) / 0.0225;
                     
	    if (distSQ < min_distSQ) {

	      dx = xDC2_glob[segdc2Nr] - xDC1_glob[segdc1Nr];
	      dy = yDC2_glob[segdc2Nr] - yDC1_glob[segdc1Nr];
	      xMean = 0.5 * (xDC1_glob[segdc1Nr] + xDC2_glob[segdc2Nr]);
	      yMean = 0.5 * (yDC1_glob[segdc1Nr] + yDC2_glob[segdc2Nr]);
	      min_distSQ = distSQ;
	      ax = (par_ab2[1][segdc2Nr] - par_ab1[1][segdc1Nr]) / 199;
	      ay = (par_ab2[3][segdc2Nr] - par_ab1[3][segdc1Nr]) / 199;
	      match_dc1_seg = segdc1Nr;
	      daX = par_ab2[0][segdc2Nr] - par_ab1[0][segdc1Nr];
	      daY = par_ab2[2][segdc2Nr] - par_ab1[2][segdc1Nr];
	    } // min_dist
	  }
	} // segdc1Nr

	if (min_distSQ >= 225) continue;
             
	if (fabs(dx) < 10 && fabs(dy) < 10) {
	  if (fabs(daX) < 0.2 && fabs(daY) < 0.2) {

	    seg_it++;
	    leng[seg_it] = sqrt(628.65 * 628.65 + (xMean * xMean));
	    x_mid[seg_it] = xMean;
	    y_mid[seg_it] = yMean;
	    a_X[seg_it] = atan(ax);
	    a_Y[seg_it] = atan(ay);
	    imp[seg_it] = -0.4332 / (ax + 0.006774);
	    Chi2_match[seg_it] = chi2_match;
	    chi2_DC2[segdc2Nr] = 999; //mark dch2 seg as used for future iterations
	    chi2_DC1[match_dc1_seg] = 999; //mark dch1 seg as used for future iterations
                        
	  } // dax  day  
	} //  dx  dy
      } // segdc2Nr
    }//  6p+8p  has7DC2

    //do 6+6p glob segs

    int match_dc1_seg = -1;
       
    float ax = -999.0;
    float ay = -999.0;
    float ax1 = -999.0;
    float ax2 = -999.0;
    float ay1 = -999.0;
    float ay2 = -999.0;

    float bx1, bx2, by1, by2;
    bx1 = bx2 = by1 = by2 = -999.0;

    for (int segdc2Nr = 0; segdc2Nr < nDC2_segments; segdc2Nr++) {
      float min_distX = 20;
      float min_distY = 15;
      float min_distSQ = 225;

      float dx = -999;
      float dy = -999;
      float daX = -999;
      float daY = -999;

      if (chi2_DC2[segdc2Nr] > 50)continue;

      float chi2_match = 0;
      for (int segdc1Nr = 0; segdc1Nr < nDC1_segments; segdc1Nr++) {
	if (chi2_DC1[segdc1Nr] > 50) continue;

	float distX = fabs(xDC1_glob[segdc1Nr] - xDC2_glob[segdc2Nr]);
	float distY = fabs(yDC1_glob[segdc1Nr] - yDC2_glob[segdc2Nr]);

	if (distX < min_distX && distY < min_distY) {
	  float distSQ = distX * distX + distY*distY;

	  chi2_match = (fabs(xDC1_glob[segdc1Nr] - xDC2_glob[segdc2Nr]) * fabs(xDC1_glob[segdc1Nr] - xDC2_glob[segdc2Nr]) / 49)+(distY * distY / 43.56)+((par_ab2[0][segdc2Nr] - par_ab1[0][segdc1Nr] * par_ab2[0][segdc2Nr] - par_ab1[0][segdc1Nr]) / 0.0144)+(par_ab2[2][segdc2Nr] - par_ab1[2][segdc1Nr] * par_ab2[2][segdc2Nr] - par_ab1[2][segdc1Nr]) / 0.0225;
	  if (distSQ < min_distSQ) {

	    dx = xDC2_glob[segdc2Nr] - xDC1_glob[segdc1Nr];
	    dy = yDC2_glob[segdc2Nr] - yDC1_glob[segdc1Nr];
	    xMean = 0.5 * (xDC1_glob[segdc1Nr] + xDC2_glob[segdc2Nr]);
	    yMean = 0.5 * (yDC1_glob[segdc1Nr] + yDC2_glob[segdc2Nr]);
	    min_distSQ = distSQ;
	    ax = (par_ab2[1][segdc2Nr] - par_ab1[1][segdc1Nr]) / 199;
	    ay = (par_ab2[3][segdc2Nr] - par_ab1[3][segdc1Nr]) / 199;
	    match_dc1_seg = segdc1Nr;
	    daX = par_ab2[0][segdc2Nr] - par_ab1[0][segdc1Nr];
	    daY = par_ab2[2][segdc2Nr] - par_ab1[2][segdc1Nr];
                        
	  } // min_dist
	}
      } // segdc1Nr
         
      if (min_distSQ >= 225) continue;
        
      if (fabs(dx) < 10 && fabs(dy) < 10) {
	if (fabs(daX) < 0.2 && fabs(daY) < 0.2) {

	  seg_it++;
	  leng[seg_it] = sqrt(628.65 * 628.65 + (xMean * xMean));
	  x_mid[seg_it] = xMean;
	  y_mid[seg_it] = yMean;
	  a_X[seg_it] = atan(ax);
	  a_Y[seg_it] = atan(ay);
	  imp[seg_it] = -0.4332 / (ax + 0.006774);
	  Chi2_match[seg_it] = chi2_match;
	  chi2_DC2[segdc2Nr] = 999; //mark dc2 seg as used for future iterations
	  chi2_DC1[match_dc1_seg] = 999; //mark dch1 seg as used for future iterations
	} // dax  day  
      } //  dx  dy
    } // segdc2Nr

       
       
    if (seg_it>-1) {
      for (int seg = 0; seg < seg_it + 1; seg++) {
	//CbmTrack TracksDchMatch;
	FairTrackParam ParamsTrackDchMatch;
	Float_t z0 = Z_dch_mid;
	Float_t x0 = x_mid[seg];
	Float_t y0 = y_mid[seg];
	ParamsTrackDchMatch.SetPosition(TVector3(x0, y0, z0));
	ParamsTrackDchMatch.SetTx(a_X[seg]);
	ParamsTrackDchMatch.SetTy(a_Y[seg]);
	//	TracksDchMatch.SetParamFirst(&ParamsTrackDchMatch);
	//                TracksDchMatch.SetChiSq(Chi2_match[seg]);
	// TracksDch2.SetNDF(Nhits_Dch2[ind_best_Dch2[iBest]]); //WARNING!!! now it is number of hits in matched track! 
	BmnTrack* newTrack = new ((*dchMatchedTracks)[dchMatchedTracks->GetEntriesFast()]) BmnTrack();
	newTrack->SetChi2(Chi2_match[seg]);
	newTrack->SetParamFirst(ParamsTrackDchMatch);

      }
    }//seg_it>0
                    
    tReco->Fill();
              
  } // event loop
    
  fReco->cd();
  tReco->Write();
  fReco->Close();
    
}

// LocalWords:  GetWireNumber X
