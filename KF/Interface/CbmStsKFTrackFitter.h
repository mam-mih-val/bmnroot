#ifndef CBMSTSKFTRACKFITTER
#define CBMSTSKFTRACKFITTER 1

#include "CbmKFStsHit.h"

#include "CbmStsTrackFitter.h"

#include <vector>

class CbmKFTrack;
class CbmVertex;

class CbmStsKFTrackFitter : public CbmStsTrackFitter{

 public:

  /** Default constructor 
   */
  CbmStsKFTrackFitter();
  
  /** Destructor 
   */
  ~CbmStsKFTrackFitter() { };


  /** Initialisation 
   */
  void Init();

  /** Fit given track using Kalman Filter algorithm 
   */
  Int_t DoFit( CbmStsTrack* track, Int_t pidHypo=211 );
 
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

 private:

  void SetKFHits(CbmKFTrack &T, CbmStsTrack* track);
  Bool_t CheckTrack( CbmKFTrack &T );

  std::vector<CbmKFStsHit>   fHits;

  TClonesArray *fMvdHitsArray;
  TClonesArray *fStsHitsArray;

  Bool_t fIsInitialised;

 public:
  
  ClassDef( CbmStsKFTrackFitter, 1 );
 private:
  CbmStsKFTrackFitter(const CbmStsKFTrackFitter&);
  void operator=(const CbmStsKFTrackFitter&);
};

#endif
