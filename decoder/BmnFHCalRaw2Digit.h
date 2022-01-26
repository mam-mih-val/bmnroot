#ifndef BmnFHCalRaw2Digit_H
#define	BmnFHCalRaw2Digit_H

#include <iostream>
#include <numeric>

#include "FairLogger.h"
#include "TString.h"
#include "TClonesArray.h"
#include "BmnADCDigit.h"
#include "BmnSyncDigit.h"

#include "Riostream.h"
#include "BmnFHCalDigi.h"
#include <cstdlib>
#include <UniDbRun.h>

#include <boost/program_options.hpp>

#include "PronyFitter.h"


class BmnFHCalRaw2Digit{

public:
    BmnFHCalRaw2Digit(Int_t period, Int_t run, TString mappingFile, TString calibrationFile = "");
    BmnFHCalRaw2Digit();

    ~BmnFHCalRaw2Digit();

    void ParseConfig(TString mappingFile);
    void ParseCalibration(TString calibrationFile);
    void fillEvent(TClonesArray *data, TClonesArray *FHCaldigit);
    void print();

    std::vector<unsigned int> GetFHCalSerials() {return fFHCalSerials;}
    std::vector<short> GetUniqueXpositions() {return fUniqueX;}
    std::vector<short> GetUniqueYpositions() {return fUniqueY;}
    std::vector<short> GetUniqueZpositions() {return fUniqueZ;}
    int GetFlatChannelFromAdcChannel(unsigned int adc_board_id, unsigned int adc_ch);
    int GetFlatCaloChannel(int mod_id, int sec_id);
 
private:
    int fPeriodId; 
    int fRunId;
    TString fmappingFileName;
    TString fcalibrationFileName;

    std::vector<unsigned int> fFHCalSerials;
    std::vector<short> fUniqueX;
    std::vector<short> fUniqueY;
    std::vector<short> fUniqueZ;
    std::vector<unsigned int> fChannelVect; // flat_channel to unique_address

    struct digiPars {
      bool isWriteWfm;
      int gateBegin;
      int gateEnd;
      float threshold;
      int signalType;
      bool doInvert;

      bool isfit;
      std::vector<std::complex<float>> harmonics;
    } fdigiPars;
    std::vector<std::pair<float,float>> fCalibVect; // flat_calo_channel to pair<calib, calibError>

    void MeanRMScalc(std::vector<float> wfm, float* Mean, float* RMS, int begin, int end, int step = 1);
    void ProcessWfm(std::vector<float> wfm, BmnFHCalDigi* digi);


    ClassDef(BmnFHCalRaw2Digit, 1);
};
#endif	/* BmnFHCalRaw2Digit_H */


