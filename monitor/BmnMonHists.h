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
#include "TH1F.h"
#include "TH2F.h"
#include "TCanvas.h"

using namespace std;

class BmnMonHists : public TNamed {
public:
    
    BmnMonHists();
    BmnMonHists(const BmnMonHists& orig);
    virtual ~BmnMonHists();
    void pushGemVec(Int_t i, TH1F* h);
    void Clear();
    vector<vector<TH1F*> > histStrip;
    TH1D *histToF400LeadingTime;
    TH1D *histToF400Amp;
    TH1I *histToF400Strip;
    TH1I *histToF400StripSimult;
    TH2F *histToF400State;
    
private:
ClassDef(BmnMonHists, 1)
};

#endif /* BMNMONHISTS_H */

