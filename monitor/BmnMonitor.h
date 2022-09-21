#ifndef BMNMONITOR_H
#define BMNMONITOR_H 1
// STL
#include <iostream>
#include <iterator>
#include <regex>
#include <exception>
// Auxillary
#include <zmq.h>
// FairSoft
#include <TNamed.h>
#include <TFile.h>
#include <TChain.h>
#include <TClonesArray.h>
#include <TColor.h>
#include <TFolder.h>
#include <THttpServer.h>
#include <TBufferFile.h>
#include <TMonitor.h>
#include <TString.h>
#include <TSystemDirectory.h>
#include <TList.h>
#include <TSystemFile.h>
#include <TServerSocket.h>
// BmnRoot
#include <BmnTrigDigit.h>
#include <BmnTof1Digit.h>
#include <BmnGemStripDigit.h>
#include <BmnDchDigit.h>
#include <BmnRawDataDecoder.h>
#include <BmnEventHeader.h>
#include <ElogRecord.h>
// BmnRoot Monitor
#include <BmnRunInfo.h>
#include "BmnDataReceiver.h"
#include "BmnHistToF.h"
#include "BmnHistToF700.h"
#include "BmnHistTrigger.h"
#include "BmnHistDch.h"
#include "BmnHistMwpc.h"
#include "BmnHistGem.h"
#include "BmnOnlineDecoder.h"
#include "BmnHistZDC.h"
#include "BmnHistScWall.h"
#include "BmnHistECAL.h"
#include "BmnHistLAND.h"
#include "BmnHistTofCal.h"
#include "BmnOnlineDecoder.h"
#include "BmnHistSilicon.h"
#include "BmnHistCsc.h"
#include "BmnHistSrc.h"

#define TTREE_MAX_SIZE          3e11

using namespace std;
using namespace TMath;

class BmnMonitor : public TNamed {
public:

    BmnMonitor();
    virtual ~BmnMonitor();
    void MonitorStreamZ(TString dir, TString refDir = "", TString decoAddr = "localhost", Int_t webPort = 9000);
    static void threadDecodeWrapper(TString dirname, TString startFile, Bool_t runCurrent);
    static void threadCmdWrapper(string cmd);
    TObjArray* GetAlikeRunsByElog(Int_t periodID, Int_t rinID);
    TObjArray* GetAlikeRunsByUniDB(Int_t periodID, Int_t rinID);
    
    // Getters
    deque<UInt_t> * GetDataQue() { return fDataQue;}

    // Setters
    void SetDataQue(deque<UInt_t> * v) { fDataQue = v;}

    void SetPeriodID(Int_t v) {
        this->fPeriodID = v;
    }

    Int_t GetPeriodID() const {
        return fPeriodID;
    }
    
    void SetBmnSetup(BmnSetup v) {
        fSetup = v;
    }

    BmnSetup GetBmnSetup() const {
        return fSetup;
    }

private:
    void InitServer();
    void ProcessDigi(Int_t iEv);
    void RegisterAll();
    void UpdateRuns();
    BmnStatus CreateFile(Int_t runID = 0);
    BmnStatus OpenStream();
    void FinishRun();
    
    
    void * _ctx;
    void * _decoSocket;
    deque<UInt_t> * fDataQue;
    TString _curFile;
    TString _curDir;
    TString _refDir;
    TString fRawDecoAddr;
    TTree *fRecoTree;
    TTree *fRecoTree4Show;
    TFile *fHistOut;
    TFile *fHistOutTemp;
    THttpServer * fServer;
    TSocket *fRawDecoSocket;
    DigiArrays *fDigiArrays;

    vector < BmnHist* > bhVec;
    vector < BmnHist* > bhVec4show;
    
    TCanvas *infoCanvas;
//    TList *refList;
    TList *refTable;
    TList *runPub;
    BmnRunInfo *CurRun;

    BmnDataReceiver *dataReceiver;
    BmnOnlineDecoder *onlineDecoder;

    Bool_t keepWorking;
    Int_t _webPort;
    Int_t fTest;
    Int_t fPeriodID;
    BmnSetup fSetup;
    Int_t fRunID;
    Int_t fEvents;
    BmnWorkerState fState;
    Int_t itersToUpdate;
    Int_t decoTimeout;
    


    ClassDef(BmnMonitor, 1)
};

#endif
