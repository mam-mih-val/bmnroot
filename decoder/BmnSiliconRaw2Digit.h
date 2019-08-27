#ifndef BMNSILICONRAW2DIGIT_H
#define BMNSILICONRAW2DIGIT_H

#include "TString.h"
#include "TTree.h"
#include "TClonesArray.h"
#include <iostream>
#include "TH1F.h"
#include "BmnADCDigit.h"
#include "BmnEnums.h"
#include "TMath.h"
#include <fstream>
#include <list>
#include <map>
#include <vector>
#include <UniDbDetectorParameter.h>
#include "BmnSiliconDigit.h"
#include "BmnAdcProcessor.h"
#include <TSystem.h>
#include <BmnEventHeader.h>

using namespace std;
using namespace TMath;

struct BmnSiliconMapping {
    Short_t layer; //X = 0 or X' = 1
    UInt_t serial;
    Short_t module;
    Short_t channel_low;
    Short_t channel_high;
    Short_t station;
};

class BmnSiliconRaw2Digit : public BmnAdcProcessor {
public:
    BmnSiliconRaw2Digit(Int_t period, Int_t run, vector<UInt_t> vSer, BmnSetup bmnSetup = kBMNSETUP);
    BmnSiliconRaw2Digit();
    virtual ~BmnSiliconRaw2Digit();

    BmnStatus FillEvent(TClonesArray *adc, TClonesArray *sts);
    BmnStatus FillEventMK(TClonesArray *adc, TClonesArray *sts);
    BmnStatus FillProfiles(TClonesArray *adc);
    BmnStatus FillNoisyChannels();
    BmnStatus RecalculatePedestalsMK(Int_t nPedEv);
    BmnStatus LoadPedestalsMK(TTree* tin, TClonesArray *adc, BmnEventHeader* evhead, Int_t npedev);
    void InitAdcProcessorMK(Int_t run, Int_t iread = 0, Int_t iped = 0, Int_t ithr = 0, Int_t test = 0);

//    vector<UInt_t> GetSerials() {
//        return fSerials;
//    }

private:

    vector<BmnSiliconMapping> fMap;
    Int_t fEventId;
    TString fMapFileName;    
        
    TH1F**** fSigProf;
    Bool_t**** fNoisyChannels;
    TCanvas *canStrip = nullptr;
    TH2F* hraw = nullptr;
    TH2F* hrms = nullptr;
    TH2F* hcorrp = nullptr;
    TH2F* hcorr = nullptr;
    TH2F* hfilter = nullptr;
    TH2F* hped = nullptr;
    TH2F* hcms = nullptr;
//    TH2F* hscms;
    TH2F* hscms = nullptr;
    TH1F* hscms1 = nullptr;
    TH1F* hscms1full = nullptr;
    TH1F* hped1 = nullptr;
    TH1F* hsig = nullptr;
    
    static const Int_t nx1bin = 640;
    static const Int_t ny1bin = 640;

    static const Int_t nallmid = 1280;

    static const Int_t maxChan = 1280;
    static const Int_t maxchip2 = 10;
    static const Int_t maxAdc = 8192;

    static const Int_t ndet = 14;
    static const Int_t nadc = 4;
    static const Int_t nadc_samples = 128;

    static const Int_t nadcmax = 5;

    static const Int_t ncoor = 28;
    static const Int_t nclmax = 2;

    static const Int_t nx1max = 640;
    static const Int_t ny1max = 640;
    static const Int_t nevmax = 2;

//    TH1I * hChan[ndet];
//    TH1F * hAmp[ndet];
    TH1I * hNhits[ndet];
//    TH1I * hNhitsall[ndet];
//    TH1F * hPeds[ndet];
//    TH1F * hPrms[ndet];
//    TH1F * hPmCmod[ndet];
//    TH1F * hPmCrms[ndet];
//    TH1F * hCmode[ndet];
//    TH1F * hCrms[ndet];
//
//    TH1F * hSCmode[ndet];
//    TH1F * hSCmodex1[ndet];
//    TH1F * hSCmodey1[ndet];
//
//    TH1F * hAmpx1[ndet];
//    TH1I * hNAmpx1[ndet];
//
//    TH1F * hAmpy1[ndet];
//    TH1I * hNAmpy1[ndet];
//
//    TH1F * Clust[ndet];
//
//    TH1F * ClustX1[ndet];
//    TH1F * ClustY1[ndet];
//
//    TH1I * NClust[ndet];
//
//    TH1I * NClustX1[ndet];
//    TH1I * NClustY1[ndet];
//
//    TH1I * Width[ndet];
//
//    TH1I * WidthX1[ndet];
//    TH1I * WidthY1[ndet];
//
//    TH1F * Samp[ndet];
//
//    TH1F * SampX1[ndet];
//    TH1F * SampY1[ndet];
//
//
//    TH1F * hrawx1[ndet];
//    TH1F * hsigx1[ndet];
//    TH1F * hpedx1[ndet];
//    TH1F * hcmdx1[ndet];
//
//    TH1F * hnoise[ndet];

    UInt_t fSerials[nadc];
//    UInt_t rSerials[nadcmax];
    Int_t nx1det[ndet];
    Int_t ny1det[ndet];
    Int_t nchdet[ndet];

    Int_t detadc[nadc][maxAdc];
    Int_t ichadc[nadc][maxAdc];

    Double_t Pedadc[nadc][maxAdc];
    Double_t Pedadc2[nadc][maxAdc];
    Int_t nchadc[nadc][maxAdc];

    // Run 6
    //   Int_t modul[ndet]    = {0,3,7,4,2,5,6,1};
    // Run7
    Int_t modul[ndet] = {0, 3, 7, 4, 2, 5, 6, 1, 0, 1, 2, 3, 1, 0};
    Int_t detorder[ndet] = {3, 3, 3, 3, 3, 3, 3, 3, 1, 1, 1, 1, 2, 2};


    Int_t noisech[ndet][maxChan];
    Double_t Pedchr[ndet][maxChan];
    Double_t Pedchr2[ndet][maxChan];
    Int_t x1map[nx1bin];
    Int_t y1map[ny1bin];
    Double_t Ampx1[ndet][nx1max];
    Double_t Ampy1[ndet][ny1max];
    Int_t Nclustx[ndet];
    Int_t Nclusty[ndet];

    Int_t nchan[ndet][maxchip2];
    Int_t nchan1[ndet][maxChan];

    Double_t Ampch[ndet][maxChan];
    Double_t Pedch[ndet][maxChan];
    Double_t Pedch2[ndet][maxChan];

    Double_t Ped1ch[ndet][maxChan];
    Double_t Ped1ch2[ndet][maxChan];

    Double_t Ped1cmod[ndet][maxChan];
    Double_t Ped1cmod2[ndet][maxChan];

    Double_t Pedcmod[ndet][maxChan];
    Double_t Pedcmod2[ndet][maxChan];

    Double_t Cmode[ndet][maxchip2];
    Double_t C1mode[ndet][maxchip2];
    Double_t Cmall[ndet][maxchip2];
    Double_t Smode[ndet][maxchip2];
    Double_t Cmode2[ndet][maxchip2];

    Double_t Cmode1[ndet][maxchip2];
    Double_t Smode1[ndet][maxchip2];

    Double_t Clustx[ndet];
    Double_t Clusty[ndet];

    Double_t Clustxx[ndet][nclmax];
    Double_t Clustyy[ndet][nclmax];

    Double_t Asample[ndet][maxChan];

    Float_t sigx1[ncoor][maxChan];
    Int_t nsigx1[ncoor][maxChan];
    Int_t nchsig[ncoor];

    Double_t rawx1[nevmax][ndet][maxChan];
    Double_t subx1[nevmax][ndet][maxChan];
    Double_t pedx1[nevmax][ndet][maxChan];
    Double_t cmdx1[nevmax][ndet][maxchip2];
    Int_t nev = -1;
    Int_t nradc = 0;
    Int_t niter = 4;
    Int_t niterped = 3;
    Int_t nchip = 128;
    Int_t nchmin = 32;
    Int_t npevents = 0;
    Double_t cmodcut = 100;

    Double_t thresh = 420;

    Float_t thrnoise = 0.03;

    Double_t dthr = 80;

    Double_t thrped = 340;

    Int_t chmap[nallmid];
    Int_t test = 0;
    Bool_t read = kFALSE;
    Bool_t pedestals = kFALSE;
    char fname[100];
    char ss[10];
    char sped[20];
    FILE *Rnoisefile = nullptr;
    FILE *Wnoisefile = nullptr;
    FILE *Pedfile = nullptr;
    FILE *Wpedfile = nullptr;
    TString wnoisename;
    TString wpedname;
    TString rnoisename;
    TString pedname;
    

    BmnStatus ReadMapFile();
    void ProcessDigit(BmnADCDigit* adcDig, BmnSiliconMapping* silM, TClonesArray *silicon, Bool_t doFill);
    
    void ProcessDigitMK(BmnADCDigit* adcDig, TClonesArray *silicon, Bool_t doFill);
    void PostprocessDigitMK(TClonesArray *silicon);
    
    ClassDef(BmnSiliconRaw2Digit, 1);
};

#endif /* BMNSILICONRAW2DIGIT_H */

