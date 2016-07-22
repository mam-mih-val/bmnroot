#ifndef BMNDCHRAW2DIGIT_H
#define BMNDCHRAW2DIGIT_H 

#include "TString.h"
#include "TClonesArray.h"
#include "BmnTDCDigit.h"
#include "BmnADCDigit.h"
#include "BmnSyncDigit.h"
#include <iostream>
#include "Riostream.h"
#include "BmnDchDigit.h"
#include <cstdlib>
#include <bitset>

enum {
    VA_1 = 0, VB_1 = 1, UA_1 = 2, UB_1 = 3, XA_1 = 4, XB_1 = 5, YA_1 = 6, YB_1 = 7,
    VA_2 = 8, VB_2 = 9, UA_2 = 10, UB_2 = 11, XA_2 = 12, XB_2 = 13, YA_2 = 14, YB_2 = 15
};

class Bmn_DCH_data {
public:

    Bmn_DCH_data() {
        ndigit = 0;
    };

    void clear() {
        ndigit = 0;
    };

    void set(UShort_t v) {
        if (ndigit < 20) {
            data[ndigit++] = v;
        }
    }

    int get(UShort_t **d) {
        *d = data;
        return ndigit;
    }
private:
    UChar_t ndigit;
    UShort_t data[20];
};

class Bmn_DCH_map_element {
public:

    Bmn_DCH_map_element() {
        id = slot = chan = 0;
    };

    void set(UInt_t v1, UChar_t v2, UChar_t v3) {
        id = v1;
        slot = v2;
        chan = v3;
    };
    UInt_t id;
    UChar_t slot;
    UChar_t chan;
};

class BmnDchRaw2Digit {
public:
    BmnDchRaw2Digit(TString mappingFile);

    BmnDchRaw2Digit() {};

    Int_t GetNDigit(Int_t plane) {
        return digits[plane & 0xF];
    }

    Int_t GetData(Int_t p, Int_t ch, UShort_t **d) {
        return data[p & 0xF][(ch / 16) & 0xF][ch % 16].get(d);
    }

    Float_t Get_t0() {
        return T0;
    }

    void Clear();
    Int_t Set(ULong_t id, UChar_t slot, UChar_t ch, Int_t val);
    void Print();
    Char_t *GetPlaneName(Int_t i);
    void GetEventInfo(Long64_t *ev, Long64_t *t1, Long64_t *t2);
//    void FillEvent(TClonesArray *data);
//    void FillEvent(TClonesArray *data, TClonesArray *sync, TClonesArray *t0);
//    void FillEvent(TClonesArray *data, TClonesArray *sync, TClonesArray *t0, TClonesArray *dhcdigit);
    void FillEvent(TClonesArray *tdc, TClonesArray *sync, TClonesArray *dch);

private:

    Char_t Bmn_DCH_names[16][5];
    Bmn_DCH_map_element dhc_map_element[16][16][16];
    Bmn_DCH_data data[16][16][16];
    Int_t digits[16];
    Int_t rel[2][21][64]; 
    Float_t T0;
    Long64_t EVENT, TIME_SEC, TIME_NS;
    TString fMapFileName;
    ifstream fMapFile;
    UInt_t fType;
    
    Int_t GetChTDC64v(UInt_t tdc, UInt_t ch);

    ClassDef(BmnDchRaw2Digit, 1);
};
#endif /* BMNDCHRAW2DIGIT_H */


