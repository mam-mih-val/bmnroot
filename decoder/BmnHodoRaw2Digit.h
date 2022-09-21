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
#include <cstdlib>
#include <UniRun.h>

#include <boost/program_options.hpp>
#include "BmnHodoDigi.h"
#include "WfmProcessor.h"

class BmnHodoRaw2Digit : public WfmProcessor {

public:
    BmnHodoRaw2Digit(int period, int run, TString mappingFile, TString calibrationFile = "");
    BmnHodoRaw2Digit();

    ~BmnHodoRaw2Digit();

    void ParseConfig(TString mappingFile);
    void ParseCalibration(TString calibrationFile);
    void fillEvent(TClonesArray *tdc_data, TClonesArray *adc_data, TClonesArray *Hododigit);
    void print();

    std::vector<unsigned int> GetHodoSerials() {return fSerials;}
    std::vector<short> GetUniqueXpositions() {return fUniqueX;}
    std::vector<short> GetUniqueYpositions() {return fUniqueY;}
    std::vector<short> GetUniqueSizes() {return fUniqueSize;}
    digiPars GetDigiPars() {return fdigiPars;}
    int GetFlatChannelFromAdcChannel(unsigned int board_id, unsigned int channel);
    std::pair<float,float> GetCalibPairFromAddress(unsigned int address);
 
private:
    static constexpr int CHANNELS_PER_BOARD = 16; // TQDC boards

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
    
    ClassDef(BmnHodoRaw2Digit, 1);
};
#endif	/* BMNHodoRAW2DIGIT_H */

