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
#include "BmnTOF1Point.h"
#include "BmnTof1GeoUtils.h"
#include "BmnTofHit.h"
#include "BmnTOF1Conteiner.h"
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
    Double_t fCorrLR[fNStr], fCorrTimeShift[fNStr];
    Double_t fDigitL[fNStr], fDigitR[fNStr], fHit[fNStr];
    TVector3 fCentrStrip[fNStr], fCrossPoint[fNStr], fVectorTemp;
    BmnTrigDigit *fT0;

    TList *fHistListStat;
    
    TH1I *hHitByCh, *hHitPerEv;
    TH2I *hHitLR, *hXY;
    TH1S *hDy_near, *hDtime_near, *hDWidth_near;
    TH1S *hDy_acros, *hDtime_acros, *hDWidth_acros;
    TH2S *hTempDtimeDy_near, *hTempDtimeDy_acros;

    TGraphErrors *gSlew[fNStr];
    
    TTree *fTree4Save;
    TClonesArray *fArrayConteiner;
            
    void FillHist();
    Double_t CalculateDt(Int_t Str);
    Bool_t GetCrossPoint(Int_t NStrip);
    void AddHit(Int_t Str, TClonesArray *TofHit);


public:
    BmnTOF1Detector();

    BmnTOF1Detector(Int_t NPlane, Int_t FillHistLevel, TTree *tree); // FillHistLevel=0-don"t fill, FillHistLevel=1-fill statistic, FillHistLevel>1-fill all

    virtual ~BmnTOF1Detector() {
    };

    void Clear();
    Bool_t SetDigit(BmnTof1Digit *TofDigit);
    void KillStrip(Int_t NumberOfStrip);
    Int_t FindHits(BmnTrigDigit *T0);
    Int_t FindHits(BmnTrigDigit *T0, TClonesArray *TofHit);
    TList* GetList(Int_t n);
    TString GetName();
    Bool_t SetCorrLR(Double_t *Mass);
    Bool_t SetCorrLR(TString NameFile);
    Bool_t SetCorrSlewing(TString NameFile);
    Bool_t SetCorrTimeShift (TString NameFile); //FIXME
    Bool_t SetGeoFile(TString NameFile);
    Bool_t SetGeo(BmnTof1GeoUtils *pGeoUtils);
    Bool_t GetXYZTime(Int_t Str, TVector3 *XYZ, Double_t *ToF);
    Double_t GetWidth(Int_t Str);
    Bool_t SaveHistToFile(TString NameFile);
    Bool_t SetTree (TTree *tree);
    Int_t GetFillHistLevel () {return fFillHist;};
    
    ClassDef(BmnTOF1Detector, 3);

};

#endif
