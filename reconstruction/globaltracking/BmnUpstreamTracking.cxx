// Author: Vasilisa Lenivenko (VBLHEP) <vasilisa@jinr.ru> 2019-07-18

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// BmnUpstreamTracking                                                        //
//                                                                            //
// Implementation of an algorithm developed by                                //
// V.Lenivenko and V.Paltchik                                                 //
// to the BmnRoot software                                                    //
//                                                                            //
// The algorithm serves for searching for tracks                              //
// in the Silicon detectors & MWPCs                                           //
// of the configuration SRC of BM@N experiment                                //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#include <Rtypes.h>
#include <TMath.h>
#include "TFile.h"
#include "FairLogger.h"
#include "BmnUpstreamTracking.h"
#include <vector>

static Double_t workTime = 0.0;
TList fhList;

//----------------------------------------------------------------------
void BmnUpstreamTracking::Exec(Option_t* opt) {
  if (!IsActive()) return;

  clock_t tStart = clock();
  PrepareArraysToProcessEvent();
  if (fDebug) cout << "\n======================== Upstream track finder exec started  ===================\n" << endl;
  if (fDebug) printf("Event number: %d\n", fEventNo);
  fEventNo++;
  
  if (fDebug) cout<<"--------------Read-------------"<<endl;
  ReadSiliconTracks(par_ab_SiTr,par_SiTr_z,NSiTracks, vec_points);
  ReadSiliconHits(SiXYhits,NSiXYhits);
  
  if (fDebug) cout<<"--------------Reco: MWPCSegments-----"<<endl;
  ReadMWPCSegments(par_ab_Ch,par_Seg_z,Nseg_Ch, vec_points);
  if (fDebug) cout<<"--------------Reco: MWPCTracks"<<endl;
  ReadMWPCTracks(par_ab_tr,par_ab_trz,NPCTracks);
  
  //-----------------Matching-------------------------------------------
  if (fDebug) cout<<"--------------Matching Si-tracks & PC-tracks-----"<<endl;
  PCTracksSiTracksMatching(par_ab_SiTr,par_SiTr_z,NSiTracks,par_ab_tr,par_ab_trz,NPCTracks,vtmpSeg,OutVector);//Step1
  if (fDebug) cout<<"--------------Matching Si-tracks & PC-segments---"<<endl;
  PCSegmentsSiTracksMatching(par_ab_SiTr,par_SiTr_z,NSiTracks,par_ab_Ch,par_Seg_z,Nseg_Ch,vtmpSeg,OutVector); //Step2
  if (fDebug) cout<<"--------------Adding hits-------------------------"<<endl;
  GetAddSiHits(OutVector,SiXYhits,NSiXYhits);
  GetHitsToPoints(OutVector,par_ab_tr,NPCTracks,par_ab_Ch,Nseg_Ch,Points,NumPoints);
  
  if (fDebug) cout<<"--------------Fit--------------------------------"<<endl;
  CalculationOfChi2(Points, NumPoints, vecUpTracks);
  
  if (fDebug)   PrintAllTracks(vecUpTracks);
  if (!expData) MCefficiencyCalculation(vec_points, vecUpTracks);
  
  if (fDebug) cout<<"----TrackRecording--------------------------------"<<endl;
  TrackRecording(vecUpTracks);
  
  if (fDebug) cout << endl;
  if (fDebug) cout << "======================== Upstream track finder exec finished ===================" << endl;
  clock_t tFinish = clock();
  workTime += ((Double_t) (tFinish - tStart)) / CLOCKS_PER_SEC;
}
//----------------------------------------------------------------------

void BmnUpstreamTracking::MCefficiencyCalculation(vector<MC_points> &points, vector<UpTracks> &vecUp){
   
    if (fDebug) cout<<" ---UpMC tracks association--"<<endl;
    //                     ax,   bx,    ay,  by
    Double_t delta2[4] = {-999.,-999.,-999.,-999.}; 
    Double_t sig[4]    = {0.03, 4., 0.03, 4.};
    
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
    
    if (fDebug) cout<<" MC "<<points.size()<<" UpTracks "<<vecUp.size()<<endl;
   
    Int_t Ngood_mc_tracks = 0;
    Int_t Ngood_reco_tracks = 0;
    for (Int_t itr = 0; itr < points.size(); itr++) {//mc_tr
      
      //---MC Eff ---
      //---Den
      if (fDebug ) cout<<" Np "<<points.at(itr).Np<<" pdg "<<points.at(itr).Pdg<<endl;
      if (fDebug && points.at(itr).Np >=7 //>=10 
        && points.at(itr).np_3si == 0
        && (points.at(itr).Np_ch2 > 3 || points.at(itr).Np_ch3 > 3) ){
          // if ( points.at(itr).xWas3 && points.at(itr).uWas3 && points.at(itr).vWas3 &&
          //   points.at(itr).xWas2 && points.at(itr).uWas2 && points.at(itr).vWas2 ) 
        hDen_mcuptr->Fill(1);
        hDen_mcuptr->Fill(0);
        hY_vs_Xmctrue->Fill(points.at(itr).param[1],points.at(itr).param[3]);
        
        Ngood_mc_tracks++;
        cout<<" itr "<<itr<<" Ngood_mc_tracks "<<Ngood_mc_tracks<<endl;
        cout<<"UpDen"<<endl;
      }
      
      for(Int_t iup= 0; iup < vecUp.size(); iup ++) {//reco
        delta2[0] = points.at(itr).param[0] - vecUp.at(iup).param[0];
        delta2[1] = points.at(itr).param[1] - vecUp.at(iup).param[1];
        delta2[2] = points.at(itr).param[2] - vecUp.at(iup).param[2];
        delta2[3] = points.at(itr).param[3] - vecUp.at(iup).param[3];
        
        dmatch = 0.;
        dmatch = (delta2[0]*delta2[0])/(sig[0]*sig[0])+ 
                 (delta2[1]*delta2[1])/(sig[1]*sig[1])+
                 (delta2[2]*delta2[2])/(sig[2]*sig[2])+
                 (delta2[3]*delta2[3])/(sig[3]*sig[3]);
                 
        //combinatorics
        if (fDebug && delta2[0] > -900.)  hdAx_tr_mc_comb->Fill(delta2[0]); 
        if (fDebug && delta2[1] > -900.)  hdX_tr_mc_comb ->Fill(delta2[1]);
        if (fDebug && delta2[2] > -900.)  hdAy_tr_mc_comb->Fill(delta2[2]);
        if (fDebug && delta2[3] > -900.)  hdY_tr_mc_comb ->Fill(delta2[3]);
                 
       if (fDebug) cout<<" imc "<<itr<<" iup "<<iup<<" Nhits "<<vecUp.at(iup).Nhits<<" ax_mc "<<points.at(itr).param[0]<<" ay_mc "<<points.at(itr).param[2]
       <<" ax_up "<<vecUp.at(iup).param[0]<<" ay_up "<<vecUp.at(iup).param[2]<<
       " dmc_match "<< dmc_match[itr] << " "<<dmatch<<" delta2[0] "<<delta2[0]<<" delta2[1] "<<delta2[1]<<" delta2[2] "<<delta2[2]<<" delta2[3] "<<delta2[3]<<endl;
                         
        if ( dmc_match[itr] > dmatch){
          dmc_match[itr]    = dmatch;
          mc_tr_assoc[itr]  = iup;
          //vecUp.at(iup).Pdg = points.at(itr).Pdg;
          dax = delta2[0];
          dx  = delta2[1];
          day = delta2[2];
          dy  = delta2[3];
        }
        
      }//vecUp.size()
      if (fDebug){
         // cout<<" itr "<<itr<<" mc_Id "<<points.at(itr).Id<<
         //  " ax_mc "<<points.at(itr).paramSi[0]<<" reco_ind "<<mc_tr_assoc[itr]<<" ax "<< vecUp.at(mc_tr_assoc[itr]).param[0] <<
         //  " dmc_match "<<dmc_match[itr]<<endl;
        if (mc_tr_assoc[itr] > -1){
          cout<<" mc itr "<<itr<<" rec "<<mc_tr_assoc[itr]<<endl;
          if (dax > -900.) hdAx_uptr_mc->Fill(dax);
          if (dx > -900.)  hdX_uptr_mc ->Fill(dx);
          if (day > -900.) hdAy_uptr_mc->Fill(day);
          if (dy > -900.)  hdY_uptr_mc ->Fill(dy);

        }//if mc_tr_assoc[itr] > -1
      }//if
    }//points.size
    
    if (fDebug) cout<<"reject poorly chosen association segments "<<endl;
    for (Int_t itr = 0; itr < points.size(); itr++) {//mc_tr
      if (mc_tr_assoc[itr] == -1) continue;
       
      for (Int_t itr2 = 0; itr2 < points.size(); itr2++) {//mc_tr
        if (itr2 == itr) continue;
        if (mc_tr_assoc[itr2] == -1) continue;
        
        if (mc_tr_assoc[itr] ==  mc_tr_assoc[itr2]){
          if (dmc_match[itr2] > dmc_match[itr] ) mc_tr_assoc[itr2] = -1;
          else {
            mc_tr_assoc[itr] = -1;
            break;
          }
        }
      }//itr2
      //---MC Eff ---
      //---Num
      if (fDebug) cout<<" mc_Id "<<points.at(itr).Id<<" pdg "<<points.at(itr).Pdg<<" assoc "<<mc_tr_assoc[itr]<<endl;
      if (fDebug && mc_tr_assoc[itr] > -1 && fDebug && points.at(itr).Np >=7 //>= 10 
        && points.at(itr).np_3si == 0
        && (points.at(itr).Np_ch2 > 3 || points.at(itr).Np_ch3 > 3)){
        if ( points.at(itr).xWas3 && points.at(itr).uWas3 && points.at(itr).vWas3 &&
             points.at(itr).xWas2 && points.at(itr).uWas2 && points.at(itr).vWas2 )hNum_mcuptr->Fill(1);
             
        hNum_mcuptr->Fill(0);
        Ngood_reco_tracks++;
        //cout<<" itr "<<itr<<" Ngood_reco_tracks "<<Ngood_reco_tracks<<endl;
        cout<<"UpNum"<<endl;
      }
    }//itr
    if (fDebug) cout<<" Ngood_mc_tracks "<<Ngood_mc_tracks<<" Ngood_reco_tracks "<<Ngood_reco_tracks<<endl;
    if (fDebug) {
      hNtr_reco->Fill(Ngood_reco_tracks);
      hNtr_mc  ->Fill(Ngood_mc_tracks);
      hNtr_mc_vs_reco ->Fill(Ngood_reco_tracks,Ngood_mc_tracks);
      if (Ngood_mc_tracks == 2) hNrecoTrif2mc->Fill(Ngood_reco_tracks);
    }
    //if (fDebug && points.size() == 2) hNrecoTrif2mc->Fill(vecUp.size());
    for (Int_t itr = 0; itr < points.size(); itr++) {//mc_tr
     if (mc_tr_assoc[itr] > -1) vecUp.at(mc_tr_assoc[itr]).Pdg = points.at(itr).Pdg;
    }
    
    twotracks tt;
    Bool_t track1_was  = 0;
    Bool_t ifNmc2      = 0;
    Bool_t ifSelectedCases = 0;
    if (fDebug) cout<<" reco tracks: "<<endl;
    
    if (Ngood_mc_tracks == 2 ) ifNmc2 = 1;
    
    if ( vecUp.size() == 2 && Ngood_mc_tracks == 2){
      for(Int_t iup= 0; iup < vecUp.size(); iup ++) {//reco
        if (fDebug) cout<<" iup "<<iup<< " found code pdg "<<vecUp.at(iup).Pdg <<endl;
        if (track1_was == 0){
          track1_was =1;
          tt.track1_pdg = vecUp.at(0).Pdg; 
          tt.track1[0]  = vecUp.at(iup).param[0];
          tt.track1[1]  = vecUp.at(iup).param[1];
          tt.track1[2]  = vecUp.at(iup).param[2];
          tt.track1[3]  = vecUp.at(iup).param[3];
        }
        if (track1_was == 1){
          tt.track2_pdg = vecUp.at(1).Pdg;
          tt.track2[0]  = vecUp.at(iup).param[0];
          tt.track2[1]  = vecUp.at(iup).param[1];
          tt.track2[2]  = vecUp.at(iup).param[2];
          tt.track2[3]  = vecUp.at(iup).param[3];
        }
      }//reco
      if (track1_was == 1) vec_twotracks.push_back(tt);
      if (fDebug) cout<<" vec_twotracks "<<vec_twotracks.size()<<endl;
      
      //for(int itr = 0; itr< vec_twotracks.size(); ++itr){
      if (vec_twotracks.size() == 1){
       Int_t itr = 0;
        if (
          (vec_twotracks.at(itr).track1_pdg == PDG_Be7  && vec_twotracks.at(itr).track2_pdg == PDG_p)   || (vec_twotracks.at(itr).track2_pdg == PDG_Be7  && vec_twotracks.at(itr).track1_pdg == PDG_p)  ||
          (vec_twotracks.at(itr).track1_pdg == PDG_Be7  && vec_twotracks.at(itr).track2_pdg == PDG_H2)  || (vec_twotracks.at(itr).track2_pdg == PDG_Be7  && vec_twotracks.at(itr).track1_pdg == PDG_H2) ||
          (vec_twotracks.at(itr).track1_pdg == PDG_Be7  && vec_twotracks.at(itr).track2_pdg == PDG_He4) || (vec_twotracks.at(itr).track2_pdg == PDG_Be7  && vec_twotracks.at(itr).track1_pdg == PDG_He4)||
          (vec_twotracks.at(itr).track1_pdg == PDG_Be7  && vec_twotracks.at(itr).track2_pdg == PDG_He3) || (vec_twotracks.at(itr).track2_pdg == PDG_Be7  && vec_twotracks.at(itr).track1_pdg == PDG_He3)||
          (vec_twotracks.at(itr).track1_pdg == PDG_Li7  && vec_twotracks.at(itr).track2_pdg == PDG_He4) || (vec_twotracks.at(itr).track2_pdg == PDG_Li7  && vec_twotracks.at(itr).track1_pdg == PDG_He4)||
          (vec_twotracks.at(itr).track1_pdg == PDG_Li7  && vec_twotracks.at(itr).track2_pdg == PDG_He3) || (vec_twotracks.at(itr).track2_pdg == PDG_Li7  && vec_twotracks.at(itr).track1_pdg == PDG_He3)||
          (vec_twotracks.at(itr).track1_pdg == PDG_Li6  && vec_twotracks.at(itr).track2_pdg == PDG_He4) || (vec_twotracks.at(itr).track2_pdg == PDG_Li6  && vec_twotracks.at(itr).track1_pdg == PDG_He4)||
          (vec_twotracks.at(itr).track1_pdg == PDG_Li6  && vec_twotracks.at(itr).track2_pdg == PDG_He3) || (vec_twotracks.at(itr).track2_pdg == PDG_Li6  && vec_twotracks.at(itr).track1_pdg == PDG_He3)||
          (vec_twotracks.at(itr).track1_pdg == PDG_He4  && vec_twotracks.at(itr).track2_pdg == PDG_He4)
        
        ) ifSelectedCases = 1;
        
        if(fDebug) cout<<" pdg1 "<<vec_twotracks.at(itr).track1_pdg <<" pdg2 "<<vec_twotracks.at(itr).track2_pdg<<" ifSelectedCases "<<ifSelectedCases<<endl;
        Double_t CosA = (vec_twotracks.at(itr).track1[0]*vec_twotracks.at(itr).track2[0] + vec_twotracks.at(itr).track1[2]*vec_twotracks.at(itr).track2[2] + 1)/
                        (sqrt((vec_twotracks.at(itr).track1[0]*vec_twotracks.at(itr).track1[0] + vec_twotracks.at(itr).track1[2]*vec_twotracks.at(itr).track1[2] + 1)*
                              (vec_twotracks.at(itr).track2[0]*vec_twotracks.at(itr).track2[0] + vec_twotracks.at(itr).track2[2]*vec_twotracks.at(itr).track2[2] + 1)));
        Double_t alpha    = TMath::ACos(CosA);
        Double_t alphaDeg = TMath::RadToDeg()* alpha;
        if(fDebug) cout<<" alpha "<<alpha<<" CosA "<<CosA<<" ToDeg "<<alphaDeg<<endl;
        hAngle_reco->Fill(alphaDeg);
        if (ifNmc2) hAngle_recoifNmc2->Fill(alphaDeg);
        if (ifNmc2 && ifSelectedCases) hAngle_recoifNmc2Cases->Fill(alphaDeg);
      }
      
    }//Ngood_mc_tracks == 2
    
  
}//MCefficiencyCalculation


//-----------------------------ReadSiliconTracks------------------------
void BmnUpstreamTracking::ReadSiliconTracks(Double_t** par_ab, Double_t*  par_z, Int_t & NTracks, vector<MC_points> & vec){
  MC_points tmpTr;
  if (!expData){
    if (fDebug) cout<<" MC_SiliconTracks:"<<endl;
    int tr_before  = -1;
    int Nmc_tracks = -1;
    
    int    Nst_mc[kMaxMC]; 
    int    Np_in_3st[kMaxMC];
    int    mcTracksArray[kMaxMC]; 
    double Xmc[kMaxMC][fNstations];
    double Ymc[kMaxMC][fNstations];
    double Zmc[kMaxMC][fNstations];
    for (Int_t  Id= 0; Id < kMaxMC; Id++) { 
      Nst_mc[Id] = 0;
      Np_in_3st[Id] = 0;
      mcTracksArray[Id] = -1;
      for (Int_t  i = 0; i < fNstations; i++) { 
        Xmc[Id][i] = -999.;
        Ymc[Id][i] = -999.;
        Zmc[Id][i] = -999.;
      }
    }
    
    for (Int_t iMC = 0; iMC < fSiTracksSim->GetEntriesFast(); ++iMC) {
      BmnSiliconHit* hit = (BmnSiliconHit*)fSiTracksSim->UncheckedAt(iMC);
      
      Double_t x_MC   = hit->GetX();
      Double_t y_MC   = hit->GetY();
      Double_t z_MC   = hit->GetZ();
      Int_t trackId_MC= hit->GetIndex();
      Int_t     st_mc = -1;
      
      if (tr_before != trackId_MC) {
        Nmc_tracks++;
        mcTracksArray[Nmc_tracks] = trackId_MC;
      }
      tr_before = trackId_MC;
      
      if (z_MC > -320.) {st_mc = 3; Np_in_3st[Nmc_tracks]++; }
      if (z_MC < -434. && z_MC > -436. ) st_mc = 2;
      if (z_MC < -438.) st_mc = 1;
      if (fDebug) cout<<" Id "<<trackId_MC<<" x "<<x_MC<<" y "<<y_MC<<" z "<<z_MC<<" st_mc "<<st_mc<<endl;
      
      Xmc[Nmc_tracks][st_mc] = x_MC;
      Ymc[Nmc_tracks][st_mc] = y_MC;
      Zmc[Nmc_tracks][st_mc] = z_MC;
      Nst_mc[Nmc_tracks]++;
      
    }//iMC
    Nmc_tracks++;
    
    for (Int_t  Id= 0; Id < kMaxMC; Id++) { 
      
      for (Int_t  i = 0; i < fNstations; i++) { 
        tmpTr.x_si[i]  = Xmc[Id][i];
        tmpTr.y_si[i]  = Ymc[Id][i];
        tmpTr.z_si[i]  = Zmc[Id][i];
      }

      tmpTr.Id    = mcTracksArray[Id];
      tmpTr.Np_Si = Nst_mc[Id];
      if (Np_in_3st[Id]  == tmpTr.Np_Si ) tmpTr.np_3si  = 1;
      if (fDebug && Np_in_3st[Id]  > 0 ) cout<<" Np_in_3st "<<Np_in_3st[Id] <<" tmpTr.Np_Si "<<tmpTr.Np_Si<<" tmpTr.np_3si "<<tmpTr.np_3si<<endl;
      if (Nst_mc[Id] >= 2 && Np_in_3st[Id]  > 0) vec.push_back(tmpTr);
    }
    
    if (fDebug) cout<<" vec_points.size() "<<vec.size()<<endl;
    
  }// !expData
  if ( fDebug ) cout<<endl;
  
  
  if (fDebug) cout<<"----Reco_SiliconTracks:"<<endl;
  for (Int_t iTrack = 0; iTrack < fSiTracks->GetEntriesFast(); iTrack++) {
    BmnTrack* track = (BmnTrack*) fSiTracks ->At(iTrack);
  
    FairTrackParam* par = track->GetParamLast();
    //FairTrackParam* par = track->GetParamFirst();
    
    if ( fDebug ) cout<<" SiTr "<<iTrack<<" SiTx "<<par->GetTx()<<" SiX "<<par->GetX()<<" SiTy "<<par->GetTy()<<" SiY "<<par->GetY()<<" SiZ "<<par->GetZ()<<" Nhits "<<track->GetNHits()<<endl;
    
    par_ab[0][NTracks]= par->GetTx();
    par_ab[1][NTracks]= par->GetX();
    par_ab[2][NTracks]= par->GetTy();
    par_ab[3][NTracks]= par->GetY();
    par_ab[4][NTracks]= track->GetNHits();
    par_z[NTracks]    = par->GetZ();
    NTracks++;
  }
  
  if ( fDebug ) cout<<endl;
  
}//ReadSiliconTracks
//----------------------------------------------------------------------

//----------------------------------------------------------------------
void BmnUpstreamTracking::ReadSiliconHits(Double_t*** hits, Int_t* NSihits){
  //-------------Read Silicon Hits------------------------------------
  if ( fDebug ) cout<<" Hits from Tracks "<<endl;
  for (Int_t iSihit = 0; iSihit < fSilHits->GetEntriesFast(); iSihit++) {
    BmnSiliconHit* hit = (BmnSiliconHit*) fSilHits ->At(iSihit);
    
    // if ( fDebug ) cout<<" iSihit "<<iSihit<<endl;
    Int_t    st   = hit->GetStation();
 
    if ( fDebug ) cout<<" st "<<st<<" hit: X "<<hit->GetX()<<" Y "<<hit->GetY()<<" Sigx "<<hit->GetDx()<<" Sigy "<<hit->GetDy()<<endl;
    hits[st][NSihits[st]][0] = hit->GetX();
    hits[st][NSihits[st]][1] = hit->GetY();
    hits[st][NSihits[st]][2] = hit->GetZ();
    hits[st][NSihits[st]][3] = hit->GetDx();
    hits[st][NSihits[st]][4] = hit->GetDy();
    hits[st][NSihits[st]][5] = hit->GetModule();
    
    NSihits[st]++;
  }
}//ReadSiliconHits
//----------------------------------------------------------------------

//----------------------------------------------------------------------
void BmnUpstreamTracking::ReadMWPCSegments(Double_t*** par_ab,  Double_t** par_z, Int_t*  Nseg,  vector<MC_points> & vec){
 for (Int_t iSeg = 0; iSeg < fMWPCSegments->GetEntries() ; ++iSeg) {
    BmnTrack* segment = (BmnTrack*) fMWPCSegments ->At(iSeg) ;
  
    Int_t    ich = -1;
    Double_t Z_mwpc = segment->GetParamFirst()->GetZ();
    Double_t Chi2   = segment->GetChi2();
    
    if ( Z_mwpc >= -859. && Z_mwpc <= -857.) ich = 0;
    if ( Z_mwpc >= -760. && Z_mwpc <= -758.) ich = 1;
    if ( Z_mwpc >= -365. && Z_mwpc <= -363.) ich = 2;
    if ( Z_mwpc >= -215. && Z_mwpc <= -213.) ich = 3;
    
    if ( ich < 0) continue;
    
    if ( fDebug ) cout<<" MWPCSeg: ich "<<ich<<" Tx "<<segment->GetParamFirst()->GetTx()<<" X "<<segment->GetParamFirst()->GetX()<<" Ty "<<segment->GetParamFirst()->GetTy()<<" Y "<<segment->GetParamFirst()->GetY()<< " Z "<<Z_mwpc<<" Nhits "<<segment->GetNHits()<<endl;
    par_ab[ich][0][Nseg[ich]]= segment->GetParamFirst()->GetTx();
    par_ab[ich][1][Nseg[ich]]= segment->GetParamFirst()->GetX();
    par_ab[ich][2][Nseg[ich]]= segment->GetParamFirst()->GetTy();
    par_ab[ich][3][Nseg[ich]]= segment->GetParamFirst()->GetY();
    par_ab[ich][4][Nseg[ich]]= segment->GetNHits();
    par_ab[ich][5][Nseg[ich]]= segment->GetParamFirst()->GetCovariance(0, 0);
    par_ab[ich][6][Nseg[ich]]= segment->GetParamFirst()->GetCovariance(1, 1);
    par_ab[ich][7][Nseg[ich]]= segment->GetParamFirst()->GetCovariance(2, 2);
    par_ab[ich][8][Nseg[ich]]= segment->GetParamFirst()->GetCovariance(3, 3);
    par_z[ich][Nseg[ich]]    = segment->GetParamFirst()->GetZ();
    
    if ( ich > 1 ){
      par_ab[ich][1][Nseg[ich]]+= -X_shift - X_shift_seg[ich];
      par_ab[ich][3][Nseg[ich]]+= -Y_shift;
    }
    Nseg[ich]++;
    
  }
  
  if (!expData){
    if (fDebug)cout<<"----MC: MWPCSegments"<<endl;
  
    Int_t mcTracksArray[kMaxMC];
    Int_t mcTracksPdg[kMaxMC];
    Double_t X2mc[kMaxMC][kNPlanes];
    Double_t Y2mc[kMaxMC][kNPlanes];
    Double_t Z2mc[kMaxMC][kNPlanes];
    Double_t X3mc[kMaxMC][kNPlanes];
    Double_t Y3mc[kMaxMC][kNPlanes];
    Double_t Z3mc[kMaxMC][kNPlanes];
    int Npl_MC2[kMaxMC]; int Npl_MC3[kMaxMC];
    
    for (Int_t  Id= 0; Id < kMaxMC; Id++) { 
      Npl_MC2[Id] = 0;
      Npl_MC3[Id] = 0;
      mcTracksArray[Id] = -1;
      mcTracksPdg[Id] = -1;
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
      Int_t    pdg_MC     = hit->GetPdgId();
      Int_t    pl_MC      = hit->GetPlaneId();
      Short_t  wire_MC    = hit->GetWireNumber();
      Double_t time_MC    = hit->GetWireTime();
        
        //if (fDebug)cout<<" st_MC "<<st_MC<<" trackId_MC "<<trackId_MC<<" pl_MC "<<pl_MC<<" X "<<hit->GetX()<<" wire_MC "<<wire_MC<<endl;
        
        if (tr_before != trackId_MC) {
          Nmc_tracks++;
          mcTracksArray[Nmc_tracks] = hit->GetHitId();
          mcTracksPdg[Nmc_tracks]   = hit->GetPdgId();
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
        //if (fDebug)cout<<" X2["<<Nmc_tracks<<"]["<<pl_MC<<"] "<<X2mc[Nmc_tracks][pl_MC]<<" Npl_MC2 "<< Npl_MC2[Nmc_tracks]<<" X3["<<Nmc_tracks<<"]["<<pl_MC<<"] "<<X3mc[Nmc_tracks][pl_MC]<<" Npl_MC3 "<< Npl_MC3[Nmc_tracks]<<" z3 "<<Z3mc[Nmc_tracks][pl_MC]<<endl;
      
    }//iMC
      Nmc_tracks++;
      
      
      if (fDebug)cout<<" Nmc_tracks "<<Nmc_tracks<<" MC vec_points.size() "<<vec.size()<<endl;
      MC_points tmpTr;
      bool mc_new = 0;
      for (Int_t  id = 0; id < Nmc_tracks; id++) { //mc_tr_curr
        if (fDebug)cout<<" id "<<id<<" Id_mc "<< mcTracksArray[id]<<" pdg "<<mcTracksPdg[id]<<" Npl2 "<<Npl_MC2[id]<<" Npl3 "<<Npl_MC3[id]<<endl;
         
        for (Int_t  itr = 0; itr < vec.size(); itr++) {// mc_tr_before
          
          if (Npl_MC2[id] >= 4 || Npl_MC3[id] >= 4){
            if (vec.at(itr).Id  == mcTracksArray[id]) {
              vec.at(itr).Pdg    = mcTracksPdg[id];
              vec.at(itr).Np_ch2 = Npl_MC2[id];
              vec.at(itr).Np_ch3 = Npl_MC3[id];
              vec.at(itr).Np_P   = Npl_MC2[id] + Npl_MC3[id];
              vec.at(itr).Np     = vec.at(itr).Np_P +  vec.at(itr).Np_Si;
              
              int i3 = -1;
              if (X3mc[id][5] > -900.)   i3 =5;
              if (i3 < 0 && X3mc[id][4] > -900.)  i3 =4;
              if (i3 < 0 && X3mc[id][3] > -900.)  i3 =3;

              int isi = -1;
              if (isi < 0 && vec.at(itr).x_si[1] > -900.)  isi = 1;
              if (isi < 0 && vec.at(itr).x_si[2] > -900.)  isi = 2;
                
                
              vec.at(itr).param[0] = (vec.at(itr).x_si[isi] - vec.at(itr).x_si[3])/(vec.at(itr).z_si[isi] - vec.at(itr).z_si[3]); 
              vec.at(itr).param[1] = vec.at(itr).param[0]*(-350 - vec.at(itr).z_si[3]) + vec.at(itr).x_si[3];
              vec.at(itr).param[2] = (vec.at(itr).y_si[isi] - vec.at(itr).y_si[3])/(vec.at(itr).z_si[isi] - vec.at(itr).z_si[3]); 
              vec.at(itr).param[3] = vec.at(itr).param[2]*(-350 - vec.at(itr).z_si[3]) + vec.at(itr).y_si[3];
              
              if (fDebug)cout<<" si mc: x3 "<<vec.at(itr).x_si[3]<<" z3 "<<vec.at(itr).z_si[3]<<" UpX "<<vec.at(itr).param[1]<<endl;
              
              if (X2mc[id][0] > -900. || X2mc[id][3] > -900.) vec.at(itr).xWas2 = 1;
              if (X2mc[id][1] > -900. || X2mc[id][4] > -900.) vec.at(itr).vWas2 = 1;
              if (X2mc[id][2] > -900. || X2mc[id][5] > -900.) vec.at(itr).uWas2 = 1;
              
              if (X3mc[id][0] > -900. || X3mc[id][3] > -900.) vec.at(itr).xWas3 = 1;
              if (X3mc[id][1] > -900. || X3mc[id][4] > -900.) vec.at(itr).vWas3 = 1;
              if (X3mc[id][2] > -900. || X3mc[id][5] > -900.) vec.at(itr).uWas3 = 1;
              
              
            }//vec_points.at(itr).Id  == mcTracksArray[id]
          }//Npl_MC2[id] >= 4 || Npl_MC3[id] >= 4
          
        }//vec_points.size
      }//Nmc_tracksId  = -1;
     
      if (fDebug){ 
        cout<<endl;
        cout<<"----MCtrue all systems, size: "<<vec.size()<<endl;
        for (Int_t  itr = 0; itr < vec.size(); itr++) { 
          cout<<" itr "<<itr<<" Id "<<vec.at(itr).Id<<" Np_Si "<<vec.at(itr).Np_Si<<" Np_P "<<vec.at(itr).Np_P<<" Np_ch2 "<<vec.at(itr).Np_ch2<<" Np_ch3 "<<vec.at(itr).Np_ch3<<endl;
          cout<<" itr "<<itr<<" Id "<<vec.at(itr).Id<<" Up: ax "<<vec.at(itr).param[0]<<" bx "<<vec.at(itr).param[1]<<
                                                                 " ay "<<vec.at(itr).param[2]<<" by "<<vec.at(itr).param[3]<<endl;
          
          cout<<" Xt "<<vec.at(itr).param[0]*( kZ_target - Zcentr ) + vec.at(itr).param[1]<<
                " Yt "<<vec.at(itr).param[2]*( kZ_target - Zcentr ) + vec.at(itr).param[3]<<endl;
          
          hAx_upmc->Fill(vec.at(itr).param[0]);
          hX_upmc ->Fill(vec.at(itr).param[1]);
          hAy_upmc->Fill(vec.at(itr).param[2]);
          hY_upmc ->Fill(vec.at(itr).param[3]);
        }
      }//if
    
  }// !expData
  if (fDebug)cout<<endl;
}//ReadMWPCSegments
//----------------------------------------------------------------------

void BmnUpstreamTracking::ReadMWPCTracks(Double_t*** par_ab, Double_t** par_z, Int_t* NTracks){
  for (Int_t iTr = 0; iTr < fMWPCTracks->GetEntriesFast() ; ++iTr) {
    BmnTrack* track =  (BmnTrack*) fMWPCTracks->At(iTr) ;
    
    Int_t ip = -1;
    Double_t Z = track->GetParamFirst()->GetZ();

    if ( Z >= -809. && Z <= -807.) ip = 0;
    if ( Z >= -290. && Z <= -288.) ip = 1;
    if ( ip < 0) continue;
    
    if ( fDebug ) cout<<" MWPCTr: ip "<<iTr<<" Tx "<<track->GetParamFirst()->GetTx()<<" X "<<track->GetParamFirst()->GetX()<<" Ty "<<track->GetParamFirst()->GetTy()<<" Y "<<track->GetParamFirst()->GetY()<<" Z "<<Z<<endl;
    
    par_ab[ip][0][NTracks[ip]] = track->GetParamFirst()->GetTx() ;
    par_ab[ip][1][NTracks[ip]] = track->GetParamFirst()->GetX() ;
    par_ab[ip][2][NTracks[ip]] = track->GetParamFirst()->GetTy() ; 
    par_ab[ip][3][NTracks[ip]] = track->GetParamFirst()->GetY() ;
    par_ab[ip][4][NTracks[ip]] = track->GetNHits();
    par_ab[ip][5][NTracks[ip]] = track->GetParamFirst()->GetCovariance(0, 0);
    par_ab[ip][6][NTracks[ip]] = track->GetParamFirst()->GetCovariance(1, 1);
    par_ab[ip][7][NTracks[ip]] = track->GetParamFirst()->GetCovariance(2, 2);
    par_ab[ip][8][NTracks[ip]] = track->GetParamFirst()->GetCovariance(3, 3);
    par_z[ip][NTracks[ip]]     = track->GetParamFirst()->GetZ();
    
    par_ab[1][1][NTracks[ip]]+= -X_shift;
    par_ab[1][3][NTracks[ip]]+= -Y_shift;
    /*
    hAxPCtr->Fill(par_ab[1][0][NTracks[ip]]);
    hxPCtr->Fill(par_ab[1][1][NTracks[ip]]);
    hAyPCtr->Fill(par_ab[1][2][NTracks[ip]]);
    hyPCtr->Fill(par_ab[1][3][NTracks[ip]]);
    hbeam_profile_pair_[ip]->Fill(track->GetParamFirst()->GetX(),track->GetParamFirst()->GetY());
    */
    NTracks[ip]++;
    
  }//itr 
  
}//ReadMWPCTracks
//----------------------------------------------------------------------


//----------------------------------------------------------------------
void BmnUpstreamTracking::PCTracksSiTracksMatching(Double_t** par_Si, Double_t* par_Siz, Int_t & NTracksSi,
Double_t*** par_PC, Double_t** par_PCz, Int_t* NTracksPC, vector<Smatch> & vec_m, vector<Smatch> & Out){
  
  Smatch tmpSeg;
  vec_m.clear();
  
  if (fDebug) cout<<" NTracksSi "<<NTracksSi<<" NPCTracks[1] "<<NPCTracks[1]<<endl;
  
  for (Int_t  iSi=0; iSi < NTracksSi; ++iSi){
    for (Int_t iPCtr = 0; iPCtr < NPCTracks[1]; ++iPCtr){
      
      Double_t dz = par_PCz[1][iPCtr] - par_Siz[iSi];
    
      Double_t dAx = par_Si[0][iSi]  - par_PC[1][0][iPCtr] ;
      Double_t dAy = par_Si[2][iSi]  - par_PC[1][2][iPCtr] ;
      Double_t dX  = (par_Si[0][iSi]* dz + par_Si[1][iSi]) - par_PC[1][1][iPCtr];
      Double_t dY  = (par_Si[2][iSi]* dz + par_Si[3][iSi]) - par_PC[1][3][iPCtr];
     /*
      hdX_MWPCpair1_Sitr->Fill(dX);
      hdAx_MWPCpair1_Sitr->Fill(dAx);
      hdY_MWPCpair1_Sitr->Fill(dY);
      hdAy_MWPCpair1_Sitr->Fill(dAy);
     */
      if (fDebug) cout<<" Si("<<iSi<<")-P1("<<iPCtr<<"): dAx("<<cutAx<<") "<<dAx<<" dAy("<<cutAy<<") "<<dAy<<" dX("<<cutX<<") "<<dX<<" dY("<<cutY<<") "<<dY<<endl;
      if (fabs(dAx) < cutAx && fabs(dAy) < cutAy && fabs(dX) < cutX && fabs(dY) < cutY ){
        if (fDebug) cout<<"after cuts: Si "<<iSi<<"- P1 "<<iPCtr<<endl;
        Double_t Chi2_match = dX*dX + 0.25*dY*dY; 

        tmpSeg.Ind1   = iSi;
        tmpSeg.Ind2   = iPCtr;
        tmpSeg.Inds2  = -1;
        tmpSeg.Inds3  = -1;
        tmpSeg.Nhits1 = par_Si[4][iSi];
        tmpSeg.Nhits2 = par_PC[1][4][iPCtr];
        tmpSeg.Z1     = par_Siz[iSi];
        tmpSeg.Z2     = par_PCz[1][iPCtr];
        tmpSeg.distX  = dX;
        tmpSeg.distY  = dY;
        tmpSeg.Chi2m  = Chi2_match;
       // if (fDebug) cout<<" iSi "<<iSi<<" iPCtr "<<iPCtr<<" dX "<<dx<<" dY "<<dy<<" Chi2_match "<<Chi2_match<<endl;

        for(int ipar = 0; ipar < 4; ipar++){
          tmpSeg.param1[ipar] = par_Si[ipar][iSi];
          tmpSeg.param2[ipar] = par_PC[1][ipar][iPCtr];
        }
        vec_m.push_back(tmpSeg);
        
       /*
       hdX_MWPCpair1_Sitr_cut->Fill(dX);
       hdAx_MWPCpair1_Sitr_cut->Fill(dAx);
       hdY_MWPCpair1_Sitr_cut->Fill(dY);
       hdAy_MWPCpair1_Sitr_cut->Fill(dAy);
       */
      }//if
    }//iPCtr
  }//iSi
  if (fDebug) cout<<" --End PCtracks: size "<<vtmpSeg.size()<<endl;
  
  if (vtmpSeg.size() > 0){
    Int_t PCtrack = 1;
    RecordingBest(PCtrack, vec_m, Out);
  }
  
  if (fDebug) cout<<"PCTracksSiTracksMatching: OutVector.size() "<<Out.size()<<endl;
  for (int iter = 0; iter < Out.size(); ++iter) {
    if (fDebug) printf(" OutVector.at(%d): %8.4f | Si %d - PC %d Seg2 %d - Seg3 %d\n", iter, Out.at(iter).Chi2m, Out.at(iter).Ind1, Out.at(iter).Ind2, Out.at(iter).Inds2, Out.at(iter).Inds3 );
  }
}//PCTracksSiTracksMatching
//----------------------------------------------------------------------

//----------------------------------------------------------------------
void BmnUpstreamTracking::PCSegmentsSiTracksMatching(Double_t** par_Si, Double_t* par_Siz, Int_t & NTracksSi,
Double_t*** par_PC, Double_t** par_PCz, Int_t* NSegPC, vector<Smatch> & vec_m, vector<Smatch> & Out){
  
  vec_m.clear();
  if ( fDebug ) cout<<endl;
  if ( fDebug ) cout<<" ----Matching: PC segments Ch2 ---"<<endl;
  if ( fDebug ) cout<<" NTracksSi "<<NTracksSi<<" NSegPC[2] "<<NSegPC[2]<<endl;
  for (int iSi=0; iSi < NTracksSi; ++iSi){
    for(int iseg2 = 0; iseg2 < NSegPC[2]; ++iseg2){
      
      Double_t dz2 = par_PCz[2][iseg2] - par_Siz[iSi];
     // if ( fDebug ) cout<<" Zpc "<<par_PCz[2][iseg2]<<" Zsi "<<par_Siz[iSi]<<endl;
      Double_t X_extr2 = par_Si[0][iSi]* dz2 + par_Si[1][iSi];
      Double_t Y_extr2 = par_Si[2][iSi]* dz2 + par_Si[3][iSi];
      
      Double_t dX = X_extr2 - par_PC[2][1][iseg2];
      Double_t dY = Y_extr2 - par_PC[2][3][iseg2];

      //hdX_MWPCSeg2_Sitr->Fill(dX);
      //hdY_MWPCSeg2_Sitr->Fill(dY);
      
      if (fDebug) cout<<" Si["<<iSi<<"]-Seg2["<<iseg2<<"] : dX("<<cutX<<") "<<dX<<" dY("<<cutY<<") "<<dY<<endl;
      if (fabs(dX) < cutX && fabs(dY) < cutY ){
        
        Int_t iseg3 = -1;
         if (fDebug) cout<<" cuts ok:iseg2 "<<iseg2<<endl;
        RecordCandidateSeg(iSi, iseg2, iseg3, par_Si, par_PC, par_Siz, par_PCz, dX, dY, vec_m);
        
      }//if
    }//iseg2
  }//iSi
  
  if (fDebug) cout<<" --End PCiseg2 : now vec_m.size "<<vec_m.size()<<endl;
  if (fDebug) cout<<endl;
  //-----------------------------------------------------------------
  if ( fDebug ) cout<<endl;
  if ( fDebug ) cout<<" ----Matching: PC segments Ch3 ---"<<endl;
  if ( fDebug ) cout<<" NTracksSi "<<NTracksSi<<" NSegPC[3] "<<NSegPC[3]<<endl;
  for (int iSi=0; iSi < NSiTracks; ++iSi){
    for(int iseg3 = 0; iseg3 < NSegPC[3]; ++iseg3){
      
      Double_t dz3     = par_PCz[3][iseg3] - par_Siz[iSi];
      Double_t X_extr3 = par_Si[0][iSi]* dz3 + par_Si[1][iSi];
      Double_t Y_extr3 = par_Si[2][iSi]* dz3 + par_Si[3][iSi];
      
      Double_t dX = X_extr3 - par_PC[3][1][iseg3];
      Double_t dY = Y_extr3 - par_PC[3][3][iseg3];
      //hdX_MWPCSeg3_Sitr->Fill(dX);
      //hdY_MWPCSeg3_Sitr->Fill(dY);
      
      if (fDebug) cout<<" Si["<<iSi<<"]-Seg3["<<iseg3<<"] : dX("<<cutX<<") "<<dX<<" dY("<<cutY<<") "<<dY<<endl;
      if (fabs(dX) < cutX && fabs(dY) < cutY ){
        
        Int_t iseg2 = -1;
        RecordCandidateSeg(iSi, iseg2, iseg3, par_Si, par_PC, par_Siz, par_PCz, dX, dY, vec_m);
       
      }//if
    }//iseg3
  }//iSi

  if (fDebug) cout<<" --End PCiseg3 : now vec_m.size "<<vec_m.size()<<endl;

  if (vec_m.size() > 0){
    Int_t PCtrack = -1;
    RecordingBest(PCtrack, vec_m, Out);
  }
  if (fDebug) cout<<" vec_m.size() "<<vec_m.size()<<" Out.size() "<<Out.size()<<endl;
  if (fDebug) cout<<endl;
}//PCSegmentsSiTracksMatching
//----------------------------------------------------------------------

//----------------------------------------------------------------------
void BmnUpstreamTracking::RecordCandidateSeg(Int_t & iSi, Int_t & is2, Int_t & is3,
Double_t ** par_Si, Double_t*** par_PC, 
Double_t* par_Siz, Double_t** ZPC, 
Double_t & dx, Double_t & dy, vector<Smatch>& vtmp){

  if ( fDebug ) cout<<"   RecordCandidateSeg: Inds: seg2 "<<is2<<" seg3 "<<is3<<endl;
  Smatch tmpSeg;
  Int_t iseg = is2;
  Int_t ich  = 2;
  if ( is2 < 0 ) { 
    iseg = is3; 
    ich = 3;
  }
  
  Double_t Chi2_match = dx*dx + 0.25*dy*dy; 
  
  tmpSeg.Ind1  = iSi;
  tmpSeg.Ind2  = -1;
  tmpSeg.Inds2 = is2;
  tmpSeg.Inds3 = is3;
  tmpSeg.Z1    = par_Siz[iSi];
  tmpSeg.Zs2   = ZPC[ich][iseg];
  tmpSeg.distX = dx;
  tmpSeg.distY = dy;
  tmpSeg.Chi2m = Chi2_match;
  if ( fDebug ) cout<<" iSi "<<iSi<<" iseg2 "<<is2<<" iseg3 "<<is3<<" dX "<<dx<<" dY "<<dy<<" Chi2_match "<<Chi2_match<<endl;

  for(int ipar = 0; ipar < 4; ipar++) {
    tmpSeg.param1[ipar]  = par_Si[ipar][iSi];
    tmpSeg.params2[ipar] = par_PC[ich][ipar][iseg]; 
  }
  vtmp.push_back(tmpSeg);
}
//----------------------------------------------------------------------


//----------------------------------------------------------------------
void BmnUpstreamTracking::RecordingBest(Int_t & iPCtrack, vector<Smatch> & vec, vector<Smatch> & OutVec){
  if ( iPCtrack > -1 ){
    if (fDebug) cout<<endl;
    if (fDebug) cout<<"--RecordingBest: tracks --"<<endl;
    // vector sorting
    sort(vec.begin(), vec.end(), compareSegments);

    //first best
    OutVec.push_back(vec.at(0));
    if (fDebug) cout<<"--isMatch: Chi2 "<<vec.at(0).Chi2m<<endl;

    //reject repeat index
    Bool_t isMatch;
    for (int iter = 0; iter < vec.size(); ++iter) {
      if (fDebug) printf(" vtmpSeg.at(%d): %8.4f | %d - %d\n", iter, vec.at(iter).Chi2m, vec.at(iter).Ind1, vec.at(iter).Ind2 );
      isMatch = 0;
      for(int InIter = 0; InIter < OutVec.size(); ++InIter) {
        if(vec.at(iter).Ind1 == OutVec.at(InIter).Ind1 || vec.at(iter).Ind2 == OutVec.at(InIter).Ind2) {
          isMatch = 1;
          continue;
        }
      }
      //writing unique index
      if (isMatch == 0) {
        if (fDebug) cout<<"--isMatch: Chi2 "<<vec.at(iter).Chi2m<<endl;
        OutVec.push_back(vec.at(iter));
      }
    }//iter
    
    if (fDebug) cout<<" Now OutVec.size "<<OutVec.size()<<endl;
    vec.clear();
  }else{//---segs--
    if (fDebug) cout<<"--RecordingBest: segs --"<<endl;
    // vector sorting
    sort(vec.begin(), vec.end(), compareSegments);

    //reject repeat index
    Bool_t isMatch;
    for (int iter = 0; iter < vec.size(); ++iter) {
      if (fDebug) printf(" vec.at(%d): %8.4f | Si %d - Seg2 %d - Seg3 %d\n", iter, vec.at(iter).Chi2m, vec.at(iter).Ind1, vec.at(iter).Inds2, vec.at(iter).Inds3 );
      isMatch = 0;
      for(int InIter = 0; InIter < OutVec.size(); ++InIter) {
        if ( vec.at(iter).Ind1 == OutVec.at(InIter).Ind1 && OutVec.at(InIter).Ind2 > -1 ) {
          isMatch = 1;
          continue;
        }
        if((vec.at(iter).Ind1 == OutVec.at(InIter).Ind1 && OutVec.at(InIter).Inds3 > -1) || (vec.at(iter).Ind1 == OutVec.at(InIter).Ind1 && vec.at(iter).Inds2 == OutVec.at(InIter).Inds2)  ){
          isMatch = 1;
          continue;
        }
        if((vec.at(iter).Ind1 == OutVec.at(InIter).Ind1 && OutVec.at(InIter).Inds2 > -1) || (vec.at(iter).Ind1 == OutVec.at(InIter).Ind1 && vec.at(iter).Inds3 == OutVec.at(InIter).Inds3)  ){
          isMatch = 1;
          continue;
        }
        if( (OutVec.at(InIter).Inds2 > -1 && vec.at(iter).Inds2 == OutVec.at(InIter).Inds2 ) || ( OutVec.at(InIter).Inds3 > -1 && vec.at(iter).Inds3 == OutVec.at(InIter).Inds3) ){
          isMatch = 1;
          continue;
        }
      }
      if (fDebug) cout<<" writing unique index "<<endl;
      if (isMatch == 0) {
        if (fDebug) cout<<"--isMatch: Chi2 "<<vec.at(iter).Chi2m<<" Inds2 "<<vec.at(iter).Inds2<<" Inds3 "<<vec.at(iter).Inds3<<endl;
        OutVec.push_back(vec.at(iter));
      }
    }//iter
    
    if (fDebug) cout<<" RecordingBest: Now OutVec.size "<<OutVec.size()<<endl;
    for (int iter = 0; iter < OutVec.size(); ++iter) {
      if (fDebug) cout<<" iter "<<iter<<" index: Si:"<<OutVec.at(iter).Ind1<<" PCtr: "<< OutVec.at(iter).Ind2<<" PCseg2: "<<OutVec.at(iter).Inds2<<" PCseg3: "<<OutVec.at(iter).Inds3<<endl;
    }
    
    vec.clear();
  }
  
}//RecordingBest
//----------------------------------------------------------------------


//----------------------------------------------------------------------
void BmnUpstreamTracking::GetAddSiHits(vector<Smatch> & Out, Double_t*** hits, Int_t* Nh){
Bool_t was_ind;
Double_t tmp_array[kNumStSi][6];
if (fDebug) cout<<"--GetAddSiHits--"<<endl;

  for (int iter = 0; iter < Out.size(); ++iter) {
   if (fDebug) printf(" Out.at(%d): %8.4f | Si %d - PC %d Seg2 %d - Seg3 %d\n", iter, Out.at(iter).Chi2m, Out.at(iter).Ind1, Out.at(iter).Ind2, Out.at(iter).Inds2, Out.at(iter).Inds3 );
   
    was_ind = 0;
    for (int st=1; st < kNumStSi; ++st){ 
      for (int i=0; i < 5; ++i){
        tmp_array[st][i] = -999.;
      }
        for (int ihit=0; ihit < Nh[st]; ++ihit){
          if ( Out.at(iter).Ind1 == ihit ){
            was_ind = 1;
            tmp_array[st][0] =  hits[st][ihit][0];
            tmp_array[st][1] =  hits[st][ihit][1];
            tmp_array[st][2] =  hits[st][ihit][2];
            tmp_array[st][3] =  hits[st][ihit][3];
            tmp_array[st][4] =  hits[st][ihit][4];
            tmp_array[st][5] =  hits[st][ihit][5];
          }
        }
    }//st
    if (was_ind){
      for(int st = 1; st < kNumStSi; ++st){
        Out.at(iter).SiCoordX[st] = tmp_array[st][0];
        Out.at(iter).SiCoordY[st] = tmp_array[st][1];
        Out.at(iter).SiCoordZ[st] = tmp_array[st][2];
        Out.at(iter).SiSigmaX[st] = tmp_array[st][3];
        Out.at(iter).SiSigmaY[st] = tmp_array[st][4];
        Out.at(iter).SiMod[st]    = tmp_array[st][5];
        if (fDebug) cout<<" st "<<st<<" X "<<tmp_array[st][0]<<" Y "<<tmp_array[st][1]<<" Z "<<tmp_array[st][2]<<" mod "<<tmp_array[st][5]<<endl;
      }
    }
  }//iter
  
}//GetAddSiHits
//----------------------------------------------------------------------


//----------------------------------------------------------------------
void BmnUpstreamTracking::GetHitsToPoints(vector<Smatch> & Out,Double_t*** ab_tr, Int_t* NPCTracks_, Double_t*** ab_Ch, 
Int_t* Nseg_ch_, Double_t*** Points_, Int_t & NumPoints_){
  if (fDebug) cout<<" ---GetHitsToPoints---"<<endl;
  for(int iter = 0; iter < Out.size(); iter++) {
      if ( fDebug ) cout<<" silicon points "<<endl;
      for (int ist=1; ist < kNumStSi; ++ist){
        if (ist < 3){
          Points_[iter][ist-1][0] = Out.at(iter).SiCoordX[ist];//x
          Points_[iter][ist-1][1] = Out.at(iter).SiCoordY[ist];//y
          Points_[iter][ist-1][2] = Out.at(iter).SiCoordZ[ist];//z
          Points_[iter][ist-1][3] = Out.at(iter).SiSigmaX[ist];//sigx
          Points_[iter][ist-1][4] = Out.at(iter).SiSigmaY[ist];//sigy
         if ( fDebug ) cout<<" X["<<ist<<"] "<<Points_[iter][ist-1][0] <<" Y["<<ist<<"] "<<Points_[ist-1][ist][1] <<endl;
        }else{
          Points_[iter][ist][0] = Out.at(iter).SiCoordX[ist];//x
          Points_[iter][ist][1] = Out.at(iter).SiCoordY[ist];//y
          Points_[iter][ist][2] = Out.at(iter).SiCoordZ[ist];//z
          Points_[iter][ist][3] = Out.at(iter).SiSigmaX[ist];//sigx
          Points_[iter][ist][4] = Out.at(iter).SiSigmaY[ist];//sigy
         if ( fDebug ) cout<<" X["<<ist<<"] "<<Points_[iter][ist][0] <<" Y["<<ist<<"] "<<Points_[iter][ist][1] <<endl;
        }
      }
    for (int ipctr=0; ipctr < NPCTracks_[1]; ++ipctr){
      // if was MWPC-track
      if ( Out.at(iter).Ind2 == ipctr){
        // x = kx*(dz) + bx  //Z_pair[kNumPairs] = {-808.816, -289.401}; //Z_Chamber[kNumChambers] = {-858., -761., -364., -214.};
        Double_t deltaAx = ab_tr[1][7][ipctr];
        Double_t deltaAy = ab_tr[1][8][ipctr];
        
        Points_[iter][2][0]  = Out.at(iter).param2[0]*(Z_Chamber[2]-Z_pair[1]) + Out.at(iter).param2[1];//x3
        Points_[iter][4][0]  = Out.at(iter).param2[0]*(Z_Chamber[3]-Z_pair[1]) + Out.at(iter).param2[1];//x4 //(Z_Chamber[3]-Z_pair[1])
        
        Points_[iter][2][1]  = Out.at(iter).param2[2]*(Z_Chamber[2]-Z_pair[1]) + Out.at(iter).param2[3];//y3
        Points_[iter][4][1]  = Out.at(iter).param2[2]*(Z_Chamber[3]-Z_pair[1]) + Out.at(iter).param2[3];//y4 //Z_pair[1] - Z_Chamber[3]
        //                     Ax*deltaX                           +  X*deltaAx                         + deltaX
        Points_[iter][2][3]  = Out.at(iter).param2[0]*SigmXmwpc + Out.at(iter).param2[1]*deltaAx + SigmXmwpc;//sigx
        Points_[iter][2][4]  = Out.at(iter).param2[2]*SigmYmwpc + Out.at(iter).param2[3]*deltaAy + SigmYmwpc;//sigy
        Points_[iter][4][3]  = SigmXmwpc;//sigx// Points_[iter][3][3];
        Points_[iter][4][4]  = SigmYmwpc;//sigy// Points_[iter][3][4];
        if ( fDebug ){
          cout<<" MWPCs from track"<<endl;
          cout<<" X3  "<<Points_[iter][2][0] <<" X4  "<<Points_[iter][4][0]  <<endl;//" sigX "<<Points_[iter][3][3]<<endl;
          cout<<" Y3  "<<Points_[iter][2][1] <<" X4  "<<Points_[iter][4][1]  <<endl;//" sigX "<<Points_[iter][3][3]<<endl;
          //" sigXcov "<<ab_tr[1][5][ipctr]<<endl;
        }
      }
    }//NPCTracks_[1]
      //--if was segment--
      for (int iseg2=0; iseg2 < Nseg_ch_[2]; ++iseg2){
        if ( Out.at(iter).Inds2 == iseg2){
          Points_[iter][2][0]  = ab_Ch[2][1][iseg2];//x
          Points_[iter][2][1]  = ab_Ch[2][3][iseg2];//y
          Points_[iter][2][3]  = SigmXmwpc;//sigx
          Points_[iter][2][4]  = SigmYmwpc;//sigy
          if ( fDebug ){
            cout<<" MWPCs from segment"<<endl;
            cout<<" X3  "<<Points_[iter][2][0] <<" Y3  "<<Points_[iter][2][1]<<endl;//" sigX "<<Points_[iter][3][3]<<endl;
            //" sigXcov "<<ab_tr[1][5][ipctr]<<endl;
          }
        }
      }//Nseg_ch_[2]
      for (int iseg3=0; iseg3 < Nseg_ch_[3]; ++iseg3){
        if ( Out.at(iter).Inds3 == iseg3){
          Points_[iter][4][0]  = ab_Ch[3][1][iseg3];//x
          Points_[iter][4][1]  = ab_Ch[3][3][iseg3];//y
          Points_[iter][4][3]  = SigmXmwpc;//sigx
          Points_[iter][4][4]  = SigmYmwpc;//sigy
          if ( fDebug ){
            cout<<" MWPCs from segment"<<endl;
            cout<<" X4  "<<Points_[iter][4][0] <<" Y4  "<<Points_[iter][4][1]<<endl;
          }
        }
      }//Nseg_ch_[2]
      

      Points_[iter][2][2]  = Z_Chamber[2];
      Points_[iter][4][2]  = Z_Chamber[3];
      NumPoints_ = iter+1;
  }//Out.size()
  //????????
}
//----------------------------------------------------------------------


//----------------------------------------------------------------------
void BmnUpstreamTracking::CalculationOfChi2(Double_t*** Points_, Int_t & NumPoints_, vector<UpTracks> & vec ){
  if (fDebug) cout<<" Check points before fit "<<" Zcentr "<<Zcentr<<endl;
  Int_t Nhits_before;
  for (int ip =0; ip < NumPoints_; ++ip){
    Nhits_before = 0;
    for (int ist =0; ist < kPoints; ++ist){
      Points_[ip][ist][0] += X_shiftUp[ist];
      Points_[ip][ist][1] += Y_shiftUp[ist];
      Points_[ip][ist][2] -= Zcentr;
     // if (fDebug) cout<<"ip "<<ip<<" X["<<ist<<"] "<< Points_[ip][ist][0]<<" Y "<<Points_[ip][ist][1]<<" Z "<<Points_[ip][ist][2]<<" sigx "<<Points_[ip][ist][3]<<" sigy "<<Points_[ip][ist][4]<<endl;
      if (Points_[ip][ist][0] > -900.) Nhits_before++; 
      if (Points_[ip][ist][1] > -900.) Nhits_before++; 
    }
  }
  if (fDebug) cout<<" Nhits_before "<<Nhits_before<<endl;
  UpTracks tmptr;

  for (Int_t i = 0; i < kPoints; ++i) {
    for (Int_t j = 0; j < 5; ++j) {
      Xforglfit[i][j] = -999.;
    }
  }
  Double_t ChiSquare_ ;
  for (Int_t itr = 0; itr < NumPoints_; ++itr) {
    
    if (Nhits_before < 6 ) continue;
    
    ChiSquare_         = 0.;
    if(fDebug) cout<<endl;
    if(fDebug) cout<<"--GlobalFit--"<<endl;
    //setting global fit matrices to zero
 
    for (Int_t i = 0; i < 4; ++i){    
     for (Int_t j = 0; j < 4; ++j){      
       Amatr[i][j] = 0.; }}  
    for (Int_t i = 0; i < 4; ++i) { rhs[i] = 0; }
    for (Int_t i = 0; i < 16; ++i) { AmatrArray[i] = 0; }
    for (Int_t i = 0; i < 16; ++i) { AmatrInverted[i] = 0; }
    
    for (Int_t i = 0; i < kPoints; ++i) {
      for (Int_t j = 0; j < kPoin_Par; ++j) {
       Xforglfit[i][j] = Points_[itr][i][j];
       //if ( fDebug && j == 0) cout<<" x ["<<i<<"]["<<j<<"] "<<Xforglfit[i][j]<<endl;
      }
      for (Int_t j = 0; j < kPoin_Par; ++j) {
       // if ( fDebug && j == 1) cout<<" y ["<<i<<"]["<<j<<"] "<<Xforglfit[i][j]<<endl;
      }
    }
    
    GlobalFit(Xforglfit,Amatr, rhs);

    //here we do smth with Amatr & rhs (solving)  
    // Amatr is row-major  
    for (Int_t row = 0; row < 4; ++row) {
      for (Int_t col = 0; col < 4; ++col) {
        AmatrArray[4*col + row] = Amatr[row][col];
      }
    }//AmatrArray is col-major

    bool ifSuccess = InvertMatrix(AmatrArray, AmatrInverted);
    
    if (!ifSuccess) {cout<< "???? InvertMatrix is unsuccessfull ????"<<endl;   }  
    if (!ifSuccess) {cout<< "???? InvertMatrix is unsuccessfull ????"<<endl; return;} 
    //line_ = { ax, bx, ay, by}
    Double_t line_[4] = {0.,0.,0.,0.};
    for (Int_t coeff = 0; coeff < 4; ++coeff) {
      for (Int_t iCol = 0; iCol < 4; ++iCol) {
       // if ( fDebug ) cout<<" AmatrInverted["<<4*iCol + coeff<<"] "<<AmatrInverted[4*iCol + coeff]<<" rhs["<<iCol<<"] "<<rhs[iCol]<<endl;
       line_[coeff] += AmatrInverted[4*iCol + coeff]*rhs[iCol]; 
      }
    }
     
    if ( fDebug ) cout<<" ax "<<line_[0]<<" bx "<<line_[1]<<" ay "<<line_[2]<<" by "<<line_[3]<<endl;
    if ( fDebug ) cout<<endl;
    
    //---------------------------Chi2---------------------------------
    Int_t nhits = 0;
    for (Int_t ist = 0; ist < kPoints; ++ist) {
      if (Xforglfit[ist][0] < -900. ) continue;//x
      
      Double_t hit = Xforglfit[ist][0] ;
      Double_t fit = line_[0]*Xforglfit[ist][2] + line_[1];
      nhits++;
      
      Double_t sigmaX_2 = Xforglfit[ist][3]*Xforglfit[ist][3];
      ChiSquare_ += ((hit-fit)*(hit-fit))/(sigmaX_2);
      //if ( fDebug ) 
     // if ((ist == 2 || ist == 4) && fabs(hit-fit) > 0.2 ) cout<<" fEventNo "<<fEventNo<<" PC ist "<<ist<<" hitx  "<<hit<<" fit "<<fit<<" dx "<<hit-fit<<" Z "<<Xforglfit[ist][2]<<" sigma "<<Xforglfit[ist][3]<<" ChiSquare_ "<<ChiSquare_<<endl;
     // if ((ist == 0 || ist == 1 || ist == 3) && fabs(hit-fit) > 0.02 ) cout<<" fEventNo "<<fEventNo<<" Si ist "<<ist<<" hitx  "<<hit<<" fit "<<fit<<" dx "<<hit-fit<<" Z "<<Xforglfit[ist][2]<<" sigma "<<Xforglfit[ist][3]<<" ChiSquare_ "<<ChiSquare_<<endl;
      if ( fDebug ) cout<<" ist "<<ist<<" hitx  "<<hit<<" fit "<<fit<<" dx "<<hit-fit<<" Z "<<Xforglfit[ist][2]<<" sigma "<<Xforglfit[ist][3]<<" ChiSquare_ "<<ChiSquare_<<endl;
      
    }
    for (Int_t ist = 0; ist < kPoints; ++ist) { 
      if (Xforglfit[ist][1] < -900. ) continue;//y
      
      Double_t hit = Xforglfit[ist][1];
      Double_t fit = line_[2]*Xforglfit[ist][2] + line_[3];
      nhits++;
      
      Double_t sigmaY_2 = Xforglfit[ist][4]*Xforglfit[ist][4];
      ChiSquare_ += ((hit-fit)*(hit-fit))/(sigmaY_2);
      //if ( fDebug) 
      //if (fabs(hit-fit) > 0.2 ) 
      if ( fDebug) cout<<" ist "<<ist<<" hity "<<hit<<" fit "<<fit<<" dy "<<hit-fit<<" Z "<<Xforglfit[ist][2]<<" sigma "<<Xforglfit[ist][4]<<" ChiSquare_ "<<ChiSquare_<<endl;
    }//ist
    
    Double_t ChiSquareNdf = ChiSquare_/(nhits - 4);

    if (ChiSquareNdf < kChi2_Max && nhits > 5){
      
      tmptr.Chi2 = ChiSquareNdf;
      
      if ( fDebug) cout<<" Chi2 "<<tmptr.Chi2<<" nhits "<<nhits<<endl;
      if ( fDebug) cout<<" ------"<<endl;
      
      for (Int_t iCol = 0; iCol < 4; ++iCol) {
        tmptr.param[iCol] = line_[iCol];
      }
     // if ( fDebug) cout<<" Ax "<<tmptr.param[0]<<" bx "<<tmptr.param[1]<<" Ay "<<tmptr.param[2]<<" by "<<tmptr.param[3]<<endl;
     // if ( fDebug) cout<<" X coor "<<endl;
      for (Int_t ist = 0; ist < kPoints; ++ist) {
        tmptr.CoordZ[ist] = Xforglfit[ist][2];
        tmptr.CoordX[ist] = Xforglfit[ist][0];
        if (tmptr.CoordX[ist]< -900. ) continue;
       // if ( fDebug) cout<<" ist "<<ist<<" hitx  "<<tmptr.CoordX[ist]<<endl;
      }
      //if ( fDebug) cout<<" Y coor "<<endl;
      for (Int_t ist = 0; ist < kPoints; ++ist) {
        tmptr.CoordY[ist] = Xforglfit[ist][1];
        if (tmptr.CoordY[ist]< -900. ) continue;
       // if ( fDebug) cout<<" ist "<<ist<<" hity  "<<tmptr.CoordY[ist]<<endl;
      }
      tmptr.Nhits = nhits;
      vec.push_back(tmptr);
    }
  }//itr
  
  
}//calculationOfChi2
//----------------------------------------------------------------------

//----------------------------------------------------------------------
void BmnUpstreamTracking::PrintAllTracks(vector<UpTracks> & vecUp){
  
  cout<<" ------PrintAllTracks: "<<vecUp.size()<<endl;
  for (int itr =0; itr < vecUp.size(); ++itr){
    cout<<endl;
    cout<<" itr "<<itr<<" Chi2 "<<vecUp.at(itr).Chi2<<" Ax "<<vecUp.at(itr).param[0]<<" bx "<<vecUp.at(itr).param[1]<<" Ay "<<vecUp.at(itr).param[2]<<" by "<<vecUp.at(itr).param[3]<<" Nhits "<<vecUp.at(itr).Nhits<<endl;

    cout<<"--X--"<<endl;
    for (Int_t st = 0; st < kPoints; ++st) {
      if ( vecUp.at(itr).CoordX[st] < -900.) continue;
      Double_t Hit = vecUp.at(itr).CoordX[st]; 
      Double_t Fit = vecUp.at(itr).param[0] * vecUp.at(itr).CoordZ[st] + vecUp.at(itr).param[1];
      hResXst[st]->Fill(Hit - Fit);
      cout<<" st "<<st<<" Xhit "<<Hit<<" Xfit "<< Fit<<" Z "<<vecUp.at(itr).CoordZ[st]<<endl;
    }
    cout<<"--Y--"<<endl;
    for (Int_t st = 0; st < kPoints; ++st) {
      if ( vecUp.at(itr).CoordY[st] < -900.) continue;
      Double_t Hit = vecUp.at(itr).CoordY[st]; 
      Double_t Fit = vecUp.at(itr).param[2] * vecUp.at(itr).CoordZ[st] + vecUp.at(itr).param[3];
      hResYst[st]->Fill(Hit - Fit);
      cout<<" st "<<st<<" Yhit "<<Hit<<" Yfit "<< Fit<<" Z "<<vecUp.at(itr).CoordZ[st]<<endl;
    }
    Double_t x_target = vecUp.at(itr).param[0]*( kZ_target - Zcentr ) + vecUp.at(itr).param[1];
    Double_t y_target = vecUp.at(itr).param[2]*( kZ_target - Zcentr ) + vecUp.at(itr).param[3];
    
    hAx_fitUp->Fill(vecUp.at(itr).param[0]);
    hAy_fitUp->Fill(vecUp.at(itr).param[2]);
    hx_fitUp->Fill(vecUp.at(itr).param[1]);
    hy_fitUp->Fill(vecUp.at(itr).param[3]);
    hvertexXYUp->Fill(x_target,y_target);
    hy_vs_x_Up->Fill(vecUp.at(itr).param[1],vecUp.at(itr).param[3]);
    hchi2_fitUp->Fill(vecUp.at(itr).Chi2);
    hNhitsUp->Fill(vecUp.at(itr).Nhits);
    hTyTx_Up->Fill(vecUp.at(itr).param[0],vecUp.at(itr).param[2]);
    
  }
  cout<<"---"<<endl;
}
//----------------------------------------------------------------------

//----------------------------------------------------------------------
void BmnUpstreamTracking::TrackRecording(vector<UpTracks> & vecUp){
  

  for (Int_t InIter = 0; InIter < vecUp.size(); ++InIter) {
/*
    Double_t z0 = vecUp.at(InIter).CoordZ[1] + Zcentr;
    Double_t x0 = vecUp.at(InIter).param[0] * vecUp.at(InIter).CoordZ[0] + vecUp.at(InIter).param[1]; //ax(z)+bx
    Double_t y0 = vecUp.at(InIter).param[2] * vecUp.at(InIter).CoordZ[0] + vecUp.at(InIter).param[3]; //y
    Double_t z4 = vecUp.at(InIter).CoordZ[4] + Zcentr;
    Double_t x4 = vecUp.at(InIter).param[0] * vecUp.at(InIter).CoordZ[4] + vecUp.at(InIter).param[1]; //ax(z)+bx
    Double_t y4 = vecUp.at(InIter).param[2] * vecUp.at(InIter).CoordZ[4] + vecUp.at(InIter).param[3]; //y
    
    FairTrackParam trackParamf;
    trackParamf.SetPosition(TVector3(x0, y0, z0));
    trackParamf.SetTx(vecUp.at(InIter).param[0]);
    trackParamf.SetTy(vecUp.at(InIter).param[2]);

    FairTrackParam trackParaml;
    trackParaml.SetPosition(TVector3(x4, y4, z4));
    trackParaml.SetTx(vecUp.at(InIter).param[0]);
    trackParaml.SetTy(vecUp.at(InIter).param[2]);
*/

    FairTrackParam trackParaml;
    trackParaml.SetPosition(TVector3(vecUp.at(InIter).param[1]+ Shift_toCenterOfMagnetX, vecUp.at(InIter).param[3]+ Shift_toCenterOfMagnetY,Zcentr));
    trackParaml.SetTx(vecUp.at(InIter).param[0]+Shift_toCenterOfMagnetAX);
    trackParaml.SetTy(vecUp.at(InIter).param[2]+Shift_toCenterOfMagnetAY);

    BmnTrack *track = new ((*fBmnUpstreamTracksArray)[fBmnUpstreamTracksArray->GetEntriesFast()]) BmnTrack();
    track->SetChi2(vecUp.at(InIter).Chi2);
    track->SetNHits(vecUp.at(InIter).Nhits);
    track->SetFlag(InIter);
    //track->SetParamFirst(trackParamf);
    track->SetParamLast(trackParaml);

  }//InIter
  
}
//----------------------------------------------------------------------

//----------------------------------------------------------------------
bool BmnUpstreamTracking::InvertMatrix(Double_t *m, Double_t *invOut) {
  Double_t inv[16], det;
  int i;

  inv[0] = m[5] * m[10] * m[15] -m[5] * m[11] * m[14] -m[9] * m[6] * m[15] +m[9] * m[7] * m[14] +m[13] * m[6] * m[11] -m[13] * m[7] * m[10];
  inv[4] = -m[4] * m[10] * m[15] +m[4] * m[11] * m[14] +m[8] * m[6] * m[15] -m[8] * m[7] * m[14] -m[12] * m[6] * m[11] +m[12] * m[7] * m[10];
  inv[8] = m[4] * m[9] * m[15] -m[4] * m[11] * m[13] -m[8] * m[5] * m[15] +m[8] * m[7] * m[13] +m[12] * m[5] * m[11] -m[12] * m[7] * m[9];
  inv[12] = -m[4] * m[9] * m[14] +m[4] * m[10] * m[13] +m[8] * m[5] * m[14] -m[8] * m[6] * m[13] -m[12] * m[5] * m[10] +m[12] * m[6] * m[9];
  inv[1] = -m[1] * m[10] * m[15] +m[1] * m[11] * m[14] +m[9] * m[2] * m[15] -m[9] * m[3] * m[14] -m[13] * m[2] * m[11] +m[13] * m[3] * m[10];
  inv[5] = m[0] * m[10] * m[15] -m[0] * m[11] * m[14] -m[8] * m[2] * m[15] +m[8] * m[3] * m[14] +m[12] * m[2] * m[11] -m[12] * m[3] * m[10];
  inv[9] = -m[0] * m[9] * m[15] +m[0] * m[11] * m[13] +m[8] * m[1] * m[15] -m[8] * m[3] * m[13] -m[12] * m[1] * m[11] +m[12] * m[3] * m[9];
  inv[13] = m[0] * m[9] * m[14] -m[0] * m[10] * m[13] -m[8] * m[1] * m[14] +m[8] * m[2] * m[13] +m[12] * m[1] * m[10] -m[12] * m[2] * m[9];
  inv[2] = m[1] * m[6] * m[15] -m[1] * m[7] * m[14] -m[5] * m[2] * m[15] + m[5] * m[3] * m[14] +m[13] * m[2] * m[7] -m[13] * m[3] * m[6];
  inv[6] = -m[0] * m[6] * m[15] +m[0] * m[7] * m[14] +m[4] * m[2] * m[15] -m[4] * m[3] * m[14] -m[12] * m[2] * m[7] +m[12] * m[3] * m[6];
  inv[10] = m[0] * m[5] * m[15] -m[0] * m[7] * m[13] -m[4] * m[1] * m[15] +m[4] * m[3] * m[13] + m[12] * m[1] * m[7] - m[12] * m[3] * m[5];
  inv[14] = -m[0] * m[5] * m[14] +m[0] * m[6] * m[13] +m[4] * m[1] * m[14] -m[4] * m[2] * m[13] -m[12] * m[1] * m[6] +m[12] * m[2] * m[5];
  inv[3] = -m[1] * m[6] * m[11] +m[1] * m[7] * m[10] + m[5] * m[2] * m[11] -m[5] * m[3] * m[10] - m[9] * m[2] * m[7] +m[9] * m[3] * m[6];
  inv[7] = m[0] * m[6] * m[11] -m[0] * m[7] * m[10] -m[4] * m[2] * m[11] +m[4] * m[3] * m[10] +m[8] * m[2] * m[7] -m[8] * m[3] * m[6];
  inv[11] = -m[0] * m[5] * m[11] +m[0] * m[7] * m[9] +m[4] * m[1] * m[11] -m[4] * m[3] * m[9] -m[8] * m[1] * m[7] +m[8] * m[3] * m[5];
  inv[15] = m[0] * m[5] * m[10] -m[0] * m[6] * m[9] -m[4] * m[1] * m[10] +m[4] * m[2] * m[9] +m[8] * m[1] * m[6] -m[8] * m[2] * m[5];
  det = m[0] * inv[0] + m[1] * inv[4] + m[2] * inv[8] + m[3] * inv[12];

  if (det == 0)
      return false;
  det = 1.0 / det;
  for (i = 0; i < 16; i++)
      invOut[i] = inv[i] * det;
  return true;
}
//----------------------------------------------------------------------

//----------------------------------------------------------------------
void BmnUpstreamTracking::GlobalFit(Double_t** XYZsigXY, Double_t** Amatr_, Double_t* rhs_){
  
    Double_t sigmaX_2 = 999.;
    Double_t sigmaY_2 = 999.;
    Double_t X = 999., Y = 999., Z =999.;
    
  for (Int_t iSt = 0; iSt < kPoints; ++iSt) { 

    Z = XYZsigXY[iSt][2];

    if (XYZsigXY[iSt][0] > -900.){
      X = XYZsigXY[iSt][0];
      sigmaX_2 = XYZsigXY[iSt][3]* XYZsigXY[iSt][3];
      
      Amatr_[0][0] += 2*Z*Z/sigmaX_2;
      Amatr_[0][1] += 2*Z/sigmaX_2;
      rhs_[0] += 2*Z*X/sigmaX_2;
      
      Amatr_[1][0] += 2*Z/sigmaX_2;
      Amatr_[1][1] += 2/sigmaX_2;
      rhs_[1] += 2*X/sigmaX_2;
    }
    
    if (XYZsigXY[iSt][1] > -900.){
      Y = XYZsigXY[iSt][1];
      sigmaY_2 = XYZsigXY[iSt][4]* XYZsigXY[iSt][4];

      Amatr_[2][2] += 2*Z*Z/sigmaY_2;
      Amatr_[2][3] += 2*Z/sigmaY_2;
      rhs_[2] += 2*Z*Y/sigmaY_2;
      
      Amatr_[3][2] += 2*Z/sigmaY_2;
      Amatr_[3][3] += 2/sigmaY_2;
      rhs_[3] += 2*Y/sigmaY_2;
    }
    
    for (Int_t coeff = 0; coeff < 4; ++coeff) {	
      for (Int_t iCol = 0; iCol < 4; ++iCol) {
        //if ( fDebug ) cout << "GLOBALFIT: station = " << iSt << " " << "Amatr_[" << coeff << "][" << iCol << "] = " << Amatr_[coeff][iCol] << "\n";   
      }
       // if ( fDebug ) cout<<" rhs_["<<coeff<<"] "<< rhs_[coeff]<<endl;
    } 
  }

}//GlobalFit
//----------------------------------------------------------------------


//----------------------------------------------------------------------
InitStatus BmnUpstreamTracking::Init() {
 
  if (fVerbose) cout << "BmnUpstreamTracking == Init ==" << endl;
  FairRootManager* ioman = FairRootManager::Instance();

  //----Data & MC---
  fSiTracks = (TClonesArray*) ioman->GetObject(fInputBranchName1);
  if (!fSiTracks)
  {
    cout<<"BmnUpstreamTracking::Init(): branch "<<fInputBranchName1<<" not found! Task will be deactivated"<<endl;
    SetActive(kFALSE);
    return kERROR;
  }
  
  fSilHits = (TClonesArray*) ioman->GetObject(fInputBranchHits);
  if (!fSilHits){
    cout<<"BmnUpstreamTracking::Init(): branch "<<fInputBranchHits<<" not found! Task will be deactivated"<<endl;
    SetActive(kFALSE);
    return kERROR;
  }
  
  fMWPCTracks = (TClonesArray*) ioman->GetObject(fInputBranchName2);
  if (!fMWPCTracks){
    cout<<"BmnUpstreamTracking::Init(): branch "<<fInputBranchName2<<" not found! Task will be deactivated"<<endl;
    SetActive(kFALSE);
    return kERROR;
  }
  fMWPCSegments = (TClonesArray*) ioman->GetObject(fInputBranchName3);
  if (!fMWPCSegments)
  {
    cout<<"BmnUpstreamTracking::Init(): branch "<<fInputBranchName3<<" not found! Task will be deactivated"<<endl;
    SetActive(kFALSE);
    return kERROR;
  }
  
  //----MC true---
  if (!expData ){
    cout<<" !expData "<<endl;
    fSiTracksSim = (TClonesArray*) ioman->GetObject(fInputBranchNameSimTrue);
    if (!fSiTracksSim){
      cout<<"BmnUpstreamTracking::Init(): branch "<<fInputBranchNameSimTrue<<" not found! Task will be deactivated"<<endl;
      SetActive(kFALSE);
      return kERROR;
    }
    fBmnHitsArray = (TClonesArray*)ioman->GetObject(fInputBranchName);
    cout << "fBmnHitsArray = " << fInputBranchName << "\n";
    if (!fBmnHitsArray) {
      cout << "BmnMwpcHitFinder::Init(): branch " << fInputBranchName << " not found! Task will be deactivated" << endl;
      SetActive(kFALSE);
      return kERROR;
    }
  }// !expData 
  
  // Create and register tracks arrays
  fBmnUpstreamTracksArray = new TClonesArray(fOutputTracksBranchName);
  ioman->Register("BmnUpstreamTrack", "UpstreamTrack", fBmnUpstreamTracksArray, kTRUE);
 
  par_ab_Ch     = new Double_t**[kNumChambers];
  par_ab_tr     = new Double_t**[kNumPairs];
  SiXYhits      = new Double_t**[kNumStSi];
  Points        = new Double_t**[kBig];
  par_ab_SiTr   = new Double_t*[kNumPars];
  par_ab_trz    = new Double_t*[kNumPairs];
  par_Seg_z     = new Double_t*[kNumChambers];
  Xforglfit     = new Double_t*[kPoints];
  Amatr         = new Double_t*[4];
  par_SiTr_z    = new Double_t[kBig];
  X_shift_seg   = new Double_t[kNumChambers];
  Z_pair        = new Double_t[kNumPairs];
  Z_Chamber     = new Double_t[kNumChambers];
  AmatrInverted = new Double_t[16];
  rhs           = new Double_t[4];
  AmatrArray    = new Double_t[16];
  line          = new Double_t[4];
  Nseg_Ch       = new Int_t[kNumChambers];
  NPCTracks     = new Int_t[kNumPairs];
  NSiXYhits     = new Int_t[kNumStSi];
  X_shiftUp     = new Double_t[kPoints];
  Y_shiftUp     = new Double_t[kPoints];
  
  for (Int_t i = 0; i < 4; i++) {
    Amatr[i] = new Double_t[4];
  }
  
  for (Int_t iPars = 0; iPars < kNumPars; iPars++) {
    par_ab_SiTr[iPars] = new Double_t[kBig];
  }
  
  for (Int_t iCh = 0; iCh < kNumChambers; iCh++) {
    par_Seg_z[iCh] = new Double_t[kBig];
    par_ab_Ch[iCh] = new Double_t*[kNumPars];
    for (Int_t iPars = 0; iPars < kNumPars; iPars++) {
      par_ab_Ch[iCh][iPars] = new Double_t[kBig];
    }
  }
  for (Int_t ip = 0; ip < kNumPairs; ip++) {
    par_ab_tr[ip] = new Double_t*[kNumPars];
    par_ab_trz[ip]  = new Double_t[kBig];
    for (Int_t iPars = 0; iPars < kNumPars; iPars++) {
      par_ab_tr[ip][iPars] = new Double_t[kBig];
    }
  }
  for (Int_t ist = 0; ist < kNumStSi; ist++) {
    SiXYhits[ist] = new Double_t*[kBig];
    for (Int_t ib = 0; ib < kBig; ib++) {
      SiXYhits[ist][ib] = new Double_t[kNumPars];
    }
  }
  for (Int_t ib = 0; ib < kBig; ib++) {
    Points[ib] = new Double_t*[kPoints];
    for (Int_t ip = 0; ip < kPoints; ip++) {
      Points[ib][ip] = new Double_t[kPoin_Par];
    }
  }
  for (Int_t ip = 0; ip < kPoints; ip++) {
    Xforglfit[ip] = new Double_t[kPoin_Par]; 
  }
  
  //--some hists--
  if (fDebug){

    hAx_fitUp   =  new TH1D("Ax_fitUp","Tx_fitUp;[rad]", 200, -0.03, 0.03);fList.Add(hAx_fitUp);
    hAy_fitUp   =  new TH1D("Ay_fitUp","Ty_fitUp;[rad]", 200, -0.05, 0.05); fList.Add(hAy_fitUp);
    hx_fitUp    =  new TH1D("x_fitUp","x_fitUp;[cm]",  200, -10, 10);fList.Add(hx_fitUp);
    hy_fitUp    =  new TH1D("y_fitUp","y_fitUp;[cm]",  200, -15, 5);fList.Add(hy_fitUp);
    hy_vs_x_Up  =  new TH2D("y_vs_x_Up",";X[cm];Y[cm]",  200, -10, 10, 200, -15, 5);fList.Add(hy_vs_x_Up);
    hvertexXYUp =  new TH2D("vertexXYUp","vertexUp;X[cm];Y[cm]",  200, -10, 10, 200, -10, 10);fList.Add(hvertexXYUp);
    hTyTx_Up= new TH2D("hTyTx_Up","hTyTx_Up", 100, -0.04, 0.04, 100, -0.04, 0.04);fList.Add(hTyTx_Up);
    
    hchi2_fitUp =  new TH1D("chi2_fitUp","Chi2",       100, 0, 100);fList.Add(hchi2_fitUp);
    hNhitsUp    =  new TH1D("NhitsUp","Number of hits", 10, 0.5, 10.5);fList.Add(hNhitsUp);
    
    hAx_upmc  =  new TH1D("Ax_upmc","Ax_upmc;[rad]", 100, -0.05, 0.05);fList.Add(hAx_upmc);
    hAy_upmc  =  new TH1D("Ay_upmc","Ay_upmc;[rad]", 100, -0.05, 0.05);fList.Add(hAy_upmc);
    hX_upmc   =  new TH1D("X_upmc","X_upmc;[rad]", 100, -10, 10);fList.Add(hX_upmc);
    hY_upmc   =  new TH1D("Y_upmc","Y_upmc;[rad]", 100, -10, 10);fList.Add(hY_upmc);
    
    hdAx_tr_mc_comb =  new TH1D("dAx_tr_mc_comb","dAx_tr_mc_comb", 100, -0.005, 0.005);fList.Add(hdAx_tr_mc_comb);
    hdAy_tr_mc_comb =  new TH1D("dAy_tr_mc_comb","dAy_tr_mc_comb", 100, -0.005, 0.005);fList.Add(hdAy_tr_mc_comb);
    hdX_tr_mc_comb  =  new TH1D("dX_tr_mc_comb","dX_tr_mc_comb", 100, -10, 10);        fList.Add(hdX_tr_mc_comb);
    hdY_tr_mc_comb  =  new TH1D("dY_tr_mc_comb","dY_tr_mc_comb", 100, -10, 10);        fList.Add(hdY_tr_mc_comb);
    
    hdAx_uptr_mc =  new TH1D("dAx_uptr_mc","dAx_uptr_mc", 200, -0.002, 0.002);fList.Add(hdAx_uptr_mc);
    hdAy_uptr_mc =  new TH1D("dAy_uptr_mc","dAy_uptr_mc", 200, -0.005, 0.005);fList.Add(hdAy_uptr_mc);
    hdX_uptr_mc  =  new TH1D("dX_uptr_mc","dX_uptr_mc", 200, -2, 2);         fList.Add(hdX_uptr_mc);
    hdY_uptr_mc  =  new TH1D("dY_uptr_mc","dY_uptr_mc", 200, -2, 2);         fList.Add(hdY_uptr_mc);
    hY_vs_Xmctrue=  new TH2D("Y_vs_Xmctrue","mctrue;X[cm];Y[cm]",  200, -10, 10, 200, -15, 5);fList.Add(hY_vs_Xmctrue);
    
    
    hNtr_reco       =  new TH1D("Ntr_reco","N up tr reco", 10, 0,10); fList.Add(hNtr_reco);
    hNtr_mc         =  new TH1D("Ntr_mc","N up tr mc", 10, 0,10);     fList.Add(hNtr_mc);
    hNtr_mc_vs_reco =  new TH2D("Ntr_mc_vs_reco","; Nupreco; Nupmctrue", 10, 0,10, 10, 0,10);fList.Add(hNtr_mc_vs_reco);
    hNrecoTrif2mc   =  new TH1D("NrecoTrif2mc","N Up tracks if Nmc=2", 3, 0,3);  fList.Add(hNrecoTrif2mc);
    hAngle_reco     =  new TH1D("Angle_reco","Angle_reco if Nreco = 2 ;[deg];N/0.02^{o}", 200, 0., 4.);     fList.Add(hAngle_reco);
    hAngle_recoifNmc2 =  new TH1D("Angle_recoifNmc2","Angle_reco if Nreco = 2 &  Nmc = 2;[deg];N/0.02^{o}", 200, 0., 4.);   fList.Add(hAngle_recoifNmc2);
    hAngle_recoifNmc2Cases =  new TH1D("Angle_recoifNmc2Cases","Angle_reco if Nreco = 2 &  Nmc = 2 & SelectedCases;[deg];N/0.02^{o}", 200, 0., 4.);   fList.Add(hAngle_recoifNmc2Cases);
    
    hNum_mcuptr =  new TH1D("hNum_mcuptr","Num_mcuptr", 2, 0, 2);fList.Add(hNum_mcuptr);
    hDen_mcuptr =  new TH1D("hDen_mcuptr","Den_mcuptr", 2, 0, 2);fList.Add(hDen_mcuptr);
    hEff_mcuptr =  new TH1D("hEff_mcuptr","Eff_mc_uptr", 2, 0, 2);fList.Add(hEff_mcuptr);
    hEff_mcuptr->GetXaxis()->SetBinLabel(1,"2+3 coord");
    hEff_mcuptr->GetXaxis()->SetBinLabel(2,"only 3coord");
    
    TH1D *h;
    for (Int_t i = 0; i < kPoints; ++i) {
      if (i == 2 || i == 4)  h = new TH1D(Form("ResXst%d", i), Form("ResXst%d", i), 100, -0.5, 0.5);
      else h = new TH1D(Form("ResXst%d", i), Form("ResXst%d", i), 150, -0.015, 0.015);
      fList.Add(h);
      hResXst.push_back(h);
    }
    for (Int_t i = 0; i < kPoints; ++i) {
      if (i == 2 || i == 4)  h = new TH1D(Form("ResYst%d", i), Form("ResYst%d", i), 100, -0.25, 0.25);
      //else if(i == 4) h = new TH1D(Form("ResYst%d", i), Form("ResYst%d", i), 100, -0.2, 0.2);
      else h = new TH1D(Form("ResYst%d", i), Form("ResYst%d", i), 100, -0.4, 0.4);
      fList.Add(h); 
      hResYst.push_back(h);
    }
  }
  
  Z_pair[0]    = -808.816;
  Z_pair[1]    = -289.401;
  Z_Chamber[0] = -858.155579;
  Z_Chamber[1] = -761.476562;
  Z_Chamber[2] = -364.255554;
  Z_Chamber[3] = -214.545547;
  
  //--Shift--
  if (expData){
    X_shift_seg[0] =  0.;
    X_shift_seg[1] =  0.;
    X_shift_seg[2] = -0.04;
    X_shift_seg[3] = -0.05;
    Shift_toCenterOfMagnetX  = 0.039;
    Shift_toCenterOfMagnetY  = 0.9;//0.13;// drawing
    Shift_toCenterOfMagnetAX = 0.;
    Shift_toCenterOfMagnetAY = 0.0019;//
    X_shift   = 0.02;
    Y_shift   = 0.3;
    
  }else{
    X_shift_seg[0] = 0.;
    X_shift_seg[1] = 0.;
    X_shift_seg[2] = 0.;
    X_shift_seg[3] = 0.;
    Shift_toCenterOfMagnetX  = 0.;
    Shift_toCenterOfMagnetY  = 0.;
    Shift_toCenterOfMagnetAX = 0.;
    Shift_toCenterOfMagnetAY = 0.;
    X_shift = 0.;
    Y_shift = 0.;
  }
  
  //-- 5 detectors Shift---
  X_shiftUp[0] = 0.;//
  X_shiftUp[1] = 0.;
  X_shiftUp[2] = 0.;
  X_shiftUp[3] = 0.;
  X_shiftUp[4] = 0.;//-0.015;
  
  Y_shiftUp[0] = 0.;//-0.005 + 0.02-0.005-0.008+0.03;
  Y_shiftUp[1] = 0.;//+0.03;
  Y_shiftUp[2] = 0.;//+0.006;
  Y_shiftUp[3] = 0.;//+.1;
  Y_shiftUp[4] = 0.;//+0.002;
  
  
  return kSUCCESS;
}
//----------------------------------------------------------------------


//---------------------Initialisation-----------------------------------
void BmnUpstreamTracking::PrepareArraysToProcessEvent() {
  fBmnUpstreamTracksArray->Clear();
  OutVector.clear();
  vecUpTracks.clear();
  vec_points.clear();
  vec_twotracks.clear();
  
  NSiTracks = 0;
  for (Int_t iPars = 0; iPars < kNumPars; iPars++) {
    for (Int_t j = 0; j < kBig; j++) {
     par_ab_SiTr[iPars][j] = -999.;
     par_SiTr_z[j]     = -999.;
    }
  }
  
  for (Int_t iCh = 0; iCh < kNumChambers; iCh++) {
    Nseg_Ch[iCh] = 0;
    
    for (Int_t iPars = 0; iPars < kNumPars; iPars++) {
      for (Int_t j = 0; j < kBig; j++) {
        par_ab_Ch[iCh][iPars][j] = -999.;
        par_Seg_z[iCh][j] = -999.;
      }
    }
  }
  for (Int_t ip = 0; ip < kNumPairs; ip++) {
    NPCTracks[ip] = 0;
    for (Int_t iPars = 0; iPars < kNumPars; iPars++) {
     for (Int_t j = 0; j < kBig; j++) {
      par_ab_tr[ip][iPars][j] = -999.;
      }
    }
  }
  for (Int_t ist = 0; ist < kNumStSi; ist++) {
    NSiXYhits[ist] = 0;
    for (Int_t ib = 0; ib < kBig; ib++) {
      for (Int_t iPars = 0; iPars < kNumPars; iPars++) {
        SiXYhits[ist][ib][iPars] = -999.;
      }
    }
  }
  for (Int_t ib = 0; ib < kBig; ib++) {
    for (Int_t ip = 0; ip < kPoints; ip++) {
      for (Int_t i = 0; i < kPoin_Par; i++) {
        Points[ib][ip][i] = -999.;
      }
    }
  }

}
//----------------------------------------------------------------------

BmnUpstreamTracking::BmnUpstreamTracking() :
  hAx_fitUp(nullptr),
  hAy_fitUp(nullptr),
  hx_fitUp(nullptr),
  hy_fitUp(nullptr),
  hchi2_fitUp(nullptr),
  hNhitsUp(nullptr),
  hdAx_tr_mc_comb(nullptr),
  hdX_tr_mc_comb(nullptr),
  hdAy_tr_mc_comb(nullptr),
  hdY_tr_mc_comb(nullptr),
  hdAx_uptr_mc(nullptr),
  hdX_uptr_mc(nullptr),
  hdAy_uptr_mc(nullptr),
  hdY_uptr_mc(nullptr),
  hDen_mcuptr(nullptr),
  hNum_mcuptr(nullptr),
  hEff_mcuptr(nullptr),
  hAx_upmc(nullptr),
  hAy_upmc(nullptr),
  hX_upmc(nullptr),
  hY_upmc(nullptr),
  hNtr_reco(nullptr),
  hNtr_mc(nullptr),
  hNrecoTrif2mc(nullptr),
  hAngle_reco(nullptr),
  hAngle_recoifNmc2(nullptr),
  hAngle_recoifNmc2Cases(nullptr),
  hNtr_mc_vs_reco(nullptr),
  hy_vs_x_Up(nullptr),
  hY_vs_Xmctrue(nullptr),
  hvertexXYUp(nullptr),
  hTyTx_Up(nullptr)
{}

//-------------------------constructor----------------------------------
BmnUpstreamTracking::BmnUpstreamTracking(Bool_t isExp, Int_t runNumber)
{
  fRunNumber = runNumber;
  expData    = isExp;
  fInputBranchName1       = "BmnSiliconTrack";
  fInputBranchName2       = "BmnMwpcTrack";
  fInputBranchName3       = "BmnMwpcSegment";
  fInputBranchHits        = "BmnSiliconHits";
  fOutputTracksBranchName = "BmnTrack";
  if (!expData){
    fRunNumber = 0001;
    fInputBranchNameSimTrue = "BmnSiliconHitClean";
    fInputBranchName        = "BmnMwpcHit";
  }
  
}
//----------------------------------------------------------------------


//-------------------Destructor-----------------------------------------
BmnUpstreamTracking::~BmnUpstreamTracking() {
  
  for (Int_t iPars = 0; iPars < kNumPars; iPars++) {
    delete[] par_ab_SiTr[iPars];
  }
  for (Int_t iCh = 0; iCh < kNumChambers; iCh++) {
    for (Int_t iPars = 0; iPars < kNumPars; iPars++) {
      delete[] par_ab_Ch[iCh][iPars];
    }
    delete[] par_ab_Ch[iCh];
    delete[] par_Seg_z[iCh];
  }
  for (Int_t ip = 0; ip < kNumPairs; ip++) {
    for (Int_t iPars = 0; iPars < kNumPars; iPars++) {
      delete[] par_ab_tr[ip][iPars];
    }
    delete[] par_ab_tr[ip];
    delete[] par_ab_trz[ip];
  }
  for (Int_t ist = 0; ist < kNumStSi; ist++) {
    for (Int_t ib = 0; ib < kBig; ib++) {
      delete[] SiXYhits[ist][ib];
    }
    delete[] SiXYhits[ist];
  }
  for (Int_t ib = 0; ib < kBig; ib++) {
    for (Int_t ip = 0; ip < kPoints; ip++) {
      delete[] Points[ib][ip];
    }
    delete[] Points[ib];
  }
  for (Int_t ip = 0; ip < kPoints; ip++) {
    delete[] Xforglfit[ip];
  }
  for (Int_t i = 0; i < 4; i++) {
    delete[] Amatr[i];
  }
  delete[] X_shiftUp;
  delete[] AmatrInverted;
  delete[] rhs;
  delete[] AmatrArray;
  delete[] Amatr;
  delete[] line;
  delete[] Xforglfit;
  delete[] Points;
  delete[] NSiXYhits;
  delete[] SiXYhits;
  delete[] par_ab_trz;
  delete[] par_ab_tr;
  delete[] par_ab_SiTr;
  delete[] par_SiTr_z;
  delete[] par_ab_Ch;
  delete[] par_Seg_z;
  delete[] Nseg_Ch;
  delete[] X_shift_seg;
  delete[] Z_pair;
  delete[] Z_Chamber;
  delete[] NPCTracks;
  delete[] Y_shiftUp;
}
//----------------------------------------------------------------------



//----------------------------------------------------------------------
void BmnUpstreamTracking::Finish() {

  if (fDebug) {
  
   hEff_mcuptr->Divide(hNum_mcuptr,hDen_mcuptr,1,1);
   printf("BmnUpstreamTracking: write hists to file... ");
   fOutputFileName = Form("hUpstreamTracks_run%d.root", fRunNumber);
   cout<< fOutputFileName <<endl;
   TFile file(fOutputFileName, "RECREATE");
   fList.Write();
   file.Close();
  }
  cout << "Work time of the UpstreamTrack finder: " << workTime << " s" << endl;
}
//----------------------------------------------------------------------

ClassImp(BmnUpstreamTracking)
