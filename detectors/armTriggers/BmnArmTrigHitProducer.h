#ifndef  BmnArmTrigHitProducer_H
#define  BmnArmTrigHitProducer_H

#include <iostream>

#include "Rtypes.h"
#include "TClonesArray.h"

#include "FairTask.h"
#include "FairMCPoint.h"
#include "FairRunSim.h"
#include "BmnArmTrigHit.h"
//#include "BmnInnTrackerAlign.h"

using namespace std;

class BmnArmTrigHitProducer : public FairTask
{
  public:
    BmnArmTrigHitProducer();
    virtual ~ BmnArmTrigHitProducer();

    virtual InitStatus Init();
    virtual void Exec(Option_t* opt);
    virtual void Finish();

    //void ProcessMCPoints();

private:
    TString fInputBranchName;
    TString fOutputHitsBranchName;
    TString fOutputHitMatchesBranchName;

    /** Input array of ArmTrig Points **/
    TClonesArray* fBmnArmTrigPointsArray;   //!

    /** Output array of ArmTrig Hits **/
    TClonesArray* fBmnArmTrigHitsArray;     //!
    
    Bool_t fOnlyPrimary;
    Bool_t fUseRealEffects;

  ClassDef(BmnArmTrigHitProducer,1);
};

#endif /*  BmnArmTrigHitProducer_H */
