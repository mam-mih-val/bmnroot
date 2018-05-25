#ifndef BMNCSCDIGIT_H
#define	BMNCSCDIGIT_H

#include <iostream>
#include <vector>
#include "Rtypes.h"
#include "TNamed.h"

using namespace std;

class BmnCscDigit : public TNamed {
public:
    BmnCscDigit();
    BmnCscDigit(BmnCscDigit* digit);
    BmnCscDigit(Short_t iStation, Short_t iModule, Short_t iStripLayer, Int_t iStripNumber, Double_t iStripSignal);

    virtual ~BmnCscDigit();

    Short_t GetStation() { return fStation; }
    Short_t GetModule() { return fModule; }
    Short_t GetStripLayer() { return fStripLayer; }
    Int_t GetStripNumber() { return fStripNumber; }
    Double_t GetStripSignal() { return fStripSignal; }

    void SetStation(Short_t station) { fStation = station; }
    void SetModule(Short_t module) { fModule = module; }
    void SetStripLayer(Short_t layer) { fStripLayer = layer; }
    void SetStripNumber(Int_t num) { fStripNumber = num; }
    void SetStripSignal(Double_t signal) { fStripSignal = signal; }
    
    void SetIsGoodDigit(Bool_t tmp) { fIsGoodDigit = tmp; }
    Bool_t IsGoodDigit() { return fIsGoodDigit; }

private:
    Short_t fStation;
    Short_t fModule;
    Short_t fStripLayer;
    Int_t fStripNumber;
    Double_t fStripSignal;
    Bool_t fIsGoodDigit;

    ClassDef(BmnCscDigit,1);
};

#endif

