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

#include <string>
#include <vector>
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
#include "CbmKFTrack.h"
#include "CbmStsTrack.h"


class TClonesArray;

using namespace std;

class BmnGlobalTracking : public FairTask {
   public:
    /**
     * \brief Constructor.
     */
    BmnGlobalTracking();
    BmnGlobalTracking(Bool_t);
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
    virtual void Exec(Option_t *opt);

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

    void SetField(Bool_t f) {
        fIsField = f;
    }

    void SetSrcSetup(Bool_t f) {
        fIsSRC = f;
    }

    void SetRunNumber(Int_t r) {
        fRunId = r;
    }

   private:

    /*
     * \brief Calculate length of the global track
     */
    void CalculateLength();

    // INPUT ARRAYS
    TClonesArray *fInnerTracks;  //GEM+SIL for BM@N, GEM for SRC
    TClonesArray *fGemTracks;
    TClonesArray *fSiliconTracks;  //for SRC only
    TClonesArray *fGemVertex;
    TClonesArray *fGemHits;
    TClonesArray *fSilHits;
    TClonesArray *fCscHits;
    TClonesArray *fMwpcTracks;
    TClonesArray *fMwpcHits;
    TClonesArray *fDchTracks;
    TClonesArray *fDchHits;
    TClonesArray *fTof1Hits;
    TClonesArray *fTof2Hits;
    TClonesArray *fUpstreamTracks;
    TClonesArray *fUpsHits;

    TClonesArray *fEvHead;

    TClonesArray *fMCTracks;
    
    TClonesArray* fCbmStsTracks;
    TClonesArray* fGlobalTracks;

    TH1F *fhXTof1GemResid;
    TH1F *fhYTof1GemResid;
    TH2F *fhXdXTof1GemResid;
    TH2F *fhYdYTof1GemResid;
    TH2F *fhTxdXTof1GemResid;
    TH2F *fhTydYTof1GemResid;

    TH1F *fhXTof2GemResid;
    TH1F *fhYTof2GemResid;
    TH2F *fhXdXTof2GemResid;
    TH2F *fhYdYTof2GemResid;
    TH2F *fhTxdXTof2GemResid;
    TH2F *fhTydYTof2GemResid;

    TH1F *fhXDch1GemResid;
    TH1F *fhYDch1GemResid;
    TH1F *fhTxDch1GemResid;
    TH1F *fhTyDch1GemResid;
    TH2F *fhXdXDch1GemResid;
    TH2F *fhYdYDch1GemResid;
    TH2F *fhTxdXDch1GemResid;
    TH2F *fhTydYDch1GemResid;

    TH1F *fhXDch2GemResid;
    TH1F *fhYDch2GemResid;
    TH1F *fhTxDch2GemResid;
    TH1F *fhTyDch2GemResid;
    TH2F *fhXdXDch2GemResid;
    TH2F *fhYdYDch2GemResid;
    TH2F *fhTxdXDch2GemResid;
    TH2F *fhTydYDch2GemResid;

    TH1F *fhXDchGGemResid;
    TH1F *fhYDchGGemResid;
    TH1F *fhTxDchGGemResid;
    TH1F *fhTyDchGGemResid;
    TH2F *fhXdXDchGGemResid;
    TH2F *fhYdYDchGGemResid;
    TH2F *fhTxdXDchGGemResid;
    TH2F *fhTydYDchGGemResid;

    TH1F *fhXCscGemResid;
    TH1F *fhYCscGemResid;
    TH2F *fhXdXCscGemResid;
    TH2F *fhYdYCscGemResid;
    TH2F *fhTxdXCscGemResid;
    TH2F *fhTydYCscGemResid;

    TH1F **fhdXGemSt;
    TH1F **fhdYGemSt;
    TH1F **fhdTxGemSt;
    TH1F **fhdTyGemSt;
    TH2F **fhXdXGemSt;
    TH2F **fhYdYGemSt;

    TH1F *fhXUResid;
    TH1F *fhYUResid;
    TH1F *fhTxUResid;
    TH1F *fhTyUResid;
    TH2F *fhXdXUResid;
    TH2F *fhYdYUResid;
    TH2F *fhTxdXUResid;
    TH2F *fhTydYUResid;
    Short_t fPeriod;
    Bool_t fIsField;  // run with mag.field or not
    Bool_t fIsSRC;    // flag to turn on specific parts for SRC
    Bool_t fIsExp;
    Bool_t fDoAlign;

    Int_t fEventNo;  // event counter
    Int_t fRunId;

    Int_t fPDG;         // PDG hypothesis
    Float_t fChiSqCut;  // Chi square cut for hit to be attached to track.

    BmnVertex *fVertex;  // vertex information

    BmnKalmanFilter *fKalman;
    Double_t fTime;

    BmnStatus MatchingTOF(BmnGlobalTrack *tr, Int_t num);
    BmnStatus MatchingTOF(CbmStsTrack *tr, BmnGlobalTrack *glTr, Int_t num);
    BmnStatus MatchingDCH(BmnGlobalTrack *tr, Int_t num);
    BmnStatus MatchingMWPC(BmnGlobalTrack *tr);
    BmnStatus MatchingUpstream(BmnGlobalTrack *tr);
    BmnStatus MatchingCSC(BmnGlobalTrack *tr);

    Int_t FindNearestHit(BmnGlobalTrack *tr, TClonesArray *hits, Float_t distCut);

    BmnStatus Refit(BmnGlobalTrack *tr);
    Double_t MagFieldIntegral(FairTrackParam& par, Double_t zMin, Double_t zMax, Double_t step);
    BmnStatus UpdateMomentum(BmnGlobalTrack *tr);

    BmnGlobalTracking(const BmnGlobalTracking &);
    BmnStatus RefitToDetector(BmnGlobalTrack *tr, Int_t hitId, TClonesArray *hitArr, FairTrackParam *par, Int_t *nodeIdx, vector<BmnFitNode> *nodes);
    void CalcSiliconDist(Int_t, BmnGlobalTrack *, map<Double_t, pair<Int_t, Int_t>> &);

    void CalcdQdn(BmnGlobalTrack *tr);

    ClassDef(BmnGlobalTracking, 1);
};

#endif /* BmnGlobalTracking_H_ */
