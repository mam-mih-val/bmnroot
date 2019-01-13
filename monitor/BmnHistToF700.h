/* 
 * File:   BmnHistToF700.h
 * Author: ilnur
 *
 * Created on November 29, 2016, 1:24 PM
 */

#ifndef BMNHISTTOF700_H
#define BMNHISTTOF700_H 1
//STL
#include <exception>
#include <stdlib.h>
#include <vector>
// FairSoft
#include <TNamed.h>
#include "TChain.h"
#include "TClonesArray.h"
#include "TH1F.h"
#include "TH1D.h"
#include "TH2F.h"
#include "TCanvas.h"
#include "TFile.h"
#include "TFolder.h"
#include "THttpServer.h"
#include "BmnEventHeader.h"
// BmnRoot
#include "BmnHist.h"
#include "BmnTof2Digit.h"
#include "BmnTof2Raw2DigitNew.h"
#define TOF_COLS    3
#define TOF_ROWS    2

class BmnHistToF700 : public BmnHist {
public:
    BmnHistToF700(TString title = "ToF700", TString path = "");
    virtual ~BmnHistToF700();
    void Register(THttpServer *serv);
    void SetDir(TFile *outFile = NULL, TTree *recoTree = NULL);
    void Reset();
    void DrawBoth();
    void ClearRefRun();
    BmnStatus  SetRefRun(Int_t id);    
    
    void FillFromDigi(DigiArrays *fDigiArrays);
    void SetSelection(Int_t Plane, Int_t Strip);
    
    void  SetPlane(Int_t v){ fSelectedPlane = v;}
    void  SetSide (Int_t v){ fSelectedSide  = v;}
    void  SetStrip(Int_t v){ fSelectedStrip = v;}
    Int_t GetPlane() { return fSelectedPlane;}
    Int_t GetSide () { return fSelectedSide ;}
    Int_t GetStrip() { return fSelectedStrip;}    
private:
    TClonesArray *Events;
    TH1D *histLeadingTime;
    TH1D *histLeadingTimeSpecific;
    TH1D *histAmp;
    TH1D *histAmpSpecific;
    TH1I *histStrip;
//    TH1I *histStripSimult;
//    TH2F *histState;
    Int_t fSelectedPlane;
    Int_t fSelectedStrip;
    Int_t fSelectedSide;
    TH1I *histL = new TH1I("", "", TOF2_MAX_STRIPS_IN_CHAMBER, 0, TOF2_MAX_STRIPS_IN_CHAMBER);
    TH1I *histR = new TH1I("", "", TOF2_MAX_STRIPS_IN_CHAMBER, 0, TOF2_MAX_STRIPS_IN_CHAMBER);
    TH1I histSimultaneous;
    vector<TString> Names;
    TCanvas *canTimes;
    vector<PadInfo*> canTimesPads;
    
void SaveHist(TH1 *hist, TString path) {
    TCanvas *c0 = new TCanvas("c0", hist->GetTitle());
    hist->Draw();
    c0->SaveAs(TString(path + TString(hist->GetTitle()) + ".png").Data());
    delete c0;
}
    
    ClassDef(BmnHistToF700, 1)
};
#endif /* BMNHISTTOF700_H */

