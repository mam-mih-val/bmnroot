/* 
 * File:   BmnZDCEventData.cxx
 * Author: pnaleks
 * 
 * Created on 5 июня 2020 г., 13:43
 */

#include "BmnZDCEventData.h"

#include <iostream>
using namespace std;

BmnZDCEventData::BmnZDCEventData() {
    fE = fEc = fEn = fEp = 0.;
    fH = fHc = fHn = fHp = 0;
    fX = fY = 0.;
    fAsymmetry = fMoment = 0.;
}

BmnZDCEventData::~BmnZDCEventData() {
}

void BmnZDCEventData::Set(TClonesArray * pArrayOfBmnZDCDigits, Float_t * pModuleScale, Float_t * pModuleThreshold) {
    fE = fEc = fEn = fEp = 0.;
    fH = fHc = fHn = fHp = 0;
    fX = fY = 0.;
    fAsymmetry = fMoment = 0.;
    
    Double_t SEx = 0, SEy = 0, SExx = 0, SEyy = 0;
    Int_t N = pArrayOfBmnZDCDigits->GetEntries();
    for (Int_t i = 0; i < N; i++) {
        BmnZDCDigit * p = (BmnZDCDigit *) pArrayOfBmnZDCDigits->At(i);
        
        Int_t ch = p->GetChannel();
        if (ch < 1 || ch > 104) continue;
        
        Float_t e = p->GetAmp() * 1e-3; // MeV -> GeV
        if (pModuleScale) e *= pModuleScale[ch];
        
        if (pModuleThreshold && e < pModuleThreshold[ch]) continue;
        
        fH++;
        fE += e;
        
        switch (ch) {
            case  3: case  4: case  5:
            case 10: case 11: case 12:
            case 17: case 18: case 19:
            case 24: case 25: case 26:
                fHn++;
                fEn += e;
                break;
            case 43: case 44: case 45:
            case 50: case 51: case 52:
            case 57: case 58: case 59:
            case 64: case 65: case 66:
                fHp++;
                fEp += e;
                break;
            default:
                if (ch > 68) {
                    fHc++;
                    fEc += e;
                }
        }
        
        SEx += e * p->GetX();
        SEy += e * p->GetY();
        SExx += e * p->GetX() * p->GetX();
        SEyy += e * p->GetY() * p->GetY();
    }
    
    if (fE > 0.) {
        fX = SEx / fE / 10.; // mm -> cm
        fY = SEy / fE / 10.; // mm -> cm
        fMoment = (SExx + SEyy) / fE / 100.; // mm^2 -> cm^2
        fAsymmetry = 2.*fEc/fE - 1.;
    }
}

Float_t BmnZDCEventData::GetEnergy(char op) {
    switch (op) {
        case 'c': case 'C': return fEc;
        case 'n': case 'N': return fEn;
        case 'p': case 'P': return fEp;
    }
    
    return fE;
}

Int_t BmnZDCEventData::GetNHits(char op) {
    switch (op) {
        case 'c': case 'C': return fHc;
        case 'n': case 'N': return fHn;
        case 'p': case 'P': return fHp;
    }
    
    return fH;
}

void BmnZDCEventData::Print(Option_t *option) const {
    cout << "BmnZDCEventData:"; 
    if (fH) {
        cout << endl;
        cout << "  Hits:   " << fH << endl;
        cout << "  Energy: " << fE << " GeV" << endl;
        cout << "  Asym.:  " << fAsymmetry << endl;
        cout << "  Moment: " << fMoment << " cm^2" << endl;
    } else {
        cout << " is empty" << endl;
    }
}

ClassImp(BmnZDCEventData)

