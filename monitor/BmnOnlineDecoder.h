/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   BmnOnlineDecoder.h
 * Author: ilnur
 *
 * Created on January 11, 2017, 11:08 AM
 */

#ifndef BMNONLINEDECODER_H
#define BMNONLINEDECODER_H 1
#include <climits>
//#include <regex>
//FairSoft
#include "TNamed.h"
#include "TFile.h"
#include "TChain.h"
#include "TClonesArray.h"
#include "TMessage.h"
#include "TServerSocket.h"
#include "TSocket.h"
#include "TString.h"
#include "TBuffer.h"
#include "TObjString.h"
#include <FairRunAna.h>
// BmnRoot
#include <BmnTrigDigit.h>
#include <BmnTof1Digit.h>
#include <BmnGemStripDigit.h>
#include <BmnDchDigit.h>
#include "BmnDataReceiver.h"
#include <BmnRawDataDecoder.h>
#include "BmnEventHeader.h"

#define RAW_DECODER_SOCKET_PORT 5555
#define RUN_FILE_CHECK_PERIOD    1e5
#define DECO_SOCK_WAIT_PERIOD     10
#define DECO_SOCK_WAIT_LIMIT     5*60e3
#define INOTIF_BUF_LEN (255 * (sizeof(struct inotify_event) + 255))
#define DAQ_ADDR     "bmn-daq"  //"bmn-hrb-3.jinr.ru"
#define DAQ_IP         "10.18.11.200"//"10.18.11.193"//
#define DAQ_PORT               32999
#define MPD_EVENT_HEAD_WORDS       3 // sync + payload lenght + iEv
#define UNKNOWN_RUNID           9999

using namespace std;

//Int_t IsRaw(struct dirent * entry) {
//        const regex re(".*(\\d+).*.data");
//        return regex_match(entry->d_name, re);
//    }

class BmnOnlineDecoder : public TNamed {
public:
    BmnOnlineDecoder();
    virtual ~BmnOnlineDecoder();
    BmnStatus Decode(TString dirname, TString startFile, Bool_t runCurrent);
    BmnStatus DecodeStream();
    BmnStatus OpenStream();
    void ProcessStream();
    BmnStatus CloseStream();
    BmnStatus BatchDirectory(TString dirname);
    void SetBmnSetup(BmnSetup v) {
        this->fBmnSetup = v;
    }
    BmnSetup GetBmnSetup() const {
        return fBmnSetup;
    }
    void SetPeriodID(Int_t v) {
        this->fPeriodID = v;
    }
    Int_t GetPeriodID() const {
        return fPeriodID;
    }
private:
    BmnStatus InitDecoder(TString);
    BmnStatus InitDecoder(Int_t runID);
    BmnStatus InitReco();
    BmnStatus IterReco();
    void ProcessFileRun(TString digiName, UInt_t timeLimit = WAIT_LIMIT);
    static TString WatchNext(TString dirname, TString filename, Int_t cycleWait);
    static TString WatchNext(Int_t inotifDir, Int_t cycleWait);
    static void threadReceiveWrapper(BmnDataReceiver * dr);
    static Int_t GetRunIdFromName(TString name);
    
    void * _ctx;
    void * _decoSocket;
    BmnSetup fBmnSetup;
    BmnRawDataDecoder *rawDataDecoder;
    TChain *fRecoChain;
    FairRunAna* fRunAna;
    TString _curFile;
    TString _curDir;
    Int_t fEvents;
    Int_t fPeriodID;
    UInt_t fRunID;
    BmnDataReceiver *dataReceiver;
    Int_t _inotifDir;
    Int_t _inotifDirW;
    Int_t _inotifFile;
    Int_t _inotifFileW;
    
    deque<UInt_t> data_queue;
    void * _socket_mcast;
    void * _socket_data;
    Byte_t buf[MAX_BUF_LEN];
//    UInt_t buf[MAX_BUF_LEN];

    ClassDef(BmnOnlineDecoder, 1)
};

#endif /* BMNONLINEDECODER_H */

