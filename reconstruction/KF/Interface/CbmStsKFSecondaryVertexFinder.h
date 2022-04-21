/** The CbmStsKFSecondaryVertexFinder class
 *
 * @author  S.Gorbunov, I.Kisel
 * @version 1.0
 * @since   06.02.06
 * 
 * Class to find secondary vertex with the Kalman Filter method
 *
 */
#ifndef CBMSTSKFSECONDARYVERTEXFINDER_H
#define CBMSTSKFSECONDARYVERTEXFINDER_H

#include "CbmKFSecondaryVertexFinder.h"
#include "CbmKFTrack.h"
#include "CbmKFVertex.h"

#include "CbmStsTrack.h"

#include <vector>

class CbmStsKFSecondaryVertexFinder:public TObject {
  
  std::vector<CbmStsTrack*> vStsTracks;
  std::vector<CbmKFTrack> vKFTracks;
  CbmKFVertex VParent, VGuess;
  CbmKFSecondaryVertexFinder Finder;

 public:

  CbmStsKFSecondaryVertexFinder(): vStsTracks(), vKFTracks(), VParent(), VGuess(), Finder() { Clear(); };
  ~CbmStsKFSecondaryVertexFinder(){};

  void Clear();
  void AddTrack( CbmStsTrack *Track );
  void SetApproximation( CbmVertex *Guess=0 );
  void SetMassConstraint( Double_t MotherMass=-1 );
  void SetTopoConstraint( CbmVertex *Parent=0 );
  void Fit();
  void GetVertex( CbmVertex  &vtx );
  //void GetFittedTrack( Int_t itrack, CbmStsTrack *Track );
  //void GetFittedTrack( Int_t itrack, FairTrackParam *Param );
  void GetMotherTrack( CbmStsTrack *MotherTrack );
  void GetMass( Double_t *M, Double_t *Error );
  CbmStsTrack * GetTrack( Int_t itrack );
  Int_t GetNTracks();

  ClassDef( CbmStsKFSecondaryVertexFinder, 1 );
};
#endif /* !CBMSTSKFSECONDARYVERTEXFINDER_H */
