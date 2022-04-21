/** Implementation of the CbmKFSecondaryVertexFinder class
 *
 * @author  S.Gorbunov, I.Kisel
 * @version 1.0
 * @since   06.02.06
 *
 */
#include "CbmStsKFSecondaryVertexFinder.h"

#include "CbmKFMath.h"

#include <vector>

using std::vector;

ClassImp(CbmStsKFSecondaryVertexFinder)

void CbmStsKFSecondaryVertexFinder::Clear(){
  Finder.Clear();
  vStsTracks.clear();
  vKFTracks.clear();
}

void CbmStsKFSecondaryVertexFinder::AddTrack( CbmStsTrack *Track ){
  if( !Track ) return;
  vStsTracks.push_back(Track);
  vKFTracks.push_back(CbmKFTrack(*Track));
}

void CbmStsKFSecondaryVertexFinder::SetApproximation( CbmVertex *Guess ){
  if( !Guess ){
    Finder.SetApproximation();
  }else{
    VGuess.SetVertex( *Guess );
    Finder.SetApproximation(&VGuess);   
  }
}

void CbmStsKFSecondaryVertexFinder::SetMassConstraint( Double_t MotherMass ){
  Finder.SetMassConstraint( MotherMass );
}

void CbmStsKFSecondaryVertexFinder::SetTopoConstraint( CbmVertex *Parent ){
  if( !Parent ){
    Finder.SetTopoConstraint();
  }else{
    VParent.SetVertex( *Parent );
    Finder.SetTopoConstraint(&VParent);   
  }
}

void CbmStsKFSecondaryVertexFinder::Fit(){
  Finder.ClearTracks();
  for(vector<CbmKFTrack>::iterator i=vKFTracks.begin(); i!=vKFTracks.end(); ++i){
    Finder.AddTrack( &*i );
  }
  Finder.Fit();
}

void CbmStsKFSecondaryVertexFinder::GetVertex( CbmVertex  &vtx ){
  Finder.GetVertex( vtx );
}
/*
void CbmStsKFSecondaryVertexFinder::GetFittedTrack( Int_t itrack, CbmStsTrack *Track ){
  if(!Track) return;
  *Track = *vStsTracks[itrack];
  double T[6], C[15];
  Finder.GetTrack(itrack, T, C );
  CbmKFMath::CopyTC2TrackParam( Track->GetParamFirst(), T, C ); 
}

void CbmStsKFSecondaryVertexFinder::GetFittedTrack( Int_t itrack, FairTrackParam *Param ){
  if(!Param) return;
  double T[6], C[15];
  Finder.GetTrack(itrack, T, C );
  CbmKFMath::CopyTC2TrackParam( Param, T, C ); 
}
*/
void CbmStsKFSecondaryVertexFinder::GetMotherTrack( CbmStsTrack *MotherTrack ){
  if( !MotherTrack ) return;
  double T[6], C[15];
  Finder.GetMotherTrack( T, C );
  CbmKFMath::CopyTC2TrackParam( MotherTrack->GetParamFirst(), T, C );
  CbmKFMath::CopyTC2TrackParam( MotherTrack->GetParamLast(), T, C );
  MotherTrack->SetPidHypo(211);
  MotherTrack->SetChi2(0);
  MotherTrack->SetNDF(1);
}

void CbmStsKFSecondaryVertexFinder::GetMass( Double_t *M, Double_t *Error_ ){
  Finder.GetMass( M, Error_ );
}

CbmStsTrack *CbmStsKFSecondaryVertexFinder::GetTrack( Int_t itrack ){
  return vStsTracks[itrack];
}

Int_t CbmStsKFSecondaryVertexFinder::GetNTracks(){
  return vStsTracks.size();
}
