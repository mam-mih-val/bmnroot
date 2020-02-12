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

#include <BmnGemStripDigit.h>
#include <BmnSiliconDigit.h>
#include <BmnCSCDigit.h>
#include <BmnGemStripStationSet.h>
#include <BmnSiliconStationSet.h>
#include <BmnCSCStationSet.h>

using namespace std;

class BmnLambdaMisc : public TNamed {
public:
    BmnLambdaMisc();

    virtual ~BmnLambdaMisc();

    /* GEM */
    Int_t GemDigiToChannel(BmnStripDigit*, Long_t&); // GEM-digi ---> channel (serial) 
    Long_t GemDigiChannelToSerial(pair <BmnStripDigit, Int_t>); // GEM-digi + channel ---> serial 
    Long_t GetGemSerial(Int_t, Int_t, Int_t, Int_t); // (st, mod, id, channel ---> serial)
    
    /* CSC */
    Int_t CscDigiToChannel(BmnStripDigit*, Long_t&); // CSC-digi ---> channel (serial)
    
    /* SILICON */
    void SiliconDigiToChannelSampleSerial(BmnStripDigit*, Int_t&, Int_t&, Long_t&); // SILICON-digi ---> &channel, &sample, &serial  

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
    map <vector <Int_t>, vector < Long_t>> serialsSilicon;  // <st, mod, layer, id> ---> <low, high, serial>
    
    /* CSC */
    // Map with CSC serials ...
    map <vector <Int_t>, vector < Long_t>> serialsCsc;  // <st, mod, layer> ---> <low, high, serial>
    
    // Geometry sets ...
    BmnGemStripStationSet* fDetectorGEM;
    BmnSiliconStationSet* fDetectorSI;
    BmnCSCStationSet* fDetectorCSC;
    
    ClassDef(BmnLambdaMisc, 1)
};

#endif