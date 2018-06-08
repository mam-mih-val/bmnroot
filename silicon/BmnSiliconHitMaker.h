#ifndef BMNSILICONHITMAKER_H
#define BMNSILICONHITMAKER_H

#include "Rtypes.h"

#include "FairTask.h"
#include "FairMCPoint.h"

#include "BmnSiliconDigit.h"
#include "BmnSiliconHit.h"
#include "BmnSiliconStationSet.h"
#include "BmnSiliconStation.h"
#include "BmnSiliconModule.h"
#include "BmnSiliconLayer.h"
#include <UniDbDetectorParameter.h>
#include <UniDbRun.h>
#include <BmnSiliconAlignCorrections.h>
#include <BmnEventQuality.h>

#include "BmnSiliconConfiguration.h"

class BmnSiliconHitMaker : public FairTask {
public:

    BmnSiliconHitMaker();
    BmnSiliconHitMaker(Bool_t isExp);

    virtual ~BmnSiliconHitMaker();

    virtual InitStatus Init();

    virtual void Exec(Option_t* opt);

    virtual void Finish();

    void ProcessDigits();

    void SetHitMatching(Bool_t opt = kTRUE) {
        fHitMatching = opt;
    }

    void SetCurrentConfig(BmnSiliconConfiguration::SILICON_CONFIG config) {
        fCurrentConfig = config;
    }

    void SetAlignmentCorrectionsFileName(TString filename) {
        fAlignCorrFileName = filename;
    }

    void SetAlignmentCorrectionsFileName(Int_t run_period, Int_t file_number) {
        if (run_period == 6) {
            fAlignCorrFileName = "alignment_SI.root";
            UniDbDetectorParameter::ReadRootFile(run_period, file_number, "BM@N", "alignment", (Char_t*) fAlignCorrFileName.Data());
        }
        else
            fAlignCorrFileName = "";
    }

private:

    TString fInputPointsBranchName;
    TString fInputDigitsBranchName;
    TString fInputDigitMatchesBranchName;

    TString fOutputHitsBranchName;
    TString fOutputHitMatchesBranchName;

    /** Input array of Silicon Points **/
    TClonesArray* fBmnSiliconPointsArray;
    TClonesArray* fBmnSiliconDigitsArray;
    TClonesArray* fBmnSiliconDigitMatchesArray;

    /** Output array of Silicon Hits **/
    TClonesArray* fBmnSiliconHitsArray;

    /** Output array of Silicon Hit Matches **/
    TClonesArray* fBmnSiliconHitMatchesArray;

    Bool_t fHitMatching;

    BmnSiliconConfiguration::SILICON_CONFIG fCurrentConfig;

    BmnSiliconStationSet *StationSet; //Entire Silicon detector
    
    TString fAlignCorrFileName; // a file with geometry corrections
    void ReadAlignCorrFile(TString, Double_t***); // read corrections from the file
    Double_t*** corr; // array to store the corrections

    Bool_t fIsExp;
    TString fBmnEvQualityBranchName;
    TClonesArray* fBmnEvQuality;

    ClassDef(BmnSiliconHitMaker, 1);
};

#endif /* BMNSILICONHITMAKER_H */

