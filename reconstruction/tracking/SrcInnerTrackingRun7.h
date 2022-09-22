/* 
 * File:   SrcInnerTrackingRun7.h
 * Author: Sergei Merts
 *
 * Created on April 6, 2020
 * 
 * 
 */

#ifndef SRCINNERTRACKINGRUN7_H
#define SRCINNERTRACKINGRUN7_H

// Base Class Headers ----------------

#include <map>
#include <vector>

#include "BmnCellDuet.h"
#include "BmnGemStripStationSet.h"
#include "BmnGlobalTrack.h"
#include "BmnKalmanFilter.h"
#include "BmnMath.h"
#include "BmnSteering.h"
#include "BmnTrack.h"
#include "CbmMCTrack.h"
#include "FairMCPoint.h"
#include "FairTask.h"
#include "FairTrackParam.h"
#include "TClonesArray.h"
#include "TString.h"

using namespace std;

class SrcInnerTrackingRun7 : public FairTask {
   public:
    // Constructors/Destructors ---------

    SrcInnerTrackingRun7(){};
    SrcInnerTrackingRun7(Int_t run, Bool_t target);
    virtual ~SrcInnerTrackingRun7();

    virtual InitStatus Init();
    virtual void Exec(Option_t* opt);
    virtual void Finish();

   private:
    BmnStatus SortHits(vector<BmnHit*>* sortedHits);

    BmnStatus FindTracks_3of6();
    BmnStatus FindTracks_6of6();
    BmnStatus FindTracks_5of6();
    BmnStatus FindTracks_4of6();

    BmnStatus MatchHit(BmnTrack* cand, vector<BmnHit*> sortedHits, Bool_t downstream);
    //BmnStatus FindCandidateByFourStations(Short_t st0, Short_t st1, Short_t st2, Short_t st3, Short_t skip0, Short_t skip1);
    BmnStatus FindCandidateByThreeStations(Short_t st0, Short_t st1, Short_t st2, vector<BmnTrack>& candidates, vector<BmnHit*>* sortedHits);
    BmnStatus FindCandidateByFourStations(Short_t st0, Short_t st1, Short_t st2, Short_t st3, vector<BmnTrack>& candidates, vector<BmnHit*>* sortedHits);
    BmnStatus FindCandidateByFiveStations(Short_t st0, Short_t st1, Short_t st2, Short_t st3, Short_t st4, vector<BmnTrack>& candidates, vector<BmnHit*>* sortedHits);

    BmnStatus TrackUpdateByKalman(vector<BmnTrack>& cands);
    BmnStatus TrackUpdateByLine(vector<BmnTrack>& cands);
    BmnStatus SortTracks(vector<BmnTrack>& inTracks, vector<BmnTrack>& sortedTracks);
    BmnStatus TrackSelection(vector<BmnTrack>& cands);

    Double_t CalcQp(BmnTrack* track, Bool_t doSimple);
    BmnStatus CalculateTrackParams(BmnTrack* tr);
    BmnStatus CalcCovMatrix(BmnTrack* tr);
    TVector2 CalcMeanSigma(vector<Double_t>);
    Double_t CalculateLength(BmnTrack* tr);
    BmnStatus CheckSharedHits(vector<BmnTrack>& sortedTracks);
    void SetHitsUsing(BmnTrack* tr, Bool_t use);
    BmnStatus DrawHits();

    BmnGemStripStationSet* fGemDetector;     //!
    TString fGemHitsBranchName;
    TString fGlobTracksBranchName;
    TString fGemTracksBranchName;

    TClonesArray* fGlobTracksArray;          //!
    TClonesArray* fGemTracksArray;           //!
    TClonesArray* fGemHitsArray;             //!
    TClonesArray* fHitsArray;                //!

    TClonesArray* fMCTracksArray;            //!
    TClonesArray* fGemPointsArray;           //!

    BmnKalmanFilter* fKalman;                //!

    Bool_t fIsTarget;  // run with target or not

    UInt_t fEventNo;
    Short_t fNStations;

    FairField* fField;                       //!

    Double_t fChiSquareCut;
    Double_t* fHitXCutMin;                   //!
    Double_t* fHitXCutMax;                   //!
    Double_t* fHitYCutMin;                   //!
    Double_t* fHitYCutMax;                   //!

    Int_t fNHitsCut;
    Double_t fDistCut;

    TString fSteerFile;
    BmnSteering* fSteering;                  //!

    ClassDef(SrcInnerTrackingRun7, 1);
};

#endif /* SRCINNERTRACKINGRUN7_H */
