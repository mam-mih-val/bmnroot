#ifndef BMNCSCHITMAKER_H
#define BMNCSCHITMAKER_H 1

#include <iostream>
#include <fstream>
#include <sstream>

#include "Rtypes.h"
#include "TClonesArray.h"
#include "TRegexp.h"
#include "TString.h"

#include "FairTask.h"
#include "FairMCPoint.h"

#include <UniDbDetectorParameter.h>
#include <UniDbRun.h>
#include "FairField.h"
#include "BmnCSCDigit.h"
#include "BmnCSCHit.h"
#include "BmnCSCStationSet.h"
#include "BmnCSCConfiguration.h"
#include "BmnCSCTransform.h"
//#include "BmnInnTrackerAlign.h"
#include <BmnEventQuality.h>

using namespace std;

class BmnCSCHitMaker : public FairTask {
public:

    BmnCSCHitMaker();
    BmnCSCHitMaker(Int_t, Int_t, Bool_t, TString alignFile = "default");

    virtual ~BmnCSCHitMaker();

    virtual InitStatus Init();

    virtual void Exec(Option_t* opt);

    virtual void Finish();

    void ProcessDigits();

    void SetHitMatching(Bool_t opt = kTRUE) {
        fHitMatching = opt;
    }

    void SetCurrentConfig(BmnCSCConfiguration::CSC_CONFIG config) {
        fCurrentConfig = config;
    }

private:

    TString fInputPointsBranchName;
    TString fInputDigitsBranchName;
    TString fInputDigitMatchesBranchName;

    TString fOutputHitsBranchName;
    TString fOutputHitMatchesBranchName;

    /** Input array of CSC Points **/
    TClonesArray* fBmnCSCPointsArray;
    TClonesArray* fBmnCSCDigitsArray;
    TClonesArray* fBmnCSCDigitMatchesArray;

    /** Output array of CSC Hits **/
    TClonesArray* fBmnCSCHitsArray;

    /** Output array of CSC Hit Matches **/
    TClonesArray* fBmnCSCHitMatchesArray;

    Bool_t fHitMatching;
    Bool_t fIsExp; // Specify type of input data (MC or real data)

    BmnCSCConfiguration::CSC_CONFIG fCurrentConfig;

    BmnCSCStationSet *StationSet; //Entire CSC detector

    BmnCSCTransform *TransfSet; //Transformations for each module of the detector

    FairField* fField;

    TString fBmnEvQualityBranchName;
    TClonesArray* fBmnEvQuality;

    ClassDef(BmnCSCHitMaker, 1);
};


#endif
