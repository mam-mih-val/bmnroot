/* 
 * File:   BmnCounterTask.h
 * Author: Sergei Merts
 *
 * Created on February 17, 2017, 11:33 AM
 */

#include "TString.h"
#include "FairTask.h"
#include "BmnEventHeader.h"
#include "TClonesArray.h"

#ifndef BMNCOUNTERTASK_H
#define BMNCOUNTERTASK_H

using namespace std;
using namespace TMath;

class BmnCounter : public FairTask {
private:
    BmnEventHeader* fEvHead;
    Long64_t fNEvents;
    Long64_t fIEvent;
    Int_t fRunId;

public:

    BmnCounter();
    BmnCounter(Long64_t nEv);
    virtual ~BmnCounter();

    virtual InitStatus Init();
    virtual void Exec(Option_t* opt);
    virtual void Finish();

    ClassDef(BmnCounter, 1)
};

#endif /* BMNCOUNTERTASK_H */

