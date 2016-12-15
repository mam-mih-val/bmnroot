
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

#define HPTIMEBIN 0.02344
#define INVHPTIMEBIN 42.6666
#define KNBDCHANNELS 40

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
    BmnTrigRaw2Digit(TString mappingFile, TString INLFile);
    BmnTrigRaw2Digit();
    ~BmnTrigRaw2Digit() {};

    vector<BmnTrigMapping> GetMap() const {
        return fMap;
    }

    BmnStatus FillEvent(TClonesArray *tdc, TClonesArray *t0, TClonesArray *bc1, TClonesArray *bc2, TClonesArray *veto, TClonesArray *fd, TClonesArray *bd, Double_t& t0time, Double_t *t0width = NULL);
    BmnStatus readINLCorrections(TString INLFile);

private:

    vector<BmnTrigMapping> fMap;
    ifstream fMapFile;
    ifstream fINLFile;
    TString fMapFileName;
    TString fINLFileName;
    Float_t fINLTable[72][1024];


    ClassDef(BmnTrigRaw2Digit, 1);
};

#endif /* BMNTRIGRAWTODIGIT_H */

