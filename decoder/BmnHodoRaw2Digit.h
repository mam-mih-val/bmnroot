#ifndef BMNHodoRAW2DIGIT_H
#define	BMNHodoRAW2DIGIT_H

#include <iostream>
#include <numeric>

#include "FairLogger.h"
#include "TString.h"
#include "TClonesArray.h"
#include "BmnTQDCADCDigit.h"
#include "BmnADCDigit.h"
#include "BmnSyncDigit.h"

#include "Riostream.h"
#include "BmnHodoDigi.h"
#include <cstdlib>
#include <UniDbRun.h>

#include <boost/program_options.hpp>

#include "PronyFitter.h"


class BmnHodoRaw2Digit{

public:
    BmnHodoRaw2Digit(int period, int run, TString mappingFile, TString calibrationFile = "");
    BmnHodoRaw2Digit();

    ~BmnHodoRaw2Digit();

    void ParseConfig(TString mappingFile);
    void ParseCalibration(TString calibrationFile);
    void fillEvent(TClonesArray *tdc_data, TClonesArray *adc_data, TClonesArray *Hododigit);
    void print();

    std::vector<unsigned int> GetHodoSerials() {return fHodoSerials;}
    std::vector<short> GetUniqueXpositions() {return fUniqueX;}
    std::vector<short> GetUniqueYpositions() {return fUniqueY;}
    std::vector<short> GetUniqueSizes() {return fUniqueSize;}
    int GetFlatChannelFromAdcChannel(unsigned int adc_board_id, unsigned int adc_ch);
 
private:
    int fPeriodId; 
    int fRunId;
    TString fmappingFileName;
    TString fcalibrationFileName;

    std::vector<unsigned int> fHodoSerials;
    std::vector<short> fUniqueX;
    std::vector<short> fUniqueY;
    std::vector<short> fUniqueSize;
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
    std::vector<std::pair<float,float>> fCalibVect; // cell_id to pair<calib, calibError>

    void MeanRMScalc(std::vector<float> wfm, float* Mean, float* RMS, int begin, int end, int step = 1);
    void ProcessWfm(std::vector<float> wfm, BmnHodoDigi* digi);

    ClassDef(BmnHodoRaw2Digit, 1);
};
#endif	/* BMNHodoRAW2DIGIT_H */

