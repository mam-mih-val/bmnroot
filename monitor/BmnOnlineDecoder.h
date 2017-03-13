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
#include <regex>
//FairSoft
#include "TNamed.h"
#include "TFile.h"
#include "TChain.h"
#include "TClonesArray.h"
#include "TMessage.h"
#include "TServerSocket.h"
#include "TSocket.h"
#include "TString.h"
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

class BmnOnlineDecoder : public TNamed {
public:
    BmnOnlineDecoder();
    virtual ~BmnOnlineDecoder();
    BmnStatus Accept();
    BmnStatus Decode(TString dirname, TString startFile, Bool_t runCurrent);
    BmnStatus BatchDirectory(TString dirname);
private:
    void InitDecoder(TString);
    void ProcessFileRun(TString digiName = "$VMCWORKDIR/macro/raw/bmn_run0084_digi.root");
    static TString WatchNext(TString dirname, TString filename, Int_t cycleWait);
    
    BmnRawDataDecoder *rawDataDecoder;
    TServerSocket *fRawDecoSocket;
    TSocket *client;
    vector <TSocket*> clients;//[MAX_CLIENTS];
    Int_t iClients;
    TString _curFile;
    TString _curDir;
    Int_t fEvents;
    

    ClassDef(BmnOnlineDecoder, 1)
};

#endif /* BMNONLINEDECODER_H */

