/*
 *====================================================================
 *
 *  CBM KF Track Quality
 *  
 *  Authors: M.Zyzak
 *
 *  e-mail : 
 *
 *====================================================================
 *
 *  KF Fit performance 
 *
 *====================================================================
 */

#ifndef _CbmKFTrackFitQA_h_
#define _CbmKFTrackFitQA_h_

#include "FairTask.h"

#include "CbmKFTrack.h"
#include "CbmMCTrack.h"
#include "CbmStsPoint.h"
#include "CbmMvdPoint.h"
#include "CbmTrackMatch.h"
#include "CbmStsTrack.h"
#include "CbmStsHit.h"
#include "CbmStsCluster.h"
#include "CbmStsDigi.h"
#include "CbmMvdHit.h"
#include "CbmMvdHitMatch.h"

#include "CbmKFTrErrMCPoints.h"

#include "TClonesArray.h"
#include "TH1.h"
#include "TProfile.h"

#include <iostream>
#include <vector>

class CbmKFTrackFitQA :public FairTask
{
 public:

  CbmKFTrackFitQA();
  ~CbmKFTrackFitQA();

  void StsHitMatch();
  void FillHitHistos();
  void FillHistoAtParticleVertex(CbmMCTrack *track_mc, CbmKFTrack *track_kf);
  void FillHistoAtFirstPoint(CbmKFTrErrMCPoints *mc_points, CbmMCTrack *track_mc, CbmKFTrack *track_kf);
  void Write();
  void FindBackTracks(CbmKFTrErrMCPoints *mc_points, CbmMCTrack *track_mc, CbmKFTrack *track_kf, int iEvent);
  void FindBackTracks();
  void Save();

  void SetOutFile(TString outname) {outfileName = outname;}

  virtual InitStatus ReInit();
  virtual InitStatus Init();
  void Exec(Option_t * option);
  void Finish();

 private:
// Arrays of points, tracks, etc

  TClonesArray *listStsPts;
  TClonesArray *listMvdPts;
  TClonesArray *listMCTracks;
  TClonesArray *listStsTracksMatch;
  TClonesArray *listStsTracks;
  TClonesArray *listStsHits;
  TClonesArray *listMvdHits;
  TClonesArray *listMvdHitMatches;
  TClonesArray *listStsClusters;
  TClonesArray *listStsDigi;


// Names of files
  TString outfileName;

  vector<int> vStsHitMatch;

// histograms

  //hit parameters
  TH1F *res_STShit_x;
  TH1F *res_STShit_y;
  TH1F *pull_STShit_x;
  TH1F *pull_STShit_y;

  TH1F *res_MVDhit_x;
  TH1F *res_MVDhit_y;
  TH1F *pull_MVDhit_x;
  TH1F *pull_MVDhit_y;


  //track parameters at the particle vertex
  TH1F *res_AtPV_x;
  TH1F *res_AtPV_y;
  TH1F *res_AtPV_tx;
  TH1F *res_AtPV_ty;
  TH1F *res_AtPV_qp;

  TH1F *pull_AtPV_x;
  TH1F *pull_AtPV_y;
  TH1F *pull_AtPV_tx;
  TH1F *pull_AtPV_ty;
  TH1F *pull_AtPV_qp;

  //track parameters at the first MC point of the track
  TH1F *res_AtFP_x;
  TH1F *res_AtFP_y;
  TH1F *res_AtFP_tx;
  TH1F *res_AtFP_ty;
  TH1F *res_AtFP_qp;

  TH1F *pull_AtFP_x;
  TH1F *pull_AtFP_y;
  TH1F *pull_AtFP_tx;
  TH1F *pull_AtFP_ty;
  TH1F *pull_AtFP_qp;

  //Q detertmination quality
  TProfile *q_QA;
  TProfile *dp_p;

  TH1F *ggg;

  int Nback;
  //FILE *fBack;

  ClassDef(CbmKFTrackFitQA,1);
 private:
  CbmKFTrackFitQA(const CbmKFTrackFitQA&);
  void operator=(const CbmKFTrackFitQA&);
};

#endif // _CbmKFTrackFitQA_h_
