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
#include "TFolder.h"
#include "THttpServer.h"

#include "BmnTof1Digit.h"
#define TOF400_PLANE_COUNT  4
#define TOF400_STRIP_COUNT 48

class BmnHistToF : public TNamed {
public:
    BmnHistToF(TString title);
    BmnHistToF(const BmnHistToF& orig);
    virtual ~BmnHistToF();
    
    void Register(THttpServer *serv, TFile *outFile = NULL, TTree *recoTree = NULL);
    void Reset();
    
    TClonesArray *Events = NULL;
    TH1D *histLeadingTime = NULL;
    TH1D *histLeadingTimeSpecific = NULL;
    TH1D *histAmp = NULL;
    TH1D *histAmpSpecific = NULL;
    TH1I *histStrip = NULL;
    TH1I *histStripSimult = NULL;
    TH2F *histState = NULL;
    
    void FillFromDigi(TClonesArray * digits);
    void SetSelection(Int_t Plane, Int_t Strip, Int_t Side);
    void SaveHists();
    
    void  SetPlane(Int_t v){ fSelectedPlane = v;}
    void  SetSide (Int_t v){ fSelectedSide  = v;}
    void  SetStrip(Int_t v){ fSelectedStrip = v;}
    Int_t GetPlane() { return fSelectedPlane;}
    Int_t GetSide () { return fSelectedSide ;}
    Int_t GetStrip() { return fSelectedStrip;}
private:
    THttpServer *fServer;
    TBranch * fEventsBranch;
    Int_t fSelectedPlane;
    Int_t fSelectedStrip;
    Int_t fSelectedSide;
    TTree *frecoTree;
    TH1I *histL = new TH1I("", "", TOF400_STRIP_COUNT, 0, TOF400_STRIP_COUNT);
    TH1I *histR = new TH1I("", "", TOF400_STRIP_COUNT, 0, TOF400_STRIP_COUNT);
    TH1I histSimultaneous;
    TString pathToImg = "/home/ilnur/Documents/BmnMonJS/public_html/img/";
    
void SaveHist(TH1 *hist, TString path) {
    TCanvas *c0 = new TCanvas("c0", hist->GetTitle());
    hist->Draw();
    c0->SaveAs(TString(path + TString(hist->GetTitle()) + ".png").Data());
    delete c0;
}
    
    ClassDef(BmnHistToF, 1)
};

#endif /* BMNHISTTOF_H */

