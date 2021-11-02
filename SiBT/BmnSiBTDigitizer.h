#ifndef BMNSiBTDIGITIZER_H
#define BMNSiBTDIGITIZER_H 1

#include <iostream>

#include "Rtypes.h"
#include "TClonesArray.h"

#include "FairTask.h"
#include "FairMCPoint.h"

#include "BmnSiBTDigit.h"
#include "BmnSiBTHit.h"
#include "BmnSiBTStationSet.h"
#include "BmnSiBTConfiguration.h"
#include "BmnSiBTTransform.h"

using namespace std;

class BmnSiBTDigitizer : public FairTask {
public:

    BmnSiBTDigitizer();

    virtual ~BmnSiBTDigitizer();

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

    void SetCurrentConfig(BmnSiBTConfiguration::SiBT_CONFIG config) {
        fCurrentConfig = config;
    }

private:

    TString fInputBranchName;
    TString fOutputDigitsBranchName;
    TString fOutputDigitMatchesBranchName;

    /** Input array of SiBT Points **/
    TClonesArray* fBmnSiBTPointsArray;

    /** Input array of MC Tracks **/
    TClonesArray* fMCTracksArray;

    /** Output array of SiBT Digits **/
    TClonesArray* fBmnSiBTDigitsArray;

    /** Output array of SiBT Digit Matches **/
    TClonesArray* fBmnSiBTDigitMatchesArray;

    Bool_t fOnlyPrimary;
    Bool_t fStripMatching;

    BmnSiBTConfiguration::SiBT_CONFIG fCurrentConfig;

    BmnSiBTStationSet *StationSet; //Entire SiBT detector

    BmnSiBTTransform *TransfSet; //Transformations for each module of the detector

    ClassDef(BmnSiBTDigitizer,1);
};

#endif
