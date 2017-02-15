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

class BmnHistMwpc : public BmnHist {
public:
    BmnHistMwpc(TString title);
    virtual ~BmnHistMwpc();
    void Reset();
    void Register(THttpServer *serv);
    void SetDir(TFile *outFile, TTree *recoTree);
    void FillFromDigi(TClonesArray * DchDigits);

private:
    TH1F * h_wires[MWPC_PLANES];
    TClonesArray* MwpcHits;
    TH2F* h_MWPC1;
    TH2F* h_MWPC2;

    ClassDef(BmnHistMwpc, 1)
};

#endif /* BMNHISTMWPC_H */

