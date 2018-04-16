#ifndef BMNEVENTQUALITY_H
#define BMNEVENTQUALITY_H 1

#include <iostream>
#include <TNamed.h>

using namespace std;

class BmnEventQuality : public TNamed {
public:

    BmnEventQuality() {};
    BmnEventQuality(TString str) {
        fIsGoodEvent = (str == "GOOD") ? kTRUE : (str == "BAD") ? kFALSE : throw;
    }

    virtual ~BmnEventQuality() {
    };
    
    void SetIsGoodEvent(TString str) {
        fIsGoodEvent = (str == "GOOD") ? kTRUE : (str == "BAD") ? kFALSE : throw;
    }
    
    Bool_t GetIsGoodEvent() {
        return fIsGoodEvent;
    }

private:
    Bool_t fIsGoodEvent;
    
    ClassDef(BmnEventQuality, 1);
};

#endif
