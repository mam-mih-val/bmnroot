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
#include <root/TPRegexp.h>
#include "TObjString.h"
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
#define RUN_FILE_CHECK_PERIOD    1e5
#define DECO_SOCK_WAIT_PERIOD    5e2
#define DECO_SOCK_WAIT_LIMIT     5e4
#define INOTIF_BUF_LEN (255 * (sizeof(struct inotify_event) + 255))

class BmnOnlineDecoder : public TNamed {
public:
    BmnOnlineDecoder();
    virtual ~BmnOnlineDecoder();
    BmnStatus Decode(TString dirname, TString startFile, Bool_t runCurrent);
    BmnStatus DecodeStream();
    BmnStatus OpenStream();
    BmnStatus BatchDirectory(TString dirname);
    void SetBmnSetup(BmnSetup v) {
        this->fBmnSetup = v;
    }
    BmnSetup GetBmnSetup() const {
        return fBmnSetup;
    }
private:
    BmnStatus InitDecoder(TString);
    void ProcessFileRun(TString digiName, UInt_t timeLimit = WAIT_LIMIT);
    static TString WatchNext(TString dirname, TString filename, Int_t cycleWait);
    static TString WatchNext(Int_t inotifDir, Int_t cycleWait);
    static void threadReceiveWrapper(BmnDataReceiver * dr);
    static Int_t GetRunIdFromName(TString name);
    
    void * _ctx;
    void * _decoSocket;
    BmnSetup fBmnSetup;
    BmnRawDataDecoder *rawDataDecoder;
    TString _curFile;
    TString _curDir;
    Int_t fEvents;
    BmnDataReceiver *dataReceiver;
    deque<UInt_t> *dataQue;
    Int_t _inotifDir;
    Int_t _inotifDirW;
    Int_t _inotifFile;
    Int_t _inotifFileW;
    

    ClassDef(BmnOnlineDecoder, 1)
};

#endif /* BMNONLINEDECODER_H */

