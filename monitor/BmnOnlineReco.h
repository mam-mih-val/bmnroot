#ifndef BMNONLINERECO_H
#define BMNONLINERECO_H 1
// STL
#include <iostream>
#include <iterator>
#include <regex>
#include <exception>
// Auxillary
#include <zmq.h>
// FairSoft
#include <TNamed.h>
#include "TFile.h"
#include "TChain.h"
#include "TClonesArray.h"
#include "TColor.h"
#include "TFolder.h"
#include "THttpServer.h"
#include "TBufferFile.h"
#include "TString.h"
#include "TSystemDirectory.h"
#include "TList.h"
#include "TSystemFile.h"
#include "TServerSocket.h"
#include "TKey.h"
#include "FairRunAna.h"
#include "FairRootFileSink.h"
#include "FairParRootFileIo.h"
#include "FairParAsciiFileIo.h"
// BmnRoot
#include <BmnTrigDigit.h>
#include <BmnTof1Digit.h>
#include <BmnGemStripDigit.h>
#include <BmnDchDigit.h>
#include "BmnDataReceiver.h"
#include "BmnRawDataDecoder.h"
#include "BmnEventHeader.h"
#include "BmnDecoSource.h"
#include "BmnFillDstTask.h"

#include "BmnMQSink.h"
#include <BmnRunInfo.h>
#include "BmnFieldMap.h"
#include "BmnNewFieldMap.h"
//#include "BmnSiliconHitMaker.h"
#include "BmnMwpcHitFinder.h"
#include "BmnSiliconConfiguration.h"
#include "BmnSiliconHitMaker.h"
#include "BmnGemStripConfiguration.h"
#include "BmnGemStripHitMaker.h"
#include "BmnCSCConfiguration.h"
#include "BmnCSCHitMaker.h"
#include "BmnTof1HitProducer.h"
#include "BmnTofHitProducer.h"
#include "BmnTriggersCheck.h"
#include "BmnGlobalTracking.h"
#include "BmnVertexFinder.h"
#include "BmnInnerTrackingRun7.h"


using namespace std;
using namespace TMath;

class BmnOnlineReco : public TNamed {
public:

    BmnOnlineReco(Int_t periodId = 7, Int_t runId = 4649, BmnSetup setup = kBMNSETUP);
    virtual ~BmnOnlineReco();
    void RecoStream(TString dir = "", TString refDir = "", TString decoAddr = "localhost", Int_t webPort = 9000);

private:
    void ProcessDigi(Int_t iEv);
//    BmnStatus OpenStream();
    void FinishRun();
    TString GetDstNameFromRunId(Int_t runId) {
        TString name(Form("bmn_run%d_dst.root", runId));
        return name;
    }
    
    
    void * _ctx;
    void * _decoSocket;
    TString _curDir;
    TString _refDir;
    TString fRawDecoAddr;
    TFile *fHistOut;
    TFile *fHistOutTemp;
    THttpServer * fServer;
    TSocket *fRawDecoSocket;
    DigiArrays *fDigiArrays;
    
    TList *runPub;
    BmnRunInfo *CurRun;
    FairRunAna* fRunAna;
    BmnDecoSource* fDecoSource;


    Bool_t keepWorking;
    Int_t _webPort;
    Int_t fTest;
    BmnSetup fSetup;
    Int_t fPeriodID;
    Int_t fRunID;
    Int_t fEvents;
    BmnWorkerState fState;
    Int_t itersToUpdate;
    Int_t decoTimeout;
    


    ClassDef(BmnOnlineReco, 1)
};

#endif
