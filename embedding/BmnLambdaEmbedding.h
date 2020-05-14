// @(#)bmnroot/embedding:$Id$
// Author: Pavel Batyuk <pavel.batyuk@jinr.ru> 2019-11-26

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// BmnLambdaEmbedding                                                         //
//                                                                            //
// Instruments to be used for embedding                                       //
//                                                                            //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
#ifndef BMNLAMBDAEMBEDDING_H
#define BMNLAMBDAEMBEDDING_H 1

#include <iostream>
#include <vector>
#include <fstream>
#include <map>
#include <unordered_map>
#include <utility>

#include <TNamed.h>
#include <TSystem.h>
#include <TFile.h>
#include <TClonesArray.h>
#include <TChain.h>

#include <CbmMCTrack.h>
#include <CbmVertex.h>
#include <FairMCPoint.h>

#include "BmnLambdaStore.h"
#include <DstEventHeader.h>
#include <BmnADCDigit.h>
#include <BmnGemStripDigit.h>
#include <BmnSiliconDigit.h>
#include <BmnRawDataDecoder.h>
#include <BmnMatch.h>
#include <BmnCSCPoint.h>
#include <BmnParticlePair.h>

#include "BmnLambdaMisc.h"
#include "BmnLambdaEmbeddingMonitor.h"
#include "BmnInnerTrackerGeometryDraw.h"

#if defined(_OPENMP)
#include "omp.h"
#endif       

using namespace std;

class BmnLambdaEmbedding : public TNamed {
public:
    BmnLambdaEmbedding();
    BmnLambdaEmbedding(TString, TString, TString, TString, Int_t nEvs = 250);
    ~BmnLambdaEmbedding();

public:
    void Embedding();
    
    TClonesArray* CreateLambdaStore();

    void SetStorePath(TString path) {
        fStorePath = path;
    }
   
    void SetNLambdaStore(Int_t nStores) {
        fNstores = nStores;
    }

    void SetLambdaEtaRange(Double_t min, Double_t max) {
        fEtaMin = min;
        fEtaMax = max;
    }

    void SetLambdaMinMomentum(Double_t min) {
        fMomMin = min;
    }

    void SetLambdaPhiRange(Double_t min, Double_t max) {
        fPhiMin = min;
        fPhiMax = max;
    }

    void SetDetsToBeEmbedded(Bool_t gem, Bool_t silicon, Bool_t csc) {
        isGemEmbedded = gem;
        isSilEmbedded = silicon;
        isCscEmbedded = csc;
    }

    void DoLambdaStore(Bool_t flag) {
        doLambdaStore = flag;
    }

    void DoListOfEventsWithReconstructedVertex(Bool_t flag) {
        doListOfEventsWithReconstructedVertex = flag;
    }

    void DoSimulateLambdaThroughSetup(Bool_t flag) {
        doSimulateLambdaThroughSetup = flag;
    }

    void DoRawRootConvertion(Bool_t flag) {
        doRawRootConvertion = flag;
    }

    void DoEmbedding(Bool_t flag) {
        doEmbedding = flag;
    }

    void DoDecode(Bool_t flag) {
        doDecode = flag;
    }

    void DoPrintStoreInfo(Bool_t flag) {
        doPrintStoreInfo = flag;
    }

    void DoEmbeddingMonitor(Bool_t flag) {
        doEmbeddingMonitor = flag;
    }

    // Set cut values ...

    void SetNHitsProton(Int_t nhits) {
        fNHitsProton = nhits;
    }

    void SetNHitsPion(Int_t nhits) {
        fNHitsPion = nhits;
    }

    void SetUseRealSignals(Bool_t flag) {
        isUseRealSignal = flag;
    }

    void SetSignal(Short_t sigGem, Short_t sigSilicon, Short_t sigCsc) {
        fSignal.push_back(sigGem);
        fSignal.push_back(sigSilicon);
        fSignal.push_back(sigCsc);
    }

private:
    Int_t fEvents;
    BmnLambdaMisc* fInfo;

    TChain* fSim;
    TChain* fReco;
    TChain* fLambdaSim;

    TClonesArray* fMCTracks;
    TClonesArray* fVertices;

    TClonesArray* fGemPoints;
    TClonesArray* fGemDigits;
    TClonesArray* fGemMatch;

    TClonesArray* fCscPoints;
    TClonesArray* fCscDigits;
    TClonesArray* fCscMatch;

    TClonesArray* fSiliconPoints;
    TClonesArray* fSiliconDigits;
    TClonesArray* fSiliconMatch;

    TClonesArray* fADC32; // GEM
    TClonesArray* fADC128; // SILICON
    TClonesArray* fSync; // SYNC

    TClonesArray* fLambdaStore;

    DstEventHeader* fHeader;

    UInt_t fRunId;
    Double_t fFieldScale;

    TString fRawRootFileName;
    TString fDataFileName;
    TString fDigiFileName;
    TString fStorePath;

    // Some bool flags to be used for steering of the whole stages of the code ...
    Bool_t doLambdaStore;
    Bool_t doListOfEventsWithReconstructedVertex;
    Bool_t doSimulateLambdaThroughSetup;
    Bool_t doRawRootConvertion;
    Bool_t doEmbedding;
    Bool_t doDecode;
    Bool_t doPrintStoreInfo;
    Bool_t doEmbeddingMonitor;

    // Cuts to be used if necessary ...
    Double_t fZmin;
    Double_t fZmax;
    Int_t fNstores;
    Int_t fNHitsProton;
    Int_t fNHitsPion;
    Bool_t isUseRealSignal;
    vector <Short_t> fSignal;

    // Dets. to be embedded ...
    Bool_t isGemEmbedded;
    Bool_t isSilEmbedded;
    Bool_t isCscEmbedded;

    // Embedding monitor ...
    BmnLambdaEmbeddingMonitor* fMon;

    // Cuts to be used when doing stores with lambda
    Double_t fEtaMin;
    Double_t fEtaMax;
    Double_t fMomMin;
    Double_t fPhiMin;
    Double_t fPhiMax;

private:
    vector <BmnStripDigit> GetDigitsFromLambda(TString, Int_t, TString);
    void PrintStoreInfo();
    TString AddInfoToRawFile(map <UInt_t, vector < BmnStripDigit>>, map <UInt_t, map <pair <Int_t, Int_t>, Long_t>>,
            map <UInt_t, vector < BmnStripDigit>>, map <UInt_t, map < vector <Int_t>, Long_t>>,
            map <UInt_t, vector < BmnStripDigit>>, map <UInt_t, map <pair <Int_t, Int_t>, Long_t>>);

    void DoRawRootFromBinaryData(TString);
    void StartDecodingWithEmbeddedLambdas(TString);

    void SimulateLambdaPassing(Double_t, TVector2, TVector3, Int_t, Int_t);
    Int_t FindReconstructableLambdaFromStore(Int_t, Int_t, BmnParticlePair&);

    map <UInt_t, TVector3> ListOfEventsWithReconstructedVp();    
    map <pair <Int_t, Int_t>, Long_t> GetGemChannelSerialFromDigi(vector <BmnStripDigit>);
    map <pair <Int_t, Int_t>, Long_t> GetCscChannelSerialFromDigi(vector <BmnStripDigit>);
    map <vector <Int_t>, Long_t> GetSiliconChannelSerialFromDigi(vector <BmnStripDigit>);

    Int_t DefineSiliconStatByZpoint(Double_t);

    ClassDef(BmnLambdaEmbedding, 1)
};

#endif

