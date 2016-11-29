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

#include "BmnTrigDigit.h"
#define BD_CHANNELS 40

class BmnHistTrigger : public TNamed {
public:
    BmnHistTrigger(TString title);
    BmnHistTrigger(const BmnHistTrigger& orig);
    virtual ~BmnHistTrigger();

    void Register(THttpServer *serv, TDirectory *dir, TTree *recoTree);
    void FillFromDigi(
            TClonesArray * BC1digits,
            TClonesArray * SDdigits,
            TClonesArray * BC2digits,
            TClonesArray * VDdigits,
            TClonesArray * FDdigits,
            TClonesArray * BDdigits);
    void Reset();
    void SaveHists(TString imgSavePath);

    void SetBDChannel(Int_t iSelChannel);

    Int_t GetBDChannel() {
        return fSelectedBDChannel;
    }

    TClonesArray *BDEvents;
    TH1D *histBC1TimeLen;
    TH1D *histBC2TimeLen;
    TH1D *histSDTimeLen;
    TH1D *histVDTimeLen;
    TH1D *histFDTimeLen;
    TH1I *histBDChannels;
    TH1D *histBDSpecific;
    TH1I *histTriggers;
private:
    THttpServer *fServer;
    TTree *frecoTree;
    Int_t fSelectedBDChannel;

    void SaveHist(TH1 *hist, TString path) {
        TCanvas *c0 = new TCanvas("c0", hist->GetTitle());
        hist->Draw();
        c0->SaveAs(TString(path + TString(hist->GetTitle()) + ".png").Data());
        delete c0;
    }

    ClassDef(BmnHistTrigger, 1)
};

#endif /* BMNHISTTRIGGER_H */

