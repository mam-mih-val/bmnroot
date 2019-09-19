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
    BmnStatus LoadPedestalsMK(TTree* tin, TClonesArray *adc, BmnEventHeader* evhead, Int_t npedev);
    void InitAdcProcessorMK(Int_t run, Int_t iread = 0, Int_t iped = 0, Int_t ithr = 0, Int_t test = 0);

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
    
    Int_t nx1bin;
    Int_t ny1bin;

    Int_t nallmid;

    Int_t maxChan;
    Int_t maxchip2;
    Int_t maxAdc;

    Int_t ndet;
    Int_t nadc;
    Int_t nadc_samples;

    Int_t nadcmax;

    Int_t ncoor;
    Int_t nclmax;

    Int_t nx1max;
    Int_t ny1max;
    Int_t nevmax;

    vector<TH1I *> hNhits;

    vector<Int_t> modul;
    vector<Int_t> detorder;
    vector<UInt_t> fSerials;
    vector<Int_t> nx1det;
    vector<Int_t> ny1det;
    vector<Int_t> nchdet;

    vector<vector<Int_t> > detadc;
    vector<vector<Int_t> > ichadc;

    vector<vector<Double_t> > Pedadc;
    vector<vector<Double_t> > Pedadc2;
    vector<vector<Int_t> > nchadc;
    vector<vector<Int_t> > noisech;
    vector<vector<Double_t> > Pedchr;
    vector<vector<Double_t> > Pedchr2;
    vector<Int_t> x1map;
    vector<Int_t> y1map;
    vector<vector<Double_t> > Ampx1;
    vector<vector<Double_t> > Ampy1;
    vector<vector<Int_t> > cmodhitx1;
    vector<vector<Int_t> > cmodhity1;
    vector<Int_t> Nclustx;
    vector<Int_t> Nclusty;

    vector<vector<Int_t> > nchan;
    vector<vector<Int_t> > nchan1;

    vector<vector<Double_t> > Ampch;
    vector<vector<Double_t> > Pedch;
    vector<vector<Double_t> > Pedch2;

    vector<vector<Double_t> > Ped1ch;
    vector<vector<Double_t> > Ped1ch2;

    vector<vector<Double_t> > Ped1cmod;
    vector<vector<Double_t> > Ped1cmod2;

    vector<vector<Double_t> > Pedcmod;
    vector<vector<Double_t> > Pedcmod2;

    vector<vector<Double_t> > Cmode;
    vector<vector<Double_t> > C1mode;
    vector<vector<Double_t> > Cmall;
    vector<vector<Double_t> > Smode;
    vector<vector<Double_t> > Cmode2;

    vector<vector<Double_t> > Cmode1;
    vector<vector<Double_t> > Smode1;

    vector<Double_t> Clustx;
    vector<Double_t> Clusty;

    vector<vector<Double_t> > Clustxx;
    vector<vector<Double_t> > Clustyy;

    vector<vector<Double_t> > Asample;

    vector<vector<Double_t> > sigx1;
    vector<vector<Int_t> > nsigx1;
    vector<vector<Int_t> > cmodfl;
    vector<Int_t> nchsig;

    vector<vector<vector<Double_t> > > rawx1;
    vector<vector<vector<Double_t> > > subx1;
    vector<vector<vector<Double_t> > > pedx1;
    vector<vector<vector<Double_t> > > cmdx1;
    vector<Int_t> chmap;
    Int_t nev = -1;
//    Int_t nradc = 0;
    Int_t niter;
    Int_t niterped;
    Int_t nchip;
    Int_t nchmin;
    Int_t npevents;
    Double_t cmodcut;

    Double_t thresh;

    Double_t thrnoise;

    Double_t dthr;

    Double_t thrped;

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

