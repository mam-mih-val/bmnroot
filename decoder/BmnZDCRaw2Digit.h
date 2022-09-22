#ifndef BMNZDCRAW2DIGIT_H
#define	BMNZDCRAW2DIGIT_H

#include "TString.h"
#include "TClonesArray.h"
#include "TMinuit.h"
#include "TH1F.h"
#include "TProfile.h"
#include "BmnTDCDigit.h"
#include "BmnADCDigit.h"
#include "BmnSyncDigit.h"
#include <iostream>
#include "Riostream.h"
#include "BmnZDCDigit.h"
#include <cstdlib>
#include <UniRun.h>

#define MAX_EVENTS 10000
#define MAX_CHANNELS 112
#define MAX_LOG_CHANNELS 16

static int nevents;
static float amp_array[MAX_EVENTS][MAX_CHANNELS];
static float pedestals[MAX_EVENTS][MAX_CHANNELS];
static float profile_amp[MAX_EVENTS][MAX_CHANNELS];
static float profile_err[MAX_EVENTS][MAX_CHANNELS];
static int use_log_function;

class Bmn_ZDC_map_element{
public:
  Bmn_ZDC_map_element(){ id=chan=adc_chan=size=x=y=-1; used=0;};
  void set(unsigned long v1,unsigned char v2,unsigned char v3,unsigned char v4,unsigned char v5,unsigned char v6,unsigned char v7, float v8, float v9){
    id=v1; chan=v2; adc_chan=v3; size=v4; ix=v5; iy=v6; used=v7; x=v8; y=v9;
  }; 
  unsigned long id;
  unsigned char chan;
  unsigned char adc_chan;
  unsigned char size;
  unsigned char ix;
  unsigned char iy;
  unsigned char used;
  float x;
  float y;
};

class BmnZDCRaw2Digit{

public:
    BmnZDCRaw2Digit(Int_t period, Int_t run, TString mappingFile, TString calibrationFile = "", TString MaxPositionFile = "");
    BmnZDCRaw2Digit();

    ~BmnZDCRaw2Digit();

    void print();

    void setParStep(Float_t p) { pstep = p;};
    Float_t getParStep() { return pstep;};

    void setParStart(Float_t p) { pstart = p;};
    Float_t getParStart() { return pstart;};

    void setThreshold(Float_t t) { thres = t;};
    Float_t getThreshold() { return thres;};

    void setEnergy(Float_t e) { shower_energy = e;};
    Float_t getEnergy() { return shower_energy;};

    void setError(Float_t s) { sigma_amp = s;};
    Float_t getError() { return sigma_amp;};

    void setNorm(Float_t n) { shower_norm = n;};
    Float_t getNorm() { return shower_norm;};

    void setType(Int_t t) { wave2amp_flag = t;};
    Int_t getType() { return wave2amp_flag;};

    void setMinSamples(Int_t m) { min_samples = m;};
    Int_t getMinSamples() { return min_samples;};

    void setPedSamples(Int_t p) { ped_samples = p;};
    Int_t getPedSamples() { return ped_samples;};

    void setUseMeanXY(Int_t u) { use_meanxy = u;};
    Int_t getUseMeanXY() { return use_meanxy;};

    void setLogFunction(Int_t u) { use_log_function = u;};
    Int_t getUseLogFunction() { return use_log_function;};

    void fillEvent(TClonesArray *data, TClonesArray *zdcdigit);

    void fillAmplitudes(TClonesArray *data);

    int fillCalibrateCluster(TClonesArray *data, Float_t x, Float_t y, Float_t e, Int_t clsize);

    int fillCalibrateNumbers(TClonesArray *data, Float_t x, Float_t y, Float_t e, Int_t ncells, Int_t *numbers);

    int fillCalibrateAll(TClonesArray *data, Float_t x, Float_t y, Float_t e);

    void fillSampleProfiles(TClonesArray *data, Float_t x, Float_t y, Float_t e, Int_t clsize);

    void fillSampleProfilesAll(TClonesArray *data, Float_t x, Float_t y, Float_t e);

    void calibrate();

    void drawzdc(int nohist = 0);

    void drawprof();

    void drawtest();

    float getAmp(int zch) { if (zch < MAX_CHANNELS) return zdc_amp[zch]; else return -1.;};

    float getLogAmp(int lch) { if (lch < MAX_LOG_CHANNELS) return log_amp[lch]; else return -1.;};

    int getLogId(int lch) { if (lch < MAX_LOG_CHANNELS) return test_id[lch]; else return -1;};

    int getLogChan(int lch) { if (lch < MAX_LOG_CHANNELS) return test_chan[lch]; else return -1;};

//    void fcn(Int_t& npar, Double_t *gin, Double_t& f, Double_t *par, Int_t iflag);

private:
    int digiPar[6];
//    char filname_base[256];
    int maxchan;
    Bmn_ZDC_map_element zdc_map_element[256];
    float cell_size[10];
    int n_rec;
    float thres;
    int wave2amp_flag;
    int MaxPos_min, MaxPos_max;
    int min_samples;
    int ped_samples;
    int use_meanxy;
    float sigma_amp;
    float shower_energy;
    float shower_norm;
    float x_beam, y_beam;
    float x_min, y_min;
    float x_max, y_max;
    float pstart, pstep;
    int ncells;
//    int use_log_function;
//    static int nevents;

//    static float amp_array[MAX_EVENTS][MAX_CHANNELS];
//    static float profile_amp[MAX_EVENTS][MAX_CHANNELS];
//    static float profile_err[MAX_EVENTS][MAX_CHANNELS];
    float zdc_amp[MAX_CHANNELS];
    float log_amp[MAX_LOG_CHANNELS];
    int number[MAX_CHANNELS];
    int index[MAX_CHANNELS];
    int channel0[MAX_CHANNELS];
    int channel1[MAX_CHANNELS];
    float cal[MAX_CHANNELS];
    float cale[MAX_CHANNELS];
    float cal_out[MAX_CHANNELS];
    float cale_out[MAX_CHANNELS];

    void cellWeight(int ievent);
//    static void fcn1(Int_t& npar, Double_t *gin, Double_t& f, Double_t *par, Int_t iflag);
    double ch(double x, double r);
    double sh(double x, double r);
    double PP1(double x, double h);
    double PP2(double x, double h);
    double shower(double x, double h);
    float wave2amp(UChar_t ns, UShort_t *s, Float_t *p, Float_t *sigMin, Float_t *sigMax, Float_t *sigPed, Float_t *sigInt);
    float testwave2amp(UChar_t ns, UShort_t *s, Float_t *p);
    Int_t periodId;
    Int_t runId;
    int n_test;
    int test_chan[MAX_LOG_CHANNELS];
    int test_id[MAX_LOG_CHANNELS];
    int is_test[256];
    TH1F *htest[MAX_LOG_CHANNELS];
    TProfile *TestProf[MAX_LOG_CHANNELS];
    TH1F *hsum_sim;
    TH1F *hsum_raw;
    TH1F *hsum;
    TH1F *hxmean, *hymean;
    TProfile *SampleProf[MAX_CHANNELS];

    ClassDef(BmnZDCRaw2Digit, 3);
};
#endif	/* BMNZDCRAW2DIGIT_H */


