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

    Double_t GetEnergyLoss() {
        return fELoss;
    }

    Int_t GetModule() {
        return fModule;
    }

private:
    Int_t fModule;
    Double_t fELoss;

    ClassDef(BmnGemStripHit, 1);
};



#endif


