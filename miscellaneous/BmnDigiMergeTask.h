#include <TSystem.h>
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

#include <BmnSiliconStationSet.h>
#include <BmnGemStripStationSet.h>
#include <BmnCSCStationSet.h>

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
    BmnDigiMergeTask(TString*);

    // RUN7 (split to detectors), to be used in case of SRC only
    BmnDigiMergeTask(vector <TString>);

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

    void SplitToDetectors(Bool_t src = kFALSE) {
        if (!src)
            isRun6 ? SplitToDetectorsRun6() : isRun7 ? SplitToDetectorsRun7() : throw;
        else
            SplitToDetectorsSrc();
    }

private:
    Bool_t isRun6;
    Bool_t isRun7;
    Bool_t isSrc;

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
    TClonesArray* fBC4Digits;
    TClonesArray* fBC1TQDCDigits;
    TClonesArray* fBC2TQDCDigits;
    TClonesArray* fBC3TQDCDigits;
    TClonesArray* fBC4TQDCDigits;
    TClonesArray* fX1LDigits;
    TClonesArray* fX2LDigits;
    TClonesArray* fY1LDigits;
    TClonesArray* fY2LDigits;
    TClonesArray* fX1RDigits;
    TClonesArray* fX2RDigits;
    TClonesArray* fY1RDigits;
    TClonesArray* fY2RDigits;
    TClonesArray* fX1LTQDCDigits;
    TClonesArray* fX2LTQDCDigits;
    TClonesArray* fY1LTQDCDigits;
    TClonesArray* fY2LTQDCDigits;
    TClonesArray* fX1RTQDCDigits;
    TClonesArray* fX2RTQDCDigits;
    TClonesArray* fY1RTQDCDigits;
    TClonesArray* fY2RTQDCDigits;
    TClonesArray* fVetoDigits;
    TClonesArray* fVetoTQDCDigits;
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
    TClonesArray* fBC4;
    TClonesArray* fBC1TQDC;
    TClonesArray* fBC2TQDC;
    TClonesArray* fBC3TQDC;
    TClonesArray* fBC4TQDC;
    TClonesArray* fX1L;
    TClonesArray* fX2L;
    TClonesArray* fY1L;
    TClonesArray* fY2L;
    TClonesArray* fX1R;
    TClonesArray* fX2R;
    TClonesArray* fY1R;
    TClonesArray* fY2R;
    TClonesArray* fX1LTQDC;
    TClonesArray* fX2LTQDC;
    TClonesArray* fY1LTQDC;
    TClonesArray* fY2LTQDC;
    TClonesArray* fX1RTQDC;
    TClonesArray* fX2RTQDC;
    TClonesArray* fY1RTQDC;
    TClonesArray* fY2RTQDC;
    TClonesArray* fSi;
    TClonesArray* fVeto;
    TClonesArray* fVetoTQDC;
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
    BmnEventHeader* fHeader2Obj;
    TClonesArray* fHeader3;
    BmnEventHeader* fHeader3Obj;

    //    TClonesArray* fHeaderOut;
    BmnEventHeader* fHeaderOut;

    BmnDigiContainer* fCont;

    TFile* fOutFile;
    TFile** fOutFiles;

    TTree* fOutTree;
    TTree** fOutTrees;

    Int_t nDets;
    TClonesArray** fInArrs;
    TClonesArray** fOutArrs;
    BmnEventHeader** fHeaders;

    BmnGemStripStationSet* fDetectorGEM;
    BmnSiliconStationSet* fDetectorSI;
    BmnCSCStationSet* fDetectorCSC;

    map <Int_t, Int_t> fGemStats;
    map <Int_t, Int_t> fSilStats;

    Long_t** evId_evNum;
    UInt_t maxEvId;

    inline Int_t GemStatPermutation(Int_t stat) {
        return fGemStats.find(stat)->second;
    }

    inline Int_t SiliconStatPermutation(Int_t stat) {
        return fSilStats.find(stat)->second;
    }

    void GlueEventsFromInputFiles();
    void CreateOutputFile(TString);
    void CreateOutputFiles();

    void SplitToDetectorsRun6();
    void SplitToDetectorsRun7();
    void SplitToDetectorsSrc();

    void CreateGeometries();
    void Run7(Int_t*, Int_t*, Int_t*, Int_t*);

    template <class T> void FillDetDigi(TClonesArray* inDigiArr, TClonesArray* outDigiArr) {
        for (UInt_t iDigi = 0; iDigi < inDigiArr->GetEntriesFast(); iDigi++)
            new ((*outDigiArr)[outDigiArr->GetEntriesFast()]) T(*(T*) inDigiArr->UncheckedAt(iDigi));
    };
 
    ClassDef(BmnDigiMergeTask, 1);

};

#endif