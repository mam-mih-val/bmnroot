#ifndef BMNTRACKCONV_H
#define BMNTRACKCONV_H

#include <limits.h>

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
#include "BmnGemStripStationSet.h"
#include "BmnSiliconStationSet.h"
#include "BmnCSCStationSet.h"
#include "BmnSiliconPoint.h"
#include "BmnCSCPoint.h"
#include "BmnBdPoint.h"
#include "BmnTOF1Point.h"

//#include "BmnRecoTools.h"

/** 
 * BmnTrackConv
 * Task for track conversion between BM@N and CBM formats
 * \author Ilnur Gabdrakhmanov <ilnur@jinr.ru>
 */
class BmnTrackConv : public FairTask {
public:
    BmnTrackConv(Int_t run_period, Int_t run_number = 0, BmnSetup setup = kBMNSETUP);
    virtual ~BmnTrackConv();
    
    InitStatus Init();
    //InitStatus ReInit();
    void Exec(Option_t *option);
    void Finish();
    void FinishEvent();
    
    void ProcessDST();    
    void ProcessEVE();    
    
protected:
//    Int_t CalcClusterSize(CbmStsCluster * cluster, TClonesArray* digiAr);
//    template<class ClHit>
//    void FillHitDetails(ClHit *hit, CbmStsHit* cbm){
//                hit->SetDetId(kSILICON);
//                hit->SetCovXY(cbmHit->GetCovXY());
//                hit->SetTimeStamp(cbmHit->GetTimeStamp());
//                hit->SetTimeStampError(cbmHit->GetTimeStampError());
//                hit->SetStation(iSt);
//                hit->SetModule(iMod);
//                hit->SetStripTotalSignalInLowerLayer(clusterF->GetQtot());
//                hit->SetStripTotalSignalInUpperLayer(clusterB->GetQtot());
//                hit->SetStripPositionInLowerLayer(clusterF->GetMean());
//                hit->SetStripPositionInUpperLayer(clusterB->GetMean());
//                hit->SetClusterSizeInLowerLayer(clusterF->GetNDigis());
//                hit->SetClusterSizeInUpperLayer(clusterB->GetNDigis());
//        
//    }
    inline Int_t GemModCbm2Bmn(Int_t iSt, Int_t iModCbm);
    inline Int_t CscModCbm2Bmn(Int_t iSt, Int_t iModCbm);
    BmnSetup fSetup;
    
    TString fDstTreeName;
    TString fDstFileName;
    
    TString fCBMMCEvHeaderName;
    TString fCBMEvHeaderName;
    TString fCBMoldBMNEvHeaderName;
    TString fCBMGlobalTracksName;
    TString fCBMGlobalTracksCSCName;
    TString fCBMHitsName;
    TString fCBMClustersName;
//    TString fCBMDigisName;
    TString fCBMVertexName;
    
    TString fCBMPointsName;
    TString fCBMCSCPointsName;
    TString fCBMBDPointsName;
    TString fTof400PointsName;
    
    TString fBMNMCEvHeaderName;
    TString fBMNEvHeaderName;
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
    TString fBMNDchHitsName;
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
    TClonesArray* fCBMClusters = nullptr;
//    TClonesArray* fCBMDigis = nullptr;
    TClonesArray* fCBMPoints = nullptr;
    TClonesArray* fCBMCSCPoints = nullptr;
    TClonesArray* fCBMBDPoints = nullptr;
    TClonesArray* fCBMTof400Points = nullptr;
    
    FairMCEventHeader* fBMNMCEvHeader = nullptr;
    DstEventHeader* fBMNEvHeader = nullptr;
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
    TClonesArray* fBMNDchHits = nullptr;
    
    TClonesArray* fBMNMwpcSegment = nullptr;
    
    TClonesArray* fBMNGemPoints = nullptr;
    TClonesArray* fBMNSilPoints = nullptr;
    TClonesArray* fBMNCSCPoints = nullptr;
    TClonesArray* fBMNBDPoints = nullptr;
    TClonesArray* fBMNTof400Points = nullptr;
    
    BmnGemStripStationSet* fGemStationSet = nullptr;
    BmnSiliconStationSet* fSilStationSet = nullptr;
    BmnCSCStationSet* fCscStationSet = nullptr;
    // map subdetector hit index to the original CBM
    vector<Int_t> fMapHit;
    
    
    Long64_t iEv;
    
    Int_t fPeriodId;
    Int_t fRunId;
    
    Bool_t isMCDST = kFALSE; // reconstructed MC
    Bool_t isMCEVE = kFALSE; // generated MC
    
    ClassDef(BmnTrackConv, 1);
};

#endif /* BMNTRACKCONV_H */

