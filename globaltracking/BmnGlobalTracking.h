/**
 * \file BmnGlobalTracking.h
 * \author Sergey Merts <Sergey.Merts@gmail.com>
 * \date 2014
 * \brief BMN task for global track reconstruction.
 *
 * Output is reconstructed global tracks BmnGlobalTrack 
 * Hit-to-track merger attaches the TOF hit finally the track is refitted.
 **/

#ifndef BmnGlobalTracking_H_
#define BmnGlobalTracking_H_

#include "FairTask.h"
#include "BmnTrackFinder.h"
//#include "BmnTrackFitter.h"
#include "BmnTrackPropagator.h"
#include "BmnHitToTrackMerger.h"
#include "BmnGlobalTrackingQA.h"
#include "BmnHitMatchingQA.h"
#include "TClonesArray.h"
#include "BmnGlobalTrack.h"
#include "CbmTofPoint.h"
#include "BmnHit.h"
#include "BmnGemTrack.h"
#include "BmnDchTrack.h"
#include "BmnDchHit.h"
#include "BmnMwpcHit.h"
#include "BmnMwpcTrack.h"
#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "FairMCPoint.h"
#include "CbmStsHit.h"
#include "CbmMCTrack.h"
#include "TMath.h"
#include <vector>
#include <string>
#include "BmnDetectorSetup.h"
#include "TCanvas.h"
#include "TColor.h"
#include "TGraph.h"
#include "BmnKalmanFilter_tmp.h"

class TClonesArray;

using namespace std;

class BmnGlobalTracking : public FairTask {
public:
    /**
     * \brief Constructor.
     */
    BmnGlobalTracking();

    /**
     * \brief Destructor.
     */
    virtual ~BmnGlobalTracking();

    /**
     * \brief Inherited from FairTask.
     */
    virtual InitStatus Init();

    /**
     * \brief Inherited from FairTask.
     */
    virtual void Exec(Option_t* opt);

    /**
     * \brief Inherited from FairTask.
     */
    virtual void Finish();
    
    Float_t Sqr(Float_t x);

    void SetRun1(Bool_t run) {
        isRUN1 = run;
    }

    void SetDetConf(Bool_t gem, Bool_t tof1, Bool_t dch1, Bool_t dch2, Bool_t tof2) {
        fDetConf = Short_t(gem) * 16 + Short_t(tof1) * 8 + Short_t(dch1) * 4 + Short_t(dch2) * 2 + Short_t(tof2) * 1;
    };

    const Short_t GetDetConf() const {
        return fDetConf;
    };
    
    BmnStatus CreateDchHitsFromTracks();

private:

    /*
     * \brief Calculate length of the global track
     */
    void CalculateLength();

    /**
     * \brief Run the track reconstruction
     */
    void RunTrackReconstruction();

    /**
     * \brief Select tracks for further merging with TOF.
     */
    void SelectTracksForTofMerging();

    // INPUT ARRAYS
    TClonesArray* fGemTracks;
    TClonesArray* fGemVertex;
    TClonesArray* fGemHits;
    TClonesArray* fMwpcTracks;
    TClonesArray* fMwpcHits;
    TClonesArray* fDchTracks;
    TClonesArray* fDchHits;
    TClonesArray* fTof1Hits;
    TClonesArray* fTof2Hits;
    
    TClonesArray* fEvHead;

    // INPUT FOR CHECKING EFFICIENCY
    TClonesArray* fGemMcPoints;
    TClonesArray* fTof1McPoints;
    TClonesArray* fTof2McPoints;
    TClonesArray* fDchMcPoints;
    TClonesArray* fMcTracks;

    // OUTPUT ARRAYS

    TClonesArray* fGlobalTracks; //output BmnGlobalTrack array


    /*
     * Detector configuration, 5 bits:
     * 1 - GEM
     * 2 - TOF1
     * 3 - DCH1
     * 4 - DCH2
     * 5 - TOF2
     */
    Short_t fDetConf;
    BmnDetectorSetup fDet; // Detector presence information

    Int_t fEventNo; // event counter

    Bool_t isRUN1; // temporary flag for switching between full GEM-geometry and prototype

    Int_t fPDG; // PDG hypothesis
    Float_t fChiSqCut; // Chi square cut for hit to be attached to track.

    BmnStatus MatchingTOF(BmnGlobalTrack* tr, Int_t num);
    BmnStatus MatchingDCH(BmnGlobalTrack* tr);
    BmnStatus MatchingMWPC(BmnGlobalTrack* tr);
    
    BmnStatus Refit(BmnGlobalTrack* tr);
    BmnStatus EfficiencyCalculation();
    
    BmnStatus Run1GlobalTrackFinder();
    BmnStatus FillHoughHistogram(TH1F* h, TGraph* orig, TH2F* cm, TGraph* seeds, TClonesArray* arr);

    void IdChecker(Int_t refId, Int_t hitId, TVector3 pos, BmnHitMatchingQA* hist);
    void FillIndexMap(map<Int_t, Int_t> &indexes, Int_t id);

    BmnGlobalTracking(const BmnGlobalTracking&);
    //    BmnGlobalTracking& operator=(const BmnGlobalTracking&);
    BmnStatus RefitToDetector(BmnGlobalTrack* tr, Int_t hitId, TClonesArray* hitArr, FairTrackParam* par, Int_t* nodeIdx, vector<BmnFitNode>* nodes);

    ClassDef(BmnGlobalTracking, 1);
};

#endif /* BmnGlobalTracking_H_ */
