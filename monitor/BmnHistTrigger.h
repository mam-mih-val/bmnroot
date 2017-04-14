/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   BmnHistTrigger.h
 * Author: ilnur
 *
 * Created on November 27, 2016, 4:18 PM
 */

#ifndef BMNHISTTRIGGER_H
#define BMNHISTTRIGGER_H 1
#include <TNamed.h>
#include <exception>
#include <stdlib.h>
#include <vector>
#include "TChain.h"
#include "TClonesArray.h"
#include "TH1F.h"
#include "TH1D.h"
#include "TH2F.h"
#include "TCanvas.h"
#include "TFile.h"
#include "THttpServer.h"

#include "BmnRawDataDecoder.h"
#include "BmnHist.h"
#include "BmnTrigDigit.h"
#include "BmnEventHeader.h"
#define BD_CHANNELS 40
#define TRIG_ROWS    3
#define TRIG_COLS    3

class BmnHistTrigger : public BmnHist {
public:
    BmnHistTrigger(TString title = "Triggers");
    virtual ~BmnHistTrigger();

    void Register(THttpServer *serv);
    void SetDir(TFile *outFile, TTree *recoTree);
    void FillFromDigi(DigiArrays *fDigiArrays);
    void Reset();
    void DrawBoth();
//    BmnStatus LoadRefRun(TString FileName);
    BmnStatus  SetRefRun(Int_t id);

    void SetBDChannel(Int_t iSelChannel);

    Int_t GetBDChannel() {
        return fSelectedBDChannel;
    }

private:
    vector<TString> Names;
    TClonesArray *BDEvents;
    TH1D *histBC1TimeLen;
    TH1D *histBC2TimeLen;
    TH1D *histSDTimeLen;
    TH1D *histVDTimeLen;
    TH1D *histFDTimeLen;
    TH1I *histBDChannels;
    TH1I *histBDSimult;
    TH1I *histBDTime;
    TH1D *histBDSpecific;
    TH1I *histTriggers;
    TCanvas *canTimes;
    vector<PadInfo*> canTimesPads;
    Int_t fSelectedBDChannel;

    ClassDef(BmnHistTrigger, 1)
};

#endif /* BMNHISTTRIGGER_H */

