#ifndef BMNSCWALLRAW2DIGIT_H
#define	BMNSCWALLRAW2DIGIT_H

#include "TString.h"
#include "TClonesArray.h"
#include "BmnADCDigit.h"
#include "BmnSyncDigit.h"
#include <iostream>
#include "Riostream.h"
#include "BmnScWallDigi.h"
#include <cstdlib>
#include <UniDbRun.h>

#include <boost/program_options.hpp>


class BmnScWallRaw2Digit{




public:
    BmnScWallRaw2Digit(Int_t period, Int_t run, TString mappingFile, TString calibrationFile = "", TString MaxPositionFile = "");
    BmnScWallRaw2Digit();

    void ParseConfig(TString mappingFile);
    std::vector<UInt_t> GetScWallSerials() {return fScWallSerials;}

    Int_t GetFlatChannelFromAdcChannel(UInt_t adc_board_id, UInt_t adc_ch);


    ~BmnScWallRaw2Digit();

    void print();

 
    void fillEvent(TClonesArray *data, TClonesArray *ScWalldigit);

 
private:

    std::vector<UInt_t> fScWallSerials;
    std::set<UInt_t> fUniqueX;
    std::set<UInt_t> fUniqueY;
    std::set<UInt_t> fUniqueSize;
    std::map<UInt_t, UInt_t> fChannelMap; // pair <flat_channel, unique_address>
    int digiPar[6];

    int maxchan;
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

    float ScWall_amp[64];
    float log_amp[64];
    int number[64];
    int index[64];
    int channel0[64];
    int channel1[64];
    float cal[64];
    float cale[64];
    float cal_out[64];
    float cale_out[64];


    float wave2amp(UChar_t ns, UShort_t *s, Float_t *p, Float_t *sigMin, Float_t *sigMax, Float_t *sigPed, Float_t *sigInt);
    Int_t periodId;
    Int_t runId;

    ClassDef(BmnScWallRaw2Digit, 1);
};
#endif	/* BMNSCWALLRAW2DIGIT_H */


