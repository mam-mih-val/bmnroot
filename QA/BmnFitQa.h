/**
 * \file CbmLitFitQa.h
 * \brief Track fit QA for track reconstruction.
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2011
 */

#ifndef CBMLITFITQA_H_
#define CBMLITFITQA_H_

#include "FairTask.h"
#include "CbmStsKFTrackFitter.h"
#include "CbmDetectorList.h"
#include "cbm/base/CbmLitDetectorSetup.h"
#include <string>

class CbmHistManager;
class CbmVertex;
class FairTrackParam;
class CbmLitMCPoint;
class CbmLitMCTrackCreator;

using std::string;

/**
 * \class CbmLitFitQa
 * \brief Track fit QA for track reconstruction.
 *
 * Calculate residual and pull distributions for
 * first and last track parameters of MVD/STS, TRD and MUCH
 * for reconstructed tracks.
 *
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2011
 */
class CbmLitFitQa : public FairTask
{
public:
   /**
    * \brief Constructor.
    */
   CbmLitFitQa();

   /**
    * \brief Destructor.
    */
   virtual ~CbmLitFitQa();

   /**
    * \brief Inherited from FairTask.
    */
   virtual InitStatus Init();

   /**
    * \brief Inherited from FairTask.
    */
   virtual void Exec(
      Option_t* opt);

   /**
    * \brief Inherited from FairTask.
    */
   virtual void Finish();

   /* Setters */
   void SetMvdMinNofHits(Int_t minNofHits) { fMvdMinNofHits = minNofHits; }
   void SetStsMinNofHits(Int_t minNofHits) { fStsMinNofHits = minNofHits; }
   void SetTrdMinNofHits(Int_t minNofHits) { fTrdMinNofHits = minNofHits; }
   void SetMuchMinNofHits(Int_t minNofHits) { fMuchMinNofHits = minNofHits; }
   void SetOutputDir(const string& dir) { fOutputDir = dir; }
   void SetFixedBounds(Bool_t isFixedBounds) { fIsFixedBounds = isFixedBounds; }

   void SetPRange(Int_t bins, Int_t min, Int_t max) {
      fPRangeBins = bins;
      fPRangeMin = min;
      fPRangeMax = max;
   }

private:
   /**
   * \brief Reads data branches.
   */
   void ReadDataBranches();

   void ProcessGlobalTracks();

   void ProcessStsTrack(
       Int_t trackId);

   void ProcessTrdTrack(
       Int_t trackId);

   void ProcessMuchTrack(
       Int_t trackId);

   void FillResidualsAndPulls(
      const FairTrackParam* par,
      const CbmLitMCPoint* mcPoint,
      const string& histName,
      Float_t wrongPar,
      DetectorId detId);

   void FillTrackParamHistogramm(
         const string& histName,
         const FairTrackParam* par);

   void ProcessTrackParamsAtVertex();

   void CreateHistograms();

   void CreateResidualAndPullHistograms(
         DetectorId detId,
         const string& detName);

   void CreateTrackParamHistograms(
         DetectorId detId,
         const string& detName);

   Bool_t fIsFixedBounds; // if true than fixed bounds are used for histograms

   Int_t fMvdMinNofHits; // Cut on minimum number of hits in track in MVD
   Int_t fStsMinNofHits; // Cut on minimum number of hits in track in STS
   Int_t fTrdMinNofHits; // Cut on minimum number of hits in track in TRD
   Int_t fMuchMinNofHits; // Cut on minimum number of hits in track in MUCH

   string fOutputDir; // Output directory for images

   Double_t fPRangeMin; // Min momentum
   Double_t fPRangeMax; // Max momentum
   Int_t fPRangeBins; // Number of bins

   CbmHistManager* fHM; // Histogram manager

   // Data branches
   TClonesArray* fGlobalTracks; // CbmGlobalTrack array
   TClonesArray* fStsTracks; // CbmStsTrack array
   TClonesArray* fStsTrackMatches; // CbmTrackMatchNew array
   TClonesArray* fStsHits; // CbmStsHit
   TClonesArray* fMvdHits; // CbmMvdHit
   TClonesArray* fTrdTracks; // CbmTrdTrack array
   TClonesArray* fTrdTrackMatches; // CbmTrackMatchNew array
   TClonesArray* fTrdHits; // CbmTrdHit array
   TClonesArray* fMuchTracks; // CbmStsTrack array
   TClonesArray* fMuchTrackMatches; // CbmTrackMatchNew array
   TClonesArray* fMuchPixelHits; // CbmMuchPixelHit array
   TClonesArray* fMuchStripHits; // CbmMuchStripHit array
   TClonesArray* fMCTracks; // CbmMCTrack array

   Double_t fQuota; // percent of correctly attached hits

   CbmVertex* fPrimVertex; // Pointer to the primary vertex
   CbmStsKFTrackFitter fKFFitter; // Pointer to the Kalman Filter Fitter algorithm

   CbmLitMCTrackCreator* fMCTrackCreator; // MC track creator tool

   CbmLitDetectorSetup fDet; // For detector setup determination

   CbmLitFitQa(const CbmLitFitQa&);
   CbmLitFitQa& operator=(const CbmLitFitQa&);
   
   ClassDef(CbmLitFitQa, 1)
};

#endif /* CBMLITFITQA_H_ */
