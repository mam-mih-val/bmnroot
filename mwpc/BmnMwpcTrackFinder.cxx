// Author: Vasilisa Lenivenko <vasilisa@jinr.ru> 2018-07-18

////////////////////////////////////////////////////////////////////////////////
//                                      //
// BmnMwpcTrackFinder                             //
//                                      //
//                                      //
//Implementation of an algorithm developed by                 //
// Vasilisa Lenivenko and Vladimir Palchik                  //
// to the BmnRoot software                          //
//                                      //
// The algorithm serves for searching for track segments           //
// in the MWPC of the BM@N experiment                     //
//                                      //
////////////////////////////////////////////////////////////////////////////////
#include <Rtypes.h>
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
  fInputBranchName = "BmnMwpcSegment";
  fOutputBranchName = "BmnMwpcTrack";
  fRunPeriod = runP;
  fRunNumber = runNumber;
  fMwpcGeo = new BmnMwpcGeometrySRC(fRunPeriod, fRunNumber);
  kBig = 100;
  kCh_min = 0;
  if (fRunPeriod == 6 || (fRunPeriod == 7 && fRunNumber > 3588) ) {
    kNumPairs = 1;
    kCh_max = 2;
  } else if ( fRunPeriod == 7 && fRunNumber <= 3588 ) {//SRC
    kNumPairs = 2;
    kCh_max = 4;
  }
}

BmnMwpcTrackFinder::~BmnMwpcTrackFinder() {
}

void BmnMwpcTrackFinder::Exec(Option_t* opt) {
  if (!IsActive()) return;
  clock_t tStart = clock();
  PrepareArraysToProcessEvent();
  if (fDebug) cout << "\n======================== MWPC track finder exec started ===================\n" << endl;
  if (fDebug) cout << "Event number: " << fEventNo++ << endl;

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

    Nhits_Ch[iCh][ise]   = segment->GetNHits();
    Chi2_ndf_Ch[iCh][ise] = segment->GetChi2();
    par_ab_Ch[iCh][0][ise] = segment->GetParamFirst()->GetTx();
    par_ab_Ch[iCh][1][ise] = segment->GetParamFirst()->GetX();
    par_ab_Ch[iCh][2][ise] = segment->GetParamFirst()->GetTy();
    par_ab_Ch[iCh][3][ise] = segment->GetParamFirst()->GetY();

    for(Int_t i1 = 0 ; i1 < 6; i1++) {
      XVU_Ch[iCh][i1][ise]  = segment -> GetCoord().at(i1);
      Clust_Ch[iCh][i1][ise] = segment -> GetClust().at(i1);
    }
    Nbest_Ch[iCh]++;
  }//iSegment
  //--------------------------------------------------------------------

  for (Int_t iChamber = 0; iChamber < kNChambers; iChamber++) {
    if (fDebug && Nbest_Ch[iChamber] > 0) {
      for (Int_t ise = 0; ise < Nbest_Ch[iChamber]; ise++) {
        hChi2_ndf_Ch.at(iChamber) -> Fill(Chi2_ndf_Ch[iChamber][ise]);
        hpar_Ax_Ch.at(iChamber) -> Fill( par_ab_Ch[iChamber][0][ise]);
        hpar_Bx_Ch.at(iChamber) -> Fill( par_ab_Ch[iChamber][1][ise]);
        hpar_Ay_Ch.at(iChamber) -> Fill( par_ab_Ch[iChamber][2][ise]);
        hpar_By_Ch.at(iChamber) -> Fill( par_ab_Ch[iChamber][3][ise]);
        cout<<" iChamber "<<iChamber<<" ax= "<<par_ab_Ch[iChamber][0][ise]<<" bx= "<<par_ab_Ch[iChamber][1][ise]<<" ay= "<<par_ab_Ch[iChamber][2][ise]<<" by= "<<par_ab_Ch[iChamber][3][ise]<<" Chi2 "<<Chi2_ndf_Ch[iChamber][ise]<<endl;
      }
    }
  }//iChamber



  //--------Track-Segment  matching between chambers--------------------
  if ( Nbest_Ch[0] > 0 && Nbest_Ch[1] > 0) {
    if (fDebug) cout<<" N0= "<<Nbest_Ch[0]<<" N1= "<<Nbest_Ch[1]<<endl;
    SegmentMatching( 0, Nbest_Ch, par_ab_Ch, kZmid, ind_best_Ch, Nbest_pair, Chi2_match_pair, XVU_Ch, Nhits_Ch, Nhits_match);
  }

  if (kNChambers > 2 && Nbest_Ch[2] > 0 && Nbest_Ch[3] > 0) {
    if (fDebug) cout<<" N2= "<<Nbest_Ch[2]<<" N3= "<<Nbest_Ch[3]<<endl;
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


  // ----------------Segmet from 2-stations Fit-------------------------
  Int_t First_Chamber = 0;
  if (fDebug) cout<<" kNumPairs "<<kNumPairs<<" Nbest_pair[0] "<<Nbest_pair[0]<<" Nbest_pair[1] "<<Nbest_pair[1]<<endl;
  if (kNumPairs > 0 && Nbest_pair[0] > 0) {
    if (fDebug) cout<<" Nbest_pair[0] "<<Nbest_pair[0]<<endl;
    SegmentFit(First_Chamber, z_gl, sigm2, Nbest_pair, ind_best_Ch, par_ab_pair, Chi2_ndf_pair, XVU_Ch, Clust_Ch, ind_best_pair, Nhits_Ch, Nhits_pair);
  }

  if (kNumPairs == 2 && Nbest_pair[1] > 0) {
    //if (fDebug) cout<<" Nbest_pair[1] "<<Nbest_pair[1]<<endl;
    SegmentFit(First_Chamber+2, z_gl, sigm2, Nbest_pair, ind_best_Ch, par_ab_pair, Chi2_ndf_pair, XVU_Ch, Clust_Ch, ind_best_pair, Nhits_Ch, Nhits_pair);
  }
  //--------------------------------------------------------------------


  //------------------Segment Parameters Alignment----------------------
  for (Int_t iPair = 0; iPair < kNumPairs; iPair++) {
    if ( Nbest_pair[iPair] > 0) SegmentParamAlignment(iPair, Nbest_pair, par_ab_pair, shift_pair);
  }
  //--------------------------------------------------------------------

  //--------------------MWPC pairs matching ----------------------------
  // if ( Nbest_pair[0] > 0 && Nbest_pair[1] > 0 ) PairMatching(Nbest_pair, par_ab_pair, kZ_midle_pair);
  //--------------------------------------------------------------------
  
  
  //----------------------Tracks storing------------------------------
  Double_t theta, phi;
  Float_t X_par_to_target, Y_par_to_target;
  for (Int_t iPair = 0; iPair < kNumPairs; iPair++) {
    if ( Nbest_pair[iPair] > 0) {

      for (Int_t itr = 0; itr < Nbest_pair[iPair]; itr++) {
        if (Chi2_ndf_pair[iPair][itr] > 1000.) continue;
        
        if (fDebug){
          cout<<" Chi2_ndf_pair "<<Chi2_ndf_pair[iPair][itr]<<endl;
          X_par_to_target = par_ab_pair[iPair][0][itr]*( kZ_target - kZ_midle_pair[iPair]) + par_ab_pair[iPair][1][itr];
          Y_par_to_target = par_ab_pair[iPair][2][itr]*( kZ_target - kZ_midle_pair[iPair]) + par_ab_pair[iPair][3][itr];
          phi = TMath::ATan2(par_ab_pair[iPair][2][itr],par_ab_pair[iPair][0][itr]); // phi = arctan(tgy/tgx)
          theta = TMath::ATan2(par_ab_pair[iPair][0][itr], TMath::Cos(phi));// theta = arctan(tgx/cos(phi))
          hpar_Ax_pair.at(iPair)   -> Fill(TMath::RadToDeg()* par_ab_pair[iPair][0][itr]);
          hpar_Bx_pair.at(iPair)   -> Fill( par_ab_pair[iPair][1][itr]);
          hpar_Ay_pair.at(iPair)   -> Fill(TMath::RadToDeg()* par_ab_pair[iPair][2][itr]);
          hpar_By_pair.at(iPair)   -> Fill(par_ab_pair[iPair][3][itr]);
          hpar_theta_pair.at(iPair)-> Fill(TMath::RadToDeg()*theta);
          hpar_phi_pair.at(iPair)  -> Fill(TMath::RadToDeg()*phi);
          hX_in_target_pair.at(iPair)   -> Fill(X_par_to_target);
          hY_in_target_pair.at(iPair)   -> Fill(Y_par_to_target);
          hAx_bx_in_target -> Fill(X_par_to_target, TMath::RadToDeg()*par_ab_pair[iPair][0][itr]);
          hAy_by_in_target -> Fill(Y_par_to_target, TMath::RadToDeg()*par_ab_pair[iPair][2][itr]);
        }

        BmnTrack *Tr = new ((*fBmnMwpcTracksArray)[fBmnMwpcTracksArray->GetEntriesFast()]) BmnTrack();
        Tr -> SetChi2(Chi2_ndf_pair[iPair][itr]);
        Tr -> SetNHits(Nhits_pair[iPair][itr]);
        Tr -> SetFlag(itr);
        FairTrackParam TrParams;
        TrParams.SetPosition(TVector3(par_ab_pair[iPair][1][itr], par_ab_pair[iPair][3][itr],kZ_midle_pair[iPair]));
        TrParams.SetTx(par_ab_pair[iPair][0][itr]);
        TrParams.SetTy(par_ab_pair[iPair][2][itr]);
        Tr -> SetParamFirst(TrParams);

      }//Nbest_pair[iPair]

    }//> 0
  }//iPair
  //--------------------------------------------------------------------


  if (fDebug) cout << "\n======================== MWPC track finder exec finished ==================" << endl;
  clock_t tFinish = clock();
  workTime += ((Float_t) (tFinish - tStart)) / CLOCKS_PER_SEC;
}//Exec
//----------------------------------------------------------------------------------






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
void BmnMwpcTrackFinder::SegmentMatching( Int_t first_Ch, Int_t *Nbest, Double_t ***par_ab, Float_t *Zmid, Int_t **best_Ch, Int_t *Nbest_pair_, Double_t **Chi2_match_, Double_t ***XVU_Ch_, Int_t **Nhits_, Int_t **Nhits_m) {
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
  if ( fRunPeriod == 7 && first_Ch == 2) Pairr = 1;
  Int_t Secon_Ch = first_Ch+1;

  if (Nbest_Ch[first_Ch] > 0 && Nbest_Ch[Secon_Ch] > 0) {

    for (Int_t bst1 = 0; bst1 < Nbest[first_Ch]; bst1++) {
      //ch1                   zloc0 -z_i
      Float_t x1mid = par_ab[first_Ch][0][bst1] *( 0 - kZmid[first_Ch]) + par_ab[first_Ch][1][bst1] ;
      Float_t y1mid = par_ab[first_Ch][2][bst1] *( 0 - kZmid[first_Ch]) + par_ab[first_Ch][3][bst1] ;

      for (Int_t bst2 = 0; bst2 < Nbest[Secon_Ch]; bst2++) {
        //ch2
        Float_t x2mid = par_ab[Secon_Ch][0][bst2] *( 0 - kZmid[Secon_Ch]) + par_ab_Ch[Secon_Ch][1][bst2] ;
        Float_t y2mid = par_ab[Secon_Ch][2][bst2] *( 0 - kZmid[Secon_Ch]) + par_ab_Ch[Secon_Ch][3][bst2] ;
        dAx12 = par_ab[first_Ch][0][bst1] - par_ab[Secon_Ch][0][bst2];
        dAy12 = par_ab[first_Ch][2][bst1] - par_ab[Secon_Ch][2][bst2];
        min_distX = x1mid - x2mid; //min
        min_distY = y1mid - y2mid; //min
        dx_loc  = par_ab[first_Ch][1][bst1] - par_ab_Ch[Secon_Ch][1][bst2];
        dy_loc  = par_ab[first_Ch][3][bst1] - par_ab_Ch[Secon_Ch][3][bst2];
        //if (fDebug) cout<<" Pairr "<<Pairr<<" sigma ax "<<sigma_delta[Pairr][0]<<" ay "<<sigma_delta[Pairr][2]<<" sigma x "<<sigma_delta[Pairr][1]<<" y "<<sigma_delta[Pairr][3]<<endl;

        Float_t Chi2_m = ( dx_loc*dx_loc/(sigma_delta[Pairr][1]*sigma_delta[Pairr][1]) + dy_loc*dy_loc/(sigma_delta[Pairr][3]*sigma_delta[Pairr][3])
                  + dAx12*dAx12 /(sigma_delta[Pairr][0]*sigma_delta[Pairr][0]) + dAy12*dAy12 /(sigma_delta[Pairr][2]*sigma_delta[Pairr][2]) );
        if (fDebug){
          hdX_Zmid_pair.at(Pairr)->Fill(min_distX);
          hdY_Zmid_pair.at(Pairr)->Fill(min_distY);
          hdAx_Zmid_pair.at(Pairr)->Fill(dAx12);
          hdAy_Zmid_pair.at(Pairr)->Fill(dAy12);
        }
        //if (fDebug)cout<<" Pairr "<<Pairr<<" bst1 "<<bst1<<" Nhits "<<Nhits_[first_Ch][bst1]<<" bst2 "<<bst2<<" Nhits "<<Nhits_[Secon_Ch][bst2]<<" Chi2_m "<<Chi2_m<<endl;

        if (Chi2_m < min_Chi2m && Nvariat < Nvariations && fabs(dx_loc) < 1.5 && fabs(dy_loc) < 1.5) {
          tmpSeg.Chi2m = Chi2_m;
          tmpSeg.Ind1  = bst1;
          tmpSeg.Ind2  = bst2;
          tmpSeg.Nhits1 = Nhits_[first_Ch][bst1];
          tmpSeg.Nhits2 = Nhits_[Secon_Ch][bst2];
          tmpSeg.distX = min_distX;
          tmpSeg.distY = min_distY;

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
  Float_t min_Chi2m = 100;
  Float_t min_distX = 99;
  Float_t min_distY = 99;
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

  Int_t Pairr = 0;//doesn't work
  if ( fRunPeriod == 7 && first_Ch == 2) Pairr = 1;// main stream
  Int_t Secon_Ch = first_Ch+1;

  if (Nbest_Ch[first_Ch] > 0 && Nbest_Ch[Secon_Ch] > 0) {
    // if (fDebug)cout<<" Nbest[ "<<first_Ch<<"] = "<<Nbest_Ch[first_Ch]<<endl;
    // if (fDebug)cout<<" Nbest[ "<<Secon_Ch<<"] = "<<Nbest_Ch[Secon_Ch]<<endl;

    for (Int_t bst1 = 0; bst1 < Nbest[first_Ch]; bst1++) {

      //ch1                   zloc0 -z_i
      Float_t x1mid = par_ab[first_Ch][0][bst1] *( 0 - kZmid[first_Ch]) + par_ab[first_Ch][1][bst1] ;
      Float_t y1mid = par_ab[first_Ch][2][bst1] *( 0 - kZmid[first_Ch]) + par_ab[first_Ch][3][bst1] ;
      //  cout<<" bst1 " <<bst1<<" x1mid "<<x1mid<<" y1mid "<<y1mid<<endl;

      for (Int_t bst2 = 0; bst2 < Nbest[Secon_Ch]; bst2++) {
        //ch2
        Float_t x2mid = par_ab[Secon_Ch][0][bst2] *( 0 - kZmid[Secon_Ch]) + par_ab_Ch[Secon_Ch][1][bst2] ;
        Float_t y2mid = par_ab[Secon_Ch][2][bst2] *( 0 - kZmid[Secon_Ch]) + par_ab_Ch[Secon_Ch][3][bst2] ;
        //if (fDebug)cout<<" bst2 " <<bst2<<" x2mid "<<x2mid<<" y2mid "<<y2mid<<endl;

        dAx12 = par_ab[first_Ch][0][bst1] - par_ab[Secon_Ch][0][bst2];
        dAy12 = par_ab[first_Ch][2][bst1] - par_ab[Secon_Ch][2][bst2];
        min_distX = x1mid - x2mid; //min
        min_distY = y1mid - y2mid; //min

        Double_t Z0_SRC = -647.476;
        Double_t Ax_23 = (par_ab[Secon_Ch][1][bst2] - par_ab[first_Ch][1][bst1])/ (kZmid[Secon_Ch] - kZmid[first_Ch]);
        Double_t x_target  = Ax_23*( Z0_SRC - ZCh[2]) + par_ab[first_Ch][1][bst1];
        Double_t Ay_23 = (par_ab[Secon_Ch][3][bst2] - par_ab[first_Ch][3][bst1])/ (kZmid[Secon_Ch] - kZmid[first_Ch]);
        Double_t y_target  = Ay_23*( Z0_SRC - ZCh[2]) + par_ab[first_Ch][3][bst1];

        if (fDebug) cout<<" dx "<<min_distX<<" dy "<<min_distY<<" x_target "<< x_target<<" y_target "<< y_target<<endl;
        //if (fDebug)	cout<<" Pairr "<<Pairr<<" sigma ax "<<sigma_delta[Pairr][0]<<" ay "<<sigma_delta[Pairr][2]<<" sigma x "<<sigma_delta[Pairr][1]<<" y "<<sigma_delta[Pairr][3]<<endl;

        Float_t Chi2_m = ( min_distX*min_distX/(sigma_delta[Pairr][1]*sigma_delta[Pairr][1]) + min_distY*min_distY/(sigma_delta[Pairr][3]*sigma_delta[Pairr][3])
                  + dAx12*dAx12 /(sigma_delta[Pairr][0]*sigma_delta[Pairr][0]) + dAy12*dAy12 /(sigma_delta[Pairr][2]*sigma_delta[Pairr][2]) );
        if (fDebug){
          hdX_Zmid_pair.at(Pairr)->Fill(min_distX);
          hdY_Zmid_pair.at(Pairr)->Fill(min_distY);
          hdAx_Zmid_pair.at(Pairr)->Fill(dAx12);
          hdAy_Zmid_pair.at(Pairr)->Fill(dAy12);
        }
        //if (fDebug)cout<<" Pairr "<<Pairr<<" bst1 "<<bst1<<" Nhits "<<Nhits_[first_Ch][bst1]<<" bst2 "<<bst2<<" Nhits "<<Nhits_[Secon_Ch][bst2]<<" Chi2_m "<<Chi2_m<<endl;

        if (Chi2_m < min_Chi2m && Nvariat < Nvariations && fabs(x_target) < ktarget_region && fabs(y_target) < ktarget_region) {

          tmpSeg.Chi2m = Chi2_m;
          tmpSeg.Ind1  = bst1;
          tmpSeg.Ind2  = bst2;
          tmpSeg.Nhits1 = Nhits_[first_Ch][bst1];
          tmpSeg.Nhits2 = Nhits_[Secon_Ch][bst2];
          tmpSeg.distX = min_distX;
          tmpSeg.distY = min_distY;
          tmpSeg.distAX = dAx12;
          tmpSeg.distAY = dAy12;

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
            exist_pair[iter] = 0;

          }
        }//iter
      }//if ( im + 1 < vtmpSeg.size())
    }//im

    if (fDebug) cout<<" OutVector.at(0).Chi2m "<<OutVector.at(0).Chi2m<<" vtmpSeg.at(0).Chi2m "<<vtmpSeg.at(0).Chi2m<<endl;
    if (fDebug && vtmpSeg.size() > 1 ) hChi2best_Chi2fake_after_target-> Fill(OutVector.at(0).Chi2m, vtmpSeg.at(1).Chi2m);

    Double_t Z0_SRC = -647.476;
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
          hdX_Zmid_pair_1 ->Fill(OutVector.at(iter).distX);
          hdY_Zmid_pair_1 ->Fill(OutVector.at(iter).distY);
          hdAx_Zmid_pair_1->Fill(OutVector.at(iter).distAX);
          hdAy_Zmid_pair_1->Fill(OutVector.at(iter).distAY);
          hChi2m_pair_1  ->Fill(OutVector.at(iter).Chi2m);
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
    Double_t x1 = par_ab_p[0][0][pair0] *( kZ_target - kZ_midle_pair_[0] ) + par_ab_p[0][1][pair0] ;
    Double_t y1 = par_ab_p[0][2][pair0] *( kZ_target - kZ_midle_pair_[0] ) + par_ab_p[0][3][pair0] ;
    // if (fDebug) cout<<" pair "<<0<<" pair0 "<<pair0<<" ax "<<par_ab_p[0][0][pair0]<<" bx "<< par_ab_p[0][1][pair0]<<" ay "<<par_ab_p[0][2][pair0]<<" by "<<par_ab_p[0][3][pair0]<<" kZ_target "<<kZ_target<<" kZ_midle_pair "<<kZ_midle_pair_[0]<<endl;

    for (Int_t pair1 = 0; pair1 < Nbest_p[1]; pair1++) {

      //pair 1
      Double_t x2 = par_ab_p[1][0][pair1] *( kZ_target - kZ_midle_pair_[1] ) + par_ab_p[1][1][pair1] ;
      Double_t y2 = par_ab_p[1][2][pair1] *( kZ_target - kZ_midle_pair_[1] ) + par_ab_p[1][3][pair1] ;
      // if (fDebug) cout<<" pair "<<1<<" pair1 "<<pair1<<" ax "<<par_ab_p[1][0][pair1]<<" bx "<< par_ab_p[1][1][pair1]<<" ay "<<par_ab_p[1][2][pair1]<<" by "<<par_ab_p[1][3][pair1]<<" kZ_target "<<kZ_target<<" kZ_midle_pair "<<kZ_midle_pair_[1]<<endl;
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
          hY_X_in_target_pair.at(0)  -> Fill(x1, y1);
          hY_X_in_target_pair.at(1)  -> Fill(x2, y2);
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
void BmnMwpcTrackFinder::SegmentFit(Int_t First_Ch, Float_t **z_gl_, Float_t *sig, Int_t *Nbest_pair_, Int_t ** ind_best_Ch_, Double_t ***par_ab_pair_, Double_t **Chi2_ndf_pair_, Double_t ***XVU_Ch_, Double_t ***Clust_Ch_, Int_t **ind_best_pair_,Int_t **Nhits_Ch_, Int_t **Nhits_pair_) {
  int chiEl = 0;
  if (First_Ch == 2) chiEl = 1;
  if (Nbest_pair_[chiEl] >= 10) {
    printf("!!! ERROR: Nbest_pair_[%d] > 10\n", chiEl);
    return;
  }

  Double_t sigm_1[kNPlanes], sigm2_1[kNPlanes];
  Double_t sigm_2[kNPlanes], sigm2_2[kNPlanes];
  Int_t Pair1 = 0;
  if (fRunPeriod == 7 && First_Ch == 2) Pair1 = 1;

  for (Int_t bst = 0; bst < Nbest_pair_[Pair1]; bst++) {

    Int_t fir = First_Ch;
    Int_t sec = First_Ch+1;
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

    Amatr = new Double_t*[4];
    bmatr = new Double_t*[4];

    for(Int_t ii=0; ii<4; ii++) {
      Amatr[ii] = new Double_t[4];
      bmatr[ii] = new Double_t[4];
    }

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
      for(Int_t i = 0; i < 6; i++) {
        // cout<<" h1 "<<h1[i]<<" sigm2 "<< sigm2_1[i]<<" XVU_Ch1 "<<XVU_Ch_[fir][i][best1]<<endl;
        //<<" z "<<z_gl_[fir][i]<<endl;
      }
      cout<<endl;
      for(Int_t i = 0; i < 6; i++) {
        // cout<<" h2 "<<h2[i]<<" sigm2 "<< sigm2_2[i]<<" XVU_Ch2 "<<XVU_Ch_[sec][i][best2]<<endl;
        //<<" z "<<z_gl_[sec][i]<<endl;
      }
    }
    //Gaussian algorithm for 4x4 matrix inversion
    Double_t A0matr[4][4];
    for (Int_t i1 = 0; i1 < 4; i1++) {
      for (Int_t j1 = 0; j1 < 4; j1++) {
        A0matr[i1][j1] = Amatr[i1][j1];
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
        //cout<<A1[i1][j1]<<" ";
      }

    for(Int_t i1 = 0 ; i1 < 4; i1++) {
      par_ab_pair_[Pair1][i1][bst] = 0;
      for(Int_t j1 = 0; j1 < 4; j1++) {
        par_ab_pair_[Pair1][i1][bst] += bmatr[i1][j1]*matrF[j1];
        // cout<<" i1 "<<i1<<" bmatr "<<bmatr[i1][j1]<<" F "<<matrF[j1] <<endl;
      }
    } // i1

    Double_t Z0_SRC = -647.476;
    Double_t Xtarget = - par_ab_pair_[Pair1][0][bst]*( -Z0_SRC - 284.763 ) + par_ab_pair_[Pair1][1][bst] + shift_pair[Pair1][1];
    Double_t Ytarget = - par_ab_pair_[Pair1][2][bst]*( -Z0_SRC - 284.763 ) + par_ab_pair_[Pair1][3][bst] + shift_pair[Pair1][3];

    if (fDebug) {
      cout<<endl;
      cout<<" Xtarget "<<Xtarget<<" Ytarget "<<Ytarget<<endl;
      cout<<" Pair "<<Pair1<<" par [0] "<<par_ab_pair_[Pair1][0][bst]<<" par [1] "<<par_ab_pair_[Pair1][1][bst]<<" par [2] "<<par_ab_pair_[Pair1][2][bst]<<" par [3] "<<par_ab_pair_[Pair1][3][bst]<<endl;
      cout<<endl;
    }

    Float_t dx_[kNPlanes];
    Chi2_ndf_pair_[Pair1][bst] = 0;

    for(Int_t i1 = 0 ; i1 < 6; i1++) {
      dx_[i1] = 0.;

      if ( XVU_Ch_[fir][i1][best1] > -999.) {
        if(i1==0 || i1==3) dx_[i1]=XVU_Ch_[fir][i1][best1]-par_ab_pair_[Pair1][0][bst]*z_gl_[fir][i1]-par_ab_pair_[Pair1][1][bst];
        if(i1==2 || i1==5) dx_[i1]=XVU_Ch_[fir][i1][best1]-0.5*(par_ab_pair_[Pair1][0][bst]+sq3*par_ab_pair_[Pair1][2][bst])*z_gl_[fir][i1]-0.5*(par_ab_pair_[Pair1][1][bst]+sq3*par_ab_pair_[Pair1][3][bst]);
        if(i1==1 || i1==4) dx_[i1]=XVU_Ch_[fir][i1][best1]-0.5*(par_ab_pair_[Pair1][0][bst]-sq3*par_ab_pair_[Pair1][2][bst])*z_gl_[fir][i1]-0.5*(par_ab_pair_[Pair1][1][bst]-sq3*par_ab_pair_[Pair1][3][bst]);
        Chi2_ndf_pair_[Pair1][bst]= Chi2_ndf_pair_[Pair1][bst]+dx_[i1]*dx_[i1]/(sigm2_1[i1]);
        // cout<<"best1 "<<best1 <<" i1 "<<i1<<" dx_ "<<dx_[i1]<<" XVU_Ch1 "<<XVU_Ch_[fir][i1][best1]<<" Chi2_ndf_Ch1_2 "<<Chi2_ndf_pair_[Pair1][bst]<<" z_gl1 "<<z_gl_[fir][i1]<<endl;

      }// if( Wires_Ch1[i1][best2]>-1){
    }

    for(Int_t i2 = 0 ; i2 < 6; i2++) {

      if ( XVU_Ch_[sec][i2][best1] > -999.) { // if(Wires_Ch2_[i2][best2]>-1){
        if(i2==0 || i2==3) dx_[i2]=XVU_Ch_[sec][i2][best2]-par_ab_pair_[Pair1][0][bst]*z_gl_[sec][i2]-par_ab_pair_[Pair1][1][bst];
        if(i2==2 || i2==5) dx_[i2]=XVU_Ch_[sec][i2][best2]-0.5*(par_ab_pair_[Pair1][0][bst]+sq3*par_ab_pair_[Pair1][2][bst])*z_gl_[sec][i2]-0.5*(par_ab_pair_[Pair1][1][bst]+sq3*par_ab_pair_[Pair1][3][bst]);
        if(i2==1 || i2==4) dx_[i2]=XVU_Ch_[sec][i2][best2]-0.5*(par_ab_pair_[Pair1][0][bst]-sq3*par_ab_pair_[Pair1][2][bst])*z_gl_[sec][i2]-0.5*(par_ab_pair_[Pair1][1][bst]-sq3*par_ab_pair_[Pair1][3][bst]);
        Chi2_ndf_pair_[Pair1][bst]= Chi2_ndf_pair_[Pair1][bst]+dx_[i2]*dx_[i2]/(sigm2_2[i2]);
        //cout<<"best2 "<<best2 <<" i2 "<<i2<<" dx_ "<<dx_[i2]<<" XVU_Ch2 "<<XVU_Ch_[sec][i2][best2]<<" Chi2_ndf_Ch1_2 "<<Chi2_ndf_pair_[Pair1][bst]<<" z_gl2 "<<z_gl_[sec][i2]<<endl;
      }// if( Wires_Ch2[i2][best2]>-1){
    }

    if (Nhits_Ch_[fir][best1]+Nhits_Ch_[sec][best2]> 4)
      Chi2_ndf_pair_[Pair1][bst]= Chi2_ndf_pair_[Pair1][bst]/(Nhits_Ch_[fir][best1]+Nhits_Ch_[sec][best2]-4);
    if (fDebug) hChi2_ndf_pair.at(Pair1)->Fill(Chi2_ndf_pair_[Pair1][bst]);
    //if ( fDebug) cout<<" in fun Chi2_ndf_pair["<<Pair1<<"]["<<bst<<"] "<< Chi2_ndf_pair_[Pair1][bst]<<endl;

    ind_best_pair_[Pair1][bst]= bst;
    Nhits_pair_[Pair1][bst] = Nhits_Ch_[fir][best1]+Nhits_Ch_[sec][best2];
  }//< Nbest_Ch12_g_l
}//SegmentFit
//----------------------------------------------------------------------



//--------------------Matrix      Coefficients Calculation--------------
void BmnMwpcTrackFinder::FillFitMatrix(Int_t chN, Double_t** AA, Float_t** z, Double_t* sigmm2, Int_t* h_) {

  // AA - matrix to be filledlayers)
  // sigm2 - square of sigma
  // h_ - array to include/exclude planes (h_[i] = 0 or 1)
  // Float_t z2_[nPlanes];
  Float_t z2_[6] = {z[chN][0] * z[chN][0], z[chN][1] * z[chN][1], z[chN][2] * z[chN][2], z[chN][3] * z[chN][3], z[chN][4] * z[chN][4], z[chN][5] * z[chN][5]}; //cm

  AA[0][0] += 2 * z2_[0] * h_[0] / sigmm2[0]
        +   z2_[2] * h_[2] / (2 * sigmm2[2])
        +   z2_[1] * h_[1] / (2 * sigmm2[1])
        + 2 * z2_[3] * h_[3] / sigmm2[3]
        +   z2_[5] * h_[5] / (2 * sigmm2[5])
        +   z2_[4] * h_[4] / (2 * sigmm2[4]); //Ax

  AA[0][1] += 2 * z[chN][0] * h_[0] / sigmm2[0]
        +   z[chN][2] * h_[2] / (2 * sigmm2[2])
        +   z[chN][1] * h_[1] / (2 * sigmm2[1])
        + 2 * z[chN][3] * h_[3] / sigmm2[3]
        +   z[chN][5] * h_[5] / (2 * sigmm2[5])
        +   z[chN][4] * h_[4] / (2 * sigmm2[4]); //Bx

  AA[0][2] += sq3 * (z2_[2] * h_[2] / (2 * sigmm2[2])
            -     z2_[1] * h_[1] / (2 * sigmm2[1])
            +     z2_[5] * h_[5] / (2 * sigmm2[5])
            -     z2_[4] * h_[4] / (2 * sigmm2[4])); //Ay

  AA[0][3] += sq3 * (z[chN][2] * h_[2] / (2 * sigmm2[2])
            -     z[chN][1] * h_[1] / (2 * sigmm2[1])
            +     z[chN][5] * h_[5] / (2 * sigmm2[5])
            -     z[chN][4] * h_[4] / (2 * sigmm2[4])); //By

  AA[1][0] = AA[0][1];

  AA[1][1] +=  2 * h_[0] / sigmm2[0]
         + 0.5 * h_[2] / sigmm2[2] + 0.5 * h_[1] / sigmm2[1]
         +  2 * h_[3] / sigmm2[3] + 0.5 * h_[5] / sigmm2[5]
         + 0.5 * h_[4] / sigmm2[4];

  AA[1][2] += sq3 * (z[chN][2] * h_[2] / sigmm2[2]
            - z[chN][1] * h_[1] / sigmm2[1]
            + z[chN][5] * h_[5] / sigmm2[5]
            - z[chN][4] * h_[4] / sigmm2[4]) * 0.5;

  AA[1][3] += sq3 * (h_[2] / sigmm2[2]
            -     h_[1] / sigmm2[1]
            +     h_[5] / sigmm2[5]
            -     h_[4] / sigmm2[4]) * 0.5;

  AA[2][0] = AA[0][2];

  AA[2][1] = AA[1][2];

  AA[2][2] += 3.0 * (z2_[2] * h_[2] / sigmm2[2]
            +     z2_[1] * h_[1] / sigmm2[1]
            +     z2_[5] * h_[5] / sigmm2[5]
            +     z2_[4] * h_[4] / sigmm2[4]) * 0.5;

  AA[2][3] += 3.0 * (z[chN][2] * h_[2] / sigmm2[2]
            +     z[chN][1] * h_[1] / sigmm2[1]
            +     z[chN][5] * h_[5] / sigmm2[5]
            +     z[chN][4] * h_[4] / sigmm2[4])  * 0.5;

  AA[3][0] = AA[0][3];
  AA[3][1] = AA[1][3];
  AA[3][2] = AA[2][3];
  AA[3][3] += 3.0 * (0.5 * h_[2] / sigmm2[2]
            + 0.5 *    h_[1] / sigmm2[1]
            + 0.5 *    h_[5] / sigmm2[5]
            + 0.5 *    h_[4] / sigmm2[4]);

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
      //	cout<<" XVU_Ch_["<<ChN<<"]["<<i1<<"]["<< ind_best<<"] = "<<XVU_Ch_[ChN][i1][ ind_best]<<endl;
      if( XVU_Ch_[ChN][i1][ ind_best ] > -999. && Nhits_Ch[ChN][ ind_best ] == MinHits) continue;//segIdx[ChN][j]
      if (fDebug) Denom_Ch.at(ChN)->Fill(i1);
      if(XVU_Ch_[ChN][i1][ ind_best ] > -999.) {
      if (fDebug)Nomin_Ch.at(ChN)->Fill(i1);
      }
    }// i1
  }//min_distX
}// FillEfficiency
//----------------------------------------------------------------------


//----------------------------------------------------------------------
InitStatus BmnMwpcTrackFinder::Init() {
  if (!expData)
    return kERROR;
  if (fDebug) cout << "BmnMwpcTrackFinder::Init()" << endl;
  FairRootManager* ioman = FairRootManager::Instance();

  fBmnMwpcSegmentsArray = (TClonesArray*) ioman->GetObject(fInputBranchName);
  if (!fBmnMwpcSegmentsArray)
  {
    cout<<"BmnMwpcTrackFinder::Init(): branch "<<fInputBranchName<<" not found! Task will be deactivated"<<endl;
    SetActive(kFALSE);
    return kERROR;
  }

  fBmnMwpcTracksArray = new TClonesArray(fOutputBranchName.Data());
  ioman ->Register(fOutputBranchName.Data(), "MWPC", fBmnMwpcTracksArray, kTRUE);


  fMwpcGeo  = new BmnMwpcGeometrySRC(fRunPeriod, fRunNumber);
  kNChambers = fMwpcGeo -> GetNChambers();
  kNPlanes  = fMwpcGeo -> GetNPlanes(); // 6
  if (fDebug) printf("fRunPeriod: %d %d %d\n", fRunPeriod, kNChambers, kNPlanes);

  kMinHits = 4;
  kChi2_Max = 20.;
  kmaxPairs = 10;//10;//5;

  dw = fMwpcGeo->GetWireStep();//0.25; // [cm] // wires step
  dw_half = 0.5*dw;
  sq3 = sqrt(3.);
  sq12 = sqrt(12.);
  sigma = dw/sq12;
  kMiddlePl = 47.25;
  ktarget_region = 4.;
  kZ_target = -645.191;//-648.4;//cm
  kZ_DC = 488.81;
  //cout<<" kZ_target "<<kZ_target<<" kNumPairs "<<kNumPairs<<endl;

  ChCent     = new TVector3[kNChambers];
  ZCh       = new Float_t[kNChambers];
  kZmid      = new Float_t[kNChambers];
  shift      = new Float_t*[kNChambers];
  shift_pair   = new Float_t*[kNumPairs];
  kZ_midle_pair  = new Float_t[kNumPairs];
  XVU_Ch     = new Double_t**[kNChambers];
  Clust_Ch     = new Double_t**[kNChambers];
  par_ab_Ch    = new Double_t**[kNChambers];
  par_ab_pair   = new Double_t**[kNumPairs];// par_ab_Ch1_2 = new Double_t*[4];
  kPln      = new Int_t*[kNChambers];
  kZ_loc     = new Float_t*[kNChambers];
  z_gl      = new Float_t*[kNChambers];
  Chi2_match_pair = new Double_t*[kNumPairs];
  Chi2_ndf_pair  = new Double_t*[kNumPairs];
  ind_best_pair  = new Int_t*[kNumPairs]; //ind_best_Ch1_2 = new Int_t[5];
  Chi2_ndf_Ch   = new Double_t*[kNChambers];
  Nhits_Ch    = new Int_t*[kNChambers];
  Nhits_match   = new Int_t*[kNChambers];
  Nhits_pair   = new Int_t*[kNumPairs];
  sigm2      = new Float_t[kNPlanes];
  ipl       = new Int_t[kNPlanes];
  Nbest_pair   = new Int_t[kNumPairs];
  Nbest_Ch    = new Int_t[kNChambers];
  ind_best_Ch   = new Int_t*[kNChambers];
  sigma_delta   = new Float_t*[kNumPairs];
  par_ab_3ch   = new Double_t*[4];
  Nbest_3ch    = new Int_t[1];


  for(Int_t i4 = 0; i4 < 4; i4++) {
    par_ab_3ch[i4] = new Double_t[kBig];
  }
  Nbest_3ch[0] = 0;


  for(Int_t i = 0; i < kNChambers; i++) {
    TH1D *h;
    h = new TH1D(Form("par_Ax_Ch%d",i), Form("par_Ax_Ch%d",i), 100, -.4, .4);
    fList.Add(h);
    hpar_Ax_Ch.push_back(h);
    h = new TH1D(Form("par_Bx_Ch%d",i), Form("par_Bx_Ch%d",i), 100, -10., 10.0);
    fList.Add(h);
    hpar_Bx_Ch.push_back(h);
    h = new TH1D(Form("par_Ay_Ch%d",i), Form("par_Ay_Ch%d",i), 100, -.4, .4);
    fList.Add(h);
    hpar_Ay_Ch.push_back(h);
    h = new TH1D(Form("par_By_Ch%d",i), Form("par_By_Ch%d",i), 100, -10., 10.0);
    fList.Add(h);
    hpar_By_Ch.push_back(h);
    h = new TH1D(Form("Chi2_ndf_Ch%d",i), Form("Chi2_ndf_Ch%d",i), 100, 0., 20.0);
    fList.Add(h);
    hChi2_ndf_Ch.push_back(h);
    h = new TH1D(Form("Nomin_Ch%d",i), Form("Nomin_Ch%d",i), 6, 0., 6.);
    fList.Add(h);
    Nomin_Ch.push_back(h);
    h = new TH1D(Form("Denom_Ch%d",i), Form("Denom_Ch%d",i), 6, 0., 6.);
    fList.Add(h);
    Denom_Ch.push_back(h);
    h = new TH1D(Form("Efficiency_Ch%d",i), Form("Efficiency_Ch%d",i), 6, 0., 6.);
    fList.Add(h);
    Eff_Ch.push_back(h);

    shift[i]    = new Float_t[4];
    ChCent[i]    = fMwpcGeo -> GetChamberCenter(i);
    ZCh[i]     = ChCent[i].Z();
    shift[i][0]   = fMwpcGeo -> GetTx(i);
    shift[i][2]   = fMwpcGeo -> GetTy(i);
    shift[i][1]   = ChCent[i].X();
    shift[i][3]   = ChCent[i].Y();
    kPln[i]     = new Int_t[kNPlanes];
    kZ_loc[i]    = new Float_t[kNPlanes];
    z_gl[i]     = new Float_t[kNPlanes];
    XVU_Ch[i]    = new Double_t*[kNPlanes];
    Clust_Ch[i]    = new Double_t*[kNPlanes];
    par_ab_Ch[i]  = new Double_t*[4];
    Nhits_Ch[i]   = new Int_t[kBig];
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
      h = new TH1D(Form("Chi2_match_pair%d",i), Form("Chi2_match_pair%d",i), 100, 0., 100.0);
      fList.Add(h);
      hChi2_match_pair.push_back(h);
      h = new TH1D(Form("par_Ax_pair%d",i), Form("slopeX pair%d; ; Events",i), 100, -2.3, 2.3);
      fList.Add(h);
      hpar_Ax_pair.push_back(h);
      h = new TH1D(Form("par_Bx_pair%d",i), Form("posX pair%d; [cm]; Events",i), 100, -10., 10.0);
      fList.Add(h);
      hpar_Bx_pair.push_back(h);
      h = new TH1D(Form("par_Ay_pair%d",i), Form("slopeY pair%d; ; Events",i), 100, -2.3, 2.3);
      fList.Add(h);
      hpar_Ay_pair.push_back(h);
      h = new TH1D(Form("par_By_pair%d",i), Form("posY pair%d; [cm]; Events",i), 100, -10., 10.0);
      fList.Add(h);
      hpar_By_pair.push_back(h);
      h = new TH1D(Form("Chi2_ndf_pair%d",i), Form("Chi2_ndf_pair%d",i), 30, 0., 30.0);
      fList.Add(h);
      hChi2_ndf_pair.push_back(h);
      h = new TH1D(Form("Nbest_pair%d", i), Form("Nbest_pair%d; Ntracks; Events", i), 5, 0., 5.);
      fList.Add(h);
      hNbest_pair.push_back(h);
      h = new TH1D(Form("theta_pair%d",i),Form("theta_pair%d; degrees; Events",i), 160, 0., 8.);
      fList.Add(h);
      hpar_theta_pair.push_back(h);
      h = new TH1D(Form("phi_pair%d",i), Form("phi_pair%d;  degrees; Events",i), 380, -190., 190.);
      fList.Add(h);
      hpar_phi_pair.push_back(h);

      h = new TH1D(Form("dX_Zmid_pair%d",i), Form("dX_Zmid_pair%d; cm; Events",i), 100, -30.,30.);
      fList.Add(h);
      hdX_Zmid_pair.push_back(h);
      h = new TH1D(Form("dY_Zmid_pair%d",i), Form("dY_Zmid_pair%d; cm; Events",i), 100, -30.,30.);
      fList.Add(h);
      hdY_Zmid_pair.push_back(h);
      h = new TH1D(Form("dAx_Zmid_pair%d",i),Form("dAx_Zmid_pair%d; rad;Events",i), 100, -.5,.5);
      fList.Add(h);
      hdAx_Zmid_pair.push_back(h);
      h = new TH1D(Form("dAy_Zmid_pair%d",i),Form("dAy_Zmid_pair%d; rad;Events",i), 100, -.5,.5);
      fList.Add(h);
      hdAy_Zmid_pair.push_back(h);

      h = new TH1D(Form("X_in_target_pair%d",i), Form(" posX_pair%d in target;[cm]; Events ",i), 100, -10.,10.);
      fList.Add(h);
      hX_in_target_pair.push_back(h);
      h = new TH1D(Form("Y_in_target_pair%d",i), Form(" posY_pair%d in target;[cm]; Events ",i), 100, -10.,10.);
      fList.Add(h);
      hY_in_target_pair.push_back(h);

      TH2D *h1;
      h1 = new TH2D(Form("Y_X_in_target_pair%d",i), Form("posY vs posX pair%d in target; X[cm]; Y[cm]",i), 100, -6.,6., 100, -6., 6.);
      fList.Add(h1);
      hY_X_in_target_pair.push_back(h1);
      h1 = new TH2D(Form("Ax_bx_in_target_pair%d",i), Form("slopeX vs posX in target pair%d; posX[cm]; slopeX",i), 100, -10.,10., 100, -2.3, 2.3);
      fList.Add(h1);
      hAx_bx_in_target_pair.push_back(h1);
      h1 = new TH2D(Form("Ay_by_in_target_pair%d",i), Form("slopeY vs posY in target pair%d; posY[cm]; slopeY",i), 100, -10.,10., 100, -2.3, 2.3);
      fList.Add(h1);
      hAy_by_in_target_pair.push_back(h1);
    }
    Chi2_match_pair[i] = new Double_t[kmaxPairs];
    Chi2_ndf_pair[i]  = new Double_t[kmaxPairs];
    ind_best_pair[i]  = new Int_t[kmaxPairs];
    Nhits_pair[i]   = new Int_t[kmaxPairs];
    par_ab_pair[i]   = new Double_t*[4];
    shift_pair[i]   = new Float_t[4];
    sigma_delta[i]   = new Float_t[4];
  }

  if (fDebug){
    hAx_bx_in_target= new TH2D("Ax_bx_in_target", "slopeX vs posX in target; posX[cm]; slopeX", 100, -10.,10., 100, -2.3, 2.3);
    fList.Add(hAx_bx_in_target);
    hAy_by_in_target= new TH2D("Ay_by_in_target", "slopeY vs posY in target; posY[cm]; slopeY", 100, -10.,10., 100, -2.3, 2.3);
    fList.Add(hAy_by_in_target);
    hY_X_in_target = new TH2D("Y_X_in_target", "posY vs posX (pair0) in target; X[cm]; Y[cm]", 100, -10.,10., 100, -10., 10.);
    fList.Add(hY_X_in_target);

    htheta_p1vsp0 =  new TH2D("theta_p1vsp0", "theta pair1 vs pair0", 160, 0., 3., 160, 0., 3.);
    fList.Add(htheta_p1vsp0);

    hdX_pair01_vs_x1 = new TH2D("dX_pair01_vs_x1","dX(pair0- pair1)_vs_Xpair1;Xpair1[cm];dX(pair0- pair1)[cm]",100, -10.,10., 100, -10., 10.);
    fList.Add(hdX_pair01_vs_x1);
    hdY_pair01_vs_y1 = new TH2D("dY_pair01_vs_y1","dY(pair0- pair1)_vs_Ypair1;Ypair1[cm];dY(pair0- pair1)[cm]",100, -10.,10., 100, -10., 10.);
    fList.Add(hdY_pair01_vs_y1);

    hdX_pair01_inZpair1= new TH1D("dX_pair01_inZpair1","dX(pair0- pair1) inZpair1;X[cm]; ", 100, -10.,10.);
    fList.Add(hdX_pair01_inZpair1);
    hdY_pair01_inZpair1= new TH1D("dY_pair01_inZpair1","dY(pair0- pair1) inZpair1;Y[cm]; ", 100, -10.,10.);
    fList.Add(hdY_pair01_inZpair1);

    hdX_target =    new TH1D("dX_target", " PosX(pair0-pair1) in target;[cm]; Events ", 200, -10.,10.);
    fList.Add(hdX_target);
    hdY_target =    new TH1D("dY_target", " PosY(pair0-pair1) in target;[cm]; Events ", 200, -10.,10.);
    fList.Add(hdY_target);
    hdAx_target =   new TH1D("dAx_target", "SlopeX(pair0-pair1); rad; Events ", 100,  -.05, .05);
    fList.Add( hdAx_target);
    hdAy_target =   new TH1D("dAy_target", "SlopeY(pair0-pair1); rad; Events ", 100,  -.05, .05);
    fList.Add( hdAy_target);
    hChi2_m_target =  new TH1D("Chi2_m_target", "Chi2_m in target;; Events ", 100,0.,100.);
    fList.Add(hChi2_m_target);

    hpar_Ax_3ch =  new TH1D("par_Ax_3ch", " tgx by 3ch-s to DC; ; Events ", 100, -0.05, 0.05);
    fList.Add(hpar_Ax_3ch);
    hpar_Bx_3ch =  new TH1D("par_Bx_3ch", " pos x by 3ch-s to DC; [cm]; Events ", 100, -30., 30.);
    fList.Add(hpar_Bx_3ch);
    hpar_Ay_3ch =  new TH1D("par_Ay_3ch", " tgy by 3ch-s to DC; ; Events ", 100, -0.05, 0.05);
    fList.Add(hpar_Ay_3ch);
    hpar_By_3ch =  new TH1D("par_By_3ch", " pos y by 3ch-s to DC; [cm]; Events ", 100, -30., 30.);
    fList.Add(hpar_By_3ch);

    hChi2best_Chi2fake_before_target = new TH2D("Chi2best_Chi2fake_before_target","Chi2best_Chi2fake_before_target; Chi2_best; Chi2_second", 20, 0., 20., 20, 0., 20.);
    fList.Add(hChi2best_Chi2fake_before_target);
    hChi2best_Chi2fake_after_target = new TH2D("Chi2best_Chi2fake_after_target","Chi2best_Chi2fake_after_target; Chi2_best; Chi2_second", 100, 0., 100., 100, 0., 100.);
    fList.Add(hChi2best_Chi2fake_after_target);

    hdX_Zmid_pair_1=  new TH1D("dX_Zmid_pair_1","dX_Zmid_pair_1", 200, -20.,20.);
    fList.Add(hdX_Zmid_pair_1);
    hdY_Zmid_pair_1 =  new TH1D("dY_Zmid_pair_1","dY_Zmid_pair_1", 200, -20.,20.);
    fList.Add(hdY_Zmid_pair_1);
    hdAx_Zmid_pair_1=  new TH1D("dAx_Zmid_pair_1","dAx_Zmid_pair_1",100,  -.3, .3);
    fList.Add(  hdAx_Zmid_pair_1);
    hdAy_Zmid_pair_1=  new TH1D("dAy_Zmid_pair_1","dAy_Zmid_pair_1",100,  -.3, .3);
    fList.Add(  hdAy_Zmid_pair_1);
    hChi2m_pair_1  =  new TH1D("Chi2m_pair_1","Chi2m_pair_1", 100, 0, 100);
    fList.Add(hChi2m_pair_1);
    hXv_pair_1  =  new TH1D("Xv_pair_1","Xv_pair_1",200, -10.,10.);
    fList.Add(hXv_pair_1);
    hYv_pair_1  =  new TH1D("Yv_pair_1","Yv_pair_1",200, -10.,10.);
    fList.Add(hYv_pair_1);
  }
  Int_t i1 = 0;
  for(Int_t i = 0; i < kNumPairs; i++) {

    i1=i;
    sigma_delta[0][0] = 3*.14;//.0624;// sigm_dax
    sigma_delta[0][2] = 3*.14;//.066; // sigm_day
    sigma_delta[0][1] = 2*.35;//4.08;// sigm_dx
    sigma_delta[0][3] = 2*.35;//4.30;// sigm_dy

    if (i == 1) {
      i1=2;
      sigma_delta[0][0] = 3*.14;
      sigma_delta[1][0] = 3*.09;// sigm_dax //VP
      sigma_delta[0][2] = 3*.14;
      sigma_delta[1][2] = 3*.09;// sigm_day
      sigma_delta[0][1] = 2*.35;
      sigma_delta[1][1] = 6.;//4.08;//2*.35;//4.08;// sigm_dx
      sigma_delta[0][3] = 2*.35;
      sigma_delta[1][3] = 2*7.;//shift +10cm?  4.30;// 2*.35;//4.30;// sigm_dy
    }
    kZ_midle_pair[i] = ZCh[i1] + kZmid[i1+1];

    shift_pair[i][0]= (shift[i1+1][1] - shift[i1][1])/( ZCh[i1+1] - ZCh[i1] );
    shift_pair[i][2]= (shift[i1+1][3] - shift[i1][3])/( ZCh[i1+1] - ZCh[i1] );
    shift_pair[i][1]= 0.5*(shift[i1+1][1] + shift[i1][1]);
    shift_pair[i][3] = 0.5*(shift[i1+1][3] + shift[i1][3]);

    if (fDebug) cout<<" i "<<i<<" kZ_midle_pair[i] "<<kZ_midle_pair[i]<<" i1 "<<i1<<" i1+1 "<<i1+1<<" -( ZCh[i1]- ZCh[i1+1] )= "<<-( ZCh[i1]- ZCh[i1+1] )<<endl;
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

      if ( fRunPeriod == 7 ) {
        if (ichh == 0 || ichh == 1) {
          kZ_loc[0][0] = -1.5;
          kZ_loc[0][1] = -0.5;
          kZ_loc[0][2] = 0.5;
          kZ_loc[0][3] = 1.5;
          kZ_loc[0][4] = 2.5;
          kZ_loc[0][5] = -2.5;

          kZ_loc[1][0] = -1.5;
          kZ_loc[1][1] = -2.5;
          kZ_loc[1][2] = 2.5;
          kZ_loc[1][3] = 1.5;
          kZ_loc[1][4] = 0.5;
          kZ_loc[1][5] = -0.5;
        }
      }
      if ( ichh == 2 || ichh == 3) {
        kZ_loc[ichh][ii] = -0.5 + ii;
        if(ii == 4) {
          kZ_loc[ichh][ii] = -2.5;
        }
        if(ii == 5) {
          kZ_loc[ichh][ii] = -1.5;
        }
      }//if ( ich == 0 || ich == 1)

      z_gl[ichh][ii] = kZmid[ichh] + kZ_loc[ichh][ii];

      // if (fDebug)	cout<<" ich "<<ichh<<" ii "<<ii<<" kZ_loc "<<kZ_loc[ichh][ii]<<" z_gl "<<z_gl[ichh][ii]<<endl;
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
      for(Int_t jj=0; jj<100; jj++) {
        par_ab_3ch[ii][jj] = 999.;
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

  Nbest_3ch[0] = 0;

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
      Eff_Ch.at(iCh)->Divide(Nomin_Ch.at(iCh),Denom_Ch.at(iCh),100,1);
    }
    fList.Write();
  }
  if (fDebug) printf("done\n");

  cout << "Work time of the MWPC track finder: " << workTime << " s" << endl;
}//Finish
//----------------------------------------------------------------------

ClassImp(BmnMwpcTrackFinder)
