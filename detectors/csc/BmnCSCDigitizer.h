#ifndef BMNCSCDIGITIZER_H
#define BMNCSCDIGITIZER_H 1

#include <iostream>

#include "Rtypes.h"
#include "TClonesArray.h"

#include "FairTask.h"
#include "FairMCPoint.h"

#include "BmnCSCDigit.h"
#include "BmnCSCHit.h"
#include "BmnCSCStationSet.h"
#include "BmnCSCConfiguration.h"
#include "BmnCSCTransform.h"

using namespace std;

class BmnCSCDigitizer : public FairTask {
public:

    BmnCSCDigitizer();

    virtual ~BmnCSCDigitizer();

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

    void SetCurrentConfig(BmnCSCConfiguration::CSC_CONFIG config) {
        fCurrentConfig = config;
    }

private:

    TString fInputBranchName;
    TString fOutputDigitsBranchName;
    TString fOutputDigitMatchesBranchName;

    /** Input array of CSC Points **/
    TClonesArray* fBmnCSCPointsArray;

    /** Input array of MC Tracks **/
    TClonesArray* fMCTracksArray;

    /** Output array of CSC Digits **/
    TClonesArray* fBmnCSCDigitsArray;

    /** Output array of CSC Digit Matches **/
    TClonesArray* fBmnCSCDigitMatchesArray;

    Bool_t fOnlyPrimary;
    Bool_t fStripMatching;

    BmnCSCConfiguration::CSC_CONFIG fCurrentConfig;

    BmnCSCStationSet *StationSet; //Entire CSC detector

    BmnCSCTransform *TransfSet; //Transformations for each module of the detector

    ClassDef(BmnCSCDigitizer,1);
};

#endif
