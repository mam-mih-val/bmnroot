#include <iostream>
#include <TNamed.h>

#include <ElogDbRecord.h>
#include <ElogDbTrigger.h>

#include <map>

#ifndef BMNDATATRIGGERINFO_H
#define BMNDATATRIGGERINFO_H 1

using namespace std;

class BmnDataTriggerInfo : public TNamed {
public:
    
    BmnDataTriggerInfo();
    
    TString GetTrigger(Int_t);
    
    virtual ~BmnDataTriggerInfo() {;}
    
private: 
    Int_t fPeriod;
    map <TString, TString> fTriggerMap;
    
    
    ClassDef(BmnDataTriggerInfo, 1)
};

#endif