/* * \file BmnGlobalTracking.h
 * \author Sergey Merts <Sergey.Merts@gmail.com>
 * \date 2014
 * \brief BMN task for global track reconstruction.
 *
 * Output is reconstructed global tracks BmnGlobalTrack
 * Hit-to-track merger attaches the TOF hit finally the track is refitted.
 **/

#ifndef BmnGlobalTracking_H_
#define BmnGlobalTracking_H_

#include "BmnDchHit.h"
#include "BmnDchTrack.h"
#include "BmnGemTrack.h"
#include "BmnGlobalTrack.h"
#include "BmnHit.h"
#include "BmnKalmanFilter.h"
#include "BmnMwpcHit.h"
#include "BmnMwpcTrack.h"
#include "BmnSiliconHit.h"
#include "BmnSiliconStationSet.h"
#include "CbmMCTrack.h"
#include "CbmStsHit.h"
#include "CbmTofPoint.h"
#include "BmnVertex.h"
#include "CbmVertex.h"
#include "CbmKFTrack.h"
#include "CbmStsTrack.h"

#include "FairMCPoint.h"
#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "FairTask.h"

#include "TClonesArray.h"
#include "TFitResult.h"
#include "TFitResultPtr.h"
#include "TGraph.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TMath.h"
#include <TStopwatch.h>

#include <string>
#include <vector>
using namespace std;

class BmnGlobalTracking : public FairTask
{
  public:
    /**
     * \brief Constructor.
     */
    BmnGlobalTracking();
    BmnGlobalTracking(Bool_t);
    BmnGlobalTracking(Bool_t isExp, Bool_t doAlign);
    BmnGlobalTracking(Bool_t isField, Bool_t isExp, Bool_t doAlign);

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

    void SetDoAlign(Bool_t a) {
        fDoAlign = a;
    };

    const Short_t GetDoAlign() const {
        return fDoAlign;
    };

    void SetSrcSetup(Bool_t f) {
        fIsSRC = f;
    }

    void SetRunNumber(Int_t r) {
        fRunId = r;
    }

    void SetPeriodNumber(Short_t p) {
        fPeriod = p;
    }

    void SetInnerTracksBranchName(TString name) {
        fInnerTrackBranchName = name;
    }

  private:
    /*
     * \brief Calculate length of the global track
     */
    Double_t CalculateLength(CbmStsTrack* tr);

    // INPUT ARRAYS
    TClonesArray* fInnerTracks;     //! GEM+SIL for BM@N, GEM for SRC
    TClonesArray* fGemTracks;       //!
    TClonesArray* fSiliconTracks;   //! for SRC only
    TClonesArray* fGemVertex;       //!
    TClonesArray* fGemHits;         //!
    TClonesArray* fSilHits;         //!
    TClonesArray* fCscHits;         //!
    TClonesArray* fMwpcTracks;      //!
    TClonesArray* fMwpcHits;        //!
    TClonesArray* fDchTracks;       //!
    TClonesArray* fDchHits;         //!
    TClonesArray* fTof1Hits;        //!
    TClonesArray* fTof2Hits;        //!
    TClonesArray* fUpstreamTracks;  //!
    TClonesArray* fUpsHits;         //!

    TClonesArray* fEvHead;          //!
    TClonesArray* fMCTracks;        //!

    TClonesArray* fStsHits;         //!
    TClonesArray* fStsTracks;       //!
    TClonesArray* fGlobalTracks;    //!

    TString fInnerTrackBranchName;

    Short_t fPeriod;
    Bool_t fIsSRC;    // flag to turn on specific parts for SRC
    Bool_t fIsExp;
    Bool_t fDoAlign;

    Int_t fEventNo;  // event counter
    Int_t fRunId;

    Int_t fPDG;         // PDG hypothesis
    Float_t fChiSqCut;  // Chi square cut for hit to be attached to track.

    BmnVertex* fVertex;     //! vertex information
    CbmVertex* fVertexL1;   //! vertex information for L1 case

    BmnKalmanFilter* fKalman;   //!
    Double_t fTime;
    
    Long_t fNMatchedDch1;
    Long_t fNMatchedDch2;
    Long_t fNMatchedTof400;
    Long_t fNMatchedTof700;
    Long_t fNMatchedNearCsc;
    Long_t fNMatchedFarCsc;
    Long_t fNInnerTracks;
    Long_t fNGoodInnerTracks;
    Long_t fNGoodGlobalTracks;

    void PrintStatistics();

    BmnStatus MatchingTOF(BmnGlobalTrack* tr, Int_t num);
    BmnStatus MatchingDCH(BmnGlobalTrack* tr);
    BmnStatus MatchingDCH(BmnGlobalTrack* tr, Int_t num);
    BmnStatus MatchingUpstream(BmnGlobalTrack* tr);
    BmnStatus MatchingCSC(BmnGlobalTrack* glTr, vector<Int_t> stations);

    Int_t FindNearestHit(FairTrackParam* par, TClonesArray* hits, Float_t xCut, Float_t yCut);
    Int_t FindNearestHit(FairTrackParam* par, TClonesArray* hits, Float_t xCut, Float_t yCut, vector<Int_t> stations);

    BmnStatus Refit(BmnGlobalTrack* tr);
    Double_t MagFieldIntegral(FairTrackParam& par, Double_t zMin, Double_t zMax, Double_t step);
    BmnStatus UpdateMomentum(BmnGlobalTrack* tr);

    BmnGlobalTracking(const BmnGlobalTracking&);

    void CalcdQdn(BmnGlobalTrack* tr);

  ClassDef(BmnGlobalTracking, 1);
};

#endif /* BmnGlobalTracking_H_ */
