/**
 * Class BmnStripDigit
 * Base class for strip detector digits
 */

#ifndef BMNSTRIPDIGIT_H
#define BMNSTRIPDIGIT_H

#include <iostream>
#include "TNamed.h"

using namespace std;

class BmnStripDigit : public TNamed {
public:
    BmnStripDigit();
    BmnStripDigit(BmnStripDigit* digit);
    BmnStripDigit(Int_t iStation, Int_t iModule, Int_t iStripLayer, Int_t iStripNumber, Double_t iStripSignal);
    virtual ~BmnStripDigit();
    
    Int_t GetStation() { return fStation; }
    Int_t GetModule() { return fModule; }
    Int_t GetStripLayer() { return fStripLayer; }
    Int_t GetStripNumber() { return fStripNumber; }
    Double_t GetStripSignal() { return fStripSignal; }
    Bool_t IsGoodDigit() { return fIsGoodDigit; }

    void SetStation(Int_t station) { fStation = station; }
    void SetModule(Int_t module) { fModule = module; }
    void SetStripLayer(Int_t layer) { fStripLayer = layer; }
    void SetStripNumber(Int_t num) { fStripNumber = num; }
    void SetStripSignal(Double_t signal) { fStripSignal = signal; }
    void SetIsGoodDigit(Bool_t tmp) { fIsGoodDigit = tmp; }
    
    void Print() {
        cout << "Digi info# " << endl;
        cout << "Stat# " << std::dec << fStation << endl;
        cout << "Mod# " << std::dec << fModule << endl;
        cout << "Layer# " << std::dec << fStripLayer << endl;
        cout << "Strip# " << std::dec << fStripNumber << endl;
        cout << "Signal# " << std::dec << fStripSignal << endl;
    }

protected:
    Int_t fStation;
    Int_t fModule;
    Int_t fStripLayer;
    Int_t fStripNumber;
    Double_t fStripSignal;
    Bool_t fIsGoodDigit;

    ClassDef(BmnStripDigit,1);

};

#endif /* BMNSTRIPDIGIT_H */

