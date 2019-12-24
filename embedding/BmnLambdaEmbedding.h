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
#include <BmnRawDataDecoder.h>
#include <BmnMatch.h>

#include "BmnLambdaMisc.h"

using namespace std;

class BmnLambdaEmbedding : public TNamed {
public:
    BmnLambdaEmbedding();
    BmnLambdaEmbedding(TString, TString, TString, TString, Int_t nEvs = 250);
    ~BmnLambdaEmbedding();

public:
    void Embedding();

    vector <BmnGemStripDigit> GetDigitsFromLambda(TString, Int_t);

    void DoLambdaStore(Bool_t flag) {
        doLambdaStore = flag;
    }

    void DoListOfEventsWithReconstructedVertex(Bool_t flag) {
        doListOfEventsWithReconstructedVertex = flag;
    }

    void DoSimulateLambdaThroughSetup(Bool_t flag) {
        doSimulateLambdaThroughSetup = flag;
    }

private:
    Int_t fEvents;
    Int_t fNeventsForStore;
    BmnLambdaMisc* fInfo;

    TChain* fSim;
    TChain* fReco;
    // TChain* fRaw;
    TChain* fLambdaSim;

    TClonesArray* fMCTracks;
    TClonesArray* fVertices;

    TClonesArray* fGemPoints;
    TClonesArray* fGemDigits;
    TClonesArray* fGemMatch;

    TClonesArray* fSiliconPoints;
    TClonesArray* fSiliconDigits;
    TClonesArray* fSiliconMatch;

    TClonesArray* fADC32; // GEM
    TClonesArray* fADC128; // SILICON

    TClonesArray* fLambdaStore;

    DstEventHeader* fHeader;

    Double_t fZmin;
    Double_t fZmax;
    UInt_t fRunId;
    Double_t fFieldScale;

    TString fRawRootFileName;
    TString fDataFileName;
    TString fDigiFileName;

    // Some bool flags to be used for steering of the whole stages of the code ...
    Bool_t doLambdaStore;
    Bool_t doListOfEventsWithReconstructedVertex;
    Bool_t doSimulateLambdaThroughSetup;

private:
    void CreateLambdaStore();
    void PrintStoreInfo();
    TString AddInfoToRawFile(map <UInt_t, vector < BmnGemStripDigit>>, map <UInt_t, map <pair <Int_t, Int_t>, Long_t>>);
        
    void DoRawRootFromBinaryData(TString);
    void StartDecodingWithEmbeddedLambdas(TString);

    void SimulateLambdaPassing(Double_t, TVector2, TVector3, Int_t, Int_t);
    Int_t FindReconstructableLambdaFromStore(Int_t, Int_t);

    map <UInt_t, TVector3> ListOfEventsWithReconstructedVp();
    map <pair <Int_t, Int_t>, Long_t> GetChannelSerialFromDigi(vector <BmnGemStripDigit>);
    
    Int_t DefineSiliconStatByZpoint(Double_t);

    ClassDef(BmnLambdaEmbedding, 1)
};


#endif


