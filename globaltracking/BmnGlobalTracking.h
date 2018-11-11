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

#include "FairTask.h"
#include "TClonesArray.h"
#include "BmnGlobalTrack.h"
#include "CbmTofPoint.h"
#include "BmnHit.h"
#include "BmnGemTrack.h"
#include "BmnSiliconHit.h"
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
#include "BmnKalmanFilter.h"
#include "CbmVertex.h"
#include "BmnSiliconStationSet.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TGraph.h"
#include "TFitResult.h"
#include "TFitResultPtr.h"


class TClonesArray;

using namespace std;

class BmnGlobalTracking : public FairTask {
public:
    /**
     * \brief Constructor.
     */
    BmnGlobalTracking();
    BmnGlobalTracking(Bool_t);

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

    void SetField(Bool_t f) {
        fIsField = f;
    }

    BmnStatus CreateDchHitsFromTracks();

private:
    BmnSiliconStationSet* fDetectorSI; // SI-geometry

    //AM 7.08
    Bool_t expData;
    TClonesArray* fGemHitArray;
    BmnGemStripHit * GetGemHit(Int_t i);
    BmnStatus MatchGemDCH(BmnGlobalTrack* tr);
    BmnStatus MatchDCHTOF(BmnGlobalTrack* tr, Int_t num);
    BmnStatus MatchDCHMPWC(BmnGlobalTrack* tr);
    void FitDCHTracks();
    void FitGemTracks();
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
    TClonesArray* fInnerTracks;
    TClonesArray* fGemVertex;
    TClonesArray* fGemHits;
    TClonesArray* fSilHits;
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

    TClonesArray* fMCTracks;
    TClonesArray* fBmnMPWCPointsArray;
    TClonesArray* fBmnTOFPointsArray;
    TClonesArray* fBmnTOF1PointsArray;
    TClonesArray* fBmnDchPointsArray;
    TClonesArray* fBmnGemPointsArray;

    // OUTPUT ARRAYS

    TClonesArray* fGlobalTracks; //output BmnGlobalTrack array

    Bool_t fIsField; // run with mag.field or not

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

    CbmVertex *fVertex; // vertex information

    BmnStatus MatchingTOF(BmnGlobalTrack* tr, Int_t num, Int_t trIndex);
    BmnStatus MatchingDCH(BmnGlobalTrack* tr);
    BmnStatus MatchingMWPC(BmnGlobalTrack* tr);
    BmnStatus MatchingSil(BmnGlobalTrack*, map <Double_t, pair<Int_t, Int_t>>&);

    BmnStatus Refit(BmnGlobalTrack* tr);
    BmnStatus EfficiencyCalculation();

    BmnStatus Run1GlobalTrackFinder();
    BmnStatus FillHoughHistogram(TH1F* h, TGraph* orig, TH2F* cm, TGraph* seeds, TClonesArray* arr);

    void FillIndexMap(map<Int_t, Int_t> &indexes, Int_t id);

    BmnGlobalTracking(const BmnGlobalTracking&);
    //    BmnGlobalTracking& operator=(const BmnGlobalTracking&);
    BmnStatus RefitToDetector(BmnGlobalTrack* tr, Int_t hitId, TClonesArray* hitArr, FairTrackParam* par, Int_t* nodeIdx, vector<BmnFitNode>* nodes);
    void CalcSiliconDist(Int_t, BmnGlobalTrack*, map <Double_t, pair<Int_t, Int_t>>&);

    ClassDef(BmnGlobalTracking, 1);
};

#endif /* BmnGlobalTracking_H_ */
