/*
 * File:   BmnHodoDigitizer.h
 * Author: Sergey Morozov
 *
 * Created on 16.09.2021, 12:00
 */

#ifndef BMNHODODIGITIZER_H
#define BMNHODODIGITIZER_H

#include <iostream>
#include <fstream>

#include <FairTask.h>
#include <TClonesArray.h>

#include "BmnHodoDigit.h"

#include "TRandom3.h"

class BmnHodoDigitizer : public FairTask {
public:
    BmnHodoDigitizer();
    virtual ~BmnHodoDigitizer();

    void SetScale(Double_t val) { fScale = val; }
    void SetThreshold(Double_t val) { fThreshold = val; }

    void SetGeV2MIP(Double_t val) { fGeV2MIP = val; }
    void SetMIP2Pix(Double_t val) { fMIP2Pix = val; }
    void SetMIPNoise(Double_t val) { fMIPNoise = val; }

    virtual InitStatus Init();
    virtual void Exec(Option_t* opt);

private:

    TClonesArray * fArrayOfHodoPoints; // input
    TClonesArray * fArrayOfHodoDigits; // output

    Double_t fScale = 1.;
    Double_t fThreshold = 0.;

    Double_t fGeV2MIP;
    Double_t fMIP2Pix;
    Double_t fMIPNoise;

    ClassDef(BmnHodoDigitizer,1);
};

#endif /* BMNHODODIGITIZER_H */

