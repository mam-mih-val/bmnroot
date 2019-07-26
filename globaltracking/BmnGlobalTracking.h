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
#include "BmnDetectorSetup.h"
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
#include "CbmVertex.h"
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

class TClonesArray;

using namespace std;

class BmnGlobalTracking : public FairTask {
   public:
    /**
     * \brief Constructor.
     */
    BmnGlobalTracking();
    BmnGlobalTracking(Bool_t);
    BmnGlobalTracking(Bool_t isField, Bool_t doAlign);

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

    TClonesArray *fBC1Digits;
    TClonesArray *fBC2Digits;
    TClonesArray *fBC3Digits;
    TClonesArray *fBC4Digits;

    TClonesArray *fBC1Digits_out;
    TClonesArray *fBC2Digits_out;
    TClonesArray *fBC3Digits_out;
    TClonesArray *fBC4Digits_out;

    TClonesArray *fEvHead;

    TClonesArray *fMCTracks;
    TH1F *fhXSiGemResid;
    TH1F *fhYSiGemResid;
    TH1F *fhTxSiGemResid;
    TH1F *fhTySiGemResid;
    TH2F *fhXdXSiGemResid;
    TH2F *fhYdYSiGemResid;
    TH2F *fhTxdXSiGemResid;
    TH2F *fhTydYSiGemResid;

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

    Short_t fPeriod;
    Bool_t fIsField;  // run with mag.field or not
    Bool_t fIsSRC;    // flag to turn on specific parts for SRC
    Bool_t fDoAlign;

    BmnDetectorSetup fDet;  // Detector presence information

    Int_t fEventNo;  // event counter

    Int_t fPDG;         // PDG hypothesis
    Float_t fChiSqCut;  // Chi square cut for hit to be attached to track.

    CbmVertex *fVertex;  // vertex information

    BmnKalmanFilter *fKalman;

    BmnStatus MatchingTOF(BmnGlobalTrack *tr, Int_t num);
    BmnStatus MatchingDCH(BmnGlobalTrack *tr, Int_t num);
    BmnStatus MatchingMWPC(BmnGlobalTrack *tr);
    BmnStatus MatchingSil(BmnGlobalTrack *tr);
    BmnStatus MatchingCSC(BmnGlobalTrack *tr);

    Int_t FindNearestHit(BmnGlobalTrack *tr, TClonesArray *hits, Float_t distCut);

    BmnStatus Refit(BmnGlobalTrack *tr);

    BmnGlobalTracking(const BmnGlobalTracking &);
    BmnStatus RefitToDetector(BmnGlobalTrack *tr, Int_t hitId, TClonesArray *hitArr, FairTrackParam *par, Int_t *nodeIdx, vector<BmnFitNode> *nodes);
    void CalcSiliconDist(Int_t, BmnGlobalTrack *, map<Double_t, pair<Int_t, Int_t>> &);

    void CalcdQdn(BmnGlobalTrack *tr);

    ClassDef(BmnGlobalTracking, 1);
};

#endif /* BmnGlobalTracking_H_ */
