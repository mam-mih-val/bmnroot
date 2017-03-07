/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   BmnMonHists.h
 * Author: ilnur
 *
 * Created on November 15, 2016, 12:57 PM
 */
#ifndef BMNMONHISTS_H
#define BMNMONHISTS_H 1
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
#include "TString.h"
#include "THttpServer.h"

#include "BmnHist.h"
#include "BmnGemStripDigit.h"

#define GEM_STATIONS_COUNT  7

using namespace std;

struct histNmask{
    TH1F*  hist;
    Int_t* mask;
};

class BmnHistGem : public BmnHist {
public:

    BmnHistGem(TString title = "GEM", TString path = "", Bool_t createNoiseMask = false);
    virtual ~BmnHistGem();
    void Reset();
    void Register(THttpServer *serv);
    void SetDir(TFile *outFile = NULL, TTree *recoTree = NULL);
    void DrawBoth();
    void FillFromDigi(TClonesArray * digits);
    void FillFromDigiMasked(TClonesArray * digits, vector<vector<vector<TH1F*> > >* hist0, Double_t threshold);
    void ApplyNoiseMask(vector<vector<vector<TH1F*> > >* hist0, Double_t threshold);
    void UpdateNoiseMask(Double_t threshold);
    BmnStatus LoadRefRun(TString FileName);
    BmnStatus  SetRefRun(Int_t id);
    
    vector<vector<vector<Int_t*> > > *GetNoiseMask(){
        return &maskGemStrip;
    }
    

private:
    vector<vector<vector<TH1F*  > > > histGemStrip;
    vector<vector<vector<Int_t*> > > maskGemStrip;
    TCanvas *canGemStrip;
//    vector<PadInfo<TH1>*> canGemStripPads;
    vector<PadInfo*> canGemStripPads;
    UInt_t sumMods;
    UInt_t maxLayers;
//    vector<vector<vector<histNmask> > > maskGemStrip;

    ClassDef(BmnHistGem, 1)
};

#endif /* BMNMONHISTS_H */

