#ifndef BMNTOF2RAW2DIGITNEW_H
#define BMNTOF2RAW2DIGITNEW_H

#define NSLFIT0 "pol5"
#define NSLFIT "pol8"
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
#include "TProfile2D.h"
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
    BmnTof2Raw2DigitNew(TString mappingFile, TString RunFile = "empty", Int_t SlewingRun = 0, UInt_t SlewingChamber = 0, TString geomFile="TOF700_geometry_run7.txt");
    BmnTof2Raw2DigitNew();

    ~BmnTof2Raw2DigitNew();

    void print();
    void getEventInfo(long long *ev,long long *t1,long long *t2);

    int get_ch_tdc32vl(unsigned int tdc,unsigned int ch);
    int get_ch_tdc64vhle(unsigned int tdc,unsigned int ch);
    int get_ch_tdc72vhl(unsigned int tdc,unsigned int ch);

    void DNL_read();

    int  get_t0() { return T0; }

    void SetWcut(int wcut) {if (wcut != Wcut) { Wcut = wcut; for (int c=0; c<MaxPlane; c++) ReBook(c); }}
    int  GetWcut() { return Wcut; }
    void SetWcutc(int c, int wcut) { if (c>0&&c<=MaxPlane) {if (wcut != Wcutc[c-1]) { Wcutc[c-1] = wcut; ReBook(c-1); } } }
    int  GetWcutc(int c) { if (c>0&&c<=MaxPlane) return Wcutc[c-1]; else return 0; }

    void SetWmax(int wm) {if (wm != Wmax) { Wmax = wm; for (int c=0; c<MaxPlane; c++) ReBook(c); }}
    int  GetWmax() { return Wmax; }
    void SetWmaxc(int c, int wm) { if (c>0&&c<=MaxPlane) {if (wm != Wmaxc[c-1]) { Wmaxc[c-1] = wm; ReBook(c-1); } } }
    int  GetWmaxc(int c) { if (c>0&&c<=MaxPlane) return Wmaxc[c-1]; else return 0; }

    void SetW(int wcut, int wm) {if (wcut != Wcut || wm != Wmax) { Wcut = wcut;  Wmax = wm; for (int c=0; c<MaxPlane; c++) ReBook(c); }}

    void SetWT0max(int wt0m) {if (wt0m != WT0max) { WT0max = wt0m; for (int c=0; c<MaxPlane; c++) ReBook(c); } }
    int  GetWT0max() { return WT0max; }

    void SetWT0min(int wt0m) {if (wt0m != WT0min) { WT0min = wt0m; for (int c=0; c<MaxPlane; c++) ReBook(c); } }
    int  GetWT0min() { return WT0min; }

    void SetWT0(int wt1, int wt2) {if (wt1 != WT0min || wt2 != WT0max) { WT0min = wt1;  WT0max = wt2; for (int c=0; c<MaxPlane; c++) ReBook(c); } }

    void SetLeadMin(int c, int leadmin) { if (c>0&&c<=MaxPlane) if (leadmin != LeadMin[c-1]) {LeadMin[c-1] = leadmin; ReBook(c-1);} }
    int  GetLeadMin(int c) { if (c>0&&c<=MaxPlane) return LeadMin[c-1]; else return 0;}

    void SetLeadMax(int c, int leadmax) { if (c>0&&c<=MaxPlane) if (leadmax != LeadMax[c-1]) {LeadMax[c-1] = leadmax; ReBook(c-1);} }
    int  GetLeadMax(int c) { if (c>0&&c<=MaxPlane) return LeadMax[c-1]; else return 0; }

    void SetLeadMinMax(int c, int leadmin, int leadmax) {if (c>0&&c<=MaxPlane) if (leadmin != LeadMin[c-1] || leadmax != LeadMax[c-1]) {LeadMin[c-1] = leadmin; LeadMax[c-1] = leadmax; ReBook(c-1);}; }

    void fillPreparation(TClonesArray *data, map<UInt_t,Long64_t> *ts, Double_t t0, Double_t t0width);
    void fillEvent(TClonesArray *data, map<UInt_t,Long64_t> *ts, Double_t t0, Double_t t0width, TClonesArray *tof2digit);
    void fillSlewingT0(TClonesArray *data, map<UInt_t,Long64_t> *ts, Double_t t0, Double_t t0width);
    void fillSlewing(TClonesArray *data, map<UInt_t,Long64_t> *ts, Double_t t0, Double_t t0width);
    void fillEqualization(TClonesArray *data, map<UInt_t,Long64_t> *ts, Double_t t0, Double_t t0width);
    void writeSlewingLimits();
    void readSlewingLimits();
//    void SlewingT0();
//    void readSlewingT0();
//    void Slewing();
    void readSlewing(Bool_t update = false);
    void SlewingResults();
//    void InitEqualization();
    void FitSlewing();
    void Equalization();
    void Equalization0();
//    float slewingt0_correction(int chamber, double width, int peak);
//    float slewing_correction(int chamber, double width, int peak);
    void drawprep();
    void drawprof();
    void drawproft0();
    int Offsets_read();
    int readGeom(const char *geomFile);
    int readLRoffsets(char *LRoffsetsFile);
    int readXYCalibration(char *xcalibration, char *ycalibration);
    int readXYCalibrationNew(char *xcalibration, char *ycalibration);
    int printGeom();
    int get_strip_xyz(int chamber, int strip, float *x, float *y, float *z);
    int get_chamber_z(int chamber, float *z);
    int get_track_hits(float *xyz, float *cxyy, int *nhits, int *chamb, int *strip);
    float get_hit_diff0(int chamber, int strip, float diff);
    float get_hit_x0(int chamber, int strip, float diff);
    void get_hit_xyz0(int chamber, int strip, float diff, float *x, float *y, float *z);
    float get_hit_xs0(int chamber, int strip, float diff);
    void get_hit_xyzs0(int chamber, int strip, float diff, float *x, float *y, float *z);
    float get_hit_diff(int chamber, int strip, float diff_corrected);
    float get_hit_x(int chamber, int strip, float diff_corrected);
    void get_hit_xyz(int chamber, int strip, float diff_corrected, float *x, float *y, float *z);
    float get_hit_xs(int chamber, int strip, float diff_corrected);
    void get_hit_xyzs(int chamber, int strip, float diff_corrected, float *x, float *y, float *z);
    float get_hit_xp(int chamber, int strip, float diff);
    void get_hit_xyzp(int chamber, int strip, float diff, float *x, float *y, float *z);
    float get_hit_xng(int chamber, int strip, float diff);
    void get_hit_xyzng(int chamber, int strip, float diff, float *x, float *y, float *z);
    float get_hit_xnl(int chamber, int strip, float diff);
    void get_hit_xyznl(int chamber, int strip, float diff, float *x, float *y, float *z);
    void ReBook(int i);
    void Book();
    void BookSlewing();
    void BookSlewingResults();
    void WriteSlewingResults();
    void WritePreparationHists();
    void WritePreparationHists0();
    Double_t *GetINL() { return &DNL_Table[0][0][0][0]; }

    void SetSlewingReference(Int_t chamber, Int_t refrun, Int_t refchamber);

private:
    char filname_base[256];
    char filname_slewing[TOF2_MAX_CHAMBERS][256];
    int chamber_slewing[TOF2_MAX_CHAMBERS];
    int type_slewing[TOF2_MAX_CHAMBERS];
    int fSlewCham;
    int fSlewRun;
    int fRUN;
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
    int LeadMinW[TOF2_MAX_CHAMBERS][500];
    int LeadMaxW[TOF2_MAX_CHAMBERS][500];
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

    double DNL_Table[TOF2_MAX_CRATES][TOF2_MAX_SLOTS_IN_CRATE][72][1024];
    int dnltype[TOF2_MAX_CRATES][TOF2_MAX_SLOTS_IN_CRATE];
    char dnlname[TOF2_MAX_CRATES][TOF2_MAX_SLOTS_IN_CRATE][128];

    TH2F *poffsets, *poffsets1, *poffsets2;
    TProfile2D *TvsW[TOF2_MAX_CHAMBERS][4];
    TProfile2D *TvsWt0[TOF2_MAX_CHAMBERS][4];
    TProfile *TvsWr[TOF2_MAX_CHAMBERS][4];
    TProfile *TvsWt0r[TOF2_MAX_CHAMBERS][4];
    TH2F *TvsWp[TOF2_MAX_CHAMBERS][4];
    TH2F *TvsWt0p[TOF2_MAX_CHAMBERS][4];
    TH2F *TvsSm[TOF2_MAX_CHAMBERS][4];
    TProfile *Toffsets[4];
    TProfile *Toffsetsf[4];
    TProfile *Toffsets0;
    TProfile *pLeadMin;
    TProfile *pLeadMax;
    TH1F *pWlimits;
    double Toffsets00[TOF2_MAX_CHANNEL];
    double Toffsets00n[TOF2_MAX_CHANNEL];
    double Toffsets00e[TOF2_MAX_CHANNEL];

    TH2F *TvsS[TOF2_MAX_CHAMBERS];
    TH2F *WvsS[TOF2_MAX_CHAMBERS];

    TH1F *Wt0;
    TH1F *Wts;
    TH2F *Hchambrate;
    TH1F *H1chambrate;
    TH2F *Hstriprate;
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

    float lroffsets[TOF2_MAX_CHAMBERS][TOF2_MAX_STRIPS_IN_CHAMBER];
    int lrsign[TOF2_MAX_CHAMBERS][TOF2_MAX_STRIPS_IN_CHAMBER];
    float fVelosity;

    float xslope[TOF2_MAX_CHAMBERS][TOF2_MAX_STRIPS_IN_CHAMBER];
    float xoffset[TOF2_MAX_CHAMBERS][TOF2_MAX_STRIPS_IN_CHAMBER];
    float yslope[TOF2_MAX_CHAMBERS];
    float yoffset[TOF2_MAX_CHAMBERS];

    float interxg[TOF2_MAX_CHAMBERS][TOF2_MAX_STRIPS_IN_CHAMBER], slopexg[TOF2_MAX_CHAMBERS][TOF2_MAX_STRIPS_IN_CHAMBER];
    float interxl[TOF2_MAX_CHAMBERS][TOF2_MAX_STRIPS_IN_CHAMBER], slopexl[TOF2_MAX_CHAMBERS][TOF2_MAX_STRIPS_IN_CHAMBER];
    float shifty[TOF2_MAX_CHAMBERS][TOF2_MAX_STRIPS_IN_CHAMBER];

    TFile *fSlewing;
    TFile *fSlewingr;

ClassDef(BmnTof2Raw2DigitNew, 1);
};
#endif	/* BMNTOF2RAW2DIGIT_H */


