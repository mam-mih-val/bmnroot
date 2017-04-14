
// Base Class Headers ----------------

#include "FairTask.h"
#include "TClonesArray.h"
#include "TString.h"
#include "BmnGemTrack.h"
#include <iostream>
#include <vector>
#include "TH1F.h"
#include "TH2F.h"
#include "TArc.h"
#include "TMath.h"
#include "TVector3.h"
#include "TCanvas.h"
#include "FairTrackParam.h"
#include "BmnTrackPropagator.h"
#include "BmnKalmanFilter.h"
#include "BmnFitNode.h"
#include "BmnGemHit.h"
#include "BmnGemTrackFinderQA.h"
#include "BmnKalmanFilter_tmp.h"
#include "BmnTrackFitter.h"
#include "BmnMath.h"

using namespace std;

class BmnGemTrackFinder : public FairTask {
public:

    // Constructors/Destructors ---------
    BmnGemTrackFinder();
    virtual ~BmnGemTrackFinder();

    BmnStatus NearestHitMerge(UInt_t station, BmnGemTrack* tr);
    BmnStatus NearestHitMerge1(UInt_t station, BmnGemTrack* tr);

    BmnStatus FitSmooth(BmnGemTrack* track);
    void Smooth(BmnFitNode* thisNode, const BmnFitNode* prevNode);

    //some useful functions
    Float_t Dist(Float_t x1, Float_t y1, Float_t x2, Float_t y2);
    Float_t Sqr(Float_t x);
    BmnHit* GetHit(Int_t i);

    BmnStatus CheckSplitting(TClonesArray* arr);
    BmnStatus ConnectNearestSeed(BmnGemTrack* seed, TClonesArray* arr);

    virtual InitStatus Init();
    virtual void Exec(Option_t* opt);
    virtual void Finish();

    void SetField(Bool_t f) {
        fIsField = f;
    }

private:

    // Private Data Members ------------
    TString fHitsBranchName;
    TString fSeedsBranchName;
    TString fTracksBranchName;

    TClonesArray* fGemHitArray;
    TClonesArray* fGemSeedsArray;
    TClonesArray* fGemTracksArray;

    /* Track propagation tool */
    BmnTrackPropagator* fPropagator;
    /* Track update tool */
    BmnKalmanFilter* fUpdate;
    Int_t fPDG; // PDG hypothesis
    Float_t fChiSqCut; // Chi square cut for hit to be attached to track.

    Bool_t fIsField;
    FairField* fField;
    BmnKalmanFilter_tmp* fKalman;
    Int_t fEventNo; // event counter

    ClassDef(BmnGemTrackFinder, 1);
};
