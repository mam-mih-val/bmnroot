#ifndef BMNTOF2RAW2DIGITNEW_H
#define BMNTOF2RAW2DIGITNEW_H

#define SLFIT0 "pol2"
#define SLFIT "pol5"
#define HPTIMEBIN 0.02344
#define INVHPTIMEBIN 42.6666

#define TOF2_MAX_STRIPS_IN_CHAMBER 32
#define TOF2_MAX_CHANNELS_IN_SLOT 64
//#define TOF2_MAX_CHANNELS_IN_MODULE TOF2_MAX_CHANNELS_IN_SLOT
#define TOF2_MAX_CHANNELS_IN_MODULE 64
#define TOF2_MAX_CRATES 5
#define TOF2_MAX_SLOTS_IN_CRATE 20
//#define TOF2_MAX_CHAMBERS 24
#define TOF2_MAX_CHAMBERS 60
#define TOF2_MAX_CHANNEL 4000

//#define TOF700_TDC_TYPE (0x11) //TDC32VL
#define TOF700_TDC_TYPE (0x53) //TDC64VHLE

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
#include "BmnEnums.h"
#include <cstdlib>
#include <bitset>
#include <map>

class Bmn_Tof2_map_element_new{
public:
   Bmn_Tof2_map_element_new(){
     plane=side=id=slot=tdc=chan=crate=strip=0;
   } 
   int crate;
   int pair;
   int plane;
   int id,slot,tdc,chan,strip;   
   char side;            
};

class BmnTof2Raw2DigitNew{

public:
    BmnTof2Raw2DigitNew(TString mappingFile, TString RunFile = "empty", UInt_t SlewingRun = 0, UInt_t SlewingChamber = 0, TString geomFile="TOF700_geometry_run7.txt");
    BmnTof2Raw2DigitNew();

    ~BmnTof2Raw2DigitNew();

    void print();
    void getEventInfo(long long *ev,long long *t1,long long *t2);

    int get_ch_tdc32vl(unsigned int tdc,unsigned int ch);
    int get_ch_tdc64vhle(unsigned int tdc,unsigned int ch);
    int get_ch_tdc72vhl(unsigned int tdc,unsigned int ch);

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

    void fillPreparation(TClonesArray *data, map<UInt_t,Long64_t> *ts, Double_t t0, Double_t t0width);
    void fillEvent(TClonesArray *data, map<UInt_t,Long64_t> *ts, Double_t t0, Double_t t0width, TClonesArray *tof2digit);
    void fillSlewingT0(TClonesArray *data, map<UInt_t,Long64_t> *ts, Double_t t0, Double_t t0width);
    void fillSlewing(TClonesArray *data, map<UInt_t,Long64_t> *ts, Double_t t0, Double_t t0width);
    void fillEqualization(TClonesArray *data, map<UInt_t,Long64_t> *ts, Double_t t0, Double_t t0width);
    void writeSlewingLimits();
    void readSlewingLimits();
    void SlewingT0();
    void readSlewingT0();
    void Slewing();
    void readSlewing();
    void SlewingResults();
    void InitEqualization();
    void Equalization();
    float slewingt0_correction(int chamber, double width, int peak);
    float slewing_correction(int chamber, double width, int peak);
    void drawprep();
    void drawprof();
    void drawproft0();
    int Offsets_read();
    int readGeom(char *geomFile);
    int printGeom();
    int get_strip_xyz(int chamber, int strip, float *x, float *y, float *z);
    int get_chamber_z(int chamber, float *z);
    int get_track_hits(float *xyz, float *cxyy, int *nhits, int *chamb, int *strip);
    void ReBook(int i);
    void Book();
    void BookSlewing();
    void BookSlewingResults();
    void WriteSlewingHists();
    Double_t *GetINL() { return &DNL_Table[0][0][0][0]; }

    void SetSlewingReference(Int_t chamber, Int_t refrun, Int_t refchamber);

private:
    char filname_base[256];
    char filname_slewing[TOF2_MAX_CHAMBERS][256];
    int chamber_slewing[TOF2_MAX_CHAMBERS];
    int type_slewing[TOF2_MAX_CHAMBERS];
    int fSlewCham;
    int n_rec;
    Bmn_Tof2_map_element_new mapa[TOF2_MAX_CHANNEL];
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
    double chtima[10][25][64];

    int numcrate(int id);
    int nrec[TOF2_MAX_CRATES][TOF2_MAX_SLOTS_IN_CRATE][TOF2_MAX_CHANNELS_IN_SLOT];

    float tmean[2][TOF2_MAX_CHANNEL];
    int ntmean[2][TOF2_MAX_CHANNEL];
    float tmean_average[2][TOF2_MAX_CHAMBERS];
    float tmeane[TOF2_MAX_CHANNEL];
    int ntmeane[TOF2_MAX_CHANNEL];
    float tmeane_average[TOF2_MAX_CHAMBERS];

    double DNL_Table[TOF2_MAX_CRATES][TOF2_MAX_SLOTS_IN_CRATE][72][1024];
    int dnltype[TOF2_MAX_CRATES][TOF2_MAX_SLOTS_IN_CRATE];
    char dnlname[TOF2_MAX_CRATES][TOF2_MAX_SLOTS_IN_CRATE][128];

    int wmint0[TOF2_MAX_CHAMBERS][2];
    int wmaxt0[TOF2_MAX_CHAMBERS][2];
    int tmint0[TOF2_MAX_CHAMBERS][2];
    int tmaxt0[TOF2_MAX_CHAMBERS][2];
    double TvsWt0_const[TOF2_MAX_CHAMBERS][2];
    double TvsWt0_slope[TOF2_MAX_CHAMBERS][2];
    double TvsWt0_parab[TOF2_MAX_CHAMBERS][2];

    int wmin[TOF2_MAX_CHAMBERS][2];
    int wmax[TOF2_MAX_CHAMBERS][2];
    int tmin[TOF2_MAX_CHAMBERS][2];
    int tmax[TOF2_MAX_CHAMBERS][2];
    double TvsW_const[TOF2_MAX_CHAMBERS][2];
    double TvsW_slope[TOF2_MAX_CHAMBERS][2];
    double TvsW_parab[TOF2_MAX_CHAMBERS][2];
    double TvsW_cubic[TOF2_MAX_CHAMBERS][2];
    double TvsW_four[TOF2_MAX_CHAMBERS][2];
    double TvsW_five[TOF2_MAX_CHAMBERS][2];

    TH2F *poffsets, *poffsets1, *poffsets2;
    TProfile *TvsW[TOF2_MAX_CHAMBERS][2];
    TProfile *TvsWt0[TOF2_MAX_CHAMBERS][2];
    TH2F *TvsSm[TOF2_MAX_CHAMBERS][2];

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

ClassDef(BmnTof2Raw2DigitNew, 1);
};
#endif	/* BMNTOF2RAW2DIGIT_H */


