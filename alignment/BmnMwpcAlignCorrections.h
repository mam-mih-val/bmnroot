#ifndef BMNMWPCALIGNCORRECTIONS_H
#define BMNMWPCALIGNCORRECTIONS_H 1

#include <iostream>
#include <TNamed.h>
#include <TVector3.h>
#include "BmnAlignCorrections.h"

using namespace std;

class BmnMwpcAlignCorrections : public BmnAlignCorrections {
public:

    BmnMwpcAlignCorrections() {};

    BmnMwpcAlignCorrections(Int_t, TVector3);
    
    virtual ~BmnMwpcAlignCorrections() { };

    void SetMwpcId(Int_t num) {
        fMwpcId = num;
    }
    
    Int_t GetMwpcId() {
        return fMwpcId;
    }

private:
    Int_t fMwpcId;
   
    ClassDef(BmnMwpcAlignCorrections, 1)
};

#endif
