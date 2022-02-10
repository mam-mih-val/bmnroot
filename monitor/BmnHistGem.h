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
#ifndef BMNHISTGEM_H
#define BMNHISTGEM_H 1
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
#include "BmnGemStripStationSet.h"
#include "BmnGemStripConfiguration.h"
#include "BmnGemStripStationSet_RunSpring2017.h"
#include "BmnGemStripStationSet_RunSummer2016.h"
#include "BmnGemStripStationSet_RunWinter2016.h"

using namespace std;

struct histNmask{
    TH1F*  hist;
    Int_t* mask;
};

class BmnHistGem : public BmnHist {
public:

    BmnHistGem(TString title, TString path = "", Int_t periodID = 7, BmnSetup setup = kBMNSETUP);
    virtual ~BmnHistGem();
    void Reset();
    void Register(THttpServer *serv);
    void SetDir(TFile *outFile = NULL, TTree *recoTree = NULL);
    void DrawBoth();
    void FillFromDigi(DigiArrays *fDigiArrays);
    void ClearRefRun();
    BmnStatus  SetRefRun(Int_t id);
    
private:
    BmnGemStripStationSet *gemStationSet;
    vector<TString> Names;
    vector<vector<vector<TH1F*  > > > histGemStrip;
    TCanvas *canGemStrip;
//    vector<PadInfo<TH1>*> canGemStripPads;
    vector<PadInfo*> canStripPads;
    UInt_t sumMods;
    UInt_t maxLayers;

    ClassDef(BmnHistGem, 1)
};

#endif /* BMNMONHISTS_H */

