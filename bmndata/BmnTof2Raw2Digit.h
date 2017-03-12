#ifndef BMNTOF2RAW2DIGIT_H
#define BMNTOF2RAW2DIGIT_H

#define SLFIT0 "pol2"
#define SLFIT "pol3"
#define HPTIMEBIN 0.02344
#define INVHPTIMEBIN 42.6666

#define TOF2_MAX_STRIPS_IN_CHAMBER 32
#define TOF2_MAX_CHANNELS_IN_SLOT 64
//#define TOF2_MAX_CHANNELS_IN_MODULE TOF2_MAX_CHANNELS_IN_SLOT
#define TOF2_MAX_CHANNELS_IN_MODULE 64
#define TOF2_MAX_CRATES 5
#define TOF2_MAX_SLOTS_IN_CRATE 20
#define TOF2_MAX_CHAMBERS 24
#define TOF2_MAX_CHANNEL 1600

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
     plane=side=id=slot=tdc=chan=crate=strip=0;
   } 
   int crate;
   int pair;
   int plane;
   int id,slot,tdc,chan,strip;   
   char side;            
};

class BmnTof2Raw2Digit{

public:
    BmnTof2Raw2Digit(TString mappingFile, TString RunFile = "empty", UInt_t SlewingRun = 0, UInt_t SlewingChamber = 0, TString geomFile = "");
    BmnTof2Raw2Digit();
    void print();
    void getEventInfo(long long *ev,long long *t1,long long *t2);

    void DNL_read();

    int  get_t0() { return T0; }

    void SetWcut(int wcut) { Wcut = wcut; for (int c=0; c<MaxPlane; c++) ReBook(c); }
    int  GetWcut() { return Wcut; }
    void SetWcutc(int c, int wcut) { if (c>0&&c<=MaxPlane) { Wcutc[c-1] = wcut; ReBook(c-1); } }
    int  GetWcutc(int c) { if (c>0&&c<=MaxPlane) return Wcutc[c-1]; else return 0; }

    void SetWmax(int wm) { Wmax = wm; for (int c=0; c<MaxPlane; c++) ReBook(c); }
    int  GetWmax() { return Wmax; }
    void SetWmaxc(int c, int wm) { if (c>0&&c<=MaxPlane) { Wmaxc[c-1] = wm; ReBook(c-1); } }
    int  GetWmaxc(int c) { if (c>0&&c<=MaxPlane) return Wmaxc[c-1]; else return 0; }

    void SetW(int wcut, int wm) { Wcut = wcut;  Wmax = wm; for (int c=0; c<MaxPlane; c++) ReBook(c); }

    void SetWT0max(int wt0m) { WT0max = wt0m; for (int c=0; c<MaxPlane; c++) ReBook(c); }
    int  GetWT0max() { return WT0max; }

    void SetWT0min(int wt0m) { WT0min = wt0m; for (int c=0; c<MaxPlane; c++) ReBook(c); }
    int  GetWT0min() { return WT0min; }

    void SetWT0(int wt1, int wt2) { WT0min = wt1;  WT0max = wt2; for (int c=0; c<MaxPlane; c++) ReBook(c); }

    void SetLeadMin(int c, int leadmin) { if (c>0&&c<=MaxPlane) {LeadMin[c-1] = leadmin; ReBook(c-1);} }
    int  GetLeadMin(int c) { if (c>0&&c<=MaxPlane) return LeadMin[c-1]; else return 0;}

    void SetLeadMax(int c, int leadmax) { if (c>0&&c<=MaxPlane) {LeadMax[c-1] = leadmax; ReBook(c-1);} }
    int  GetLeadMax(int c) { if (c>0&&c<=MaxPlane) return LeadMax[c-1]; else return 0; }

    void SetLeadMinMax(int c, int leadmin, int leadmax) { if (c>0&&c<=MaxPlane) {LeadMin[c-1] = leadmin; LeadMax[c-1] = leadmax; ReBook(c-1);}; }

    void fillPreparation(TClonesArray *data, TClonesArray *sync, TClonesArray *t0);
    void fillEvent(TClonesArray *data, TClonesArray *sync, TClonesArray *t0, TClonesArray *tof2digit);
    void fillSlewingT0(TClonesArray *data,  TClonesArray *sync, TClonesArray *t0);
    void fillSlewing(TClonesArray *data, TClonesArray *sync, TClonesArray *t0);
    void readSlewingLimits();
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
    int readGeom(char *geomFile);
    int printGeom();
    int get_strip_xyz(int chamber, int strip, float *x, float *y, float *z);
    int get_chamber_z(int chamber, float *z);
    int get_track_hits(float *xyz, float *cxyy, int *nhits, int *chamb, int *strip);
    void ReBook(int i);

private:
    char filname_base[256];
    int fSlewCham;
    int n_rec;
    Bmn_Tof2_map_element map[TOF2_MAX_CHANNEL];
    long long EVENT,TIME_SEC,TIME_NS;
    float T0;
    float T0shift;
    int Wcut;
    int Wmax;
    int Wcutc[TOF2_MAX_CHAMBERS];
    int Wmaxc[TOF2_MAX_CHAMBERS];
    int WT0min;
    int WT0max;
    int LeadMin[TOF2_MAX_CHAMBERS];
    int LeadMax[TOF2_MAX_CHAMBERS];
    int MaxPlane;
    int numstrip[TOF2_MAX_CHAMBERS];
    int numcr[TOF2_MAX_CRATES*TOF2_MAX_SLOTS_IN_CRATE], numcha[TOF2_MAX_CHAMBERS];
    int nslots, ncrates, nchambers;
    float idchambers[TOF2_MAX_CHAMBERS]; 
    int numslots[TOF2_MAX_CRATES*TOF2_MAX_SLOTS_IN_CRATE]; 
    int idcrates[TOF2_MAX_CRATES], numcrates[TOF2_MAX_CRATES]; 

    float tmean[2][TOF2_MAX_CHANNEL];
    int ntmean[2][TOF2_MAX_CHANNEL];
    float tmean_average[2][TOF2_MAX_CHAMBERS];

    int numcrate(int id);
    int nrec[TOF2_MAX_CRATES][TOF2_MAX_SLOTS_IN_CRATE][TOF2_MAX_CHANNELS_IN_SLOT];

    double DNL_Table[TOF2_MAX_CRATES][TOF2_MAX_SLOTS_IN_CRATE][72][1024];
    int dnltype[TOF2_MAX_CRATES][TOF2_MAX_SLOTS_IN_CRATE];
    char dnlname[TOF2_MAX_CRATES][TOF2_MAX_SLOTS_IN_CRATE][128];

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
    float TvsW_cubic[TOF2_MAX_CHAMBERS][2];

    TProfile *TvsW[TOF2_MAX_CHAMBERS][2];
    TProfile *TvsWt0[TOF2_MAX_CHAMBERS][2];

    TH2F *TvsS[TOF2_MAX_CHAMBERS];
    TH2F *WvsS[TOF2_MAX_CHAMBERS];

    TH1F *Wt0;
    TH1F *Wts;
    TH2F *TvsWall[TOF2_MAX_CHAMBERS];
    TH2F *TvsWallmax[TOF2_MAX_CHAMBERS];

    float halfxwidth[TOF2_MAX_CHAMBERS];
    float halfywidth[TOF2_MAX_CHAMBERS];
    float xoffs, yoffs, zoffs;
    int nstrips[TOF2_MAX_CHAMBERS];
    float zchamb[TOF2_MAX_CHAMBERS];
    float xcens[TOF2_MAX_CHAMBERS][TOF2_MAX_STRIPS_IN_CHAMBER];
    float ycens[TOF2_MAX_CHAMBERS][TOF2_MAX_STRIPS_IN_CHAMBER];
    float xmins[TOF2_MAX_CHAMBERS][TOF2_MAX_STRIPS_IN_CHAMBER];
    float xmaxs[TOF2_MAX_CHAMBERS][TOF2_MAX_STRIPS_IN_CHAMBER];
    float ymins[TOF2_MAX_CHAMBERS][TOF2_MAX_STRIPS_IN_CHAMBER];
    float ymaxs[TOF2_MAX_CHAMBERS][TOF2_MAX_STRIPS_IN_CHAMBER];

ClassDef(BmnTof2Raw2Digit, 1);
};
#endif	/* BMNTOF2RAW2DIGIT_H */


