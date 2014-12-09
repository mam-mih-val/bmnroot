/**
 * \file BmnFindGlobalTracks.h
 * \author Andrey Lebedev <andrey.lebedev@gsi.de> - Original author. First version of code for CBM experiment.
 * \author Sergey Merts <Sergey.Merts@gmail.com> - Modification for BMN experiment.
 * \date 2009-2014
 * \brief BMN task for global track reconstruction.
 *
 * Output is reconstructed global tracks CbmGlobalTrack 
 * Hit-to-track merger attaches the TOF hit finally the track is refitted.
 **/

#ifndef BMNFINDGLOBALTRACKS_H_
#define BMNFINDGLOBALTRACKS_H_

#include "FairTask.h"
#include "BmnDetectorSetup.h"
#include "BmnTrackFinder.h"
#include "BmnTrackFitter.h"
#include "BmnTrackPropagator.h"
#include "BmnHitToTrackMerger.h"
#include "BmnGlobalTrackingQA.h"
#include "TClonesArray.h"
#include "TStopwatch.h"
#include "BmnHitToTrackMerger.h"

#include <string>
#include <vector>

class TClonesArray;

using namespace std;

class BmnFindGlobalTracks : public FairTask {
public:
    /**
     * \brief Constructor.
     */
    BmnFindGlobalTracks();

    /**
     * \brief Destructor.
     */
    virtual ~BmnFindGlobalTracks();

    /**
     * \brief Inherited from FairTask.
     */
    virtual InitStatus Init();

    /**
     * \brief Inherited from FairTask.
     */
    virtual void Exec(Option_t* opt);

    /**
     * \brief Inherited from FairTask.
     */
    virtual void Finish();

    /* Setters */
    void SetTrackingType(const string& trackingType) {
        fTrackingType = trackingType;
    }

    void SetMergerType(const string& mergerType) {
        fMergerType = mergerType;
    }

    void SetFitterType(const string& fitterType) {
        fFitterType = fitterType;
    }
    
    void SetMakeQA(const Bool_t makeQA) {
        fMakeQA = makeQA;
    }

private:

    /*
     * \brief Calculate length of the global track
     */
    void CalculateLength();

    /**
     * \brief Run the track reconstruction
     */
    void RunTrackReconstruction();

    /**
     * \brief Print output stopwatch statistics for track-finder and hit-to-track merger.
     */
    void PrintStopwatchStatistics();

    /**
     * \brief Select tracks for further merging with TOF.
     */
    void SelectTracksForTofMerging();

    BmnDetectorSetup fDet;

    // INPUT ARRAYS

    TClonesArray* fStsTracks; // CbmStsTrack array
    TClonesArray* fStsHits; // CbmStsHit
    TClonesArray* fTofHits; // CbmTofHit array
    
    // OUTPUT ARRAYS
    
    TClonesArray* fTofTracks; // output CbmTofTrack array
    TClonesArray* fGlobalTracks; //output CbmGlobalTrack array

    // Tools
    BmnTrackFinder* fFinder; // track finder
    BmnHitToTrackMerger* fMerger; // hit-to-track merger
    BmnTrackFitter* fFitter; // track fitter
    // track propagator
    // Used to propagate STS track to the last STS station!!!
    // Since this cannot be done in parallel mode!!!
    BmnTrackPropagator* fPropagator;
    BmnKalmanFilter* fUpdater;

    // Settings
    // Tracking method to be used
    // "branch" - branching method
    // "nn" - nearest neighbor method
    TString fTrackingType;

    // Merger method to be used
    // "nearest_hit" - assigns nearest hit to the track
    TString fMergerType;

    // Track fitter to be used for the final track fit
    // "lit_kalman" - forward Kalman track fit with LIT propagation and TGeo navigation
    TString fFitterType;

    // stopwatches
    TStopwatch fTrackingWatch; // stopwatch for tracking
    TStopwatch fMergerWatch; // stopwatch for merger

    Int_t fEventNo; // event counter


    BmnGlobalTrackingQA *fHisto; // pointer to object needed only for QA creating
    Bool_t fIsHistogramsInitialized; // is QA histograms initialized or not
    Bool_t fMakeQA; // create or not in output tree branch with QA histograms

    BmnFindGlobalTracks(const BmnFindGlobalTracks&);
    BmnFindGlobalTracks& operator=(const BmnFindGlobalTracks&);

    ClassDef(BmnFindGlobalTracks, 1);
};

#endif /* BMNFINDGLOBALTRACKS_H_ */
