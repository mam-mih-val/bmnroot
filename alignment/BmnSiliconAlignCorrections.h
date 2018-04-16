#ifndef BMNSILICONALIGNCORRECTIONS_H
#define BMNSILICONALIGNCORRECTIONS_H 1

#include <iostream>
#include <TNamed.h>
#include <TVector3.h>
#include "BmnAlignCorrections.h"

using namespace std;

class BmnSiliconAlignCorrections : public BmnAlignCorrections {
public:

    BmnSiliconAlignCorrections() {
    };

    BmnSiliconAlignCorrections(TVector3);
    BmnSiliconAlignCorrections(Int_t, Int_t, TVector3);
    
    virtual ~BmnSiliconAlignCorrections() { };
    
    void SetStation(Int_t num) {
        fStation = num;
    }

    void SetModule(Int_t num) {
        fModule = num;
    }
    
    Int_t GetStation() {
        return fStation;
    }
    
    Int_t GetModule() {
        return fModule;
    }

private:
    Int_t fStation;
    Int_t fModule;
  
    ClassDef(BmnSiliconAlignCorrections, 1)
};

#endif
