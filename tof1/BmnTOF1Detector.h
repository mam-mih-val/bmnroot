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
#include "BmnTOF1Point.h"
#include "BmnTof1GeoUtils.h"
#include "BmnTofHit.h"
#include "BmnTOF1Conteiner.h"
#include "TF1.h"
#include "TH1I.h"
#include "TH2I.h"
#include "TFile.h"
#include "TTree.h"
#include "TList.h"
#include "TClonesArray.h"
#include "TGraphErrors.h"
#include "TVector3.h"
#include "TDirectory.h"
#include <TGeoManager.h>
#include <TKey.h>
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
    Int_t fVerbose;
    Double_t fStripLength, fSignalVelosity;
    TString fName;
    Int_t fNPlane;
    Int_t fMaxL, fMaxR, fMax;
    Int_t fFillHist;
    Double_t fTimeL[fNStr], fTimeR[fNStr], fTimeLtemp[fNStr], fTimeRtemp[fNStr], fTime[fNStr];
    Double_t fWidthL[fNStr], fWidthR[fNStr], fWidthLtemp[fNStr], fWidthRtemp[fNStr], fWidth[fNStr];
    Double_t fTof[fNStr];
    Double_t fDoubleTemp, fMaxDelta;
    Int_t fHit_Per_Ev, fNEvents, fStrip;
    Bool_t fFlagHit[fNStr], fKilled[fNStr];
    Int_t fKillSide;
    Double_t fCorrLR[fNStr], fCorrTimeShift[fNStr];
    Double_t fDigitL[fNStr], fDigitR[fNStr], fHit[fNStr];
    Double_t fCommonTimeShift;
    TVector3 fCentrStrip[fNStr], fStripAngle[fNStr], fCrossPoint[fNStr], fVectorTemp;
    BmnTrigDigit *fT0;

    TList *fHistListStat;
    TList *fHistListdt;

    TH2S *hdT_vs_WidthDet[fNStr + 1], *hdT_vs_WidthT0[fNStr + 1];
    TH1I * hdT[fNStr + 1];
    TH1I *hHitByCh, *hHitPerEv;
    TH2I *hHitLR, *hXY;
    TH1S *hDy_near, *hDtime_near, *hDWidth_near;
    TH1S *hDy_acros, *hDtime_acros, *hDWidth_acros;
    TH2S *hTempDtimeDy_near, *hTempDtimeDy_acros;

    TGraphErrors *gSlew[fNStr];
    TF1 *funT0[fNStr], *funRPC[fNStr];

    void FillHist();
    Double_t CalculateDt(Int_t Str);
    Bool_t GetCrossPoint(Int_t NStrip);
    void AddHit(Int_t Str, TClonesArray *TofHit);
    void AddConteiner(Int_t Str, TClonesArray *TofHit);


public:
    BmnTOF1Detector();

    BmnTOF1Detector(Int_t NPlane, Int_t FillHistLevel, Int_t Verbose); // FillHistLevel=0-don"t fill, FillHistLevel=1-fill statistic, FillHistLevel>1-fill all

    virtual ~BmnTOF1Detector() {
    };

    void Clear();
    Bool_t SetDigit(BmnTof1Digit *TofDigit);
    void KillStrip(Int_t NumberOfStrip);
    void KillSide(Int_t NumberOfSide);
    Int_t FindHits(BmnTrigDigit *T0);
    Int_t FindHits(BmnTrigDigit *T0, TClonesArray *TofHit);
    TList* GetList(Int_t n);
    TString GetName();
    Bool_t SetCorrLR(Double_t *Mass);
    Bool_t SetCorrLR(TString NameFile);
    Bool_t SetCorrSlewing(TString NameFile);
    Bool_t SetCorrTimeShift(TString NameFile); 
    Bool_t SetGeoFile(TString NameFile);
    Bool_t SetGeo(BmnTof1GeoUtils *pGeoUtils);
    Bool_t GetXYZTime(Int_t Str, TVector3 *XYZ, Double_t *ToF);
    Bool_t GetLRTime(Int_t Str, Double_t *LMinusRTime);
    Bool_t GetXYZ4Strip(Int_t Str, TVector3 *XYZ);
    Double_t GetWidth(Int_t Str);
    Double_t GetTime(Int_t Str);
    Bool_t SaveHistToFile(TString NameFile);

    Int_t GetFillHistLevel() {
        return fFillHist;
    };

    ClassDef(BmnTOF1Detector, 4);

};

#endif
