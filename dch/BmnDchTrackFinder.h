#ifndef BMNDCHTRACKFINDER_H
#define BMNDCHTRACKFINDER_H 1

#include <fstream>

#include <TMath.h>
#include <TNamed.h>
#include <TString.h>
#include <TClonesArray.h>
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
    
    Float_t scale[16];  //scaling for transfer function coeffs
    
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
     
  
    void fit_seg(Float_t*, Float_t*, Float_t*, Int_t, Int_t);
    void CreateDchTrack(Int_t, Int_t, Float_t*, Float_t**, Int_t*);
    void AssignTimesToWires(Short_t, Double_t, Int_t, Double_t*, Double_t*, Bool_t);
         
    ClassDef(BmnDchTrackFinder, 1)
};

#endif