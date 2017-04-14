
#ifndef BMNMWPCRAW2DIGIT_H
#define BMNMWPCRAW2DIGIT_H

#include "TString.h"
#include "TClonesArray.h"
#include "BmnHRBDigit.h"
#include "BmnSyncDigit.h"
#include <iostream>
#include "Riostream.h"
#include "BmnMwpcDigit.h"
#include "BmnEnums.h"
#include <cstdlib>
#include <bitset>
#include <map>
#include <UniDbDetectorParameter.h>

#define N_WIRES_MWPC 96

class BmnMwpcRaw2Digit {

private:
    
    TString fMapFileName;
    ifstream fMapFile;
    map<UInt_t, UInt_t> mapping;

public:
    BmnMwpcRaw2Digit(TString mapName);
    BmnMwpcRaw2Digit() {};
    ~BmnMwpcRaw2Digit() {};
    
    void SetMapFileName(TString name) {
        fMapFileName = name;
    };
    void FillEvent(TClonesArray *hrb, TClonesArray *mwpc);

    ClassDef(BmnMwpcRaw2Digit, 1);
};

#endif /* BMNMWPCRAW2DIGIT_H */

