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

class Bmn_ZDC_map_element{
public:
  Bmn_ZDC_map_element(){ id=chan=front_chan=size=x=y=-1; used=0;};
  void set(unsigned long v1,unsigned char v2,unsigned char v3,unsigned char v4,unsigned char v5,unsigned char v6,unsigned char v7){
    id=v1; chan=v2; front_chan=v3; size=v4; x=v5; y=v6; used=v7;
  }; 
  unsigned long id;
  unsigned char chan;
  unsigned char front_chan;
  unsigned char size;
  unsigned char x;
  unsigned char y;
  unsigned char used;
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
    ClassDef(BmnZDCRaw2Digit, 1);
};
#endif	/* BMNZDCRAW2DIGIT_H */


