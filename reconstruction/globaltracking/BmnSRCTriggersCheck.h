#ifndef BMNSRCTRIGGERSCHECK_H
#define BMNSRCTRIGGERSCHECK_H 1

#include "FairTask.h"
#include "FairRootManager.h"
#include "BmnEventHeader.h"
#include <UniDetectorParameter.h>
#include <UniRun.h>
#include <TClonesArray.h>
#include <TString.h>
#include <TMath.h>
#include "BmnEventQuality.h"

using namespace std;
using namespace TMath;

class BmnSRCTriggersCheck : public FairTask {
public:

    BmnSRCTriggersCheck() {
    };

    BmnSRCTriggersCheck(Bool_t);

    virtual ~BmnSRCTriggersCheck() {
    };

    virtual InitStatus Init();
    virtual void Exec(Option_t* opt);
    virtual void Finish();

private:
    Bool_t fIsExp;

    TString fBmnEventHeaderBranchName;
    TString fT0Branch;
    TString fVetoBranch;
    TString fBC1Branch;
    TString fBC2Branch;
    TString fBC3Branch;
    TString fX1RBranch;
    TString fX2RBranch;
    TString fX1LBranch;
    TString fX2LBranch;
    TString fY1RBranch;
    TString fY2RBranch;
    TString fY1LBranch;
    TString fY2LBranch;
    TString fBDBranch;
    
    TString fBmnEvQualityBranchName;

    TClonesArray* fBmnEventHeader;
    TClonesArray* fT0Array;
    TClonesArray* fVetoArray;
    TClonesArray* fBC1Array;
    TClonesArray* fBC2Array;
    TClonesArray* fBC3Array;
    TClonesArray* fY1RArray;
    TClonesArray* fY1LArray;
    TClonesArray* fY2RArray;
    TClonesArray* fY2LArray;
    TClonesArray* fX1RArray;
    TClonesArray* fX1LArray;
    TClonesArray* fX2RArray;
    TClonesArray* fX2LArray;
    TClonesArray* fBDArray;
    
    //TClonesArray* fBmnGemStripDigitsArray;
    TClonesArray* fBmnEvQuality;

    ClassDef(BmnSRCTriggersCheck, 1);
};

#endif
