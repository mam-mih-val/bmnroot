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
    BmnHistTrigger(TString title = "Triggers", TString path = "");
    virtual ~BmnHistTrigger();

    void Register(THttpServer *serv);
    void SetDir(TFile *outFile, TTree *recoTree);
    void FillFromDigi(DigiArrays *fDigiArrays);
    void Reset();
    void DrawBoth();
    void ClearRefRun();
    BmnStatus  SetRefRun(Int_t id);

    void SetBDChannel(Int_t iSelChannel);

    Int_t GetBDChannel() {
        return fSelectedBDChannel;
    }

private:
    void InitHistsFromArr(vector<TClonesArray*> *trigAr);
    void SetDir(TDirectory *Dir);
    Int_t fRows = 0;
    Int_t fCols = 0;
    vector<TString> histNames;
    vector<TString> trigNames;
    TClonesArray *BDEvents;
//    TH1D *histBC1TimeLen;
//    TH1D *histBC2TimeLen;
//    TH1D *histSDTimeLen;
//    TH1D *histVDTimeLen;
//    TH1D *histFDTimeLen;
//    TH1I *histBDTime;
    TH2I *histBDTimeByChannel;
    TH2I *histTrigTimeByChannel;
    TH1I *histBDChannels;
    TH1I *histBDSimult;
    TH1I *histBDSpecific;
    TH1I *histTriggers;
    TCanvas *canTimes;
    TCanvas *can2d;
    vector<TH1I*> hists;
    vector<PadInfo*> canTimesPads;
    vector<PadInfo*> can2dPads;
    Int_t fSelectedBDChannel;

    ClassDef(BmnHistTrigger, 1)
};

#endif /* BMNHISTTRIGGER_H */

