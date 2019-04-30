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

    TString* fDetectors;
    TDirectory** fDirectories;

    FairRootManager* ioman;
    BmnEventHeader* fBmnHeader;

    // Common detectors (BM@N + SRC) - GEM, SILICON, TOF400, DCH, MWPC, TOF700, ECAL, ZDC, CSC 
    TString fGem;
    TString fCsc;
    TString fSi;
    TString fTOF400;
    TString fDch;
    TString fMwpc;
    TString fTOF700;
    TString fECAL;
    TString fZDC;

    TClonesArray* fGemDigits;
    TClonesArray* fCscDigits;
    TClonesArray* fSiDigits;
    TClonesArray* fTOF400Digits;
    TClonesArray* fDchDigits;
    TClonesArray* fMwpcDigits;
    TClonesArray* fTOF700Digits;
    TClonesArray* fECALDigits;
    TClonesArray* fZDCDigits;

    // Common triggers (BM@N + SRC) - BC1, BC2, VC
    TString fBC1;
    TString fBC2;
    TString fVeto;

    TClonesArray* fBC1Digits;
    TClonesArray* fBC2Digits;
    TClonesArray* fVetoDigits;

    // SRC triggers - BC3, BC4, X1L, X2L, Y1L, Y2L, X1R, X2R, Y1R, Y2R, TQDC_BC1, TQDC_BC2, TQDC_BC3, TQDC_BC4, TQDC_VETO
    TString fBC3;
    TString fBC4;
    TString fX1L;
    TString fX2L;
    TString fY1L;
    TString fY2L;
    TString fX1R;
    TString fX2R;
    TString fY1R;
    TString fY2R;
    TString fTQDC_BC1;
    TString fTQDC_BC2;
    TString fTQDC_BC3;
    TString fTQDC_BC4;
    TString fTQDC_Veto;

    TClonesArray* fBC3Digits;
    TClonesArray* fBC4Digits;
    TClonesArray* fX1LDigits;
    TClonesArray* fX2LDigits;
    TClonesArray* fY1LDigits;
    TClonesArray* fY2LDigits;
    TClonesArray* fX1RDigits;
    TClonesArray* fX2RDigits;
    TClonesArray* fY1RDigits;
    TClonesArray* fY2RDigits;
    TClonesArray* fTQDC_BC1Digits;
    TClonesArray* fTQDC_BC2Digits;
    TClonesArray* fTQDC_BC3Digits;
    TClonesArray* fTQDC_BC4Digits;
    TClonesArray* fTQDC_VetoDigits;

    // BM@N triggers
    TString fSiTrig;
    TString fBD;

    TClonesArray* fSiTrigDigits;
    TClonesArray* fBDDigits;

    BmnCoordinateDetQa* gem;
    BmnCoordinateDetQa* silicon;
    BmnCoordinateDetQa* csc;

    BmnTimeDetQa* tof400;
    BmnTimeDetQa* tof700;
    BmnTimeDetQa* dch;
    BmnTimeDetQa* mwpc;

    BmnCalorimeterDetQa* ecal;
    BmnCalorimeterDetQa* zdc;

    BmnTrigDetQa* triggers;

    TClonesArray** fTriggers;
    map <TClonesArray*, TString> fTrigCorr;
    
    Bool_t isDstRead;
    TChain* fChainDst;
    TClonesArray* fSiliconHits;
    TClonesArray* fSiliconTracks;
    TClonesArray* fGemHits;
    TClonesArray* fGemTracks;
    TClonesArray* fVertex;
    TClonesArray* fGlobalTracks;
    
    BmnDstQa* dst;

private:
    Bool_t ReadDstTree(TString);

    // Coordinate detectors
    template <class T> void GetDistributionOfFiredStrips(TClonesArray*, BmnCoordinateDetQa*, TString);

    // Time detectors
    template <class T> void GetCommonInfo(TClonesArray*, BmnTimeDetQa*, TString);
    template <class T> void GetMwpcDchInfo(TClonesArray*, BmnTimeDetQa*, TString);
    template <class T> void GetTofInfo(TClonesArray*, BmnTimeDetQa*, TString);

    // Calorim. detectors
    template <class T> void GetCommonInfo(TClonesArray*, BmnCalorimeterDetQa*, TString);

    // Trigger detectors
    template <class T> void GetCommonInfo(TClonesArray*, BmnTrigDetQa*, TString);
    
    // Dst
    void GetGlobalTracksDistributions(TClonesArray*, BmnDstQa*);
    template <class T> void GetInnerTracksDistributions(TClonesArray*, BmnDstQa*, TString);

    ClassDef(BmnQaOffline, 1);
};

#endif