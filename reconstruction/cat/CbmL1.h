/*
 *====================================================================
 *
 *  CBM Level 1 Reconstruction 
 *  
 *  Authors: I.Kisel,  S.Gorbunov
 *
 *  e-mail : ikisel@kip.uni-heidelberg.de 
 *
 *====================================================================
 *
 *  CbmL1 header file
 *
 *====================================================================
 */

#ifndef _CbmL1_h_
#define _CbmL1_h_

#include "CbmL1Track.h"
#include "CbmL1Vtx.h"
#include "CbmKFParticle.h"
#include "CbmL1MCTrack.h"
#include "CbmL1MCPoint.h"
#include "CbmL1StsHit.h"
#include "CbmStsDigiScheme.h"
#include "CbmL1ParticlesFinder.h"
#include "KFMCParticle.h"
#include "KFParticleMatch.h"

#include "FairTask.h"

#include "TClonesArray.h"

#include <vector>

using std::vector;

class L1Algo;

class CbmL1HitStore{
 public:
  int ExtIndex;
  int iStation;
  double x, y;
};

class CbmL1 : public FairTask 
{
  private:
    CbmL1(const CbmL1&);
    CbmL1 operator=(const CbmL1&);
  public:

   L1Algo *algo;                //! for access to L1 Algorithm from L1::Instance
   CbmL1ParticlesFinder *PF;    //!

   vector<CbmL1Track> vRTracks; //! reconstructed tracks
   
  static CbmL1 *Instance(){ return fInstance; }

  void SetParContainers();
  virtual InitStatus ReInit();
  virtual InitStatus Init();
  void Exec(Option_t * option);

  CbmL1();
  
    /**                                  Constructor
      * @param _fPerformance - type of Efficiency output: 0 - w\o efficiencies, doesn't use MC data; 1 - L1 standard efficiency definition; 2 - QA efficiency definition
      * @param fSTAPDataMode_ - way to work with files for the standalone package: 0 - off , 1 - write, 2  - read data and work only with it, 3 - write and read (debug)
      * @param findParticleMode_ : 0 - don't run FindParticles; 1 - run, all MC particle is reco-able; 2 - run, MC particle is reco-able if created from reco-able tracks; 3 - run, MC particle is reco-able if created from reconstructed tracks
      */
  CbmL1(const char *name, Int_t iVerbose = 1, Int_t _fPerformance = 0, int fSTAPDataMode_ = 0, TString fSTAPDataDir_ = "./", int findParticleMode_ = 0);

  ~CbmL1( /*if (targetFieldSlice) delete;*/ );

  void SetMaterialBudgetFileName( TString s ){ fMatBudgetFileName = s; }
  void SetExtrapolateToTheEndOfSTS( bool b ){ fExtrapolateToTheEndOfSTS = b; }
  void Finish();

//   void SetTrackingLevel( Int_t iLevel ){ fTrackingLevel = iLevel; }
//   void MomentumCutOff( Double_t cut ){ fMomentumCutOff = cut; }
//   void SetGhostSuppression( Bool_t b ){ fGhostSuppression= b; }
//   void SetDetectorEfficiency( Double_t eff ){ fDetectorEfficiency = eff; }

  vector<CbmL1HitStore> vHitStore; // diff hit information

  void Reconstruct();
  
  friend class L1AlgoDraw;
  friend class L1AlgoPulls;
  template<int NHits>  friend class L1AlgoEfficiencyPerformance;
  friend class CbmL1MCTrack;
  friend class CbmL1PFFitter;
 private:
   void IdealTrackFinder(); // just copy all reconstructable MCTracks into RecoTracks.

    /// Read information about hits, mcPoints and mcTracks into L1 classes
   void ReadEvent();
   bool ReadMCPoint( CbmL1MCPoint *MC, int iPoint, bool MVD ); // help procedure

    /// Input Performance
   void HitMatch();         // Procedure for match hits and MCPoints.
   void FieldApproxCheck(); // Build histos with difference between Field map and approximated field
   void FieldIntegralCheck(); // Build 2D histo: dependence of the field integral on phi and theta
   void InputPerformance(); // Build histos about input data, like hit pulls, etc.
  
    /// Reconstruction Performance
   void TrackMatch();              // Procedure for match Reconstructed and MC Tracks. Should be called before Performances
   void EfficienciesPerformance(); // calculate efficiencies
   void GetMCParticles();          // create MC particles from MC tracks
   void FindReconstructableMCParticles();
   void MatchParticles();          // Procedure for match Reconstructed and MC Particles. Should be called before Performances
   void PartEffPerformance(); // calculate efficiencies
   void TrackFitPerformance();     // pulls & residuals. Can be called only after Performance()
   void HistoPerformance();        // fill some histograms and calculate efficiencies
   void PartHistoPerformance();    // histograms for particle finder

      /// STandAlone Package service-functions
   void WriteSTAPGeoData(void *geo, int size); // create geo_algo.dat
   void WriteSTAPAlgoData(); // create data_algo.dat
   void WriteSTAPPerfData(); // create data_perfo.dat
   void ReadSTAPGeoData(void *geo, int &size);
   void ReadSTAPAlgoData();
   void ReadSTAPPerfData();
      /// SIMD KF Banchmark service-functions
   void WriteSIMDKFData();

   static istream& eatwhite(istream& is); // skip spaces
   static void writedir2current( TObject *obj ); // help procedure

   int NStation, NMvdStations, NStsStations; // number of detector stations (all\sts\mvd)
   Int_t fPerformance; // 0 - w\o perf. 1 - L1-Efficiency definition. 2 - QA-Eff.definition
   int fSTAPDataMode; // way to work with file for standalone package. 0 (off) , 1 (write), 2 (read data and work only with it), 3 (debug - write and read)
   TString fSTAPDataDir;

   Int_t fTrackingLevel;  // really doesn't used
   Double_t fMomentumCutOff;  // really doesn't used
   Bool_t fGhostSuppression;  // really doesn't used
   Bool_t fUseMVD;  // really doesn't used
//   Double_t fDetectorEfficiency;  // really doesn't used

   //AZ CbmStsDigiScheme StsDigi;
   CbmStsDigiScheme *StsDigi;   //!
   CbmL1Vtx PrimVtx;            //!
//    L1FieldSlice *targetFieldSlice  _fvecalignment;

    /// Input data
   TClonesArray *listMCTracks;      //!
   TClonesArray *listStsPts;        //! Sts MC points
   TClonesArray *listStsDigi;       //!
   TClonesArray *listStsClusters;   //!
   TClonesArray *listStsHits;       //!

   TClonesArray *listMvdPts;        //! Mvd MC points
   TClonesArray *listMvdHits;       //!
   TClonesArray *listMvdHitMatches; //!

    /// Used data = Repacked input data
   vector<CbmL1StsHit>  vStsHits;   //! hits with hit-mcpoint match information
   vector<CbmL1MCPoint> vMCPoints;  //!
   vector<CbmL1MCTrack> vMCTracks;  //!
   vector<int>          vHitMCRef;  // indices of MCPoints in vMCPoints, indexed by index of hit in algo->vStsHits array. According to StsMatch. Used for IdealResponce

  vector<CbmKFParticle> vRParticles;    //! reco particles
  vector<KFMCParticle> vMCParticles;    //! MC particles
  vector<KFMatchParticles> MCtoRParticleId; //! array for match
  vector<KFMatchParticles> RtoMCParticleId; //!
  
  TDirectory *histodir;                 //!
   
  static CbmL1 *fInstance;              //!

 private:
  void CheckMCParticleIsReconstructable(KFMCParticle &part); // recursive func, used in FindReconstructableMCParticles

  int fFindParticlesMode;

  TString fMatBudgetFileName;
  bool fExtrapolateToTheEndOfSTS;
  
   ClassDef(CbmL1,1);
};

#endif //_CbmL1_h_
