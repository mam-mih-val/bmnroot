#include "CbmPVFinderKF.h"

#include "CbmKFTrack.h"
#include "CbmKFVertex.h"
#include "CbmKFPrimaryVertexFinder.h"

#include "CbmStsTrack.h"

#include "TClonesArray.h"

#include "math.h"

ClassImp(CbmPVFinderKF)

Int_t CbmPVFinderKF::FindPrimaryVertex(TClonesArray* tracks, CbmVertex* vertex ){

  Int_t NTracks = tracks->GetEntries();

  CbmKFPrimaryVertexFinder Finder;
  CbmKFTrack* CloneArray = new CbmKFTrack[NTracks];
  for( Int_t i=0; i<NTracks; i++ ){    
    CbmStsTrack* st = (CbmStsTrack*) tracks->At(i);
    Int_t NHits = st->GetNStsHits();
    if( NHits < 4 ) continue;
    if( st->GetFlag() ) continue;
    if( st->GetChi2()<0. || st->GetChi2()>3.5*3.5*st->GetNDF() ) continue;
    CbmKFTrack &T = CloneArray[i];
    T.SetStsTrack( *st );
    if( !finite(T.GetTrack()[0]) || !finite(T.GetCovMatrix()[0])) continue;
    Finder.AddTrack(&T);
  }
  CbmKFVertex v;
  Finder.Fit( v );
  v.GetVertex(*vertex);
  delete[] CloneArray;
  return 0;
}
