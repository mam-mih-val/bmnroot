#include <TFile.h>
#include <TTree.h>
#include <TNamed.h>
#include <TMath.h>
#include <TChain.h>
#include <TString.h>
#include <TClonesArray.h>
#include <iostream>
#include <map>
#include <vector>

#include <BmnEventHeader.h>
#include <BmnDigiContainer.h>
#include <BmnGemStripDigit.h>

#ifndef BMNDIGIMERGETASK_H
#define BMNDIGIMERGETASK_H 1

using namespace std;
using namespace TMath;

class BmnDigiMergeTask : public TNamed {
public:

    /** Default constructor **/
    BmnDigiMergeTask() {
    };

    BmnDigiMergeTask(TString, TString, TString, TString);

    /** Destructor **/
    virtual ~BmnDigiMergeTask() {
        fOutTree->Write();
        fOutFile->Close();
    }

    // Setters
    void SetNevsInSample(UInt_t nEvs) {
        fNevsInSample = nEvs;   
    } 

    void SetNevsToBeProcessed(UInt_t nEvs) {
        fNevsToBeProcessed = nEvs;   
    } 
    
    void ProcessEvents();

private:    
    UInt_t fNevsInSample;
    UInt_t fNevsToBeProcessed;

    TChain* fIn1;
    TChain* fIn2;
    TChain* fIn3;

    TClonesArray* fGemDigits;
    TClonesArray* fSilDigits;
    TClonesArray* fZdcDigits;
    TClonesArray* fT0Digits;
    TClonesArray* fBC1Digits;
    TClonesArray* fBC2Digits;
    TClonesArray* fVetoDigits;
    TClonesArray* fFdDigits;
    TClonesArray* fBdDigits;
    TClonesArray* fMwpcDigits;
    TClonesArray* fDchDigits;
    TClonesArray* fTof400Digits;
    TClonesArray* fTof700Digits;
    TClonesArray* fEcalDigits;
       
    TClonesArray* fGem;
    TClonesArray* fSil;
    TClonesArray* fZdc;
    TClonesArray* fT0;
    TClonesArray* fBC1;
    TClonesArray* fBC2;
    TClonesArray* fVeto;
    TClonesArray* fFd;
    TClonesArray* fBd;
    TClonesArray* fMwpc;
    TClonesArray* fDch;
    TClonesArray* fTof400;
    TClonesArray* fTof700;
    TClonesArray* fEcal;

    // Three versions of BmnEventHeader to be read from files (Here fuck my life twice!)
    TClonesArray* fHeader1;
    TClonesArray* fHeader2;
    TClonesArray* fHeader3;
    
    TClonesArray* fHeaderOut;

    BmnDigiContainer* fCont;
    
    TFile* fOutFile;
    TTree* fOutTree;

    void FillDigiContainer(UInt_t, UInt_t);
    pair <UInt_t, UInt_t> FindStartFinishEventId();
    void GlueEventsFromInputFiles();
    void CreateOutputFile(TString);

    ClassDef(BmnDigiMergeTask, 1);

};

#endif