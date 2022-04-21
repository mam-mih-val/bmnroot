// -------------------------------------------------------------------------
// -----                BmnStsKFTrackFitter header file                -----
// -----          Created 27/03/21  by A.Zinchenko, D.Zinchenko        -----
// -----                (from CbmStsKFTrackFitter.h)                   -----
// -----         BM@N STS track Kalman fitter with strip hits.         -----
// -------------------------------------------------------------------------

#ifndef BMNSTSKFTRACKFITTER
#define BMNSTSKFTRACKFITTER 1

//AZ #include "CbmKFStsHit.h"
#include "BmnKFStsHit.h"

#include "CbmStsTrackFitter.h"

#include <map>
#include <vector>

class CbmKFTrack;
class CbmVertex;
class TClonesArray;
class TProfile2D;

//AZ class CbmStsKFTrackFitter : public CbmStsTrackFitter{
class BmnStsKFTrackFitter : public CbmStsTrackFitter{

 public:

  /** Default constructor 
   */
  //AZ CbmStsKFTrackFitter();
  BmnStsKFTrackFitter();
  
  /** Destructor 
   */
  //AZ ~CbmStsKFTrackFitter() { };
  ~BmnStsKFTrackFitter() { };


  /** Initialisation 
   */
  void Init();

  /** Fit given track using Kalman Filter algorithm 
   */
  Int_t DoFit( CbmStsTrack* track, Int_t pidHypo=211 );

  // Fit given track using Kalman Filter algorithm (with material budget)
  void ReadMatBudget(TString &matBudgetFileName); 
  Int_t Fit( CbmStsTrack* track, Int_t pidHypo=211 ); //AZ
  Int_t FitWithMat (CbmKFTrack& track, Int_t downstream); //AZ
  // Evaluate material between hits (stations)  
  void EvalMaterial (CbmKFTrack& track, int ihit, Int_t downstream, CbmKFMaterial &mat); //AZ
  //void SetMatHistos(std::map<Double_t,TProfile2D*> *matHistos) { fMatHistos = matHistos; } //AZ
 
  /** Extrapolate track to any Z position 
   *
   * @param track    already fitted STS track
   * @param z        new z-position
   * @param e_track  (return value) extrapolated track parameters
   */
  void Extrapolate( CbmStsTrack* track, Double_t z, FairTrackParam *e_track );

  /** Extrapolate track param. to any Z position 
   *
   * @param track    already fitted STS track
   * @param z        new z-position
   * @param e_track  (return value) extrapolated track parameters
   */
  void Extrapolate( FairTrackParam* trackPar, Double_t z, FairTrackParam *e_track );

  /** Get normalized (in sigma's) impact parameter form vertex vtx
   *  if no vtx given, return deviation from primary vertex
   */
  Double_t GetChiToVertex( CbmStsTrack* track,  CbmVertex *vtx = 0 );

  /** Fit track to a vertex
   *
   * @param track    already fitted STS track
   * @param vtx      vertex
   * @param v_track  (return value) fitted track parameters
   * @param retrun   sqrt(Chi^2/NDF)
   */
  Double_t FitToVertex( CbmStsTrack* track, CbmVertex *vtx, FairTrackParam *v_track );
  void SetKFHits(CbmKFTrack &T, CbmStsTrack* track); //AZ

 private:

  //AZ void SetKFHits(CbmKFTrack &T, CbmStsTrack* track);
  Bool_t CheckTrack( CbmKFTrack &T );

  //AZ std::vector<CbmKFStsHit>   fHits;
  std::vector<BmnKFStsHit>   fHits;

  TClonesArray *fMvdHitsArray;
  TClonesArray *fStsHitsArray;

  Bool_t fIsInitialised;

  std::map<Double_t,TProfile2D*> fMatHistos;

 public:
  
  //AZ ClassDef( CbmStsKFTrackFitter, 1 );
  ClassDef( BmnStsKFTrackFitter, 1 );
 private:
  //AZ CbmStsKFTrackFitter(const CbmStsKFTrackFitter&);
  //AZ void operator=(const CbmStsKFTrackFitter&);
  BmnStsKFTrackFitter(const BmnStsKFTrackFitter&);
  void operator=(const BmnStsKFTrackFitter&);
};

#endif
