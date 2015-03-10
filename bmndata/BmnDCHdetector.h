#ifndef BMNDCHDETECTOR_H
#define	BMNDCHDETECTOR_H 

#include "TString.h"
#include "TClonesArray.h"
#include "BmnTDCDigit.h"
#include "BmnADCDigit.h"
#include "BmnSyncDigit.h"
#include <iostream>
#include "Riostream.h"

enum {
    VA_1 = 0, VB_1 = 1, UA_1 = 2, UB_1 = 3, XA_1 = 4, XB_1 = 5, YA_1 = 6, YB_1 = 7,
    VA_2 = 8, VB_2 = 9, UA_2 = 10, UB_2 = 11, XA_2 = 12, XB_2 = 13, YA_2 = 14, YB_2 = 15
};

class Bmn_DCH_data {
public:

    Bmn_DCH_data() {
        nhit = 0;
    };

    void clear() {
        nhit = 0;
    };

    void set(unsigned short v) {
        if (nhit < 20) {
            data[nhit++] = v;
        }
    }

    int get(unsigned short **d) {
        *d = data;
        return nhit;
    }
private:
    unsigned char nhit;
    unsigned short data[20];
};

class Bmn_DCH_map_element {
public:

    Bmn_DCH_map_element() {
        id = slot = chan = 0;
    };

    void set(unsigned long v1, unsigned char v2, unsigned char v3) {
        id = v1;
        slot = v2;
        chan = v3;
    };
    unsigned long id;
    unsigned char slot;
    unsigned char chan;
};

class BmnDCHdetector {
public:
    BmnDCHdetector(TString mappingFile);

    BmnDCHdetector() {
    };

    void clear();
    int set(unsigned long id, unsigned char slot, unsigned char ch, int val);
    void print();
    char *getPlaneName(int i);
    int get(int p, int ch, unsigned short **d);
    int getnhits(int plane);
    void fillEvent(TClonesArray *data, TClonesArray *sync, TClonesArray *t0);
    void fillEvent(TClonesArray *data);
    float get_t0();
private:
    char Bmn_DCH_names[16][5];

    Bmn_DCH_map_element dhc_map_element[16][16][16];
    Bmn_DCH_data data[16][16][16];
    int hits[16];
    int rel[21][64];
    float T0;
    ClassDef(BmnDCHdetector, 1);
};
#endif	/* BMNDCHDETECTOR_H */


