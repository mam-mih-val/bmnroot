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

#include <map>
#include <algorithm>
#include <Rtypes.h>
#include <TClonesArray.h>
#include <TVector3.h>
#include <TMath.h>
#include <TString.h>
#include "FairTask.h"
#include "BmnMwpcHit.h"
#include "BmnMwpcDigit.h"
#include "BmnMwpcGeometrySRC.h"
#include "FairTask.h"
#include "TList.h"
#include "BmnTrack.h"
#include "BmnMwpcTrack.h"
#include <vector>
#include <BmnEventQuality.h>

#include "TH1D.h"
#include "TH2D.h"

class TH1D;
class TH2D;

using namespace std;

class BmnMwpcHitFinder : public FairTask {
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

  private:
  Bool_t expData;
  Bool_t fDebug   = 0;
  Bool_t fDoTest  = 1;
  UInt_t fEventNo; // event counter

  TString fInputBranchName;
  TString fOutputBranchName;
  TString fBmnEventHeaderBranchName;

  /** Input array of MWPC digits **/
  TClonesArray* fBmnMwpcDigitArray;
  /** Output array of MWPC hits **/
  TClonesArray* fBmnMwpcSegmentsArray;
  TClonesArray* fBmnMwpcEventHeader;

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
  TH1D * hNclust_ch0_pl1, *hNclust_ch3_pl1;
  vector<TH1D*> hNp_best_Ch, hNbest_Ch, hOccupancy, hTime,
         hoccupancyXp, hoccupancyUp, hoccupancyVp, hoccupancyXm, hoccupancyUm, hoccupancyVm, hfiredWire_Ch, hClusterSize, hChisq_ndf_Ch, hNFired_layers_Ch,
         hWiresXp, hWiresUp, hWiresVp, hWiresXm, hWiresUm, hWiresVm,
         hResiduals_pl0_Ch, hResiduals_pl1_Ch, hResiduals_pl2_Ch, hResiduals_pl3_Ch, hResiduals_pl4_Ch, hResiduals_pl5_Ch,hfired_wire_Ch, hNum_layers_out_beam_Ch;
  vector<TH2D*>   hEvent_display_Ch, htime_wire_Ch;
  Int_t kMinHits;
  Int_t kMinHits_before_target;
  Int_t *Nlay_w_wires;
  Int_t kmaxSeg;
  Int_t kChMaxAllWires;
  Double_t kChi2_Max;
  Double_t dw, dw_half;
  Double_t sq3, sq12, sigma;
  Double_t kMiddlePl;

  // Arrays
  Int_t    *Nseg_Ch;
  Int_t    *Nbest_Ch;
  Float_t  *sigm2;
  Float_t  *z2;
  Int_t    *ipl;
  Float_t  *dX_i;
  Float_t  **shift;
  Float_t  **kZ_loc;
  Float_t  **z_gl;
  Int_t    **kPln;
  Int_t    **iw;
  Int_t    **iw_Ch;
  Int_t    **Nhits_Ch;
  Float_t  **XVU;
  Float_t  **XVU_cl;
  Int_t    **ind_best_Ch;
  Int_t    **best_Ch_gl;
  Double_t **Chi2_ndf_Ch;
  Double_t **Chi2_ndf_best_Ch;
  Int_t    ***wire_Ch;
  Float_t  ***xuv_Ch;
  Int_t    ***Wires_Ch;
  Int_t    ***clust_Ch;
  Float_t  ***XVU_Ch;
  Double_t ***par_ab_Ch;
  Double_t ***Coord_xuv;
  Double_t ***XVU_coord;
  Double_t ***Coord_wire;


  Int_t **Nclust;
  Double_t ***Cluster_coord;
  Int_t ***ClusterSize;
  Double_t ***DigitsArray;
  Int_t    **Nhits_seg;
  Double_t **Chi2_ndf_seg;
  Double_t ***Coor_seg;
  Double_t ***Cluster_seg;
  Double_t ***par_ab_seg;
  Double_t ****sigma2_seg;
  Int_t    *Nbest_seg;

  Double_t **matrA;
  Double_t **matrb;
  Double_t **Amatr;
  Double_t **bmatr;

  //functions for Vasilisa method:
  void PrepareArraysToProcessEvent();
  void Clustering(Int_t, Int_t***, Double_t***, Double_t***, Double_t***, Int_t **);
  void SegmentParamAlignment(Int_t, Int_t *,  Double_t ***, Float_t **);
  void SegmentFinder(Int_t , Int_t** , Double_t ***,  Int_t ***,Int_t *, Double_t ***, Double_t ***, Int_t **,Int_t , Short_t , Int_t );
  void ProcessSegments( Int_t ,  Int_t *, Double_t ***, Double_t ***, Int_t **, Float_t **, Int_t , Double_t , Double_t ,Int_t ** , Double_t **, Double_t ***, Double_t ***, Double_t ***, Int_t *, Int_t *, Double_t ****);
  void FillFreeCoefVectorXUV(Int_t, Double_t*, Float_t**,  Float_t**, Float_t*, Int_t*);
  void FillFreeCoefVector(Int_t , Double_t*, Double_t*** , Int_t , Float_t** , Double_t*, Int_t*);
  void FillFitMatrix(Int_t, Double_t **, Float_t **, Double_t *, Int_t *);
  void InverseMatrix(Double_t**, Double_t**);

  TString fBmnEvQualityBranchName;
  TClonesArray* fBmnEvQuality;

  ClassDef(BmnMwpcHitFinder, 1);
};

#endif

