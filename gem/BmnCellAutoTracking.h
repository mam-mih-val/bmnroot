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
#include "BmnGemTrack.h"
#include <vector>
#include <map>
#include "FairTrackParam.h"
#include "BmnKalmanFilter.h"
#include "BmnMath.h"
#include "BmnCellDuet.h"
#include "BmnGemStripStationSet.h"

using namespace std;

class BmnCellAutoTracking : public FairTask {
public:

    // Constructors/Destructors ---------

    BmnCellAutoTracking() {
    };
    BmnCellAutoTracking(Short_t period, Bool_t field, Bool_t target);
    virtual ~BmnCellAutoTracking();

    virtual InitStatus Init();
    virtual void Exec(Option_t* opt);
    virtual void Finish();

private:

    BmnStatus CellsCreation(vector<BmnCellDuet>* cells);
    BmnStatus StateCalculation(vector<BmnCellDuet>* cells);
    BmnStatus CellsConnection(vector<BmnCellDuet>* cells, vector<BmnGemTrack>& cands);
    BmnStatus TrackUpdateByKalman(vector<BmnGemTrack>& cands);
    void TrackUpdateByLine(vector<BmnGemTrack>& cands);
    BmnStatus SortTracks(vector<BmnGemTrack>& inTracks, vector<BmnGemTrack>& sortedTracks);
    BmnStatus TrackSelection(vector<BmnGemTrack>& cands);
    
    Double_t CalcQp(BmnGemTrack* track);
    BmnStatus CalculateTrackParams(BmnGemTrack* tr);    
    BmnStatus CalcCovMatrix(BmnGemTrack * tr);    
    TVector2 CalcMeanSigma(vector <Double_t>);
    Double_t CalculateLength(BmnGemTrack* tr);
    BmnStatus CheckSharedHits(vector<BmnGemTrack>& sortedTracks);
    void SetHitsUsing(BmnGemTrack* tr, Bool_t use);
    BmnStatus DrawHits();
    
    BmnGemStripStationSet* fGemDetector;
    TString fGemHitsBranchName;
    TString fTracksBranchName;

    TClonesArray* fGemTracksArray;
    TClonesArray* fGemHitsArray;
    BmnKalmanFilter* fKalman;

    Double_t* fGemDistCut;

    Bool_t fIsField; // run with mag.field or not
    Bool_t fIsTarget; // run with target or not

    UInt_t fEventNo;
    Short_t fPeriodId;

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
    
    Double_t fCellDiffSlopeYZCut;
    Double_t fCellDiffSlopeXZCut;
    Int_t fNHitsCut;

    vector<Int_t>* fHitsOnStation;

    ClassDef(BmnCellAutoTracking, 1);
};


#endif /* BMNCELLAUTOTRACKING_H */

