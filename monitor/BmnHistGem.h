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

#include "BmnGemStripDigit.h"

#define GEM_LAYERS_COUNT    4
#define GEM_STATIONS_COUNT  7

using namespace std;

class BmnHistGem : public TNamed {
public:

    BmnHistGem(TString title);
    BmnHistGem(const BmnHistGem& orig);
    virtual ~BmnHistGem();
    void Reset();
    void Register(THttpServer *serv, TDirectory *dir, TTree *recoTree);
    void FillFromDigi(TClonesArray * digits);
    vector<vector<vector<TH1F*> > > histGemStrip;

private:
    THttpServer *fServer;
    TTree *frecoTree;

    ClassDef(BmnHistGem, 1)
};

#endif /* BMNMONHISTS_H */

