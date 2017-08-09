
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
#include "BmnGemStripStationSet.h"
#include "BmnGemStripStationSet_RunSpring2017.h"

using namespace std;

class BmnGemSeedFinder : public FairTask {
public:

    // Constructors/Destructors ---------
    BmnGemSeedFinder();
    virtual ~BmnGemSeedFinder();
    void SearchTrackCandInLine(const Int_t i, const Int_t y, BmnGemTrack* tr, Int_t* hitCntr, Int_t* maxDist, Int_t* dist, Int_t* startBin, Int_t* prevStation, Int_t gate, Bool_t isIdeal);
    Bool_t CalculateTrackParamsSpiral(BmnGemTrack* tr);
    Bool_t CalculateTrackParamsParabolicSpiral(BmnGemTrack* tr, TLorentzVector* spirPar, TVector3* linePar, Short_t q);

    BmnStatus FindSeeds(vector<BmnGemTrack>& cand);
    BmnStatus FindSeedsCombinatorics(vector<BmnGemTrack>& cand);
    BmnStatus FindSeedsByCombinatoricsInCoridor(Int_t iCorridor, BmnGemTrack* cand);
    BmnStatus SeedsByThreeStations(Int_t i0, Int_t i1, Int_t i2, vector<Int_t>* hits, BmnGemTrack* cand);
    BmnStatus FitSeeds(BmnGemTrack* cand);
    BmnStatus CalculateTrackParamsLine(BmnGemTrack* tr);
    BmnStatus CalculateTrackParamsCircle(BmnGemTrack* tr);
    BmnStatus CalculateTrackParamsPol2(BmnGemTrack* tr);

    void SetHitsUnused(BmnGemTrack* tr);

    void FillAddr();
    void FillAddrWithLorentz();

    void SetUseLorentz(Bool_t use) {
        fUseLorentz = use;
    }

    BmnGemStripStationSet* fDetector;

    //some useful functions
    Float_t Dist(Float_t x1, Float_t y1, Float_t x2, Float_t y2);
    BmnGemStripHit* GetHit(Int_t i);

    TVector2 GetTransXY(BmnGemStripHit* hit);

    void SetLorentzThresh(Float_t trs) {
        fLorentzThresh = trs;
    }

    void SetYstep(Float_t stp) {
        fYstep = stp;
    }

    void SetSigX(Float_t sig) {
        fSigX = sig;
    }

    void SetNbins(Int_t n) {
        fNBins = n;
    }

    void SetField(Bool_t f) {
        fIsField = f;
    }

    void SetDirection(Bool_t dir) {
        fGoForward = dir;
    }

    void SetTarget(Bool_t f) {
        fIsTarget = f;
    }

    void SetXRange(Float_t xMin, Float_t xMax) {
        fXmax = xMax;
        fXmin = xMin;
    }

    void SetYRange(Float_t yMin, Float_t yMax) {
        fYmax = yMax;
        fYmin = yMin;
    }

    void AddStationToSkip(Short_t st) {
        skipStations.push_back(st);
    }

    void SetRoughVertex(TVector3 v) {
        fRoughVertex = v;
    }
    
    void SetLineFitCut(Double_t cut) {
        fLineFitCut = cut;
    }    
    
    void SetNHitsCut(Short_t n) {
        fNHitsCut = n;
    }

    virtual InitStatus Init();
    virtual void Exec(Option_t* opt);
    virtual void Finish();

private:

    // Private Data Members ------------
    TString fHitsBranchName;
    TString fSeedsBranchName;

    Bool_t fUseLorentz; //flag for using Lorentz filtration
    Bool_t fIsField; // run with mag.field or not
    Bool_t fIsTarget; // run with target or not
    Bool_t fGoForward;

    Float_t fSigX;
    UInt_t fYstep;
    Float_t fLorentzThresh;
    UInt_t fNHitsCut;
    UInt_t fNHitsInGemCut;
    UInt_t fNSeedsCut;
    UInt_t fNFoundSeeds;

    UInt_t fNBins; // number of bins in histogram  
    Float_t fMin;
    Float_t fMax;
    Float_t fWidth;

    UInt_t fEventNo; // event counter

    TVector3 fRoughVertex; // for correct transformation

    //ranges for seed finder
    Float_t fXmin;
    Float_t fXmax;
    Float_t fYmin;
    Float_t fYmax;

    vector<Short_t> skipStations;

    TClonesArray* fGemHitsArray;
    TClonesArray* fGemSeedsArray;

    FairField* fField;

    Int_t** fAddresses;
    Double_t fLineFitCut;

    ClassDef(BmnGemSeedFinder, 1);
};
