
#ifndef BMNTRIGRAWTODIGIT_H
#define BMNTRIGRAWTODIGIT_H

#include "BmnEnums.h"
#include "TString.h"
#include "TClonesArray.h"
#include "BmnTDCDigit.h"
#include <iostream>
#include <fstream>
#include <map>
#include <vector>
#include "BmnTrigDigit.h"
#include "TMath.h"

using namespace std;
using namespace TMath;

struct BmnTrigMapping {
    TString name;
    UInt_t serial;
    Short_t slot;
    Short_t channel;
};

class BmnTrigRaw2Digit {

public:
    BmnTrigRaw2Digit(TString mappingFile);
    BmnTrigRaw2Digit();
    ~BmnTrigRaw2Digit() {};

    vector<BmnTrigMapping> GetMap() const {
        return fMap;
    }
    
    BmnStatus FillEvent(TClonesArray *tdc, TClonesArray *t0, TClonesArray *bc1, TClonesArray *bc2, TClonesArray *veto);
    
private:
            
    vector<BmnTrigMapping> fMap;
    ifstream fMapFile;
    TString fMapFileName;
    

    ClassDef(BmnTrigRaw2Digit, 1);
};

#endif /* BMNTRIGRAWTODIGIT_H */

