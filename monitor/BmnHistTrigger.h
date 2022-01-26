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
#include <algorithm> 
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
#include "BmnTrigRaw2Digit.h"
#include "BmnEventHeader.h"
#define TRIG_TIME_WIN 6000
#define AMP_WIN       3000
#define TRIG_MULTIPL    40
#define BD_CHANNELS     40
#define SI_CHANNELS     64
#define TRIG_ROWS        3
#define TRIG_COLS        3

class BmnHistTrigger : public BmnHist {
public:
    BmnHistTrigger(TString title = "Triggers", TString path = "", Int_t periodID = 7, BmnSetup = kBMNSETUP);
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
    vector<TString> shortNames;
    TClonesArray *BDEvents;
//    TH1D *histBC1TimeLen;
//    TH1D *histBC2TimeLen;
//    TH1D *histSDTimeLen;
//    TH1D *histVDTimeLen;
//    TH1D *histFDTimeLen;
//    TH1I *histBDTime;
    TH1I *histTriggers;
    TH2I *histTrigTimeByChannel;
    TH2I *histBDTimeByChannel;
    TH1I *histBDChannels;
    TH1I *histBDSimult;
    TH1I *histBDCircular;
    TH2I *histSiTimeByChannel;
    TH1I *histSiChannels;
    TH1I *histSiSimult;
    TH1I *histSiCircular;
    TH1I *histSiBSum;
    TH2I *histCorrBCBC;
    TH2I *histCorrBCVC;
    TH2I *histCorrSiBD;
    TCanvas *can2d;
    TCanvas *canProfile;
    TCanvas *canTimes;
    vector<TH1I*> hists;
    vector<PadInfo*> canProfilePads;
    vector<PadInfo*> canTimesPads;
    vector<PadInfo*> can2dPads;
    Int_t fSelectedBDChannel;

    ClassDef(BmnHistTrigger, 1)
};

#endif /* BMNHISTTRIGGER_H */

