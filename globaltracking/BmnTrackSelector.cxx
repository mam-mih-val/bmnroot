
#include "BmnTrackSelector.h"
#include "CbmGlobalTrack.h"
#include "TMath.h"

BmnTrackSelector::BmnTrackSelector() :
   fMaxChiSq(10e10),
   fMinLastPlaneId(-1),
   fMinNofHits(-1),
   fMinMomentum(0.)
{
}

BmnTrackSelector::BmnTrackSelector(
   Float_t maxChiSq, Int_t minLastPlaneId, Int_t minNofHits, Float_t minMomentum):
   fMaxChiSq(maxChiSq),
   fMinLastPlaneId(minLastPlaneId),
   fMinNofHits(minNofHits),
   fMinMomentum(minMomentum)
{

}

BmnTrackSelector::~BmnTrackSelector() {}

BmnStatus BmnTrackSelector::TrackSelectCuts(TClonesArray* tracks) {
//   if (itBegin == itEnd) { return kLITSUCCESS; }

   for (Int_t iTrack = 0; iTrack < tracks->GetEntriesFast(); ++iTrack) {
       CbmGlobalTrack* track = (CbmGlobalTrack*) tracks->At(iTrack);
      if (track->GetFlag() == kBMNBAD) { continue; }

      if (track->GetChi2() / track->GetNDF() > fMaxChiSq 
//              || track->GetNofHits() < fMinNofHits ||                              //FIXME!!!
//            track->GetLastStationId() < fMinLastPlaneId                       //FIXME!!!
              ) {
         track->SetFlag(kBMNBAD);
      }

      // Momentum cut
      if (fMinMomentum != 0.) {
         if (track->GetParamLast()->GetQp() == 0.) {
            track->SetFlag(kBMNBAD);
            continue;
         }
         Float_t mom = TMath::Abs(1. / track->GetParamLast()->GetQp());
         if (mom < fMinMomentum) { track->SetFlag(kBMNBAD); }
      }
   }

   return kBMNSUCCESS;
}
