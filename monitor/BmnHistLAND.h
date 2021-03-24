/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   BmnHistLAND.h
 * Author: ilnur
 *
 * Created on 23 января 2018 г., 12:50
 */

#ifndef BMNHISTLAND_H
#define BMNHISTLAND_H
// FairSoft
#include <TNamed.h>
#include "TChain.h"
#include "TClonesArray.h"
#include "TGaxis.h"
#include "TH1F.h"
#include "TH1D.h"
#include "TH2F.h"
#include "TCanvas.h"
#include "TFile.h"
#include "TFolder.h"
#include "THttpServer.h"
// BmnRoot
#include "BmnHist.h"
#include "BmnLANDDigit.h"
#define BAR_COUNT   120
#define TOF_MIN    -200
#define TOF_MAX     200
#define Q_MAX      4000
#define T_MAX      4000
#define CANVAS_ROWS   3
#define CANVAS_COLS   2

class BmnHistLAND : public BmnHist {
public:
    BmnHistLAND(TString title, TString path = "");
    virtual ~BmnHistLAND();
    
    void Register(THttpServer *serv);
    void SetDir(TFile *outFile = NULL, TTree *recoTree = NULL);
    void Reset();
    void DrawBoth();
    void ClearRefRun();
    BmnStatus  SetRefRun(Int_t id);
        
    void FillFromDigi(DigiArrays *fDigiArrays);
private:
    TH2F *Q0vsBar;
    TH2F *Q1vsBar;
    TH2F *T0vsBar;
    TH2F *T1vsBar;
    TH2F *TDiffvsBar;
    TH2F *QvsToF;
    vector<TString> Names;
    TCanvas *can;
    vector<PadInfo*> canPads;

    ClassDef(BmnHistLAND, 1)
};

#endif /* BMNHISTLAND_H */

