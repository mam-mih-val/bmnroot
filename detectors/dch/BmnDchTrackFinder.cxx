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
#include <cmath>
#include "FairLogger.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TFile.h"
#include "TProfile.h"

#include "BmnDchTrackFinder.h"
#include "FairMCPoint.h"
#include "FitWLSQ.h"
static Double_t workTime = 0.0;

BmnDchTrackFinder::BmnDchTrackFinder(Int_t period, Int_t number, Bool_t isExp)
  : fPeriod(period), fRunId(number), expData(isExp), fhTestFlnm("test.BmnDCHTracking.root")
{
    fEventNo = 0;
    N = (fPeriod == 7 && fRunId > 3589) ? 100 : 15;  //needs to be adjusted according to hit multiplicity
    tracksDch = "BmnDchTrack";
    if(expData){
      InputDigitsBranchName = "DCH";
      fTransferFunctionName = "transfer_func.txt";

      prev_wire = -1;
      prev_time = -1;
    }else{
      InputDigitsBranchName = "BmnDchHit";
    }

    layers_with = 0;
    layers_with2 = 0;

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
    if(fPeriod == 7){

      x1_sh = 0;//10.3;//0;       //11.2;//-8.56;//run6 10.9;// 5.34;//5.14;//10.31; //
      y1_sh = 0;//-3;//0;       //1.76;//5.76;//run6 -1.19;//0;//-1.03;
      x2_sh = -5.99;;//-5.4;//4.7;//8.60; //9.-3.5;//0.0;//run6 5.56;//0;//7.95;// //
      y2_sh = -.24;//-.47;//-3.3;//-12.40;  //1.8;//-7.1;//run6 -2.47;//-1.38;//-3.7;

      x1_slope_sh = 0;//-.070;//0;
      y1_slope_sh = 0;//.060;//0;
      x2_slope_sh = -.005;//-.034;//-.071;//0.;
      y2_slope_sh = .005;//.014;//.061;//0.007;
    }

    if(!expData){//no alignment shifts for MC data

      x1_sh = 0.;
      y1_sh = 0.;
      x2_sh = 0.;
      y2_sh = 0.;

      x1_slope_sh = 0.;
      y1_slope_sh = 0.;
      x2_slope_sh = 0.;
      y2_slope_sh = 0.;
    }
    scale = 0.5;
    // cout<<" x2 sh "<<x2_sh<<endl;
}

void BmnDchTrackFinder::initHists()
{
  //added temporary hists
  hNLayWithFiredWiresDc1 = new TH1F("hNLayWithFiredWiresDc1","layers with fired wires in dc1",9,0,9);
  hNLayWithFiredWiresDc2 = new TH1F("hNLayWithFiredWiresDc2","layers with fired wires in dc2",9,0,9);
  hNoFiredWiresOnLayerDc1 = new TH1F("hNoFiredWiresOnLayerDc1","no fired wires on a layer in dc1",8,0,8);
  hNoFiredWiresOnLayerDc2 = new TH1F("hNoFiredWiresOnLayerDc2","no fired wires on a layer in dc2",8,0,8);
  hNomin1 = new TH1F("hNomin1"," eff nominator dc1", 8,0,8);
  hDenom1 = new TH1F("hDenom1"," eff denominator dc1", 8,0,8); 
  hNomin2 = new TH1F("hNomin2"," eff nominator dc2", 8,0,8);
  hDenom2 = new TH1F("hDenom2"," eff denominator dc2", 8,0,8);

  hEff1 = new TH1F("hEff1"," eff dc1", 8,0,8);
  hEff2 = new TH1F("hEff2"," eff dc2", 8,0,8);
  //    hNomin1->Sumw2();
  //hDenom1->Sumw2();
  //hNomin2->Sumw2();
  //hDenom2->Sumw2();
  hEff1->Sumw2();
  hEff2->Sumw2(); 

  hXDC1_atZ0 = new TH1F("hXDC1_atZ0"," XDC1 extrapol Z = 0", 200,-100,100);
  hYDC1_atZ0 = new TH1F("hYDC1_atZ0"," YDC1 extrapol Z = 0", 200,-100,100);
  hXDC2_atZ0 = new TH1F("hXDC2_atZ0"," XDC2 extrapol Z = 0", 200,-100,100);
  hYDC2_atZ0 = new TH1F("hYDC2_atZ0"," YDC2 extrapol Z = 0", 200,-100,100);

  hSlot_1xa_time8p = new TH1F("hSlot_1xa_time8p", "time8ps_for_plane_DC1_xa", 2000,0,2000);
  hSlot_1xb_time8p = new TH1F("hSlot_1xb_time8p", "time8ps_for_plane_DC1_xb", 2000,0,2000);
  hSlot_1ya_time8p = new TH1F("hSlot_1ya_time8p", "time8ps_for_plane_DC1_ya", 2000,0,2000);
  hSlot_1yb_time8p = new TH1F("hSlot_1yb_time8p", "time8ps_for_plane_DC1_yb", 2000,0,2000);
  hSlot_1ua_time8p = new TH1F("hSlot_1ua_time8p", "time8ps_for_plane_DC1_ua", 2000,0,2000);
  hSlot_1ub_time8p = new TH1F("hSlot_1ub_time8p", "time8ps_for_plane_DC1_ub", 2000,0,2000);
  hSlot_1va_time8p = new TH1F("hSlot_1va_time8p", "time8ps_for_plane_DC1_va", 2000,0,2000);
  hSlot_1vb_time8p = new TH1F("hSlot_1vb_time8p", "time8ps_for_plane_DC1_vb", 2000,0,2000);
  hSlot_2xa_time8p = new TH1F("hSlot_2xa_time8p", "time8ps_for_plane_DC2_xa", 2000,0,2000);
  hSlot_2xb_time8p = new TH1F("hSlot_2xb_time8p", "time8ps_for_plane_DC2_xb", 2000,0,2000);
  hSlot_2ya_time8p = new TH1F("hSlot_2ya_time8p", "time8ps_for_plane_DC2_ya", 2000,0,2000);
  hSlot_2yb_time8p = new TH1F("hSlot_2yb_time8p", "time8ps_for_plane_DC2_yb", 2000,0,2000);
  hSlot_2ua_time8p = new TH1F("hSlot_2ua_time8p", "time8ps_for_plane_DC2_ua", 2000,0,2000);
  hSlot_2ub_time8p = new TH1F("hSlot_2ub_time8p", "time8ps_for_plane_DC2_ub", 2000,0,2000);
  hSlot_2va_time8p = new TH1F("hSlot_2va_time8p", "time8ps_for_plane_DC2_va", 2000,0,2000);
  hSlot_2vb_time8p = new TH1F("hSlot_2vb_time8p", "time8ps_for_plane_DC2_vb", 2000,0,2000);

  hResidx1a= new TH1F("hResidx1a","dc1 xa_mes - xa_from_fit",500,-0.1,0.1);
  hResidu1a= new TH1F("hResidu1a","dc1 ua_mes - ua_from_fit",500,-0.1,0.1);
  hResidy1a= new TH1F("hResidy1a","dc1 ya_mes - ya_from_fit",500,-0.1,0.1);
  hResidv1a= new TH1F("hResidv1a","dc1 va_mes - va_from_fit",500,-0.1,0.1);
  hResidx2a= new TH1F("hResidx2a","dc2 xa_mes - xa_from_fit",500,-0.1,0.1);
  hResidu2a= new TH1F("hResidu2a","dc2 ua_mes - ua_from_fit",500,-0.1,0.1);
  hResidy2a= new TH1F("hResidy2a","dc2 ya_mes - ya_from_fit",500,-0.1,0.1);
  hResidv2a= new TH1F("hResidv2a","dc2 va_mes - va_from_fit",500,-0.1,0.1);

  hResidx1b= new TH1F("hResidx1b","dc1 xb_mes - xb_from_fit",500,-0.1,0.1);
  hResidu1b= new TH1F("hResidu1b","dc1 ub_mes - ub_from_fit",500,-0.1,0.1);
  hResidy1b= new TH1F("hResidy1b","dc1 yb_mes - yb_from_fit",500,-0.1,0.1);
  hResidv1b= new TH1F("hResidv1b","dc1 vb_mes - vb_from_fit",500,-0.1,0.1);
  hResidx2b= new TH1F("hResidx2b","dc2 xb_mes - xb_from_fit",500,-0.1,0.1);
  hResidu2b= new TH1F("hResidu2b","dc2 ub_mes - ub_from_fit",500,-0.1,0.1);
  hResidy2b= new TH1F("hResidy2b","dc2 yb_mes - yb_from_fit",500,-0.1,0.1);
  hResidv2b= new TH1F("hResidv2b","dc2 vb_mes - vb_from_fit",500,-0.1,0.1);

  hResidx1a_0p1= new TH1F("hResidx1a_0p1","dc1 xa_mes - x_from_fit(w/o x in fit) for hit at (0; 0.1) away from wire",500,-0.1,0.1);
  hResidu1a_0p1= new TH1F("hResidu1a_0p1","dc1 ua_mes - u_from_fit(w/o u in fit) for hit at (0; 0.1) away from wire",500,-0.1,0.1);
  hResidy1a_0p1= new TH1F("hResidy1a_0p1","dc1 ya_mes - y_from_fit(w/o y in fit) for hit at (0; 0.1) away from wire",500,-0.1,0.1);
  hResidv1a_0p1= new TH1F("hResidv1a_0p1","dc1 va_mes - v_from_fit(w/o v in fit) for hit at (0; 0.1) away from wire",500,-0.1,0.1);
  hResidx2a_0p1= new TH1F("hResidx2a_0p1","dc2 xa_mes - x_from_fit(w/o x in fit) for hit at (0; 0.1) away from wire",500,-0.1,0.1);
  hResidu2a_0p1= new TH1F("hResidu2a_0p1","dc2 ua_mes - u_from_fit(w/o u in fit) for hit at (0; 0.1) away from wire",500,-0.1,0.1);
  hResidy2a_0p1= new TH1F("hResidy2a_0p1","dc2 ya_mes - y_from_fit(w/o y in fit) for hit at (0; 0.1) away from wire",500,-0.1,0.1);
  hResidv2a_0p1= new TH1F("hResidv2a_0p1","dc2 va_mes - v_from_fit(w/o v in fit) for hit at (0; 0.1) away from wire",500,-0.1,0.1);


  hResidx1a_0p1_0p4= new TH1F("hResidx1a_0p1_0p4","dc1 xa_mes - x_from_fit(w/o x in fit) for hit at (0.1; 0.4) away from wire",500,-0.1,0.1);
  hResidu1a_0p1_0p4= new TH1F("hResidu1a_0p1_0p4","dc1 ua_mes - u_from_fit(w/o u in fit) for hit at (0.1; 0.4) away from wire",500,-0.1,0.1);
  hResidy1a_0p1_0p4= new TH1F("hResidy1a_0p1_0p4","dc1 ya_mes - y_from_fit(w/o y in fit) for hit at (0.1; 0.4) away from wire",500,-0.1,0.1);
  hResidv1a_0p1_0p4= new TH1F("hResidv1a_0p1_0p4","dc1 va_mes - v_from_fit(w/o v in fit) for hit at (0.1; 0.4) away from wire",500,-0.1,0.1);
  hResidx2a_0p1_0p4= new TH1F("hResidx2a_0p1_0p4","dc2 xa_mes - x_from_fit(w/o x in fit) for hit at (0.1; 0.4) away from wire",500,-0.1,0.1);
  hResidu2a_0p1_0p4= new TH1F("hResidu2a_0p1_0p4","dc2 ua_mes - u_from_fit(w/o u in fit) for hit at (0.1; 0.4) away from wire",500,-0.1,0.1);
  hResidy2a_0p1_0p4= new TH1F("hResidy2a_0p1_0p4","dc2 ya_mes - y_from_fit(w/o y in fit) for hit at (0.1; 0.4) away from wire",500,-0.1,0.1);
  hResidv2a_0p1_0p4= new TH1F("hResidv2a_0p1_0p4","dc2 va_mes - v_from_fit(w/o v in fit) for hit at (0.1; 0.4) away from wire",500,-0.1,0.1);

  hResidx1a_0p4_0p5= new TH1F("hResidx1a_0p4_0p5","dc1 xa_mes - x_from_fit(w/o x in fit) for hit at (0.4; 0.5) away from wire",500,-0.1,0.1);
  hResidu1a_0p4_0p5= new TH1F("hResidu1a_0p4_0p5","dc1 ua_mes - u_from_fit(w/o u in fit) for hit at (0.4; 0.5) away from wire",500,-0.1,0.1);
  hResidy1a_0p4_0p5= new TH1F("hResidy1a_0p4_0p5","dc1 ya_mes - y_from_fit(w/o y in fit) for hit at (0.4; 0.5) away from wire",500,-0.1,0.1);
  hResidv1a_0p4_0p5= new TH1F("hResidv1a_0p4_0p5","dc1 va_mes - v_from_fit(w/o v in fit) for hit at (0.4; 0.5) away from wire",500,-0.1,0.1);
  hResidx2a_0p4_0p5= new TH1F("hResidx2a_0p4_0p5","dc2 xa_mes - x_from_fit(w/o x in fit) for hit at (0.4; 0.5) away from wire",500,-0.1,0.1);
  hResidu2a_0p4_0p5= new TH1F("hResidu2a_0p4_0p5","dc2 ua_mes - u_from_fit(w/o u in fit) for hit at (0.4; 0.5) away from wire",500,-0.1,0.1);
  hResidy2a_0p4_0p5= new TH1F("hResidy2a_0p4_0p5","dc2 ya_mes - y_from_fit(w/o y in fit) for hit at (0.4; 0.5) away from wire",500,-0.1,0.1);
  hResidv2a_0p4_0p5= new TH1F("hResidv2a_0p4_0p5","dc2 va_mes - v_from_fit(w/o v in fit) for hit at (0.4; 0.5) away from wire",500,-0.1,0.1);

  hResidx1b_0p1= new TH1F("hResidx1b_0p1","dc1 xb_mes - x_from_fit(w/o x in fit) for hit at (0; 0.1) away from wire",500,-0.1,0.1);
  hResidu1b_0p1= new TH1F("hResidu1b_0p1","dc1 ub_mes - u_from_fit(w/o u in fit) for hit at (0; 0.1) away from wire",500,-0.1,0.1);
  hResidy1b_0p1= new TH1F("hResidy1b_0p1","dc1 yb_mes - y_from_fit(w/o y in fit) for hit at (0; 0.1) away from wire",500,-0.1,0.1);
  hResidv1b_0p1= new TH1F("hResidv1b_0p1","dc1 vb_mes - v_from_fit(w/o v in fit) for hit at (0; 0.1) away from wire",500,-0.1,0.1);
  hResidx2b_0p1= new TH1F("hResidx2b_0p1","dc2 xb_mes - x_from_fit(w/o x in fit) for hit at (0; 0.1) away from wire",500,-0.1,0.1);
  hResidu2b_0p1= new TH1F("hResidu2b_0p1","dc2 ub_mes - u_from_fit(w/o u in fit) for hit at (0; 0.1) away from wire",500,-0.1,0.1);
  hResidy2b_0p1= new TH1F("hResidy2b_0p1","dc2 yb_mes - y_from_fit(w/o y in fit) for hit at (0; 0.1) away from wire",500,-0.1,0.1);
  hResidv2b_0p1= new TH1F("hResidv2b_0p1","dc2 vb_mes - v_from_fit(w/o v in fit) for hit at (0; 0.1) away from wire",500,-0.1,0.1);

  hResidx1b_0p1_0p4= new TH1F("hResidx1b_0p1_0p4","dc1 xb_mes - x_from_fit(w/o x in fit) for hit at (0.1; 0.4) away from wire",500,-0.1,0.1);
  hResidu1b_0p1_0p4= new TH1F("hResidu1b_0p1_0p4","dc1 ub_mes - u_from_fit(w/o u in fit) for hit at (0.1; 0.4) away from wire",500,-0.1,0.1);
  hResidy1b_0p1_0p4= new TH1F("hResidy1b_0p1_0p4","dc1 yb_mes - y_from_fit(w/o y in fit) for hit at (0.1; 0.4) away from wire",500,-0.1,0.1);
  hResidv1b_0p1_0p4= new TH1F("hResidv1b_0p1_0p4","dc1 vb_mes - v_from_fit(w/o v in fit) for hit at (0.1; 0.4) away from wire",500,-0.1,0.1);
  hResidx2b_0p1_0p4= new TH1F("hResidx2b_0p1_0p4","dc2 xb_mes - x_from_fit(w/o x in fit) for hit at (0.1; 0.4) away from wire",500,-0.1,0.1);
  hResidu2b_0p1_0p4= new TH1F("hResidu2b_0p1_0p4","dc2 ub_mes - u_from_fit(w/o u in fit) for hit at (0.1; 0.4) away from wire",500,-0.1,0.1);
  hResidy2b_0p1_0p4= new TH1F("hResidy2b_0p1_0p4","dc2 yb_mes - y_from_fit(w/o y in fit) for hit at (0.1; 0.4) away from wire",500,-0.1,0.1);
  hResidv2b_0p1_0p4= new TH1F("hResidv2b_0p1_0p4","dc2 vb_mes - v_from_fit(w/o v in fit) for hit at (0.1; 0.4) away from wire",500,-0.1,0.1);

  hResidx1b_0p4_0p5= new TH1F("hResidx1b_0p4_0p5","dc1 xb_mes - x_from_fit(w/o x in fit) for hit at (0.4; 0.5) away from wire",500,-0.1,0.1);
  hResidu1b_0p4_0p5= new TH1F("hResidu1b_0p4_0p5","dc1 ub_mes - u_from_fit(w/o u in fit) for hit at (0.4; 0.5) away from wire",500,-0.1,0.1);
  hResidy1b_0p4_0p5= new TH1F("hResidy1b_0p4_0p5","dc1 yb_mes - y_from_fit(w/o y in fit) for hit at (0.4; 0.5) away from wire",500,-0.1,0.1);
  hResidv1b_0p4_0p5= new TH1F("hResidv1b_0p4_0p5","dc1 vb_mes - v_from_fit(w/o v in fit) for hit at (0.4; 0.5) away from wire",500,-0.1,0.1);
  hResidx2b_0p4_0p5= new TH1F("hResidx2b_0p4_0p5","dc2 xb_mes - x_from_fit(w/o x in fit) for hit at (0.4; 0.5) away from wire",500,-0.1,0.1);
  hResidu2b_0p4_0p5= new TH1F("hResidu2b_0p4_0p5","dc2 ub_mes - u_from_fit(w/o u in fit) for hit at (0.4; 0.5) away from wire",500,-0.1,0.1);
  hResidy2b_0p4_0p5= new TH1F("hResidy2b_0p4_0p5","dc2 yb_mes - y_from_fit(w/o y in fit) for hit at (0.4; 0.5) away from wire",500,-0.1,0.1);
  hResidv2b_0p4_0p5= new TH1F("hResidv2b_0p4_0p5","dc2 vb_mes - v_from_fit(w/o v in fit) for hit at (0.4; 0.5) away from wire",500,-0.1,0.1);


  //pulls

  hPullsx1a_0p1= new TH1F("hPullsx1a_0p1","dc1 xa pull (0 - 0.1) away from wire",100,-3,3);
  hPullsu1a_0p1= new TH1F("hPullsu1a_0p1","dc1 ua pull (0 - 0.1) away from wire",100,-3,3);
  hPullsy1a_0p1= new TH1F("hPullsy1a_0p1","dc1 ya pull (0 - 0.1) away from wire",100,-3,3);
  hPullsv1a_0p1= new TH1F("hPullsv1a_0p1","dc1 va pull (0 - 0.1) away from wire",100,-3,3);
  hPullsx2a_0p1= new TH1F("hPullsx2a_0p1","dc2 xa pull (0 - 0.1) away from wire",100,-3,3);
  hPullsu2a_0p1= new TH1F("hPullsu2a_0p1","dc2 ua pull (0 - 0.1) away from wire",100,-3,3);
  hPullsy2a_0p1= new TH1F("hPullsy2a_0p1","dc2 ya pull (0 - 0.1) away from wire",100,-3,3);
  hPullsv2a_0p1= new TH1F("hPullsv2a_0p1","dc2 va pull (0 - 0.1) away from wire",100,-3,3);


  hPullsx1a_0p1_0p4= new TH1F("hPullsx1a_0p1_0p4","dc1 xa pull (0.1 - 0.4) away from wire",100,-3,3);
  hPullsu1a_0p1_0p4= new TH1F("hPullsu1a_0p1_0p4","dc1 ua pull (0.1 - 0.4) away from wire",100,-3,3);
  hPullsy1a_0p1_0p4= new TH1F("hPullsy1a_0p1_0p4","dc1 ya pull (0.1 - 0.4) away from wire",100,-3,3);
  hPullsv1a_0p1_0p4= new TH1F("hPullsv1a_0p1_0p4","dc1 va pull (0.1 - 0.4) away from wire",100,-3,3);
  hPullsx2a_0p1_0p4= new TH1F("hPullsx2a_0p1_0p4","dc2 xa pull (0.1 - 0.4) away from wire",100,-3,3);
  hPullsu2a_0p1_0p4= new TH1F("hPullsu2a_0p1_0p4","dc2 ua pull (0.1 - 0.4) away from wire",100,-3,3);
  hPullsy2a_0p1_0p4= new TH1F("hPullsy2a_0p1_0p4","dc2 ya pull (0.1 - 0.4) away from wire",100,-3,3);
  hPullsv2a_0p1_0p4= new TH1F("hPullsv2a_0p1_0p4","dc2 va pull (0.1 - 0.4) away from wire",100,-3,3);

  hPullsx1a_0p4_0p5= new TH1F("hPullsx1a_0p4_0p5","dc1 xa pull (0.4 - 0.5) away from wire",100,-3,3);
  hPullsu1a_0p4_0p5= new TH1F("hPullsu1a_0p4_0p5","dc1 ua pull (0.4 - 0.5) away from wire",100,-3,3);
  hPullsy1a_0p4_0p5= new TH1F("hPullsy1a_0p4_0p5","dc1 ya pull (0.4 - 0.5) away from wire",100,-3,3);
  hPullsv1a_0p4_0p5= new TH1F("hPullsv1a_0p4_0p5","dc1 va pull (0.4 - 0.5) away from wire",100,-3,3);
  hPullsx2a_0p4_0p5= new TH1F("hPullsx2a_0p4_0p5","dc2 xa pull (0.4 - 0.5) away from wire",100,-3,3);
  hPullsu2a_0p4_0p5= new TH1F("hPullsu2a_0p4_0p5","dc2 ua pull (0.4 - 0.5) away from wire",100,-3,3);
  hPullsy2a_0p4_0p5= new TH1F("hPullsy2a_0p4_0p5","dc2 ya pull (0.4 - 0.5) away from wire",100,-3,3);
  hPullsv2a_0p4_0p5= new TH1F("hPullsv2a_0p4_0p5","dc2 va pull (0.4 - 0.5) away from wire",100,-3,3);

  hPullsx1b_0p1= new TH1F("hPullsx1b_0p1","dc1 xb pull (0 - 0.1) away from wire",100,-3,3);
  hPullsu1b_0p1= new TH1F("hPullsu1b_0p1","dc1 ub pull (0 - 0.1) away from wire",100,-3,3);
  hPullsy1b_0p1= new TH1F("hPullsy1b_0p1","dc1 yb pull (0 - 0.1) away from wire",100,-3,3);
  hPullsv1b_0p1= new TH1F("hPullsv1b_0p1","dc1 vb pull (0 - 0.1) away from wire",100,-3,3);
  hPullsx2b_0p1= new TH1F("hPullsx2b_0p1","dc2 xb pull (0 - 0.1) away from wire",100,-3,3);
  hPullsu2b_0p1= new TH1F("hPullsu2b_0p1","dc2 ub pull (0 - 0.1) away from wire",100,-3,3);
  hPullsy2b_0p1= new TH1F("hPullsy2b_0p1","dc2 yb pull (0 - 0.1) away from wire",100,-3,3);
  hPullsv2b_0p1= new TH1F("hPullsv2b_0p1","dc2 vb pull (0 - 0.1) away from wire",100,-3,3);

  hPullsx1b_0p1_0p4= new TH1F("hPullsx1b_0p1_0p4","dc1 xb pull (0.1 - 0.4) away from wire",100,-3,3);
  hPullsu1b_0p1_0p4= new TH1F("hPullsu1b_0p1_0p4","dc1 ub pull (0.1 - 0.4) away from wire",100,-3,3);
  hPullsy1b_0p1_0p4= new TH1F("hPullsy1b_0p1_0p4","dc1 yb pull (0.1 - 0.4) away from wire",100,-3,3);
  hPullsv1b_0p1_0p4= new TH1F("hPullsv1b_0p1_0p4","dc1 vb pull (0.1 - 0.4) away from wire",100,-3,3);
  hPullsx2b_0p1_0p4= new TH1F("hPullsx2b_0p1_0p4","dc2 xb pull (0.1 - 0.4) away from wire",100,-3,3);
  hPullsu2b_0p1_0p4= new TH1F("hPullsu2b_0p1_0p4","dc2 ub pull (0.1 - 0.4) away from wire",100,-3,3);
  hPullsy2b_0p1_0p4= new TH1F("hPullsy2b_0p1_0p4","dc2 yb pull (0.1 - 0.4) away from wire",100,-3,3);
  hPullsv2b_0p1_0p4= new TH1F("hPullsv2b_0p1_0p4","dc2 vb pull (0.1 - 0.4) away from wire",100,-3,3);

  hPullsx1b_0p4_0p5= new TH1F("hPullsx1b_0p4_0p5","dc1 xb pull (0.4 - 0.5) away from wire",100,-3,3);
  hPullsu1b_0p4_0p5= new TH1F("hPullsu1b_0p4_0p5","dc1 ub pull (0.4 - 0.5) away from wire",100,-3,3);
  hPullsy1b_0p4_0p5= new TH1F("hPullsy1b_0p4_0p5","dc1 yb pull (0.4 - 0.5) away from wire",100,-3,3);
  hPullsv1b_0p4_0p5= new TH1F("hPullsv1b_0p4_0p5","dc1 vb pull (0.4 - 0.5) away from wire",100,-3,3);
  hPullsx2b_0p4_0p5= new TH1F("hPullsx2b_0p4_0p5","dc2 xb pull (0.4 - 0.5) away from wire",100,-3,3);
  hPullsu2b_0p4_0p5= new TH1F("hPullsu2b_0p4_0p5","dc2 ub pull (0.4 - 0.5) away from wire",100,-3,3);
  hPullsy2b_0p4_0p5= new TH1F("hPullsy2b_0p4_0p5","dc2 yb pull (0.4 - 0.5) away from wire",100,-3,3);
  hPullsv2b_0p4_0p5= new TH1F("hPullsv2b_0p4_0p5","dc2 vb pull (0.4 - 0.5) away from wire",100,-3,3);

  hSlot_1xa_time = new TH1F("hSlot_1xa_time", "times_for_plane_DC1_xa", 2000, 0, 2000);
  hSlot_1xb_time = new TH1F("hSlot_1xb_time", "times_for_plane_DC1_xb", 2000, 0, 2000);
  Ntrack1 = new TH1D("Ntrack1", "N segs in dch1", 20, 0, 20);
  Ntrack2 = new TH1D("Ntrack2", "N segs in dch2", 20, 0, 20);
  NGtracks = new TH1D("NGtracks", "N global dch tracks per event", 10, 0, 10);
  hXa_wireOccupancy = new TH1F("hXa_wireOccupancy", " xa wire nr ", 250, 0, 250);
  hXb_wireOccupancy = new TH1F("hXb_wireOccupancy", " xb wire nr ", 250, 0, 250);
  hYa_wireOccupancy = new TH1F("hYa_wireOccupancy", " ya wire nr ", 250, 0, 250);
  hYb_wireOccupancy = new TH1F("hYb_wireOccupancy", " yb wire nr ", 250, 0, 250);
  hUa_wireOccupancy = new TH1F("hUa_wireOccupancy", " ua wire nr ", 250, 0, 250);
  hUb_wireOccupancy = new TH1F("hUb_wireOccupancy", " ub wire nr ", 250, 0, 250);
  hVa_wireOccupancy = new TH1F("hVa_wireOccupancy", " va wire nr ", 250, 0, 250);
  hVb_wireOccupancy = new TH1F("hVb_wireOccupancy", " vb wire nr ", 250, 0, 250);
  hXa2_wireOccupancy = new TH1F("hXa2_wireOccupancy", " xa2 wire nr ", 250, 0, 250);
  hXb2_wireOccupancy = new TH1F("hXb2_wireOccupancy", " xb2 wire nr ", 250, 0, 250);
  hYa2_wireOccupancy = new TH1F("hYa2_wireOccupancy", " ya2 wire nr ", 250, 0, 250);
  hYb2_wireOccupancy = new TH1F("hYb2_wireOccupancy", " yb2 wire nr ", 250, 0, 250);
  hUa2_wireOccupancy = new TH1F("hUa2_wireOccupancy", " ua2 wire nr ", 250, 0, 250);
  hUb2_wireOccupancy = new TH1F("hUb2_wireOccupancy", " ub2 wire nr ", 250, 0, 250);
  hVa2_wireOccupancy = new TH1F("hVa2_wireOccupancy", " va2 wire nr ", 250, 0, 250);
  hVb2_wireOccupancy = new TH1F("hVb2_wireOccupancy", " vb2 wire nr ", 250, 0, 250);
  haX2_aX1 = new TH1F("haX2_aX1", "aX2 - aX1", 200, -0.25, 0.25);
  haY2_aY1 = new TH1F("haY2_aY1", "aY2 - aY1", 200, -0.25, 0.25);
  hX1_X2_glob_all = new TH1F("hX1_X2_glob_all", " Xdc2 - Xdc1 extrapol Zmid, all closest", 300, -30, 30);
  hY1_Y2_glob_all = new TH1F("hY1_Y2_glob_all", " Ydc2 - Ydc1 extrapol Zmid, all closest", 300, -30, 30);
  haX2_aX1m = new TH1F("haX2_aX1m", "aX2 - aX1m", 200, -0.1, 0.1);
  haY2_aY1m = new TH1F("haY2_aY1m", "aY2 - aY1m", 200, -0.1, 0.1);
  hX1_X2_glob_m = new TH1F("hX1_X2_glob_m", " Xdc2 - Xdc1 extrapol Zmid, m closest", 300, -15, 15);
  hY1_Y2_glob_m = new TH1F("hY1_Y2_glob_m", " Ydc2 - Ydc1 extrapol Zmid, m closest", 300, -15, 15);
  hx1Da_Db = new TH1F("hx1Da_Db", "d_a + d_b - 5mm dc1", 200, -0.5, 0.5);
  hx2Da_Db = new TH1F("hx2Da_Db", "d_a + d_b - 5mm dc2", 200, -0.5, 0.5);
  hy1Da_Db = new TH1F("hy1Da_Db", "d_a + d_b - 5mm dc1", 200, -0.5, 0.5);
  hy2Da_Db = new TH1F("hy2Da_Db", "d_a + d_b - 5mm dc2", 200, -0.5, 0.5);
  hu1Da_Db = new TH1F("hu1Da_Db", "d_a + d_b - 5mm dc1", 200, -0.5, 0.5);
  hu2Da_Db = new TH1F("hu2Da_Db", "d_a + d_b - 5mm dc2", 200, -0.5, 0.5);
  hv1Da_Db = new TH1F("hv1Da_Db", "d_a + d_b - 5mm dc1", 200, -0.5, 0.5);
  hv2Da_Db = new TH1F("hv2Da_Db", "d_a + d_b - 5mm dc2", 200, -0.5, 0.5);
  hx1Da_Db_best = new TH1F("hx1Da_Db_best", "d_a + d_b - 5mm for best dc1", 200, -0.5, 0.5);
  hx2Da_Db_best = new TH1F("hx2Da_Db_best", "d_a + d_b - 5mm for best dc2", 200, -0.5, 0.5);
  hy1Da_Db_best = new TH1F("hy1Da_Db_best", "d_a + d_b - 5mm for best dc1", 200, -0.5, 0.5);
  hy2Da_Db_best = new TH1F("hy2Da_Db_best", "d_a + d_b - 5mm for best dc2", 200, -0.5, 0.5);
  hu1Da_Db_best = new TH1F("hu1Da_Db_best", "d_a + d_b - 5mm for best dc1", 200, -0.5, 0.5);
  hu2Da_Db_best = new TH1F("hu2Da_Db_best", "d_a + d_b - 5mm for best dc2", 200, -0.5, 0.5);
  hv1Da_Db_best = new TH1F("hv1Da_Db_best", "d_a + d_b - 5mm for best dc1", 200, -0.5, 0.5);
  hv2Da_Db_best = new TH1F("hv2Da_Db_best", "d_a + d_b - 5mm for best dc2", 200, -0.5, 0.5);
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
  haX = new TH1F("haX", "aX", 300, -0.05, 0.15);
  haY = new TH1F("haY", "aY", 200, -0.1, 0.1);
  hlocXY1 = new TH2F("hlocXY1", " (X,Y) local coord of a seg in dc1", 240, -120, 120, 240, -120, 120);
  hlocXY2 = new TH2F("hlocXY2", " (X,Y) local coord of a seg in dc2", 240, -120, 120, 240, -120, 120);
  hXvsAx = new TH2F("hXvsAx", " X coord of a seg in dc1 vs x slope", 240, -120, 120, 100, -10, 10);

  hAx1_loc = new TH1F("hAx1_loc","slope x dc1 in loc coord system",400, -0.2, 0.2);
  hAy1_loc = new TH1F("hAy1_loc","slope y dc1 in loc coord system",400, -0.2, 0.2);
  hAu1_loc = new TH1F("hAu1_loc","slope u dc1 in loc coord system",400, -0.2, 0.2);
  hAv1_loc = new TH1F("hAv1_loc","slope v dc1 in loc coord system",400, -0.2, 0.2);

  hAx12_loc = new TH1F("hAx12_loc","slope x dc1 in loc coord system",200, -0.2, 0.2);
  hAy12_loc = new TH1F("hAy12_loc","slope y dc1 in loc coord system",200, -0.2, 0.2);
  hAu12_loc = new TH1F("hAu12_loc","slope u dc1 in loc coord system",200, -0.2, 0.2);
  hAv12_loc = new TH1F("hAv12_loc","slope v dc1 in loc coord system",200, -0.2, 0.2);
    
  hAx2_loc = new TH1F("hAx2_loc","slope x dc2 in loc coord system",400, -0.2, 0.2);
  hAy2_loc = new TH1F("hAy2_loc","slope y dc2 in loc coord system",400, -0.2, 0.2);
  hAu2_loc = new TH1F("hAu2_loc","slope u dc2 in loc coord system",400, -0.2, 0.2);
  hAv2_loc = new TH1F("hAv2_loc","slope v dc2 in loc coord system",400, -0.2, 0.2);

  hAx122loc = new TH1F("hAx122loc","slope x dc2 in loc coord system",200, -0.2, 0.2);
  hAy122loc = new TH1F("hAy122loc","slope y dc2 in loc coord system",200, -0.2, 0.2);
  hAu122loc = new TH1F("hAu122loc","slope u dc2 in loc coord system",200, -0.2, 0.2);
  hAv122loc = new TH1F("hAv122loc","slope v dc2 in loc coord system",200, -0.2, 0.2);

  hXm_Xe_loc = new TH1F("hXm_Xe_loc"," x_mes - x_est(u,v)", 100, -1.5, 1.5);
  hYm_Ye_loc = new TH1F("hYm_Ye_loc"," y_mes - y_est(u,v)", 100, -1.5, 1.5);
  hUm_Ue_loc = new TH1F("hUm_Ue_loc"," u_mes - u_est(x,y)", 100, -1.5, 1.5);
  hVm_Ve_loc = new TH1F("hVm_Ve_loc"," v_mes - v_est(x,y)", 100, -1.5, 1.5);

  hDc1XaResVsDa = new TProfile("hDc1XaResVsDa"," res x1a vs da", 100, 0, .5, 0, 5);
  hDc1XbResVsDb = new TProfile("hDc1XbResVsDb"," res x1b vs db", 100, 0, .5, 0, 5);
  hDc1XaResVsDa_p = new TProfile("hDc1XaResVsDa_p"," res x1a vs da +", 100, 0, .5, 0, 5);
  hDc1XbResVsDb_p = new TProfile("hDc1XbResVsDb_p"," res x1b vs db +", 100, 0, .5, 0, 5);
  hDc1XaResVsDa_m = new TProfile("hDc1XaResVsDa_m"," res x1a vs da -", 100, 0, .5, 0, 5);
  hDc1XbResVsDb_m = new TProfile("hDc1XbResVsDb_m"," res x1b vs db -", 100, 0, .5, 0, 5);
    
  hUvsXV1 = new TH1F("hUvsXV1","u1 diff", 100, -2,2);
  hYvsXV1 = new TH1F("hYvsXV1","y1 diff", 100, -2,2);
  hUvsXV2 = new TH1F("hUvsXV2","u2 diff", 100, -2,2);
  hYvsXV2 = new TH1F("hYvsXV2","y2 diff", 100, -2,2);
  hChi2ndf_dc1 = new TH1F("hChi2ndf_dc1","chi2/nDoF for dc1 segments", 300,0,300);
  hChi2ndf_dc2 = new TH1F("hChi2ndf_dc2","chi2/nDoF for dc2 segments", 300,0,300);
  hSegSize_dc1 = new TH1F("hSegSize_dc1"," segment size for dc1",9,0,9);
  hSegSize_dc2 = new TH1F("hSegSize_dc2"," segment size for dc2",9,0,9);
  hChi2Matched  = new TH1F("hChi2Matched"," chi2 for matched pair",2000,0,2000);
  hP = new TH1F("hP"," momentum = .3*Int(BL)/[sin(alphaX_out-alphaX_in)+C]", 200,5,15);
 
  fhList.Add(hNLayWithFiredWiresDc1);
  fhList.Add(hNLayWithFiredWiresDc2);
  fhList.Add(hNoFiredWiresOnLayerDc1);
  fhList.Add(hNoFiredWiresOnLayerDc2); 

  fhList.Add(hNomin1);
  fhList.Add(hDenom1);
  fhList.Add(hNomin2);
  fhList.Add(hDenom2);
    
  fhList.Add(hEff1);
  fhList.Add(hEff2);
  fhList.Add(hSlot_1xa_time8p);
  fhList.Add(hSlot_1xb_time8p);
  fhList.Add(hSlot_1ya_time8p);
  fhList.Add(hSlot_1yb_time8p);
  fhList.Add(hSlot_1ua_time8p);
  fhList.Add(hSlot_1ub_time8p);
  fhList.Add(hSlot_1va_time8p);
  fhList.Add(hSlot_1vb_time8p);
  fhList.Add(hSlot_2xa_time8p);
  fhList.Add(hSlot_2xb_time8p);
  fhList.Add(hSlot_2ya_time8p);
  fhList.Add(hSlot_2yb_time8p);
  fhList.Add(hSlot_2ua_time8p);
  fhList.Add(hSlot_2ub_time8p);
  fhList.Add(hSlot_2va_time8p);
  fhList.Add(hSlot_2vb_time8p);

  fhList.Add(hUvsXV1);
  fhList.Add(hYvsXV1);
  fhList.Add(hUvsXV2);
  fhList.Add(hYvsXV2);
  fhList.Add(hDc1XaResVsDa);
  fhList.Add(hDc1XbResVsDb);

  fhList.Add(hDc1XaResVsDa_m);
  fhList.Add(hDc1XbResVsDb_m);
  fhList.Add(hDc1XaResVsDa_p);
  fhList.Add(hDc1XbResVsDb_p);


  fhList.Add(hXm_Xe_loc);
  fhList.Add(hYm_Ye_loc);
  fhList.Add(hUm_Ue_loc);
  fhList.Add(hVm_Ve_loc);
    
  fhList.Add(hAx1_loc);
  fhList.Add(hAy1_loc);
  fhList.Add(hAu1_loc);
  fhList.Add(hAv1_loc);
  fhList.Add(hAx12_loc);
  fhList.Add(hAy12_loc);
  fhList.Add(hAu12_loc);
  fhList.Add(hAv12_loc);
  fhList.Add(hAx2_loc);
  fhList.Add(hAy2_loc);
  fhList.Add(hAu2_loc);
  fhList.Add(hAv2_loc);
  fhList.Add(hAx122loc);
  fhList.Add(hAy122loc);
  fhList.Add(hAu122loc);
  fhList.Add(hAv122loc);
  fhList.Add(hResidx1a);
  fhList.Add(hResidy1a);
  fhList.Add(hResidu1a);
  fhList.Add(hResidv1a);
  fhList.Add(hResidx2a);
  fhList.Add(hResidy2a);
  fhList.Add(hResidu2a);
  fhList.Add(hResidv2a);

  fhList.Add(hResidx1b);
  fhList.Add(hResidy1b);
  fhList.Add(hResidu1b);
  fhList.Add(hResidv1b);
  fhList.Add(hResidx2b);
  fhList.Add(hResidy2b);
  fhList.Add(hResidu2b);
  fhList.Add(hResidv2b);


  fhList.Add(hResidx1a_0p1);
  fhList.Add(hResidy1a_0p1);
  fhList.Add(hResidu1a_0p1);
  fhList.Add(hResidv1a_0p1);
  fhList.Add(hResidx2a_0p1);
  fhList.Add(hResidy2a_0p1);
  fhList.Add(hResidu2a_0p1);
  fhList.Add(hResidv2a_0p1);

  fhList.Add(hResidx1a_0p1_0p4);
  fhList.Add(hResidy1a_0p1_0p4);
  fhList.Add(hResidu1a_0p1_0p4);
  fhList.Add(hResidv1a_0p1_0p4);
  fhList.Add(hResidx2a_0p1_0p4);
  fhList.Add(hResidy2a_0p1_0p4);
  fhList.Add(hResidu2a_0p1_0p4);
  fhList.Add(hResidv2a_0p1_0p4);

  fhList.Add(hResidx1a_0p4_0p5);
  fhList.Add(hResidy1a_0p4_0p5);
  fhList.Add(hResidu1a_0p4_0p5);
  fhList.Add(hResidv1a_0p4_0p5);
  fhList.Add(hResidx2a_0p4_0p5);
  fhList.Add(hResidy2a_0p4_0p5);
  fhList.Add(hResidu2a_0p4_0p5);
  fhList.Add(hResidv2a_0p4_0p5);  

  fhList.Add(hResidx1b_0p1);
  fhList.Add(hResidy1b_0p1);
  fhList.Add(hResidu1b_0p1);
  fhList.Add(hResidv1b_0p1);
  fhList.Add(hResidx2b_0p1);
  fhList.Add(hResidy2b_0p1);
  fhList.Add(hResidu2b_0p1);
  fhList.Add(hResidv2b_0p1);

  fhList.Add(hResidx1b_0p1_0p4);
  fhList.Add(hResidy1b_0p1_0p4);
  fhList.Add(hResidu1b_0p1_0p4);
  fhList.Add(hResidv1b_0p1_0p4);
  fhList.Add(hResidx2b_0p1_0p4);
  fhList.Add(hResidy2b_0p1_0p4);
  fhList.Add(hResidu2b_0p1_0p4);
  fhList.Add(hResidv2b_0p1_0p4);

  fhList.Add(hResidx1b_0p4_0p5);
  fhList.Add(hResidy1b_0p4_0p5);
  fhList.Add(hResidu1b_0p4_0p5);
  fhList.Add(hResidv1b_0p4_0p5);
  fhList.Add(hResidx2b_0p4_0p5);
  fhList.Add(hResidy2b_0p4_0p5);
  fhList.Add(hResidu2b_0p4_0p5);
  fhList.Add(hResidv2b_0p4_0p5);  


  //puls
  fhList.Add(hP);
  fhList.Add(hPullsx1a_0p1);
  fhList.Add(hPullsy1a_0p1);
  fhList.Add(hPullsu1a_0p1);
  fhList.Add(hPullsv1a_0p1);
  fhList.Add(hPullsx2a_0p1);
  fhList.Add(hPullsy2a_0p1);
  fhList.Add(hPullsu2a_0p1);
  fhList.Add(hPullsv2a_0p1);

  fhList.Add(hPullsx1a_0p1_0p4);
  fhList.Add(hPullsy1a_0p1_0p4);
  fhList.Add(hPullsu1a_0p1_0p4);
  fhList.Add(hPullsv1a_0p1_0p4);
  fhList.Add(hPullsx2a_0p1_0p4);
  fhList.Add(hPullsy2a_0p1_0p4);
  fhList.Add(hPullsu2a_0p1_0p4);
  fhList.Add(hPullsv2a_0p1_0p4);

  fhList.Add(hPullsx1a_0p4_0p5);
  fhList.Add(hPullsy1a_0p4_0p5);
  fhList.Add(hPullsu1a_0p4_0p5);
  fhList.Add(hPullsv1a_0p4_0p5);
  fhList.Add(hPullsx2a_0p4_0p5);
  fhList.Add(hPullsy2a_0p4_0p5);
  fhList.Add(hPullsu2a_0p4_0p5);
  fhList.Add(hPullsv2a_0p4_0p5);  

  fhList.Add(hPullsx1b_0p1);  fEventNo = 0;
  N = (fPeriod == 7 && fRunId > 3589) ? 100 : 15;  //needs to be adjusted according to hit multiplicity
  tracksDch = "BmnDchTrack";
  if(expData){
    InputDigitsBranchName = "DCH";
    fTransferFunctionName = "transfer_func.txt";

    prev_wire = -1;
    prev_time = -1;
  }else{
    InputDigitsBranchName = "BmnDchHit";
  }

  layers_with = 0;
  layers_with2 = 0;

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
  if(fPeriod == 7){

    x1_sh = 0;//10.3;//0;       //11.2;//-8.56;//run6 10.9;// 5.34;//5.14;//10.31; //
    y1_sh = 0;//-3;//0;       //1.76;//5.76;//run6 -1.19;//0;//-1.03;
    x2_sh = -5.99;;//-5.4;//4.7;//8.60; //9.-3.5;//0.0;//run6 5.56;//0;//7.95;// //
    y2_sh = -.24;//-.47;//-3.3;//-12.40;  //1.8;//-7.1;//run6 -2.47;//-1.38;//-3.7;

    x1_slope_sh = 0;//-.070;//0;
    y1_slope_sh = 0;//.060;//0;
    x2_slope_sh = -.005;//-.034;//-.071;//0.;
    y2_slope_sh = .005;//.014;//.061;//0.007;
  }

  if(!expData){//no alignment shifts for MC data

    x1_sh = 0.;
    y1_sh = 0.;
    x2_sh = 0.;
    y2_sh = 0.;

    x1_slope_sh = 0.;
    y1_slope_sh = 0.;
    x2_slope_sh = 0.;
    y2_slope_sh = 0.;
  }
  scale = 0.5;
  // cout<<" x2 sh "<<x2_sh<<endl;
  fhList.Add(hPullsy1b_0p1);
  fhList.Add(hPullsu1b_0p1);
  fhList.Add(hPullsv1b_0p1);
  fhList.Add(hPullsx2b_0p1);
  fhList.Add(hPullsy2b_0p1);
  fhList.Add(hPullsu2b_0p1);
  fhList.Add(hPullsv2b_0p1);

  fhList.Add(hPullsx1b_0p1_0p4);
  fhList.Add(hPullsy1b_0p1_0p4);
  fhList.Add(hPullsu1b_0p1_0p4);
  fhList.Add(hPullsv1b_0p1_0p4);
  fhList.Add(hPullsx2b_0p1_0p4);
  fhList.Add(hPullsy2b_0p1_0p4);
  fhList.Add(hPullsu2b_0p1_0p4);
  fhList.Add(hPullsv2b_0p1_0p4);

  fhList.Add(hPullsx1b_0p4_0p5);
  fhList.Add(hPullsy1b_0p4_0p5);
  fhList.Add(hPullsu1b_0p4_0p5);
  fhList.Add(hPullsv1b_0p4_0p5);
  fhList.Add(hPullsx2b_0p4_0p5);
  fhList.Add(hPullsy2b_0p4_0p5);
  fhList.Add(hPullsu2b_0p4_0p5);
  fhList.Add(hPullsv2b_0p4_0p5);  

  fhList.Add(hXDC1_atZ0);
  fhList.Add(hYDC1_atZ0);
  fhList.Add(hXDC2_atZ0);
  fhList.Add(hYDC2_atZ0);
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

 fhList.Add(hXa_wireOccupancy);
  fhList.Add(hXb_wireOccupancy);
  fhList.Add(hYa_wireOccupancy);
  fhList.Add(hYb_wireOccupancy);
 fhList.Add(hUa_wireOccupancy);
  fhList.Add(hUb_wireOccupancy);
  fhList.Add(hVa_wireOccupancy);
  fhList.Add(hVb_wireOccupancy);
  fhList.Add(hXa2_wireOccupancy);
  fhList.Add(hXb2_wireOccupancy);
  fhList.Add(hYa2_wireOccupancy);
  fhList.Add(hYb2_wireOccupancy);
  fhList.Add(hUa2_wireOccupancy);
  fhList.Add(hUb2_wireOccupancy);
  fhList.Add(hVa2_wireOccupancy);
  fhList.Add(hVb2_wireOccupancy);

  fhList.Add(hChi2ndf_dc1);
  fhList.Add(hChi2ndf_dc2);
  fhList.Add(hChi2Matched);
  fhList.Add(hSegSize_dc1);
  fhList.Add(hSegSize_dc2);
  fhList.Add(Ntrack1);
  fhList.Add(Ntrack2);
  fhList.Add(NGtracks);
  fhList.Add(hx1Da_Db);
  fhList.Add(hy1Da_Db);
  fhList.Add(hu1Da_Db);
  fhList.Add(hv1Da_Db);

  fhList.Add(hx2Da_Db);
  fhList.Add(hy2Da_Db);
  fhList.Add(hu2Da_Db);
  fhList.Add(hv2Da_Db);
  fhList.Add(hx1Da_Db_best);
  fhList.Add(hy1Da_Db_best);
  fhList.Add(hu1Da_Db_best);
  fhList.Add(hv1Da_Db_best);

  fhList.Add(hx2Da_Db_best);
  fhList.Add(hy2Da_Db_best);
  fhList.Add(hu2Da_Db_best);
  fhList.Add(hv2Da_Db_best);
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
}

BmnDchTrackFinder::~BmnDchTrackFinder()
{
  if (isInitialized)
  {
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
    delete[] SegMCId[iChamber];
    delete[] SegMCIdCount[iChamber];
    delete[] pairs[iChamber];
    delete[] segment_size[iChamber];
    for (Int_t iDim = 0; iDim < N; iDim++) {
      delete[] v[iChamber][iDim];
      delete[] u[iChamber][iDim];
      delete[] y[iChamber][iDim];
      delete[] x[iChamber][iDim];
      delete[] vId[iChamber][iDim];
      delete[] uId[iChamber][iDim];
      delete[] yId[iChamber][iDim];
      delete[] xId[iChamber][iDim];
      delete[] v_time[iChamber][iDim];
      delete[] u_time[iChamber][iDim];
      delete[] y_time[iChamber][iDim];
      delete[] x_time[iChamber][iDim];
      delete[] sigm_v[iChamber][iDim];
      delete[] sigm_u[iChamber][iDim];
      delete[] sigm_y[iChamber][iDim];
      delete[] sigm_x[iChamber][iDim];
      delete[] v_Single[iChamber][iDim];
      delete[] u_Single[iChamber][iDim];
      delete[] y_Single[iChamber][iDim];
      delete[] x_Single[iChamber][iDim];
      delete[] v_SingleId[iChamber][iDim];
      delete[] u_SingleId[iChamber][iDim];
      delete[] y_SingleId[iChamber][iDim];
      delete[] x_SingleId[iChamber][iDim];
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
      delete[] rhId_segment[iChamber][iDim];
      delete[] rh_segment_time[iChamber][iDim];
      delete[] rh_sigm_segment[iChamber][iDim];
    }
    delete[] v[iChamber];
    delete[] u[iChamber];
    delete[] y[iChamber];
    delete[] x[iChamber];
    delete[] vId[iChamber];
    delete[] uId[iChamber];
    delete[] yId[iChamber];
    delete[] xId[iChamber];
    delete[] v_time[iChamber];
    delete[] u_time[iChamber];
    delete[] y_time[iChamber];
    delete[] x_time[iChamber];
    delete[] sigm_v[iChamber];
    delete[] sigm_u[iChamber];
    delete[] sigm_y[iChamber];
    delete[] sigm_x[iChamber];
    delete[] v_Single[iChamber];
    delete[] u_Single[iChamber];
    delete[] y_Single[iChamber];
    delete[] x_Single[iChamber];
    delete[] v_SingleId[iChamber];
    delete[] u_SingleId[iChamber];
    delete[] y_SingleId[iChamber];
    delete[] x_SingleId[iChamber];
    delete[] Sigm_v_single[iChamber];
    delete[] Sigm_u_single[iChamber];
    delete[] Sigm_y_single[iChamber];
    delete[] Sigm_x_single[iChamber];
    delete[] params[iChamber];
    delete[] params_sigmas[iChamber];
    delete[] rh_segment[iChamber];
    delete[] rhId_segment[iChamber];
    delete[] rh_segment_time[iChamber];
    delete[] rh_sigm_segment[iChamber];
    for (Int_t iWire = 0; iWire < nWires; iWire++)
      delete[] singles[iChamber][iWire];
    delete[] singles[iChamber];
  }
  }

  delete[] x_global;
  delete[] y_global;
  delete[] Chi2;
  delete[] pairs;
  delete[] SegMCId;
  delete[] SegMCIdCount;
  delete[] segment_size;
  delete[] v;
  delete[] u;
  delete[] y;
  delete[] x;
  delete[] v_time;
  delete[] u_time;
  delete[] y_time;
  delete[] x_time;
  delete[] sigm_v;
  delete[] sigm_u;
  delete[] sigm_y;
  delete[] sigm_x;
  delete[] v_Single;
  delete[] u_Single;
  delete[] y_Single;
  delete[] x_Single;
  delete[] v_SingleId;
  delete[] u_SingleId;
  delete[] y_SingleId;
  delete[] x_SingleId;
  delete[] Sigm_v_single;
  delete[] Sigm_u_single;
  delete[] Sigm_y_single;
  delete[] Sigm_x_single;
  delete[] params;
  delete[] params_sigmas;
  delete[] rh_segment;
  delete[] rhId_segment;
  delete[] rh_segment_time;
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
  // cout << "------------------------------------------------------------Event number: " << fEventNo << endl;
  Double_t driftLength[nPlanes][nDim];
  Double_t wires[nPlanes][nDim];
  Int_t hitId[nPlanes][nDim];
  Double_t xMC[nPlanes][nDim];
  Double_t yMC[nPlanes][nDim];
  Double_t zMC[nPlanes][nDim];
  
  Int_t it[nPlanes];
  Bool_t used[nPlanes][nDim];
  for (Int_t iPlanes = 0; iPlanes < nPlanes; iPlanes++) {
    it[iPlanes] = 0;
    for (Int_t iDim = 0; iDim < nDim; iDim++) {
      used[iPlanes][iDim] = kFALSE;
      driftLength[iPlanes][iDim] = 0.;
      hitId[iPlanes][iDim] = -999;
      wires[iPlanes][iDim] = 0.;
      xMC[iPlanes][iDim] = 999.;
      yMC[iPlanes][iDim] = 999.;
      zMC[iPlanes][iDim] = 999.;
    }
  }

  if (expData) {
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

      // Order used: vb1(0), va1(1), ub1(2), ua1(3), yb1(4), ya1(5), xb1(6), xa1(7) ->
      //             vb2(8), va2(9), ub2(10), ua2(11), yb2(12), ya2(13), xb2(14), xa2(15)
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
      driftLength[plane][it[plane]] = time;

      it[plane]++;
      if (plane == 6) hSlot_1xa_time->Fill(time);
      if (plane == 7) hSlot_1xb_time->Fill(time);
     
      if (plane == 1) hVa_wireOccupancy->Fill(wire);
      if (plane == 0) hVb_wireOccupancy->Fill(wire);
      if (plane == 3) hUa_wireOccupancy->Fill(wire);
      if (plane == 2) hUb_wireOccupancy->Fill(wire);
      if (plane == 5) hYa_wireOccupancy->Fill(wire);
      if (plane == 4) hYb_wireOccupancy->Fill(wire);
      if (plane == 7) hXa_wireOccupancy->Fill(wire);
      if (plane == 6) hXb_wireOccupancy->Fill(wire);
      if (plane == 9) hVa2_wireOccupancy->Fill(wire);
      if (plane == 8) hVb2_wireOccupancy->Fill(wire);
      if (plane == 11) hUa2_wireOccupancy->Fill(wire);
      if (plane == 10) hUb2_wireOccupancy->Fill(wire);
      if (plane == 13) hYa2_wireOccupancy->Fill(wire);
      if (plane == 12) hYb2_wireOccupancy->Fill(wire);
      if (plane == 15) hXa2_wireOccupancy->Fill(wire);
      if (plane == 14) hXb2_wireOccupancy->Fill(wire);


    }//iDig
  }//isExp
  else{
    
    //read MC data
    Double_t x1_MC1 = 999;
    Double_t y1_MC1 = 999;
    Double_t z1_MC1 = 999;
    Double_t x1_MC2 = 999;
    Double_t y1_MC2 = 999;
    Double_t z1_MC2 = 999;

    bool _MC1 = false;
    bool _MC2 = false;
    for (Int_t iMC = 0; iMC < fBmnHitsArray->GetEntriesFast(); ++iMC) {
      BmnDchHit* hit = (BmnDchHit*)fBmnHitsArray->UncheckedAt(iMC);
   
      // Order used: vb1(0), va1(1), ub1(2), ua1(3), yb1(4), ya1(5), xb1(6), xa1(7) ->
      //             vb2(8), va2(9), ub2(10), ua2(11), yb2(12), ya2(13), xb2(14), xa2(15)
      Short_t plane = hit->GetLayerNumber();
      Short_t wire = hit->GetWirePosition();
      Double_t time = hit->GetDrift();
      Double_t x_MC = hit->GetX();
      Double_t y_MC = hit->GetY();
      Double_t z_MC = hit->GetZ();

      hitId[plane][it[plane]] = hit->GetHitId();
      wires[plane][it[plane]] = wire;
      driftLength[plane][it[plane]] = time;
      xMC[plane][it[plane]] = x_MC;
      yMC[plane][it[plane]] = y_MC;
      zMC[plane][it[plane]] = z_MC;
      it[plane]++;
      if (plane == 7) hSlot_1xa_time->Fill(time);
      if (plane == 6) hSlot_1xb_time->Fill(time);
    
    } //end MC read
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
      Int_t*** coordId = (iWire == 0) ? vId : (iWire == 1) ? uId : (iWire == 2) ? yId : xId;
      Double_t*** sigma = (iWire == 0) ? sigm_v : (iWire == 1) ? sigm_u : (iWire == 2) ? sigm_y : sigm_x;
      Double_t*** coord_time = (iWire == 0) ? v_time : (iWire == 1) ? u_time : (iWire == 2) ? y_time : x_time;
      if (expData) {	   
	pairs[iDch][iWire] = Reconstruction(iDch + 1, wireNames[iWire], pairs[iDch][iWire], it[start], it[finish],
					    wires[start], wires[finish], driftLength[start], driftLength[finish], used[start], used[finish],
					    coord[iDch], sigma[iDch], coord_time[iDch]);

	for (Int_t iLayer = 0; iLayer < nLayers; iLayer++) {
	  Double_t*** single_coord = (iWire == 0) ? v_Single : (iWire == 1) ? u_Single : (iWire == 2) ? y_Single : x_Single;
	  Double_t*** single_sigma = (iWire == 0) ? Sigm_v_single : (iWire == 1) ? Sigm_u_single : (iWire == 2) ? Sigm_y_single : Sigm_x_single;
	  singles[iDch][iWire][iLayer] = ReconstructionSingle(iDch + 1, wireNames[iWire], layNames[iLayer],
							      singles[iDch][iWire][iLayer], it[cntr], wires[cntr], driftLength[cntr], used[cntr], single_coord[iDch], single_sigma[iDch]);
	  cntr++;
	}
      }else{   
	pairs[iDch][iWire] = ReconstructionMC(iDch + 1, wireNames[iWire], pairs[iDch][iWire], it[start], it[finish],
					      wires[start], wires[finish], driftLength[start], driftLength[finish], hitId[start], hitId[finish], used[start], used[finish], coord[iDch], coordId[iDch], sigma[iDch], coord_time[iDch]);
    
	for (Int_t iLayer = 0; iLayer < nLayers; iLayer++) {
	  Double_t*** single_coord = (iWire == 0) ? v_Single : (iWire == 1) ? u_Single : (iWire == 2) ? y_Single : x_Single;
	  Int_t*** single_coordId = (iWire == 0) ? v_SingleId : (iWire == 1) ? u_SingleId : (iWire == 2) ? y_SingleId : x_SingleId;
	  Double_t*** single_sigma = (iWire == 0) ? Sigm_v_single : (iWire == 1) ? Sigm_u_single : (iWire == 2) ? Sigm_y_single : Sigm_x_single;
	  singles[iDch][iWire][iLayer] = ReconstructionSingleMC(iDch + 1, wireNames[iWire], layNames[iLayer],
								singles[iDch][iWire][iLayer], it[cntr], wires[cntr], driftLength[cntr], hitId[cntr], used[cntr], single_coord[iDch], single_coordId[iDch], single_sigma[iDch]);
	  cntr++;
	}
      }
    }           

    nSegments[iDch] = BuildUVSegments(iDch + 1,
				      pairs[iDch][1], pairs[iDch][0], pairs[iDch][3], pairs[iDch][2], singles[iDch][1][0], singles[iDch][1][1], singles[iDch][0][0], singles[iDch][0][1],
				      x[iDch], y[iDch], u[iDch], v[iDch], xId[iDch], yId[iDch], uId[iDch], vId[iDch],x_time[iDch], y_time[iDch], u_time[iDch], v_time[iDch], sigm_x[iDch], sigm_y[iDch], sigm_u[iDch], sigm_v[iDch], rh_segment[iDch],rhId_segment[iDch],rh_segment_time[iDch], rh_sigm_segment[iDch],
				      u_Single[iDch], v_Single[iDch],u_SingleId[iDch], v_SingleId[iDch], Sigm_u_single[iDch], Sigm_v_single[iDch]);
    nSegments[iDch] = BuildXYSegments(iDch + 1,
				      pairs[iDch][1], pairs[iDch][0], pairs[iDch][3], pairs[iDch][2], 
				      singles[iDch][3][0], singles[iDch][3][1], singles[iDch][2][0], singles[iDch][2][1],
				      x[iDch], y[iDch], u[iDch], v[iDch], 
				      xId[iDch], yId[iDch], uId[iDch], vId[iDch], x_time[iDch], y_time[iDch], u_time[iDch], v_time[iDch],
				      sigm_x[iDch], sigm_y[iDch], sigm_u[iDch], sigm_v[iDch], 
				      rh_segment[iDch],rhId_segment[iDch],rh_segment_time[iDch], rh_sigm_segment[iDch],
				      x_Single[iDch], y_Single[iDch], x_SingleId[iDch], y_SingleId[iDch], Sigm_x_single[iDch], Sigm_y_single[iDch]);
   
                               
    has7DC[iDch] = FitDchSegments(iDch + 1, segment_size[iDch], rh_segment[iDch], rh_sigm_segment[iDch], params[iDch], Chi2[iDch], x_global[iDch], y_global[iDch]);  // Fit found segments
    SelectLongestAndBestSegments(iDch + 1, segment_size[iDch], rh_segment[iDch], Chi2[iDch]);                                                                        // Save only longest and best chi2 segments
    FillSegmentParametersSigmas(iDch + 1, rh_segment[iDch], rh_sigm_segment[iDch], Chi2[iDch], params_sigmas[iDch]);   
    FindSegmentTrackMCId(iDch + 1, rhId_segment[iDch], Chi2[iDch], SegMCId[iDch], SegMCIdCount[iDch]);

    //count wire multiplicity per layer
    if(iDch == 0){
      layers_with = 0;
      for(Int_t i = 0; i < 8; i++){
	if(it[i] > 0)layers_with++;
	if(it[i] == 0) hNoFiredWiresOnLayerDc1->Fill(i); 
      }
      hNLayWithFiredWiresDc1->Fill(layers_with);
    }else{
      layers_with2 = 0;
      for(Int_t i = 8; i < 16; i++){
	if(it[i] > 0)layers_with2++;
	if(it[i] == 0) hNoFiredWiresOnLayerDc2->Fill(i-8); 
      }
      hNLayWithFiredWiresDc2->Fill(layers_with2);
    }
    //calculate segment parameters errors
    CreateDchTrack(iDch + 1, Chi2[iDch], params[iDch], segment_size[iDch], rh_segment[iDch],rh_segment_time[iDch], params_sigmas[iDch], SegMCId[iDch], SegMCIdCount[iDch]);  	
    // ! Fill segment parameters
    // - Next function works if the local shifts are set to 0. 
    // - Be careful - the refit brackes segment parameters. 
    // - This function must be used only for residual calculation. Everything else becomes wrong!

    //	FillPlaneResiduals(iDch + 1, segment_size[iDch], rh_segment[iDch], rh_sigm_segment[iDch], params[iDch], Chi2[iDch]);
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
    double chi2_Match_min = 1200.;

    Double_t dx = -999;
    Double_t dy = -999;
    Double_t daX = -999;
    Double_t daY = -999;

    for (Int_t segdc2Nr = 0; segdc2Nr < nSegments[1]; segdc2Nr++) {
      if (Chi2[1][segdc2Nr] > 998.)
	continue;

      Double_t chi2_match = 0;
      for (int segdc1Nr = 0; segdc1Nr < nSegments[0]; segdc1Nr++) {
	if (Chi2[0][segdc1Nr] > 998.)
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
	if (fabs(dx) > 20. || fabs(dy) > 20. || fabs(daX) > 0.25 || fabs(daY) > 0.25) continue;
	if(chi2_Match<chi2_Match_min){
	  chi2_Match_min = chi2_Match;
	  best1 = segdc1Nr;
	  best2 = segdc2Nr;
	  hX1_X2_glob_m->Fill(dx);
	  hY1_Y2_glob_m->Fill(dy);
	  haX2_aX1m->Fill(daX);
	  haY2_aY1m->Fill(daY);
	}
      }  // segdc1Nr

    }  // segdc2Nr

    if (chi2_Match_min > 1199.) break;
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
    //    hP->Fill(0.3*2.84/(atan(aX[nSegmentsToBeMatched])));
    //set of field integrals to choose from
    // float intBL = 0.984;//600A  818
    // float intBL = 1.455;//900A 816 817
    // float intBL = 2.725;//1750A
    //float intBL = 1.93;//1200A 815
    // float intBL = 2.38;//1500A
    // float intBL = 3.38;//2100A
    // float intBL = 2.55;//1600A 814 834
    // float intBL = 2.84;//1800A 813
    hChi2Matched->Fill(chi2_Match_min);
    Chi2[0][best1] = 999;
    Chi2[1][best2] = 999;

  }  //while
}

Int_t BmnDchTrackFinder::BuildXYSegments(Int_t dchID,
                                         Int_t pairU, Int_t pairV, Int_t pairX, Int_t pairY, 
					 Int_t single_xa, Int_t single_xb, Int_t single_ya, Int_t single_yb,
                                         Double_t** x_ab, Double_t** y_ab, Double_t** u_ab, Double_t** v_ab,
					 Int_t** x_abId, Int_t** y_abId, Int_t** u_abId, Int_t** v_abId,
					 Double_t** x_ab_time, Double_t** y_ab_time, Double_t** u_ab_time, Double_t** v_ab_time,
                                         Double_t** sigm_x_ab, Double_t** sigm_y_ab, Double_t** sigm_u_ab, Double_t** sigm_v_ab,
                                         Double_t** rh_seg, Int_t** rhId_seg, Double_t** rh_seg_time, Double_t** rh_sigm_seg,
                                         Double_t** x_single, Double_t** y_single, Int_t** x_singleId, Int_t** y_singleId, Double_t** sigm_x_single, Double_t** sigm_y_single) {
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
      if(fabs(u_coord)<12. && fabs(v_coord)<12.)continue;
      if (fPeriod == 7 && fRunId > 3589) {
	if(dchID == 1){
	  if(fabs(u_coord-30)<12 && fabs(v_coord+22)<12)continue;//cut on beam for Ar
	}
	if(dchID == 2){
	  if(fabs(u_coord-37)<14 && fabs(v_coord+25)<16)continue;//cut on beam for Ar   
	}
      }
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
	  if(dchID == 1)hXm_Xe_loc->Fill(x_coord - x_est);
	  if (Abs(x_coord - x_est) > dX_thresh)
	    continue;
	  dX_thresh = Abs(x_coord - x_est);

	  rh_seg[0][nDC_segments] = x_ab[0][k];
	  rh_seg[1][nDC_segments] = x_ab[1][k];
	  rh_seg[4][nDC_segments] = u_ab[0][i];
	  rh_seg[5][nDC_segments] = u_ab[1][i];
	  rh_seg[6][nDC_segments] = v_ab[0][j];
	  rh_seg[7][nDC_segments] = v_ab[1][j];
	  rhId_seg[0][nDC_segments] = x_abId[0][k];
	  rhId_seg[1][nDC_segments] = x_abId[1][k];
	  rhId_seg[4][nDC_segments] = u_abId[0][i];
	  rhId_seg[5][nDC_segments] = u_abId[1][i];
	  rhId_seg[6][nDC_segments] = v_abId[0][j];
	  rhId_seg[7][nDC_segments] = v_abId[1][j];
	  rh_seg_time[0][nDC_segments] = x_ab_time[0][k];
	  rh_seg_time[1][nDC_segments] = x_ab_time[1][k];
	  rh_seg_time[4][nDC_segments] = u_ab_time[0][i];
	  rh_seg_time[5][nDC_segments] = u_ab_time[1][i];
	  rh_seg_time[6][nDC_segments] = v_ab_time[0][j];
	  rh_seg_time[7][nDC_segments] = v_ab_time[1][j];
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
	  if(dchID == 1)hYm_Ye_loc->Fill(y_coord - y_est);
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
	  rhId_seg[2][nDC_segments] = y_abId[0][m];
	  rhId_seg[3][nDC_segments] = y_abId[1][m];
	  rhId_seg[4][nDC_segments] = u_abId[0][i];
	  rhId_seg[5][nDC_segments] = u_abId[1][i];
	  rhId_seg[6][nDC_segments] = v_abId[0][j];
	  rhId_seg[7][nDC_segments] = v_abId[1][j];
	  rh_seg_time[2][nDC_segments] = y_ab_time[0][m];
	  rh_seg_time[3][nDC_segments] = y_ab_time[1][m];
	  rh_seg_time[4][nDC_segments] = u_ab_time[0][i];
	  rh_seg_time[5][nDC_segments] = u_ab_time[1][i];
	  rh_seg_time[6][nDC_segments] = v_ab_time[0][j];
	  rh_seg_time[7][nDC_segments] = v_ab_time[1][j];
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
	      if (Abs(x_single[0][kk] - x_est) > 1.2)
		continue;  //????? 0.5 needs to be reviewed

	      if (Abs(x_single[0][kk] - x_est) < min_a) {
		min_a = Abs(x_single[0][kk] - x_est);
		rh_seg[0][nDC_segments] = x_single[0][kk];
		rhId_seg[0][nDC_segments] = x_singleId[0][kk];
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
		rhId_seg[1][nDC_segments] = x_singleId[1][kk];
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
	      rhId_seg[2][nDC_segments] = y_singleId[0][kk];
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
	      rhId_seg[3][nDC_segments] = y_singleId[1][kk];
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
					 Int_t** x_abId, Int_t** y_abId, Int_t** u_abId, Int_t** v_abId,
					 Double_t** x_ab_time, Double_t** y_ab_time, Double_t** u_ab_time, Double_t** v_ab_time,
					 Double_t** sigm_x_ab, Double_t** sigm_y_ab, Double_t** sigm_u_ab, Double_t** sigm_v_ab,
                                         Double_t** rh_seg, Int_t** rhId_seg,Double_t** rh_seg_time, Double_t** rh_sigm_seg,
                                         Double_t** u_single, Double_t** v_single, Int_t** u_singleId, Int_t** v_singleId, Double_t** sigm_u_single, Double_t** sigm_v_single) {
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
      if (fPeriod == 7 && fRunId > 3589) {
	if(dchID == 1){ 
	  if(fabs(x_coord+37)<12 && fabs(y_coord-5)<12)continue;//cut on beam for Ar
	}
	if(dchID == 2){
	  if(fabs(x_coord+45)<12 && fabs(y_coord-8)<12)continue;//cut on beam for Ar
	}
      }
      if(fabs(y_coord)<12. && fabs(x_coord)<12.)continue;
      Double_t y_slope = (y_ab[0][j] - y_ab[1][j]) / (z_loc[2] - z_loc[3]);
      Double_t YU = y_coord + y_slope * 0.5 * (z_loc[5] + z_loc[4] - z_loc[3] - z_loc[2]);
      Double_t YV = y_coord + y_slope * 0.5 * (z_loc[7] + z_loc[6] - z_loc[3] - z_loc[2]);
      Bool_t foundU = kFALSE;
      Double_t u_est = isqrt_2 * (YU - XU);
      Double_t v_est = isqrt_2 * (YV + XV);

      Double_t dU_thresh = 1.2;
      for (Int_t k = 0; k < pairU; k++) {
	Double_t u_coord = (u_ab[0][k] + u_ab[1][k]) / 2;
	if(dchID == 1)hUm_Ue_loc->Fill(u_coord - u_est);
	if (Abs(u_coord - u_est) > dU_thresh)
	  continue;
	dU_thresh = Abs(u_coord - u_est);

	rh_seg[0][nDC_segments] = x_ab[0][i];
	rh_seg[1][nDC_segments] = x_ab[1][i];
	rh_seg[2][nDC_segments] = y_ab[0][j];
	rh_seg[3][nDC_segments] = y_ab[1][j];
	rh_seg[4][nDC_segments] = u_ab[0][k];
	rh_seg[5][nDC_segments] = u_ab[1][k];
	rhId_seg[0][nDC_segments] = x_abId[0][i];
	rhId_seg[1][nDC_segments] = x_abId[1][i];
	rhId_seg[2][nDC_segments] = y_abId[0][j];
	rhId_seg[3][nDC_segments] = y_abId[1][j];
	rhId_seg[4][nDC_segments] = u_abId[0][k];
	rhId_seg[5][nDC_segments] = u_abId[1][k];
	rh_seg_time[0][nDC_segments] = x_ab_time[0][i];
	rh_seg_time[1][nDC_segments] = x_ab_time[1][i];
	rh_seg_time[2][nDC_segments] = y_ab_time[0][j];
	rh_seg_time[3][nDC_segments] = y_ab_time[1][j];
	rh_seg_time[4][nDC_segments] = u_ab_time[0][k];
	rh_seg_time[5][nDC_segments] = u_ab_time[1][k];
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
	if(dchID == 1)hVm_Ve_loc->Fill(v_coord - v_est);
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
	rhId_seg[0][nDC_segments] = x_abId[0][i];
	rhId_seg[1][nDC_segments] = x_abId[1][i];
	rhId_seg[2][nDC_segments] = y_abId[0][j];
	rhId_seg[3][nDC_segments] = y_abId[1][j];
	rhId_seg[6][nDC_segments] = v_abId[0][m];
	rhId_seg[7][nDC_segments] = v_abId[1][m];
	rh_seg_time[0][nDC_segments] = x_ab_time[0][i];
	rh_seg_time[1][nDC_segments] = x_ab_time[1][i];
	rh_seg_time[2][nDC_segments] = y_ab_time[0][j];
	rh_seg_time[3][nDC_segments] = y_ab_time[1][j];
	rh_seg_time[6][nDC_segments] = v_ab_time[0][m];
	rh_seg_time[7][nDC_segments] = v_ab_time[1][m];
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
	      rhId_seg[4][nDC_segments] = u_singleId[0][kk];
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
	      rhId_seg[5][nDC_segments] = u_singleId[1][kk];
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
      if (!foundV && foundU) {
	Double_t min_a = 999;
	Double_t min_b = 999;
	for (Int_t kk = 0; kk < single_va; kk++) {
	  if (Abs(v_single[0][kk] - v_est) > 1.2)
	    continue;  //????? 0.5 needs to be reviewed
	  if (Abs(v_single[0][kk] - v_est) < min_a) {
	    min_a = Abs(v_single[0][kk] - v_est);
	    rh_seg[6][nDC_segments] = v_single[0][kk];
	    rhId_seg[6][nDC_segments] = v_singleId[0][kk];
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
	    rhId_seg[7][nDC_segments] = v_singleId[1][kk];
	    rh_sigm_seg[7][nDC_segments] = sigm_v_single[1][kk];
	    foundV = kTRUE;
	  }
	}
      }
      if (foundV || foundU) nDC_segments++;
    }//y
  }
  return nDC_segments;
}

Bool_t BmnDchTrackFinder::FitDchSegments(Int_t dchID, Int_t* size_seg, Double_t** rh_seg, Double_t** rh_sigm_seg, Double_t** par_ab, Double_t* chi2, Double_t* x_glob, Double_t* y_glob) {
  Int_t nDC_segments = (dchID == 1) ? nSegments[0] : nSegments[1];
  Double_t Z_dch = (dchID == 1) ? Z_dch1 : Z_dch2;
  Bool_t hasSuffNumberOfSegments = kFALSE;
  Double_t sqrt_2 = sqrt(2.);
  Double_t isqrt_2 = 1. / sqrt_2;
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
      if (chi2[j] > 180.)
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
    Double_t x_slope_sh = (dchID == 1) ? x1_slope_sh : x2_slope_sh;
    Double_t y_slope_sh = (dchID == 1) ? y1_slope_sh : y2_slope_sh;
    Double_t x_sh = (dchID == 1) ? x1_sh : x2_sh;
    Double_t y_sh = (dchID == 1) ? y1_sh : y2_sh;
    //the following 4 lines to be commented for residual calculation
    par_ab[0][j] += x_slope_sh + x_slope_sh * par_ab[0][j] * par_ab[0][j];
    par_ab[2][j] += y_slope_sh + y_slope_sh * par_ab[2][j] * par_ab[2][j];
    par_ab[1][j] += x_sh;
    par_ab[3][j] += y_sh;
	
    //    cout<<" par_ab[1][j] "<< par_ab[1][j]<<endl;
    if(dchID == 1){
      hXDC1_atZ0 ->Fill(par_ab[0][j]*(-Z_dch+249)+par_ab[1][j]);
      hYDC1_atZ0 ->Fill(par_ab[2][j]*(-Z_dch+249)+par_ab[3][j]);
    }else{
      hXDC2_atZ0 ->Fill(par_ab[0][j]*(-Z_dch+249)+par_ab[1][j]);
      hYDC2_atZ0 ->Fill(par_ab[2][j]*(-Z_dch+249)+par_ab[3][j]);
    }
    //cut on x0 and y0 at back of the magnet
    if(expData){
      if((par_ab[2][j]*(-Z_dch+249)+par_ab[3][j]) < -50 || (par_ab[2][j]*(-Z_dch+249)+par_ab[3][j]) > 50 || (par_ab[0][j]*(-Z_dch+249)+par_ab[1][j]) < -85 || (par_ab[0][j]*(-Z_dch+249)+par_ab[1][j]) > 85){
	chi2[j] = 999;
      }
    }
    Int_t coeff = (dchID == 1) ? 1 : -1;
    x_glob[j] = coeff * par_ab[0][j] * dZ_dch_mid + par_ab[1][j];
    y_glob[j] = coeff * par_ab[2][j] * dZ_dch_mid + par_ab[3][j];
    if (size_seg[j] > 5)
      hasSuffNumberOfSegments = kTRUE;
  }
  return hasSuffNumberOfSegments;
}

void BmnDchTrackFinder::CompareDaDb(Double_t d, Double_t& ele) {
  ele = (d < 0.02) ? (0.04 * 0.04) : (d >= 0.02 && d < 0.1) ? (0.03* 0.03) : (d >= 0.1 && d < 0.4) ? (0.015 * 0.015) : (d >= 0.4 && d < 0.41) ? (0.04 * 0.04) : (0.05 * 0.05);//2019.11.5
}

void BmnDchTrackFinder::CompareDaDb(Double_t d, Double_t& ele1, Double_t& ele2) {
  ele1 = (d < 0.02) ? (0.04 * 0.04) : (d >= 0.02 && d < 0.1) ? (0.03* 0.03) : (d >= 0.1 && d < 0.4) ? (0.015 * 0.015) : (d >= 0.4 && d < 0.41) ? (0.04 * 0.04) : (0.05 * 0.05);//2019.11.5
  ele2 = ele1;
}

void BmnDchTrackFinder::SelectLongestAndBestSegments(Int_t dchID, Int_t* size_seg, Double_t** rh_seg, Double_t* chi2) {
  Int_t nDC_segments = (dchID == 1) ? nSegments[0] : nSegments[1];
  for (Int_t max_size = 8; max_size > 5; max_size--){
    for (Int_t it1 = 0; it1 < nDC_segments; it1++) {
      if (size_seg[it1] != max_size || chi2[it1] > 990.)
	continue;
      for (Int_t it2 = 0; it2 < nDC_segments; it2++) {
	if (it2 == it1 || chi2[it2] > 990.)
	  continue;
	for (Int_t hit = 0; hit < 4; hit++){
	  if ((rh_seg[2 * hit][it1] == rh_seg[2 * hit][it2] || rh_seg[2 * hit + 1][it1] == rh_seg[2 * hit + 1][it2])
	      && (rh_seg[2 * hit][it1] > -998 && rh_seg[2 * hit + 1][it1] > -998)){
	    if( size_seg[it1] == size_seg[it2]) {
	      if (chi2[it1] <= chi2[it2]){
		chi2[it2] = 999.;
		break;
	      }else{
		chi2[it1] = 999.;
		break;
	      }
	    }//SAME SIZE
	    if( size_seg[it1] > size_seg[it2]) {
	      chi2[it2] = 999.;
	      break;
	    }
	  }//have common hits
	}//rh cycle
      }//SECOND CYCLE           
    }//FIRST CYCLE
  }//SCAN SEG SIZE
}  
  

void BmnDchTrackFinder::FillPlaneResiduals(Int_t dchID, Int_t* size_seg, Double_t** rh_seg, Double_t** rh_sigm_seg, Double_t** par_ab, Double_t* chi2){
  Int_t nDC_segments = (dchID == 1) ? nSegments[0] : nSegments[1];
  for (Int_t it1 = 0; it1 < nDC_segments; it1++) {
    if (chi2[it1] > 990.) continue;
    Double_t _rh_seg[8];
    Double_t _rh_sigm_seg[8];
    Double_t _par_ab[4];
       
    for (Int_t i = 0; i < 8; i++) {
      _rh_seg[i] = rh_seg[i][it1];
      _rh_sigm_seg[i] = rh_sigm_seg[i][it1];
    }

    fit_seg(z_loc, _rh_seg, _rh_sigm_seg, _par_ab, -1, -1); //usual fit without skipping any plane
    for (Int_t i = 0; i < 4; i++)
      par_ab[i][it1] = _par_ab[i];
    //fill residuals
    if(size_seg[it1] == 8){
      if(dchID == 1){
	 
	double res1 = CalculateResidual(0, it1, rh_seg, par_ab);
	double res2 = CalculateResidual(1, it1, rh_seg, par_ab);

	hResidx1a->Fill(res1);
	hResidx1b->Fill(res2);
	 
	double int1;
	double int2;
	double dist_a = 999;
	double dist_b = 999;
	if(fabs(modf(rh_seg[0][it1],&int1))< .5){
	  dist_a = fabs(modf(rh_seg[0][it1],&int1));
	  hDc1XaResVsDa->Fill(dist_a,fabs(res1),1);
	  if (res1 < 0.)hDc1XaResVsDa_m->Fill(dist_a,-res1,1);
	  if (res1 > 0.)hDc1XaResVsDa_p->Fill(dist_a,res1,1);
	}else{
	  dist_a = 1 - fabs(modf(rh_seg[0][it1],&int1));
	  hDc1XaResVsDa->Fill(dist_a,fabs(res1),1);
	  if (res1 < 0.)hDc1XaResVsDa_m->Fill(dist_a,-res1,1);
	  if (res1 > 0.)hDc1XaResVsDa_p->Fill(dist_a,res1,1);
	}
	if(fabs(modf(rh_seg[1][it1],&int1))< .5){
	  dist_b = 0.5 - fabs(modf(rh_seg[1][it1],&int1));
	  hDc1XbResVsDb->Fill(dist_b,fabs(res2),1);
	  if (res2 < 0.)hDc1XbResVsDb_m->Fill(dist_b,-res2,1);
	  if (res2 > 0.)hDc1XbResVsDb_p->Fill(dist_b,res2,1);
	}else{
	  dist_b = fabs(modf(rh_seg[1][it1],&int1)) - .5;
	  hDc1XbResVsDb->Fill(dist_b, fabs(res2),1);
	  if (res2 < 0.)hDc1XbResVsDb_m->Fill(dist_b,-res2,1);
	  if (res2 > 0.)hDc1XbResVsDb_p->Fill(dist_b,res2,1);
	}
	if(dist_a < .1){
	  hResidx1a_0p1->Fill(res1);
	  hPullsx1a_0p1->Fill(res1/sqrt(rh_sigm_seg[0][it1]));
	}
	if(dist_a > .1 && dist_a < .4){
	  hResidx1a_0p1_0p4->Fill(res1);
	  hPullsx1a_0p1_0p4->Fill(res1/sqrt(rh_sigm_seg[0][it1]));
	}
	if(dist_a > .4){
	  hResidx1a_0p4_0p5->Fill(res1);
	  hPullsx1a_0p4_0p5->Fill(res1/sqrt(rh_sigm_seg[0][it1]));
	}
	if(dist_b < .1){
	  hResidx1b_0p1->Fill(res2);
	  hPullsx1b_0p1->Fill(res2/sqrt(rh_sigm_seg[1][it1]));
	}
	if(dist_b > .1 && dist_b < .4){
	  hResidx1b_0p1_0p4->Fill(res2);
	  hPullsx1b_0p1_0p4->Fill(res2/sqrt(rh_sigm_seg[1][it1]));
	}
	if(dist_b > .4){
	  hResidx1b_0p4_0p5->Fill(res2);
	  hPullsx1b_0p4_0p5->Fill(res2/sqrt(rh_sigm_seg[1][it1]));
	}
	hx1Da_Db_best->Fill(dist_a+dist_b-0.5);

	//y
	dist_a = 999;
	dist_b = 999;
	res1 = CalculateResidual(2, it1, rh_seg, par_ab);
	res2 = CalculateResidual(3, it1, rh_seg, par_ab);
	hResidy1a->Fill(res1);
	hResidy1b->Fill(res2);
	if(fabs(modf(rh_seg[2][it1],&int1))< .5){
	  dist_a = fabs(modf(rh_seg[2][it1],&int1));
	}else{
	  dist_a = 1 - fabs(modf(rh_seg[2][it1],&int1));
	}
	if(fabs(modf(rh_seg[3][it1],&int1))< .5){
	  dist_b = 0.5 - fabs(modf(rh_seg[3][it1],&int1));
	}else{
	  dist_b = fabs(modf(rh_seg[3][it1],&int1)) - .5;
	}
	if(dist_a < .1)hResidy1a_0p1->Fill(res1);
	if(dist_a > .1 && dist_a < .4)hResidy1a_0p1_0p4->Fill(res1);
	if(dist_a > .4)hResidy1a_0p4_0p5->Fill(res1);
	if(dist_b < .1)hResidy1b_0p1->Fill(res2);
	if(dist_b > .1 && dist_b < .4)hResidy1b_0p1_0p4->Fill(res2);
	if(dist_b > .4)hResidy1b_0p4_0p5->Fill(res2);
	hy1Da_Db_best->Fill(dist_a+dist_b-0.5);
	if(dist_a < .1){
	  hPullsy1a_0p1->Fill(res1/sqrt(rh_sigm_seg[2][it1]));
	}
	if(dist_a > .1 && dist_a < .4){
	  hPullsy1a_0p1_0p4->Fill(res1/sqrt(rh_sigm_seg[2][it1]));
	}
	if(dist_a > .4){
	  hPullsy1a_0p4_0p5->Fill(res1/sqrt(rh_sigm_seg[2][it1]));
	}
	if(dist_b < .1){
	  hPullsy1b_0p1->Fill(res2/sqrt(rh_sigm_seg[3][it1]));
	}
	if(dist_b > .1 && dist_b < .4){
	  hPullsy1b_0p1_0p4->Fill(res2/sqrt(rh_sigm_seg[3][it1]));
	}
	if(dist_b > .4){
	  hPullsy1b_0p4_0p5->Fill(res2/sqrt(rh_sigm_seg[3][it1]));
	}


	//u
	dist_a = 999;
	dist_b = 999;
	res1 = CalculateResidual(4, it1, rh_seg, par_ab);
	res2 = CalculateResidual(5, it1, rh_seg, par_ab);
	hResidu1a->Fill(res1);
	hResidu1b->Fill(res2);

	if(fabs(modf(rh_seg[4][it1],&int1))< .5){
	  dist_a = fabs(modf(rh_seg[4][it1],&int1));
	}else{
	  dist_a = 1 - fabs(modf(rh_seg[4][it1],&int1));
	}
	if(fabs(modf(rh_seg[5][it1],&int1))< .5){
	  dist_b = 0.5 - fabs(modf(rh_seg[5][it1],&int1));
	}else{
	  dist_b = fabs(modf(rh_seg[5][it1],&int1)) - .5;
	}
	if(dist_a < .1)hResidu1a_0p1->Fill(res1);
	if(dist_a > .1 && dist_a < .4)hResidu1a_0p1_0p4->Fill(res1);
	if(dist_a > .4)hResidu1a_0p4_0p5->Fill(res1);
	if(dist_b < .1)hResidu1b_0p1->Fill(res2);
	if(dist_b > .1 && dist_b < .4)hResidu1b_0p1_0p4->Fill(res2);
	if(dist_b > .4)hResidu1b_0p4_0p5->Fill(res2);
	hu1Da_Db_best->Fill(dist_a+dist_b-0.5);
	if(dist_a < .1){
	  hPullsu1a_0p1->Fill(res1/sqrt(rh_sigm_seg[4][it1]));
	}
	if(dist_a > .1 && dist_a < .4){
	  hPullsu1a_0p1_0p4->Fill(res1/sqrt(rh_sigm_seg[4][it1]));
	}
	if(dist_a > .4){
	  hPullsu1a_0p4_0p5->Fill(res1/sqrt(rh_sigm_seg[4][it1]));
	}
	if(dist_b < .1){
	  hPullsu1b_0p1->Fill(res2/sqrt(rh_sigm_seg[5][it1]));
	}
	if(dist_b > .1 && dist_b < .4){
	  hPullsu1b_0p1_0p4->Fill(res2/sqrt(rh_sigm_seg[5][it1]));
	}
	if(dist_b > .4){
	  hPullsu1b_0p4_0p5->Fill(res2/sqrt(rh_sigm_seg[5][it1]));
	}
	//v
	dist_a = 999;
	dist_b = 999;
	res1 = CalculateResidual(6, it1, rh_seg, par_ab);
	res2 = CalculateResidual(7, it1, rh_seg, par_ab);
	hResidv1a->Fill(res1);
	hResidv1b->Fill(res2);
	if(fabs(modf(rh_seg[6][it1],&int1))< .5){



	  dist_a = fabs(modf(rh_seg[6][it1],&int1));
	}else{
	  dist_a = 1 - fabs(modf(rh_seg[6][it1],&int1));
	}
	if(fabs(modf(rh_seg[7][it1],&int1))< .5){
	  dist_b = 0.5 - fabs(modf(rh_seg[7][it1],&int1));
	}else{
	  dist_b = fabs(modf(rh_seg[7][it1],&int1)) - .5;
	}
	if(dist_a < .1)hResidv1a_0p1->Fill(res1);
	if(dist_a > .1 && dist_a < .4)hResidv1a_0p1_0p4->Fill(res1);
	if(dist_a > .4)hResidv1a_0p4_0p5->Fill(res1);
	if(dist_b < .1)hResidv1b_0p1->Fill(res2);
	if(dist_b > .1 && dist_b < .4)hResidv1b_0p1_0p4->Fill(res2);
	if(dist_b > .4)hResidv1b_0p4_0p5->Fill(res2);
	hv1Da_Db_best->Fill(dist_a+dist_b-0.5);

	if(dist_a < .1){
	  hPullsv1a_0p1->Fill(res1/sqrt(rh_sigm_seg[6][it1]));
	}
	if(dist_a > .1 && dist_a < .4){
	  hPullsv1a_0p1_0p4->Fill(res1/sqrt(rh_sigm_seg[6][it1]));
	}
	if(dist_a > .4){
	  hPullsv1a_0p4_0p5->Fill(res1/sqrt(rh_sigm_seg[6][it1]));
	}
	if(dist_b < .1){
	  hPullsv1b_0p1->Fill(res2/sqrt(rh_sigm_seg[7][it1]));
	}
	if(dist_b > .1 && dist_b < .4){
	  hPullsv1b_0p1_0p4->Fill(res2/sqrt(rh_sigm_seg[7][it1]));
	}
	if(dist_b > .4){
	  hPullsv1b_0p4_0p5->Fill(res2/sqrt(rh_sigm_seg[7][it1]));
	}

      }else{
	double res1 = CalculateResidual(0, it1, rh_seg, par_ab);
	double res2 = CalculateResidual(1, it1, rh_seg, par_ab);
	hResidx2a->Fill(res1);
	hResidx2b->Fill(res2);

	double int1;
	double int2;
	double dist_a = 999;
	double dist_b = 999;
	if(fabs(modf(rh_seg[0][it1],&int1))< .5){
	  dist_a = fabs(modf(rh_seg[0][it1],&int1));
	}else{
	  dist_a = 1 - fabs(modf(rh_seg[0][it1],&int1));
	}
	if(fabs(modf(rh_seg[1][it1],&int1))< .5){
	  dist_b = 0.5 - fabs(modf(rh_seg[1][it1],&int1));
	}else{
	  dist_b = fabs(modf(rh_seg[1][it1],&int1)) - .5;
	}
	if(dist_a < .1)hResidx2a_0p1->Fill(res1);
	if(dist_a > .1 && dist_a < .4)hResidx2a_0p1_0p4->Fill(res1);
	if(dist_a > .4)hResidx2a_0p4_0p5->Fill(res1);
	if(dist_b < .1)hResidx2b_0p1->Fill(res2);
	if(dist_b > .1 && dist_b < .4)hResidx2b_0p1_0p4->Fill(res2);
	if(dist_b > .4)hResidx2b_0p4_0p5->Fill(res2);
	if(dist_a < .1){
	  hPullsx2a_0p1->Fill(res1/sqrt(rh_sigm_seg[0][it1]));
	}
	if(dist_a > .1 && dist_a < .4){
	  hPullsx2a_0p1_0p4->Fill(res1/sqrt(rh_sigm_seg[0][it1]));
	}
	if(dist_a > .4){
	  hPullsx2a_0p4_0p5->Fill(res1/sqrt(rh_sigm_seg[0][it1]));
	}
	if(dist_b < .1){
	  hPullsx2b_0p1->Fill(res2/sqrt(rh_sigm_seg[1][it1]));
	}
	if(dist_b > .1 && dist_b < .4){
	  hPullsx2b_0p1_0p4->Fill(res2/sqrt(rh_sigm_seg[1][it1]));
	}
	if(dist_b > .4){
	  hPullsx2b_0p4_0p5->Fill(res2/sqrt(rh_sigm_seg[1][it1]));
	}
	hx2Da_Db_best->Fill(dist_a+dist_b-0.5);

	//y
	dist_a = 999;
	dist_b = 999;
	res1 = CalculateResidual(2, it1, rh_seg, par_ab);
	res2 = CalculateResidual(3, it1, rh_seg, par_ab);
	hResidy2a->Fill(res1);
	hResidy2b->Fill(res2);

	if(fabs(modf(rh_seg[2][it1],&int1))< .5){
	  dist_a = fabs(modf(rh_seg[2][it1],&int1));
	}else{
	  dist_a = 1 - fabs(modf(rh_seg[2][it1],&int1));
	}
	if(fabs(modf(rh_seg[3][it1],&int1))< .5){
	  dist_b = 0.5 - fabs(modf(rh_seg[3][it1],&int1));
	}else{
	  dist_b = fabs(modf(rh_seg[3][it1],&int1)) - .5;
	}
	if(dist_a < .1)hResidy2a_0p1->Fill(res1);
	if(dist_a > .1 && dist_a < .4)hResidy2a_0p1_0p4->Fill(res1);
	if(dist_a > .4)hResidy2a_0p4_0p5->Fill(res1);
	if(dist_b < .1)hResidy2b_0p1->Fill(res2);
	if(dist_b > .1 && dist_b < .4)hResidy2b_0p1_0p4->Fill(res2);
	if(dist_b > .4)hResidy2b_0p4_0p5->Fill(res2);
	if(dist_a < .1){
	  hPullsy2a_0p1->Fill(res1/sqrt(rh_sigm_seg[2][it1]));
	}
	if(dist_a > .1 && dist_a < .4){
	  hPullsy2a_0p1_0p4->Fill(res1/sqrt(rh_sigm_seg[2][it1]));
	}
	if(dist_a > .4){
	  hPullsy2a_0p4_0p5->Fill(res1/sqrt(rh_sigm_seg[2][it1]));
	}
	if(dist_b < .1){
	  hPullsy2b_0p1->Fill(res2/sqrt(rh_sigm_seg[3][it1]));
	}
	if(dist_b > .1 && dist_b < .4){
	  hPullsy2b_0p1_0p4->Fill(res2/sqrt(rh_sigm_seg[3][it1]));
	}
	if(dist_b > .4){
	  hPullsy2b_0p4_0p5->Fill(res2/sqrt(rh_sigm_seg[3][it1]));
	}
	hy2Da_Db_best->Fill(dist_a+dist_b-0.5);


	//u
	dist_a = 999;
	dist_b = 999;
	res1 = CalculateResidual(4, it1, rh_seg, par_ab);
	res2 = CalculateResidual(5, it1, rh_seg, par_ab);
	hResidu2a->Fill(res1);
	hResidu2b->Fill(res2);

	if(fabs(modf(rh_seg[4][it1],&int1))< .5){
	  dist_a = fabs(modf(rh_seg[4][it1],&int1));
	}else{
	  dist_a = 1 - fabs(modf(rh_seg[4][it1],&int1));
	}
	if(fabs(modf(rh_seg[5][it1],&int1))< .5){
	  dist_b = 0.5 - fabs(modf(rh_seg[5][it1],&int1));
	}else{
	  dist_b = fabs(modf(rh_seg[5][it1],&int1)) - .5;
	}
	if(dist_a < .1)hResidu2a_0p1->Fill(res1);
	if(dist_a > .1 && dist_a < .4)hResidu2a_0p1_0p4->Fill(res1);
	if(dist_a > .4)hResidu2a_0p4_0p5->Fill(res1);
	if(dist_b < .1)hResidu2b_0p1->Fill(res2);
	if(dist_b > .1 && dist_b < .4)hResidu2b_0p1_0p4->Fill(res2);
	if(dist_b > .4)hResidu2b_0p4_0p5->Fill(res2);
	if(dist_a < .1){
	  hPullsu2a_0p1->Fill(res1/sqrt(rh_sigm_seg[4][it1]));
	}
	if(dist_a > .1 && dist_a < .4){
	  hPullsu2a_0p1_0p4->Fill(res1/sqrt(rh_sigm_seg[4][it1]));
	}
	if(dist_a > .4){
	  hPullsu2a_0p4_0p5->Fill(res1/sqrt(rh_sigm_seg[4][it1]));
	}
	if(dist_b < .1){
	  hPullsu2b_0p1->Fill(res2/sqrt(rh_sigm_seg[5][it1]));
	}
	if(dist_b > .1 && dist_b < .4){
	  hPullsu2b_0p1_0p4->Fill(res2/sqrt(rh_sigm_seg[5][it1]));
	}
	if(dist_b > .4){
	  hPullsu2b_0p4_0p5->Fill(res2/sqrt(rh_sigm_seg[5][it1]));
	}
	hu2Da_Db_best->Fill(dist_a+dist_b-0.5);

	//v
	dist_a = 999;
	dist_b = 999;
	res1 = CalculateResidual(6, it1, rh_seg, par_ab);
	res2 = CalculateResidual(7, it1, rh_seg, par_ab);
	hResidv2a->Fill(res1);
	hResidv2b->Fill(res2);

	if(fabs(modf(rh_seg[6][it1],&int1))< .5){
	  dist_a = fabs(modf(rh_seg[6][it1],&int1));
	}else{
	  dist_a = 1 - fabs(modf(rh_seg[6][it1],&int1));
	}
	if(fabs(modf(rh_seg[7][it1],&int1))< .5){
	  dist_b = 0.5 - fabs(modf(rh_seg[7][it1],&int1));
	}else{
	  dist_b = fabs(modf(rh_seg[7][it1],&int1)) - .5;
	}
	if(dist_a < .1)hResidv2a_0p1->Fill(res1);
	if(dist_a > .1 && dist_a < .4)hResidv2a_0p1_0p4->Fill(res1);
	if(dist_a > .4)hResidv2a_0p4_0p5->Fill(res1);
	if(dist_b < .1)hResidv2b_0p1->Fill(res2);
	if(dist_b > .1 && dist_b < .4)hResidv2b_0p1_0p4->Fill(res2);
	if(dist_b > .4)hResidv2b_0p4_0p5->Fill(res2);
	if(dist_a < .1){
	  hPullsv2a_0p1->Fill(res1/sqrt(rh_sigm_seg[6][it1]));
	}
	if(dist_a > .1 && dist_a < .4){
	  hPullsv2a_0p1_0p4->Fill(res1/sqrt(rh_sigm_seg[6][it1]));
	}
	if(dist_a > .4){
	  hPullsv2a_0p4_0p5->Fill(res1/sqrt(rh_sigm_seg[6][it1]));
	}
	if(dist_b < .1){
	  hPullsv2b_0p1->Fill(res2/sqrt(rh_sigm_seg[7][it1]));
	}
	if(dist_b > .1 && dist_b < .4){
	  hPullsv2b_0p1_0p4->Fill(res2/sqrt(rh_sigm_seg[7][it1]));
	}
	if(dist_b > .4){
	  hPullsv2b_0p4_0p5->Fill(res2/sqrt(rh_sigm_seg[7][it1]));
	}
	hv2Da_Db_best->Fill(dist_a+dist_b-0.5);

	
      }//else
	
      //   end fill residuals
    }
  }
}

void BmnDchTrackFinder::FindSegmentTrackMCId(Int_t dchID, Int_t** rhId_seg, Double_t* chi2, Int_t* SegMC_Id, Int_t* SegMC_IdCount) {
  Int_t nDC_segments = (dchID == 1) ? nSegments[0] : nSegments[1];
  for (Int_t it1 = 0; it1 < nDC_segments; it1++) {
    if (chi2[it1] > 990.) continue;
    //calculate segment param errors
    
    Int_t mcId = -999;  
    int prev = -9;
    int count = 0;
    Int_t maxHits = 0;
    Int_t bestId = -99;
    
    for (int i = 0; i < 8; i++) {
      if (rhId_seg[i][it1] == -999 || rhId_seg[i][it1] == bestId) continue;
      count = 1;
      for (int j = i+1; j < 8; j++) {
	if (rhId_seg[j][it1] == -999) continue;
	if(rhId_seg[i][it1] == rhId_seg[j][it1])count++;
      }
      if(count>maxHits){
	maxHits = count;
	bestId = rhId_seg[i][it1];
      }
    }

    if(maxHits>3){ 
      SegMC_Id[it1] = bestId;
      SegMC_IdCount[it1] = maxHits;
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
    sigm_seg_par[0][it1] = sqrt(XSum / Xdelta);//ax
    sigm_seg_par[1][it1] = sqrt(XSumZZ / Xdelta);//bx
    sigm_seg_par[2][it1] = sqrt(YSum / Ydelta);//ay
    sigm_seg_par[3][it1] = sqrt(YSumZZ / Ydelta);//by
  }
}

void BmnDchTrackFinder::CreateDchTrack(Int_t dchID, Double_t* chi2Arr, Double_t** parArr, Int_t* sizeArr, Double_t** rh_seg, Double_t** rh_seg_time, Double_t** sigm_seg_par, Int_t* SegMC_Id, Int_t* SegMC_IdCount) {
  Int_t nDC_segments = (dchID == 1) ? nSegments[0] : nSegments[1];
  Int_t good_dc_segs = 0;
  Int_t best_seg1 = -1;
  Int_t best_chi1 = 1000.;
  Double_t sqrt_2 = sqrt(2.);
  Double_t isqrt_2 = 1. / sqrt_2;
  for (Int_t iSegment = 0; iSegment < nDC_segments; iSegment++) {
    if (chi2Arr[iSegment] > 990.)
      continue;
    good_dc_segs++;
    
    if(sizeArr[iSegment] == 8){
      if(chi2Arr[iSegment]<best_chi1){
	best_chi1 = chi2Arr[iSegment];
	best_seg1 = iSegment;
      }
    }
    FairTrackParam trackParam;
    Double_t z0 = (dchID == 1) ? Z_dch1 : Z_dch2;
    Double_t x0 = parArr[1][iSegment];
    Double_t y0 = parArr[3][iSegment];
    Double_t x_align = (dchID == 1) ? 0.0 : 0.0;//-10.89 : +4.93;
    Double_t y_align = (dchID == 1) ? 0.0 : 0.0;//-2.95 : +9.12;
    Double_t tx_align = (dchID == 1) ? 0.0 : 0.0;//+0.073 + 0.049 : +0.070 + 0.052;
    Double_t ty_align = (dchID == 1) ? 0.0 : 0.0;//+0.054 : +0.062 - 0.047;
    trackParam.SetPosition(TVector3(-x0 + x_align, y0 + y_align, z0));
    trackParam.SetTx(-parArr[0][iSegment] + tx_align);
    trackParam.SetTy(parArr[2][iSegment] + ty_align);
    trackParam.SetCovariance(0, 0, sigm_seg_par[1][iSegment] * sigm_seg_par[1][iSegment]);  // bx^2
    trackParam.SetCovariance(1, 1, sigm_seg_par[3][iSegment] * sigm_seg_par[3][iSegment]);  // by^2
    trackParam.SetCovariance(2, 2, sigm_seg_par[0][iSegment] * sigm_seg_par[0][iSegment]);  // tx^2
    trackParam.SetCovariance(3, 3, sigm_seg_par[2][iSegment] * sigm_seg_par[2][iSegment]);  // ty^2
    BmnDchTrack* track = new ((*fDchTracks)[fDchTracks->GetEntriesFast()]) BmnDchTrack();
    track->SetChi2(chi2Arr[iSegment]);
    if (dchID == 1) {
      hChi2ndf_dc1->Fill(chi2Arr[iSegment]);
    }else{
      hChi2ndf_dc2->Fill(chi2Arr[iSegment]);
    }
    track->SetTrackId(SegMC_Id[iSegment]);
    track->SetLength(SegMC_IdCount[iSegment]);
    track->SetFlag(iSegment);
    track->SetNHits(sizeArr[iSegment]);
    track->SetParamFirst(trackParam);
    if (dchID == 1) {
      haX1->Fill(-parArr[0][iSegment]);
      haY1->Fill(parArr[2][iSegment]);
      hx1->Fill(parArr[1][iSegment]);
      hy1->Fill(parArr[3][iSegment]);
      hlocXY1->Fill(rh_seg[0][iSegment], rh_seg[2][iSegment]);
      hXvsAx->Fill(parArr[1][iSegment], -parArr[0][iSegment]);
      hSegSize_dc1->Fill(sizeArr[iSegment]);
      for(int k = 0; k<8; k++){
	if( sizeArr[iSegment] == 6 && rh_seg[k][iSegment] == -999) hDenom1->Fill(k,1);
	if( sizeArr[iSegment] != 6){
	  if(rh_seg[k][iSegment] == -999){
	    hDenom1->Fill(k,1);
	  }else{
	    hNomin1->Fill(k,1);
	    hDenom1->Fill(k,1);
	  }
	}
      }//for

    } else {
      haX2->Fill(-parArr[0][iSegment]);
      haY2->Fill(parArr[2][iSegment]);
      hx2->Fill(parArr[1][iSegment]);
      hy2->Fill(parArr[3][iSegment]);
      hlocXY2->Fill(rh_seg[0][iSegment], rh_seg[2][iSegment]);
      hSegSize_dc2->Fill(sizeArr[iSegment]);
      for(int k = 0; k<8; k++){
	if( sizeArr[iSegment] == 6 && rh_seg[k][iSegment] == -999) hDenom2->Fill(k,1);
	if( sizeArr[iSegment] != 6){
	  if(rh_seg[k][iSegment] == -999){
	    hDenom2->Fill(k,1);
	  }else{
	    hNomin2->Fill(k,1);
	    hDenom2->Fill(k,1);
	  }
	}
      }//for

    }
  }//for
  if(dchID == 1  && best_seg1 != -1){
    hSlot_1xa_time8p->Fill(rh_seg_time[0][best_seg1]);
    hSlot_1xb_time8p->Fill(rh_seg_time[1][best_seg1]);
    hSlot_1ya_time8p->Fill(rh_seg_time[2][best_seg1]);
    hSlot_1yb_time8p->Fill(rh_seg_time[3][best_seg1]);
    hSlot_1ua_time8p->Fill(rh_seg_time[4][best_seg1]);
    hSlot_1ub_time8p->Fill(rh_seg_time[5][best_seg1]);
    hSlot_1va_time8p->Fill(rh_seg_time[6][best_seg1]);
    hSlot_1vb_time8p->Fill(rh_seg_time[7][best_seg1]);

    double vAtu = 4.8*(-.074) + (0.5*(rh_seg[6][best_seg1]+rh_seg[7][best_seg1]));
    double xAtu = (-9.6)*(-.107) + (0.5*(rh_seg[0][best_seg1]+rh_seg[1][best_seg1]));
    double uEst = vAtu - sqrt(2.)*xAtu;
    double diffU = 0.5*(rh_seg[4][best_seg1]+rh_seg[5][best_seg1]) - uEst;
    hUvsXV1->Fill(diffU);
      
      
    double vAty = 9.6*(-.074) + (0.5*(rh_seg[6][best_seg1]+rh_seg[7][best_seg1]));//-.0802
    double xAty = (-4.8)*(-.107) + (0.5*(rh_seg[0][best_seg1]+rh_seg[1][best_seg1]));//-.1135
    double yEst = sqrt(2.)*vAty - xAty;
    double diffY = 0.5*(rh_seg[2][best_seg1]+rh_seg[3][best_seg1]) - yEst;
    hYvsXV1->Fill(diffY);
      
    hAx12_loc->Fill((rh_seg[0][best_seg1]-rh_seg[1][best_seg1])/(z_loc[0]-z_loc[1]));
    hAy12_loc->Fill((rh_seg[2][best_seg1]-rh_seg[3][best_seg1])/(z_loc[2]-z_loc[3]));
    hAu12_loc->Fill((rh_seg[4][best_seg1]-rh_seg[5][best_seg1])/(z_loc[4]-z_loc[5]));
    hAv12_loc->Fill((rh_seg[6][best_seg1]-rh_seg[7][best_seg1])/(z_loc[6]-z_loc[7]));

    hAx1_loc->Fill(parArr[0][best_seg1]);
    hAy1_loc->Fill(parArr[2][best_seg1]);//
    hAu1_loc->Fill(isqrt_2*(parArr[2][best_seg1]-parArr[0][best_seg1]));
    hAv1_loc->Fill(isqrt_2*(parArr[2][best_seg1]+parArr[0][best_seg1]));
  }

  if(dchID == 2 && best_seg1 != -1){
    hSlot_2xa_time8p->Fill(rh_seg_time[0][best_seg1]);
    hSlot_2xb_time8p->Fill(rh_seg_time[1][best_seg1]);
    hSlot_2ya_time8p->Fill(rh_seg_time[2][best_seg1]);
    hSlot_2yb_time8p->Fill(rh_seg_time[3][best_seg1]);
    hSlot_2ua_time8p->Fill(rh_seg_time[4][best_seg1]);
    hSlot_2ub_time8p->Fill(rh_seg_time[5][best_seg1]);
    hSlot_2va_time8p->Fill(rh_seg_time[6][best_seg1]);
    hSlot_2vb_time8p->Fill(rh_seg_time[7][best_seg1]);
    double vAtu = 4.8*(-.075) + (0.5*(rh_seg[6][best_seg1]+rh_seg[7][best_seg1]));//-.0632
    double xAtu = (-9.6)*(-.103) + (0.5*(rh_seg[0][best_seg1]+rh_seg[1][best_seg1]));//-.0781
    double uEst = vAtu - sqrt(2.)*xAtu;
    double diffU = 0.5*(rh_seg[4][best_seg1]+rh_seg[5][best_seg1]) - uEst;
    hUvsXV2->Fill(diffU);
      
      
    double vAty = 9.6*(-.075) + (0.5*(rh_seg[6][best_seg1]+rh_seg[7][best_seg1]));
    double xAty = (-4.8)*(-.103) + (0.5*(rh_seg[0][best_seg1]+rh_seg[1][best_seg1]));
    double yEst = sqrt(2.)*vAty - xAty;
    double diffY = 0.5*(rh_seg[2][best_seg1]+rh_seg[3][best_seg1]) - yEst;
    hYvsXV2->Fill(diffY);
      

    hAx122loc->Fill((rh_seg[0][best_seg1]-rh_seg[1][best_seg1])/(z_loc[0]-z_loc[1]));
    hAy122loc->Fill((rh_seg[2][best_seg1]-rh_seg[3][best_seg1])/(z_loc[2]-z_loc[3]));
    hAu122loc->Fill((rh_seg[4][best_seg1]-rh_seg[5][best_seg1])/(z_loc[4]-z_loc[5]));
    hAv122loc->Fill((rh_seg[6][best_seg1]-rh_seg[7][best_seg1])/(z_loc[6]-z_loc[7]));

    hAx2_loc->Fill(parArr[0][best_seg1]);
    hAy2_loc->Fill(parArr[2][best_seg1]);//
    hAu2_loc->Fill(isqrt_2*(parArr[2][best_seg1]-parArr[0][best_seg1]));
    hAv2_loc->Fill(isqrt_2*(parArr[2][best_seg1]+parArr[0][best_seg1]));
  }

  if (dchID == 1 && layers_with >5) {
    Ntrack1->Fill(good_dc_segs);
  } 
  if(dchID == 2 && layers_with2 >5){
    Ntrack2->Fill(good_dc_segs);
  }
}

void BmnDchTrackFinder::CreateDchTrack() {
    
   if(layers_with >5 && layers_with2 >5)  NGtracks->Fill(nSegmentsToBeMatched+1);
  for (Int_t iSeg = 0; iSeg < nSegmentsToBeMatched + 1; iSeg++) {
    FairTrackParam trackParam;
    Double_t z0 = Z_dch_mid;
    Double_t x0 = -x_mid[iSeg];
    Double_t y0 = y_mid[iSeg];
    trackParam.SetPosition(TVector3(x0, y0, z0));  // Go to right reference frame
    trackParam.SetTx(aX[iSeg]);                          // Go to right reference frame
    trackParam.SetTy(aY[iSeg]);
    hP->Fill(0.3*2.84/(atan(aX[iSeg])));
    Int_t iseg1 = int((pairID[iSeg] - 1000) / 1000);
    Int_t iseg2 = (pairID[iSeg] % 1000) - 1;
    Double_t sigma2_dx = ((params_sigmas[1][1][iseg2] * params_sigmas[1][1][iseg2]) +
			  (params_sigmas[0][1][iseg1] * params_sigmas[0][1][iseg1])) *
      99.75 * 99.75 +
      ((params_sigmas[0][0][iseg1]) * (params_sigmas[0][0][iseg1])) +
      ((params_sigmas[1][0][iseg2]) * (params_sigmas[1][0][iseg2]));

    Double_t sigma2_dy = ((params_sigmas[1][3][iseg2] * params_sigmas[1][3][iseg2]) +
			  (params_sigmas[0][3][iseg1] * params_sigmas[0][3][iseg1])) *
      99.75 * 99.75 +
      ((params_sigmas[0][2][iseg1]) * (params_sigmas[0][2][iseg1])) +
      ((params_sigmas[1][2][iseg2]) * (params_sigmas[1][2][iseg2]));

    Double_t sigma2_dax = 9. * ((params_sigmas[1][0][iseg2] * params_sigmas[1][0][iseg2]) +
				(params_sigmas[0][0][iseg1] * params_sigmas[0][0][iseg1]));

    Double_t sigma2_day = 9. * ((params_sigmas[1][2][iseg2] * params_sigmas[1][2][iseg2]) +
				(params_sigmas[0][2][iseg1] * params_sigmas[0][2][iseg1]));
    trackParam.SetCovariance(0, 0, sigma2_dx);
    trackParam.SetCovariance(1, 1, sigma2_dy);
    trackParam.SetCovariance(2, 2, sigma2_dax);
    trackParam.SetCovariance(3, 3, sigma2_day);
    BmnDchTrack* track = new ((*fDchTracks)[fDchTracks->GetEntriesFast()]) BmnDchTrack();
    track->SetChi2(Chi2_match[iSeg]);
    track->SetNHits(nhits[iSeg]);
    track->SetFlag(pairID[iSeg]);
    track->SetParamFirst(trackParam);

    if(SegMCId[0][iseg1] != SegMCId[1][iseg2]){
      if(SegMCIdCount[0][iseg1]!=SegMCIdCount[1][iseg2]){
	if(SegMCIdCount[0][iseg1]>SegMCIdCount[1][iseg2]){
	  track->SetTrackId(SegMCId[0][iseg1]);
	  track->SetLength(SegMCIdCount[0][iseg1]);
	  if(layers_with >5 && layers_with2 >5) {
	    // if(SegMCId[0][iseg1]==0)NGtracks_beam->Fill(1);
	    // if(SegMCId[0][iseg1]>0)NGtracks_bkg->Fill(1);
          }
	}
	if(SegMCIdCount[0][iseg1]<SegMCIdCount[1][iseg2]){
	  track->SetTrackId(SegMCId[1][iseg2]);
	  track->SetLength(SegMCIdCount[1][iseg2]);
	  if(layers_with >5 && layers_with2 >5) {
	    //  if(SegMCId[1][iseg2]==0)NGtracks_beam->Fill(1);
	    //  if(SegMCId[1][iseg2]>0)NGtracks_bkg->Fill(1);
          }
	}
      }else{
	if(Chi2[0][iseg1]<Chi2[1][iseg2]){
	  track->SetTrackId(SegMCId[0][iseg1]);
	  track->SetLength(SegMCIdCount[0][iseg1]);
	  if(layers_with >5 && layers_with2 >5) {
	    // if(SegMCId[0][iseg1]==0)NGtracks_beam->Fill(1);
	    // if(SegMCId[0][iseg1]>0)NGtracks_bkg->Fill(1);
	  }
	}else{      
	  
	  track->SetTrackId(SegMCId[1][iseg2]);
	  track->SetLength(SegMCIdCount[1][iseg2]);
	  if(layers_with >5 && layers_with2 >5) {
	    // if(SegMCId[1][iseg2]==0)NGtracks_beam->Fill(1);
	    // if(SegMCId[1][iseg2]>0)NGtracks_bkg->Fill(1);
	  }
	}
      }
    }else{
      if(layers_with >5 && layers_with2 >5) {
	//	if(SegMCId[0][iseg1]==0)NGtracks_beam->Fill(1);
	//	if(SegMCId[0][iseg1]>0)NGtracks_bkg->Fill(1);
      }
      
      track->SetTrackId(SegMCId[0][iseg1]);
      track->SetLength(SegMCIdCount[0][iseg1]+SegMCIdCount[1][iseg2]);
    }
  }
}
Double_t BmnDchTrackFinder::CalculateResidualMatch(Int_t dchID, Int_t i, Int_t j, Double_t*** rh_seg, Double_t*** par_ab) {
  Double_t sqrt_2 = sqrt(2.);
  Double_t isqrt_2 = 1 / sqrt_2;

  return (i < 2) ? rh_seg[dchID][i][j] - z_loc[i] * par_ab[dchID][0][j] - par_ab[dchID][1][j] : (i >= 2 && i < 4) ? rh_seg[dchID][i][j] - z_loc[i] * par_ab[dchID][2][j] - par_ab[dchID][3][j] : (i >= 4 && i < 6) ? rh_seg[dchID][i][j] - isqrt_2 * z_loc[i] * (par_ab[dchID][2][j] - par_ab[dchID][0][j]) - isqrt_2 * (par_ab[dchID][3][j] - par_ab[dchID][1][j]) : rh_seg[dchID][i][j] - isqrt_2 * z_loc[i] * (par_ab[dchID][2][j] + par_ab[dchID][0][j]) - isqrt_2 * (par_ab[dchID][3][j] + par_ab[dchID][1][j]);
}

Double_t BmnDchTrackFinder::CalculateResidual(Int_t i, Int_t j, Double_t** rh_seg, Double_t** par_ab) {
  Double_t sqrt_2 = sqrt(2.);
  Double_t isqrt_2 = 1 / sqrt_2;

  return (i < 2) ? rh_seg[i][j] - z_loc[i] * par_ab[0][j] - par_ab[1][j] : (i >= 2 && i < 4) ? rh_seg[i][j] - z_loc[i] * par_ab[2][j] - par_ab[3][j] : (i >= 4 && i < 6) ? rh_seg[i][j] - isqrt_2 * z_loc[i] * (par_ab[2][j] - par_ab[0][j]) - isqrt_2 * (par_ab[3][j] - par_ab[1][j]) : rh_seg[i][j] - isqrt_2 * z_loc[i] * (par_ab[2][j] + par_ab[0][j]) - isqrt_2 * (par_ab[3][j] + par_ab[1][j]);
}

InitStatus BmnDchTrackFinder::Init() {
  if (!expData) {
     cout << "BmnDchTrackFinder::Init(): simulation data is reconstructed " << endl;
    SetActive(kTRUE);
  }

  if (fVerbose) cout << "BmnDchTrackFinder::Init()" << endl;
  FairRootManager* ioman = FairRootManager::Instance();

  if (expData) {
    fBmnDchDigitsArray = (TClonesArray*)ioman->GetObject(InputDigitsBranchName);
    if (!fBmnDchDigitsArray) {
      cout << "BmnDchTrackFinder::Init(): branch " << InputDigitsBranchName << " not found! Task will be deactivated" << endl;
      SetActive(kFALSE);
      return kERROR;
    }
  }else{
    fBmnHitsArray = (TClonesArray*)ioman->GetObject(InputDigitsBranchName);
    if (!fBmnHitsArray) {
      cout << "BmnDchTrackFinder::Init(): branch " << InputDigitsBranchName << " not found! Task will be deactivated" << endl;
      SetActive(kFALSE);
      return kERROR;
    }
  }

  initHists();

  // Create and register track arrays
  fDchTracks = new TClonesArray(tracksDch.Data());
  ioman->Register(tracksDch.Data(), "DCH", fDchTracks, kTRUE);

  if (expData) {//transfer function read for experimental data
    ifstream fin;
    TString dir = getenv("VMCWORKDIR");
    dir += "/input/";
    fin.open((TString(dir + fTransferFunctionName)).Data(), ios::in);
    cout << "Transfer function for DCH: " << fTransferFunctionName << endl;
    for (Int_t fi = 0; fi < 16; fi++) {
      fin >> t_dc[0][fi] >> t_dc[1][fi] >> t_dc[2][fi] >> t_dc[3][fi] >>
	pol_par_dc[0][0][fi] >> pol_par_dc[0][1][fi] >> pol_par_dc[0][2][fi] >> pol_par_dc[0][3][fi] >> pol_par_dc[0][4][fi] >>
	pol_par_dc[1][0][fi] >> pol_par_dc[1][1][fi] >> pol_par_dc[1][2][fi] >> pol_par_dc[1][3][fi] >> pol_par_dc[1][4][fi] >>
	pol_par_dc[2][0][fi] >> pol_par_dc[2][1][fi] >> pol_par_dc[2][2][fi] >> pol_par_dc[2][3][fi] >> pol_par_dc[2][4][fi] >>
	scaling[fi];
      if (fVerbose) {
	cout<< t_dc[0][fi] << t_dc[1][fi] << t_dc[2][fi] << t_dc[3][fi] <<
	  pol_par_dc[0][0][fi] << pol_par_dc[0][1][fi] << pol_par_dc[0][2][fi] << pol_par_dc[0][3][fi] << pol_par_dc[0][4][fi] <<
	  pol_par_dc[1][0][fi] << pol_par_dc[1][1][fi] << pol_par_dc[1][2][fi] << pol_par_dc[1][3][fi] << pol_par_dc[1][4][fi] <<
	  pol_par_dc[2][0][fi] << pol_par_dc[2][1][fi] << pol_par_dc[2][2][fi] << pol_par_dc[2][3][fi] << pol_par_dc[2][4][fi] <<endl;
      }
    }
    fin.close();
  }

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
  vId = new Int_t**[nChambers];
  uId = new Int_t**[nChambers];
  yId = new Int_t**[nChambers];
  xId = new Int_t**[nChambers];
  v_time = new Double_t**[nChambers];
  u_time = new Double_t**[nChambers];
  y_time = new Double_t**[nChambers];
  x_time = new Double_t**[nChambers];
  v_Single = new Double_t**[nChambers];
  u_Single = new Double_t**[nChambers];
  y_Single = new Double_t**[nChambers];
  x_Single = new Double_t**[nChambers];
  v_SingleId = new Int_t**[nChambers];
  u_SingleId = new Int_t**[nChambers];
  y_SingleId = new Int_t**[nChambers];
  x_SingleId = new Int_t**[nChambers];
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
  SegMCId = new Int_t*[nChambers];
  SegMCIdCount = new Int_t*[nChambers];
  x_global = new Double_t*[nChambers];
  y_global = new Double_t*[nChambers];
  params = new Double_t**[nChambers];
  params_sigmas = new Double_t**[nChambers];
  rh_segment = new Double_t**[nChambers];
  rhId_segment = new Int_t**[nChambers];
  rh_segment_time = new Double_t**[nChambers];
  rh_sigm_segment = new Double_t**[nChambers];
  for (Int_t iChamber = 0; iChamber < nChambers; iChamber++) {
    v[iChamber] = new Double_t*[N];
    u[iChamber] = new Double_t*[N];
    y[iChamber] = new Double_t*[N];
    x[iChamber] = new Double_t*[N];
    vId[iChamber] = new Int_t*[N];
    uId[iChamber] = new Int_t*[N];
    yId[iChamber] = new Int_t*[N];
    xId[iChamber] = new Int_t*[N];
    v_time[iChamber] = new Double_t*[N];
    u_time[iChamber] = new Double_t*[N];
    y_time[iChamber] = new Double_t*[N];
    x_time[iChamber] = new Double_t*[N];
    v_Single[iChamber] = new Double_t*[N];
    u_Single[iChamber] = new Double_t*[N];
    y_Single[iChamber] = new Double_t*[N];
    x_Single[iChamber] = new Double_t*[N];
    v_SingleId[iChamber] = new Int_t*[N];
    u_SingleId[iChamber] = new Int_t*[N];
    y_SingleId[iChamber] = new Int_t*[N];
    x_SingleId[iChamber] = new Int_t*[N];
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
    SegMCId[iChamber] = new Int_t[75 * N];
    SegMCIdCount[iChamber] = new Int_t[75 * N];
    x_global[iChamber] = new Double_t[75 * N];
    y_global[iChamber] = new Double_t[75 * N];
    params[iChamber] = new Double_t*[4];
    params_sigmas[iChamber] = new Double_t*[4];
    rh_segment[iChamber] = new Double_t*[8];
    rhId_segment[iChamber] = new Int_t*[8];
    rh_segment_time[iChamber] = new Double_t*[8];
    rh_sigm_segment[iChamber] = new Double_t*[8];
    for (Int_t iDim = 0; iDim < N; iDim++) {
      v[iChamber][iDim] = new Double_t[75 * N];
      u[iChamber][iDim] = new Double_t[75 * N];
      y[iChamber][iDim] = new Double_t[75 * N];
      x[iChamber][iDim] = new Double_t[75 * N];
      vId[iChamber][iDim] = new Int_t[75 * N];
      uId[iChamber][iDim] = new Int_t[75 * N];
      yId[iChamber][iDim] = new Int_t[75 * N];
      xId[iChamber][iDim] = new Int_t[75 * N];
      v_time[iChamber][iDim] = new Double_t[75 * N];
      u_time[iChamber][iDim] = new Double_t[75 * N];
      y_time[iChamber][iDim] = new Double_t[75 * N];
      x_time[iChamber][iDim] = new Double_t[75 * N];
      v_Single[iChamber][iDim] = new Double_t[20 * N];
      u_Single[iChamber][iDim] = new Double_t[20 * N];
      y_Single[iChamber][iDim] = new Double_t[20 * N];
      x_Single[iChamber][iDim] = new Double_t[20 * N];
      v_SingleId[iChamber][iDim] = new Int_t[20 * N];
      u_SingleId[iChamber][iDim] = new Int_t[20 * N];
      y_SingleId[iChamber][iDim] = new Int_t[20 * N];
      x_SingleId[iChamber][iDim] = new Int_t[20 * N];
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
      rhId_segment[iChamber][iDim] = new Int_t[75 * N];
      rh_segment_time[iChamber][iDim] = new Double_t[75 * N];
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

  isInitialized = true;

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
      SegMCId[iChamber][iDim1] = -999;
      SegMCIdCount[iChamber][iDim1] = 0;
      x_global[iChamber][iDim1] = -999.;
      y_global[iChamber][iDim1] = -999.;
    }
    for (Int_t iDim1 = 0; iDim1 < N; iDim1++) {
      for (Int_t iDim2 = 0; iDim2 < 75 * N; iDim2++) {
	v[iChamber][iDim1][iDim2] = -999.;
	u[iChamber][iDim1][iDim2] = -999.;
	y[iChamber][iDim1][iDim2] = -999.;
	x[iChamber][iDim1][iDim2] = -999.;
	vId[iChamber][iDim1][iDim2] = -999;
	uId[iChamber][iDim1][iDim2] = -999;
	yId[iChamber][iDim1][iDim2] = -999;
	xId[iChamber][iDim1][iDim2] = -999;
	v_time[iChamber][iDim1][iDim2] = -999.;
	u_time[iChamber][iDim1][iDim2] = -999.;
	y_time[iChamber][iDim1][iDim2] = -999.;
	x_time[iChamber][iDim1][iDim2] = -999.;
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
	v_SingleId[iChamber][iDim1][iDim3] = -999;
	u_SingleId[iChamber][iDim1][iDim3] = -999;
	y_SingleId[iChamber][iDim1][iDim3] = -999;
	x_SingleId[iChamber][iDim1][iDim3] = -999;
	Sigm_v_single[iChamber][iDim1][iDim3] = 1.;
	Sigm_u_single[iChamber][iDim1][iDim3] = 1.;
	Sigm_y_single[iChamber][iDim1][iDim3] = 1.;
	Sigm_x_single[iChamber][iDim1][iDim3] = 1.;
      }
    }
    for (Int_t iDim1 = 0; iDim1 < 8; iDim1++)
      for (Int_t iDim2 = 0; iDim2 < 75 * N; iDim2++) {
	rh_segment[iChamber][iDim1][iDim2] = -999.;
	rhId_segment[iChamber][iDim1][iDim2] = -999;
	rh_segment_time[iChamber][iDim1][iDim2] = -999.;
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

  if (fVerbose) {
    TFile file(fhTestFlnm.Data(), "RECREATE");
     
    hEff1->Divide(hNomin1,hDenom1,1.,1.);
    hEff2->Divide(hNomin2,hDenom2,1.,1.);
      
    fhList.Write();
    file.Close();
  }
  //===============================================================================================================

  ///end

  cout << "Work time of the DCH track finder: " << workTime << " s" << endl;
}

Int_t BmnDchTrackFinder::Reconstruction(Int_t dchID, TString wire, Int_t pair, Int_t it_a, Int_t it_b,
                                        Double_t* wirenr_a, Double_t* wirenr_b, Double_t* time_a, Double_t* time_b,
                                        Bool_t* used_a, Bool_t* used_b,
                                        Double_t** _ab, Double_t** sigm_ab, Double_t** ab_time) {
  //cout<<"61"<<endl;
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

     if(fPeriod == 7 && fRunId > 3589 && fabs(d_a+d_b -.5)>.35) continue;//get rid of bad timing scale

      Double_t dmin2 = LDBL_MAX;
      Double_t dmin1 = Abs(a_pm[0] - b_pm[0]);//set first pair as one of the minima
      Double_t a_coord1 = a_pm[0];
      Double_t b_coord1 = b_pm[0];
      Double_t a_coord2 = LDBL_MAX;
      Double_t b_coord2 = LDBL_MAX;
      for (Int_t k = 0; k < 2; k++){
	for (Int_t m = 0; m < 2; m++){
	  if(k == 0 && m == 0)continue;
	  if (Abs(a_pm[k] - b_pm[m]) < dmin1) {
	    dmin2 = dmin1;
	    dmin1 = Abs(a_pm[k] - b_pm[m]);
	    a_coord2 = a_coord1;
	    b_coord2 = b_coord1;
	    a_coord1 = a_pm[k];
	    b_coord1 = b_pm[m];
		  
	  }else if(Abs(a_pm[k] - b_pm[m]) < dmin2 && Abs(a_pm[k] - b_pm[m]) != dmin1){
	    dmin2 = Abs(a_pm[k] - b_pm[m]);
	    a_coord2 = a_pm[k];
	    b_coord2 = b_pm[m];
	  }
	}
      }
	    
      _ab[0][pair] = a_coord1;
      _ab[1][pair] = b_coord1;
      _ab[0][pair+1] = a_coord2;
      _ab[1][pair+1] = b_coord2;
	    
      ab_time[0][pair] = time_a[i];
      ab_time[1][pair] = time_b[j];

      if (arrIdxStart == 0) {
	if (arrIdxShift == 0) hx1Da_Db->Fill(d_a + d_b - .5);
	if (arrIdxShift == 8) hx2Da_Db->Fill(d_a + d_b - .5);
	      
      }
      if (arrIdxStart == 4) {
	if (arrIdxShift == 0) hu1Da_Db->Fill(d_a + d_b - .5);
	if (arrIdxShift == 8) hu2Da_Db->Fill(d_a + d_b - .5);
      }
      if (arrIdxStart == 6) {
	if (arrIdxShift == 0) hv1Da_Db->Fill(d_a + d_b - .5);
	if (arrIdxShift == 8) hv2Da_Db->Fill(d_a + d_b - .5);
      }
	    
      if (arrIdxStart == 2) {
	if (arrIdxShift == 0) hy1Da_Db->Fill(d_a + d_b - .5);
	if (arrIdxShift == 8) hy2Da_Db->Fill(d_a + d_b - .5);
      }
	    
      //add shifts for y and u
      //dc1
      if (arrIdxStart == 2 && arrIdxShift == 0) {
	_ab[0][pair] -= .461; 
	_ab[1][pair] -= .461;  
      }
      if (arrIdxStart == 4 && arrIdxShift == 0) {
	_ab[0][pair] -= 1.056; 
	_ab[1][pair] -= 1.056;  
      }
      //dc2
      if (arrIdxStart == 2 && arrIdxShift == 8) {
	_ab[0][pair] -= .346;
	_ab[1][pair] -= .346;
      }
      if (arrIdxStart == 4 && arrIdxShift == 8) {
	_ab[0][pair] -= .966;
	_ab[1][pair] -= .966;
      }
      //dc1
      if (arrIdxStart == 2 && arrIdxShift == 0) {
	_ab[0][pair+1] -= .461; 
	_ab[1][pair+1] -= .461;  
      }
      if (arrIdxStart == 4 && arrIdxShift == 0) {
	_ab[0][pair+1] -= 1.056; 
	_ab[1][pair+1] -= 1.056;  
      }
      //dc2
      if (arrIdxStart == 2 && arrIdxShift == 8) {
	_ab[0][pair+1] -= .346;
	_ab[1][pair+1] -= .346;
      }
      if (arrIdxStart == 4 && arrIdxShift == 8) {
	_ab[0][pair+1] -= .966;
	_ab[1][pair+1] -= .966;
      }
	    
      //end
      CompareDaDb(d_a, sigm_ab[0][pair]);
      CompareDaDb(d_b, sigm_ab[1][pair]);
      CompareDaDb(d_a, sigm_ab[0][pair+1]);
      CompareDaDb(d_b, sigm_ab[1][pair+1]);
	    
      pair += 2;

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

    //add shifts for y and u
    //dc1
    if (arrIdxStart == 2 && arrIdx2 == 0) {
      _single[0 + arrIdx1][single] -= .461;
      _single[0 + arrIdx1][single + 1] -= .461;
    }
    if (arrIdxStart == 4 && arrIdx2 == 0) {
      _single[0 + arrIdx1][single] -= 1.056; 
      _single[0 + arrIdx1][single+1] -= 1.056;  
    }
    //dc2
    if (arrIdxStart == 2 && arrIdx2 == 8) {
      _single[0 + arrIdx1][single] -= .346; 
      _single[0 + arrIdx1][single+1] -= .346;  
    }
    if (arrIdxStart == 4 && arrIdx2 == 8) {
      _single[0 + arrIdx1][single] -= .966;
      _single[0 + arrIdx1][single+1] -= .966;  
    }

    CompareDaDb(d, sigm_single[0 + arrIdx1][single], sigm_single[0 + arrIdx1][single + 1]);

    single += 2;
  }
  return single;
}

Int_t BmnDchTrackFinder::ReconstructionMC(Int_t dchID, TString wire, Int_t pair, Int_t it_b, Int_t it_a,
					  Double_t* wirenr_b, Double_t* wirenr_a, Double_t* time_b, Double_t* time_a,
					  Int_t* hitId_b, Int_t* hitId_a, Bool_t* used_b, Bool_t* used_a,
					  Double_t** _ab, Int_t** _abId, Double_t** sigm_ab, Double_t** ab_time) {
  //cout<<"61"<<endl;
  const Int_t arrIdxShift = (dchID == 2) ? 8 : 0;
  const Int_t arrIdxStart = (wire == "x") ? 0 : (wire == "y") ? 2 : (wire == "u") ? 4 : 6;
   
  Double_t a_pm[2], b_pm[2];

  for (Int_t i = 0; i < it_a; ++i)
    for (Int_t j = 0; j < it_b; ++j) {
      if (pair > 25 * N - 1)
	break;
      if ((wirenr_a[i] != wirenr_b[j] && wirenr_a[i] != wirenr_b[j] + 1))
	continue;
      
      Double_t d_a = time_a[i];
      Double_t d_b = time_b[j];
      a_pm[0] = wirenr_a[i] - 119 + d_a;
      a_pm[1] = wirenr_a[i] - 119 - d_a;
      b_pm[0] = wirenr_b[j] - 118.5 + d_b;
      b_pm[1] = wirenr_b[j] - 118.5 - d_b;
	 	    
      Double_t dmin2 = LDBL_MAX;
      Double_t dmin1 = Abs(a_pm[0] - b_pm[0]);//set first pair as one of the minima
      Double_t a_coord1 = a_pm[0];
      Double_t b_coord1 = b_pm[0];
      Double_t a_coord2 = LDBL_MAX;
      Double_t b_coord2 = LDBL_MAX;
      for (Int_t k = 0; k < 2; k++){
	for (Int_t m = 0; m < 2; m++){
	  if(k == 0 && m == 0)continue;
	  if (Abs(a_pm[k] - b_pm[m]) < dmin1) {
	    dmin2 = dmin1;
	    dmin1 = Abs(a_pm[k] - b_pm[m]);
	    a_coord2 = a_coord1;
	    b_coord2 = b_coord1;
	    a_coord1 = a_pm[k];
	    b_coord1 = b_pm[m];
		  
	  }else if(Abs(a_pm[k] - b_pm[m]) < dmin2 && Abs(a_pm[k] - b_pm[m]) != dmin1){
	    dmin2 = Abs(a_pm[k] - b_pm[m]);
	    a_coord2 = a_pm[k];
	    b_coord2 = b_pm[m];
	  }
	}
      }
	    
      _ab[0][pair] = a_coord1;
      _ab[1][pair] = b_coord1;
      _ab[0][pair+1] = a_coord2;
      _ab[1][pair+1] = b_coord2;

      _abId[0][pair] = hitId_a[i];
      _abId[1][pair] =  hitId_b[j];
      _abId[0][pair+1] =  hitId_a[i];
      _abId[1][pair+1] =  hitId_b[j];
	    
      ab_time[0][pair] = time_a[i];
      ab_time[1][pair] = time_b[j];

      if (arrIdxStart == 0) {
	if (arrIdxShift == 0) hx1Da_Db->Fill(d_a + d_b - .5);
	if (arrIdxShift == 8) hx2Da_Db->Fill(d_a + d_b - .5);
	      
      }
      if (arrIdxStart == 4) {
	if (arrIdxShift == 0) hu1Da_Db->Fill(d_a + d_b - .5);
	if (arrIdxShift == 8) hu2Da_Db->Fill(d_a + d_b - .5);
      }
      if (arrIdxStart == 6) {
	if (arrIdxShift == 0) hv1Da_Db->Fill(d_a + d_b - .5);
	if (arrIdxShift == 8) hv2Da_Db->Fill(d_a + d_b - .5);
      }
	    
      if (arrIdxStart == 2) {
	if (arrIdxShift == 0) hy1Da_Db->Fill(d_a + d_b - .5);
	if (arrIdxShift == 8) hy2Da_Db->Fill(d_a + d_b - .5);
      }
     
      CompareDaDb(d_a, sigm_ab[0][pair]);
      CompareDaDb(d_b, sigm_ab[1][pair]);
      CompareDaDb(d_a, sigm_ab[0][pair+1]);
      CompareDaDb(d_b, sigm_ab[1][pair+1]);
	    
      pair += 2;

      used_a[i] = kTRUE;
      used_b[j] = kTRUE;
    }
  return pair;
}

Int_t BmnDchTrackFinder::ReconstructionSingleMC(Int_t dchID, TString wire, TString lay, Int_t single, Int_t it,
						Double_t* wirenr, Double_t* time_, Int_t* hitId, Bool_t* used,
						Double_t** _single, Int_t** _singleId, Double_t** sigm_single) {
  const Int_t arrIdxStart = (wire == "x") ? 0 : (wire == "y") ? 2 : (wire == "u") ? 4 : 6;

  const Int_t arrIdx1 = (lay == "a") ? 0 : 1;
  const Int_t arrIdx2 = (dchID == 2) ? 8 : 0;
  const Double_t coeff = (lay == "a") ? 119 : 118.5;

  for (Int_t i = 0; i < it; ++i) {
    if (used[i])
      continue;

    Double_t d = time_[i];
    
    _single[0 + arrIdx1][single] = wirenr[i] - coeff + d;
    _single[0 + arrIdx1][single + 1] = wirenr[i] - coeff - d;
    _singleId[0 + arrIdx1][single] =  hitId[i];
    _singleId[0 + arrIdx1][single + 1] = hitId[i];

    CompareDaDb(d, sigm_single[0 + arrIdx1][single], sigm_single[0 + arrIdx1][single + 1]);

    single += 2;
  }
  return single;
}
ClassImp(BmnDchTrackFinder)
