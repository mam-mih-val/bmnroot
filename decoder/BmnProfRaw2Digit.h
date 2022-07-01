#ifndef BMNPROFRAW2DIGIT_H
#define BMNPROFRAW2DIGIT_H

// Auxillary
#include <boost/exception/all.hpp>
#include <boost/circular_buffer.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>
// Root
#include "TMath.h"
#include "TString.h"
#include "TTree.h"
#include "TClonesArray.h"
#include "TH1F.h"
// BmnRoot
#include "BmnADCDigit.h"
#include "BmnSiBTDigit.h"
#include "BmnEnums.h"
#include "BmnMath.h"
#include "BmnAdcProcessor.h"
#include "BmnProfRawTools.h"

#include <BmnSiBTStationSet.h>

#define N_CSC_MODULES 2
#define N_CSC_CHANNELS 2048

using namespace std;
using namespace TMath;
namespace pt = boost::property_tree;

struct ProfiMap {
    char ChannelName = 0;
    char LayerType = 0;
    uint16_t LayerId = 0;
    uint16_t StationId = 0;
    uint16_t ModuleId = 0;
    int StripMap[BmnProfRawTools::ChannelCnt()] = {};
};

class BmnProfRaw2Digit : public BmnAdcProcessor {
public:
    BmnProfRaw2Digit(Int_t period, Int_t run);
    BmnProfRaw2Digit();
    ~BmnProfRaw2Digit();

    map<PlMapKey, ProfiMap> & GetGlobalMap() { return fGlobalMap;}
    
    BmnStatus FillEvent(TClonesArray *adc, TClonesArray *csc);
    BmnStatus FillProfiles(TClonesArray *adc);
    BmnStatus FillNoisyChannels();
    

    static unique_ptr<BmnSiBTStationSet> GetProfStationSet(Int_t period);

    int FillExtractedBits(TClonesArray *fInAdcArray, TClonesArray *fWorkAdcArray) {
        printf("in entries %d\n", fInAdcArray->GetEntriesFast());
        for (UInt_t idigit = 0; idigit < fInAdcArray->GetEntriesFast(); idigit++) {
            BmnADCDigit* digit = (BmnADCDigit*) fInAdcArray->At(idigit);
            uint16_t * arr = digit->GetUShortValue();
            for (auto &m : fChannelMaps) {
                int16_t arr_out[BmnProfRawTools::ChannelCnt()] = {};
                for (size_t j = 0; j < BmnProfRawTools::ChannelCnt(); j++)
                    arr_out[j] = BmnProfRawTools::adc_ch(arr, j * 6, m.second.ChannelName);
                BmnADCDigit* wd = new ((*fWorkAdcArray)[fWorkAdcArray->GetEntriesFast()])
                        BmnADCDigit(
                        digit->GetSerial(),
                        digit->GetChannel(),
                        BmnProfRawTools::ChannelCnt(), arr_out);
                //                int16_t * arr_out = wd->GetShortValue();
            }
        }
        return 0;
    }

private:

    string fLocalMapFileName;
    string fGlobalMapFileName;

    map<PlMapKey, ProfiMap> fGlobalMap;
    map<uint16_t, ProfiMap> fChannelMaps;
    vector<UInt_t> fSerials;
    Int_t fEventId;
    unique_ptr<BmnSiBTStationSet> fSiBTStationSet;

    TH1F**** fSigProf;
    Bool_t**** fNoisyChannels;

    void ProcessAdc(TClonesArray *adc, TClonesArray *csc, Bool_t doFill);
    BmnStatus ReadGlobalMapFile(string name);
    BmnStatus ReadLocalMapFile(string name);

    ClassDef(BmnProfRaw2Digit, 1);
};

#endif /* BMNPROFRAW2DIGIT_H */

