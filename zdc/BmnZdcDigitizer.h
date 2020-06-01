/* 
 * File:   BmnZdcDigitizer.h
 * Author: Petr Alekseev
 *
 * Created on 29.05.2020, 18:00
 */

#ifndef BMNZDCDIGITIZER_H
#define BMNZDCDIGITIZER_H

#include <iostream> 
#include <fstream>

#include <FairTask.h>
#include <TClonesArray.h>

#include "BmnZDCDigit.h"

class BmnZdcDigitizer : public FairTask {
public:
    BmnZdcDigitizer();
    virtual ~BmnZdcDigitizer();
   
    void LoadMap(const char * fileName);
    
    void SetScale(Double_t val) { fScale = val; }
    void SetThreshold(Double_t val) { fThreshold = val; }
    
    virtual InitStatus Init();
    virtual void Exec(Option_t* opt);
    
private:
    
    BmnZDCDigit fModules[106];
    
    TClonesArray * fArrayOfZdcPoints; // input
    TClonesArray * fArrayOfZdcDigits; // output
    
    Double_t fScale = 1.;
    Double_t fThreshold = 0.;

    ClassDef(BmnZdcDigitizer,2);
};

#endif /* BMNZDCDIGITIZER_H */

