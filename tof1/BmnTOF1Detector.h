/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   BmnTOF1Detector.h
 * Author: mikhail
 *
 * Created on March 13, 2017, 4:52 PM
 */

#ifndef BMNTOF1DETECTOR_H
#define BMNTOF1DETECTOR_H 1

#include "TString.h"
#include "TSystem.h"
#include "BmnEnums.h"
#include "BmnTof1Digit.h"
#include "BmnEventHeader.h"
#include "BmnTrigDigit.h"
#include "BmnRunHeader.h"
#include "TH1I.h"
#include "TH2I.h"
#include "TFile.h"
#include "TTree.h"
#include "TList.h"
#include "TClonesArray.h"
#include "TGraphErrors.h"
#include "TGraphErrors.h"
#include "TDirectory.h"
#include <iostream>
#include <vector>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <cstdlib>
#include <cstdio>
#include <list>
#include <map>
#include <deque>

using namespace std;

class BmnTOF1Detector {
private:

    static const Int_t fNStr = 48;
    TString fName;
    Int_t fNPlane;
    Int_t fMaxL, fMaxR, fMax;
    Double_t fTimeL[fNStr], fTimeR[fNStr], fTime[fNStr], fWidthL[fNStr], fWidthR[fNStr], fWidth[fNStr], fTof[fNStr];
    Double_t fDoubleTemp;
    Int_t fHit_Per_Ev, fNEvents, fStrip;
    Bool_t fFlagHit[fNStr], fKilled[fNStr], fFillHist;
    Double_t CorrLR[fNStr];

    Double_t fDigitL[fNStr], fDigitR[fNStr], fHit[fNStr];

    BmnTrigDigit *fT0;

    TList *fHistListStat, *fHistListCh, *fHistListDt;

    TH1I *hHitByCh, *hHitPerEv;
    TH2I *hLeftDigitToHit, *hRightDigitToHit;
    TH2I *hHitLR, *hDtvsWidthDet[fNStr], *hDtvsWidthT0[fNStr];
    TH1I *hWidth[fNStr + 1], *hDtLR[fNStr + 1], *hTime[fNStr + 1], *hDt[fNStr + 1];
    TGraphErrors *gSlew[fNStr];

    void FillHist();
    Double_t CalculateDt(Int_t Str);

public:
    BmnTOF1Detector();

    BmnTOF1Detector(Int_t NPlane, Bool_t FillHist);

    virtual ~BmnTOF1Detector() {
    };

    void Clear();
    Bool_t SetDigit(BmnTof1Digit *TofDigit);
    void KillStrip(Int_t NumberOfStrip);
    Int_t FindHits(BmnTrigDigit *T0);
    TList* GetList(Int_t n);
    TString GetName();
    void SetCorrLR(Double_t *Mass);
    void SetCorrLR(TString NameFile);
    void SetCorrSlewing(TString NameFile);

    ClassDef(BmnTOF1Detector, 2);

};

#endif