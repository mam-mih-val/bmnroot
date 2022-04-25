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
#include <stdlib.h>
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

#include "BmnParticleStore.h"
#include <DstEventHeader.h>
#include <BmnADCDigit.h>
#include <BmnGemStripDigit.h>
#include <BmnSiliconDigit.h>
#include <BmnRawDataDecoder.h>
#include <BmnMatch.h>
#include <BmnCSCPoint.h>
#include <BmnParticlePair.h>
#include <BmnRecoTools.h>

#include "BmnLambdaMisc.h"
#include "BmnLambdaEmbeddingMonitor.h"
#include "BmnInnerTrackerGeometryDraw.h"

#if defined(_OPENMP)
#include "omp.h"
#endif  

using namespace std;

class SignalNormalizationUtils : public TNamed {
public:

    SignalNormalizationUtils() {
        ;
    }

    virtual ~SignalNormalizationUtils() {
        ;
    }

    SignalNormalizationUtils(TString digiFile, TString recoFile, Int_t clustSize = 0, Int_t lowThresh = 10) :
    expGem(nullptr),
    mcGem(nullptr),
    expSil(nullptr),
    mcSil(nullptr),
    fClustSizeThresh(clustSize),
    fLowThresh(lowThresh) {

        TChain* chainEx = new TChain("bmndata");
        chainEx->Add(digiFile.Data());

        TClonesArray* gemEx = nullptr;
        chainEx->SetBranchAddress("GEM", &gemEx);

        TClonesArray* silEx = nullptr;
        chainEx->SetBranchAddress("SILICON", &silEx);

        TChain* chainDST = new TChain("bmndata");
        chainDST->Add(recoFile.Data());

        TClonesArray* gemHits = nullptr;
        chainDST->SetBranchAddress("BmnGemStripHit", &gemHits);

        TClonesArray* silHits = nullptr;
        chainDST->SetBranchAddress("BmnSiliconHit", &silHits);

        chainEx->GetEntry();
        chainDST->GetEntry();

        // Getting experimental TF1 from current dst to be used further for rescaling ...
        expGem = BmnRecoTools::GetSignalDistribution(chainEx, gemEx, chainDST, gemHits, nullptr, nullptr, 0, fClustSizeThresh);
        expSil = BmnRecoTools::GetSignalDistribution(chainEx, silEx, chainDST, silHits, nullptr, nullptr, 0, fClustSizeThresh);

        delete chainEx;
        delete chainDST;
    }

    void SetMcDataSet(TChain* chainMC) {
        TClonesArray* gemMC = nullptr;
        chainMC->SetBranchAddress("BmnGemStripDigit", &gemMC);

        TClonesArray* silMC = nullptr;
        chainMC->SetBranchAddress("BmnSiliconDigit", &silMC);

        chainMC->GetEntry();

        mcGem = BmnRecoTools::GetSignalDistribution(chainMC, gemMC,
                nullptr, nullptr, nullptr, nullptr, fLowThresh, 0, 1e6);

        mcSil = BmnRecoTools::GetSignalDistribution(chainMC, silMC,
                nullptr, nullptr, nullptr, nullptr, fLowThresh, 0, 1e6);
    }

    TF1* GetRescaleFunction(TString det) {
        if (det.Contains("GEM") && mcGem && expGem)
            return BmnRecoTools::GetRescaleFunc(TString("RescaleFuncGem"), mcGem, expGem);
        else if (det.Contains("SILICON") && mcSil && expSil)
            return BmnRecoTools::GetRescaleFunc(TString("RescaleFuncSil"), mcSil, expSil);
        else
            return nullptr;
    }

private:
    TF1* expGem;
    TF1* mcGem;

    TF1* expSil;
    TF1* mcSil;

    Int_t fClustSizeThresh;
    Int_t fLowThresh;
};

class BmnLambdaEmbedding : public TNamed {
public:
    BmnLambdaEmbedding();
    BmnLambdaEmbedding(TString, TString, TString, TString, Int_t nEvs = 250, TString lambdaStore = "");
    virtual ~BmnLambdaEmbedding();

public:
    void Embedding();

    TClonesArray* CreateLambdaStore();

    void SetStorePath(TString store_path) {
        fStorePath = store_path;
    }

    void SetNLambdaStore(Int_t nStores) {
        fNstores = nStores;
    }

    void SetNeventsToBeSimulated(Int_t nEvs) {
        fNeventsToBeSimulated = nEvs;
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

    // Set a store to be passed to simulations if necessary 
    void SetStoreToProcess(Int_t storeNumber) {
        fStoreToProcess = storeNumber;
    }

    // Setting a pdg hypo to be checked ...

    void SetDecayingParticle(Int_t pdg) {
        fPdgDecParticle = pdg;
    }

    void SetParticles(Int_t pPdg, Int_t nPdg) {
        fPdgPostive = pPdg;
        fPdgNegative = nPdg;
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
    TClonesArray** fWrittenStores;

    DstEventHeader* fHeader;

    Int_t fStoreToProcess;
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
    Int_t fNeventsToBeSimulated;
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

    // Normalization utils ...
    SignalNormalizationUtils* normUtils;

    // Cuts to be used when doing stores with lambda
    Double_t fEtaMin;
    Double_t fEtaMax;
    Double_t fMomMin;
    Double_t fPhiMin;
    Double_t fPhiMax;

    // Pdg codes to define a decay we are searching for ... 
    Int_t fPdgDecParticle;
    Int_t fPdgPostive;
    Int_t fPdgNegative;

    TString fFileNamePart;

private:
    vector <BmnStripDigit> GetDigitsFromLambda(TString, Int_t, TString);
    void PrintStoreInfo();
    TString AddInfoToRawFile(map <UInt_t, vector < BmnStripDigit>>, map <UInt_t, map <pair <Int_t, Int_t>, Long_t>>,
            map <UInt_t, vector < BmnStripDigit>>, map <UInt_t, map < vector <Int_t>, Long_t>>,
            map <UInt_t, vector < BmnStripDigit>>, map <UInt_t, map <pair <Int_t, Int_t>, Long_t>>);

    void DoRawRootFromBinaryData(TString);
    void StartDecodingWithEmbeddedLambdas(TString);

    void SimulateLambdaPassing(Int_t, Double_t, TVector2, TVector3, Int_t, Int_t);
    Int_t FindReconstructableLambdaFromStore(Int_t, Int_t, BmnParticlePair&);

    map <UInt_t, TVector3> ListOfEventsWithReconstructedVp();
    map <pair <Int_t, Int_t>, Long_t> GetGemChannelSerialFromDigi(vector <BmnStripDigit>);
    map <pair <Int_t, Int_t>, Long_t> GetCscChannelSerialFromDigi(vector <BmnStripDigit>);
    map <vector <Int_t>, Long_t> GetSiliconChannelSerialFromDigi(vector <BmnStripDigit>);

    Int_t DefineSiliconStatByZpoint(Double_t);

    ClassDef(BmnLambdaEmbedding, 1)
};

#endif

