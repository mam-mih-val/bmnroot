// -------------------------------------------------------------------------
// -----              CbmStsFitPerformanceTask header file                -----
// -----               Created 27/01/05  by E. Kryshen                 -----
// -------------------------------------------------------------------------


/**  CbmStsFitPerformanceTask.h
 **@author S.Gorbunov <sgorboun@mail.desy.de>
 **@author I.Kisel    <ikisel@kip.uni-heidelberg.de>
 **@author E.Kryshen  <e.kryshen@gsi.de>
 **
 ** Analysis of the KalmanTrackFitter perfomance
 **/

#ifndef CBMSTSFITPERFORMANCE_H
#define CBMSTSFITPERFORMANCE_H

#include "CbmStsKFTrackFitter.h"

#include "FairTrackParam.h"
#include "CbmStsPoint.h"
#include "CbmMvdPoint.h"
#include "CbmStsTrack.h"
#include "FairTask.h"
#include "CbmMCTrack.h"
#include "CbmVertex.h"

#include "TClonesArray.h"

#include "TH1D.h"
#include "TH2D.h"

class CbmStsFitPerformanceTask : public FairTask
{

public:

  /** Standard constructor */
  CbmStsFitPerformanceTask(const char *name="CbmStsFitPerformanceTask", Int_t iVerbose = 1);


  /** Destructor */
  ~CbmStsFitPerformanceTask();


  /** Initialization of the task */ 
  InitStatus Init();

  /** ReInitialization of the task */ 
  InitStatus ReInit();


  /** Execute task */ 
  void Exec(Option_t* option);


  /** Finish task */ 
  void Finish();

  /** This function fill state vectors t[], mc[] and covariance 
   ** matrix c[] from the given FairTrackParam object and 
   ** related CbmStsPoint object. 
   ** @param in  is set to 1 if in values of position and momentum
   ** are considered.
   */
  
  void FillMCStateVectors(CbmStsPoint* point, Double_t mc[], Bool_t out = 0);
  void FillMCStateVectors(CbmMvdPoint* point, Double_t mc[], Bool_t out = 0);

  /** This function fills track pulls and residuals at certain z-position */
  void FillTrackHisto(const Double_t mc[6], CbmStsTrack *track, TH1D* hist[10]);
 
  /** This function fills vertex pulls and residuals */
  void FillVertexHisto(TVector3 &mc, CbmVertex *V, TH1D* hist[8]  );

  /** Returns 1 if track has >=8 measurements and stMaxID-stMinID>=4 */
  Bool_t IsLong(CbmStsTrack* track);

  void DoTrackAnalysis(const bool ok=1 ) {fTrackAnalysis=ok;}
  void DoVertexAnalysis(const bool ok=1 ) {fVertexAnalysis=ok;}
  void DoD0Analysis(const bool ok=1 ) {fD0Analysis=ok;}
  
  Double_t GetCharge(CbmMCTrack* mcTrack);
  
 protected:

  void CreateTrackHisto(TH1D* hist[10], const char* name, const char* title );
  void CreateVertexHisto(TH1D* hist[9], const char* name, const char* title );
  void CreateD0Histo(TH1D* hist[14], const char* name, const char* title );

  Int_t fEvent;
  Bool_t fVertexAnalysis, fD0Analysis, fTrackAnalysis;

  TClonesArray* fMCTrackArray;      ///< MCTracks
  TClonesArray* fStsPointArray;     ///< StsPoints
  TClonesArray* fMvdPointArray;     ///< StsPoints
  TClonesArray* fRecStsTrackArray;  ///< Reconstructed StsTracks
  TClonesArray* fStsHitArray;          ///< Sts hits
  TClonesArray* fMvdHitArray;          ///< Sts hits
  CbmVertex* fPrimaryVertex;     ///< Primary vertex
  TClonesArray* fSTSTrackMatch;  ///< Related MC tracks

  TH1D* fhChi2;      // x=chi2; y=entries for all
  TH1D* fhProb;      // x=Prob function; y=entries for all
  TH1D* fhDP;
  TH2D* fhDP2;
  TH1D* fhDsP;
  TH2D* fhDsP2;

  TH1D *fhZMCf, *fhZMCl;  // z first/last of MC track
  TH1D *fhZRecof, *fhZRecol;  // z first/last of Reco track
  
  TH2D *fhRes_vs_Mom_f, *fhRes_vs_Mom_l;      // resolution vs momentum
  TH2D *fhExtraTracks2ndMVD; // extra tracks not detected in the second mvd chamber

  TH1D* fhFrst[10]; 
  TH1D* fhMid[10];
  TH1D* fhLast[10];
  TH1D* fhImp[10];
  TH1D* fhImpPi[10];
  TH1D* fhVfit[10];
  TH1D* fhVtx[9];
  TH1D* fhV[13][9];
  TH2D* fhPq;
  TH1D *fhD0[4][14];

  TH1D *fhHitDensity[10];
  TH1D *fhTrackDensity[8];
  TH1D *fhTrackDensity0L;

  TDirectory *histodir;
  CbmStsKFTrackFitter fFitter;

  ClassDef(CbmStsFitPerformanceTask,1);
 private:
  CbmStsFitPerformanceTask(const CbmStsFitPerformanceTask&);
  void operator=(const CbmStsFitPerformanceTask&);
};

#endif
