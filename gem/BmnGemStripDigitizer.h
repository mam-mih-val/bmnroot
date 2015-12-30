#ifndef BMNGEMSTRIPDIGITIZER_H
#define BMNGEMSTRIPDIGITIZER_H 1

#include <iostream>

#include "Rtypes.h"
#include "TClonesArray.h"

#include "FairTask.h"
#include "FairMCPoint.h"

#include "BmnGemStripDigit.h"
#include "BmnGemStripHit.h"
#include "BmnGemStripStationSet_v3.h"

using namespace std;

class BmnGemStripDigitizer : public FairTask {
public:

    BmnGemStripDigitizer();

    virtual ~BmnGemStripDigitizer();

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

private:

    TString fInputBranchName;
    TString fOutputDigitsBranchName;
    TString fOutputDigitMatchesBranchName;

    /** Input array of Gem Points **/
    TClonesArray* fBmnGemStripPointsArray;

    /** Input array of MC Tracks **/
    TClonesArray* fMCTracksArray;

    /** Output array of Gem Digits **/
    TClonesArray* fBmnGemStripDigitsArray;

    /** Output array of GEM Digit Matches **/
    TClonesArray* fBmnGemStripDigitMatchesArray;

    Bool_t fOnlyPrimary;
    Bool_t fStripMatching;

    Double_t fSmearingSigma; //cm

    ClassDef(BmnGemStripDigitizer,1);
};

#endif
