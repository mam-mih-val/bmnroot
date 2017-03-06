/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   BmnHistMwpc.h
 * Author: ilnur
 *
 * Created on December 13, 2016, 2:47 PM
 */

#ifndef BMNHISTMWPC_H
#define BMNHISTMWPC_H 1

#include <TNamed.h>
#include "TChain.h"
#include "TColor.h"
#include "TClonesArray.h"
#include "TH1F.h"
#include "TH1D.h"
#include "TH2F.h"
#include "TCanvas.h"
#include "TFile.h"
#include "TFolder.h"
#include "TString.h"
#include "THttpServer.h"

#include "BmnHist.h"
#include "BmnMwpcDigit.h"
#include "BmnEventHeader.h"

#define MWPC_PLANES 12
#define MWPC_WIRES 96
#define MWPC_ROWS    4
#define MWPC_COLS    (MWPC_PLANES/MWPC_ROWS)

class BmnHistMwpc : public BmnHist {
public:
    BmnHistMwpc(TString title);
    virtual ~BmnHistMwpc();
    void Reset();
    void Register(THttpServer *serv);
    void SetDir(TFile *outFile, TTree *recoTree);
    void DrawBoth();
    void FillFromDigi(TClonesArray * DchDigits);
    BmnStatus LoadRefRun(TString FileName);
    BmnStatus  SetRefRun(Int_t id);

private:
    TH1F * h_wires[MWPC_PLANES];
    TH1F * h_times[MWPC_PLANES];
    TClonesArray* MwpcHits;
    TH2F* h_MWPC1;
    TH2F* h_MWPC2;
    TCanvas *canWires;
    TCanvas *canTimes;
    vector<PadInfo*> canWiresPads;
    vector<PadInfo*> canTimesPads;

    ClassDef(BmnHistMwpc, 1)
};

#endif /* BMNHISTMWPC_H */

