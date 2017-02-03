#ifndef BMNGEMSTRIPHITMAKER_H
#define BMNGEMSTRIPHITMAKER_H 1

#include <iostream>
#include <fstream>
#include <sstream>

#include "Rtypes.h"
#include "TClonesArray.h"
#include "TRegexp.h"
#include "TString.h"

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

    void SetAlignmentCorrectionsFileName(TString filename) {
        // temporary patch to be able to run re-reconstructions and subsequent re-alignments iteratively
        // Anatoly.Solomin@jinr.ru 2017-02-01 16:43:03
        TRegexp re = "_it[0-9]+"; // to match patterns like "_it02" in the file name with corrections after iteration 2
        if ( ! filename.Contains(re)) {  // normal, not an iterative alignment running
            TString dir = getenv("VMCWORKDIR");
            fAlignCorrFileName = dir+"/input/"+filename; }
        else// iterative case: file name [with its relative or absolute  path] is used as is
            fAlignCorrFileName = filename;
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

    TString fAlignCorrFileName; // a file with geometry corrections
    void ReadAlignCorrFile(TString, Double_t***); // read corrections from the file
    Double_t*** corr; // array to store the corrections

    ClassDef(BmnGemStripHitMaker,1);
};


#endif
