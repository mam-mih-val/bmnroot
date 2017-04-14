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
    
    Double_t scale;  //scaling for transfer function coeffs
    
    Double_t z_loc[8];   // z local xa->vb (cm)
    Double_t z_glob[16]; // z global dc 1 & dc 2 (cm)
    Double_t Z_dch1; //z coord in the middle of dch1	
    Double_t Z_dch2; //z coord in the middle of dch2
    Double_t Z_dch_mid; //z coord between the two chambers, this z is considered the global z for the matched dch-segment
    Double_t dZ_dch_mid; 
    Double_t dZ_dch;
    
    Double_t x1_sh; 
    Double_t x2_sh; 
    Double_t y1_sh;
    Double_t y2_sh;
    
    Double_t x1_slope_sh;
    Double_t y1_slope_sh; 
    Double_t x2_slope_sh;
    Double_t y2_slope_sh; 
    
    Bool_t* has7DC;
    
    Int_t nSegmentsToBeMatched;
    
    Double_t** x_global;
    Double_t** y_global;
    Double_t** Chi2;
        
    Double_t*** v;
    Double_t*** u;
    Double_t*** y;
    Double_t*** x;
    Double_t*** v_Single;
    Double_t*** u_Single;
    Double_t*** y_Single;
    Double_t*** x_Single;
    Double_t*** sigm_v;
    Double_t*** sigm_u;
    Double_t*** sigm_y;
    Double_t*** sigm_x;
    Double_t*** Sigm_v_single;
    Double_t*** Sigm_u_single;
    Double_t*** Sigm_y_single;
    Double_t*** Sigm_x_single;
    Double_t*** params;
    Double_t*** rh_segment;
    Double_t*** rh_sigm_segment;
    
    Int_t* nSegments;
    Int_t** pairs;
    Int_t** segment_size;
    Int_t*** singles;
    
    Double_t* x_mid; //x glob of matched segment in the z situated between the two DCH chambers
    Double_t* y_mid; //y glob of matched segment in the z situated between the two DCH chambers
    Double_t* aX; //x slope of the matched segment
    Double_t* aY; //y slope of the matched segment
    Double_t* imp; //reconstructed particle trajectory momentum 
    Double_t* leng; //the distance from z = 0 to the global point of the matched segment
    Double_t* Chi2_match; //chi2 of the matched seg
      
    void CreateDchTrack();
    void CreateDchTrack(Int_t, Double_t*, Double_t**, Int_t*);
    void SelectLongestAndBestSegments(Int_t, Int_t*, Double_t**, Double_t*);
    void CompareDaDb(Double_t, Double_t&);
    void CompareDaDb(Double_t, Double_t&, Double_t&);
    void PrepareArraysToProcessEvent();
    Bool_t FitDchSegments(Int_t, Int_t*, Double_t**, Double_t**, Double_t**, Double_t*, Double_t*, Double_t*);
    Double_t CalculateResidual(Int_t, Int_t, Double_t**, Double_t**);
    Int_t BuildUVSegments(Int_t, Int_t, Int_t, Int_t, Int_t, Int_t, Int_t, Int_t, Int_t,
        Double_t**, Double_t**, Double_t**, Double_t**,
        Double_t**, Double_t**, Double_t**, Double_t**,
        Double_t**, Double_t**, Double_t**,
        Double_t**, Double_t**, Double_t**);
    Int_t BuildXYSegments(Int_t, Int_t, Int_t, Int_t, Int_t, Int_t, Int_t, Int_t, Int_t,
        Double_t**, Double_t**, Double_t**, Double_t**,
        Double_t**, Double_t**, Double_t**, Double_t**,
        Double_t**, Double_t**, Double_t**,
        Double_t**, Double_t**, Double_t**);
    Int_t Reconstruction(Int_t, TString, Int_t, Int_t, Int_t,
        Double_t*, Double_t*, Double_t*, Double_t*,
        Bool_t*, Bool_t*,
        Double_t**, Double_t**);
    Int_t ReconstructionSingle(Int_t, TString, TString, Int_t, Int_t,
        Double_t*, Double_t*, Bool_t*,
        Double_t**, Double_t**);
    void SegmentsToBeMatched();
         
    ClassDef(BmnDchTrackFinder, 1)
};

#endif
