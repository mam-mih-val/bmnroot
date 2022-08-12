//Author: Vasilisa Lenivenko <vasilisa@jinr.ru> 2018-07-18

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// BmnMwpcHitFinder                                                           //
//                                                                            //
// Implementation of an algorithm developed by                                //
// Vasilisa Lenivenko  and Vladimir Palchik                                   //
// to the BmnRoot software                                                    //
//                                                                            //
// The algorithm serves for searching for hits                                //
// in the Multi Wire Prop. Chambers of the BM@N experiment                    //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef BMNMWPCHITFINDER_H
#define	BMNMWPCHITFINDER_H

#include <BmnEventQuality.h>
#include "BmnMwpcHit.h"
#include "BmnMwpcDigit.h"
#include "BmnMwpcGeometrySRC.h"
#include "BmnTrack.h"
#include "BmnMwpcTrack.h"

#include "FairTask.h"

#include <TClonesArray.h>
#include <TVector3.h>
#include <TMath.h>
#include <TString.h>
#include "TList.h"
#include "TH1D.h"
#include "TH2D.h"

#include <map>
#include <algorithm>
#include <vector>
using namespace std;

class BmnMwpcHitFinder : public FairTask
{
public:
    /** Default constructor **/
    BmnMwpcHitFinder() {};
    /** Constructor to be used **/
    BmnMwpcHitFinder(Bool_t, Int_t, Int_t);
    /** Destructor **/
    virtual ~BmnMwpcHitFinder();

    /** Virtual method Init **/
    virtual InitStatus Init();
    /** Virtual method Exec **/
    virtual void Exec(Option_t* opt);
    /** Virtual method Finish **/
    virtual void Finish();

    struct MC_points{
        Int_t  Id  = -1;//Id
        Int_t  Pdg = -999;//Id
        Int_t  Np2 = 0;
        Int_t  Np3 = 0;
        Bool_t xWas2=0;
        Bool_t uWas2=0;
        Bool_t vWas2=0;
        Bool_t xWas3=0;
        Bool_t uWas3=0;
        Bool_t vWas3=0;

        Double_t param2[4]   = {999., 999., 999., 999.};
        Double_t param3[4]   = {999., 999., 999., 999.};

        Double_t x2[6] = {-999., -999.,-999.,-999.,-999.,-999.};
        Double_t y2[6] = {-999., -999.,-999.,-999.,-999.,-999.};
        Double_t z2[6] = {-999., -999.,-999.,-999.,-999.,-999.};
        Double_t x3[6] = {-999., -999.,-999.,-999.,-999.,-999.};
        Double_t y3[6] = {-999., -999.,-999.,-999.,-999.,-999.};
        Double_t z3[6] = {-999., -999.,-999.,-999.,-999.,-999.};
        Double_t xt = -999.;
        Double_t yt = -999.;
    };

private:
    Bool_t expData;
    Bool_t fDebug   = 0;
    Bool_t fDoTest  = 0;
    UInt_t fEventNo; // event counter

    TString fInputBranchName;
    TString fOutputBranchName;
    TString fBmnEventHeaderBranchName;

    /** Input array of MWPC digits **/
    TClonesArray* fBmnMwpcDigitArray;       //!
    TClonesArray* fBmnHitsArray;            //!
    /** Output array of MWPC hits **/
    TClonesArray* fBmnMwpcSegmentsArray;    //!
    TClonesArray* fBmnMwpcEventHeader;      //!

    TString fOutputFileName;

    Int_t nInputDigits;  // max. number of found digits per plane
    Int_t nTimeSamples;  //

    BmnMwpcGeometrySRC* fMwpcGeometrySRC;

    TList fList;

    Int_t fRunPeriod;
    Int_t fRunNumber;
    Short_t kNChambers, kNPlanes, kNWires;
    Int_t kNumPairs;
    Int_t kCh_min, kCh_max;
    Int_t kBig;
    TVector3 *ChCent;
    Float_t *Zmid;
    Float_t *ChZ;
    TH1D  *hNtrMC, *hDen_mc, *hNum_mc, *hEff_mc,
    *hx_target, *hy_target, *hNp_MCtrue_ch2, *hNp_MCtrue_ch3,
    *hDen_mcreaction2, *hNum_mcreaction2,*hEff_mcreaction2,
    *hDen_mcreaction3, *hNum_mcreaction3,*hEff_mcreaction3, *hHalfDeadCh;   //!

    TH2D  *hYvsX_mc_ch2, *hYvsX_mc_ch3;     //!

    vector<TH1D*> hNp_best_Ch, hNbest_Ch, hOccupancy, hTime,
    hoccupancyXp, hoccupancyUp, hoccupancyVp, hoccupancyXm, hoccupancyUm, hoccupancyVm, hfiredWire_Ch, hClusterSize, hChisq_ndf_Ch, hNFired_layers_Ch,
    hWiresXp, hWiresUp, hWiresVp, hWiresXm, hWiresUm, hWiresVm,
    hResiduals_pl0_Ch, hResiduals_pl1_Ch, hResiduals_pl2_Ch, hResiduals_pl3_Ch, hResiduals_pl4_Ch, hResiduals_pl5_Ch,hfired_wire_Ch, hNum_layers_out_beam_Ch,
    hpar_Ax_Ch, hpar_Ay_Ch,hpar_Bx_Ch,hpar_By_Ch,
    hx_target_best, hy_target_best,
    hAx_mc_ch, hAy_mc_ch,hBx_mc_ch,hBy_mc_ch,
    hdAx_mc_SegCh, hdAy_mc_SegCh,hdX_mc_SegCh,hdY_mc_SegCh,
    hdAx_mc_Seg_deltaCh, hdAy_mc_Seg_deltaCh, hdX_mc_Seg_deltaCh,hdY_mc_Seg_deltaCh;

    vector<TH2D*>   hEvent_display_Ch, htime_wire_Ch, hY_X_Ch;
    Int_t kMinHits;
    Int_t kMinHits_before_target;
    Int_t kmaxSeg;
    Int_t kChMaxAllWires;
    Double_t kChi2_Max;
    Double_t dw, dw_half;
    Double_t sq3, sq12, sigma;
    Double_t kMiddlePl;

    vector<MC_points> vec_points;

    // Arrays
    Int_t    *Nseg_Ch;          //!
    Int_t    *counter_pl;       //!
    Int_t    *Nbest_Ch;         //!
    Int_t    *ipl;              //!
    Int_t    *Nlay_w_wires;     //!
    Float_t  *sigm2;            //!
    Float_t  *z2;               //!
    Float_t  **shift;           //!
    Float_t  **kZ_loc;          //!
    Float_t  **z_gl;            //!
    Int_t    **kPln;            //!
    Int_t    **iw;              //!
    Int_t    **iw_Ch;           //!
    Int_t    **Nhits_Ch;        //!
    Int_t    **Beam_wires_min;  //!
    Int_t    **Beam_wires_max;  //!
    Float_t  **XVU;             //!
    Float_t  **XVU_cl;          //!
    Int_t    **ind_best_Ch;     //!
    Int_t    **best_Ch_gl;      //!
    Double_t **Chi2_ndf_Ch;     //!
    Double_t **Chi2_ndf_best_Ch;//!
    Int_t    ***wire_Ch;        //!
    Float_t  ***xuv_Ch;         //!
    Int_t    ***Wires_Ch;       //!
    Int_t    ***clust_Ch;       //!
    Float_t  ***XVU_Ch;         //!
    Double_t ***par_ab_Ch;      //!
    Double_t ***Coord_xuv;      //!
    Double_t ***XVU_coord;      //!
    Double_t ***Coord_wire;     //!

    Int_t    *Nbest_seg;        //!
    Int_t    **Nclust;          //!
    Int_t    **Nhits_seg;       //!
    Int_t    ***ClusterSize;    //!
    Double_t ***DigitsArray;    //!
    Double_t **Chi2_ndf_seg;    //!
    Double_t ***Coor_seg;       //!
    Double_t ***Cluster_seg;    //!
    Double_t ***par_ab_seg;     //!
    Double_t ****sigma2_seg;    //!
    Double_t ***Cluster_coord;  //!
    Int_t    **Fired_layer;     //!

    Double_t **matrA;           //!
    Double_t **matrb;           //!
    Double_t **Amatr;           //!
    Double_t **bmatr;           //!
    const int kMaxMC      = 100;
    Double_t Z0_SRC;

    //functions for Vasilisa method:
    void PrepareArraysToProcessEvent();
    void ReadWires(Double_t ***, Int_t **, vector<MC_points>&);
    void Clustering(Int_t, Int_t***, Double_t***, Double_t***, Double_t***, Int_t **, Int_t *);
    void SegmentParamAlignment(Int_t, Int_t *,  Double_t ***, Float_t **);
    void SegmentFinder(Int_t , Int_t** , Double_t ***,  Int_t ***,Int_t *, Double_t ***, Double_t ***, Int_t **,Int_t , Short_t , Int_t );
    void SegmentFinder2coord(Int_t, Int_t **, Double_t ***, Int_t ***,Int_t *, Double_t ***, Double_t ***, Int_t **,Int_t, Short_t , Int_t ,Int_t *, Double_t ***);
    void ProcessSegments( Int_t ,  Int_t *, Double_t ***, Double_t ***, Int_t **, Float_t **, Int_t , Double_t , Double_t ,Int_t ** , Double_t **, Double_t ***, Double_t ***, Double_t ***, Int_t *, Int_t *, Double_t ****);
    void ProcessSegments2coord( Int_t ,  Int_t *, Double_t ***, Double_t ***, Int_t **, Float_t **, Int_t , Double_t , Double_t ,Int_t ** , Double_t **, Double_t ***, Double_t ***, Double_t ***, Int_t *, Int_t *, Double_t ****);
    void FillFreeCoefVectorXUV(Int_t, Double_t*, Float_t**,  Float_t**, Float_t*, Int_t*);
    void FillFreeCoefVector(Int_t , Double_t*, Double_t*** , Int_t , Float_t** , Double_t*, Int_t*);
    void FillFitMatrix(Int_t, Double_t **, Float_t **, Double_t *, Int_t *);
    void InverseMatrix(Double_t**, Double_t**);
    void MCefficiencyCalculation(Int_t, vector<MC_points>&, Double_t ***, Int_t *);
    void SegmentsStoring(Int_t *, Double_t ***,Double_t **, Int_t **, Double_t ***, Double_t ***, Double_t ****);

    const Int_t PDG_Li7 = 1000030070;//Li7
    const Int_t PDG_He4 = 1000020040;//He4

    TString fBmnEvQualityBranchName;
    TClonesArray* fBmnEvQuality;        //!

  ClassDef(BmnMwpcHitFinder, 1);
};

#endif
