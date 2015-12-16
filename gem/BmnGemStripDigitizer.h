#ifndef BMNGEMSTRIPDIGITIZER_H
#define BMNGEMSTRIPDIGITIZER_H 1

#include <iostream>

#include "Rtypes.h"
#include "TClonesArray.h"
//#include "TVector3.h"

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

private:

    TString fInputBranchName;
    TString fOutputDigitsBranchName;

    /** Input array of Gem Points **/
    TClonesArray* fBmnGemStripPointsArray;

    /** Input array of MC Tracks **/
    TClonesArray* fMCTracksArray;

    /** Output array of Gem Digits **/
    TClonesArray* fBmnGemStripDigitsArray;

    Bool_t fOnlyPrimary;

    Double_t fSmearingSigma; //cm

    ClassDef(BmnGemStripDigitizer,1);
};

#endif
