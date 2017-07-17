#ifndef BMNSILICONHITMAKER_H
#define BMNSILICONHITMAKER_H

#include "Rtypes.h"

#include "FairTask.h"
#include "FairMCPoint.h"

#include "BmnSiliconDigit.h"
#include "BmnSiliconHit.h"
#include "BmnSiliconStationSet.h"
#include "BmnSiliconStation.h"
#include "BmnSiliconModule.h"
#include "BmnSiliconLayer.h"

class BmnSiliconHitMaker : public FairTask {

public:

    BmnSiliconHitMaker();

    virtual ~BmnSiliconHitMaker();

    virtual InitStatus Init();

    virtual void Exec(Option_t* opt);

    virtual void Finish();

    void ProcessDigits();

private:

    TString fInputPointsBranchName;
    TString fInputDigitsBranchName;

    TString fOutputHitsBranchName;

    /** Input array of Silicon Points **/
    TClonesArray* fBmnSiliconPointsArray;
    TClonesArray* fBmnSiliconDigitsArray;

    /** Output array of Silicon Hits **/
    TClonesArray* fBmnSiliconHitsArray;

    BmnSiliconStationSet *StationSet; //Entire Silicon detector

    ClassDef(BmnSiliconHitMaker,1);
};

#endif /* BMNSILICONHITMAKER_H */

