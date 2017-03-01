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

#ifndef BMNDCHTRACKFINDER_H
#define BMNDCHTRACKFINDER_H 1

#include <fstream>

#include <TMath.h>
#include <TNamed.h>
#include <TString.h>
#include <TClonesArray.h>
#include <TVector2.h>
#include <Rtypes.h>
#include  "FairTask.h"
#include  "FairTrackParam.h"

#include "BmnDchDigit.h"
#include "BmnDchTrack.h"
#include "BmnMath.h"

using namespace std;
using namespace TMath;

class BmnDchTrackFinder : public FairTask {
public:

    BmnDchTrackFinder() {};
    BmnDchTrackFinder(Bool_t);
    virtual ~BmnDchTrackFinder();
    
    virtual InitStatus Init();

    virtual void Exec(Option_t* opt);

    virtual void Finish();
    
    void SetSegmentMatching(Bool_t flag) {
        fSegmentMatching = flag;
    }
    
    void SetTransferFunction(TString name) {
        fTransferFunctionName = name;
    }
     
private:
    Int_t N;
    Bool_t expData;
    UInt_t fEventNo; // event counter
    
    TString InputDigitsBranchName;
    TString tracksDch;
    
    TString fTransferFunctionName;
    
    TClonesArray* fBmnDchDigitsArray;
    TClonesArray* fDchTracks;    
    
    Int_t prev_wire;
    Int_t prev_time;
    
    Int_t nChambers; // dch1 or dch2
    Int_t nWires;    // corresponding to the found v(0), u(1), y(2), x(3)-pairs
    Int_t nLayers;   // a(0) or b(1)
    Int_t nSegmentsMax; // Max. number of found segments to store
    
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
    Bool_t has7DC1;
    Bool_t has7DC2;
    
    Float_t** x_global;
    Float_t** y_global;
    Float_t** Chi2;
        
    Float_t*** v;
    Float_t*** u;
    Float_t*** y;
    Float_t*** x;
    Float_t*** v_Single;
    Float_t*** u_Single;
    Float_t*** y_Single;
    Float_t*** x_Single;
    Float_t*** sigm_v;
    Float_t*** sigm_u;
    Float_t*** sigm_y;
    Float_t*** sigm_x;
    Float_t*** Sigm_v_single;
    Float_t*** Sigm_u_single;
    Float_t*** Sigm_y_single;
    Float_t*** Sigm_x_single;
    Float_t*** params;
    Float_t*** rh_segment;
    Float_t*** rh_sigm_segment;
    
    Int_t* nSegments;
    Int_t** pairs;
    Int_t** segment_size;
    Int_t*** singles;
  
    void CreateDchTrack(Int_t, Float_t*, Float_t**, Int_t*);
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
