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

using namespace std;

class BmnLambdaMisc : public TNamed {
public:
    BmnLambdaMisc();

    ~BmnLambdaMisc() {
    };

    Int_t GemDigiToChannel(BmnGemStripDigit*); // GEM-digi ---> channel 
    Long_t GemDigiChannelToSerial(pair <BmnGemStripDigit, Int_t>); // GEM-digi + channel ---> serial 
    Long_t GetGemSerial(Int_t, Int_t, Int_t, Int_t); // (st, mod, id, channel ---> serial)

    inline void GetSerial() {
        // To be used when doing debug procedure ...
        cout << "Printed sorted GEM mapping info ... " << endl;
        for (auto it : fGem)
            cout << std::dec << it.first[0] << " " << it.first[1] << " " << std::hex << it.first[2] << " ---> "
                << std::dec << it.second[0] << " " << it.second[1] << " " << it.second[2] << endl;
    }

private:
    void GEM();
    void SILICON();

private:
    vector <vector <Int_t>> fGemStatModId; // vector of (stat, module, id)

    // All entries are unique by key !!!
    // The separation done in GEM() is taken into account here !!!
    map <vector <Long_t>, vector <Int_t>> fGem; // <low, high, serial> ---> <st, mod, id>


    ClassDef(BmnLambdaMisc, 1)
};

#endif