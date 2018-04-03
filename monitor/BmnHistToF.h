/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   BmnHistToF.h
 * Author: ilnur
 *
 * Created on November 27, 2016, 4:24 PM
 */

#ifndef BMNHISTTOF_H
#define BMNHISTTOF_H 1
// STL
#include <exception>
#include <stdlib.h>
#include <vector>
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
#include "BmnTof1Digit.h"
#define TOF400_PLANE_COUNT 20
#define TOF400_STRIP_COUNT 48
#define TOF_COLS    3
#define TOF_ROWS    2

class BmnHistToF : public BmnHist {
public:
    BmnHistToF(TString title, TString path = "");
    virtual ~BmnHistToF();
    
    void Register(THttpServer *serv);
    void SetDir(TFile *outFile = NULL, TTree *recoTree = NULL);
    void Reset();
    void DrawBoth();
    void ClearRefRun();
    BmnStatus  SetRefRun(Int_t id);
        
    void FillFromDigi(DigiArrays *fDigiArrays);
    void SetSelection(Int_t Plane, Int_t Strip, Int_t Side);
    
    void  SetPlane(Int_t v){ fSelectedPlane = v;}
    void  SetSide (Int_t v){ fSelectedSide  = v;}
    void  SetStrip(Int_t v){ fSelectedStrip = v;}
    Int_t GetPlane() { return fSelectedPlane;}
    Int_t GetSide () { return fSelectedSide ;}
    Int_t GetStrip() { return fSelectedStrip;}
private:
//    TBranch * fEventsBranch;
    TClonesArray *Events;
    TH1D *histLeadingTime;
    TH1D *histLeadingTimeSpecific;
    TH1D *histAmp;
    TH1D *histAmpSpecific;
    TH1I *histStrip;
    TH1I *histStripSimult;
    TH2I *histStrip2d;
    TH2F *histState;
    Int_t fSelectedPlane;
    Int_t fSelectedStrip;
    Int_t fSelectedSide;
    TH1I *histL = new TH1I("", "", TOF400_STRIP_COUNT, 0, TOF400_STRIP_COUNT);
    TH1I *histR = new TH1I("", "", TOF400_STRIP_COUNT, 0, TOF400_STRIP_COUNT);
    TH1I histSimultaneous;
    vector<TString> Names;
    TCanvas *canTimes;
    TCanvas *can2d;
    vector<PadInfo*> canTimesPads;
    vector<PadInfo*> can2dPads;
    
    ClassDef(BmnHistToF, 1)
};

#endif /* BMNHISTTOF_H */

