#ifndef BMNMQSOURCE_H
#define BMNMQSOURCE_H

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
#include "BmnParts.h"
#include "DstEventHeader.h"
#include "BmnEventHeader.h"
//#include "BmnTof1Digit.h"
//#include "BmnTof2Digit.h"
//#include "BmnGemStripDigit.h"
//#include "BmnSiliconDigit.h"

class BmnMQSource : public FairOnlineSource {
public:
    BmnMQSource(std::string addr = "tcp://localhost:6666", Bool_t toFile = kFALSE);
    virtual ~BmnMQSource();
    
    Bool_t Init();
    Int_t ReadEvent(UInt_t i=0);
    void Close();
//    void Reset();
    void FillEventHeader(FairEventHeader* feh);
    FairRunAna * GetRunInstance(){ return fRunInst;}
    void SetRunInstance(FairRunAna * run){ fRunInst = run;}
    
private:
    TString GetDstNameFromRunId(Int_t runId) {
        TString name(Form("bmn_run%d_dst.root", runId));
        return name;
    }
    Bool_t InitZMQ();
    FairRunAna *fRunInst;
    zmq_msg_t _msg;
    TBufferFile *_tBuf;
    void * _ctx;
    void * _decoSocket;
    std::string _addrString;
    std::vector<TNamed*> fNamVec;
    std::vector<TClonesArray*> fArrVec;
    
    Bool_t fFirstEvent;
    Bool_t fToFile;
    std::string fT0BranchName;
    
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
    ClassDef(BmnMQSource, 1);
};

#endif /* BMNMQSOURCE_H */