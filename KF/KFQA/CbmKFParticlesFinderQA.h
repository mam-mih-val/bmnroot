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
 *  KF Particles Finder performance 
 *
 *====================================================================
 */

#ifndef _CbmKFParticlesFinderQA_h_
#define _CbmKFParticlesFinderQA_h_

#include "KFMCParticle.h"
#include "KFParticleMatch.h"
#include "CbmKFPartEfficiencies.h"
#include "CbmKFTrErrMCPoints.h"
#include "FairTask.h"
#include <vector>
#include <map>
#include <cstring>

class CbmKFParticlesFinder;
class TClonesArray;
class CbmVertex;
class TDirectory;
class TH1F;
class TH2F;

class CbmKFParticlesFinderQA :public FairTask
{
 public:

  CbmKFParticlesFinderQA(CbmKFParticlesFinder *pf=0, Int_t iVerbose = 1, int findParticlesMode = 1, int perf = 3,
                         const char *name ="CbmKFParticlesFinderQA",
                         const char *title="Cbm KF Particles Finder QA");
  ~CbmKFParticlesFinderQA();

  void SetOutFile(TString outname) {outfileName = outname;}
  void SetEffFileName(TString effname) {fEfffileName = effname;}

  void SaveParticles(Bool_t b) { fSaveParticles = 1; }
  void SaveMCParticles(Bool_t b) { fSaveMCParticles = 1; }

  virtual InitStatus ReInit();
  virtual InitStatus Init();
  void Exec(Option_t * option);
  void Finish();

 private:
 
  void StsHitMatch();
  void WriteHistos( TObject *obj );
  void WriteHistosCurFile( TObject *obj );
  void GetMCParticles();
  void FindReconstructableMCParticles();
  void CheckMCParticleIsReconstructable(KFMCParticle &part);
  void MatchParticles();
  void PartEffPerformance();
  void PartHistoPerformance();
  
  // performance modes and definitions
  const int fFindParticlesMode;
  const int fPerformance;
  const int fMinNStations;
  const float fMinRecoMom;
// CbmKFParticlesFinder object
  CbmKFParticlesFinder *fPF;

  vector<CbmKFTrErrMCPoints> fMCTrackPoints;
  vector<KFMCParticle> vMCParticles;  // MC particles
  vector<KFMatchParticles> MCtoRParticleId; // array for match
  vector<KFMatchParticles> RtoMCParticleId;
  vector<bool> vIsBkgWithSamePDG;
// Arrays of points, tracks, etc

  TClonesArray* flistStsPts;
  TClonesArray* flistMvdPts;
  TClonesArray* flistMCTracks;
  TClonesArray* flistStsTracksMatch;
  TClonesArray* flistStsTracks;
  TClonesArray* flistStsHits;
  TClonesArray* flistMvdHits;
  TClonesArray* flistMvdHitMatches;
  TClonesArray* flistStsClusters;
  TClonesArray* flistStsDigi;
  CbmVertex*    fPrimVtx;

// output arrays of particles
  TClonesArray* fRecParticles;    // output array of KF Particles
  TClonesArray* fMCParticles;     // output array of MC Particles
  TClonesArray* fMatchParticles;  // output array of match objects

  Bool_t fSaveParticles;
  Bool_t fSaveMCParticles;

// Names of files
  TString outfileName;
  TString fEfffileName;

  TDirectory* histodir;

  std::vector<int> vStsHitMatch;
  std::vector<int> vStsPointMatch;
  std::vector<int> vMvdPointMatch;
  std::vector<int> vMCTrackMatch;

// efficiencies
  CbmKFPartEfficiencies fParteff;
  int fNEvents;

//histos
  static const int nFitQA = 16;
  TH1F *hFitDaughtersQA[CbmKFPartEfficiencies::nParticles][nFitQA];
  TH1F *hFitQA[CbmKFPartEfficiencies::nParticles][nFitQA];

  static const int nHistoPartParam = 13;
  TH1F *hPartParam[CbmKFPartEfficiencies::nParticles][nHistoPartParam]; // mass, p, pt, Y, decay length, c*tau, chi/ndf, prob, theta, phi, z
  TH1F *hPartParamBG[CbmKFPartEfficiencies::nParticles][nHistoPartParam];
  TH1F *hPartParamGhost[CbmKFPartEfficiencies::nParticles][nHistoPartParam];
//   TH1F *hPartParamCorrBG[CbmKFPartEfficiencies::nParticles][nHistoPartParam];
  TH1F *hPartParamSignal[CbmKFPartEfficiencies::nParticles][nHistoPartParam];
  static const int nHistoPartParamQA = 3;
  TH1F *hPartParamQA[CbmKFPartEfficiencies::nParticles][nHistoPartParamQA*2]; // residuals and pulls of these parameters

  static const int nHistoPartParam2D = 1;
  TH2F *hPartParam2D[CbmKFPartEfficiencies::nParticles][nHistoPartParam2D]; // y-pt,
  TH2F *hPartParam2DBG[CbmKFPartEfficiencies::nParticles][nHistoPartParam2D];
  TH2F *hPartParam2DGhost[CbmKFPartEfficiencies::nParticles][nHistoPartParam2D];
//   TH2F *hPartParam2DCorrBG[CbmKFPartEfficiencies::nParticles][nHistoPartParam2D];
  TH2F *hPartParam2DSignal[CbmKFPartEfficiencies::nParticles][nHistoPartParam2D];
  
  static const int nHistosPV = 6;
  TH1F *hPVFitQa[nHistosPV];

  static const int nHistoMotherPdg = 17;
  std::map<int,int> fMotherPdgToIndex;
  TH1F *hMotherPdg[nHistoMotherPdg];
  int GetMotherPdgIndex(int pdg)
  {
    std::map<int, int>::iterator it;
    it=fMotherPdgToIndex.find(pdg);
    if(it != fMotherPdgToIndex.end()) return it->second;
    else return -1;
  }

  static const int nHistosTP = CbmKFPartEfficiencies::nParticles + 1;
  TH1F *hTrackParameters[nHistosTP];
  
  ClassDef(CbmKFParticlesFinderQA,1);

 private:
  CbmKFParticlesFinderQA(const CbmKFParticlesFinderQA&);
  void operator=(const CbmKFParticlesFinderQA&);
};

#endif // _CbmKFParticlesFinderQA_h_
