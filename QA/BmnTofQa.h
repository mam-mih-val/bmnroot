/**
 * \file CbmLitTofQa.h
 * \brief Task for TOF QA.
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2013
 */

#ifndef CBMLITTOFQA_H_
#define CBMLITTOFQA_H_

#include "FairTask.h"
#include "CbmStsKFTrackFitter.h"
#include <string>
#include <map>
#include <vector>
#include <set>

class CbmHistManager;
class CbmVertex;
class FairTrackParam;

using std::string;
using std::map;
using std::vector;
using std::set;

/**
 * \class CbmLitTofQa
 * \brief  Task for TOF QA.
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2013
 */
class CbmLitTofQa : public FairTask
{
public:
	typedef Bool_t (*LitTrackAcceptanceFunction)(const TClonesArray* mcTracks, Int_t index);
   /**
    * \brief Constructor.
    */
   CbmLitTofQa();

   /**
    * \brief Destructor.
    */
   virtual ~CbmLitTofQa();

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
   void SetOutputDir(const string& dir) { fOutputDir = dir; }

   /**
    * \brief Set momentum range and number of bins for histograms.
    */
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

   /**
    * \brief Assign default track categories and track acceptance functions.
    */
   void FillTrackCategoriesAndAcceptanceFunctions();

   /**
    * \brief Creates histograms.
    */
   void CreateHistograms();

   void ProcessMC();

   void ProcessGlobalTracks();

   void ProcessTofHits();

   void ProcessTofTracks();

   void FitHistograms();

   CbmLitTofQa(const CbmLitTofQa&) = delete;
   CbmLitTofQa& operator=(const CbmLitTofQa&) = delete;

   Bool_t fIsFixedBounds; // if true than fixed bounds are used for histograms

   string fOutputDir; // Output directory for images

   Double_t fPRangeMin; // Min momentum
   Double_t fPRangeMax; // Max momentum
   Int_t fPRangeBins; // Number of bins

   CbmHistManager* fHM; // Histogram manager

   // Data branches
   TClonesArray* fGlobalTracks; // CbmGlobalTrack array
   TClonesArray* fStsTracks; // CbmStsTrack array
   TClonesArray* fStsTrackMatches; // CbmTrackMatchNew array
   TClonesArray* fTofHits; // CbmTofHit array
   TClonesArray* fTofPoints; // CbmTofPoint array
   TClonesArray* fTofTracks; // CbmTofTrack array
   TClonesArray* fMCTracks; // CbmMCTrack array

   CbmVertex* fPrimVertex; // Pointer to the primary vertex
   CbmStsKFTrackFitter fKFFitter; // Pointer to the Kalman Filter Fitter algorithm

   vector<string> fTrackCategories; // Vector of track category names
   map<string, LitTrackAcceptanceFunction> fTrackAcceptanceFunctions; // maps track category name to track acceptance function

   set<Int_t> fMCTrackIdForTofHits; // Set of MC track IDs for all existing TOF hits
   set<Int_t> fMCTrackIdForTofPoints; // Set of MC track IDs for all existing TOF points

   ClassDef(CbmLitTofQa, 1)
};

#endif /* CBMLITTOFQA_H_ */

