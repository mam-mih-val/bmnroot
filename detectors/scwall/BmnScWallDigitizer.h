/*
 * File:   BmnScWallDigitizer.h
 * Author: Sergey Morozov
 *
 * Created on 16.09.2021, 12:00
 */

#ifndef BMNSCWALLDIGITIZER_H
#define BMNSCWALLDIGITIZER_H

#include <iostream>
#include <fstream>

#include <FairTask.h>
#include <TClonesArray.h>

#include "BmnScWallDigit.h"

#include "TRandom3.h"

class BmnScWallDigitizer : public FairTask {
public:
    BmnScWallDigitizer();
    virtual ~BmnScWallDigitizer();

    void SetScale(Double_t val) { fScale = val; }
    void SetThreshold(Double_t val) { fSmallModThreshold = fLargeModThreshold = val; }
    void SetThreshold(Double_t small, Double_t large) {
        fSmallModThreshold = small;
        fLargeModThreshold = large;
    }

    void SetGeV2MIP(Double_t val) { fGeV2MIP = val; }
    void SetMIP2Pix(Double_t val) { fMIP2Pix = val; }
    void SetMIPNoise(Double_t val) { fMIPNoise = val; }

    virtual InitStatus Init();
    virtual void Exec(Option_t* opt);

private:

    TClonesArray * fArrayOfScWallPoints; // input
    TClonesArray * fArrayOfScWallDigits; // output

    Double_t fScale = 1.;
    Double_t fSmallModThreshold = 0.;
    Double_t fLargeModThreshold = 0.;

    Double_t fGeV2MIP;
    Double_t fMIP2Pix;
    Double_t fMIPNoise;

    ClassDef(BmnScWallDigitizer,1);
};

#endif /* BMNSCWALLDIGITIZER_H */

