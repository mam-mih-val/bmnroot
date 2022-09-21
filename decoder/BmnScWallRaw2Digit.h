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
#include <cstdlib>
#include <UniRun.h>

#include <boost/program_options.hpp>
#include "BmnScWallDigi.h"
#include "WfmProcessor.h"

class BmnScWallRaw2Digit : public WfmProcessor {

public:
    BmnScWallRaw2Digit(int period, int run, TString mappingFile, TString calibrationFile = "");
    BmnScWallRaw2Digit();

    ~BmnScWallRaw2Digit();

    void ParseConfig(TString mappingFile);
    void ParseCalibration(TString calibrationFile);
    void fillEvent(TClonesArray *data, TClonesArray *ScWalldigit);
    void print();

    std::vector<unsigned int> GetScWallSerials() {return fSerials;}
    std::vector<short> GetUniqueXpositions() {return fUniqueX;}
    std::vector<short> GetUniqueYpositions() {return fUniqueY;}
    std::vector<short> GetUniqueSizes() {return fUniqueSize;}
    digiPars GetDigiPars() {return fdigiPars;}
    int GetFlatChannelFromAdcChannel(unsigned int board_id, unsigned int channel);
    std::pair<float,float> GetCalibPairFromAddress(unsigned int address);
 
private:
    static constexpr int CHANNELS_PER_BOARD = 64; // ADC64 boards

    int fPeriodId; 
    int fRunId;
    TString fmappingFileName;
    TString fcalibrationFileName;

    std::vector<unsigned int> fSerials;
    std::vector<short> fUniqueX;
    std::vector<short> fUniqueY;
    std::vector<short> fUniqueSize;
    std::vector<unsigned int> fChannelVect; // flat_channel to unique_address
    std::vector<std::pair<float,float>> fCalibVect; // cell_id to pair<calib, calibError>

    ClassDef(BmnScWallRaw2Digit, 1);
};
#endif	/* BMNSCWALLRAW2DIGIT_H */


