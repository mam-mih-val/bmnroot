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

#include "BmnTrack.h"
#include "BmnSiliconHit.h"
#include "BmnSiliconTrackFinder.h"
#include "BmnMwpcTrack.h"
#include "BmnMwpcHit.h"
#include "BmnMwpcSegment.h"

#include "FairTask.h"
#include "FairTrackParam.h"
#include "FairMCPoint.h"

#include <TMath.h>
#include "TList.h"
#include "TH1D.h"
#include "TH2D.h"
#include <TString.h>
#include <TClonesArray.h>
#include <TVector2.h>

#include <fstream>
#include <vector>
using namespace std;
using namespace TMath;


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
  Int_t     Pdg       = -1;
};



class BmnUpstreamTracking : public FairTask
{
  public:
  BmnUpstreamTracking();
  BmnUpstreamTracking(Bool_t, Int_t);
  virtual ~BmnUpstreamTracking();

  virtual InitStatus Init();
  virtual void Exec(Option_t* opt);
  virtual void Finish();
  
  static bool compareSegments(const Smatch &a, const Smatch &b) {
   return a.Chi2m < b.Chi2m;
  }
  
  struct MC_points{
    Int_t  Id  = -1;//
    Int_t  Pdg = -1;//
    Int_t  Np_P   = 0;
    Int_t  Np_Si  = 0;
    Int_t  Np_ch2 = 0;
    Int_t  Np_ch3 = 0;
    Int_t  Np = 0;
    
    Bool_t xWas2=0;
    Bool_t uWas2=0;
    Bool_t vWas2=0;
    Bool_t xWas3=0;
    Bool_t uWas3=0;
    Bool_t vWas3=0;
    Bool_t np_3si=0;
    
    Double_t param[4]= {999., 999., 999., 999.};
    Double_t x_si[4] = {-999., -999.,-999.,-999.};
    Double_t y_si[4] = {-999., -999.,-999.,-999.};
    Double_t z_si[4] = {-999., -999.,-999.,-999.};
  };
  struct twotracks{
    Double_t track1[5]   = {-999.,-999.,-999.,-999.,-999.};//Ax, x, Ay, Y
    Int_t    track1_pdg  = -999;
    Double_t track2[5]   = {-999.,-999.,-999.,-999.,-999.};//Ax, x, Ay, Y
    Int_t    track2_pdg  = -999;
  };

  private:
  Bool_t fDebug   = 0;
  UInt_t fEventNo = 0; // event counter
  Int_t  fRunNumber;
  Bool_t expData;
  TList  fList;
  TClonesArray* fSiTracks;                  //!
  TClonesArray* fSilHits;                   //!
  TClonesArray* fMWPCTracks;                //!
  TClonesArray* fMWPCSegments;              //!
  TString fInputBranchName1;
  TString fInputBranchName2;
  TString fInputBranchName3;
  TString fInputBranchHits;
  //--mc--
  TClonesArray* fSiTracksSim;               //!
  TString fInputBranchNameSimTrue;
  TClonesArray* fBmnHitsArray;              //!
  TString fInputBranchName;
  
  /** Output array of Silicon Hits **/
  TClonesArray* fBmnUpstreamTracksArray;    //!
  TString fOutputTracksBranchName;
  TString fOutputFileName;
  //--------
  Double_t*** par_ab_Ch;                    //!
  Double_t*** par_ab_tr;                    //!
  Double_t*** SiXYhits;                     //!
  Double_t*** Points;                       //!
  Double_t**  par_ab_SiTr;                  //!
  Double_t**  par_Seg_z;                    //!
  Double_t**  par_ab_trz;                   //!
  Double_t**  Xforglfit;                    //!
  Double_t**  Amatr;                        //!
  Double_t*   par_SiTr_z;                   //!
  Double_t*   X_shift_seg;                  //!
  Double_t*   X_shiftUp;                    //!
  Double_t*   Y_shiftUp;                    //!
  Double_t*   Z_pair;                       //!
  Double_t*   Z_Chamber;                    //!
  Double_t*   AmatrInverted ;               //!
  Double_t*   rhs;                          //!
  Double_t*   AmatrArray;                   //!
  Double_t*   line;                         //!
  Int_t*      Nseg_Ch;                      //!
  Int_t*      NPCTracks;                    //!
  Int_t*      NSiXYhits;                    //!
  Int_t       NSiTracks;
  Int_t       NumPoints;
  Double_t    Shift_toCenterOfMagnetX;//  = 0.039;
  Double_t    Shift_toCenterOfMagnetY;//  = 0.13;
  Double_t    Shift_toCenterOfMagnetAX;// = 0.;
  Double_t    Shift_toCenterOfMagnetAY;// = 0.0019;
  Double_t    X_shift;
  Double_t    Y_shift;
  
  vector<Smatch>    vtmpSeg;
  vector<Smatch>    OutVector;
  vector<UpTracks>  vecUpTracks;
  vector<MC_points> vec_points;
  vector<twotracks> vec_twotracks;
  //--------
  void PrepareArraysToProcessEvent();
  void ReadSiliconTracks(Double_t**, Double_t*, Int_t &, vector<MC_points> &);
  void ReadSiliconHits(Double_t***, Int_t*);
  void ReadMWPCSegments(Double_t***, Double_t**, Int_t*, vector<MC_points> &);
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
  void MCefficiencyCalculation(vector<MC_points> &, vector<UpTracks> &);
  
  //--------
  const Int_t kBig         = 200;
  const Int_t kPoints      = 5;//kNumDets
  const Int_t kPoin_Par    = 5;//parameters
  const Int_t kNumPars     = 10;//parameters
  const Int_t kNumPairs    = 2;//number of MWPC pairs 
  const Int_t kNumChambers = 4;//number of MWPC stat
  const Int_t kNumStSi     = 4;//number of Si stat
  const Int_t kMaxMC       = 100;
  const Int_t fNstations   = 4;
  const Int_t kNPlanes     = 6;
  const Double_t kChi2_Max = 50.;
  const Double_t dw        = 0.25; // [cm]//MWPC wire step
  const Double_t SigmXmwpc = sqrt(2)*dw / 6.;//dw/6.;//dw / sq12;
  const Double_t SigmYmwpc = sqrt(2)*dw / 3.;//dw/3.
  //matching consts 
  const Double_t cutAx     = 0.02;//0.012;
  const Double_t cutAy     = 0.02;//0.015;
  const Double_t cutX      = 1.5;//cm
  const Double_t cutY      = 2.0;//cm
  //important Z
  const Double_t Zcentr    = -350.;//cm
  const Double_t kZ_target = -645.191;//cm
  const Double_t kZSi_cent = -392.5;
  //MC
  const Int_t PDG_Be7 = 1000040070;//Be7
  const Int_t PDG_Li6 = 1000030060;//Li6
  const Int_t PDG_Li7 = 1000030070;//Li7
  const Int_t PDG_He3 = 1000020030;//He3
  const Int_t PDG_He4 = 1000020040;//He4
  const Int_t PDG_H2  = 1000010020;//H2
  const Int_t PDG_p   = 2212;//proton 

  //some hists
  TH1D *hAx_fitUp,*hAy_fitUp,* hx_fitUp,* hy_fitUp,* hchi2_fitUp,* hNhitsUp, 
    *hdAx_tr_mc_comb, *hdX_tr_mc_comb, *hdAy_tr_mc_comb, *hdY_tr_mc_comb,
    *hdAx_uptr_mc, *hdX_uptr_mc, *hdAy_uptr_mc, *hdY_uptr_mc,
    *hDen_mcuptr, *hNum_mcuptr, *hEff_mcuptr,
    *hAx_upmc, *hAy_upmc, *hX_upmc, *hY_upmc,
    *hNtr_reco, *hNtr_mc, *hNrecoTrif2mc, *hAngle_reco, *hAngle_recoifNmc2, *hAngle_recoifNmc2Cases;    //!
  TH2D  *hNtr_mc_vs_reco, *hy_vs_x_Up, *hY_vs_Xmctrue, *hvertexXYUp, *hTyTx_Up;                         //!
  vector<TH1D*> hResXst, hResYst;

  ClassDef(BmnUpstreamTracking, 1)
};

#endif
