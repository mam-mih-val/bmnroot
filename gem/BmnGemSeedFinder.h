
// Base Class Headers ----------------

#include "FairTask.h"
#include "TClonesArray.h"
#include "TString.h"
#include "BmnGemTrack.h"
#include <iostream>
#include <vector>
#include "TMath.h"
#include "TVector3.h"
#include "BmnGemSeedFinderQA.h"
#include "BmnGemStripHit.h"
#include "FairMCPoint.h"
#include "CbmMCTrack.h"
#include "CbmStsPoint.h"
#include "BmnEnums.h"
#include "FairRunAna.h"
#include "FairField.h"

using namespace std;

class BmnGemSeedFinder : public FairTask {
public:

    // Constructors/Destructors ---------
    BmnGemSeedFinder();
    virtual ~BmnGemSeedFinder();

    void FindXYRSeeds(TH1F* h);
    void FindSeeds(Int_t station, Int_t gate, Bool_t isIdeal);
    UInt_t SearchTrackCandidates(Int_t startStation, Int_t gate, Bool_t isIdeal, Bool_t isLeft);
    void SearchTrackCandInLine(const Int_t i, const Int_t y, BmnGemTrack* tr, Int_t* hitCntr, Int_t* maxDist, Int_t* dist, Int_t* startBin, Int_t* prevStation, Int_t gate, Bool_t isIdeal);
    Bool_t CalculateTrackParams(BmnGemTrack* tr, TVector3 circPar, TVector3 linePar);
    Bool_t CalculateTrackParamsSpiral(BmnGemTrack* tr, TVector3 spirPar, TVector3 linePar);
    BmnStatus DoSeeding();
    TVector3 CircleFit(BmnGemTrack* track);
    TVector3 LineFit(BmnGemTrack* track);
    TVector3 CircleBy3Hit(BmnGemTrack* track);
    TVector3 SpiralFit(BmnGemTrack* track);
    
    Bool_t IsParCorrect(const FairTrackParam* par);
    
    Float_t NewtonSolver(Float_t A0, Float_t A1, Float_t A2, Float_t A22);
    
    void FillAddr();
    void FillAddrWithLorentz(Float_t sigma_x, Float_t yStep, Float_t trs);
    
    void SetUseLorentz(Bool_t use) {
        fUseLorentz = use;
    }

    //some useful functions
    Float_t Dist(Float_t x1, Float_t y1, Float_t x2, Float_t y2);
    Float_t Sqr(Float_t x);
    BmnGemStripHit* GetHit(Int_t i);

    virtual InitStatus Init();
    virtual void Exec(Option_t* opt);
    virtual void Finish();

private:

    // Private Data Members ------------
    TString fHitsBranchName;
    TString fSeedsBranchName;

    Bool_t fUseLorentz; //flag for using Lorentz filtration
    
    Int_t fNBins; // number of bins in histogram  
    Float_t fMin;
    Float_t fMax;
    Float_t fWidth;

    Int_t fEventNo; // event counter

    TClonesArray* fGemHitsArray;
    TClonesArray* fGemSeedsArray;
    TClonesArray* fMCTracksArray;
    TClonesArray* fMCPointsArray;

    FairField* fField;

    ClassDef(BmnGemSeedFinder, 1);
};
