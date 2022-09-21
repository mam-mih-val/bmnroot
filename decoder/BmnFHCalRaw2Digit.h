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
#include <cstdlib>
#include <UniRun.h>

#include <boost/program_options.hpp>
#include "BmnFHCalDigi.h"
#include "WfmProcessor.h"

class BmnFHCalRaw2Digit : public WfmProcessor {

public:
    BmnFHCalRaw2Digit(Int_t period, Int_t run, TString mappingFile, TString calibrationFile = "");
    BmnFHCalRaw2Digit();

    ~BmnFHCalRaw2Digit();

    void ParseConfig(TString mappingFile);
    void ParseCalibration(TString calibrationFile);
    void fillEvent(TClonesArray *data, TClonesArray *FHCaldigit);
    void print();

    std::vector<unsigned int> GetFHCalSerials() {return fSerials;}
    std::vector<short> GetUniqueXpositions() {return fUniqueX;}
    std::vector<short> GetUniqueYpositions() {return fUniqueY;}
    std::vector<short> GetUniqueZpositions() {return fUniqueZ;}
    digiPars GetDigiPars() {return fdigiPars;}
    int GetFlatChannelFromAdcChannel(unsigned int board_id, unsigned int channel);
    int GetFlatIndex(int mod_id, int sec_id);
    std::vector<unsigned int> GetChannelVect() {return fChannelVect;}
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
    std::vector<short> fUniqueZ;
    std::vector<unsigned int> fChannelVect; // flat_channel to unique_address
    std::vector<std::pair<float,float>> fCalibVect; // flat_calo_channel to pair<calib, calibError>

    ClassDef(BmnFHCalRaw2Digit, 1);
};
#endif	/* BmnFHCalRaw2Digit_H */


