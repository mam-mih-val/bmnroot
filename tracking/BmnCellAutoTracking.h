/* 
 * File:   BmnCellAutoTracking.h
 * Author: Sergey Merts
 *
 * Created on July 30, 2018, 11:29 AM
 * 
 * Attempt to implement Cellular Automaton Tracking.
 */

#ifndef BMNCELLAUTOTRACKING_H
#define BMNCELLAUTOTRACKING_H


// Base Class Headers ----------------

#include "FairTask.h"
#include "TClonesArray.h"
#include "TString.h"
#include "BmnTrack.h"
#include "BmnGlobalTrack.h"
#include <vector>
#include <map>
#include "FairTrackParam.h"
#include "BmnKalmanFilter.h"
#include "BmnMath.h"
#include "BmnCellDuet.h"
#include "BmnGemStripStationSet.h"
#include "BmnSiliconStationSet.h"
#include "FairMCPoint.h"
#include "CbmMCTrack.h"
#include "BmnSteering.h"

using namespace std;

class BmnCellAutoTracking : public FairTask {
public:

    // Constructors/Destructors ---------

    BmnCellAutoTracking() {};
    BmnCellAutoTracking(Short_t period, Int_t run, Bool_t field, Bool_t target, TString steerFile = "");
    virtual ~BmnCellAutoTracking();

    virtual InitStatus Init();
    virtual void Exec(Option_t* opt);
    virtual void Finish();
    
    void SetInnerTrackerSetup(map<DetectorId, Bool_t> setup) {
        fInnerTrackerSetup = setup;
    };

    void SetDetectorPresence(DetectorId det, Bool_t use) {
        fInnerTrackerSetup[det] = use;
    };

private:

    BmnStatus CellsCreation(vector<BmnCellDuet>* cells);
    BmnStatus StateCalculation(vector<BmnCellDuet>* cells);
    vector<BmnTrack> CellsConnection(vector<BmnCellDuet>* cells);
    BmnStatus TrackUpdateByKalman(vector<BmnTrack>& cands);
    BmnStatus TrackUpdateByLine(vector <BmnTrack>& cands);
    BmnStatus SortTracks(vector<BmnTrack>& inTracks, vector<BmnTrack>& sortedTracks);
    BmnStatus TrackSelection(vector<BmnTrack>& cands);

    Double_t CalcQp(BmnTrack* track);
    BmnStatus CalculateTrackParams(BmnTrack* tr);
    BmnStatus CalcCovMatrix(BmnTrack * tr);
    TVector2 CalcMeanSigma(vector <Double_t>);
    Double_t CalculateLength(BmnTrack* tr);
    BmnStatus CheckSharedHits(vector<BmnTrack>& sortedTracks);
    void SetHitsUsing(BmnTrack* tr, Bool_t use);
    BmnStatus DrawHits();

    BmnGemStripStationSet* fGemDetector;
    BmnSiliconStationSet* fSilDetector;
    TString fGemHitsBranchName;
    TString fSilHitsBranchName;
    TString fSsdHitsBranchName;
    TString fGlobTracksBranchName;
    TString fGemTracksBranchName;
    TString fSilTracksBranchName;
    TString fSsdTracksBranchName;

    TClonesArray* fGlobTracksArray;
    TClonesArray* fSilTracksArray;
    TClonesArray* fGemTracksArray;
    TClonesArray* fSsdTracksArray;
    TClonesArray* fSilHitsArray;
    TClonesArray* fSsdHitsArray;
    TClonesArray* fGemHitsArray;
    TClonesArray* fHitsArray;
    
    TClonesArray* fMCTracksArray;
    TClonesArray* fSilPointsArray;
    TClonesArray* fSsdPointsArray;
    TClonesArray* fGemPointsArray;

    BmnKalmanFilter* fKalman;

    Bool_t fIsField; // run with mag.field or not
    Bool_t fIsTarget; // run with target or not
    map<DetectorId, Bool_t> fInnerTrackerSetup;

    UInt_t fEventNo;
    Short_t fPeriodId;
    Short_t fNStations;
    Short_t fNSiliconStations;

    TVector3 fRoughVertex; // for correct transformation

    FairField* fField;

    Double_t fChiSquareCut;
    Double_t* fCellDistCut;
    Double_t* fHitXCutMin;
    Double_t* fHitXCutMax;
    Double_t* fHitYCutMin;
    Double_t* fHitYCutMax;
    Double_t* fCellSlopeXZCutMin;
    Double_t* fCellSlopeXZCutMax;
    Double_t* fCellSlopeYZCutMin;
    Double_t* fCellSlopeYZCutMax;

    Double_t* fCellDiffSlopeYZCut;
    Double_t* fCellDiffSlopeXZCut;
    Int_t fNHitsCut;
    
    TString fSteerFile; 
    BmnSteering* fSteering;

    Bool_t isSRC;
    
    size_t kCellsCut;
    
    ClassDef(BmnCellAutoTracking, 1);
};


#endif /* BMNCELLAUTOTRACKING_H */

