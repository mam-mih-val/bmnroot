// Author: Vasilisa Lenivenko <vasilisa@jinr.ru> 2018-07-18

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// BmnMwpcTrackFinder                                                         //
//                                                                            //
// Vasilisa Lenivenko  and Vladimir Palchik                                   //
// The algorithm serves for searching for track segments                      //
// in the MWPC of the BM@N experiment                                         //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef BMNMWPCTRACKFINDER_H
#define BMNMWPCTRACKFINDER_H

#include "BmnMwpcTrack.h"
#include "BmnMwpcHit.h"
#include "BmnMwpcGeometrySRC.h"
#include "BmnEnums.h"
#include "BmnMath.h"

#include "FairTask.h"

#include <TMath.h>
#include <TClonesArray.h>
#include <TString.h>
#include "TH1D.h"
#include "TH2D.h"

#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>

using namespace std;
using namespace TMath;

class BmnMwpcTrackFinder : public FairTask
{
  public:
    BmnMwpcTrackFinder() {}
    BmnMwpcTrackFinder(Bool_t isExp, Int_t runPeriod, Int_t runNumber);
    virtual ~BmnMwpcTrackFinder();
    virtual InitStatus Init();
    virtual void Exec(Option_t* opt);
    virtual void Finish();
    
    struct MC_points{
      Double_t x2[6] = {-999., -999.,-999.,-999.,-999.,-999.};
      Double_t y2[6] = {-999., -999.,-999.,-999.,-999.,-999.};
      Double_t z2[6] = {-999., -999.,-999.,-999.,-999.,-999.};
      Double_t x3[6] = {-999., -999.,-999.,-999.,-999.,-999.};
      Double_t y3[6] = {-999., -999.,-999.,-999.,-999.,-999.};
      Double_t z3[6] = {-999., -999.,-999.,-999.,-999.,-999.};
      
      Bool_t xWas2=0;
      Bool_t uWas2=0;
      Bool_t vWas2=0;
      Bool_t xWas3=0;
      Bool_t uWas3=0;
      Bool_t vWas3=0;
      
      Double_t param[4]  = {999., 999., 999., 999.};
      Double_t param2[4] = {999., 999., 999., 999.};
      Double_t param3[4] = {999., 999., 999., 999.};
      Int_t  Np2 = 0;
      Int_t  Np3 = 0;
      Int_t  Np  = 0;
      Int_t  Id  = -1;
    };

  private:
    Bool_t expData;
    Bool_t fDebug = 0;
    UInt_t fEventNo; // event counter
    Int_t  fRunPeriod;
    Int_t  fRunNumber;

    TString fInputBranchName;
    TString fInputBranchNameHit;
    TString fOutputBranchName;
    TString fOutputFileName;

    /** Input array of MWPC hits **/
    TClonesArray* fBmnMwpcSegmentsArray;    //!
    TClonesArray* fBmnHitsArray;            //!
    /** Output array of MWPC tracks **/
    TClonesArray* fBmnMwpcTracksArray;      //!

    BmnMwpcGeometrySRC* fMwpcGeo;           //!
    TList fList;

    vector<TH1D*> hpar_Ax_Ch, hpar_Bx_Ch, hpar_Ay_Ch, hpar_By_Ch, hChi2_match_pair, hChi2_ndf_Ch, hpar_Ax_pair, hpar_Bx_pair, hpar_Ay_pair, hpar_By_pair, hpar_theta_pair, hpar_phi_pair, 
    Nomin_Ch, Denom_Ch, Eff_Ch, hX_in_target_pair, hY_in_target_pair, hdX_pair, hdY_pair, hdAx_pair, hdAy_pair, 
    hoccupancyXp, hoccupancyUp, hoccupancyVp,hoccupancyXm, hoccupancyUm, hoccupancyVm, hNbest_pair, hChi2_ndf_pair;

    vector<TH2D*> hAx_bx_in_target_pair, hAy_by_in_target_pair, hY_X_in_target_pair, hYvsX_pair ;
    TH1D *hdX_target, *hdY_target, *hX_in_target, *hY_in_target, *hdAx_target, *hdAy_target, *hChi2_m_target, *hdX_pair01_inZpair1, *hdY_pair01_inZpair1,
         *hdX_pair_1,*hdY_pair_1,*hdAx_pair_1,*hdAy_pair_1,*hChi2m_pair_1,*hXv_pair_1  ,*hYv_pair_1,
         *hDen, *hNum, *hEff,
         *hDen_mctr, *hNum_mctr, *hEff_mctr,
         *hdAx_tr_mc, *hdX_tr_mc, *hdAy_tr_mc, *hdY_tr_mc, *hNtrpc_reco, *hNtrpc_mc;    //!
    TH2D *hNtrpc_mc_vs_reco;    //!
    TH2D *hAx_bx_in_target, *hAy_by_in_target, *hY_X_in_target, *hdX_pair01_vs_x1, *hdY_pair01_vs_y1, *htheta_p1vsp0, *hChi2best_Chi2fake_after_target, *hChi2best_Chi2fake_before_target, *hYvsX_mc_pair;  //!

    Short_t kNChambers;
    Short_t kNPlanes;
    Int_t kBig;
    Int_t kCh_min;
    Int_t kCh_max;
    Int_t kNumPairs;
    TVector3 *ChCent;               //!
    vector<MC_points> vec_points;
    Double_t Z0_SRC;
    Double_t ktarget_region;
    Double_t ktarget_regionY;
    Double_t kX_target;
    Double_t kY_target;
    Float_t *kZmid;                 //!
    Float_t *ZCh;                   //!
    Float_t **kZ_loc;               //!
    Float_t *kZ_midle_pair;         //!

    Float_t **z_gl;                 //!
    Float_t **shift;                //!
    Float_t **shift_pair;           //!

    Float_t kZ_to_pole;
    Float_t kZ_target;
    Int_t kMinHits;
    Int_t kmaxPairs;
    Double_t kChi2_Max;

    Float_t dw;
    Float_t dw_half;
    Double_t sq3;
    Double_t sq2;
    Double_t sq12;
    Double_t sigma;
    Short_t kMiddlePl;

    Int_t **kPln;                   //!
    Float_t *sigm2;                 //!
    Float_t **sigma_delta;          //!
    Int_t *ipl;                     //!
    Double_t **matrA;               //!
    Double_t **matrb;               //!
    Double_t **Amatr;               //!
    Double_t **bmatr;               //!

    Double_t ***par_ab_Ch;          //!
    Double_t ***par_ab_pair;        //!
    Double_t****sigma2_pair;        //!
    Double_t ***XVU_Ch;             //!
    Double_t ***Clust_Ch;           //!
    Double_t **Chi2_match_pair;     //!
    Double_t **Chi2_ndf_pair;       //!
    Double_t **Chi2_ndf_Ch;         //!
    Int_t **ind_best_pair;          //!
    Int_t **ind_best_Ch;            //!
    Int_t **Nhits_match;            //!
    Int_t *Nbest_pair;              //!
    Int_t *Nbest_Ch;                //!
    Int_t **Nhits_Ch;               //!
    Int_t **Nhits_pair;             //!
    
    void ReadSegments(Double_t ***, Int_t **, Double_t **, Int_t *, Double_t ***, Double_t ***, vector<MC_points>&);
    void PrepareArraysToProcessEvent();
    void SegmentParamAlignment(Int_t, Int_t *,  Double_t ***, Float_t **);
    void SegmentMatching(Int_t,Int_t *, Double_t ***, Float_t *, Int_t **,  Int_t *,Double_t **,Double_t ***, Int_t **, Int_t **);
    void SegmentMatchingAfterTarget(Int_t,Int_t *, Double_t ***, Float_t *, Int_t **,  Int_t *,Double_t **,Double_t ***, Int_t **, Int_t **);
    void SegmentFit(Int_t, Float_t **, Float_t *,Int_t *, Int_t **, Double_t ***,  Double_t **, Double_t ***, Double_t ***, Int_t **,Int_t **, Int_t **, Double_t ****);
    void FillFitMatrix(Int_t, Double_t** , Float_t** , Double_t* , Int_t*);
    void FillFreeCoefVector(Int_t, Double_t*, Double_t***, Int_t, Float_t**, Double_t*, Int_t*);
    void InverseMatrix(Double_t**, Double_t**);
    void PairMatching( Int_t *, Double_t ***,  Float_t *);
    void FillEfficiency(Int_t, Double_t ***, Int_t **, Int_t, Int_t, Float_t, Float_t);
    void StraightByTwoPoints(Double_t ***, Int_t *, Double_t ***, Int_t *, Float_t *, Double_t **, Int_t *);
    void MCefficiencyCalculation(vector<MC_points>&, Int_t *, Double_t ***);
    void TracksStoring(Int_t *, Double_t ***, Double_t **, Int_t **, Double_t****);

    const int kMaxMC      = 100;

  ClassDef(BmnMwpcTrackFinder, 1)
};

#endif
