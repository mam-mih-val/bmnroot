#ifndef BMNECALRAW2DIGIT_H
#define	BMNECALRAW2DIGIT_H 

#include <cstdlib> 
#include <iostream>

#include <TGraph.h>

#include "TString.h"
#include "TClonesArray.h"
#include "BmnADCDigit.h"
#include "BmnECALDigit.h"

#define MAX_ECAL_CHANNELS 1024

class BmnECALRaw2Digit{

public:
        
    BmnECALRaw2Digit() {}
    BmnECALRaw2Digit(ULong_t runId);
    BmnECALRaw2Digit(const char * map_file_name, const char * calibr_file_name, const char * geo_file_name);
    void print();

    Bool_t LoadAdcMap(const char * map_file_name);
    Bool_t LoadCalibration(const char * calibr_file_name);
    Bool_t LoadGeometry(const char * geo_file_name);
    
    void fillEvent(TClonesArray *data, TClonesArray *ecaldigit);

private:
    BmnECALMapElement fECALMapArray[MAX_ECAL_CHANNELS+1];
    
    struct {
        Int_t sample0;
        Int_t samples;
        Float_t min;
        Float_t max;
        void Print() { printf("sample0=%d, samples=%d, min=%g, max=%g\n",sample0,samples,min,max); }
    } fPedestal = {0,8,-30.,30.}, fSignal = {15,20,-5e4,-50.};
            
    Bool_t UseNovosibirskFit(BmnADCDigit* digit, Int_t chan, TClonesArray *ecaldigit);
    
    static Double_t Novosibirsk(Double_t* x, Double_t* p);
    
    ClassDef(BmnECALRaw2Digit, 3);
};
#endif	/* BMNECALRAW2DIGIT_H */
