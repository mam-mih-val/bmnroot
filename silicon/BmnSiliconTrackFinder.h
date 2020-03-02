// Author: Vasilisa Lenivenko (VBLHEP) <vasilisa@jinr.ru> 2019-07-18

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// BmnSiliconTrackFinder                                                      //
//                                                                            //
// Implementation of an algorithm developed by                                //
// V.Lenivenko and V.Paltchik                                                 //
// to the BmnRoot software                                                    //
//                                                                            //
// The algorithm serves for searching for track segments                      //
// in the Silicon detectors of the BM@N experiment                            //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////


#ifndef BMNSILICONTRACKFINDER_H
#define BMNSILICONTRACKFINDER_H 1

#include <fstream>
#include <stdlib.h>
#include <TMath.h>
#include <TNamed.h>
#include <TString.h>
#include <TClonesArray.h>
#include <TVector2.h>
#include <Rtypes.h>
#include "FairTask.h"
#include "FairTrackParam.h"
#include "FairMCPoint.h"
#include <vector>
#include "TList.h"
#include "TH1D.h"
#include "TH2D.h"
#include "BmnTrack.h"
#include "BmnSiliconDigit.h"
#include "BmnSiliconTrack.h"
#include "BmnSiliconHit.h"


struct tracksX {
  Int_t    Nhits = 0;
  Double_t Chi2  = 0.;
  Double_t Xv    = 999.;
  Double_t Yv    = 999.;
  Double_t param[4]   = {999., 999., 999., 999.};
  Double_t CoordX[4]  = {-999., -999., -999., -999.};
  Double_t CoordXp[4] = {-999., -999., -999., -999.};
  Double_t CoordY[4]  = {-999., -999., -999., -999.};
  Double_t CoordZ[4]  = {-999., -999., -999., -999.};
  Double_t SigmaX[4]  = {-999., -999., -999., -999.};
  Double_t SigmaXp[4] = {-999., -999., -999., -999.};
  Double_t SigmaY[4]  = {-999., -999., -999., -999.};
  
  Int_t    ModNum[4]    = {-1, -1, -1, -1};
  Double_t StripNumX[4] = {-999., -999., -999., -999.};
  Double_t StripNumXp[4]= {-999., -999., -999., -999.};
  Double_t ClSizeX[4]   = {-999., -999., -999., -999.};
  Double_t ClSizeXp[4]  = {-999., -999., -999., -999.};
  Double_t SumQX[4]     = {-999., -999., -999., -999.};
  Double_t SumQXp[4]    = {-999., -999., -999., -999.};
};

using namespace std;
using namespace TMath;

class BmnSiliconTrackFinder : public FairTask {
  
public:
  
  BmnSiliconTrackFinder(Bool_t, Bool_t, Int_t);
  virtual ~BmnSiliconTrackFinder();
  
  virtual InitStatus Init();

  virtual void Exec(Option_t* opt);

  virtual void Finish();

  static bool compareSegments(const tracksX &a, const tracksX &b){
   return a.Chi2 < b.Chi2;
  }

private:

  Bool_t fDebug   = 0;
  UInt_t fEventNo = 0; // event counter
  Int_t fRunNumber;
  Bool_t fTarget;
  Int_t N;
  Bool_t expData;
  TList fList;
  TClonesArray* fBmnSiDigitsArray;
  TClonesArray* fSiTracks;
  /** Output array of Silicon Hits **/
  TString fOutputHitsBranchName;
  TClonesArray* fBmnSiliconHitsArray;
  TClonesArray* fBmnSiliconHitsXArray;
  TClonesArray* fBmnSiliconHitsXpArray;
  TClonesArray* fBmnSiliconHitsXYArray;
  TString fOutputFileName;
  
  //--
  Double_t  ChiSquare;
  Int_t    *iClustXMod;
  Int_t    *iModX;
  Int_t    *Nsp_st;
  Double_t *AmatrInverted ;
  Double_t *rhs;
  Double_t *AmatrArray;
  Double_t *line;
  Double_t *shiftStXtoGlob;
  Double_t *shiftStYtoGlob;
  Double_t *Zposition;
  Double_t *SensitiveAreaY;
  Double_t *Ampl_strX;
  Double_t *Ampl_strXp;
  Int_t    **NclustX;
  Int_t    **NclustXp;
  Int_t    **NhitsXYmod;
  Int_t    **Nleftoversp;
  Int_t    **NleftoverX;
  Int_t    **NleftoverXp;
  Double_t **Amatr;
  Double_t **shiftX;
  Double_t **shiftY;
  Double_t **Zstation;
  Double_t **Xforglfit;
  Double_t **Xpforglfit;
  Double_t **Yforglfit;
  Double_t **SigmXforglfit;
  Double_t **SigmXpforglfit;
  Double_t **SigmYforglfit;
  Double_t **ZstnCforglfit;
  Double_t ***DigitsArrayX;
  Double_t ***DigitsArrayXp;
  Double_t ***XClust;
  Double_t ***XpClust;
  Double_t ***XClust_width;
  Double_t ***XpClust_width;
  Double_t ***sumQx;
  Double_t ***sumQxp;
  Double_t ***XspClust;
  Double_t ***XpspClust;
  Double_t ***XspClust_width;
  Double_t ***XpspClust_width;
  Double_t ***sumQxsp;
  Double_t ***sumQxpsp;
  Double_t ***XCoord;
  Double_t ***XpCoord;
  Double_t ***XspCoord;
  Double_t ***XpspCoord;
  Double_t ***YspCoord;
  Double_t ***SigmaX;
  Double_t ***SigmaXp;
  Double_t ***SigmspX;
  Double_t ***SigmspXp;
  Double_t ***SigmspY;
  Double_t ***leftoverXsp;
  Double_t ***leftoverXpsp;
  Double_t ***leftoverYsp;
  Double_t ***leftoverXsigsp;
  Double_t ***leftoverXpsigsp;
  Double_t ***leftoverYsigsp;
  Double_t ***leftoverX;
  Double_t ***leftoverXp;
  Double_t ***leftoverXsig;
  Double_t ***leftoverXpsig;
  
  vector<tracksX> vec_tracks;
  vector<tracksX> CleanTr;
  
  TH1D *hNpoint, *hChiSquareNdf, *hNtracks, *hAxglob, *hBxglob, *hAyglob,*hByglob, *hY_st_1,*hY_st_2,*hY_st_3, *hdY_st_1,*hdY_st_2,*hdY_st_3, 
      *hX_st_1,*hX_st_2,*hX_st_3, *hdX_st_1,*hdX_st_2,*hdX_st_3,
      *N_eff,* D_eff,* E_eff, *hdX_st1_st2, *hdX_st2_st3, *hdX_st1_st3,*hdY_st1_st2, *hdY_st2_st3, *hdY_st1_st3,*hdXst1_0_st2_0,*hdXst1_0_st2_1,*hdXst1_1_st2_0,*hdXst1_1_st2_1,
      *hdXst1_2_st2_0,*hdXst1_2_st2_1,*hdXst1_3_st2_0,*hdXst1_3_st2_1,*hdXst2_0_st3_1,*hdXst2_0_st3_2,*hdXst2_1_st3_1,*hdXst2_1_st3_2,*hdYst1_0_st2_0,
      *hdYst1_0_st2_1,*hdYst1_1_st2_0,*hdYst1_1_st2_1,*hdYst1_2_st2_0,*hdYst1_2_st2_1,*hdYst1_3_st2_0,*hdYst1_3_st2_1,*hdYst2_0_st3_1,*hdYst2_0_st3_2,
      *hdYst2_1_st3_1,*hdYst2_1_st3_2,*hX13_X2_m0,*hX13_X2_m1, *hXp13_Xp2_m0,*hXp13_Xp2_m1, *hY13_Y2_m0,*hY13_Y2_m1, *hY1m0_Y23, *hY1m1_Y23, *hY1m2_Y23, *hY1m3_Y23;
  TH2D *hvertexXY, * hvertex_aver_XY,* hprofile_beam_z1,* hprofile_beam_z2,* hprofile_beam_z3, *hdYvsYst_1,*hdYvsYst_2,*hdYvsYst_3,
      *hdXvsXst_1,*hdXvsXst_2,*hdXvsXst_3,
     *hdYvsYst1_mod0, *hdYvsYst1_mod1,*hdYvsYst1_mod2,*hdYvsYst1_mod3,*hdYvsYst2_mod0,*hdYvsYst2_mod1,*hdYvsYst3_mod1,*hdYvsYst3_mod2,
     *hdXvsXst1_0,*hdXvsXst1_1,*hdXvsXst1_2,*hdXvsXst1_3,*hdXvsXst2_0,*hdXvsXst2_1,*hdXvsXst3_1,*hdXvsXst3_2;
     
  void PrepareArraysToProcessEvent();
  void Case1(Int_t **, Double_t ***, Double_t ***, Double_t ***, Double_t ***, Double_t ***, Double_t ***, vector<tracksX>&,
            Double_t ***, Double_t ***, Double_t ***, Double_t ***, Double_t ***, Double_t ***);
  void Case2(Int_t & , Int_t **, Double_t ***, Double_t ***, Double_t ***,Double_t ***, Double_t ***, Double_t ***, Int_t  **, Int_t **,Double_t ***, Double_t ***, Double_t ***, Double_t ***, vector<tracksX>&,
            Double_t ***, Double_t ***, Double_t ***, Double_t ***, Double_t ***, Double_t ***,
            Double_t ***, Double_t ***, Double_t ***, Double_t ***, Double_t ***, Double_t ***);
  void Case3(Int_t **, Double_t ***, Double_t ***, Double_t ***,Double_t ***, Double_t ***, Double_t ***, Int_t  **, Int_t **,Double_t ***, Double_t ***, Double_t ***, Double_t ***, vector<tracksX>&, 
            Double_t ***, Double_t ***, Double_t ***, Double_t ***, Double_t ***, Double_t ***,
            Double_t ***, Double_t ***, Double_t ***, Double_t ***, Double_t ***, Double_t ***);
  void Case4(Int_t & , Int_t **, Double_t ***, Double_t ***, Double_t ***,Double_t ***, Double_t ***, Double_t ***, Int_t  **, Int_t **,Double_t ***, Double_t ***, Double_t ***, Double_t ***, vector<tracksX>&,
            Double_t ***, Double_t ***, Double_t ***, Double_t ***, Double_t ***, Double_t ***,
            Double_t ***, Double_t ***, Double_t ***, Double_t ***, Double_t ***, Double_t ***);
  
  void Clustering( Double_t***, Double_t *** , Double_t *** ,  Double_t *** , Double_t  *** , Int_t **, Double_t *** , Double_t *** , Double_t *** , Int_t **);
  void GlobalFit( Double_t **, Double_t **, Double_t **,Double_t **, Double_t **, Double_t **, Double_t *);
  bool InvertMatrix(Double_t *, Double_t *);
  void GetXYspatial(Int_t **NClX, Int_t **NClXp, Double_t ***XCoor, Double_t ***XpCoor, Double_t ***Xsp, Double_t ***Xpsp, Double_t ***Ysp, Int_t **NXYsp, Double_t ***SigmX, Double_t ***SigmXp, Double_t ***SigspX, Double_t ***SigspXp, Double_t ***SigspY,
                    Double_t ***, Double_t ***, Double_t ***, Double_t ***, Double_t ***, Double_t ***,
                    Double_t ***, Double_t ***, Double_t ***, Double_t ***, Double_t ***, Double_t ***);
  void CoordinateCalculation(Int_t **, Int_t **, Double_t ***, Double_t ***, Double_t ***, Double_t ***, Double_t ***, Double_t ***, Double_t ***, Double_t ***, Double_t ***, Double_t ***, Int_t **,
                             Double_t ***, Double_t ***, Double_t ***, Double_t ***, Double_t ***, Double_t ***,
                             Double_t ***, Double_t ***, Double_t ***, Double_t ***, Double_t ***, Double_t ***);
  void calculationOfChi2(Double_t **, Double_t **, Double_t **, Double_t **,  Int_t *,  Int_t *, Double_t & , Double_t *);
  void CoordinateAlignment( Int_t **,  Double_t ***, Double_t ***, Double_t ***, Int_t **, Int_t **, Double_t ***, Double_t ***);
  void RecordingTracksAfterSpatialPoints(vector<tracksX> & , vector<tracksX> & );
  void RecordingTracks(vector<tracksX> & , vector<tracksX> & );
  void RecordingTracks_case3(vector<tracksX> & , vector<tracksX> & );
  void CheckPoints(Int_t **, Int_t **, Double_t ***, Double_t ***, Double_t ***, Double_t ***,Double_t ***, Double_t ***, Double_t ***, Double_t ***, Double_t ***, Double_t ***, 
                  Int_t **,  vector<tracksX> & ,Int_t **, Int_t **,Int_t **, Double_t ***, Double_t ***,Double_t ***,Double_t ***,Double_t ***,Double_t ***,Double_t ***,Double_t ***,Double_t ***,Double_t ***);
  void PrintAllTracks(vector<tracksX> & );
  void CountSpatialPoints(Int_t **, Int_t *);
  Double_t GetXYspatial_station1_2(Int_t &, Int_t &, Double_t & , Double_t &);
  Double_t FindClusterCenter(Double_t*, Int_t , Double_t &);
  Bool_t   SkipEvent(Int_t **NclustX_, Int_t **NclustXp_, Double_t ***XCoord_, Double_t ***XpCoord_);
  Int_t    Angle(Int_t &, Int_t &);
  
  
  const float tg2_5      = tan(2.5*TMath::Pi()/180.);
  const float sin2_5     = sin(2.5*TMath::Pi()/180.);
  const float cos2_5     = cos(2.5*TMath::Pi()/180.);
  const Int_t nstripXp   = 640;
  const Int_t nstripXpsm = 614;
  const Double_t sq12    = sqrt(12.);
  const Int_t    kBig               = 300;
  const Int_t    fNstations         = 4;
  const Int_t    fNmodules          = 8;
  const Int_t    fNmodules1         = 4;
  const Int_t    fNmodules2         = 2;
  const Int_t    fNmodules3         = 8;
  const Int_t    fNstrips           = 643;
  const Double_t deltaX             = 0.0095;    //cm
  const Double_t deltaXp            = 0.01030001;//cm 
  const Double_t Z0_SRC_target      = -645.191;
  const Double_t Zcentr             = -392.524; // Zc = Zi/3
  const Double_t half_module        = 6.0705; 
  const Double_t half_target_region = 2.5;
  const Double_t shift_after_zigzag = 0.01;//cm 
  const Double_t Xv_av              = 0.4;
  const Double_t Yv_av              = 0.4;
  const Double_t half_roadX1_X2     = 0.11;
  const Double_t half_roadXp1_Xp2   = 0.11;
  const Double_t half_roadX1_X3     = 0.7;
  const Double_t half_roadX1_Xp1    = 0.3;
  const Double_t half_roadX2_Xp2    = 0.3;
  const Double_t half_roadX3_Xp3    = 0.5;
  const Double_t half_roadY1_Y2     = 0.45;
  const Double_t half_roadXp1_Xp2_diff_sign = 0.3;
  const Double_t half_target_regionX = 2.5;
  const Double_t half_target_regionY = 3.5;
  const Double_t half_roadX1_Xp2     = .5;
  const Double_t Shift_toCenterOfMagnetX = 0.55;
  const Double_t Shift_toCenterOfMagnetY = 0.11 -4.5;

  //cut for cluster charge
  const Double_t Cut_AmplX       = 120.;
  const Double_t Cut_AmplXp      = 120.;
  const Double_t Cut_overflow    = 1800.;
  //cut for strip charge
  const Double_t Cut_AmplStripX  = 120.;
  const Double_t Cut_AmplStripXp = 120.;
  const Double_t Chi2_Globcut    = 80.;//20.;
  const Int_t    N_min_points    = 4;

  ClassDef(BmnSiliconTrackFinder, 1)
};

#endif
