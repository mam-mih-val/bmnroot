
#ifndef BMNT0RAWTODIGIT_H
#define BMNT0RAWTODIGIT_H

#include "BmnEnums.h"
#include "TString.h"
#include "TClonesArray.h"
#include "BmnTDCDigit.h"
#include <iostream>
#include <fstream>
#include <map>
#include <vector>
#include "BmnT0Digit.h"
#include "TMath.h"

using namespace std;
using namespace TMath;

struct BmnT0Mapping {
    UInt_t serial;
    Short_t slot;
};

class BmnT0Raw2Digit {

public:
    BmnT0Raw2Digit(TString mappingFile);
    BmnT0Raw2Digit();
    ~BmnT0Raw2Digit() {};

    vector<BmnT0Mapping> GetMap() const {
        return fMap;
    }
    
    BmnStatus FillEvent(TClonesArray *tdc, TClonesArray *t0);
    
private:
            
    vector<BmnT0Mapping> fMap;
    ifstream fMapFile;
    TString fMapFileName;
    

    ClassDef(BmnT0Raw2Digit, 1);
};

#endif /* BMNT0RAWTODIGIT_H */

