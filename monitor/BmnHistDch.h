/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   BmnHistDch.h
 * Author: ilnur
 *
 * Created on November 29, 2016, 2:44 PM
 */

#ifndef BMNHISTDCH_H
#define BMNHISTDCH_H 1
#include <TNamed.h>
#include <exception>
#include <stdlib.h>
#include <vector>
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
#include "TGaxis.h"

#include "BmnHist.h"
#include "BmnDchDigit.h"
#include "BmnDchHit.h"
#include "BmnEventHeader.h"

#define kNPLANES     16
#define kNREALWIRES 240
#define kNWIRES     256
#define Number        4
#define DCH_WDTH    150
#define PLANE_ROWS    4
#define PLANE_COLS    (kNPLANES/PLANE_ROWS)

using namespace std;

class BmnHistDch : public BmnHist {
public:
    BmnHistDch(TString title, TString path = "");
    virtual ~BmnHistDch();
    void Reset();
    void Register(THttpServer *serv);
    void SetDir(TFile *outFile, TTree *recoTree);
    void FillFromDigi(DigiArrays *fDigiArrays);
    void DrawBoth();
    void ClearRefRun();
    BmnStatus  SetRefRun(Int_t id);

private:
    void SetDir(TDirectory *Dir);
    vector<TString> NamesWires;
    vector<TString> NamesTimes;
    TH1F * h_wires[kNPLANES];
    TH1F * h_times[kNPLANES];
    TH2F* h_DCH1;
    TH2F* h_DCH2;
    TCanvas *canWires;
    TCanvas *canTimes;
    vector<PadInfo*> canWiresPads;
    vector<PadInfo*> canTimesPads;



    ClassDef(BmnHistDch, 1)
};

#endif /* BMNHISTDCH_H */

