#ifndef BMNTRIGGERSCHECK_H
#define BMNTRIGGERSCHECK_H 1

#include "FairTask.h"
#include "FairRootManager.h"
#include "BmnEventHeader.h"
#include <UniDbDetectorParameter.h>
#include <UniDbRun.h>
#include <TClonesArray.h>
#include <TString.h>
#include <TMath.h>
#include "BmnEventQuality.h"

using namespace std;
using namespace TMath;

class BmnTriggersCheck : public FairTask {
public:

    BmnTriggersCheck() {
    };

    BmnTriggersCheck(Bool_t);

    virtual ~BmnTriggersCheck() {
    };

    virtual InitStatus Init();
    virtual void Exec(Option_t* opt);
    virtual void Finish();

private:
    Bool_t fIsExp;

    TString fBmnEventHeaderBranchName;
    TString fT0Branch;
    TString fVetoBranch;
    TString fBC2Branch;
    TString fBDBranch;
    
    TString fBmnEvQualityBranchName;

    TClonesArray* fBmnEventHeader;
    TClonesArray* fT0Array;
    TClonesArray* fVetoArray;
    TClonesArray* fBC2Array;
    TClonesArray* fBDArray;
    
    //TClonesArray* fBmnGemStripDigitsArray;
    TClonesArray* fBmnEvQuality;

    ClassDef(BmnTriggersCheck, 1);
};

#endif