#ifndef BMNTOF2RAW2DIGIT_H
#define	BMNTOF2RAW2DIGIT_H 

#define TOF2_MAX_CHANNEL 112

#include "TString.h"
#include "TClonesArray.h"
#include "BmnTDCDigit.h"
#include "BmnADCDigit.h"
#include "BmnSyncDigit.h"
#include <iostream>
#include "Riostream.h"
#include "BmnTof2Digit.h"
class Bmn_Tof2_map_element{
public:
   Bmn_Tof2_map_element(){
     plane=side=id,slot,chan,strip=0;
   } 
   int plane;
   char side;            
   int id,slot,chan,strip;   
};

class BmnTof2Raw2Digit{

public:
    BmnTof2Raw2Digit(TString mappingFile);
    BmnTof2Raw2Digit();
    void print();
    char *getPlaneName(int i);
    void getEventInfo(unsigned long *ev,unsigned long *t1,unsigned long *t2);
    void fillEvent(TClonesArray *data, TClonesArray *sync, TClonesArray *t0, TClonesArray *tof2digit);

private:
    int n_rec;
    Bmn_Tof2_map_element map[TOF2_MAX_CHANNEL];
    char Bmn_Tof2_names[10][4];
    unsigned long EVENT,TIME_SEC,TIME_NS;
    float T0;
    ClassDef(BmnTof2Raw2Digit, 1);
};
#endif	/* BMNTOF2RAW2DIGIT_H */


