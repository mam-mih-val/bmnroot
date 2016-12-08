#ifndef BMNMONITOR_H
#define BMNMONITOR_H 1

#include <iostream>
#include <iterator>

#include <TNamed.h>
#include "TFile.h"
#include "TChain.h"
#include "TClonesArray.h"
#include "TColor.h"
#include "TFolder.h"
#include "THttpServer.h"
#include "TString.h"

#include <BmnTrigDigit.h>
#include <BmnTof1Digit.h>
#include <BmnGemStripDigit.h>
#include <BmnDchDigit.h>
#include "BmnDataReceiver.h"
#include <BmnRawDataDecoder.h>
#include "BmnEventHeader.h"

#include "BmnHistToF.h"
#include "BmnHistToF700.h"
#include "BmnHistTrigger.h"
#include "BmnHistDch.h"
#include "BmnHistGem.h"

#define RUN_FILE_CHECK_PERIOD 100000

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
    void Monitor();
    void ProcessRun(TString digiName = "$VMCWORKDIR/macro/raw/bmn_run0084_digi.root");
    void ProcessStreamRun();
    void ProcessFileRun(TString digiName = "$VMCWORKDIR/macro/raw/bmn_run0084_digi.root");
    
    // Getters
    deque<UInt_t> * GetDataQue() { return fDataQue;}
    TTree * GetDigiTree()       { return fDigiTree;}

    // Setters
    void SetDataQue(deque<UInt_t> * v) { fDataQue = v;}
    void SetDigiTree(TTree * v)       { fDigiTree = v;}

private:
    void ProcessDigi(Int_t iEv);
    void RegisterAll();
    static void CheckFileTime(TString Dir, vector<BmnRunInfo>* FileList);
    BmnStatus OpenFile(TString rawFileName);
    BmnStatus OpenStream();
    void FinishRun();
    
    static void threadWrapper(BmnDataReceiver * dr);
    static void threadDecodeWrapper(BmnRawDataDecoder* rdd);
    
    deque<UInt_t> * fDataQue;
    vector<BmnRunInfo> *_fileList;
//    void *fDataMutex; // actually pthread_mutex_t
    TTree *fDigiTree;
    TTree *fRecoTree = NULL;
    TFile *fHistOut = NULL;
    THttpServer * fServer = NULL;
    struct DigiArrays fDigiArrays;

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
    BmnHistTrigger *bhTrig;
    
    BmnHistGem     *bhGem_4show;
    BmnHistToF     *bhToF400_4show;
    BmnHistToF700  *bhToF700_4show;
    BmnHistDch     *bhDCH_4show;
    BmnHistTrigger *bhTrig_4show;

    BmnDataReceiver *dataReceiver;
    BmnRawDataDecoder *rawDataDecoder;

    Int_t fTest;
    Int_t runIndex = 0;
    TString imgSavePath = "~/Documents/BmnMonJS/public_html/img/";
    Int_t itersToUpdate = 1000;
    // GEM config
    Int_t hitBins = 100;


    ClassDef(BmnMonitor, 1)
};

#endif