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
#include <UniDbDetectorParameter.h>
#include <UniDbRun.h>
#include "BmnInnTrackerAlign.h"
#include <BmnEventQuality.h>

#include "BmnSiliconConfiguration.h"

class BmnSiliconHitMaker : public FairTask {
public:

    BmnSiliconHitMaker();
    BmnSiliconHitMaker(Int_t run_period, Int_t run_number, Bool_t isExp, TString alignFile = "default");

    virtual ~BmnSiliconHitMaker();

    virtual InitStatus Init();

    virtual void Exec(Option_t* opt);

    virtual void Finish();

    void ProcessDigits();

    void SetHitMatching(Bool_t opt = kTRUE) {
        fHitMatching = opt;
    }

    void SetCurrentConfig(BmnSiliconConfiguration::SILICON_CONFIG config) {
        fCurrentConfig = config;
    }

private:

    TString fInputPointsBranchName;
    TString fInputDigitsBranchName;
    TString fInputDigitMatchesBranchName;

    TString fOutputHitsBranchName;
    TString fOutputHitMatchesBranchName;

    /** Input array of Silicon Points **/
    TClonesArray* fBmnSiliconPointsArray;
    TClonesArray* fBmnSiliconDigitsArray;
    TClonesArray* fBmnSiliconDigitMatchesArray;

    /** Output array of Silicon Hits **/
    TClonesArray* fBmnSiliconHitsArray;

    /** Output array of Silicon Hit Matches **/
    TClonesArray* fBmnSiliconHitMatchesArray;

    Bool_t fHitMatching;

    BmnSiliconConfiguration::SILICON_CONFIG fCurrentConfig;

    BmnSiliconStationSet *StationSet; //Entire Silicon detector  

    Bool_t fIsExp;
    TString fBmnEvQualityBranchName;
    TClonesArray* fBmnEvQuality;
    
    BmnInnTrackerAlign* fAlign;

    ClassDef(BmnSiliconHitMaker, 1);
};

#endif /* BMNSILICONHITMAKER_H */

