#ifndef BMNDATATRIGGERINFO_H
#define BMNDATATRIGGERINFO_H 1

#include <iostream>
#include <TNamed.h>

#include <ElogRecord.h>
#include <ElogTrigger.h>

#include <map>

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
