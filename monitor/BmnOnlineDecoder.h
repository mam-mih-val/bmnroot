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
#include "TServerSocket.h"
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

class BmnOnlineDecoder : public TNamed {
public:
    BmnOnlineDecoder();
    virtual ~BmnOnlineDecoder();
    BmnStatus Decode();
private:
    void InitDecoder();
    void InitDecoder(TString);
    void ProcessFileRun(TString digiName = "$VMCWORKDIR/macro/raw/bmn_run0084_digi.root");
    static TString WatchNext(TString dirname, TString filename, Int_t cycleWait);
    
    BmnRawDataDecoder *rawDataDecoder;
    TServerSocket *fRawDecoSocket;
    TString _curFile;
    TString _curDir;
    

    ClassDef(BmnOnlineDecoder, 1)
};

#endif /* BMNONLINEDECODER_H */

