/* 
 * File:   BmnInnerTrackingRun7.h
 * Author: Sergei Merts
 *
 * Created on April 6, 2020
 * 
 * 
 */

#ifndef BMNINNERTRACKINGRUN7_H
#define BMNINNERTRACKINGRUN7_H

// Base Class Headers ----------------

#include <map>
#include <vector>

#include "BmnCellDuet.h"
#include "BmnGemStripStationSet.h"
#include "BmnGlobalTrack.h"
#include "BmnKalmanFilter.h"
#include "BmnMath.h"
#include "BmnSiliconStationSet.h"
#include "BmnSteering.h"
#include "BmnTrack.h"
#include "CbmMCTrack.h"
#include "FairMCPoint.h"
#include "FairTask.h"
#include "FairTrackParam.h"
#include "TClonesArray.h"
#include "TString.h"

using namespace std;

class BmnInnerTrackingRun7 : public FairTask {
   public:
    // Constructors/Destructors ---------

    BmnInnerTrackingRun7(){};
    BmnInnerTrackingRun7(Int_t run, Bool_t target);
    void SetFiltration(Bool_t f) {
        fDoHitAsymFiltration = f;
    }
    virtual ~BmnInnerTrackingRun7();

    virtual InitStatus Init();
    virtual void Exec(Option_t* opt);
    virtual void Finish();

   private:
    BmnStatus SortHits(vector<BmnHit*>* sortedHits);

    BmnStatus FindTracks_4of4_OnLastGEMStations();
    BmnStatus FindTracks_3of4_OnLastGEMStations();
    BmnStatus FindTracks_2of2_OnFirstGEMStationsDownstream();
    BmnStatus FindTracks_2of2_OnFirstGEMStationsUpstream();

    BmnStatus MatchHit(BmnTrack* cand, vector<BmnHit*> sortedHits, Bool_t downstream);
    BmnStatus FindCandidateByThreeStations(Short_t st0, Short_t st1, Short_t st2, vector<BmnTrack>& candidates, vector<BmnHit*>* sortedHits);
    BmnStatus FindCandidateByTwoStations(Short_t st0, Short_t st1, vector<BmnTrack>& candidates, vector<BmnHit*>* sortedHits);

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

    BmnGemStripStationSet* fGemDetector;        //!
    BmnSiliconStationSet* fSilDetector;         //!
    TString fGemHitsBranchName;
    TString fSilHitsBranchName;
    TString fGlobTracksBranchName;
    TString fGemTracksBranchName;
    TString fSilTracksBranchName;

    TClonesArray* fGlobTracksArray;             //!
    TClonesArray* fSilTracksArray;              //!
    TClonesArray* fGemTracksArray;              //!
    TClonesArray* fSilHitsArray;                //!
    TClonesArray* fGemHitsArray;                //!
    TClonesArray* fHitsArray;                   //!

    TClonesArray* fMCTracksArray;               //!
    TClonesArray* fSilPointsArray;              //!
    TClonesArray* fGemPointsArray;              //!

    BmnKalmanFilter* fKalman;                   //!

    Bool_t fIsTarget;  // run with target or not
    Bool_t fDoHitAsymFiltration;

    UInt_t fEventNo;
    Short_t fNStations;
    Short_t fNSiliconStations;

    TVector3 fRoughVertex;  // for correct transformation

    FairField* fField;                          //!

    Double_t fChiSquareCut;
    Double_t* fHitXCutMin;                      //!
    Double_t* fHitXCutMax;                      //!
    Double_t* fHitYCutMin;                      //!
    Double_t* fHitYCutMax;                      //!

    Int_t fNHitsCut;
    Double_t fDistCut;

    TString fSteerFile;
    BmnSteering* fSteering;                     //!

    ClassDef(BmnInnerTrackingRun7, 1);
};

#endif /* BMNINNERTRACKINGRUN7_H */
