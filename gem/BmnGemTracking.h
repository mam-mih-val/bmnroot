
// Base Class Headers ----------------

#include "FairTask.h"
#include "TClonesArray.h"
#include "TString.h"
#include "BmnGemTrack.h"
#include <iostream>
#include <vector>
#include <map>
#include "TH1F.h"
#include "TH2F.h"
#include "TArc.h"
#include "TMath.h"
#include "TVector3.h"
#include "FairTrackParam.h"
#include "BmnFitNode.h"
#include "BmnKalmanFilter.h"
#include "BmnMath.h"
#include "BmnGemStripStationSet.h"
#include "BmnGemStripStationSet_RunSpring2017.h"
#include "FitWLSQ.h"
#include "BmnSteeringGemTracking.h"

using namespace std;

class BmnGemTracking : public FairTask {
public:

    // Constructors/Destructors ---------
    BmnGemTracking() {};
    BmnGemTracking(Short_t period, Bool_t field, Bool_t target, TString steerFile);
    virtual ~BmnGemTracking();

    virtual InitStatus Init();
    virtual void Exec(Option_t* opt);
    virtual void Finish();

    BmnStatus FindSeedsByCombinatoricsInCoridor(Int_t iCorridor, vector<BmnGemTrack>& cand);
    BmnStatus SeedsByThreeStations(Int_t i0, Int_t i1, Int_t i2, vector<Int_t>* hits, vector<BmnGemTrack>& cand);
    BmnStatus FitSeeds(vector<BmnGemTrack>& cand);
    BmnStatus CalculateTrackParamsLine(BmnGemTrack* tr);
    BmnStatus CalculateTrackParamsCircle(BmnGemTrack* tr);
    BmnStatus CalculateTrackParamsPol2(BmnGemTrack *tr);
    BmnStatus NearestHitMergeGem(UInt_t station, BmnGemTrack* tr, Bool_t& wasSkipped);
    Double_t CalculateLength(BmnGemTrack* tr);
    void SetHitsUsing(BmnGemTrack* tr, Bool_t use);
    void FillAddrWithLorentz();
    Int_t Tracking(vector<BmnGemTrack>& seeds);
    BmnGemStripHit* GetHit(Int_t i);
    BmnStatus RefitTrack(BmnGemTrack* track);
    TVector2 GetTransXY(BmnGemStripHit* hit);
    BmnStatus SortTracks(vector<BmnGemTrack>& inTracks, vector<BmnGemTrack>& sortedTracks);
    BmnStatus CheckSharedHits(vector<BmnGemTrack>& sortedTracks);
    
    void SetLorentzThresh(Double_t trs) {
        fLorentzThresh = trs;
    }

    void SetYstep(Double_t stp) {
        fYstep = stp;
    }

    void SetSigX(Double_t sig) {
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

//    void SetXRange(Double_t xMin, Double_t xMax) {
//        fXmax = xMax;
//        fXmin = xMin;
//    }
//
//    void SetYRange(Double_t yMin, Double_t yMax) {
//        fYmax = yMax;
//        fYmin = yMin;
//    }

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
    
    void SetUseRefit(Bool_t flag) {
        fUseRefit = flag;
    }

private:
    TVector2 CalcMeanSigma(vector <Double_t>); 
      
    BmnGemStripStationSet* fGemDetector;
    TString fGemHitsBranchName;
    TString fTracksBranchName;

    TClonesArray* fGemTracksArray;
    TClonesArray* fGemHitsArray;
    BmnKalmanFilter* fKalman;

    Int_t fPDG; // PDG hypothesis
    Double_t* fGemDistCut;

    Bool_t fIsField; // run with mag.field or not
    Bool_t fIsTarget; // run with target or not
    
    // fGoForward - direction of approximation:
    // fGoForward = 1 - forward (0 --> nStation)
    // fGoForward = 0 - backward (nStation --> 0)
    Bool_t fGoForward;

    Double_t fSigX;
    UInt_t fYstep;
    Double_t fLorentzThresh;
    UInt_t fNHitsCut;
    UInt_t fNHitsInGemCut;
    UInt_t fNSeedsCut;

    UInt_t fNBins; // number of bins in histogram  
    Double_t fMin;
    Double_t fMax;
    Double_t fWidth;

    UInt_t fEventNo; // event counter
    Short_t fPeriodId;

    TVector3 fRoughVertex; // for correct transformation

    //ranges for seed finder
//    Double_t fXmin;
//    Double_t fXmax;
//    Double_t fYmin;
//    Double_t fYmax;

    vector<Short_t> skipStations;

    FairField* fField;

    Int_t** fAddresses;
    Double_t fLineFitCut;
    
    vector<Int_t>* fHitsOnStation;
    
    Bool_t fUseRefit;
    
    TString fSteerFile; 
    BmnSteeringGemTracking* fSteering;
    
    ClassDef(BmnGemTracking, 1);
};
