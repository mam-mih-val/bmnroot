#ifndef BMNTRACKCONV_H
#define BMNTRACKCONV_H

#include <limits.h>
#include <pthread.h>

#include <TClonesArray.h>
#include <TFile.h>

#include <FairTask.h>
#include <FairMCEventHeader.h>

#include <CbmStsTrack.h>
#include <CbmStsHit.h>
#include <CbmStsDigi.h>
#include <CbmStsCluster.h>
#include <CbmVertex.h>
#include <CbmStsPoint.h>
#include <CbmMCTrack.h>

#include <BmnEnums.h>
#include <BmnVertex.h>
#include <BmnEventHeader.h>
#include <DstEventHeader.h>
#include <BmnGlobalTrack.h>
#include <BmnSiliconHit.h>
#include <BmnGemStripHit.h>
#include <BmnCSCHit.h>
#include <BmnTofHit.h>
#include <BmnDchTrack.h>
#include <BmnZDCEventData.h>
#include <BmnFairRunSim.h>
#include "BmnGemStripStationSet.h"
#include "BmnSiliconStationSet.h"
#include "BmnCSCStationSet.h"
#include "BmnSiliconPoint.h"
#include "BmnCSCPoint.h"
#include "BmnBdPoint.h"
#include "BmnTOF1Point.h"

class BmnZDCEventDataExt : public BmnZDCEventData {
public:
    BmnZDCEventDataExt() : BmnZDCEventData() {
        
    }
    virtual ~BmnZDCEventDataExt(){
    ;}

    void SetEnergy(Int_t v, char op = '\0') {
        switch (op) {
            case 'c': case 'C':
                fEc = v;
                break;
            case 'n': case 'N':
                fEn = v;
                break;
            case 'p': case 'P':
                fEp = v;
                break;
            default: fE = v;
        }
    }
    
    void SetNHits(Int_t v, char op = '\0') {
        switch (op) {
            case 'c': case 'C':
                fHc = v;
                break;
            case 'n': case 'N':
                fHn = v;
                break;
            case 'p': case 'P':
                fHp = v;
                break;
            default: fH = v;
        }
    }

    void SetX(Float_t v) {
        fX = v;
    }

    void SetY(Float_t v) {
        fY = v;
    }

    void SetAsymmetry(Float_t v) {
        fAsymmetry = v;
    }

    void SetMoment(Float_t v) {
        fMoment = v;
    }
private:
    ClassDef(BmnZDCEventDataExt, 1);
};

/** 
 * BmnTrackConv
 * Task for track conversion between BM@N and CBM formats
 * \author Ilnur Gabdrakhmanov <ilnur@jinr.ru>
 */
class BmnTrackConv : public FairTask {
public:
    BmnTrackConv(Int_t run_period, Int_t run_number = 0, BmnSetup setup = kBMNSETUP,
            TString inFileAdd = "",
            vector<TString> branchesToClone = {"BmnTof400Hit", "BmnTof700Hit", "BmnDchTrack"});
    virtual ~BmnTrackConv();

    BmnFairRunSim* GetRunSimInst() {
        return fRunSimInst;
    }

    void SetRunSimInst(BmnFairRunSim* v) {
        fRunSimInst = v;
    }

    InitStatus Init();
    //InitStatus ReInit();
    void Exec(Option_t *option);
    void Finish();
    void FinishEvent();

    void ProcessDST();
    void ProcessEVE();

protected:

    inline Int_t GemModCbm2Bmn(Int_t iSt, Int_t iModCbm);
    inline Int_t CscModCbm2Bmn(Int_t iSt, Int_t iModCbm);
    BmnSetup fSetup;

    BmnFairRunSim * fRunSimInst; ///< instance of the task manager

    TString fDstTreeName;
    TString fDstTreeNameNew;
    TString fDstFileName;

    TString fCBMMCEvHeaderName;
    TString fCBMEvHeaderName;
    TString fCBMoldBMNEvHeaderName;
    TString fCBMGlobalTracksName;
    TString fCBMGlobalTracksCSCName;
    TString fCBMHitsName;
    TString fCBMToF400HitsName;
    TString fCBMClustersName;
    //    TString fCBMDigisName;
    TString fCBMVertexName;

    TString fCBMPointsName;
    TString fCBMCSCPointsName;
    TString fCBMBDPointsName;
    TString fTof400PointsName;

    TString fBMNMCEvHeaderName;
    TString fBMNEvHeaderName;
    TString fBMNZDCName;
    TString fBMNMCGlobalTracksName;
    TString fBMNGlobalTracksName;
    TString fBMNGemTracksName;
    TString fBMNSilTracksName;
    TString fBMNCscTracksName;
    TString fBMNMwpcTracksName;
    TString fBMNInnerHitsName;
    TString fBMNGemHitsName;
    TString fBMNSilHitsName;
    TString fBMNCscHitsName;
    TString fBMNTof400HitsName;
    TString fBMNTof700HitsName;
    TString fBMNDchTracksName;
    TString fBMNMwpcHitsName;
    TString fBMNVertexName;
    TString fBMNGemPointsName;
    TString fBMNSilPointsName;
    TString fBMNCSCPointsName;
    TString fBMNBDPointsName;
    TString fBMNTof400PointsName;

    FairMCEventHeader* fCBMMCEvHeader = nullptr;
    FairEventHeader* fCBMEvHeader = nullptr;
    CbmVertex* fCBMVertex = nullptr;
    TClonesArray* fCBMoldBMNEvHeader = nullptr;
    TClonesArray* fCBMGlobalTracks = nullptr;
    TClonesArray* fCBMHits = nullptr;
    TClonesArray* fCBMToF400Hits = nullptr;
    TClonesArray* fCBMClusters = nullptr;
    //    TClonesArray* fCBMDigis = nullptr;
    TClonesArray* fCBMPoints = nullptr;
    TClonesArray* fCBMCSCPoints = nullptr;
    TClonesArray* fCBMBDPoints = nullptr;
    TClonesArray* fCBMTof400Points = nullptr;

    FairMCEventHeader* fBMNMCEvHeader = nullptr;
    DstEventHeader* fBMNEvHeader = nullptr;
    BmnZDCEventDataExt* fBMNZDC = nullptr;
    TClonesArray* fBMNVertex = nullptr;
    TClonesArray* fBMNGlobalTracks = nullptr;
    TClonesArray* fBMNGemTracks = nullptr;
    TClonesArray* fBMNSilTracks = nullptr;
    TClonesArray* fBMNCscTracks = nullptr;
    TClonesArray* fBMNMwpcTracks = nullptr;

    TClonesArray* fBMNInnerHits = nullptr;
    TClonesArray* fBMNGemHits = nullptr;
    TClonesArray* fBMNSilHits = nullptr;
    TClonesArray* fBMNCscHits = nullptr;
    TClonesArray* fBMNTof400Hits = nullptr;
    TClonesArray* fBMNTof700Hits = nullptr;
    TClonesArray* fBMNDchTracks = nullptr;
    DstEventHeader* fBMNEvHeaderIn = nullptr;
    BmnZDCEventData* fBMNZDCIn = nullptr;
    TClonesArray* fBMNTof400HitsIn = nullptr;
    TClonesArray* fBMNTof700HitsIn = nullptr;
    TClonesArray* fBMNDchTracksIn = nullptr;

    TClonesArray* fBMNMwpcSegment = nullptr;

    TClonesArray* fBMNGemPoints = nullptr;
    TClonesArray* fBMNSilPoints = nullptr;
    TClonesArray* fBMNCSCPoints = nullptr;
    TClonesArray* fBMNBDPoints = nullptr;
    TClonesArray* fBMNTof400Points = nullptr;

    BmnGemStripStationSet* fGemStationSet = nullptr;
    BmnSiliconStationSet* fSilStationSet = nullptr;
    BmnCSCStationSet* fCscStationSet = nullptr;

    TFile *fDstFile = nullptr;
    TTree *fDstTree = nullptr;
    vector<TString> fBrNames;
    vector<TClonesArray*> fInArrays;
    vector<TClonesArray*> fOutArrays;

    // map subdetector hit index to the original CBM
    vector<Int_t> fMapHit;


    Long64_t iEv;
    Long64_t iEvFile;

    Int_t fPeriodId;
    Int_t fRunId;

    Bool_t isMCDST = kFALSE; // reconstructed MC
    Bool_t isMCEVE = kFALSE; // generated MC

    ClassDef(BmnTrackConv, 1);
};

#endif /* BMNTRACKCONV_H */

