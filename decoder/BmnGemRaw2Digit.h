#ifndef BMNGEMRAW2DIGIT_H
#define BMNGEMRAW2DIGIT_H

#include "TString.h"
#include "TTree.h"
#include "TClonesArray.h"
#include "TH1F.h"
#include <iostream>
#include "BmnGemStripDigit.h"
#include "BmnADCDigit.h"
#include "BmnEnums.h"
#include "TMath.h"
#include <fstream>
#include <list>
#include <map>
#include "BmnAdcProcessor.h"
#include <vector>
#include <UniDbDetectorParameter.h>
#include <UniDbDetector.h>

#include <TSystem.h>
#include <BmnEventHeader.h>
#include "BmnCSCDigit.h"

#define N_CH_IN_CRATE 2048 //number of channels in one crate (64ch x 32smpl))
#define N_CH_IN_SMALL_GEM 512 //number of channels in small GEM stations (sum of all redout channels)
#define N_CH_IN_MID_GEM 2176 //number of channels in middle GEM stations (sum of all redout channels)
#define N_CH_IN_BIG_GEM 3200 //number of channels in one part of big GEM stations (sum of all redout channels)
#define N_CH_IN_BIG_GEM_0 1023//988 //number of channels in hot zone of one part of big GEM stations (sum of redout channels from X0 and Y0)
#define N_CH_IN_BIG_GEM_1 2176//2100 //number of channels in big zone of one part of big GEM stations (sum of redout channels from X1 and Y1)
#define N_CH_BUF 4096
#define N_MODULES 2
#define N_LAYERS 4

using namespace std;
using namespace TMath;

struct BmnGemMap {
    Int_t strip;
    Int_t lay; // strip type: 0 - x, 1 - y
    Int_t mod; //hot zones: 1 - inner zone, 0 - outer zone

    BmnGemMap(Int_t s, Int_t l, Int_t m) : strip(s), lay(l), mod(m) {
    }

    BmnGemMap() : strip(0), lay(0), mod(0) {
    }
};

class BmnGemRaw2Digit : public BmnAdcProcessor {
public:
    BmnGemRaw2Digit(Int_t period, Int_t run, vector<UInt_t> vSer, TString mapFileName, BmnSetup bmnSetup = kBMNSETUP, BmnADCDecoMode decoMode = kBMNADCMK);
    BmnGemRaw2Digit();
    ~BmnGemRaw2Digit();

    BmnStatus FillEvent(TClonesArray *adc, TClonesArray *gem);
    BmnStatus FillEventMK(TClonesArray *adc, TClonesArray *gem, TClonesArray *csc);
    BmnStatus FillProfiles(TClonesArray *adc);
    BmnStatus FillNoisyChannels();
    BmnStatus RecalculatePedestalsMK(Int_t nPedEv);
    BmnStatus LoadPedestalsMK(TTree* tin, TClonesArray *adc, BmnEventHeader* evhead, Int_t npedev);
    void InitAdcProcessorMK(Int_t run, Int_t iread = 0, Int_t iped = 0, Int_t ithr = 0, Int_t test = 0);

private:
    static const Int_t nx0bin = 190;
    static const Int_t ny0bin = 215;
    static const Int_t nx1bin = 825;
    static const Int_t ny1bin = 930;

    static const Int_t nallcsc = 2048;
    static const Int_t nallmid = 2176;
    static const Int_t nallbig = 3200;

    static const Int_t maxcard = 25;
    static const Int_t maxChan = 3200;
    static const Int_t maxchip = 100;
    static const Int_t maxchip2 = 200;
    static const Int_t maxAdc = 2048;

    Int_t nmiddle;
    Int_t nbig;
    Int_t ndet;
    Int_t nadc;
    static const Int_t nadcmax = 30;
    static const Int_t nadc_samples = 32;

    Int_t ncoor;
    static const Int_t nclmax = 4;

    static const Int_t nx0big = 500;
    static const Int_t nx1big = 1019;

    static const Int_t ny0bigL = 488;
    static const Int_t ny1bigL = 1081;

    static const Int_t ny0bigR = 506;
    static const Int_t ny1bigR = 1130;

    static const Int_t nx0max = 500;
    static const Int_t nx1max = 1019;

    static const Int_t ny0max = 506;
    static const Int_t ny1max = 1130;

    static const Int_t ncscver = 452;
    static const Int_t ncscin = 466;
    static const Int_t ncscout = 498;

    static const Int_t nvercsc = 1024;
    static const Int_t nadcgem = 24;
    Int_t ndetgem = 14;
    static const Int_t ncsc = 2;
    Int_t nev = -1;
    
    Int_t nbigL = 5;
    Int_t nbigR = 6;
    // only for period 6
    static const Int_t nsmall = 256;
    static const Int_t nallsma = 512;
    Int_t nallbig2 = 4224;
    Int_t nbigL2 = 7;
    Int_t nbigR2 = 8;
    Int_t nsma = 9;

    vector<Int_t> nmidadd;
    vector<Int_t> nbigLdet;
    vector<Int_t> nbigRdet;

    vector<Int_t> nbigLxy;
    vector<Int_t> nbigLxy0;

    vector<Int_t> nbigRxy;
    vector<Int_t> nbigRxy0;

    vector<Int_t> nbigshift;
    // swaped R<->L Gem 6 and Gem7
    // correspondence adc - > detector (big Gems), additional channels
    vector<Int_t> nbigLadd;
    vector<Int_t> nbigRadd;

    vector<UInt_t> fSerials;
    vector<UInt_t> rSerials;
    
    vector<Int_t> nx0det;
    vector<Int_t> ny0det;
    vector<Int_t> nx1det;
    vector<Int_t> ny1det;
    vector<Int_t> nchdet;

    //Run 7 BM@N configuration

    // detector order and modul number in the final array
    vector<Int_t> detorder;
    vector<Int_t> modul;

    // det mapping type GEM / GEM2 / CSC
    vector<Int_t> dettype;


    vector< vector<Int_t> > detadc;
    vector< vector<Int_t> > ichadc;

    /*
        Double_t Pedadc[nadc][maxAdc];
        Double_t Pedadc2[nadc][maxAdc];

        Int_t nchadc[nadc][maxAdc];
     */

    vector< vector<Float_t> > Pedchr;
    vector< vector<Float_t> > Pedchr2;
    
    Int_t x0big[nx0big] = {};
    Int_t x1big[nx1big] = {};

    Int_t y0bigL[ny0bigL] = {};
    Int_t y1bigL[ny1bigL] = {};

    Int_t y0bigR[ny0bigR] = {};
    Int_t y1bigR[ny1bigR] = {};

    Int_t x0big2[nx0big] = {};
    Int_t x1big2[nx1big] = {};

    Int_t y0bigL2[ny0bigL] = {};
    Int_t y1bigL2[ny1bigL] = {};
    Int_t y0bigR2[ny0bigR] = {};
    Int_t y1bigR2[ny1bigR] = {};

    Int_t x0map[nx0bin] = {};
    Int_t y0map[ny0bin] = {};
    Int_t x1map[nx1bin] = {};
    Int_t y1map[ny1bin] = {};

    vector< vector<Float_t> > Ampx0;
    vector< vector<Float_t> > Ampy0;
    vector< vector<Float_t> > Ampx1;
    vector< vector<Float_t> > Ampy1;
    
    Int_t x1csc[ncscver] = {};
    Int_t x2csc[ncscver] = {};
    Int_t x3csc[ncscver] = {};
    Int_t x4csc[ncscver] = {};

    Int_t y1csc[ncscout] = {};
    Int_t y2csc[ncscin] = {};
    Int_t y3csc[ncscin] = {};
    Int_t y4csc[ncscout] = {};
    
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
    Bool_t read = kFALSE;
    Bool_t pedestals = kFALSE;
    Int_t chmap[nallmid] = {-1};
    Int_t chbigL[nallbig] = {-1};
    Int_t chbigR[nallbig] = {-1};
    Int_t chbigL2[nallbig] = {-1};
    Int_t chbigR2[nallbig] = {-1};
    Int_t chuppercsc[nallcsc] = {-1};
    Int_t chlowercsc[nallcsc] = {-1};
    Int_t chsma[nallsma] = {-1};
    Int_t xsmall[nsmall];
    Int_t ysmall[nsmall];
    
    Int_t nchip = 32;
    Int_t nchmin = 8;
    Float_t thresh = 35;
    Float_t thrcsc = 80;
    Float_t thrnoise = 0.03;
    Float_t dthr = 10;
    Float_t dthrcsc = 15;

    // starting thresholds, number of iterations
    Int_t niter = 3;
    Int_t niterped = 3;
    Float_t thrped = 35;
    Float_t thrpedcsc = 80;
    vector<TH1I *> hNhits;
    
    vector<Int_t> Nclustx;
    vector<Int_t> Nclusty;
// (\w+) (\w+)\[(\w+)\]\[(\w+)\].+;
    // $2.resize($3, vector<$1>($4, 0));
    // vector< vector<$1> > $2;
    vector< vector<Int_t> > nchan;
    vector< vector<Int_t> > nchan1;

    vector< vector<Float_t> > Ampch;
    vector< vector<Float_t> > Pedch;
    vector< vector<Double_t> > Pedch2;

    vector< vector<Float_t> > Ped1ch;
    vector< vector<Double_t> > Ped1ch2;

    vector< vector<Float_t> > Ped1cmod;
    vector< vector<Double_t> > Ped1cmod2;

    vector< vector<Float_t> > Pedcmod;
    vector< vector<Double_t> > Pedcmod2;

    vector< vector<Float_t> > Cmode;
    vector< vector<Float_t> > C1mode;
    vector< vector<Float_t> > Cmall;
    vector< vector<Float_t> > Smode;
    vector< vector<Float_t> > Cmode2;

    vector< vector<Float_t> > Cmode1;
    vector< vector<Float_t> > Smode1;

    vector<Float_t> Clustx;
    vector<Float_t> Clusty;

    vector< vector<Float_t> > Clustxx;
    vector< vector<Float_t> > Clustyy;

    vector< vector<Float_t> > Asample;

    vector< vector<Float_t> > sigx1;
    vector< vector<Int_t> > nsigx1;
    vector<Int_t> nchsig;
    vector< vector<Int_t> > noisech;

    const Int_t nevmax = 1;
    Int_t nradc = 0;
    Int_t npevents = 0;
    Int_t test = 0;

    BmnGemMap* fSmall;
    BmnGemMap* fMid;
    BmnGemMap* fBigL0;
    BmnGemMap* fBigL1;
    BmnGemMap* fBigR0;
    BmnGemMap* fBigR1;
        
    TH1F**** fSigProf;
    Bool_t**** fNoisyChannels;
    
    TString fMapFileName;

    vector<GemMapStructure> fMap;

    void ProcessDigit(BmnADCDigit* adcDig, GemMapStructure* gemM, TClonesArray *gem, Bool_t doFill);
    void ProcessDigitMK(BmnADCDigit* adcDig, TClonesArray *gem, Bool_t doFill);
    void PostprocessDigitMK(TClonesArray *gem, TClonesArray *csc);
    BmnStatus ReadMap(TString parName, BmnGemMap* m, Int_t lay, Int_t mod);

    Int_t fEntriesInGlobMap; // number of entries in BD table for Global Mapping

    Int_t fEventId;

    ClassDef(BmnGemRaw2Digit, 1);
};

#endif /* BMNGEMRAW2DIGIT_H */

