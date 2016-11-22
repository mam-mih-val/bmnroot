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
    BmnDchRaw2Digit() {};
    ~BmnDchRaw2Digit() {};
    
    void FillEvent(TClonesArray *tdc, map<UInt_t, Long64_t> *ts, TClonesArray *dch, Double_t t0);

private:
    
    DchMapStructure* fMap1;
    DchMapStructure* fMap2;
    
    Int_t fEntriesInMap; // member of entries in BD table
    
    Int_t GetChTDC64v(UInt_t tdc, UInt_t ch);
    BmnStatus FindInMap(DchMapStructure* map, BmnTDCDigit* dig, TClonesArray* arr, Long64_t ts, Double_t t0);

    ClassDef(BmnDchRaw2Digit, 1);
};
#endif /* BMNDCHRAW2DIGIT_H */


