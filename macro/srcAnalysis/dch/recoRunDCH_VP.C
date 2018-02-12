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

//#include <BmnEventHeader.h>

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
#include "TROOT.h"
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

//#include "/nica/user/n/nvoytish/bmn_new_1/bmnroot/bmndata/BmnDchDigit.h"

#include "BmnDchDigit.h"
#include "BmnTrack.h"
#include "BmnTrigDigit.h"
#include <vector>
// #include <set>

using namespace std;
using namespace TMath;


//
//  runType - "run1", "run2", "run3"
//

//some global constants

const Float_t z_loc[8] = {7.8, 6.6, 3.0, 1.8, -1.8, -3.0, -6.6, -7.8}; // z local xa->vb (cm)
const Float_t z_glob[16] = {-45.7, -46.9, -51.5, -52.7, -57.3, -58.5, -63.1, -64.3, 64.3, 63.1, 58.5, 57.3, 52.7, 51.5, 46.9, 45.7}; // z global dc 1 & dc 2 (cm)
const Float_t Z_dch1 = 536.2;//514.3; //z coord in the midle of dch1	
const Float_t Z_dch2 = 735.7;//713.8; //z coord in the midle of dch2
const Float_t Z_dch_mid = 635.95;//614.05; //z coord between the two chambers, this z is considered the global z for the matched dch segment


Double_t sqrt_2 = sqrt(2.);
Double_t isqrt_2 = 1 / sqrt_2;
//shift variables                                                                                                                                                            
float intBL = 1.93; //1200A switch for different fields

/*   
// run2
const Float_t x1_sh = -5.2 ;float intBL = 1.93;//1200A 815   
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

const Float_t x1_sh = 10.9;// 5.34;//5.14;//10.31; //
const Float_t x2_sh = 5.56;//0;//7.95;// //
const Float_t y1_sh = -1.19;//0;//-1.03;
const Float_t y2_sh = -2.47;//-1.38;//-3.7;

const Float_t u1_sh = isqrt_2 * (y1_sh - x1_sh);
const Float_t u2_sh = isqrt_2 * (y2_sh - x2_sh);
const Float_t v1_sh = isqrt_2 * (x1_sh + y1_sh);
const Float_t v2_sh = isqrt_2 * (x2_sh + y2_sh);

const Float_t x1_slope_sh = -0.0191;//-0.1047;//-0.108;//-0.0837;//-0.0093;//-0.0892;
const Float_t y1_slope_sh = 0.0277;//0.0563;//04;//0.0085;//0.0475; //was .05
const Float_t x2_slope_sh = -0.0181;//-0.1037;//6;//-0.088;
const Float_t y2_slope_sh = 0.0348;//0.0651;//533;//0.0062;//0.055; //

const Float_t u1_slope_sh = isqrt_2 * (y1_slope_sh - x1_slope_sh);
const Float_t u2_slope_sh = isqrt_2 * (y2_slope_sh - x2_slope_sh);
const Float_t v1_slope_sh = isqrt_2 * (y1_slope_sh + x1_slope_sh);
const Float_t v2_slope_sh = isqrt_2 * (y2_slope_sh + x2_slope_sh);

void fit_seg(double *rh_seg, double *rh_sigm_seg, double *par_ab, int skip_first, int skip_second) {

  //linear fit
  double z_loc[8] = {7.8, 6.6, 3.0, 1.8, -1.8, -3.0, -6.6, -7.8}; // z local xa->vb (cm)
  double A[4][4] = {
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0}
  }; //coef matrix
  double f[4] = {0.}; //free coef 
  //      float sigm_sq[8] = {1,1,1,1,1,1,1,1};
  int h[8] = {0, 0, 0, 0, 0, 0, 0, 0};


  for (int i = 0; i < 4; i++) {
    par_ab[i] = 999.;
  }

  for (int i = 0; i < 8; i++) {
    h[i] = 1;

    if (i == skip_first || i == skip_second || rh_seg[i] < -998.) {
      h[i] = 0;
    }
    //    cout<<"setting h[i] "<<i<<" hi "<<h[i]<<endl;

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
  double factor;
  double temp[4];
  double b[4][4];
  double A0[4][4];

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

  double sum;

  double A1[4][4] = {
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0}
  };

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
    //    cout<<" in fit par ab "<<i1<<" par "<<par_ab[i1]<<endl;
  }
  
}//fit_seg

void recoRunDCH_VP(Int_t runId = 1882) {

  TChain *bmnTree = new TChain("cbmsim");
  bmnTree->Add(TString::Format("/nica/mpd13/nvoytish/run6_digis/bmn_run%04d_digi.root", runId));

  TClonesArray *tof700 = new TClonesArray("BmnTof2Digit");
  TClonesArray *tof400 = new TClonesArray("BmnTof1Digit");
  TClonesArray *eventHeader = new TClonesArray("BmnEventHeader");
  TClonesArray *t0Digits = new TClonesArray("BmnTrigDigit");
  TClonesArray *bc2Digits = new TClonesArray("BmnTrigDigit");
  TClonesArray *vetoDigits = new TClonesArray("BmnTrigDigit");
  bmnTree->SetBranchAddress("TOF700", &tof700);
  bmnTree->SetBranchAddress("TOF400", &tof400);
  bmnTree->SetBranchAddress("EventHeader", &eventHeader);
  bmnTree->SetBranchAddress("T0", &t0Digits);
  bmnTree->SetBranchAddress("BC2", &bc2Digits);
  bmnTree->SetBranchAddress("VETO", &vetoDigits);
  TClonesArray *dchDigits = NULL;
  bmnTree->SetBranchAddress("DCH", &dchDigits);

  Int_t startEvent = 1;
  Int_t nEvents =  bmnTree->GetEntries();
  //          nEvents = 1000;

  TString outName = Form("bmn_reco_DCH_run%04d.root", runId);
  TFile* fReco = new TFile(outName, "RECREATE"); //the root file where the dch hits and tracks will be filled
  TTree* tReco = new TTree("cbmsim", "test_bmn");

  TClonesArray* dchHits = new TClonesArray("BmnDchHit");
  TClonesArray* dchTracks = new TClonesArray("BmnTrack");
  TClonesArray* dch1Tracks_out_of_dch2_accept = new TClonesArray("BmnTrack");
  TClonesArray* dchMatchedTracks = new TClonesArray("BmnTrack");

  tReco->Branch("EventHeader", &eventHeader);
  tReco->Branch("TOF700", &tof700);
  tReco->Branch("TOF400", &tof400);
  tReco->Branch("BmnDchHit", &dchHits);
  tReco->Branch("DchTracks", &dchTracks);
  tReco->Branch("Dch1Tracks_out_of_Dch2_accept", &dch1Tracks_out_of_dch2_accept);
  tReco->Branch("DchMatchedTracks", &dchMatchedTracks);

  TH1F *hNr_segs_dc1 =  new TH1F("hNr_segs_dc1"," number segs in dc1 per event", 50,0,50);
  TH1F *hNr_segs_dc2 =  new TH1F("hNr_segs_dc2"," number segs in dc2 per event", 50,0,50);
  TH1F *hXDC1_atZ0 = new TH1F("hXDC1_atZ0"," XDC1 extrapol Z = 0", 100,-50,50);
  TH1F *hYDC1_atZ0 = new TH1F("hYDC1_atZ0"," YDC1 extrapol Z = 0", 100,-50,50);
  TH1F *hXDC2_atZ0 = new TH1F("hXDC2_atZ0"," XDC2 extrapol Z = 0", 100,-50,50);
  TH1F *hYDC2_atZ0 = new TH1F("hYDC2_atZ0"," YDC2 extrapol Z = 0", 100,-50,50);

  TH1F *hX_matched = new TH1F("hX_matched"," X of matched segs extrapol Z = 641 cm (ch9)", 200,17,37);
  TH1F *hX_matchedZ1 = new TH1F("hX_matchedZ1"," X of matched segs extrapol Z_dc1", 200,17,37);
  TH1F *hX_matched_p_outDC2 = new TH1F("hX_matched_p_outDC2"," X of matched and out of dc2 acceptance segs extrapol Z = 641cm (ch9)", 200,17,37);
  TH1F *hXa_ccup = new TH1F("hXa_ccup"," xa wire nr ", 200,0,200);
  TH1F *hXb_ccup = new TH1F("hXb_ccup"," xb wire nr ", 200,0,200);

  //read and memorize the transfer fuctions' parameters

  double t_dc[4][16]; //[time interval][plane number]
  double pol_par_dc[3][5][16]; //[polinom number][param number][plane number]
  double scaling[16]; //[plane number]

  ifstream fin;
  fin.open(TString::Format("pol_coord0%04d.txt", runId), ios::in);
  //fin.open("transfer_func.txt",ios::in);
  for (int fi = 0; fi < 16; fi++) {
    fin >> t_dc[0][fi] >> t_dc[1][fi] >> t_dc[2][fi] >> t_dc[3][fi] >> pol_par_dc[0][0][fi] >> pol_par_dc[0][1][fi] >> pol_par_dc[0][2][fi] >> pol_par_dc[0][3][fi] >> pol_par_dc[0][4][fi] >> pol_par_dc[1][0][fi] >> pol_par_dc[1][1][fi] >> pol_par_dc[1][2][fi] >> pol_par_dc[1][3][fi] >> pol_par_dc[1][4][fi] >> pol_par_dc[2][0][fi] >> pol_par_dc[2][1][fi] >> pol_par_dc[2][2][fi] >> pol_par_dc[2][3][fi] >> pol_par_dc[2][4][fi] >> scaling[fi];
 }
  //cout<< t_dc[0][0] <<" "<< t_dc[1][0] <<" "<< t_dc[2][0] <<" "<< t_dc[3][0] <<" "<< pol_par_dc[0][0][0] <<" "<< pol_par_dc[0][1][0] <<" "<< pol_par_dc[0][2][0] <<" "<< pol_par_dc[0][3][0] <<" "<< pol_par_dc[0][4][0] <<" "<< pol_par_dc[1][0][0] <<" "<< pol_par_dc[1][1][0] <<" "<< pol_par_dc[1][2][0] <<" "<< pol_par_dc[1][3][0] <<" "<< pol_par_dc[1][4][0] <<" "<< pol_par_dc[2][0][0] <<" "<< pol_par_dc[2][1][0] <<" "<< pol_par_dc[2][2][0] <<" "<< pol_par_dc[2][3][0] <<" "<< pol_par_dc[2][4][0] <<" "<< scaling[0]<<endl;
  fin.close();

  float scale[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  for (Int_t sc = 0; sc < 16; sc++) {
    scale[sc] = 0.5; //  / scaling[sc];
  }//scaling for transfer function coeffs

  int prev_wire = -1;
  int prev_time = -1;
  int skipped_ev = 0;
  int poss = 0;
  int imposs = 0;
  for (Int_t iEv = startEvent; iEv < startEvent + nEvents; iEv++) {
    eventHeader->Clear();
    tof700->Clear();
    tof400->Clear();
    bmnTree->GetEntry(iEv);
    dchHits->Clear();
    dchTracks->Clear();
    dch1Tracks_out_of_dch2_accept->Clear();
    dchMatchedTracks->Clear();
   


    //               cout <<"------------" <<iEv << endl;


       
    //if (iEv % 1 == 0) cout<<" ev curr "<<iEv<<" event header id "<<((BmnEventHeader*) eventHeader->At(0))->GetEventId()<<endl;
    if (iEv % 1000 == 0) cout << "Event/last : " << iEv + 1 << "/" << startEvent + nEvents << endl;
    //	if (iEv % 1 == 0) cout<<" ev curr "<<iEv<<" event header id "<<((BmnEventHeader*) eventHeader->At(0))->GetEventId()<<endl;

    BmnDchDigit* digit = NULL;

    if(Int_t(t0Digits->GetEntriesFast()) != 1 && Int_t(bc2Digits->GetEntriesFast()) != 1 )continue;

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
    bool xa86 = false;
    bool xa87 = false;
    bool xa88 = false;
    bool xb87 = false;
    bool xb88 = false;

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
	if(digit->GetWireNumber() > 239 ) {
	  wirenr_va1[it_va1] = digit->GetWireNumber() - 128;
	}else{
	  wirenr_va1[it_va1] = digit->GetWireNumber();
	}
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
	if(digit->GetWireNumber() > 239 ) {
	  wirenr_vb1[it_vb1] = digit->GetWireNumber() - 128;
	}else{
	  wirenr_vb1[it_vb1] = digit->GetWireNumber();
	}
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
	if(digit->GetWireNumber() > 239 ) { wirenr_ua1[it_ua1] = digit->GetWireNumber() - 128;
	}else{wirenr_ua1[it_ua1] = digit->GetWireNumber();}
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
	if(digit->GetWireNumber() > 239 ) { wirenr_ub1[it_ub1] = digit->GetWireNumber() - 128;
	}else{wirenr_ub1[it_ub1] = digit->GetWireNumber();}
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

	if(digit->GetWireNumber() > 239 ) {wirenr_ya1[it_ya1] = digit->GetWireNumber()-128;
	}else{wirenr_ya1[it_ya1] = digit->GetWireNumber();}
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
	if(digit->GetWireNumber() > 239 ) {wirenr_yb1[it_yb1] = digit->GetWireNumber()-128;
	}else{wirenr_yb1[it_yb1] = digit->GetWireNumber();}
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
	if(digit->GetWireNumber() > 239 ) {
	  wirenr_xa1[it_xa1] = digit->GetWireNumber() - 128;
	}else{
	  wirenr_xa1[it_xa1] = digit->GetWireNumber();
	}
	hXa_ccup->Fill(wirenr_xa1[it_xa1]);
	//	cout<<" xa "<<wirenr_xa1[it_xa1]<<endl;
	if(wirenr_xa1[it_xa1] == 86)xa86 = true;
        if(wirenr_xa1[it_xa1] == 88)xa88 = true;

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
	if(digit->GetWireNumber() > 239 ) {
	  wirenr_xb1[it_xb1] = digit->GetWireNumber() - 128;
	}else{
	  wirenr_xb1[it_xb1] = digit->GetWireNumber();
	}
        hXb_ccup->Fill(wirenr_xb1[it_xb1]);
        //cout<<" xb "<<wirenr_xb1[it_xb1]<<endl;

	time_xb1[it_xb1] = times;
	//missing xa wire fix
	if(wirenr_xb1[it_xb1] == 87)xb87 = true;
        if(wirenr_xb1[it_xb1] == 88)xb88 = true;

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
	if(digit->GetWireNumber() > 239 ) { wirenr_va2[it_va2] = digit->GetWireNumber()-128;
	}else{wirenr_va2[it_va2] = digit->GetWireNumber();}
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
	if(digit->GetWireNumber() > 239 ) {
	  wirenr_vb2[it_vb2] = digit->GetWireNumber() - 128;
	}else{
	  wirenr_vb2[it_vb2] = digit->GetWireNumber();
	} 
	time_vb2[it_vb2] = times;
	//cout<<" vb2 wire "<<wirenr_vb2[it_vb2]<<" time  "<< time_vb2[it_vb2]<<endl;
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
	if(digit->GetWireNumber() > 239 ) {wirenr_ua2[it_ua2] = digit->GetWireNumber()-128;
	}else{wirenr_ua2[it_ua2] = digit->GetWireNumber();}
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
	if(digit->GetWireNumber() > 239 ) {wirenr_ub2[it_ub2] = digit->GetWireNumber()-128;
	}else{wirenr_ub2[it_ub2] = digit->GetWireNumber();}
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
	if(digit->GetWireNumber() > 239 ) {wirenr_ya2[it_ya2] = digit->GetWireNumber()-128;
	}else{wirenr_ya2[it_ya2] = digit->GetWireNumber();}
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
	if(digit->GetWireNumber() > 239 ) {wirenr_yb2[it_yb2] = digit->GetWireNumber()-128;
	}else{wirenr_yb2[it_yb2] = digit->GetWireNumber();}
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
	if(digit->GetWireNumber() > 239 ) {wirenr_xa2[it_xa2] = digit->GetWireNumber()-128;
	}else{wirenr_xa2[it_xa2] = digit->GetWireNumber();}
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
	if(digit->GetWireNumber() > 239 ) {wirenr_xb2[it_xb2] = digit->GetWireNumber()-128;
	}else{wirenr_xb2[it_xb2] = digit->GetWireNumber();}
	time_xb2[it_xb2] = times;
	it_xb2++;
	break;

      }
    }//for digis in event iDig

    if (!goodEv)continue;
    float minChi2_up = 50;
    float minChi2_down = 30;

    double rh_segDC1[8][50];
    double rh_segDC2[8][50];
    double rh_sigm_segDC1[8][50];
    double rh_sigm_segDC2[8][50];
    
    for (int i = 0; i < 8; i++) {
      for (int j = 0; j < 50; j++) {
        rh_segDC1[i][j] = -999.;
        rh_segDC2[i][j] = -999.;
        rh_sigm_segDC1[i][j] = 1.;
        rh_sigm_segDC2[i][j] = 1.;
      }
    }

    double xDC1_glob[50];
    double yDC1_glob[50];
    double xDC1_glob_to_DC2[50];
    double yDC1_glob_to_DC2[50];
    double xDC2_glob[50];
    double yDC2_glob[50];
    double chi2_DC1[50];
    double chi2_DC2[50];
    int size_segDC1[50];
    int size_segDC2[50];
    double sigm_segDC1_par_ax[50];
    double sigm_segDC1_par_bx[50];
    double sigm_segDC2_par_ax[50];
    double sigm_segDC2_par_bx[50];
    double sigm_segDC1_par_ay[50];
    double sigm_segDC1_par_by[50];
    double sigm_segDC2_par_ay[50];
    double sigm_segDC2_par_by[50];
    for (int j = 0; j < 50; j++) {
      xDC1_glob[j] = -999.;
      yDC1_glob[j] = -999.;
      xDC1_glob_to_DC2[j] = -999.;
      yDC1_glob_to_DC2[j] = -999.;
      xDC2_glob[j] = -999.;
      yDC2_glob[j] = -999.;
      sigm_segDC1_par_ax[j] = -999.;
      sigm_segDC2_par_ax[j] = -999.;
      sigm_segDC1_par_bx[j] = -999.;
      sigm_segDC2_par_bx[j] = -999.;
      sigm_segDC1_par_ay[j] = -999.;
      sigm_segDC2_par_ay[j] = -999.;
      sigm_segDC1_par_by[j] = -999.;
      sigm_segDC2_par_by[j] = -999.;
      size_segDC1[j] = 0;
      size_segDC2[j] = 0;
      chi2_DC1[j] = 999.;
      chi2_DC2[j] = 999.;
    }
    
    double par_ab1[4][50];
    double par_ab2[4][50];
    for (int i = 0; i < 4; i++) {
      for (int j = 0; j < 50; j++) {
        par_ab1[i][j] = -999.;
        par_ab2[i][j] = -999.;

      }
    }


    int nDC1_segments = 0;
    int nDC2_segments = 0;
    bool has7DC1 = false;
    bool has7DC2 = false;

    double x1_ab[2][50];
    double y1_ab[2][50];
    double u1_ab[2][50];
    double v1_ab[2][50];
    double sigm_x1_ab[2][50];
    double sigm_y1_ab[2][50];
    double sigm_u1_ab[2][50];
    double sigm_v1_ab[2][50];
    double x2_ab[2][50];
    double y2_ab[2][50];
    double u2_ab[2][50];
    double v2_ab[2][50];
    double sigm_x2_ab[2][50];
    double sigm_y2_ab[2][50];
    double sigm_u2_ab[2][50];
    double sigm_v2_ab[2][50];

    for (int i = 0; i < 2; i++) {
      for (int j = 0; j < 50; j++) {
	x1_ab[i][j] = -999.;
        y1_ab[i][j] = -999.;
        u1_ab[i][j] = -999.;
        v1_ab[i][j] = -999.;
        x2_ab[i][j] = -999.;
        y2_ab[i][j] = -999.;
        u2_ab[i][j] = -999.;
        v2_ab[i][j] = -999.;
        sigm_x1_ab[i][j] = 1.;
        sigm_y1_ab[i][j] = 1.;
        sigm_u1_ab[i][j] = 1.;
        sigm_v1_ab[i][j] = 1.;
        sigm_x2_ab[i][j] = 1.;
        sigm_y2_ab[i][j] = 1.;
        sigm_u2_ab[i][j] = 1.;
        sigm_v2_ab[i][j] = 1.;

      }
    }

    //single hits on ab-plane
    double x1_single[2][50];
    double y1_single[2][50];
    double u1_single[2][50];
    double v1_single[2][50];
    double sigm_x1_single[2][50];
    double sigm_y1_single[2][50];
    double sigm_u1_single[2][50];
    double sigm_v1_single[2][50];
    double x2_single[2][50];
    double y2_single[2][50];
    double u2_single[2][50];
    double v2_single[2][50];
    double sigm_x2_single[2][50];
    double sigm_y2_single[2][50];
    double sigm_u2_single[2][50];
    double sigm_v2_single[2][50];

    for (int i = 0; i < 2; i++) {
      for (int j = 0; j < 50; j++) {
	x1_single[i][j] = -999.;
        y1_single[i][j] = -999.;
        u1_single[i][j] = -999.;
        v1_single[i][j] = -999.;
        x2_single[i][j] = -999.;
        y2_single[i][j] = -999.;
        u2_single[i][j] = -999.;
        v2_single[i][j] = -999.;
        sigm_x1_single[i][j] = 1.;
        sigm_y1_single[i][j] = 1.;
        sigm_u1_single[i][j] = 1.;
        sigm_v1_single[i][j] = 1.;
        sigm_x2_single[i][j] = 1.;
        sigm_y2_single[i][j] = 1.;
        sigm_u2_single[i][j] = 1.;
        sigm_v2_single[i][j] = 1.;

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

    double xa1_pm[2] = {-999.};
    double xb1_pm[2] = {-999.};
    double ya1_pm[2] = {-999.};
    double yb1_pm[2] = {-999.};
    double ua1_pm[2] = {-999.};
    double ub1_pm[2] = {-999.};
    double va1_pm[2] = {-999.};
    double vb1_pm[2] = {-999.};
    //new reco
     
    //   ---   X   ---

    if(!xa86 && xb87 && !xa87){
      wirenr_xa1[it_xa1] = 87;
      time_xa1[it_xa1] = 1;
      it_xa1++;
      xa87 = true;
      //cout<<"added xa"<<endl;
    }
    if(!xa88 && xb88 && !xa87){
      wirenr_xa1[it_xa1] = 87;
      time_xa1[it_xa1] = 1;
      it_xa1++;
      xa87 = true;
    }




    for (Int_t i = 0; i < it_xa1; ++i) {
      int func_nr_a = -1;
      for(int t_it = 0; t_it<4;t_it++){
	if(time_xa1[i] >= t_dc[t_it][0] && time_xa1[i] < t_dc[t_it+1][0]){
	  func_nr_a = t_it;
	  break;
	}
      }
      if(func_nr_a == -1)continue;

      for (Int_t j = 0; j < it_xb1; ++j) {
	if(pair_x1 > 48)break;
	if (( wirenr_xa1[i] != wirenr_xb1[j] && wirenr_xa1[i] != wirenr_xb1[j]+1)) continue;

	int func_nr_b = -1;                                                    

	double time_xa = time_xa1[i];
	double d_a = 0.;
	double d_b = 0.;

	d_a = scale[0]*( pol_par_dc[func_nr_a][0][0] +  pol_par_dc[func_nr_a][1][0]*time_xa +  pol_par_dc[func_nr_a][2][0]*time_xa*time_xa + pol_par_dc[func_nr_a][3][0]*time_xa*time_xa*time_xa + pol_par_dc[func_nr_a][4][0]*time_xa*time_xa*time_xa*time_xa);

	  
	for(int t_it = 0; t_it<4;t_it++){
	  if(time_xb1[j] >= t_dc[t_it][1] && time_xb1[j] < t_dc[t_it+1][1]){
	    func_nr_b = t_it;
	    break;
	  }
	}
	if(func_nr_b == -1)continue;

	time_xa = time_xb1[j];

	d_b = scale[1]*( pol_par_dc[func_nr_b][0][1] +  pol_par_dc[func_nr_b][1][1]*time_xa +  pol_par_dc[func_nr_b][2][1]*time_xa*time_xa + pol_par_dc[func_nr_b][3][1]*time_xa*time_xa*time_xa + pol_par_dc[func_nr_b][4][1]*time_xa*time_xa*time_xa*time_xa);

	//missing xa wire fix
	if(wirenr_xa1[i] == 87){
	   d_a = 0.5 - d_b; 
	   //cout<<" d_a "<<d_a<<" d_b "<<d_b<<endl;
	}
	xa1_pm[0] =  wirenr_xa1[i]-120.+ d_a;
	xa1_pm[1] =  wirenr_xa1[i]-120.- d_a;
	xb1_pm[0] =  wirenr_xb1[j]-119.5 + d_b;
	xb1_pm[1] =  wirenr_xb1[j]-119.5 - d_b;

	double dmin1 = 999;
	for(int k = 0; k<2;k++){
	  for(int m = 0; m<2;m++){
	    if (  fabs(xa1_pm[k]- xb1_pm[m]) < dmin1){
	      dmin1 = fabs(xa1_pm[k]- xb1_pm[m]);
	      x1_ab[0][pair_x1] = xa1_pm[k];
	      x1_ab[1][pair_x1] = xb1_pm[m];
	    }
	  }
	}
	//	if(wirenr_xa1[i] == 87)	cout<<" xa "<< x1_ab[0][pair_x1]<<" xb "<<x1_ab[1][pair_x1]<<endl;
	if(d_a < 0.02){sigm_x1_ab[0][pair_x1] = 0.08*0.08;}
	if(d_a >= 0.02 && d_a < 0.1){sigm_x1_ab[0][pair_x1] = 0.06*0.06;}
	if(d_a >= 0.1 && d_a < 0.4){sigm_x1_ab[0][pair_x1] = 0.025*0.025;}
	if(d_a >= 0.4 && d_a < 0.41){sigm_x1_ab[0][pair_x1] = 0.08*0.08;}
	if(d_a >= 0.41){sigm_x1_ab[0][pair_x1] = 0.10*0.10;}

	//missing xa wire fix
        if(wirenr_xa1[i] == 87)sigm_x1_ab[0][pair_x1] = 0.083;

	if(d_b < 0.02){sigm_x1_ab[1][pair_x1] = 0.08*0.08;}
	if(d_b >= 0.02 && d_b < 0.1){sigm_x1_ab[1][pair_x1] = 0.06*0.06;}
	if(d_b >= 0.1 && d_b < 0.4){sigm_x1_ab[1][pair_x1] = 0.025*0.025;}
	if(d_b >= 0.4 && d_b < 0.41){sigm_x1_ab[1][pair_x1] = 0.08*0.08;}
	if(d_b >= 0.41){sigm_x1_ab[1][pair_x1] = 0.10*0.10;}
	pair_x1++;

	used_xa1[i] = true;
	used_xb1[j] = true;

      }// j for2
    } // i for1  X.

      //reconstruct single X-plane hits

    for (Int_t i = 0; i < it_xa1; ++i) {
      if ( used_xa1[i]) continue;

      int func_nr_a = -1;                                                    
      for(int t_it = 0; t_it<4;t_it++){
	if(time_xa1[i] >= t_dc[t_it][0] && time_xa1[i] < t_dc[t_it+1][0]){
	  func_nr_a = t_it;
	  break;
	}
      }
      if(func_nr_a == -1)continue;
      double time_xa = time_xa1[i];
      double d_a = 0.;
      double d_b = 0.;
      
      d_a = scale[0]*( pol_par_dc[func_nr_a][0][0] +  pol_par_dc[func_nr_a][1][0]*time_xa +  pol_par_dc[func_nr_a][2][0]*time_xa*time_xa + pol_par_dc[func_nr_a][3][0]*time_xa*time_xa*time_xa + pol_par_dc[func_nr_a][4][0]*time_xa*time_xa*time_xa*time_xa);

      x1_single[0][single_xa1] = wirenr_xa1[i]-120.+ d_a;
      x1_single[0][single_xa1+1] =  wirenr_xa1[i]-120.- d_a;

      if(d_a < 0.02){sigm_x1_single[0][single_xa1] = 0.08*0.08;sigm_x1_single[0][single_xa1+1] = 0.08*0.08;}
      if(d_a >= 0.02 && d_a < 0.1){sigm_x1_single[0][single_xa1] = 0.06*0.06;sigm_x1_single[0][single_xa1+1] = 0.06*0.06;}
      if(d_a >= 0.1 && d_a < 0.4){sigm_x1_single[0][single_xa1] = 0.025*0.025;sigm_x1_single[0][single_xa1+1] = 0.025*0.025;}
      if(d_a >= 0.4 && d_a < 0.41){sigm_x1_single[0][single_xa1] = 0.08*0.08;sigm_x1_single[0][single_xa1+1] = 0.08*0.08;}
      if(d_a >= 0.41){sigm_x1_single[0][single_xa1] = 0.10*0.10;sigm_x1_single[0][single_xa1+1] = 0.10*0.10;}

      single_xa1 += 2;
    }//for single xa

    for (Int_t j = 0; j < it_xb1; ++j) {
      if ( used_xb1[j]) continue;
      int func_nr_b = -1;                                                    

      double time_xa = time_xb1[j];
      double d_a = 0.; double d_b = 0.;
      for(int t_it = 0; t_it<4;t_it++){
	if(time_xb1[j] >= t_dc[t_it][1] && time_xb1[j] < t_dc[t_it+1][1]){
	  func_nr_b = t_it;
	  break;
	}
      }
      if(func_nr_b == -1)continue;
      time_xa = time_xb1[j];
      d_b = scale[1]*( pol_par_dc[func_nr_b][0][1] +  pol_par_dc[func_nr_b][1][1]*time_xa +  pol_par_dc[func_nr_b][2][1]*time_xa*time_xa + pol_par_dc[func_nr_b][3][1]*time_xa*time_xa*time_xa + pol_par_dc[func_nr_b][4][1]*time_xa*time_xa*time_xa*time_xa);
    
      x1_single[1][single_xb1] = wirenr_xb1[j]-119.5 + d_b;
      x1_single[1][single_xb1+1] =  wirenr_xb1[j]-119.5 - d_b;
	
      if(d_b < 0.02){sigm_x1_single[1][single_xb1] = 0.08*0.08;sigm_x1_single[1][single_xb1+1] = 0.08*0.08;}
      if(d_b >= 0.02 && d_b < 0.1){sigm_x1_single[1][single_xb1] = 0.06*0.06;sigm_x1_single[1][single_xb1+1] = 0.06*0.06;}
      if(d_b >= 0.1 && d_b < 0.4){sigm_x1_single[1][single_xb1] = 0.0250*0.0250;sigm_x1_single[1][single_xb1+1] = 0.0250*0.0250;}
      if(d_b >= 0.4 && d_b < 0.41){sigm_x1_single[1][single_xb1] = 0.08*0.08;sigm_x1_single[1][single_xb1+1] = 0.08*0.08;}
      if(d_b >= 0.41){sigm_x1_single[1][single_xb1] = 0.10*0.10;sigm_x1_single[1][single_xb1+1] = 0.10*0.10;}

      single_xb1 += 2;

    } // i for1  X. sigm_y1_single[i][j]1

      //   ----   Y   ---sigm_y1_single

    for (Int_t i = 0; i < it_ya1; ++i) {
      int func_nr_a = -1;
      for(int t_it = 0; t_it<4;t_it++){
	if(time_ya1[i] >= t_dc[t_it][2] && time_ya1[i] < t_dc[t_it+1][2]){
	  func_nr_a = t_it;
	  break;
	}
      }
      if(func_nr_a == -1)continue;

      for (Int_t j = 0; j < it_yb1; ++j) {
	if(pair_y1 > 48)break;
	if (( wirenr_ya1[i] != wirenr_yb1[j] && wirenr_ya1[i] != wirenr_yb1[j]+1)) continue;
	int func_nr_b = -1;

	double d_a = 0.; double d_b = 0.;

	d_a = scale[2]*( pol_par_dc[func_nr_a][0][2] +  pol_par_dc[func_nr_a][1][2]*time_ya1[i] +  pol_par_dc[func_nr_a][2][2]*time_ya1[i]*time_ya1[i] + pol_par_dc[func_nr_a][3][2]*time_ya1[i]*time_ya1[i]*time_ya1[i] + pol_par_dc[func_nr_a][4][2]*time_ya1[i]*time_ya1[i]*time_ya1[i]*time_ya1[i]);

	for(int t_it = 0; t_it<4;t_it++){
	  if(time_yb1[j] >= t_dc[t_it][3] && time_yb1[j] < t_dc[t_it+1][3]){
	    func_nr_b = t_it;
	    break;
	  }
	}
	if(func_nr_b == -1)continue;

	d_b = scale[3]*( pol_par_dc[func_nr_b][0][3] +  pol_par_dc[func_nr_b][1][3]*time_yb1[j] +  pol_par_dc[func_nr_b][2][3]*time_yb1[j]*time_yb1[j] + pol_par_dc[func_nr_b][3][3]*time_yb1[j]*time_yb1[j]*time_yb1[j] + pol_par_dc[func_nr_b][4][3]*time_yb1[j]*time_yb1[j]*time_yb1[j]*time_yb1[j]);

	ya1_pm[0] =  wirenr_ya1[i]-119.+ d_a;
	ya1_pm[1] =  wirenr_ya1[i]-119.- d_a;
	yb1_pm[0] =  wirenr_yb1[j]-118.5 + d_b;
	yb1_pm[1] =  wirenr_yb1[j]-118.5 - d_b;

	double dmin1 = 999;

	for(int k = 0; k<2;k++){
	  for(int m = 0; m<2;m++){
	    if (  fabs(ya1_pm[k]- yb1_pm[m]) < dmin1){
	      dmin1 = fabs(ya1_pm[k]- yb1_pm[m]);
	      y1_ab[0][pair_y1] = ya1_pm[k];
	      y1_ab[1][pair_y1] = yb1_pm[m];
	    }
	  }
	}
	if(d_a < 0.02){sigm_y1_ab[0][pair_y1] = 0.08*0.08;}
	if(d_a >= 0.02 && d_a < 0.1){sigm_y1_ab[0][pair_y1] = 0.06*0.06;}
	if(d_a >= 0.1 && d_a < 0.4){sigm_y1_ab[0][pair_y1] = 0.025*0.025;}
	if(d_a >= 0.4 && d_a < 0.41){sigm_y1_ab[0][pair_y1] = 0.08*0.08;}
	if(d_a >= 0.41){sigm_y1_ab[0][pair_y1] = 0.10*0.10;}

	if(d_b < 0.02){sigm_y1_ab[1][pair_y1] = 0.08*0.08;}
	if(d_b >= 0.02 && d_b < 0.1){sigm_y1_ab[1][pair_y1] = 0.06*0.06;}
	if(d_b >= 0.1 && d_b < 0.4){sigm_y1_ab[1][pair_y1] = 0.025*0.025;}
	if(d_b >= 0.4 && d_b < 0.41){sigm_y1_ab[1][pair_y1] = 0.08*0.08;}
	if(d_b >= 0.41){sigm_y1_ab[1][pair_y1] = 0.10*0.10;}
	pair_y1++;


	used_ya1[i] = true;
	used_yb1[j] = true;
         
 
      }// j for2
    } // i for1  Y.

      //reconstruct single Y-plane hits

    for (Int_t i = 0; i < it_ya1; ++i) {
      if ( used_ya1[i]) continue;
      double d_a = 0.;
      int func_nr_a = -1;                                                    
      for(int t_it = 0; t_it<4;t_it++){
	if(time_ya1[i] >= t_dc[t_it][2] && time_ya1[i] < t_dc[t_it+1][2]){
	  func_nr_a = t_it;
	  break;
	}
      }
      if(func_nr_a == -1)continue;

      d_a = scale[2]*( pol_par_dc[func_nr_a][0][2] +  pol_par_dc[func_nr_a][1][2]*time_ya1[i] +  pol_par_dc[func_nr_a][2][2]*time_ya1[i]*time_ya1[i] + pol_par_dc[func_nr_a][3][2]*time_ya1[i]*time_ya1[i]*time_ya1[i] + pol_par_dc[func_nr_a][4][2]*time_ya1[i]*time_ya1[i]*time_ya1[i]*time_ya1[i]);
   
	
      y1_single[0][single_ya1] = wirenr_ya1[i]-119.+ d_a;
      y1_single[0][single_ya1+1] =  wirenr_ya1[i]-119.- d_a;

      if(d_a < 0.02){sigm_y1_single[0][single_ya1] = 0.08*0.08;sigm_y1_single[0][single_ya1+1] = 0.08*0.08;}
      if(d_a >= 0.02 && d_a < 0.1){sigm_y1_single[0][single_ya1] = 0.06*0.06;sigm_y1_single[0][single_ya1+1] = 0.06*0.06;}
      if(d_a >= 0.1 && d_a < 0.4){sigm_y1_single[0][single_ya1] = 0.025*0.025;sigm_y1_single[0][single_ya1+1] = 0.025*0.025;}
      if(d_a >= 0.4 && d_a < 0.41){sigm_y1_single[0][single_ya1] = 0.08*0.08;sigm_y1_single[0][single_ya1+1] = 0.08*0.08;}
      if(d_a >= 0.41){sigm_y1_single[0][single_ya1] = 0.10*0.10;sigm_y1_single[0][single_ya1+1] = 0.10*0.10;}
      single_ya1 += 2;
    }//for single ya

    for (Int_t j = 0; j < it_yb1; ++j) {
      if ( used_yb1[j]) continue;
      double time_ya = time_yb1[j];
      int func_nr_b = -1;
      for(int t_it = 0; t_it<4;t_it++){
	if(time_yb1[j] >= t_dc[t_it][3] && time_yb1[j] < t_dc[t_it+1][3]){
	  func_nr_b = t_it;
	  break;
	}
      }
      if(func_nr_b == -1)continue;

      double d_b = 0.;
      d_b = scale[3]*( pol_par_dc[func_nr_b][0][3] +  pol_par_dc[func_nr_b][1][3]*time_yb1[j] +  pol_par_dc[func_nr_b][2][3]*time_yb1[j]*time_yb1[j] + pol_par_dc[func_nr_b][3][3]*time_yb1[j]*time_yb1[j]*time_yb1[j] + pol_par_dc[func_nr_b][4][3]*time_yb1[j]*time_yb1[j]*time_yb1[j]*time_yb1[j]);
   
      y1_single[1][single_yb1] = wirenr_yb1[j]-118.5 + d_b;
      y1_single[1][single_yb1+1] =  wirenr_yb1[j]-118.5 - d_b;

      if(d_b < 0.02){sigm_y1_single[1][single_yb1] = 0.08*0.08;sigm_y1_single[1][single_yb1+1] = 0.08*0.08;}
      if(d_b >= 0.02 && d_b < 0.1){sigm_y1_single[1][single_yb1] = 0.06*0.06;sigm_y1_single[1][single_yb1+1] = 0.06*0.06;}
      if(d_b >= 0.1 && d_b < 0.4){sigm_y1_single[1][single_yb1] = 0.025*0.025;sigm_y1_single[1][single_yb1+1] = 0.025*0.025;}
      if(d_b >= 0.4 && d_b < 0.41){sigm_y1_single[1][single_yb1] = 0.10*0.10;sigm_y1_single[1][single_yb1+1] = 0.10*0.10;}
      if(d_b >= 0.41){sigm_y1_single[1][single_yb1] = 0.10*0.10;sigm_y1_single[1][single_yb1+1] = 0.10*0.10;}
      single_yb1 += 2;
	
    } // i for1  Y.

      //   ----   U   ---

    for (Int_t i = 0; i < it_ua1; ++i) {
      int func_nr_a = -1;
      for(int t_it = 0; t_it<4;t_it++){
	if(time_ua1[i] >= t_dc[t_it][4] && time_ua1[i] < t_dc[t_it+1][4]){
	  func_nr_a = t_it;
	  break;
	}
      }
      if(func_nr_a == -1)continue;

      for (Int_t j = 0; j < it_ub1; ++j) {
	if(pair_u1 > 48)break;
	if (( wirenr_ua1[i] != wirenr_ub1[j] && wirenr_ua1[i] != wirenr_ub1[j]+1)) continue;
	double d_a = 0.; double d_b = 0.;

	int func_nr_b = -1;

	d_a = scale[4]*( pol_par_dc[func_nr_a][0][4] +  pol_par_dc[func_nr_a][1][4]*time_ua1[i] +  pol_par_dc[func_nr_a][2][4]*time_ua1[i]*time_ua1[i] + pol_par_dc[func_nr_a][3][4]*time_ua1[i]*time_ua1[i]*time_ua1[i] + pol_par_dc[func_nr_a][4][4]*time_ua1[i]*time_ua1[i]*time_ua1[i]*time_ua1[i]);


	for(int t_it = 0; t_it<4;t_it++){
	  if(time_ub1[j] >= t_dc[t_it][5] && time_ub1[j] < t_dc[t_it+1][5]){
	    func_nr_b = t_it;
	    break;
	  }
	}
	if(func_nr_b == -1)continue;

	d_b = scale[5]*( pol_par_dc[func_nr_b][0][5] +  pol_par_dc[func_nr_b][1][5]*time_ub1[j] +  pol_par_dc[func_nr_b][2][5]*time_ub1[j]*time_ub1[j] + pol_par_dc[func_nr_b][3][5]*time_ub1[j]*time_ub1[j]*time_ub1[j] + pol_par_dc[func_nr_b][4][5]*time_ub1[j]*time_ub1[j]*time_ub1[j]*time_ub1[j]);

	ua1_pm[0] =  wirenr_ua1[i]-119.+ d_a;
	ua1_pm[1] =  wirenr_ua1[i]-119.- d_a;
	ub1_pm[0] =  wirenr_ub1[j]-118.5 + d_b;
	ub1_pm[1] =  wirenr_ub1[j]-118.5 - d_b;

	double dmin1 = 999;

	for(int k = 0; k<2;k++){
	  for(int m = 0; m<2;m++){
	    if (  fabs(ua1_pm[k]- ub1_pm[m]) < dmin1){
	      dmin1 = fabs(ua1_pm[k]- ub1_pm[m]);
	      u1_ab[0][pair_u1] = ua1_pm[k];
	      u1_ab[1][pair_u1] = ub1_pm[m];
	    }
	  }
	}
	if(d_a < 0.02){sigm_u1_ab[0][pair_u1] = 0.08*0.08;}
	if(d_a >= 0.02 && d_a < 0.1){sigm_u1_ab[0][pair_u1] = 0.06*0.06;}
	if(d_a >= 0.1 && d_a < 0.4){sigm_u1_ab[0][pair_u1] = 0.025*0.025;}
	if(d_a >= 0.4 && d_a < 0.41){sigm_u1_ab[0][pair_u1] = 0.08*0.08;}
	if(d_a >= 0.41){sigm_u1_ab[0][pair_u1] = 0.10*0.10;}

	if(d_b < 0.02){sigm_u1_ab[1][pair_u1] = 0.08*0.08;}
	if(d_b >= 0.02 && d_b < 0.1){sigm_u1_ab[1][pair_u1] = 0.06*0.06;}
	if(d_b >= 0.1 && d_b < 0.4){sigm_u1_ab[1][pair_u1] = 0.025*0.025;}
	if(d_b >= 0.4 && d_b < 0.41){sigm_u1_ab[1][pair_u1] = 0.08*0.08;}
	if(d_b >= 0.41){sigm_u1_ab[1][pair_u1] = 0.10*0.10;}
	pair_u1++;

	used_ua1[i] = true;
	used_ub1[j] = true;

 
      }// j for2
    } // i for1  U.

      //reconstruct single U-plane hits

    for (Int_t i = 0; i < it_ua1; ++i) {
      if ( used_ua1[i]) continue;
    
      double d_a = 0.; double d_b = 0.;
      int func_nr_a = -1;                                                    
      for(int t_it = 0; t_it<4;t_it++){
	if(time_ua1[i] >= t_dc[t_it][4] && time_ua1[i] < t_dc[t_it+1][4]){
	  func_nr_a = t_it;
	  break;
	}
      }
      if(func_nr_a == -1)continue;
      d_a = scale[4]*( pol_par_dc[func_nr_a][0][4] +  pol_par_dc[func_nr_a][1][4]*time_ua1[i] +  pol_par_dc[func_nr_a][2][4]*time_ua1[i]*time_ua1[i] + pol_par_dc[func_nr_a][3][4]*time_ua1[i]*time_ua1[i]*time_ua1[i] + pol_par_dc[func_nr_a][4][4]*time_ua1[i]*time_ua1[i]*time_ua1[i]*time_ua1[i]);
    
      u1_single[0][single_ua1] = (wirenr_ua1[i]-119.+ d_a);
      u1_single[0][single_ua1+1] = ( wirenr_ua1[i]-119.- d_a);
      if(d_a < 0.02){sigm_u1_single[0][single_ua1] = 0.08*0.08;sigm_u1_single[0][single_ua1+1] = 0.08*0.08;}
      if(d_a >= 0.02 && d_a < 0.1){sigm_u1_single[0][single_ua1] = 0.06*0.06;sigm_u1_single[0][single_ua1+1] = 0.06*0.06;}
      if(d_a >= 0.1 && d_a < 0.4){sigm_u1_single[0][single_ua1] = 0.025*0.025;sigm_u1_single[0][single_ua1+1] = 0.025*0.025;}
      if(d_a >= 0.4 && d_a < 0.41){sigm_u1_single[0][single_ua1] = 0.08*0.08;sigm_u1_single[0][single_ua1+1] = 0.08*0.08;}
      if(d_a >= 0.41){sigm_u1_single[0][single_ua1] = 0.10*0.10;sigm_u1_single[0][single_ua1+1] = 0.10*0.10;}
      single_ua1 += 2;
    }//for single ua

    for (Int_t j = 0; j < it_ub1; ++j) {
      if ( used_ub1[j]) continue;
      int func_nr_b = -1;                                                    
      double d_a = 0.; double d_b = 0.;
      for(int t_it = 0; t_it<4;t_it++){
	if(time_ub1[j] >= t_dc[t_it][5] && time_ub1[j] < t_dc[t_it+1][5]){
	  func_nr_b = t_it;
	  break;
	}
      }
      if(func_nr_b == -1)continue;
      d_b = scale[5]*( pol_par_dc[func_nr_b][0][5] +  pol_par_dc[func_nr_b][1][5]*time_ub1[j] +  pol_par_dc[func_nr_b][2][5]*time_ub1[j]*time_ub1[j] + pol_par_dc[func_nr_b][3][5]*time_ub1[j]*time_ub1[j]*time_ub1[j] + pol_par_dc[func_nr_b][4][5]*time_ub1[j]*time_ub1[j]*time_ub1[j]*time_ub1[j]);
      
      u1_single[1][single_ub1] = (wirenr_ub1[j]-118.5 + d_b);
      u1_single[1][single_ub1+1] =  (wirenr_ub1[j]-118.5 - d_b);

      //hdev_occup[1]->Fill(d_b);
      if(d_b < 0.02){sigm_u1_single[1][single_ub1] = 0.08*0.08;sigm_u1_single[1][single_ub1+1] = 0.08*0.08;}
      if(d_b >= 0.02 && d_b < 0.1){sigm_u1_single[1][single_ub1] = 0.06*0.06;sigm_u1_single[1][single_ub1+1] = 0.06*0.06;}
      if(d_b >= 0.1 && d_b < 0.4){sigm_u1_single[1][single_ub1] = 0.025*0.025;sigm_u1_single[1][single_ub1+1] = 0.025*0.025;}
      if(d_b >= 0.4 && d_b < 0.41){sigm_u1_single[1][single_ub1] = 0.08*0.08;sigm_u1_single[1][single_ub1+1] = 0.08*0.08;}
      if(d_b >= 0.41){sigm_u1_single[1][single_ub1] = 0.10*0.10;sigm_u1_single[1][single_ub1+1] = 0.10*0.10;}
      single_ub1 += 2;

    
    } // i for1  U.


      //   ----   V   ---

    for (Int_t i = 0; i < it_va1; ++i) {
      int func_nr_a = -1;
      for(int t_it = 0; t_it<4;t_it++){
	if(time_va1[i] >= t_dc[t_it][6] && time_va1[i] < t_dc[t_it+1][6]){
	  func_nr_a = t_it;
	  break;
	}
      }
      if(func_nr_a == -1)continue;
      for (Int_t j = 0; j < it_vb1; ++j) {

	if(pair_v1 > 48)break;
	if (( wirenr_va1[i] != wirenr_vb1[j] && wirenr_va1[i] != wirenr_vb1[j]+1)) continue;

	double d_a = 0.; double d_b = 0.;

	int func_nr_b = -1;

	 
	d_a = scale[6]*( pol_par_dc[func_nr_a][0][6] +  pol_par_dc[func_nr_a][1][6]*time_va1[i] +  pol_par_dc[func_nr_a][2][6]*time_va1[i]*time_va1[i] + pol_par_dc[func_nr_a][3][6]*time_va1[i]*time_va1[i]*time_va1[i] + pol_par_dc[func_nr_a][4][6]*time_va1[i]*time_va1[i]*time_va1[i]*time_va1[i]);
	//if(time_va1[i]<t_dc[0][6])d_a = 0.0;
        //if(time_va1[i]>t_dc[3][6])d_a = 0.5;

	for(int t_it = 0; t_it<4;t_it++){
	  if(time_vb1[j] >= t_dc[t_it][7] && time_vb1[j] < t_dc[t_it+1][7]){
	    func_nr_b = t_it;
	    break;
	  }
	}
	if(func_nr_b == -1)continue;
	d_b = scale[7]*(pol_par_dc[func_nr_b][0][7] +  pol_par_dc[func_nr_b][1][7]*time_vb1[j] +  pol_par_dc[func_nr_b][2][7]*time_vb1[j]*time_vb1[j] + pol_par_dc[func_nr_b][3][7]*time_vb1[j]*time_vb1[j]*time_vb1[j] + pol_par_dc[func_nr_b][4][7]*time_vb1[j]*time_vb1[j]*time_vb1[j]*time_vb1[j]);

	va1_pm[0] =  wirenr_va1[i]-119.0 + d_a;
	va1_pm[1] =  wirenr_va1[i]-119.0 - d_a;
	vb1_pm[0] =  wirenr_vb1[j]-118.5 + d_b;
	vb1_pm[1] =  wirenr_vb1[j]-118.5 - d_b;
	double dmin1 = 999;

	for(int k = 0; k<2;k++){
	  for(int m = 0; m<2;m++){
	    if (  fabs(va1_pm[k]- vb1_pm[m]) < dmin1){
	      dmin1 = fabs(va1_pm[k]- vb1_pm[m]);
	      v1_ab[0][pair_v1] = va1_pm[k];
	      v1_ab[1][pair_v1] = vb1_pm[m];
	    }
	  }
	}
	if(d_a < 0.02){sigm_v1_ab[0][pair_v1] = 0.08*0.08;}
	if(d_a >= 0.02 && d_a < 0.1){sigm_v1_ab[0][pair_v1] = 0.06*0.06;}
	if(d_a >= 0.1 && d_a < 0.4){sigm_v1_ab[0][pair_v1] = 0.025*0.025;}
	if(d_a >= 0.4 && d_a < 0.41){sigm_v1_ab[0][pair_v1] = 0.08*0.08;}
	if(d_a >= 0.41){sigm_v1_ab[0][pair_v1] = 0.10*0.10;}

	if(d_b < 0.02){sigm_v1_ab[1][pair_v1] = 0.08*0.08;}
	if(d_b >= 0.02 && d_b < 0.1){sigm_v1_ab[1][pair_v1] = 0.06*0.06;}
	if(d_b >= 0.1 && d_b < 0.4){sigm_v1_ab[1][pair_v1] = 0.025*0.025;}
	if(d_b >= 0.4 && d_b < 0.41){sigm_v1_ab[1][pair_v1] = 0.08*0.08;}
	if(d_b >= 0.41){sigm_v1_ab[1][pair_v1] = 0.10*0.10;}
	pair_v1++;


	used_va1[i] = true;
	used_vb1[j] = true;

      }// j for2
    } // i for1  V.

      //reconstruct single V-plane hits

    for (Int_t i = 0; i < it_va1; ++i) {
      if ( used_va1[i]) continue;
   
      double d_a = 0.; double d_b = 0.;
      int func_nr_a = -1;                                                    
      for(int t_it = 0; t_it<4;t_it++){
	if(time_va1[i] >= t_dc[t_it][6] && time_va1[i] < t_dc[t_it+1][6]){
	  func_nr_a = t_it;
	  break;
	}
      } 
      if(func_nr_a == -1)continue;

      d_a = scale[6]*( pol_par_dc[func_nr_a][0][6] +  pol_par_dc[func_nr_a][1][6]*time_va1[i] +  pol_par_dc[func_nr_a][2][6]*time_va1[i]*time_va1[i] + pol_par_dc[func_nr_a][3][6]*time_va1[i]*time_va1[i]*time_va1[i] + pol_par_dc[func_nr_a][4][6]*time_va1[i]*time_va1[i]*time_va1[i]*time_va1[i]);
     	 
      v1_single[0][single_va1] = wirenr_va1[i]-119.+ d_a;
      v1_single[0][single_va1+1] =  wirenr_va1[i]-119.- d_a;

      if(d_a < 0.02){sigm_v1_single[0][single_va1] = 0.08*0.08;sigm_v1_single[0][single_va1+1] = 0.08*0.08;}
      if(d_a >= 0.02 && d_a < 0.1){sigm_v1_single[0][single_va1] = 0.06*0.06;sigm_v1_single[0][single_va1+1] = 0.06*0.06;}
      if(d_a >= 0.1 && d_a < 0.4){sigm_v1_single[0][single_va1] = 0.025*0.025;sigm_v1_single[0][single_va1+1] = 0.025*0.025;}
      if(d_a >= 0.4 && d_a < 0.41){sigm_v1_single[0][single_va1] = 0.08*0.08;sigm_v1_single[0][single_va1+1] = 0.08*0.08;}
      if(d_a >= 0.41){sigm_v1_single[0][single_va1] = 0.10*0.10;sigm_v1_single[0][single_va1+1] = 0.10*0.10;}
      single_va1 += 2;
    }//for single va

    for (Int_t j = 0; j < it_vb1; ++j) {
      if ( used_vb1[j]) continue;
      double d_a = 0.; double d_b = 0.;
      int func_nr_b = -1;
      for(int t_it = 0; t_it<4;t_it++){
	if(time_vb1[j] >= t_dc[t_it][7] && time_vb1[j] < t_dc[t_it+1][7]){
	  func_nr_b = t_it;
	  break;
	}
      }
      if(func_nr_b == -1)continue;
      d_b = scale[7]*(pol_par_dc[func_nr_b][0][7] +  pol_par_dc[func_nr_b][1][7]*time_vb1[j] +  pol_par_dc[func_nr_b][2][7]*time_vb1[j]*time_vb1[j] + pol_par_dc[func_nr_b][3][7]*time_vb1[j]*time_vb1[j]*time_vb1[j] + pol_par_dc[func_nr_b][4][7]*time_vb1[j]*time_vb1[j]*time_vb1[j]*time_vb1[j]);
     
      v1_single[1][single_vb1] = wirenr_vb1[j]-118.5 + d_b;
      v1_single[1][single_vb1+1] =  wirenr_vb1[j]-118.5 - d_b;
      if(d_b < 0.02){sigm_v1_single[1][single_vb1] = 0.08*0.08;sigm_v1_single[1][single_vb1+1] = 0.08*0.08;}
      if(d_b >= 0.02 && d_b < 0.1){sigm_v1_single[1][single_vb1] = 0.06*0.06;sigm_v1_single[1][single_vb1+1] = 0.06*0.06;}
      if(d_b >= 0.1 && d_b < 0.4){sigm_v1_single[1][single_vb1] = 0.0250*0.0250;sigm_v1_single[1][single_vb1+1] = 0.0250*0.0250;}
      if(d_b >= 0.4 && d_b < 0.41){sigm_v1_single[1][single_vb1] = 0.08*0.08;sigm_v1_single[1][single_vb1+1] = 0.08*0.08;}
      if(d_b >= 0.41){sigm_v1_single[1][single_vb1] = 0.10*0.10;sigm_v1_single[1][single_vb1+1] = 0.10*0.10;}
      single_vb1 += 2;

    } // i for1  V.


    //end new reco
   
    //  ---  DC1 Segment Building  ------

    bool found_8p_seg = false;
    float x_slope = 0.0;
    float y_slope = 0.0;

    if (pair_x1 * pair_y1 > 0) { //(x,y)first
      for (int i = 0; i < pair_x1; i++) {
	if (nDC1_segments > 48)break;
	float x_coord = (x1_ab[0][i] + x1_ab[1][i]) / 2;
	float x_slope = ( x1_ab[0][i] - x1_ab[1][i] ) / ( z_loc[0] - z_loc[1] );
	x_slope = 0;
	float XU = x_coord + x_slope * 0.5 * (z_loc[5] + z_loc[4] - z_loc[1] - z_loc[0]);
	float XV = x_coord + x_slope * 0.5 * (z_loc[7] + z_loc[6] - z_loc[1] - z_loc[0]);
	for (int j = 0; j < pair_y1; j++) {
	  if (nDC1_segments > 48)break;
	  float y_coord = (y1_ab[0][j] + y1_ab[1][j]) / 2;
	  float y_slope = ( y1_ab[0][j] - y1_ab[1][j] ) / ( z_loc[2] - z_loc[3] );
	  y_slope = 0;
	  float YU = y_coord + y_slope * 0.5 * (z_loc[5] + z_loc[4] - z_loc[3] - z_loc[2]);
	  float YV = y_coord + y_slope * 0.5 * (z_loc[7] + z_loc[6] - z_loc[3] - z_loc[2]);
	  float u_est = isqrt_2 * (YU - XU);
	  float v_est = isqrt_2 * (YV + XV);
	  bool foundU = false;

	  if (pair_u1 > 0) {
	    double dU_thresh =  1.2;//1.3;
	    double cand_ua = -999;
	    double cand_ub = -999;
	    double cand_ua_sigm = -999;
	    double cand_ub_sigm = -999;

	    for (int k = 0; k < pair_u1; k++) {
	      float u_coord = (u1_ab[0][k] + u1_ab[1][k]) / 2;
	      if (fabs(u_coord - u_est ) > dU_thresh)continue;
	      dU_thresh = fabs(u_coord - u_est);
	      cand_ua = u1_ab[0][k];
	      cand_ub = u1_ab[1][k];
	      cand_ua_sigm = sigm_u1_ab[0][k];
	      cand_ub_sigm = sigm_u1_ab[1][k];
	    }//k
	    if(dU_thresh < 1.2){

	      rh_segDC1[0][nDC1_segments] = x1_ab[0][i];
	      rh_segDC1[1][nDC1_segments] = x1_ab[1][i];
	      rh_segDC1[2][nDC1_segments] = y1_ab[0][j];
	      rh_segDC1[3][nDC1_segments] = y1_ab[1][j];
	      rh_segDC1[4][nDC1_segments] = cand_ua;
	      rh_segDC1[5][nDC1_segments] = cand_ub;
	      rh_sigm_segDC1[0][nDC1_segments] = sigm_x1_ab[0][i];
	      rh_sigm_segDC1[1][nDC1_segments] = sigm_x1_ab[1][i];
	      rh_sigm_segDC1[2][nDC1_segments] = sigm_y1_ab[0][j];
	      rh_sigm_segDC1[3][nDC1_segments] = sigm_y1_ab[1][j];
	      rh_sigm_segDC1[4][nDC1_segments] = cand_ua_sigm;
	      rh_sigm_segDC1[5][nDC1_segments] = cand_ub_sigm;
			  
	      foundU = true;
	      if (nDC1_segments > 48)break;
	    }
	  }//(pair_u1>0)
	  if (found_8p_seg && !foundU)continue;
	  bool foundV = false;
	  if (pair_v1 > 0) {
	    double dV_thresh =  1.2;//1.3;
                       
	    double cand_va = -999;
	    double cand_vb = -999;
	    double cand_va_sigm = -999;
	    double cand_vb_sigm = -999;

	    for (int m = 0; m < pair_v1; m++) {
	      if (nDC1_segments > 48)break;
	      float v_coord = (v1_ab[0][m] + v1_ab[1][m]) / 2;

	      if (fabs(v_coord - v_est) > dV_thresh)continue;
	      dV_thresh = fabs(v_coord - v_est);
	      cand_va = v1_ab[0][m];
	      cand_vb = v1_ab[1][m];
	      cand_va_sigm = sigm_v1_ab[0][m];
	      cand_vb_sigm = sigm_v1_ab[1][m];
	    }//m
	    if(dV_thresh < 1.2){
		
	      foundV = true;
	      rh_segDC1[0][nDC1_segments] = x1_ab[0][i];
	      rh_segDC1[1][nDC1_segments] = x1_ab[1][i];
	      rh_segDC1[2][nDC1_segments] = y1_ab[0][j];
	      rh_segDC1[3][nDC1_segments] = y1_ab[1][j];
	      rh_segDC1[6][nDC1_segments] = cand_va;
	      rh_segDC1[7][nDC1_segments] = cand_vb;
	      rh_sigm_segDC1[0][nDC1_segments] = sigm_x1_ab[0][i];
	      rh_sigm_segDC1[1][nDC1_segments] = sigm_x1_ab[1][i];
	      rh_sigm_segDC1[2][nDC1_segments] = sigm_y1_ab[0][j];
	      rh_sigm_segDC1[3][nDC1_segments] = sigm_y1_ab[1][j];
	      rh_sigm_segDC1[6][nDC1_segments] = cand_va_sigm;
	      rh_sigm_segDC1[7][nDC1_segments] = cand_vb_sigm;
	    }
	  }//pair v1
	  if (!foundU && foundV) {
		      
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
	  //cout<<" new cand nr "<<nDC1_segments<<endl;
	  if (foundV || foundU){//see if the built seg is worth keeping and remove any dplicates if needed 
	      int worst_hit = -1;
	      double max_resid = 0;
	      for (int i = 0; i < 8; i++) {
		if (rh_segDC1[i][nDC1_segments] > -998.)size_segDC1[nDC1_segments]++;
	      }
	      for (int rej = 0; rej < 2; rej++) {//allow 2 passes max 8->7 & 7->6

		//end linear fit
		double rh_seg[8] = {-999};
		double rh_sigm_seg[8] = {-999};
		double par_ab[4] = {999, 999, 999, 999};

		for (int i = 0; i < 8; i++) {
		  rh_seg[i] = rh_segDC1[i][nDC1_segments];
		  rh_sigm_seg[i] = rh_sigm_segDC1[i][nDC1_segments];
		}

		fit_seg(rh_seg, rh_sigm_seg, par_ab, -1, -1); //usual fit without skipping any plane
		for (int i = 0; i < 4; i++) {
		  par_ab1[i][nDC1_segments] = par_ab[i];
		}

		chi2_DC1[nDC1_segments] = 0;

		double resid = 999;

		for (int i = 0; i < 8; i++) {
		  if (i == 0 && rh_segDC1[i][nDC1_segments] > -998.) {
		    resid = rh_segDC1[i][nDC1_segments] - z_loc[i] * par_ab1[0][nDC1_segments] - par_ab1[1][nDC1_segments];
		    chi2_DC1[nDC1_segments] += (resid * resid) / rh_sigm_segDC1[i][nDC1_segments];
		    if (fabs(resid) > max_resid) {
		      worst_hit = i;
		      max_resid = fabs(resid);
		    }
		  }
		  if (i == 1 && rh_segDC1[i][nDC1_segments] > -998.) {
		    resid = rh_segDC1[i][nDC1_segments] - z_loc[i] * par_ab1[0][nDC1_segments] - par_ab1[1][nDC1_segments];

		    chi2_DC1[nDC1_segments] += (resid * resid) / rh_sigm_segDC1[i][nDC1_segments];

		    if (fabs(resid) > max_resid) {
		      worst_hit = i;
		      max_resid = fabs(resid);
		    }
		  }
		  if (i == 2 && rh_segDC1[i][nDC1_segments] > -998.) {
		    resid = rh_segDC1[i][nDC1_segments] - z_loc[i] * par_ab1[2][nDC1_segments] - par_ab1[3][nDC1_segments];

		    chi2_DC1[nDC1_segments] += (resid * resid) / rh_sigm_segDC1[i][nDC1_segments];

		    if (fabs(resid) > max_resid) {
		      worst_hit = i;
		      max_resid = fabs(resid);
		    }
		  }
		  if (i == 3 && rh_segDC1[i][nDC1_segments] > -998.) {
		    resid = rh_segDC1[i][nDC1_segments] - z_loc[i] * par_ab1[2][nDC1_segments] - par_ab1[3][nDC1_segments];

		    chi2_DC1[nDC1_segments] += (resid * resid) / rh_sigm_segDC1[i][nDC1_segments];

		    if (fabs(resid) > max_resid) {
		      worst_hit = i;
		      max_resid = fabs(resid);
		    }
		  }
		  if (i == 4 && rh_segDC1[i][nDC1_segments] > -998.) {
		    resid = rh_segDC1[i][nDC1_segments] - isqrt_2 * z_loc[i]*(par_ab1[2][nDC1_segments] - par_ab1[0][nDC1_segments]) - isqrt_2 * (par_ab1[3][nDC1_segments] - par_ab1[1][nDC1_segments]);
		    chi2_DC1[nDC1_segments] += (resid * resid) / rh_sigm_segDC1[i][nDC1_segments];


		    if (fabs(resid) > max_resid) {
		      worst_hit = i;
		      max_resid = fabs(resid);
		    }
		  }
		  if (i == 5 && rh_segDC1[i][nDC1_segments] > -998.) {
		    resid = rh_segDC1[i][nDC1_segments] - isqrt_2 * z_loc[i]*(par_ab1[2][nDC1_segments] - par_ab1[0][nDC1_segments]) - isqrt_2 * (par_ab1[3][nDC1_segments] - par_ab1[1][nDC1_segments]);
		    chi2_DC1[nDC1_segments] += (resid * resid) / rh_sigm_segDC1[i][nDC1_segments];
		    if (fabs(resid) > max_resid) {
		      worst_hit = i;
		      max_resid = fabs(resid);
		    }
		  }
		  if (i == 6 && rh_segDC1[i][nDC1_segments] > -998.) {
		    resid = rh_segDC1[i][nDC1_segments] - isqrt_2 * z_loc[i]*(par_ab1[2][nDC1_segments] + par_ab1[0][nDC1_segments]) - isqrt_2 * (par_ab1[3][nDC1_segments] + par_ab1[1][nDC1_segments]);
		    chi2_DC1[nDC1_segments] += (resid * resid) / rh_sigm_segDC1[i][nDC1_segments];

		    if (fabs(resid) > max_resid) {
		      worst_hit = i;
		      max_resid = fabs(resid);
		    }
		  }
		  if (i == 7 && rh_segDC1[i][nDC1_segments] > -998.) {
		    resid = rh_segDC1[i][nDC1_segments] - isqrt_2 * z_loc[i]*(par_ab1[2][nDC1_segments] + par_ab1[0][nDC1_segments]) - isqrt_2 * (par_ab1[3][nDC1_segments] + par_ab1[1][nDC1_segments]);
		    chi2_DC1[nDC1_segments] += (resid * resid) / rh_sigm_segDC1[i][nDC1_segments];

		    if (fabs(resid) > max_resid) {
		      worst_hit = i;
		      max_resid = fabs(resid);
		    }
		  }
		}//i
		chi2_DC1[nDC1_segments] /= (size_segDC1[nDC1_segments] - 4);
		//if chi2 is big and seg_size = min erase this seg
		if(nDC1_segments > 24 ){
		  minChi2_up = 30;
		  minChi2_down = 20;
		}else{
		  minChi2_up = 50;
		  minChi2_down = 30;
		}

		if (chi2_DC1[nDC1_segments] > minChi2_down) {
		  if (size_segDC1[nDC1_segments] == 6) {
		    chi2_DC1[nDC1_segments] = 999;
		    break;
		  } else {
		    rh_segDC1[worst_hit][nDC1_segments] = -999; //erase worst hit and refit
		    size_segDC1[nDC1_segments]--;
		    max_resid = 0;
		    continue;
		  }
		}
	      }//rej 0 1 2
	      for (int ii = 0; ii < 2; ii++) {

		//	cout<<" i "<<ii<<rh_segDC1[ii][nDC1_segments]<<endl;
	      }
	      //cout<<" after fit chi2_DC1[nDC1_segments] "<<nDC1_segments<<" "<<chi2_DC1[nDC1_segments]<<" size "<<size_segDC1[nDC1_segments]<<  " ax "<< par_ab1[0][nDC1_segments] <<" bx  "<<par_ab1[1][nDC1_segments]<<" ay "<<par_ab1[2][nDC1_segments]<<" by "<<par_ab1[3][nDC1_segments]<<endl;

                    
	      if(chi2_DC1[nDC1_segments]>minChi2_up){//need to add function for cleaning up the seg candidate
		//initialize its hits
		for (int ii = 0; ii < 8; ii++) {
		  rh_segDC1[ii][nDC1_segments] = - 999.;
		  rh_sigm_segDC1[ii][nDC1_segments] = 1.;
		}
		size_segDC1[nDC1_segments] = 0;
		chi2_DC1[nDC1_segments] = 999.;
		continue;
	      }//if chi2 too big
	      //	      cout<<" after fit chi2_DC1[nDC1_segments] "<<nDC1_segments<<" "<<chi2_DC1[nDC1_segments]<<" size "<<size_segDC1[nDC1_segments]<<  " ax "<< par_ab1[0][nDC1_segments] <<" bx  "<<par_ab1[1][nDC1_segments]<<" ay "<<par_ab1[2][nDC1_segments]<<" by "<<par_ab1[3][nDC1_segments]<<endl;

	      //see if the seg points towards the region of IP (rough check |x|<50 & |y|<30) 
	      par_ab1[0][nDC1_segments] = -1 * (par_ab1[0][nDC1_segments] + x1_slope_sh + x1_slope_sh * par_ab1[0][nDC1_segments] * par_ab1[0][nDC1_segments]);
	      par_ab1[2][nDC1_segments] = par_ab1[2][nDC1_segments] + y1_slope_sh + y1_slope_sh * par_ab1[2][nDC1_segments] * par_ab1[2][nDC1_segments];
	      par_ab1[1][nDC1_segments] = -1 * (par_ab1[1][nDC1_segments] + x1_sh);
	      par_ab1[3][nDC1_segments] = par_ab1[3][nDC1_segments] + y1_sh;

	      float x_z0 = par_ab1[0][nDC1_segments]*(-Z_dch1)+par_ab1[1][nDC1_segments];
	      float y_z0 = par_ab1[2][nDC1_segments]*(-Z_dch1)+par_ab1[3][nDC1_segments];
	      //cout<<" x_zo "<<x_z0<<" y_zo "<<y_z0<<endl;
	      if(fabs(x_z0) > 50. || fabs(y_z0) > 30.){
		//initialize its hits
                for (int ii = 0; ii < 8; ii++) {
                  rh_segDC1[ii][nDC1_segments] = - 999.;
                  rh_sigm_segDC1[ii][nDC1_segments] = 1.;
                }
                size_segDC1[nDC1_segments] = 0;
                chi2_DC1[nDC1_segments] = 999.;
		//cout<<" initialize "<<endl;
		continue;
	      }//if (is not poining to IP region)
	      //cout<<" looking towards IP seg nr "<<nDC1_segments<<" size "<<size_segDC1[nDC1_segments]<<" chi2 "<<chi2_DC1[nDC1_segments]<<  " ax "<< par_ab1[0][nDC1_segments] <<" bx  "<<par_ab1[1][nDC1_segments]<<" ay "<<par_ab1[2][nDC1_segments]<<" by "<<par_ab1[3][nDC1_segments]<<endl;
	      // for(int hit = 0; hit < 8; hit++){cout<<" i "<<rh_segDC1[hit][nDC1_segments];}
	      //cout<<endl;


	      //leave only longest and best chi2 segments
	      //dc1
	      bool replaced = false;
	      for (int sizeit1_1 = 0; sizeit1_1 < nDC1_segments-1; sizeit1_1++) {
		if(chi2_DC1[sizeit1_1]>50.)continue;	
		for (int hit = 0; hit < 4; hit++) {
		  if (rh_segDC1[2 * hit][nDC1_segments] == rh_segDC1[2 * hit][sizeit1_1] && rh_segDC1[2 * hit + 1][nDC1_segments] == rh_segDC1[2 * hit + 1][sizeit1_1]){
		    //cout<<" duplicate search "<<endl;
		    //cout<< "cand nr "<<nDC1_segments<<" size "<<size_segDC1[nDC1_segments]<<" chi2 "<<chi2_DC1[nDC1_segments] <<endl;
		    //cout<<" old nr "<<sizeit1_1<<" size "<<size_segDC1[sizeit1_1]<<" chi2 "<<chi2_DC1[sizeit1_1]<<endl;
		    if (size_segDC1[nDC1_segments] > size_segDC1[sizeit1_1]) {
		      if (!replaced){		   
			for (int ii = 0; ii < 8; ii++) {
			  rh_segDC1[ii][sizeit1_1] =  rh_segDC1[ii][nDC1_segments];
			  rh_sigm_segDC1[ii][nDC1_segments] =  rh_sigm_segDC1[ii][nDC1_segments];
			}
			size_segDC1[sizeit1_1] = size_segDC1[nDC1_segments];
			chi2_DC1[sizeit1_1] = chi2_DC1[nDC1_segments];
			par_ab1[0][sizeit1_1] = par_ab1[0][nDC1_segments];
                        par_ab1[2][sizeit1_1] = par_ab1[2][nDC1_segments];
                        par_ab1[1][sizeit1_1] = par_ab1[1][nDC1_segments];
                        par_ab1[3][sizeit1_1] = par_ab1[3][nDC1_segments];
                        xDC1_glob_to_DC2[sizeit1_1] = par_ab1[0][sizeit1_1]*(199.5) + par_ab1[1][sizeit1_1];
                        yDC1_glob_to_DC2[sizeit1_1] = par_ab1[2][sizeit1_1]*(199.5) + par_ab1[3][sizeit1_1];

                        xDC1_glob[sizeit1_1] = par_ab1[0][sizeit1_1]*(99.75) + par_ab1[1][sizeit1_1];
                        yDC1_glob[sizeit1_1] = par_ab1[2][sizeit1_1]*(99.75) + par_ab1[3][sizeit1_1];
			
			replaced = true;
		      }else{
			chi2_DC1[sizeit1_1] = 999.;//just mark seg as bad !!!! needs to be reviewed		   
		      }
		      break;//stop searching common hits
		    }//size cand > size  old
		    if (size_segDC1[nDC1_segments] == size_segDC1[sizeit1_1]) {
		      if(chi2_DC1[nDC1_segments] < chi2_DC1[sizeit1_1]){
			if (!replaced){
			  for (int ii = 0; ii < 8; ii++) {
			    rh_segDC1[ii][sizeit1_1] =  rh_segDC1[ii][nDC1_segments];
			    rh_sigm_segDC1[ii][nDC1_segments] =  rh_sigm_segDC1[ii][nDC1_segments];
			  }
			  size_segDC1[sizeit1_1] = size_segDC1[nDC1_segments];
			  chi2_DC1[sizeit1_1] = chi2_DC1[nDC1_segments];
			 
			  par_ab1[0][sizeit1_1] = par_ab1[0][nDC1_segments];
			  par_ab1[2][sizeit1_1] = par_ab1[2][nDC1_segments];
			  par_ab1[1][sizeit1_1] = par_ab1[1][nDC1_segments];
			  par_ab1[3][sizeit1_1] = par_ab1[3][nDC1_segments];
			  xDC1_glob_to_DC2[sizeit1_1] = par_ab1[0][sizeit1_1]*(199.5) + par_ab1[1][sizeit1_1];
			  yDC1_glob_to_DC2[sizeit1_1] = par_ab1[2][sizeit1_1]*(199.5) + par_ab1[3][sizeit1_1];

			  xDC1_glob[sizeit1_1] = par_ab1[0][sizeit1_1]*(99.75) + par_ab1[1][sizeit1_1];
			  yDC1_glob[sizeit1_1] = par_ab1[2][sizeit1_1]*(99.75) + par_ab1[3][sizeit1_1];
			  
			  replaced = true;
			}else{
			  chi2_DC1[sizeit1_1] = 999.;//just mark seg as bad !!!! needs to be reviewed
			}
			break;//stop searching common hits
		      }//chi2 cand is better
		      else{//cout<<" chi2 cand is =< "<<endl;
			for (int ii = 0; ii < 8; ii++) {
			  rh_segDC1[ii][nDC1_segments] = - 999.;
			  rh_sigm_segDC1[ii][nDC1_segments] = 1.;
			}
			size_segDC1[nDC1_segments] = 0;
			chi2_DC1[nDC1_segments] = 999.;
		      }//chi2 cand is worse
		      break;//stop searching common hits
		    }//size cand = size old
		    if (size_segDC1[nDC1_segments] < size_segDC1[sizeit1_1]) {
		      for (int ii = 0; ii < 8; ii++) {
			rh_segDC1[ii][nDC1_segments] = - 999.;
		      rh_sigm_segDC1[ii][nDC1_segments] = 1.;
		      }
		      size_segDC1[nDC1_segments] = 0;
		      chi2_DC1[nDC1_segments] = 999.;
		      break;//stop searching common hits
		    }//size cand < size old
		  }//cand has coomon hits with old
		}//hit
	      }//nDC1_segments-1

	      if(chi2_DC1[nDC1_segments]>minChi2_up || replaced){
		for (int ii = 0; ii < 8; ii++) {
		  rh_segDC1[ii][nDC1_segments] = - 999.;
		  rh_sigm_segDC1[ii][nDC1_segments] = 1.;
		}
		size_segDC1[nDC1_segments] = 0;
		chi2_DC1[nDC1_segments] = 999.;
		continue;
	      }	   
	      //cout<<" end of cand examination chi2_DC1[nDC1_segments] "<<nDC1_segments<<" "<<chi2_DC1[nDC1_segments]<<" size "<<size_segDC1[nDC1_segments]<<endl;
	      xDC1_glob_to_DC2[nDC1_segments] = par_ab1[0][nDC1_segments]*(199.5) + par_ab1[1][nDC1_segments];
              yDC1_glob_to_DC2[nDC1_segments] = par_ab1[2][nDC1_segments]*(199.5) + par_ab1[3][nDC1_segments];

	      xDC1_glob[nDC1_segments] = par_ab1[0][nDC1_segments]*(99.75) + par_ab1[1][nDC1_segments];
	      yDC1_glob[nDC1_segments] = par_ab1[2][nDC1_segments]*(99.75) + par_ab1[3][nDC1_segments];
	      nDC1_segments++;
	  }//found u || foundv   
	  
	}//j
      }//i
      
    }//(x,y)first
    
    
    if (pair_u1 * pair_v1 > 0 && !found_8p_seg) { // (u,v) first

      for (int i = 0; i < pair_u1; i++) {
	if (nDC1_segments > 48)break;
	float u_coord = (u1_ab[0][i] + u1_ab[1][i]) / 2;
	float u_slope = ( u1_ab[0][i] - u1_ab[1][i] ) / ( z_loc[4] - z_loc[5] );
	u_slope = 0;
	float UX = u_coord + u_slope * 0.5 * (z_loc[1] + z_loc[0] - z_loc[5] - z_loc[4]);
	float UY = u_coord + u_slope * 0.5 * (z_loc[3] + z_loc[2] - z_loc[5] - z_loc[4]);

	for (int j = 0; j < pair_v1; j++) {
	  if (nDC1_segments > 48)break;
	  float v_coord = (v1_ab[0][j] + v1_ab[1][j]) / 2;
	  float v_slope = ( v1_ab[0][j] - v1_ab[1][j] ) / ( z_loc[6] - z_loc[7] );
	  v_slope = 0;
	  float VX = v_coord + v_slope * 0.5 * (z_loc[1] + z_loc[0] - z_loc[6] - z_loc[7]);
	  float VY = v_coord + v_slope * 0.5 * (z_loc[3] + z_loc[2] - z_loc[6] - z_loc[7]);

	  bool foundX = false;
	  float y_est = isqrt_2 * (UY + VY);
	  float x_est = isqrt_2 * (VX - UX);
	  if (pair_x1 > 0) {

	    double dX_thresh =  1.2;//1.5;
	    double cand_xa = -999;
	    double cand_xb = -999;
	    double cand_xa_sigm = -999;
	    double cand_xb_sigm = -999;

	    for (int k = 0; k < pair_x1; k++) {
	      float x_coord = (x1_ab[0][k] + x1_ab[1][k]) / 2;
	      if (nDC1_segments > 48)break;
                     
	      if (fabs(x_coord - x_est) > dX_thresh)continue;
	      dX_thresh = fabs(x_coord - x_est);

	      cand_xa = x1_ab[0][k];
	      cand_xb = x1_ab[1][k];
	      cand_xa_sigm = sigm_x1_ab[0][k];
	      cand_xb_sigm = sigm_x1_ab[1][k];

	    }//k
	    if(dX_thresh < 1.2){
		 
	      rh_segDC1[0][nDC1_segments] = cand_xa;
	      rh_segDC1[1][nDC1_segments] = cand_xb;
	      rh_segDC1[4][nDC1_segments] = u1_ab[0][i];
	      rh_segDC1[5][nDC1_segments] = u1_ab[1][i];
	      rh_segDC1[6][nDC1_segments] = v1_ab[0][j];
	      rh_segDC1[7][nDC1_segments] = v1_ab[1][j];
	      rh_sigm_segDC1[0][nDC1_segments] = cand_xa_sigm;
	      rh_sigm_segDC1[1][nDC1_segments] = cand_xb_sigm;
	      rh_sigm_segDC1[4][nDC1_segments] = sigm_u1_ab[0][i];
	      rh_sigm_segDC1[5][nDC1_segments] = sigm_u1_ab[1][i];
	      rh_sigm_segDC1[6][nDC1_segments] = sigm_v1_ab[0][j];
	      rh_sigm_segDC1[7][nDC1_segments] = sigm_v1_ab[1][j];
			  
	      foundX = true;
	      if (nDC1_segments > 48)break;
	    }//dx_thresh<.25

	  }//(pair_x1>0)
	  if (found_8p_seg && !foundX)continue;
	  bool foundY = false;
	  if (pair_y1 > 0) {

	    double dY_thresh =  1.2;//1.0;
	    double cand_ya = -999;
	    double cand_yb = -999;
	    double cand_ya_sigm = -999;
	    double cand_yb_sigm = -999;
	    for (int m = 0; m < pair_y1; m++) {
	      if (nDC1_segments > 48)break;
	      float y_coord = (y1_ab[0][m] + y1_ab[1][m]) / 2;
                        
	      if (fabs(y_coord - y_est) > dY_thresh)continue;
	      dY_thresh = fabs(y_coord - y_est);
	      cand_ya = y1_ab[0][m];
	      cand_yb = y1_ab[1][m];
	      cand_ya_sigm = sigm_y1_ab[0][m];
	      cand_yb_sigm = sigm_y1_ab[1][m];
	    }//m
	    if(dY_thresh < 1.2){
		
	      foundY = true;
	      if (nDC1_segments > 48)break;
	      rh_segDC1[2][nDC1_segments] = cand_ya;
	      rh_segDC1[3][nDC1_segments] = cand_yb;
	      rh_segDC1[4][nDC1_segments] = u1_ab[0][i];
	      rh_segDC1[5][nDC1_segments] = u1_ab[1][i];
	      rh_segDC1[6][nDC1_segments] = v1_ab[0][j];
	      rh_segDC1[7][nDC1_segments] = v1_ab[1][j];
	      rh_sigm_segDC1[2][nDC1_segments] = cand_ya_sigm;
	      rh_sigm_segDC1[3][nDC1_segments] = cand_yb_sigm;
	      rh_sigm_segDC1[4][nDC1_segments] = sigm_u1_ab[0][i];
	      rh_sigm_segDC1[5][nDC1_segments] = sigm_u1_ab[1][i];
	      rh_sigm_segDC1[6][nDC1_segments] = sigm_v1_ab[0][j];
	      rh_sigm_segDC1[7][nDC1_segments] = sigm_v1_ab[1][j];
	    }//dy_thresh<.25
	  }//pair y1
	  if (!foundX && foundY) {
		      
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
                    
	  if (foundX || foundY){//see if the built seg is worth keeping and remove any dplicates if needed 
	      int worst_hit = -1;
	      double max_resid = 0;
	      for (int i = 0; i < 8; i++) {
		if (rh_segDC1[i][nDC1_segments] > -998.)size_segDC1[nDC1_segments]++;
	      }
	      for (int rej = 0; rej < 2; rej++) {//allow 2 passes max 8->7 & 7->6

		//end linear fit
		double rh_seg[8] = {-999};
		double rh_sigm_seg[8] = {-999};
		double par_ab[4] = {999, 999, 999, 999};

		for (int i = 0; i < 8; i++) {
		  rh_seg[i] = rh_segDC1[i][nDC1_segments];
		  rh_sigm_seg[i] = rh_sigm_segDC1[i][nDC1_segments];
		}

		fit_seg(rh_seg, rh_sigm_seg, par_ab, -1, -1); //usual fit without skipping any plane
		for (int i = 0; i < 4; i++) {
		  par_ab1[i][nDC1_segments] = par_ab[i];
		}

		chi2_DC1[nDC1_segments] = 0;

		double resid = 999;

		for (int i = 0; i < 8; i++) {
		  if (i == 0 && rh_segDC1[i][nDC1_segments] > -998.) {
		    resid = rh_segDC1[i][nDC1_segments] - z_loc[i] * par_ab1[0][nDC1_segments] - par_ab1[1][nDC1_segments];
		    chi2_DC1[nDC1_segments] += (resid * resid) / rh_sigm_segDC1[i][nDC1_segments];
		    if (fabs(resid) > max_resid) {
		      worst_hit = i;
		      max_resid = fabs(resid);
		    }
		  }
		  if (i == 1 && rh_segDC1[i][nDC1_segments] > -998.) {
		    resid = rh_segDC1[i][nDC1_segments] - z_loc[i] * par_ab1[0][nDC1_segments] - par_ab1[1][nDC1_segments];

		    chi2_DC1[nDC1_segments] += (resid * resid) / rh_sigm_segDC1[i][nDC1_segments];

		    if (fabs(resid) > max_resid) {
		      worst_hit = i;
		      max_resid = fabs(resid);
		    }
		  }
		  if (i == 2 && rh_segDC1[i][nDC1_segments] > -998.) {
		    resid = rh_segDC1[i][nDC1_segments] - z_loc[i] * par_ab1[2][nDC1_segments] - par_ab1[3][nDC1_segments];

		    chi2_DC1[nDC1_segments] += (resid * resid) / rh_sigm_segDC1[i][nDC1_segments];

		    if (fabs(resid) > max_resid) {
		      worst_hit = i;
		      max_resid = fabs(resid);
		    }
		  }
		  if (i == 3 && rh_segDC1[i][nDC1_segments] > -998.) {
		    resid = rh_segDC1[i][nDC1_segments] - z_loc[i] * par_ab1[2][nDC1_segments] - par_ab1[3][nDC1_segments];

		    chi2_DC1[nDC1_segments] += (resid * resid) / rh_sigm_segDC1[i][nDC1_segments];

		    if (fabs(resid) > max_resid) {
		      worst_hit = i;
		      max_resid = fabs(resid);
		    }
		  }
		  if (i == 4 && rh_segDC1[i][nDC1_segments] > -998.) {
		    resid = rh_segDC1[i][nDC1_segments] - isqrt_2 * z_loc[i]*(par_ab1[2][nDC1_segments] - par_ab1[0][nDC1_segments]) - isqrt_2 * (par_ab1[3][nDC1_segments] - par_ab1[1][nDC1_segments]);
		    chi2_DC1[nDC1_segments] += (resid * resid) / rh_sigm_segDC1[i][nDC1_segments];


		    if (fabs(resid) > max_resid) {
		      worst_hit = i;
		      max_resid = fabs(resid);
		    }
		  }
		  if (i == 5 && rh_segDC1[i][nDC1_segments] > -998.) {
		    resid = rh_segDC1[i][nDC1_segments] - isqrt_2 * z_loc[i]*(par_ab1[2][nDC1_segments] - par_ab1[0][nDC1_segments]) - isqrt_2 * (par_ab1[3][nDC1_segments] - par_ab1[1][nDC1_segments]);
		    chi2_DC1[nDC1_segments] += (resid * resid) / rh_sigm_segDC1[i][nDC1_segments];
		    if (fabs(resid) > max_resid) {
		      worst_hit = i;
		      max_resid = fabs(resid);
		    }
		  }
		  if (i == 6 && rh_segDC1[i][nDC1_segments] > -998.) {
		    resid = rh_segDC1[i][nDC1_segments] - isqrt_2 * z_loc[i]*(par_ab1[2][nDC1_segments] + par_ab1[0][nDC1_segments]) - isqrt_2 * (par_ab1[3][nDC1_segments] + par_ab1[1][nDC1_segments]);
		    chi2_DC1[nDC1_segments] += (resid * resid) / rh_sigm_segDC1[i][nDC1_segments];

		    if (fabs(resid) > max_resid) {
		      worst_hit = i;
		      max_resid = fabs(resid);
		    }
		  }
		  if (i == 7 && rh_segDC1[i][nDC1_segments] > -998.) {
		    resid = rh_segDC1[i][nDC1_segments] - isqrt_2 * z_loc[i]*(par_ab1[2][nDC1_segments] + par_ab1[0][nDC1_segments]) - isqrt_2 * (par_ab1[3][nDC1_segments] + par_ab1[1][nDC1_segments]);
		    chi2_DC1[nDC1_segments] += (resid * resid) / rh_sigm_segDC1[i][nDC1_segments];

		    if (fabs(resid) > max_resid) {
		      worst_hit = i;
		      max_resid = fabs(resid);
		    }
		  }
		}//i
		chi2_DC1[nDC1_segments] /= (size_segDC1[nDC1_segments] - 4);
		//if chi2 is big and seg_size = min erase this seg
		if(nDC1_segments > 24 ){
		  minChi2_up = 30;
		  minChi2_down = 20;
		}else{
		  minChi2_up = 50;
		  minChi2_down = 30;
		}

		if (chi2_DC1[nDC1_segments] > minChi2_down) {
		  if (size_segDC1[nDC1_segments] == 6) {
		    chi2_DC1[nDC1_segments] = 999;
		    break;
		  } else {
		    rh_segDC1[worst_hit][nDC1_segments] = -999; //erase worst hit and refit
		    size_segDC1[nDC1_segments]--;
		    max_resid = 0;
		    continue;
		  }
		}
	      }//rej 0 1 2
	                    
	      if(chi2_DC1[nDC1_segments]>minChi2_up){//need to add function for cleaning up the seg candidate
		//initialize its hits
		for (int ii = 0; ii < 8; ii++) {
		  rh_segDC1[ii][nDC1_segments] = - 999.;
		  rh_sigm_segDC1[ii][nDC1_segments] = 1.;
		}
		size_segDC1[nDC1_segments] = 0;
		chi2_DC1[nDC1_segments] = 999.;
		continue;
	      }//if chi2 too big
	      //cout<<" after fit chi2_DC1[nDC1_segments] "<<nDC1_segments<<" "<<chi2_DC1[nDC1_segments]<<" size "<<size_segDC1[nDC1_segments]<<  " ax "<< par_ab1[0][nDC1_segments] <<" bx  "<<par_ab1[1][nDC1_segments]<<" ay "<<par_ab1[2][nDC1_segments]<<" by "<<par_ab1[3][nDC1_segments]<<endl;

	      //see if the seg points towards the region of IP (rough check |x|<50 & |y|<30) 
	      par_ab1[0][nDC1_segments] = -1 * (par_ab1[0][nDC1_segments] + x1_slope_sh + x1_slope_sh * par_ab1[0][nDC1_segments] * par_ab1[0][nDC1_segments]);
	      par_ab1[2][nDC1_segments] = par_ab1[2][nDC1_segments] + y1_slope_sh + y1_slope_sh * par_ab1[2][nDC1_segments] * par_ab1[2][nDC1_segments];
	      par_ab1[1][nDC1_segments] = -1 * (par_ab1[1][nDC1_segments] + x1_sh);
	      par_ab1[3][nDC1_segments] = par_ab1[3][nDC1_segments] + y1_sh;

	      float x_z0 = par_ab1[0][nDC1_segments]*(-Z_dch1)+par_ab1[1][nDC1_segments];
	      float y_z0 = par_ab1[2][nDC1_segments]*(-Z_dch1)+par_ab1[3][nDC1_segments];
	      //cout<<" x_zo "<<x_z0<<" y_zo "<<y_z0<<endl;

	      if(fabs(x_z0) > 50. || fabs(y_z0) > 30.){
		//initialize its hits
                for (int ii = 0; ii < 8; ii++) {
                  rh_segDC1[ii][nDC1_segments] = - 999.;
                  rh_sigm_segDC1[ii][nDC1_segments] = 1.;
                }
                size_segDC1[nDC1_segments] = 0;
                chi2_DC1[nDC1_segments] = 999.;
		//cout<<" initialize"<<endl;
                continue;
	      }//if (is not poining to IP region)

	      //cout<<" looking towards IP seg nr "<<nDC1_segments<<" size "<<size_segDC1[nDC1_segments]<<" chi2 "<<chi2_DC1[nDC1_segments]<<  " ax "<< par_ab1[0][nDC1_segments] <<" bx  "<<par_ab1[1][nDC1_segments]<<" ay "<<par_ab1[2][nDC1_segments]<<" by "<<par_ab1[3][nDC1_segments]<<endl;
	      //              for(int hit = 0; hit < 8; hit++){cout<<" i "<<rh_segDC1[hit][nDC1_segments];}
              //cout<<endl;

	      //leave only longest and best chi2 segments
	      //dc1
	      bool replaced = false;
	      for (int sizeit1_1 = 0; sizeit1_1 < nDC1_segments-1; sizeit1_1++) {
		if(chi2_DC1[sizeit1_1]>50.)continue;	
		for (int hit = 0; hit < 4; hit++) {
		  if (rh_segDC1[2 * hit][nDC1_segments] == rh_segDC1[2 * hit][sizeit1_1] && rh_segDC1[2 * hit + 1][nDC1_segments] == rh_segDC1[2 * hit + 1][sizeit1_1]){
		    //cout<<" duplicate search "<<endl;
		    //cout<< "cand nr "<<nDC1_segments<<" size "<<size_segDC1[nDC1_segments]<<" chi2 "<<chi2_DC1[nDC1_segments] <<endl;
		    //cout<<" old nr "<<sizeit1_1<<" size "<<size_segDC1[sizeit1_1]<<" chi2 "<<chi2_DC1[sizeit1_1]<<endl;

		    if (size_segDC1[nDC1_segments] > size_segDC1[sizeit1_1]) {
		      if (!replaced){		   
			for (int ii = 0; ii < 8; ii++) {
			  rh_segDC1[ii][sizeit1_1] =  rh_segDC1[ii][nDC1_segments];
			  rh_sigm_segDC1[ii][nDC1_segments] =  rh_sigm_segDC1[ii][nDC1_segments];
			}
			size_segDC1[sizeit1_1] = size_segDC1[nDC1_segments];
			chi2_DC1[sizeit1_1] = chi2_DC1[nDC1_segments];

			par_ab1[0][sizeit1_1] = par_ab1[0][nDC1_segments];
			par_ab1[2][sizeit1_1] = par_ab1[2][nDC1_segments];
			par_ab1[1][sizeit1_1] = par_ab1[1][nDC1_segments];
			par_ab1[3][sizeit1_1] = par_ab1[3][nDC1_segments];
			xDC1_glob_to_DC2[sizeit1_1] = par_ab1[0][sizeit1_1]*(199.5) + par_ab1[1][sizeit1_1];
			yDC1_glob_to_DC2[sizeit1_1] = par_ab1[2][sizeit1_1]*(199.5) + par_ab1[3][sizeit1_1];

			xDC1_glob[sizeit1_1] = par_ab1[0][sizeit1_1]*(99.75) + par_ab1[1][sizeit1_1];
			yDC1_glob[sizeit1_1] = par_ab1[2][sizeit1_1]*(99.75) + par_ab1[3][sizeit1_1];

			replaced = true;
		      }else{
			chi2_DC1[sizeit1_1] = 999.;//just mark seg as bad !!!! needs to be reviewed		   
		      }
		      break;//stop searching common hits
		    }//size cand > size  old
		    if (size_segDC1[nDC1_segments] == size_segDC1[sizeit1_1]) {
		      if(chi2_DC1[nDC1_segments] < chi2_DC1[sizeit1_1]){
			if (!replaced){
			  for (int ii = 0; ii < 8; ii++) {
			    rh_segDC1[ii][sizeit1_1] =  rh_segDC1[ii][nDC1_segments];
			    rh_sigm_segDC1[ii][nDC1_segments] =  rh_sigm_segDC1[ii][nDC1_segments];
			  }
			  size_segDC1[sizeit1_1] = size_segDC1[nDC1_segments];
			  chi2_DC1[sizeit1_1] = chi2_DC1[nDC1_segments];
			  par_ab1[0][sizeit1_1] = par_ab1[0][nDC1_segments];
			  par_ab1[2][sizeit1_1] = par_ab1[2][nDC1_segments];
			  par_ab1[1][sizeit1_1] = par_ab1[1][nDC1_segments];
			  par_ab1[3][sizeit1_1] = par_ab1[3][nDC1_segments];
			  xDC1_glob_to_DC2[sizeit1_1] = par_ab1[0][sizeit1_1]*(199.5) + par_ab1[1][sizeit1_1];
			  yDC1_glob_to_DC2[sizeit1_1] = par_ab1[2][sizeit1_1]*(199.5) + par_ab1[3][sizeit1_1];

			  xDC1_glob[sizeit1_1] = par_ab1[0][sizeit1_1]*(99.75) + par_ab1[1][sizeit1_1];
			  yDC1_glob[sizeit1_1] = par_ab1[2][sizeit1_1]*(99.75) + par_ab1[3][sizeit1_1];
			  
			  replaced = true;
			}else{
			  chi2_DC1[sizeit1_1] = 999.;//just mark seg as bad !!!! needs to be reviewed
			}
			break;//stop searching common hits
		      }//chi2 cand is better
		      else{
			for (int ii = 0; ii < 8; ii++) {
			  rh_segDC1[ii][nDC1_segments] = - 999.;
			  rh_sigm_segDC1[ii][nDC1_segments] = 1.;
			}
			size_segDC1[nDC1_segments] = 0;
			chi2_DC1[nDC1_segments] = 999.;
		      }//chi2 cand is worse
		      break;//stop searching common hits
		    }//size cand = size old
		    if (size_segDC1[nDC1_segments] < size_segDC1[sizeit1_1]) {
		      for (int ii = 0; ii < 8; ii++) {
			rh_segDC1[ii][nDC1_segments] = - 999.;
		      rh_sigm_segDC1[ii][nDC1_segments] = 1.;
		      }
		      size_segDC1[nDC1_segments] = 0;
		      chi2_DC1[nDC1_segments] = 999.;
		      break;//stop searching common hits
		    }//size cand < size old
		  }//cand has coomon hits with old
		}//hit
	      }//nDC1_segments-1
	      if(chi2_DC1[nDC1_segments]>minChi2_up || replaced){
                for (int ii = 0; ii < 8; ii++) {
                  rh_segDC1[ii][nDC1_segments] = - 999.;
                  rh_sigm_segDC1[ii][nDC1_segments] = 1.;
                }
                size_segDC1[nDC1_segments] = 0;
                chi2_DC1[nDC1_segments] = 999.;
		continue;
              }
	      //cout<<" end of cand examination chi2_DC1[nDC1_segments] "<<nDC1_segments<<" "<<chi2_DC1[nDC1_segments]<<" size "<<size_segDC1[nDC1_segments]<<endl;
	      xDC1_glob_to_DC2[nDC1_segments] = par_ab1[0][nDC1_segments]*(199.5) + par_ab1[1][nDC1_segments];
	      yDC1_glob_to_DC2[nDC1_segments] = par_ab1[2][nDC1_segments]*(199.5) + par_ab1[3][nDC1_segments];

	      xDC1_glob[nDC1_segments] = par_ab1[0][nDC1_segments]*(99.75) + par_ab1[1][nDC1_segments];
	      yDC1_glob[nDC1_segments] = par_ab1[2][nDC1_segments]*(99.75) + par_ab1[3][nDC1_segments];
	      nDC1_segments++;
	  }//found x or y
	}//j
      }//i
    }//(u,v) first
	
    ///-------------DCH2 hit reco------------------///
  
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

    double xa2_pm[2] = {-999.};
    double xb2_pm[2] = {-999.};
    double ya2_pm[2] = {-999.};
    double yb2_pm[2] = {-999.};
    double ua2_pm[2] = {-999.};
    double ub2_pm[2] = {-999.};
    double va2_pm[2] = {-999.};
    double vb2_pm[2] = {-999.};

    //new reco

    //   ---   X   ---

    for (Int_t i = 0; i < it_xa2; ++i) {
      int func_nr_a = -1;
      for(int t_it = 0; t_it<4;t_it++){
	if(time_xa2[i] >= t_dc[t_it][8] && time_xa2[i] < t_dc[t_it+1][8]){
	  func_nr_a = t_it;
	  break;
	}
      }
      if(func_nr_a == -1)continue;

      for (Int_t j = 0; j < it_xb2; ++j) {
	if(pair_x2 > 48)break;
	if ( ( wirenr_xa2[i] != wirenr_xb2[j] && wirenr_xa2[i] != wirenr_xb2[j]+1)) continue;
	int func_nr_b = -1;                                                    

	double time_xa = time_xa2[i];
	double d_a = 0.; double d_b = 0.;

	d_a = scale[8]*( pol_par_dc[func_nr_a][0][8] +  pol_par_dc[func_nr_a][1][8]*time_xa +  pol_par_dc[func_nr_a][2][8]*time_xa*time_xa + pol_par_dc[func_nr_a][3][8]*time_xa*time_xa*time_xa + pol_par_dc[func_nr_a][4][8]*time_xa*time_xa*time_xa*time_xa);

	for(int t_it = 0; t_it<4;t_it++){
	  if(time_xb2[j] >= t_dc[t_it][9] && time_xb2[j] < t_dc[t_it+1][9]){
	    func_nr_b = t_it;
	    break;
	  }
	}
	if(func_nr_b == -1)continue;

	time_xa = time_xb2[j];

	d_b = scale[9]*( pol_par_dc[func_nr_b][0][9] +  pol_par_dc[func_nr_b][1][9]*time_xa +  pol_par_dc[func_nr_b][2][9]*time_xa*time_xa + pol_par_dc[func_nr_b][3][9]*time_xa*time_xa*time_xa + pol_par_dc[func_nr_b][4][9]*time_xa*time_xa*time_xa*time_xa);

	//cout<<" time "<< " da "<<d_a<<" db "<<d_b<<endl; 
	xa2_pm[0] =  wirenr_xa2[i]-120.+ d_a;
	xa2_pm[1] =  wirenr_xa2[i]-120.- d_a;
	xb2_pm[0] =  wirenr_xb2[j]-119.5 + d_b;
	xb2_pm[1] =  wirenr_xb2[j]-119.5 - d_b;


	double dmin1 = 999;
	for(int k = 0; k<2;k++){
	  for(int m = 0; m<2;m++){
	    if (  fabs(xa2_pm[k]- xb2_pm[m]) < dmin1){
	      dmin1 = fabs(xa2_pm[k]- xb2_pm[m]);
	      x2_ab[0][pair_x2] = xa2_pm[k];
	      x2_ab[1][pair_x2] = xb2_pm[m];
	    }
	  }
	}


	if(d_a < 0.02){sigm_x2_ab[0][pair_x2] = 0.08*0.08;}
	if(d_a >= 0.02 && d_a < 0.1){sigm_x2_ab[0][pair_x2] = 0.06*0.06;}
	if(d_a >= 0.1 && d_a < 0.4){sigm_x2_ab[0][pair_x2] = 0.025*0.025;}
	if(d_a >= 0.4 && d_a < 0.41){sigm_x2_ab[0][pair_x2] = 0.08*0.08;}
	if(d_a >= 0.41){sigm_x2_ab[0][pair_x2] = 0.10*0.10;}

	if(d_b < 0.02){sigm_x2_ab[1][pair_x2] = 0.08*0.08;}
	if(d_b >= 0.02 && d_b < 0.1){sigm_x2_ab[1][pair_x2] = 0.06*0.06;}
	if(d_b >= 0.1 && d_b < 0.4){sigm_x2_ab[1][pair_x2] = 0.025*0.025;}
	if(d_b >= 0.4 && d_b < 0.41){sigm_x2_ab[1][pair_x2] = 0.08*0.08;}
	if(d_b >= 0.41){sigm_x2_ab[1][pair_x2] = 0.10*0.10;}
        //cout<<" time "<< " a "<<time_xa2[i]<<" b "<<time_xb2[j]<<" x2_ab[0][pair_x2] "<<x2_ab[0][pair_x2]<<" sigma2 "<<sigm_x2_ab[0][pair_x2]<<" x2_ab[1][pair_x2] "<<x2_ab[1][pair_x2]<<" sigma2 "<<sigm_x2_ab[1][pair_x2]<<endl;

	pair_x2++;


	used_xa2[i] = true;
	used_xb2[j] = true;
      }// j for2
    } // i for1  X.

      //reconstruct single X-plane hits

    for (Int_t i = 0; i < it_xa2; ++i) {
      if ( used_xa2[i]) continue;
      double d_a = 0.;

      int func_nr_a = -1;                                                    
      for(int t_it = 0; t_it<4;t_it++){
	if(time_xa2[i] >= t_dc[t_it][8] && time_xa2[i] < t_dc[t_it+1][8]){
	  func_nr_a = t_it;
	  break;
	}
      }
      if(func_nr_a == -1)continue;

      double time_xa = time_xa2[i];
      d_a = scale[8]*( pol_par_dc[func_nr_a][0][8] +  pol_par_dc[func_nr_a][1][8]*time_xa +  pol_par_dc[func_nr_a][2][8]*time_xa*time_xa + pol_par_dc[func_nr_a][3][8]*time_xa*time_xa*time_xa + pol_par_dc[func_nr_a][4][8]*time_xa*time_xa*time_xa*time_xa);
      
      x2_single[0][single_xa2] = wirenr_xa2[i]-120. + d_a;
      x2_single[0][single_xa2+1] =  wirenr_xa2[i]-120. - d_a;
      if(d_a < 0.02){sigm_x2_single[0][single_xa2] = 0.08*0.08;sigm_x2_single[0][single_xa2+1] = 0.08*0.08;}
      if(d_a >= 0.02 && d_a < 0.1){sigm_x2_single[0][single_xa2] = 0.06*0.06;sigm_x2_single[0][single_xa2+1] = 0.06*0.06;}
      if(d_a >= 0.1 && d_a < 0.4){sigm_x2_single[0][single_xa2] = 0.0250*0.0250;sigm_x2_single[0][single_xa2+1] = 0.0250*0.0250;}
      if(d_a >= 0.4 && d_a < 0.41){sigm_x2_single[0][single_xa2] = 0.08*0.08;sigm_x2_single[0][single_xa2+1] = 0.08*0.08;}
      if(d_a >= 0.41){sigm_x2_single[0][single_xa2] = 0.10*0.10;sigm_x2_single[0][single_xa2+1] = 0.10*0.10;}
      single_xa2 += 2;
    }//for single xa
      
    for (Int_t j = 0; j < it_xb2; ++j) {
      if ( used_xb2[j]) continue;
      int func_nr_b = -1;                                                    
      double d_b = 0.;

      for(int t_it = 0; t_it<4;t_it++){
	if(time_xb2[j] >= t_dc[t_it][9] && time_xb2[j] < t_dc[t_it+1][9]){
	  func_nr_b = t_it;
	  break;
	}
      }
      if(func_nr_b == -1)continue;

      double time_xa = time_xb2[j];
      d_b = scale[9]*( pol_par_dc[func_nr_b][0][9] +  pol_par_dc[func_nr_b][1][9]*time_xa +  pol_par_dc[func_nr_b][2][9]*time_xa*time_xa + pol_par_dc[func_nr_b][3][9]*time_xa*time_xa*time_xa + pol_par_dc[func_nr_b][4][9]*time_xa*time_xa*time_xa*time_xa);
  
      x2_single[1][single_xb2] = wirenr_xb2[j]-119.5 + d_b;
      x2_single[1][single_xb2+1] =  wirenr_xb2[j]-119.5 - d_b;
      if(d_b < 0.02){sigm_x2_single[1][single_xb2] = 0.08*0.08;sigm_x2_single[1][single_xb2+1] = 0.08*0.08;}
      if(d_b >= 0.02 && d_b < 0.1){sigm_x2_single[1][single_xb2] = 0.06*0.06;sigm_x2_single[1][single_xb2+1] = 0.06*0.06;}
      if(d_b >= 0.1 && d_b < 0.4){sigm_x2_single[1][single_xb2] = 0.0250*0.0250;sigm_x2_single[1][single_xb2+1] = 0.0250*0.0250;}
      if(d_b >= 0.4 && d_b < 0.41){sigm_x2_single[1][single_xb2] = 0.08*0.08;sigm_x2_single[1][single_xb2+1] = 0.08*0.08;}
      if(d_b >= 0.41){sigm_x2_single[1][single_xb2] = 0.1*0.1;sigm_x2_single[1][single_xb2+1] = 0.1*0.1;}
      single_xb2 += 2;
    }//j xb2

    //------Y-------

    for (Int_t i = 0; i < it_ya2; ++i) {
      int func_nr_a = -1;
      for(int t_it = 0; t_it<4;t_it++){
	if(time_ya2[i] >= t_dc[t_it][10] && time_ya2[i] < t_dc[t_it+1][10]){
	  func_nr_a = t_it;
	  break;
	}
      }
      if(func_nr_a == -1)continue;

      for (Int_t j = 0; j < it_yb2; ++j) {
	if(pair_y2 > 48)break;
	if ( ( wirenr_ya2[i] != wirenr_yb2[j] && wirenr_ya2[i] != wirenr_yb2[j]+1)) continue;
	double d_a = 0.; double d_b = 0.;
	int func_nr_b = -1;

	d_a = scale[10]*( pol_par_dc[func_nr_a][0][10] +  pol_par_dc[func_nr_a][1][10]*time_ya2[i] +  pol_par_dc[func_nr_a][2][10]*time_ya2[i]*time_ya2[i] + pol_par_dc[func_nr_a][3][10]*time_ya2[i]*time_ya2[i]*time_ya2[i] + pol_par_dc[func_nr_a][4][10]*time_ya2[i]*time_ya2[i]*time_ya2[i]*time_ya2[i]);

	for(int t_it = 0; t_it<4;t_it++){
	  if(time_yb2[j] >= t_dc[t_it][11] && time_yb2[j] < t_dc[t_it+1][11]){
	    func_nr_b = t_it;
	    break;
	  }
	}
	if(func_nr_b == -1)continue;
	d_b = scale[11]*( pol_par_dc[func_nr_b][0][11] +  pol_par_dc[func_nr_b][1][11]*time_yb2[j] +  pol_par_dc[func_nr_b][2][11]*time_yb2[j]*time_yb2[j] + pol_par_dc[func_nr_b][3][11]*time_yb2[j]*time_yb2[j]*time_yb2[j] + pol_par_dc[func_nr_b][4][11]*time_yb2[j]*time_yb2[j]*time_yb2[j]*time_yb2[j]);

	ya2_pm[0] =  wirenr_ya2[i]-119.+ d_a;
	ya2_pm[1] =  wirenr_ya2[i]-119.- d_a;
	yb2_pm[0] =  wirenr_yb2[j]-118.5 + d_b;
	yb2_pm[1] =  wirenr_yb2[j]-118.5 - d_b;


	double dmin1 = 999;
	for(int k = 0; k<2;k++){
	  for(int m = 0; m<2;m++){
	    if (  fabs(ya2_pm[k]- yb2_pm[m]) < dmin1){
	      dmin1 = fabs(ya2_pm[k]- yb2_pm[m]);
	      y2_ab[0][pair_y2] = ya2_pm[k];
	      y2_ab[1][pair_y2] = yb2_pm[m];
	    }
	  }
	}

	if(d_a < 0.02){sigm_y2_ab[0][pair_y2] = 0.08*0.08;}
	if(d_a >= 0.02 && d_a < 0.1){sigm_y2_ab[0][pair_y2] = 0.06*0.06;}
	if(d_a >= 0.1 && d_a < 0.4){sigm_y2_ab[0][pair_y2] = 0.025*0.025;}
	if(d_a >= 0.4 && d_a < 0.41){sigm_y2_ab[0][pair_y2] = 0.08*0.08;}
	if(d_a >= 0.41){sigm_y2_ab[0][pair_y2] = 0.10*0.10;}

	if(d_b < 0.02){sigm_y2_ab[1][pair_y2] = 0.08*0.08;}
	if(d_b >= 0.02 && d_b < 0.1){sigm_y2_ab[1][pair_y2] = 0.06*0.06;}
	if(d_b >= 0.1 && d_b < 0.4){sigm_y2_ab[1][pair_y2] = 0.025*0.025;}
	if(d_b >= 0.4 && d_b < 0.41){sigm_y2_ab[1][pair_y2] = 0.08*0.08;}
	if(d_b >= 0.41){sigm_y2_ab[1][pair_y2] = 0.10*0.10;}
	pair_y2++;

	used_ya2[i] = true;
	used_yb2[j] = true;

      }// j for2
    } // i for1  Y.

      //reconstruct single Y-plane hits

    for (Int_t i = 0; i < it_ya2; ++i) {
      if ( used_ya2[i]) continue;
      double d_a = 0.;

      int func_nr_a = -1;                                                    
      for(int t_it = 0; t_it<4;t_it++){
	if(time_ya2[i] >= t_dc[t_it][10] && time_ya2[i] < t_dc[t_it+1][10]){
	  func_nr_a = t_it;
	  break;
	}
      }
      if(func_nr_a == -1)continue;
      d_a = scale[10]*( pol_par_dc[func_nr_a][0][10] +  pol_par_dc[func_nr_a][1][10]*time_ya2[i] +  pol_par_dc[func_nr_a][2][10]*time_ya2[i]*time_ya2[i] + pol_par_dc[func_nr_a][3][10]*time_ya2[i]*time_ya2[i]*time_ya2[i] + pol_par_dc[func_nr_a][4][10]*time_ya2[i]*time_ya2[i]*time_ya2[i]*time_ya2[i]);
      
      y2_single[0][single_ya2] = wirenr_ya2[i]-119.+ d_a;
      y2_single[0][single_ya2+1] =  wirenr_ya2[i]-119.- d_a;
      if(d_a < 0.02){sigm_y2_single[0][single_ya2] = 0.08*0.08;sigm_y2_single[0][single_ya2+1] = 0.08*0.08;}
      if(d_a >= 0.02 && d_a < 0.1){sigm_y2_single[0][single_ya2] = 0.06*0.06;sigm_y2_single[0][single_ya2+1] = 0.06*0.06;}
      if(d_a >= 0.1 && d_a < 0.4){sigm_y2_single[0][single_ya2] = 0.025*0.025;sigm_y2_single[0][single_ya2+1] = 0.025*0.025;}
      if(d_a >= 0.4 && d_a < 0.41){sigm_y2_single[0][single_ya2] = 0.08*0.08;sigm_y2_single[0][single_ya2+1] = 0.08*0.08;}
      if(d_a >= 0.41){sigm_y2_single[0][single_ya2] = 0.10*0.10;sigm_y2_single[0][single_ya2+1] = 0.10*0.10;}
      single_ya2 += 2;
    }//for single ya

    for (Int_t j = 0; j < it_yb2; ++j) {
      if ( used_yb2[j]) continue;
      double d_b = 0.;
      int func_nr_b = -1;                                                    
      for(int t_it = 0; t_it<4;t_it++){
	if(time_yb2[j] >= t_dc[t_it][11] && time_yb2[j] < t_dc[t_it+1][11]){
	  func_nr_b = t_it;
	  break;
	}
      }    
      if(func_nr_b == -1)continue;

      d_b = scale[11]*( pol_par_dc[func_nr_b][0][11] +  pol_par_dc[func_nr_b][1][11]*time_yb2[j] +  pol_par_dc[func_nr_b][2][11]*time_yb2[j]*time_yb2[j] + pol_par_dc[func_nr_b][3][11]*time_yb2[j]*time_yb2[j]*time_yb2[j] + pol_par_dc[func_nr_b][4][11]*time_yb2[j]*time_yb2[j]*time_yb2[j]*time_yb2[j]);
  
      y2_single[1][single_yb2] = wirenr_yb2[j]-118.5 + d_b;
      y2_single[1][single_yb2+1] =  wirenr_yb2[j]-118.5 - d_b;
      if(d_b < 0.02){sigm_y2_single[1][single_yb2] = 0.08*0.08;sigm_y2_single[1][single_yb2+1] = 0.08*0.08;}
      if(d_b >= 0.02 && d_b < 0.1){sigm_y2_single[1][single_yb2] = 0.06*0.06;sigm_y2_single[1][single_yb2+1] = 0.06*0.06;}
      if(d_b >= 0.1 && d_b < 0.4){sigm_y2_single[1][single_yb2] = 0.0250*0.0250;sigm_y2_single[1][single_yb2+1] = 0.0250*0.0250;}
      if(d_b >= 0.4 && d_b < 0.41){sigm_y2_single[1][single_yb2] = 0.1*0.1;sigm_y2_single[1][single_yb2+1] = 0.1*0.1;}
      if(d_b >= 0.41){sigm_y2_single[1][single_yb2] = 0.10*0.10;sigm_y2_single[1][single_yb2+1] = 0.10*0.10;}
      single_yb2 += 2;

    } // i for1  Y.


      //   ----   U   ---

    for (Int_t i = 0; i < it_ua2; ++i) {
      int func_nr_a = -1; 
      for(int t_it = 0; t_it<4;t_it++){
	if(time_ua2[i] >= t_dc[t_it][12] && time_ua2[i] < t_dc[t_it+1][12]){
	  func_nr_a = t_it;
	  break;
	}
      }
      if(func_nr_a == -1)continue;

      for (Int_t j = 0; j < it_ub2; ++j) {
	if(pair_u2 > 48)break;
	if (( wirenr_ua2[i] != wirenr_ub2[j] && wirenr_ua2[i] != wirenr_ub2[j]+1)) continue;
                                                                                                                                                          
	double d_a = 0.; double d_b = 0.;
	  
	int func_nr_b = -1;
	d_a = scale[12]*( pol_par_dc[func_nr_a][0][12] +  pol_par_dc[func_nr_a][1][12]*time_ua2[i] +  pol_par_dc[func_nr_a][2][12]*time_ua2[i]*time_ua2[i] + pol_par_dc[func_nr_a][3][12]*time_ua2[i]*time_ua2[i]*time_ua2[i] + pol_par_dc[func_nr_a][4][12]*time_ua2[i]*time_ua2[i]*time_ua2[i]*time_ua2[i]);


	for(int t_it = 0; t_it<4;t_it++){
	  if(time_ub2[j] >= t_dc[t_it][13] && time_ub2[j] < t_dc[t_it+1][13]){
	    func_nr_b = t_it;
	    break;
	  }
	}
	if(func_nr_b == -1)continue;

	d_b = scale[13]*( pol_par_dc[func_nr_b][0][13] +  pol_par_dc[func_nr_b][1][13]*time_ub2[j] +  pol_par_dc[func_nr_b][2][13]*time_ub2[j]*time_ub2[j] + pol_par_dc[func_nr_b][3][13]*time_ub2[j]*time_ub2[j]*time_ub2[j] + pol_par_dc[func_nr_b][4][13]*time_ub2[j]*time_ub2[j]*time_ub2[j]*time_ub2[j]);


	ua2_pm[0] =  wirenr_ua2[i]-119.+ d_a;
	ua2_pm[1] =  wirenr_ua2[i]-119.- d_a;
	ub2_pm[0] =  wirenr_ub2[j]-118.5 + d_b;
	ub2_pm[1] =  wirenr_ub2[j]-118.5 - d_b;

	double dmin1 = 999;
	for(int k = 0; k<2;k++){
	  for(int m = 0; m<2;m++){
	    if (  fabs(ua2_pm[k]- ub2_pm[m]) < dmin1){
	      dmin1 = fabs(ua2_pm[k]- ub2_pm[m]);
	      u2_ab[0][pair_u2] = ua2_pm[k];
	      u2_ab[1][pair_u2] = ub2_pm[m];
	    }
	  }
	}


	if(d_a < 0.02){sigm_u2_ab[0][pair_u2] = 0.08*0.08;}
	if(d_a >= 0.02 && d_a < 0.1){sigm_u2_ab[0][pair_u2] = 0.06*0.06;}
	if(d_a >= 0.1 && d_a < 0.4){sigm_u2_ab[0][pair_u2] = 0.025*0.025;}
	if(d_a >= 0.4 && d_a < 0.41){sigm_u2_ab[0][pair_u2] = 0.08*0.08;}
	if(d_a >= 0.41){sigm_u2_ab[0][pair_u2] = 0.10*0.10;}

	if(d_b < 0.02){sigm_u2_ab[1][pair_u2] = 0.08*0.08;}
	if(d_b >= 0.02 && d_b < 0.1){sigm_u2_ab[1][pair_u2] = 0.06*0.06;}
	if(d_b >= 0.1 && d_b < 0.4){sigm_u2_ab[1][pair_u2] = 0.025*0.025;}
	if(d_b >= 0.4 && d_b < 0.41){sigm_u2_ab[1][pair_u2] = 0.08*0.08;}
	if(d_b >= 0.41){sigm_u2_ab[1][pair_u2] = 0.10*0.10;}
	pair_u2++;
	used_ua2[i] = true;
	used_ub2[j] = true;

      }// j for2
    } // i for1  U.

      //reconstruct single U-plane hits

    for (Int_t i = 0; i < it_ua2; ++i) {
      if ( used_ua2[i]) continue;
      double d_a = 0.; double d_b = 0.;

      int func_nr_a = -1;                                                    
      for(int t_it = 0; t_it<4;t_it++){
	if(time_ua2[i] >= t_dc[t_it][12] && time_ua2[i] < t_dc[t_it+1][12]){
	  func_nr_a = t_it;
	  break;
	}
      }
      if(func_nr_a == -1)continue;

      d_a = scale[12]*( pol_par_dc[func_nr_a][0][12] +  pol_par_dc[func_nr_a][1][12]*time_ua2[i] +  pol_par_dc[func_nr_a][2][12]*time_ua2[i]*time_ua2[i] + pol_par_dc[func_nr_a][3][12]*time_ua2[i]*time_ua2[i]*time_ua2[i] + pol_par_dc[func_nr_a][4][12]*time_ua2[i]*time_ua2[i]*time_ua2[i]*time_ua2[i]);
   
      u2_single[0][single_ua2] = (wirenr_ua2[i]-119.+ d_a);
      u2_single[0][single_ua2+1] = ( wirenr_ua2[i]-119.- d_a);
      if(d_a < 0.02){sigm_u2_single[0][single_ua2] = 0.08*0.08;sigm_u2_single[0][single_ua2+1] = 0.08*0.08;}
      if(d_a >= 0.02 && d_a < 0.1){sigm_u2_single[0][single_ua2] = 0.06*0.06;sigm_u2_single[0][single_ua2+1] = 0.06*0.06;}
      if(d_a >= 0.1 && d_a < 0.4){sigm_u2_single[0][single_ua2] = 0.025*0.025;sigm_u2_single[0][single_ua2+1] = 0.025*0.025;}
      if(d_a >= 0.4 && d_a < 0.41){sigm_u2_single[0][single_ua2] = 0.08*0.08;sigm_u2_single[0][single_ua2+1] = 0.08*0.08;}
      if(d_a >= 0.41){sigm_u2_single[0][single_ua2] = 0.10*0.10;sigm_u2_single[0][single_ua2+1] = 0.10*0.10;}
      single_ua2 += 2;
    }//for single ua

    for (Int_t j = 0; j < it_ub2; ++j) {
      if ( used_ub2[j]) continue;
      int func_nr_b = -1;                                                    
      double d_a = 0.; double d_b = 0.;

      for(int t_it = 0; t_it<4;t_it++){
	if(time_ub2[j] >= t_dc[t_it][13] && time_ub2[j] < t_dc[t_it+1][13]){
	  func_nr_b = t_it;
	  break;
	}
      }
      if(func_nr_b == -1)continue;

      d_b = scale[13]*( pol_par_dc[func_nr_b][0][13] +  pol_par_dc[func_nr_b][1][13]*time_ub2[j] +  pol_par_dc[func_nr_b][2][13]*time_ub2[j]*time_ub2[j] + pol_par_dc[func_nr_b][3][13]*time_ub2[j]*time_ub2[j]*time_ub2[j] + pol_par_dc[func_nr_b][4][13]*time_ub2[j]*time_ub2[j]*time_ub2[j]*time_ub2[j]);
     
      u2_single[1][single_ub2] = (wirenr_ub2[j]-118.5 + d_b);
      u2_single[1][single_ub2+1] =  (wirenr_ub2[j]-118.5 - d_b);
      if(d_b < 0.02){sigm_u2_single[1][single_ub2] = 0.08*0.08;sigm_u2_single[1][single_ub2+1] = 0.08*0.08;}
      if(d_b >= 0.02 && d_b < 0.1){sigm_u2_single[1][single_ub2] = 0.060*0.060;sigm_u2_single[1][single_ub2+1] = 0.060*0.060;}
      if(d_b >= 0.1 && d_b < 0.4){sigm_u2_single[1][single_ub2] = 0.025*0.025;sigm_u2_single[1][single_ub2+1] = 0.0250*0.0250;}
      if(d_b >= 0.4 && d_b < 0.41){sigm_u2_single[1][single_ub2] = 0.08*0.08;sigm_u2_single[1][single_ub2+1] = 0.08*0.08;}
      if(d_b >= 0.41){sigm_u2_single[1][single_ub2] = 0.10*0.10;sigm_u2_single[1][single_ub2+1] = 0.10*0.10;}
      single_ub2 += 2;

    } // i for1  U.

      //   ----   V   ---

    for (Int_t i = 0; i < it_va2; ++i) {
      int func_nr_a = -1; 
      for(int t_it = 0; t_it<4;t_it++){
	if(time_va2[i] >= t_dc[t_it][14] && time_va2[i] < t_dc[t_it+1][14]){
	  func_nr_a = t_it;
	  break;
	}
      }
      if(func_nr_a == -1)continue;

      for (Int_t j = 0; j < it_vb2; ++j) {
	if(pair_v2 > 48)break;
	if (( wirenr_va2[i] != wirenr_vb2[j] && wirenr_va2[i] != wirenr_vb2[j]+1))continue;

	//added                                                                                                                                                                   
	double d_a = 0.; double d_b = 0.;

	int func_nr_b = -1;
	d_a = scale[14]*( pol_par_dc[func_nr_a][0][14] +  pol_par_dc[func_nr_a][1][14]*time_va2[i] +  pol_par_dc[func_nr_a][2][14]*time_va2[i]*time_va2[i] + pol_par_dc[func_nr_a][3][14]*time_va2[i]*time_va2[i]*time_va2[i] + pol_par_dc[func_nr_a][4][14]*time_va2[i]*time_va2[i]*time_va2[i]*time_va2[i]);

	for(int t_it = 0; t_it<4;t_it++){
	  if(time_vb2[j] >= t_dc[t_it][15] && time_vb2[j] < t_dc[t_it+1][15]){
	    func_nr_b = t_it;
	    break;
	  }
	}
	if(func_nr_b == -1)continue;

	d_b = scale[15]*(  pol_par_dc[func_nr_b][0][15] +  pol_par_dc[func_nr_b][1][15]*time_vb2[j] +  pol_par_dc[func_nr_b][2][15]*time_vb2[j]*time_vb2[j] + pol_par_dc[func_nr_b][3][15]*time_vb2[j]*time_vb2[j]*time_vb2[j] + pol_par_dc[func_nr_b][4][15]*time_vb2[j]*time_vb2[j]*time_vb2[j]*time_vb2[j]);

	va2_pm[0] =  wirenr_va2[i]-119.+ d_a;
	va2_pm[1] =  wirenr_va2[i]-119.- d_a;
	vb2_pm[0] =  wirenr_vb2[j]-118.5 + d_b;
	vb2_pm[1] =  wirenr_vb2[j]-118.5 - d_b;

	double dmin1 = 999;
	for(int k = 0; k<2;k++){
	  for(int m = 0; m<2;m++){
	    if (  fabs(va2_pm[k]- vb2_pm[m]) < dmin1){
	      dmin1 = fabs(va2_pm[k]- vb2_pm[m]);
	      v2_ab[0][pair_v2] = va2_pm[k];
	      v2_ab[1][pair_v2] = vb2_pm[m];
	    }
	  }
	}


	if(d_a < 0.02){sigm_v2_ab[0][pair_v2] = 0.08*0.08;}
	if(d_a >= 0.02 && d_a < 0.1){sigm_v2_ab[0][pair_v2] = 0.06*0.06;}
	if(d_a >= 0.1 && d_a < 0.4){sigm_v2_ab[0][pair_v2] = 0.025*0.025;}
	if(d_a >= 0.4 && d_a < 0.41){sigm_v2_ab[0][pair_v2] = 0.08*0.08;}
	if(d_a >= 0.41){sigm_v2_ab[0][pair_v2] = 0.10*0.10;}

	if(d_b < 0.02){sigm_v2_ab[1][pair_v2] = 0.08*0.08;}
	if(d_b >= 0.02 && d_b < 0.1){sigm_v2_ab[1][pair_v2] = 0.06*0.06;}
	if(d_b >= 0.1 && d_b < 0.4){sigm_v2_ab[1][pair_v2] = 0.025*0.025;}
	if(d_b >= 0.4 && d_b < 0.41){sigm_v2_ab[1][pair_v2] = 0.08*0.08;}
	if(d_b >= 0.41){sigm_v2_ab[1][pair_v2] = 0.10*0.10;}

	//cout<<" va "<<v2_ab[0][pair_v2] <<" vb "<< v2_ab[1][pair_v2]<<endl;
	  pair_v2++;

	used_va2[i] = true;
	used_vb2[j] = true;


      }// j for2
    } // i for1  V.

      //reconstruct single V-plane hits

    for (Int_t i = 0; i < it_va2; ++i) {
      if ( used_va2[i]) continue;
      double d_a = 0.;

      //added
      int func_nr_a = -1;                                                    
      for(int t_it = 0; t_it<4;t_it++){
	if(time_va2[i] >= t_dc[t_it][14] && time_va2[i] < t_dc[t_it+1][14]){
	  func_nr_a = t_it;
	  break;
	}
      }
      if(func_nr_a == -1)continue;

      d_a = scale[14]*( pol_par_dc[func_nr_a][0][14] +  pol_par_dc[func_nr_a][1][14]*time_va2[i] +  pol_par_dc[func_nr_a][2][14]*time_va2[i]*time_va2[i] + pol_par_dc[func_nr_a][3][14]*time_va2[i]*time_va2[i]*time_va2[i] + pol_par_dc[func_nr_a][4][14]*time_va2[i]*time_va2[i]*time_va2[i]*time_va2[i]);
      
      v2_single[0][single_va2] = wirenr_va2[i]-119.+ d_a;
      v2_single[0][single_va2+1] =  wirenr_va2[i]-119.- d_a;
      if(d_a < 0.02){sigm_v2_single[0][single_va2] = 0.08*0.08;sigm_v2_single[0][single_va2+1] = 0.08*0.08;}
      if(d_a >= 0.02 && d_a < 0.1){sigm_v2_single[0][single_va2] = 0.06*0.06;sigm_v2_single[0][single_va2+1] = 0.06*0.06;}
      if(d_a >= 0.1 && d_a < 0.4){sigm_v2_single[0][single_va2] = 0.025*0.025;sigm_v2_single[0][single_va2+1] = 0.025*0.025;}
      if(d_a >= 0.4 && d_a < 0.41){sigm_v2_single[0][single_va2] = 0.08*0.08;sigm_v2_single[0][single_va2+1] = 0.08*0.08;}
      if(d_a >= 0.41){sigm_v2_single[0][single_va2] = 0.10*0.10;sigm_v2_single[0][single_va2+1] = 0.10*0.10;}

      single_va2 += 2;
    }//for single va

    for (Int_t j = 0; j < it_vb2; ++j) {
      if ( used_vb2[j]) continue;
      double d_b = 0.;
      int  func_nr_b = -1;                                                    
      for(int t_it = 0; t_it<4;t_it++){
	if(time_vb2[j] >= t_dc[t_it][15] && time_vb2[j] < t_dc[t_it+1][15]){
	  func_nr_b = t_it;
	  break;
	}
      }
      if(func_nr_b == -1)continue;

      d_b = scale[15]*( pol_par_dc[func_nr_b][0][15] +  pol_par_dc[func_nr_b][1][15]*time_vb2[j] +  pol_par_dc[func_nr_b][2][15]*time_vb2[j]*time_vb2[j] + pol_par_dc[func_nr_b][3][15]*time_vb2[j]*time_vb2[j]*time_vb2[j] + pol_par_dc[func_nr_b][4][15]*time_vb2[j]*time_vb2[j]*time_vb2[j]*time_vb2[j]);
    
      v2_single[1][single_vb2] = wirenr_vb2[j]-118.5 + d_b;
      v2_single[1][single_vb2+1] =  wirenr_vb2[j]-118.5 - d_b;
      if(d_b < 0.02){sigm_v2_single[1][single_vb2] = 0.08*0.08;sigm_v2_single[1][single_vb2+1] = 0.08*0.08;}
      if(d_b >= 0.02 && d_b < 0.1){sigm_v2_single[1][single_vb2] = 0.060*0.060;sigm_v2_single[1][single_vb2+1] = 0.060*0.060;}
      if(d_b >= 0.1 && d_b < 0.4){sigm_v2_single[1][single_vb2] = 0.0250*0.0250;sigm_v2_single[1][single_vb2+1] = 0.0250*0.0250;}
      if(d_b >= 0.4 && d_b < 0.41){sigm_v2_single[1][single_vb2] = 0.08*0.08;sigm_v2_single[1][single_vb2+1] = 0.08*0.08;}
      if(d_b >= 0.41){sigm_v2_single[1][single_vb2] = 0.10*0.10;sigm_v2_single[1][single_vb2+1] = 0.10*0.10;}
      //cout<<" single vb 0 "<< v2_single[1][single_vb2]<<"  vb 1 "<<v2_single[1][single_vb2+1]<<endl;


      single_vb2 += 2;
	
     
    } // i for1  V.

    //end new reco

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

	    double dU_thresh = 1.2;//1.3;
	    double cand_ua = -999;
	    double cand_ub = -999;
	    double cand_ua_sigm = -999;
	    double cand_ub_sigm = -999;
		      
	    for (int k = 0; k < pair_u2; k++) {
	      float u_coord = (u2_ab[0][k] + u2_ab[1][k]) / 2;
	      if (fabs(u_coord - u_est) > dU_thresh)continue;
	      dU_thresh = fabs(u_coord - u_est);
	      cand_ua = u2_ab[0][k];
	      cand_ub = u2_ab[1][k];
	      cand_ua_sigm = sigm_u2_ab[0][k];
	      cand_ub_sigm = sigm_u2_ab[1][k];
	    }//k
	    if(dU_thresh < 1.2){

	      rh_segDC2[0][nDC2_segments] = x2_ab[0][i];
	      rh_segDC2[1][nDC2_segments] = x2_ab[1][i];
	      rh_segDC2[2][nDC2_segments] = y2_ab[0][j];
	      rh_segDC2[3][nDC2_segments] = y2_ab[1][j];
	      rh_segDC2[4][nDC2_segments] = cand_ua;
	      rh_segDC2[5][nDC2_segments] = cand_ub;
	      rh_sigm_segDC2[0][nDC2_segments] = sigm_x2_ab[0][i];
	      rh_sigm_segDC2[1][nDC2_segments] = sigm_x2_ab[1][i];
	      rh_sigm_segDC2[2][nDC2_segments] = sigm_y2_ab[0][j];
	      rh_sigm_segDC2[3][nDC2_segments] = sigm_y2_ab[1][j];
	      rh_sigm_segDC2[4][nDC2_segments] = cand_ua_sigm;
	      rh_sigm_segDC2[5][nDC2_segments] = cand_ub_sigm;

	      foundU = true;
	      if (nDC2_segments > 48)break;
	    }//k
	  }//(pair_u2>0)
	  if (found_8p_seg && !foundU)continue;
	  bool foundV = false;
	  if (pair_v2 > 0) {
	    double dV_thresh = 1.2;//1.3;
	    double cand_va = -999.;
	    double cand_vb = -999.;
	    double cand_va_sigm = -999.;
	    double cand_vb_sigm = -999.;
	    for (int m = 0; m < pair_v2; m++) {
	      if (nDC2_segments > 48)break;
	      float v_coord = (v2_ab[0][m] + v2_ab[1][m]) / 2;
	      if (fabs(v_coord - v_est) > dV_thresh)continue;
	      dV_thresh = fabs(v_coord - v_est);
	      cand_va = v2_ab[0][m];
	      cand_vb = v2_ab[1][m];
	      //cout<<" n seg "<<nDC2_segments<<" vb "<<v2_ab[1][m]<<endl;
	      cand_va_sigm = sigm_v2_ab[0][m];
	      cand_vb_sigm = sigm_v2_ab[1][m];
	    }//m
	    if(dV_thresh < 1.2){
	      foundV = true;
	      rh_segDC2[0][nDC2_segments] = x2_ab[0][i];
	      rh_segDC2[1][nDC2_segments] = x2_ab[1][i];
	      rh_segDC2[2][nDC2_segments] = y2_ab[0][j];
	      rh_segDC2[3][nDC2_segments] = y2_ab[1][j];
	      rh_segDC2[6][nDC2_segments] = cand_va;
	      rh_segDC2[7][nDC2_segments] = cand_vb;
	      rh_sigm_segDC2[0][nDC2_segments] = sigm_x2_ab[0][i];
	      rh_sigm_segDC2[1][nDC2_segments] = sigm_x2_ab[1][i];
	      rh_sigm_segDC2[2][nDC2_segments] = sigm_y2_ab[0][j];
	      rh_sigm_segDC2[3][nDC2_segments] = sigm_y2_ab[1][j];
	      rh_sigm_segDC2[6][nDC2_segments] = cand_va_sigm;
	      rh_sigm_segDC2[7][nDC2_segments] = cand_vb_sigm;
	    }
	  }//pair v2
	  if (!foundU && foundV) {
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
		//cout<<" v2_single[1][kk] "<<v2_single[1][kk]<<" v_est "<<v_est<<" min b "<<min_b<<endl;
		foundV = true;
	      }
	    }//for kk                                                                                                                           
	  }//!foundV 
	  if (foundV || foundU){//see if the built seg is worth keeping and remove any dplicates if needed 
	      int worst_hit = -1;
	      double max_resid = 0;
	      for (int i = 0; i < 8; i++) {
		if (rh_segDC2[i][nDC2_segments] > -998.)size_segDC2[nDC2_segments]++;
	      }
	      for (int rej = 0; rej < 2; rej++) {//allow 2 passes max 8->7 & 7->6

		//end linear fit
		double rh_seg[8] = {-999};
		double rh_sigm_seg[8] = {-999};
		double par_ab[4] = {999, 999, 999, 999};

		for (int i = 0; i < 8; i++) {
		  rh_seg[i] = rh_segDC2[i][nDC2_segments];
		  rh_sigm_seg[i] = rh_sigm_segDC2[i][nDC2_segments];
		}

		fit_seg(rh_seg, rh_sigm_seg, par_ab, -1, -1); //usual fit without skipping any plane
		for (int i = 0; i < 4; i++) {
		  par_ab2[i][nDC2_segments] = par_ab[i];
		}

		chi2_DC2[nDC2_segments] = 0;

		double resid = 999;

		for (int i = 0; i < 8; i++) {
		  if (i == 0 && rh_segDC2[i][nDC2_segments] > -998.) {
		    resid = rh_segDC2[i][nDC2_segments] - z_loc[i] * par_ab2[0][nDC2_segments] - par_ab2[1][nDC2_segments];
		    chi2_DC2[nDC2_segments] += (resid * resid) / rh_sigm_segDC2[i][nDC2_segments];
		    if (fabs(resid) > max_resid) {
		      worst_hit = i;
		      max_resid = fabs(resid);
		    }
		  }
		  if (i == 1 && rh_segDC2[i][nDC2_segments] > -998.) {
		    resid = rh_segDC2[i][nDC2_segments] - z_loc[i] * par_ab2[0][nDC2_segments] - par_ab2[1][nDC2_segments];

		    chi2_DC2[nDC2_segments] += (resid * resid) / rh_sigm_segDC2[i][nDC2_segments];

		    if (fabs(resid) > max_resid) {
		      worst_hit = i;
		      max_resid = fabs(resid);
		    }
		  }
		  if (i == 2 && rh_segDC2[i][nDC2_segments] > -998.) {
		    resid = rh_segDC2[i][nDC2_segments] - z_loc[i] * par_ab2[2][nDC2_segments] - par_ab2[3][nDC2_segments];

		    chi2_DC2[nDC2_segments] += (resid * resid) / rh_sigm_segDC2[i][nDC2_segments];

		    if (fabs(resid) > max_resid) {
		      worst_hit = i;
		      max_resid = fabs(resid);
		    }
		  }
		  if (i == 3 && rh_segDC2[i][nDC2_segments] > -998.) {
		    resid = rh_segDC2[i][nDC2_segments] - z_loc[i] * par_ab2[2][nDC2_segments] - par_ab2[3][nDC2_segments];

		    chi2_DC2[nDC2_segments] += (resid * resid) / rh_sigm_segDC2[i][nDC2_segments];

		    if (fabs(resid) > max_resid) {
		      worst_hit = i;
		      max_resid = fabs(resid);
		    }
		  }
		  if (i == 4 && rh_segDC2[i][nDC2_segments] > -998.) {
		    resid = rh_segDC2[i][nDC2_segments] - isqrt_2 * z_loc[i]*(par_ab2[2][nDC2_segments] - par_ab2[0][nDC2_segments]) - isqrt_2 * (par_ab2[3][nDC2_segments] - par_ab2[1][nDC2_segments]);
		    chi2_DC2[nDC2_segments] += (resid * resid) / rh_sigm_segDC2[i][nDC2_segments];


		    if (fabs(resid) > max_resid) {
		      worst_hit = i;
		      max_resid = fabs(resid);
		    }
		  }
		  if (i == 5 && rh_segDC2[i][nDC2_segments] > -998.) {
		    resid = rh_segDC2[i][nDC2_segments] - isqrt_2 * z_loc[i]*(par_ab2[2][nDC2_segments] - par_ab2[0][nDC2_segments]) - isqrt_2 * (par_ab2[3][nDC2_segments] - par_ab2[1][nDC2_segments]);
		    chi2_DC2[nDC2_segments] += (resid * resid) / rh_sigm_segDC2[i][nDC2_segments];
		    if (fabs(resid) > max_resid) {
		      worst_hit = i;
		      max_resid = fabs(resid);
		    }
		  }
		  if (i == 6 && rh_segDC2[i][nDC2_segments] > -998.) {
		    resid = rh_segDC2[i][nDC2_segments] - isqrt_2 * z_loc[i]*(par_ab2[2][nDC2_segments] + par_ab2[0][nDC2_segments]) - isqrt_2 * (par_ab2[3][nDC2_segments] + par_ab2[1][nDC2_segments]);
		    chi2_DC2[nDC2_segments] += (resid * resid) / rh_sigm_segDC2[i][nDC2_segments];

		    if (fabs(resid) > max_resid) {
		      worst_hit = i;
		      max_resid = fabs(resid);
		    }
		  }
		  if (i == 7 && rh_segDC2[i][nDC2_segments] > -998.) {
		    resid = rh_segDC2[i][nDC2_segments] - isqrt_2 * z_loc[i]*(par_ab2[2][nDC2_segments] + par_ab2[0][nDC2_segments]) - isqrt_2 * (par_ab2[3][nDC2_segments] + par_ab2[1][nDC2_segments]);
		    chi2_DC2[nDC2_segments] += (resid * resid) / rh_sigm_segDC2[i][nDC2_segments];

		    if (fabs(resid) > max_resid) {
		      worst_hit = i;
		      max_resid = fabs(resid);
		    }
		  }
		}//i
		chi2_DC2[nDC2_segments] /= (size_segDC2[nDC2_segments] - 4);
		//if chi2 is big and seg_size = min erase this seg
		if(nDC2_segments > 24 ){
		  minChi2_up = 30;
		  minChi2_down = 20;
		}else{
		  minChi2_up = 50;
		  minChi2_down = 30;
		}

		if (chi2_DC2[nDC2_segments] > minChi2_down) {
		  if (size_segDC2[nDC2_segments] == 6) {
		    chi2_DC2[nDC2_segments] = 999;
		    break;
		  } else {
		    rh_segDC2[worst_hit][nDC2_segments] = -999; //erase worst hit and refit
		    size_segDC2[nDC2_segments]--;
		    max_resid = 0;
		    continue;
		  }
		}
	      }//rej 0 1 2
	                    
	      if(chi2_DC2[nDC2_segments]>minChi2_up){//need to add function for cleaning up the seg candidate
		//initialize its hits
		for (int ii = 0; ii < 8; ii++) {
		  rh_segDC2[ii][nDC2_segments] = - 999.;
		  rh_sigm_segDC2[ii][nDC2_segments] = 1.;
		}
		size_segDC2[nDC2_segments] = 0;
		chi2_DC2[nDC2_segments] = 999.;
		continue;
	      }//if chi2 too big

	      //see if the seg points towards the region of IP (rough check |x|<50 & |y|<30) 
	      par_ab2[0][nDC2_segments] = -1 * (par_ab2[0][nDC2_segments] + x2_slope_sh + x2_slope_sh * par_ab2[0][nDC2_segments] * par_ab2[0][nDC2_segments]);
	      par_ab2[2][nDC2_segments] = par_ab2[2][nDC2_segments] + y2_slope_sh + y2_slope_sh * par_ab2[2][nDC2_segments] * par_ab2[2][nDC2_segments];
	      par_ab2[1][nDC2_segments] = -1 * (par_ab2[1][nDC2_segments] + x2_sh);
	      par_ab2[3][nDC2_segments] = par_ab2[3][nDC2_segments] + y2_sh;

	      float x_z0 = par_ab2[0][nDC2_segments]*(-Z_dch2)+par_ab2[1][nDC2_segments];
	      float y_z0 = par_ab2[2][nDC2_segments]*(-Z_dch2)+par_ab2[3][nDC2_segments];
	      if(fabs(x_z0) > 50. || fabs(y_z0) > 30.){
		//initialize its hits
                for (int ii = 0; ii < 8; ii++) {
                  rh_segDC2[ii][nDC2_segments] = - 999.;
                  rh_sigm_segDC2[ii][nDC2_segments] = 1.;
                }
                size_segDC2[nDC2_segments] = 0;
                chi2_DC2[nDC2_segments] = 999.;
                continue;
	      }//if (is not poining to IP region)


	      //leave only longest and best chi2 segments
	      //dc2
	      bool replaced = false;
	      for (int sizeit1_1 = 0; sizeit1_1 < nDC2_segments-1; sizeit1_1++) {
		if(chi2_DC2[sizeit1_1]>50.)continue;	
		for (int hit = 0; hit < 4; hit++) {
		  if (rh_segDC2[2 * hit][nDC2_segments] == rh_segDC2[2 * hit][sizeit1_1] && rh_segDC2[2 * hit + 1][nDC2_segments] == rh_segDC2[2 * hit + 1][sizeit1_1]){
		    if (size_segDC2[nDC2_segments] > size_segDC2[sizeit1_1]) {
		      if (!replaced){		   
			for (int ii = 0; ii < 8; ii++) {
			  rh_segDC2[ii][sizeit1_1] =  rh_segDC2[ii][nDC2_segments];
			  rh_sigm_segDC2[ii][nDC2_segments] =  rh_sigm_segDC2[ii][nDC2_segments];
			}
			size_segDC2[sizeit1_1] = size_segDC2[nDC2_segments];
			chi2_DC2[sizeit1_1] = chi2_DC2[nDC2_segments];
			par_ab2[0][sizeit1_1] = par_ab2[0][nDC2_segments];
			par_ab2[2][sizeit1_1] = par_ab2[2][nDC2_segments];
			par_ab2[1][sizeit1_1] = par_ab2[1][nDC2_segments];
			par_ab2[3][sizeit1_1] = par_ab2[3][nDC2_segments];

			xDC2_glob[sizeit1_1] = par_ab2[0][sizeit1_1]*(-99.75) + par_ab2[1][sizeit1_1];
			yDC2_glob[sizeit1_1] = par_ab2[2][sizeit1_1]*(-99.75) + par_ab2[3][sizeit1_1];

			
			replaced = true;
		      }else{
			chi2_DC2[sizeit1_1] = 999.;//just mark seg as bad !!!! needs to be reviewed		   
		      }
		      break;//stop searching common hits
		    }//size cand > size  old
		    if (size_segDC2[nDC2_segments] == size_segDC2[sizeit1_1]) {
		      if(chi2_DC2[nDC2_segments] < chi2_DC2[sizeit1_1]){
			if (!replaced){
			  for (int ii = 0; ii < 8; ii++) {
			    rh_segDC2[ii][sizeit1_1] =  rh_segDC2[ii][nDC2_segments];
			    rh_sigm_segDC2[ii][nDC2_segments] =  rh_sigm_segDC2[ii][nDC2_segments];
			  }
			  size_segDC2[sizeit1_1] = size_segDC2[nDC2_segments];
			  chi2_DC2[sizeit1_1] = chi2_DC2[nDC2_segments];
		

			  par_ab2[0][sizeit1_1] = par_ab2[0][nDC2_segments];
			  par_ab2[2][sizeit1_1] = par_ab2[2][nDC2_segments];
			  par_ab2[1][sizeit1_1] = par_ab2[1][nDC2_segments];
			  par_ab2[3][sizeit1_1] = par_ab2[3][nDC2_segments];

			  xDC2_glob[sizeit1_1] = par_ab2[0][sizeit1_1]*(-99.75) + par_ab2[1][sizeit1_1];
			  yDC2_glob[sizeit1_1] = par_ab2[2][sizeit1_1]*(-99.75) + par_ab2[3][sizeit1_1];
			  
			  replaced = true;
			}else{
			  chi2_DC2[sizeit1_1] = 999.;//just mark seg as bad !!!! needs to be reviewed
			}
			break;//stop searching common hits
		      }//chi2 cand is better
		      else{
			for (int ii = 0; ii < 8; ii++) {
			  rh_segDC2[ii][nDC2_segments] = - 999.;
			  rh_sigm_segDC2[ii][nDC2_segments] = 1.;
			}
			size_segDC2[nDC2_segments] = 0;
			chi2_DC2[nDC2_segments] = 999.;
		      }//chi2 cand is worse
		      break;//stop searching common hits
		    }//size cand = size old
		    if (size_segDC2[nDC2_segments] < size_segDC2[sizeit1_1]) {
		      for (int ii = 0; ii < 8; ii++) {
			rh_segDC2[ii][nDC2_segments] = - 999.;
		      rh_sigm_segDC2[ii][nDC2_segments] = 1.;
		      }
		      size_segDC2[nDC2_segments] = 0;
		      chi2_DC2[nDC2_segments] = 999.;
		      break;//stop searching common hits
		    }//size cand < size old
		  }//cand has coomon hits with old
		}//hit
	      }//nDC2_segments-1
 	      if(chi2_DC2[nDC2_segments]>minChi2_up || replaced){
                for (int ii = 0; ii < 8; ii++) {
                  rh_segDC2[ii][nDC2_segments] = - 999.;
                  rh_sigm_segDC2[ii][nDC2_segments] = 1.;
                }
                size_segDC2[nDC2_segments] = 0;
                chi2_DC2[nDC2_segments] = 999.;
                continue;
              }

	      //cout<<" after fit chi2_DC2[nDC2_segments] "<<nDC2_segments<<" "<<chi2_DC2[nDC2_segments]<<" size "<<size_segDC2[nDC2_segments]<<endl;
	      xDC2_glob[nDC2_segments] = par_ab2[0][nDC2_segments]*(-99.75) + par_ab2[1][nDC2_segments];
	      yDC2_glob[nDC2_segments] = par_ab2[2][nDC2_segments]*(-99.75) + par_ab2[3][nDC2_segments];
	      nDC2_segments++;
	  }//found u || foundv   
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
	    double dX_thresh = 1.2;//1.5;
	    double cand_xa = -999;
	    double cand_xb = -999;
	    double cand_xa_sigm = -999;
	    double cand_xb_sigm = -999;
	    for (int k = 0; k < pair_x2; k++) {
	      float x_coord = (x2_ab[0][k] + x2_ab[1][k]) / 2;
	      if (nDC2_segments > 48)break;
	      if (fabs(x_coord - x_est) > dX_thresh)continue;
	      dX_thresh = fabs(x_coord - x_est);
	      cand_xa = x2_ab[0][k];
	      cand_xb = x2_ab[1][k];
	      cand_xa_sigm = sigm_x2_ab[0][k];
	      cand_xb_sigm = sigm_x2_ab[1][k];
	    }//k
	    if(dX_thresh < 1.2){
	      rh_segDC2[0][nDC2_segments] = cand_xa;
	      rh_segDC2[1][nDC2_segments] = cand_xb;
	      rh_segDC2[4][nDC2_segments] = u2_ab[0][i];
	      rh_segDC2[5][nDC2_segments] = u2_ab[1][i];
	      rh_segDC2[6][nDC2_segments] = v2_ab[0][j];
	      rh_segDC2[7][nDC2_segments] = v2_ab[1][j];
	      rh_sigm_segDC2[0][nDC2_segments] = cand_xa_sigm;
	      rh_sigm_segDC2[1][nDC2_segments] = cand_xb_sigm;
	      rh_sigm_segDC2[4][nDC2_segments] = sigm_u2_ab[0][i];
	      rh_sigm_segDC2[5][nDC2_segments] = sigm_u2_ab[1][i];
	      rh_sigm_segDC2[6][nDC2_segments] = sigm_v2_ab[0][j];
	      rh_sigm_segDC2[7][nDC2_segments] = sigm_v2_ab[1][j];

	      foundX = true;
	      if (nDC2_segments > 48)break;
	    }//dx_thresh<.25
	  }//(pair_x2>0)
	  if (found_8p_seg && !foundX)continue;
	  bool foundY = false;
	  if (pair_y2 > 0) {
	    double dY_thresh = 1.2;//1.0;
	    double cand_ya = -999;
	    double cand_yb = -999;
	    double cand_ya_sigm = -999;
	    double cand_yb_sigm = -999;
	    for (int m = 0; m < pair_y2; m++) {
	      if (nDC2_segments > 48)break;
	      float y_coord = (y2_ab[0][m] + y2_ab[1][m]) / 2;
	      if (fabs(y_coord - y_est) > dY_thresh)continue;
	      dY_thresh = fabs(y_coord - y_est);
	      cand_ya = y2_ab[0][m];
	      cand_yb = y2_ab[1][m];
	      cand_ya_sigm = sigm_y2_ab[0][m];
	      cand_yb_sigm = sigm_y2_ab[1][m];
	    }//m
	    if(dY_thresh < 1.2){
	      foundY = true;
	      rh_segDC2[2][nDC2_segments] = cand_ya;
	      rh_segDC2[3][nDC2_segments] = cand_yb;
	      rh_segDC2[4][nDC2_segments] = u2_ab[0][i];
	      rh_segDC2[5][nDC2_segments] = u2_ab[1][i];
	      rh_segDC2[6][nDC2_segments] = v2_ab[0][j];
	      rh_segDC2[7][nDC2_segments] = v2_ab[1][j];
	      rh_sigm_segDC2[2][nDC2_segments] = cand_ya_sigm;
	      rh_sigm_segDC2[3][nDC2_segments] = cand_yb_sigm;
	      rh_sigm_segDC2[4][nDC2_segments] = sigm_u2_ab[0][i];
	      rh_sigm_segDC2[5][nDC2_segments] = sigm_u2_ab[1][i];
	      rh_sigm_segDC2[6][nDC2_segments] = sigm_v2_ab[0][j];
	      rh_sigm_segDC2[7][nDC2_segments] = sigm_v2_ab[1][j];
	    }//dy_thresh<.25
	  }//pair y1
	  if (!foundX && foundY) {
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
                  
	  if (foundX || foundY){//see if the built seg is worth keeping and remove any dplicates if needed 
	      int worst_hit = -1;
	      double max_resid = 0;
	      for (int i = 0; i < 8; i++) {
		if (rh_segDC2[i][nDC2_segments] > -998.)size_segDC2[nDC2_segments]++;
	      }
	      for (int rej = 0; rej < 2; rej++) {//allow 2 passes max 8->7 & 7->6

		//end linear fit
		double rh_seg[8] = {-999};
		double rh_sigm_seg[8] = {-999};
		double par_ab[4] = {999, 999, 999, 999};

		for (int i = 0; i < 8; i++) {
		  rh_seg[i] = rh_segDC2[i][nDC2_segments];
		  rh_sigm_seg[i] = rh_sigm_segDC2[i][nDC2_segments];
		}

		fit_seg(rh_seg, rh_sigm_seg, par_ab, -1, -1); //usual fit without skipping any plane
		for (int i = 0; i < 4; i++) {
		  par_ab2[i][nDC2_segments] = par_ab[i];
		}

		chi2_DC2[nDC2_segments] = 0;

		double resid = 999;

		for (int i = 0; i < 8; i++) {
		  if (i == 0 && rh_segDC2[i][nDC2_segments] > -998.) {
		    resid = rh_segDC2[i][nDC2_segments] - z_loc[i] * par_ab2[0][nDC2_segments] - par_ab2[1][nDC2_segments];
		    chi2_DC2[nDC2_segments] += (resid * resid) / rh_sigm_segDC2[i][nDC2_segments];
		    if (fabs(resid) > max_resid) {
		      worst_hit = i;
		      max_resid = fabs(resid);
		    }
		  }
		  if (i == 1 && rh_segDC2[i][nDC2_segments] > -998.) {
		    resid = rh_segDC2[i][nDC2_segments] - z_loc[i] * par_ab2[0][nDC2_segments] - par_ab2[1][nDC2_segments];

		    chi2_DC2[nDC2_segments] += (resid * resid) / rh_sigm_segDC2[i][nDC2_segments];

		    if (fabs(resid) > max_resid) {
		      worst_hit = i;
		      max_resid = fabs(resid);
		    }
		  }
		  if (i == 2 && rh_segDC2[i][nDC2_segments] > -998.) {
		    resid = rh_segDC2[i][nDC2_segments] - z_loc[i] * par_ab2[2][nDC2_segments] - par_ab2[3][nDC2_segments];

		    chi2_DC2[nDC2_segments] += (resid * resid) / rh_sigm_segDC2[i][nDC2_segments];

		    if (fabs(resid) > max_resid) {
		      worst_hit = i;
		      max_resid = fabs(resid);
		    }
		  }
		  if (i == 3 && rh_segDC2[i][nDC2_segments] > -998.) {
		    resid = rh_segDC2[i][nDC2_segments] - z_loc[i] * par_ab2[2][nDC2_segments] - par_ab2[3][nDC2_segments];

		    chi2_DC2[nDC2_segments] += (resid * resid) / rh_sigm_segDC2[i][nDC2_segments];

		    if (fabs(resid) > max_resid) {
		      worst_hit = i;
		      max_resid = fabs(resid);
		    }
		  }
		  if (i == 4 && rh_segDC2[i][nDC2_segments] > -998.) {
		    resid = rh_segDC2[i][nDC2_segments] - isqrt_2 * z_loc[i]*(par_ab2[2][nDC2_segments] - par_ab2[0][nDC2_segments]) - isqrt_2 * (par_ab2[3][nDC2_segments] - par_ab2[1][nDC2_segments]);
		    chi2_DC2[nDC2_segments] += (resid * resid) / rh_sigm_segDC2[i][nDC2_segments];


		    if (fabs(resid) > max_resid) {
		      worst_hit = i;
		      max_resid = fabs(resid);
		    }
		  }
		  if (i == 5 && rh_segDC2[i][nDC2_segments] > -998.) {
		    resid = rh_segDC2[i][nDC2_segments] - isqrt_2 * z_loc[i]*(par_ab2[2][nDC2_segments] - par_ab2[0][nDC2_segments]) - isqrt_2 * (par_ab2[3][nDC2_segments] - par_ab2[1][nDC2_segments]);
		    chi2_DC2[nDC2_segments] += (resid * resid) / rh_sigm_segDC2[i][nDC2_segments];
		    if (fabs(resid) > max_resid) {
		      worst_hit = i;
		      max_resid = fabs(resid);
		    }
		  }
		  if (i == 6 && rh_segDC2[i][nDC2_segments] > -998.) {
		    resid = rh_segDC2[i][nDC2_segments] - isqrt_2 * z_loc[i]*(par_ab2[2][nDC2_segments] + par_ab2[0][nDC2_segments]) - isqrt_2 * (par_ab2[3][nDC2_segments] + par_ab2[1][nDC2_segments]);
		    chi2_DC2[nDC2_segments] += (resid * resid) / rh_sigm_segDC2[i][nDC2_segments];

		    if (fabs(resid) > max_resid) {
		      worst_hit = i;
		      max_resid = fabs(resid);
		    }
		  }
		  if (i == 7 && rh_segDC2[i][nDC2_segments] > -998.) {
		    resid = rh_segDC2[i][nDC2_segments] - isqrt_2 * z_loc[i]*(par_ab2[2][nDC2_segments] + par_ab2[0][nDC2_segments]) - isqrt_2 * (par_ab2[3][nDC2_segments] + par_ab2[1][nDC2_segments]);
		    chi2_DC2[nDC2_segments] += (resid * resid) / rh_sigm_segDC2[i][nDC2_segments];

		    if (fabs(resid) > max_resid) {
		      worst_hit = i;
		      max_resid = fabs(resid);
		    }
		  }
		}//i
		chi2_DC2[nDC2_segments] /= (size_segDC2[nDC2_segments] - 4);
		//if chi2 is big and seg_size = min erase this seg
		if(nDC2_segments > 24 ){
		  minChi2_up = 30;
		  minChi2_down = 20;
		}else{
		  minChi2_up = 50;
		  minChi2_down = 30;
		}

		if (chi2_DC2[nDC2_segments] > minChi2_down) {
		  if (size_segDC2[nDC2_segments] == 6) {
		    chi2_DC2[nDC2_segments] = 999;
		    break;
		  } else {
		    rh_segDC2[worst_hit][nDC2_segments] = -999; //erase worst hit and refit
		    size_segDC2[nDC2_segments]--;
		    max_resid = 0;
		    continue;
		  }
		}
	      }//rej 0 1 2
	                    
	      if(chi2_DC2[nDC2_segments]>minChi2_up){//need to add function for cleaning up the seg candidate
		//initialize its hits
		for (int ii = 0; ii < 8; ii++) {
		  rh_segDC2[ii][nDC2_segments] = - 999.;
		  rh_sigm_segDC2[ii][nDC2_segments] = 1.;
		}
		size_segDC2[nDC2_segments] = 0;
		chi2_DC2[nDC2_segments] = 999.;
		continue;
	      }//if chi2 too big

	      //see if the seg points towards the region of IP (rough check |x|<50 & |y|<30) 
	      par_ab2[0][nDC2_segments] = -1 * (par_ab2[0][nDC2_segments] + x2_slope_sh + x2_slope_sh * par_ab2[0][nDC2_segments] * par_ab2[0][nDC2_segments]);
	      par_ab2[2][nDC2_segments] = par_ab2[2][nDC2_segments] + y2_slope_sh + y2_slope_sh * par_ab2[2][nDC2_segments] * par_ab2[2][nDC2_segments];
	      par_ab2[1][nDC2_segments] = -1 * (par_ab2[1][nDC2_segments] + x2_sh);
	      par_ab2[3][nDC2_segments] = par_ab2[3][nDC2_segments] + y2_sh;

	      float x_z0 = par_ab2[0][nDC2_segments]*(-Z_dch2)+par_ab2[1][nDC2_segments];
	      float y_z0 = par_ab2[2][nDC2_segments]*(-Z_dch2)+par_ab2[3][nDC2_segments];
	      if(fabs(x_z0) > 50. || fabs(y_z0) > 30.){
		//initialize its hits
                for (int ii = 0; ii < 8; ii++) {
                  rh_segDC2[ii][nDC2_segments] = - 999.;
                  rh_sigm_segDC2[ii][nDC2_segments] = 1.;
                }
                size_segDC2[nDC2_segments] = 0;
                chi2_DC2[nDC2_segments] = 999.;
                continue;
	      }//if (is not poining to IP region)


	      //leave only longest and best chi2 segments
	      //dc2
	      bool replaced = false;
	      for (int sizeit1_1 = 0; sizeit1_1 < nDC2_segments-1; sizeit1_1++) {
		if(chi2_DC2[sizeit1_1]>50.)continue;	
		for (int hit = 0; hit < 4; hit++) {
		  if (rh_segDC2[2 * hit][nDC2_segments] == rh_segDC2[2 * hit][sizeit1_1] && rh_segDC2[2 * hit + 1][nDC2_segments] == rh_segDC2[2 * hit + 1][sizeit1_1]){
		    if (size_segDC2[nDC2_segments] > size_segDC2[sizeit1_1]) {
		      if (!replaced){		   
			for (int ii = 0; ii < 8; ii++) {
			  rh_segDC2[ii][sizeit1_1] =  rh_segDC2[ii][nDC2_segments];
			  rh_sigm_segDC2[ii][nDC2_segments] =  rh_sigm_segDC2[ii][nDC2_segments];
			}
			size_segDC2[sizeit1_1] = size_segDC2[nDC2_segments];
			chi2_DC2[sizeit1_1] = chi2_DC2[nDC2_segments];
			par_ab2[0][sizeit1_1] = par_ab2[0][nDC2_segments];
                        par_ab2[2][sizeit1_1] = par_ab2[2][nDC2_segments];
                        par_ab2[1][sizeit1_1] = par_ab2[1][nDC2_segments];
                        par_ab2[3][sizeit1_1] = par_ab2[3][nDC2_segments];

                        xDC2_glob[sizeit1_1] = par_ab2[0][sizeit1_1]*(-99.75) + par_ab2[1][sizeit1_1];
                        yDC2_glob[sizeit1_1] = par_ab2[2][sizeit1_1]*(-99.75) + par_ab2[3][sizeit1_1];

			
			replaced = true;
		      }else{
			chi2_DC2[sizeit1_1] = 999.;//just mark seg as bad !!!! needs to be reviewed		   
		      }
		      break;//stop searching common hits
		    }//size cand > size  old
		    if (size_segDC2[nDC2_segments] == size_segDC2[sizeit1_1]) {
		      if(chi2_DC2[nDC2_segments] < chi2_DC2[sizeit1_1]){
			if (!replaced){
			  for (int ii = 0; ii < 8; ii++) {
			    rh_segDC2[ii][sizeit1_1] =  rh_segDC2[ii][nDC2_segments];
			    rh_sigm_segDC2[ii][nDC2_segments] =  rh_sigm_segDC2[ii][nDC2_segments];
			  }
			  size_segDC2[sizeit1_1] = size_segDC2[nDC2_segments];
			  chi2_DC2[sizeit1_1] = chi2_DC2[nDC2_segments];
			  par_ab2[0][sizeit1_1] = par_ab2[0][nDC2_segments];
			  par_ab2[2][sizeit1_1] = par_ab2[2][nDC2_segments];
			  par_ab2[1][sizeit1_1] = par_ab2[1][nDC2_segments];
			  par_ab2[3][sizeit1_1] = par_ab2[3][nDC2_segments];

			  xDC2_glob[sizeit1_1] = par_ab2[0][sizeit1_1]*(-99.75) + par_ab2[1][sizeit1_1];
			  yDC2_glob[sizeit1_1] = par_ab2[2][sizeit1_1]*(-99.75) + par_ab2[3][sizeit1_1];
			  
			  replaced = true;
			}else{
			  chi2_DC2[sizeit1_1] = 999.;//just mark seg as bad !!!! needs to be reviewed
			}
			break;//stop searching common hits
		      }//chi2 cand is better
		      else{
			for (int ii = 0; ii < 8; ii++) {
			  rh_segDC2[ii][nDC2_segments] = - 999.;
			  rh_sigm_segDC2[ii][nDC2_segments] = 1.;
			}
			size_segDC2[nDC2_segments] = 0;
			chi2_DC2[nDC2_segments] = 999.;
		      }//chi2 cand is worse
		      break;//stop searching common hits
		    }//size cand = size old
		    if (size_segDC2[nDC2_segments] < size_segDC2[sizeit1_1]) {
		      for (int ii = 0; ii < 8; ii++) {
			rh_segDC2[ii][nDC2_segments] = - 999.;
		      rh_sigm_segDC2[ii][nDC2_segments] = 1.;
		      }
		      size_segDC2[nDC2_segments] = 0;
		      chi2_DC2[nDC2_segments] = 999.;
		      break;//stop searching common hits
		    }//size cand < size old
		  }//cand has coomon hits with old
		}//hit
	      }//nDC2_segments-1
	      if(chi2_DC2[nDC2_segments]>minChi2_up || replaced){
                for (int ii = 0; ii < 8; ii++) {
                  rh_segDC2[ii][nDC2_segments] = - 999.;
                  rh_sigm_segDC2[ii][nDC2_segments] = 1.;
                }
                size_segDC2[nDC2_segments] = 0;
                chi2_DC2[nDC2_segments] = 999.;
                continue;
              }
	      //cout<<" after fit chi2_DC2[nDC2_segments] "<<nDC2_segments<<" "<<chi2_DC2[nDC2_segments]<<" size "<<size_segDC2[nDC2_segments]<<endl;
	      xDC2_glob[nDC2_segments] = par_ab2[0][nDC2_segments]*(-99.75) + par_ab2[1][nDC2_segments];
	      yDC2_glob[nDC2_segments] = par_ab2[2][nDC2_segments]*(-99.75) + par_ab2[3][nDC2_segments];
	      nDC2_segments++;
	  }//foun x or y
	}//j
      }//i
    }//(u,v) first

    //linear fit for dch1 segs
    /*
    for (int j = 0; j < nDC1_segments; j++) {
      //fit the initial seg and see if the chi2/ndof is too big try to get rid of the hit with the biggest deviation from the fit
      int worst_hit = -1;
      double max_resid = 0;
      for (int i = 0; i < 8; i++) {
	if (rh_segDC1[i][j] > -998.)size_segDC1[j]++;
      }
      for (int rej = 0; rej < 2; rej++) {//allow 2 passes max 8->7 & 7->6

	double rh_seg[8] = {-999.};
	double rh_sigm_seg[8] = {-999.};
	double par_ab[4] = {999., 999., 999., 999.};

	for (int i = 0; i < 8; i++) {
	  rh_seg[i] = rh_segDC1[i][j];
	  rh_sigm_seg[i] = rh_sigm_segDC1[i][j];
	}

	fit_seg(rh_seg, rh_sigm_seg, par_ab, -1, -1); //usual fit without skipping any plane
	for (int i = 0; i < 4; i++) {
	  par_ab1[i][j] = par_ab[i];
	}

	chi2_DC1[j] = 0.;
	double resid = 999.;

	for (int i = 0; i < 8; i++) {
	  if (i == 0 && rh_segDC1[i][j] > -998.) {

	    resid = rh_segDC1[i][j] - z_loc[i] * par_ab1[0][j] - par_ab1[1][j];

	    if (fabs(resid) > max_resid) {
	      worst_hit = i;
	      max_resid = fabs(resid);
	    }
	    chi2_DC1[j] += (resid * resid) / rh_sigm_segDC1[i][j];

	  }
	  if (i == 1 && rh_segDC1[i][j] > -998.) {
	    resid = rh_segDC1[i][j] - z_loc[i] * par_ab1[0][j] - par_ab1[1][j];
	    if (fabs(resid) > max_resid) {
	      worst_hit = i;
	      max_resid = fabs(resid);
	    }
	    chi2_DC1[j] += (resid * resid) / rh_sigm_segDC1[i][j];

	  }
	  if (i == 2 && rh_segDC1[i][j] > -998.) {
	    resid = rh_segDC1[i][j] - z_loc[i] * par_ab1[2][j] - par_ab1[3][j];
	    if (fabs(resid) > max_resid) {
	      worst_hit = i;
	      max_resid = fabs(resid);
	    }
	    chi2_DC1[j] += (resid * resid) / rh_sigm_segDC1[i][j];

	  }
	  if (i == 3 && rh_segDC1[i][j] > -998.) {
	    resid = rh_segDC1[i][j] - z_loc[i] * par_ab1[2][j] - par_ab1[3][j];

	    if (fabs(resid) > max_resid) {
	      worst_hit = i;
	      max_resid = fabs(resid);
	    }
	    chi2_DC1[j] += (resid * resid) / rh_sigm_segDC1[i][j];
	  }
	  if (i == 4 && rh_segDC1[i][j] > -998.) {
	    resid = rh_segDC1[i][j] - isqrt_2 * z_loc[i]*(par_ab1[2][j] - par_ab1[0][j]) - isqrt_2 * (par_ab1[3][j] - par_ab1[1][j]);
	    if (fabs(resid) > max_resid) {
	      worst_hit = i;
	      max_resid = fabs(resid);
	    }
	    chi2_DC1[j] += (resid * resid) / rh_sigm_segDC1[i][j];

	  }
	  if (i == 5 && rh_segDC1[i][j] > -998.) {
	    resid = rh_segDC1[i][j] - isqrt_2 * z_loc[i]*(par_ab1[2][j] - par_ab1[0][j]) - isqrt_2 * (par_ab1[3][j] - par_ab1[1][j]);
	    if (fabs(resid) > max_resid) {
	      worst_hit = i;
	      max_resid = fabs(resid);
	    }
	    chi2_DC1[j] += (resid * resid) / rh_sigm_segDC1[i][j];
	  }
	  if (i == 6 && rh_segDC1[i][j] > -998.) {
	    resid = rh_segDC1[i][j] - isqrt_2 * z_loc[i]*(par_ab1[2][j] + par_ab1[0][j]) - isqrt_2 * (par_ab1[3][j] + par_ab1[1][j]);
	    if (fabs(resid) > max_resid) {
	      worst_hit = i;
	      max_resid = fabs(resid);
	    }
	    chi2_DC1[j] += (resid * resid) / rh_sigm_segDC1[i][j];
	  }
	  if (i == 7 && rh_segDC1[i][j] > -998.) {


	    resid = rh_segDC1[i][j] - isqrt_2 * z_loc[i]*(par_ab1[2][j] + par_ab1[0][j]) - isqrt_2 * (par_ab1[3][j] + par_ab1[1][j]);
	    if (fabs(resid) > max_resid) {
	      worst_hit = i;
	      max_resid = fabs(resid);
	    }
	    chi2_DC1[j] += (resid * resid) / rh_sigm_segDC1[i][j];

	  }
	}//i

	chi2_DC1[j] /= (size_segDC1[j] - 4);

	if (chi2_DC1[j] > 30.) {
	  if (size_segDC1[j] == 6) {
	    chi2_DC1[j] = 999.;
	    break;
	  } else {
	    rh_segDC1[worst_hit][j] = -999.; //erase worst hit and refit
	    size_segDC1[j]--;
	    max_resid = 0;
	    continue;
	  }
	}
      }//rej 0 1 2

	 

      //add shifts to slopes and coords
      par_ab1[0][j] = -1 * (par_ab1[0][j] + x1_slope_sh + x1_slope_sh * par_ab1[0][j] * par_ab1[0][j]);
      par_ab1[2][j] = par_ab1[2][j] + y1_slope_sh + y1_slope_sh * par_ab1[2][j] * par_ab1[2][j];
      par_ab1[1][j] = -1 * (par_ab1[1][j] + x1_sh);
      par_ab1[3][j] = par_ab1[3][j] + y1_sh;

      xDC1_glob[j] = par_ab1[0][j]*(99.75) + par_ab1[1][j];
      yDC1_glob[j] = par_ab1[2][j]*(99.75) + par_ab1[3][j];
      xDC1_glob_to_DC2[j] = par_ab1[0][j]*(199.5) + par_ab1[1][j];
      yDC1_glob_to_DC2[j] = par_ab1[2][j]*(199.5) + par_ab1[3][j];
         
	  
      if (size_segDC1[j] > 6) has7DC1 = true;
    }//for DC1 segs

    //linear fit for dch1 segs
    for (int j = 0; j < nDC2_segments; j++) {
      int worst_hit = -1;
      double max_resid = 0;
      for (int i = 0; i < 8; i++) {
	if (rh_segDC2[i][j] > -998.)size_segDC2[j]++;
      }
      for (int rej = 0; rej < 2; rej++) {//allow 2 passes max 8->7 & 7->6

	//end linear fit
	double rh_seg[8] = {-999.};
	double rh_sigm_seg[8] = {-999.};
	double par_ab[4] = {999., 999., 999., 999.};

	for (int i = 0; i < 8; i++) {
	  rh_seg[i] = rh_segDC2[i][j];
	  rh_sigm_seg[i] = rh_sigm_segDC2[i][j];

	}

	fit_seg(rh_seg, rh_sigm_seg, par_ab, -1, -1); //usual fit without skipping any plane
	for (int i = 0; i < 4; i++) {
	  par_ab2[i][j] = par_ab[i];
	}

	chi2_DC2[j] = 0.;

	double resid = 999.;

	for (int i = 0; i < 8; i++) {
	  //cout<<" chi2_DC2[j] calc "<<i<<" "<<rh_segDC2[i][j]<<" sigm "<< rh_sigm_segDC2[i][j] <<endl;

	  if (i == 0 && rh_segDC2[i][j] > -998.) {
	    resid = rh_segDC2[i][j] - z_loc[i] * par_ab2[0][j] - par_ab2[1][j];
	    chi2_DC2[j] += (resid * resid) / rh_sigm_segDC2[i][j];
	    //cout<<" chi2_DC2[j] calc "<<i<<" resid "<<resid<<" hit "<<rh_segDC2[i][j]<<"  z_loc[i]  "<< z_loc[i] <<" par_ab2[0][j] "<<par_ab2[0][j]<<" par_ab2[1][j] "<<par_ab2[1][j]<<endl;
	    if (fabs(resid) > max_resid) {
	      worst_hit = i;
	      max_resid = fabs(resid);
	    }
	  }
	  if (i == 1 && rh_segDC2[i][j] > -998.) {
	    resid = rh_segDC2[i][j] - z_loc[i] * par_ab2[0][j] - par_ab2[1][j];

	    chi2_DC2[j] += (resid * resid) / rh_sigm_segDC2[i][j];
            //cout<<" chi2_DC2[j] calc "<<i<<" resid "<<resid*resid<<" sigm "<<rh_sigm_segDC2[i][j]<<endl;

	    if (fabs(resid) > max_resid) {
	      worst_hit = i;
	      max_resid = fabs(resid);
	    }
	  }
	  if (i == 2 && rh_segDC2[i][j] > -998.) {
	    resid = rh_segDC2[i][j] - z_loc[i] * par_ab2[2][j] - par_ab2[3][j];

	    chi2_DC2[j] += (resid * resid) / rh_sigm_segDC2[i][j];
            //cout<<" chi2_DC2[j] calc "<<i<<" resid "<<resid*resid<<" sigm "<<rh_sigm_segDC2[i][j]<<endl;

	    if (fabs(resid) > max_resid) {
	      worst_hit = i;
	      max_resid = fabs(resid);
	    }
	  }
	  if (i == 3 && rh_segDC2[i][j] > -998.) {
	    resid = rh_segDC2[i][j] - z_loc[i] * par_ab2[2][j] - par_ab2[3][j];

	    chi2_DC2[j] += (resid * resid) / rh_sigm_segDC2[i][j];
            //cout<<" chi2_DC2[j] calc "<<i<<" resid "<<resid*resid<<" sigm "<<rh_sigm_segDC2[i][j]<<endl;

	    if (fabs(resid) > max_resid) {
	      worst_hit = i;
	      max_resid = fabs(resid);
	    }
	  }
	  if (i == 4 && rh_segDC2[i][j] > -998.) {
	    resid = rh_segDC2[i][j] - isqrt_2 * z_loc[i]*(par_ab2[2][j] - par_ab2[0][j]) - isqrt_2 * (par_ab2[3][j] - par_ab2[1][j]);
	    chi2_DC2[j] += (resid * resid) / rh_sigm_segDC2[i][j];

            //cout<<" chi2_DC2[j] calc "<<i<<" resid "<<resid*resid<<" sigm "<<rh_sigm_segDC2[i][j]<<endl;

	    if (fabs(resid) > max_resid) {
	      worst_hit = i;
	      max_resid = fabs(resid);
	    }
	  }
	  if (i == 5 && rh_segDC2[i][j] > -998.) {
	    resid = rh_segDC2[i][j] - isqrt_2 * z_loc[i]*(par_ab2[2][j] - par_ab2[0][j]) - isqrt_2 * (par_ab2[3][j] - par_ab2[1][j]);
	    chi2_DC2[j] += (resid * resid) / rh_sigm_segDC2[i][j];
            //cout<<" chi2_DC2[j] calc "<<i<<" resid "<<resid*resid<<" sigm "<<rh_sigm_segDC2[i][j]<<endl;
	    if (fabs(resid) > max_resid) {
	      worst_hit = i;
	      max_resid = fabs(resid);
	    }
	  }
	  if (i == 6 && rh_segDC2[i][j] > -998.) {
	    resid = rh_segDC2[i][j] - isqrt_2 * z_loc[i]*(par_ab2[2][j] + par_ab2[0][j]) - isqrt_2 * (par_ab2[3][j] + par_ab2[1][j]);
	    chi2_DC2[j] += (resid * resid) / rh_sigm_segDC2[i][j];
            //cout<<" chi2_DC2[j] calc "<<i<<" resid "<<resid*resid<<" sigm "<<rh_sigm_segDC2[i][j]<<endl;

	    if (fabs(resid) > max_resid) {
	      worst_hit = i;
	      max_resid = fabs(resid);
	    }
	  }
	  if (i == 7 && rh_segDC2[i][j] > -998.) {
	    resid = rh_segDC2[i][j] - isqrt_2 * z_loc[i]*(par_ab2[2][j] + par_ab2[0][j]) - isqrt_2 * (par_ab2[3][j] + par_ab2[1][j]);
	    chi2_DC2[j] += (resid * resid) / rh_sigm_segDC2[i][j];
            //cout<<" chi2_DC2[j] calc "<<i<<" resid "<<resid*resid<<" sigm "<<rh_sigm_segDC2[i][j]<<endl;

	    if (fabs(resid) > max_resid) {
	      worst_hit = i;
	      max_resid = fabs(resid);
	    }
	  }
	}//i
        //cout<<" chi2_DC2[j] before division "<<chi2_DC2[j]<<endl;

	chi2_DC2[j] /= (size_segDC2[j] - 4);
	//cout<<" chi2_DC2[j] after division "<<chi2_DC2[j]<<endl;
	//if chi2 is big and seg_size = min erase this seg
	if (chi2_DC2[j] > 30.) {
	  if (size_segDC2[j] == 6) {
	    chi2_DC2[j] = 999.;
	    break;
	  } else {
	    rh_segDC2[worst_hit][j] = -999.; //erase worst hit and refit
	    size_segDC2[j]--;
	    max_resid = 0.;
	    continue;
	  }
	}
      }//rej 0 1 2
	  
      //add shifts to slopes and coords
      par_ab2[0][j] = -1 * (par_ab2[0][j] + x2_slope_sh + x2_slope_sh * par_ab2[0][j] * par_ab2[0][j]);
      par_ab2[2][j] = par_ab2[2][j] + y2_slope_sh + y2_slope_sh * par_ab2[2][j] * par_ab2[2][j];
      par_ab2[1][j] = -1 * (par_ab2[1][j] + x2_sh);
      par_ab2[3][j] = par_ab2[3][j] + y2_sh;
      cout<<" after fit chi2_DC2[j] "<<j<<" size "<<size_segDC2[j]<<" chi2 "<<chi2_DC2[j]<<  " ax "<< par_ab2[0][j] <<" bx  "<<par_ab2[1][j]<<" ay "<<par_ab2[2][j]<<" by "<<par_ab2[3][j]<<endl;
      xDC2_glob[j] = par_ab2[0][j]*(-99.75) + par_ab2[1][j];
      yDC2_glob[j] = par_ab2[2][j]*(-99.75) + par_ab2[3][j];
      //
      if (size_segDC2[j] > 6) has7DC2 = true;
    }//for DC2 segs
    */
    //count the number of rh per segment
    double x_mid[50]; //x glob of matched segment in the z situated between the two DCH chambers
    double y_mid[50]; //y glob of matched segment in the z situated between the two DCH chambers
    double a_X[50]; //x slope of the matched segment
    double a_Y[50]; //y slope of the matched segment
    double imp[50]; //reconstructed particle trajectory momentum 
    double leng[50]; //the distance from z = 0 to the global point of the matched segment
    bool matched_dc1_seg_nr[50];//the matched dc1 segments to be skipped for out of dc 2 acceptance segments
    int pair_id[50];
    int seg_it = -1;
    int dc1_best[50];
    int dc2_best[50];
    double Chi2_match[50]; //chi2 of the matched seg
    for (int seg = 0; seg < 50; seg++) {
      x_mid[seg] = -999.;
      y_mid[seg] = -999.;
      a_X[seg] = -999.;
      a_Y[seg] = -999.;
      imp[seg] = -999.;
      leng[seg] = -999.;
      Chi2_match[seg] = 999.;
      pair_id[seg] = -1;
      dc1_best[seg] = 0;
      dc2_best[seg] = 0;
      matched_dc1_seg_nr[seg] = false;
    }


    float xMean = 999;
    float yMean = 999;

    for (int sizeit1 = 0; sizeit1 < nDC1_segments; sizeit1++) {

      //    cout<<" before final check seg nr "<<sizeit1<<" size "<<size_segDC1[sizeit1]<<" chi2 "<<chi2_DC1[sizeit1]<<  " ax "<< par_ab1[0][sizeit1] <<" bx  "<<par_ab1[1][sizeit1]<<" ay "<<par_ab1[2][sizeit1]<<" by "<<par_ab1[3][sizeit1]<<endl;
      // for(int hit = 0; hit < 8; hit++){cout<<" i "<<rh_segDC1[hit][sizeit1];}
      // cout<<endl;
    }
    //leave only longest and best chi2 segments
    //dc1
    for (int max_size = 8; max_size > 5; max_size--) {
      //find longest and best chi2 seg
      for (int sizeit1 = 0; sizeit1 < nDC1_segments; sizeit1++) {
	if (size_segDC1[sizeit1] != max_size || chi2_DC1[sizeit1] > 998.)continue;
	for (int sizeit1_1 = 0; sizeit1_1 < nDC1_segments; sizeit1_1++) {
	  if (sizeit1_1 == sizeit1 || chi2_DC1[sizeit1_1] > 998.)continue;
	  for (int hit = 0; hit < 4; hit++) {
	    if (rh_segDC1[2 * hit][sizeit1] == rh_segDC1[2 * hit][sizeit1_1] && rh_segDC1[2 * hit + 1][sizeit1] == rh_segDC1[2 * hit + 1][sizeit1_1] && (chi2_DC1[sizeit1] <= chi2_DC1[sizeit1_1] || size_segDC1[sizeit1] > size_segDC1[sizeit1_1])) {
	      chi2_DC1[sizeit1_1] = 999.; //mark seg as bad                                                                                                   
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
	if (size_segDC2[sizeit2] != max_size || chi2_DC2[sizeit2] > 998.)continue;
	for (int sizeit2_2 = 0; sizeit2_2 < nDC2_segments; sizeit2_2++) {
	  if (sizeit2_2 == sizeit2 || chi2_DC2[sizeit2_2] > 998.)continue;
	  for (int hit = 0; hit < 4; hit++) {
	    if (rh_segDC2[2 * hit][sizeit2] == rh_segDC2[2 * hit][sizeit2_2] && rh_segDC2[2 * hit + 1][sizeit2] == rh_segDC2[2 * hit + 1][sizeit2_2] && (chi2_DC2[sizeit2] <= chi2_DC2[sizeit2_2] || size_segDC2[sizeit2] > size_segDC2[sizeit2_2])) {
	      chi2_DC2[sizeit2_2] = 999.; //mark seg as bad                                                                                                     
	      break;
	    }
	  }//hit                                                                                                                                                 
	}
      }
    }//max_size           



    //fill local segments z,x,y global coords; x-slope; y-slope; Chi2; to be continued...
    for (int sizeit1 = 0; sizeit1 < nDC1_segments; sizeit1++) {
      if (chi2_DC1[sizeit1] > 50.) continue;
             
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
      //      TracksDch1.SetNDF(1[ind_best_Dch1[iBest]]); //WARNING!!! now it is number of hits in matched track!                                             
      BmnTrack* tr_dch1 = new((*dchTracks)[dchTracks->GetEntriesFast()]) BmnTrack();
      tr_dch1->SetChi2(chi2_DC1[sizeit1]);
      tr_dch1->SetNHits(size_segDC1[sizeit1]);
      tr_dch1->SetParamFirst(ParamsTrackDch1);
      tr_dch1->SetFlag(1000 * (sizeit1 + 1));

      hXDC1_atZ0->Fill(par_ab1[0][sizeit1]*(-Z_dch1)+par_ab1[1][sizeit1]);
      hYDC1_atZ0->Fill(par_ab1[2][sizeit1]*(-Z_dch1)+par_ab1[3][sizeit1]);
      //   cout<<" filling to branch seg nr "<<sizeit1<<" size "<<size_segDC1[sizeit1]<<" chi2 "<<chi2_DC1[sizeit1]<<  " ax "<< par_ab1[0][sizeit1] <<" bx  "<<par_ab1[1][sizeit1]<<" ay "<<par_ab1[2][sizeit1]<<" by "<<par_ab1[3][sizeit1]<<endl;



    }

    for (int sizeit2 = 0; sizeit2 < nDC2_segments; sizeit2++) {
      if (chi2_DC2[sizeit2] > 50.) continue;

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
      tr_dch2->SetFlag(sizeit2 + 1);

      hXDC2_atZ0->Fill(par_ab2[0][sizeit2]*(-Z_dch2)+par_ab2[1][sizeit2]);
      hYDC2_atZ0->Fill(par_ab2[2][sizeit2]*(-Z_dch2)+par_ab2[3][sizeit2]);

    }



  
   
    for(int j = 0; j<nDC1_segments; j++){
      if( chi2_DC1[j] > 50 )continue;

      //calculate segment param errors
      double XSum = 0.;
      double XSumZ = 0.;
      double XSumZZ = 0.;
      double YSum = 0.;
      double YSumZ = 0.;
      double YSumZZ = 0.;
      for (int i = 0; i < 8; i++) {
	if( rh_segDC1[i][j] == -999 || i == 2 || i == 3) continue;
	if(i == 0 || i == 1) {
	  XSum += 1./rh_sigm_segDC1[i][j];
	  XSumZ += z_loc[i]/rh_sigm_segDC1[i][j];
	  XSumZZ += z_loc[i]*z_loc[i]/rh_sigm_segDC1[i][j];
	}else{       
	  XSum += 1./(2.*rh_sigm_segDC1[i][j]);
	  XSumZ += z_loc[i]/(2.*rh_sigm_segDC1[i][j]);
	  XSumZZ += z_loc[i]*z_loc[i]/(2.*rh_sigm_segDC1[i][j]);
	}
      }
      for (int i = 0; i < 8; i++) {
	if( rh_segDC1[i][j] == -999 || i == 0 || i == 1) continue;
	if(i == 2 || i == 3) {
	  YSum += 1./rh_sigm_segDC1[i][j];
	  YSumZ += z_loc[i]/rh_sigm_segDC1[i][j];
	  YSumZZ += z_loc[i]*z_loc[i]/rh_sigm_segDC1[i][j];
	}else{       
	  YSum += 1./(2.*rh_sigm_segDC1[i][j]);
	  YSumZ += z_loc[i]/(2.*rh_sigm_segDC1[i][j]);
	  YSumZZ += z_loc[i]*z_loc[i]/(2.*rh_sigm_segDC1[i][j]);
	}
      }
      double Xdelta = XSum*XSumZZ- XSumZ*XSumZ;	   
      double Ydelta = YSum*YSumZZ- YSumZ*YSumZ;
      sigm_segDC1_par_ax[j] = sqrt(XSum/Xdelta); 
      sigm_segDC1_par_bx[j] = sqrt(XSumZZ/Xdelta);
      sigm_segDC1_par_ay[j] = sqrt(YSum/Ydelta); 
      sigm_segDC1_par_by[j] = sqrt(YSumZZ/Ydelta);
       //cout<<" seg j "<<j<<" size "<<size_segDC1[j]<<" sigm_segDC1_par_ax[j] "<<sigm_segDC1_par_ax[j]<<endl;  
      //cout<<" after duplicate cleaning chi2_DC1[j] "<<j<<" size "<<size_segDC1[j]<<" chi2 "<<chi2_DC1[j]<<  " ax "<< par_ab1[0][j] <<" bx  "<<par_ab1[1][j]<<" ay "<<par_ab1[2][j]<<" by "<<par_ab1[3][j]<<endl;

      //cout<<" ax "<<sigm_segDC1_par_ax[j]<<" bx "<<sigm_segDC1_par_bx[j]<<" ay "<<sigm_segDC1_par_ay[j]<<" by "<<sigm_segDC1_par_by[j]<<endl;	


    }//dc1 segs
    
    for(int j = 0; j<nDC2_segments; j++){
      if( chi2_DC2[j] > 50  )continue;
      //cout<<" after fit chi2_DC2[j] "<<j<<" "<<chi2_DC2[j]<<endl;
     
      //calculate segment param errors
      double XSum = 0.;
      double XSumZ = 0.;
      double XSumZZ = 0.;
      double YSum = 0.;
      double YSumZ = 0.;
      double YSumZZ = 0.;
      for (int i = 0; i < 8; i++) {
	if( rh_segDC2[i][j] == -999 || i == 2 || i == 3) continue;
	if(i == 0 || i == 1) {
	  XSum += 1./rh_sigm_segDC2[i][j];
	  XSumZ += z_loc[i]/rh_sigm_segDC2[i][j];
	  XSumZZ += z_loc[i]*z_loc[i]/rh_sigm_segDC2[i][j];
	}else{       
	  XSum += 1./(2.*rh_sigm_segDC2[i][j]);
	  XSumZ += z_loc[i]/(2.*rh_sigm_segDC2[i][j]);
	  XSumZZ += z_loc[i]*z_loc[i]/(2.*rh_sigm_segDC2[i][j]);
	}
      }
      for (int i = 0; i < 8; i++) {
	if( rh_segDC2[i][j] == -999 || i == 0 || i == 1) continue;
	if(i == 2 || i == 3) {
	  YSum += 1./rh_sigm_segDC2[i][j];
	  YSumZ += z_loc[i]/rh_sigm_segDC2[i][j];
	  YSumZZ += z_loc[i]*z_loc[i]/rh_sigm_segDC2[i][j];
	}else{       
	  YSum += 1./(2.*rh_sigm_segDC2[i][j]);
	  YSumZ += z_loc[i]/(2.*rh_sigm_segDC2[i][j]);
	  YSumZZ += z_loc[i]*z_loc[i]/(2.*rh_sigm_segDC2[i][j]);
	}
      }
      double Xdelta = XSum*XSumZZ- XSumZ*XSumZ;	   
      double Ydelta = YSum*YSumZZ- YSumZ*YSumZ;
      sigm_segDC2_par_ax[j] = sqrt(XSum/Xdelta); 
      sigm_segDC2_par_bx[j] = sqrt(XSumZZ/Xdelta);
      sigm_segDC2_par_ay[j] = sqrt(YSum/Ydelta); 
      sigm_segDC2_par_by[j] = sqrt(YSumZZ/Ydelta);
      //cout<<" after duplicate cleaning chi2_DC2[j] "<<j<<" size "<<size_segDC2[j]<<" chi2 "<<chi2_DC2[j]<<  " ax "<< par_ab2[0][j] <<" bx  "<<par_ab2[1][j]<<" ay "<<par_ab2[2][j]<<" by "<<par_ab2[3][j]<<endl;

      //       cout<<" dc2 seg j "<<j<<" size "<<size_segDC2[j]<<" chi2 "<<chi2_DC2[j]<<endl;  
    }

    hNr_segs_dc1->Fill(nDC1_segments);
    hNr_segs_dc2->Fill(nDC2_segments);

    //begin
    while(true){
      int match_dc2_seg = -1;
      float ax = -999.0;
      float ay = -999.0;
      float ax1 = -999.0;
      float ax2 = -999.0;
      float ay1 = -999.0;
      float ay2 = -999.0;
      float bx1, bx2, by1, by2;
      bx1 = bx2 = by1 = by2 = -999.0;
      int best1 = -1;
      int best2 = -1;
      double chi2_Match_min = 40.;
      float dx = -999;
      float dy = -999;
      float daX = -999;
      float daY = -999;
      for (int segdc1Nr = 0; segdc1Nr < nDC1_segments; segdc1Nr++) {
	//cout<<" bef dc1 nr "<<segdc1Nr<<" size "<<size_segDC1[segdc1Nr]<<endl;
if (chi2_DC1[segdc1Nr] > 50. || size_segDC1[segdc1Nr] < 6)continue; //skip rejected segs with chi2 = 999 
//cout<<" dc1 nr "<<segdc1Nr<<endl;   
	for (int segdc2Nr = 0; segdc2Nr < nDC2_segments; segdc2Nr++) {
	  // if(segdc2Nr == 71)//cout<<" bef dc2 nr "<<segdc2Nr<<" chi2 "<<chi2_DC2[segdc2Nr]<<segdc2Nr<<" size "<<size_segDC2[segdc2Nr]<<endl;    
  if (chi2_DC2[segdc2Nr] > 50. || size_segDC2[segdc2Nr] < 6 )continue; //skip rejected segs with chi2 = 999 
	  //  if(segdc2Nr == 71)//cout<<" dc2 nr "<<segdc2Nr<<" chi2 "<<chi2_DC2[segdc2Nr]<<segdc2Nr<<" size "<<size_segDC2[segdc2Nr]<<endl;     
	  dx = xDC2_glob[segdc2Nr] - xDC1_glob[segdc1Nr];
	  dy = yDC2_glob[segdc2Nr] - yDC1_glob[segdc1Nr];
	  daX = par_ab2[0][segdc2Nr] - par_ab1[0][segdc1Nr];
	  daY = par_ab2[2][segdc2Nr] - par_ab1[2][segdc1Nr];
	
	  xMean = 0.5 * (xDC1_glob[segdc1Nr] + xDC2_glob[segdc2Nr]);
	  yMean = 0.5 * (yDC1_glob[segdc1Nr] + yDC2_glob[segdc2Nr]);
	  ax = (par_ab2[1][segdc2Nr] - par_ab1[1][segdc1Nr]) / 199.5;
	  ay = (par_ab2[3][segdc2Nr] - par_ab1[3][segdc1Nr]) / 199.5;

	  ax1 = par_ab1[0][segdc1Nr];
	  ax2 = par_ab2[0][segdc2Nr];
	  ay1 = par_ab1[2][segdc1Nr];
	  ay2 = par_ab2[2][segdc2Nr];
	  bx2 = par_ab2[1][segdc2Nr];
	  by2 = par_ab2[3][segdc2Nr];
			
	  double chi2_match = 0.;
	  chi2_match = ( dx*dx/49.)+(dy*dy/ 43.56)+(daX *daX / 0.0144)+(daY*daY / 0.0225);
	  double sigma2_dx = ((sigm_segDC2_par_ax[segdc2Nr]*sigm_segDC2_par_ax[segdc2Nr])+(sigm_segDC1_par_ax[segdc1Nr]*sigm_segDC1_par_ax[segdc1Nr]))*99.75*99.75 + ((sigm_segDC1_par_bx[segdc1Nr])*(sigm_segDC1_par_bx[segdc1Nr])) + ((sigm_segDC2_par_bx[segdc2Nr])*(sigm_segDC2_par_bx[segdc2Nr]));
	  double sigma2_dy = ((sigm_segDC2_par_ay[segdc2Nr]*sigm_segDC2_par_ay[segdc2Nr])+(sigm_segDC1_par_ay[segdc1Nr]*sigm_segDC1_par_ay[segdc1Nr]))*99.75*99.75 + ((sigm_segDC1_par_by[segdc1Nr])*(sigm_segDC1_par_by[segdc1Nr])) + ((sigm_segDC2_par_by[segdc2Nr])*(sigm_segDC2_par_by[segdc2Nr])); 
	  double sigma2_dax = 4.*((sigm_segDC2_par_ax[segdc2Nr]*sigm_segDC2_par_ax[segdc2Nr])+(sigm_segDC1_par_ax[segdc1Nr]*sigm_segDC1_par_ax[segdc1Nr]));
	  double sigma2_day = 4.*((sigm_segDC2_par_ay[segdc2Nr]*sigm_segDC2_par_ay[segdc2Nr])+(sigm_segDC1_par_ay[segdc1Nr]*sigm_segDC1_par_ay[segdc1Nr]));
	  double chi2_Match = (((dx*dx)/sigma2_dx) + ((dy*dy)/sigma2_dy) + ((daX*daX)/sigma2_dax) + ((daY*daY)/sigma2_day))/(size_segDC1[segdc1Nr] + size_segDC2[segdc2Nr] - 8);
	  //cout<<" before matching chi2 "<<chi2_match<<" dx "<<dx<<" dy "<<dy<<" dax "<<daX<<" day "<<daY<<endl;

	  if(chi2_Match > chi2_Match_min || (size_segDC1[segdc1Nr] + size_segDC2[segdc2Nr]) < 13) continue;
	  if(fabs(dx) > 8. || fabs(dy) > 10. || fabs(daX) > 0.1 || fabs(daY) > 0.14 ) continue;
	  chi2_Match_min = chi2_Match;
	  best1 = segdc1Nr;
	  best2 = segdc2Nr;
	  	  //cout<<" !---- seg1 "<<segdc1Nr<<" np1 "<<size_segDC1[segdc1Nr]<<" seg2 "<<segdc2Nr<<" np2 "<<size_segDC2[segdc2Nr]<<endl;
	  //cout<<" chi2 "<<chi2_match<<" dx "<<dx<<" dy "<<dy<<" dax "<<daX<<" day "<<daY<<endl;
	 //cout<<" sigma ax1 "<<sigm_segDC1_par_ax[segdc1Nr]<<" sigma bx1 "<<sigm_segDC1_par_bx[segdc1Nr]<<" sigma ay1 "<<sigm_segDC1_par_ay[segdc1Nr]<<" sigma by1 "<<sigm_segDC1_par_by[segdc1Nr]<<endl;
	  //cout<<" sigma ax2 "<<sigm_segDC2_par_ax[segdc2Nr]<<" sigma bx2 "<<sigm_segDC2_par_bx[segdc2Nr]<<" sigma ay2 "<<sigm_segDC2_par_ay[segdc2Nr]<<" sigma by2 "<<sigm_segDC2_par_by[segdc2Nr]<<endl;
	  //cout<<" sigma dax "<<sqrt(sigma2_dax)<<" sigma day "<<sqrt(sigma2_day)<<endl;
	  //cout<<" sigma dx "<<sqrt(sigma2_dx)<<endl;
	  //cout<<" sigma dy "<<sqrt(sigma2_dy)<<endl;
	  //cout<<" chi2 matched "<<chi2_Match<<" chi2_Match_min "<<chi2_Match_min<<endl;
	} // segdc2Nr

	 
      } // segdc1Nr

      if(chi2_Match_min > 39.99)break;
      seg_it++;
      dx = xDC2_glob[best2] - xDC1_glob[best1];
      dy = yDC2_glob[best2] - yDC1_glob[best1];
      daX = par_ab2[0][best2] - par_ab1[0][best1];
      daY = par_ab2[2][best2] - par_ab1[2][best1];

      xMean = 0.5 * (xDC1_glob[best1] + xDC2_glob[best2]);
      yMean = 0.5 * (yDC1_glob[best1] + yDC2_glob[best2]);

           //cout<<"!!!! seg matched dc1 "<<best1<<" size "<<size_segDC1[best1]<<" dc2 "<<best2<<" size "<<size_segDC2[best2]<<endl;
      leng[seg_it] = sqrt(Z_dch_mid * Z_dch_mid + (xMean * xMean));
      pair_id[seg_it] = 1000 * (best1 + 1) + best2 + 1;
      x_mid[seg_it] = par_ab1[1][best1];//xDC1_glob[best1];//xMean;
      y_mid[seg_it] = par_ab1[3][best1];//yDC1_glob[best1];//yMean;
      a_X[seg_it] = (par_ab2[1][best2] - par_ab1[1][best1]) / 199.5;
      a_Y[seg_it] = (par_ab2[3][best2] - par_ab1[3][best1]) / 199.5;
      imp[seg_it] = -0.4332 / (ax + 0.006774);
      Chi2_match[seg_it] = chi2_Match_min;
      matched_dc1_seg_nr[best1] = true;

      BmnTrack TracksDch1_matched;
      FairTrackParam ParamsTrackDch1_matched;
      Float_t z0 = Z_dch1;
      Float_t x0 = par_ab1[1][best1];
      Float_t y0 = par_ab1[3][best1];
      ParamsTrackDch1_matched.SetPosition(TVector3(x0, y0, z0));
      ParamsTrackDch1_matched.SetTx(a_X[seg_it]);
      ParamsTrackDch1_matched.SetTy(a_Y[seg_it]);
      ParamsTrackDch1_matched.SetCovariance(0,0,sigm_segDC1_par_bx[best1]);
      ParamsTrackDch1_matched.SetCovariance(1,1,sigm_segDC1_par_by[best1]);
      ParamsTrackDch1_matched.SetCovariance(2,2,(((sigm_segDC1_par_bx[best1]*sigm_segDC1_par_bx[best1]) + (sigm_segDC2_par_bx[best2]*sigm_segDC2_par_bx[best2])))/39800.25);
      ParamsTrackDch1_matched.SetCovariance(3,3,(((sigm_segDC1_par_by[best1]*sigm_segDC1_par_by[best1]) + (sigm_segDC2_par_by[best2]*sigm_segDC2_par_by[best2])))/39800.25);

      //  TracksDch1.SetParamFirst(ParamsTrackDch1);
      //TracksDch1.SetChi2(chi2_DC1[best1]);
      //TracksDch1.SetNDF(Nhits_Dch1[ind_best_Dch1[iBest]]); //WARNING!!! now it is number of hits in matched track!                                             
      BmnTrack* tr_dch1_matched = new((*dchMatchedTracks)[dchMatchedTracks->GetEntriesFast()]) BmnTrack();
      tr_dch1_matched->SetChi2(Chi2_match[seg_it]);
      tr_dch1_matched->SetNHits(size_segDC1[best1]+size_segDC2[best2]);
      tr_dch1_matched->SetParamFirst(ParamsTrackDch1_matched);
      tr_dch1_matched->SetFlag(1000 * (best1 + 1));
      hX_matchedZ1->Fill(par_ab1[1][best1]);
      hX_matched->Fill(a_X[seg_it]*(662. - z0) + x0);
      chi2_DC1[best1] = 999; //mark dch1 seg as used for future iterations
      chi2_DC2[best2] = 999; //mark dch2 seg as used for future iterations

      hX_matched_p_outDC2->Fill( a_X[seg_it]*(662. - z0) + x0);


    } // while
 
    for(int j = 0; j<nDC1_segments; j++){
      if( chi2_DC1[j] > 50.  ||  size_segDC1[j]< 7 ||  matched_dc1_seg_nr[j])continue;
    
      if((xDC1_glob_to_DC2[j] > -29.56 && xDC1_glob_to_DC2[j] < 114.44) && (yDC1_glob_to_DC2[j] > -58.47 && yDC1_glob_to_DC2[j] < 53.53))continue;

      //fill branck with dc1 seg that is out of dc2 acceptance
      BmnTrack TracksDch1_out_of_dch2_accept;
      FairTrackParam ParamsTrackDch1_out_of_dch2_accept;
      Float_t z0 = Z_dch1;
      Float_t x0 = par_ab1[1][j];
      Float_t y0 = par_ab1[3][j];
      ParamsTrackDch1_out_of_dch2_accept.SetPosition(TVector3(x0, y0, z0));
      ParamsTrackDch1_out_of_dch2_accept.SetTx(par_ab1[0][j]);
      ParamsTrackDch1_out_of_dch2_accept.SetTy(par_ab1[2][j]);
      ParamsTrackDch1_out_of_dch2_accept.SetCovariance(0,0,sigm_segDC1_par_bx[j]);
      ParamsTrackDch1_out_of_dch2_accept.SetCovariance(1,1,sigm_segDC1_par_by[j]);
      ParamsTrackDch1_out_of_dch2_accept.SetCovariance(2,2,sigm_segDC1_par_ax[j]);
      ParamsTrackDch1_out_of_dch2_accept.SetCovariance(3,3,sigm_segDC1_par_ay[j]);
      hX_matched_p_outDC2->Fill( par_ab1[0][j]*(662.-z0) + x0);

      //  TracksDch1.SetParamFirst(ParamsTrackDch1);
      //TracksDch1.SetChi2(chi2_DC1[j]);
      //TracksDch1.SetNDF(Nhits_Dch1[ind_best_Dch1[iBest]]); //WARNING!!! now it is number of hits in matched track!                                             
      BmnTrack* tr_dch1_out_of_dch2_accept = new((*dch1Tracks_out_of_dch2_accept)[dch1Tracks_out_of_dch2_accept->GetEntriesFast()]) BmnTrack();
      tr_dch1_out_of_dch2_accept->SetChi2(chi2_DC1[j]);
      tr_dch1_out_of_dch2_accept->SetNHits(size_segDC1[j]);
      tr_dch1_out_of_dch2_accept->SetParamFirst(ParamsTrackDch1_out_of_dch2_accept);
      tr_dch1_out_of_dch2_accept->SetFlag(1000 * (j + 1));
     

    }

 

    tReco->Fill();

  } // event loop

  fReco->cd();
  hXb_ccup->Write();
  hXa_ccup->Write();

  hX_matched_p_outDC2->Write();
  hX_matched->Write();
  hX_matchedZ1->Write();
  hNr_segs_dc1->Write();
  hNr_segs_dc2->Write();
  hXDC1_atZ0->Write();
  hYDC1_atZ0->Write();
  hXDC2_atZ0->Write();
  hYDC2_atZ0->Write();

  tReco->Write();

  fReco->Close();

}

// LocalWords:  GetWireNumber X
