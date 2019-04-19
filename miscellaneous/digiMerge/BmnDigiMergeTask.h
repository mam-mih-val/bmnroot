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

#include <BmnDigiContainer.h>

#ifndef BMNDIGIMERGETASK_H
#define BMNDIGIMERGETASK_H 1

using namespace std;
using namespace TMath;

class BmnDigiMergeTask : public TNamed {
public:

    /** Default constructor **/
    BmnDigiMergeTask() {
    };

    // RUN6 (split to detectors)
    BmnDigiMergeTask(TString, TString, TString);

    // RUN7 (split to detectors)
    BmnDigiMergeTask(TString, TString);

    // All-digits-together
    BmnDigiMergeTask(vector <TString>, TString);

    /** Destructor **/
    virtual ~BmnDigiMergeTask() {
        if (fOutTree && fOutFile) {
            fOutTree->Write();
            fOutFile->Close();
        }

        if (fOutTrees && fOutFiles)
            for (Int_t iDet = 0; iDet < nDets; iDet++) {
                fOutFiles[iDet]->cd();
                fOutTrees[iDet]->Write();
                fOutFiles[iDet]->Close();
            }
    }

    // Setters

    void SetNevsInSample(UInt_t nEvs) {
        fNevsInSample = nEvs;
    }

    void SetNevsToBeProcessed(UInt_t nEvs) {
        fNevsToBeProcessed = nEvs;
    }

    void ProcessEvents();

    void SplitToDetectors() {
        isRun6 ? SplitToDetectorsRun6() : isRun7 ? SplitToDetectorsRun7() : throw;
    }

private:
    Bool_t isRun6;
    Bool_t isRun7;

    UInt_t fNevsInSample;
    UInt_t fNevsToBeProcessed;

    TChain* fIn1;
    TChain* fIn2;
    TChain* fIn3;

    TChain** fInFiles;

    TClonesArray* fGemDigits;
    TClonesArray* fCscDigits;
    TClonesArray* fSilDigits;
    TClonesArray* fZdcDigits;
    TClonesArray* fT0Digits;
    TClonesArray* fBC1Digits;
    TClonesArray* fBC2Digits;
    TClonesArray* fBC3Digits;
    TClonesArray* fVetoDigits;
    TClonesArray* fFdDigits;
    TClonesArray* fSiDigits;
    TClonesArray* fBdDigits;
    TClonesArray* fMwpcDigits;
    TClonesArray* fDchDigits;
    TClonesArray* fTof400Digits;
    TClonesArray* fTof700Digits;
    TClonesArray* fEcalDigits;

    TClonesArray* fGem;
    TClonesArray* fCsc;
    TClonesArray* fSil;
    TClonesArray* fZdc;
    TClonesArray* fT0;
    TClonesArray* fBC1;
    TClonesArray* fBC2;
    TClonesArray* fBC3;
    TClonesArray* fSi;
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
    TFile** fOutFiles;

    TTree* fOutTree;
    TTree** fOutTrees;

    Int_t nDets;
    TClonesArray** fInArrs;
    TClonesArray** fOutArrs;
    TClonesArray** fHeaders;

    void FillDigiContainer(UInt_t, UInt_t);
    void GlueEventsFromInputFiles(Bool_t, UInt_t, UInt_t);
    void CreateOutputFile(TString);
    void CreateOutputFiles();

    void SplitToDetectorsRun6();
    void SplitToDetectorsRun7();
    
    Bool_t IsArraysEmpty();

    ClassDef(BmnDigiMergeTask, 1);

};

#endif