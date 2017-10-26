#ifndef BMNDCHALIGNCORRECTIONS_H
#define BMNDCHALIGNCORRECTIONS_H 1

#include <iostream>
#include <TNamed.h>
#include <TVector3.h>
#include "BmnAlignCorrections.h"

using namespace std;

class BmnDchAlignCorrections : public BmnAlignCorrections {
public:

    BmnDchAlignCorrections() {
    };

    BmnDchAlignCorrections(Int_t, TVector3);
    
    virtual ~BmnDchAlignCorrections() { };

    void SetDchId(Int_t num) {
        fDchId = num;
    }
    
    Int_t GetDchId() {
        return fDchId;
    }

private:
    Int_t fDchId;
   
    ClassDef(BmnDchAlignCorrections, 1)
};

#endif
