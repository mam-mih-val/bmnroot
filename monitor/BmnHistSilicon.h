/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   BmnHistSilicon.h
 * Author: ilnur
 *
 * Created on March 9, 2017, 10:39 AM
 */

#ifndef BMNHISTSILICON_H
#define BMNHISTSILICON_H
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

using namespace std;

class BmnHistSilicon : public BmnHist {
public:
    BmnHistSilicon();
    virtual ~BmnHistSilicon();
private:
    TCanvas *canStrip;
    vector<PadInfo*> canStripPads;

    ClassDef(BmnHistSilicon, 1)
};

#endif /* BMNHISTSILICON_H */

