#ifndef BMNZDCRAW2DIGIT_H
#define	BMNZDCRAW2DIGIT_H 

#include "TString.h"
#include "TClonesArray.h"
#include "BmnTDCDigit.h"
#include "BmnADCDigit.h"
#include "BmnSyncDigit.h"
#include <iostream>
#include "Riostream.h"
#include "BmnZDCDigit.h"
#include <cstdlib> 

class Bmn_ZDC_map_element{
public:
  Bmn_ZDC_map_element(){ id=chan=front_chan=size=x=y=-1; used=0;};
  void set(unsigned long v1,unsigned char v2,unsigned char v3,unsigned char v4,unsigned char v5,unsigned char v6,unsigned char v7, float v8, float v9){
    id=v1; chan=v2; front_chan=v3; size=v4; ix=v5; iy=v6; used=v7; x=v8; y=v9;
  }; 
  unsigned long id;
  unsigned char chan;
  unsigned char front_chan;
  unsigned char size;
  unsigned char ix;
  unsigned char iy;
  unsigned char used;
  float x;
  float y;
};

class BmnZDCRaw2Digit{

public:
    BmnZDCRaw2Digit(TString mappingFile);
    BmnZDCRaw2Digit();
    void print();

    void fillEvent(TClonesArray *data, TClonesArray *zdcdigit);

private:
    Bmn_ZDC_map_element zdc_map_element[256];
    int n_rec;
    ClassDef(BmnZDCRaw2Digit, 2);
};
#endif	/* BMNZDCRAW2DIGIT_H */


