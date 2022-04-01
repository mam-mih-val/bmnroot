/* 
 * File:   BmnFHcalDigitizer.h
 * Author: Sergey Morozov
 *
 * Created on 16.09.2021, 12:00
 */

#ifndef BMNFHCALDIGITIZER_H
#define BMNFHCALDIGITIZER_H

#include <iostream>
#include <fstream>

#include <FairTask.h>
#include <TClonesArray.h>

#include "BmnFHCalDigit.h"

#include "TRandom3.h"

class BmnFHCalDigitizer : public FairTask {
public:
    BmnFHCalDigitizer();
    virtual ~BmnFHCalDigitizer();

    void SetScale(Double_t val) { fScale = val; }
    void SetThreshold(Double_t val) { fSmallModThreshold = fLargeModThreshold = val; }
    void SetThreshold(Double_t small, Double_t lagre) {
        fSmallModThreshold = small;
        fLargeModThreshold = lagre;
    }

    void SetGeV2MIP(Double_t val) { fGeV2MIP = val; }
    void SetMIP2Pix(Double_t val) { fMIP2Pix = val; }
    void SetMIPNoise(Double_t val) { fMIPNoise = val; }

    virtual InitStatus Init();
    virtual void Exec(Option_t* opt);

private:

    TClonesArray * fArrayOfFHCalPoints; // input
    TClonesArray * fArrayOfFHCalDigits; // output

    Double_t fScale = 1.;
    Double_t fSmallModThreshold = 0.;
    Double_t fLargeModThreshold = 0.;

    Double_t fGeV2MIP;
    Double_t fMIP2Pix;
    Double_t fMIPNoise;

    ClassDef(BmnFHCalDigitizer,1);
};

#endif /* BMNFHCALDIGITIZER_H */

