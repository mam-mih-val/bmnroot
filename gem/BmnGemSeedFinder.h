
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
    BmnStatus DoSeeding();
    TVector3 CircleFit(BmnGemTrack* track);
    TVector3 LineFit(BmnGemTrack* track);
    
    BmnStatus DoHistoTracking();
    BmnStatus FindSeedInYSlice(Int_t yAddr, Int_t yStep);

    void SetMakeQA(Bool_t qa) {
        fMakeQA = qa;
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

    Bool_t fMakeQA; // create or not in output tree branch with QA histograms
    Bool_t isHistogramsInitialized; // is QA histograms initialized or not
    BmnGemSeedFinderQA* fHisto; // pointer to object needed for QA creating

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
