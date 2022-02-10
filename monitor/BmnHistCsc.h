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
#ifndef BMNHISTCSC_H
#define BMNHISTCSC_H 1
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

#include "BmnCSCDigit.h"
#include "BmnCSCStationSet.h"
using namespace std;

class BmnHistCsc : public BmnHist {
public:

    BmnHistCsc(TString title, TString path = "", Int_t periodID = 7, BmnSetup setup = kBMNSETUP);
    virtual ~BmnHistCsc();
    void Reset();
    void Register(THttpServer *serv);
    void SetDir(TFile *outFile = NULL, TTree *recoTree = NULL);
    void DrawBoth();
    void FillFromDigi(DigiArrays *fDigiArrays);
    void ClearRefRun();
    BmnStatus  SetRefRun(Int_t id);

private:
    BmnCSCStationSet *cscStationSet;
    vector<TString> Names;
    vector<vector<vector<TH1F*  > > > histCscStrip;
    TCanvas *canCscStrip;
//    vector<PadInfo<TH1>*> canCscStripPads;
    vector<PadInfo*> canStripPads;
    UInt_t sumMods;
    UInt_t maxLayers;

    ClassDef(BmnHistCsc, 1)
};

#endif /* BMNMONHISTS_H */

