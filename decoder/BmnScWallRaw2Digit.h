#ifndef BMNSCWALLRAW2DIGIT_H
#define	BMNSCWALLRAW2DIGIT_H

#include <iostream>
#include <numeric>

#include "FairLogger.h"
#include "TString.h"
#include "TClonesArray.h"
#include "BmnADCDigit.h"
#include "BmnSyncDigit.h"

#include "Riostream.h"
#include "BmnScWallDigi.h"
#include <cstdlib>
#include <UniDbRun.h>

#include <boost/program_options.hpp>

#include "PronyFitter.h"

struct digiPars {
  int gateBegin;
  int gateEnd;
  float threshold;
  int signalType;
  bool doInvert;

  bool isfit;
  std::vector<std::complex<float>> harmonics;
};


class BmnScWallRaw2Digit{

public:
    BmnScWallRaw2Digit(Int_t period, Int_t run, TString mappingFile, TString calibrationFile = "");
    BmnScWallRaw2Digit();

    ~BmnScWallRaw2Digit();

    void ParseConfig(TString mappingFile);
    void ParseCalibration(TString calibrationFile);
    void fillEvent(TClonesArray *data, TClonesArray *ScWalldigit);
    void print();

    std::vector<UInt_t> GetScWallSerials() {return fScWallSerials;}
    Int_t GetFlatChannelFromAdcChannel(UInt_t adc_board_id, UInt_t adc_ch);
 
private:
    int fPeriodId; 
    int fRunId;
    TString fmappingFileName;
    TString fcalibrationFileName;

    std::vector<UInt_t> fScWallSerials;
    std::set<UInt_t> fUniqueX;
    std::set<UInt_t> fUniqueY;
    std::set<UInt_t> fUniqueSize;
    std::vector<UInt_t> fChannelVect; // flat_channel to unique_address

    digiPars fdigiPars;
    std::vector<std::pair<float,float>> fCalibVect; // flat_channel to pair<calib, calibError>

    void MeanRMScalc(std::vector<float> wfm, float* Mean, float* RMS, int begin, int end, int step = 1);
    void ProcessWfm(std::vector<float> wfm, BmnScWallDigi* digi);


    ClassDef(BmnScWallRaw2Digit, 1);
};
#endif	/* BMNSCWALLRAW2DIGIT_H */


