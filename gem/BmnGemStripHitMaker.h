#ifndef BMNGEMSTRIPHITMAKER_H
#define BMNGEMSTRIPHITMAKER_H 1

#include <iostream>
#include <fstream>
#include <sstream>

#include "Rtypes.h"
#include "TClonesArray.h"

#include "FairTask.h"
#include "FairMCPoint.h"

#include "BmnGemStripDigit.h"
#include "BmnGemStripHit.h"
#include "BmnGemStripStationSet.h"
#include "BmnGemStripConfiguration.h"
#include "BmnGemAlignmentCorrections.h"

using namespace std;

class BmnGemStripHitMaker : public FairTask {
public:

    BmnGemStripHitMaker();
    BmnGemStripHitMaker(Bool_t);

    virtual ~BmnGemStripHitMaker();

    //void SetVerbosity(Bool_t verbose);

    virtual InitStatus Init();

    virtual void Exec(Option_t* opt);

    virtual void Finish();

    void ProcessDigits();

    void SetHitMatching(Bool_t opt = kTRUE) {
        fHitMatching = opt;
    }

    void SetCurrentConfig(BmnGemStripConfiguration::GEM_CONFIG config) {
        fCurrentConfig = config;
    }

     void SetAlignmentCorrections(TString fname) {
        TString dir = getenv("VMCWORKDIR");
        fFile = dir + "/input/" + fname;
    }

private:

    TString fInputPointsBranchName;
    TString fInputDigitsBranchName;
    TString fInputDigitMatchesBranchName;

    TString fOutputHitsBranchName;
    TString fOutputHitMatchesBranchName;

    /** Input array of Gem Points **/
    TClonesArray* fBmnGemStripPointsArray;
    TClonesArray* fBmnGemStripDigitsArray;
    TClonesArray* fBmnGemStripDigitMatchesArray;

    /** Output array of Gem Hits **/
    TClonesArray* fBmnGemStripHitsArray;

    /** Output array of GEM Hit Matches **/
    TClonesArray* fBmnGemStripHitMatchesArray;

    Bool_t fHitMatching;

    BmnGemStripConfiguration::GEM_CONFIG fCurrentConfig;

    BmnGemStripStationSet *StationSet; //Entire GEM detector

    TString fFile; // a file with geometry corrections
    void ReadFileCorrections(TString, Double_t***); // read corrections from the file
    Double_t*** corr; // array to store the corrections

    ClassDef(BmnGemStripHitMaker,1);
};


#endif