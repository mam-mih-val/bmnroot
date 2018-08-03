#ifndef BMNDECOSOURCE_H
#define BMNDECOSOURCE_H

#include <iostream>
#include <zmq.h>
// ROOT
#include "TString.h"
#include "TBufferFile.h"
#include "TChain.h"
// FairRoot
#include "FairRootManager.h"
#include "FairOnlineSource.h"
// BmnRoot
#include "BmnEnums.h"
#include "DigiArrays.h"
#include "BmnEventHeader.h"

class BmnDecoSource : public FairOnlineSource {
public:
    BmnDecoSource(TString addr);
    BmnDecoSource(const BmnDecoSource& orig);
    virtual ~BmnDecoSource();
    
    Bool_t Init();
    Int_t ReadEvent(UInt_t i=0);
    void Close();
//    void Reset();
    TTree* GetInTree() {return fInChain->GetTree();}
    TChain* GetInChain() {return fInChain;}
    
private:
    zmq_msg_t _msg;
    TBufferFile *_tBuf;//= TBufferFile((TBuffer::EMode)0);// = TBufferFile(TBuffer::kRead);
    void * _ctx;
    void * _decoSocket;
    TString _addrString;
    DigiArrays *fDigiArrays;
    /**Input Chain */
    TChain* fInChain;
    /**Input Tree */
    TTree* fInTree;

    Int_t iEventNumber;
    TClonesArray* fEventHeader;
    TClonesArray* fGemDigits;
    TClonesArray* fTof1Digits;
    TClonesArray* fT0Digits;
    ClassDef(BmnDecoSource, 1);
};

#endif /* BMNDECOSOURCE_H */

