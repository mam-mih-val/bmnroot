#ifndef BMNGEMSTRIPHIT_H
#define	BMNGEMSTRIPHIT_H

#include "Rtypes.h"

#include "BmnHit.h"

class BmnGemStripHit : public BmnHit {
public:

    BmnGemStripHit();
    BmnGemStripHit(Int_t detID, TVector3 pos, TVector3 dpos, Int_t index);

    virtual ~BmnGemStripHit();

    void SetModule(Int_t module) {
        fModule = module;
    }

    void SetEnergyLoss(Double_t de) {
        fELoss = de;
    }
//
//    void SetType(Int_t type) {
//        fType = type;
//    }

    void SetSignalDiff(Double_t sdiff) {
        fNormSignalDiff = sdiff;
    }

    Double_t GetEnergyLoss() {
        return fELoss;
    }

    Int_t GetModule() {
        return fModule;
    }

//    Int_t GetType() {
//        return fType;
//    }

    Double_t GetSignalDiff() {
        return fNormSignalDiff;
    }

private:
    Int_t fModule;
    Double_t fELoss;
//    Int_t fType; // 0 - fake, 1 - hit, -1 - undefined
    Double_t fNormSignalDiff; //normalized signal difference between lower and upper strips (0 is min diff,..., 1 is max dif)


    ClassDef(BmnGemStripHit, 1);
};



#endif


