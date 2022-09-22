#include <iostream>
#include <vector>
#include <utility>

#include <TNamed.h>
#include <TSystem.h>
#include <TChain.h>
#include <TH1.h>
#include <TClonesArray.h>
#include <TGeoManager.h>

#include "BmnTrackingQaOfflineDraw.h"
#include <FairTrackParam.h>
#include <BmnGlobalTrack.h>
#include <BmnFieldMap.h>
#include <BmnNewFieldMap.h>
#include <UniRun.h>
#include <ElogRecord.h>
#include <ElogTrigger.h>
#include <DstEventHeader.h>
#include <BmnKalmanFilter.h>
#include <CbmVertex.h> 

#ifndef BMNTRACKINGQAOFFLINE_H
#define BMNTRACKINGQAOFFLINE_H 1
     
using namespace std;

class BmnTrackingQaOffline : public TNamed {
public:

    BmnTrackingQaOffline();
    BmnTrackingQaOffline(TString, Int_t period = 7, TString ext = "pdf");

    void SetDstPath(TString path) {
        fPath = path;
    }

    void SetBeams(vector <TString> beams) {
        fBeams = beams;
    }

    void SetTargets(vector <TString> targets) {
        fTargets = targets;
    }

    void SetTriggers(vector <TString> triggers) {
        fTriggers = triggers;
    }

    virtual ~BmnTrackingQaOffline() {
//        if (fKalman)
//            delete fKalman;
        
//        if (fMagField)
//            delete fMagField;
        
        if (histoMan)
            histoMan->ProcessHistos(isEventAnal, isTrackAnal, isMatchAnal);
    }
    
    void SetEosHost(TString host) {
        fEosHost = host;
    }

    void DoAnalisys(Bool_t evAnal = kTRUE, Bool_t trackAnal = kTRUE, Bool_t matchAnal = kFALSE);

private:
    Int_t fPeriod;
    TString fSetup;

public:
    Int_t start;
    Int_t finish;

private:
    Bool_t isEventAnal;
    Bool_t isTrackAnal;
    Bool_t isMatchAnal;
    Bool_t isEoS; // Support of EoS filesystem on ncx.jinr.ru
    
    TString fEosHost;

    map <TString, TString> fTriggerMap;

    std::vector <TString> fBeams;
    vector <TString> fTargets;
    vector <TString> fTriggers;

    BmnTrackingQaOfflineDraw* histoMan;
    TString fPath;

    DstEventHeader* hDst;
    TClonesArray* fGlobTracks;
    TClonesArray* fTof400Hits;
    TClonesArray* fTof700Hits;
    TClonesArray* fDchHits;
    TClonesArray* fCscHits;
    TClonesArray* fVertices;
    
    FairRunAna* fAna;
    BmnKalmanFilter* fKalman;
    BmnFieldMap* fMagField;
    Bool_t fIsField;

public:
    TString GetTrigger(Int_t);
    
private: 
    pair <Int_t, Int_t> FindBinXY(TString);
    
    map <Int_t, pair <Double_t, Double_t>> GetResiduals(BmnGlobalTrack*, vector <BmnHit*>);

    ClassDef(BmnTrackingQaOffline, 1)
};

#endif
