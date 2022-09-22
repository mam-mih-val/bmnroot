#ifndef BMNDCHRAW2DIGIT_H
#define BMNDCHRAW2DIGIT_H 

#include "TString.h"
#include "TClonesArray.h"
#include "BmnTDCDigit.h"
#include "BmnSyncDigit.h"
#include <iostream>
#include "Riostream.h"
#include "BmnDchDigit.h"
#include "BmnEnums.h"
#include <cstdlib>
#include <bitset>
#include <map>
#include <UniDetectorParameter.h>

#define DCH_TDC_TYPE (0x10) //TDC64V

class BmnDchRaw2Digit {
public:
    BmnDchRaw2Digit(Int_t period, Int_t run);
    BmnDchRaw2Digit() {}
    ~BmnDchRaw2Digit() {
        if (!fMap1.empty()) for (int i = 0; i < fMap1.size(); i++) delete fMap1[i];
        if (!fMap2.empty()) for (int i = 0; i < fMap2.size(); i++) delete fMap2[i];
    }
    
    void FillEvent(TClonesArray *tdc, map<UInt_t, Long64_t> *ts, TClonesArray *dch, Double_t t0);

private:
    vector<UniValue*> fMap1;
    vector<UniValue*> fMap2;
    
    Int_t GetChTDC64v(UInt_t tdc, UInt_t ch);
    BmnStatus FindInMap(BmnTDCDigit* dig, TClonesArray* arr, Long64_t ts, Double_t t0, Int_t dchId);
    
    BmnStatus ReadMapFromFile(Int_t period);
    BmnStatus ReadMapFromDB(Int_t period, Int_t run);

    ClassDef(BmnDchRaw2Digit, 1);
};
#endif /* BMNDCHRAW2DIGIT_H */


