#ifndef BMNSILICONDIGIT_H
#define BMNSILICONDIGIT_H

#include <iostream>
#include <vector>
#include "Rtypes.h"
#include "TNamed.h"

using namespace std;

class BmnSiliconDigit : public TNamed {
private:
    Int_t fStation;
    Int_t fModule;
    Int_t fLayer; // 0 - lower layer, 1 - upper layer
    Int_t fStrip;
    Double_t fSignal;
    Bool_t fIsGoodDigit;
    
public:
    BmnSiliconDigit();
    BmnSiliconDigit(BmnSiliconDigit* digit);
    BmnSiliconDigit(Int_t iStation, Int_t iModule, Int_t iLayer, Int_t iStrip, Double_t iSignal);

    virtual ~BmnSiliconDigit();

    Int_t GetStation() {
        return fStation;
    }

    Int_t GetModule() {
        return fModule;
    }

    Int_t GetStripLayer() {
        return fLayer;
    }

    Int_t GetStripNumber() {
        return fStrip;
    }

    Double_t GetStripSignal() {
        return fSignal;
    }

    void SetStation(Int_t station) {
        fStation = station;
    }

    void SetModule(Int_t module) {
        fModule = module;
    }

    void SetStripLayer(Int_t layer) {
        fLayer = layer;
    }

    void SetStripNumber(Int_t num) {
        fStrip = num;
    }

    void SetStripSignal(Double_t signal) {
        fSignal = signal;
    }
        
    void SetIsGoodDigit(Bool_t tmp) { fIsGoodDigit = tmp; }
    Bool_t IsGoodDigit() { return fIsGoodDigit; }

    ClassDef(BmnSiliconDigit, 1);
};

#endif /* BMNSILICONDIGIT_H */

