// BmnDchHit class

#ifndef BmnDchHit_H
#define BmnDchHit_H 1

#include <math.h>
#include <iostream>
#include "FairHit.h"
#include <TObject.h>

using namespace std;

// class TClonesArray;

class BmnDchHit : public FairHit {
public:

    /** Default constructor **/
    BmnDchHit();

    /** Constructor to use **/
    BmnDchHit(Int_t detUID, TVector3 posHit, TVector3 posHitErr, Int_t pointIndex);

    Bool_t IsUsed() const {
        return fUsing;
    }
    
    void SetUsing(Bool_t use) {
        fUsing = use;
    }

    /** Destructor **/
    virtual ~BmnDchHit();

private:
    
    Bool_t fUsing;

    ClassDef(BmnDchHit, 1);

};

#endif