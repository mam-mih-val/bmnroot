// Author: Vasilisa Lenivenko (VBLHEP) <vasilisa@jinr.ru> 2019-07-18

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// BmnUpstreamTracking                                                      //
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

#ifndef BMNUPSTREAMTRACKING_H
#define BMNUPSTREAMTRACKING_H 1

#include <fstream>
#include <stdlib.h>
#include <TMath.h>
#include <vector>
#include "TList.h"
#include "TH1D.h"
#include "TH2D.h"
#include <TNamed.h>
#include <TString.h>
#include <TClonesArray.h>
#include <TVector2.h>
#include <Rtypes.h>
#include "FairTask.h"
#include "FairTrackParam.h"
#include "FairMCPoint.h"
#include "BmnTrack.h"
#include "BmnSiliconHit.h"
#include "BmnSiliconTrackFinder.h"
#include "BmnMwpcTrack.h"
#include "BmnMwpcSegment.h"

class TH1D;
class TH2D;

struct Smatch {
  Int_t     Ind1   = -1;//Si
  Int_t     Ind2   = -1;//Pctr
  Int_t     Inds2  = -1;//seg2
  Int_t     Inds3  = -1;//seg3 
  Int_t     Nhits1 = -1;
  Int_t     Nhits2 = -1;
  Double_t  Z1     = -999.;
  Double_t  Z2     = -999.;
  Double_t  Zs2    = -999.;
  Double_t  Zs3    = -999.;
  Double_t  distX  = -1.;
  Double_t  distY  = -1.;
  Double_t  Chi2m  =  999.;
  Double_t  param1[4]    = { 999., 999., 999., 999.};
  Double_t  param2[4]    = { 999., 999., 999., 999.};
  Double_t  params2[4]   = { 999., 999., 999., 999.};
  Double_t  params3[4]   = { 999., 999., 999., 999.};
  //Silicon
  Double_t  SiCoordX[4]  = {-999., -999., -999., -999.};
  Double_t  SiCoordY[4]  = {-999., -999., -999., -999.};
  Double_t  SiCoordZ[4]  = {-999., -999., -999., -999.};
  Double_t  SiSigmaX[4]  = {-999., -999., -999., -999.};
  Double_t  SiSigmaY[4]  = {-999., -999., -999., -999.};
  Double_t  SiMod[4]     = {-999., -999., -999., -999.};
};

struct UpTracks {
  Double_t  param[4]  = { 999., 999., 999., 999.};
  Double_t  CoordX[5] = {-999., -999., -999., -999., -999.};
  Double_t  CoordY[5] = {-999., -999., -999., -999., -999.};
  Double_t  CoordZ[5] = {-999., -999., -999., -999., -999.};
  Double_t  Chi2      =  999.;
  Int_t     Nhits     = -1;
};

using namespace std;
using namespace TMath;

class BmnUpstreamTracking : public FairTask {
  
public:
  
  BmnUpstreamTracking(Int_t);
  virtual ~BmnUpstreamTracking();
  virtual InitStatus Init();
  virtual void Exec(Option_t* opt);
  virtual void Finish();
  
  static bool compareSegments(const Smatch &a, const Smatch &b) {
   return a.Chi2m < b.Chi2m;
  }

private:

  Bool_t fDebug   = 0;
  UInt_t fEventNo = 0; // event counter
  Int_t  fRunNumber;
  Bool_t expData;
  TList  fList;
  TClonesArray* fSiTracks;
  TClonesArray* fSilHits;
  TClonesArray* fMWPCTracks;
  TClonesArray* fMWPCSegments;
  TString fInputBranchName1;
  TString fInputBranchName2;
  TString fInputBranchName3;
  TString fInputBranchHits;
  /** Output array of Silicon Hits **/
  TClonesArray* fBmnUpstreamTracksArray;
  TString fOutputTracksBranchName;
  TString fOutputFileName;
  //--------
  Double_t*** par_ab_Ch;
  Double_t*** par_ab_tr;
  Double_t*** SiXYhits;
  Double_t*** Points;
  Double_t**  par_ab_SiTr;
  Double_t**  par_Seg_z;
  Double_t**  par_ab_trz;
  Double_t**  Xforglfit;
  Double_t**  Amatr;
  Double_t*   par_SiTr_z;
  Double_t*   X_shift_seg;
  Double_t*   X_shiftUp;
  Double_t*   Y_shiftUp;
  Double_t*   Z_pair;
  Double_t*   Z_Chamber;
  Double_t*   AmatrInverted ;
  Double_t*   rhs;
  Double_t*   AmatrArray;
  Double_t*   line;
  Int_t*      Nseg_Ch;
  Int_t*      NPCTracks;
  Int_t*      NSiXYhits;
  Int_t       NSiTracks;
  Int_t       NumPoints;
  
  vector<Smatch> vtmpSeg;
  vector<Smatch> OutVector;
  vector<UpTracks> vecUpTracks;
  
  //--------
  void PrepareArraysToProcessEvent();
  void ReadSiliconTracks(Double_t**, Double_t*, Int_t &);
  void ReadSiliconHits(Double_t***, Int_t*);
  void ReadMWPCSegments(Double_t***, Double_t**, Int_t*);
  void ReadMWPCTracks(Double_t***, Double_t**, Int_t*);
  void PCTracksSiTracksMatching(Double_t**, Double_t*, Int_t &, Double_t***, Double_t**, Int_t*, vector<Smatch> &,vector<Smatch> &);
  void PCSegmentsSiTracksMatching(Double_t**, Double_t*, Int_t &,Double_t***, Double_t**, Int_t*, vector<Smatch> &, vector<Smatch> &);
  void RecordingBest(Int_t & , vector<Smatch> & , vector<Smatch> & );
  void RecordCandidateSeg(Int_t &, Int_t &, Int_t &, Double_t**, Double_t***, Double_t*, Double_t**, Double_t &, Double_t &, vector<Smatch>& );
  void GetAddSiHits(vector<Smatch> &, Double_t***, Int_t* );
  void GetHitsToPoints(vector<Smatch> &,Double_t***, Int_t*, Double_t***, Int_t*,  Double_t***, Int_t &);
  void GlobalFit(Double_t**, Double_t**, Double_t*);
  bool InvertMatrix(Double_t*, Double_t*);
  void CalculationOfChi2(Double_t***, Int_t &, vector<UpTracks> &);
  void PrintAllTracks(vector<UpTracks> & );
  void TrackRecording(vector<UpTracks> & );
  
  //--------
  const Int_t kBig         = 200;
  const Int_t kNumPars     = 10;//parameters
  const Int_t kNumPairs    = 2; 
  const Int_t kNumChambers = 4; 
  const Int_t kNumStSi     = 4;
  const Int_t kPoints      = 5;//kNumDets
  const Int_t kPoin_Par    = 5;//parameters
  const Double_t dw        = 0.25; // [cm]
  const Double_t SigmXmwpc = dw / 6.;//dw / sq12;
  const Double_t SigmYmwpc = dw / 3.;
  const Double_t kZ_target = -645.191;//cm
  const Double_t kZSi_cent = -392.5;
  const Double_t kChi2_Max = 50.;
  const Double_t X_shift   = 0.02;
  const Double_t Y_shift   = 0.3;
  const Double_t cutAx     = 0.02;//0.012;
  const Double_t cutAy     = 0.02;//0.015;
  const Double_t cutX      = 1.5;//cm
  const Double_t cutY      = 2.0;//cm
  const Double_t Zcentr    = -350.;//cm
  
  //--------
  TH1D  *hNSi_NPC, *hNPC_NSi, *hAx_fitUp,*hAy_fitUp,* hx_fitUp,* hy_fitUp,* hchi2_fitUp,* hNhitsUp;
  vector<TH1D*> hResXst, hResYst;

  ClassDef(BmnUpstreamTracking, 1)
};

#endif
