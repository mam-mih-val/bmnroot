// Author: Vasilisa Lenivenko <vasilisa@jinr.ru> 2018-07-18

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// BmnMwpcHitFinder                                                           //
//                                                                            //
// Implementation of an algorithm developed by                                //
// Vasilisa Lenivenko and Vladimir Palchik                                    //
// to the BmnRoot software                                                    //
//                                                                            //
// The algorithm serves for searching for hits                                //
// in the Multi Wire Prop. Chambers of the BM@N experiment                    //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
#include <Rtypes.h>
#include <climits>
#include <vector>
#include "TCanvas.h"
#include "TFile.h"
#include "BmnMwpcHitFinder.h"
#include "BmnTrack.h"
#include "BmnMwpcTrack.h"
#include "BmnMwpcSegment.h"
#include <BmnEventHeader.h>
#include <algorithm>

static Float_t workTime = 0.0;

using namespace std;
using namespace TMath;

struct segments {
  Int_t  Nhits   = 0;
  Double_t Chi2   = 999.;
  Double_t coord[6] = {-999., -999., -999., -999., -999., -999.};
  Double_t clust[6] = {0.,  0.,  0.,  0.,  0.,  0.};
  Double_t param[4] = { 999., 999., 999., 999.};
  Double_t sigma2[4][4] = {{ 0., 0., 0., 0.},{ 0., 0., 0., 0.},{ 0., 0., 0., 0.},{ 0., 0., 0., 0.}};
};

bool compareSegments(const segments &a, const segments &b) {
  return a.Chi2 < b.Chi2;
}

BmnMwpcHitFinder::BmnMwpcHitFinder(Bool_t isExp, Int_t runPeriod, Int_t runNumber) :
   fEventNo(0),
  expData(isExp) {
    
  if(expData){ 
    fRunPeriod    = runPeriod;
    fRunNumber    = runNumber;
    fInputBranchName = "MWPC";
    fBmnEventHeaderBranchName = "EventHeader";
    
    if(fRunPeriod == 6 || (fRunPeriod == 7 && fRunNumber > 3588) ) { //bmn
    kNumPairs     = 1;
    kCh_max       = 2;
    } else if(fRunPeriod == 7 && fRunNumber <= 3588) { //src
      kNumPairs   = 2;
      kCh_max     = 4;
    }
  }else{
    fInputBranchName = "BmnMwpcHit";
    kNumPairs    = 2;
    kCh_max      = 4;
    fRunPeriod   = 7;
    fRunNumber   = 0001;
  }
  fOutputBranchName = "BmnMwpcSegment";
  nInputDigits   = 3;
  nTimeSamples   = 3;
  kBig           = 100;
  kCh_min        = 0;
  //fBmnEvQualityBranchName = "BmnEventQuality";
}

BmnMwpcHitFinder::~BmnMwpcHitFinder() {
}

InitStatus BmnMwpcHitFinder::Init() {
 if (!expData) {
     if (fDebug) cout << "BmnMwpcHitFinder::Init(): simulation data is reconstructed " << endl;
    SetActive(kTRUE);
  }else{
    if (fDebug) cout << " BmnMwpcHitFinder::Init() " << endl;
  }

  FairRootManager* ioman = FairRootManager::Instance();
  
  if (expData) {
    fBmnMwpcDigitArray = (TClonesArray*) ioman->GetObject(fInputBranchName);
    if (!fBmnMwpcDigitArray) {
      cout<<"BmnMwpcHitFinder::Init(): branch "<<fInputBranchName<<" not found! Task will be deactivated"<<endl;
      SetActive(kFALSE);
      return kERROR;
    }
  }else{
    fBmnHitsArray = (TClonesArray*)ioman->GetObject(fInputBranchName);
    cout << "fBmnHitsArray = " << fInputBranchName << "\n";
    if (!fBmnHitsArray) {
      cout << "BmnMwpcHitFinder::Init(): branch " << fInputBranchName << " not found! Task will be deactivated" << endl;
      SetActive(kFALSE);
      return kERROR;
    }
  }

  fBmnMwpcSegmentsArray = new TClonesArray(fOutputBranchName);//fBmnMwpcSegmentsArray = new TClonesArray(fOutputBranchName.Data());
  ioman->Register(fOutputBranchName.Data(), "MWPC", fBmnMwpcSegmentsArray, kTRUE);
  
  //fMwpcGeo = new BmnMwpcGeometrySRC(fRunPeriod, fRunNumber);
  fMwpcGeometrySRC = new BmnMwpcGeometrySRC(fRunPeriod, fRunNumber);
  kNChambers = fMwpcGeometrySRC -> GetNChambers();
  kNPlanes  = fMwpcGeometrySRC -> GetNPlanes();
  kNWires  = fMwpcGeometrySRC -> GetNWires();
  if(fDebug) printf("C-P-W: %d %d %d\n", kNChambers, kNPlanes, kNWires);
  // cout<<" MWPC runPeriod "<<fRunPeriod<<" fRunNumber "<<fRunNumber<<endl;

  ChCent= new TVector3[kNChambers];
  ChZ =  new Float_t[kNChambers];
  Zmid = new Float_t[kNChambers];

  kChi2_Max = 20.;

  for (int i=0; i < kNChambers; ++i) {

    if (fDebug) {
      TH1D *h;

      h = new TH1D(Form("Chisq_ndf_Ch%d",i), Form("Chisq_ndf_Ch%d", i), 100, 0.,kChi2_Max);fList.Add(h);hChisq_ndf_Ch.push_back(h);
      h = new TH1D(Form("Np_best_Ch%d", i), Form("Np_best_Ch%d", i), 6, 1.0, 7.0);fList.Add(h);hNp_best_Ch.push_back(h);
      h = new TH1D(Form("Nbest_Ch%d",  i), Form("Nbest_Ch%d",  i), 6, 0.0, 6.0);fList.Add(h);hNbest_Ch.push_back(h);
      h = new TH1D(Form("NFired_layers_Ch%d",  i), Form("NFired_layers_Ch%d; NFired_layers; Events",  i), 7, 0.0, 7.0); fList.Add(h);hNFired_layers_Ch.push_back(h);
      h = new TH1D(Form("Num_layers_out_beam_Ch%d",  i), Form("Num_layers_out_beam_Ch%d; NFired_layers; Events",  i), 7, 0.0, 7.0);
      fList.Add(h);
      hNum_layers_out_beam_Ch.push_back(h);
      h = new TH1D(Form("Residuals_pl0_Ch%d",  i),Form("Residuals_pl0_Ch%d",i), 10, 0.,10.);
      fList.Add(h);
      hResiduals_pl0_Ch.push_back(h);
      h = new TH1D(Form("Residuals_pl1_Ch%d",  i),Form("Residuals_pl1_Ch%d",i), 10, 0.,10.);
      fList.Add(h);
      hResiduals_pl1_Ch.push_back(h);
      h = new TH1D(Form("Residuals_pl2_Ch%d",  i),Form("Residuals_pl2_Ch%d",i), 10, 0.,10.);
      fList.Add(h);
      hResiduals_pl2_Ch.push_back(h);
      h = new TH1D(Form("Residuals_pl3_Ch%d",  i),Form("Residuals_pl3_Ch%d",i), 10, 0.,10.);
      fList.Add(h);
      hResiduals_pl3_Ch.push_back(h);
      h = new TH1D(Form("Residuals_pl4_Ch%d",  i),Form("Residuals_pl4_Ch%d",i), 10, 0.,10.);
      fList.Add(h);
      hResiduals_pl4_Ch.push_back(h);
      h = new TH1D(Form("Residuals_pl5_Ch%d",  i),Form("Residuals_pl5_Ch%d",i), 10, 0.,10.);
      fList.Add(h);
      hResiduals_pl5_Ch.push_back(h);
      h = new TH1D(Form("occupancyXp%d", i), Form("occupancyXp%d",i), 250, -10.,10.);
      fList.Add(h);
      hoccupancyXp.push_back(h);
      h = new TH1D(Form("occupancyUp%d", i), Form("occupancyUp%d",i), 250, -10.,10.);
      fList.Add(h);
      hoccupancyUp.push_back(h);
      h = new TH1D(Form("occupancyVp%d", i), Form("occupancyVp%d",i), 250, -10.,10.);
      fList.Add(h);
      hoccupancyVp.push_back(h);
      h = new TH1D(Form("occupancyXm%d", i), Form("occupancyXm%d",i), 250, -10.,10.);
      fList.Add(h);
      hoccupancyXm.push_back(h);
      h = new TH1D(Form("occupancyUm%d", i), Form("occupancyUm%d",i), 250, -10.,10.);
      fList.Add(h);
      hoccupancyUm.push_back(h);
      h = new TH1D(Form("occupancyVm%d", i), Form("occupancyVm%d",i), 250, -10.,10.);
      fList.Add(h);
      hoccupancyVm.push_back(h);
      h = new TH1D(Form("firedWire_Ch%d", i), Form("firedWire_Ch%d",i), 100, 0., 100.);
      fList.Add(h);
      hfiredWire_Ch.push_back(h);
      h = new TH1D(Form("ClusterSize_Ch%d", i), Form("ClusterSize_Ch%d", i), 15, 0.,15.);
      fList.Add(h);
      hClusterSize.push_back(h);
      
      h = new TH1D(Form("fired_wire_Ch%d", i),Form("fired_wire_Ch%d;Wires;Events", i), kNWires*6, 0., kNWires*6);
      fList.Add(h);
      hfired_wire_Ch.push_back(h);
      
      h = new TH1D(Form("WiresXm_Ch%d", i),Form("WiresXm_Ch%d", i),kNWires, 0., kNWires);
      fList.Add(h);
      hWiresXm.push_back(h);
      h = new TH1D(Form("WiresVm_Ch%d", i),Form("WiresVm_Ch%d", i),kNWires, 0., kNWires);
      fList.Add(h);
      hWiresVm.push_back(h);
      h = new TH1D(Form("WiresUp_Ch%d", i),Form("WiresUp_Ch%d", i),kNWires, 0., kNWires);
      fList.Add(h);
      hWiresUp.push_back(h);
      h = new TH1D(Form("WiresXp_Ch%d", i),Form("WiresXp_Ch%d", i),kNWires, 0., kNWires);
      fList.Add(h);
      hWiresXp.push_back(h);
      h = new TH1D(Form("WiresVp_Ch%d", i),Form("WiresVp_Ch%d", i),kNWires, 0., kNWires);
      fList.Add(h);
      hWiresVp.push_back(h);
      h = new TH1D(Form("WiresUm_Ch%d", i),Form("WiresUm_Ch%d", i),kNWires, 0., kNWires);
      fList.Add(h);
      hWiresUm.push_back(h);

      TH2D *h2;
      h2 = new TH2D(Form("Event_display_Ch%d", i), Form("Event_display_Ch%d; Events; Wires",i), 100, 0., 700., 100, 0., 100.);
      fList.Add(h2);
      hEvent_display_Ch.push_back(h2);
      
      h2 = new TH2D(Form("all_pl_time_wire_Ch%d",  i),Form("time_wire_allplane_Ch%d; Wires; Time",  i), kNWires*6, 0., kNWires*6, 100, 0, 500.);
      fList.Add(h2);
      htime_wire_Ch.push_back(h2);
      
      
    }
    ChCent[i] = fMwpcGeometrySRC->GetChamberCenter(i);
    ChZ[i]  = ChCent[i].Z();
  }
  if (fDebug) {
    
    for (int i=0; i < kNChambers*kNPlanes; ++i) {
      TH1D *h;
      h = new TH1D(Form("Time%d", i), Form("Time%d", i), 500, 0., 500.);
      fList.Add(h);
      hTime.push_back(h);
      h = new TH1D(Form("Occupancy%d", i), Form("Occupancy%d", i), 100, 0., 100.);
      fList.Add(h);
      hOccupancy.push_back(h);
    }
    
  }
  // Segment finding cuts
  kMinHits = 4;//5;// for alignment kMinHits = 6;
  kMinHits_before_target = 5;
  kmaxSeg = 15;
  kChMaxAllWires = 200;
  // Constants
  dw    = fMwpcGeometrySRC -> GetWireStep(); //0.25; // [cm] // wires step
  dw_half  = 0.5 * dw;
  sq3    = sqrt(3.);
  sq12   = sqrt(12.);
  sigma   = dw / sq12;
  kMiddlePl = 47.25; // Center of wires plane
  // Matrices
  matrA = new Double_t*[4];
  matrb = new Double_t*[4];
  Amatr = new Double_t*[4];
  bmatr = new Double_t*[4];
  // Arrays
  Nlay_w_wires  = new Int_t[kNChambers];
  kPln          = new Int_t*[kNChambers];
  iw_Ch         = new Int_t*[kNChambers];
  shift         = new Float_t*[kNChambers];
  wire_Ch       = new Int_t**[kNChambers];
  xuv_Ch        = new Float_t**[kNChambers];
  Wires_Ch      = new Int_t**[kNChambers];
  clust_Ch      = new Int_t**[kNChambers];
  XVU_Ch        = new Float_t**[kNChambers];
  Nhits_Ch      = new Int_t*[kNChambers];
  Nseg_Ch       = new Int_t[kNChambers];
  Nbest_Ch      = new Int_t[kNChambers];
  ind_best_Ch   = new Int_t*[kNChambers];
  best_Ch_gl    = new Int_t*[kNChambers];
  Chi2_ndf_Ch   = new Double_t*[kNChambers];
  Chi2_ndf_best_Ch = new Double_t*[kNChambers];
  par_ab_Ch     = new Double_t**[kNChambers];
  XVU           = new Float_t*[kNChambers];
  XVU_cl        = new Float_t*[kNChambers];
  kZ_loc        = new Float_t*[kNChambers];
  z_gl          = new Float_t*[kNChambers];
  sigm2         = new Float_t[kNPlanes];
  ipl           = new Int_t[kNPlanes];
  z2            = new Float_t[kNPlanes];
  DigitsArray   = new Double_t**[kNChambers];
  ClusterSize   = new Int_t**[kNChambers];
  Nclust        = new Int_t*[kNChambers];
  Coord_wire    = new Double_t**[kNChambers];
  Coord_xuv     = new Double_t**[kNChambers];
  XVU_coord     = new Double_t**[kNChambers];
  Cluster_coord = new Double_t**[kNChambers];
  Nhits_seg     = new Int_t*[kNChambers];
  Chi2_ndf_seg  = new Double_t*[kNChambers];
  Coor_seg      = new Double_t**[kNChambers];
  Cluster_seg   = new Double_t**[kNChambers];
  par_ab_seg    = new Double_t**[kNChambers];
  sigma2_seg    = new Double_t***[kNChambers];
  Nbest_seg     = new Int_t[kNChambers];

  for(Int_t i = 0; i < kNChambers; ++i) {

    if (i== 0 || i== 2) {
      Zmid[i] = (ChZ[i]   - ChZ[i + 1]) * 0.5;
    }
    if (i== 1 || i== 3) {
      Zmid[i] = (ChZ[i - 1] - ChZ[i])   * -0.5;
    }
    if (fDebug) printf("Chamber %d Z: %f Zmid: %f\n", i, ChZ[i], Zmid[i]);

    kPln[i]          = new Int_t[kNPlanes];
    iw_Ch[i]         = new Int_t[kNPlanes];
    kZ_loc[i]        = new Float_t[kNPlanes];
    z_gl[i]          = new Float_t[kNPlanes];
    Nhits_Ch[i]      = new Int_t[kBig];
    shift[i]         = new Float_t[4];
    wire_Ch[i]       = new Int_t*[kNWires];
    xuv_Ch[i]        = new Float_t*[kNWires];
    Wires_Ch[i]      = new Int_t*[kNPlanes];
    clust_Ch[i]      = new Int_t*[kNPlanes];
    XVU_Ch[i]        = new Float_t*[kNPlanes];
    par_ab_Ch[i]     = new Double_t*[4];
    XVU[i]           = new Float_t[kNPlanes];
    XVU_cl[i]        = new Float_t[kNPlanes];
    counter_pl       = new Int_t[kNPlanes];
    ind_best_Ch[i]   = new Int_t[kmaxSeg];
    best_Ch_gl[i]    = new Int_t[kmaxSeg];
    Chi2_ndf_Ch[i]   = new Double_t[kBig];
    Chi2_ndf_best_Ch[i] = new Double_t[kmaxSeg];
    DigitsArray[i]   = new Double_t*[kNPlanes];
    ClusterSize[i]   = new Int_t*[kNPlanes];
    Nclust[i]        = new Int_t[kNPlanes];
    Coord_wire[i]    = new Double_t*[kNPlanes];
    Coord_xuv[i]     = new Double_t*[kNPlanes];
    XVU_coord[i]     = new Double_t*[kNPlanes];
    Cluster_coord[i] = new Double_t*[kNPlanes];
    Nhits_seg[i]     = new Int_t[kBig];
    Chi2_ndf_seg[i]  = new Double_t[kBig];
    Coor_seg[i]      = new Double_t*[kNPlanes];
    Cluster_seg[i]   = new Double_t*[kNPlanes];
    par_ab_seg[i]    = new Double_t*[4];
    sigma2_seg[i]    = new Double_t**[kBig];

    shift[i][0]   = fMwpcGeometrySRC -> GetTx(i);
    shift[i][2]   = fMwpcGeometrySRC -> GetTy(i);
    shift[i][1]   = ChCent[i].X();
    shift[i][3]   = ChCent[i].Y();

    for(int iWire = 0; iWire < kNWires; iWire++) {
      wire_Ch[i][iWire] = new Int_t[kNPlanes];
      xuv_Ch[i][iWire] = new Float_t[kNPlanes];
    }
    for(int iPla = 0; iPla < kNPlanes; ++iPla) {
      Wires_Ch[i][iPla]    = new Int_t[kBig];
      clust_Ch[i][iPla]    = new Int_t[kBig];
      XVU_Ch[i][iPla]      = new Float_t[kBig];
      DigitsArray[i][iPla] = new Double_t[kNWires];
      ClusterSize[i][iPla] = new Int_t[kBig];
      Coord_wire[i][iPla]  = new Double_t[kBig];
      Coord_xuv[i][iPla]   = new Double_t[kBig];
      XVU_coord[i][iPla]   = new Double_t[kBig];
      Cluster_coord[i][iPla]= new Double_t[kBig];
      Coor_seg[i][iPla]    = new Double_t[kBig];
      Cluster_seg[i][iPla] = new Double_t[kBig];

      if (fRunPeriod == 6 || (fRunPeriod == 7 && fRunNumber > 3588) ) { // if ( i == 2 || i == 3){

        kPln[i][0] = 4;
        kPln[i][1] = 5;
        kPln[i][2] = 0;
        kPln[i][3] = 1;
        kPln[i][4] = 2;
        kPln[i][5] = 3;//{4,5,0,1,2,3, 7,11,6,10,9,8, 0,0,0,0,0,0, 0,0,0,0,0,0};
        kZ_loc[i][iPla] = -0.5 + iPla;
        if(iPla == 4) {
          kZ_loc[i][iPla] = -2.5;
        }
        if(iPla == 5) {
          kZ_loc[i][iPla] = -1.5;
        }
      }
      if (fRunPeriod == 7 && fRunNumber <= 3588 ) { //SRC

        if ( i == 0 ) {
          kPln[i][0] = 5;
          kZ_loc[i][0] = -1.5;
          kPln[i][1] = 0;
          kZ_loc[i][1] = -0.5;
          kPln[i][2] = 1;
          kZ_loc[i][2] = 0.5;
          kPln[i][3] = 2;
          kZ_loc[i][3] = 1.5;
          kPln[i][4] = 3;
          kZ_loc[i][4] = 2.5;
          kPln[i][5] = 4;
          kZ_loc[i][5] = -2.5;
        }
        else if(i == 1) {
          kPln[i][0] = 1;
          kZ_loc[1][0] = -1.5;
          kPln[i][1] = 0;
          kZ_loc[i][1] = -2.5;
          kPln[i][2] = 5;
          kZ_loc[i][2] = 2.5;
          kPln[i][3] = 4;
          kZ_loc[i][3] = 1.5;
          kPln[i][4] = 3;
          kZ_loc[i][4] = 0.5;
          kPln[i][5] = 2;
          kZ_loc[i][5] = -0.5;
        }
        else if ( i == 2 || i == 3) {
          kPln[i][0] = 4;
          kPln[i][1] = 5;
          kPln[i][2] = 0;
          kPln[i][3] = 1;
          kPln[i][4] = 2;
          kPln[i][5] = 3;

          kZ_loc[i][iPla] = -0.5 + iPla;
          if(iPla == 4) {
            kZ_loc[i][iPla] = -2.5;
          }
          if(iPla == 5) {
            kZ_loc[i][iPla] = -1.5;
          }

        }// i 2 3
      }//7 run

    }//iPla

    for(int ii = 0; ii < 4; ++ii) { // 4 parameters: tan(x), tan(y), x ,y
      par_ab_Ch[i][ii] = new Double_t[kBig];
      par_ab_seg[i][ii] = new Double_t[kBig];
      matrA[ii]    = new Double_t[4];
      matrb[ii]    = new Double_t[4];
      Amatr[ii] = new Double_t[4];
      bmatr[ii] = new Double_t[4];
    }//4
  }//kChamber
  for(Int_t i = 0; i < kNChambers; ++i) {
    for(Int_t j = 0; j < kBig; ++j) {
      sigma2_seg[i][j]    = new Double_t*[4];
    }
  }
  for(Int_t i = 0; i < kNChambers; ++i) {
    for(Int_t j = 0; j < kBig; ++j) {
      for(Int_t k = 0; k < 4; ++k) {
        sigma2_seg[i][j][k]  = new Double_t[4];
      }
    }
  }

  if (fRunPeriod == 6) {
    // kPln[1][0] = 1; kZ_loc[1][0] =-2.5;//  kPln[3][0] = 1;//run6-II
    // kPln[1][3] = 4; kZ_loc[1][3] = 0.5;//  kPln[3][3] = 4;//
  }

  // if (fDebug) printf("Chamber Plane kZ_loc  z_gl\n");
  for(Int_t i = 0; i < kNChambers; ++i) {
    for(int ii = 0; ii < kNPlanes; ii++) {
      z_gl[i][ii] = Zmid[i] + kZ_loc[i][ii];
      // if (fDebug) printf("%5d %5d %8.4f %8.4f\n", i, ii, kZ_loc[i][ii], z_gl[i][ii]);
    }
  }

  fBmnEvQuality = (TClonesArray*) ioman->GetObject(fBmnEvQualityBranchName);

  return kSUCCESS;
}


void BmnMwpcHitFinder::Exec(Option_t* opt) {
  if (!IsActive()) return;
  clock_t tStart = clock();
  PrepareArraysToProcessEvent();
  if (fDebug) cout << "\n======================== MWPC hit finder exec started =====================\n" << endl;
  if (fDebug) printf("Event number: %d\n", fEventNo++);

  // ----------   Digi-file reading-------------------------------------
  ReadWires(DigitsArray, iw_Ch, vec_points);

  //--------- Big chambers cycle -> looking for track-segments ---------
  for (Int_t iChamber = 0; iChamber < kNChambers; iChamber++) {
    Int_t counter = 0;

    for (Int_t iplane = 0; iplane < kNPlanes; iplane++) {
      counter += iw_Ch[iChamber][iplane];
    }//iplane
    if (counter < kMinHits || counter > kChMaxAllWires ) continue; //too many or too few wires per Chamber
    
    if (fDebug) cout<<"-- Clustering --"<<endl;
    Clustering(iChamber, ClusterSize, DigitsArray, Coord_wire, Coord_xuv, Nclust, counter_pl); // Cluster production function

    if (fDebug) cout<<"-- SegmentFinder --"<<endl;
    for(Int_t iCase= 1; iCase < 9; iCase ++) {
      SegmentFinder(iChamber, Nclust, Coord_xuv, ClusterSize, Nseg_Ch, XVU_coord, Cluster_coord, Nhits_Ch, kMinHits, iCase, kBig); // Combinatorial segment selection
    }
    if (fDebug) cout<<"-- after SegmentFinder: Nseg_["<<iChamber<<"]= "<<Nseg_Ch[iChamber]<<endl;

    ProcessSegments(iChamber, Nseg_Ch, XVU_coord, Cluster_coord, Nhits_Ch, kZ_loc, kMinHits, sigma,
      kChi2_Max,Nhits_seg ,Chi2_ndf_seg, Coor_seg, Cluster_seg, par_ab_seg, Nbest_seg, Nlay_w_wires,sigma2_seg); // Segment fitting & finding the best track-candidate

    if (fDebug) cout<<"--after ProcessSegments: Nbest["<<iChamber<<"] "<<Nbest_seg[iChamber]<<endl;
    if (fDebug && Nbest_seg[iChamber] > 0) hNbest_Ch.at(iChamber) -> Fill(Nbest_seg[iChamber]);

    if (fDebug) {
      for(Int_t iseg= 0; iseg < Nbest_seg[iChamber]; iseg ++) {
        cout<<" iseg "<<iseg<<" Nhits_Ch "<<Nhits_seg[iChamber][iseg]<<" Chi2 "<<Chi2_ndf_seg[iChamber][iseg]<<" Ax "<< par_ab_seg[iChamber][0][iseg]<<" bx "<< par_ab_seg[iChamber][1][iseg]<<" by "<< par_ab_seg[iChamber][3][iseg]<<endl;
        hChisq_ndf_Ch.at(iChamber) ->Fill(Chi2_ndf_seg[iChamber][iseg]);
        if ( Nhits_seg[iChamber][iseg] > 0 ) hNp_best_Ch.at(iChamber)->Fill(Nhits_seg[iChamber][iseg]);
      }
    }

    SegmentParamAlignment(iChamber, Nbest_seg, par_ab_seg, shift); // Alignment
  }//iChamber
  
  //--efficiency calculation for MC--
  if(!expData) MCefficiencyCalculation(vec_points,par_ab_seg,Nbest_seg);
  
  SegmentsStoring(Nbest_seg, par_ab_seg,Chi2_ndf_seg, Nhits_seg, Coor_seg, Cluster_seg, sigma2_seg);

  clock_t tFinish = clock();
  workTime += ((Float_t) (tFinish - tStart)) / CLOCKS_PER_SEC;
  if (fDebug) cout << "\n======================== MWPC hit finder exec finished ====================" << endl;
}



void BmnMwpcHitFinder::SegmentsStoring(Int_t *Nbest, Double_t ***par_ab,Double_t **Chi2_ndf, Int_t **Nhits, Double_t ***Coor, Double_t ***Cluster, Double_t ****sigma2){
  if (fDebug) cout<<"--SegmentsStoring--"<<endl;
  vector<Double_t>vtmpCoor;
  vector<Double_t>vtmpClust;
  for (Int_t iChamber = 0; iChamber < kNChambers; iChamber++) {
    for (Int_t ise = 0; ise < Nbest[iChamber]; ise++) {
      if (Nhits[iChamber][ise] > 3) {
        
        BmnMwpcSegment *pSeg = new ((*fBmnMwpcSegmentsArray)[fBmnMwpcSegmentsArray->GetEntriesFast()]) BmnMwpcSegment();
        pSeg->SetChi2(Chi2_ndf[iChamber][ise]);
        pSeg->SetNHits(Nhits[iChamber][ise]);
        pSeg->SetFlag(ise);

        vtmpCoor.clear();
        vtmpClust.clear();

        for(Int_t i1 = 0 ; i1 < 6; i1++) {
          vtmpCoor.push_back(Coor[iChamber][i1][ise]);
          vtmpClust.push_back(Cluster[iChamber][i1][ise]);
        }
        pSeg -> SetClust(vtmpClust);
        pSeg -> SetCoord(vtmpCoor);
        
        FairTrackParam pSegParams;
        pSegParams.SetPosition(TVector3(par_ab[iChamber][1][ise], par_ab[iChamber][3][ise],ChZ[iChamber]));
        pSegParams.SetTx(par_ab[iChamber][0][ise]);
        pSegParams.SetTy(par_ab[iChamber][2][ise]);
        for(Int_t i1 = 0 ; i1 < 4; i1++) {
          for(Int_t j1 = 0; j1 < 4; j1++) {
            //cout<<" sigma2_seg["<<iChamber<<"]["<<ise<<"]["<<i1<<"]["<<j1<<"] "<<sigma2_seg[iChamber][ise][i1][j1]<<endl;
            pSegParams.SetCovariance(i1, j1, sigma2[iChamber][ise][i1][j1]);
          }
        } 
        pSeg->SetParamFirst(pSegParams);
      }//if
    }//ise
  }//[iChamber]
  //--------------------------------------------------------------------
}



void BmnMwpcHitFinder::MCefficiencyCalculation(vector<MC_points>& vec, Double_t ***par_ab_seg_, Int_t *Nbest_seg_){
  if(!expData){
    
    if (fDebug) cout<<" ---MC tracks association--"<<endl;
    //                           ax, bx,    ay, by
    Double_t delta2[4]       = {-99.,-999.,-99.,-999.}; 
    Double_t delta3[4]       = {-99.,-999.,-99.,-999.}; 
    
    //                   ax,   bx,   ay,   by
    Double_t sig[4] = {0.04, 0.08, 0.05, 0.08};
    
    Double_t dmatch = 0.;
    Double_t dmc_match[kNChambers][kMaxMC];
    Int_t    mc_tr_assoc[kNChambers][kMaxMC];
    
    for (Int_t i = 0; i < kNChambers; i++) {
      for (Int_t j = 0; j < kMaxMC; j++) {
        dmc_match[i][j]   = 1000.;
        mc_tr_assoc[i][j] = -1;
      }
    }
    Double_t dax = -999.;
    Double_t day = -999.;
    Double_t dx  = -999.;
    Double_t dy  = -999.;
    
    for (Int_t itr = 0; itr < vec.size(); itr++) {//mc_tr
      
      if (fDebug) cout<<" Np2 "<<vec.at(itr).Np2<<" Np3 "<<vec.at(itr).Np3<<endl;
      
      if (fDebug) hNp_MCtrue_ch2 -> Fill(vec.at(itr).Np2);
      if (fDebug) hNp_MCtrue_ch3 -> Fill(vec.at(itr).Np3);
        //---MC Eff ---
        //---Den
        if (vec.at(itr).Np2 >= 4 && vec.at(itr).xWas2 && vec.at(itr).uWas2 && vec.at(itr).vWas2){
         if (fDebug) hDen_mc->Fill(0);
         if (fDebug) cout<<" Den_mcPC2 "<<endl;
        }
       // if (fDebug) cout<<" xWas3 "<<vec.at(itr).xWas3<<" uWas3 "<<vec.at(itr).uWas3<<" vWas3 "<<vec.at(itr).vWas3<<endl;
        if (vec.at(itr).Np3 >= 4 && vec.at(itr).xWas3 && vec.at(itr).uWas3 && vec.at(itr).vWas3){
         if (fDebug) hDen_mc->Fill(1);
         if (fDebug) cout<<" Den_mcPC3 "<<endl;
        }
      for (Int_t iChamber = 2; iChamber < kNChambers; iChamber++) {

        if (iChamber == 2){
          for(Int_t iseg= 0; iseg < Nbest_seg_[iChamber]; iseg ++) {
            delta2[0] = vec.at(itr).param2[0] - par_ab_seg_[iChamber][0][iseg];
            delta2[1] = vec.at(itr).param2[1] - par_ab_seg_[iChamber][1][iseg];
            delta2[2] = vec.at(itr).param2[2] - par_ab_seg_[iChamber][2][iseg];
            delta2[3] = vec.at(itr).param2[3] - par_ab_seg_[iChamber][3][iseg];
           
            if (fDebug){
              //combinatorics
              if (delta2[0] > -90.)  hdAx_mc_SegCh.at(iChamber)->Fill(delta2[0]); 
              if (delta2[1] > -900.) hdX_mc_SegCh.at(iChamber) ->Fill(delta2[1]);
              if (delta2[2] > -90.)  hdAy_mc_SegCh.at(iChamber)->Fill(delta2[2]);
              if (delta2[3] > -900.) hdY_mc_SegCh.at(iChamber) ->Fill(delta2[3]);
            }
          
            dmatch = 0.;
            dmatch = (delta2[0]*delta2[0])/(sig[0]*sig[0])+ 
                        (delta2[1]*delta2[1])/(sig[1]*sig[1])+
                        (delta2[2]*delta2[2])/(sig[2]*sig[2])+
                        (delta2[3]*delta2[3])/(sig[3]*sig[3]);
                         
            if ( dmc_match[iChamber][itr] > dmatch){
                dmc_match[iChamber][itr]   = dmatch;
                mc_tr_assoc[iChamber][itr] = iseg;
                dax = delta2[0];
                dx  = delta2[1];
                day = delta2[2];
                dy  = delta2[3];
            }
          }
          if (fDebug){cout<<" itr "<<itr<<" Np2 "<<vec.at(itr).Np2<<" mc_Id "<<vec.at(itr).Id<<
           " bx_mc "<<vec.at(itr).param2[1]<<" iseg_ind "<<mc_tr_assoc[iChamber][itr]<<" bx "<< par_ab_seg_[iChamber][1][mc_tr_assoc[iChamber][itr]] 
           <<" dmc_match "<<dmc_match[iChamber][itr]<<endl;
            if (mc_tr_assoc[iChamber][itr] > -1){
              if (dax > -900.) hdAx_mc_Seg_deltaCh.at(iChamber)->Fill(dax);
              if (dx > -900.)  hdX_mc_Seg_deltaCh.at(iChamber) ->Fill(dx);
              if (day > -900.) hdAy_mc_Seg_deltaCh.at(iChamber)->Fill(day);
              if (dy > -900.)  hdY_mc_Seg_deltaCh.at(iChamber) ->Fill(dy);
            }
          }
        }//iChamber == 2
        
        if (iChamber == 3){
          for(Int_t iseg= 0; iseg < Nbest_seg_[iChamber]; iseg ++) {
            delta3[0] = vec.at(itr).param3[0] - par_ab_seg_[iChamber][0][iseg];
            delta3[1] = vec.at(itr).param3[1] - par_ab_seg_[iChamber][1][iseg];
            delta3[2] = vec.at(itr).param3[2] - par_ab_seg_[iChamber][2][iseg];
            delta3[3] = vec.at(itr).param3[3] - par_ab_seg_[iChamber][3][iseg];
           
            if (fDebug){
              //combinatorics
              if (delta3[0] > -90.)  hdAx_mc_SegCh.at(iChamber)->Fill(delta3[0]); 
              if (delta3[1] > -900.) hdX_mc_SegCh.at(iChamber) ->Fill(delta3[1]);
              if (delta3[2] > -90.)  hdAy_mc_SegCh.at(iChamber)->Fill(delta3[2]);
              if (delta3[3] > -900.) hdY_mc_SegCh.at(iChamber) ->Fill(delta3[3]);
            }
          
          dmatch = 0.;
          dmatch = (delta3[0]*delta3[0])/(sig[0]*sig[0])+ 
                      (delta3[1]*delta3[1])/(sig[1]*sig[1])+
                      (delta3[2]*delta3[2])/(sig[2]*sig[2])+
                      (delta3[3]*delta3[3])/(sig[3]*sig[3]);
                       
          if ( dmc_match[iChamber][itr] > dmatch){
              dmc_match[iChamber][itr]   = dmatch;
              mc_tr_assoc[iChamber][itr] = iseg;
              dax = delta3[0];
              dx  = delta3[1];
              day = delta3[2];
              dy  = delta3[3];
          }
        }
        
        if (fDebug){ cout<<" itr "<<itr<<" Np3 "<<vec.at(itr).Np3<<" mc_Id "<<vec.at(itr).Id<<
         " bx_mc "<<vec.at(itr).param3[1]<<" iseg_ind "<<mc_tr_assoc[iChamber][itr]<<" bx "<< par_ab_seg_[iChamber][1][mc_tr_assoc[iChamber][itr]]<<
         " dmc_match "<<dmc_match[iChamber][itr]<<endl;
        
          if (mc_tr_assoc[iChamber][itr] > -1){
            
            if (dax > -900.) hdAx_mc_Seg_deltaCh.at(iChamber)->Fill(dax);
            if (dx > -900.)  hdX_mc_Seg_deltaCh.at(iChamber) ->Fill(dx);
            if (day > -900.) hdAy_mc_Seg_deltaCh.at(iChamber)->Fill(day);
            if (dy > -900.)  hdY_mc_Seg_deltaCh.at(iChamber) ->Fill(dy);
            
          }
        }
        }//iChamber == 3
      }//iChamber
        
    }//vec_points.size
    
    if (fDebug) cout<<"reject poorly chosen association segments "<<endl;
    
    for (Int_t iChamber = 2; iChamber < kNChambers; iChamber++) {
      for (Int_t itr = 0; itr < vec.size(); itr++) {//mc_tr
        if (mc_tr_assoc[iChamber][itr] == -1) continue;
         
        for (Int_t itr2 = 0; itr2 < vec.size(); itr2++) {//mc_tr
          if (itr2 == itr) continue;
          if (mc_tr_assoc[iChamber][itr2] == -1) continue;
          
          if (mc_tr_assoc[iChamber][itr] ==  mc_tr_assoc[iChamber][itr2]){
            if (dmc_match[iChamber][itr2] > dmc_match[iChamber][itr] ) mc_tr_assoc[iChamber][itr2] = -1;
            else {
              mc_tr_assoc[iChamber][itr] = -1;
              break;
            }
          }
        }//itr2
        //---MC Eff ---
        //---Num
        if (fDebug) cout<<" mc_Id "<<vec.at(itr).Id<<" assoc "<<mc_tr_assoc[iChamber][itr]<<" iChamber "<<iChamber<<endl;
        if (fDebug && mc_tr_assoc[iChamber][itr] > -1){
          if(fDebug && iChamber == 2 && vec.at(itr).Np2 >= 4 && 
            vec.at(itr).xWas2 && vec.at(itr).uWas2 && vec.at(itr).vWas2 ){
            hNum_mc->Fill(0);
            if (fDebug) cout<<" Num_mcPC2 "<<endl;
          }
          if(fDebug && iChamber == 3 && vec.at(itr).Np3 >= 4 &&  
            vec.at(itr).xWas3 && vec.at(itr).uWas3 && vec.at(itr).vWas3 ){
            hNum_mc->Fill(1);
            if (fDebug) cout<<" Num_mcPC3 "<<endl;
          }
        }
      }//itr
    }//iChamber
    
    
  }//if(!expData)
}



void BmnMwpcHitFinder::ReadWires(Double_t ***DigitsArray_, Int_t **iw_Ch_, vector<MC_points> & vec){
  if (fDebug) cout<<"--ReadWires--"<<endl;
  
  Int_t  Fired_layer_[kNChambers][kNPlanes];
  int Npl_MC2[kMaxMC]; int Npl_MC3[kMaxMC];
  Short_t st, wire, pn, pl;
  UInt_t ts;
  
  for (Int_t iChamber = 0; iChamber < kNChambers; iChamber++) {
    for (Int_t ipll = 0; ipll < kNPlanes; ipll++) {
      Fired_layer_[iChamber][ipll] =0;
    }
  }
  
  if(!expData){
    
    Int_t mcTracksArray[kMaxMC];
    Double_t X2mc[kMaxMC][kNPlanes];
    Double_t Y2mc[kMaxMC][kNPlanes];
    Double_t Z2mc[kMaxMC][kNPlanes];
    Double_t X3mc[kMaxMC][kNPlanes];
    Double_t Y3mc[kMaxMC][kNPlanes];
    Double_t Z3mc[kMaxMC][kNPlanes];
    for (Int_t  Id= 0; Id < kMaxMC; Id++) { 
      Npl_MC2[Id] = 0;
      Npl_MC3[Id] = 0;
      mcTracksArray[Id] = -1;
       for (Int_t  i = 0; i < kNPlanes; i++) { 
          X2mc[Id][i] = -999.;
          Y2mc[Id][i] = -999.;
          Z2mc[Id][i] = -999.;
          X3mc[Id][i] = -999.;
          Y3mc[Id][i] = -999.;
          Z3mc[Id][i] = -999.;
        }
    }

    int tr_before  = -1;
    int Nmc_tracks = -1;
    
    for (Int_t iMC = 0; iMC < fBmnHitsArray->GetEntriesFast(); ++iMC) {
      BmnMwpcHit* hit = (BmnMwpcHit*)fBmnHitsArray->UncheckedAt(iMC);
      
      Int_t    st_MC      = hit->GetMwpcId();
      Int_t    trackId_MC = hit->GetHitId();
      Int_t    pl_MC      = hit->GetPlaneId();
      Short_t  wire_MC    = hit->GetWireNumber();
      Double_t time_MC    = hit->GetWireTime();
      
      if (fDebug)cout<<" st_MC "<<st_MC<<" trackId_MC "<<trackId_MC<<" pl_MC "<<pl_MC<<" X "<<hit->GetX()<<" wire_MC "<<wire_MC<<endl;
      
      if (tr_before != trackId_MC) {
        Nmc_tracks++;
        mcTracksArray[Nmc_tracks] = hit->GetHitId();
      }
      tr_before = trackId_MC;
      
      if (st_MC == 2){
        X2mc[Nmc_tracks][pl_MC] = hit->GetX();
        Y2mc[Nmc_tracks][pl_MC] = hit->GetY();
        Z2mc[Nmc_tracks][pl_MC] = hit->GetZ();
        Npl_MC2[Nmc_tracks]++;       
      }
      if (st_MC == 3){
        X3mc[Nmc_tracks][pl_MC] = hit->GetX();
        Y3mc[Nmc_tracks][pl_MC] = hit->GetY();
        Z3mc[Nmc_tracks][pl_MC] = hit->GetZ();
        Npl_MC3[Nmc_tracks]++;
      }
      //if (fDebug)cout<<" X2["<<Nmc_tracks<<"]["<<pl_MC<<"] "<<X2mc[Nmc_tracks][pl_MC]<<" Npl_MC2 "<< Npl_MC2[Nmc_tracks]<<" X3["<<Nmc_tracks<<"]["<<pl_MC<<"] "<<X3mc[Nmc_tracks][pl_MC]<<" Npl_MC3 "<< Npl_MC3[Nmc_tracks]<<endl;
      
      pn = pl_MC;
      DigitsArray_[st_MC][pn][wire_MC] = time_MC;
      Fired_layer_[st_MC][pn] = 1;
      iw_Ch_[st_MC][pn]++;
    }//iMC
    Nmc_tracks++;
    
    MC_points tmpTr;
    if (fDebug)cout<<" Nmc_tracks "<<Nmc_tracks<<endl;
    for (Int_t  id = 0; id < Nmc_tracks; id++) { 
       if (fDebug)cout<<" id "<<id<<" Id_mc "<< mcTracksArray[id]<<" Npl2 "<<Npl_MC2[id]<<" Npl3 "<<Npl_MC3[id]<<endl;
        for (Int_t i= 0; i < 6; i++) { 
          if (fDebug && X2mc[id][i] > -900.)cout<<" ipl "<<i<<" X2 "<<X2mc[id][i]<<endl;
          tmpTr.x2[i]  = X2mc[id][i];
          tmpTr.y2[i]  = Y2mc[id][i];
          tmpTr.z2[i]  = Z2mc[id][i];
        }
        for (Int_t i= 0; i < 6; i++) { 
          if (fDebug && X3mc[id][i] > -900.)cout<<" ipl "<<i<<" X3 "<<X3mc[id][i]<<endl;
          tmpTr.x3[i]  = X3mc[id][i];
          tmpTr.y3[i]  = Y3mc[id][i];
          tmpTr.z3[i]  = Z3mc[id][i];
        }
        if (fDebug)cout<<endl;
        tmpTr.Id  = mcTracksArray[id];
        tmpTr.Np2 = Npl_MC2[id];
        tmpTr.Np3 = Npl_MC3[id];
        
        if (Npl_MC2[id] >= 4 || Npl_MC3[id] >= 4 ) vec.push_back(tmpTr);
    }//Nmc_tracks
    
    if (fDebug)cout<<" MC vec_points.size() "<<vec.size()<<endl;
    if (fDebug) hNtrMC->Fill(vec.size());
    
    Double_t x_target_ch2, y_target_ch2, x_target_ch3, y_target_ch3;
    
    for (Int_t itr = 0; itr < vec.size(); itr++) {
      
      if (vec.at(itr).x2[0] > -900. || vec.at(itr).x2[3] > -900.) vec.at(itr).xWas2 = 1;
      if (vec.at(itr).x2[1] > -900. || vec.at(itr).x2[4] > -900.) vec.at(itr).vWas2 = 1;
      if (vec.at(itr).x2[2] > -900. || vec.at(itr).x2[5] > -900.) vec.at(itr).uWas2 = 1;
      
      if (vec.at(itr).x3[0] > -900. || vec.at(itr).x3[3] > -900.) vec.at(itr).xWas3 = 1;
      if (vec.at(itr).x3[1] > -900. || vec.at(itr).x3[4] > -900.) vec.at(itr).vWas3 = 1;
      if (vec.at(itr).x3[2] > -900. || vec.at(itr).x3[5] > -900.) vec.at(itr).uWas3 = 1;
      
      int i2 = 5;
      if (vec.at(itr).x2[i2] < -900.) i2 =4;
      if (vec.at(itr).x2[i2] < -900.) i2 =3;
      int i20 = 0;
      if (vec.at(itr).x2[i20] < -900.) i20 =1;
      if (vec.at(itr).x2[i20] < -900.) i20 =2;
      
      vec.at(itr).param2[0]  = (vec.at(itr).x2[i20] - vec.at(itr).x2[i2])/ (vec.at(itr).z2[i20] - vec.at(itr).z2[i2]); 
      vec.at(itr).param2[1]  = (vec.at(itr).x2[i20] + vec.at(itr).x2[i2])*0.5;
      vec.at(itr).param2[2]  = (vec.at(itr).y2[i20] - vec.at(itr).y2[i2])/ (vec.at(itr).z2[i20] - vec.at(itr).z2[i2]); 
      vec.at(itr).param2[3]  = (vec.at(itr).y2[i20] + vec.at(itr).y2[i2])*0.5;
      x_target_ch2  = vec.at(itr).param2[0]*( Z0_SRC - ChZ[2]) + vec.at(itr).param2[1] ;
      y_target_ch2  = vec.at(itr).param2[2]*( Z0_SRC - ChZ[2]) + vec.at(itr).param2[3];
      
      int i3 = 5;
      if (vec.at(itr).x3[i3] < -900.) i3 =4;
      if (vec.at(itr).x3[i3] < -900.) i3 =3;
      int i30 = 0;
      if (vec.at(itr).x3[i30] < -900.) i30 =1;
      if (vec.at(itr).x3[i30] < -900.) i30 =2;
      
      vec.at(itr).param3[0]  = (vec.at(itr).x3[i30] - vec.at(itr).x3[i3])/ (vec.at(itr).z3[i30] - vec.at(itr).z3[i3]); 
      vec.at(itr).param3[1]  = (vec.at(itr).x3[i30] + vec.at(itr).x3[i3])*0.5;
      vec.at(itr).param3[2]  = (vec.at(itr).y3[i30] - vec.at(itr).y3[i3])/ (vec.at(itr).z3[i30] - vec.at(itr).z3[i3]); 
      vec.at(itr).param3[3]  = (vec.at(itr).y3[i30] + vec.at(itr).y3[i3])*0.5;
      x_target_ch3  = vec.at(itr).param3[0]*( Z0_SRC - ChZ[3]) + vec.at(itr).param3[1] ;
      y_target_ch3  = vec.at(itr).param3[2]*( Z0_SRC - ChZ[3]) + vec.at(itr).param3[3];
      
      vec.at(itr).xt = (x_target_ch2 + x_target_ch3)/2;
      vec.at(itr).yt = (y_target_ch2 + y_target_ch3)/2;
      
      if (fDebug) cout<<" itr "<<itr<<" Id_mc "<<vec.at(itr).Id<<" 2:Ax "<<vec.at(itr).param2[0]<<" bx "<<vec.at(itr).param2[1]<<" Ay "<<vec.at(itr).param2[2]<<" by "<<vec.at(itr).param2[3]<<" x_target "<<x_target_ch2<<" y_target "<<y_target_ch2<<" Np2 "<<vec.at(itr).Np2<<endl;
      if (fDebug) cout<<" itr "<<itr<<" Id_mc "<<vec.at(itr).Id<<" 3:Ax "<<vec.at(itr).param3[0]<<" bx "<<vec.at(itr).param3[1]<<" Ay "<<vec.at(itr).param3[2]<<" by "<<vec.at(itr).param3[3]<<" x_target "<<x_target_ch3<<" y_target "<<y_target_ch3<<" Np3 "<<vec.at(itr).Np3<<endl;
      if (fDebug) cout<<" xt "<<vec.at(itr).xt<<" yt "<<vec.at(itr).yt<<endl;
      
      if (fDebug) {
        hAx_mc_ch.at(2)->Fill(vec.at(itr).param2[0]);
        hBx_mc_ch.at(2)->Fill(vec.at(itr).param2[1]);
        hAy_mc_ch.at(2)->Fill(vec.at(itr).param2[2]);
        hBy_mc_ch.at(2)->Fill(vec.at(itr).param2[3]);
        
        hAx_mc_ch.at(3)->Fill(vec.at(itr).param3[0]);
        hBx_mc_ch.at(3)->Fill(vec.at(itr).param3[1]);
        hAy_mc_ch.at(3)->Fill(vec.at(itr).param3[2]);
        hBy_mc_ch.at(3)->Fill(vec.at(itr).param3[3]);
        
        hYvsX_mc_ch2->Fill(vec.at(itr).param2[1],vec.at(itr).param2[3]);
        hYvsX_mc_ch3->Fill(vec.at(itr).param3[1],vec.at(itr).param3[3]);
        
      }
      
    }//vec_points
    if (fDebug) cout<<endl;
    
  }else{
    for (Int_t iDigit = 0; iDigit < fBmnMwpcDigitArray -> GetEntries(); iDigit++) {
      BmnMwpcDigit* digit = (BmnMwpcDigit*) fBmnMwpcDigitArray ->At (iDigit);

      st   = digit -> GetStation();
      wire = digit -> GetWireNumber();
      pl   = digit -> GetPlane();
      ts   = digit -> GetTime(); //ns

      if (fRunPeriod == 7 && fRunNumber > 3588) {
        if(st>1) st = st - 2;
      }

      Fired_layer_[st][pl] = 1;
      Int_t ind = st*kNPlanes + pl;
      if (fDebug) hTime.at(ind) -> Fill(ts);
      if (fDebug) hOccupancy.at(ind) -> Fill(wire);
      if ( ts < 80 || ts > 280 ) continue;

      pn = kPln[st][pl];// made for the canonical sequence / x- v- u+ x+ v+ u-/

      // Loop over repeated wires
      Bool_t repeat_wire = 0;
      if (iw_Ch_[st][pn] > 0) {
        for (Int_t ix = 0; ix < iw_Ch[st][pn]; ix++) {
          if (wire == wire_Ch[st][ix][pn] ) {
            repeat_wire = 1;
            break;
          }
        }//ix
      }
      wire_Ch[st][iw_Ch_[st][pn]][pn] = wire;
      
      if (repeat_wire) continue;
      if (iw_Ch_[st][pn] >= 80) continue;

      DigitsArray_[st][pn][wire] = ts;
      iw_Ch_[st][pn]++;
    }// iDigit
  }//else
  counter_pl[kNPlanes];
  for (Int_t iChamber = 0; iChamber < kNChambers; iChamber++) {
    for (Int_t iplane = 0; iplane < kNPlanes; iplane++) {
      Nlay_w_wires[iChamber] += Fired_layer_[iChamber][iplane];
      counter_pl[iplane] = 0;
      counter_pl[iplane] += iw_Ch_[iChamber][iplane];
      if (fDebug) hfiredWire_Ch.at(iChamber)->Fill(counter_pl[iplane]);
    }//iplane
    if (fDebug) hNFired_layers_Ch[iChamber]->Fill(Nlay_w_wires[iChamber]);
  }
  
}
//--------------------------------------------------------------------


//------------------ Cluster production function------------------------
void BmnMwpcHitFinder::Clustering(Int_t chNum, Int_t*** ClusterSize_, Double_t*** DigitsArray_, Double_t*** Coord_wire_, 
Double_t*** Coord_xuv_, Int_t **Nclust_, Int_t *counter_pl_) {
 
  Int_t Nfirst[kCh_max][kNPlanes], Nlast[kCh_max][kNPlanes];
  Int_t Min_time_wires, fast_wire ;
  Int_t N_wires[kCh_max][kNPlanes];
  Int_t N_1_cluster[kCh_max][kNPlanes];
  Double_t earlyWires[kNChambers][kNPlanes][kNWires];
  Double_t Coord_fast[kNChambers][kNPlanes][kBig];
  Double_t Cut_time_wire = 16.;//ns
  Double_t Num_layers_out_beam = 0;
  
  
  for (Int_t ipll = 0; ipll < kNPlanes; ipll++) {
    //Fired_layer[chNum][ipll] =0;
    for (Int_t iwire = 0; iwire < kNWires; iwire++) {
      //if ( DigitsArray_[chNum][ipll][iwire] > 0 ) Fired_layer[chNum][ipll] = 1;
    }
  }
  

  for (Int_t ipll = 0; ipll < kNPlanes; ipll++) {
    N_wires[chNum][ipll] = 0;
    N_1_cluster[chNum][ipll] = 0;
    for (Int_t ib = 0; ib < kBig; ib++) {
      Coord_fast[chNum][ipll][ib] = 0.;
    }
  }
  /*
  if ( chNum < 2 ) {
    for (Int_t ipll = 0; ipll < kNPlanes; ipll++) {
      Nfirst[chNum][ipll] = -1;
      Nlast[chNum][ipll] = -1;
      Min_time_wires = 999.;
      fast_wire = -1;

      for (Int_t iwire = 0; iwire < kNWires; iwire++) {
        earlyWires[chNum][ipll][iwire] = 0.;
        if ( DigitsArray_[chNum][ipll][iwire] > 0 && DigitsArray_[chNum][ipll][iwire] < Min_time_wires ) {
          Min_time_wires = DigitsArray_[chNum][ipll][iwire];
        }
      }
      if (fDebug) cout<<" --ipll "<<ipll<<" Min_time_wires "<<Min_time_wires<<endl;

      for (Int_t iwire = 0; iwire < kNWires; iwire++) {
        if (fDebug && DigitsArray_[chNum][ipll][iwire] > 0) cout<<" DigitsArray_["<<chNum<<"]["<<ipll<<"]["<<iwire<<"] "<<DigitsArray_[chNum][ipll][iwire]<<endl;

        if ( fDebug && DigitsArray_[chNum][ipll][iwire] > 0 ){
          htime_wire_Ch.at(chNum)->Fill(iwire + kNWires*ipll, DigitsArray_[chNum][ipll][iwire]);
          hfired_wire_Ch.at(chNum)->Fill(iwire + kNWires*ipll);
          
          if(ipll == 0) hWiresXm.at(chNum)->Fill(iwire);
          if(ipll == 1) hWiresVm.at(chNum)->Fill(iwire);
          if(ipll == 2) hWiresUp.at(chNum)->Fill(iwire);
          if(ipll == 3) hWiresXp.at(chNum)->Fill(iwire);
          if(ipll == 4) hWiresVp.at(chNum)->Fill(iwire);
          if(ipll == 5) hWiresUm.at(chNum)->Fill(iwire);
          
          if ( ipll == 0 && fEventNo < 700) hEvent_display_Ch.at(chNum)->Fill(fEventNo,iwire);
        }

        if( Nfirst[chNum][ipll] < 0 && DigitsArray_[chNum][ipll][iwire] == 0.)  continue;
        if( Nfirst[chNum][ipll] < 0 && DigitsArray_[chNum][ipll][iwire] > (Min_time_wires + Cut_time_wire) ) continue;

        if( Nfirst[chNum][ipll] < 0 && DigitsArray_[chNum][ipll][iwire] > 0.
            && DigitsArray_[chNum][ipll][iwire] <= (Min_time_wires + Cut_time_wire)) Nfirst[chNum][ipll] = iwire;

        if( Nfirst[chNum][ipll] >=0 && (DigitsArray_[chNum][ipll][iwire+1] == 0.
                         || DigitsArray_[chNum][ipll][iwire+ 1] > (Min_time_wires + Cut_time_wire) ) ) Nlast[chNum][ipll] = iwire;

        if (Nfirst[chNum][ipll] >= 0 && Nlast[chNum][ipll] >= Nfirst[chNum][ipll]) {
          Int_t min_time_in_clust = 999, num_fast_wire = -1;

          for (Int_t iww = Nfirst[chNum][ipll] ; iww < Nlast[chNum][ipll] + 1; iww++) {

            if ( DigitsArray_[chNum][ipll][iww] < min_time_in_clust ) {
              min_time_in_clust = DigitsArray_[chNum][ipll][iww] ;
              num_fast_wire = iww;
            }
          }

          ClusterSize_[chNum][ipll][Nclust_[chNum][ipll]] = Nlast[chNum][ipll] - Nfirst[chNum][ipll] + 1;

         
          if (fDebug) hClusterSize.at(chNum)->Fill(ClusterSize_[chNum][ipll][Nclust_[chNum][ipll]]);

          if ( min_time_in_clust < 900) {
            Coord_wire_[chNum][ipll][Nclust_[chNum][ipll] ] = Nfirst[chNum][ipll] + 0.5*ClusterSize_[chNum][ipll][Nclust_[chNum][ipll]];
            Coord_xuv_[chNum][ipll][Nclust_[chNum][ipll]] = (Coord_wire_[chNum][ipll][Nclust_[chNum][ipll]] - kMiddlePl)* dw;//cm

            // made for the canonical sequence / x- v- u+ x+ v+ u-/ 0 1 5 have back reading
            if (ipll == 0 || ipll == 1 || ipll == 5 ) Coord_xuv_[chNum][ipll][Nclust_[chNum][ipll]] = - Coord_xuv_[chNum][ipll][Nclust_[chNum][ipll]];
          }
           if ( fDebug)  cout<<" Nfirst "<<Nfirst[chNum][ipll]<<" Nlast "<<Nlast[chNum][ipll]<<" ClusterSize_["<<chNum<<"]["<<ipll<<"] "<<ClusterSize_[chNum][ipll][Nclust_[chNum][ipll]]<<" Coord_xuv "<<Coord_xuv_[chNum][ipll][Nclust_[chNum][ipll]]<<endl;

          if (fDebug) {
            hoccupancyXm.at(chNum)->Fill(Coord_xuv_[chNum][0][Nclust_[chNum][ipll]]);
            hoccupancyVm.at(chNum)->Fill(Coord_xuv_[chNum][1][Nclust_[chNum][ipll]]);
            hoccupancyUp.at(chNum)->Fill(Coord_xuv_[chNum][2][Nclust_[chNum][ipll]]);
            hoccupancyXp.at(chNum)->Fill(Coord_xuv_[chNum][3][Nclust_[chNum][ipll]]);
            hoccupancyVp.at(chNum)->Fill(Coord_xuv_[chNum][4][Nclust_[chNum][ipll]]);
            hoccupancyUm.at(chNum)->Fill(Coord_xuv_[chNum][5][Nclust_[chNum][ipll]]);
          }

          Nclust_[chNum][ipll]++;

          Nfirst[chNum][ipll] = -1;
          Nlast[chNum][ipll] = -1;

        } //if (Nfirst[chNum][ipll] >= 0
      }//iwire
    }// ipll
  } else {
  */
    // if (fDebug ) cout<<"chamber 2 or 3"<<endl;
    Num_layers_out_beam = 0;
    
    for (Int_t ipll = 0; ipll < kNPlanes; ipll++) {

      Nfirst[chNum][ipll] = -1;
      Nlast[chNum][ipll] = -1;
      Bool_t Next_next_wire = 0;
      Int_t Last_wire_Digit = 1000;

      for (Int_t iwire = 0; iwire < kNWires; iwire++) {
        
        if((ipll == 0 || ipll == 3) && (iwire < 33 || iwire > 63) ){
         // if ( DigitsArray_[chNum][ipll][iwire] > 0 ) Fired_layer[chNum][ipll] = 1;
        }
        
        if((ipll == 1 || ipll == 2) && (iwire < 12 || iwire > 42) ){
         // if ( DigitsArray_[chNum][ipll][iwire] > 0 ) Fired_layer[chNum][ipll] = 1;
        }
        if((ipll == 4 || ipll == 5) && (iwire < 50 || iwire > 80) ){
          //if ( DigitsArray_[chNum][ipll][iwire] > 0 ) Fired_layer[chNum][ipll] = 1;
        }
        
        if ( fDebug && DigitsArray_[chNum][ipll][iwire] > 0 ){
          htime_wire_Ch.at(chNum)->Fill(iwire + kNWires*ipll, DigitsArray_[chNum][ipll][iwire]);
          hfired_wire_Ch.at(chNum)->Fill(iwire + kNWires*ipll);
          
          if(ipll == 0) hWiresXm.at(chNum)->Fill(iwire);
          if(ipll == 1) hWiresVm.at(chNum)->Fill(iwire);
          if(ipll == 2) hWiresUp.at(chNum)->Fill(iwire);
          if(ipll == 3) hWiresXp.at(chNum)->Fill(iwire);
          if(ipll == 4) hWiresVp.at(chNum)->Fill(iwire);
          if(ipll == 5) hWiresUm.at(chNum)->Fill(iwire);
          
          if ( ipll == 0 && fEventNo < 700) hEvent_display_Ch.at(chNum)->Fill(fEventNo,iwire);
        }
        
        //if (fDebug ) cout<<" 1 first "<<Nfirst[chNum][ipll]<<" time "<<DigitsArray_[chNum][ipll][iwire]<<endl;
        if (fDebug && DigitsArray_[chNum][ipll][iwire] > 0 ) cout<<" DigitsArray_["<<chNum<<"]["<<ipll<<"]["<<iwire<<"] "<<DigitsArray_[chNum][ipll][iwire]<<" first "<<Nfirst[chNum][ipll]<<endl;

        if( Nfirst[chNum][ipll] < 0 && DigitsArray_[chNum][ipll][iwire] == 0) {
          Next_next_wire = 0;
          Last_wire_Digit = 1000;
          //if (fDebug) cout<<" -------------0? Next_next_wire "<<Next_next_wire<<" wire "<<iwire<<endl;
          continue;
        }
         //if (fDebug) cout<<" Next_next_wire "<<Next_next_wire<<endl;
        if( Nfirst[chNum][ipll] < 0 && DigitsArray_[chNum][ipll][iwire] > 0. && Next_next_wire) {

          if ( DigitsArray_[chNum][ipll][iwire] > DigitsArray_[chNum][ipll][iwire-1] ) {
            Next_next_wire = 0;
            Last_wire_Digit = 1000;
            continue;
          } else {
            Nfirst[chNum][ipll] = iwire;
            ClusterSize_[chNum][ipll][Nclust_[chNum][ipll]] = 1;
            Coord_wire_[chNum][ipll][Nclust_[chNum][ipll]] = Nfirst[chNum][ipll]+ 0.5*ClusterSize_[chNum][ipll][Nclust_[chNum][ipll]];
            Coord_xuv_[chNum][ipll][Nclust_[chNum][ipll]] = (Coord_wire_[chNum][ipll][Nclust_[chNum][ipll]]- kMiddlePl)* dw;
            Next_next_wire = 1;
            Last_wire_Digit = DigitsArray_[chNum][ipll][Nfirst[chNum][ipll]];

            // made for the canonical sequence / x- v- u+ x+ v+ u-/ 0 1 5 have back reading
            if (ipll == 0 || ipll == 1 || ipll == 5 ) Coord_xuv_[chNum][ipll][Nclust_[chNum][ipll]] = - Coord_xuv_[chNum][ipll][Nclust_[chNum][ipll]];

            if (fDebug) {
              hClusterSize.at(chNum)->Fill(ClusterSize_[chNum][ipll][Nclust_[chNum][ipll]]);
              cout<<" Coord_xuv_["<< chNum<<"]["<<ipll<<"]["<<Nclust_[chNum][ipll]<<"] "<<Coord_xuv_[chNum][ipll][Nclust_[chNum][ipll]]<<endl;
              cout<<"2 Nfirst "<< Nfirst[chNum][ipll]<<" ClusterSize_ "<<ClusterSize_[chNum][ipll][Nclust_[chNum][ipll]]<<" Coord_wire_ "<<Coord_wire_[chNum][ipll][Nclust_[chNum][ipll]]<<endl;

              hoccupancyXm.at(chNum)->Fill(Coord_xuv_[chNum][0][Nclust_[chNum][ipll]]);
              hoccupancyVm.at(chNum)->Fill(Coord_xuv_[chNum][1][Nclust_[chNum][ipll]]);
              hoccupancyUp.at(chNum)->Fill(Coord_xuv_[chNum][2][Nclust_[chNum][ipll]]);
              hoccupancyXp.at(chNum)->Fill(Coord_xuv_[chNum][3][Nclust_[chNum][ipll]]);
              hoccupancyVp.at(chNum)->Fill(Coord_xuv_[chNum][4][Nclust_[chNum][ipll]]);
              hoccupancyUm.at(chNum)->Fill(Coord_xuv_[chNum][5][Nclust_[chNum][ipll]]);
            }

            Nclust_[chNum][ipll]++;
            Nfirst[chNum][ipll] = -1;
          }//else
        }

        if( Nfirst[chNum][ipll] >= 0 && (DigitsArray_[chNum][ipll][iwire] > 0 && DigitsArray_[chNum][ipll][iwire] < DigitsArray_[chNum][ipll][Nfirst[chNum][ipll]]) )
          {Nfirst[chNum][ipll] = iwire; 
            if (fDebug) cout<<" first  Nfirst "<<Nfirst[chNum][ipll]<<endl;
            }

        if( Nfirst[chNum][ipll] >= 0 && (DigitsArray_[chNum][ipll][iwire] == 0 ||
                         (DigitsArray_[chNum][ipll][iwire] > 0 && DigitsArray_[chNum][ipll][iwire] > DigitsArray_[chNum][ipll][Nfirst[chNum][ipll]]) )) {

          ClusterSize_[chNum][ipll][Nclust_[chNum][ipll]] = 1;
          Coord_wire_[chNum][ipll][Nclust_[chNum][ipll]] = Nfirst[chNum][ipll]+ 0.5*ClusterSize_[chNum][ipll][Nclust_[chNum][ipll]];
          Coord_xuv_[chNum][ipll][Nclust_[chNum][ipll]] = (Coord_wire_[chNum][ipll][Nclust_[chNum][ipll]]- kMiddlePl)* dw;
          if ( DigitsArray_[chNum][ipll][iwire] > 0) Next_next_wire = 1;
          
          Last_wire_Digit = DigitsArray_[chNum][ipll][Nfirst[chNum][ipll]];

          // made for the canonical sequence / x- v- u+ x+ v+ u-/ 0 1 5 have back reading
          if (ipll == 0 || ipll == 1 || ipll == 5 ) Coord_xuv_[chNum][ipll][Nclust_[chNum][ipll]] = - Coord_xuv_[chNum][ipll][Nclust_[chNum][ipll]];
          if (fDebug) {
            hClusterSize.at(chNum)->Fill(ClusterSize_[chNum][ipll][Nclust_[chNum][ipll]]);
            cout<<" Coord_xuv_["<< chNum<<"]["<<ipll<<"]["<<Nclust_[chNum][ipll]<<"] "<<Coord_xuv_[chNum][ipll][Nclust_[chNum][ipll]]<<endl;
            cout<<"1 Nfirst "<< Nfirst[chNum][ipll]<<" ClusterSize_ "<<ClusterSize_[chNum][ipll][Nclust_[chNum][ipll]]<<" Coord_wire_ "<<Coord_wire_[chNum][ipll][Nclust_[chNum][ipll]]<<endl;
            //cout<<" iwire "<<iwire<<endl;

            hoccupancyXm.at(chNum)->Fill(Coord_xuv_[chNum][0][Nclust_[chNum][ipll]]);
            hoccupancyVm.at(chNum)->Fill(Coord_xuv_[chNum][1][Nclust_[chNum][ipll]]);
            hoccupancyUp.at(chNum)->Fill(Coord_xuv_[chNum][2][Nclust_[chNum][ipll]]);
            hoccupancyXp.at(chNum)->Fill(Coord_xuv_[chNum][3][Nclust_[chNum][ipll]]);
            hoccupancyVp.at(chNum)->Fill(Coord_xuv_[chNum][4][Nclust_[chNum][ipll]]);
            hoccupancyUm.at(chNum)->Fill(Coord_xuv_[chNum][5][Nclust_[chNum][ipll]]);
          }
          
          Nclust_[chNum][ipll]++;
          Nfirst[chNum][ipll] = -1;
        }
        
        if( Nfirst[chNum][ipll] < 0 && DigitsArray_[chNum][ipll][iwire] > 0. && !Next_next_wire)  Nfirst[chNum][ipll] = iwire;
        
        if (iwire == 95 && Nfirst[chNum][ipll] > 0 ){
          ClusterSize_[chNum][ipll][Nclust_[chNum][ipll]] = 1;
          Coord_wire_[chNum][ipll][Nclust_[chNum][ipll]] = Nfirst[chNum][ipll]+ 0.5*ClusterSize_[chNum][ipll][Nclust_[chNum][ipll]];
          Coord_xuv_[chNum][ipll][Nclust_[chNum][ipll]] = (Coord_wire_[chNum][ipll][Nclust_[chNum][ipll]]- kMiddlePl)* dw;
          if (fDebug) cout<<" Coord_xuv_["<< chNum<<"]["<<ipll<<"]["<<Nclust_[chNum][ipll]<<"] "<<Coord_xuv_[chNum][ipll][Nclust_[chNum][ipll]]<<endl;
          if (fDebug)  cout<<"3 Nfirst "<< Nfirst[chNum][ipll]<<" ClusterSize_ "<<ClusterSize_[chNum][ipll][Nclust_[chNum][ipll]]<<" Coord_wire_ "<<Coord_wire_[chNum][ipll][Nclust_[chNum][ipll]]<<endl;
          if (fDebug)  cout<<" iwire "<<iwire<<endl;
          Nclust_[chNum][ipll]++;
          Nfirst[chNum][ipll] = -1;
        }

      }//iwire
      //Num_layers_out_beam += Fired_layer[chNum][ipll];
    }// ipll
    
   // if (fDebug) hNum_layers_out_beam_Ch.at(chNum)->Fill( Num_layers_out_beam);
 //}//else chamber 23
  

}//Clustering
//----------------------------------------------------------------------


//----------------- Combinatorial segment selection --------------------
void BmnMwpcHitFinder::SegmentFinder(Int_t chNum, Int_t **Nclust_, Double_t ***Coord_xuv_, Int_t ***ClusterSize_,
                   Int_t *Nsegm, Double_t ***XVU_coor, Double_t ***Cluster_coor, Int_t **Nhits_Ch_,Int_t minHits, Short_t code, Int_t kBig_ ) {

  //Coord_xuv_   - coordinates of all clusters
  Int_t minHits4_5;  
  if ( chNum > 1 ) minHits4_5 = minHits;//for run7
  else minHits4_5 = kMinHits_before_target;
  //if (fDebug) cout<<" chNum "<<chNum<<" minHits4_5 "<<minHits4_5<<endl;

  Double_t min_for_triples  = 5.; // minimum delta wires for tree planes
  Double_t min_for_conjugate = 3.; // minimum delta wires for conjugate plane

  // code : first triples is
  // 1 {X-, V-, U+}
  // 2 {X-, V+, U+}
  // 3 {X-, V-, U-}
  // 7 {X+, V-, U+}
  // 5 {X+, V+, U+}
  // 6 {X+, V-, U-}
  // 4 {X-, V+, U-}
  // 8 {X+, V+, U-}

  Int_t x = 0, v = 1, u = 2 , x1 = 3, v1 = 4, u1 = 5;//MK

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
  case 7:
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
  case 4:
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

  if (Nsegm[chNum] > kBig_ - 2) return;// MP

  for (Int_t ix = 0; ix < Nclust_[chNum][x]; ix++) {

    for (Int_t iv = 0; iv < Nclust_[chNum][v]; iv++) {

      for (Int_t iu = 0; iu < Nclust_[chNum][u]; iu++) {

        if (Nsegm[chNum] > kBig_ - 2) return;

        // --for repeated triples--
        Bool_t it_was = 0;
        if (Nsegm[chNum] > 0) {
          for (Int_t iseg = 0; iseg < Nsegm[chNum]; iseg++) {
            Bool_t it_was_x = 0;
            Bool_t it_was_v = 0;
            Bool_t it_was_u = 0;

            if (XVU_coor[chNum][x][iseg] == Coord_xuv_[chNum][x][ix]) it_was_x = 1;
            if (XVU_coor[chNum][v][iseg] == Coord_xuv_[chNum][v][iv]) it_was_v = 1;
            if (XVU_coor[chNum][u][iseg] == Coord_xuv_[chNum][u][iu]) it_was_u = 1;

            it_was = it_was_x * it_was_v * it_was_u;

            if (it_was) {
              break;
            }
          } // iseg
        } // Nsegm[chNum] > 0

        if (it_was) continue;
        // --for repeated triples.

        //--- main equation---// u + v - x = delta
        if (fabs(Coord_xuv_[chNum][u][iu] + Coord_xuv_[chNum][v][iv] - Coord_xuv_[chNum][x][ix]) < min_for_triples*dw ) {

          // 3p-candidate new Nsegm
          XVU_coor[chNum][x][Nsegm[chNum]]   = Coord_xuv_[chNum][x][ix];
          XVU_coor[chNum][v][Nsegm[chNum]]   = Coord_xuv_[chNum][v][iv];
          XVU_coor[chNum][u][Nsegm[chNum]]   = Coord_xuv_[chNum][u][iu];
          Cluster_coor[chNum][x][Nsegm[chNum]] = ClusterSize_[chNum][x][ix];
          Cluster_coor[chNum][v][Nsegm[chNum]] = ClusterSize_[chNum][v][iv];
          Cluster_coor[chNum][u][Nsegm[chNum]] = ClusterSize_[chNum][u][iu];

          XVU_coor[chNum][x1][Nsegm[chNum]]  = -999.;
          XVU_coor[chNum][v1][Nsegm[chNum]]  = -999.;
          XVU_coor[chNum][u1][Nsegm[chNum]]  = -999.;
          Cluster_coor[chNum][x1][Nsegm[chNum]]= -1;
          Cluster_coor[chNum][v1][Nsegm[chNum]]= -1;
          Cluster_coor[chNum][u1][Nsegm[chNum]]= -1;

          Nhits_Ch_[chNum][Nsegm[chNum]] = 3;
          //if (fDebug)cout<<" Nhits 3 = "<<Nhits_Ch_[chNum][Nsegm[chNum]]<<endl;
          //if (fDebug)cout<<endl;

          //-- if 3p-candidate was look for conjugate coord
          if ( XVU_coor[chNum][x][Nsegm[chNum]] != -999.) {
            Bool_t point_was = 0;
            Float_t Min_D = min_for_conjugate*dw;
            for (Int_t ix2 = 0; ix2 < Nclust_[chNum][x1]; ix2++) {

              if(abs( XVU_coor[chNum][x][Nsegm[chNum]] - Coord_xuv_[chNum][x1][ix2] ) < Min_D) {
                XVU_coor[chNum][x1][Nsegm[chNum]]   = Coord_xuv_[chNum][x1][ix2];
                Cluster_coor[chNum][x1][Nsegm[chNum]] = ClusterSize_[chNum][x1][ix2];
                if ( !point_was ) Nhits_Ch_[chNum][Nsegm[chNum]]++;// 4 points
                point_was = 1;
                Min_D = abs( XVU_coor[chNum][x][Nsegm[chNum]] - Coord_xuv_[chNum][x1][ix2] );
              }//abs(
            }//ix2
          } //if it was

          //if (fDebug)cout<<" Nhits 4 = "<<Nhits_Ch_[chNum][Nsegm[chNum]]<<endl;
          //if (fDebug)cout<<endl;

          if ( XVU_coor[chNum][v][Nsegm[chNum]] != -999.) {
            Bool_t point_was = 0;
            Float_t Min_D = min_for_conjugate*dw;
            for (Int_t iv2 = 0; iv2 < Nclust_[chNum][v1]; iv2++) {

              if(abs( XVU_coor[chNum][v][Nsegm[chNum]] - Coord_xuv_[chNum][v1][iv2] ) < Min_D) {
                XVU_coor[chNum][v1][Nsegm[chNum]]   = Coord_xuv_[chNum][v1][iv2];
                Cluster_coor[chNum][v1][Nsegm[chNum]] = ClusterSize_[chNum][v1][iv2];
                if ( !point_was ) Nhits_Ch_[chNum][Nsegm[chNum]]++;// 5 points
                point_was = 1;
                Min_D =abs( XVU_coor[chNum][v][Nsegm[chNum]] - Coord_xuv_[chNum][v1][iv2] );
              }//abs(
            }//iv2
          } //if it was
          //if (fDebug)cout<<" Nhits 5 = "<<Nhits_Ch_[chNum][Nsegm[chNum]]<<endl;
          //if (fDebug)cout<<endl;

          if ( XVU_coor[chNum][u][Nsegm[chNum]] != -999.) {
            Bool_t point_was = 0;
            Float_t Min_D = min_for_conjugate*dw;
            for (Int_t iu2 = 0; iu2 < Nclust_[chNum][u1]; iu2++) {
              if(abs( XVU_coor[chNum][u][Nsegm[chNum]] - Coord_xuv_[chNum][u1][iu2] ) < Min_D) {
                XVU_coor[chNum][u1][Nsegm[chNum]]   = Coord_xuv_[chNum][u1][iu2];
                Cluster_coor[chNum][u1][Nsegm[chNum]] = ClusterSize_[chNum][u1][iu2];
                if ( !point_was ) Nhits_Ch_[chNum][Nsegm[chNum]]++;// 6 points
                point_was = 1;
                Min_D =abs( XVU_coor[chNum][u][Nsegm[chNum]] - Coord_xuv_[chNum][u1][iu2] );
              }//abs(
            }//iu2
          } //if it was

          if ( chNum > 1 && Nhits_Ch_[chNum][Nsegm[chNum]] > 4 ) minHits4_5 = minHits;
          if (Nsegm[chNum] > 15) minHits4_5 = 5;
          if (Nsegm[chNum] > 30) minHits4_5 = 6;
          
          //if (fDebug) cout<<" chNum "<<chNum<<" minHits4_5 "<<minHits4_5<<" Nsegm "<< Nsegm[chNum]<<endl;

          if (Nhits_Ch_[chNum][Nsegm[chNum]] >= minHits4_5) {
            //if (fDebug) cout<<endl;
            //if (fDebug) cout<<" Nsegm "<< Nsegm[chNum] <<" Nhits_Ch_["<<chNum<<"]["<<Nsegm[chNum]<<"] "<<Nhits_Ch_[chNum][Nsegm[chNum]]<<" minHits4_5 "<< minHits4_5 <<endl;
            //if (fDebug) cout<<" coord "<<XVU_coor[chNum][0][Nsegm[chNum]]<<" "<<XVU_coor[chNum][1][Nsegm[chNum]]<<" "<<XVU_coor[chNum][2][Nsegm[chNum]]<<" "<<XVU_coor[chNum][3][Nsegm[chNum]]<<" "<<XVU_coor[chNum][4][Nsegm[chNum]]<<" "<<XVU_coor[chNum][5][Nsegm[chNum]]<<" "<<endl;

            Nsegm[chNum]++; //go to next segment
          }
          if (Nhits_Ch_[chNum][Nsegm[chNum]] < minHits4_5) {
            //--segment deleting
            Nhits_Ch_[chNum][Nsegm[chNum]] = 0;

            XVU_coor[chNum][x ][Nsegm[chNum]]  = -999.;
            XVU_coor[chNum][v ][Nsegm[chNum]]  = -999.;
            XVU_coor[chNum][u ][Nsegm[chNum]]  = -999.;
            XVU_coor[chNum][x1][Nsegm[chNum]]  = -999.;
            XVU_coor[chNum][v1][Nsegm[chNum]]  = -999.;
            XVU_coor[chNum][u1][Nsegm[chNum]]  = -999.;
            Cluster_coor[chNum][x ][Nsegm[chNum]]= -1;
            Cluster_coor[chNum][v ][Nsegm[chNum]]= -1;
            Cluster_coor[chNum][u ][Nsegm[chNum]]= -1;
            Cluster_coor[chNum][x1][Nsegm[chNum]]= -1;
            Cluster_coor[chNum][v1][Nsegm[chNum]]= -1;
            Cluster_coor[chNum][u1][Nsegm[chNum]]= -1;
          }//else
        }// u + v - x = delta
        if (Nsegm[chNum] > kBig_ - 2) break;
      }//iu
      if (Nsegm[chNum] > kBig_ - 2) break;
    }//iv
    if (Nsegm[chNum] > kBig_ - 2) break;
  }//ix
  if (Nsegm[chNum] > kBig_ - 2)return;
  //if (fDebug) cout<<" --end SegmentFinder: Nsegm["<<chNum<<"] "<<Nsegm[chNum]<<endl;

}//SegmentFinder1
//----------------------------------------------------------------------


//------------ Segment fitting & finding the best track-candidate ------
void BmnMwpcHitFinder::ProcessSegments( Int_t chNum, Int_t *Nsegm, Double_t ***XVU_coor, Double_t ***Cluster_coor, Int_t **Nhits_Ch_,
  Float_t **z_loc, Int_t Min_hits, Double_t sigma_, Double_t kChi2_Max_,
  Int_t **Nhits_seg_ , Double_t **Chi2_ndf_seg_, Double_t ***coor_seg, Double_t ***cluster_seg, Double_t ***Par_ab_seg, Int_t *Nbest_seg_, Int_t *Nlay_w_wires_,
  Double_t ****sigma2_s) {

  segments seg[kNChambers][kBig];

  Double_t par_ab[kNChambers][kNPlanes][kBig];
  Double_t dx_[kNPlanes];
  Double_t Chi2[kNChambers][kBig];
  Double_t Chi2_ndf[kNChambers][kBig];
  Double_t sigm[kNPlanes], sigm2_[kNPlanes];
  Int_t h1[kNPlanes];
  Double_t bmatr_seg[kBig][4][4];
  for (Int_t i = 0; i < kBig; i++) {
    for (Int_t i1 = 0; i1 < 4; i1++) {
      for (Int_t j1 = 0; j1 < 4; j1++) {
        bmatr_seg[i][i1][j1] = 0.;
      }
    }
  }

  Double_t delta = 3*dw;
  Double_t Chi2_Super_min = 0.001;
  Int_t Min_hits6;
  if ( chNum > 1 ) Min_hits6 = Min_hits;//for run7
  else Min_hits6 = kMinHits_before_target;

  // cout<<" Nlay_w_wires["<<chNum<<"] "<<Nlay_w_wires_[chNum]<<endl;
  // TEMPORARY OUT SEGMENTS ARRY
  int   OutSegCount = 0;
  segments OutSegArray[kmaxSeg];

  if ( Nlay_w_wires_[chNum] < 4 ) return;

  if (chNum > 1 && Min_hits < 6 ) {
    if ( Nlay_w_wires_[chNum] == 4) Min_hits6 = 4;
  }

  if (Nsegm[chNum] > kBig - 2) return;
  /*
  if (fDebug) cout<<" ProcessSegments "<<endl;
  for (Int_t iseg = 0; iseg < Nsegm[chNum]; iseg++) { //---print number of hits
    if (fDebug) cout<<" iseg "<<iseg<<" Nhits "<<Nhits_Ch_[chNum][iseg]<<endl;
  }
  */

  for (Int_t Nhitm = kNPlanes; Nhitm > Min_hits6 - 1; Nhitm--) {// -- first view 6 points
    Bool_t ifNhitm = 0;

    if (Nhitm < Min_hits6) break;

    for (Int_t iseg = 0; iseg < Nsegm[chNum]; iseg++) { //---cycle by segments
      if (Nbest_seg_[chNum] > kBig - 2) return;
      ifNhitm = 1;

      if (Nhits_Ch_[chNum][iseg] != Nhitm) continue;
     // if (fDebug) cout<<" iseg "<<iseg<<" Nhits "<<Nhits_Ch_[chNum][iseg]<<endl;
      if ( Nhits_Ch_[chNum][iseg] != 0) {
        Nhits_Ch_[chNum][iseg] = 0;
        for (Int_t i1 = 0; i1 < kNPlanes; i1++) {
          if (XVU_coor[chNum][i1][iseg] > -900.) Nhits_Ch_[chNum][iseg]++;
        }
        if (Nhits_Ch_[chNum][iseg] < Min_hits6) Nhits_Ch_[chNum][iseg] = 0; // control shot
      }


      //if (fDebug) cout<<"after recalc nhits iseg "<<iseg<<" Nhits "<<Nhits_Ch_[chNum][iseg]<<endl;
      //-----!!!----
      if ( Nhits_Ch_[chNum][iseg] == 0) continue; // go to next segment!

      for(Int_t i = 0; i < 6; i++) {
        sigm[i]= 0.;
        h1[i] = 0;
        if ( XVU_coor[chNum][i][iseg] > -900.) {
          h1[i] = 1;
          sigm[i] = ( Cluster_coor[chNum][i][iseg]*dw)/sq12;
          sigm2_[i] = sigm[i]*sigm[i];
        }//if coord was
        //if (fDebug) cout<<" chNum "<<chNum<<" i "<<i<<" iseg "<<iseg<<" coor "<<XVU_coor[chNum][i][iseg]<<endl;
        //if (fDebug) cout<<" chNum "<<chNum<<" h "<<h1[i]<<" coor "<<XVU_coor[chNum][i][iseg]<<" z_loc "<<z_loc[chNum][i]<<endl;
        //if (fDebug)	cout<<" h "<<h1[i]<<" Cluster_coor["<<chNum<<"]["<<i<<"]["<<iseg<<"]= "<<Cluster_coor[chNum][i][iseg]<<" sigm "<<sigm[i]<<endl;
      }
      if (fDebug) cout<<endl;
      
      for(Int_t im=0; im<4; im++) {
        for(Int_t ii=0; ii<4; ii++) {
          Amatr[im][ii] = 0.;
          bmatr[im][ii] = 0.;
        }
      }
      Double_t matrF[4] = {0,0,0,0};//free coef

      FillFitMatrix(chNum, Amatr, z_loc, sigm2_, h1);
      FillFreeCoefVector(chNum, matrF, XVU_coor, iseg, z_loc , sigm2_, h1);

      //Gaussian algorithm for 4x4 matrix inversion
      Double_t A0matr[4][4];
      for (Int_t i1 = 0; i1 < 4; i1++) {
        for (Int_t j1 = 0; j1 < 4; j1++) {
          A0matr[i1][j1] = Amatr[i1][j1];
          //if (fDebug) cout<<" Amatr["<<i1<<"]["<<j1<<"] "<<Amatr[i1][j1]<<endl;
        }
      }

      InverseMatrix(Amatr,bmatr);
      Double_t sum;
      Double_t A1[4][4] = {{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0}};
      //cout<<" A1 "<<endl;

      for (Int_t i1 = 0; i1 < 4; ++i1)
        for (Int_t j1 = 0; j1 < 4; ++j1) {
          sum = 0;
          for (Int_t k1 = 0; k1 < 4; ++k1) {
            Double_t a0 = A0matr[i1][k1];
            Double_t b0 = bmatr[k1][j1];
            sum += a0 * b0;
            A1[i1][j1] = sum;
          }
        }

      for(Int_t i1 = 0 ; i1 < 4; i1++) {
        par_ab[chNum][i1][iseg] = 0;
        for(Int_t j1 = 0; j1 < 4; j1++) {
          par_ab[chNum][i1][iseg] += bmatr[i1][j1]*matrF[j1];
          bmatr_seg[iseg][i1][j1] = 2*bmatr[i1][j1];
          //if (fDebug) 	cout<<" i1 "<<i1<<" bmatr "<<bmatr[i1][j1]<<" F "<<matrF[j1] <<endl;
        }
      } // i1

      //---Chi2 calculating---
      Chi2[chNum][iseg] = 0.;
      Chi2_ndf[chNum][iseg] = 999.;

      for(Int_t i1 = 0 ; i1 < 6; i1++) {
        dx_[i1] = 0.;

        if ( XVU_coor[chNum][i1][iseg] > -999.) {
          if(i1==0 || i1==3) {
            dx_[i1] = XVU_coor[chNum][i1][iseg] - par_ab[chNum][0][iseg]*z_loc[chNum][i1]-par_ab[chNum][1][iseg];
            //if (fDebug) cout<<" fitX "<<par_ab[chNum][0][iseg]*z_loc[chNum][i1]+par_ab[chNum][1][iseg]<<endl;
          }
          if (fDebug && i1 == 0) hResiduals_pl0_Ch.at(chNum)->Fill(dx_[i1]);
          if (fDebug && i1 == 3) hResiduals_pl3_Ch.at(chNum)->Fill(dx_[i1]);
          if(i1==2 || i1==5) {
            dx_[i1] = XVU_coor[chNum][i1][iseg]-0.5*(par_ab[chNum][0][iseg]+sq3*par_ab[chNum][2][iseg])*z_loc[chNum][i1]-0.5*(par_ab[chNum][1][iseg]+sq3*par_ab[chNum][3][iseg]);
           // if (fDebug) cout<<" fitU "<<0.5*(par_ab[chNum][0][iseg]+sq3*par_ab[chNum][2][iseg])*z_loc[chNum][i1]+0.5*(par_ab[chNum][1][iseg]+sq3*par_ab[chNum][3][iseg])<<endl;
          }
          if (fDebug && i1 == 2) hResiduals_pl2_Ch.at(chNum)->Fill(dx_[i1]);
          if (fDebug && i1 == 5) hResiduals_pl5_Ch.at(chNum)->Fill(dx_[i1]);
          if(i1==1 || i1==4) {
            dx_[i1] = XVU_coor[chNum][i1][iseg]-0.5*(par_ab[chNum][0][iseg]-sq3*par_ab[chNum][2][iseg])*z_loc[chNum][i1]-0.5*(par_ab[chNum][1][iseg]-sq3*par_ab[chNum][3][iseg]);
            //if (fDebug) cout<<" fitV "<<0.5*(par_ab[chNum][0][iseg]-sq3*par_ab[chNum][2][iseg])*z_loc[chNum][i1]+0.5*(par_ab[chNum][1][iseg]-sq3*par_ab[chNum][3][iseg])<<endl;
          }
          if (fDebug && i1 == 1) hResiduals_pl1_Ch.at(chNum)->Fill(dx_[i1]);
          if (fDebug && i1 == 4) hResiduals_pl4_Ch.at(chNum)->Fill(dx_[i1]);
          Chi2[chNum][iseg] += dx_[i1]*dx_[i1]/(sigm2_[i1]);
          //if (fDebug) cout<<"iseg "<<iseg <<" i "<<i1<<" dx "<<dx_[i1]<<" coor "<<XVU_coor[chNum][i1][iseg]<<" Chi2 "<<Chi2[chNum][iseg]<<" z "<<z_loc[chNum][i1]<<endl;
        }
      }//i1
      //---Chi2 calculating.---
      if (fDebug) cout<<"---Chi2 calculating.--- Nhits["<<chNum<<"]["<<iseg<<"] "<<Nhits_Ch_[chNum][iseg]<<" Chi2 "<< Chi2[chNum][iseg]<<endl;
      if (Nhits_Ch_[chNum][iseg] > 4) {
        Chi2_ndf[chNum][iseg] = Chi2[chNum][iseg]/(Nhits_Ch_[chNum][iseg]-4);
        if (fDebug)	cout<<" Chi2_ndf["<<chNum<<"]["<<iseg<<"] "<< Chi2_ndf[chNum][iseg]<<endl;
      }else{ 
        Chi2_ndf[chNum][iseg] = 0.;
      }

      if (Chi2_ndf[chNum][iseg] > kChi2_Max_ ){ // --if bad chi2--
       
        if (Nhits_Ch_[chNum][iseg] <= Min_hits6) { // --if no enough points--
          Nhits_Ch_[chNum][iseg] = 0;
          continue;
        } else { //--reject most distant point--
          if (fDebug) cout<<" iseg "<<iseg<<" --reject bad point"<<endl;
          for (Int_t i1 = 0; i1 < kNPlanes; i1++) {
            if (XVU_coor[chNum][i1][iseg] > -999. && fabs(dx_[i1]) > delta) {
              XVU_coor[chNum][i1][iseg] = -999.;
              Nhits_Ch_[chNum][iseg]--;// --reject bad point
              if (Nhits_Ch_[chNum][iseg] < Min_hits6) Nhits_Ch_[chNum][iseg] = 0;
              break;//continue;
            }//if point was
          }//i1
        }//else

      }// if bad chi2.
      
      if (fDebug) cout<<"---Nhits calc--- Nhits["<<chNum<<"]["<<iseg<<"] "<<Nhits_Ch_[chNum][iseg]<<" Chi2 "<< Chi2[chNum][iseg]<<endl;

      if ( Nhits_Ch_[chNum][iseg] != 0) {
        // if ( Chi2_ndf[chNum][iseg] > Chi2_Super_min && Chi2_ndf[chNum][iseg] < kChi2_Max_ && Nhits_Ch_[chNum][iseg] != 0){
        Nhits_Ch_[chNum][iseg] = 0;
        for (Int_t i1 = 0; i1 < kNPlanes; i1++) {
          if (XVU_coor[chNum][i1][iseg] > -900.) Nhits_Ch_[chNum][iseg]++;
        }
        if (fDebug) cout<<"if ( Nhits_Ch_[chNum][iseg] != 0)  Nhits_Ch "<<Nhits_Ch_[chNum][iseg]<<" Min_hits6 "<<Min_hits6<<endl;
        if (Nhits_Ch_[chNum][iseg] < Min_hits6) Nhits_Ch_[chNum][iseg] = 0; // control shot
      }

    }//--iseg--------------


    Double_t x_target,y_target;

    for (Int_t iseg = 0; iseg < Nsegm[chNum]; iseg++) {
      x_target  = par_ab[chNum][0][iseg]*( Z0_SRC - ChZ[chNum]) + par_ab[chNum][1][iseg];
      y_target  = par_ab[chNum][2][iseg]*( Z0_SRC - ChZ[chNum]) + par_ab[chNum][3][iseg];
     // if (fDebug) cout<<Nhits_Ch_[chNum][iseg]<<" ";
     // if (fDebug && Nhits_Ch_[chNum][iseg] == Nhitm ) cout<<" chNum "<<chNum<<" iseg "<<iseg<<" Nhits_Ch "<<Nhits_Ch_[chNum][iseg]<<" Chi2 "<<Chi2_ndf[chNum][iseg]<<" x_target "<<x_target<<" y_target "<<y_target<<endl;
     // if (fDebug && Nhits_Ch_[chNum][iseg] == Nhitm ) cout<<" ax "<<par_ab[chNum][0][iseg]<<" bx "<<par_ab[chNum][1][iseg]<<" ay "<<par_ab[chNum][2][iseg]<<" by "<<par_ab[chNum][3][iseg]<<endl;
    }
    if (fDebug) cout<<endl;
    if (!ifNhitm) continue;

    //--choice of min chi2

    Int_t vec_ind_best_seg[kmaxSeg];
    Int_t Nbest_vec_ind = 0;

    for (Int_t ind_best = 0; ind_best < kmaxSeg; ind_best++) {
      Double_t Chi2_best = 999.;
      Int_t iseg_best = -1;
      vec_ind_best_seg[ind_best] = -1;

      for (Int_t iseg = 0; iseg < Nsegm[chNum]; iseg++) {

        if (Nhits_Ch_[chNum][iseg] != Nhitm) continue;
        if (Nhitm > 4 && Chi2_ndf[chNum][iseg] >= Chi2_best) continue;

        Bool_t best_was = 0;
        if ( Nbest_vec_ind > 0 ) {
          for ( Int_t ii = 0; ii < Nbest_vec_ind; ii++) {
            if ( iseg == vec_ind_best_seg[ii] ) {
              best_was = 1;
              break;
            }
          }
        }
        if ( best_was ) continue;
        Chi2_best = Chi2_ndf[chNum][iseg];
        iseg_best = iseg;
      } // iseg

      if (iseg_best == -1) continue;
      vec_ind_best_seg[ind_best] = iseg_best;
      Nbest_vec_ind++;

      //-- reject(common points)
      for (Int_t iseg = 0; iseg < Nsegm[chNum]; iseg++) {
        //if (iseg == iseg_best)continue;
        Bool_t best_was= 0;
        if ( Nbest_vec_ind >0 ) {
          for (Int_t ii = 0; ii < Nbest_vec_ind; ii++) {
            if ( iseg == vec_ind_best_seg[ii] ) {
              best_was = 1;
              break;
            }
          }
        }
        if ( best_was ) continue;

        for (Int_t i1 = 0; i1 < kNPlanes; i1++) {
          if ( XVU_coor[chNum][i1][iseg] > -999.) {
            if( fabs(XVU_coor[chNum][i1][iseg] - XVU_coor[chNum][i1][iseg_best]) < 3*dw_half ) {
              //if (fDebug) cout<<" reject point Nhits_Ch = 0 iseg = "<<iseg<<endl;
              Nhits_Ch_[chNum][iseg] = 0;
            }
          }
        }//i1
      }// iseg
      //if (fDebug) cout<<" Nbest_vec_ind "<<Nbest_vec_ind<<endl;
      //if (fDebug) cout<<" Chi2_best "<<Chi2_best<<" Nhits "<<Nhits_Ch_[chNum][iseg_best]<<endl;
    }//ind_best
    
    vector<segments> vtmpSeg;
    segments tmpSeg;

    for (int itSeg = 0; itSeg < Nsegm[chNum]; ++itSeg) {
      if (Nhits_Ch_[chNum][itSeg] == Nhitm && Chi2_ndf[chNum][itSeg] < kChi2_Max_) {
        tmpSeg.Nhits = Nhits_Ch_[chNum][itSeg];
        tmpSeg.Chi2 = Chi2_ndf[chNum][itSeg];

        for(int ipla = 0; ipla < kNPlanes; ipla++) {
          tmpSeg.coord[ipla] = XVU_coor[chNum][ipla][itSeg];
          tmpSeg.clust[ipla] = Cluster_coor[chNum][ipla][itSeg];
        }
        for(int ipar = 0; ipar < 4; ipar++) {
          tmpSeg.param[ipar] = par_ab[chNum][ipar][itSeg];
        }
        for (Int_t i1 = 0; i1 < 4; i1++) {
          for (Int_t j1 = 0; j1 < 4; j1++) {
            tmpSeg.sigma2[i1][j1] = bmatr_seg[itSeg][i1][j1];
          }
        }
        vtmpSeg.push_back(tmpSeg);
      }
    }

    // vector sorting
    if ( Nhitm > 4) sort(vtmpSeg.begin(), vtmpSeg.end(), compareSegments);

    // storing
    for (int iterOut = 0; iterOut < vtmpSeg.size(); iterOut++) {
      if (OutSegCount < kmaxSeg) {
        OutSegArray[OutSegCount] = vtmpSeg.at(iterOut);
        OutSegCount++;
      }
    }
    // !!! vector clear for next Nhitm
    //if ( Nhitm == 5)
    vtmpSeg.clear(); //clear for 4p-segments
    
   // if (fDebug) cout<<" ---------Nhitm "<<Nhitm<<endl;
   // for (Int_t iseg = 0; iseg < Nsegm[chNum]; iseg++) { //---print number of hits
   // if (fDebug) cout<<" iseg "<<iseg<<" Nhits "<<Nhits_Ch_[chNum][iseg]<<endl;
   // }
   // if (fDebug) cout<<" ---------"<<endl;
  }//Nhitm--

  for (int iterOut = 0; iterOut < kmaxSeg; iterOut++) {
    if(OutSegArray[iterOut].Chi2 < 900.) {
      Nbest_seg_[chNum]++;
      Nhits_seg_[chNum][iterOut] = OutSegArray[iterOut].Nhits;
      Chi2_ndf_seg_[chNum][iterOut] = OutSegArray[iterOut].Chi2;

      for (int iterCoord = 0; iterCoord < kNPlanes; iterCoord++) {
        coor_seg[chNum][iterCoord][iterOut] = OutSegArray[iterOut].coord[iterCoord];
        cluster_seg[chNum][iterCoord][iterOut] = OutSegArray[iterOut].clust[iterCoord];
      }
      for(int ipar = 0; ipar < 4; ipar++) {
        Par_ab_seg[chNum][ipar][iterOut] = OutSegArray[iterOut].param[ipar];
      }
      for (int i1 = 0; i1 < 4; i1++) {
        for (int j1 = 0; j1 < 4; j1++) {
          sigma2_s[chNum][iterOut][i1][j1] =  OutSegArray[iterOut].sigma2[i1][j1];
        }
      }
    }
  }

  // printf(">>>TESTING\n");
  // for (int iterOut = 0; iterOut < kmaxSeg; iterOut++){
  // if(OutSegArray[iterOut].Chi2 < 900.){
  // printf("> Ch: %d, Seg %d, Nhits: %d, Chi2: %8.4f\n", chNum, iterOut, OutSegArray[iterOut].Nhits, OutSegArray[iterOut].Chi2);
  //   for (int iterCoord = 0; iterCoord < kNPlanes; iterCoord++)
  //    printf("> %8.4f - %8.4f\n", OutSegArray[iterOut].coord[iterCoord], OutSegArray[iterOut].clust[iterCoord]);
  //  }
  // }

}//ProcessSegments
//----------------------------------------------------------------------


// --------------local parameters to Global parameters------------------
void BmnMwpcHitFinder::SegmentParamAlignment(Int_t chNum, Int_t *Nbest, Double_t ***par_ab, Float_t **shiftt ) {
  if (fDebug) cout<<endl;
  if (fDebug) cout<<" SegmentParamAlignment "<<endl;

  for (Int_t iBest = 0; iBest < Nbest[chNum]; iBest++) {
    // cout<<" chNum "<<chNum<<" before Alignment: iBest "<<iBest<<" Ax "<< par_ab[chNum][0][iBest]<<" bx "<< par_ab[chNum][1][iBest]<<" Ay "<< par_ab[chNum][1][iBest]<<" by "<< par_ab[chNum][3][iBest]<<endl;
    //                   ax     alpha                   ax^2
    par_ab[chNum][0][iBest] += shiftt[chNum][0] + shiftt[chNum][0]* par_ab[chNum][0][iBest]* par_ab[chNum][0][iBest];
    par_ab[chNum][2][iBest] += shiftt[chNum][2] + shiftt[chNum][2]* par_ab[chNum][2][iBest]* par_ab[chNum][2][iBest];
    par_ab[chNum][1][iBest] += shiftt[chNum][1];
    par_ab[chNum][3][iBest] += shiftt[chNum][3];
    if (fDebug ) cout<<" ax "<<par_ab[chNum][0][iBest]<<" bx "<<par_ab[chNum][1][iBest]<<" ay "<<par_ab[chNum][2][iBest] <<" by "<<par_ab[chNum][3][iBest]<<endl;

    Double_t x_target  = par_ab[chNum][0][iBest]*( Z0_SRC - ChZ[chNum]) + par_ab[chNum][1][iBest];
    Double_t y_target  = par_ab[chNum][2][iBest]*( Z0_SRC - ChZ[chNum]) + par_ab[chNum][3][iBest];
    //if (fDebug ) cout<<" chNum "<<chNum<<" Z "<<ChZ[chNum]<<" iBest "<<iBest<<" x_target "<< x_target<<" y_target "<< y_target<<endl;
    //if (fDebug) cout<<endl;

  }//iBest
}//SegmentParamAlignment
//----------------------------------------------------------------------


//------ Arrays Initialization -----------------------------------------
void BmnMwpcHitFinder::PrepareArraysToProcessEvent() {
  fBmnMwpcSegmentsArray->Delete();
  vec_points.clear();

  for(Int_t iCh = 0; iCh < kNChambers; iCh++) {
    Nseg_Ch[iCh] = 0;
    Nbest_Ch[iCh] = 0;
    Nbest_seg[iCh] = 0;
    Nlay_w_wires[iCh] = 0;

    for(Int_t iPl = 0; iPl < kNPlanes; iPl++) {
      iw_Ch[iCh][iPl] = 0;
      XVU[iCh][iPl]  = 0;
      XVU_cl[iCh][iPl] = 0;
      Nclust[iCh][iPl] = 0;
      for(Int_t iWire=0; iWire<kNWires; iWire++) {
        DigitsArray[iCh][iPl][iWire] = 0.;
      }

      for(Int_t iBig=0; iBig<kBig; iBig++) {
        Wires_Ch[iCh][iPl][iBig]   = -1;
        clust_Ch[iCh][iPl][iBig]   = -1;
        XVU_Ch[iCh][iPl][iBig]    = -999.;
        Coord_wire[iCh][iPl][iBig]  = -999.;
        Coord_xuv[iCh][iPl][iBig]  = -999.;
        ClusterSize[iCh][iPl][iBig] = 0;
        XVU_coord[iCh][iPl][iBig]  = -999.;
        Cluster_coord[iCh][iPl][iBig]= -1;
        Coor_seg[iCh][iPl][iBig]  = -999.;
        Cluster_seg[iCh][iPl][iBig]= -1;
      }
      for(Int_t iWire=0; iWire<kNWires; iWire++) {
        wire_Ch[iCh][iWire][iPl] = 0;
        xuv_Ch[iCh][iWire][iPl] = 0.;
      }
    }

    for(Int_t ii = 0; ii < 4; ii++) {
      for(Int_t jj=0; jj < kBig; jj++) {
        par_ab_Ch[iCh][ii][jj] = 999.;
        par_ab_seg[iCh][ii][jj] = 999.;
        for(Int_t j = 0; j < 4; j++) {
          sigma2_seg[iCh][jj][ii][j] = 0;
        }
      }
    }

    for(Int_t iBig=0; iBig<kBig; iBig++) {
      Nhits_Ch[iCh][iBig]  = 0;
      Nhits_seg[iCh][iBig]  = 0;
      Chi2_ndf_Ch[iCh][iBig] = 0;
    }

    for(Int_t i=0; i < kmaxSeg; i++) {
      ind_best_Ch[iCh][i]   = 0;
      best_Ch_gl[iCh][i]    = -1;
      Chi2_ndf_best_Ch[iCh][i] = -999.;
      Chi2_ndf_seg[iCh][i] = -999.;
    }
  }//iCh

  for(Int_t iPl=0; iPl<kNPlanes; iPl++) {
    sigm2[iPl] = sigma*sigma;
    ipl[iPl] = 6;
    z2[iPl] = 0;
  }

  for(Int_t ii = 0; ii < 4; ii++) {
    for(Int_t jj = 0; jj < 4; jj++) {
      matrA[ii][jj] = 0.;
      matrb[ii][jj] = 0.;
    }
  }
}//PrepareArraysToProcessEvent
//----------------------------------------------------------------------


//----------------------Fill FitMatrix----------------------------------
void BmnMwpcHitFinder::FillFitMatrix(Int_t chN, Double_t** AA, Float_t** z, Double_t* sigm2_, Int_t* h_) {
  // AA - matrix to be filledlayers)
  // sigm2 - square of sigma
  // h_ - array to include/exclude planes (h_[i] = 0 or 1)
  // Float_t z2_[nPlanes];
  Float_t z2_[6] = {z[chN][0] * z[chN][0], z[chN][1] * z[chN][1], z[chN][2] * z[chN][2], z[chN][3] * z[chN][3], z[chN][4] * z[chN][4], z[chN][5] * z[chN][5]}; //cm

  AA[0][0] += 2 * z2_[0] * h_[0] / sigm2_[0] +   z2_[2] * h_[2] / (2 * sigm2_[2])  +   z2_[1] * h_[1] / (2 * sigm2_[1]) + 2 * z2_[3] * h_[3] / sigm2_[3] +   z2_[5] * h_[5] / (2 * sigm2_[5]) +   z2_[4] * h_[4] / (2 * sigm2_[4]); //Ax
  AA[0][1] += 2 * z[chN][0] * h_[0] / sigm2_[0] +   z[chN][2] * h_[2] / (2 * sigm2_[2]) +   z[chN][1] * h_[1] / (2 * sigm2_[1]) + 2 * z[chN][3] * h_[3] / sigm2_[3] +   z[chN][5] * h_[5] / (2 * sigm2_[5]) +   z[chN][4] * h_[4] / (2 * sigm2_[4]); //Bx
  AA[0][2] += sq3 * (z2_[2] * h_[2] / (2 * sigm2_[2]) -     z2_[1] * h_[1] / (2 * sigm2_[1]) +     z2_[5] * h_[5] / (2 * sigm2_[5]) -     z2_[4] * h_[4] / (2 * sigm2_[4])); //Ay
  AA[0][3] += sq3 * (z[chN][2] * h_[2] / (2 * sigm2_[2])  -     z[chN][1] * h_[1] / (2 * sigm2_[1]) +     z[chN][5] * h_[5] / (2 * sigm2_[5]) -     z[chN][4] * h_[4] / (2 * sigm2_[4])); //By
  AA[1][0] = AA[0][1];
  AA[1][1] +=  2 * h_[0] / sigm2_[0] + 0.5 * h_[2] / sigm2_[2] + 0.5 * h_[1] / sigm2_[1] +  2 * h_[3] / sigm2_[3] + 0.5 * h_[5] / sigm2_[5] + 0.5 * h_[4] / sigm2_[4];
  AA[1][2] += sq3 * (z[chN][2] * h_[2] / sigm2_[2] - z[chN][1] * h_[1] / sigm2_[1] + z[chN][5] * h_[5] / sigm2_[5] - z[chN][4] * h_[4] / sigm2_[4]) * 0.5;
  AA[1][3] += sq3 * (h_[2] / sigm2_[2]  - h_[1] / sigm2_[1] +  h_[5] / sigm2_[5] - h_[4] / sigm2_[4]) * 0.5;
  AA[2][0] = AA[0][2];
  AA[2][1] = AA[1][2];
  AA[2][2] += 3.0 * (z2_[2] * h_[2] / sigm2_[2]  +  z2_[1] * h_[1] / sigm2_[1] + z2_[5] * h_[5] / sigm2_[5]  +  z2_[4] * h_[4] / sigm2_[4]) * 0.5;
  AA[2][3] += 3.0 * (z[chN][2] * h_[2] / sigm2_[2] +  z[chN][1] * h_[1] / sigm2_[1]  + z[chN][5] * h_[5] / sigm2_[5] + z[chN][4] * h_[4] / sigm2_[4])  * 0.5;
  AA[3][0] = AA[0][3];
  AA[3][1] = AA[1][3];
  AA[3][2] = AA[2][3];
  AA[3][3] += 3.0 * (0.5 * h_[2] / sigm2_[2] + 0.5 * h_[1] / sigm2_[1]  + 0.5 * h_[5] / sigm2_[5] + 0.5 * h_[4] / sigm2_[4]);
}
//----------------------------------------------------------------------


//--------------------Matrix Free Coefficients Calculation--------------
void BmnMwpcHitFinder::FillFreeCoefVector(Int_t ichNum, Double_t* F, Double_t*** XVU_, Int_t ise, Float_t** z, Double_t* sigmm2, Int_t* h_) {
  // F - vector to be filled
  // XVU_ - coordinates of segment in chamber (Is it correct definition?)
  // segIdx - index of current segment
  // z - local z-positions of planes(layers)
  // sigmm2 - square of sigma
  // h_ - array to include/exclude planes (h_[i] = 0 or 1)

  F[0] += 2 * XVU_[ichNum][0][ise] * z[ichNum][0] * h_[0] / sigmm2[0] + XVU_[ichNum][1][ise] * z[ichNum][1] * h_[1] / sigmm2[1] + XVU_[ichNum][2][ise] * z[ichNum][2] * h_[2] / sigmm2[2] + 2 * XVU_[ichNum][3][ise] * z[ichNum][3] * h_[3] / sigmm2[3] + XVU_[ichNum][4][ise] * z[ichNum][4] * h_[4] / sigmm2[4]
      + XVU_[ichNum][5][ise] * z[ichNum][5] * h_[5] / sigmm2[5];
  F[1] += 2 * XVU_[ichNum][0][ise] * h_[0] / sigmm2[0] + XVU_[ichNum][1][ise] * h_[1] / sigmm2[1] + XVU_[ichNum][2][ise] * h_[2] / sigmm2[2] + 2 * XVU_[ichNum][3][ise] * h_[3] / sigmm2[3] + XVU_[ichNum][4][ise] * h_[4] / sigmm2[4] + XVU_[ichNum][5][ise] * h_[5] / sigmm2[5];
  F[2] += sq3*(-XVU_[ichNum][1][ise] * z[ichNum][1] * h_[1] / sigmm2[1] + XVU_[ichNum][2][ise] * z[ichNum][2] * h_[2] / sigmm2[2] - XVU_[ichNum][4][ise] * z[ichNum][4] * h_[4] / sigmm2[4] + XVU_[ichNum][5][ise] * z[ichNum][5] * h_[5] / sigmm2[5]);
  F[3] += sq3*(-XVU_[ichNum][1][ise] * h_[1] / sigmm2[1] + XVU_[ichNum][2][ise] * h_[2] / sigmm2[2] - XVU_[ichNum][4][ise] * h_[4] / sigmm2[4] + XVU_[ichNum][5][ise] * h_[5] / sigmm2[5]);
}
//----------------------------------------------------------------------


//--------------------Matrix      Coefficients Calculation--------------
void BmnMwpcHitFinder::FillFreeCoefVectorXUV(Int_t ichNum , Double_t* F, Float_t** XVU_, Float_t** z, Float_t* sigm2_, Int_t* h_) {
  // F - vector to be filled
  // XVU_ - coordinates of segment in chamber (Is it correct definition?)
  // segIdx - index of current segment
  // z - local z-positions of planes(layers)
  // sigm2_ - square of sigma
  // h_ - array to include/exclude planes (h_[i] = 0 or 1)
  F[0] += 2 * XVU_[ichNum][0] * z[ichNum][0] * h_[0] / sigm2_[0] + XVU_[ichNum][1]  * z[ichNum][1] * h_[1] / sigm2_[1] + XVU_[ichNum][2] * z[ichNum][2] * h_[2] / sigm2_[2] + 2 * XVU_[ichNum][3] * z[ichNum][3] * h_[3] / sigm2_[3] + XVU_[ichNum][4] * z[ichNum][4] * h_[4] / sigm2_[4] + XVU_[ichNum][5] * z[ichNum][5] * h_[5] / sigm2_[5];
  F[1] += 2 * XVU_[ichNum][0] * h_[0] / sigm2_[0] + XVU_[ichNum][1] * h_[1] / sigm2_[1] + XVU_[ichNum][2] * h_[2] / sigm2_[2] + 2 * XVU_[ichNum][3] * h_[3] / sigm2_[3] + XVU_[ichNum][4] * h_[4] / sigm2_[4] + XVU_[ichNum][5] * h_[5] / sigm2_[5];
  F[2] += (-XVU_[ichNum][1] * z[ichNum][1] * h_[1] / sigm2_[1] + XVU_[ichNum][2] * z[ichNum][2] * h_[2] / sigm2_[2] - XVU_[ichNum][4] * z[ichNum][4] * h_[4] / sigm2_[4] + XVU_[ichNum][5] * z[ichNum][5] * h_[5] / sigm2_[5]);
  F[3] += (-XVU_[ichNum][1] * h_[1] / sigm2_[1] + XVU_[ichNum][2] * h_[2] / sigm2_[2] - XVU_[ichNum][4] * h_[4] / sigm2_[4] + XVU_[ichNum][5] * h_[5] / sigm2_[5]);
  F[2]=F[2]*sq3;
  F[3]=F[3]*sq3;
}
//----------------------------------------------------------------------



//--------------------Matrix      Coefficients Calculation--------------
void BmnMwpcHitFinder::InverseMatrix(Double_t** AA, Double_t** bb) {
  // Gaussian algorithm for 4x4 matrix inversion
  Double_t factor;
  Double_t temp[4];
  // Set b to I
  for (Int_t i1 = 0; i1 < 4; i1++) {
    for (Int_t j1 = 0; j1 < 4; j1++) {
      if (i1 == j1) bb[i1][j1] = 1.0;
      else bb[i1][j1] = 0.0;
    }
  }
  for (Int_t i1 = 0; i1 < 4; i1++) {
    for (Int_t j1 = i1 + 1; j1 < 4; j1++) {
      if (fabs(AA[i1][i1]) < fabs(AA[j1][i1])) {
        for (Int_t l1 = 0; l1 < 4; l1++) temp[l1]   = AA[i1][l1];
        for (Int_t l1 = 0; l1 < 4; l1++) AA[i1][l1] = AA[j1][l1];
        for (Int_t l1 = 0; l1 < 4; l1++) AA[j1][l1] = temp[l1];
        for (Int_t l1 = 0; l1 < 4; l1++) temp[l1]   = bb[i1][l1];
        for (Int_t l1 = 0; l1 < 4; l1++) bb[i1][l1] = bb[j1][l1];
        for (Int_t l1 = 0; l1 < 4; l1++) bb[j1][l1] = temp[l1];
      }
    }
    factor = AA[i1][i1];
    for (Int_t j1 = 4 - 1; j1>-1; j1--) {
      bb[i1][j1] /= factor;
      AA[i1][j1] /= factor;
    }
    for (Int_t j1 = i1 + 1; j1 < 4; j1++) {
      factor = -AA[j1][i1];
      for (Int_t k1 = 0; k1 < 4; k1++) {
        AA[j1][k1] += AA[i1][k1] * factor;
        bb[j1][k1] += bb[i1][k1] * factor;
      }
    }
  } // i1
  for (Int_t i1 = 3; i1 > 0; i1--) {
    for (Int_t j1 = i1 - 1; j1>-1; j1--) {
      factor = -AA[j1][i1];
      for (Int_t k1 = 0; k1 < 4; k1++) {
        AA[j1][k1] += AA[i1][k1] * factor;
        bb[j1][k1] += bb[i1][k1] * factor;
      }
    }
  } // i1
}  //end inverse
//----------------------------------------------------------------------


//----------------------------------------------------------------------
void BmnMwpcHitFinder::Finish() {

  if (fDebug) {
	  
	hEff_mc->Divide(hNum_mc,hDen_mc,1,1);
	  
    printf("MWPC hit finder: write hists to file... ");
    fOutputFileName = Form("hMWPChits_p%d_run%d.root", fRunPeriod, fRunNumber);
    cout<< fOutputFileName <<endl;
    TFile file(fOutputFileName, "RECREATE");
    if(fDoTest) fList.Write();
    file.Close();
  }

  if (fDebug) printf("done\n");
  delete fMwpcGeometrySRC;

  cout << "Work time of the MWPC hit finder: " << workTime << " s" << endl;
}
//----------------------------------------------------------------------

ClassImp(BmnMwpcHitFinder)


