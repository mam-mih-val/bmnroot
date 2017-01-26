#ifndef BMNMONITOR_H
#define BMNMONITOR_H 1
// STL
#include <iostream>
#include <iterator>
#include <regex>
// FairROOT
#include <TNamed.h>
#include "TFile.h"
#include "TChain.h"
#include "TClonesArray.h"
#include "TColor.h"
#include "TFolder.h"
#include "THttpServer.h"
#include "TMessage.h"
#include "TMonitor.h"
#include "TString.h"
#include "TSystemDirectory.h"
#include "TList.h"
#include "TSystemFile.h"
#include "TServerSocket.h"
// BmnRoot
#include <BmnTrigDigit.h>
#include <BmnTof1Digit.h>
#include <BmnGemStripDigit.h>
#include <BmnDchDigit.h>
#include "BmnDataReceiver.h"
#include <BmnRawDataDecoder.h>
#include "BmnEventHeader.h"
// BmnRoot Monitor
#include "BmnHistToF.h"
#include "BmnHistToF700.h"
#include "BmnHistTrigger.h"
#include "BmnHistDch.h"
#include "BmnHistMwpc.h"
#include "BmnHistGem.h"
#include "BmnOnlineDecoder.h"

#define RAW_DECODER_SOCKET_PORT 9090
#define RUN_FILE_CHECK_PERIOD    1e5
#define DECO_SOCK_WAIT_PERIOD    1e2
#define DECO_SOCK_WAIT_LIMIT     5e4
#define TTREE_MAX_SIZE          3e11

using namespace std;
using namespace TMath;

struct BmnRunInfo {
    TString Name;
    struct stat attr;
};

class BmnMonitor : public TNamed {
public:

    BmnMonitor();
    virtual ~BmnMonitor();
    void Monitor(TString dir, TString startFile = "", Bool_t runCurrent = kTRUE);
    void MonitorStream(TString dir, TString startFile = "", Bool_t runCurrent = kTRUE);
    BmnStatus BatchDirectory(TString dirname);
    BmnStatus BatchList(TString*, Int_t count);
    void ProcessRun(TString digiName = "$VMCWORKDIR/macro/raw/bmn_run0084_digi.root");
    void ProcessStreamRun();
    void ProcessFileRun(TString digiName = "$VMCWORKDIR/macro/raw/bmn_run0084_digi.root");
    TString WatchNext(TString dirname, TString filename, Int_t cycleWait);
    static TString WatchNext(Int_t inotifDir, Int_t cycleWait);
    
    // Getters
    deque<UInt_t> * GetDataQue() { return fDataQue;}
    TTree * GetDigiTree()       { return fDigiTree;}

    // Setters
    void SetDataQue(deque<UInt_t> * v) { fDataQue = v;}
    void SetDigiTree(TTree * v)       { fDigiTree = v;}

private:
    void InitServer();
    void InitDecoder();
    void ProcessDigi(Int_t iEv);
    void RegisterAll();
    static void CheckFileTime(TString Dir, vector<BmnRunInfo>* FileList);
    BmnStatus CreateFile(Int_t runID = 0);
    BmnStatus OpenStream();
    void FinishRun();
    
    static void threadReceiveWrapper(BmnDataReceiver * dr);
    static void threadDecodeWrapper(TString dirname, TString startFile, Bool_t runCurrent);
    
    deque<UInt_t> * fDataQue;
    vector<BmnRunInfo> *_fileList;
    TString _curFile;
    TString _curDir;
    TString fRawDecoAddr;
    TTree *fDigiTree;
    TTree *fRecoTree;
    TTree *fRecoTree4Show;
    TFile *fHistOut;
    THttpServer * fServer;
    TSocket *fRawDecoSocket;
//    struct DigiArrays fDigiArrays;
    DigiArrays *fDigiArrays;

//    TClonesArray *header = NULL;
//    TClonesArray *gemDigits = NULL;
//    TClonesArray *ToF4Digits = NULL;
//    TClonesArray *ToF7Digits = NULL;
//    TClonesArray *DchDigits = NULL;
//    TClonesArray *trigBC1Digits = NULL;
//    TClonesArray *trigBC2Digits = NULL;
//    TClonesArray *trigVDDigits = NULL;
//    TClonesArray *trigBDDigits = NULL;
//    TClonesArray *trigSDDigits = NULL;
//    TClonesArray *trigFDDigits = NULL;

    BmnHistGem     *bhGem;
    BmnHistToF     *bhToF400;
    BmnHistToF700  *bhToF700;
    BmnHistDch     *bhDCH;
    BmnHistMwpc    *bhMWPC;
    BmnHistTrigger *bhTrig;
    
    BmnHistGem     *bhGem_4show;
    BmnHistToF     *bhToF400_4show;
    BmnHistToF700  *bhToF700_4show;
    BmnHistDch     *bhDCH_4show;
    BmnHistMwpc    *bhMWPC_4show;
    BmnHistTrigger *bhTrig_4show;
    
    TCanvas *infoCanvas;

    BmnDataReceiver *dataReceiver;
    BmnRawDataDecoder *rawDataDecoder;
    BmnOnlineDecoder *onlineDecoder;

    Int_t fTest;
    Int_t fRunID;
    BmnWorkerState fState;
    Int_t itersToUpdate;
    Int_t decoTimeout;
    
    Int_t _inotifDir;
    Int_t _inotifDirW;
    Int_t _inotifFile;
    Int_t _inotifFileW;


    ClassDef(BmnMonitor, 1)
};

#endif