/*
 * File:   BmnBeamTracking.h
 * Author: Sergei Merts
 *
 * Created on December 10, 2021
 *
 *
 */

#ifndef BMNBEAMTRACKING_H
#define BMNBEAMTRACKING_H

#include <map>
#include <vector>

#include "BmnKalmanFilter.h"
#include "BmnMath.h"
#include "BmnSiBTStationSet.h"
#include "BmnSiBTHit.h"
#include "BmnTrack.h"
#include "FairTask.h"
#include "FairTrackParam.h"
#include "TClonesArray.h"
#include "TString.h"

class BmnBeamTracking : public FairTask {
public:
    // Constructors/Destructors ---------

    BmnBeamTracking() {};
    BmnBeamTracking(Int_t run);

    virtual ~BmnBeamTracking();

    virtual InitStatus Init();
    virtual void Exec(Option_t* opt);
    virtual void Finish();

private:
    BmnStatus SortHits(vector<BmnSiBTHit*>* sortedHits);
    BmnStatus FindTracks();
    BmnStatus TrackUpdateByKalman(BmnTrack* cand);
    BmnStatus CalculateTrackParams(BmnTrack* tr);
    Double_t CalcQp(BmnTrack* track);
    BmnStatus CalcCovMatrix(BmnTrack* tr);

    BmnSiBTStationSet* fSiBTDetector;       //!
    TString fSiBTHitsBranchName;
    TString fBeamTracksBranchName;

    //output array of Beam tracks
    TClonesArray* fBeamTracksArray;         //!
    //input array of hits
    TClonesArray* fSiBTHitsArray;           //!

    BmnKalmanFilter* fKalman;               //!
    UInt_t fEventNo;
    FairField* fField;                      //!

  ClassDef(BmnBeamTracking, 1);
};

#endif /* BMNBEAMTRACKING_H */
