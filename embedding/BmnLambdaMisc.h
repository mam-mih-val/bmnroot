// @(#)bmnroot/embedding:$Id$
// Author: Pavel Batyuk <pavel.batyuk@jinr.ru> 2019-12-07

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// BmnLambdaMisc                                                              //
//                                                                            //
// Useful instruments to work with mapping files (GEM, SILICON)               //
// (reading, sorting, correspondence e.t.c.)                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef BMNLAMBDAMISC_H
#define BMNLAMBDAMISC_H 1

#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <TNamed.h>
#include <TSystem.h>
#include <TClonesArray.h>

#include <BmnGemStripDigit.h>
#include <BmnSiliconDigit.h>
#include <BmnCSCDigit.h>
#include <BmnGemStripStationSet.h>
#include <BmnSiliconStationSet.h>
#include <BmnCSCStationSet.h>

#include <BmnLambdaEmbeddingQa.h>

using namespace std;

class MappingInfo : public TObject {
public:
    MappingInfo() {};

    MappingInfo(Int_t stat, TString map, pair <Int_t, Int_t> ch, pair <Int_t, Int_t> str) :
    channel(-1),
    strip(-1) {
        station = stat;
        mapFile = map;
        channels = ch;
        strips = str;
    }

    MappingInfo(Int_t stat, TString map, Int_t ch, Int_t str) :
    channels(make_pair(-1, -1.)),
    strips(make_pair(-1, -1)) {
        station = stat;
        mapFile = map;
        channel = ch;
        strip = str;
    }

    MappingInfo(Int_t stat, TString mapping, map <Int_t, Int_t> strGlobChan) :
    channels(make_pair(-1, -1.)),
    strips(make_pair(-1, -1)),
    channel(-1),
    strip(-1) {
        station = stat;
        mapFile = mapping;
        stripChan = strGlobChan;
    }

    virtual ~MappingInfo() {
    };

    Int_t station;
    TString mapFile;
    pair <Int_t, Int_t> channels;
    pair <Int_t, Int_t> strips;

    // Additional data members ...
    Int_t channel;
    Int_t strip;

    map <Int_t, Int_t> stripChan;
};

class BmnLambdaMisc : public TNamed {
public:
    BmnLambdaMisc();

    virtual ~BmnLambdaMisc();

    /* GEM */
    Int_t GemDigiToChannel(BmnStripDigit*, Long_t&); // GEM-digi ---> channel (serial) 
    TString GemDigiToMapping(BmnStripDigit*); // GEM-digi ---> corresponding <strip-channel> mapping 
    Long_t GemDigiChannelToSerial(pair <BmnStripDigit, Int_t>); // GEM-digi + channel ---> serial 
    Long_t GetGemSerial(Int_t, Int_t, Int_t, Int_t); // (st, mod, id, channel ---> serial)

    /* CSC */
    Int_t CscDigiToChannel(BmnStripDigit*, Long_t&); // CSC-digi ---> channel (serial)

    /* SILICON */
    void SiliconDigiToChannelSampleSerial(BmnStripDigit*, Int_t&, Int_t&, Long_t&); // SILICON-digi ---> &channel, &sample, &serial  

    /* Supplementary methods to be used for testing */
    void CheckStripOverlaps(); // To be used for checking whether strips from common serial are overlapped with those ones from other serials or not ...

private:
    void GEM();
    void SILICON();
    void CSC();

private:
    /* GEM */
    vector <vector <Int_t>> fGemStatModId; // vector of (stat, module, id)
    /* SILICON */
    vector <vector <Int_t>> fSiliconStatModLayId; // vector of (stat, module, layer, id)
    /* CSC */
    vector <vector <Int_t>> fCscStatModLay; // vector of (stat, module, layer)

    /* GEM */
    // Map with commonSerial only ...
    map <vector <Int_t>, vector < Long_t>> corrMapCommonSerial; // <st, mod, id> ---> <low, high, serial>

    // Map with no commonSerial included ...
    map <vector <Int_t>, vector < Long_t>> corrMapNoCommonSerial; // <st, mod, id> ---> <low, high, serial>

    /* SILICON */
    // Map with SILICON serials ...
    map <vector <Int_t>, vector < Long_t>> serialsSilicon; // <st, mod, layer, id> ---> <low, high, serial>

    /* CSC */
    // Map with CSC serials ...
    map <vector <Int_t>, vector < Long_t>> serialsCsc; // <st, mod, layer> ---> <low, high, serial>

    // Geometry sets ...
    BmnGemStripStationSet* fDetectorGEM;
    BmnSiliconStationSet* fDetectorSI;
    BmnCSCStationSet* fDetectorCSC;

    UInt_t FindChannelByStrip(TString, Int_t);

    void ParseStripChannelMapping(TString, Int_t, map <Int_t, Int_t>&);

    ClassDef(BmnLambdaMisc, 1)
};

#endif