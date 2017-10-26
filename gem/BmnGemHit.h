// BmnGemHit class

#ifndef BMNGEMHIT_H
#define BMNGEMHIT_H 1

#include <math.h>
#include <iostream>
#include <vector>
#include "FairHit.h"
#include <TObject.h>

using namespace std;

// class TClonesArray;

class BmnGemHit : public FairHit {
public:

    /** Default constructor **/
    BmnGemHit();

    /** Constructor to use **/
    BmnGemHit(Int_t detUID, TVector3 posHit, TVector3 posHitErr, Int_t pointIndx);

    Int_t GetStationNumber() const {
        return fStation;
    }

    Int_t GetXaddr() const {
        return fXaddr;
    }

    Int_t GetYaddr() const {
        return fYaddr;
    }

    ULong_t GetAddr() const {
        return fAddr;
    }

    Bool_t IsUsed() const {
        return fUsing;
    }

    void SetUsing(Bool_t use) {
        fUsing = use;
    }

    void SetXaddr(Int_t addr) {
        fXaddr = addr;
    }

    void SetYaddr(Int_t addr) {
        fYaddr = addr;
    }

    void SetAddr(ULong_t addr) {
        fAddr = addr;
    }

    Bool_t IsChanged() const {
        return fChange;
    }

    void SetChange(Bool_t ch) {
        fChange = ch;
    }

    /** Destructor **/
    virtual ~BmnGemHit();

private:
    Int_t fStation;
    Bool_t fUsing;
    Int_t fXaddr;
    Int_t fYaddr;
    ULong_t fAddr;

    Bool_t fChange; //TMP

    ClassDef(BmnGemHit, 1);

};

#endif
