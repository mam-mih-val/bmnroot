#ifndef BMNTOF2RAW2DIGIT_H
#define BMNTOF2RAW2DIGIT_H

#define TOF2_MAX_MODULES 4
#define TOF2_MAX_CHAMBERS 4
#define TOF2_MAX_STRIPS 32
#define TOF2_MAX_CHANNEL 112
#define SLFIT "pol2"
#define HPTIMEBIN 0.02344
#define INVHPTIMEBIN 42.6666

#include "TString.h"
#include "TProfile.h"
#include "TH2F.h"
#include "TClonesArray.h"
#include "BmnTDCDigit.h"
#include "BmnADCDigit.h"
#include "BmnSyncDigit.h"
#include <iostream>
#include "Riostream.h"
#include "BmnTof2Digit.h"
#include <cstdlib>

class Bmn_Tof2_map_element{
public:
   Bmn_Tof2_map_element(){
     plane=side=id,slot,tdc,chan,strip=0;
   } 
   int pair;
   int plane;
   char side;            
   int id,slot,tdc,chan,strip;   
};

class BmnTof2Raw2Digit{

public:
    BmnTof2Raw2Digit(TString mappingFile, TString RunFile = "empty");
    BmnTof2Raw2Digit();
    void print();
    void getEventInfo(long long *ev,long long *t1,long long *t2);

    void INL_read25();
    void INL_read25_gol();
    void INL_read25_ffd();

    int  get_t0() { return T0; }

    void SetWcut(int wcut) { Wcut = wcut; for (int c=0; c<TOF2_MAX_CHAMBERS; c++) ReBook(c); }
    int  GetWcut() { return Wcut; }

    void SetWmax(int wm) { Wmax = wm; for (int c=0; c<TOF2_MAX_CHAMBERS; c++) ReBook(c); }
    int  GetWmax() { return Wmax; }

    void SetWT0max(int wt0m) { WT0max = wt0m; for (int c=0; c<TOF2_MAX_CHAMBERS; c++) ReBook(c); }
    int  GetWT0max() { return WT0max; }

    void SetWT0min(int wt0m) { WT0min = wt0m; for (int c=0; c<TOF2_MAX_CHAMBERS; c++) ReBook(c); }
    int  GetWT0min() { return WT0min; }

    void SetLeadMin(int c, int leadmin) { if (c>0&&c<=TOF2_MAX_CHAMBERS) {LeadMin[c-1] = leadmin; ReBook(c-1);} }
    int  GetLeadMin(int c) { return LeadMin[c]; }

    void SetLeadMax(int c, int leadmax) { if (c>0&&c<=TOF2_MAX_CHAMBERS) {LeadMax[c-1] = leadmax; ReBook(c-1);} }
    int  GetLeadMax(int c) { return LeadMax[c]; }

    void SetLeadMinMax(int c, int leadmin, int leadmax) { if (c>0&&c<=TOF2_MAX_CHAMBERS) {LeadMin[c-1] = leadmin; LeadMax[c-1] = leadmax; ReBook(c-1);}; }

    void fillPreparation(TClonesArray *data, TClonesArray *sync, TClonesArray *t0);
    void fillEvent(TClonesArray *data, TClonesArray *sync, TClonesArray *t0, TClonesArray *tof2digit);
    void fillSlewingT0(TClonesArray *data,  TClonesArray *sync, TClonesArray *t0);
    void fillSlewing(TClonesArray *data, TClonesArray *sync, TClonesArray *t0);
    void SlewingT0();
    void readSlewingT0();
    void Slewing();
    void readSlewing();
    void SlewingResults();
    float slewingt0_correction(int chamber, float width, int peak);
    float slewing_correction(int chamber, float width, int peak);
    void drawprep();
    void drawprof();
    void drawproft0();
    int readGeom(int *numgeom);
    int printGeom();
    int get_strip_xyz(int chamber, int strip, float *x, float *y, float *z);
    int get_chamber_z(int chamber, float *z);
    int get_track_hits(float *xyz, float *cxyy, int *nhits, int *chamb, int *strip);
    void ReBook(int i);

private:
    char filname_base[256];
    int n_rec;
    Bmn_Tof2_map_element map[TOF2_MAX_CHANNEL];
    long long EVENT,TIME_SEC,TIME_NS;
    float T0;
    float T0shift;
    int Wcut;
    int Wmax;
    int WT0min;
    int WT0max;
    int LeadMin[TOF2_MAX_CHAMBERS];
    int LeadMax[TOF2_MAX_CHAMBERS];
    int MaxPlane;
    int numstrip[TOF2_MAX_CHAMBERS];

    float tmean[2][TOF2_MAX_CHANNEL];
    int ntmean[2][TOF2_MAX_CHANNEL];
    float tmean_average[2][TOF2_MAX_CHAMBERS];

    float INL_Table25[TOF2_MAX_MODULES+1][32][1024];

    int wmint0[TOF2_MAX_CHAMBERS][2];
    int wmaxt0[TOF2_MAX_CHAMBERS][2];
    int tmint0[TOF2_MAX_CHAMBERS][2];
    int tmaxt0[TOF2_MAX_CHAMBERS][2];
    float TvsWt0_const[TOF2_MAX_CHAMBERS][2];
    float TvsWt0_slope[TOF2_MAX_CHAMBERS][2];
    float TvsWt0_parab[TOF2_MAX_CHAMBERS][2];

    int wmin[TOF2_MAX_CHAMBERS][2];
    int wmax[TOF2_MAX_CHAMBERS][2];
    int tmin[TOF2_MAX_CHAMBERS][2];
    int tmax[TOF2_MAX_CHAMBERS][2];
    float TvsW_const[TOF2_MAX_CHAMBERS][2];
    float TvsW_slope[TOF2_MAX_CHAMBERS][2];
    float TvsW_parab[TOF2_MAX_CHAMBERS][2];

    TProfile *TvsW[TOF2_MAX_CHAMBERS][2];
    TProfile *TvsWt0[TOF2_MAX_CHAMBERS][2];

    TH2F *TvsS[TOF2_MAX_CHAMBERS];
    TH2F *WvsS[TOF2_MAX_CHAMBERS];

    TH1F *Wt0;
    TH1F *Wts;
    TH2F *TvsWall[TOF2_MAX_CHAMBERS];
    TH2F *TvsWallmax[TOF2_MAX_CHAMBERS];

    int nstrips[TOF2_MAX_CHAMBERS];
    float zchamb[TOF2_MAX_CHAMBERS];
    float xcens[TOF2_MAX_CHAMBERS][TOF2_MAX_STRIPS];
    float ycens[TOF2_MAX_CHAMBERS][TOF2_MAX_STRIPS];
    float xmins[TOF2_MAX_CHAMBERS][TOF2_MAX_STRIPS];
    float xmaxs[TOF2_MAX_CHAMBERS][TOF2_MAX_STRIPS];
    float ymins[TOF2_MAX_CHAMBERS][TOF2_MAX_STRIPS];
    float ymaxs[TOF2_MAX_CHAMBERS][TOF2_MAX_STRIPS];

ClassDef(BmnTof2Raw2Digit, 1);
};
#endif	/* BMNTOF2RAW2DIGIT_H */


