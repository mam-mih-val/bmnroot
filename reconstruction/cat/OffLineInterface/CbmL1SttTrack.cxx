
#include "CbmL1SttTrack.h"
#include "CbmStsTrack.h"
#include "CbmMuchTrack.h"
#include "CbmKFMath.h"

ClassImp(CbmL1SttTrack);

void CbmL1SttTrack::SetStsTrack( CbmStsTrack* track)
{
  CbmKFMath::CopyTrackParam2TC( track->GetParamLast(), T, C );
  chi2 = 0;//track->GetChi2();
  NDF = 0;//track->GetNDF();
}

void CbmL1SttTrack::SetMuchTrack( CbmMuchTrack* track)
{
  CbmKFMath::CopyTrackParam2TC( track->GetMuchTrack(), T, C );
  chi2 = 0;//track->GetChi2();
  NDF = 0;//track->GetNDF();
}

