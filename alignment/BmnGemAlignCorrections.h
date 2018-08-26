#ifndef BMNGEMALIGNCORRECTIONS_H
#define BMNGEMALIGNCORRECTIONS_H 1

#include <iostream>
#include <TNamed.h>
#include <TVector3.h>
#include "BmnAlignCorrections.h"

using namespace std;

class BmnGemAlignCorrections : public BmnAlignCorrections {
public:

    BmnGemAlignCorrections();

    BmnGemAlignCorrections(Int_t, Int_t, TVector3);
    
    virtual ~BmnGemAlignCorrections() { };

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
   
    ClassDef(BmnGemAlignCorrections, 1)
};

#endif
