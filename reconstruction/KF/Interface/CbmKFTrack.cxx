#include "CbmKFTrack.h"

#include "CbmKFMath.h"

#include "FairTrackParam.h"
#include "CbmStsTrack.h"

#include "TParticlePDG.h"
#include "TDatabasePDG.h"
#include "TMath.h"

ClassImp(CbmKFTrack)

CbmKFTrack::CbmKFTrack(): fMass(0), fChi2(0), fIsElectron(kFALSE), fNDF(0), fHits(){
  for (Int_t i=0; i<6; i++) fT[i]=0.;
  for (Int_t i=0; i<15; i++) fC[i]=0.;
}



void CbmKFTrack::SetTrack( CbmKFTrackInterface &track ){
  for( Int_t i=0; i<6;  i++) fT[i] = track.GetTrack()[i];
  for( Int_t i=0; i<15; i++) fC[i] = track.GetCovMatrix()[i];
  fMass = track.GetMass();
  fIsElectron = track.IsElectron();
  fChi2 = track.GetRefChi2();
  fNDF  = track.GetRefNDF();
}

void CbmKFTrack::SetTrackParam( FairTrackParam &track )
{
  CbmKFMath::CopyTrackParam2TC( &track, fT, fC ); 
}

void CbmKFTrack::SetStsTrack( CbmStsTrack &track, bool first )
{
  SetPID( track.GetPidHypo() );
  SetTrackParam( first? *track.GetParamFirst() : *track.GetParamLast() );
  GetRefChi2() = track.GetChi2();
  GetRefNDF() = track.GetNDF();
}

void CbmKFTrack::GetTrackParam( FairTrackParam &track )
{
  CbmKFMath::CopyTC2TrackParam( &track, fT, fC ); 
}

void CbmKFTrack::GetStsTrack( CbmStsTrack &track, bool first )
{
  GetTrackParam( first? *track.GetParamFirst() : *track.GetParamLast() );
  track.SetChi2( GetRefChi2() );
  track.SetNDF( GetRefNDF() );
}

void CbmKFTrack::SetPID( Int_t pidHypo )
{ 
  TParticlePDG* particlePDG = TDatabasePDG::Instance()->GetParticle(pidHypo);
  fMass = (particlePDG) ? particlePDG->Mass() :0.13957;
  fIsElectron = (TMath::Abs(pidHypo)==11); 
}
