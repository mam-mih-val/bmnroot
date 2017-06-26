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
#include <UniDbDetectorParameter.h>

#define DCH_TDC_TYPE (0x10) //TDC64V

class BmnDchRaw2Digit {
public:
    BmnDchRaw2Digit(Int_t period, Int_t run);
    BmnDchRaw2Digit() {
        fMap1 = NULL;
        fMap2 = NULL;
    };
    ~BmnDchRaw2Digit() {
        if (fMap1) delete[] fMap1;
        if (fMap2) delete[] fMap2;
    };
    
    void FillEvent(TClonesArray *tdc, map<UInt_t, Long64_t> *ts, TClonesArray *dch, Double_t t0);

private:
    
    DchMapStructure* fMap1;
    DchMapStructure* fMap2;
    
    Int_t fEntriesInMap1; // member of entries in BD table for DCH1
    Int_t fEntriesInMap2; // member of entries in BD table for DCH2
    
    Int_t GetChTDC64v(UInt_t tdc, UInt_t ch);
    BmnStatus FindInMap(BmnTDCDigit* dig, TClonesArray* arr, Long64_t ts, Double_t t0, Int_t dchId);

    ClassDef(BmnDchRaw2Digit, 1);
};
#endif /* BMNDCHRAW2DIGIT_H */


