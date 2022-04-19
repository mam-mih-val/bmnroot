/**
 * \file BmnAcceptanceFunction.h
 * \author Andrey Lebedev <andrey.lebedev@gsi.de> - original author for CBM experiment
 * \author Sergey Merts <sergey.merts@gmail.com> - modifications for BMN experiment
 * \date 2014
 * \brief Global function to define the track acceptance. Used in QA.
 */

#ifndef BMNACCEPTANCEFUNCTION_H_
#define BMNACCEPTANCEFUNCTION_H_

#include "CbmMCTrack.h"
#include "BmnTrackMatch.h"
#include "TClonesArray.h"
#include "TMath.h"
#include "TDatabasePDG.h"
//#include <cmath>
//using std::abs;


class BmnAcceptanceFunction
{
public:

   static Bool_t AllTrackAcceptanceFunction(
         const TClonesArray* mcTracks,
         Int_t index)
   {
      return true;
   }

   static Bool_t NegativeTrackAcceptanceFunction(
         const TClonesArray* mcTracks,
         Int_t index)
   {
      if (index < 0) return false;
      const CbmMCTrack* mcTrack = static_cast<const CbmMCTrack*>(mcTracks->At(index));
      const TParticlePDG* particle = TDatabasePDG::Instance()->GetParticle(mcTrack->GetPdgCode());
      if (particle == NULL) return false;
      return particle->Charge() < 0;
   }

   static Bool_t PositiveTrackAcceptanceFunction(
         const TClonesArray* mcTracks,
         Int_t index)
   {
      if (index < 0) return false;
      const CbmMCTrack* mcTrack = static_cast<const CbmMCTrack*>(mcTracks->At(index));
      const TParticlePDG* particle = TDatabasePDG::Instance()->GetParticle(mcTrack->GetPdgCode());
      if (particle == NULL) return false;
      return particle->Charge() > 0;
   }

   static Bool_t PrimaryTrackAcceptanceFunction(
         const TClonesArray* mcTracks,
         Int_t index)
   {
      if (index < 0) return false;
      const CbmMCTrack* mcTrack = static_cast<const CbmMCTrack*>(mcTracks->At(index));
      return (mcTrack->GetMotherId() == -1);
   }

   static Bool_t ReferenceTrackAcceptanceFunction(
         const TClonesArray* mcTracks,
         Int_t index)
   {
      if (index < 0) return false;
      const CbmMCTrack* mcTrack = static_cast<const CbmMCTrack*>(mcTracks->At(index));
      return (mcTrack->GetMotherId() == -1) && (mcTrack->GetP() > 1.);
   }

   static Bool_t SecondaryTrackAcceptanceFunction(
         const TClonesArray* mcTracks,
         Int_t index)
   {
      if (index < 0) return false;
      const CbmMCTrack* mcTrack = static_cast<const CbmMCTrack*>(mcTracks->At(index));
      return (mcTrack->GetMotherId() != -1);
   }

   static Bool_t PrimaryElectronTrackAcceptanceFunction(
         const TClonesArray* mcTracks,
         Int_t index)
   {
      if (index < 0) return false;
      const CbmMCTrack* mcTrack = static_cast<const CbmMCTrack*>(mcTracks->At(index));
      return (TMath::Abs(mcTrack->GetPdgCode()) == 11) && (mcTrack->GetMotherId() == -1);
   }

   static Bool_t PrimaryMuonTrackAcceptanceFunction(
         const TClonesArray* mcTracks,
         Int_t index)
   {
      if (index < 0) return false;
      const CbmMCTrack* mcTrack = static_cast<const CbmMCTrack*>(mcTracks->At(index));
      return (TMath::Abs(mcTrack->GetPdgCode()) == 13) && (mcTrack->GetMotherId() == -1);
   }

   static Bool_t ElectronTrackAcceptanceFunction(
         const TClonesArray* mcTracks,
         Int_t index)
   {
      if (index < 0) return false;
      const CbmMCTrack* mcTrack = static_cast<const CbmMCTrack*>(mcTracks->At(index));
      return TMath::Abs(mcTrack->GetPdgCode()) == 11;
   }

   static Bool_t MuonTrackAcceptanceFunction(
         const TClonesArray* mcTracks,
         Int_t index)
   {
      if (index < 0) return false;
      const CbmMCTrack* mcTrack = static_cast<const CbmMCTrack*>(mcTracks->At(index));
      return TMath::Abs(mcTrack->GetPdgCode()) == 13;
   }

   static Bool_t ProtonTrackAcceptanceFunction(
         const TClonesArray* mcTracks,
         Int_t index)
   {
      if (index < 0) return false;
      const CbmMCTrack* mcTrack = static_cast<const CbmMCTrack*>(mcTracks->At(index));
      return (TMath::Abs(mcTrack->GetPdgCode()) == 2212);
   }

   static Bool_t AntiProtonTrackAcceptanceFunction(
         const TClonesArray* mcTracks,
         Int_t index)
   {
      if (index < 0) return false;
      const CbmMCTrack* mcTrack = static_cast<const CbmMCTrack*>(mcTracks->At(index));
      return mcTrack->GetPdgCode() == -2212;
   }

   static Bool_t PionTrackAcceptanceFunction(
         const TClonesArray* mcTracks,
         Int_t index)
   {
      if (index < 0) return false;
      const CbmMCTrack* mcTrack = static_cast<const CbmMCTrack*>(mcTracks->At(index));
      return (TMath::Abs(mcTrack->GetPdgCode()) == 211);
   }

   static Bool_t PionPlusTrackAcceptanceFunction(
         const TClonesArray* mcTracks,
         Int_t index)
   {
      if (index < 0) return false;
      const CbmMCTrack* mcTrack = static_cast<const CbmMCTrack*>(mcTracks->At(index));
      return (mcTrack->GetPdgCode() == 211);
   }

   static Bool_t PionMinusTrackAcceptanceFunction(
         const TClonesArray* mcTracks,
         Int_t index)
   {
      if (index < 0) return false;
      const CbmMCTrack* mcTrack = static_cast<const CbmMCTrack*>(mcTracks->At(index));
      return (mcTrack->GetPdgCode() == -211);
   }

   static Bool_t KaonTrackAcceptanceFunction(
         const TClonesArray* mcTracks,
         Int_t index)
   {
      if (index < 0) return false;
      const CbmMCTrack* mcTrack = static_cast<const CbmMCTrack*>(mcTracks->At(index));
      return (TMath::Abs(mcTrack->GetPdgCode()) == 321);
   }

   static Bool_t KaonPlusTrackAcceptanceFunction(
         const TClonesArray* mcTracks,
         Int_t index)
   {
      if (index < 0) return false;
      const CbmMCTrack* mcTrack = static_cast<const CbmMCTrack*>(mcTracks->At(index));
      return (mcTrack->GetPdgCode() == 321);
   }

   static Bool_t KaonMinusTrackAcceptanceFunction(
         const TClonesArray* mcTracks,
         Int_t index)
   {
      if (index < 0) return false;
      const CbmMCTrack* mcTrack = static_cast<const CbmMCTrack*>(mcTracks->At(index));
      return (mcTrack->GetPdgCode() == -321);
   }

   static Bool_t AllRingAcceptanceFunction(
         const TClonesArray* mcTracks,
         Int_t index,
         Int_t nofHitsInRing)
   {
      return true;
   }

   static Bool_t AllReferenceRingAcceptanceFunction(
         const TClonesArray* mcTracks,
         Int_t index,
         Int_t nofHitsInRing)
   {
      if (index < 0) return false;
      const CbmMCTrack* mcTrack = static_cast<const CbmMCTrack*>(mcTracks->At(index));
      return (mcTrack->GetMotherId() == -1) && (mcTrack->GetP() > 1.) && (nofHitsInRing >= 15);
   }

   static Bool_t PrimaryElectronRingAcceptanceFunction(
         const TClonesArray* mcTracks,
         Int_t index,
         Int_t nofHitsInRing)
   {
      if (index < 0) return false;
      const CbmMCTrack* mcTrack = static_cast<const CbmMCTrack*>(mcTracks->At(index));
      return (mcTrack->GetMotherId() == -1) && (TMath::Abs(mcTrack->GetPdgCode()) == 11);
   }

   static Bool_t PrimaryElectronReferenceRingAcceptanceFunction(
         const TClonesArray* mcTracks,
         Int_t index,
         Int_t nofHitsInRing)
   {
      if (index < 0) return false;
      const CbmMCTrack* mcTrack = static_cast<const CbmMCTrack*>(mcTracks->At(index));
      return (mcTrack->GetMotherId() == -1) && (TMath::Abs(mcTrack->GetPdgCode()) == 11) && (mcTrack->GetP() > 1.) && (nofHitsInRing >= 15);
   }

   static Bool_t PionRingAcceptanceFunction(
         const TClonesArray* mcTracks,
         Int_t index,
         Int_t nofHitsInRing)
   {
      if (index < 0) return false;
      const CbmMCTrack* mcTrack = static_cast<const CbmMCTrack*>(mcTracks->At(index));
      return (TMath::Abs(mcTrack->GetPdgCode()) == 211);
   }

   static Bool_t PionReferenceRingAcceptanceFunction(
         const TClonesArray* mcTracks,
         Int_t index,
         Int_t nofHitsInRing)
   {
      if (index < 0) return false;
      const CbmMCTrack* mcTrack = static_cast<const CbmMCTrack*>(mcTracks->At(index));
      return (TMath::Abs(mcTrack->GetPdgCode()) == 211) && (mcTrack->GetP() > 1.) && (nofHitsInRing >= 15);
   }

   static Bool_t AllPiSuppAcceptanceFunction(
         const TClonesArray* globalTracks,
         const TClonesArray* stsMatches,
         const TClonesArray* richMatches,
         Int_t index)
   {
      return true;
   }

   static Bool_t TrueMatchPiSuppAcceptanceFunction(
         const TClonesArray* globalTracks,
         const TClonesArray* stsMatches,
         const TClonesArray* richMatches,
         Int_t index)
   {
//      const CbmGlobalTrack* gTrack = static_cast<const CbmGlobalTrack*>(globalTracks->At(index));
//      Int_t stsInd = gTrack->GetStsTrackIndex();
//      Int_t richInd = gTrack->GetRichRingIndex();
//      if (stsInd == -1 || richInd == -1) return false;
//      const BmnTrackMatch* stsMatch = static_cast<const BmnTrackMatch*>(stsMatches->At(stsInd));
//      const BmnTrackMatch* richMatch = static_cast<const BmnTrackMatch*>(richMatches->At(richInd));
//      if (NULL == stsMatch || NULL == richMatch) return false;
//
//      if (stsMatch->GetMatchedLink().GetIndex() == richMatch->GetMatchedLink().GetIndex()) return true;
      return false;
   }

   static Bool_t WrongMatchPiSuppAcceptanceFunction(
         const TClonesArray* globalTracks,
         const TClonesArray* stsMatches,
         const TClonesArray* richMatches,
         Int_t index)
   {
      return !TrueMatchPiSuppAcceptanceFunction(globalTracks, stsMatches, richMatches, index);
   }

};

#endif /* BMNACCEPTANCEFUNCTION_H_ */
