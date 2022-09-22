#include <FairTask.h>
#include <FairRootManager.h>

#include <CbmVertex.h>
#include <BmnGemStripDigit.h>
#include <BmnCSCDigit.h>
#include <BmnSiliconDigit.h>
#include <BmnTof1Digit.h>
#include <BmnTof2Digit.h>
#include <BmnDchDigit.h>
#include <BmnMwpcDigit.h>
#include <BmnECALDigit.h>
#include <BmnZDCDigit.h>
#include <BmnTrigDigit.h>
#include <BmnTrigWaveDigit.h>
#include <BmnSiliconTrack.h>
#include <BmnGemTrack.h>
#include <BmnDchTrack.h>

#include <BmnGemStripHit.h>
#include <BmnSiliconHit.h>
#include <BmnCSCHit.h>
#include <BmnTofHit.h>
#include <BmnDchHit.h>

#include <BmnOfflineQaSteering.h>

#include <BmnCoordinateDetQa.h>
#include <BmnTimeDetQa.h>
#include <BmnCalorimeterDetQa.h>
#include <BmnTrigDetQa.h>
#include <BmnDstQa.h>

#include <BmnEventHeader.h>
#include <TString.h>
#include <TClonesArray.h>
#include <TGeoManager.h>
#include "TFile.h"

#include <DstEventHeader.h>
#include <BmnKalmanFilter.h>

#ifndef BMNQAOFFLINE_H
#define BMNQAOFFLINE_H 1

using namespace std;
using namespace TMath;

class BmnQaOffline : public FairTask {
public:

    BmnQaOffline() {};
    BmnQaOffline(TString);

    virtual ~BmnQaOffline() {
    };

    virtual InitStatus Init();

    virtual void Exec(Option_t* opt);

    virtual void Finish();

private:
    void doOccupancyAnal();
    void doOccupancy(TClonesArray*);
    void doMatchingAnal();
    void doEfficiencyAnal();
    void doResidualsPullsAnal();
    void doAverageStripValuePerHit(TClonesArray*);
    void doAverageStripValuePerTrack();
    
    void changeHistoContent(TH2F*);
    
    static Int_t fCurrentEvent;
    Int_t fNEvents;

    FairRootManager* ioman;
    BmnEventHeader* fBmnHeader;
    DstEventHeader* fDstHeader;
    
    TClonesArray** DETECTORS;
    TClonesArray** TRIGGERS;
 
    map <TClonesArray*, TString> fTrigCorr;
    
    Bool_t isDstRead;
    Bool_t isField;
    
    Int_t nDets;   
    Int_t nCoordinate;
    Int_t nTime;
    Int_t nCalorimeter;
        
    TChain* fChainDst;
    TClonesArray* fSiliconHits;
    TClonesArray* fSiliconTracks;
    TClonesArray* fGemHits;
    TClonesArray* fGemTracks;
    TClonesArray* fCscHits;
    TClonesArray* fTof400Hits;
    TClonesArray* fTof700Hits;
    TClonesArray* fDchTracks;
    TClonesArray* fMwpcTracks;
    TClonesArray* fDchHits;
    TClonesArray* fMwpcHits;
    TClonesArray* fVertex;
    TClonesArray* fGlobalTracks;
   
    // Qa-classes
    BmnCoordinateDetQa** coordinate;
    BmnTimeDetQa** time;
    BmnCalorimeterDetQa** calorimeter;
    BmnTrigDetQa* triggers;
    BmnDstQa* dst;
    
    Int_t period;
    Int_t fRunId;
    TString setup;
    TString prefix;
    
    // Steering for the QA-system
    BmnOfflineQaSteering* fSteering;
    
    // Detector geometries 
    BmnGemStripStationSet* fDetGem;
    BmnCSCStationSet* fDetCsc;
    BmnSiliconStationSet* fDetSilicon;
    
private:
    Bool_t ReadDstTree(TString);
    
    BmnHit* MatchDetector(FairTrackParam*, TClonesArray*, Bool_t, Int_t);

    // Coordinate detectors
    template <class T> void GetDistributionOfFiredStrips(TClonesArray*, BmnCoordinateDetQa*, TString);
    template <class T> void GetDistributionOfFiredStripsVsSignal(TClonesArray*, BmnCoordinateDetQa*, TString);

    // Time detectors
    template <class T> void GetCommonInfo(TClonesArray*, BmnTimeDetQa*, TString);
    template <class T> void GetMwpcDchInfo(TClonesArray*, BmnTimeDetQa*, TString);
    template <class T> void GetTofInfo(TClonesArray*, BmnTimeDetQa*, TString);

    // Calorim. detectors
    template <class T> void GetCommonInfo(TClonesArray*, BmnCalorimeterDetQa*, TString);

    // Trigger detectors
    template <class T> void GetCommonInfo(TClonesArray*, BmnTrigDetQa*, TString);
    
    // Dst
    void GetBasicTrackDistributions();
    // template <class T> void GetInnerTracksDistributions(TClonesArray*, BmnDstQa*, TString, TString);
    void doHitsDistributions(TClonesArray*, BmnDstQa*, TString);
    //template <class T> void GetSpecificHitsDistributions(TClonesArray*, BmnDstQa*, TString, TString);

    ClassDef(BmnQaOffline, 1);
};

#endif
