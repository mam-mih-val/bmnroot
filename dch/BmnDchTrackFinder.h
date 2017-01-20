#ifndef BMNDCHTRACKFINDER_H
#define BMNDCHTRACKFINDER_H 1

#include <fstream>

#include <TMath.h>
#include <TNamed.h>
#include <TString.h>
#include <TClonesArray.h>
#include <TVector2.h>
#include  "FairTask.h"
#include  "FairTrackParam.h"

#include "BmnDchDigit.h"
#include "BmnDchTrack.h"

using namespace std;
using namespace TMath;

class BmnDchTrackFinder : public FairTask {
public:

    BmnDchTrackFinder();
    virtual ~BmnDchTrackFinder();
    
    virtual InitStatus Init();

    virtual void Exec(Option_t* opt);

    virtual void Finish();
    
    void SetSegmentMatching(Bool_t flag) {
        fSegmentMatching = flag;
    }
    
     
private:
    UInt_t fEventNo; // event counter
    
    TString InputDigitsBranchName;
    TString tracksDch;
    
    TClonesArray* fBmnDchDigitsArray;
    TClonesArray* fDchTracks;    
    
    Int_t prev_wire;
    Int_t prev_time;
    
    Double_t t_dc[5][16]; //[time interval][plane number]
    Double_t pol_par_dc[3][5][16]; //[polinom number][param number][plane number]
    Double_t scaling[16]; //[plane number]
    
    Float_t scale;  //scaling for transfer function coeffs
    
    Float_t z_loc[8];   // z local xa->vb (cm)
    Float_t z_glob[16]; // z global dc 1 & dc 2 (cm)
    Float_t Z_dch1; //z coord in the middle of dch1	
    Float_t Z_dch2; //z coord in the middle of dch2
    Float_t Z_dch_mid; //z coord between the two chambers, this z is considered the global z for the matched dch-segment
    
    Float_t x1_sh; 
    Float_t x2_sh; 
    Float_t y1_sh;
    Float_t y2_sh;
    
    Float_t x1_slope_sh;
    Float_t y1_slope_sh; //was .05
    Float_t x2_slope_sh;
    Float_t y2_slope_sh; //
    
    Bool_t fSegmentMatching; 
    
    Float_t** par_ab1;
    Float_t** par_ab2;
    
    Float_t* chi2_DC1;
    Float_t* chi2_DC2;
    Int_t* size_segDC1;
    Int_t* size_segDC2;
    
    Float_t** rh_segDC1;
    Float_t** rh_segDC2;
    
    Float_t* xDC1_glob;
    Float_t* yDC1_glob;
    Float_t* xDC2_glob;
    Float_t* yDC2_glob;
    
    Float_t** rh_sigm_segDC1;
    Float_t** rh_sigm_segDC2;
       
    Float_t** x1_ab;
    Float_t** y1_ab;
    Float_t** u1_ab;
    Float_t** v1_ab;
    Float_t** sigm_x1_ab;
    Float_t** sigm_y1_ab;
    Float_t** sigm_u1_ab;
    Float_t** sigm_v1_ab;
    Float_t** x2_ab;
    Float_t** y2_ab;
    Float_t** u2_ab;
    Float_t** v2_ab;
    Float_t** sigm_x2_ab;
    Float_t** sigm_y2_ab;
    Float_t** sigm_u2_ab;
    Float_t** sigm_v2_ab;

    //single hits on ab-plane
    Float_t** x1_single;
    Float_t** y1_single;
    Float_t** u1_single;
    Float_t** v1_single;
    Float_t** sigm_x1_single;
    Float_t** sigm_y1_single;
    Float_t** sigm_u1_single;
    Float_t** sigm_v1_single;
    Float_t** x2_single;
    Float_t** y2_single;
    Float_t** u2_single;
    Float_t** v2_single;
    Float_t** sigm_x2_single;
    Float_t** sigm_y2_single;
    Float_t** sigm_u2_single;
    Float_t** sigm_v2_single;
    
    Bool_t has7DC1;
    Bool_t has7DC2;
    
    Int_t nDC1_segments;
    Int_t nDC2_segments;
    
    Int_t pair_x2;
    Int_t pair_y2;
    Int_t pair_u2;
    Int_t pair_v2;
    Int_t single_xa2;
    Int_t single_ya2;
    Int_t single_ua2;
    Int_t single_va2;
    Int_t single_xb2;
    Int_t single_yb2;
    Int_t single_ub2;
    Int_t single_vb2;
    
    Int_t pair_x1;
    Int_t pair_y1;
    Int_t pair_u1;
    Int_t pair_v1;
    Int_t single_xa1;
    Int_t single_ya1;
    Int_t single_ua1;
    Int_t single_va1;
    Int_t single_xb1;
    Int_t single_yb1;
    Int_t single_ub1;
    Int_t single_vb1;
  
    void fit_seg(Float_t*, Float_t*, Float_t*, Int_t, Int_t);
    void CreateDchTrack(Int_t, Float_t*, Float_t**, Int_t*);
    void AssignTimesToWires(Short_t, Double_t, Int_t, Double_t*, Double_t*, Bool_t);
    void SelectLongestAndBestSegments(Int_t, Int_t*, Float_t**, Float_t*);
    void CompareDaDb(Float_t, Float_t&);
    void CompareDaDb(Float_t, Float_t&, Float_t&);
    void PrepareArraysToProcessEvent();
    void FitDchSegments(Int_t, Int_t*, Float_t**, Float_t**, Float_t**, Float_t*, Float_t*, Float_t*);
    Float_t CalculateResidual(Int_t, Int_t, Float_t**, Float_t**);
    Int_t BuildUVSegments(Int_t, Int_t, Int_t, Int_t, Int_t, Int_t, Int_t, Int_t, Int_t,
        Float_t**, Float_t**, Float_t**, Float_t**,
        Float_t**, Float_t**, Float_t**, Float_t**,
        Float_t**, Float_t**, Float_t**,
        Float_t**, Float_t**, Float_t**);
    Int_t BuildXYSegments(Int_t, Int_t, Int_t, Int_t, Int_t, Int_t, Int_t, Int_t, Int_t,
        Float_t**, Float_t**, Float_t**, Float_t**,
        Float_t**, Float_t**, Float_t**, Float_t**,
        Float_t**, Float_t**, Float_t**,
        Float_t**, Float_t**, Float_t**);
    Int_t Reconstruction(Int_t, TString, Int_t, Int_t, Int_t,
        Double_t*, Double_t*, Double_t*, Double_t*,
        Bool_t*, Bool_t*,
        Float_t**, Float_t**);
    Int_t ReconstructionSingle(Int_t, TString, TString, Int_t, Int_t,
        Double_t*, Double_t*, Bool_t*,
        Float_t**, Float_t**);
         
    ClassDef(BmnDchTrackFinder, 1)
};

#endif