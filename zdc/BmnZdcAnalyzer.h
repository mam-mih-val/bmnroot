/* 
 * File:   BmnZdcAnalyzer.h
 * Author: pnaleks
 *
 * Created on 5 июня 2020 г., 18:58
 */

#ifndef BMNZDCANALYZER_H
#define BMNZDCANALYZER_H

#include <FairTask.h>
#include <TClonesArray.h>

#include "BmnZDCEventData.h"

class BmnZdcAnalyzer : public FairTask {
public:
    BmnZdcAnalyzer();
    virtual ~BmnZdcAnalyzer();

    virtual InitStatus Init();
    virtual void Exec(Option_t* opt);

private:

    TClonesArray * fArrayOfZdcDigits; // input
    BmnZDCEventData * fBmnZDCEventData; // output

    //ClassDef(BmnZdcAnalyzer,1);
};

#endif /* BMNZDCANALYZER_H */

