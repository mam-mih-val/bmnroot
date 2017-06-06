
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
#include "BmnGemStripStationSet.h"
#include "BmnGemStripStationSet_RunSpring2017.h"

using namespace std;

class BmnGemTrackFinder : public FairTask {
public:

    // Constructors/Destructors ---------
    BmnGemTrackFinder();
    virtual ~BmnGemTrackFinder();

    BmnStatus NearestHitMerge(UInt_t station, BmnGemTrack* tr, Bool_t goForward);
    Double_t CalculateLength(BmnGemTrack* tr);

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

    void SetTarget(Bool_t f) {
        fIsTarget = f;
    }
    
    void SetDirection(Bool_t dir) {
        fGoForward = dir;
    }
    
    void SetDistCut(Float_t dist) {
        fDistCut = dist;
    }
    
    void SetNHitsCut(Short_t n) {
        fNHitsCut = n;
    }

private:

    BmnGemStripStationSet* fDetector;

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
    Float_t fDistCut;
    Short_t fNHitsCut;

    Bool_t fGoForward;
    Bool_t fIsTarget;
    Bool_t fIsField;
    FairField* fField;
    BmnKalmanFilter_tmp* fKalman;
    Int_t fEventNo; // event counter

    ClassDef(BmnGemTrackFinder, 1);
};
