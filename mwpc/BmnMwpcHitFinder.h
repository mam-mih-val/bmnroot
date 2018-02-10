// @(#)bmnroot/mwpc:$Id$
// Author: Pavel Batyuk (VBLHEP) <pavel.batyuk@jinr.ru> 2017-02-10

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// BmnMwpcHitFinder                                                          //
//                                                                            //
// Implementation of an algorithm developed by                                // 
// S. Merts and P. Batyuk                                                     //
// to the BmnRoot software                                                    //
//                                                                            //
// The algorithm serves for searching for hits                                //
// in the Multi Wire Prop. Chambers of the BM@N experiment                    //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef BMNMWPCHITFINDER_H
#define	BMNMWPCHITFINDER_H

#include <map>
#include <algorithm>
#include <Rtypes.h>
#include <TClonesArray.h>
#include <TVector3.h>
#include <TMath.h>
#include <TString.h>
#include  "FairTask.h"
#include  "BmnMwpcHit.h"
#include  "BmnMwpcDigit.h"
#include  "BmnMwpcGeometry.h"
#include  "FairTask.h"

using namespace std;

class BmnMwpcHitFinder : public FairTask {
public:

    /** Default constructor **/
    BmnMwpcHitFinder() {};
    
    /** Constructor to be used **/
    BmnMwpcHitFinder(Bool_t);

    /** Destructor **/
    virtual ~BmnMwpcHitFinder();

    /** Virtual method Init **/
    virtual InitStatus Init();

    /** Virtual method Exec **/
    virtual void Exec(Option_t* opt);

    /** Virtual method Finish **/
    virtual void Finish();
    
    void SetUseDigitsInTimeBin(Bool_t flag) {
        fUseDigitsInTimeBin = flag;
    }

private:
    Bool_t expData;
    UInt_t fEventNo; // event counter
    
    TString fInputBranchName;
    TString fOutputBranchName;
    
    /** Input array of MWPC digits **/
    TClonesArray* fBmnMwpcDigitArray;
    
    /** Output array of MWPC hits **/
    TClonesArray* fBmnMwpcHitArray; 
    
    vector <BmnMwpcDigit*> CheckDigits(vector <BmnMwpcDigit*>);
    void FindNeighbour(BmnMwpcDigit*, vector <BmnMwpcDigit*>, vector<BmnMwpcDigit*>);
    vector <TVector3> CreateHitsBy3Planes(vector <BmnMwpcDigit*>, vector <BmnMwpcDigit*>, vector <BmnMwpcDigit*>, Float_t);
    TVector3 CalcHitPosByTwoDigits(BmnMwpcDigit*, BmnMwpcDigit*);
    void CreateMwpcHits(vector <TVector3>, TClonesArray*, Short_t);
    void DefineCoordinateAngle(Short_t, Double_t&, Double_t&);
    void FindPairs(vector <BmnMwpcDigit*>, vector <BmnMwpcDigit*>, vector <TVector3>&);
    
    Float_t thDist;      // distance between found hits [cm]
    Int_t nInputDigits;  // max. number of found digits per plane
    Int_t nTimeSamples;  // 
      
    Bool_t fUseDigitsInTimeBin; // use digits found in a time bin of width = kTimeBin (8 ns).
    
    BmnMwpcGeometry* fMwpcGeometry;

    Short_t kNChambers;
    Short_t kNPlanes;
    Short_t kNWires;

    Int_t kMinHits;
    Double_t kChi2_Max;

    Float_t kX1_sh;
    Float_t kY1_sh;
    Float_t kX1_slope_sh;
    Float_t kY1_slope_sh;

    Float_t kX2_sh;
    Float_t kY2_sh;
    Float_t kX2_slope_sh;
    Float_t kY2_slope_sh;

    Float_t dw;
    Float_t dw_half;
    Double_t sq3;
    Double_t sq12;
    Double_t sigma;

    Int_t Nbest_Ch1;
    Int_t Nbest_Ch2;
    Int_t Nseg_Ch1;
    Int_t Nseg_Ch2;
    
    Int_t kBig;

    Int_t *kPln;
    Float_t *kZ1_loc;
    Float_t *kZ2_loc;

    Int_t *iw;
    Int_t *iw_Ch1;
    Int_t *iw_Ch2;

    Int_t **wire_Ch1;
    Int_t **wire_Ch2;
    Float_t **xuv_Ch1;
    Float_t **xuv_Ch2;

    Int_t *ind_best_Ch1;
    Int_t *ind_best_Ch2;    
    Int_t **Wires_Ch1;
    Int_t **Wires_Ch2;    
    Int_t **clust_Ch1;
    Int_t **clust_Ch2;
    Float_t **XVU_Ch1;
    Float_t **XVU_Ch2;    
    Int_t *Nhits_Ch1;
    Int_t *Nhits_Ch2;    

    Double_t *Chi2_ndf_Ch1;
    Double_t **par_ab_Ch1;
    Double_t *Chi2_ndf_Ch2;
    Double_t **par_ab_Ch2;
    Double_t *Chi2_ndf_best_Ch1;
    Double_t *Chi2_ndf_best_Ch2;    

    Double_t **A;
    Float_t *sigm2;
    Int_t *h;
    Int_t *h6;
    Int_t *ipl;
    Float_t *XVU;
    Float_t *XVU_cl;
    Double_t **b;
    Float_t *dX_i;
    Float_t *z2;

    //functions for Vasilisa method:
    void PrepareArraysToProcessEvent();
    //    void SegmentFinder(Int_t, Int_t**, Int_t**, Float_t**, Int_t*, Int_t*, Int_t&, Int_t**, Float_t**, Int_t, Short_t, Int_t);
    void SegmentFinder(Int_t, Int_t**, Int_t**, Float_t**, Int_t);
    void ProcessSegments(Int_t, Double_t, Float_t, Float_t*, Int_t, Int_t, Int_t*, Int_t**, Int_t**, Float_t**, Int_t, Int_t*,  Double_t*, Double_t*, Double_t**, Double_t**, Int_t, Int_t*, Float_t*, Float_t*, Double_t, Float_t*);
    void FillFitMatrix(Double_t**, Float_t*, Float_t*, Int_t*, Int_t, Float_t*);
    void FillFreeCoefVector(Double_t*, Float_t*, Float_t*, Float_t*, Int_t*, Int_t);
    void InverseMatrix(Double_t**, Double_t**);
    
    ClassDef(BmnMwpcHitFinder, 1);

};

#endif	

