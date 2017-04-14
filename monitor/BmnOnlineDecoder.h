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
// BmnRoot
#include <BmnTrigDigit.h>
#include <BmnTof1Digit.h>
#include <BmnGemStripDigit.h>
#include <BmnDchDigit.h>
#include "BmnDataReceiver.h"
#include <BmnRawDataDecoder.h>
#include "BmnEventHeader.h"

#define RAW_DECODER_SOCKET_PORT 9090
#define RUN_FILE_CHECK_PERIOD    1e5
#define MAX_CLIENTS               20
#define RUN_FILE_CHECK_PERIOD    1e5
#define DECO_SOCK_WAIT_PERIOD    5e2
#define DECO_SOCK_WAIT_LIMIT     5e4
#define INOTIF_BUF_LEN (255 * (sizeof(struct inotify_event) + 255))

class BmnOnlineDecoder : public TNamed {
public:
    BmnOnlineDecoder();
    virtual ~BmnOnlineDecoder();
    BmnStatus Accept();
    BmnStatus Decode(TString dirname, TString startFile, Bool_t runCurrent);
    BmnStatus DecodeStream();
    BmnStatus OpenStream();
    BmnStatus BatchDirectory(TString dirname);
private:
    BmnStatus InitDecoder(TString);
    BmnStatus InitDecoder(Int_t periodID, Int_t runID, deque<UInt_t> *dq);
    void ProcessFileRun(TString digiName);
    static TString WatchNext(TString dirname, TString filename, Int_t cycleWait);
    static TString WatchNext(Int_t inotifDir, Int_t cycleWait);
    static void threadReceiveWrapper(BmnDataReceiver * dr);
    
    void * _ctx;
    void * _decoSocket;
    BmnRawDataDecoder *rawDataDecoder;
    TServerSocket *fRawDecoSocket;
    TSocket *client;
    vector <TSocket*> clients;//[MAX_CLIENTS];
    Int_t iClients;
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

