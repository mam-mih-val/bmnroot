#ifndef BMNDECOSOURCE_H
#define BMNDECOSOURCE_H

#include <iostream>
#include <string>
#include <cstring>
#include <vector>
#include <zmq.h>
// ROOT
#include "TString.h"
#include "TChain.h"
#include <TBufferFile.h>
// FairRoot
#include "FairRootManager.h"
#include "FairRootFileSink.h"
#include "FairOnlineSource.h"
#include "FairEventHeader.h"
#include "FairRunAna.h"
// BmnRoot
#include "BmnEnums.h"
#include "DigiArrays.h"
#include "BmnEventHeader.h"
//#include "BmnTof1Digit.h"
//#include "BmnTof2Digit.h"
//#include "BmnGemStripDigit.h"
//#include "BmnSiliconDigit.h"

using namespace std;

class BmnDecoSource : public FairOnlineSource {
public:
    BmnDecoSource(TString addr = "localhost");
    virtual ~BmnDecoSource();
    
    Bool_t Init();
    Int_t ReadEvent(UInt_t i=0);
    void Close();
//    void Reset();
    void FillEventHeader(FairEventHeader* feh);
    TTree* GetInTree() {return fInChain->GetTree();}
    TChain* GetInChain() {return fInChain;}
    FairRunAna * GetRunInstance(){ return fRunInst;}
    void SetRunInstance(FairRunAna * run){ fRunInst = run;}
    
private:
    TString GetDstNameFromRunId(Int_t runId) {
        TString name(Form("bmn_run%d_dst.root", runId));
        return name;
    }
    FairRunAna *fRunInst;
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
    
    Bool_t fFirstEvent;
    string fT0BranchName;
    
    Int_t fRunId;
    Int_t fPeriodId;

    Int_t iEventNumber;
    Int_t iT0BranchIndex;
    BmnEventHeader* fEventHeader;
    TClonesArray* fGemDigits;
    TClonesArray* fSilDigits;
    TClonesArray* fCscDigits;
    TClonesArray* fTof400Digits;
    TClonesArray* fTof700Digits;
    TClonesArray* fT0Digits;
    ClassDef(BmnDecoSource, 1);
};

#endif /* BMNDECOSOURCE_H */