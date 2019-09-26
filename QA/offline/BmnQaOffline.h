#include "FairTask.h"
#include "FairRootManager.h"

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
#include <BmnGlobalTrack.h>
#include <BmnSiliconTrack.h>
#include <BmnGemTrack.h>

#include <BmnOfflineQaSteering.h>

#include <BmnCoordinateDetQa.h>
#include <BmnTimeDetQa.h>
#include <BmnCalorimeterDetQa.h>
#include <BmnTrigDetQa.h>
#include <BmnDstQa.h>

#include <BmnEventHeader.h>
#include <TString.h>
#include <TClonesArray.h>

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
    static Int_t fCurrentEvent;
    Int_t fNEvents;

    FairRootManager* ioman;
    BmnEventHeader* fBmnHeader;
    
    TClonesArray** DETECTORS;
    TClonesArray** TRIGGERS;
 
    map <TClonesArray*, TString> fTrigCorr;
    
    Bool_t isDstRead;
    
    Int_t nDets;   
    Int_t nCoordinate;
    Int_t nTime;
    Int_t nCalorimeter;
        
    TChain* fChainDst;
    TClonesArray* fSiliconHits;
    TClonesArray* fSiliconTracks;
    TClonesArray* fGemHits;
    TClonesArray* fGemTracks;
    TClonesArray* fVertex;
    TClonesArray* fGlobalTracks;
   
    // Qa-classes
    BmnCoordinateDetQa** coordinate;
    BmnTimeDetQa** time;
    BmnCalorimeterDetQa** calorimeter;
    BmnTrigDetQa* triggers;
    BmnDstQa* dst;
    
    Int_t period;
    TString setup;
    
    // Steering for the QA-system
    BmnOfflineQaSteering* fSteering;
    
private:
    Bool_t ReadDstTree(TString);

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
    void GetGlobalTracksDistributions(TClonesArray*, TClonesArray*, BmnDstQa*);
    template <class T> void GetInnerTracksDistributions(TClonesArray*, BmnDstQa*, TString);

    ClassDef(BmnQaOffline, 1);
};

#endif