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

#include <UniDbDetectorParameter.h>
#include <UniDbRun.h>
#include "FairField.h"
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
    }

    void SetAlignmentCorrectionsFileName(Int_t run_period, Int_t file_number) {
        if (run_period == 5)
            fAlignCorrFileName = "$VMCWORKDIR/input/alignCorrsLocal_GEM.root";
        else if (run_period == 6) {
            fAlignCorrFileName = "alignment_GEM.root";
            UniDbDetectorParameter::ReadRootFile(run_period, file_number, "BM@N", "alignment", (Char_t*) fAlignCorrFileName.Data());
        }
        else
            fAlignCorrFileName = "";
    }

    Double_t GetLorentzByField(Double_t By, Int_t station) { //By in kGs
        const Int_t nStation = 6;

        Double_t p0[nStation] = {-0.00130114, 0.000724722, 0.000545885, -0.000127976, 0.000654366, 0.000850249};
        Double_t p1[nStation] = {0.034723, 0.0175508, 0.0258856, 0.0300613, 0.0150505, 0.0272029};
        Double_t p2[nStation] = {-0.00113774, 0.000707385, 0.000521317, -0.000209771, 0.00078009, -0.00011422};
        return p0[station] + p1[station] * By + p2[station] * By * By;
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

    FairField* fField;

    ClassDef(BmnGemStripHitMaker, 1);
};


#endif
