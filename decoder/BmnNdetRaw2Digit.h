#ifndef BmnNdetRaw2Digit_H
#define	BmnNdetRaw2Digit_H

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

#include <unordered_map>
#include <boost/functional/hash.hpp>
#include <boost/program_options.hpp>
#include "BmnNdetDigi.h"
#include "WfmProcessor.h"


class BmnNdetRaw2Digit : public WfmProcessor {

public:
    BmnNdetRaw2Digit(Int_t period, Int_t run, TString mappingFile, TString calibrationFile = "");
    BmnNdetRaw2Digit();

    ~BmnNdetRaw2Digit();

    void ParseConfig(TString mappingFile);
    void ParseCalibration(TString calibrationFile);
    void fillEvent(TClonesArray *tdc_data, TClonesArray *adc_data, TClonesArray *Hododigit);
    void print();

    std::vector<unsigned int> GetNdetSerials() {return fSerials;}
    std::vector<short> GetUniqueXpositions() {return fUniqueX;}
    std::vector<short> GetUniqueYpositions() {return fUniqueY;}
    std::vector<short> GetUniqueZpositions() {return fUniqueZ;}
    digiPars GetDigiPars() {return fdigiPars;}
    int GetFlatChannelFromAdcChannel(unsigned int board_id, unsigned int channel);
    std::vector<unsigned int> GetChannelVect() {return fChannelVect;}
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
    std::vector<short> fUniqueZ;
    std::vector<unsigned int> fChannelVect; // flat_channel to unique_address
    std::unordered_map<std::pair<int,int>, std::pair<float,float>, boost::hash<pair<int,int>>> fuoCalibMap; // physical channel to pair<calib, calibError>

    ClassDef(BmnNdetRaw2Digit, 1);
};
#endif	/* BmnNdetRaw2Digit_H */


