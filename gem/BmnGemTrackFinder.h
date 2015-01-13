
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

using namespace std;

class BmnGemTrackFinder : public FairTask {
public:

    // Constructors/Destructors ---------
    BmnGemTrackFinder();
    virtual ~BmnGemTrackFinder();
    
    BmnStatus Refit(BmnGemTrack*);
    BmnStatus NearestHitMerge(UInt_t station, BmnGemTrack* tr);
    
    //some useful functions
    Float_t Dist(Float_t x1, Float_t y1, Float_t x2, Float_t y2);
    Float_t Sqr(Float_t x);
    BmnHit* GetHit(Int_t i);

    virtual InitStatus Init();
    virtual void Exec(Option_t* opt);
    virtual void Finish();

private:

    // Private Data Members ------------
    TString fHitsBranchName;
    TString fSeedsBranchName;
    TString fTracksBranchName;

    TClonesArray* fGemHitArray;
    TClonesArray* fGemSeedsArray;
    TClonesArray* fGemTracksArray;
    TClonesArray* fMCTracksArray;
    TClonesArray* fMCPointsArray;

    /* Track propagation tool */
    BmnTrackPropagator* fPropagator;
    /* Track update tool */
    BmnKalmanFilter* fUpdate;
    Int_t fPDG; // PDG hypothesis
    Float_t fChiSqCut; // Chi square cut for hit to be attached to track.
    
    FairField* fField;
    Int_t fEventNo; // event counter
        
    Bool_t fMakeQA; // create or not in output tree branch with QA histograms
    Bool_t isHistogramsInitialized; // is QA histograms initialized or not
    BmnGemTrackFinderQA* fHisto; // pointer to object needed for QA creating

    ClassDef(BmnGemTrackFinder, 1);
};
