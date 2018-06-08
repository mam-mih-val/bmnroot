#ifndef BMNSILICONDIGITIZER_H
#define BMNSILICONDIGITIZER_H

#include <iostream>

#include "Rtypes.h"
#include "TClonesArray.h"

#include "FairTask.h"
#include "FairMCPoint.h"

#include "BmnSiliconDigit.h"
#include "BmnSiliconStationSet.h"

#include "BmnSiliconConfiguration.h"

using namespace std;

class BmnSiliconDigitizer : public FairTask {
public:

    BmnSiliconDigitizer();

    virtual ~BmnSiliconDigitizer();

    virtual InitStatus Init();

    virtual void Exec(Option_t* opt);

    virtual void Finish();

    void ProcessMCPoints();

    //Setters

    void SetOnlyPrimary(Bool_t opt = kFALSE) {
        fOnlyPrimary = opt;
    }

    void SetStripMatching(Bool_t opt = kTRUE) {
        fStripMatching = opt;
    }

    void SetCurrentConfig(BmnSiliconConfiguration::SILICON_CONFIG config) {
        fCurrentConfig = config;
    }

private:

    TString fInputBranchName;
    TString fOutputDigitsBranchName;
    TString fOutputDigitMatchesBranchName;

    /** Input array of Silicon Points **/
    TClonesArray* fBmnSiliconPointsArray;

    /** Input array of MC Tracks **/
    TClonesArray* fMCTracksArray;

    /** Output array of Silicon Digits **/
    TClonesArray* fBmnSiliconDigitsArray;

    /** Output array of Silicon Digit Matches **/
    TClonesArray* fBmnSiliconDigitMatchesArray;

    Bool_t fOnlyPrimary;
    Bool_t fStripMatching;

    BmnSiliconConfiguration::SILICON_CONFIG fCurrentConfig;

    BmnSiliconStationSet *StationSet; //Entire Silicon detector

    ClassDef(BmnSiliconDigitizer,1);
};

#endif /* BMNSILICONDIGITIZER_H */

