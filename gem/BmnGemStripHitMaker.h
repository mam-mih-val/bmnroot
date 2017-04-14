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
#include "BmnGemAlignCorrections.h"

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
        fAlignCorrFileName = filename;
        // filename [with its relative or absolute  path] is used as is and is
        // taken from alignCorrFileName parameter in run_reco_bmn.C
        //
        // NB! As soon as storage of the alignment corrections is arranged in
        // the UniDb, we will need to change this, so that the default values
        // are taken from there. Candidate for the default filename defined in
        // run_reco_bmn.C is alignCorrFileName = "UniDb" (case insensitive!)).
        //
        // Anatoly.Solomin@jinr.ru 2017-02-21 14:05:00
    }
    
    // All the corrections should be migrated to database!
    void SetAlignmentCorrectionsFileName(Int_t run_period, Int_t file_number) {
        fAlignCorrFileName = (run_period == 5) ? "$VMCWORKDIR/input/alignCorrsLocal_GEM.root" : 
            (run_period == 6) ? "$VMCWORKDIR/input/align.root" : "";      
    }

private:

    TString fInputPointsBranchName;
    TString fInputDigitsBranchName;
    TString fInputDigitMatchesBranchName;
    TString fBmnEventHeaderBranchName;

    TString fOutputHitsBranchName;
    TString fOutputHitMatchesBranchName;

    /** Input array of Gem Points **/
    TClonesArray* fBmnGemStripPointsArray;
    TClonesArray* fBmnGemStripDigitsArray;
    TClonesArray* fBmnGemStripDigitMatchesArray;
    TClonesArray* fBmnEventHeader;

    /** Output array of Gem Hits **/
    TClonesArray* fBmnGemStripHitsArray;

    /** Output array of GEM Hit Matches **/
    TClonesArray* fBmnGemStripHitMatchesArray;

    Bool_t fHitMatching;
    Bool_t fIsExp; // Specify type of input data (MC or real data)

    BmnGemStripConfiguration::GEM_CONFIG fCurrentConfig;

    BmnGemStripStationSet *StationSet; //Entire GEM detector

    TString fAlignCorrFileName; // a file with geometry corrections
    void ReadAlignCorrFile(TString, Double_t***); // read corrections from the file
    Double_t*** corr; // array to store the corrections

    ClassDef(BmnGemStripHitMaker,1);
};


#endif
