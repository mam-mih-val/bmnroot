// Author: Vasilisa Lenivenko <vasilisa@jinr.ru> 2018-07-18

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// BmnMwpcTrackFinder                                                         //
//                                                                            //
//                                                                            //
//Implementation of an algorithm developed by                                 //
// Vasilisa Lenivenko and Vladimir Palchik                                    //
// to the BmnRoot software                                                    //
//                                                                            //
// The algorithm serves for searching for track segments                      //
// in the MWPC of the BM@N experiment                                         //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
#include <Rtypes.h>
#include "TFile.h"
#include "BmnMwpcTrack.h"
#include "BmnMwpcSegment.h"
#include "BmnMwpcTrackFinder.h"
#include "BmnMwpcSegment.h"

using namespace std;
static Float_t workTime = 0.0;

struct match {
  Double_t Chi2m   = 0.;
  Int_t  Ind1   = -1;
  Int_t  Ind2   = -1;
  Int_t  Nhits1  = 0;
  Int_t  Nhits2  = 0;
  Double_t param1[4] = { 999., 999., 999., 999.};
  Double_t param2[4] = { 999., 999., 999., 999.};
  Float_t distX   = -1.;
  Float_t distY   = -1.;
  Float_t distAX   = -1.;
  Float_t distAY   = -1.;
};

bool compareSegments(const match &a, const match &b) {
  return a.Chi2m < b.Chi2m;
}


BmnMwpcTrackFinder::BmnMwpcTrackFinder(Bool_t isExp, Int_t runP, Int_t runNumber) :
  fEventNo(0),
  expData(isExp) {
    
  if(expData){ 
    fRunPeriod = runP;
    fRunNumber = runNumber;
    
    if (fRunPeriod == 6 || (fRunPeriod == 7 && fRunNumber > 3588) ) {//BMN
      kNumPairs = 1;
      kCh_max = 2;
    } else if ( fRunPeriod == 7 && fRunNumber <= 3588 || fRunPeriod == 8) {//SRC
      kNumPairs = 2;
      kCh_max = 4;
    }
  }else{
    fInputBranchNameHit = "BmnMwpcHit";
    kNumPairs    = 2;
    kCh_max      = 4;
    fRunPeriod   = 7;
    fRunNumber   = 0001;
  }
  
  fMwpcGeo = new BmnMwpcGeometrySRC(fRunPeriod, fRunNumber);
  fInputBranchName  = "BmnMwpcSegment";
  fOutputBranchName = "BmnMwpcTrack";
  kBig = 100;
  kCh_min = 0;
}

BmnMwpcTrackFinder::~BmnMwpcTrackFinder() {

  for(Int_t iCh = 0; iCh<kNChambers; ++iCh){
    for(Int_t iPlane = 0; iPlane<kNPlanes; ++iPlane){
      delete[] XVU_Ch[iCh][iPlane];
      delete[] Clust_Ch[iCh][iPlane]; 
    }
    for(Int_t i = 0; i<4; ++i){
     delete[] par_ab_Ch[iCh][i]; 
    }
    delete[] XVU_Ch[iCh];
    delete[] Clust_Ch[iCh];
    delete[] par_ab_Ch[iCh];
    delete[] shift[iCh];
    delete[] kPln[iCh];
    delete[] kZ_loc[iCh];
    delete[] z_gl[iCh];
    delete[] Chi2_ndf_Ch[iCh];
    delete[] Nhits_Ch[iCh];
    delete[] Nhits_match[iCh];
    delete[] ind_best_Ch[iCh];
  }

  for(Int_t iPair = 0; iPair<kNumPairs; ++iPair){
    for(Int_t iBig = 0; iBig<kBig; ++iBig){
      for(Int_t i = 0; i<4; ++i){
        delete[] sigma2_pair[iPair][iBig][i];
      }
      delete[]  sigma2_pair[iPair][iBig];
    }

    for(Int_t i = 0; i<4; ++i){
      delete[] par_ab_pair[iPair][i];
    }

    delete[] sigma2_pair[iPair];
    delete[] par_ab_pair[iPair];
    delete[] shift_pair[iPair];
    delete[] Chi2_match_pair[iPair];
    delete[] Chi2_ndf_pair[iPair];
    delete[] ind_best_pair[iPair];
    delete[] Nhits_pair[iPair];
    delete[] sigma_delta[iPair];
  }

    for(Int_t ii=0; ii<4; ii++) {
    delete[] Amatr[ii];
    delete[] bmatr[ii];
  }

  delete[] Amatr;
  delete[] bmatr;
  
  delete[] kPln;
  delete[] kZ_loc;
  delete[] z_gl;
  delete[] XVU_Ch;
  delete[] Clust_Ch;
  delete[] par_ab_Ch;
  delete[] par_ab_pair;
  delete[] sigma2_pair;
  delete[] shift;
  delete[] shift_pair;
  delete[] kZ_midle_pair;

  delete[] ChCent;
  delete[] ZCh;
  delete[] kZmid;
  delete[] Chi2_match_pair;
  delete[] Chi2_ndf_pair;
  delete[] ind_best_pair;
  delete[] Chi2_ndf_Ch;
  delete[] Nhits_Ch;
  delete[] Nhits_match;
  delete[] Nhits_pair;
  delete[] sigm2;
  delete[] ipl;
  delete[] Nbest_pair;
  delete[] Nbest_Ch;
  delete[] ind_best_Ch;
  delete[] sigma_delta;

}

void BmnMwpcTrackFinder::Exec(Option_t* opt) {
  if (!IsActive()) return;
  clock_t tStart = clock();
  PrepareArraysToProcessEvent();
  if (fDebug) cout << "\n======================== MWPC track finder exec started ===================\n" << endl;
  if (fDebug) cout << "Event number: " << fEventNo++ << endl;
  
  
  ReadSegments(par_ab_Ch, Nhits_Ch, Chi2_ndf_Ch, Nbest_Ch, XVU_Ch, Clust_Ch, vec_points);


  //--------Track-Segment  matching between chambers--------------------
  if ( Nbest_Ch[0] > 0 && Nbest_Ch[1] > 0) {
    if (fDebug) cout<<"Reco: N0= "<<Nbest_Ch[0]<<" N1= "<<Nbest_Ch[1]<<endl;
    SegmentMatching( 0, Nbest_Ch, par_ab_Ch, kZmid, ind_best_Ch, Nbest_pair, Chi2_match_pair, XVU_Ch, Nhits_Ch, Nhits_match);
  }

  if (kNChambers > 2 && Nbest_Ch[2] > 0 && Nbest_Ch[3] > 0) {
    if (fDebug) cout<<"Reco: N2= "<<Nbest_Ch[2]<<" N3= "<<Nbest_Ch[3]<<endl;
    SegmentMatchingAfterTarget(2, Nbest_Ch, par_ab_Ch, kZmid, ind_best_Ch, Nbest_pair, Chi2_match_pair, XVU_Ch, Nhits_Ch, Nhits_match);
  }
  
  if (fDebug){
    // printf("kNumPairs: %d\n", kNumPairs);
    for (Int_t p = 0; p < kNumPairs; p++) {
      for (Int_t se = 0; se < kmaxPairs; se++) {
        if (Chi2_match_pair[p][se] != 999.) {
          hChi2_match_pair.at(p) -> Fill( Chi2_match_pair[p][se]);
          //cout<<" pair "<<p<<" se "<<se<<" Chi2_match "<<Chi2_match_pair[p][se]<<endl;
        }
      }
      //cout<<"Nbest_pair["<<p<<"]= "<<Nbest_pair[p]<<endl;
      if ( Nbest_pair[p] ) {
        hNbest_pair.at(p)->Fill(Nbest_pair[p]);
      }
    }//p
  }
  //--------------------------------------------------------------------


  // ----------------Segment from 2-stations Fit-------------------------
  Int_t First_Chamber = 0;
  if (fDebug) cout<<" kNumPairs "<<kNumPairs<<" Nbest_pair[0] "<<Nbest_pair[0]<<" Nbest_pair[1] "<<Nbest_pair[1]<<endl;
  if (kNumPairs > 0 && Nbest_pair[0] > 0) {
    if (fDebug) cout<<" Nbest_pair[0] "<<Nbest_pair[0]<<endl;
    SegmentFit(First_Chamber, z_gl, sigm2, Nbest_pair, ind_best_Ch, par_ab_pair, Chi2_ndf_pair, XVU_Ch, Clust_Ch, ind_best_pair, Nhits_Ch, Nhits_pair,sigma2_pair);
  }

  if (kNumPairs == 2 && Nbest_pair[1] > 0) {
    //if (fDebug) cout<<" Nbest_pair[1] "<<Nbest_pair[1]<<endl;
    SegmentFit(First_Chamber+2, z_gl, sigm2, Nbest_pair, ind_best_Ch, par_ab_pair, Chi2_ndf_pair, XVU_Ch, Clust_Ch, ind_best_pair, Nhits_Ch, Nhits_pair,sigma2_pair);
  }
  //--------------------------------------------------------------------
  if(!expData) MCefficiencyCalculation(vec_points, Nbest_pair, par_ab_pair);
  //------------------Segment Parameters Alignment----------------------
  for (Int_t iPair = 0; iPair < kNumPairs; iPair++) {
    if ( Nbest_pair[iPair] > 0 ) SegmentParamAlignment(iPair, Nbest_pair, par_ab_pair, shift_pair);
  }
  //--------------------------------------------------------------------

  //--------------------MWPC pairs matching ----------------------------
   // if ( Nbest_pair[0] > 0 && Nbest_pair[1] > 0 ) PairMatching(Nbest_pair, par_ab_pair, kZ_midle_pair);
  //--------------------------------------------------------------------
  
  TracksStoring(Nbest_pair, par_ab_pair, Chi2_ndf_pair, Nhits_pair, sigma2_pair);


  if (fDebug) cout << "\n======================== MWPC track finder exec finished ==================" << endl;
  clock_t tFinish = clock();
  workTime += ((Float_t) (tFinish - tStart)) / CLOCKS_PER_SEC;
}//Exec
//----------------------------------------------------------------------------------

void BmnMwpcTrackFinder::TracksStoring(Int_t *Nbest, Double_t ***par_ab, Double_t **Chi2_ndf,  Int_t **Nhits, Double_t****sigma2){
  Double_t theta, phi;
  Float_t X_par_to_target, Y_par_to_target;
  for (Int_t iPair = 0; iPair < kNumPairs; iPair++) {
    if ( Nbest[iPair] > 0) {
      if (fDebug)cout<<" Nbest["<<iPair<<"] "<<Nbest[iPair]<<endl;
      for (Int_t itr = 0; itr < Nbest[iPair]; itr++) {
        if (fDebug)cout<<" pair "<<itr<<" Chi2_ndf_pair "<<Chi2_ndf[iPair][itr]<<endl;
        
        if (Chi2_ndf[iPair][itr] > 1000.) continue;
        
        if (fDebug){
          cout<<" Chi2_ndf_pair "<<Chi2_ndf[iPair][itr]<<" Nhits_pair["<<iPair<<"]["<<itr<<"] "<<Nhits[iPair][itr]<<endl;
          X_par_to_target = par_ab[iPair][0][itr]*( Z0_SRC - kZ_midle_pair[iPair]) + par_ab[iPair][1][itr];
          Y_par_to_target = par_ab[iPair][2][itr]*( Z0_SRC - kZ_midle_pair[iPair]) + par_ab[iPair][3][itr];
          phi = TMath::ATan2(par_ab[iPair][2][itr],par_ab[iPair][0][itr]); // phi = arctan(tgy/tgx)
          theta = TMath::ATan2(par_ab[iPair][0][itr], TMath::Cos(phi));// theta = arctan(tgx/cos(phi))
          hpar_Ax_pair.at(iPair)   -> Fill(TMath::RadToDeg()* par_ab[iPair][0][itr]);
          hpar_Bx_pair.at(iPair)   -> Fill( par_ab[iPair][1][itr]);
          hpar_Ay_pair.at(iPair)   -> Fill(TMath::RadToDeg()* par_ab[iPair][2][itr]);
          hpar_By_pair.at(iPair)   -> Fill(par_ab[iPair][3][itr]);
          hYvsX_pair.at(iPair)     -> Fill( par_ab[iPair][1][itr],par_ab[iPair][3][itr]);
          
          hpar_theta_pair.at(iPair)-> Fill(TMath::RadToDeg()*theta);
          hpar_phi_pair.at(iPair)  -> Fill(TMath::RadToDeg()*phi);
          hX_in_target_pair.at(iPair)   -> Fill(X_par_to_target);
          hY_in_target_pair.at(iPair)   -> Fill(Y_par_to_target);
          hY_X_in_target_pair.at(iPair)   -> Fill(X_par_to_target, Y_par_to_target);
          hAx_bx_in_target -> Fill(X_par_to_target, TMath::RadToDeg()*par_ab[iPair][0][itr]);
          hAy_by_in_target -> Fill(Y_par_to_target, TMath::RadToDeg()*par_ab[iPair][2][itr]);
        }
        
        BmnTrack *Tr = new ((*fBmnMwpcTracksArray)[fBmnMwpcTracksArray->GetEntriesFast()]) BmnTrack();
        Tr -> SetChi2(Chi2_ndf[iPair][itr]);
        Tr -> SetNHits(Nhits[iPair][itr]);
        Tr -> SetFlag(itr);
        FairTrackParam TrParams;
        TrParams.SetPosition(TVector3(par_ab[iPair][1][itr], par_ab[iPair][3][itr],kZ_midle_pair[iPair]));
        TrParams.SetTx(par_ab[iPair][0][itr]);
        TrParams.SetTy(par_ab[iPair][2][itr]);
        
        for(Int_t i1 = 0 ; i1 < 4; i1++) {
          for(Int_t j1 = 0; j1 < 4; j1++) {
            //if (fDebug) cout<<" sigma2["<<iPair<<"]["<<itr<<"]["<<i1<<"]["<<j1<<"] "<<sigma2[iPair][itr][i1][j1]<<endl;
            TrParams.SetCovariance(i1, j1, sigma2[iPair][itr][i1][j1]);
          }
        } 
        Tr -> SetParamFirst(TrParams);

      }//Nbest_pair[iPair]

    }//> 0
  }//iPair
  
}


void BmnMwpcTrackFinder::MCefficiencyCalculation(vector<MC_points>& vec, Int_t *Nbest, Double_t ***par_ab){
   
    if (fDebug) cout<<" ---MC tracks association--"<<endl;
    //                     ax,   bx,    ay,  by
    Double_t delta2[4] = {-999.,-999.,-999.,-999.}; 
    Double_t sig[4]    = {0.04, 0.08, 0.05, 0.08};
    
    Double_t dmatch = 0.;
    Double_t dmc_match[kMaxMC];
    Int_t    mc_tr_assoc[kMaxMC];
    
    for (Int_t j = 0; j < kMaxMC; j++) {
      dmc_match[j]   = 1000.;
      mc_tr_assoc[j] = -1;
    }
    
    Double_t dax = -999.;
    Double_t day = -999.;
    Double_t dx  = -999.;
    Double_t dy  = -999.;
    Int_t Ngood_mc_tracks = 0;
    Int_t Ngood_reco_tracks = 0;

    for (Int_t itr = 0; itr < vec.size(); itr++) {//mc_tr
      
        //---MC Eff ---
        //---Den
        if (fDebug && vec.at(itr).Np >= 8 ){//min 4 + 4
          //if ( vec.at(itr).xWas3 && vec.at(itr).uWas3 && vec.at(itr).vWas3 &&
          //vec.at(itr).xWas2 && vec.at(itr).uWas2 && vec.at(itr).vWas2) 
          hDen_mctr->Fill(1);
          hDen_mctr->Fill(0);
          cout<<" PCDen "<<endl;
          Ngood_mc_tracks++;
        }
        
          for(Int_t iseg= 0; iseg < Nbest[1]; iseg ++) {//reco-tr
            delta2[0] = vec.at(itr).param[0] - par_ab[1][0][iseg];
            delta2[1] = vec.at(itr).param[1] - par_ab[1][1][iseg];
            delta2[2] = vec.at(itr).param[2] - par_ab[1][2][iseg];
            delta2[3] = vec.at(itr).param[3] - par_ab[1][3][iseg];
          
            dmatch = 0.;
            dmatch = (delta2[0]*delta2[0])/(sig[0]*sig[0])+ 
                        (delta2[1]*delta2[1])/(sig[1]*sig[1])+
                        (delta2[2]*delta2[2])/(sig[2]*sig[2])+
                        (delta2[3]*delta2[3])/(sig[3]*sig[3]);
                         
            if ( dmc_match[itr] > dmatch){
                dmc_match[itr]   = dmatch;
                mc_tr_assoc[itr] = iseg;
                dax = delta2[0];
                dx  = delta2[1];
                day = delta2[2];
                dy  = delta2[3];
            }
          }//Nbest_pair[1]
          if (fDebug){
            if (mc_tr_assoc[itr] > -1){
              cout<<" index_mc "<<itr<<" Np "<<vec.at(itr).Np<<" mc_Id "<<vec.at(itr).Id<<" index_track "<<mc_tr_assoc[itr] <<
               // " ax_mc "<<vec.at(itr).param[0]<<" reco_ind "<<mc_tr_assoc[itr]<<" ax "<< par_ab[1][0][mc_tr_assoc[itr]] <<
                 " dmc_match "<<dmc_match[itr]<<endl;
              if (dax > -900.) hdAx_tr_mc->Fill(dax);
              if (dx > -900.)  hdX_tr_mc ->Fill(dx);
              if (day > -900.) hdAy_tr_mc->Fill(day);
              if (dy > -900.)  hdY_tr_mc ->Fill(dy);
            }
          }
    
    }//vec_points.size
    
    if (fDebug) cout<<"reject poorly chosen association PC tracks "<<endl;
    for (Int_t itr = 0; itr < vec.size(); itr++) {//mc_tr
      if (mc_tr_assoc[itr] == -1) continue;
       
      for (Int_t itr2 = 0; itr2 < vec.size(); itr2++) {//mc_tr
        if (itr2 == itr) continue;
        if (mc_tr_assoc[itr2] == -1) continue;
        
        if (mc_tr_assoc[itr] ==  mc_tr_assoc[itr2]){
          if (dmc_match[itr2] > 20. ) mc_tr_assoc[itr2] = -1;
          else {
            //mc_tr_assoc[itr] = -1;
            continue;//break;
          }
        }
      }//itr2
      //---MC Eff ---
      //---Num
      if (fDebug) cout<<" mc_Id "<<vec.at(itr).Id<<" assoc "<<mc_tr_assoc[itr]<<endl;
      if (fDebug && mc_tr_assoc[itr] > -1 && vec.at(itr).Np >=8 ){
        if ( vec.at(itr).xWas3 && vec.at(itr).uWas3 && vec.at(itr).vWas3 &&
        vec.at(itr).xWas2 && vec.at(itr).uWas2 && vec.at(itr).vWas2) 
        hNum_mctr->Fill(1);
        hNum_mctr->Fill(0);
        cout<<" PCNum "<<endl;
        Ngood_reco_tracks++;
      }
    }//itr
    if (fDebug){
      hNtrpc_mc ->Fill(Ngood_mc_tracks);
      hNtrpc_reco->Fill(Ngood_reco_tracks);
      hNtrpc_mc_vs_reco ->Fill(Ngood_reco_tracks,Ngood_mc_tracks);
      cout<<"PCp: Ngood_mc_tracks "<<Ngood_mc_tracks<<" Ngood_reco_tracks "<<Ngood_reco_tracks<<endl;
    }
}

void BmnMwpcTrackFinder::ReadSegments(Double_t ***par_ab, Int_t **Nhits, Double_t **Chi2_ndf, Int_t *Nbest, Double_t ***XVU, Double_t ***Clust, vector<MC_points>&vec){
  if (fDebug) cout<<"--ReadSegments--"<<endl;
  int Npl_MC2[kMaxMC]; int Npl_MC3[kMaxMC];
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
        
        Int_t    st_MC   = hit->GetMwpcId();
        Int_t    trackId_MC  = hit->GetHitId();
        Int_t    pl_MC   = hit->GetPlaneId();
        Short_t  wire_MC = hit->GetWireNumber();
        Double_t time_MC = hit->GetWireTime();
        
        //if (fDebug)cout<<" st_MC "<<st_MC<<" trackId_MC "<<trackId_MC<<" pl_MC "<<pl_MC<<" X "<<hit->GetX()<<" wire_MC "<<wire_MC<<endl;
        
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
          tmpTr.Id = mcTracksArray[id];
          tmpTr.Np2 = Npl_MC2[id];
          tmpTr.Np3 = Npl_MC3[id];
          tmpTr.Np  = Npl_MC2[id] + Npl_MC3[id];
          
          if (Npl_MC2[id] >= 4 || Npl_MC3[id] >= 4 ) vec.push_back(tmpTr);
      }
      
    if (fDebug)cout<<" MC vec_points.size() "<<vec.size()<<endl;
    
    Double_t x_target_ch2, y_target_ch2, x_target_ch3, y_target_ch3;
    
    for (Int_t itr = 0; itr < vec.size(); itr++) {
      //ch2
      int i2 = -1;
      if (vec.at(itr).x2[5] > -900.)   i2 =5;
      if (i2 < 0 && vec.at(itr).x2[4] > -900.)  i2 =4;
      if (i2 < 0 && vec.at(itr).x2[3] > -900.)  i2 =3;
      int i20 = 0;
      if (vec.at(itr).x2[i20] < -900.) i20 =1;
      if (vec.at(itr).x2[i20] < -900.) i20 =2;
      if (i2 > -1 && vec.at(itr).Np2 > 3){
        vec.at(itr).param2[0]  = (vec.at(itr).x2[i20] - vec.at(itr).x2[i2])/ (vec.at(itr).z2[i20] - vec.at(itr).z2[i2]); 
        vec.at(itr).param2[1]  = (vec.at(itr).x2[i20] + vec.at(itr).x2[i2])*0.5;
        vec.at(itr).param2[2]  = (vec.at(itr).y2[i20] - vec.at(itr).y2[i2])/ (vec.at(itr).z2[i20] - vec.at(itr).z2[i2]); 
        vec.at(itr).param2[3]  = (vec.at(itr).y2[i20] + vec.at(itr).y2[i2])*0.5;
      }
      //ch3
      int i3 = -1;
      if (vec.at(itr).x3[5] > -900.)   i3 =5;
      if (i3 < 0 && vec.at(itr).x3[4] > -900.)  i3 =4;
      if (i3 < 0 && vec.at(itr).x3[3] > -900.)  i3 =3;
      int i30 = 0;
      if (vec.at(itr).x3[i30] < -900.) i30 =1;
      if (vec.at(itr).x3[i30] < -900.) i30 =2;
      
      if (i3 > -1 && vec.at(itr).Np3 > 3){
        vec.at(itr).param3[0]  = (vec.at(itr).x3[i30] - vec.at(itr).x3[i3])/ (vec.at(itr).z3[i30] - vec.at(itr).z3[i3]); 
        vec.at(itr).param3[1]  = (vec.at(itr).x3[i30] + vec.at(itr).x3[i3])*0.5;
        vec.at(itr).param3[2]  = (vec.at(itr).y3[i30] - vec.at(itr).y3[i3])/ (vec.at(itr).z3[i30] - vec.at(itr).z3[i3]); 
        vec.at(itr).param3[3]  = (vec.at(itr).y3[i30] + vec.at(itr).y3[i3])*0.5;
      }
      //pair
      if (i2 > -1 && vec.at(itr).Np2 > 3 && i3 > -1 && vec.at(itr).Np3 > 3){
        vec.at(itr).param[0]  = (vec.at(itr).x2[i20] - vec.at(itr).x3[i3])/ 
                                (vec.at(itr).z2[i20] - vec.at(itr).z3[i3]); 
        vec.at(itr).param[1]  = (vec.at(itr).x2[i20] + vec.at(itr).x3[i3])*0.5;
        vec.at(itr).param[2]  = (vec.at(itr).y2[i20] - vec.at(itr).y3[i3])/ 
                                (vec.at(itr).z2[i20] - vec.at(itr).z3[i3]); 
        vec.at(itr).param[3]  = (vec.at(itr).y2[i20] + vec.at(itr).y3[i3])*0.5;
      }
      //triplet check
      if (vec.at(itr).x2[0] > -900. || vec.at(itr).x2[3] > -900.) vec.at(itr).xWas2 = 1;
      if (vec.at(itr).x2[1] > -900. || vec.at(itr).x2[4] > -900.) vec.at(itr).vWas2 = 1;
      if (vec.at(itr).x2[2] > -900. || vec.at(itr).x2[5] > -900.) vec.at(itr).uWas2 = 1;
      
      if (vec.at(itr).x3[0] > -900. || vec.at(itr).x3[3] > -900.) vec.at(itr).xWas3 = 1;
      if (vec.at(itr).x3[1] > -900. || vec.at(itr).x3[4] > -900.) vec.at(itr).vWas3 = 1;
      if (vec.at(itr).x3[2] > -900. || vec.at(itr).x3[5] > -900.) vec.at(itr).uWas3 = 1;
      
      if (fDebug && vec.at(itr).Np > 7) hYvsX_mc_pair->Fill(vec.at(itr).param[1],vec.at(itr).param[3]);


    }
  }// !expData
  

  //----------------------- Read MWPC-Segmets---------------------------
  for (Int_t iSegment = 0; iSegment < fBmnMwpcSegmentsArray->GetEntries(); iSegment++) {
    BmnMwpcSegment* segment = (BmnMwpcSegment*) fBmnMwpcSegmentsArray->At(iSegment);
    Int_t iCh;
    Double_t Z = segment->GetParamFirst()->GetZ();
    Int_t ise = segment->GetFlag();//iSegmentID

    if ( ZCh[0] == Z) iCh = 0;
    if ( ZCh[1] == Z) iCh = 1;
    if ( ZCh[2] == Z) iCh = 2;
    if ( ZCh[3] == Z) iCh = 3;

    Nhits[iCh][ise]     = segment->GetNHits();
    Chi2_ndf[iCh][ise]  = segment->GetChi2();
    par_ab[iCh][0][ise] = segment->GetParamFirst()->GetTx();
    par_ab[iCh][1][ise] = segment->GetParamFirst()->GetX();
    par_ab[iCh][2][ise] = segment->GetParamFirst()->GetTy();
    par_ab[iCh][3][ise] = segment->GetParamFirst()->GetY();

    for(Int_t i1 = 0 ; i1 < 6; i1++) {
      XVU[iCh][i1][ise]   = segment -> GetCoord().at(i1);
      Clust[iCh][i1][ise] = segment -> GetClust().at(i1);
    }
    Nbest[iCh]++;
  }//iSegment
  //--------------------------------------------------------------------

  for (Int_t iChamber = 0; iChamber < kNChambers; iChamber++) {
    if (fDebug && Nbest[iChamber] > 0) {
      for (Int_t ise = 0; ise < Nbest[iChamber]; ise++) {
        hChi2_ndf_Ch.at(iChamber) -> Fill(Chi2_ndf[iChamber][ise]);
        hpar_Ax_Ch.at(iChamber) -> Fill( par_ab[iChamber][0][ise]);
        hpar_Bx_Ch.at(iChamber) -> Fill( par_ab[iChamber][1][ise]);
        hpar_Ay_Ch.at(iChamber) -> Fill( par_ab[iChamber][2][ise]);
        hpar_By_Ch.at(iChamber) -> Fill( par_ab[iChamber][3][ise]);
         //if (fDebug) cout<<" iChamber "<<iChamber<<" ax= "<<par_ab[iChamber][0][ise]<<" bx= "<<par_ab[iChamber][1][ise]<<" ay= "<<par_ab[iChamber][2][ise]<<" by= "<<par_ab[iChamber][3][ise]<<" Chi2 "<<Chi2_ndf[iChamber][ise]<<endl;
        for(Int_t i1 = 0 ; i1 < 6; i1++) {
           //if (fDebug) cout<<" Coord "<<i1 <<" " <<XVU[iChamber][i1][ise] <<endl;
        }
      }
    }//if (fDebug)
  }//iChamber
  
}





//------------------Segment Parameters Alignment----------------------
void BmnMwpcTrackFinder::SegmentParamAlignment(Int_t chNum, Int_t *Nbest, Double_t ***par_ab, Float_t **shiftt ) {
  if (fDebug) cout<<endl;
  for (Int_t iBest = 0; iBest < Nbest[chNum]; iBest++) {
    //                   ax     alpha                   ax^2
    par_ab[chNum][0][iBest] += shiftt[chNum][0] + shiftt[chNum][0]* par_ab[chNum][0][iBest]* par_ab[chNum][0][iBest];
    par_ab[chNum][2][iBest] += shiftt[chNum][2] + shiftt[chNum][2]* par_ab[chNum][2][iBest]* par_ab[chNum][2][iBest];
    par_ab[chNum][1][iBest] += shiftt[chNum][1];
    par_ab[chNum][3][iBest] += shiftt[chNum][3];
    // if (fDebug) cout<<" chNum "<<chNum<<" after Alignment: iBest "<<iBest<<" Ax "<< par_ab[chNum][0][iBest]<<" bx "<< par_ab[chNum][1][iBest]<<" Ay "<< par_ab[chNum][1][iBest]<<" by "<< par_ab[chNum][3][iBest]<<endl;

  }//iBest
}//SegmentParamAlignment
//----------------------------------------------------------------------


//--------------Track Matching------------------------------------------
void BmnMwpcTrackFinder::SegmentMatching( Int_t first_Ch, 
Int_t *Nbest, Double_t ***par_ab, Float_t *Zmid, 
Int_t **best_Ch, Int_t *Nbest_pair_, Double_t **Chi2_match_, Double_t ***XVU_Ch_, Int_t **Nhits_, Int_t **Nhits_m) {
  //cout<<" SegmentMatching "<<endl;
  Float_t min_Chi2m = 100;
  Float_t min_distX = 99;
  Float_t min_distY = 99;
  Double_t dx_loc = 99;
  Double_t dy_loc = 99;
  Float_t dAx12 = 0;
  Float_t dAy12 = 0;
  Float_t Min_distX[kmaxPairs];
  Float_t Min_distY[kmaxPairs];

  for (Int_t i = 0; i < kmaxPairs ; i++) {
    Min_distX[i] = -1;
    Min_distY[i] = -1;
  }

  Int_t Nvariat = 0;
  Int_t Nvariations = 100;

  vector<match> vtmpSeg;
  vector<match> OutVector;
  match tmpSeg;
  match OutSegArray[kmaxPairs];

  Int_t Pairr = 0;
  if ( fRunPeriod == 7 && first_Ch == 2 || fRunPeriod == 8) Pairr = 1;
  Int_t Secon_Ch = first_Ch+1;

  if (Nbest_Ch[first_Ch] > 0 && Nbest_Ch[Secon_Ch] > 0) {

    for (Int_t bst1 = 0; bst1 < Nbest[first_Ch]; bst1++) {
      //ch1                   zloc0 -z_i
      Float_t x1mid = par_ab[first_Ch][0][bst1] *( 0 - kZmid[first_Ch]) + par_ab[first_Ch][1][bst1] ;
      Float_t y1mid = par_ab[first_Ch][2][bst1] *( 0 - kZmid[first_Ch]) + par_ab[first_Ch][3][bst1] ;

      for (Int_t bst2 = 0; bst2 < Nbest[Secon_Ch]; bst2++) {
        //ch2
        Float_t x2mid = par_ab[Secon_Ch][0][bst2] *( 0 - kZmid[Secon_Ch]) + par_ab[Secon_Ch][1][bst2] ;
        Float_t y2mid = par_ab[Secon_Ch][2][bst2] *( 0 - kZmid[Secon_Ch]) + par_ab[Secon_Ch][3][bst2] ;
        dAx12 = par_ab[first_Ch][0][bst1] - par_ab[Secon_Ch][0][bst2];
        dAy12 = par_ab[first_Ch][2][bst1] - par_ab[Secon_Ch][2][bst2];
        min_distX = x1mid - x2mid; //min
        min_distY = y1mid - y2mid; //min
        dx_loc  = par_ab[first_Ch][1][bst1] - par_ab[Secon_Ch][1][bst2];
        dy_loc  = par_ab[first_Ch][3][bst1] - par_ab[Secon_Ch][3][bst2];
        //if (fDebug) cout<<" Pairr "<<Pairr<<" sigma ax "<<sigma_delta[Pairr][0]<<" ay "<<sigma_delta[Pairr][2]<<" sigma x "<<sigma_delta[Pairr][1]<<" y "<<sigma_delta[Pairr][3]<<endl;

        Float_t Chi2_m = ( dx_loc*dx_loc/(sigma_delta[Pairr][1]*sigma_delta[Pairr][1]) + dy_loc*dy_loc/(sigma_delta[Pairr][3]*sigma_delta[Pairr][3]) );
                 // + dAx12*dAx12 /(sigma_delta[Pairr][0]*sigma_delta[Pairr][0]) + dAy12*dAy12 /(sigma_delta[Pairr][2]*sigma_delta[Pairr][2]) ); //13.09.2019
        if (fDebug){
          hdX_pair.at(Pairr)->Fill(dx_loc);
          hdY_pair.at(Pairr)->Fill(dy_loc);
          hdAx_pair.at(Pairr)->Fill(dAx12);
          hdAy_pair.at(Pairr)->Fill(dAy12);
        }
        //if (fDebug)cout<<" Pairr "<<Pairr<<" bst1 "<<bst1<<" Nhits "<<Nhits_[first_Ch][bst1]<<" bst2 "<<bst2<<" Nhits "<<Nhits_[Secon_Ch][bst2]<<" Chi2_m "<<Chi2_m<<endl;

        if (Chi2_m < min_Chi2m && Nvariat < Nvariations && fabs(dx_loc) < 1.5 && fabs(dy_loc) < 1.5) {//09.2019
       
          tmpSeg.Chi2m = Chi2_m;
          tmpSeg.Ind1  = bst1;
          tmpSeg.Ind2  = bst2;
          tmpSeg.Nhits1 = Nhits_[first_Ch][bst1];
          tmpSeg.Nhits2 = Nhits_[Secon_Ch][bst2];
          tmpSeg.distX = dx_loc;//min_distX;
          tmpSeg.distY = dy_loc;//min_distY;

          for(int ipar = 0; ipar < 4; ipar++) {
            tmpSeg.param1[ipar] = par_ab[first_Ch][ipar][bst1];
            tmpSeg.param2[ipar] = par_ab[Secon_Ch][ipar][bst2];
          }

          vtmpSeg.push_back(tmpSeg);
          Nvariat++;
        }  //if (Chi2_m

      }//bst2++
    }//bst1++

    if (vtmpSeg.size() < 1) return;

    // vector sorting
    sort(vtmpSeg.begin(), vtmpSeg.end(), compareSegments);
    OutVector.clear();

    //first best
    OutVector.push_back(vtmpSeg.at(0));

    //reject repeat index
    Bool_t isMatch;
    for (int iter = 0; iter < vtmpSeg.size(); ++iter) {
      //printf("vtmpSeg.at(%d): %8.4f | %d - %d\n", iter, vtmpSeg.at(iter).Chi2m, vtmpSeg.at(iter).Ind1, vtmpSeg.at(iter).Ind2 );
      isMatch = 0;
      for(int InIter = 0; InIter < OutVector.size(); ++InIter) {
        if(vtmpSeg.at(iter).Ind1 == OutVector.at(InIter).Ind1 || vtmpSeg.at(iter).Ind2 == OutVector.at(InIter).Ind2) {
          isMatch = 1;
          continue;
        }
      }
      //writing unique index
      if (isMatch == 0) OutVector.push_back(vtmpSeg.at(iter));
    }//iter

    if (fDebug && vtmpSeg.size() > 1 ) hChi2best_Chi2fake_before_target-> Fill(OutVector.at(0).Chi2m, vtmpSeg.at(1).Chi2m);

    for(int iter = 0; iter < OutVector.size(); ++iter) {
      // printf("OutVector.at(%d): %8.4f | %d - %d\n", iter, OutVector.at(iter).Chi2m, OutVector.at(iter).Ind1, OutVector.at(iter).Ind2);
      if (Nbest_pair_[Pairr] < kmaxPairs) {
        Chi2_match_[Pairr][Nbest_pair_[Pairr]]= OutVector.at(iter).Chi2m;
        best_Ch[first_Ch][Nbest_pair_[Pairr]] = OutVector.at(iter).Ind1;
        best_Ch[Secon_Ch][Nbest_pair_[Pairr]] = OutVector.at(iter).Ind2;
        Nhits_m[first_Ch][Nbest_pair_[Pairr]] = OutVector.at(iter).Nhits1;
        Nhits_m[Secon_Ch][Nbest_pair_[Pairr]] = OutVector.at(iter).Nhits2;
        Min_distX[Nbest_pair_[Pairr]]     = OutVector.at(iter).distX;
        Min_distY[Nbest_pair_[Pairr]]     = OutVector.at(iter).distY;

        Nbest_pair_[Pairr]++;
      }// < kmaxPairs){
    }//iter

    if (fDebug) {
      for (Int_t ii = 0; ii < Nbest_pair_[Pairr]; ii++) {
        FillEfficiency( first_Ch, XVU_Ch, Nhits_m, kMinHits, best_Ch[first_Ch][ii], Min_distX[ii], Min_distY[ii]);
        FillEfficiency( Secon_Ch, XVU_Ch, Nhits_m, kMinHits, best_Ch[Secon_Ch][ii], Min_distX[ii], Min_distY[ii]);
      }
    }

  }//if (Nbest_Ch[first_Ch] > 0 && Nbest_Ch[Secon_Ch] > 0)
}// SegmentMatching
//----------------------------------------------------------------------


//-----------------Segment Matching after target------------------------
void BmnMwpcTrackFinder::SegmentMatchingAfterTarget( Int_t first_Ch, Int_t *Nbest, Double_t ***par_ab, Float_t *Zmid, Int_t **best_Ch, Int_t *Nbest_pair_, Double_t **Chi2_match_, Double_t ***XVU_Ch_, Int_t **Nhits_, Int_t **Nhits_m) {
  if (fDebug) cout<<" SegmentMatching AfterTarget"<<endl;
  Float_t max_Chi2m = 100;
  Float_t dAx = 0;
  Float_t dAy = 0;
  Double_t dx_loc = 10;
  Double_t dy_loc = 10;
  Float_t Min_distX[kmaxPairs];
  Float_t Min_distY[kmaxPairs];

  for (Int_t i = 0; i < kmaxPairs ; i++) {
    Min_distX[i] = -1;
    Min_distY[i] = -1;
  }

  Int_t Nvariat = 0;
  Int_t Nvariations = 100;
  vector<match> vtmpSeg;
  vector<match> OutVector;
  match tmpSeg;
  match OutSegArray[kmaxPairs];

  Int_t Pairr = 0;//doesn't work
  if ( fRunPeriod == 7 && first_Ch == 2 || fRunPeriod == 8) Pairr = 1;// main stream
  Int_t Secon_Ch = first_Ch+1;

  if (Nbest_Ch[first_Ch] > 0 && Nbest_Ch[Secon_Ch] > 0) {
    if (fDebug) hDen->Fill(0);
    // if (fDebug)cout<<" Nbest[ "<<first_Ch<<"] = "<<Nbest_Ch[first_Ch]<<endl;
    // if (fDebug)cout<<" Nbest[ "<<Secon_Ch<<"] = "<<Nbest_Ch[Secon_Ch]<<endl;

    for (Int_t bst1 = 0; bst1 < Nbest[first_Ch]; bst1++) {

      //ch1                   zloc0 -z_i
      Float_t x1mid = par_ab[first_Ch][0][bst1] *( 0 - kZmid[first_Ch]) + par_ab[first_Ch][1][bst1] ;
      Float_t y1mid = par_ab[first_Ch][2][bst1] *( 0 - kZmid[first_Ch]) + par_ab[first_Ch][3][bst1] ;
      //  cout<<" bst1 " <<bst1<<" x1mid "<<x1mid<<" y1mid "<<y1mid<<endl;

      for (Int_t bst2 = 0; bst2 < Nbest[Secon_Ch]; bst2++){
        //ch2
        Float_t x2mid = par_ab[Secon_Ch][0][bst2] *( 0 - kZmid[Secon_Ch]) + par_ab[Secon_Ch][1][bst2] ;
        Float_t y2mid = par_ab[Secon_Ch][2][bst2] *( 0 - kZmid[Secon_Ch]) + par_ab[Secon_Ch][3][bst2] ;
        //if (fDebug)cout<<" bst2 " <<bst2<<" x2mid "<<x2mid<<" y2mid "<<y2mid<<endl;
        
        dx_loc  = par_ab[first_Ch][1][bst1] - par_ab[Secon_Ch][1][bst2];
        dy_loc  = par_ab[first_Ch][3][bst1] - par_ab[Secon_Ch][3][bst2];
        dAx     = par_ab[first_Ch][0][bst1] - par_ab[Secon_Ch][0][bst2];
        dAy     = par_ab[first_Ch][2][bst1] - par_ab[Secon_Ch][2][bst2];

        Double_t Ax_23     = (par_ab[Secon_Ch][1][bst2] - par_ab[first_Ch][1][bst1])/ (kZmid[Secon_Ch] - kZmid[first_Ch]);
        Double_t Ay_23     = (par_ab[Secon_Ch][3][bst2] - par_ab[first_Ch][3][bst1])/ (kZmid[Secon_Ch] - kZmid[first_Ch]);
        Double_t x_target  = Ax_23*( Z0_SRC - ZCh[2]) + par_ab[first_Ch][1][bst1];
        Double_t y_target  = Ay_23*( Z0_SRC - ZCh[2]) + par_ab[first_Ch][3][bst1];

        //if (fDebug) cout<<" x_target "<< x_target<<" y_target "<< y_target<<endl;
        //if (fDebug)	cout<<" Pairr "<<Pairr<<" sigma ax "<<sigma_delta[Pairr][0]<<" ay "<<sigma_delta[Pairr][2]<<" sigma x "<<sigma_delta[Pairr][1]<<" y "<<sigma_delta[Pairr][3]<<endl;

        Float_t Chi2_m = ( dx_loc*dx_loc/(sigma_delta[Pairr][1]*sigma_delta[Pairr][1]) + dy_loc*dy_loc/(sigma_delta[Pairr][3]*sigma_delta[Pairr][3]) );
                  //+ dAx*dAx /(sigma_delta[Pairr][0]*sigma_delta[Pairr][0]) + dAy*dAy /(sigma_delta[Pairr][2]*sigma_delta[Pairr][2]) );  //13.09.19
        if (fDebug){
          hdX_pair.at(Pairr)->Fill(dx_loc);//Fill(min_distX);
          hdY_pair.at(Pairr)->Fill(dy_loc);//Fill(min_distY);
          hdAx_pair.at(Pairr)->Fill(dAx);
          hdAy_pair.at(Pairr)->Fill(dAy);
        }
        //if (fDebug)cout<<" Pairr "<<Pairr<<" bst1 "<<bst1<<" Nhits "<<Nhits_[first_Ch][bst1]<<" bst2 "<<bst2<<" Nhits "<<Nhits_[Secon_Ch][bst2]<<" Chi2_m "<<Chi2_m<<endl;
        
        if (fDebug)cout<<" x_target "<<x_target<<" fabs(x_target - kX_target) "<<fabs(x_target - kX_target)<<" fabs(y_target - kX_target) "<<fabs(y_target - kX_target)<<endl;
        if (fDebug) cout<<" bst1 "<<bst1<<" bst2 "<<bst2<<" Chi2_m "<<Chi2_m<<endl;
        if (Chi2_m < max_Chi2m && Nvariat < Nvariations && fabs(x_target - kX_target) < ktarget_region && fabs(y_target - kY_target) < ktarget_regionY) {

          tmpSeg.Chi2m  = Chi2_m;
          tmpSeg.Ind1   = bst1;
          tmpSeg.Ind2   = bst2;
          tmpSeg.Nhits1 = Nhits_[first_Ch][bst1];
          tmpSeg.Nhits2 = Nhits_[Secon_Ch][bst2];
          tmpSeg.distX  = dx_loc;//min_distX;
          tmpSeg.distY  = dy_loc;//min_distY;
          tmpSeg.distAX = dAx;
          tmpSeg.distAY = dAy;

          for(int ipar = 0; ipar < 4; ipar++) {
            tmpSeg.param1[ipar] = par_ab[first_Ch][ipar][bst1];
            tmpSeg.param2[ipar] = par_ab[Secon_Ch][ipar][bst2];
          }

          vtmpSeg.push_back(tmpSeg);
          Nvariat++;
        }//if (Chi2_m
      }//bst2++
    }//bst1++

    if (vtmpSeg.size() < 1) return;

    // vector sorting
    sort(vtmpSeg.begin(), vtmpSeg.end(), compareSegments);
    OutVector.clear();

    Bool_t exist_pair[Nvariations];
    for(int im = 0; im < vtmpSeg.size(); ++im) {
      exist_pair[im] = 1;
    }

    //cout<<" kmaxPairs "<<kmaxPairs<<endl;
    for(int im = 0; im < vtmpSeg.size(); ++im) {
      if ( !exist_pair[im]) continue;
      OutVector.push_back(vtmpSeg.at(im));
      int InIter = OutVector.size() - 1;
      if (fDebug) cout<<" im "<<im<<" InIter "<<InIter<<endl;

      //reject repeat index
      if ( im + 1 < vtmpSeg.size()) {
        for (int iter = im + 1 ; iter < vtmpSeg.size(); ++iter) {
          if (fDebug) cout<<" iter "<<iter<<endl;
          //printf("vtmpSeg.at(%d): %8.4f | %d - %d\n", iter, vtmpSeg.at(iter).Chi2m, vtmpSeg.at(iter).Ind1, vtmpSeg.at(iter).Ind2 );
          if ( !exist_pair[iter]) continue;
          if (fDebug) cout<<" vtmpSeg.at(iter).Ind1 "<<vtmpSeg.at(iter).Ind1<<" OutVector.at(InIter).Ind1 "<<OutVector.at(InIter).Ind1<<endl;
          if(vtmpSeg.at(iter).Ind1 == OutVector.at(InIter).Ind1 || vtmpSeg.at(iter).Ind2 == OutVector.at(InIter).Ind2) {
            exist_pair[iter] = 0; //VVP 17.07.2019 -> for matching with silicon!!!

          }
        }//iter
      }//if ( im + 1 < vtmpSeg.size())
    }//im

    if (fDebug) cout<<" OutVector.at(0).Chi2m "<<OutVector.at(0).Chi2m<<" vtmpSeg.at(0).Chi2m "<<vtmpSeg.at(0).Chi2m<<endl;
    if (fDebug && vtmpSeg.size() > 1 ) hChi2best_Chi2fake_after_target-> Fill(OutVector.at(0).Chi2m, vtmpSeg.at(1).Chi2m);

    for(int iter = 0; iter < OutVector.size(); ++iter) {
      if (fDebug) printf("OutVector.at(%d): %8.4f | %d - %d\n", iter, OutVector.at(iter).Chi2m, OutVector.at(iter).Ind1, OutVector.at(iter).Ind2);
      if (Nbest_pair_[Pairr] < kmaxPairs) {

        Chi2_match_[Pairr][Nbest_pair_[Pairr]]= OutVector.at(iter).Chi2m;
        best_Ch[first_Ch][Nbest_pair_[Pairr]] = OutVector.at(iter).Ind1;
        best_Ch[Secon_Ch][Nbest_pair_[Pairr]] = OutVector.at(iter).Ind2;
        Nhits_m[first_Ch][Nbest_pair_[Pairr]] = OutVector.at(iter).Nhits1;
        Nhits_m[Secon_Ch][Nbest_pair_[Pairr]] = OutVector.at(iter).Nhits2;
        Min_distX[Nbest_pair_[Pairr]]     = OutVector.at(iter).distX;
        Min_distY[Nbest_pair_[Pairr]]     = OutVector.at(iter).distY;

        Double_t Ax = (OutVector.at(iter).param2[1] - OutVector.at(iter).param1[1])/(ZCh[3] - ZCh[2]);
        Double_t Xv = Ax*(Z0_SRC - ZCh[2]) + OutVector.at(iter).param1[1];
        Double_t Ay = (OutVector.at(iter).param2[3] - OutVector.at(iter).param1[3])/(ZCh[3] - ZCh[2]);
        Double_t Yv = Ay*(Z0_SRC - ZCh[2]) + OutVector.at(iter).param1[3];
        if (fDebug){
          hdX_pair_1 ->Fill(OutVector.at(iter).distX);
          hdY_pair_1 ->Fill(OutVector.at(iter).distY);
          hdAx_pair_1->Fill(OutVector.at(iter).distAX);
          hdAy_pair_1->Fill(OutVector.at(iter).distAY);
          hChi2m_pair_1->Fill(OutVector.at(iter).Chi2m);
          hXv_pair_1  ->Fill(Xv);
          hYv_pair_1  ->Fill(Yv);
        }

        Nbest_pair_[Pairr]++;
      }// < kmaxPairs){
    }//iter
    
    if (fDebug) {
      for (Int_t ii = 0; ii < Nbest_pair_[Pairr]; ii++) {
        FillEfficiency( first_Ch, XVU_Ch, Nhits_m, kMinHits, best_Ch[first_Ch][ii], Min_distX[ii], Min_distY[ii]);
        FillEfficiency( Secon_Ch, XVU_Ch, Nhits_m, kMinHits, best_Ch[Secon_Ch][ii], Min_distX[ii], Min_distY[ii]);
      }
    }
    if (fDebug) cout<<"--Matching efficiency--"<<endl;
    if (fDebug) cout<<" Nbest_pair_["<<Pairr<<"] "<<Nbest_pair_[Pairr]<<endl;
    if (fDebug && Nbest_pair_[Pairr] > 0 ) hNum->Fill(0);
    
  }//if (Nbest_Ch[first_Ch] > 0 && Nbest_Ch[Secon_Ch] > 0)
}// SegmentMatching
//----------------------------------------------------------------------



//------------------Pair Matching---------------------------------------
void BmnMwpcTrackFinder::PairMatching( Int_t *Nbest_p, Double_t ***par_ab_p, Float_t *kZ_midle_pair_) {
  Int_t Npair_dist = 0;
  Double_t sig_dx = 1., sig_dy = 1., sig_dax = .006, sig_day = .006;
  Double_t min_Chi2m = 50.;
  Double_t dXm, dYm , dAx12m, dAy12m;
  Float_t X_pair0_to_pair1, Y_pair0_to_pair1;
  Double_t phi0, theta0, phi1, theta1;

  for (Int_t pair0 = 0; pair0 < Nbest_p[0]; pair0++) {

    //pair 0                 zloc0 -z_i
    Double_t x1 = par_ab_p[0][0][pair0] *( Z0_SRC - kZ_midle_pair_[0] ) + par_ab_p[0][1][pair0] ;
    Double_t y1 = par_ab_p[0][2][pair0] *( Z0_SRC - kZ_midle_pair_[0] ) + par_ab_p[0][3][pair0] ;
    // if (fDebug) cout<<" pair "<<0<<" pair0 "<<pair0<<" ax "<<par_ab_p[0][0][pair0]<<" bx "<< par_ab_p[0][1][pair0]<<" ay "<<par_ab_p[0][2][pair0]<<" by "<<par_ab_p[0][3][pair0]<<" Z0_SRC "<<Z0_SRC<<" kZ_midle_pair "<<kZ_midle_pair_[0]<<endl;

    for (Int_t pair1 = 0; pair1 < Nbest_p[1]; pair1++) {

      //pair 1
      Double_t x2 = par_ab_p[1][0][pair1] *( Z0_SRC - kZ_midle_pair_[1] ) + par_ab_p[1][1][pair1] ;
      Double_t y2 = par_ab_p[1][2][pair1] *( Z0_SRC - kZ_midle_pair_[1] ) + par_ab_p[1][3][pair1] ;
      // if (fDebug) cout<<" pair "<<1<<" pair1 "<<pair1<<" ax "<<par_ab_p[1][0][pair1]<<" bx "<< par_ab_p[1][1][pair1]<<" ay "<<par_ab_p[1][2][pair1]<<" by "<<par_ab_p[1][3][pair1]<<" Z0_SRC "<<Z0_SRC<<" kZ_midle_pair "<<kZ_midle_pair_[1]<<endl;
      Float_t dAx12 = par_ab_p[0][0][pair0] - par_ab_p[1][0][pair1];
      Float_t dAy12 = par_ab_p[0][2][pair0] - par_ab_p[1][2][pair1];
      Float_t dX = x1 - x2;
      Float_t dY = y1 - y2;

      Float_t Chi2_m = dX/(sig_dx*sig_dx) + dY/(sig_dy*sig_dy) + dAx12*dAx12/(sig_dax*sig_dax) + dAy12*dAy12/(sig_day*sig_day);
      //                  Pair
      X_pair0_to_pair1 = par_ab_p[0][0][pair0]*( kZ_midle_pair[0] - kZ_midle_pair[1]) + par_ab_p[0][1][pair0];
      Y_pair0_to_pair1 = par_ab_p[0][2][pair0]*( kZ_midle_pair[0] - kZ_midle_pair[1]) + par_ab_p[0][3][pair0];

      if (Chi2_m < min_Chi2m) {
        min_Chi2m = Chi2_m; //min
        dXm  = dX;
        dYm  = dY;
        dAx12m = dAx12;
        dAy12m = dAy12;
        Npair_dist++;
        
        if (fDebug){
          hdX_pair01_vs_x1->Fill(x2,dXm);
          hdY_pair01_vs_y1->Fill(y2,dYm);
          hdX_pair01_inZpair1->Fill(X_pair0_to_pair1 - par_ab_p[1][1][pair1]);
          hdY_pair01_inZpair1->Fill(Y_pair0_to_pair1 - par_ab_p[1][3][pair1]);
          //hY_X_in_target_pair.at(0)  -> Fill(x1, y1);
          //hY_X_in_target_pair.at(1)  -> Fill(x2, y2);
          hAx_bx_in_target_pair.at(0) -> Fill(x1, TMath::RadToDeg()*par_ab_p[0][0][pair0]);
          hAx_bx_in_target_pair.at(1) -> Fill(x2, TMath::RadToDeg()*par_ab_p[1][0][pair1]);
          hAy_by_in_target_pair.at(0) -> Fill(y1, TMath::RadToDeg()*par_ab_p[0][2][pair0]);
          hAy_by_in_target_pair.at(1) -> Fill(y2, TMath::RadToDeg()*par_ab_p[1][2][pair1]);
        }

        phi0 =  TMath::ATan2(par_ab_pair[0][2][pair0],par_ab_pair[0][0][pair0]); // phi = arctan(tgy/tgx)
        theta0 = TMath::ATan2(par_ab_pair[0][0][pair0], TMath::Cos(phi0));// theta = arctan(tgx/cos(phi))
        phi1 =  TMath::ATan2(par_ab_pair[1][2][pair1],par_ab_pair[1][0][pair1]); // phi = arctan(tgy/tgx)
        theta1 = TMath::ATan2(par_ab_pair[1][0][pair1], TMath::Cos(phi1));// theta = arctan(tgx/cos(phi))

        if (fDebug) htheta_p1vsp0 -> Fill(TMath::RadToDeg()*theta0,TMath::RadToDeg()*theta1);
      }

    }//pair1
  }//pair0

  if (fDebug) hChi2_m_target->Fill(min_Chi2m);
  if (fDebug && min_Chi2m < 50 ) {
    hdX_target ->Fill(dXm);
    hdY_target ->Fill(dYm);
    hdAx_target ->Fill(dAx12m);
    hdAy_target ->Fill(dAy12m);
  }

}//PairMatching
//----------------------------------------------------------------------

//-------------Segment Fit----------------------------------------------
void BmnMwpcTrackFinder::SegmentFit(Int_t First_Ch, Float_t **z_gl_, Float_t *sig, Int_t *Nbest_pair_, Int_t ** ind_best_Ch_, Double_t ***par_ab_pair_, Double_t **Chi2_ndf_pair_, Double_t ***XVU_Ch_, Double_t ***Clust_Ch_, Int_t **ind_best_pair_,Int_t **Nhits_Ch_, Int_t **Nhits_pair_, Double_t ****sigma2_p) {
  int chiEl = 0;
  if (First_Ch == 2) chiEl = 1;
  if (Nbest_pair_[chiEl] >= kmaxPairs) {
    printf("!!! ERROR: Nbest_pair_[%d] > 10\n", chiEl);
    return;
  }

  Double_t sigm_1[kNPlanes], sigm2_1[kNPlanes];
  Double_t sigm_2[kNPlanes], sigm2_2[kNPlanes];
  Int_t Pair1 = 0;
  if (fRunPeriod == 7 && First_Ch == 2 || fRunPeriod == 8) Pair1 = 1;

  for (Int_t bst = 0; bst < Nbest_pair_[Pair1]; bst++) {
     if (fDebug) cout<<"SegmentFit bst "<<bst<<endl;

    Int_t fir   = First_Ch;
    Int_t sec   = First_Ch+1;
    Int_t best1 = ind_best_Ch_[fir][bst];
    Int_t best2 = ind_best_Ch_[sec][bst];

    int h1[6] = {0,0,0,0,0,0};
    int h2[6] = {0,0,0,0,0,0};

    for(Int_t i = 0; i < 6; i++) {
      sigm_1[i]= 1.;
      sigm_2[i]= 1.;
      sigm2_1[i]= 1.;
      sigm2_2[i]= 1.;
      h1[i] = 0;
      h2[i] = 0;

      if ( XVU_Ch_[fir][i][best1] > -900.) {
        h1[i] = 1;
        sigm_1[i] = (Clust_Ch_[fir][i][best1]*dw)/sq12;
        sigm2_1[i] = sigm_1[i]*sigm_1[i];
      }//if coord was

      if ( XVU_Ch_[sec][i][best2] > -900.) {
        h2[i] = 1;
        sigm_2[i] = (Clust_Ch_[sec][i][best2]*dw)/sq12;
        sigm2_2[i] = sigm_2[i]*sigm_2[i];
      }//if coord was
    }//i6


    for(Int_t im=0; im<4; im++) {
      for(Int_t ii=0; ii<4; ii++) {
        Amatr[im][ii] = 0.;
        bmatr[im][ii] = 0.;
      }
    }

    FillFitMatrix(fir, Amatr, z_gl_, sigm2_1, h1);
    FillFitMatrix(sec, Amatr, z_gl_, sigm2_2, h2);

    Double_t matrF[4] = {0,0,0,0};//free coef

    FillFreeCoefVector(fir, matrF, XVU_Ch, best1, z_gl_ , sigm2_1, h1);
    FillFreeCoefVector(sec, matrF, XVU_Ch, best2, z_gl_ , sigm2_2, h2);

    if (fDebug) {
      for(Int_t i = 0; i < kNPlanes; i++) {
         //cout<<" h1 "<<h1[i]<<" XVU_Ch1 "<<XVU_Ch_[fir][i][best1]<<" sigm2 "<< sigm2_1[i]<<endl;
        //<<" z "<<z_gl_[fir][i]<<endl;
      }
      cout<<endl;
      for(Int_t i = 0; i < 6; i++) {
         //cout<<" h2 "<<h2[i]<<" XVU_Ch2 "<<XVU_Ch_[sec][i][best2]<<" sigm2 "<< sigm2_2[i]<<endl;
        //<<" z "<<z_gl_[sec][i]<<endl;
      }
    }
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
      par_ab_pair_[Pair1][i1][bst] = 0;
      for(Int_t j1 = 0; j1 < 4; j1++) {
        par_ab_pair_[Pair1][i1][bst] += bmatr[i1][j1]*matrF[j1];
         //if (fDebug)cout<<"bmatr["<<i1<<"]["<<j1<<"] "<<bmatr[i1][j1]<<endl;
      }
    } // i1

    Double_t Xtarget = par_ab_pair_[Pair1][0][bst]*( Z0_SRC - kZ_midle_pair[Pair1] ) + par_ab_pair_[Pair1][1][bst];//? + shift_pair[Pair1][1];
    Double_t Ytarget = par_ab_pair_[Pair1][2][bst]*( Z0_SRC - kZ_midle_pair[Pair1] ) + par_ab_pair_[Pair1][3][bst];//? + shift_pair[Pair1][3];

    if (fDebug) {
      cout<<endl;
      cout<<" Xtarget "<<Xtarget<<" Ytarget "<<Ytarget<<endl;
      cout<<" Pair "<<Pair1<<" par [0] "<<par_ab_pair_[Pair1][0][bst]<<" par [1] "<<par_ab_pair_[Pair1][1][bst]<<" par [2] "<<par_ab_pair_[Pair1][2][bst]<<" par [3] "<<par_ab_pair_[Pair1][3][bst]<<endl;
      cout<<endl;
    }

    Float_t dx_[kNPlanes];
    Chi2_ndf_pair_[Pair1][bst] = 0;

    for(Int_t i1 = 0 ; i1 < kNPlanes; i1++) {
      dx_[i1] = 0.;

      if ( XVU_Ch_[fir][i1][best1] > -900.) {
        if(i1==0 || i1==3) dx_[i1]=XVU_Ch_[fir][i1][best1]-par_ab_pair_[Pair1][0][bst]*z_gl_[fir][i1]-par_ab_pair_[Pair1][1][bst];
        if(i1==2 || i1==5) dx_[i1]=XVU_Ch_[fir][i1][best1]-0.5*(par_ab_pair_[Pair1][0][bst]+sq3*par_ab_pair_[Pair1][2][bst])*z_gl_[fir][i1]-0.5*(par_ab_pair_[Pair1][1][bst]+sq3*par_ab_pair_[Pair1][3][bst]);
        if(i1==1 || i1==4) dx_[i1]=XVU_Ch_[fir][i1][best1]-0.5*(par_ab_pair_[Pair1][0][bst]-sq3*par_ab_pair_[Pair1][2][bst])*z_gl_[fir][i1]-0.5*(par_ab_pair_[Pair1][1][bst]-sq3*par_ab_pair_[Pair1][3][bst]);
        Chi2_ndf_pair_[Pair1][bst]= Chi2_ndf_pair_[Pair1][bst]+dx_[i1]*dx_[i1]/(sigm2_1[i1]);
        //if ( fDebug) cout<<"best1 "<<best1 <<" i1 "<<i1<<" dx_ "<<dx_[i1]<<" XVU_Ch1 "<<XVU_Ch_[fir][i1][best1]<<" Chi2_ndf_Ch1_2 "<<Chi2_ndf_pair_[Pair1][bst]<<" z_gl1 "<<z_gl_[fir][i1]<<endl;

      }// if( Wires_Ch1[i1][best2]>-1){
    }

    for(Int_t i2 = 0 ; i2 < kNPlanes; i2++) {

      if ( XVU_Ch_[sec][i2][best2] > -900.) { // if(Wires_Ch2_[i2][best2]>-1){
        if(i2==0 || i2==3) dx_[i2]=XVU_Ch_[sec][i2][best2]-par_ab_pair_[Pair1][0][bst]*z_gl_[sec][i2]-par_ab_pair_[Pair1][1][bst];
        if(i2==2 || i2==5) dx_[i2]=XVU_Ch_[sec][i2][best2]-0.5*(par_ab_pair_[Pair1][0][bst]+sq3*par_ab_pair_[Pair1][2][bst])*z_gl_[sec][i2]-0.5*(par_ab_pair_[Pair1][1][bst]+sq3*par_ab_pair_[Pair1][3][bst]);
        if(i2==1 || i2==4) dx_[i2]=XVU_Ch_[sec][i2][best2]-0.5*(par_ab_pair_[Pair1][0][bst]-sq3*par_ab_pair_[Pair1][2][bst])*z_gl_[sec][i2]-0.5*(par_ab_pair_[Pair1][1][bst]-sq3*par_ab_pair_[Pair1][3][bst]);
        Chi2_ndf_pair_[Pair1][bst]= Chi2_ndf_pair_[Pair1][bst]+dx_[i2]*dx_[i2]/(sigm2_2[i2]);
        //if ( fDebug) cout<<"best2 "<<best2 <<" i2 "<<i2<<" dx_ "<<dx_[i2]<<" XVU_Ch2 "<<XVU_Ch_[sec][i2][best2]<<" Chi2_ndf_Ch1_2 "<<Chi2_ndf_pair_[Pair1][bst]<<" z_gl2 "<<z_gl_[sec][i2]<<endl;
      }// if( Wires_Ch2[i2][best2]>-1){
    }

    if (Nhits_Ch_[fir][best1]+Nhits_Ch_[sec][best2]> 4)
      Chi2_ndf_pair_[Pair1][bst]= Chi2_ndf_pair_[Pair1][bst]/(Nhits_Ch_[fir][best1]+Nhits_Ch_[sec][best2]-4);
    if (fDebug) hChi2_ndf_pair.at(Pair1)->Fill(Chi2_ndf_pair_[Pair1][bst]);
    if ( fDebug) cout<<" in fun Chi2_ndf_pair["<<Pair1<<"]["<<bst<<"] "<< Chi2_ndf_pair_[Pair1][bst]<<endl;

    ind_best_pair_[Pair1][bst]= bst;
    Nhits_pair_[Pair1][bst] = Nhits_Ch_[fir][best1]+Nhits_Ch_[sec][best2];
    for(Int_t i1 = 0 ; i1 < 4; i1++) {
      for(Int_t j1 = 0; j1 < 4; j1++) {
        sigma2_p[Pair1][bst][i1][j1] = 2*bmatr[i1][j1];
      }
    }

  }//< Nbest_Ch12_g_l
}//SegmentFit
//----------------------------------------------------------------------



//--------------------Matrix      Coefficients Calculation--------------
void BmnMwpcTrackFinder::FillFitMatrix(Int_t chN, Double_t** AA, Float_t** z, Double_t* sigmm2, Int_t* h_) {

  // AA - matrix to be filledlayers)
  // sigm2 - square of sigma
  // h_ - array to include/exclude planes (h_[i] = 0 or 1)

  Float_t z2_[6] = {z[chN][0] * z[chN][0], z[chN][1] * z[chN][1], z[chN][2] * z[chN][2], z[chN][3] * z[chN][3], z[chN][4] * z[chN][4], z[chN][5] * z[chN][5]}; //cm

  AA[0][0] += 2 * z2_[0] * h_[0] / sigmm2[0]+   z2_[2] * h_[2] / (2 * sigmm2[2])+   z2_[1] * h_[1] / (2 * sigmm2[1])+ 2 * z2_[3] * h_[3] / sigmm2[3]+   z2_[5] * h_[5] / (2 * sigmm2[5]) +   z2_[4] * h_[4] / (2 * sigmm2[4]); //Ax
  AA[0][1] += 2 * z[chN][0] * h_[0] / sigmm2[0]+   z[chN][2] * h_[2] / (2 * sigmm2[2])+   z[chN][1] * h_[1] / (2 * sigmm2[1])+ 2 * z[chN][3] * h_[3] / sigmm2[3]+   z[chN][5] * h_[5] / (2 * sigmm2[5])+   z[chN][4] * h_[4] / (2 * sigmm2[4]); //Bx
  AA[0][2] += sq3 * (z2_[2] * h_[2] / (2 * sigmm2[2]) -     z2_[1] * h_[1] / (2 * sigmm2[1]) +     z2_[5] * h_[5] / (2 * sigmm2[5])-     z2_[4] * h_[4] / (2 * sigmm2[4])); //Ay
  AA[0][3] += sq3 * (z[chN][2] * h_[2] / (2 * sigmm2[2]) -     z[chN][1] * h_[1] / (2 * sigmm2[1]) +     z[chN][5] * h_[5] / (2 * sigmm2[5]) -     z[chN][4] * h_[4] / (2 * sigmm2[4])); //By
  AA[1][0] = AA[0][1];
  AA[1][1] +=  2 * h_[0] / sigmm2[0] + 0.5 * h_[2] / sigmm2[2] + 0.5 * h_[1] / sigmm2[1]+  2 * h_[3] / sigmm2[3] + 0.5 * h_[5] / sigmm2[5]+ 0.5 * h_[4] / sigmm2[4];
  AA[1][2] += sq3 * (z[chN][2] * h_[2] / sigmm2[2]- z[chN][1] * h_[1] / sigmm2[1] + z[chN][5] * h_[5] / sigmm2[5]- z[chN][4] * h_[4] / sigmm2[4]) * 0.5;
  AA[1][3] += sq3 * (h_[2] / sigmm2[2] -     h_[1] / sigmm2[1] +     h_[5] / sigmm2[5]-     h_[4] / sigmm2[4]) * 0.5;
  AA[2][0] = AA[0][2];
  AA[2][1] = AA[1][2];
  AA[2][2] += 3.0 * (z2_[2] * h_[2] / sigmm2[2]+     z2_[1] * h_[1] / sigmm2[1]+     z2_[5] * h_[5] / sigmm2[5]+     z2_[4] * h_[4] / sigmm2[4]) * 0.5;
  AA[2][3] += 3.0 * (z[chN][2] * h_[2] / sigmm2[2]+     z[chN][1] * h_[1] / sigmm2[1] +     z[chN][5] * h_[5] / sigmm2[5] +     z[chN][4] * h_[4] / sigmm2[4])  * 0.5;
  AA[3][0] = AA[0][3];
  AA[3][1] = AA[1][3];
  AA[3][2] = AA[2][3];
  AA[3][3] += 3.0 * (0.5 * h_[2] / sigmm2[2]+ 0.5 *    h_[1] / sigmm2[1]+ 0.5 *    h_[5] / sigmm2[5]+ 0.5 *    h_[4] / sigmm2[4]);

}
//----------------------------------------------------------------------


//--------------------Matrix      Coefficients Calculation--------------
void BmnMwpcTrackFinder::FillFreeCoefVector(Int_t ichNum, Double_t* F, Double_t*** XVU_, Int_t ise, Float_t** z, Double_t* sigmm2, Int_t* h_) {
  // F - vector to be filled
  // XVU_ - coordinates of segment in chamber (Is it correct definition?)
  // segIdx - index of current segment
  // z - local z-positions of planes(layers)
  // sigmm2 - square of sigma
  // h_ - array to include/exclude planes (h_[i] = 0 or 1)

  F[0] += 2 * XVU_[ichNum][0][ise] * z[ichNum][0] * h_[0] / sigmm2[0] + XVU_[ichNum][1][ise] * z[ichNum][1] * h_[1] / sigmm2[1]+ XVU_[ichNum][2][ise] * z[ichNum][2] * h_[2] / sigmm2[2] + 2 * XVU_[ichNum][3][ise] * z[ichNum][3] * h_[3] / sigmm2[3] +XVU_[ichNum][4][ise] * z[ichNum][4] * h_[4] / sigmm2[4] + XVU_[ichNum][5][ise] * z[ichNum][5] * h_[5] / sigmm2[5];
  F[1] += 2 * XVU_[ichNum][0][ise] * h_[0] / sigmm2[0] + XVU_[ichNum][1][ise] * h_[1] / sigmm2[1] + XVU_[ichNum][2][ise] * h_[2] / sigmm2[2] + 2 * XVU_[ichNum][3][ise] * h_[3] / sigmm2[3] + XVU_[ichNum][4][ise] * h_[4] / sigmm2[4] + XVU_[ichNum][5][ise] * h_[5] / sigmm2[5];
  F[2] += sq3*(-XVU_[ichNum][1][ise] * z[ichNum][1] * h_[1] / sigmm2[1] + XVU_[ichNum][2][ise] * z[ichNum][2] * h_[2] / sigmm2[2] - XVU_[ichNum][4][ise] * z[ichNum][4] * h_[4] / sigmm2[4] + XVU_[ichNum][5][ise] * z[ichNum][5] * h_[5] / sigmm2[5]);
  F[3] += sq3*(-XVU_[ichNum][1][ise] * h_[1] / sigmm2[1] + XVU_[ichNum][2][ise] * h_[2] / sigmm2[2] - XVU_[ichNum][4][ise] * h_[4] / sigmm2[4] + XVU_[ichNum][5][ise] * h_[5] / sigmm2[5]);
}
//----------------------------------------------------------------------


//--------------------Matrix      Coefficients Calculation--------------
void BmnMwpcTrackFinder::InverseMatrix(Double_t** AA, Double_t** bb) {
  // Gaussian algorithm for 4x4 matrix inversion
  Double_t factor;
  Double_t temp[4];

  // Set b to I
  for (Int_t i1 = 0; i1 < 4; i1++)
    for (Int_t j1 = 0; j1 < 4; j1++)
      if (i1 == j1) bb[i1][j1] = 1.0;
      else bb[i1][j1] = 0.0;

  for (Int_t i1 = 0; i1 < 4; i1++) {
    for (Int_t j1 = i1 + 1; j1 < 4; j1++) {
      if (fabs(AA[i1][i1]) < fabs(AA[j1][i1])) {
        for (Int_t l1 = 0; l1 < 4; l1++) temp[l1] = AA[i1][l1];
        for (Int_t l1 = 0; l1 < 4; l1++) AA[i1][l1] = AA[j1][l1];
        for (Int_t l1 = 0; l1 < 4; l1++) AA[j1][l1] = temp[l1];
        for (Int_t l1 = 0; l1 < 4; l1++) temp[l1] = bb[i1][l1];
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
        //if ( fDebug) cout<<"AA["<<j1<<"]["<<k1<<"] "<<AA[j1][k1]<<endl;
        //if ( fDebug) cout<<"bb["<<j1<<"]["<<k1<<"] "<<bb[j1][k1]<<endl;
      }
    }
  } // i1
  //end inverse
}
//----------------------------------------------------------------------



//--------------------Calculation Efficiency----------------------------
void BmnMwpcTrackFinder::FillEfficiency(Int_t ChN, Double_t ***XVU_Ch_, Int_t **Nhits_C, Int_t MinHits, Int_t ind_best, Float_t min_distX, Float_t min_distY ) {
  //cout<<" FillEfficiency : ChN "<<ChN<<" min_distX "<<min_distX<<" min_distY "<<min_distY<<" Nhits_Ch["<<ChN<<"]["<<ind_best<<"] "<<Nhits_Ch[ChN][ ind_best ]<<endl;
  // segIdx - index of current segment ch2 or ch3 // Int_t ind_best_Ch[5]
  //4p&4p -> all matched / Efficiency per layer

  if (fabs(min_distX)< 5. && fabs(min_distY)< 5.5) {
    for(int i1 = 0 ; i1 < 6; i1++) {
      //if (fDebug) cout<<" XVU_Ch_["<<ChN<<"]["<<i1<<"]["<< ind_best<<"] = "<<XVU_Ch_[ChN][i1][ ind_best]<<endl;
      if( XVU_Ch_[ChN][i1][ ind_best ] > -999. && Nhits_Ch[ChN][ ind_best ] == MinHits) continue;//segIdx[ChN][j]
        if (fDebug)Denom_Ch.at(ChN)->Fill(i1);
      if(XVU_Ch_[ChN][i1][ ind_best ] > -999.) {
        if (fDebug)Nomin_Ch.at(ChN)->Fill(i1);
      }
    }// i1
  }//min_distX
}// FillEfficiency
//----------------------------------------------------------------------


//----------------------------------------------------------------------
InitStatus BmnMwpcTrackFinder::Init() {
  
  FairRootManager* ioman = FairRootManager::Instance();
  
  if (!expData) {
    if (fDebug) cout<<" !expData "<<endl;
    fBmnHitsArray = (TClonesArray*)ioman->GetObject(fInputBranchNameHit);
    if(fVerbose) cout << "fBmnHitsArray = " << fInputBranchNameHit << "\n";
    if (!fBmnHitsArray) {
      cout << "BmnMwpcTrackFinder::Init(): branchHits " << fInputBranchNameHit << " not found! Task will be deactivated" << endl;
      SetActive(kFALSE);
      return kERROR;
    }
  }else{
    if (fDebug) cout << " BmnMwpcTrackFinder::Init() " << endl;
  }
  

  fBmnMwpcSegmentsArray = (TClonesArray*) ioman->GetObject(fInputBranchName);
  if (!fBmnMwpcSegmentsArray)
  {
    cout<<"BmnMwpcTrackFinder::Init(): branch "<<fInputBranchName<<" not found! Task will be deactivated"<<endl;
    SetActive(kFALSE);
    return kERROR;
  }

  fBmnMwpcTracksArray = new TClonesArray(fOutputBranchName.Data());
  ioman ->Register(fOutputBranchName.Data(), "MWPC", fBmnMwpcTracksArray, kTRUE);

  fMwpcGeo   = new BmnMwpcGeometrySRC(fRunPeriod, fRunNumber);
  kNChambers = fMwpcGeo -> GetNChambers();
  kNPlanes   = fMwpcGeo -> GetNPlanes(); // 6
  if (fDebug) printf("fRunPeriod: %d %d %d\n", fRunPeriod, kNChambers, kNPlanes);

  kMinHits  = 4;
  kChi2_Max = 20.;
  kmaxPairs = 15;//10;//5;

  dw = fMwpcGeo->GetWireStep();//0.25; // [cm] // wires step
  dw_half   = 0.5*dw;
  sq3       = sqrt(3.);
  sq2       = sqrt(2.);
  sq12      = sqrt(12.);
  sigma     = dw/sq12;
  kMiddlePl = 47.25;
  ktarget_region  = 10.;// 04.05.21 VVP // 4.;
  ktarget_regionY = 10.;// 4.*sq2;
  kX_target = 0;
  kY_target = 0;
  Z0_SRC = 0.;
  if (fRunPeriod == 7){
    kX_target =  0.5;
    kY_target = -4.5;
    Z0_SRC    = -647.476;
  }
  if (fRunPeriod == 8){
    kX_target =  0.;
    kY_target =  0.;
    Z0_SRC    = -574.91;
  }

  ChCent          = new TVector3[kNChambers];
  ZCh             = new Float_t[kNChambers];
  kZmid           = new Float_t[kNChambers];
  shift           = new Float_t*[kNChambers];
  shift_pair      = new Float_t*[kNumPairs];
  kZ_midle_pair   = new Float_t[kNumPairs];
  XVU_Ch          = new Double_t**[kNChambers];
  Clust_Ch        = new Double_t**[kNChambers];
  par_ab_Ch       = new Double_t**[kNChambers];
  par_ab_pair     = new Double_t**[kNumPairs];
  sigma2_pair     = new Double_t***[kNumPairs];
  kPln            = new Int_t*[kNChambers];
  kZ_loc          = new Float_t*[kNChambers];
  z_gl            = new Float_t*[kNChambers];
  Chi2_match_pair = new Double_t*[kNumPairs];
  Chi2_ndf_pair   = new Double_t*[kNumPairs];
  ind_best_pair   = new Int_t*[kNumPairs]; 
  Chi2_ndf_Ch     = new Double_t*[kNChambers];
  Nhits_Ch        = new Int_t*[kNChambers];
  Nhits_match     = new Int_t*[kNChambers];
  Nhits_pair      = new Int_t*[kNumPairs];
  sigm2           = new Float_t[kNPlanes];
  ipl             = new Int_t[kNPlanes];
  Nbest_pair      = new Int_t[kNumPairs];
  Nbest_Ch        = new Int_t[kNChambers];
  ind_best_Ch     = new Int_t*[kNChambers];
  sigma_delta     = new Float_t*[kNumPairs];
  
  Amatr = new Double_t*[4];
  bmatr = new Double_t*[4];

  for(Int_t ii=0; ii<4; ii++) {
    Amatr[ii] = new Double_t[4];
    bmatr[ii] = new Double_t[4];
  }
  

  for(Int_t i = 0; i < kNChambers; i++) {
    if (fDebug){
      TH1D *h;
      h = new TH1D(Form("par_Ax_Ch%d",i), Form("par_Ax_Ch%d",i), 100, -.4, .4);     fList.Add(h);hpar_Ax_Ch.push_back(h);
      h = new TH1D(Form("par_Bx_Ch%d",i), Form("par_Bx_Ch%d",i), 100, -10., 10.0);  fList.Add(h);hpar_Bx_Ch.push_back(h);
      h = new TH1D(Form("par_Ay_Ch%d",i), Form("par_Ay_Ch%d",i), 100, -.4, .4);     fList.Add(h);hpar_Ay_Ch.push_back(h);
      h = new TH1D(Form("par_By_Ch%d",i), Form("par_By_Ch%d",i), 100, -10., 10.0);  fList.Add(h);hpar_By_Ch.push_back(h);
      h = new TH1D(Form("Chi2_ndf_Ch%d",i), Form("Chi2_ndf_Ch%d",i), 100, 0., 20.0);fList.Add(h);hChi2_ndf_Ch.push_back(h);
      h = new TH1D(Form("Nomin_Ch%d",i), Form("Nomin_Ch%d",i), 6, 0., 6.);          fList.Add(h);Nomin_Ch.push_back(h);
      h = new TH1D(Form("Denom_Ch%d",i), Form("Denom_Ch%d",i), 6, 0., 6.);          fList.Add(h);Denom_Ch.push_back(h);
      h = new TH1D(Form("Efficiency_Ch%d",i), Form("Efficiency_Ch%d",i), 6, 0., 6.);fList.Add(h);Eff_Ch.push_back(h);
    }

    shift[i]       = new Float_t[4];
    ChCent[i]      = fMwpcGeo -> GetChamberCenter(i);
    ZCh[i]         = ChCent[i].Z();
    shift[i][0]    = fMwpcGeo -> GetTx(i);
    shift[i][2]    = fMwpcGeo -> GetTy(i);
    shift[i][1]    = ChCent[i].X();
    shift[i][3]    = ChCent[i].Y();
    kPln[i]        = new Int_t[kNPlanes];
    kZ_loc[i]      = new Float_t[kNPlanes];
    z_gl[i]        = new Float_t[kNPlanes];
    XVU_Ch[i]      = new Double_t*[kNPlanes];
    Clust_Ch[i]    = new Double_t*[kNPlanes];
    par_ab_Ch[i]   = new Double_t*[4];
    Nhits_Ch[i]    = new Int_t[kBig];
    Chi2_ndf_Ch[i] = new Double_t[kBig];
    ind_best_Ch[i] = new Int_t[kmaxPairs];
    Nhits_match[i] = new Int_t[kmaxPairs];
  }

  //----- hists booking -----
  for(Int_t i = 0; i < kNChambers; i++) {
    if (i== 0 || i== 2) {
      kZmid[i] = (ZCh[i]   - ZCh[i + 1]) * 0.5;
    }
    if (i== 1 || i== 3) {
      kZmid[i] = (ZCh[i - 1] - ZCh[i])   * -0.5;
    }
    if (fDebug) printf("Chamber %d Z: %f Zmid: %f\n", i, ZCh[i], kZmid[i]);
  }

  for (int i=0; i < kNumPairs; ++i) {
    if (fDebug){
      TH1D *h;
      h = new TH1D(Form("Chi2_match_pair%d",i), Form("Chi2_match_pair%d",i), 100, 0., 100.0);fList.Add(h);hChi2_match_pair.push_back(h);
      h = new TH1D(Form("par_Ax_pair%d",i), Form("slopeX pair%d; [degrees]; Events",i), 100, -2.3, 2.3);fList.Add(h);hpar_Ax_pair.push_back(h);
      h = new TH1D(Form("par_Bx_pair%d",i), Form("posX pair%d; [cm]; Events",i), 100, -10., 10.0);fList.Add(h);hpar_Bx_pair.push_back(h);
      h = new TH1D(Form("par_Ay_pair%d",i), Form("slopeY pair%d; [degrees]; Events",i), 100, -2.3, 2.3);fList.Add(h);hpar_Ay_pair.push_back(h);
      h = new TH1D(Form("par_By_pair%d",i), Form("posY pair%d; [cm]; Events",i), 100, -10., 10.0);fList.Add(h);hpar_By_pair.push_back(h);
      
      h = new TH1D(Form("Chi2_ndf_pair%d",i), Form("Chi2_ndf_pair%d",i), 30, 0., 30.0);fList.Add(h);hChi2_ndf_pair.push_back(h);
      h = new TH1D(Form("Nbest_pair%d", i), Form("Nbest_pair%d; Ntracks; Events", i), 5, 0., 5.);fList.Add(h);hNbest_pair.push_back(h);
      h = new TH1D(Form("theta_pair%d",i),Form("theta_pair%d; [degrees]; Events",i), 160, 0., 8.);fList.Add(h);hpar_theta_pair.push_back(h);
      h = new TH1D(Form("phi_pair%d",i), Form("phi_pair%d;  [degrees]; Events",i), 380, -190., 190.);fList.Add(h);hpar_phi_pair.push_back(h);

      h = new TH1D(Form("dX_pair%d",i), Form("dX_pair%d; cm; Events",i), 100, -5.,5.);fList.Add(h);hdX_pair.push_back(h);
      h = new TH1D(Form("dY_pair%d",i), Form("dY_pair%d; cm; Events",i), 100, -5.,5.);fList.Add(h);hdY_pair.push_back(h);
      h = new TH1D(Form("dAx_pair%d",i),Form("dAx_pair%d; rad;Events",i), 100, -.5,.5);fList.Add(h);hdAx_pair.push_back(h);
      h = new TH1D(Form("dAy_pair%d",i),Form("dAy_pair%d; rad;Events",i), 100, -.5,.5);fList.Add(h);hdAy_pair.push_back(h);

      h = new TH1D(Form("X_in_target_pair%d",i), Form(" posX_pair%d in target;[cm]; Events ",i), 100, -10.,10.);fList.Add(h);hX_in_target_pair.push_back(h);
      h = new TH1D(Form("Y_in_target_pair%d",i), Form(" posY_pair%d in target;[cm]; Events ",i), 100, -10.,10.);fList.Add(h);hY_in_target_pair.push_back(h);

      TH2D *h1;
      h1 = new TH2D(Form("YvsX_pair%d",i), Form("YvsX_pair%d; X[cm]; Y[cm]",i),  100, -25, 25, 100, -21, 21);fList.Add(h1);hYvsX_pair.push_back(h1);
     
      h1 = new TH2D(Form("Y_X_in_target_pair%d",i), Form("posY vs posX pair%d in target; X[cm]; Y[cm]",i), 150, -7.5,7.5, 150, -13., 2.);fList.Add(h1);hY_X_in_target_pair.push_back(h1);
      h1 = new TH2D(Form("Ax_bx_in_target_pair%d",i), Form("slopeX vs posX in target pair%d; posX[cm]; slopeX",i), 100, -10.,10., 100, -2.3, 2.3);fList.Add(h1);hAx_bx_in_target_pair.push_back(h1);
      h1 = new TH2D(Form("Ay_by_in_target_pair%d",i), Form("slopeY vs posY in target pair%d; posY[cm]; slopeY",i), 100, -10.,10., 100, -2.3, 2.3);fList.Add(h1);hAy_by_in_target_pair.push_back(h1);
    }//if (fDebug)
    Chi2_match_pair[i] = new Double_t[kmaxPairs];
    Chi2_ndf_pair[i]   = new Double_t[kmaxPairs];
    ind_best_pair[i]   = new Int_t[kmaxPairs];
    Nhits_pair[i]      = new Int_t[kmaxPairs];
    par_ab_pair[i]     = new Double_t*[4];
    sigma2_pair[i]     = new Double_t**[kBig];
    shift_pair[i]      = new Float_t[4];
    sigma_delta[i]     = new Float_t[4];
  }
  
  for(Int_t i = 0; i < kNumPairs; i++) {
    for(Int_t j = 0; j < kBig; j++) {
     sigma2_pair[i][j] = new Double_t*[4];
    }
  }
  for(Int_t i = 0; i < kNumPairs; i++) {
    for(Int_t j = 0; j < kBig; j++) {
      for(Int_t k = 0; k < 4; k++) {
       sigma2_pair[i][j][k] = new Double_t[4];
      }
    }
  }

  if (fDebug){
    hAx_bx_in_target= new TH2D("Ax_bx_in_target", "slopeX vs posX in target; posX[cm]; slopeX", 100, -10.,10., 100, -2.3, 2.3);fList.Add(hAx_bx_in_target);
    hAy_by_in_target= new TH2D("Ay_by_in_target", "slopeY vs posY in target; posY[cm]; slopeY", 100, -10.,10., 100, -2.3, 2.3);fList.Add(hAy_by_in_target);
    hY_X_in_target = new TH2D("Y_X_in_target", "posY vs posX (pair0) in target; X[cm]; Y[cm]", 100, -10.,10., 100, -10., 10.);fList.Add(hY_X_in_target);

    htheta_p1vsp0 =  new TH2D("theta_p1vsp0", "theta pair1 vs pair0", 160, 0., 3., 160, 0., 3.);fList.Add(htheta_p1vsp0);
    hdX_pair01_vs_x1 = new TH2D("dX_pair01_vs_x1","dX(pair0- pair1)_vs_Xpair1;Xpair1[cm];dX(pair0- pair1)[cm]",100, -10.,10., 100, -10., 10.);fList.Add(hdX_pair01_vs_x1);
    hdY_pair01_vs_y1 = new TH2D("dY_pair01_vs_y1","dY(pair0- pair1)_vs_Ypair1;Ypair1[cm];dY(pair0- pair1)[cm]",100, -10.,10., 100, -10., 10.);fList.Add(hdY_pair01_vs_y1);
    hdX_pair01_inZpair1= new TH1D("dX_pair01_inZpair1","dX(pair0- pair1) inZpair1;X[cm]; ", 100, -10.,10.);fList.Add(hdX_pair01_inZpair1);
    hdY_pair01_inZpair1= new TH1D("dY_pair01_inZpair1","dY(pair0- pair1) inZpair1;Y[cm]; ", 100, -10.,10.);fList.Add(hdY_pair01_inZpair1);

    hdX_target =    new TH1D("dX_target", " PosX(pair0-pair1) in target;[cm]; Events ", 200, -10.,10.);fList.Add(hdX_target);
    hdY_target =    new TH1D("dY_target", " PosY(pair0-pair1) in target;[cm]; Events ", 200, -10.,10.);fList.Add(hdY_target);
    hdAx_target =   new TH1D("dAx_target", "SlopeX(pair0-pair1); rad; Events ", 100,  -.05, .05);fList.Add( hdAx_target);
    hdAy_target =   new TH1D("dAy_target", "SlopeY(pair0-pair1); rad; Events ", 100,  -.05, .05);fList.Add( hdAy_target);
    hChi2_m_target =  new TH1D("Chi2_m_target", "Chi2_m in target;; Events ", 100,0.,100.);fList.Add(hChi2_m_target);

    hChi2best_Chi2fake_before_target = new TH2D("Chi2best_Chi2fake_before_target","Chi2best_Chi2fake_before_target; Chi2_best; Chi2_second", 20, 0., 20., 20, 0., 20.);fList.Add(hChi2best_Chi2fake_before_target);
    hChi2best_Chi2fake_after_target = new TH2D("Chi2best_Chi2fake_after_target","Chi2best_Chi2fake_after_target; Chi2_best; Chi2_second", 100, 0., 100., 100, 0., 100.);fList.Add(hChi2best_Chi2fake_after_target);

    hdX_pair_1  =  new TH1D("dX_pair_1","dX_pair_1", 200, -20.,20.); fList.Add(hdX_pair_1);
    hdY_pair_1  =  new TH1D("dY_pair_1","dY_pair_1", 200, -20.,20.); fList.Add(hdY_pair_1);
    hdAx_pair_1 =  new TH1D("dAx_pair_1","dAx_pair_1",100,  -.3, .3);fList.Add(  hdAx_pair_1);
    hdAy_pair_1 =  new TH1D("dAy_pair_1","dAy_pair_1",100,  -.3, .3);fList.Add(  hdAy_pair_1);
    hChi2m_pair_1= new TH1D("Chi2m_pair_1","Chi2_xy_pair_1", 100, 0, 50);fList.Add(hChi2m_pair_1);
    hXv_pair_1  =  new TH1D("Xv_pair_1","Xv_pair_1",200, -10.,10.);fList.Add(hXv_pair_1);
    hYv_pair_1  =  new TH1D("Yv_pair_1","Yv_pair_1",200, -10.,10.);fList.Add(hYv_pair_1);
    
    hDen  =  new TH1D("hDen","hDen",1, 0, 1);fList.Add(hDen);
    hNum  =  new TH1D("hNum","hNum",1, 0, 1);fList.Add(hNum);
    hEff  =  new TH1D("hEff","hEff",1, 0, 1);fList.Add(hEff);
    
    hYvsX_mc_pair = new TH2D("hYvsX_mc_pair","YvsX_mc_pair" , 100, -25, 25, 100, -21, 21); fList.Add(hYvsX_mc_pair);
    
    hdAx_tr_mc =  new TH1D("hdAx_tr_mc","dAx_tr_mc",200, -0.005, 0.005);fList.Add(hdAx_tr_mc);
    hdX_tr_mc  =  new TH1D("hdX_tr_mc","dX_tr_mc",200, -0.2, 0.2);  fList.Add(hdX_tr_mc);
    hdAy_tr_mc =  new TH1D("hdAy_tr_mc","dAy_tr_mc",200, -0.005, 0.005);fList.Add(hdAy_tr_mc);
    hdY_tr_mc  =  new TH1D("hdY_tr_mc","dY_tr_mc",200, -0.2, 0.2);  fList.Add(hdY_tr_mc);
    
    hDen_mctr = new TH1D("hDen_mctr", "Den_mctr", 2, 0, 2);fList.Add(hDen_mctr);
    hNum_mctr = new TH1D("hNum_mctr", "Num_mctr", 2, 0, 2);fList.Add(hNum_mctr);
    hEff_mctr = new TH1D("hEff_mctr", "Eff_mctr", 2, 0, 2);fList.Add(hEff_mctr);
    hEff_mctr->GetXaxis()->SetBinLabel(1,"2+3 coord");
    hEff_mctr->GetXaxis()->SetBinLabel(2,"only 3coord");
    
    hNtrpc_reco       =  new TH1D("Ntrpc_reco",";N of pair1 tr reco", 10, 0,10); fList.Add(hNtrpc_reco);
    hNtrpc_mc         =  new TH1D("Ntrpc_mc",";N of tr pair1 mc", 10, 0,10);     fList.Add(hNtrpc_mc);
    hNtrpc_mc_vs_reco =  new TH2D("Ntrpc_mc_vs_reco","Ntr_mc_vs_reco; Nreco;Nmctrue", 10, 0,10, 10, 0,10);fList.Add(hNtrpc_mc_vs_reco);

    
  }//if (fDebug)
  Int_t i1 = 0;
  for(Int_t i = 0; i < kNumPairs; i++) {

    i1=i;
    sigma_delta[0][0] = 3*.14;//.0624;// sigm_dax
    sigma_delta[0][2] = 3*.14;//.066; // sigm_day
    sigma_delta[0][1] = 2*.35;//4.08;// sigm_dx
    sigma_delta[0][3] = 2*.35;//4.30;// sigm_dy

    if (i == 1) {
      i1=2;
      // pair 0
      sigma_delta[0][0] = 3*.14;
      sigma_delta[0][1] = 1.2;//2*.35;
      sigma_delta[0][2] = 3*.14;
      sigma_delta[0][3] = 1.3;//2*.35;
      // pair 1
      sigma_delta[1][0] = 6 *.1;// sigm_dax //VP
      sigma_delta[1][1] = 1.;//1.;//6.;//4.08;//2*.35;//4.08;// sigm_dx
      sigma_delta[1][2] = 6 *.15;// sigm_day
      sigma_delta[1][3] = 1.5;//1.//2*7.;//shift +10cm?  4.30;// 2*.35;//4.30;// sigm_dy
    }
    kZ_midle_pair[i] = ZCh[i1] + kZmid[i1+1];

    shift_pair[i][0]= (shift[i1+1][1] - shift[i1][1])/( ZCh[i1+1] - ZCh[i1] );
    shift_pair[i][2]= (shift[i1+1][3] - shift[i1][3])/( ZCh[i1+1] - ZCh[i1] );
    shift_pair[i][1]= 0.5*(shift[i1+1][1] + shift[i1][1]);
    shift_pair[i][3]= 0.5*(shift[i1+1][3] + shift[i1][3]);
    //Additional alignment 
    /*
    if (i == 0){// alignment with GEM detectors
      shift_pair[i][0] +=  0.;//Ax
      shift_pair[i][1] +=  0.05;//Bx
      shift_pair[i][2] += -0.0008;//Ay
      shift_pair[i][3] += -0.4;//By
    }
    */
    if (i == 0){
      shift_pair[i][0] +=  0.;//Ax
      shift_pair[i][1] +=  0.;//Bx
      shift_pair[i][2] +=  0.;//Ay
      shift_pair[i][3] +=  0.;//By
    }
    if (i == 1 && expData){
      shift_pair[i][0] +=  0.;//Ax
      shift_pair[i][1] +=  -0.05;//Bx
      shift_pair[i][2] +=  0.0009;//Ay
      shift_pair[i][3] +=  0.7;//By
    }

    //if (fDebug) cout<<" i "<<i<<" kZ_midle_pair[i] "<<kZ_midle_pair[i]<<" i1 "<<i1<<" i1+1 "<<i1+1<<" -( ZCh[i1]- ZCh[i1+1] )= "<<-( ZCh[i1]- ZCh[i1+1] )<<endl;
  }

  for(Int_t ichh = 0; ichh < kNChambers; ichh++) {
    for(int ii = 0; ii < 6; ii++) {

      if ( fRunPeriod == 6 ) {

        if ( ichh == 0 || ichh == 1) {
          kZ_loc[ichh][ii] = -0.5 + ii;
          if(ii == 4) {
            kZ_loc[ichh][ii] = -2.5;
          }
          if(ii == 5) {
            kZ_loc[ichh][ii] = -1.5;
          }
        }
      }

     if (fRunPeriod == 6 || (fRunPeriod == 7 && fRunNumber > 3588) ) { // BMN

        if ( ichh == 0 || ichh == 1) {
          kZ_loc[ichh][ii] = -0.5 + ii;
          if(ii == 4) {
            kZ_loc[ichh][ii] = -2.5;
          }
          if(ii == 5) {
            kZ_loc[ichh][ii] = -1.5;
          }
        }
      }
      if (fRunPeriod == 7 && fRunNumber <= 3588 || fRunPeriod == 8) { //SRC

        if ( ichh == 0 ) {
          kZ_loc[ichh][0] = -1.5;
          kZ_loc[ichh][1] = -0.5;
          kZ_loc[ichh][2] =  0.5;
          kZ_loc[ichh][3] =  1.5;
          kZ_loc[ichh][4] =  2.5;
          kZ_loc[ichh][5] = -2.5;
        }
        if(ichh == 1) {
          kZ_loc[ichh][0] = -1.5;
          kZ_loc[ichh][1] = -2.5;
          kZ_loc[ichh][2] =  2.5;
          kZ_loc[ichh][3] =  1.5;
          kZ_loc[ichh][4] =  0.5;
          kZ_loc[ichh][5] = -0.5;
        }
        if ( fRunPeriod == 7 && (ichh == 2 || ichh == 3) ) {

          kZ_loc[ichh][ii] = -0.5 + ii;
          if(ii == 4) {
            kZ_loc[ichh][ii] = -2.5;
          }
          if(ii == 5) {
            kZ_loc[ichh][ii] = -1.5;
          }
        }//fRunPeriod == 7  ch 2 3
        if ( fRunPeriod == 8 && (ichh == 2 || ichh == 3) ) {
          kZ_loc[ichh][0] =  0.5;
          kZ_loc[ichh][1] = -0.5;
          kZ_loc[ichh][2] = -1.5;
          kZ_loc[ichh][3] = -2.5;
          kZ_loc[ichh][4] =  2.5;
          kZ_loc[ichh][5] =  1.5;
        }//fRunPeriod == 8  ch 2 3
        
      }//SRC
      
      z_gl[ichh][ii] = kZmid[ichh] + kZ_loc[ichh][ii];

      if (fDebug) cout<<" ich "<<ichh<<" ii "<<ii<<" kZ_loc "<<kZ_loc[ichh][ii]<<" z_gl "<<z_gl[ichh][ii]<<endl;
    }
  }//ich


  for(Int_t ii = 0; ii < kNChambers; ii++) {
    for(Int_t iPla=0; iPla < kNPlanes; iPla++) {
      XVU_Ch[ii][iPla] = new Double_t[kBig];
      Clust_Ch[ii][iPla] = new Double_t[kBig];
    }
    for(Int_t i=0; i<4; i++) {
      par_ab_Ch[ii][i] = new Double_t[kBig];
    }
  }

  for(Int_t ip=0; ip < kNumPairs; ip++) {
    for(Int_t i4=0; i4<4; i4++) {
      par_ab_pair[ip][i4] = new Double_t[kmaxPairs];
    }
  }

  return kSUCCESS;
}//Init
//----------------------------------------------------------------------


//------ Arrays Initialization -----------------------------------------
void BmnMwpcTrackFinder::PrepareArraysToProcessEvent() {
  fBmnMwpcTracksArray->Clear();
  vec_points.clear();

  // Clean and initialize arrays:

  for(Int_t iCh = 0; iCh < kNChambers; iCh++) {
    Nbest_Ch[iCh] = 0;

    for(Int_t iPlane=0; iPlane<kNPlanes; iPlane++) {
      for(Int_t iBig=0; iBig<kBig; iBig++) {
        XVU_Ch[iCh][iPlane][iBig] = -999.;
        Clust_Ch[iCh][iPlane][iBig] = -1.;
      }//iBig
    }//iPlane

    for(Int_t ii=0; ii<4; ii++) {
      for(Int_t jj=0; jj<kBig; jj++) {
        par_ab_Ch[iCh][ii][jj] = 999.;
      }
    }

    for(Int_t iBig=0; iBig<kBig; iBig++) {
      Nhits_Ch[iCh][iBig] = 0;
      Chi2_ndf_Ch[iCh][iBig] = 0;
    }

    for(Int_t i=0; i< kmaxPairs; i++) {
      ind_best_Ch[iCh][i] = -1;
      Nhits_match[iCh][i] = 0;
    }
  }//iCh

  for(Int_t iPl=0; iPl<kNPlanes; iPl++) {
    sigm2[iPl] = sigma*sigma;
    ipl[iPl] = 6;
  }


  for(Int_t ip=0; ip < kNumPairs; ip++) {
    Nbest_pair[ip] = 0; //Nbest_Ch12_gl = 0;

    for(Int_t i4=0; i4 < 4; i4++) {
      for(Int_t i5=0; i5 < kmaxPairs; i5++) {
        par_ab_pair[ip][i4][i5] =999.;//par_ab_Ch1_2[ii][jj] = 999.;
      }
    }
    for(Int_t i=0; i <kmaxPairs; i++) {
      Chi2_match_pair[ip][i] = 999.;
      Chi2_ndf_pair[ip][i] = 999.;
      ind_best_pair[ip][i]= -1;
    }
    for(Int_t j = 0; j < kBig; j++) {
      for(Int_t k = 0; k < 4; k++) {
        for(Int_t i4=0; i4 < 4; i4++) {
          sigma2_pair[ip][j][k][i4] = 0.;
        }
      }
    }
  }

}//PrepareArraysToProcessEvent
//----------------------------------------------------------------------


//----------------------------------------------------------------------
void BmnMwpcTrackFinder::Finish() {
  delete fMwpcGeo;

  if (fDebug) {
    printf("MWPC track finder: write hists to file... ");
    fOutputFileName = Form("hMWPCtracks_p%d_run%d.root", fRunPeriod, fRunNumber);
    cout<< fOutputFileName <<endl;
    TFile file(fOutputFileName, "RECREATE");

    for(Int_t iCh = 0; iCh < kNChambers; iCh++) {
      Eff_Ch.at(iCh)->Sumw2();
      Eff_Ch.at(iCh)->Divide(Nomin_Ch.at(iCh),Denom_Ch.at(iCh),1,1);
    }
    
    hEff->Divide(hNum,hDen,1,1);
    hEff_mctr->Divide(hNum_mctr,hDen_mctr,1,1);
    fList.Write();
    file.Close();
  }
  if (fDebug) printf("done\n");

  cout << "Work time of the MWPC track finder: " << workTime << " s" << endl;
}//Finish
//----------------------------------------------------------------------

ClassImp(BmnMwpcTrackFinder)
