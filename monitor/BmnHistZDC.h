/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   BmnHistZDC.h
 * Author: ilnur
 *
 * Created on March 17, 2017, 2:48 PM
 */

#ifndef BMNHISTZDC_H
#define BMNHISTZDC_H 1

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
#include "BmnZDCDigit.h"
#define ZDC_ROWS    1
#define ZDC_COLS    2


class BmnHistZDC : public BmnHist {
public:
    BmnHistZDC(TString title = "ZDC");
    virtual ~BmnHistZDC();
    void Reset();
    void Register(THttpServer *serv);
    void SetDir(TFile *outFile, TTree *recoTree);
    void DrawBoth();
    void FillFromDigi(TClonesArray * DchDigits);
    BmnStatus  SetRefRun(Int_t id);
private:
    TClonesArray* ZDCHits;
    TH2D* h2d_grid;
    TH2D* h2d_profile;
    TH1D* hx;
    TH1D* hy;
    TCanvas *canAmps;
    vector<PadInfo*> canAmpsPads;
    vector<TString> NamesAmps;

    ClassDef(BmnHistZDC, 1)
};

#endif /* BMNHISTZDC_H */

